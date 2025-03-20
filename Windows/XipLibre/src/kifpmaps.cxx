/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2010 - 2011 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the XIP library or the KiF library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : KiF
Version    : 1.00
filename   : treemaps.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language
Programmer : Claude ROUX
Reviewer   :
*/

//#define PROFILING

#include "kif.h"
#include "kifcontainer.h"
#include "fractalhmap.h"
#include "kifpmaps.h"

//-----------------------------------------------------------------------------------

extern const char* kifErrorStrings[];
bool TestkifFunctions(short n, hmap<short, bool>& kiffunc);

KifElement* KifPrimeMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<string, KifElement*> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	prime_iterator<string, KifElement*> it;
	for (it = values; !it.end(); it++)
		kvect->Push(it.value());
	return kvect;
}

string KifPrimeMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<string, KifElement*> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it.key();
		stringing(res, sx);
		res << ":";
		sx = it.value()->StringForDisplay();
		if (!it.value()->aString())
			res << sx;
		else
			stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifPrimeMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<string, KifElement*> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it.key();
		jstringing(res, sx);
		res << ":" << it.value()->JSonString();
	}
	res << "}";
	return res.str();
}

string KifPrimeMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<string, KifElement*> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it.key();
		res << "'" << Trim(sx) << "'" << ":" << it.value()->BaseString();
	}
	res << "}";
	return res.str();
}


KifElement* KifPrimeMap::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifPrimeMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifPrimeMap)
		return kifNULL;

	KifPrimeMap* A = (KifPrimeMap*)a;
	prime_iterator<string, KifElement*> it;

	KifPrimeMap* ref = new KifPrimeMap(kifcode, NULL);
	KifPrimeMap* bref = (KifPrimeMap*)b;
	string ky;
	KifContainerLock _lock(bref);

	for (it = bref->values; !it.end(); it++) {
		ky = it.key();
		if (A->values.find(ky) == false) {
			ref->Push(ky, it.value());
			it.value()->Setreference();
		}
	}

	KifContainerLock _locka(A);
	for (it = A->values; !it.end(); it++) {
		ky = it.key();
		if (bref->values.find(ky) == false) {
			ref->Push(ky, it.value());
			it.value()->Setreference();
		}
	}
	return ref;
}

KifElement* KifPrimeMap::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifPrimeMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifPrimeMap)
		return kifNULL;

	KifPrimeMap* A = (KifPrimeMap*)a;
	prime_iterator<string, KifElement*> it;

	KifPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifPrimeMap*)a->Copy(NULL);

	KifPrimeMap* bref = (KifPrimeMap*)b;
	string ky;
	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values; !it.end(); it++) {
		if (A->values.find(it.key()) == false) {
			ky = it.key();
			ref->Push(ky, it.value());
			it.value()->Setreference();
		}
	}
	return ref;
}

KifElement* KifPrimeMap::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifPrimeMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifPrimeMap)
		return kifNULL;

	KifPrimeMap* A = (KifPrimeMap*)a;
	prime_iterator<string, KifElement*> it;
	KifElement* kval;
	KifPrimeMap* ref = new KifPrimeMap(kifcode, NULL);
	KifPrimeMap* bref = (KifPrimeMap*)b;
	string ky;

	primehash_value<string, KifElement*>* cell;
	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);

	for (it = bref->values; !it.end(); it++) {
		cell = A->values.search(it.key());
		if (cell != NULL) {
			kval = cell->value;
			if (it.value()->same(kval) == kifTRUE) {
				ky = it.key();
				ref->Push(ky, it.value());
				kval->Setreference();
			}
		}
	}
	return ref;
}

Exported KifElement* KifPrimeMap::same(KifElement* a) {

	if (a->type != kifPrimeMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMap* m = (KifPrimeMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	primehash_value<string, KifElement*>* cell;
	prime_iterator<string, KifElement*> it;
	for (it = m->values; !it.end(); it++) {
		cell = values.search(it.key());
		if (cell == NULL)
			return kifFALSE;
		if (it.value()->same(cell->value) == kifFALSE)
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifPrimeMap::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifPrimeMap)
		return a->plus(a, b, context, autoself);

	KifPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMap*)a;

	prime_iterator<string, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->plus(ke, kref, context, true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->plus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifPrimeMap::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifPrimeMap)
		return a->minus(a, b, context, autoself);

	KifPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMap*)a;

	prime_iterator<string, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->minus(ke, kref, context, true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->minus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifPrimeMap::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifPrimeMap)
		return a->multiply(a, b, context, autoself);

	KifPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMap*)a;

	prime_iterator<string, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->multiply(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->multiply(ke, b, context, true);
	}
	return ref;
}

KifElement* KifPrimeMap::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifPrimeMap)
		return a->divide(a, b, context, autoself);

	KifPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMap*)a;

	prime_iterator<string, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->divide(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->divide(ke, b, context, true);
	}
	return ref;
}

KifElement* KifPrimeMap::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifPrimeMap)
		return a->mod(a, b, context, autoself);

	KifPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMap*)a;

	prime_iterator<string, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->mod(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->mod(ke, b, context, true);
	}
	return ref;
}

KifElement* KifPrimeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifPrimeMap)
		return a->shiftright(a, b, context, autoself);

	KifPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMap*)a;

	prime_iterator<string, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->shiftright(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftright(ke, b, context, true);
	}
	return ref;
}

KifElement* KifPrimeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifPrimeMap)
		return a->shiftleft(a, b, context, autoself);

	KifPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMap*)a;

	prime_iterator<string, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->shiftleft(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftleft(ke, b, context, true);
	}
	return ref;
}

KifElement* KifPrimeMap::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifPrimeMap)
		return a->power(a, b, context, autoself);

	KifPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMap*)a;

	prime_iterator<string, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->power(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->power(ke, b, context, true);
	}
	return ref;
}

KifElement* KifPrimeMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorPrimeMap* iter = new KifIteratorPrimeMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifPrimeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	int nb;
	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->aMapContainer()) {
			ke->Setpopped(1, true);
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				push(itr->IteratorKeyString(), itr->IteratorValue());
			itr->Release();
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		char ch[20];
		if (ke->aVectorContainer()) {
			ke->Setpopped(1, true);
			Clear();
			nb = 0;
			for (int it = 0; it < ke->Size(); it++) {
				sprintf_s(ch, 20, "%d", nb);
				Push(ch, ke->ValueOnIndex(it));
				nb++;
			}
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		if (ke->type == kifList) {
			ke->Setpopped(1, true);
			KifList* kvect = (KifList*)ke;
			Clear();
			nb = 0;
			{
				KifContainerLock _lock(kvect);
				list<KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
					sprintf_s(ch, 20, "%d", nb);
					Push(ch, *it);
					nb++;
				}
			}
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		if (ke != this) {
			Clear();
			KifMap* kvect = (KifMap*)ke;
			//We copy all values from ke to this
			{
				KifContainerLock _lock(kvect);
				hmap<string, KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++)
					push(it->first, it->second);
			}
		}
		return true;
	}

	Push(STR(idx->String()), ke);
	return true;
}

KifElement* KifPrimeMap::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseString localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	prime_iterator<string, KifElement*> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

KifElement* KifPrimeMap::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMap* ke = new KifPrimeMap(kifcode, NULL);
	prime_iterator<string, KifElement*> itx;
	string n;
	for (itx = values; !itx.end(); itx++) {
		n = itx.key();
		if (itx.value() != NULL) {
			ke->values[n] = itx.value()->Copy(kp, dom);
			ke->values[n]->Setreference();
		}
		else
			ke->values[n] = NULL;
	}
	return ke;
}

KifElement* KifPrimeMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}
		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					Setvalue(res, itr->IteratorKey());
				else
					res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

Exported void KifPrimeMap::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror("KIF(689): Circular structure");
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	prime_iterator<string, KifElement*> itx;
	vector<KifElement*> doubles;
	{
		KifContainerLock _lock(this);
		for (itx = values; !itx.end(); itx++) {
			if (itx.value() != NULL)
				doubles.push_back(itx.value());
		}
	}
	for (int itx = 0; itx < doubles.size(); itx++)
		doubles[itx]->Setreference(inc);
	idgarbage = id;
}


KifElement* KifIteratorPrimeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (it.end())
		return kifNULL;
	return it.value()->Exec(contextualpattern, dom, val, idthread, callfunc);
}



static void resetMap(KifPrimeMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	prime_iterator<string, KifElement*> itx;
	for (itx = kmap->values; !itx.end(); itx++)
		itx.value()->Resetreference(inc);

	if (clr && !popped)
		kmap->values.clear();
}

void KifPrimeMap::Resetreference(int inc) {
	reference -= inc;
	if (reference > 0)
		resetMap(this, inc, popped, false);
	else {
		resetMap(this, inc + 1 + reference - popped, popped, true);
		if (popped == 1)
			popped = 0;
		else
			KifElement::Resetreference(0);
	}
}

void KifPrimeMap::Clear() {
	KifContainerLock _lock(this);
	prime_iterator<string, KifElement*> itx;
	for (itx = values; !itx.end(); itx++)
		itx.value()->Resetreference(reference + 1);
	values.clear();
}

void KifPrimeMap::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string n = itr->IteratorKeyString();
		if (values.find(n) == false)
			Push(n, itr->IteratorValue());
	}
	itr->Release();
}

KifElement* KifPrimeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* ke;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			prime_iterator<string, KifElement*> it;
			for (it = values; !it.end(); it++) {
				ke = kifcode->Providestringraw(it.key());
				vect->Push(ke);
			}
			return vect;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifPrimeMap* kmap = new KifPrimeMap(kifcode, NULL);
			prime_iterator<string, KifElement*> it;
			for (it = values; !it.end(); it++) {
				ke = it.value()->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
				if (ke == kifRAISEERROR) {
					kmap->Release();
					return kifRAISEERROR;
				}
				kmap->push(it.key(), ke);
			}
			return kmap;
		}
		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMap* kmap = new KifPrimeMap(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			string vleft = key->String();
			string vright = keyright->String();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<string, KifElement*> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->push(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
	if (key == kifNULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	string skey = key->String();
	key->Release();
	KifElement* kval = Values(skey);
	if (kval == kifNOELEMENT) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kval;
}

void KifPrimeMap::Pop(KifElement* ke) {
	string s = ke->String();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);
		primehash_value<string, KifElement*>* cell = values.search(s);
		if (cell == NULL) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(kifErrorStrings[223]);
			return;
		}

		kres = cell->value;
		values.erase(s);
	}
	kres->Resetreference(reference + 1);
}

Exported void KifPrimeMap::push(string key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	primehash_value<string, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}



//To insert a value into a map through the API 
Exported void KifPrimeMap::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	primehash_value<string, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifPrimeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	primehash_value<string, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifPrimeMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	string key = k->String();

	KifContainerLock _lock(this);
	primehash_value<string, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

//--------------------------------------------------------------------------------------------------
KifElement* KifIntegerPrimeMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	prime_iterator<long, KifElement*> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifIntegerPrimeMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	prime_iterator<long, KifElement*> it;
	for (it = values; !it.end(); it++)
		kvect->Push(it.value());
	return kvect;
}

string KifIntegerPrimeMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<long, KifElement*> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":";
		sx = it.value()->StringForDisplay();
		if (!it.value()->aString())
			res << sx;
		else
			stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifIntegerPrimeMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<long, KifElement*> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":" << it.value()->JSonString();
	}
	res << "}";
	return res.str();
}

string KifIntegerPrimeMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<long, KifElement*> it;
	res << "{";
	bool beg = true;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":" << it.value()->BaseString();
	}
	res << "}";
	return res.str();
}


KifElement* KifIntegerPrimeMap::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifIntegerPrimeMap)
		return kifNULL;

	KifIntegerPrimeMap* A = (KifIntegerPrimeMap*)a;
	prime_iterator<long, KifElement*> it;

	KifIntegerPrimeMap* ref = new KifIntegerPrimeMap(kifcode, NULL);
	KifIntegerPrimeMap* bref = (KifIntegerPrimeMap*)b;
	long ky;
	KifContainerLock _lock(bref);

	for (it = bref->values; !it.end(); it++) {
		ky = it.key();
		if (A->values.find(ky) == false) {
			ref->push(ky, it.value());
			it.value()->Setreference();
		}
	}

	KifContainerLock _locka(A);
	for (it = A->values; !it.end(); it++) {
		ky = it.key();
		if (bref->values.find(ky) == false) {
			ref->push(ky, it.value());
			it.value()->Setreference();
		}
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifIntegerPrimeMap)
		return kifNULL;

	KifIntegerPrimeMap* A = (KifIntegerPrimeMap*)a;
	prime_iterator<long, KifElement*> it;

	KifIntegerPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerPrimeMap*)a->Copy(NULL);

	KifIntegerPrimeMap* bref = (KifIntegerPrimeMap*)b;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values; !it.end(); it++) {
		if (A->values.find(it.key()) == false) {
			ref->push(it.key(), it.value());
			it.value()->Setreference();
		}
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifIntegerPrimeMap)
		return kifNULL;

	KifIntegerPrimeMap* A = (KifIntegerPrimeMap*)a;
	prime_iterator<long, KifElement*> it;

	KifIntegerPrimeMap* ref = new KifIntegerPrimeMap(kifcode, NULL);
	KifIntegerPrimeMap* bref = (KifIntegerPrimeMap*)b;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	KifElement* kval;
	for (it = bref->values; !it.end(); it++) {
		if (A->values.find(it.key())) {
			kval = A->values[it.key()];
			if (it.value()->same(kval) == kifTRUE) {
				ref->push(it.key(), it.value());
				kval->Setreference();
			}
		}
	}
	return ref;
}

Exported KifElement* KifIntegerPrimeMap::same(KifElement* a) {

	if (a->type != kifIntegerPrimeMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifIntegerPrimeMap* m = (KifIntegerPrimeMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<long, KifElement*> it;
	primehash_value<long, KifElement*>* cell;
	for (it = m->values; !it.end(); it++) {
		cell = values.search(it.key());
		if (cell == NULL)
			return kifFALSE;
		if (it.value()->same(cell->value) == kifFALSE)
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifIntegerPrimeMap::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->plus(a, b, context, autoself);

	KifIntegerPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerPrimeMap*)a;

	prime_iterator<long, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnIntegerKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->plus(ke, kref, context, true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->plus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->minus(a, b, context, autoself);

	KifIntegerPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerPrimeMap*)a;

	prime_iterator<long, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnIntegerKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->minus(ke, kref, context, true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->minus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->multiply(a, b, context, autoself);

	KifIntegerPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerPrimeMap*)a;

	prime_iterator<long, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnIntegerKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->multiply(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->multiply(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->divide(a, b, context, autoself);

	KifIntegerPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerPrimeMap*)a;

	prime_iterator<long, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnIntegerKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->divide(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->divide(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->mod(a, b, context, autoself);

	KifIntegerPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerPrimeMap*)a;

	prime_iterator<long, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnIntegerKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->mod(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->mod(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->shiftright(a, b, context, autoself);

	KifIntegerPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerPrimeMap*)a;

	prime_iterator<long, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnIntegerKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->shiftright(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftright(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->shiftleft(a, b, context, autoself);

	KifIntegerPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerPrimeMap*)a;

	prime_iterator<long, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnIntegerKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->shiftleft(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftleft(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->power(a, b, context, autoself);

	KifIntegerPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerPrimeMap*)a;

	prime_iterator<long, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnIntegerKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->power(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->power(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorIntegerPrimeMap* iter = new KifIteratorIntegerPrimeMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifIntegerPrimeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->aMapContainer()) {
			ke->Setpopped(1, true);
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				push(itr->IteratorKeyInteger(), itr->IteratorValue());
			itr->Release();
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		if (ke->aVectorContainer()) {
			ke->Setpopped(1, true);
			Clear();
			for (int it = 0; it < ke->Size(); it++)
				push(it, ke->ValueOnIndex(it));
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		if (ke->type == kifList) {
			long n = 0;
			ke->Setpopped(1, true);
			KifList* kvect = (KifList*)ke;
			Clear();
			{
				KifContainerLock _lock(kvect);
				list<KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
					push(n, *it);
					n++;
				}
			}
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		if (ke != this) {
			long n = 0;
			Clear();
			KifMap* kvect = (KifMap*)ke;
			//We copy all values from ke to this
			{
				KifContainerLock _lock(kvect);
				hmap<string, KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
					n = conversionintegerhexa(STR(it->first));
					push(n, it->second);
				}
			}
		}
		return true;
	}

	push(idx->Integer(), ke);
	return true;
}

KifElement* KifIntegerPrimeMap::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseInteger localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	prime_iterator<long, KifElement*> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

KifElement* KifIntegerPrimeMap::Copy(KifDomain* kp, KifElement* dom) {
	KifIntegerPrimeMap* ke = new KifIntegerPrimeMap(kifcode, NULL);
	prime_iterator<long, KifElement*> itx;
	long n;
	for (itx = values; !itx.end(); itx++) {
		n = itx.key();
		if (itx.value() != NULL) {
			ke->values[n] = itx.value()->Copy(kp, dom);
			ke->values[n]->Setreference();
		}
		else
			ke->values[n] = NULL;
	}
	return ke;
}

KifElement* KifIntegerPrimeMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}
		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					Setvalue(res, itr->IteratorKey());
				else
					res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

Exported void KifIntegerPrimeMap::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror("KIF(689): Circular structure");
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	prime_iterator<long, KifElement*> itx;
	vector<KifElement*> doubles;
	{
		KifContainerLock _lock(this);
		for (itx = values; !itx.end(); itx++) {
			if (itx.value() != NULL)
				doubles.push_back(itx.value());
		}
	}
	for (int itx = 0; itx < doubles.size(); itx++)
		doubles[itx]->Setreference(inc);
	idgarbage = id;
}


string KifIteratorIntegerPrimeMap::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

KifElement* KifIteratorIntegerPrimeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (it.end())
		return kifNULL;
	return it.value()->Exec(contextualpattern, dom, val, idthread, callfunc);
}



static void resetMap(KifIntegerPrimeMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	prime_iterator<long, KifElement*> itx;
	for (itx = kmap->values; !itx.end(); itx++)
		itx.value()->Resetreference(inc);

	if (clr && !popped)
		kmap->values.clear();
}

void KifIntegerPrimeMap::Resetreference(int inc) {
	reference -= inc;
	if (reference > 0)
		resetMap(this, inc, popped, false);
	else {
		resetMap(this, inc + 1 + reference - popped, popped, true);
		if (popped == 1)
			popped = 0;
		else
			KifElement::Resetreference(0);
	}
}

void KifIntegerPrimeMap::Clear() {
	KifContainerLock _lock(this);
	prime_iterator<long, KifElement*> itx;
	for (itx = values; !itx.end(); itx++)
		itx.value()->Resetreference(reference + 1);
	values.clear();
}

void KifIntegerPrimeMap::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (values.find(n) == false) {
			ke = itr->IteratorValue()->Atom();
			values[n] = ke;
			ke->Setreference(reference + 1);
		}
	}
	itr->Release();
}


KifElement* KifIntegerPrimeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* ke;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			prime_iterator<long, KifElement*> it;
			for (it = values; !it.end(); it++)
				vect->Push(kifcode->Provideinteger(it.key()));
			return vect;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifIntegerPrimeMap* kmap = new KifIntegerPrimeMap(kifcode, NULL);
			prime_iterator<long, KifElement*> it;
			for (it = values; !it.end(); it++) {
				ke = it.value()->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
				if (ke == kifRAISEERROR) {
					kmap->Release();
					return kifRAISEERROR;
				}
				kmap->push(it.key(), ke);
			}
			return kmap;
		}
		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifIntegerPrimeMap* kmap = new KifIntegerPrimeMap(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<long, KifElement*> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->push(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	long skey = key->Integer();
	key->Release();
	primehash_value<long, KifElement*>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return cell->value;
}

void KifIntegerPrimeMap::Pop(KifElement* ke) {
	long s = ke->Integer();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);
		primehash_value<long, KifElement*>* cell = values.search(s);
		if (cell == NULL) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(kifErrorStrings[223]);
			return;
		}
		kres = cell->value;
		values.erase(s);
	}
	kres->Resetreference(reference + 1);
}

