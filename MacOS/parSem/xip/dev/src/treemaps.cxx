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
#include "kiftreemaps.h"


extern const char* kifErrorStrings[];
bool TestkifFunctions(short n, hmap<short, bool>& kiffunc);

KifElement* KifTreeMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<string, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	map<string, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->Push(it->second);
	return kvect;
}

string KifTreeMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<string, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->first;
		stringing(res, sx);
		res << ":";
		sx = it->second->StringForDisplay();
		if (!it->second->aString())
			res << sx;
		else
			stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifTreeMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<string, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->first;
		jstringing(res, sx);
		res << ":" << it->second->JSonString();
	}
	res << "}";
	return res.str();
}


string KifTreeMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<string, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->first;
		res << "'" << Trim(sx) << "'" << ":" << it->second->BaseString();
	}
	res << "}";
	return res.str();
}


KifElement* KifTreeMap::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifTreeMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifTreeMap)
		return kifNULL;

	KifTreeMap* A = (KifTreeMap*)a;
	map<string, KifElement*>::iterator it;

	KifTreeMap* ref = new KifTreeMap(kifcode, NULL);
	KifTreeMap* bref = (KifTreeMap*)b;
	string ky;
	KifContainerLock _lock(bref);

	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		ky = it->first;
		if (A->values.find(ky) == A->values.end()) {
			ref->Push(ky, it->second);
			it->second->Setreference();
		}
	}

	KifContainerLock _locka(A);
	for (it = A->values.begin(); it != A->values.end(); it++) {
		ky = it->first;
		if (bref->values.find(ky) == bref->values.end()) {
			ref->Push(ky, it->second);
			it->second->Setreference();
		}
	}
	return ref;
}

KifElement* KifTreeMap::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifTreeMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifTreeMap)
		return kifNULL;

	KifTreeMap* A = (KifTreeMap*)a;
	map<string, KifElement*>::iterator it;

	KifTreeMap* ref = this;
	if (autoself == false)
		ref = (KifTreeMap*)a->Copy(NULL);

	KifTreeMap* bref = (KifTreeMap*)b;
	string ky;
	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		if (A->values.find(it->first) == A->values.end()) {
			ky = it->first;
			ref->Push(ky, it->second);
			it->second->Setreference();
		}
	}
	return ref;
}

KifElement* KifTreeMap::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifTreeMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifTreeMap)
		return kifNULL;

	KifTreeMap* A = (KifTreeMap*)a;
	map<string, KifElement*>::iterator it;
	map<string, KifElement*>::iterator itres;

	KifTreeMap* ref = new KifTreeMap(kifcode, NULL);
	KifTreeMap* bref = (KifTreeMap*)b;
	string ky;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		itres = A->values.find(it->first);
		if (itres != A->values.end()) {
			if (it->second->same(itres->second) == kifTRUE) {
				ky = it->first;
				ref->Push(ky, it->second);
				itres->second->Setreference();
			}
		}
	}
	return ref;
}

Exported KifElement* KifTreeMap::same(KifElement* a) {

	if (a->type != kifTreeMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMap* m = (KifTreeMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<string, KifElement*>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second->same(values[it->first]) == kifFALSE)
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifTreeMap::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifTreeMap)
		return a->plus(a, b, context, autoself);

	KifTreeMap* ref = this;
	if (autoself == false)
		ref = (KifTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMap*)a;

	map<string, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifTreeMap::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifTreeMap)
		return a->minus(a, b, context, autoself);

	KifTreeMap* ref = this;
	if (autoself == false)
		ref = (KifTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMap*)a;

	map<string, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifTreeMap::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifTreeMap)
		return a->multiply(a, b, context, autoself);

	KifTreeMap* ref = this;
	if (autoself == false)
		ref = (KifTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMap*)a;

	map<string, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifTreeMap::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifTreeMap)
		return a->divide(a, b, context, autoself);

	KifTreeMap* ref = this;
	if (autoself == false)
		ref = (KifTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMap*)a;

	map<string, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifTreeMap::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifTreeMap)
		return a->mod(a, b, context, autoself);

	KifTreeMap* ref = this;
	if (autoself == false)
		ref = (KifTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMap*)a;

	map<string, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifTreeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifTreeMap)
		return a->shiftright(a, b, context, autoself);

	KifTreeMap* ref = this;
	if (autoself == false)
		ref = (KifTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMap*)a;

	map<string, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifTreeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifTreeMap)
		return a->shiftleft(a, b, context, autoself);

	KifTreeMap* ref = this;
	if (autoself == false)
		ref = (KifTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMap*)a;

	map<string, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifTreeMap::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifTreeMap)
		return a->power(a, b, context, autoself);

	KifTreeMap* ref = this;
	if (autoself == false)
		ref = (KifTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMap*)a;

	map<string, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifTreeMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorTreeMap* iter = new KifIteratorTreeMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifTreeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
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

KifElement* KifTreeMap::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<string, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

KifElement* KifTreeMap::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Map::Copy");
#endif
	KifTreeMap* ke = new KifTreeMap(kifcode, NULL);
	map<string, KifElement*>::iterator itx;
	string n;
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

KifElement* KifTreeMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

Exported void KifTreeMap::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror("KIF(689): Circular structure");
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	map<string, KifElement*>::iterator itx;
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


KifElement* KifIteratorTreeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (reverse == false) {
		if (it == values->end())
			return kifNULL;
		return it->second->Exec(contextualpattern, dom, val, idthread, callfunc);
	}
	if (itr == values->rend())
		return kifNULL;
	return itr->second->Exec(contextualpattern, dom, val, idthread, callfunc);
}



static void resetMap(KifTreeMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	map<string, KifElement*>& values = kmap->values;
	if (values.size() == 0)
		return;

	map<string, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}

void KifTreeMap::Resetreference(int inc) {
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

void KifTreeMap::Clear() {
	KifContainerLock _lock(this);
	map<string, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(reference + 1);
	values.clear();
}

void KifTreeMap::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string n = itr->IteratorKeyString();
		if (values.find(n) == values.end())
			Push(n, itr->IteratorValue());
	}
	itr->Release();
}

KifElement* KifTreeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* ke;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			map<string, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++) {
				ke = kifcode->Providestringraw(it->first);
				vect->Push(ke);
			}
			return vect;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifTreeMap* kmap = new KifTreeMap(kifcode, NULL);
			map<string, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++) {
				ke = it->second->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
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
			KifTreeMap* kmap = new KifTreeMap(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			string vleft = key->String();
			string vright = keyright->String();
			map<string, KifElement*>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<string, KifElement*>::iterator itr = values.find(vright);
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

void KifTreeMap::Pop(KifElement* ke) {
	string s = ke->String();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return;

		map<string, KifElement*>::iterator itx = values.find(s);

		if (itx == values.end()) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(kifErrorStrings[223]);
			return;
		}

		kres = itx->second;
		values.erase(itx);
	}
	kres->Resetreference(reference + 1);
}

Exported void KifTreeMap::push(string key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		if (values[key] == val)
			return;

		val = val->Atom();
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}



//To insert a value into a map through the API 
Exported void KifTreeMap::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		if (values[key] == val)
			return;

		val = val->Atom();
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifTreeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		if (values[key] == val)
			return;

		val = val->Atom();
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifTreeMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	string key = k->String();
	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		if (values[key] == val)
			return;

		val = val->Atom();
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}
	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

//--------------------------------------------------------------------------------------------------
KifElement* KifIntegerTreeMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	map<long, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifIntegerTreeMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	map<long, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->Push(it->second);
	return kvect;
}

string KifIntegerTreeMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<long, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":";
		sx = it->second->StringForDisplay();
		if (!it->second->aString())
			res << sx;
		else
			stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifIntegerTreeMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<long, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":" << it->second->JSonString();
	}
	res << "}";
	return res.str();
}

string KifIntegerTreeMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<long, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":" << it->second->BaseString();
	}
	res << "}";
	return res.str();
}


KifElement* KifIntegerTreeMap::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifIntegerTreeMap)
		return kifNULL;

	KifIntegerTreeMap* A = (KifIntegerTreeMap*)a;
	map<long, KifElement*>::iterator it;

	KifIntegerTreeMap* ref = new KifIntegerTreeMap(kifcode, NULL);
	KifIntegerTreeMap* bref = (KifIntegerTreeMap*)b;
	long ky;
	KifContainerLock _lock(bref);

	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		ky = it->first;
		if (A->values.find(ky) == A->values.end()) {
			ref->push(ky, it->second);
			it->second->Setreference();
		}
	}

	KifContainerLock _locka(A);
	for (it = A->values.begin(); it != A->values.end(); it++) {
		ky = it->first;
		if (bref->values.find(ky) == bref->values.end()) {
			ref->push(ky, it->second);
			it->second->Setreference();
		}
	}
	return ref;
}

KifElement* KifIntegerTreeMap::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifIntegerTreeMap)
		return kifNULL;

	KifIntegerTreeMap* A = (KifIntegerTreeMap*)a;
	map<long, KifElement*>::iterator it;

	KifIntegerTreeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerTreeMap*)a->Copy(NULL);

	KifIntegerTreeMap* bref = (KifIntegerTreeMap*)b;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		if (A->values.find(it->first) == A->values.end()) {
			ref->push(it->first, it->second);
			it->second->Setreference();
		}
	}
	return ref;
}

KifElement* KifIntegerTreeMap::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifIntegerTreeMap)
		return kifNULL;

	KifIntegerTreeMap* A = (KifIntegerTreeMap*)a;
	map<long, KifElement*>::iterator it;
	map<long, KifElement*>::iterator itres;

	KifIntegerTreeMap* ref = new KifIntegerTreeMap(kifcode, NULL);
	KifIntegerTreeMap* bref = (KifIntegerTreeMap*)b;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		itres = A->values.find(it->first);
		if (itres != A->values.end()) {
			if (it->second->same(itres->second) == kifTRUE) {
				ref->push(it->first, it->second);
				itres->second->Setreference();
			}
		}
	}
	return ref;
}

Exported KifElement* KifIntegerTreeMap::same(KifElement* a) {

	if (a->type != kifIntegerTreeMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifIntegerTreeMap* m = (KifIntegerTreeMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<long, KifElement*>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second->same(values[it->first]) == kifFALSE)
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifIntegerTreeMap::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->plus(a, b, context, autoself);

	KifIntegerTreeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerTreeMap*)a;

	map<long, KifElement*>::iterator it;
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

KifElement* KifIntegerTreeMap::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->minus(a, b, context, autoself);

	KifIntegerTreeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerTreeMap*)a;

	map<long, KifElement*>::iterator it;
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

KifElement* KifIntegerTreeMap::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->multiply(a, b, context, autoself);

	KifIntegerTreeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerTreeMap*)a;

	map<long, KifElement*>::iterator it;
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

KifElement* KifIntegerTreeMap::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->divide(a, b, context, autoself);

	KifIntegerTreeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerTreeMap*)a;

	map<long, KifElement*>::iterator it;
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

KifElement* KifIntegerTreeMap::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->mod(a, b, context, autoself);

	KifIntegerTreeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerTreeMap*)a;

	map<long, KifElement*>::iterator it;
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

KifElement* KifIntegerTreeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->shiftright(a, b, context, autoself);

	KifIntegerTreeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerTreeMap*)a;

	map<long, KifElement*>::iterator it;
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

