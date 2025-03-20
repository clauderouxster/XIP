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
#include "kifinit.h"
#include "kifcontainer.h"
#include "fractalhmap.h"
#include "kifpmaps.h"


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


KifElement* KifPrimeMap::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifPrimeMap::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifPrimeMap::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifPrimeMap::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifPrimeMap)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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
				ke->plus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->plus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifPrimeMap::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifPrimeMap)
		return a->minus(a, b, context, idthread,callfunc,autoself);

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
				ke->minus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->minus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifPrimeMap::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifPrimeMap)
		return a->multiply(a, b, context, idthread,callfunc,autoself);

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
				ke->multiply(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->multiply(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifPrimeMap::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifPrimeMap)
		return a->divide(a, b, context, idthread,callfunc,autoself);

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
				ke->divide(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->divide(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifPrimeMap::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifPrimeMap)
		return a->mod(a, b, context, idthread,callfunc,autoself);

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
				ke->mod(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->mod(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifPrimeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifPrimeMap)
		return a->shiftright(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftright(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftright(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifPrimeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifPrimeMap)
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftleft(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftleft(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifPrimeMap::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifPrimeMap)
		return a->power(a, b, context, idthread,callfunc,autoself);

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
				ke->power(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->power(ke, b, context, idthread,callfunc,true);
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

bool KifPrimeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifPrimeMap::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Map::Declaration");
#endif
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifPrimeMap::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifPrimeMap::Methods(KifVectorString* fcts) {
#ifdef PROFILING
	profilingcall("Map::List");
#endif
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
}

KifElement* KifPrimeMap::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Map::Copy");
#endif
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

Exported void KifPrimeMap::Setreference(int inc) {
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
}


KifElement* KifIteratorPrimeMap::Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val) {
	if (it.end())
		return kifNULL;
	return it.value()->Execute(contextualpattern, dom, val);
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifPrimeMap* kmap = new KifPrimeMap(kifcode, NULL);
			prime_iterator<string, KifElement*> it;
			for (it = values; !it.end(); it++)
				kmap->push(it.key(), it.value());
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
	val = val->Atom();
	val->Setreference(reference + 1);

	KifContainerLock _lock(this);
	primehash_value<string, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[key] = val;
}



//To insert a value into a map through the API 
Exported void KifPrimeMap::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	val->Setreference(reference + 1);

	KifContainerLock _lock(this);
	primehash_value<string, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[key] = val;
}

Exported void KifPrimeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	KifContainerLock _lock(this);
	primehash_value<string, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[key] = val;
	val->Setreference(reference + 1);
}

Exported void KifPrimeMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	string key = k->String();

	KifContainerLock _lock(this);
	primehash_value<string, KifElement*>* cell = values.search(key);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[key] = val;
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


KifElement* KifIntegerPrimeMap::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifIntegerPrimeMap::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifIntegerPrimeMap::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifIntegerPrimeMap::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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
				ke->plus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->plus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->minus(a, b, context, idthread,callfunc,autoself);

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
				ke->minus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->minus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->multiply(a, b, context, idthread,callfunc,autoself);

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
				ke->multiply(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->multiply(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->divide(a, b, context, idthread,callfunc,autoself);

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
				ke->divide(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->divide(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->mod(a, b, context, idthread,callfunc,autoself);

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
				ke->mod(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->mod(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->shiftright(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftright(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftright(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftleft(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftleft(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerPrimeMap::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifIntegerPrimeMap)
		return a->power(a, b, context, idthread,callfunc,autoself);

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
				ke->power(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->power(ke, b, context, idthread,callfunc,true);
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

bool KifIntegerPrimeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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
					n = s_XLConvert(STR(it->first));
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

KifElement* KifIntegerPrimeMap::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Map::Declaration");
#endif
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifIntegerPrimeMap::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifIntegerPrimeMap::Methods(KifVectorString* fcts) {
#ifdef PROFILING
	profilingcall("Map::List");
#endif
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
}

KifElement* KifIntegerPrimeMap::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Map::Copy");
#endif
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

Exported void KifIntegerPrimeMap::Setreference(int inc) {
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
}


string KifIteratorIntegerPrimeMap::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

KifElement* KifIteratorIntegerPrimeMap::Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val) {
	if (it.end())
		return kifNULL;
	return it.value()->Execute(contextualpattern, dom, val);
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
			ke->Setreference(reference + 1);
			values[n] = ke;
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifIntegerPrimeMap* kmap = new KifIntegerPrimeMap(kifcode, NULL);
			prime_iterator<long, KifElement*> it;
			for (it = values; !it.end(); it++)
				kmap->push(it.key(), it.value());
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
	val = val->Atom();
	val->Setreference(reference + 1);

	long n = s_XLConvert(STR(key));
	KifContainerLock _lock(this);
	primehash_value<long, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[n] = val;
}

Exported void KifIntegerPrimeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	KifContainerLock _lock(this);
	long n = s_XLConvert(key);
	primehash_value<long, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[n] = val;
	val->Setreference(reference + 1);
}

Exported void KifIntegerPrimeMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	long n = k->Integer();
	KifContainerLock _lock(this);
	primehash_value<long, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[n] = val;
	val->Setreference(reference + 1);
}


void KifIntegerPrimeMap::push(long n, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	val->Setreference(reference + 1);

	KifContainerLock _lock(this);
	primehash_value<long, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[n] = val;
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


KifElement* KifFloatPrimeMap::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifFloatPrimeMap::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifFloatPrimeMap::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifFloatPrimeMap::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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
				ke->plus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->plus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->minus(a, b, context, idthread,callfunc,autoself);

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
				ke->minus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->minus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->multiply(a, b, context, idthread,callfunc,autoself);

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
				ke->multiply(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->multiply(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->divide(a, b, context, idthread,callfunc,autoself);

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
				ke->divide(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->divide(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->mod(a, b, context, idthread,callfunc,autoself);

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
				ke->mod(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->mod(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->shiftright(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftright(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftright(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftleft(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->shiftleft(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatPrimeMap::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifFloatPrimeMap)
		return a->power(a, b, context, idthread,callfunc,autoself);

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
				ke->power(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values; !it.end(); it++) {
		ke = it.value();
		ke->power(ke, b, context, idthread,callfunc,true);
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

bool KifFloatPrimeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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
					n = s_XLConvert(STR(it->first));
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

KifElement* KifFloatPrimeMap::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Map::Declaration");
#endif
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifFloatPrimeMap::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifFloatPrimeMap::Methods(KifVectorString* fcts) {
#ifdef PROFILING
	profilingcall("Map::List");
#endif
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
}

KifElement* KifFloatPrimeMap::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Map::Copy");
#endif
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

Exported void KifFloatPrimeMap::Setreference(int inc) {
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
}


string KifIteratorFloatPrimeMap::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (it.end())
		return "";
	res << it.key();
	return res.str();
}

KifElement* KifIteratorFloatPrimeMap::Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val) {
	if (it.end())
		return kifNULL;
	return it.value()->Execute(contextualpattern, dom, val);
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
			ke->Setreference(reference + 1);
			values[n] = ke;
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifFloatPrimeMap* kmap = new KifFloatPrimeMap(kifcode, NULL);
			prime_iterator<double, KifElement*> it;
			for (it = values; !it.end(); it++)
				kmap->push(it.key(), it.value());
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
	val = val->Atom();

	double n = convertfloat(STR(key));
	KifContainerLock _lock(this);
	primehash_value<double, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[n] = val;
	val->Setreference(reference + 1);
}

Exported void KifFloatPrimeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	KifContainerLock _lock(this);
	double n = convertfloat(key);
	primehash_value<double, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[n] = val;
	val->Setreference(reference + 1);
}

Exported void KifFloatPrimeMap::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	double n = key->Float();
	KifContainerLock _lock(this);
	primehash_value<double, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[n] = val;
	val->Setreference(reference + 1);
}


void KifFloatPrimeMap::push(double n, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();

	KifContainerLock _lock(this);
	primehash_value<double, KifElement*>* cell = values.search(n);
	if (cell != NULL) {
		cell->value->Resetreference(reference + 1);
		cell->Value(val);
	}
	else
		values[n] = val;
	val->Setreference(reference + 1);
}

//----------------------------------------------------------------------------------
Exported KifElement* KifPrimeMapString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifPrimeMapString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapString* kmap = (KifPrimeMapString*)ke;
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

KifElement* KifPrimeMapString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<string, string> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<string, string> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}


Exported void KifPrimeMapString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->String();
}

Exported void KifPrimeMapString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->String();
}

Exported void KifPrimeMapString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->String();
}

string KifPrimeMapString::String() {
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

string KifPrimeMapString::JSonString() {
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

KifElement* KifPrimeMapString::Newiterator(bool rev) {
	KifIteratorPrimeMapString* iter = new KifIteratorPrimeMapString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	string s = ke->String();
	values.erase(s);
}

bool KifPrimeMapString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapString) {
			KifPrimeMapString* kvect = (KifPrimeMapString*)ke;
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

KifElement* KifPrimeMapString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifPrimeMapString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifPrimeMapString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifPrimeMapString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifPrimeMapString* kmap = new KifPrimeMapString(kifcode, NULL);
			kmap->values = values;
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

Exported KifElement* KifPrimeMapString::Map(KifElement* contextualpattern, KifMap* m) {
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

KifElement* KifPrimeMapString::Vector(KifElement* contextualpattern, KifVector* v) {
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

Exported KifElement* KifPrimeMapString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapString* m = (KifPrimeMapString*)a;
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

KifElement* KifPrimeMapString::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapString* ref = (KifPrimeMapString*)a->Copy(NULL);
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

KifElement* KifPrimeMapString::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapString* ref = new KifPrimeMapString(kifcode, NULL);
		KifPrimeMapString* A = (KifPrimeMapString*)a->Copy(NULL);
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

KifElement* KifPrimeMapString::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifPrimeMapString* ref = new KifPrimeMapString(kifcode, NULL);
		prime_hash<string, string>& val = ((KifPrimeMapString*)a)->values;
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

KifElement* KifPrimeMapString::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapString* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapString*)a;

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

	if (b->type == kifPrimeMapString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifPrimeMapString* bref = (KifPrimeMapString*)b;
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


KifElement* KifPrimeMapString::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapString* ke = new KifPrimeMapString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//----------------------------------------------------------------------------------
Exported KifElement* KifPrimeMapInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

KifElement* KifPrimeMapInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<string, long> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	prime_iterator<string, long> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}

void KifPrimeMapInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapInteger* kmap = (KifPrimeMapInteger*)ke;
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

Exported void KifPrimeMapInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}


Exported void KifPrimeMapInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}

Exported void KifPrimeMapInteger::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}



string KifPrimeMapInteger::String() {
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


string KifPrimeMapInteger::JSonString() {
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


KifElement* KifPrimeMapInteger::Newiterator(bool rev) {
	KifIteratorPrimeMapInteger* iter = new KifIteratorPrimeMapInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}


void KifPrimeMapInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	string s = ke->String();
	values.erase(s);
}

bool KifPrimeMapInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapInteger) {
			KifPrimeMapInteger* kvect = (KifPrimeMapInteger*)ke;
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


KifElement* KifPrimeMapInteger::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifPrimeMapInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifPrimeMapInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifPrimeMapInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifPrimeMapInteger* kmap = new KifPrimeMapInteger(kifcode, NULL);
			kmap->values = values;
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

Exported KifElement* KifPrimeMapInteger::Map(KifElement* contextualpattern, KifMap* m) {
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

KifElement* KifPrimeMapInteger::Vector(KifElement* contextualpattern, KifVector* v) {
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

Exported KifElement* KifPrimeMapInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapInteger* m = (KifPrimeMapInteger*)a;
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

KifElement* KifPrimeMapInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapInteger* ke = new KifPrimeMapInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifPrimeMapInteger::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapInteger::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapInteger::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapInteger::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapInteger::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapInteger::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapInteger::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	long v;
	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapInteger::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	double v, vv;
	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapInteger::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapInteger*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, long> it;
	long v;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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
Exported KifElement* KifPrimeMapFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifPrimeMapFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifPrimeMapFloat* kmap = (KifPrimeMapFloat*)ke;
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

KifElement* KifPrimeMapFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	prime_iterator<string, double> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.key());
	return kvect;
}

KifElement* KifPrimeMapFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	prime_iterator<string, double> it;
	for (it = values; !it.end(); it++)
		kvect->storevalue(it.value());
	return kvect;
}


Exported void KifPrimeMapFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifPrimeMapFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifPrimeMapFloat::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->Float();
}


string KifPrimeMapFloat::String() {
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

string KifPrimeMapFloat::JSonString() {
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

KifElement* KifPrimeMapFloat::Newiterator(bool rev) {
	KifIteratorPrimeMapFloat* iter = new KifIteratorPrimeMapFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifPrimeMapFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	string s = ke->String();
	values.erase(s);
}

bool KifPrimeMapFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifPrimeMapFloat) {
			KifPrimeMapFloat* kvect = (KifPrimeMapFloat*)ke;
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

KifElement* KifPrimeMapFloat::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifPrimeMapFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifPrimeMapFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifPrimeMapFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifPrimeMapFloat* kmap = new KifPrimeMapFloat(kifcode, NULL);
			kmap->values = values;
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

Exported KifElement* KifPrimeMapFloat::Map(KifElement* contextualpattern, KifMap* m) {
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

KifElement* KifPrimeMapFloat::Vector(KifElement* contextualpattern, KifVector* v) {
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

Exported KifElement* KifPrimeMapFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifPrimeMapFloat* m = (KifPrimeMapFloat*)a;
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

KifElement* KifPrimeMapFloat::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapFloat::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapFloat::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapFloat::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapFloat) {
		KifPrimeMapFloat* bref = (KifPrimeMapFloat*)b;
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

KifElement* KifPrimeMapFloat::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapFloat) {
		KifPrimeMapFloat* bref = (KifPrimeMapFloat*)b;
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

KifElement* KifPrimeMapFloat::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapFloat) {
		KifPrimeMapFloat* bref = (KifPrimeMapFloat*)b;
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

KifElement* KifPrimeMapFloat::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	double v;
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapFloat) {
		KifPrimeMapFloat* bref = (KifPrimeMapFloat*)b;
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

KifElement* KifPrimeMapFloat::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	long v;
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapFloat) {
		KifPrimeMapFloat* bref = (KifPrimeMapFloat*)b;
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

KifElement* KifPrimeMapFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifPrimeMapFloat* ke = new KifPrimeMapFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifPrimeMapFloat::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	prime_iterator<string, double> it;
	KifContainerLock _lock(this);
	double v, vv;

	if (b->type == kifPrimeMapFloat) {
		KifPrimeMapFloat* bref = (KifPrimeMapFloat*)b;
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

KifElement* KifPrimeMapFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

KifElement* KifPrimeMapFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifPrimeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifPrimeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifPrimeMapFloat*)a;

	KifContainerLock _lock(this);
	prime_iterator<string, double> it;
	if (b->type == kifPrimeMapInteger) {
		KifPrimeMapInteger* bref = (KifPrimeMapInteger*)b;
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

Exported KifElement* KifPrimeMapIntegerInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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
	long nb = s_XLConvert(key);
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
	long nb = s_XLConvert(STR(key));
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

bool KifPrimeMapIntegerInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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
			nb = s_XLConvert(STR(it->first));
			values[nb] = it->second->Integer();
		}
		kmap->Release();
		return true;
	}

	values[idx->Integer()] = ke->Integer();
	return true;
}

KifElement* KifPrimeMapIntegerInteger::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifPrimeMapIntegerInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifPrimeMapIntegerInteger* kmap = new KifPrimeMapIntegerInteger(kifcode, NULL);
			kmap->values = values;
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

KifElement* KifPrimeMapIntegerInteger::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapIntegerInteger::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapIntegerInteger::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapIntegerInteger::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerInteger::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerInteger::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerInteger::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerInteger::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerInteger::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

Exported KifElement* KifPrimeMapIntegerString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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
	long nb = s_XLConvert(STR(key));
	values[nb] = val->String();
}

Exported void KifPrimeMapIntegerString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = s_XLConvert(key);
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

bool KifPrimeMapIntegerString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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
			nb = s_XLConvert(STR(it->first));
			values[nb] = it->second->String();
		}
		kmap->Release();
		return true;
	}
	values[idx->Integer()] = ke->String();
	return true;
}

KifElement* KifPrimeMapIntegerString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifPrimeMapIntegerString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifPrimeMapIntegerString* kmap = new KifPrimeMapIntegerString(kifcode, NULL);
			kmap->values = values;
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

KifElement* KifPrimeMapIntegerString::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapIntegerString::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapIntegerString::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapIntegerString::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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


Exported KifElement* KifPrimeMapIntegerFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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
	long nb = s_XLConvert(key);
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
	long nb = s_XLConvert(STR(key));
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

bool KifPrimeMapIntegerFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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
			nb = s_XLConvert(STR(it->first));
			values[nb] = it->second->Float();
		}
		kmap->Release();
		return true;
	}
	values[idx->Integer()] = ke->Float();
	return true;
}

KifElement* KifPrimeMapIntegerFloat::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifPrimeMapIntegerFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifPrimeMapIntegerFloat* kmap = new KifPrimeMapIntegerFloat(kifcode, NULL);
			kmap->values = values;
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

KifElement* KifPrimeMapIntegerFloat::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapIntegerFloat::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapIntegerFloat::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapIntegerFloat::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerFloat::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerFloat::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerFloat::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerFloat::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerFloat::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapIntegerFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

Exported KifElement* KifPrimeMapFloatInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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

bool KifPrimeMapFloatInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifPrimeMapFloatInteger::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifPrimeMapFloatInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifPrimeMapFloatInteger* kmap = new KifPrimeMapFloatInteger(kifcode, NULL);
			kmap->values = values;
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

KifElement* KifPrimeMapFloatInteger::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapFloatInteger::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapFloatInteger::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapFloatInteger::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatInteger::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatInteger::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatInteger::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatInteger::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatInteger::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

Exported KifElement* KifPrimeMapFloatString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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

bool KifPrimeMapFloatString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifPrimeMapFloatString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifPrimeMapFloatString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifPrimeMapFloatString* kmap = new KifPrimeMapFloatString(kifcode, NULL);
			kmap->values = values;
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

KifElement* KifPrimeMapFloatString::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapFloatString::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapFloatString::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapFloatString::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

Exported KifElement* KifPrimeMapFloatFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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

bool KifPrimeMapFloatFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifPrimeMapFloatFloat::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifPrimeMapFloatFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifPrimeMapFloatFloat* kmap = new KifPrimeMapFloatFloat(kifcode, NULL);
			kmap->values = values;
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

KifElement* KifPrimeMapFloatFloat::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapFloatFloat::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapFloatFloat::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifPrimeMapFloatFloat::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatFloat::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatFloat::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatFloat::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatFloat::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatFloat::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifPrimeMapFloatFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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
	KifPrimeMapString* kv = new KifPrimeMapString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapInteger* kv = new KifPrimeMapInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPrimeMapFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPrimeMapFloat* kv = new KifPrimeMapFloat(this, var);
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

void KifPrimeMapString::storeInLoop(KifElement** alls) {
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

void KifPrimeMapInteger::storeInLoop(KifElement** alls) {
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

void KifPrimeMapFloat::storeInLoop(KifElement** alls) {
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