//To insert a value into a map through the API 
Exported void KifIntegerPrimeMap::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	long n = conversionintegerhexa(STR(key));
	KifContainerLock _lock(this);
	primehash_value<long, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifIntegerPrimeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	long n = conversionintegerhexa(key);
	primehash_value<long, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifIntegerPrimeMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long n = k->Integer();
	KifContainerLock _lock(this);
	primehash_value<long, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}


void KifIntegerPrimeMap::push(long n, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	primehash_value<long, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}


//--------------------------------------------------------------------------------------------------
KifElement* KifFloatPrimeMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	prime_iterator<double, KifElement*> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifFloatPrimeMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	prime_iterator<double, KifElement*> it;
	for (it = values; !it.end(); it++)
		kvect->Push(it.value());
	return kvect;
}

string KifFloatPrimeMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<double, KifElement*> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":";
		sx = it.value()->StringForDisplay();
		if (!it.value()->aString())
			res << sx;
		else
			stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifFloatPrimeMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<double, KifElement*> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":" << it.value()->JSonString();
	}
	res << "}";
	return res.str();
}

string KifFloatPrimeMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<double, KifElement*> it;
	res << "{";
	bool beg = true;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":" << it.value()->BaseString();
	}
	res << "}";
	return res.str();
}


KifElement* KifFloatPrimeMap::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifFloatPrimeMap)
		return kifNULL;

	KifFloatPrimeMap* A = (KifFloatPrimeMap*)a;
	prime_iterator<double, KifElement*> it;

	KifFloatPrimeMap* ref = new KifFloatPrimeMap(kifcode, NULL);
	KifFloatPrimeMap* bref = (KifFloatPrimeMap*)b;
	double ky;
	KifContainerLock _lock(bref);

	for (it = bref->values; !it.end(); it++) {
		ky = it.key();
		if (A->values.find(ky) == false) {
			ref->push(ky, it.value());
			it.value()->Setreference();
		}
	}

	KifContainerLock _locka(A);
	for (it = A->values; !it.end(); it++) {
		ky = it.key();
		if (bref->values.find(ky) == false) {
			ref->push(ky, it.value());
			it.value()->Setreference();
		}
	}
	return ref;
}

KifElement* KifFloatPrimeMap::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifFloatPrimeMap)
		return kifNULL;

	KifFloatPrimeMap* A = (KifFloatPrimeMap*)a;
	prime_iterator<double, KifElement*> it;

	KifFloatPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatPrimeMap*)a->Copy(NULL);

	KifFloatPrimeMap* bref = (KifFloatPrimeMap*)b;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values; !it.end(); it++) {
		if (A->values.find(it.key()) == false) {
			ref->push(it.key(), it.value());
			it.value()->Setreference();
		}
	}
	return ref;
}

KifElement* KifFloatPrimeMap::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifFloatPrimeMap)
		return kifNULL;

	KifFloatPrimeMap* A = (KifFloatPrimeMap*)a;
	prime_iterator<double, KifElement*> it;

	KifFloatPrimeMap* ref = new KifFloatPrimeMap(kifcode, NULL);
	KifFloatPrimeMap* bref = (KifFloatPrimeMap*)b;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	KifElement* kval;
	for (it = bref->values; !it.end(); it++) {
		if (A->values.find(it.key())) {
			kval = A->values[it.key()];
			if (it.value()->same(kval) == kifTRUE) {
				ref->push(it.key(), it.value());
				kval->Setreference();
			}
		}
	}
	return ref;
}

Exported KifElement* KifFloatPrimeMap::same(KifElement* a) {

	if (a->type != kifFloatPrimeMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifFloatPrimeMap* m = (KifFloatPrimeMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<double, KifElement*> it;
	primehash_value<double, KifElement*>* cell;
	for (it = m->values; !it.end(); it++) {
		cell = values.search(it.key());
		if (cell == NULL)
			return kifFALSE;
		if (it.value()->same(cell->value) == kifFALSE)
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifFloatPrimeMap::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->plus(a, b, context, autoself);

	KifFloatPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatPrimeMap*)a;

	prime_iterator<double, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnFloatKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->plus(ke, kref, context, true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->plus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->minus(a, b, context, autoself);

	KifFloatPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatPrimeMap*)a;

	prime_iterator<double, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnFloatKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->minus(ke, kref, context, true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->minus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->multiply(a, b, context, autoself);

	KifFloatPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatPrimeMap*)a;

	prime_iterator<double, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnFloatKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->multiply(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->multiply(ke, b, context, true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->divide(a, b, context, autoself);

	KifFloatPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatPrimeMap*)a;

	prime_iterator<double, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnFloatKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->divide(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->divide(ke, b, context, true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->mod(a, b, context, autoself);

	KifFloatPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatPrimeMap*)a;

	prime_iterator<double, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnFloatKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->mod(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->mod(ke, b, context, true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->shiftright(a, b, context, autoself);

	KifFloatPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatPrimeMap*)a;

	prime_iterator<double, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnFloatKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->shiftright(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftright(ke, b, context, true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->shiftleft(a, b, context, autoself);

	KifFloatPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatPrimeMap*)a;

	prime_iterator<double, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnFloatKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->shiftleft(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftleft(ke, b, context, true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->power(a, b, context, autoself);

	KifFloatPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatPrimeMap*)a;

	prime_iterator<double, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnFloatKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->power(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->power(ke, b, context, true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorFloatPrimeMap* iter = new KifIteratorFloatPrimeMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifFloatPrimeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;


	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		double n = 0;
		if (ke->aMapContainer()) {
			ke->Setpopped(1, true);
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				push(itr->IteratorKeyFloat(), itr->IteratorValue());
			itr->Release();
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		if (ke->aVectorContainer()) {
			ke->Setpopped(1, true);
			Clear();
			for (int it = 0; it < ke->Size(); it++)
				push(it, ke->ValueOnIndex(it));
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		if (ke->type == kifList) {
			ke->Setpopped(1, true);
			KifList* kvect = (KifList*)ke;
			Clear();
			{
				KifContainerLock _lock(kvect);
				list<KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
					push(n, *it);
					n++;
				}
			}
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		if (ke != this) {
			Clear();
			KifMap* kvect = (KifMap*)ke;
			//We copy all values from ke to this
			{
				KifContainerLock _lock(kvect);
				hmap<string, KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
					n = conversionintegerhexa(STR(it->first));
					push(n, it->second);
				}
			}
		}
		return true;
	}

	push(idx->Float(), ke);
	return true;
}

KifElement* KifFloatPrimeMap::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseFloat localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	prime_iterator<double, KifElement*> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

KifElement* KifFloatPrimeMap::Copy(KifDomain* kp, KifElement* dom) {
	KifFloatPrimeMap* ke = new KifFloatPrimeMap(kifcode, NULL);
	prime_iterator<double, KifElement*> itx;
	double n;
	for (itx = values; !itx.end(); itx++) {
		n = itx.key();
		if (itx.value() != NULL) {
			ke->values[n] = itx.value()->Copy(kp, dom);
			ke->values[n]->Setreference();
		}
		else
			ke->values[n] = NULL;
	}
	return ke;
}

KifElement* KifFloatPrimeMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}
		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					Setvalue(res, itr->IteratorKey());
				else
					res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

Exported void KifFloatPrimeMap::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror("KIF(689): Circular structure");
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	prime_iterator<double, KifElement*> itx;
	vector<KifElement*> doubles;
	{
		KifContainerLock _lock(this);
		for (itx = values; !itx.end(); itx++) {
			if (itx.value() != NULL)
				doubles.push_back(itx.value());
		}
	}
	for (int itx = 0; itx < doubles.size(); itx++)
		doubles[itx]->Setreference(inc);
	idgarbage = id;
}


string KifIteratorFloatPrimeMap::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

KifElement* KifIteratorFloatPrimeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (it.end())
		return kifNULL;
	return it.value()->Exec(contextualpattern, dom, val, idthread, callfunc);
}



static void resetMap(KifFloatPrimeMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	prime_iterator<double, KifElement*> itx;
	for (itx = kmap->values; !itx.end(); itx++)
		itx.value()->Resetreference(inc);

	if (clr && !popped)
		kmap->values.clear();
}

void KifFloatPrimeMap::Resetreference(int inc) {
	reference -= inc;
	if (reference > 0)
		resetMap(this, inc, popped, false);
	else {
		resetMap(this, inc + 1 + reference - popped, popped, true);
		if (popped == 1)
			popped = 0;
		else
			KifElement::Resetreference(0);
	}
}

void KifFloatPrimeMap::Clear() {
	KifContainerLock _lock(this);
	prime_iterator<double, KifElement*> itx;
	for (itx = values; !itx.end(); itx++)
		itx.value()->Resetreference(reference + 1);
	values.clear();
}

void KifFloatPrimeMap::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		double n = itr->IteratorKeyFloat();
		if (values.find(n) == false) {
			ke = itr->IteratorValue()->Atom();
			values[n] = ke;
			ke->Setreference(reference + 1);
		}
	}
	itr->Release();
}

KifElement* KifFloatPrimeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* ke;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			prime_iterator<double, KifElement*> it;
			for (it = values; !it.end(); it++)
				vect->Push(kifcode->Providefloat(it.key()));
			return vect;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifFloatPrimeMap* kmap = new KifFloatPrimeMap(kifcode, NULL);
			prime_iterator<double, KifElement*> it;
			for (it = values; !it.end(); it++) {
				ke = it.value()->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
				if (ke == kifRAISEERROR) {
					kmap->Release();
					return kifRAISEERROR;
				}
				kmap->push(it.key(), ke);
			}
			return kmap;
		}
		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifFloatPrimeMap* kmap = new KifFloatPrimeMap(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			double vleft = key->Float();
			double vright = keyright->Float();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<double, KifElement*> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->push(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
	if (key == kifNULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	double skey = key->Float();
	key->Release();
	primehash_value<double, KifElement*>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return cell->value;
}

void KifFloatPrimeMap::Pop(KifElement* ke) {
	double s = ke->Float();
	KifElement* kres;
	{
		KifContainerLock _lock(this);
		primehash_value<double, KifElement*>* cell = values.search(s);
		if (cell == NULL) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(kifErrorStrings[223]);
			return;
		}
		kres = cell->value;
		values.erase(s);
	}
	kres->Resetreference(reference + 1);
}

//To insert a value into a map through the API 
Exported void KifFloatPrimeMap::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	double n = convertfloat(STR(key));
	KifContainerLock _lock(this);
	primehash_value<double, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifFloatPrimeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	double n = convertfloat(key);
	primehash_value<double, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifFloatPrimeMap::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	double n = key->Float();
	KifContainerLock _lock(this);
	primehash_value<double, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}


void KifFloatPrimeMap::push(double n, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	primehash_value<double, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}

//----------------------------------------------------------------------------------
void KifPrimeMapStringString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapStringString* kmap = (KifPrimeMapStringString*)ke;
		prime_iterator<string, string> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string n = itr->IteratorKeyString();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueString();
	}
	itr->Release();
}

KifElement* KifPrimeMapStringString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<string, string> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapStringString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<string, string> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}


Exported void KifPrimeMapStringString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->String();
}

Exported void KifPrimeMapStringString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->String();
}

Exported void KifPrimeMapStringString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->String();
}

string KifPrimeMapStringString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<string, string> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it.key();
		stringing(res, sx);
		res << ":";
		sx = it.value();
		stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifPrimeMapStringString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<string, string> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it.key();
		jstringing(res, sx);
		res << ":";
		sx = it.value();
		jstringing(res, sx);
	}
	res << "}";
	return res.str();
}

KifElement* KifPrimeMapStringString::Newiterator(bool rev) {
	KifIteratorPrimeStringString* iter = new KifIteratorPrimeStringString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapStringString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	string s = ke->String();
	values.erase(s);
}

bool KifPrimeMapStringString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapStringString) {
			KifPrimeMapStringString* kvect = (KifPrimeMapStringString*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyString()] = itr->IteratorValueString();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++)
			values[it->first] = it->second->String();
		kmap->Release();
		return true;
	}
	values[idx->String()] = ke->String();
	return true;
}

KifElement* KifPrimeMapStringString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyString()] = res->String();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapStringString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<string, string> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Providestringraw(it.key()));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapStringString* kmap = new KifPrimeMapStringString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			string vleft = key->String();
			string vright = keyright->String();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<string, string> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNOELEMENT;
	string skey = key->String();
	key->Release();
	primehash_value<string, string>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providestringraw(cell->value);
}

Exported KifElement* KifPrimeMapStringString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<string, string> it;
	for (it = values; !it.end(); it++)
		kmap->push(it.key(), (KifElement*)kifcode->Providestring(it.value()));
	return kmap;
}

KifElement* KifPrimeMapStringString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseString localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<string, string> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifPrimeMapStringString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapStringString* m = (KifPrimeMapStringString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<string, string> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifPrimeMapStringString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapStringString* ref = (KifPrimeMapStringString*)a->Copy(NULL);
		string v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyString();
			if (ref->values.find(v) == false)
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifPrimeMapStringString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapStringString* ref = new KifPrimeMapStringString(kifcode, NULL);
		KifPrimeMapStringString* A = (KifPrimeMapStringString*)a->Copy(NULL);
		string v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		primehash_value<string, string>* cell;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyString();
			cell = A->values.search(v);
			if (cell != NULL) {
				if (cell->value == itr->IteratorValueString())
					ref->values[v] = cell->value;
			}
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifPrimeMapStringString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapStringString* ref = new KifPrimeMapStringString(kifcode, NULL);
		prime_hash<string, string>& val = ((KifPrimeMapStringString*)a)->values;
		string v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyString();
			if (val.find(v) == false)
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();

		prime_iterator<string, string> it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnKey(it.key()) == kifNOELEMENT)
				ref->values[it.key()] = it.value();
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifPrimeMapStringString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringString* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringString*)a;

	KifContainerLock _lock(this);

	prime_iterator<string, string> it;
	if (b->type == kifList) {
		KifList* bref = (KifList*)b;
		KifContainerLock _lock(bref);
		list<KifElement*>::iterator itl;
		for (itl = bref->values.begin(), it = ref->values.begin(); itl != bref->values.end(), it != ref->values.end(); it++, itl++)
			it.value(it.value() + (*itl)->String());
		return ref;
	}


	if (b->aVector()) {
		int j = 0;
		for (it = ref->values; !it.end(); it++) {
			if (j >= b->Size())
				break;
			it.value(it.value() + b->ValueOnIndex(j++)->String());
		}
		return ref;
	}

	if (b->type == kifPrimeMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMap* bref = (KifPrimeMap*)b;
		primehash_value<string, KifElement*>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value->String());
		}
		return ref;
	}

	if (b->type == kifPrimeMapStringString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMapStringString* bref = (KifPrimeMapStringString*)b;
		primehash_value<string, string>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}
		return ref;
	}

	string n = b->String();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + n);
	return ref;
}


KifElement* KifPrimeMapStringString::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapStringString* ke = new KifPrimeMapStringString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//----------------------------------------------------------------------------------
KifElement* KifPrimeMapStringInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<string, long> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapStringInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	prime_iterator<string, long> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}

void KifPrimeMapStringInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapStringInteger* kmap = (KifPrimeMapStringInteger*)ke;
		prime_iterator<string, long> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string n = itr->IteratorKeyString();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifPrimeMapStringInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}


Exported void KifPrimeMapStringInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}

Exported void KifPrimeMapStringInteger::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}



string KifPrimeMapStringInteger::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<string, long> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it.key();
		stringing(res, sx);
		res << ":" << it.value();
	}
	res << "}";
	return res.str();
}


string KifPrimeMapStringInteger::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<string, long> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it.key();
		jstringing(res, sx);
		res << ":" << it.value();
	}
	res << "}";
	return res.str();
}


KifElement* KifPrimeMapStringInteger::Newiterator(bool rev) {
	KifIteratorPrimeMapStringInteger* iter = new KifIteratorPrimeMapStringInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}


void KifPrimeMapStringInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	string s = ke->String();
	values.erase(s);
}

bool KifPrimeMapStringInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapStringInteger) {
			KifPrimeMapStringInteger* kvect = (KifPrimeMapStringInteger*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyString()] = itr->IteratorValueInteger();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++)
			values[it->first] = it->second->Integer();
		kmap->Release();
		return true;
	}

	values[idx->String()] = ke->Integer();
	return true;
}

KifElement* KifPrimeMapStringInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyString()] = res->Integer();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapStringInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<string, long> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Providestringraw(it.key()));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapStringInteger* kmap = new KifPrimeMapStringInteger(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			string vleft = key->String();
			string vright = keyright->String();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<string, long> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	string skey = key->String();
	key->Release();
	primehash_value<string, long>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Provideinteger(cell->value);
}

Exported KifElement* KifPrimeMapStringInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<string, long> it;
	for (it = values; !it.end(); it++)
		kmap->push(it.key(), (KifElement*)kifcode->Provideinteger(it.value()));
	return kmap;
}

KifElement* KifPrimeMapStringInteger::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseString localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<string, long> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifPrimeMapStringInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapStringInteger* m = (KifPrimeMapStringInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<string, long> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifPrimeMapStringInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapStringInteger* ke = new KifPrimeMapStringInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifPrimeMapStringInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() | cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() | v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() | v);
	return ref;
}

KifElement* KifPrimeMapStringInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()&cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value()&v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()&v);
	return ref;
}

KifElement* KifPrimeMapStringInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() ^ cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() ^ v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() ^ v);
	return ref;
}

KifElement* KifPrimeMapStringInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() + v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + v);

	return ref;
}

KifElement* KifPrimeMapStringInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() - cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() - v);
		}
		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() - v);
	return ref;
}

KifElement* KifPrimeMapStringInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()*cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value()*v);
		}
		return ref;
	}
	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()*v);
	return ref;
}

KifElement* KifPrimeMapStringInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	long v;
	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() / v);
	return ref;
}

KifElement* KifPrimeMapStringInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	double v, vv;
	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = it.value();
				vv = cell->value;
				it.value(pow(v, vv));
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			vv = b->ValueOnKeyFloat(it.key(), add);
			if (add) {
				v = it.value();
				it.value(pow(v, vv));
			}
		}
		return ref;
	}


	vv = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		v = it.value();
		it.value(pow(v, vv));
	}
	return ref;
}

KifElement* KifPrimeMapStringInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() >> cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() >> v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() >> v);
	return ref;
}

KifElement* KifPrimeMapStringInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() << cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() << v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() << v);
	return ref;
}

KifElement* KifPrimeMapStringInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	long v;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() % v);
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);

				it.value(it.value() % v);
			}
		}
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);

	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() % v);
	return ref;
}

//----------------------------------------------------------------------------------------
void KifPrimeMapStringFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapStringFloat* kmap = (KifPrimeMapStringFloat*)ke;
		prime_iterator<string, double> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string n = itr->IteratorKeyString();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueFloat();
	}
	itr->Release();
}

KifElement* KifPrimeMapStringFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<string, double> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapStringFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	prime_iterator<string, double> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}


Exported void KifPrimeMapStringFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifPrimeMapStringFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifPrimeMapStringFloat::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->Float();
}


string KifPrimeMapStringFloat::String() {
	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	stringstream res;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it.key();
		stringing(res, sx);
		res << ":" << it.value();
	}
	res << "}";
	return res.str();
}