KifElement* KifIntegerTreeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->shiftleft(a, b, context, autoself);

	KifIntegerTreeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerTreeMap*)a;

	map<long, KifElement*>::iterator it;
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

KifElement* KifIntegerTreeMap::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->power(a, b, context, autoself);

	KifIntegerTreeMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerTreeMap*)a;

	map<long, KifElement*>::iterator it;
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

KifElement* KifIntegerTreeMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorIntegerTreeMap* iter = new KifIteratorIntegerTreeMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifIntegerTreeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
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

KifElement* KifIntegerTreeMap::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<long, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

KifElement* KifIntegerTreeMap::Copy(KifDomain* kp, KifElement* dom) {
	KifIntegerTreeMap* ke = new KifIntegerTreeMap(kifcode, NULL);
	map<long, KifElement*>::iterator itx;
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

KifElement* KifIntegerTreeMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

Exported void KifIntegerTreeMap::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror("KIF(689): Circular structure");
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	map<long, KifElement*>::iterator itx;
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


string KifIteratorIntegerTreeMap::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (reverse == false) {
		if (it == values->end())
			return "";
		res << it->first;
		return res.str();
	}
	if (itr == values->rend())
		return "";

	res << itr->first;
	return res.str();
}

KifElement* KifIteratorIntegerTreeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (reverse == false) {
		if (it == values->end())
			return kifNULL;
		return it->second->Exec(contextualpattern, dom, val, idthread, callfunc);
	}
	if (itr == values->rend())
		return kifNULL;
	return itr->second->Exec(contextualpattern, dom, val, idthread, callfunc);
}



static void resetMap(KifIntegerTreeMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	map<long, KifElement*>& values = kmap->values;
	if (values.size() == 0)
		return;

	map<long, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}

void KifIntegerTreeMap::Resetreference(int inc) {
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

void KifIntegerTreeMap::Clear() {
	KifContainerLock _lock(this);
	map<long, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(reference + 1);
	values.clear();
}

void KifIntegerTreeMap::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (values.find(n) == values.end()) {
			ke = itr->IteratorValue()->Atom();
			values[n] = ke;
			ke->Setreference(reference + 1);
		}
	}
	itr->Release();
}


KifElement* KifIntegerTreeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* ke;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			map<long, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				vect->Push(kifcode->Provideinteger(it->first));
			return vect;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifIntegerTreeMap* kmap = new KifIntegerTreeMap(kifcode, NULL);
			map<long, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++) {
				ke = it->second->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
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
			KifIntegerTreeMap* kmap = new KifIntegerTreeMap(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			map<long, KifElement*>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<long, KifElement*>::iterator itr = values.find(vright);
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

	long skey = key->Integer();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return values[skey];
}

void KifIntegerTreeMap::Pop(KifElement* ke) {
	long s = ke->Integer();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return;

		map<long, KifElement*>::iterator itx = values.find(s);

		if (itx == values.end()) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(kifErrorStrings[223]);
			return;
		}

		kres = itx->second;
		values.erase(itx);
	}
	kres->Resetreference(reference + 1);
}

//To insert a value into a map through the API 
Exported void KifIntegerTreeMap::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	long n = conversionintegerhexa(STR(key));
	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
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

Exported void KifIntegerTreeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long n = conversionintegerhexa(key);
	if (values.find(n) != values.end()) {
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

Exported void KifIntegerTreeMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	long n = k->Integer();
	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
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


void KifIntegerTreeMap::push(long n, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
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
KifElement* KifFloatTreeMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	map<double, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifFloatTreeMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	map<double, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->Push(it->second);
	return kvect;
}

string KifFloatTreeMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<double, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":";
		sx = it->second->StringForDisplay();
		if (!it->second->aString())
			res << sx;
		else
			stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifFloatTreeMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<double, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":" << it->second->JSonString();
	}
	res << "}";
	return res.str();
}

string KifFloatTreeMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<double, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":" << it->second->BaseString();
	}
	res << "}";
	return res.str();
}


KifElement* KifFloatTreeMap::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifFloatTreeMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifFloatTreeMap)
		return kifNULL;

	KifFloatTreeMap* A = (KifFloatTreeMap*)a;
	map<double, KifElement*>::iterator it;

	KifFloatTreeMap* ref = new KifFloatTreeMap(kifcode, NULL);
	KifFloatTreeMap* bref = (KifFloatTreeMap*)b;
	double ky;
	KifContainerLock _lock(bref);

	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		ky = it->first;
		if (A->values.find(ky) == A->values.end()) {
			ref->push(ky, it->second);
			it->second->Setreference();
		}
	}

	KifContainerLock _locka(A);
	for (it = A->values.begin(); it != A->values.end(); it++) {
		ky = it->first;
		if (bref->values.find(ky) == bref->values.end()) {
			ref->push(ky, it->second);
			it->second->Setreference();
		}
	}
	return ref;
}

KifElement* KifFloatTreeMap::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifFloatTreeMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifFloatTreeMap)
		return kifNULL;

	KifFloatTreeMap* A = (KifFloatTreeMap*)a;
	map<double, KifElement*>::iterator it;

	KifFloatTreeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatTreeMap*)a->Copy(NULL);

	KifFloatTreeMap* bref = (KifFloatTreeMap*)b;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		if (A->values.find(it->first) == A->values.end()) {
			ref->push(it->first, it->second);
			it->second->Setreference();
		}
	}
	return ref;
}

KifElement* KifFloatTreeMap::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifFloatTreeMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifFloatTreeMap)
		return kifNULL;

	KifFloatTreeMap* A = (KifFloatTreeMap*)a;
	map<double, KifElement*>::iterator it;
	map<double, KifElement*>::iterator itres;

	KifFloatTreeMap* ref = new KifFloatTreeMap(kifcode, NULL);
	KifFloatTreeMap* bref = (KifFloatTreeMap*)b;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		itres = A->values.find(it->first);
		if (itres != A->values.end()) {
			if (it->second->same(itres->second) == kifTRUE) {
				ref->push(it->first, it->second);
				itres->second->Setreference();
			}
		}
	}
	return ref;
}

Exported KifElement* KifFloatTreeMap::same(KifElement* a) {

	if (a->type != kifFloatTreeMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifFloatTreeMap* m = (KifFloatTreeMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<double, KifElement*>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second->same(values[it->first]) == kifFALSE)
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifFloatTreeMap::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifFloatTreeMap)
		return a->plus(a, b, context, autoself);

	KifFloatTreeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatTreeMap*)a;

	map<double, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnFloatKey(it->first);
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

KifElement* KifFloatTreeMap::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifFloatTreeMap)
		return a->minus(a, b, context, autoself);

	KifFloatTreeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatTreeMap*)a;

	map<double, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnFloatKey(it->first);
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

KifElement* KifFloatTreeMap::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifFloatTreeMap)
		return a->multiply(a, b, context, autoself);

	KifFloatTreeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatTreeMap*)a;

	map<double, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnFloatKey(it->first);
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

KifElement* KifFloatTreeMap::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifFloatTreeMap)
		return a->divide(a, b, context, autoself);

	KifFloatTreeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatTreeMap*)a;

	map<double, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnFloatKey(it->first);
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

KifElement* KifFloatTreeMap::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifFloatTreeMap)
		return a->mod(a, b, context, autoself);

	KifFloatTreeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatTreeMap*)a;

	map<double, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnFloatKey(it->first);
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

KifElement* KifFloatTreeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifFloatTreeMap)
		return a->shiftright(a, b, context, autoself);

	KifFloatTreeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatTreeMap*)a;

	map<double, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnFloatKey(it->first);
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

KifElement* KifFloatTreeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifFloatTreeMap)
		return a->shiftleft(a, b, context, autoself);

	KifFloatTreeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatTreeMap*)a;

	map<double, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnFloatKey(it->first);
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

KifElement* KifFloatTreeMap::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifFloatTreeMap)
		return a->power(a, b, context, autoself);

	KifFloatTreeMap* ref = this;
	if (autoself == false)
		ref = (KifFloatTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatTreeMap*)a;

	map<double, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {


		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnFloatKey(it->first);
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

KifElement* KifFloatTreeMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorFloatTreeMap* iter = new KifIteratorFloatTreeMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifFloatTreeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
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

KifElement* KifFloatTreeMap::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<double, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

KifElement* KifFloatTreeMap::Copy(KifDomain* kp, KifElement* dom) {
	KifFloatTreeMap* ke = new KifFloatTreeMap(kifcode, NULL);
	map<double, KifElement*>::iterator itx;
	double n;
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

KifElement* KifFloatTreeMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

Exported void KifFloatTreeMap::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror("KIF(689): Circular structure");
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	map<double, KifElement*>::iterator itx;
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


string KifIteratorFloatTreeMap::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (reverse == false) {
		if (it == values->end())
			return "";
		res << it->first;
		return res.str();
	}
	if (itr == values->rend())
		return "";

	res << itr->first;
	return res.str();
}

KifElement* KifIteratorFloatTreeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (reverse == false) {
		if (it == values->end())
			return kifNULL;
		return it->second->Exec(contextualpattern, dom, val, idthread, callfunc);
	}
	if (itr == values->rend())
		return kifNULL;
	return itr->second->Exec(contextualpattern, dom, val, idthread, callfunc);
}



static void resetMap(KifFloatTreeMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	map<double, KifElement*>& values = kmap->values;
	if (values.size() == 0)
		return;

	map<double, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}

void KifFloatTreeMap::Resetreference(int inc) {
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

void KifFloatTreeMap::Clear() {
	KifContainerLock _lock(this);
	map<double, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(reference + 1);
	values.clear();
}

void KifFloatTreeMap::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		double n = itr->IteratorKeyFloat();
		if (values.find(n) == values.end()) {
			ke = itr->IteratorValue()->Atom();
			values[n] = ke;
			ke->Setreference(reference + 1);
		}
	}
	itr->Release();
}

KifElement* KifFloatTreeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* ke;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			map<double, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				vect->Push(kifcode->Providefloat(it->first));
			return vect;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifFloatTreeMap* kmap = new KifFloatTreeMap(kifcode, NULL);
			map<double, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++) {
				ke = it->second->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
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
			KifFloatTreeMap* kmap = new KifFloatTreeMap(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			double vleft = key->Float();
			double vright = keyright->Float();
			map<double, KifElement*>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<double, KifElement*>::iterator itr = values.find(vright);
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

	double skey = key->Float();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return values[skey];
}

void KifFloatTreeMap::Pop(KifElement* ke) {
	double s = ke->Float();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return;

		map<double, KifElement*>::iterator itx = values.find(s);

		if (itx == values.end()) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(kifErrorStrings[223]);
			return;
		}

		kres = itx->second;
		values.erase(itx);
	}
	kres->Resetreference(reference + 1);
}

//To insert a value into a map through the API 
Exported void KifFloatTreeMap::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	double n = convertfloat(STR(key));
	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
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

Exported void KifFloatTreeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	double n = convertfloat(key);
	if (values.find(n) != values.end()) {
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

Exported void KifFloatTreeMap::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	double n = key->Float();
	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
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


void KifFloatTreeMap::push(double n, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
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

//----------------------------------------------------------------------------------
void KifTreeMapStringString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapStringString* kmap = (KifTreeMapStringString*)ke;
		map<string, string>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string n = itr->IteratorKeyString();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueString();
	}
	itr->Release();
}

