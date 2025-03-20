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


KifElement* KifTreeMap::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifTreeMap::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifTreeMap::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifTreeMap::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifTreeMap)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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
				ke->plus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->plus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifTreeMap::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifTreeMap)
		return a->minus(a, b, context, idthread,callfunc,autoself);

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
				ke->minus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->minus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifTreeMap::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifTreeMap)
		return a->multiply(a, b, context, idthread,callfunc,autoself);

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
				ke->multiply(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->multiply(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifTreeMap::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifTreeMap)
		return a->divide(a, b, context, idthread,callfunc,autoself);

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
				ke->divide(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->divide(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifTreeMap::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifTreeMap)
		return a->mod(a, b, context, idthread,callfunc,autoself);

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
				ke->mod(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->mod(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifTreeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifTreeMap)
		return a->shiftright(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftright(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->shiftright(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifTreeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifTreeMap)
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftleft(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->shiftleft(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifTreeMap::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifTreeMap)
		return a->power(a, b, context, idthread,callfunc,autoself);

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
				ke->power(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->power(ke, b, context, idthread,callfunc,true);
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

bool KifTreeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifTreeMap::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Map::Declaration");
#endif
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifTreeMap::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifTreeMap::Methods(KifVectorString* fcts) {
#ifdef PROFILING
	profilingcall("Map::List");
#endif
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
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

Exported void KifTreeMap::Setreference(int inc) {
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
}


KifElement* KifIteratorTreeMap::Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val) {
	if (reverse == false) {
		if (it == values->end())
			return kifNULL;
		return it->second->Execute(contextualpattern, dom, val);
	}
	if (itr == values->rend())
		return kifNULL;
	return itr->second->Execute(contextualpattern, dom, val);
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
				ke = it->second->Execute(kifNULL, (KifDomain*)dom);
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifTreeMap* kmap = new KifTreeMap(kifcode, NULL);
			map<string, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kmap->push(it->first, it->second);
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom);
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
	val = val->Atom();
	val->Setreference(reference + 1);

	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}
	else
		values[key] = val;
}



//To insert a value into a map through the API 
Exported void KifTreeMap::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	val->Setreference(reference + 1);

	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}
	else
		values[key] = val;
}

Exported void KifTreeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}
	else
		values[key] = val;
	val->Setreference(reference + 1);
}

Exported void KifTreeMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	string key = k->String();
	KifContainerLock _lock(this);
	if (values.find(key) != values.end()) {
		values[key]->Resetreference(reference + 1);
		values[key] = val;
	}
	else
		values[key] = val;
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


KifElement* KifIntegerTreeMap::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifIntegerTreeMap::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifIntegerTreeMap::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifIntegerTreeMap::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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
				ke->plus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->plus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerTreeMap::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->minus(a, b, context, idthread,callfunc,autoself);

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
				ke->minus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->minus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerTreeMap::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->multiply(a, b, context, idthread,callfunc,autoself);

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
				ke->multiply(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->multiply(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerTreeMap::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->divide(a, b, context, idthread,callfunc,autoself);

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
				ke->divide(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->divide(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerTreeMap::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->mod(a, b, context, idthread,callfunc,autoself);

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
				ke->mod(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->mod(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerTreeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->shiftright(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftright(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->shiftright(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerTreeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftleft(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->shiftleft(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifIntegerTreeMap::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifIntegerTreeMap)
		return a->power(a, b, context, idthread,callfunc,autoself);

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
				ke->power(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->power(ke, b, context, idthread,callfunc,true);
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

bool KifIntegerTreeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifIntegerTreeMap::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Map::Declaration");
#endif
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifIntegerTreeMap::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifIntegerTreeMap::Methods(KifVectorString* fcts) {
#ifdef PROFILING
	profilingcall("Map::List");
#endif
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
}

KifElement* KifIntegerTreeMap::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Map::Copy");
#endif
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

Exported void KifIntegerTreeMap::Setreference(int inc) {
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

KifElement* KifIteratorIntegerTreeMap::Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val) {
	if (reverse == false) {
		if (it == values->end())
			return kifNULL;
		return it->second->Execute(contextualpattern, dom, val);
	}
	if (itr == values->rend())
		return kifNULL;
	return itr->second->Execute(contextualpattern, dom, val);
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
			ke->Setreference(reference + 1);
			values[n] = ke;
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
				ke = it->second->Execute(kifNULL, (KifDomain*)dom);
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifIntegerTreeMap* kmap = new KifIntegerTreeMap(kifcode, NULL);
			map<long, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kmap->push(it->first, it->second);
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom);
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
	val = val->Atom();
	val->Setreference(reference + 1);

	long n = s_XLConvert(STR(key));
	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else
		values[n] = val;
}

Exported void KifIntegerTreeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	KifContainerLock _lock(this);
	long n = s_XLConvert(key);
	if (values.find(n) != values.end()) {
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else
		values[n] = val;
	val->Setreference(reference + 1);
}

Exported void KifIntegerTreeMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	long n = k->Integer();
	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else
		values[n] = val;
	val->Setreference(reference + 1);
}


void KifIntegerTreeMap::push(long n, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	val->Setreference(reference + 1);

	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else
		values[n] = val;
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


KifElement* KifFloatTreeMap::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifFloatTreeMap::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifFloatTreeMap::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
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

KifElement* KifFloatTreeMap::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifFloatTreeMap)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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
				ke->plus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->plus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatTreeMap::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifFloatTreeMap)
		return a->minus(a, b, context, idthread,callfunc,autoself);

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
				ke->minus(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->minus(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatTreeMap::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifFloatTreeMap)
		return a->multiply(a, b, context, idthread,callfunc,autoself);

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
				ke->multiply(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->multiply(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatTreeMap::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifFloatTreeMap)
		return a->divide(a, b, context, idthread,callfunc,autoself);

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
				ke->divide(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->divide(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatTreeMap::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifFloatTreeMap)
		return a->mod(a, b, context, idthread,callfunc,autoself);

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
				ke->mod(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->mod(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatTreeMap::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifFloatTreeMap)
		return a->shiftright(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftright(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->shiftright(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatTreeMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifFloatTreeMap)
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);

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
				ke->shiftleft(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->shiftleft(ke, b, context, idthread,callfunc,true);
	}
	return ref;
}

KifElement* KifFloatTreeMap::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifFloatTreeMap)
		return a->power(a, b, context, idthread,callfunc,autoself);

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
				ke->power(ke, kref, context, idthread,callfunc,true);
			}
		}
		return ref;
	}
	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = it->second;
		ke->power(ke, b, context, idthread,callfunc,true);
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

bool KifFloatTreeMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifFloatTreeMap::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Map::Declaration");
#endif
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifFloatTreeMap::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifFloatTreeMap::Methods(KifVectorString* fcts) {
#ifdef PROFILING
	profilingcall("Map::List");
#endif
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
}

KifElement* KifFloatTreeMap::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Map::Copy");
#endif
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

Exported void KifFloatTreeMap::Setreference(int inc) {
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

KifElement* KifIteratorFloatTreeMap::Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val) {
	if (reverse == false) {
		if (it == values->end())
			return kifNULL;
		return it->second->Execute(contextualpattern, dom, val);
	}
	if (itr == values->rend())
		return kifNULL;
	return itr->second->Execute(contextualpattern, dom, val);
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
			ke->Setreference(reference + 1);
			values[n] = ke;
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
				ke = it->second->Execute(kifNULL, (KifDomain*)dom);
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifFloatTreeMap* kmap = new KifFloatTreeMap(kifcode, NULL);
			map<double, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kmap->push(it->first, it->second);
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom);
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
	val = val->Atom();
	val->Setreference(reference + 1);

	double n = convertfloat(STR(key));
	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else
		values[n] = val;
}

Exported void KifFloatTreeMap::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	KifContainerLock _lock(this);
	double n = convertfloat(key);
	if (values.find(n) != values.end()) {
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else
		values[n] = val;
	val->Setreference(reference + 1);
}

Exported void KifFloatTreeMap::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	double n = key->Float();
	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else
		values[n] = val;
	val->Setreference(reference + 1);
}


void KifFloatTreeMap::push(double n, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	val = val->Atom();
	val->Setreference(reference + 1);

	KifContainerLock _lock(this);
	if (values.find(n) != values.end()) {
		values[n]->Resetreference(reference + 1);
		values[n] = val;
	}
	else
		values[n] = val;
}

//----------------------------------------------------------------------------------
Exported KifElement* KifTreeMapString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifTreeMapString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapString* kmap = (KifTreeMapString*)ke;
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

KifElement* KifTreeMapString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


Exported void KifTreeMapString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->String();
}

Exported void KifTreeMapString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->String();
}

Exported void KifTreeMapString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->String();
}

string KifTreeMapString::String() {
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

string KifTreeMapString::JSonString() {
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

KifElement* KifTreeMapString::Newiterator(bool rev) {
	KifIteratorTreeMapString* iter = new KifIteratorTreeMapString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapString::Pop(KifElement* ke) {
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

bool KifTreeMapString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapString) {
			KifTreeMapString* kvect = (KifTreeMapString*)ke;
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

KifElement* KifTreeMapString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifTreeMapString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifTreeMapString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifTreeMapString* kmap = new KifTreeMapString(kifcode, NULL);
			kmap->values = values;
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom, kifNULL);
	}
	else
		key = idx->Execute(contextualpattern, dom, kifNULL);

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

Exported KifElement* KifTreeMapString::Map(KifElement* contextualpattern, KifMap* m) {
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

KifElement* KifTreeMapString::Vector(KifElement* contextualpattern, KifVector* v) {
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

Exported KifElement* KifTreeMapString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapString* m = (KifTreeMapString*)a;
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

KifElement* KifTreeMapString::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapString* ref = (KifTreeMapString*)a->Copy(NULL);
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

KifElement* KifTreeMapString::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapString* ref = new KifTreeMapString(kifcode, NULL);
		KifTreeMapString* A = (KifTreeMapString*)a->Copy(NULL);
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

KifElement* KifTreeMapString::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifTreeMapString* ref = new KifTreeMapString(kifcode, NULL);
		map<string, string>& val = ((KifTreeMapString*)a)->values;
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

KifElement* KifTreeMapString::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapString* ref = this;
	if (autoself == false)
		ref = (KifTreeMapString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapString*)a;

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

	if (b->type == kifTreeMapString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifTreeMapString* bref = (KifTreeMapString*)b;
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


KifElement* KifTreeMapString::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapString* ke = new KifTreeMapString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//----------------------------------------------------------------------------------
Exported KifElement* KifTreeMapInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

KifElement* KifTreeMapInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	map<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

void KifTreeMapInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapInteger* kmap = (KifTreeMapInteger*)ke;
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

Exported void KifTreeMapInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}


Exported void KifTreeMapInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}

Exported void KifTreeMapInteger::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}



string KifTreeMapInteger::String() {
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

string KifTreeMapInteger::JSonString() {
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


KifElement* KifTreeMapInteger::Newiterator(bool rev) {
	KifIteratorTreeMapInteger* iter = new KifIteratorTreeMapInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}


void KifTreeMapInteger::Pop(KifElement* ke) {
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

bool KifTreeMapInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapInteger) {
			KifTreeMapInteger* kvect = (KifTreeMapInteger*)ke;
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


KifElement* KifTreeMapInteger::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifTreeMapInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifTreeMapInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifTreeMapInteger* kmap = new KifTreeMapInteger(kifcode, NULL);
			kmap->values = values;
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom, kifNULL);
	}
	else
		key = idx->Execute(contextualpattern, dom, kifNULL);

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

Exported KifElement* KifTreeMapInteger::Map(KifElement* contextualpattern, KifMap* m) {
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

KifElement* KifTreeMapInteger::Vector(KifElement* contextualpattern, KifVector* v) {
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

Exported KifElement* KifTreeMapInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapInteger* m = (KifTreeMapInteger*)a;
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

KifElement* KifTreeMapInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapInteger* ke = new KifTreeMapInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifTreeMapInteger::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapInteger::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapInteger::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapInteger::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapInteger::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapInteger::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapInteger::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	long v;
	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapInteger::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapInteger::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapInteger* ref = this;
	if (autoself == false)
		ref = (KifTreeMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapInteger*)a;

	KifContainerLock _lock(this);
	map<string, long>::iterator it;
	long v;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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
Exported KifElement* KifTreeMapFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifTreeMapFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifTreeMapFloat* kmap = (KifTreeMapFloat*)ke;
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

KifElement* KifTreeMapFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	map<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifTreeMapFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	map<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


Exported void KifTreeMapFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifTreeMapFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifTreeMapFloat::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->Float();
}


string KifTreeMapFloat::String() {
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

string KifTreeMapFloat::JSonString() {
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

KifElement* KifTreeMapFloat::Newiterator(bool rev) {
	KifIteratorTreeMapFloat* iter = new KifIteratorTreeMapFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifTreeMapFloat::Pop(KifElement* ke) {
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

bool KifTreeMapFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifTreeMapFloat) {
			KifTreeMapFloat* kvect = (KifTreeMapFloat*)ke;
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

KifElement* KifTreeMapFloat::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifTreeMapFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifTreeMapFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifTreeMapFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifTreeMapFloat* kmap = new KifTreeMapFloat(kifcode, NULL);
			kmap->values = values;
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom, kifNULL);
	}
	else
		key = idx->Execute(contextualpattern, dom, kifNULL);

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

Exported KifElement* KifTreeMapFloat::Map(KifElement* contextualpattern, KifMap* m) {
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

KifElement* KifTreeMapFloat::Vector(KifElement* contextualpattern, KifVector* v) {
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

Exported KifElement* KifTreeMapFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifTreeMapFloat* m = (KifTreeMapFloat*)a;
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

KifElement* KifTreeMapFloat::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapFloat::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapFloat::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapFloat::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	map<string, double>::iterator it;
	if (b->type == kifTreeMapFloat) {
		KifTreeMapFloat* bref = (KifTreeMapFloat*)b;
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

KifElement* KifTreeMapFloat::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	map<string, double>::iterator it;
	if (b->type == kifTreeMapFloat) {
		KifTreeMapFloat* bref = (KifTreeMapFloat*)b;
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

KifElement* KifTreeMapFloat::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	map<string, double>::iterator it;
	if (b->type == kifTreeMapFloat) {
		KifTreeMapFloat* bref = (KifTreeMapFloat*)b;
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

KifElement* KifTreeMapFloat::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	double v;
	map<string, double>::iterator it;
	if (b->type == kifTreeMapFloat) {
		KifTreeMapFloat* bref = (KifTreeMapFloat*)b;
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

KifElement* KifTreeMapFloat::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	long v;
	map<string, double>::iterator it;
	if (b->type == kifTreeMapFloat) {
		KifTreeMapFloat* bref = (KifTreeMapFloat*)b;
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

KifElement* KifTreeMapFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifTreeMapFloat* ke = new KifTreeMapFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifTreeMapFloat::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	map<string, double>::iterator it;
	KifContainerLock _lock(this);

	if (b->type == kifTreeMapFloat) {
		KifTreeMapFloat* bref = (KifTreeMapFloat*)b;
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

KifElement* KifTreeMapFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

KifElement* KifTreeMapFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifTreeMapFloat* ref = this;
	if (autoself == false)
		ref = (KifTreeMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTreeMapFloat*)a;

	KifContainerLock _lock(this);
	map<string, double>::iterator it;
	if (b->type == kifTreeMapInteger) {
		KifTreeMapInteger* bref = (KifTreeMapInteger*)b;
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

Exported KifElement* KifTreeMapIntegerInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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
	long nb = s_XLConvert(key);
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
	long nb = s_XLConvert(STR(key));
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

bool KifTreeMapIntegerInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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
			nb = s_XLConvert(STR(it->first));
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

KifElement* KifTreeMapIntegerInteger::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifTreeMapIntegerInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifTreeMapIntegerInteger* kmap = new KifTreeMapIntegerInteger(kifcode, NULL);
			kmap->values = values;
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom, kifNULL);
	}
	else
		key = idx->Execute(contextualpattern, dom, kifNULL);

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

KifElement* KifTreeMapIntegerInteger::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapIntegerInteger::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapIntegerInteger::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapIntegerInteger::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerInteger::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerInteger::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerInteger::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerInteger::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerInteger::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

Exported KifElement* KifTreeMapIntegerString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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
	long nb = s_XLConvert(STR(key));
	values[nb] = val->String();
}

Exported void KifTreeMapIntegerString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = s_XLConvert(key);
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

bool KifTreeMapIntegerString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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
			nb = s_XLConvert(STR(it->first));
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

KifElement* KifTreeMapIntegerString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifTreeMapIntegerString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifTreeMapIntegerString* kmap = new KifTreeMapIntegerString(kifcode, NULL);
			kmap->values = values;
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom, kifNULL);
	}
	else
		key = idx->Execute(contextualpattern, dom, kifNULL);

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

KifElement* KifTreeMapIntegerString::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapIntegerString::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapIntegerString::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapIntegerString::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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


Exported KifElement* KifTreeMapIntegerFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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
	long nb = s_XLConvert(key);
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
	long nb = s_XLConvert(STR(key));
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

bool KifTreeMapIntegerFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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
			nb = s_XLConvert(STR(it->first));
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

KifElement* KifTreeMapIntegerFloat::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifTreeMapIntegerFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifTreeMapIntegerFloat* kmap = new KifTreeMapIntegerFloat(kifcode, NULL);
			kmap->values = values;
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom, kifNULL);
	}
	else
		key = idx->Execute(contextualpattern, dom, kifNULL);

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

KifElement* KifTreeMapIntegerFloat::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapIntegerFloat::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapIntegerFloat::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapIntegerFloat::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerFloat::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerFloat::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerFloat::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerFloat::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerFloat::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapIntegerFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

Exported KifElement* KifTreeMapFloatInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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

bool KifTreeMapFloatInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifTreeMapFloatInteger::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifTreeMapFloatInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifTreeMapFloatInteger* kmap = new KifTreeMapFloatInteger(kifcode, NULL);
			kmap->values = values;
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom, kifNULL);
	}
	else
		key = idx->Execute(contextualpattern, dom, kifNULL);

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

KifElement* KifTreeMapFloatInteger::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapFloatInteger::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapFloatInteger::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapFloatInteger::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatInteger::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatInteger::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatInteger::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatInteger::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatInteger::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

Exported KifElement* KifTreeMapFloatString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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

bool KifTreeMapFloatString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifTreeMapFloatString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifTreeMapFloatString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifTreeMapFloatString* kmap = new KifTreeMapFloatString(kifcode, NULL);
			kmap->values = values;
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom, kifNULL);
	}
	else
		key = idx->Execute(contextualpattern, dom, kifNULL);

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

KifElement* KifTreeMapFloatString::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapFloatString::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapFloatString::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapFloatString::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

Exported KifElement* KifTreeMapFloatFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
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

bool KifTreeMapFloatFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifTreeMapFloatFloat::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifTreeMapFloatFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifTreeMapFloatFloat* kmap = new KifTreeMapFloatFloat(kifcode, NULL);
			kmap->values = values;
			return kmap;
		}
		key = kind->key->Execute(kifNULL, dom, kifNULL);
	}
	else
		key = idx->Execute(contextualpattern, dom, kifNULL);

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

KifElement* KifTreeMapFloatFloat::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapFloatFloat::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapFloatFloat::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

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

KifElement* KifTreeMapFloatFloat::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatFloat::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatFloat::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatFloat::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatFloat::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatFloat::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifTreeMapFloatFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

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

KifElement* KifCode::KifParseVarMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMap* kv;
	if (kifGlobals->evaluation) {
		kv = Providemap();
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

KifElement* KifCode::KifParseVarTreeMapString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapString* kv = new KifTreeMapString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapInteger* kv = new KifTreeMapInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTreeMapFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTreeMapFloat* kv = new KifTreeMapFloat(this, var);
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

void KifTreeMapString::storeInLoop(KifElement** alls) {
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

void KifTreeMapInteger::storeInLoop(KifElement** alls) {
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

void KifTreeMapFloat::storeInLoop(KifElement** alls) {
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