string KifPrimeMapStringFloat::JSonString() {
	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	stringstream res;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it.key();
		jstringing(res, sx);
		res << ":" << it.value();
	}
	res << "}";
	return res.str();
}

KifElement* KifPrimeMapStringFloat::Newiterator(bool rev) {
	KifIteratorPrimeMapStringFloat* iter = new KifIteratorPrimeMapStringFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapStringFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	string s = ke->String();
	values.erase(s);
}

bool KifPrimeMapStringFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapStringFloat) {
			KifPrimeMapStringFloat* kvect = (KifPrimeMapStringFloat*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyString()] = itr->IteratorValueFloat();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++)
			values[it->first] = it->second->Float();
		kmap->Release();
		return true;
	}
	values[idx->String()] = ke->Float();
	return true;
}

KifElement* KifPrimeMapStringFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyString()] = res->Float();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapStringFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<string, double> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Providestringraw(it.key()));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapStringFloat* kmap = new KifPrimeMapStringFloat(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			string vleft = key->String();
			string vright = keyright->String();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<string, double> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	string skey = key->String();
	key->Release();
	primehash_value<string, double>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providefloat(cell->value);
}

Exported KifElement* KifPrimeMapStringFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<string, double> it;
	for (it = values; !it.end(); it++)
		kmap->push(it.key(), (KifElement*)kifcode->Providefloat(it.value()));
	return kmap;
}

KifElement* KifPrimeMapStringFloat::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseString localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<string, double> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifPrimeMapStringFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapStringFloat* m = (KifPrimeMapStringFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<string, double> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifPrimeMapStringFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value((long)it.value() | cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value((long)it.value() | v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() | v);
	return ref;
}

KifElement* KifPrimeMapStringFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value((long)it.value()&cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value((long)it.value()&v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value()&v);
	return ref;
}

KifElement* KifPrimeMapStringFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value((long)it.value() ^ cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value((long)it.value() ^ v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() ^ v);
	return ref;
}

KifElement* KifPrimeMapStringFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapStringFloat) {
		KifPrimeMapStringFloat* bref = (KifPrimeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFloat(it.key(), add);
			if (add)
				it.value(it.value() + v);
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + v);
	return ref;
}

KifElement* KifPrimeMapStringFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapStringFloat) {
		KifPrimeMapStringFloat* bref = (KifPrimeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() - cell->value);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFloat(it.key(), add);
			if (add)
				it.value(it.value() - v);
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() - v);
	return ref;
}

KifElement* KifPrimeMapStringFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapStringFloat) {
		KifPrimeMapStringFloat* bref = (KifPrimeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()*cell->value);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFloat(it.key(), add);
			if (add)
				it.value(it.value()*v);
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()*v);
	return ref;
}

KifElement* KifPrimeMapStringFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	double v;
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapStringFloat) {
		KifPrimeMapStringFloat* bref = (KifPrimeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFloat(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}

	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() / v);
	return ref;
}

KifElement* KifPrimeMapStringFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	long v;
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapStringFloat) {
		KifPrimeMapStringFloat* bref = (KifPrimeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value((long)it.value() % v);
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFloat(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value((long)it.value() % v);
			}
		}
		return ref;
	}

	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() % v);
	return ref;
}

KifElement* KifPrimeMapStringFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapStringFloat* ke = new KifPrimeMapStringFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifPrimeMapStringFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	prime_iterator<string, double> it;
	KifContainerLock _lock(this);
	double v, vv;

	if (b->type == kifPrimeMapStringFloat) {
		KifPrimeMapStringFloat* bref = (KifPrimeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = it.value();
				vv = cell->value;
				it.value(pow(v, vv));
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			vv = b->ValueOnKeyFloat(it.key(), add);
			if (add) {
				v = it.value();
				it.value(pow(v, vv));
			}
		}
		return ref;
	}

	vv = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		v = it.value();
		it.value(pow(v, vv));
	}
	return ref;
}

KifElement* KifPrimeMapStringFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value((long)it.value() >> cell->value);
		}
		return ref;
	}

	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value((long)it.value() >> v);
		}
		return ref;
	}

	v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() >> v);
	return ref;
}

KifElement* KifPrimeMapStringFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapStringFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapStringInteger) {
		KifPrimeMapStringInteger* bref = (KifPrimeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<string, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value((long)it.value() << cell->value);
		}
		return ref;
	}

	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value((long)it.value() << v);
		}
		return ref;
	}

	v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() << v);
	return ref;
}

//----------------------------------------------------------------------------------------
string KifIteratorPrimeMapIntegerInteger::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

KifElement* KifPrimeMapIntegerInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	prime_iterator<long, long> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapIntegerInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	prime_iterator<long, long> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}


void KifPrimeMapIntegerInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapIntegerInteger* kmap = (KifPrimeMapIntegerInteger*)ke;
		prime_iterator<long, long> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifPrimeMapIntegerInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(key);
	values[nb] = val->Integer();
}

Exported void KifPrimeMapIntegerInteger::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long nb = key->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->Integer();
}


Exported void KifPrimeMapIntegerInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(STR(key));
	values[nb] = val->Integer();
}


string KifPrimeMapIntegerInteger::String() {
	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	stringstream res;
	res << "{";
	bool beg = true;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":" << it.value();
	}
	res << "}";
	return res.str();
}

KifElement* KifPrimeMapIntegerInteger::Newiterator(bool rev) {
	KifIteratorPrimeMapIntegerInteger* iter = new KifIteratorPrimeMapIntegerInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapIntegerInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	long s = ke->Integer();
	values.erase(s);
}

bool KifPrimeMapIntegerInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapIntegerInteger) {
			KifPrimeMapIntegerInteger* kvect = (KifPrimeMapIntegerInteger*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyInteger()] = itr->IteratorValueInteger();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		long nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = conversionintegerhexa(STR(it->first));
			values[nb] = it->second->Integer();
		}
		kmap->Release();
		return true;
	}

	values[idx->Integer()] = ke->Integer();
	return true;
}

KifElement* KifPrimeMapIntegerInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyInteger()] = res->Integer();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapIntegerInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<long, long> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Provideinteger(it.key()));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapIntegerInteger* kmap = new KifPrimeMapIntegerInteger(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<long, long> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	long skey = key->Integer();
	key->Release();
	primehash_value<long, long>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Provideinteger(cell->value);
}

Exported KifElement* KifPrimeMapIntegerInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<long, long> it;
	char buffer[50];
	for (it = values; !it.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it.key());
		kmap->Push(buffer, (KifElement*)kifcode->Provideinteger(it.value()));
	}
	return kmap;
}

KifElement* KifPrimeMapIntegerInteger::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseInteger localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<long, long> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifPrimeMapIntegerInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapIntegerInteger* m = (KifPrimeMapIntegerInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<long, long> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifPrimeMapIntegerInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapIntegerInteger* ke = new KifPrimeMapIntegerInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifPrimeMapIntegerInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() | cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value(it.value() | v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() | v);
	return ref;
}

KifElement* KifPrimeMapIntegerInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()&cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value(it.value()&v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()&v);
	return ref;
}

KifElement* KifPrimeMapIntegerInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() ^ cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value(it.value() ^ v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() ^ v);
	return ref;
}

KifElement* KifPrimeMapIntegerInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value(it.value() + v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + v);

	return ref;
}

KifElement* KifPrimeMapIntegerInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() - cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value(it.value() - v);
		}
		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() - v);
	return ref;
}

KifElement* KifPrimeMapIntegerInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()*cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value(it.value()*v);
		}
		return ref;
	}
	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()*v);
	return ref;
}

KifElement* KifPrimeMapIntegerInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	long v;
	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() / v);
	return ref;
}

KifElement* KifPrimeMapIntegerInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	double v, vv;
	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = it.value();
				vv = cell->value;
				it.value(pow(v, vv));
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			vv = b->ValueOnKeyIFloat(it.key(), add);
			if (add) {
				v = it.value();
				it.value(pow(v, vv));
			}
		}
		return ref;
	}


	vv = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		v = it.value();
		it.value(pow(v, vv));
	}
	return ref;
}

KifElement* KifPrimeMapIntegerInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() >> cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value(it.value() >> v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() >> v);
	return ref;
}

KifElement* KifPrimeMapIntegerInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() << cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value(it.value() << v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() << v);
	return ref;
}

KifElement* KifPrimeMapIntegerInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, long> it;
	long v;
	if (b->type == kifPrimeMapIntegerInteger) {
		KifPrimeMapIntegerInteger* bref = (KifPrimeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() % v);
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);

				it.value(it.value() % v);
			}
		}
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);

	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() % v);
	return ref;
}


//---------------------------------------------------------------------------------------------
string KifIteratorPrimeMapIntegerString::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

void KifPrimeMapIntegerString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapIntegerString* kmap = (KifPrimeMapIntegerString*)ke;
		prime_iterator<long, string> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueString();
	}
	itr->Release();
}

KifElement* KifPrimeMapIntegerString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	prime_iterator<long, string> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapIntegerString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<long, string> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}

Exported void KifPrimeMapIntegerString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(STR(key));
	values[nb] = val->String();
}

Exported void KifPrimeMapIntegerString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(key);
	values[nb] = val->String();
}

Exported void KifPrimeMapIntegerString::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long nb = key->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->String();
}


string KifPrimeMapIntegerString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<long, string> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":";
		sx = it.value();
		stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifPrimeMapIntegerString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<long, string> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":";
		sx = it.value();
		jstringing(res, sx);
	}
	res << "}";
	return res.str();
}

KifElement* KifPrimeMapIntegerString::Newiterator(bool rev) {
	KifIteratorPrimeMapIntegerString* iter = new KifIteratorPrimeMapIntegerString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapIntegerString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	long s = ke->Integer();
	values.erase(s);
}

bool KifPrimeMapIntegerString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapIntegerString) {
			KifPrimeMapIntegerString* kvect = (KifPrimeMapIntegerString*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyInteger()] = itr->IteratorValueString();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		long nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = conversionintegerhexa(STR(it->first));
			values[nb] = it->second->String();
		}
		kmap->Release();
		return true;
	}
	values[idx->Integer()] = ke->String();
	return true;
}

KifElement* KifPrimeMapIntegerString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyInteger()] = res->String();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapIntegerString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<long, string> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Provideinteger(it.key()));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapIntegerString* kmap = new KifPrimeMapIntegerString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<long, string> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNOELEMENT;

	long skey = key->Integer();
	key->Release();
	primehash_value<long, string>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providestringraw(cell->value);
}

Exported KifElement* KifPrimeMapIntegerString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<long, string> it;
	char buffer[50];
	for (it = values; !it.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it.key());
		kmap->Push(buffer, (KifElement*)kifcode->Providestring(it.value()));
	}
	return kmap;
}

KifElement* KifPrimeMapIntegerString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseInteger localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<long, string> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifPrimeMapIntegerString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapIntegerString* m = (KifPrimeMapIntegerString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<long, string> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifPrimeMapIntegerString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapIntegerString* ref = (KifPrimeMapIntegerString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (ref->values.find(v) == false)
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifPrimeMapIntegerString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapIntegerString* ref = new KifPrimeMapIntegerString(kifcode, NULL);
		KifPrimeMapIntegerString* A = (KifPrimeMapIntegerString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		primehash_value<long, string>* cell;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			cell = A->values.search(v);
			if (cell != NULL) {
				if (cell->value == itr->IteratorValueString())
					ref->values[v] = cell->value;
			}
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifPrimeMapIntegerString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapIntegerString* ref = new KifPrimeMapIntegerString(kifcode, NULL);
		prime_hash<long, string>& val = ((KifPrimeMapIntegerString*)a)->values;
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (val.find(v) == false)
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();

		prime_iterator<long, string> it;
		char buffer[50];
		for (it = val.begin(); it != val.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it.key());
			if (b->ValueOnKey(buffer) == kifNOELEMENT)
				ref->values[it.key()] = it.value();
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifPrimeMapIntegerString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerString* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerString*)a;

	KifContainerLock _lock(this);

	prime_iterator<long, string> it;
	if (b->type == kifList) {
		KifList* bref = (KifList*)b;
		KifContainerLock _lock(bref);
		list<KifElement*>::iterator itl;
		for (itl = bref->values.begin(), it = ref->values.begin(); itl != bref->values.end(), it != ref->values.end(); it++, itl++)
			it.value(it.value() + (*itl)->String());
		return ref;
	}


	if (b->aVector()) {
		int j = 0;
		for (it = ref->values; !it.end(); it++) {
			if (j >= b->Size())
				break;
			it.value(it.value() + b->ValueOnIndex(j++)->String());
		}
		return ref;
	}

	if (b->type == kifPrimeMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMap* bref = (KifPrimeMap*)b;
		char buffer[50];
		primehash_value<string, KifElement*>* cell;
		for (it = ref->values; !it.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it.key());
			cell = bref->values.search(buffer);
			if (cell != NULL)
				it.value(it.value() + cell->value->String());
		}
		return ref;
	}

	if (b->type == kifPrimeMapIntegerString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMapIntegerString* bref = (KifPrimeMapIntegerString*)b;
		primehash_value<long, string>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}
		return ref;
	}

	string n = b->String();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + n);
	return ref;
}


KifElement* KifPrimeMapIntegerString::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapIntegerString* ke = new KifPrimeMapIntegerString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//---------------------------------------------------------------------------------------------
string KifIteratorPrimeMapIntegerFloat::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

KifElement* KifPrimeMapIntegerFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	prime_iterator<long, double> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapIntegerFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	prime_iterator<long, double> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}


void KifPrimeMapIntegerFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapIntegerFloat* kmap = (KifPrimeMapIntegerFloat*)ke;
		prime_iterator<long, double> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifPrimeMapIntegerFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(key);
	values[nb] = val->Float();
}

Exported void KifPrimeMapIntegerFloat::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long nb = key->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->Float();
}


Exported void KifPrimeMapIntegerFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(STR(key));
	values[nb] = val->Float();
}


string KifPrimeMapIntegerFloat::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<long, double> it;
	res << "{";
	bool beg = true;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":" << it.value();
	}
	res << "}";
	return res.str();
}

KifElement* KifPrimeMapIntegerFloat::Newiterator(bool rev) {
	KifIteratorPrimeMapIntegerFloat* iter = new KifIteratorPrimeMapIntegerFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapIntegerFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	long s = ke->Integer();
	values.erase(s);
}

bool KifPrimeMapIntegerFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapIntegerFloat) {
			KifPrimeMapIntegerFloat* kvect = (KifPrimeMapIntegerFloat*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyInteger()] = itr->IteratorValueFloat();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		long nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = conversionintegerhexa(STR(it->first));
			values[nb] = it->second->Float();
		}
		kmap->Release();
		return true;
	}
	values[idx->Integer()] = ke->Float();
	return true;
}

KifElement* KifPrimeMapIntegerFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyInteger()] = res->Float();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapIntegerFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<long, double> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Provideinteger(it.key()));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapIntegerFloat* kmap = new KifPrimeMapIntegerFloat(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<long, double> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	long skey = key->Integer();
	key->Release();
	primehash_value<long, double>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providefloat(cell->value);
}

Exported KifElement* KifPrimeMapIntegerFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<long, double> it;
	char buffer[50];
	for (it = values; !it.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it.key());
		kmap->Push(buffer, (KifElement*)kifcode->Providefloat(it.value()));
	}
	return kmap;
}

KifElement* KifPrimeMapIntegerFloat::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseInteger localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<long, double> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifPrimeMapIntegerFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapIntegerFloat* m = (KifPrimeMapIntegerFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<long, double> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifPrimeMapIntegerFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, double> it;
	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value((long)it.value() | v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() | v);
	return ref;
}

KifElement* KifPrimeMapIntegerFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, double> it;

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value((long)it.value()&v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value()&v);
	return ref;
}

KifElement* KifPrimeMapIntegerFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, double> it;

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value((long)it.value() ^ v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() ^ v);
	return ref;
}

KifElement* KifPrimeMapIntegerFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	prime_iterator<long, double> it;
	if (b->type == kifPrimeMapIntegerFloat) {
		KifPrimeMapIntegerFloat* bref = (KifPrimeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIFloat(it.key(), add);
			if (add)
				it.value(it.value() + v);
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + v);
	return ref;
}

KifElement* KifPrimeMapIntegerFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	prime_iterator<long, double> it;
	if (b->type == kifPrimeMapIntegerFloat) {
		KifPrimeMapIntegerFloat* bref = (KifPrimeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() - cell->value);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIFloat(it.key(), add);
			if (add)
				it.value(it.value() - v);
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() - v);
	return ref;
}

KifElement* KifPrimeMapIntegerFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	prime_iterator<long, double> it;
	if (b->type == kifPrimeMapIntegerFloat) {
		KifPrimeMapIntegerFloat* bref = (KifPrimeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()*cell->value);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIFloat(it.key(), add);
			if (add)
				it.value(it.value()*v);
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()*v);
	return ref;
}

KifElement* KifPrimeMapIntegerFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	double v;
	prime_iterator<long, double> it;
	if (b->type == kifPrimeMapIntegerFloat) {
		KifPrimeMapIntegerFloat* bref = (KifPrimeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIFloat(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}

	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() / v);
	return ref;
}

KifElement* KifPrimeMapIntegerFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	long v;
	prime_iterator<long, double> it;
	if (b->type == kifPrimeMapIntegerFloat) {
		KifPrimeMapIntegerFloat* bref = (KifPrimeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value((long)it.value() % v);
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value((long)it.value() % v);
			}
		}
		return ref;
	}

	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() % v);
	return ref;
}

KifElement* KifPrimeMapIntegerFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapIntegerFloat* ke = new KifPrimeMapIntegerFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifPrimeMapIntegerFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	prime_iterator<long, double> it;
	KifContainerLock _lock(this);
	double v, vv;

	if (b->type == kifPrimeMapIntegerFloat) {
		KifPrimeMapIntegerFloat* bref = (KifPrimeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<long, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = it.value();
				vv = cell->value;
				it.value(pow(v, vv));
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			vv = b->ValueOnKeyIFloat(it.key(), add);
			if (add) {
				v = it.value();
				it.value(pow(v, vv));
			}
			return ref;
		}
	}
	vv = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		v = it.value();
		it.value(pow(v, vv));
	}
	return ref;
}

KifElement* KifPrimeMapIntegerFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, double> it;

	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value((long)it.value() >> v);
		}
		return ref;
	}

	v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() >> v);
	return ref;
}

KifElement* KifPrimeMapIntegerFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<long, double> it;

	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyIInteger(it.key(), add);
			if (add)
				it.value((long)it.value() << v);
		}
		return ref;
	}

	v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() << v);
	return ref;
}
//-------------------------------------------------------------------------------
string KifIteratorPrimeMapFloatInteger::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

KifElement* KifPrimeMapFloatInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	prime_iterator<double, long> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}

KifElement* KifPrimeMapFloatInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	prime_iterator<double, long> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}


void KifPrimeMapFloatInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapFloatInteger* kmap = (KifPrimeMapFloatInteger*)ke;
		prime_iterator<double, long> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		double n = itr->IteratorKeyFloat();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifPrimeMapFloatInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(key);
	values[nb] = val->Integer();
}

Exported void KifPrimeMapFloatInteger::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->Integer();
}


Exported void KifPrimeMapFloatInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(STR(key));
	values[nb] = val->Integer();
}


string KifPrimeMapFloatInteger::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<double, long> it;
	res << "{";
	bool beg = true;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":" << it.value();
	}
	res << "}";
	return res.str();
}

KifElement* KifPrimeMapFloatInteger::Newiterator(bool rev) {
	KifIteratorPrimeMapFloatInteger* iter = new KifIteratorPrimeMapFloatInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapFloatInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	double s = ke->Float();
	values.erase(s);
}