KifElement* KifTreeMapStringString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapStringString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


Exported void KifTreeMapStringString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->String();
}

Exported void KifTreeMapStringString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->String();
}

Exported void KifTreeMapStringString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->String();
}

string KifTreeMapStringString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<string, string>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->first;
		stringing(res, sx);
		res << ":";
		sx = it->second;
		stringing(res, sx);
	}
	res << "}";
	return res.str();
}

string KifTreeMapStringString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<string, string>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->first;
		jstringing(res, sx);
		res << ":";
		sx = it->second;
		jstringing(res, sx);
	}
	res << "}";
	return res.str();
}

KifElement* KifTreeMapStringString::Newiterator(bool rev) {
	KifIteratorTreeMapStringString* iter = new KifIteratorTreeMapStringString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapStringString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	string s = ke->String();
	map<string, string>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifTreeMapStringString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapStringString) {
			KifTreeMapStringString* kvect = (KifTreeMapStringString*)ke;
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
	else {
		string key = idx->String();
		values[key] = ke->String();
	}
	return true;
}

KifElement* KifTreeMapStringString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapStringString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<string, string>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Providestringraw(it->first));
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
			KifTreeMapStringString* kmap = new KifTreeMapStringString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			string vleft = key->String();
			string vright = keyright->String();
			map<string, string>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<string, string>::iterator itr = values.find(vright);
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
	string skey = key->String();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providestringraw(values[skey]);
}

Exported KifElement* KifTreeMapStringString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kmap->push(it->first, (KifElement*)kifcode->Providestring(it->second));
	return kmap;
}

KifElement* KifTreeMapStringString::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifTreeMapStringString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapStringString* m = (KifTreeMapStringString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<string, string>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifTreeMapStringString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapStringString* ref = (KifTreeMapStringString*)a->Copy(NULL);
		string v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyString();
			if (ref->values.find(v) == ref->values.end())
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifTreeMapStringString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapStringString* ref = new KifTreeMapStringString(kifcode, NULL);
		KifTreeMapStringString* A = (KifTreeMapStringString*)a->Copy(NULL);
		string v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyString();
			if (A->values.find(v) != A->values.end())
			if (A->values[v] == itr->IteratorValueString())
				ref->values[v] = A->values[v];
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifTreeMapStringString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapStringString* ref = new KifTreeMapStringString(kifcode, NULL);
		map<string, string>& val = ((KifTreeMapStringString*)a)->values;
		string v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyString();
			if (val.find(v) == val.end())
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();

		map<string, string>::iterator it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnKey(it->first) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifTreeMapStringString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringString* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringString*)a;

	KifContainerLock _lock(this);

	map<string, string>::iterator it;
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

	if (b->type == kifTreeMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifTreeMap* bref = (KifTreeMap*)b;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
				it->second += bref->values[it->first]->String();
			}
		}
		return ref;
	}

	if (b->type == kifTreeMapStringString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifTreeMapStringString* bref = (KifTreeMapStringString*)b;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}
		return ref;
	}

	string n = b->String();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second += n;
	return ref;
}


KifElement* KifTreeMapStringString::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapStringString* ke = new KifTreeMapStringString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//----------------------------------------------------------------------------------
KifElement* KifTreeMapStringInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapStringInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	map<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

void KifTreeMapStringInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapStringInteger* kmap = (KifTreeMapStringInteger*)ke;
		map<string, long>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string n = itr->IteratorKeyString();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifTreeMapStringInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}


Exported void KifTreeMapStringInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}

Exported void KifTreeMapStringInteger::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}



string KifTreeMapStringInteger::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<string, long>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->first;
		stringing(res, sx);
		res << ":" << it->second;
	}
	res << "}";
	return res.str();
}

string KifTreeMapStringInteger::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<string, long>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->first;
		jstringing(res, sx);
		res << ":" << it->second;
	}
	res << "}";
	return res.str();
}


KifElement* KifTreeMapStringInteger::Newiterator(bool rev) {
	KifIteratorTreeMapStringInteger* iter = new KifIteratorTreeMapStringInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}


void KifTreeMapStringInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	string s = ke->String();
	map<string, long>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifTreeMapStringInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapStringInteger) {
			KifTreeMapStringInteger* kvect = (KifTreeMapStringInteger*)ke;
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
	else {
		string key = idx->String();
		values[key] = ke->Integer();
	}
	return true;
}


KifElement* KifTreeMapStringInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapStringInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<string, long>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Providestringraw(it->first));
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
			KifTreeMapStringInteger* kmap = new KifTreeMapStringInteger(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			string vleft = key->String();
			string vright = keyright->String();
			map<string, long>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<string, long>::iterator itr = values.find(vright);
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
		return kifNULL;
	string skey = key->String();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Provideinteger(values[skey]);
}

Exported KifElement* KifTreeMapStringInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kmap->push(it->first, (KifElement*)kifcode->Provideinteger(it->second));
	return kmap;
}

KifElement* KifTreeMapStringInteger::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifTreeMapStringInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapStringInteger* m = (KifTreeMapStringInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<string, long>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifTreeMapStringInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapStringInteger* ke = new KifTreeMapStringInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifTreeMapStringInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second |= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second &= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second ^= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second -= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second *= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	long v;
	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = pow((double)it->second, (double)bref->values[it->first]);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapStringInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second >>= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second <<= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	long v;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyInteger(it->first, add);
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

//----------------------------------------------------------------------------------------
void KifTreeMapStringFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapStringFloat* kmap = (KifTreeMapStringFloat*)ke;
		map<string, double>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		string n = itr->IteratorKeyString();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueFloat();
	}
	itr->Release();
}

KifElement* KifTreeMapStringFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapStringFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	map<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


Exported void KifTreeMapStringFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifTreeMapStringFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifTreeMapStringFloat::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->Float();
}


string KifTreeMapStringFloat::String() {
	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	stringstream res;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->first;
		stringing(res, sx);
		res << ":" << it->second;
	}
	res << "}";
	return res.str();
}

string KifTreeMapStringFloat::JSonString() {
	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	stringstream res;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->first;
		jstringing(res, sx);
		res << ":" << it->second;
	}
	res << "}";
	return res.str();
}

KifElement* KifTreeMapStringFloat::Newiterator(bool rev) {
	KifIteratorTreeMapStringFloat* iter = new KifIteratorTreeMapStringFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapStringFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	string s = ke->String();
	map<string, double>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifTreeMapStringFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapStringFloat) {
			KifTreeMapStringFloat* kvect = (KifTreeMapStringFloat*)ke;
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
	else {
		string key = idx->String();
		values[key] = ke->Float();
	}
	return true;
}

KifElement* KifTreeMapStringFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapStringFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<string, double>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Providestringraw(it->first));
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
			KifTreeMapStringFloat* kmap = new KifTreeMapStringFloat(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			string vleft = key->String();
			string vright = keyright->String();
			map<string, double>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<string, double>::iterator itr = values.find(vright);
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
		return kifNULL;
	string skey = key->String();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providefloat(values[skey]);
}

Exported KifElement* KifTreeMapStringFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kmap->push(it->first, (KifElement*)kifcode->Providefloat(it->second));
	return kmap;
}

KifElement* KifTreeMapStringFloat::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifTreeMapStringFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapStringFloat* m = (KifTreeMapStringFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<string, double>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifTreeMapStringFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = (long)it->second | bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = (long)it->second&bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = (long)it->second^bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	map<string, double>::iterator it;
	if (b->type == kifTreeMapStringFloat) {
		KifTreeMapStringFloat* bref = (KifTreeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapStringFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	map<string, double>::iterator it;
	if (b->type == kifTreeMapStringFloat) {
		KifTreeMapStringFloat* bref = (KifTreeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second -= bref->values[it->first];
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapStringFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	map<string, double>::iterator it;
	if (b->type == kifTreeMapStringFloat) {
		KifTreeMapStringFloat* bref = (KifTreeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second *= bref->values[it->first];
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapStringFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	double v;
	map<string, double>::iterator it;
	if (b->type == kifTreeMapStringFloat) {
		KifTreeMapStringFloat* bref = (KifTreeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapStringFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	long v;
	map<string, double>::iterator it;
	if (b->type == kifTreeMapStringFloat) {
		KifTreeMapStringFloat* bref = (KifTreeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapStringFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapStringFloat* ke = new KifTreeMapStringFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifTreeMapStringFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	map<string, double>::iterator it;
	KifContainerLock _lock(this);

	if (b->type == kifTreeMapStringFloat) {
		KifTreeMapStringFloat* bref = (KifTreeMapStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = pow(it->second, bref->values[it->first]);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapStringFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = (long)it->second >> bref->values[it->first];
		}
		return ref;
	}

	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapStringFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapStringFloat*)a;

	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	if (b->type == kifTreeMapStringInteger) {
		KifTreeMapStringInteger* bref = (KifTreeMapStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = (long)it->second << bref->values[it->first];
		}
		return ref;
	}

	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

//----------------------------------------------------------------------------------------
string KifIteratorTreeMapIntegerInteger::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (reverse == false) {
		if (it == values->end())
			return "";
		res << it->first;
		return res.str();
	}
	if (itr == values->rend())
		return "";

	res << itr->first;
	return res.str();
}

KifElement* KifTreeMapIntegerInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	map<long, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapIntegerInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	map<long, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


void KifTreeMapIntegerInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapIntegerInteger* kmap = (KifTreeMapIntegerInteger*)ke;
		map<long, long>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifTreeMapIntegerInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(key);
	values[nb] = val->Integer();
}

Exported void KifTreeMapIntegerInteger::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long nb = key->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->Integer();
}


Exported void KifTreeMapIntegerInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(STR(key));
	values[nb] = val->Integer();
}


string KifTreeMapIntegerInteger::String() {
	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	stringstream res;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":" << it->second;
	}
	res << "}";
	return res.str();
}

KifElement* KifTreeMapIntegerInteger::Newiterator(bool rev) {
	KifIteratorTreeMapIntegerInteger* iter = new KifIteratorTreeMapIntegerInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapIntegerInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	long s = ke->Integer();
	map<long, long>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifTreeMapIntegerInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapIntegerInteger) {
			KifTreeMapIntegerInteger* kvect = (KifTreeMapIntegerInteger*)ke;
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
		long key = idx->Integer();
		values[key] = ke->Integer();
	}
	return true;
}

KifElement* KifTreeMapIntegerInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapIntegerInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<long, long>::iterator it;
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
			KifTreeMapIntegerInteger* kmap = new KifTreeMapIntegerInteger(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			map<long, long>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<long, long>::iterator itr = values.find(vright);
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
		return kifNULL;
	long skey = key->Integer();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Provideinteger(values[skey]);
}

Exported KifElement* KifTreeMapIntegerInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<long, long>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Provideinteger(it->second));
	}
	return kmap;
}

KifElement* KifTreeMapIntegerInteger::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<long, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifTreeMapIntegerInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapIntegerInteger* m = (KifTreeMapIntegerInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<long, long>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifTreeMapIntegerInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapIntegerInteger* ke = new KifTreeMapIntegerInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifTreeMapIntegerInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	long v;
	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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

KifElement* KifTreeMapIntegerInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	map<long, long>::iterator it;
	long v;
	if (b->type == kifTreeMapIntegerInteger) {
		KifTreeMapIntegerInteger* bref = (KifTreeMapIntegerInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
string KifIteratorTreeMapIntegerString::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (reverse == false) {
		if (it == values->end())
			return "";
		res << it->first;
		return res.str();
	}
	if (itr == values->rend())
		return "";

	res << itr->first;
	return res.str();
}

void KifTreeMapIntegerString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapIntegerString* kmap = (KifTreeMapIntegerString*)ke;
		map<long, string>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueString();
	}
	itr->Release();
}

KifElement* KifTreeMapIntegerString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	map<long, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapIntegerString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<long, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

Exported void KifTreeMapIntegerString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(STR(key));
	values[nb] = val->String();
}

Exported void KifTreeMapIntegerString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(key);
	values[nb] = val->String();
}

Exported void KifTreeMapIntegerString::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long nb = key->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->String();
}


string KifTreeMapIntegerString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<long, string>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":";
		stringing(res, it->second);
	}
	res << "}";
	return res.str();
}

string KifTreeMapIntegerString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<long, string>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":";
		jstringing(res, it->second);
	}
	res << "}";
	return res.str();
}

KifElement* KifTreeMapIntegerString::Newiterator(bool rev) {
	KifIteratorTreeMapIntegerString* iter = new KifIteratorTreeMapIntegerString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapIntegerString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	long s = ke->Integer();
	map<long, string>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifTreeMapIntegerString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapIntegerString) {
			KifTreeMapIntegerString* kvect = (KifTreeMapIntegerString*)ke;
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
		long key = idx->Integer();
		values[key] = ke->String();
	}
	return true;
}

KifElement* KifTreeMapIntegerString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapIntegerString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<long, string>::iterator it;
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
			KifTreeMapIntegerString* kmap = new KifTreeMapIntegerString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			map<long, string>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<long, string>::iterator itr = values.find(vright);
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

	long skey = key->Integer();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providestringraw(values[skey]);
}

Exported KifElement* KifTreeMapIntegerString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<long, string>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Providestring(it->second));
	}
	return kmap;
}

KifElement* KifTreeMapIntegerString::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<long, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifTreeMapIntegerString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapIntegerString* m = (KifTreeMapIntegerString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<long, string>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifTreeMapIntegerString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapIntegerString* ref = (KifTreeMapIntegerString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (ref->values.find(v) == ref->values.end())
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifTreeMapIntegerString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapIntegerString* ref = new KifTreeMapIntegerString(kifcode, NULL);
		KifTreeMapIntegerString* A = (KifTreeMapIntegerString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (A->values.find(v) != A->values.end())
			if (A->values[v] == itr->IteratorValueString())
				ref->values[v] = A->values[v];
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifTreeMapIntegerString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapIntegerString* ref = new KifTreeMapIntegerString(kifcode, NULL);
		map<long, string>& val = ((KifTreeMapIntegerString*)a)->values;
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (val.find(v) == val.end())
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();

		map<long, string>::iterator it;
		char buffer[50];
		for (it = val.begin(); it != val.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it->first);
			if (b->ValueOnKey(buffer) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifTreeMapIntegerString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerString* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerString*)a;

	KifContainerLock _lock(this);

	map<long, string>::iterator it;
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

	if (b->type == kifTreeMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifTreeMap* bref = (KifTreeMap*)b;
		char buffer[50];
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it->first);
			if (bref->values.find(buffer) != bref->values.end()) {
				it->second += bref->values[buffer]->String();
			}
		}
		return ref;
	}

	if (b->type == kifTreeMapIntegerString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifTreeMapIntegerString* bref = (KifTreeMapIntegerString*)b;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}
		return ref;
	}

	string n = b->String();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second += n;
	return ref;
}


KifElement* KifTreeMapIntegerString::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapIntegerString* ke = new KifTreeMapIntegerString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//---------------------------------------------------------------------------------------------
string KifIteratorTreeMapIntegerFloat::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (reverse == false) {
		if (it == values->end())
			return "";
		res << it->first;
		return res.str();
	}
	if (itr == values->rend())
		return "";

	res << itr->first;
	return res.str();
}


KifElement* KifTreeMapIntegerFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	map<long, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapIntegerFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	map<long, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


void KifTreeMapIntegerFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapIntegerFloat* kmap = (KifTreeMapIntegerFloat*)ke;
		map<long, double>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifTreeMapIntegerFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(key);
	values[nb] = val->Float();
}

Exported void KifTreeMapIntegerFloat::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long nb = key->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->Float();
}


Exported void KifTreeMapIntegerFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(STR(key));
	values[nb] = val->Float();
}


string KifTreeMapIntegerFloat::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<long, double>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":" << it->second;
	}
	res << "}";
	return res.str();
}

KifElement* KifTreeMapIntegerFloat::Newiterator(bool rev) {
	KifIteratorTreeMapIntegerFloat* iter = new KifIteratorTreeMapIntegerFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapIntegerFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	long s = ke->Integer();
	map<long, double>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifTreeMapIntegerFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapIntegerFloat) {
			KifTreeMapIntegerFloat* kvect = (KifTreeMapIntegerFloat*)ke;
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
		long key = idx->Integer();
		values[key] = ke->Float();
	}
	return true;
}

KifElement* KifTreeMapIntegerFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapIntegerFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<long, double>::iterator it;
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
			KifTreeMapIntegerFloat* kmap = new KifTreeMapIntegerFloat(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			map<long, double>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<long, double>::iterator itr = values.find(vright);
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
		return kifNULL;
	long skey = key->Integer();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providefloat(values[skey]);
}

Exported KifElement* KifTreeMapIntegerFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<long, double>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Providefloat(it->second));
	}
	return kmap;
}

KifElement* KifTreeMapIntegerFloat::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<long, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifTreeMapIntegerFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapIntegerFloat* m = (KifTreeMapIntegerFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<long, double>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifTreeMapIntegerFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	map<long, double>::iterator it;
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

KifElement* KifTreeMapIntegerFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	map<long, double>::iterator it;

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

KifElement* KifTreeMapIntegerFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	map<long, double>::iterator it;

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

KifElement* KifTreeMapIntegerFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	map<long, double>::iterator it;
	if (b->type == kifTreeMapIntegerFloat) {
		KifTreeMapIntegerFloat* bref = (KifTreeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	map<long, double>::iterator it;
	if (b->type == kifTreeMapIntegerFloat) {
		KifTreeMapIntegerFloat* bref = (KifTreeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	map<long, double>::iterator it;
	if (b->type == kifTreeMapIntegerFloat) {
		KifTreeMapIntegerFloat* bref = (KifTreeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	double v;
	map<long, double>::iterator it;
	if (b->type == kifTreeMapIntegerFloat) {
		KifTreeMapIntegerFloat* bref = (KifTreeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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

KifElement* KifTreeMapIntegerFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	long v;
	map<long, double>::iterator it;
	if (b->type == kifTreeMapIntegerFloat) {
		KifTreeMapIntegerFloat* bref = (KifTreeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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

KifElement* KifTreeMapIntegerFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapIntegerFloat* ke = new KifTreeMapIntegerFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifTreeMapIntegerFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	map<long, double>::iterator it;
	KifContainerLock _lock(this);

	if (b->type == kifTreeMapIntegerFloat) {
		KifTreeMapIntegerFloat* bref = (KifTreeMapIntegerFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
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

KifElement* KifTreeMapIntegerFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	map<long, double>::iterator it;

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

KifElement* KifTreeMapIntegerFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	map<long, double>::iterator it;

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
//-------------------------------------------------------------------------------
string KifIteratorTreeMapFloatInteger::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (reverse == false) {
		if (it == values->end())
			return "";
		res << it->first;
		return res.str();
	}
	if (itr == values->rend())
		return "";

	res << itr->first;
	return res.str();
}

KifElement* KifTreeMapFloatInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	map<double, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

KifElement* KifTreeMapFloatInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	map<double, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}


void KifTreeMapFloatInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapFloatInteger* kmap = (KifTreeMapFloatInteger*)ke;
		map<double, long>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		double n = itr->IteratorKeyFloat();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifTreeMapFloatInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(key);
	values[nb] = val->Integer();
}

Exported void KifTreeMapFloatInteger::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->Integer();
}


Exported void KifTreeMapFloatInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(STR(key));
	values[nb] = val->Integer();
}


string KifTreeMapFloatInteger::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<double, long>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":" << it->second;
	}
	res << "}";
	return res.str();
}

KifElement* KifTreeMapFloatInteger::Newiterator(bool rev) {
	KifIteratorTreeMapFloatInteger* iter = new KifIteratorTreeMapFloatInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapFloatInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	double s = ke->Float();
	map<double, long>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifTreeMapFloatInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapFloatInteger) {
			KifTreeMapFloatInteger* kvect = (KifTreeMapFloatInteger*)ke;
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
	else {
		double key = idx->Float();
		values[key] = ke->Integer();
	}
	return true;
}

KifElement* KifTreeMapFloatInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapFloatInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<double, long>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Providefloat(it->first));
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
			KifTreeMapFloatInteger* kmap = new KifTreeMapFloatInteger(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			double vleft = key->Float();
			double vright = keyright->Float();
			map<double, long>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<double, long>::iterator itr = values.find(vright);
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
		return kifNULL;
	double skey = key->Float();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Provideinteger(values[skey]);
}

Exported KifElement* KifTreeMapFloatInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<double, long>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%g", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Provideinteger(it->second));
	}
	return kmap;
}

KifElement* KifTreeMapFloatInteger::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<double, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifTreeMapFloatInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapFloatInteger* m = (KifTreeMapFloatInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<double, long>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifTreeMapFloatInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapFloatInteger* ke = new KifTreeMapFloatInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifTreeMapFloatInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second |= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second &= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second ^= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second -= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second *= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	long v;
	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = pow((double)it->second, (double)bref->values[it->first]);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFFloat(it->first, add);
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

KifElement* KifTreeMapFloatInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second >>= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second <<= bref->values[it->first];
		}
		return ref;
	}

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatInteger*)a;

	KifContainerLock _lock(this);
	map<double, long>::iterator it;
	long v;
	if (b->type == kifTreeMapFloatInteger) {
		KifTreeMapFloatInteger* bref = (KifTreeMapFloatInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyFInteger(it->first, add);
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
string KifIteratorTreeMapFloatString::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (reverse == false) {
		if (it == values->end())
			return "";
		res << it->first;
		return res.str();
	}
	if (itr == values->rend())
		return "";

	res << itr->first;
	return res.str();
}

void KifTreeMapFloatString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapFloatString* kmap = (KifTreeMapFloatString*)ke;
		map<double, string>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		double n = itr->IteratorKeyFloat();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueString();
	}
	itr->Release();
}

KifElement* KifTreeMapFloatString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	map<double, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapFloatString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<double, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


Exported void KifTreeMapFloatString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(STR(key));
	values[nb] = val->String();
}

Exported void KifTreeMapFloatString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(key);
	values[nb] = val->String();
}

Exported void KifTreeMapFloatString::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->String();
}


string KifTreeMapFloatString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<double, string>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":";
		stringing(res, it->second);
	}
	res << "}";
	return res.str();
}