bool KifPrimeMapFloatInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapFloatInteger) {
			KifPrimeMapFloatInteger* kvect = (KifPrimeMapFloatInteger*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyFloat()] = itr->IteratorValueInteger();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		double nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = convertfloat(STR(it->first));
			values[nb] = it->second->Integer();
		}
		kmap->Release();
		return true;
	}

	values[idx->Float()] = ke->Integer();
	return true;
}

KifElement* KifPrimeMapFloatInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyFloat()] = res->Integer();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapFloatInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<double, long> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Providefloat(it.key()));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapFloatInteger* kmap = new KifPrimeMapFloatInteger(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			double vleft = key->Float();
			double vright = keyright->Float();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<double, long> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	double skey = key->Float();
	key->Release();
	primehash_value<double, long>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Provideinteger(cell->value);
}

Exported KifElement* KifPrimeMapFloatInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<double, long> it;
	char buffer[50];
	for (it = values; !it.end(); it++) {
		sprintf_s(buffer, 50, "%g", it.key());
		kmap->Push(buffer, (KifElement*)kifcode->Provideinteger(it.value()));
	}
	return kmap;
}

KifElement* KifPrimeMapFloatInteger::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseFloat localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<double, long> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifPrimeMapFloatInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapFloatInteger* m = (KifPrimeMapFloatInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<double, long> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifPrimeMapFloatInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapFloatInteger* ke = new KifPrimeMapFloatInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifPrimeMapFloatInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() | cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value(it.value() | v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() | v);
	return ref;
}

KifElement* KifPrimeMapFloatInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()&cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value(it.value()&v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()&v);
	return ref;
}

KifElement* KifPrimeMapFloatInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() ^ cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value(it.value() ^ v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() ^ v);
	return ref;
}

KifElement* KifPrimeMapFloatInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value(it.value() + v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + v);

	return ref;
}

KifElement* KifPrimeMapFloatInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() - cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value(it.value() - v);
		}
		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() - v);
	return ref;
}

KifElement* KifPrimeMapFloatInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()*cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value(it.value()*v);
		}
		return ref;
	}
	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()*v);
	return ref;
}

KifElement* KifPrimeMapFloatInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	long v;
	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() / v);
	return ref;
}

KifElement* KifPrimeMapFloatInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	double v, vv;
	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = it.value();
				vv = cell->value;
				it.value(pow(v, vv));
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			vv = b->ValueOnKeyFFloat(it.key(), add);
			if (add) {
				v = it.value();
				it.value(pow(v, vv));
			}
		}
		return ref;
	}


	vv = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		v = it.value();
		it.value(pow(v, vv));
	}
	return ref;
}

KifElement* KifPrimeMapFloatInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() >> cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value(it.value() >> v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() >> v);
	return ref;
}

KifElement* KifPrimeMapFloatInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() << cell->value);
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value(it.value() << v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() << v);
	return ref;
}

KifElement* KifPrimeMapFloatInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, long> it;
	long v;
	if (b->type == kifPrimeMapFloatInteger) {
		KifPrimeMapFloatInteger* bref = (KifPrimeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() % v);
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);

				it.value(it.value() % v);
			}
		}
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);

	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() % v);
	return ref;
}


//---------------------------------------------------------------------------------------------
string KifIteratorPrimeMapFloatString::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

void KifPrimeMapFloatString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapFloatString* kmap = (KifPrimeMapFloatString*)ke;
		prime_iterator<double, string> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		double n = itr->IteratorKeyFloat();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueString();
	}
	itr->Release();
}

KifElement* KifPrimeMapFloatString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	prime_iterator<double, string> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapFloatString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<double, string> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}


Exported void KifPrimeMapFloatString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(STR(key));
	values[nb] = val->String();
}

Exported void KifPrimeMapFloatString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(key);
	values[nb] = val->String();
}

Exported void KifPrimeMapFloatString::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->String();
}


string KifPrimeMapFloatString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<double, string> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":";
		sx = it.value();
		stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifPrimeMapFloatString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<double, string> it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":";
		sx = it.value();
		jstringing(res, sx);
	}
	res << "}";
	return res.str();
}

KifElement* KifPrimeMapFloatString::Newiterator(bool rev) {
	KifIteratorPrimeMapFloatString* iter = new KifIteratorPrimeMapFloatString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapFloatString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	double s = ke->Float();
	values.erase(s);
}

bool KifPrimeMapFloatString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapFloatString) {
			KifPrimeMapFloatString* kvect = (KifPrimeMapFloatString*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyFloat()] = itr->IteratorValueString();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		double nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = convertfloat(STR(it->first));
			values[nb] = it->second->String();
		}
		kmap->Release();
		return true;
	}
	values[idx->Float()] = ke->String();
	return true;
}

KifElement* KifPrimeMapFloatString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyFloat()] = res->String();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapFloatString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<double, string> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Providefloat(it.key()));

			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapFloatString* kmap = new KifPrimeMapFloatString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			double vleft = key->Float();
			double vright = keyright->Float();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<double, string> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNOELEMENT;
	double skey = key->Float();
	key->Release();
	primehash_value<double, string>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providestringraw(cell->value);
}

Exported KifElement* KifPrimeMapFloatString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<double, string> it;
	char buffer[50];
	for (it = values; !it.end(); it++) {
		sprintf_s(buffer, 50, "%g", it.key());
		kmap->Push(buffer, (KifElement*)kifcode->Providestring(it.value()));
	}
	return kmap;
}

KifElement* KifPrimeMapFloatString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseFloat localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<double, string> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifPrimeMapFloatString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapFloatString* m = (KifPrimeMapFloatString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<double, string> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifPrimeMapFloatString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapFloatString* ref = (KifPrimeMapFloatString*)a->Copy(NULL);
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (ref->values.find(v) == false)
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifPrimeMapFloatString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapFloatString* ref = new KifPrimeMapFloatString(kifcode, NULL);
		KifPrimeMapFloatString* A = (KifPrimeMapFloatString*)a->Copy(NULL);
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		primehash_value<double, string>* cell;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			cell = A->values.search(v);
			if (cell != NULL) {
				if (cell->value == itr->IteratorValueString())
					ref->values[v] = cell->value;
			}
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifPrimeMapFloatString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapFloatString* ref = new KifPrimeMapFloatString(kifcode, NULL);
		prime_hash<double, string>& val = ((KifPrimeMapFloatString*)a)->values;
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (val.find(v) == false)
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();

		prime_iterator<double, string> it;
		char buffer[50];
		for (it = val.begin(); it != val.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it.key());
			if (b->ValueOnKey(buffer) == kifNOELEMENT)
				ref->values[it.key()] = it.value();
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifPrimeMapFloatString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatString* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatString*)a;

	KifContainerLock _lock(this);

	prime_iterator<double, string> it;
	if (b->type == kifList) {
		KifList* bref = (KifList*)b;
		KifContainerLock _lock(bref);
		list<KifElement*>::iterator itl;
		for (itl = bref->values.begin(), it = ref->values.begin(); itl != bref->values.end(), it != ref->values.end(); it++, itl++)
			it.value(it.value() + (*itl)->String());
		return ref;
	}


	if (b->aVector()) {
		int j = 0;
		for (it = ref->values; !it.end(); it++) {
			if (j >= b->Size())
				break;
			it.value(it.value() + b->ValueOnIndex(j++)->String());
		}
		return ref;
	}

	if (b->type == kifPrimeMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMap* bref = (KifPrimeMap*)b;
		char buffer[50];
		primehash_value<string, KifElement*>* cell;
		for (it = ref->values; !it.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it.key());
			cell = bref->values.search(buffer);
			if (cell != NULL) {
				it.value(it.value() + cell->value->String());
			}
		}
		return ref;
	}

	if (b->type == kifPrimeMapFloatString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMapFloatString* bref = (KifPrimeMapFloatString*)b;
		primehash_value<double, string>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}
		return ref;
	}

	string n = b->String();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + n);
	return ref;
}


KifElement* KifPrimeMapFloatString::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapFloatString* ke = new KifPrimeMapFloatString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//---------------------------------------------------------------------------------------------
string KifIteratorPrimeMapFloatFloat::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

KifElement* KifPrimeMapFloatFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	prime_iterator<double, double> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapFloatFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	prime_iterator<double, double> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}

void KifPrimeMapFloatFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapFloatFloat* kmap = (KifPrimeMapFloatFloat*)ke;
		prime_iterator<double, double> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		double n = itr->IteratorKeyFloat();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifPrimeMapFloatFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(key);
	values[nb] = val->Float();
}

Exported void KifPrimeMapFloatFloat::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->Float();
}


Exported void KifPrimeMapFloatFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(STR(key));
	values[nb] = val->Float();
}


string KifPrimeMapFloatFloat::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<double, double> it;
	res << "{";
	bool beg = true;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":" << it.value();
	}
	res << "}";
	return res.str();
}

KifElement* KifPrimeMapFloatFloat::Newiterator(bool rev) {
	KifIteratorPrimeMapFloatFloat* iter = new KifIteratorPrimeMapFloatFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapFloatFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	double s = ke->Float();
	values.erase(s);
}

bool KifPrimeMapFloatFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapFloatFloat) {
			KifPrimeMapFloatFloat* kvect = (KifPrimeMapFloatFloat*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyFloat()] = itr->IteratorValueFloat();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		double nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = convertfloat(STR(it->first));
			values[nb] = it->second->Float();
		}
		kmap->Release();
		return true;
	}
	values[idx->Float()] = ke->Float();
	return true;
}

KifElement* KifPrimeMapFloatFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyFloat()] = res->Float();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapFloatFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<double, double> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Providefloat(it.key()));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());
		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapFloatFloat* kmap = new KifPrimeMapFloatFloat(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			double vleft = key->Float();
			double vright = keyright->Float();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<double, double> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	double skey = key->Float();
	key->Release();
	primehash_value<double, double>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providefloat(cell->value);
}

Exported KifElement* KifPrimeMapFloatFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<double, double> it;
	char buffer[50];
	for (it = values; !it.end(); it++) {
		sprintf_s(buffer, 50, "%g", it.key());
		kmap->Push(buffer, (KifElement*)kifcode->Providefloat(it.value()));
	}
	return kmap;
}

KifElement* KifPrimeMapFloatFloat::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseFloat localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<double, double> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifPrimeMapFloatFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapFloatFloat* m = (KifPrimeMapFloatFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<double, double> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifPrimeMapFloatFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, double> it;
	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value((long)it.value() | v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() | v);
	return ref;
}

KifElement* KifPrimeMapFloatFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, double> it;

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value((long)it.value()&v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value()&v);
	return ref;
}

KifElement* KifPrimeMapFloatFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, double> it;

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value((long)it.value() ^ v);
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() ^ v);
	return ref;
}

KifElement* KifPrimeMapFloatFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	prime_iterator<double, double> it;
	if (b->type == kifPrimeMapFloatFloat) {
		KifPrimeMapFloatFloat* bref = (KifPrimeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFFloat(it.key(), add);
			if (add)
				it.value(it.value() + v);
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + v);
	return ref;
}

KifElement* KifPrimeMapFloatFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	prime_iterator<double, double> it;
	if (b->type == kifPrimeMapFloatFloat) {
		KifPrimeMapFloatFloat* bref = (KifPrimeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() - cell->value);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFFloat(it.key(), add);
			if (add)
				it.value(it.value() - v);
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() - v);
	return ref;
}

KifElement* KifPrimeMapFloatFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	prime_iterator<double, double> it;
	if (b->type == kifPrimeMapFloatFloat) {
		KifPrimeMapFloatFloat* bref = (KifPrimeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()*cell->value);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFFloat(it.key(), add);
			if (add)
				it.value(it.value()*v);
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()*v);
	return ref;
}

KifElement* KifPrimeMapFloatFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	double v;
	prime_iterator<double, double> it;
	if (b->type == kifPrimeMapFloatFloat) {
		KifPrimeMapFloatFloat* bref = (KifPrimeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFFloat(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}
		}
		return ref;
	}

	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() / v);
	return ref;
}

KifElement* KifPrimeMapFloatFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	long v;
	prime_iterator<double, double> it;
	if (b->type == kifPrimeMapFloatFloat) {
		KifPrimeMapFloatFloat* bref = (KifPrimeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value((long)it.value() % v);
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value((long)it.value() % v);
			}
		}
		return ref;
	}

	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() % v);
	return ref;
}

KifElement* KifPrimeMapFloatFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapFloatFloat* ke = new KifPrimeMapFloatFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifPrimeMapFloatFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	prime_iterator<double, double> it;
	KifContainerLock _lock(this);
	double v, vv;
	if (b->type == kifPrimeMapFloatFloat) {
		KifPrimeMapFloatFloat* bref = (KifPrimeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<double, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = it.value();
				vv = cell->value;
				it.value(pow(v, vv));
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			vv = b->ValueOnKeyFFloat(it.key(), add);
			if (add) {
				v = it.value();
				it.value(pow(v, vv));
			}
		}
		return ref;
	}

	vv = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		v = it.value();
		it.value(pow(v, vv));
	}
	return ref;
}

KifElement* KifPrimeMapFloatFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, double> it;

	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value((long)it.value() >> v);
		}
		return ref;
	}

	v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() >> v);
	return ref;
}

KifElement* KifPrimeMapFloatFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<double, double> it;

	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFInteger(it.key(), add);
			if (add)
				it.value((long)it.value() << v);
		}
		return ref;
	}

	v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() << v);
	return ref;
}


KifElement* KifCode::KifParseVarIntegerBinMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifIntegerBinMap* kv = new KifIntegerBinMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarBinMapIntegerFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifBinMapIntegerFloat* kv = new KifBinMapIntegerFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarBinMapIntegerInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifBinMapIntegerInteger* kv = new KifBinMapIntegerInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarBinMapIntegerString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifBinMapIntegerString* kv = new KifBinMapIntegerString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarBinMapIntegerUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifBinMapIntegerUString* kv = new KifBinMapIntegerUString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMap* kv = new KifPrimeMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarUStringPrimeMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifUStringPrimeMap* kv = new KifUStringPrimeMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarIntegerPrimeMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifIntegerPrimeMap* kv = new KifIntegerPrimeMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarFloatPrimeMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifFloatPrimeMap* kv = new KifFloatPrimeMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapStringString* kv = new KifPrimeMapStringString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapStringInteger* kv = new KifPrimeMapStringInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapStringFloat* kv = new KifPrimeMapStringFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}


KifElement* KifCode::KifParseVarPrimeMapUStringUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapUStringUString* kv = new KifPrimeMapUStringUString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapUStringInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapUStringInteger* kv = new KifPrimeMapUStringInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapUStringFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapUStringFloat* kv = new KifPrimeMapUStringFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapIntegerUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapIntegerUString* kv = new KifPrimeMapIntegerUString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapFloatUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapFloatUString* kv = new KifPrimeMapFloatUString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapIntegerString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapIntegerString* kv = new KifPrimeMapIntegerString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapIntegerInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapIntegerInteger* kv = new KifPrimeMapIntegerInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapIntegerFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapIntegerFloat* kv = new KifPrimeMapIntegerFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapFloatString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapFloatString* kv = new KifPrimeMapFloatString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapFloatInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapFloatInteger* kv = new KifPrimeMapFloatInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapFloatFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapFloatFloat* kv = new KifPrimeMapFloatFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
void KifIntegerBinMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hash_bin<ushort, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifPrimeMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<string, KifElement*> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifPrimeMapStringString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<string, string> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifPrimeMapStringInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<string, long> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifPrimeMapStringFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<string, double> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifIntegerPrimeMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<long, KifElement*> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifPrimeMapIntegerString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<long, string> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifPrimeMapIntegerInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<long, long> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifPrimeMapIntegerFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<long, double> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifFloatPrimeMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<double, KifElement*> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifPrimeMapFloatString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<double, string> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifPrimeMapFloatInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<double, long> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifPrimeMapFloatFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<double, double> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifBinMapIntegerString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hash_bin<ushort, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifBinMapIntegerInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hash_bin<ushort, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------
void KifBinMapIntegerUString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hash_bin<ushort, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}

//-----------------------------
void KifBinMapIntegerFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hash_bin<ushort, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------
KifElement* KifUStringPrimeMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<wstring, KifElement*> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifUStringPrimeMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	prime_iterator<wstring, KifElement*> it;
	for (it = values; !it.end(); it++)
		kvect->Push(it.value());
	return kvect;
}

wstring KifUStringPrimeMap::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	prime_iterator<wstring, KifElement*> it;
	res << "{";
	bool beg = true;
	wstring sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ustringing(res, sx);
		res << ":";
		sx = it.value()->UString();
		if (!it.value()->aString())
			res << sx;
		else
			ustringing(res, sx);
	}

	res << "}";

	return res.str();
}


string KifUStringPrimeMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<wstring, KifElement*> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it.key();
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
		res << ":";
		sx = it.value()->StringForDisplay();
		if (!it.value()->aString())
			res << sx;
		else
			stringing(res, sx);
	}

	res << "}";

	return res.str();
}

string KifUStringPrimeMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<wstring, KifElement*> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it.key();
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
		res << ":" << it.value()->JSonString();
	}

	res << "}";

	return res.str();
}

string KifUStringPrimeMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<wstring, KifElement*> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it.key();
		sc_unicode_to_utf8(sx, ws);
		res << "'" << Trim(sx) << "'" << ":" << it.value()->BaseString();
	}

	res << "}";

	return res.str();
}

KifElement* KifUStringPrimeMap::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifUStringPrimeMap)
		return kifNULL;

	KifUStringPrimeMap* A = (KifUStringPrimeMap*)a;
	prime_iterator<wstring, KifElement*> it;

	KifUStringPrimeMap* ref = new KifUStringPrimeMap(kifcode, NULL);
	KifUStringPrimeMap* bref = (KifUStringPrimeMap*)b;
	wstring ky;
	KifContainerLock _lock(bref);

	for (it = bref->values; !it.end(); it++) {
		ky = it.key();
		if (A->values.find(ky) == false) {
			ref->Push(ky, it.value());
			it.value()->Setreference();
		}

	}


	KifContainerLock _locka(A);
	for (it = A->values; !it.end(); it++) {
		ky = it.key();
		if (bref->values.find(ky) == false) {
			ref->Push(ky, it.value());
			it.value()->Setreference();
		}

	}

	return ref;
}

KifElement* KifUStringPrimeMap::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifUStringPrimeMap)
		return kifNULL;

	KifUStringPrimeMap* A = (KifUStringPrimeMap*)a;
	prime_iterator<wstring, KifElement*> it;

	KifUStringPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringPrimeMap*)a->Copy(NULL);

	KifUStringPrimeMap* bref = (KifUStringPrimeMap*)b;
	wstring ky;
	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values; !it.end(); it++) {
		if (A->values.find(it.key()) == false) {
			ky = it.key();
			ref->Push(ky, it.value());
			it.value()->Setreference();
		}

	}

	return ref;
}

KifElement* KifUStringPrimeMap::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifUStringPrimeMap)
		return kifNULL;

	KifUStringPrimeMap* A = (KifUStringPrimeMap*)a;
	prime_iterator<wstring, KifElement*> it;
	KifElement* kval;
	KifUStringPrimeMap* ref = new KifUStringPrimeMap(kifcode, NULL);
	KifUStringPrimeMap* bref = (KifUStringPrimeMap*)b;
	wstring ky;

	primehash_value<wstring, KifElement*>* cell;
	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);

	for (it = bref->values; !it.end(); it++) {
		cell = A->values.search(it.key());
		if (cell != NULL) {
			kval = cell->value;
			if (it.value()->same(kval) == kifTRUE) {
				ky = it.key();
				ref->Push(ky, it.value());
				kval->Setreference();
			}

		}

	}

	return ref;
}