string KifTreeMapFloatString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<double, string>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":";
		jstringing(res, it->second);
	}
	res << "}";
	return res.str();
}

KifElement* KifTreeMapFloatString::Newiterator(bool rev) {
	KifIteratorTreeMapFloatString* iter = new KifIteratorTreeMapFloatString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapFloatString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	double s = ke->Float();
	map<double, string>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifTreeMapFloatString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapFloatString) {
			KifTreeMapFloatString* kvect = (KifTreeMapFloatString*)ke;
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
	else {
		double key = idx->Float();
		values[key] = ke->String();
	}
	return true;
}

KifElement* KifTreeMapFloatString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapFloatString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<double, string>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Providefloat(it->first));

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
			KifTreeMapFloatString* kmap = new KifTreeMapFloatString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			double vleft = key->Float();
			double vright = keyright->Float();
			map<double, string>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<double, string>::iterator itr = values.find(vright);
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
	double skey = key->Float();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providestringraw(values[skey]);
}

Exported KifElement* KifTreeMapFloatString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<double, string>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%g", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Providestring(it->second));
	}
	return kmap;
}

KifElement* KifTreeMapFloatString::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<double, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifTreeMapFloatString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapFloatString* m = (KifTreeMapFloatString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<double, string>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifTreeMapFloatString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapFloatString* ref = (KifTreeMapFloatString*)a->Copy(NULL);
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (ref->values.find(v) == ref->values.end())
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifTreeMapFloatString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapFloatString* ref = new KifTreeMapFloatString(kifcode, NULL);
		KifTreeMapFloatString* A = (KifTreeMapFloatString*)a->Copy(NULL);
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (A->values.find(v) != A->values.end())
			if (A->values[v] == itr->IteratorValueString())
				ref->values[v] = A->values[v];
		}
		itr->Release();
		return ref;
	}
	return kifNULL;
}

KifElement* KifTreeMapFloatString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapFloatString* ref = new KifTreeMapFloatString(kifcode, NULL);
		map<double, string>& val = ((KifTreeMapFloatString*)a)->values;
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (val.find(v) == val.end())
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();

		map<double, string>::iterator it;
		char buffer[50];
		for (it = val.begin(); it != val.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it->first);
			if (b->ValueOnKey(buffer) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifTreeMapFloatString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatString* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatString*)a;

	KifContainerLock _lock(this);

	map<double, string>::iterator it;
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

	if (b->type == kifTreeMap) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifTreeMap* bref = (KifTreeMap*)b;
		char buffer[50];
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			sprintf_s(buffer, 50, "%ld", it->first);
			if (bref->values.find(buffer) != bref->values.end()) {
				it->second += bref->values[buffer]->String();
			}
		}
		return ref;
	}

	if (b->type == kifTreeMapFloatString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifTreeMapFloatString* bref = (KifTreeMapFloatString*)b;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}
		return ref;
	}

	string n = b->String();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second += n;
	return ref;
}


KifElement* KifTreeMapFloatString::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapFloatString* ke = new KifTreeMapFloatString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//---------------------------------------------------------------------------------------------
string KifIteratorTreeMapFloatFloat::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (reverse == false) {
		if (it == values->end())
			return "";
		res << it->first;
		return res.str();
	}

	if (itr == values->rend())
		return "";

	res << itr->first;
	return res.str();
}

KifElement* KifTreeMapFloatFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	map<double, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapFloatFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	map<double, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

void KifTreeMapFloatFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapFloatFloat* kmap = (KifTreeMapFloatFloat*)ke;
		map<double, double>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}
		return;
	}

	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		double n = itr->IteratorKeyFloat();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueInteger();
	}
	itr->Release();
}

Exported void KifTreeMapFloatFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(key);
	values[nb] = val->Float();
}

Exported void KifTreeMapFloatFloat::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->Float();
}


Exported void KifTreeMapFloatFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(STR(key));
	values[nb] = val->Float();
}


string KifTreeMapFloatFloat::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<double, double>::iterator it;
	res << "{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":" << it->second;
	}
	res << "}";
	return res.str();
}

KifElement* KifTreeMapFloatFloat::Newiterator(bool rev) {
	KifIteratorTreeMapFloatFloat* iter = new KifIteratorTreeMapFloatFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapFloatFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	double s = ke->Float();
	map<double, double>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifTreeMapFloatFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapFloatFloat) {
			KifTreeMapFloatFloat* kvect = (KifTreeMapFloatFloat*)ke;
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
	else {
		double key = idx->Float();
		values[key] = ke->Float();
	}
	return true;
}

KifElement* KifTreeMapFloatFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapFloatFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<double, double>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Providefloat(it->first));
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
			KifTreeMapFloatFloat* kmap = new KifTreeMapFloatFloat(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			double vleft = key->Float();
			double vright = keyright->Float();
			map<double, double>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<double, double>::iterator itr = values.find(vright);
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
		return kifNULL;
	double skey = key->Float();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providefloat(values[skey]);
}

Exported KifElement* KifTreeMapFloatFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<double, double>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%g", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Providefloat(it->second));
	}
	return kmap;
}

KifElement* KifTreeMapFloatFloat::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<double, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifTreeMapFloatFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapFloatFloat* m = (KifTreeMapFloatFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<double, double>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifTreeMapFloatFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	KifContainerLock _lock(this);
	map<double, double>::iterator it;
	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	KifContainerLock _lock(this);
	map<double, double>::iterator it;

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	KifContainerLock _lock(this);
	map<double, double>::iterator it;

	if (b->aMapContainer()) {


		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	map<double, double>::iterator it;
	if (b->type == kifTreeMapFloatFloat) {
		KifTreeMapFloatFloat* bref = (KifTreeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFFloat(it->first, add);
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

KifElement* KifTreeMapFloatFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	map<double, double>::iterator it;
	if (b->type == kifTreeMapFloatFloat) {
		KifTreeMapFloatFloat* bref = (KifTreeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second -= bref->values[it->first];
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFFloat(it->first, add);
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

KifElement* KifTreeMapFloatFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	map<double, double>::iterator it;
	if (b->type == kifTreeMapFloatFloat) {
		KifTreeMapFloatFloat* bref = (KifTreeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second *= bref->values[it->first];
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFFloat(it->first, add);
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

KifElement* KifTreeMapFloatFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	double v;
	map<double, double>::iterator it;
	if (b->type == kifTreeMapFloatFloat) {
		KifTreeMapFloatFloat* bref = (KifTreeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyFFloat(it->first, add);
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

KifElement* KifTreeMapFloatFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	long v;
	map<double, double>::iterator it;
	if (b->type == kifTreeMapFloatFloat) {
		KifTreeMapFloatFloat* bref = (KifTreeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapFloatFloat* ke = new KifTreeMapFloatFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifTreeMapFloatFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	map<double, double>::iterator it;
	KifContainerLock _lock(this);

	if (b->type == kifTreeMapFloatFloat) {
		KifTreeMapFloatFloat* bref = (KifTreeMapFloatFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = pow(it->second, bref->values[it->first]);
		}
		return ref;
	}

	double v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFFloat(it->first, add);
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

KifElement* KifTreeMapFloatFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	KifContainerLock _lock(this);
	map<double, double>::iterator it;

	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifTreeMapFloatFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatFloat*)a;

	KifContainerLock _lock(this);
	map<double, double>::iterator it;

	long v;
	if (b->aMapContainer()) {


		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFInteger(it->first, add);
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

KifElement* KifCode::KifParseVarTreeMapIntegerUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapIntegerUString* kv = new KifTreeMapIntegerUString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapFloatUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapFloatUString* kv = new KifTreeMapFloatUString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}


KifElement* KifCode::KifParseVarMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMap* kv;
	if (kifGlobals->evaluation) {
		kv = new KifMap(this, NULL);
		var->Add(kv);
	}
	else
		kv = new KifMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMap* kv = new KifTreeMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarIntegerTreeMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifIntegerTreeMap* kv = new KifIntegerTreeMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarUStringTreeMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifUStringTreeMap* kv = new KifUStringTreeMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarFloatTreeMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifFloatTreeMap* kv = new KifFloatTreeMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapUStringUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapUStringUString* kv = new KifTreeMapUStringUString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapUStringInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapUStringInteger* kv = new KifTreeMapUStringInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapUStringFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapUStringFloat* kv = new KifTreeMapUStringFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapStringString* kv = new KifTreeMapStringString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapStringInteger* kv = new KifTreeMapStringInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapStringFloat* kv = new KifTreeMapStringFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapIntegerString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapIntegerString* kv = new KifTreeMapIntegerString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapIntegerInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapIntegerInteger* kv = new KifTreeMapIntegerInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapIntegerFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapIntegerFloat* kv = new KifTreeMapIntegerFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapFloatString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapFloatString* kv = new KifTreeMapFloatString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapFloatInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapFloatInteger* kv = new KifTreeMapFloatInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapFloatFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapFloatFloat* kv = new KifTreeMapFloatFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarSet(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifSet* kv = new KifSet(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}


KifElement* KifCode::KifParseVarSetString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifSetString* kv = new KifSetString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarSetUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifSetUString* kv = new KifSetUString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarSetInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifSetInteger* kv = new KifSetInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarSetFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifSetFloat* kv = new KifSetFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

//--------------------------
void KifTreeMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<string, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifTreeMapStringString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifTreeMapStringInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifTreeMapStringFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifIntegerTreeMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<long, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifTreeMapIntegerString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<long, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifTreeMapIntegerInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<long, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifTreeMapIntegerFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<long, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifFloatTreeMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<double, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifTreeMapFloatString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<double, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifTreeMapFloatInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<double, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifTreeMapFloatFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<double, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------



string KifUStringTreeMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<wstring, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it->first;
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
		res << ":";
		sx = it->second->String();
		if (!it->second->aString())
			res << sx;
		else
			stringing(res, sx);
	}

	res << "}";
	return res.str();
}

wstring KifUStringTreeMap::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	map<wstring, KifElement*>::iterator it;
	res << L"{";
	bool beg = true;
	wstring sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->first;
		ustringing(res, sx);
		res << L":";
		sx = it->second->UString();
		if (!it->second->aString())
			res << sx;
		else
			ustringing(res, sx);
	}

	res << L"}";

	return res.str();
}

string KifUStringTreeMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<wstring, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it->first;
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
		res << ":" << it->second->JSonString();
	}

	res << "}";

	return res.str();
}

string KifUStringTreeMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<wstring, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it->first;
		sc_unicode_to_utf8(sx, ws);
		res << "'" << Trim(sx) << "'" << ":" << it->second->BaseString();
	}

	res << "}";

	return res.str();
}

KifElement* KifUStringTreeMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	map<wstring, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifUStringTreeMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	map<wstring, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->Push(it->second);
	return kvect;
}

KifElement* KifUStringTreeMap::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != kifUStringTreeMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifUStringTreeMap)
		return kifNULL;

	KifUStringTreeMap* A = (KifUStringTreeMap*)a;
	map<wstring, KifElement*>::iterator it;

	KifUStringTreeMap* ref = new KifUStringTreeMap(kifcode, NULL);
	KifUStringTreeMap* bref = (KifUStringTreeMap*)b;
	wstring ky;
	KifContainerLock _lock(bref);

	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		ky = it->first;
		if (A->values.find(ky) == A->values.end()) {
			ref->Push(ky, it->second);
			it->second->Setreference();
		}

	}


	KifContainerLock _locka(A);
	for (it = A->values.begin(); it != A->values.end(); it++) {
		ky = it->first;
		if (bref->values.find(ky) == bref->values.end()) {
			ref->Push(ky, it->second);
			it->second->Setreference();
		}

	}

	return ref;
}

KifElement* KifUStringTreeMap::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != kifUStringTreeMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifUStringTreeMap)
		return kifNULL;

	KifUStringTreeMap* A = (KifUStringTreeMap*)a;
	map<wstring, KifElement*>::iterator it;

	KifUStringTreeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringTreeMap*)a->Copy(NULL);

	KifUStringTreeMap* bref = (KifUStringTreeMap*)b;
	wstring ky;
	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		if (A->values.find(it->first) == A->values.end()) {
			ky = it->first;
			ref->Push(ky, it->second);
			it->second->Setreference();
		}

	}

	return ref;
}

KifElement* KifUStringTreeMap::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifUStringTreeMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifUStringTreeMap)
		return kifNULL;

	KifUStringTreeMap* A = (KifUStringTreeMap*)a;
	map<wstring, KifElement*>::iterator it;
	map<wstring, KifElement*>::iterator itres;

	KifUStringTreeMap* ref = new KifUStringTreeMap(kifcode, NULL);
	KifUStringTreeMap* bref = (KifUStringTreeMap*)b;
	wstring ky;

	KifContainerLock _lock(bref);
	KifContainerLock _locka(A);
	for (it = bref->values.begin(); it != bref->values.end(); it++) {
		itres = A->values.find(it->first);
		if (itres != A->values.end()) {
			if (it->second->same(itres->second) == kifTRUE) {
				ky = it->first;
				ref->Push(ky, it->second);
				itres->second->Setreference();
			}

		}

	}

	return ref;
}

Exported KifElement* KifUStringTreeMap::same(KifElement* a) {

	if (a->type != kifUStringTreeMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifUStringTreeMap* m = (KifUStringTreeMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<wstring, KifElement*>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second->same(values[it->first]) == kifFALSE)
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifUStringTreeMap::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifUStringTreeMap)
		return a->plus(a, b, context, autoself);

	KifUStringTreeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringTreeMap*)a;

	map<wstring, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifUStringTreeMap::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifUStringTreeMap)
		return a->minus(a, b, context, autoself);

	KifUStringTreeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringTreeMap*)a;

	map<wstring, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifUStringTreeMap::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifUStringTreeMap)
		return a->multiply(a, b, context, autoself);

	KifUStringTreeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringTreeMap*)a;

	map<wstring, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifUStringTreeMap::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifUStringTreeMap)
		return a->divide(a, b, context, autoself);

	KifUStringTreeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringTreeMap*)a;

	map<wstring, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifUStringTreeMap::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifUStringTreeMap)
		return a->mod(a, b, context, autoself);

	KifUStringTreeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringTreeMap*)a;

	map<wstring, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifUStringTreeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifUStringTreeMap)
		return a->shiftright(a, b, context, autoself);

	KifUStringTreeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringTreeMap*)a;

	map<wstring, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;
	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifUStringTreeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifUStringTreeMap)
		return a->shiftleft(a, b, context, autoself);

	KifUStringTreeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringTreeMap*)a;

	map<wstring, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);

	KifElement* kref;
	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifUStringTreeMap::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifUStringTreeMap)
		return a->power(a, b, context, autoself);

	KifUStringTreeMap* ref = this;
	if (autoself == false)
		ref = (KifUStringTreeMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifUStringTreeMap*)a;

	map<wstring, KifElement*>::iterator it;
	KifElement* ke;
	KifContainerLock _lock(this);
	KifElement* kref;

	if (b->aMapContainer()) {
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = b->ValueOnKey(it->first);
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

KifElement* KifUStringTreeMap::Newiterator(bool rev) {
	KifIteratorUStringTreeMap* iter = new KifIteratorUStringTreeMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifUStringTreeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
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
			KifUStringTreeMap* kvect = (KifUStringTreeMap*)ke;
			//We copy all values from ke to this
			{
				KifContainerLock _lock(kvect);
				map<wstring, KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++)
					push(it->first, it->second);
			}

		}

		return true;
	}


	Push(STR(idx->String()), ke);
	return true;
}

KifElement* KifUStringTreeMap::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifUString* localstring;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	map<wstring, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring = kifcode->Provideustring(it->first);
		kvect->Push(localstring);
	}

	return kvect;
}

KifElement* KifUStringTreeMap::Copy(KifDomain* kp, KifElement* dom) {
	KifUStringTreeMap* ke = new KifUStringTreeMap(kifcode, NULL);
	map<wstring, KifElement*>::iterator itx;
	wstring n;
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

KifElement* KifUStringTreeMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

Exported void KifUStringTreeMap::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror("KIF(689): Circular structure");
		kifGlobals->executionbreak = 1;
		return;
	}

	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	map<wstring, KifElement*>::iterator itx;
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

static void resetUStringTreeMap(KifUStringTreeMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	map<wstring, KifElement*>& values = kmap->values;
	if (values.size() == 0)
		return;

	map<wstring, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}


void KifUStringTreeMap::Resetreference(int inc) {
	reference -= inc;
	if (reference > 0)
		resetUStringTreeMap(this, inc, popped, false);
	else {
		resetUStringTreeMap(this, inc + 1 + reference - popped, popped, true);
		if (popped == 1)
			popped = 0;
		else
			KifElement::Resetreference(0);
	}

}

void KifUStringTreeMap::Clear() {
	KifContainerLock _lock(this);
	map<wstring, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(reference + 1);
	values.clear();
}

void KifUStringTreeMap::Merge(KifElement* ke) {
	//Three cases:
	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		wstring n = itr->IteratorKeyUString();
		if (values.find(n) == values.end())
			Push(n, itr->IteratorValue());
	}

	itr->Release();
}

KifElement* KifUStringTreeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		KifElement* ke;
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			map<wstring, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				vect->Push(kifcode->Provideustring(it->first));
			return vect;
		}


		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifUStringTreeMap* kmap = new KifUStringTreeMap(kifcode, NULL);
			map<wstring, KifElement*>::iterator it;
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
			KifUStringTreeMap* kmap = new KifUStringTreeMap(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			wstring vleft = key->UString();
			wstring vright = keyright->UString();
			map<wstring, KifElement*>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<wstring, KifElement*>::iterator itr = values.find(vright);
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

void KifUStringTreeMap::Pop(KifElement* ke) {
	wstring s = ke->UString();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return;

		map<wstring, KifElement*>::iterator itx = values.find(s);

		if (itx == values.end()) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(kifErrorStrings[223]);
			return;
		}


		kres = itx->second;
		values.erase(itx);
	}

	kres->Resetreference(reference + 1);
}

Exported void KifUStringTreeMap::push(wstring key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		if (values[key] == val)
			return;

		val = val->Atom();
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}

	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifUStringTreeMap::Push(wstring& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		if (values[key] == val)
			return;

		val = val->Atom();
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}

	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifUStringTreeMap::push(string key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	wstring wkey;
	s_utf8_to_unicode(wkey, STR(key));
	KifContainerLock _lock(this);
	if (values.find(wkey) != values.end()) {
		if (values[wkey] == val)
			return;

		val = val->Atom();
		values[wkey]->Resetreference(reference + 1);
		values[wkey] = val;
	}

	else {
		val = val->Atom();
		values[wkey] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifUStringTreeMap::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	wstring wkey;
	s_utf8_to_unicode(wkey, STR(key));
	KifContainerLock _lock(this);
	if (values.find(wkey) != values.end()) {
		if (values[wkey] == val)
			return;

		val = val->Atom();
		values[wkey]->Resetreference(reference + 1);
		values[wkey] = val;
	}

	else {
		val = val->Atom();
		values[wkey] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifUStringTreeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	wstring wkey;
	s_utf8_to_unicode(wkey, key);

	KifContainerLock _lock(this);
	if (values.find(wkey) != values.end()) {
		if (values[wkey] == val)
			return;

		val = val->Atom();
		values[wkey]->Resetreference(reference + 1);
		values[wkey] = val;
	}

	else {
		val = val->Atom();
		values[wkey] = val;
	}
	val->Setreference(reference + 1);
}

Exported void KifUStringTreeMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	wstring key = k->UString();

	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		if (values[key] == val)
			return;

		val = val->Atom();
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}

	else {
		val = val->Atom();
		values[key] = val;
	}
	val->Setreference(reference + 1);
}

void KifUStringTreeMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<wstring, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseUString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}


KifElement* KifIteratorUStringTreeMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (reverse == false) {
		if (it == values->end())
			return kifNULL;
		return it->second->Exec(contextualpattern, dom, val, idthread, callfunc);
	}

	if (itr == values->Rend())
		return kifNULL;
	return itr->second->Exec(contextualpattern, dom, val, idthread, callfunc);
}

//--------------------------------------------------------------------------
void KifTreeMapUStringUString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapUStringUString* kmap = (KifTreeMapUStringUString*)ke;
		map<wstring, wstring>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		wstring n = itr->IteratorKeyUString();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueUString();
	}

	itr->Release();
}

KifElement* KifTreeMapUStringUString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	map<wstring, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapUStringUString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	map<wstring, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

Exported void KifTreeMapUStringUString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key = k->UString();
	KifContainerLock _lock(this);
	values[key] = val->UString();
}

Exported void KifTreeMapUStringUString::Push(string& skey, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key;
	s_utf8_to_unicode(key, STR(skey));
	KifContainerLock _lock(this);
	values[key] = val->UString();
}

Exported void KifTreeMapUStringUString::Push(char* skey, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key;
	s_utf8_to_unicode(key, skey);
	KifContainerLock _lock(this);
	values[key] = val->UString();
}

wstring KifTreeMapUStringUString::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	map<wstring, wstring>::iterator it;
	res << L"{";
	bool beg = true;
	wstring sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		sx = it->first;
		ustringing(res, sx);
		res << ":";
		sx = it->second;
		ustringing(res, sx);
	}

	res << L"}";

	return res.str();
}


string KifTreeMapUStringUString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<wstring, wstring>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it->first;
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
		res << ":";
		ws = it->second;
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
	}

	res << "}";

	return res.str();
}

string KifTreeMapUStringUString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<wstring, wstring>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it->first;
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
		res << ":";
		ws = it->second;
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
	}

	res << "}";

	return res.str();
}

KifElement* KifTreeMapUStringUString::Newiterator(bool rev) {
	KifIteratorTreeMapUStringUString* iter = new KifIteratorTreeMapUStringUString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapUStringUString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	wstring s = ke->UString();
	map<wstring, wstring>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}


	values.erase(itx);
}

bool KifTreeMapUStringUString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifTreeMapUStringUString) {
			KifTreeMapUStringUString* kvect = (KifTreeMapUStringUString*)ke;
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
		wstring key;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			sc_utf8_to_unicode(key, STR(it->first));
			values[key] = it->second->UString();
		}
		kmap->Release();
		return true;
	}

	else {
		wstring key = idx->UString();
		values[key] = ke->UString();
	}

	return true;
}