Exported KifElement* KifUStringPrimeMap::same(KifElement* a) {

	if (a->type != kifUStringPrimeMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifUStringPrimeMap* m = (KifUStringPrimeMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	primehash_value<wstring, KifElement*>* cell;
	prime_iterator<wstring, KifElement*> it;
	for (it = m->values; !it.end(); it++) {
		cell = values.search(it.key());
		if (cell == NULL)
			return kifFALSE;
		if (it.value()->same(cell->value) == kifFALSE)
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifUStringPrimeMap::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->plus(a, b, context, autoself);

	KifUStringPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringPrimeMap*)a;

	prime_iterator<wstring, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->plus(ke, kref, context, true);
			}

		}

		return ref;
	}


	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->plus(ke, b, context, true);
	}

	return ref;
}

KifElement* KifUStringPrimeMap::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->minus(a, b, context, autoself);

	KifUStringPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringPrimeMap*)a;

	prime_iterator<wstring, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->minus(ke, kref, context, true);
			}

		}

		return ref;
	}


	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->minus(ke, b, context, true);
	}

	return ref;
}

KifElement* KifUStringPrimeMap::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->multiply(a, b, context, autoself);

	KifUStringPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringPrimeMap*)a;

	prime_iterator<wstring, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->multiply(ke, kref, context, true);
			}

		}

		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->multiply(ke, b, context, true);
	}

	return ref;
}

KifElement* KifUStringPrimeMap::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->divide(a, b, context, autoself);

	KifUStringPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringPrimeMap*)a;

	prime_iterator<wstring, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->divide(ke, kref, context, true);
			}

		}

		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->divide(ke, b, context, true);
	}

	return ref;
}

KifElement* KifUStringPrimeMap::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->mod(a, b, context, autoself);

	KifUStringPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringPrimeMap*)a;

	prime_iterator<wstring, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->mod(ke, kref, context, true);
			}

		}

		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->mod(ke, b, context, true);
	}

	return ref;
}

KifElement* KifUStringPrimeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->shiftright(a, b, context, autoself);

	KifUStringPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringPrimeMap*)a;

	prime_iterator<wstring, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->shiftright(ke, kref, context, true);
			}

		}

		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftright(ke, b, context, true);
	}

	return ref;
}

KifElement* KifUStringPrimeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->shiftleft(a, b, context, autoself);

	KifUStringPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringPrimeMap*)a;

	prime_iterator<wstring, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->shiftleft(ke, kref, context, true);
			}

		}

		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftleft(ke, b, context, true);
	}

	return ref;
}

KifElement* KifUStringPrimeMap::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifUStringPrimeMap)
		return a->power(a, b, context, autoself);

	KifUStringPrimeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringPrimeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringPrimeMap*)a;

	prime_iterator<wstring, KifElement*> it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values; !it.end(); it++) {
			kref = b->ValueOnKey(it.key());
			if (kref != kifNOELEMENT) {
				ke = it.value();
				ke->power(ke, kref, context, true);
			}

		}

		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->power(ke, b, context, true);
	}

	return ref;
}

KifElement* KifUStringPrimeMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorUStringPrimeMap* iter = new KifIteratorUStringPrimeMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifUStringPrimeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	int nb;
	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}


		if (ke->aMapContainer()) {
			ke->Setpopped(1, true);
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				push(itr->IteratorKeyString(), itr->IteratorValue());
			itr->Release();
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}


		char ch[20];
		if (ke->aVectorContainer()) {
			ke->Setpopped(1, true);
			Clear();
			nb = 0;
			for (int it = 0; it < ke->Size(); it++) {
				sprintf_s(ch, 20, "%d", nb);
				Push(ch, ke->ValueOnIndex(it));
				nb++;
			}

			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}


		if (ke->type == kifList) {
			ke->Setpopped(1, true);
			KifList* kvect = (KifList*)ke;
			Clear();
			nb = 0;
			{
				KifContainerLock _lock(kvect);
				list<KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
					sprintf_s(ch, 20, "%d", nb);
					Push(ch, *it);
					nb++;
				}

			}

			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}


		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}

		if (ke != this) {
			Clear();
			KifMap* kvect = (KifMap*)ke;
			//We copy all values from ke to this
			{
				KifContainerLock _lock(kvect);
				hmap<string, KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++)
					push(it->first, it->second);
			}

		}

		return true;
	}


	Push(STR(idx->String()), ke);
	return true;
}

KifElement* KifUStringPrimeMap::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifUString* localstring;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	prime_iterator<wstring, KifElement*> it;
	for (it = values; !it.end(); it++) {
		localstring = kifcode->Provideustring(it.key());
		kvect->Push(localstring);
	}

	return kvect;
}

KifElement* KifUStringPrimeMap::Copy(KifDomain* kp, KifElement* dom) {
	KifUStringPrimeMap* ke = new KifUStringPrimeMap(kifcode, NULL);
	prime_iterator<wstring, KifElement*> itx;
	wstring n;
	for (itx = values; !itx.end(); itx++) {
		n = itx.key();
		if (itx.value() != NULL) {
			ke->values[n] = itx.value()->Copy(kp, dom);
			ke->values[n]->Setreference();
		}

		else
			ke->values[n] = NULL;
	}

	return ke;
}

KifElement* KifUStringPrimeMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}


		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}

				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}

					else {
						itr->Release();
						return kifTRUE;
					}

				}

			}

		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					Setvalue(res, itr->IteratorKey());
			}

		}

		itr->Release();
		return this;
	}


	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

Exported void KifUStringPrimeMap::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror("KIF(689): Circular structure");
		kifGlobals->executionbreak = 1;
		return;
	}

	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	prime_iterator<wstring, KifElement*> itx;
	vector<KifElement*> doubles;
	{
		KifContainerLock _lock(this);
		for (itx = values; !itx.end(); itx++) {
			if (itx.value() != NULL)
				doubles.push_back(itx.value());
		}

	}

	for (int itx = 0; itx < doubles.size(); itx++)
		doubles[itx]->Setreference(inc);
	idgarbage = id;
}

static void resetUStringPrimeMap(KifUStringPrimeMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	prime_iterator<wstring, KifElement*> itx;
	for (itx = kmap->values; !itx.end(); itx++)
		itx.value()->Resetreference(inc);

	if (clr && !popped)
		kmap->values.clear();
}


void KifUStringPrimeMap::Resetreference(int inc) {
	reference -= inc;
	if (reference > 0)
		resetUStringPrimeMap(this, inc, popped, false);
	else {
		resetUStringPrimeMap(this, inc + 1 + reference - popped, popped, true);
		if (popped == 1)
			popped = 0;
		else
			KifElement::Resetreference(0);
	}

}

void KifUStringPrimeMap::Clear() {
	KifContainerLock _lock(this);
	prime_iterator<wstring, KifElement*> itx;
	for (itx = values; !itx.end(); itx++)
		itx.value()->Resetreference(reference + 1);
	values.clear();
}

void KifUStringPrimeMap::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		wstring n = itr->IteratorKeyUString();
		if (values.find(n) == false)
			Push(n, itr->IteratorValue());
	}

	itr->Release();
}

KifElement* KifUStringPrimeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* ke;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			prime_iterator<wstring, KifElement*> it;
			for (it = values; !it.end(); it++) {
				ke = kifcode->Provideustring(it.key());
				vect->Push(ke);
			}

			return vect;
		}


		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifUStringPrimeMap* kmap = new KifUStringPrimeMap(kifcode, NULL);
			prime_iterator<wstring, KifElement*> it;
			for (it = values; !it.end(); it++) {
				ke = it.value()->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
				if (ke == kifRAISEERROR) {
					kmap->Release();
					return kifRAISEERROR;
				}

				kmap->push(it.key(), ke);
			}

			return kmap;
		}

		return this;
	}


	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifUStringPrimeMap* kmap = new KifUStringPrimeMap(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			wstring vleft = key->UString();
			wstring vright = keyright->UString();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<wstring, KifElement*> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->push(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}

		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
	if (key == kifNULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}


	string skey = key->String();
	key->Release();
	KifElement* kval = Values(skey);
	if (kval == kifNOELEMENT) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kval;
}

void KifUStringPrimeMap::Pop(KifElement* ke) {
	wstring s = ke->UString();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);
		primehash_value<wstring, KifElement*>* cell = values.search(s);
		if (cell == NULL) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(kifErrorStrings[223]);
			return;
		}


		kres = cell->value;
		values.erase(s);
	}

	kres->Resetreference(reference + 1);
}

Exported void KifUStringPrimeMap::push(wstring key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;


	KifContainerLock _lock(this);
	primehash_value<wstring, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifUStringPrimeMap::Push(wstring& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	primehash_value<wstring, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifUStringPrimeMap::push(string skey, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	wstring key;
	s_utf8_to_unicode(key, STR(skey));
	KifContainerLock _lock(this);
	primehash_value<wstring, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifUStringPrimeMap::Push(string& skey, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	wstring key;
	s_utf8_to_unicode(key, STR(skey));

	KifContainerLock _lock(this);
	primehash_value<wstring, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifUStringPrimeMap::Push(char* skey, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key;
	s_utf8_to_unicode(key, skey);

	KifContainerLock _lock(this);
	primehash_value<wstring, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifUStringPrimeMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key = k->UString();

	KifContainerLock _lock(this);
	primehash_value<wstring, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		if (cell->value == val)
			return;
		val = val->Atom();
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

void KifUStringPrimeMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<wstring, KifElement*> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseUString(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}

KifElement* KifIteratorUStringPrimeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (it.end())
		return kifNULL;
	return it.value()->Exec(contextualpattern, dom, val, idthread, callfunc);
}

//------------------------------------------------------------------------------------
void KifPrimeMapUStringUString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapUStringUString* kmap = (KifPrimeMapUStringUString*)ke;
		prime_iterator<wstring, wstring> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		wstring n = itr->IteratorKeyUString();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueUString();
	}

	itr->Release();
}

KifElement* KifPrimeMapUStringUString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	prime_iterator<wstring, wstring> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapUStringUString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	prime_iterator<wstring, wstring> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}

Exported void KifPrimeMapUStringUString::Push(wstring& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->UString();
}


Exported void KifPrimeMapUStringUString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring k;
	s_utf8_to_unicode(k, STR(key));
	KifContainerLock _lock(this);
	values[k] = val->UString();
}

Exported void KifPrimeMapUStringUString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring k;
	s_utf8_to_unicode(k, key);
	KifContainerLock _lock(this);
	values[k] = val->UString();
}

Exported void KifPrimeMapUStringUString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key = k->UString();
	KifContainerLock _lock(this);
	values[key] = val->UString();
}

wstring KifPrimeMapUStringUString::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	prime_iterator<wstring, wstring> it;
	res << L"{";
	bool beg = true;
	wstring sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		sx = it.key();
		ustringing(res, sx);
		res << L":";
		sx = it.value();
		ustringing(res, sx);
	}

	res << L"}";

	return res.str();
}


string KifPrimeMapUStringUString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<wstring, wstring> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it.key();
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
		res << ":";
		ws = it.value();
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
	}

	res << "}";

	return res.str();
}

string KifPrimeMapUStringUString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<wstring, wstring> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it.key();
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
		res << ":";
		ws = it.value();
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
	}

	res << "}";

	return res.str();
}

KifElement* KifPrimeMapUStringUString::Newiterator(bool rev) {
	KifIteratorPrimeUStringUString* iter = new KifIteratorPrimeUStringUString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapUStringUString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	wstring s = ke->UString();
	values.erase(s);
}

bool KifPrimeMapUStringUString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifPrimeMapUStringUString) {
			KifPrimeMapUStringUString* kvect = (KifPrimeMapUStringUString*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}


		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyUString()] = itr->IteratorValueUString();
			itr->Release();
			return true;
		}


		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}

		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		wstring k;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			sc_utf8_to_unicode(k, STR(it->first));
			values[k] = it->second->UString();
		}
		kmap->Release();
		return true;
	}

	values[idx->UString()] = ke->UString();
	return true;
}

KifElement* KifPrimeMapUStringUString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}


		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}

				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}

					else {
						itr->Release();
						return kifTRUE;
					}

				}

			}

		}


		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}



	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyUString()] = res->UString();
				res->Release();
			}

		}

		itr->Release();
		return this;
	}


	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapUStringUString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<wstring, wstring> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Provideustring(it.key()));
			return kv;
		}


		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}


	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapUStringUString* kmap = new KifPrimeMapUStringUString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			wstring vleft = key->UString();
			wstring vright = keyright->UString();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<wstring, wstring> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}

		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNOELEMENT;
	wstring skey = key->UString();
	key->Release();
	primehash_value<wstring, wstring>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Provideustring(cell->value);
}

Exported KifElement* KifPrimeMapUStringUString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<wstring, wstring> it;
	string s;
	wstring ws;
	for (it = values; !it.end(); it++) {
		ws = it.key();
		sc_unicode_to_utf8(s, ws);
		kmap->push(s, kifcode->Provideustring(it.value()));
	}
	return kmap;
}

KifElement* KifPrimeMapUStringUString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<wstring, wstring> it;
	for (it = values; !it.end(); it++) {
		kvect->Push(kifcode->Provideustring(it.key()));
	}

	return kvect;
}

Exported KifElement* KifPrimeMapUStringUString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapUStringUString* m = (KifPrimeMapUStringUString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<wstring, wstring> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifPrimeMapUStringUString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapUStringUString* ref = (KifPrimeMapUStringUString*)a->Copy(NULL);
		wstring v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyUString();
			if (ref->values.find(v) == false)
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifPrimeMapUStringUString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapUStringUString* ref = new KifPrimeMapUStringUString(kifcode, NULL);
		KifPrimeMapUStringUString* A = (KifPrimeMapUStringUString*)a->Copy(NULL);
		wstring v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		primehash_value<wstring, wstring>* cell;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyUString();
			cell = A->values.search(v);
			if (cell != NULL) {
				if (cell->value == itr->IteratorValueUString())
					ref->values[v] = cell->value;
			}
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifPrimeMapUStringUString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapUStringUString* ref = new KifPrimeMapUStringUString(kifcode, NULL);
		prime_hash<wstring, wstring>& val = ((KifPrimeMapUStringUString*)a)->values;
		wstring v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyUString();
			if (val.find(v) == false)
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();

		prime_iterator<wstring, wstring> it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnKey(it.key()) == kifNOELEMENT)
				ref->values[it.key()] = it.value();
		}

		return ref;
	}

	return kifNULL;
}

KifElement* KifPrimeMapUStringUString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringUString* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringUString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringUString*)a;

	KifContainerLock _lock(this);

	prime_iterator<wstring, wstring> it;
	if (b->type == kifList) {
		KifList* bref = (KifList*)b;
		KifContainerLock _lock(bref);
		list<KifElement*>::iterator itl;
		for (itl = bref->values.begin(), it = ref->values.begin(); itl != bref->values.end(), it != ref->values.end(); it++, itl++)
			it.value(it.value() + (*itl)->UString());
		return ref;
	}



	if (b->aVector()) {
		int j = 0;
		for (it = ref->values; !it.end(); it++) {
			if (j >= b->Size())
				break;
			it.value(it.value() + b->ValueOnIndex(j++)->UString());
		}

		return ref;
	}


	if (b->type == kifPrimeMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMap* bref = (KifPrimeMap*)b;
		primehash_value<string, KifElement*>* cell;
		string s;
		wstring ws;
		for (it = ref->values; !it.end(); it++) {
			ws = it.key();
			sc_unicode_to_utf8(s, ws);
			cell = bref->values.search(s);
			if (cell != NULL)
				it.value(it.value() + cell->value->UString());
		}

		return ref;
	}


	if (b->type == kifPrimeMapUStringUString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMapUStringUString* bref = (KifPrimeMapUStringUString*)b;
		primehash_value<wstring, wstring>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}

		return ref;
	}


	wstring n = b->UString();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + n);
	return ref;
}

KifElement* KifPrimeMapUStringUString::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapUStringUString* ke = new KifPrimeMapUStringUString(kifcode, NULL);
	ke->values = values;
	return ke;
}

void KifPrimeMapUStringUString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<wstring, wstring> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseUString(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}
//------------------------------------------------------------------------------------------------
KifElement* KifPrimeMapUStringInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	prime_iterator<wstring, long> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapUStringInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	prime_iterator<wstring, long> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}

void KifPrimeMapUStringInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapUStringInteger* kmap = (KifPrimeMapUStringInteger*)ke;
		prime_iterator<wstring, long> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		wstring n = itr->IteratorKeyUString();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueInteger();
	}

	itr->Release();
}

Exported void KifPrimeMapUStringInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring s;
	s_utf8_to_unicode(s, STR(key));
	KifContainerLock _lock(this);
	values[s] = val->Integer();
}

Exported void KifPrimeMapUStringInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring s;
	s_utf8_to_unicode(s, key);
	KifContainerLock _lock(this);
	values[s] = val->Integer();
}

Exported void KifPrimeMapUStringInteger::Push(wstring& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}

Exported void KifPrimeMapUStringInteger::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key = k->UString();
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}

wstring KifPrimeMapUStringInteger::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	prime_iterator<wstring, long> it;
	res << L"{";
	bool beg = true;
	wstring sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		sx = it.key();
		ustringing(res, sx);
		res << L":" << it.value();
	}

	res << L"}";

	return res.str();
}

string KifPrimeMapUStringInteger::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<wstring, long> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it.key();
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
		res << ":" << it.value();
	}

	res << "}";

	return res.str();
}

string KifPrimeMapUStringInteger::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<wstring, long> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it.key();
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
		res << ":" << it.value();
	}

	res << "}";

	return res.str();
}

KifElement* KifPrimeMapUStringInteger::Newiterator(bool rev) {
	KifIteratorPrimeMapUStringInteger* iter = new KifIteratorPrimeMapUStringInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapUStringInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	wstring s = ke->UString();
	values.erase(s);
}

bool KifPrimeMapUStringInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifPrimeMapUStringInteger) {
			KifPrimeMapUStringInteger* kvect = (KifPrimeMapUStringInteger*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}


		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyUString()] = itr->IteratorValueInteger();
			itr->Release();
			return true;
		}


		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}

		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		wstring s;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			s_utf8_to_unicode(s, STR(it->first));
			values[s] = it->second->Integer();
		}
		kmap->Release();
		return true;
	}


	values[idx->UString()] = ke->Integer();
	return true;
}

KifElement* KifPrimeMapUStringInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}


		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}

				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}

					else {
						itr->Release();
						return kifTRUE;
					}

				}

			}

		}


		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}



	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyUString()] = res->Integer();
				res->Release();
			}

		}

		itr->Release();
		return this;
	}


	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapUStringInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<wstring, long> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Provideustring(it.key()));
			return kv;
		}


		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}


	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapUStringInteger* kmap = new KifPrimeMapUStringInteger(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			wstring vleft = key->UString();
			wstring vright = keyright->UString();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<wstring, long> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}

		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	wstring skey = key->UString();
	key->Release();
	primehash_value<wstring, long>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Provideinteger(cell->value);
}

Exported KifElement* KifPrimeMapUStringInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<wstring, long> it;
	string s;
	wstring ws;
	for (it = values; !it.end(); it++) {
		ws = it.key();
		s_unicode_to_utf8(s, ws);
		kmap->push(s, (KifElement*)kifcode->Provideinteger(it.value()));
	}
	return kmap;
}

KifElement* KifPrimeMapUStringInteger::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<wstring, long> it;
	for (it = values; !it.end(); it++)
		kvect->Push(kifcode->Provideustring(it.key()));

	return kvect;
}

Exported KifElement* KifPrimeMapUStringInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapUStringInteger* m = (KifPrimeMapUStringInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<wstring, long> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifPrimeMapUStringInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapUStringInteger* ke = new KifPrimeMapUStringInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifPrimeMapUStringInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() | cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() | v);
		}

		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() | v);
	return ref;
}

KifElement* KifPrimeMapUStringInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()&cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value()&v);
		}

		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()&v);
	return ref;
}

KifElement* KifPrimeMapUStringInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() ^ cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() ^ v);
		}

		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() ^ v);
	return ref;
}

KifElement* KifPrimeMapUStringInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() + v);
		}

		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + v);

	return ref;
}

KifElement* KifPrimeMapUStringInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() - cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() - v);
		}

		return ref;
	}



	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() - v);
	return ref;
}

KifElement* KifPrimeMapUStringInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()*cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value()*v);
		}

		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()*v);
	return ref;
}

KifElement* KifPrimeMapUStringInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	long v;
	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}

		}

		return ref;
	}



	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}

		}

		return ref;
	}


	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() / v);
	return ref;
}

KifElement* KifPrimeMapUStringInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	double v, vv;
	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = it.value();
				vv = cell->value;
				it.value(pow(v, vv));
			}

		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			vv = b->ValueOnKeyFloat(it.key(), add);
			if (add) {
				v = it.value();
				it.value(pow(v, vv));
			}

		}

		return ref;
	}



	vv = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		v = it.value();
		it.value(pow(v, vv));
	}

	return ref;
}

KifElement* KifPrimeMapUStringInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() >> cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() >> v);
		}

		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() >> v);
	return ref;
}

KifElement* KifPrimeMapUStringInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() << cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value(it.value() << v);
		}

		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() << v);
	return ref;
}

KifElement* KifPrimeMapUStringInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, long> it;
	long v;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() % v);
			}

		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);

				it.value(it.value() % v);
			}

		}

		return ref;
	}


	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);

	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() % v);
	return ref;
}

void KifPrimeMapUStringInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<wstring, long> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseUString(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}
//------------------------------------------------------------------------------------------------
void KifPrimeMapUStringFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapUStringFloat* kmap = (KifPrimeMapUStringFloat*)ke;
		prime_iterator<wstring, double> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		wstring n = itr->IteratorKeyUString();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueFloat();
	}

	itr->Release();
}

KifElement* KifPrimeMapUStringFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	prime_iterator<wstring, double> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapUStringFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	prime_iterator<wstring, double> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}

Exported void KifPrimeMapUStringFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring k;
	s_utf8_to_unicode(k, STR(key));
	KifContainerLock _lock(this);
	values[k] = val->Float();
}

Exported void KifPrimeMapUStringFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring k;
	s_utf8_to_unicode(k, key);
	KifContainerLock _lock(this);
	values[k] = val->Float();
}

Exported void KifPrimeMapUStringFloat::Push(wstring& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifPrimeMapUStringFloat::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key = k->UString();
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

wstring KifPrimeMapUStringFloat::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	prime_iterator<wstring, double> it;
	res << L"{";
	bool beg = true;
	wstring sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		sx = it.key();
		ustringing(res, sx);
		res << L":" << it.value();
	}

	res << L"}";

	return res.str();
}

string KifPrimeMapUStringFloat::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<wstring, double> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it.key();
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
		res << ":" << it.value();
	}

	res << "}";

	return res.str();
}

string KifPrimeMapUStringFloat::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<wstring, double> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it.key();
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
		res << ":" << it.value();
	}

	res << "}";

	return res.str();
}

KifElement* KifPrimeMapUStringFloat::Newiterator(bool rev) {
	KifIteratorPrimeMapUStringFloat* iter = new KifIteratorPrimeMapUStringFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapUStringFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	wstring s = ke->UString();
	values.erase(s);
}

bool KifPrimeMapUStringFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifPrimeMapUStringFloat) {
			KifPrimeMapUStringFloat* kvect = (KifPrimeMapUStringFloat*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}


		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyUString()] = itr->IteratorValueFloat();
			itr->Release();
			return true;
		}


		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}

		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		wstring s;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			sc_utf8_to_unicode(s, STR(it->first));
			values[s] = it->second->Float();
		}
		kmap->Release();
		return true;
	}

	values[idx->UString()] = ke->Float();
	return true;
}

KifElement* KifPrimeMapUStringFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}


		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}

				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}

					else {
						itr->Release();
						return kifTRUE;
					}

				}

			}

		}


		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}



	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyUString()] = res->Float();
				res->Release();
			}

		}

		itr->Release();
		return this;
	}


	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapUStringFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<wstring, double> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Provideustring(it.key()));
			return kv;
		}


		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}


	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapUStringFloat* kmap = new KifPrimeMapUStringFloat(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			wstring vleft = key->UString();
			wstring vright = keyright->UString();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<wstring, double> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}

		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	wstring skey = key->UString();
	key->Release();
	primehash_value<wstring, double>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Providefloat(cell->value);
}

Exported KifElement* KifPrimeMapUStringFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<wstring, double> it;
	string s;
	wstring ws;
	for (it = values; !it.end(); it++) {
		ws = it.key();
		sc_unicode_to_utf8(s, ws);
		kmap->push(s, (KifElement*)kifcode->Providefloat(it.value()));
	}
	return kmap;
}

KifElement* KifPrimeMapUStringFloat::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<wstring, double> it;
	for (it = values; !it.end(); it++)
		kvect->Push(kifcode->Provideustring(it.key()));

	return kvect;
}

Exported KifElement* KifPrimeMapUStringFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapUStringFloat* m = (KifPrimeMapUStringFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<wstring, double> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifPrimeMapUStringFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, double> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value((long)it.value() | cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value((long)it.value() | v);
		}

		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() | v);
	return ref;
}

KifElement* KifPrimeMapUStringFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, double> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value((long)it.value()&cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value((long)it.value()&v);
		}

		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value()&v);
	return ref;
}

KifElement* KifPrimeMapUStringFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, double> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value((long)it.value() ^ cell->value);
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		wstring ws;
		for (it = ref->values; !it.end(); it++) {
			ws = it.key();
			v = b->ValueOnKeyInteger(ws, add);
			if (add)
				it.value((long)it.value() ^ v);
		}

		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() ^ v);
	return ref;
}

KifElement* KifPrimeMapUStringFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	prime_iterator<wstring, double> it;
	if (b->type == kifPrimeMapUStringFloat) {
		KifPrimeMapUStringFloat* bref = (KifPrimeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}

		return ref;
	}


	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFloat(it.key(), add);
			if (add)
				it.value(it.value() + v);
		}

		return ref;
	}


	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + v);
	return ref;
}

KifElement* KifPrimeMapUStringFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	prime_iterator<wstring, double> it;
	if (b->type == kifPrimeMapUStringFloat) {
		KifPrimeMapUStringFloat* bref = (KifPrimeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() - cell->value);
		}

		return ref;
	}


	double v;
	if (b->aMapContainer()) {
		bool add = true;
		wstring ws;
		for (it = ref->values; !it.end(); it++) {
			ws = it.key();
			v = b->ValueOnKeyFloat(ws, add);
			if (add)
				it.value(it.value() - v);
		}

		return ref;
	}


	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() - v);
	return ref;
}

KifElement* KifPrimeMapUStringFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	prime_iterator<wstring, double> it;
	if (b->type == kifPrimeMapUStringFloat) {
		KifPrimeMapUStringFloat* bref = (KifPrimeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value()*cell->value);
		}

		return ref;
	}


	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFloat(it.key(), add);
			if (add)
				it.value(it.value()*v);
		}

		return ref;
	}


	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value()*v);
	return ref;
}

KifElement* KifPrimeMapUStringFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	double v;
	prime_iterator<wstring, double> it;
	if (b->type == kifPrimeMapUStringFloat) {
		KifPrimeMapUStringFloat* bref = (KifPrimeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}

		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFloat(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value(it.value() / v);
			}

		}

		return ref;
	}


	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() / v);
	return ref;
}

KifElement* KifPrimeMapUStringFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	long v;
	prime_iterator<wstring, double> it;
	if (b->type == kifPrimeMapUStringFloat) {
		KifPrimeMapUStringFloat* bref = (KifPrimeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = cell->value;
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value((long)it.value() % v);
			}

		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyFloat(it.key(), add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it.value((long)it.value() % v);
			}

		}

		return ref;
	}


	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() % v);
	return ref;
}

KifElement* KifPrimeMapUStringFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapUStringFloat* ke = new KifPrimeMapUStringFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifPrimeMapUStringFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	prime_iterator<wstring, double> it;
	KifContainerLock _lock(this);
	double v, vv;

	if (b->type == kifPrimeMapUStringFloat) {
		KifPrimeMapUStringFloat* bref = (KifPrimeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, double>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL) {
				v = it.value();
				vv = cell->value;
				it.value(pow(v, vv));
			}

		}

		return ref;
	}


	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			vv = b->ValueOnKeyFloat(it.key(), add);
			if (add) {
				v = it.value();
				it.value(pow(v, vv));
			}

		}

		return ref;
	}


	vv = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		v = it.value();
		it.value(pow(v, vv));
	}

	return ref;
}

KifElement* KifPrimeMapUStringFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, double> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value((long)it.value() >> cell->value);
		}

		return ref;
	}


	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value((long)it.value() >> v);
		}

		return ref;
	}


	v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() >> v);
	return ref;
}

KifElement* KifPrimeMapUStringFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapUStringFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<wstring, double> it;
	if (b->type == kifPrimeMapUStringInteger) {
		KifPrimeMapUStringInteger* bref = (KifPrimeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		primehash_value<wstring, long>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value((long)it.value() << cell->value);
		}

		return ref;
	}


	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values; !it.end(); it++) {
			v = b->ValueOnKeyInteger(it.key(), add);
			if (add)
				it.value((long)it.value() << v);
		}

		return ref;
	}


	v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value((long)it.value() << v);
	return ref;
}

void KifPrimeMapUStringFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<wstring, double> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseUString(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}

//------------------------------------------------------------------------------------------------
string KifIteratorPrimeMapIntegerUString::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

void KifPrimeMapIntegerUString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapIntegerUString* kmap = (KifPrimeMapIntegerUString*)ke;
		prime_iterator<long, wstring> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueUString();
	}

	itr->Release();
}

KifElement* KifPrimeMapIntegerUString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	prime_iterator<long, wstring> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapIntegerUString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	prime_iterator<long, wstring> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}

Exported void KifPrimeMapIntegerUString::Push(wstring& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = convertinteger(key);
	values[nb] = val->UString();
}


Exported void KifPrimeMapIntegerUString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(STR(key));
	values[nb] = val->UString();
}

Exported void KifPrimeMapIntegerUString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(key);
	values[nb] = val->UString();
}

Exported void KifPrimeMapIntegerUString::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long nb = key->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->UString();
}

wstring KifPrimeMapIntegerUString::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	prime_iterator<long, wstring> it;
	res << L"{";
	bool beg = true;
	wstring sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		res << it.key() << L":";
		sx = it.value();
		ustringing(res, sx);
	}

	res << L"}";

	return res.str();
}

string KifPrimeMapIntegerUString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<long, wstring> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":";
		ws = it.value();
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
	}

	res << "}";

	return res.str();
}

string KifPrimeMapIntegerUString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<long, wstring> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":";
		ws = it.value();
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
	}

	res << "}";

	return res.str();
}

KifElement* KifPrimeMapIntegerUString::Newiterator(bool rev) {
	KifIteratorPrimeMapIntegerUString* iter = new KifIteratorPrimeMapIntegerUString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapIntegerUString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	long s = ke->Integer();
	values.erase(s);
}

bool KifPrimeMapIntegerUString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifPrimeMapIntegerUString) {
			KifPrimeMapIntegerUString* kvect = (KifPrimeMapIntegerUString*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}


		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyInteger()] = itr->IteratorValueUString();
			itr->Release();
			return true;
		}


		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}

		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		long nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = conversionintegerhexa(STR(it->first));
			values[nb] = it->second->UString();
		}

		kmap->Release();
		return true;
	}

	values[idx->Integer()] = ke->UString();
	return true;
}

KifElement* KifPrimeMapIntegerUString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}


		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}

				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}

					else {
						itr->Release();
						return kifTRUE;
					}

				}

			}

		}


		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}



	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyInteger()] = res->UString();
				res->Release();
			}

		}

		itr->Release();
		return this;
	}


	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapIntegerUString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<long, wstring> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Provideinteger(it.key()));
			return kv;
		}


		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}


	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapIntegerUString* kmap = new KifPrimeMapIntegerUString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<long, wstring> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}

		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNOELEMENT;

	long skey = key->Integer();
	key->Release();
	primehash_value<long, wstring>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Provideustring(cell->value);
}

Exported KifElement* KifPrimeMapIntegerUString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<long, wstring> it;
	char buffer[50];
	for (it = values; !it.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it.key());
		kmap->Push(buffer, (KifElement*)kifcode->Provideustring(it.value()));
	}

	return kmap;
}

KifElement* KifPrimeMapIntegerUString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseInteger localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<long, wstring> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}

	return kvect;
}

Exported KifElement* KifPrimeMapIntegerUString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapIntegerUString* m = (KifPrimeMapIntegerUString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<long, wstring> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifPrimeMapIntegerUString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapIntegerUString* ref = (KifPrimeMapIntegerUString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (ref->values.find(v) == false)
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifPrimeMapIntegerUString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapIntegerUString* ref = new KifPrimeMapIntegerUString(kifcode, NULL);
		KifPrimeMapIntegerUString* A = (KifPrimeMapIntegerUString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		primehash_value<long, wstring>* cell;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			cell = A->values.search(v);
			if (cell != NULL) {
				if (cell->value == itr->IteratorValueUString())
					ref->values[v] = cell->value;
			}

		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifPrimeMapIntegerUString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapIntegerUString* ref = new KifPrimeMapIntegerUString(kifcode, NULL);
		prime_hash<long, wstring>& val = ((KifPrimeMapIntegerUString*)a)->values;
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (val.find(v) == false)
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();

		prime_iterator<long, wstring> it;
		char buffer[50];
		for (it = val.begin(); it != val.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it.key());
			if (b->ValueOnKey(buffer) == kifNOELEMENT)
				ref->values[it.key()] = it.value();
		}

		return ref;
	}

	return kifNULL;
}

KifElement* KifPrimeMapIntegerUString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapIntegerUString* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapIntegerUString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapIntegerUString*)a;

	KifContainerLock _lock(this);

	prime_iterator<long, wstring> it;
	if (b->type == kifList) {
		KifList* bref = (KifList*)b;
		KifContainerLock _lock(bref);
		list<KifElement*>::iterator itl;
		for (itl = bref->values.begin(), it = ref->values.begin(); itl != bref->values.end(), it != ref->values.end(); it++, itl++)
			it.value(it.value() + (*itl)->UString());
		return ref;
	}



	if (b->aVector()) {
		int j = 0;
		for (it = ref->values; !it.end(); it++) {
			if (j >= b->Size())
				break;
			it.value(it.value() + b->ValueOnIndex(j++)->UString());
		}

		return ref;
	}


	if (b->type == kifPrimeMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMap* bref = (KifPrimeMap*)b;
		char buffer[50];
		primehash_value<string, KifElement*>* cell;
		for (it = ref->values; !it.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it.key());
			cell = bref->values.search(buffer);
			if (cell != NULL)
				it.value(it.value() + cell->value->UString());
		}

		return ref;
	}


	if (b->type == kifPrimeMapIntegerUString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMapIntegerUString* bref = (KifPrimeMapIntegerUString*)b;
		primehash_value<long, wstring>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}

		return ref;
	}


	wstring n = b->UString();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + n);
	return ref;
}

KifElement* KifPrimeMapIntegerUString::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapIntegerUString* ke = new KifPrimeMapIntegerUString(kifcode, NULL);
	ke->values = values;
	return ke;
}

void KifPrimeMapIntegerUString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<long, wstring> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}

wstring KifIteratorPrimeMapIntegerUString::IteratorKeyUString() {
	KifContainerLock _lock(kbase);
	std::wstringstream res;
	if (it.end())
		return L"";
	res << it.key();
	return res.str();
}

//------------------------------------------------------------------------------------------------
string KifIteratorPrimeMapFloatUString::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

void KifPrimeMapFloatUString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapFloatUString* kmap = (KifPrimeMapFloatUString*)ke;
		prime_iterator<double, wstring> it;
		for (it = kmap->values; !it.end(); it++) {
			if (values.find(it.key()) == false)
				values[it.key()] = it.value();
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		double n = itr->IteratorKeyFloat();
		if (values.find(n) == false)
			values[n] = itr->IteratorValueUString();
	}

	itr->Release();
}

KifElement* KifPrimeMapFloatUString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	prime_iterator<double, wstring> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapFloatUString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<double, wstring> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}

Exported void KifPrimeMapFloatUString::Push(wstring& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(key);
	values[nb] = val->UString();
}

Exported void KifPrimeMapFloatUString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(STR(key));
	values[nb] = val->UString();
}

Exported void KifPrimeMapFloatUString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(key);
	values[nb] = val->UString();
}

Exported void KifPrimeMapFloatUString::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->UString();
}

wstring KifPrimeMapFloatUString::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	prime_iterator<double, wstring> it;
	res << L"{";
	bool beg = true;
	wstring sx;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		res << it.key() << L":";
		sx = it.value();
		ustringing(res, sx);
	}

	res << L"}";

	return res.str();
}

string KifPrimeMapFloatUString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<double, wstring> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":";
		ws = it.value();
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
	}

	res << "}";

	return res.str();
}

string KifPrimeMapFloatUString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	prime_iterator<double, wstring> it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;

	for (it = values; !it.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it.key() << ":";
		ws = it.value();
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
	}

	res << "}";

	return res.str();
}

KifElement* KifPrimeMapFloatUString::Newiterator(bool rev) {
	KifIteratorPrimeMapFloatUString* iter = new KifIteratorPrimeMapFloatUString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapFloatUString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	double s = ke->Float();
	values.erase(s);
}

bool KifPrimeMapFloatUString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifPrimeMapFloatUString) {
			KifPrimeMapFloatUString* kvect = (KifPrimeMapFloatUString*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}


		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyFloat()] = itr->IteratorValueUString();
			itr->Release();
			return true;
		}


		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}

		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		double nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = convertfloat(STR(it->first));
			values[nb] = it->second->UString();
		}

		kmap->Release();
		return true;
	}

	values[idx->Float()] = ke->UString();
	return true;
}

KifElement* KifPrimeMapFloatUString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}


		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}

				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}

					else {
						itr->Release();
						return kifTRUE;
					}

				}

			}

		}


		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}



	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyFloat()] = res->UString();
				res->Release();
			}

		}

		itr->Release();
		return this;
	}


	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifPrimeMapFloatUString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			prime_iterator<double, wstring> it;
			for (it = values; !it.end(); it++)
				kv->Push(kifcode->Providefloat(it.key()));

			return kv;
		}


		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}


	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifPrimeMapFloatUString* kmap = new KifPrimeMapFloatUString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			double vleft = key->Float();
			double vright = keyright->Float();
			if (values.search(vleft) == NULL && key != kifNULL) {
				key->Release();
				return kmap;
			}
			key->Release();
			if (values.search(vright) == NULL && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			bool add = false;
			if (key == kifNULL)
				add = true;
			prime_iterator<double, wstring> it;
			for (it = values; !it.end(); it++) {
				if (!add) {
					if (it.key() == vleft) {
						add = true;
					}
				}
				if (add) {
					kmap->storekey(it.key(), it.value());
					if (it.key() == vright)
						return kmap;
				}
			}

			if (keyright != kifNULL)
				kmap->Clear();
			return kmap;

		}

		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNOELEMENT;
	double skey = key->Float();
	key->Release();
	primehash_value<double, wstring>* cell = values.search(skey);
	if (cell == NULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Provideustring(cell->value);
}

Exported KifElement* KifPrimeMapFloatUString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	prime_iterator<double, wstring> it;
	char buffer[50];
	for (it = values; !it.end(); it++) {
		sprintf_s(buffer, 50, "%g", it.key());
		kmap->Push(buffer, (KifElement*)kifcode->Provideustring(it.value()));
	}

	return kmap;
}