KifElement* KifTreeMapUStringUString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapUStringUString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<wstring, wstring>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Provideustring(it->first));
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
			KifTreeMapUStringUString* kmap = new KifTreeMapUStringUString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			wstring vleft = key->UString();
			wstring vright = keyright->UString();
			map<wstring, wstring>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<wstring, wstring>::iterator itr = values.find(vright);
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
	wstring skey = key->UString();
	key->Release();

	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Provideustring(values[skey]);
}

Exported KifElement* KifTreeMapUStringUString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<wstring, wstring>::iterator it;
	string key;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		ws = it->first;
		sc_unicode_to_utf8(key, ws);
		kmap->push(key, kifcode->Provideustring(it->second));
	}
	return kmap;
}

KifElement* KifTreeMapUStringUString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifUString* localstring;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	map<wstring, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring = kifcode->Provideustring(it->first);
		kvect->Push(localstring);
	}

	return kvect;
}

Exported KifElement* KifTreeMapUStringUString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapUStringUString* m = (KifTreeMapUStringUString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<wstring, wstring>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifTreeMapUStringUString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapUStringUString* ref = (KifTreeMapUStringUString*)a->Copy(NULL);
		wstring v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyUString();
			if (ref->values.find(v) == ref->values.end())
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifTreeMapUStringUString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapUStringUString* ref = new KifTreeMapUStringUString(kifcode, NULL);
		KifTreeMapUStringUString* A = (KifTreeMapUStringUString*)a->Copy(NULL);
		wstring v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyUString();
			if (A->values.find(v) != A->values.end())
			if (A->values[v] == itr->IteratorValueUString())
				ref->values[v] = A->values[v];
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifTreeMapUStringUString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifTreeMapUStringUString* ref = new KifTreeMapUStringUString(kifcode, NULL);
		map<wstring, wstring>& val = ((KifTreeMapUStringUString*)a)->values;
		wstring v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyUString();
			if (val.find(v) == val.end())
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();

		map<wstring, wstring>::iterator it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnKey(it->first) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}

		return ref;
	}

	return kifNULL;
}

KifElement* KifTreeMapUStringUString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringUString* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringUString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringUString*)a;

	KifContainerLock _lock(this);

	map<wstring, wstring>::iterator it;
	if (b->type == kifList) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifList* bref = (KifList*)b;
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
		string s;
		wstring ws;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			ws = it->first;
			sc_unicode_to_utf8(s, ws);
			if (bref->values.find(s) != bref->values.end()) {
				it->second += bref->values[s]->UString();
			}

		}

		return ref;
	}


	if (b->type == kifTreeMapUStringUString) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapUStringUString* bref = (KifTreeMapUStringUString*)b;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}

		return ref;
	}


	wstring n = b->UString();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second += n;
	return ref;
}

KifElement* KifTreeMapUStringUString::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapUStringUString* ke = new KifTreeMapUStringUString(kifcode, NULL);
	ke->values = values;
	return ke;
}

void KifTreeMapUStringUString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<wstring, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseUString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}
//---------------------------------------------------------------------------------
KifElement* KifTreeMapUStringInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	map<wstring, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapUStringInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	map<wstring, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

void KifTreeMapUStringInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapUStringInteger* kmap = (KifTreeMapUStringInteger*)ke;
		map<wstring, long>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		wstring n = itr->IteratorKeyUString();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueInteger();
	}

	itr->Release();
}

Exported void KifTreeMapUStringInteger::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key = k->UString();
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}

Exported void KifTreeMapUStringInteger::Push(wstring& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}

Exported void KifTreeMapUStringInteger::Push(string& skey, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key;
	s_utf8_to_unicode(key, STR(skey));
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}

Exported void KifTreeMapUStringInteger::Push(char* skey, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key;
	s_utf8_to_unicode(key, skey);
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}

wstring KifTreeMapUStringInteger::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	map<wstring, long>::iterator it;
	res << L"{";
	bool beg = true;
	wstring sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		sx = it->first;
		ustringing(res, sx);
		res << ":" << it->second;
	}

	res << L"}";

	return res.str();
}

string KifTreeMapUStringInteger::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<wstring, long>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it->first;
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
		res << ":" << it->second;
	}

	res << "}";

	return res.str();
}

string KifTreeMapUStringInteger::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<wstring, long>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it->first;
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
		res << ":" << it->second;
	}

	res << "}";

	return res.str();
}

KifElement* KifTreeMapUStringInteger::Newiterator(bool rev) {
	KifIteratorTreeMapUStringInteger* iter = new KifIteratorTreeMapUStringInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapUStringInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	wstring s = ke->UString();
	map<wstring, long>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}


	values.erase(itx);
}

bool KifTreeMapUStringInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifTreeMapUStringInteger) {
			KifTreeMapUStringInteger* kvect = (KifTreeMapUStringInteger*)ke;
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
		wstring k;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			sc_utf8_to_unicode(k, STR(it->first));
			values[k] = it->second->Integer();
		}
		kmap->Release();
		return true;
	}

	else {
		wstring key = idx->UString();
		values[key] = ke->Integer();
	}

	return true;
}

KifElement* KifTreeMapUStringInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapUStringInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<wstring, long>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Provideustring(it->first));

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
			KifTreeMapUStringInteger* kmap = new KifTreeMapUStringInteger(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			wstring vleft = key->UString();
			wstring vright = keyright->UString();
			map<wstring, long>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<wstring, long>::iterator itr = values.find(vright);
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
		return kifNULL;
	wstring skey = key->UString();
	key->Release();

	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Provideinteger(values[skey]);
}

Exported KifElement* KifTreeMapUStringInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<wstring, long>::iterator it;
	wstring ws;
	string k;
	for (it = values.begin(); it != values.end(); it++) {
		ws = it->first;
		sc_unicode_to_utf8(k, ws);
		kmap->push(k, (KifElement*)kifcode->Provideinteger(it->second));
	}
	return kmap;
}

KifElement* KifTreeMapUStringInteger::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifUString* localstring;

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	map<wstring, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring = kifcode->Provideustring(it->first);
		kvect->Push(localstring);
	}

	return kvect;
}

Exported KifElement* KifTreeMapUStringInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapUStringInteger* m = (KifTreeMapUStringInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<wstring, long>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifTreeMapUStringInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapUStringInteger* ke = new KifTreeMapUStringInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifTreeMapUStringInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second |= bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second &= bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second ^= bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second -= bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second *= bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	long v;
	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = pow((double)it->second, (double)bref->values[it->first]);
		}

		return ref;
	}


	double v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapUStringInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second >>= bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second <<= bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringInteger*)a;

	KifContainerLock _lock(this);
	map<wstring, long>::iterator it;
	long v;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyInteger(it->first, add);
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

void KifTreeMapUStringInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<wstring, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseUString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}
//---------------------------------------------------------------------------------
void KifTreeMapUStringFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapUStringFloat* kmap = (KifTreeMapUStringFloat*)ke;
		map<wstring, double>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		wstring n = itr->IteratorKeyUString();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueFloat();
	}

	itr->Release();
}

KifElement* KifTreeMapUStringFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	map<wstring, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapUStringFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	map<wstring, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

Exported void KifTreeMapUStringFloat::Push(string& skey, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key;
	s_utf8_to_unicode(key, STR(skey));
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifTreeMapUStringFloat::Push(char* skey, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key;
	s_utf8_to_unicode(key, skey);
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifTreeMapUStringFloat::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	wstring key = k->UString();
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

wstring KifTreeMapUStringFloat::UString() {
	KifContainerLock _lock(this);
	map<wstring, double>::iterator it;
	std::wstringstream res;
	res << L"{";
	bool beg = true;
	wstring sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		sx = it->first;
		ustringing(res, sx);
		res << L":" << it->second;
	}

	res << L"}";

	return res.str();
}


string KifTreeMapUStringFloat::String() {
	KifContainerLock _lock(this);
	map<wstring, double>::iterator it;
	stringstream res;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it->first;
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
		res << ":" << it->second;
	}

	res << "}";

	return res.str();
}

string KifTreeMapUStringFloat::JSonString() {
	KifContainerLock _lock(this);
	map<wstring, double>::iterator it;
	stringstream res;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		ws = it->first;
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
		res << ":" << it->second;
	}

	res << "}";

	return res.str();
}

KifElement* KifTreeMapUStringFloat::Newiterator(bool rev) {
	KifIteratorTreeMapUStringFloat* iter = new KifIteratorTreeMapUStringFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapUStringFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	wstring s = ke->UString();
	map<wstring, double>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}


	values.erase(itx);
}

bool KifTreeMapUStringFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifTreeMapUStringFloat) {
			KifTreeMapUStringFloat* kvect = (KifTreeMapUStringFloat*)ke;
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
		wstring ws;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			sc_utf8_to_unicode(ws, STR(it->first));
			values[ws] = it->second->Float();
		}
		kmap->Release();
		return true;
	}

	else {
		wstring key = idx->UString();
		values[key] = ke->Float();
	}

	return true;
}

KifElement* KifTreeMapUStringFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapUStringFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<wstring, double>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Provideustring(it->first));
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
			KifTreeMapUStringFloat* kmap = new KifTreeMapUStringFloat(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			wstring vleft = key->UString();
			wstring vright = keyright->UString();
			map<wstring, double>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<wstring, double>::iterator itr = values.find(vright);
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
		return kifNULL;
	wstring skey = key->UString();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Providefloat(values[skey]);
}

Exported KifElement* KifTreeMapUStringFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<wstring, double>::iterator it;
	wstring ws;
	string k;
	for (it = values.begin(); it != values.end(); it++) {
		ws = it->first;
		sc_unicode_to_utf8(k, ws);
		kmap->push(k, (KifElement*)kifcode->Providefloat(it->second));
	}
	return kmap;
}

KifElement* KifTreeMapUStringFloat::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	//We copy all values from ke to this
	map<wstring, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->Push(kifcode->Provideustring(it->first));
	return kvect;
}

Exported KifElement* KifTreeMapUStringFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapUStringFloat* m = (KifTreeMapUStringFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<wstring, double>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifTreeMapUStringFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	KifContainerLock _lock(this);
	map<wstring, double>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = (long)it->second | bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	KifContainerLock _lock(this);
	map<wstring, double>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = (long)it->second&bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	KifContainerLock _lock(this);
	map<wstring, double>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = (long)it->second^bref->values[it->first];
		}

		return ref;
	}


	if (b->aMapContainer()) {
		bool add = true;
		long v;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	map<wstring, double>::iterator it;
	if (b->type == kifTreeMapUStringFloat) {
		KifTreeMapUStringFloat* bref = (KifTreeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}

		return ref;
	}


	double v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapUStringFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	map<wstring, double>::iterator it;
	if (b->type == kifTreeMapUStringFloat) {
		KifTreeMapUStringFloat* bref = (KifTreeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second -= bref->values[it->first];
		}

		return ref;
	}


	double v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapUStringFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	map<wstring, double>::iterator it;
	if (b->type == kifTreeMapUStringFloat) {
		KifTreeMapUStringFloat* bref = (KifTreeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second *= bref->values[it->first];
		}

		return ref;
	}


	double v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapUStringFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	double v;
	map<wstring, double>::iterator it;
	if (b->type == kifTreeMapUStringFloat) {
		KifTreeMapUStringFloat* bref = (KifTreeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapUStringFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	long v;
	map<wstring, double>::iterator it;
	if (b->type == kifTreeMapUStringFloat) {
		KifTreeMapUStringFloat* bref = (KifTreeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
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
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapUStringFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapUStringFloat* ke = new KifTreeMapUStringFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifTreeMapUStringFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	map<wstring, double>::iterator it;
	KifContainerLock _lock(this);

	if (b->type == kifTreeMapUStringFloat) {
		KifTreeMapUStringFloat* bref = (KifTreeMapUStringFloat*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = pow(it->second, bref->values[it->first]);
		}

		return ref;
	}


	double v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyFloat(it->first, add);
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

KifElement* KifTreeMapUStringFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	KifContainerLock _lock(this);
	map<wstring, double>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = (long)it->second >> bref->values[it->first];
		}

		return ref;
	}


	long v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

KifElement* KifTreeMapUStringFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapUStringFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapUStringFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapUStringFloat*)a;

	KifContainerLock _lock(this);
	map<wstring, double>::iterator it;
	if (b->type == kifTreeMapUStringInteger) {
		KifTreeMapUStringInteger* bref = (KifTreeMapUStringInteger*)b;
		KifContainerLock _breflock(bref);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second = (long)it->second << bref->values[it->first];
		}

		return ref;
	}


	long v;
	if (b->aMapContainer()) {
		bool add = true;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			v = b->ValueOnKeyInteger(it->first, add);
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

void KifTreeMapUStringFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<wstring, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseUString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}
//-----------------------------------------------------------------------
string KifIteratorTreeMapIntegerUString::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (reverse == false) {
		if (it == values->end())
			return "";
		res << it->first;
		return res.str();
	}

	if (itr == values->Rend())
		return "";

	res << itr->first;
	return res.str();
}

void KifTreeMapIntegerUString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapIntegerUString* kmap = (KifTreeMapIntegerUString*)ke;
		map<long, wstring>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		long n = itr->IteratorKeyInteger();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueUString();
	}

	itr->Release();
}

KifElement* KifTreeMapIntegerUString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	map<long, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapIntegerUString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	map<long, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

Exported void KifTreeMapIntegerUString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(STR(key));
	values[nb] = val->UString();
}

Exported void KifTreeMapIntegerUString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = conversionintegerhexa(key);
	values[nb] = val->UString();
}

Exported void KifTreeMapIntegerUString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long nb = k->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->UString();
}

wstring KifTreeMapIntegerUString::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	map<long, wstring>::iterator it;
	res << L"{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		res << it->first << L":";
		ustringing(res, it->second);
	}

	res << L"}";

	return res.str();
}


string KifTreeMapIntegerUString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<long, wstring>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":";
		ws = it->second;
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
	}

	res << "}";

	return res.str();
}

string KifTreeMapIntegerUString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<long, wstring>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	wstring ws;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":";
		ws = it->second;
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
	}

	res << "}";

	return res.str();
}

KifElement* KifTreeMapIntegerUString::Newiterator(bool rev) {
	KifIteratorTreeMapIntegerUString* iter = new KifIteratorTreeMapIntegerUString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapIntegerUString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	long s = ke->Integer();
	map<long, wstring>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}


	values.erase(itx);
}

bool KifTreeMapIntegerUString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifTreeMapIntegerUString) {
			KifTreeMapIntegerUString* kvect = (KifTreeMapIntegerUString*)ke;
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
		long key = idx->Integer();
		values[key] = ke->UString();
	}

	return true;
}

KifElement* KifTreeMapIntegerUString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapIntegerUString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<long, wstring>::iterator it;
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
			KifTreeMapIntegerUString* kmap = new KifTreeMapIntegerUString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			long vleft = key->Integer();
			long vright = keyright->Integer();
			map<long, wstring>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<long, wstring>::iterator itr = values.find(vright);
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
	long skey = key->Integer();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Provideustring(values[skey]);
}

Exported KifElement* KifTreeMapIntegerUString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<long, wstring>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, kifcode->Provideustring(it->second));
	}

	return kmap;
}

KifElement* KifTreeMapIntegerUString::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<long, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}

	return kvect;
}

Exported KifElement* KifTreeMapIntegerUString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapIntegerUString* m = (KifTreeMapIntegerUString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<long, wstring>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifTreeMapIntegerUString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapIntegerUString* ref = (KifTreeMapIntegerUString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (ref->values.find(v) == ref->values.end())
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifTreeMapIntegerUString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapIntegerUString* ref = new KifTreeMapIntegerUString(kifcode, NULL);
		KifTreeMapIntegerUString* A = (KifTreeMapIntegerUString*)a->Copy(NULL);
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (A->values.find(v) != A->values.end())
			if (A->values[v] == itr->IteratorValueUString())
				ref->values[v] = A->values[v];
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifTreeMapIntegerUString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapIntegerUString* ref = new KifTreeMapIntegerUString(kifcode, NULL);
		map<long, wstring>& val = ((KifTreeMapIntegerUString*)a)->values;
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (val.find(v) == val.end())
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();

		map<long, wstring>::iterator it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnIntegerKey(it->first) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}

		return ref;
	}

	return kifNULL;
}

KifElement* KifTreeMapIntegerUString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapIntegerUString* ref = this;
	if (autoself == false)
		ref = (KifTreeMapIntegerUString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapIntegerUString*)a;

	KifContainerLock _lock(this);

	map<long, wstring>::iterator it;
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


	if (b->type == kifTreeMapIntegerUString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifTreeMapIntegerUString* bref = (KifTreeMapIntegerUString*)b;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}

		return ref;
	}


	wstring n = b->UString();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second += n;
	return ref;
}

KifElement* KifTreeMapIntegerUString::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapIntegerUString* ke = new KifTreeMapIntegerUString(kifcode, NULL);
	ke->values = values;
	return ke;
}

void KifTreeMapIntegerUString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<long, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}

//-------------------------------------------------------------------------------------------------------------
string KifIteratorTreeMapFloatUString::IteratorKeyString() {
	KifContainerLock _lock(kbase);
	stringstream res;
	if (reverse == false) {
		if (it == values->end())
			return "";
		res << it->first;
		return res.str();
	}

	if (itr == values->Rend())
		return "";

	res << itr->first;
	return res.str();
}

void KifTreeMapFloatUString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapFloatUString* kmap = (KifTreeMapFloatUString*)ke;
		map<double, wstring>::iterator it;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (values.find(it->first) == values.end())
				values[it->first] = it->second;
		}

		return;
	}


	if (!ke->aContainer())
		return;

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		double n = itr->IteratorKeyFloat();
		if (values.find(n) == values.end())
			values[n] = itr->IteratorValueUString();
	}

	itr->Release();
}

KifElement* KifTreeMapFloatUString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	map<double, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapFloatUString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
	map<double, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

Exported void KifTreeMapFloatUString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = convertfloat(STR(key));
	KifContainerLock _lock(this);
	values[nb] = val->UString();
}

Exported void KifTreeMapFloatUString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = convertfloat(key);
	KifContainerLock _lock(this);
	values[nb] = val->UString();
}

Exported void KifTreeMapFloatUString::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->UString();
}

wstring KifTreeMapFloatUString::UString() {
	KifContainerLock _lock(this);
	std::wstringstream res;
	map<double, wstring>::iterator it;
	res << L"{";
	bool beg = true;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << L",";
		beg = false;
		res << it->first << L":";
		ustringing(res, it->second);
	}

	res << L"}";

	return res.str();
}

string KifTreeMapFloatUString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	map<double, wstring>::iterator it;
	res << "{";
	bool beg = true;
	wstring ws;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":";
		ws = it->second;
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
	}

	res << "}";

	return res.str();
}

string KifTreeMapFloatUString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	map<double, wstring>::iterator it;
	res << "{";
	bool beg = true;
	wstring ws;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":";
		ws = it->second;
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
	}

	res << "}";

	return res.str();
}

KifElement* KifTreeMapFloatUString::Newiterator(bool rev) {
	KifIteratorTreeMapFloatUString* iter = new KifIteratorTreeMapFloatUString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapFloatUString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	double s = ke->Float();
	map<double, wstring>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}


	values.erase(itx);
}

bool KifTreeMapFloatUString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifTreeMapFloatUString) {
			KifTreeMapFloatUString* kvect = (KifTreeMapFloatUString*)ke;
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

	else {
		double key = idx->Float();
		values[key] = ke->UString();
	}

	return true;
}

KifElement* KifTreeMapFloatUString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapFloatUString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			map<double, wstring>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kv->Push(kifcode->Providefloat(it->first));
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
			KifTreeMapFloatUString* kmap = new KifTreeMapFloatUString(kifcode, NULL);
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			KifElement* keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			double vleft = key->Float();
			double vright = keyright->Float();
			map<double, wstring>::iterator it = values.find(vleft);
			if (it == values.end() && key != kifNULL) {
				key->Release();
				return kmap;
			}
			if (key == kifNULL)
				it = values.begin();
			key->Release();
			map<double, wstring>::iterator itr = values.find(vright);
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
	double skey = key->Float();
	key->Release();
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	return kifcode->Provideustring(values[skey]);
}

Exported KifElement* KifTreeMapFloatUString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	map<double, wstring>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%g", it->first);
		kmap->Push(buffer, kifcode->Provideustring(it->second));
	}

	return kmap;
}

KifElement* KifTreeMapFloatUString::Vector(KifElement* contextualpattern, KifVector* v) {
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
	map<double, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}

	return kvect;
}

Exported KifElement* KifTreeMapFloatUString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapFloatUString* m = (KifTreeMapFloatUString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	map<double, wstring>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}

	return kifTRUE;
}

KifElement* KifTreeMapFloatUString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapFloatUString* ref = (KifTreeMapFloatUString*)a->Copy(NULL);
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (ref->values.find(v) == ref->values.end())
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifTreeMapFloatUString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapFloatUString* ref = new KifTreeMapFloatUString(kifcode, NULL);
		KifTreeMapFloatUString* A = (KifTreeMapFloatUString*)a->Copy(NULL);
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (A->values.find(v) != A->values.end())
			if (A->values[v] == itr->IteratorValueUString())
				ref->values[v] = A->values[v];
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifTreeMapFloatUString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapFloatUString* ref = new KifTreeMapFloatUString(kifcode, NULL);
		map<double, wstring>& val = ((KifTreeMapFloatUString*)a)->values;
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (val.find(v) == val.end())
				ref->values[v] = itr->IteratorValueUString();
		}

		itr->Release();

		map<double, wstring>::iterator it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnFloatKey(it->first) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}

		return ref;
	}

	return kifNULL;
}

KifElement* KifTreeMapFloatUString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifTreeMapFloatUString* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloatUString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloatUString*)a;

	KifContainerLock _lock(this);

	map<double, wstring>::iterator it;
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


	if (b->type == kifTreeMapFloatUString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifTreeMapFloatUString* bref = (KifTreeMapFloatUString*)b;
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end())
				it->second += bref->values[it->first];
		}

		return ref;
	}


	wstring n = b->UString();
	for (it = ref->values.begin(); it != ref->values.end(); it++)
		it->second += n;
	return ref;
}

KifElement* KifTreeMapFloatUString::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapFloatUString* ke = new KifTreeMapFloatUString(kifcode, NULL);
	ke->values = values;
	return ke;
}

void KifTreeMapFloatUString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	map<double, wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}

}