KifElement* KifPrimeMapFloatUString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseFloat localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	prime_iterator<double, wstring> it;
	for (it = values; !it.end(); it++) {
		localstring.value = it.key();
		kvect->Push(&localstring);
	}

	return kvect;
}

Exported KifElement* KifPrimeMapFloatUString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapFloatUString* m = (KifPrimeMapFloatUString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	prime_iterator<double, wstring> it;
	for (it = m->values; !it.end(); it++) {
		if (values.find(it.key()) == false)
			return kifFALSE;
		if (it.value() != values[it.key()])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifPrimeMapFloatUString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapFloatUString* ref = (KifPrimeMapFloatUString*)a->Copy(NULL);
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (ref->values.find(v) == false)
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifPrimeMapFloatUString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapFloatUString* ref = new KifPrimeMapFloatUString(kifcode, NULL);
		KifPrimeMapFloatUString* A = (KifPrimeMapFloatUString*)a->Copy(NULL);
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		primehash_value<double, wstring>* cell;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			cell = A->values.search(v);
			if (cell != NULL) {
				if (cell->value == itr->IteratorValueUString())
					ref->values[v] = cell->value;
			}

		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifPrimeMapFloatUString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapFloatUString* ref = new KifPrimeMapFloatUString(kifcode, NULL);
		prime_hash<double, wstring>& val = ((KifPrimeMapFloatUString*)a)->values;
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (val.find(v) == false)
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();

		prime_iterator<double, wstring> it;
		char buffer[50];
		for (it = val.begin(); it != val.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it.key());
			if (b->ValueOnKey(buffer) == kifNOELEMENT)
				ref->values[it.key()] = it.value();
		}

		return ref;
	}

	return kifNULL;
}

KifElement* KifPrimeMapFloatUString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifPrimeMapFloatUString* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloatUString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloatUString*)a;

	KifContainerLock _lock(this);

	prime_iterator<double, wstring> it;
	if (b->type == kifList) {
		KifList* bref = (KifList*)b;
		KifContainerLock _lock(bref);
		list<KifElement*>::iterator itl;
		for (itl = bref->values.begin(), it = ref->values.begin(); itl != bref->values.end(), it != ref->values.end(); it++, itl++)
			it.value(it.value() + (*itl)->UString());
		return ref;
	}



	if (b->aVector()) {
		int j = 0;
		for (it = ref->values; !it.end(); it++) {
			if (j >= b->Size())
				break;
			it.value(it.value() + b->ValueOnIndex(j++)->UString());
		}

		return ref;
	}


	if (b->type == kifPrimeMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMap* bref = (KifPrimeMap*)b;
		char buffer[50];
		primehash_value<string, KifElement*>* cell;
		for (it = ref->values; !it.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it.key());
			cell = bref->values.search(buffer);
			if (cell != NULL) {
				it.value(it.value() + cell->value->UString());
			}

		}

		return ref;
	}


	if (b->type == kifPrimeMapFloatUString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMapFloatUString* bref = (KifPrimeMapFloatUString*)b;
		primehash_value<double, wstring>* cell;
		for (it = ref->values; !it.end(); it++) {
			cell = bref->values.search(it.key());
			if (cell != NULL)
				it.value(it.value() + cell->value);
		}

		return ref;
	}


	wstring n = b->UString();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it.value(it.value() + n);
	return ref;
}

KifElement* KifPrimeMapFloatUString::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapFloatUString* ke = new KifPrimeMapFloatUString(kifcode, NULL);
	ke->values = values;
	return ke;
}

void KifPrimeMapFloatUString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	prime_iterator<double, wstring> it;
	for (it = values; !it.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it.key());
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}

wstring KifIteratorPrimeMapFloatUString::IteratorKeyUString() {
	KifContainerLock _lock(kbase);
	std::wstringstream res;
	if (it.end())
		return L"";
	res << it.key();
	return res.str();
}

//--------------------------------------------------------------------------------------------------------

string KifIntegerBinMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hash_bin<ushort, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << (short)it->first << ":";
		sx = it->second->StringForDisplay();
		if (!it->second->aString())
			res << sx;
		else
			stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifIntegerBinMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	hash_bin<ushort, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << (short)it->first << ":" << it->second->JSonString();
	}
	res << "}";
	return res.str();
}

string KifIntegerBinMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	hash_bin<ushort, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << (short)it->first << ":" << it->second->BaseString();
	}
	res << "}";
	return res.str();
}

KifElement* KifIntegerBinMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hash_bin<ushort, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue((long)it->first);
	return kvect;
}

KifElement* KifIntegerBinMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	hash_bin<ushort, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->Push(it->second);
	return kvect;
}

KifElement* KifIntegerBinMap::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifIntegerBinMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifIntegerBinMap)
		return kifNULL;

	KifIntegerBinMap* A = (KifIntegerBinMap*)a;
	hash_bin<ushort, KifElement*>::iterator it;

	KifIntegerBinMap* ref = new KifIntegerBinMap(kifcode, NULL);
	KifIntegerBinMap* bref = (KifIntegerBinMap*)b;
	long ky;
	KifContainerLock _lock(bref);

	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		ky = it->first;
		if (!A->values.check(ky)) {
			ref->push(ky, it->second);
			it->second->Setreference();
		}
	}

	KifContainerLock _locka(A);
	for (it = A->values.begin(); it != A->values.end(); it++) {
		ky = it->first;
		if (!bref->values.check(ky)) {
			ref->push(ky, it->second);
			it->second->Setreference();
		}
	}
	return ref;
}

KifElement* KifIntegerBinMap::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifIntegerBinMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifIntegerBinMap)
		return kifNULL;

	KifIntegerBinMap* A = (KifIntegerBinMap*)a;
	hash_bin<ushort, KifElement*>::iterator it;

	KifIntegerBinMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerBinMap*)a->Copy(NULL);

	KifIntegerBinMap* bref = (KifIntegerBinMap*)b;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		if (!A->values.check(it->first)) {
			ref->push(it->first, it->second);
			it->second->Setreference();
		}
	}
	return ref;
}

KifElement* KifIntegerBinMap::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifIntegerBinMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifIntegerBinMap)
		return kifNULL;

	KifIntegerBinMap* A = (KifIntegerBinMap*)a;
	hash_bin<ushort, KifElement*>::iterator it;
	KifElement* e;


	KifIntegerBinMap* ref = new KifIntegerBinMap(kifcode, NULL);
	KifIntegerBinMap* bref = (KifIntegerBinMap*)b;


	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		e = A->values.search(it->first);
		if (e != NULL) {
			if (it->second->same(e) == kifTRUE) {
				ref->push(it->first, it->second);
				e->Setreference();
			}
		}
	}
	return ref;
}

Exported KifElement* KifIntegerBinMap::same(KifElement* a) {

	if (a->type != kifIntegerBinMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifIntegerBinMap* m = (KifIntegerBinMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hash_bin<ushort, KifElement*>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (!values.check(it->first))
			return kifFALSE;
		if (it->second->same(values[it->first]) == kifFALSE)
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifIntegerBinMap::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifIntegerBinMap)
		return a->plus(a, b, context, autoself);

	KifIntegerBinMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerBinMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerBinMap*)a;

	hash_bin<ushort, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnIntegerKey(it->first);
			if (kref != kifNOELEMENT) {
				ke = it->second;
				ke->plus(ke, kref, context, true);
			}
		}
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->plus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerBinMap::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifIntegerBinMap)
		return a->minus(a, b, context, autoself);

	KifIntegerBinMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerBinMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerBinMap*)a;

	hash_bin<ushort, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnIntegerKey(it->first);
			if (kref != kifNOELEMENT) {
				ke = it->second;
				ke->minus(ke, kref, context, true);
			}
		}
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->minus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerBinMap::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifIntegerBinMap)
		return a->multiply(a, b, context, autoself);

	KifIntegerBinMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerBinMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerBinMap*)a;

	hash_bin<ushort, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnIntegerKey(it->first);
			if (kref != kifNOELEMENT) {
				ke = it->second;
				ke->multiply(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->multiply(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerBinMap::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifIntegerBinMap)
		return a->divide(a, b, context, autoself);

	KifIntegerBinMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerBinMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerBinMap*)a;

	hash_bin<ushort, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnIntegerKey(it->first);
			if (kref != kifNOELEMENT) {
				ke = it->second;
				ke->divide(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->divide(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerBinMap::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifIntegerBinMap)
		return a->mod(a, b, context, autoself);

	KifIntegerBinMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerBinMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerBinMap*)a;

	hash_bin<ushort, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnIntegerKey(it->first);
			if (kref != kifNOELEMENT) {
				ke = it->second;
				ke->mod(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->mod(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerBinMap::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifIntegerBinMap)
		return a->shiftright(a, b, context, autoself);

	KifIntegerBinMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerBinMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerBinMap*)a;

	hash_bin<ushort, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnIntegerKey(it->first);
			if (kref != kifNOELEMENT) {
				ke = it->second;
				ke->shiftright(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->shiftright(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerBinMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifIntegerBinMap)
		return a->shiftleft(a, b, context, autoself);

	KifIntegerBinMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerBinMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerBinMap*)a;

	hash_bin<ushort, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnIntegerKey(it->first);
			if (kref != kifNOELEMENT) {
				ke = it->second;
				ke->shiftleft(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->shiftleft(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerBinMap::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifIntegerBinMap)
		return a->power(a, b, context, autoself);

	KifIntegerBinMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerBinMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerBinMap*)a;

	hash_bin<ushort, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnIntegerKey(it->first);
			if (kref != kifNOELEMENT) {
				ke = it->second;
				ke->power(ke, kref, context, true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->power(ke, b, context, true);
	}
	return ref;
}

KifElement* KifIntegerBinMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorIntegerBinMap* iter = new KifIteratorIntegerBinMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifIntegerBinMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->aMapContainer()) {
			ke->Setpopped(1, true);
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				push(itr->IteratorKeyInteger(), itr->IteratorValue());
			itr->Release();
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		if (ke->aVectorContainer()) {
			ke->Setpopped(1, true);
			Clear();
			for (int it = 0; it < ke->Size(); it++)
				push(it, ke->ValueOnIndex(it));
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		if (ke->type == kifList) {
			long n = 0;
			ke->Setpopped(1, true);
			KifList* kvect = (KifList*)ke;
			Clear();
			{
				KifContainerLock _lock(kvect);
				list<KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
					push(n, *it);
					n++;
				}
			}
			if (ke->popped)
				ke->Setpopped(0, true);
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		if (ke != this) {
			long n = 0;
			Clear();
			KifMap* kvect = (KifMap*)ke;
			//We copy all values from ke to this
			{
				KifContainerLock _lock(kvect);
				hmap<string, KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
					n = conversionintegerhexa(STR(it->first));
					push(n, it->second);
				}
			}
		}
		return true;
	}

	push(idx->Integer(), ke);
	return true;
}

KifElement* KifIntegerBinMap::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseInteger localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	hash_bin<ushort, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

KifElement* KifIntegerBinMap::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Map::Copy");
#endif
	KifIntegerBinMap* ke = new KifIntegerBinMap(kifcode, NULL);
	hash_bin<ushort, KifElement*>::iterator itx;
	long n;
	for (itx = values.begin(); itx != values.end(); itx++) {
		n = itx->first;
		if (itx->second != NULL) {
			ke->values[n] = itx->second->Copy(kp, dom);
			ke->values[n]->Setreference();
		}
		else
			ke->values[n] = NULL;
	}
	return ke;
}

KifElement* KifIntegerBinMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}
		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					Setvalue(res, itr->IteratorKey());
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

Exported void KifIntegerBinMap::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror("KIF(689): Circular structure");
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	hash_bin<ushort, KifElement*>::iterator itx;
	vector<KifElement*> doubles;
	{
		KifContainerLock _lock(this);
		for (itx = values.begin(); itx != values.end(); itx++) {
			if (itx->second != NULL)
				doubles.push_back(itx->second);
		}
	}
	for (int itx = 0; itx < doubles.size(); itx++)
		doubles[itx]->Setreference(inc);
	idgarbage = id;
}


string KifIteratorIntegerBinMap::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it == values->end())
		return "";
	res << (short)it->first;
	return res.str();
}

KifElement* KifIteratorIntegerBinMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (it == values->end())
		return kifNULL;
	return it->second->Exec(contextualpattern, dom, val, idthread, callfunc);
}

static void resetMap(KifIntegerBinMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	hash_bin<ushort, KifElement*>& values = kmap->values;
	if (values.size() == 0)
		return;

	hash_bin<ushort, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}

void KifIntegerBinMap::Resetreference(int inc) {
	reference -= inc;
	if (reference > 0)
		resetMap(this, inc, popped, false);
	else {
		resetMap(this, inc + 1 + reference - popped, popped, true);
		if (popped == 1)
			popped = 0;
		else
			KifElement::Resetreference(0);
	}
}

void KifIntegerBinMap::Clear() {
	KifContainerLock _lock(this);
	hash_bin<ushort, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(reference + 1);
	values.clear();
}

void KifIntegerBinMap::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (!values.check(n)) {
			ke = itr->IteratorValue()->Atom();
			values[n] = ke;
			ke->Setreference(reference + 1);
		}
	}
	itr->Release();
}


KifElement* KifIntegerBinMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* ke;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			hash_bin<ushort, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				vect->Push(kifcode->Provideinteger(it->first));
			return vect;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifIntegerBinMap* kmap = new KifIntegerBinMap(kifcode, NULL);
			hash_bin<ushort, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++) {
				ke = it->second->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
				if (ke == kifRAISEERROR) {
					kmap->Release();
					return kifRAISEERROR;
				}
				kmap->push(it->first, ke);
			}
			return kmap;
		}
		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifIntegerBinMap* kmap = new KifIntegerBinMap(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			hash_bin<ushort, KifElement*>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			hash_bin<ushort, KifElement*>::iterator itr = values.find(vright);
			if (itr == values.end() && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			for (; it != values.end(); it++) {
				kmap->push(it->first, it->second);
				if (it == itr)
					return kmap;
			}

			if (itr != values.end())
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
	if (key == kifNULL) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	ushort skey = key->Integer();
	key->Release();
	if (!values.check(skey)) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return values[skey];
}

void KifIntegerBinMap::Pop(KifElement* ke) {
	ushort s = ke->Integer();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);

		kres = values.search(s);
		if (kres != NULL)
			values.erase(s);
		else {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(kifErrorStrings[223]);
			return;
		}
	}

	kres->Resetreference(reference + 1);
}

//To insert a value into a map through the API 
Exported void KifIntegerBinMap::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	long n = conversionintegerhexa(STR(key));
	KifContainerLock _lock(this);
	if (values.check(n)) {
		if (values[n] == val)
			return;
		val = val->Atom();
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifIntegerBinMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long n = conversionintegerhexa(key);
	if (values.check(n)) {
		if (values[n] == val)
			return;
		val = val->Atom();
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifIntegerBinMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	ushort n = k->Integer();
	KifContainerLock _lock(this);
	if (values.check(n)) {
		if (values[n] == val)
			return;
		val = val->Atom();
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}

void KifIntegerBinMap::push(long n, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	if (values.check(n)) {
		if (values[n] == val)
			return;
		val = val->Atom();
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else {
		val = val->Atom();
		values[n] = val;
	}
	val->Setreference(reference + 1);
}

//--------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
string KifIteratorBinMapIntegerInteger::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it == values->end())
		return "";
	res << (short)it->first;
	return res.str();
}

KifElement* KifBinMapIntegerInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hash_bin<ushort, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue((long)it->first);
	return kvect;
}

KifElement* KifBinMapIntegerInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hash_bin<ushort, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


void KifBinMapIntegerInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifBinMapIntegerInteger* kmap = (KifBinMapIntegerInteger*)ke;
		hash_bin<ushort, long>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (!values.check(it->first))
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (!values.check(n))
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifBinMapIntegerInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ushort nb = conversionintegerhexa(key);
	values[nb] = val->Integer();
}

Exported void KifBinMapIntegerInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ushort nb = conversionintegerhexa(STR(key));
	values[nb] = val->Integer();
}

Exported void KifBinMapIntegerInteger::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	ushort nb = k->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->Integer();
}


string KifBinMapIntegerInteger::String() {
	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	stringstream res;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << (short)it->first << ":" << it->second;
	}
	res << "}";
	return res.str();
}

KifElement* KifBinMapIntegerInteger::Newiterator(bool rev) {
	KifIteratorBinMapIntegerInteger* iter = new KifIteratorBinMapIntegerInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifBinMapIntegerInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	ushort s = ke->Integer();
	if (!values.check(s)) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(s);
}

bool KifBinMapIntegerInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifBinMapIntegerInteger) {
			KifBinMapIntegerInteger* kvect = (KifBinMapIntegerInteger*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyInteger()] = itr->IteratorValueInteger();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		long nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = conversionintegerhexa(STR(it->first));
			values[nb] = it->second->Integer();
		}
		kmap->Release();
		return true;
	}
	else {
		ushort key = idx->Integer();
		values[key] = ke->Integer();
	}
	return true;
}

KifElement* KifBinMapIntegerInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyInteger()] = res->Integer();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifBinMapIntegerInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hash_bin<ushort, long>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Provideinteger(it->first));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifBinMapIntegerInteger* kmap = new KifBinMapIntegerInteger(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			hash_bin<ushort, long>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			hash_bin<ushort, long>::iterator itr = values.find(vright);
			if (itr == values.end() && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			for (; it != values.end(); it++) {
				kmap->storekey((long)it->first, it->second);
				if (it == itr)
					return kmap;
			}

			if (itr != values.end())
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	ushort skey = key->Integer();
	key->Release();
	if (!values.check(skey)) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Provideinteger(values[skey]);
}

Exported KifElement* KifBinMapIntegerInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hash_bin<ushort, long>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Provideinteger(it->second));
	}
	return kmap;
}

KifElement* KifBinMapIntegerInteger::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseInteger localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	hash_bin<ushort, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifBinMapIntegerInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifBinMapIntegerInteger* m = (KifBinMapIntegerInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hash_bin<ushort, long>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (!values.check(it->first))
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifBinMapIntegerInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifBinMapIntegerInteger* ke = new KifBinMapIntegerInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifBinMapIntegerInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second |= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second |= v;
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second |= v;
	return ref;
}

KifElement* KifBinMapIntegerInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second &= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second &= v;
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second &= v;
	return ref;
}

KifElement* KifBinMapIntegerInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second ^= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second ^= v;
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second ^= v;
	return ref;
}

KifElement* KifBinMapIntegerInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second += bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second += v;
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second += v;

	return ref;
}

KifElement* KifBinMapIntegerInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second -= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second -= v;
		}
		return ref;
	}


	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second -= v;
	return ref;
}

KifElement* KifBinMapIntegerInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second *= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second *= v;
		}
		return ref;
	}
	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second *= v;
	return ref;
}

KifElement* KifBinMapIntegerInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	long v;
	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first)) {
				v = bref->values[it->first];
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it->second /= v;
			}
		}
		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it->second /= v;
			}
		}
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second /= v;
	return ref;
}

KifElement* KifBinMapIntegerInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second = pow((double)it->second, (double)bref->values[it->first]);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIFloat(it->first, add);
			if (add) {
				it->second = pow((double)it->second, v);
			}
		}
		return ref;
	}


	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second = pow((double)it->second, v);
	return ref;
}

KifElement* KifBinMapIntegerInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second >>= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second >>= v;
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second >>= v;
	return ref;
}

KifElement* KifBinMapIntegerInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second <<= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second <<= v;
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second <<= v;
	return ref;
}

KifElement* KifBinMapIntegerInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, long>::iterator it;
	long v;
	if (b->type == kifBinMapIntegerInteger) {
		KifBinMapIntegerInteger* bref = (KifBinMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first)) {
				v = bref->values[it->first];
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it->second %= v;
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);

				it->second %= v;
			}
		}
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);

	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second %= v;
	return ref;
}


//---------------------------------------------------------------------------------------------
string KifIteratorBinMapIntegerString::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it == values->end())
		return "";
	res << (short)it->first;
	return res.str();
}

void KifBinMapIntegerString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifBinMapIntegerString* kmap = (KifBinMapIntegerString*)ke;
		hash_bin<ushort, string>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (!values.check(it->first))
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (!values.check(n))
			values[n] = itr->IteratorValueString();
	}
	itr->Release();
}

KifElement* KifBinMapIntegerString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hash_bin<ushort, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue((long)it->first);
	return kvect;
}

KifElement* KifBinMapIntegerString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	hash_bin<ushort, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

Exported void KifBinMapIntegerString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ushort nb = conversionintegerhexa(STR(key));
	values[nb] = val->String();
}

Exported void KifBinMapIntegerString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ushort nb = conversionintegerhexa(key);
	values[nb] = val->String();
}

Exported void KifBinMapIntegerString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	ushort nb = k->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->String();
}


string KifBinMapIntegerString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hash_bin<ushort, string>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << (short)it->first << ":";
		stringing(res, it->second);
	}
	res << "}";
	return res.str();
}

string KifBinMapIntegerString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	hash_bin<ushort, string>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << (short)it->first << ":";
		jstringing(res, it->second);
	}
	res << "}";
	return res.str();
}

KifElement* KifBinMapIntegerString::Newiterator(bool rev) {
	KifIteratorBinMapIntegerString* iter = new KifIteratorBinMapIntegerString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifBinMapIntegerString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	ushort s = ke->Integer();
	if (!values.check(s)) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(s);
}

bool KifBinMapIntegerString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifBinMapIntegerString) {
			KifBinMapIntegerString* kvect = (KifBinMapIntegerString*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyInteger()] = itr->IteratorValueString();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		long nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = conversionintegerhexa(STR(it->first));
			values[nb] = it->second->String();
		}
		kmap->Release();
		return true;
	}
	else {
		ushort key = idx->Integer();
		values[key] = ke->String();
	}
	return true;
}

KifElement* KifBinMapIntegerString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyInteger()] = res->String();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifBinMapIntegerString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hash_bin<ushort, string>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Provideinteger(it->first));

			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifBinMapIntegerString* kmap = new KifBinMapIntegerString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			hash_bin<ushort, string>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			hash_bin<ushort, string>::iterator itr = values.find(vright);
			if (itr == values.end() && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			for (; it != values.end(); it++) {
				kmap->storekey(it->first, it->second);
				if (it == itr)
					return kmap;
			}

			if (itr != values.end())
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNOELEMENT;
	ushort skey = key->Integer();
	key->Release();
	if (!values.check(skey)) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providestringraw(values[skey]);
}

Exported KifElement* KifBinMapIntegerString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hash_bin<ushort, string>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Providestring(it->second));
	}
	return kmap;
}

KifElement* KifBinMapIntegerString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseInteger localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	hash_bin<ushort, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifBinMapIntegerString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifBinMapIntegerString* m = (KifBinMapIntegerString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hash_bin<ushort, string>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (!values.check(it->first))
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifBinMapIntegerString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifBinMapIntegerString* ref = (KifBinMapIntegerString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (!ref->values.check(v))
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifBinMapIntegerString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifBinMapIntegerString* ref = new KifBinMapIntegerString(kifcode, NULL);
		KifBinMapIntegerString* A = (KifBinMapIntegerString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (A->values.check(v)) {
				if (A->values[v] == itr->IteratorValueString())
					ref->values[v] = A->values[v];
			}
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifBinMapIntegerString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifBinMapIntegerString* ref = new KifBinMapIntegerString(kifcode, NULL);
		hash_bin<ushort, string>& val = ((KifBinMapIntegerString*)a)->values;
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (!val.check(v))
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();

		hash_bin<ushort, string>::iterator it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnIntegerKey(it->first) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifBinMapIntegerString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerString* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerString*)a;

	KifContainerLock _lock(this);

	hash_bin<ushort, string>::iterator it;
	if (b->type == kifList) {
		KifList* bref = (KifList*)b;
		KifContainerLock _lock(bref);
		list<KifElement*>::iterator itl;
		for (itl = bref->values.begin(), it = ref->values.begin(); itl != bref->values.end(), it != ref->values.end(); it++, itl++)
			it->second += (*itl)->String();
		return ref;
	}


	if (b->aVector()) {
		int j = 0;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (j >= b->Size())
				break;
			it->second += b->ValueOnIndex(j++)->String();
		}
		return ref;
	}

	if (b->type == kifMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifMap* bref = (KifMap*)b;
		char buffer[50];
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it->first);
			if (bref->values.find(buffer) != bref->values.end()) {
				it->second += bref->values[buffer]->String();
			}
		}
		return ref;
	}

	if (b->type == kifBinMapIntegerString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifBinMapIntegerString* bref = (KifBinMapIntegerString*)b;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second += bref->values[it->first];
		}
		return ref;
	}

	string n = b->String();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second += n;
	return ref;
}


KifElement* KifBinMapIntegerString::Copy(KifDomain* kp, KifElement* dom) {
	KifBinMapIntegerString* ke = new KifBinMapIntegerString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//---------------------------------------------------------------------------------------------
string KifIteratorBinMapIntegerFloat::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it == values->end())
		return "";
	res << (short)it->first;
	return res.str();
}

KifElement* KifBinMapIntegerFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hash_bin<ushort, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue((long)it->first);
	return kvect;
}

KifElement* KifBinMapIntegerFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	hash_bin<ushort, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


void KifBinMapIntegerFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifBinMapIntegerFloat* kmap = (KifBinMapIntegerFloat*)ke;
		hash_bin<ushort, double>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (!values.check(it->first))
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (!values.check(n))
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifBinMapIntegerFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ushort nb = conversionintegerhexa(key);
	values[nb] = val->Float();
}

Exported void KifBinMapIntegerFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ushort nb = conversionintegerhexa(STR(key));
	values[nb] = val->Float();
}

Exported void KifBinMapIntegerFloat::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	ushort nb = k->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->Float();
}


string KifBinMapIntegerFloat::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hash_bin<ushort, double>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << (short)it->first << ":" << it->second;
	}
	res << "}";
	return res.str();
}

KifElement* KifBinMapIntegerFloat::Newiterator(bool rev) {
	KifIteratorBinMapIntegerFloat* iter = new KifIteratorBinMapIntegerFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifBinMapIntegerFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	ushort s = ke->Integer();
	if (!values.check(s)) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(s);
}

bool KifBinMapIntegerFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifBinMapIntegerFloat) {
			KifBinMapIntegerFloat* kvect = (KifBinMapIntegerFloat*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyInteger()] = itr->IteratorValueFloat();
			itr->Release();
			return true;
		}

		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}
		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		long nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = conversionintegerhexa(STR(it->first));
			values[nb] = it->second->Float();
		}
		kmap->Release();
		return true;
	}
	else {
		ushort key = idx->Integer();
		values[key] = ke->Float();
	}
	return true;
}

KifElement* KifBinMapIntegerFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}
					else {
						itr->Release();
						return kifTRUE;
					}
				}
			}
		}

		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyInteger()] = res->Float();
				res->Release();
			}
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifBinMapIntegerFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hash_bin<ushort, double>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Provideinteger(it->first));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}

	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifBinMapIntegerFloat* kmap = new KifBinMapIntegerFloat(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			hash_bin<ushort, double>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			hash_bin<ushort, double>::iterator itr = values.find(vright);
			if (itr == values.end() && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			for (; it != values.end(); it++) {
				kmap->storekey((long)it->first, it->second);
				if (it == itr)
					return kmap;
			}

			if (itr != values.end())
				kmap->Clear();
			return kmap;

		}
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNULL;
	ushort skey = key->Integer();
	key->Release();
	if (!values.check(skey)) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providefloat(values[skey]);
}

Exported KifElement* KifBinMapIntegerFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hash_bin<ushort, double>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Providefloat(it->second));
	}
	return kmap;
}

KifElement* KifBinMapIntegerFloat::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseInteger localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	hash_bin<ushort, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifBinMapIntegerFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifBinMapIntegerFloat* m = (KifBinMapIntegerFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hash_bin<ushort, double>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (!values.check(it->first))
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifBinMapIntegerFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, double>::iterator it;
	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second = (long)it->second | v;
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second = (long)it->second | v;
	return ref;
}

KifElement* KifBinMapIntegerFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, double>::iterator it;

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second = (long)it->second&v;
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second = (long)it->second&v;
	return ref;
}

KifElement* KifBinMapIntegerFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, double>::iterator it;

	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second = (long)it->second^v;
		}
		return ref;
	}

	long v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second = (long)it->second^v;
	return ref;
}

KifElement* KifBinMapIntegerFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	hash_bin<ushort, double>::iterator it;
	if (b->type == kifBinMapIntegerFloat) {
		KifBinMapIntegerFloat* bref = (KifBinMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second += bref->values[it->first];
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIFloat(it->first, add);
			if (add)
				it->second += v;
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second += v;
	return ref;
}

KifElement* KifBinMapIntegerFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	hash_bin<ushort, double>::iterator it;
	if (b->type == kifBinMapIntegerFloat) {
		KifBinMapIntegerFloat* bref = (KifBinMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second -= bref->values[it->first];
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIFloat(it->first, add);
			if (add)
				it->second -= v;
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second -= v;
	return ref;
}

KifElement* KifBinMapIntegerFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	hash_bin<ushort, double>::iterator it;
	if (b->type == kifBinMapIntegerFloat) {
		KifBinMapIntegerFloat* bref = (KifBinMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second *= bref->values[it->first];
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIFloat(it->first, add);
			if (add)
				it->second *= v;
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second *= v;
	return ref;
}

KifElement* KifBinMapIntegerFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	double v;
	hash_bin<ushort, double>::iterator it;
	if (b->type == kifBinMapIntegerFloat) {
		KifBinMapIntegerFloat* bref = (KifBinMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first)) {
				v = bref->values[it->first];
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it->second /= v;
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIFloat(it->first, add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it->second /= v;
			}
		}
		return ref;
	}

	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second /= v;
	return ref;
}

KifElement* KifBinMapIntegerFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	long v;
	hash_bin<ushort, double>::iterator it;
	if (b->type == kifBinMapIntegerFloat) {
		KifBinMapIntegerFloat* bref = (KifBinMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first)) {
				v = bref->values[it->first];
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it->second = (long)it->second%v;
			}
		}
		return ref;
	}

	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add) {
				if (v == 0)
					return kifcode->Returnerror(kifErrorStrings[225]);
				it->second = (long)it->second%v;
			}
		}
		return ref;
	}

	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(kifErrorStrings[225]);
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second = (long)it->second%v;
	return ref;
}

KifElement* KifBinMapIntegerFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifBinMapIntegerFloat* ke = new KifBinMapIntegerFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifBinMapIntegerFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	hash_bin<ushort, double>::iterator it;
	KifContainerLock _lock(this);

	if (b->type == kifBinMapIntegerFloat) {
		KifBinMapIntegerFloat* bref = (KifBinMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second = pow(it->second, bref->values[it->first]);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIFloat(it->first, add);
			if (add)
				it->second = pow(it->second, v);
		}
		return ref;
	}

	v = b->Float();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second = pow((double)it->second, v);
	return ref;
}

KifElement* KifBinMapIntegerFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, double>::iterator it;

	long v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second = (long)it->second >> v;
		}
		return ref;
	}

	v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second = (long)it->second >> v;
	return ref;
}

KifElement* KifBinMapIntegerFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	hash_bin<ushort, double>::iterator it;

	long v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyIInteger(it->first, add);
			if (add)
				it->second = (long)it->second << v;
		}
		return ref;
	}

	v = b->Integer();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second = (long)it->second << v;
	return ref;
}

//-------------------------------------------------------------------------------------------------------------
string KifIteratorBinMapIntegerUString::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it == values->end())
		return "";
	res << (short)it->first;
	return res.str();
}

void KifBinMapIntegerUString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifBinMapIntegerUString* kmap = (KifBinMapIntegerUString*)ke;
		hash_bin<ushort, wstring>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (!values.check(it->first))
				values[it->first] = it->second;
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (!values.check(n))
			values[n] = itr->IteratorValueUString();
	}

	itr->Release();
}

KifElement* KifBinMapIntegerUString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hash_bin<ushort, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue((long)it->first);
	return kvect;
}

KifElement* KifBinMapIntegerUString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	hash_bin<ushort, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

Exported void KifBinMapIntegerUString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ushort nb = conversionintegerhexa(STR(key));
	values[nb] = val->UString();
}

Exported void KifBinMapIntegerUString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ushort nb = conversionintegerhexa(key);
	values[nb] = val->UString();
}

Exported void KifBinMapIntegerUString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	ushort nb = k->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->UString();
}

wstring KifBinMapIntegerUString::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	hash_bin<ushort, wstring>::iterator it;
	res << L"{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		res << (short)it->first << L":";
		ustringing(res, it->second);
	}

	res << L"}";

	return res.str();
}


string KifBinMapIntegerUString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hash_bin<ushort, wstring>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << (short)it->first << ":";
		ws = it->second;
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
	}

	res << "}";

	return res.str();
}

string KifBinMapIntegerUString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	hash_bin<ushort, wstring>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << (short)it->first << ":";
		ws = it->second;
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
	}

	res << "}";

	return res.str();
}

KifElement* KifBinMapIntegerUString::Newiterator(bool rev) {
	KifIteratorBinMapIntegerUString* iter = new KifIteratorBinMapIntegerUString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifBinMapIntegerUString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	ushort s = ke->Integer();
	if (!values.check(s)) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(s);
}

bool KifBinMapIntegerUString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifBinMapIntegerUString) {
			KifBinMapIntegerUString* kvect = (KifBinMapIntegerUString*)ke;
			//We copy all values from ke to this
			Clear();
			values = kvect->values;
			return true;
		}


		if (ke->aContainer()) {
			Clear();
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values[itr->IteratorKeyInteger()] = itr->IteratorValueUString();
			itr->Release();
			return true;
		}


		ke = ke->Map(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(kifErrorStrings[207]);
			return false;
		}

		values.clear();
		KifMap* kmap = (KifMap*)ke;
		//We copy all values from ke to this
		hmap<string, KifElement*>::iterator it;
		long nb;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			nb = conversionintegerhexa(STR(it->first));
			values[nb] = it->second->UString();
		}

		kmap->Release();
		return true;
	}

	else {
		ushort key = idx->Integer();
		values[key] = ke->UString();
	}

	return true;
}

KifElement* KifBinMapIntegerUString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* k;
	KifElement* res;
	KifElement* found = kifFALSE;


	if (b->aMapContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		if (b != this)
			Clear();

		if (a->aMapContainer()) {
			//They must have the same keys and values...
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}


		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this) {
					Setvalue(res, itr->IteratorKey());
					found = this;
				}
				else {
					res->Release();
					if (idx == true) {
						k = itr->IteratorKey();
						itr->Release();
						return k;
					}

					else {
						itr->Release();
						return kifTRUE;
					}

				}

			}

		}


		itr->Release();
		if (b == this && idx == true)
			return kifNOELEMENT;
		return found;
	}



	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorValue();
			if (a->aContainer() && !k->aContainer() && k->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = k->in(a, k, NULL, this, idx);
			else
				res = applyfunc(a, k, kfunc);
			k->Release();
			if (res->Succeed()) {
				if (b != this)
					values[itr->IteratorKeyInteger()] = res->UString();
				res->Release();
			}

		}

		itr->Release();
		return this;
	}


	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);

	if (idx == true)
		return kifNOELEMENT;

	return kifFALSE;
}

KifElement* KifBinMapIntegerUString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hash_bin<ushort, wstring>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Provideinteger(it->first));

			return kv;
		}


		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		return this;
	}


	KifElement* key;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;

		if (kind->interval == true) {
			KifBinMapIntegerUString* kmap = new KifBinMapIntegerUString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			hash_bin<ushort, wstring>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			hash_bin<ushort, wstring>::iterator itr = values.find(vright);
			if (itr == values.end() && keyright != kifNULL) {
				keyright->Release();
				return kmap;
			}
			keyright->Release();
			for (; it != values.end(); it++) {
				kmap->storekey(it->first, it->second);
				if (it == itr)
					return kmap;
			}

			if (itr != values.end())
				kmap->Clear();
			return kmap;

		}

		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (key == kifNULL)
		return kifNOELEMENT;
	ushort skey = key->Integer();
	key->Release();
	if (!values.check(skey)) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Provideustring(values[skey]);
}

Exported KifElement* KifBinMapIntegerUString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hash_bin<ushort, wstring>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, kifcode->Provideustring(it->second));
	}

	return kmap;
}

KifElement* KifBinMapIntegerUString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifBaseInteger localstring(NULL, NULL);
	localstring.kifcode = kifcode;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	hash_bin<ushort, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}

	return kvect;
}

Exported KifElement* KifBinMapIntegerUString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifBinMapIntegerUString* m = (KifBinMapIntegerUString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hash_bin<ushort, wstring>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (!values.check(it->first))
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifBinMapIntegerUString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifBinMapIntegerUString* ref = (KifBinMapIntegerUString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (!ref->values.check(v))
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifBinMapIntegerUString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifBinMapIntegerUString* ref = new KifBinMapIntegerUString(kifcode, NULL);
		KifBinMapIntegerUString* A = (KifBinMapIntegerUString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (A->values.check(v)) {
				if (A->values[v] == itr->IteratorValueUString())
					ref->values[v] = A->values[v];
			}
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifBinMapIntegerUString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifBinMapIntegerUString* ref = new KifBinMapIntegerUString(kifcode, NULL);
		hash_bin<ushort, wstring>& val = ((KifBinMapIntegerUString*)a)->values;
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (!val.check(v))
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();

		hash_bin<ushort, wstring>::iterator it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnIntegerKey(it->first) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}

		return ref;
	}

	return kifNULL;
}

KifElement* KifBinMapIntegerUString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifBinMapIntegerUString* ref = this;
	if (autoself == false)
		ref = (KifBinMapIntegerUString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifBinMapIntegerUString*)a;

	KifContainerLock _lock(this);

	hash_bin<ushort, wstring>::iterator it;
	if (b->type == kifList) {
		KifList* bref = (KifList*)b;
		KifContainerLock _lock(bref);
		list<KifElement*>::iterator itl;
		for (itl = bref->values.begin(), it = ref->values.begin(); itl != bref->values.end(), it != ref->values.end(); it++, itl++)
			it->second += (*itl)->UString();
		return ref;
	}



	if (b->aVector()) {
		int j = 0;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (j >= b->Size())
				break;
			it->second += b->ValueOnIndex(j++)->UString();
		}

		return ref;
	}


	if (b->type == kifMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifMap* bref = (KifMap*)b;
		char buffer[50];
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it->first);
			if (bref->values.find(buffer) != bref->values.end()) {
				it->second += bref->values[buffer]->UString();
			}

		}

		return ref;
	}


	if (b->type == kifBinMapIntegerUString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifBinMapIntegerUString* bref = (KifBinMapIntegerUString*)b;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.check(it->first))
				it->second += bref->values[it->first];
		}

		return ref;
	}


	wstring n = b->UString();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second += n;
	return ref;
}

KifElement* KifBinMapIntegerUString::Copy(KifDomain* kp, KifElement* dom) {
	KifBinMapIntegerUString* ke = new KifBinMapIntegerUString(kifcode, NULL);
	ke->values = values;
	return ke;
}






