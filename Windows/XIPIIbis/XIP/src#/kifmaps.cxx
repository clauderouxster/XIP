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
filename   : kif.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language
Programmer : Claude ROUX
Reviewer   :
*/

//#define PROFILING

#include "kif.h"
#include "kifinit.h"
#include "kifcontainer.h"
#include "kifmaps.h"
#include "kiftreemaps.h"
#include "fractalhmap.h"
#include "kifpmaps.h"

extern const char* kifErrorStrings[];
bool TestkifFunctions(short n, hmap<short, bool>& kiffunc);
//---------------------------------------------------------------------------------------------------------

KifMap* Selectmap(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->type == kifMap && context->Evaluate()) {
		context->Clear();
		return (KifMap*)context;
	}

	return new KifMap(kref->kifcode, NULL);
}

KifElement* Selectamap(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->aMapContainer() && context->Evaluate()) {
		context->Clear();
		return context;
	}

	return new KifMap(kref->kifcode, NULL);
}


KifElement* Selectmapstring(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->aMapContainer() && context->Evaluate()) {
		context->Clear();
		return context;
	}

	return new KifMapString(kref->kifcode, NULL);
}

KifElement* Selectmapinteger(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->aMapContainer() && context->Evaluate()) {
		context->Clear();
		return context;
	}

	return new KifMapInteger(kref->kifcode, NULL);
}

KifElement* Selectmapfloat(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->aMapContainer() && context->Evaluate()) {
		context->Clear();
		return context;
	}

	return new KifMapFloat(kref->kifcode, NULL);
}

static inline bool localevaluate(KifType typecontext, KifType thetype) {
	if (thetype == kifMap) {
		if (typecontext >= kifMap && typecontext <= kifPrimeMapFloatFloat)
			return true;
	}
	else {
		if (thetype == kifIntegerMap) {
			if (typecontext >= kifIntegerMap && typecontext <= kifMapIntegerFloat)
				return true;
			if (typecontext >= kifIntegerTreeMap && typecontext <= kifTreeMapIntegerFloat)
				return true;
			if (typecontext >= kifIntegerPrimeMap && typecontext <= kifPrimeMapIntegerFloat)
				return true;
		}
		else {
			if (typecontext >= kifFloatMap && typecontext <= kifMapFloatFloat)
				return true;
			if (typecontext >= kifFloatTreeMap && typecontext <= kifTreeMapFloatFloat)
				return true;
			if (typecontext >= kifFloatPrimeMap && typecontext <= kifPrimeMapFloatFloat)
				return true;
		}
	}
	return false;
}

KifElement* Selectmapaccordingly(KifCode* kifcode, KifElement* context, KifType thetype) {
	if (context != NULL && context->reference <= 1 && localevaluate(context->type, thetype)) {
		context->Clear();
		return context;
	}

	KifType ltype = kifVOID;

	if (context != NULL) {
		if (context->type == kifCouple)
			ltype = ((KifCouple*)context)->recipient->type;
		else
			ltype = context->type;
	}

	if (thetype == kifMap) {
		if (ltype < kifTreeMap)
			return new KifMap(kifcode, NULL);
		if (ltype < kifPrimeMap)
			return new KifTreeMap(kifcode, NULL);
		return new KifPrimeMap(kifcode, NULL);
	}


	if (thetype == kifIntegerMap) {
		if (ltype < kifTreeMap)
			return new KifIntegerMap(kifcode, NULL);
		if (ltype < kifPrimeMap)
			return new KifIntegerTreeMap(kifcode, NULL);
		return new KifIntegerPrimeMap(kifcode, NULL);
	}

	if (thetype == -1 && context != NULL)
		return context->Copy(NULL);

	if (ltype < kifTreeMap)
		return new KifFloatMap(kifcode, NULL);
	if (ltype < kifPrimeMap)
		return new KifFloatTreeMap(kifcode, NULL);
	return new KifFloatPrimeMap(kifcode, NULL);
}


//---------------------------------------------------------------------------------------------------------
KifElement* KifCode::KifParseVarIntegerMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifIntegerMap* kv = new KifIntegerMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarFloatMap(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifFloatMap* kv = new KifFloatMap(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarMapString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMapString* kv = new KifMapString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarMapInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMapInteger* kv = new KifMapInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarMapFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMapFloat* kv = new KifMapFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarMapIntegerString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMapIntegerString* kv = new KifMapIntegerString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarMapIntegerInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMapIntegerInteger* kv = new KifMapIntegerInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarMapIntegerFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMapIntegerFloat* kv = new KifMapIntegerFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarMapFloatString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMapFloatString* kv = new KifMapFloatString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarMapFloatInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMapFloatInteger* kv = new KifMapFloatInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarMapFloatFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifMapFloatFloat* kv = new KifMapFloatFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror("KIF(881): Error, wrong function arguments");

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

//----------------------------------------------------------------------------------
Exported void KifBaseMap::Setreference(int inc) {
	ThreadLock _lock(kifBaseMap);
	for (int i = 0; i < keys.size(); i++) {
		keys[i]->Putpopped(0);
		values[i]->Putpopped(0);
		keys[i]->Setreference(inc);
		values[i]->Setreference(inc);
	}
	reference += inc;
}

Exported void KifBaseMap::Resetreference(int inc) {
	{
		ThreadLock _lock(kifBaseMap);
		for (int i = 0; i < keys.size(); i++) {
			keys[i]->Resetpopped();
			values[i]->Resetpopped();
			keys[i]->Resetreference(inc);
			values[i]->Resetreference(inc);
		}
	}
	KifElement::Resetreference(inc);
}

void KifBaseMap::Release() {
	if (canbereleased) {
		if (reference == 0) {
			for (int i = 0; i < keys.size(); i++) {
				keys[i]->Release();
				values[i]->Release();
			}
			KifElement::Resetreference();
		}
	}
}

Exported KifElement* KifBaseMap::Atom(bool forced) {
	if (popped == staticval)
		return Copy(NULL);
	else
	if (forced == false)
		return this;

	KifBaseMap* base = new KifBaseMap(kifcode, NULL, false);
	ThreadLock _lock(kifBaseMap);
	for (int i = 0; i < keys.size(); i++) {
		base->keys.push_back(keys[i]->Atom(true));
		base->values.push_back(values[i]->Atom(true));
	}
	return base;
}

Exported KifElement* KifBaseMap::Map(KifElement* dom, KifMap* m) {
	int idthread = kifGlobals->GetThreadid();
	KifCallFunction* callfunc = (KifCallFunction*)kifGlobals->Topid(idthread);
	if (m == NULL)
		return Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
	return Exec(m, (KifDomain*)dom, kifNULL, idthread, callfunc);
}

Exported KifElement* KifBaseMap::Vector(KifElement* dom, KifVector* v) {
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();
	KifElement* kval;
	kvect->Reserve(keys.size());
	int idthread = kifGlobals->GetThreadid();
	KifCallFunction* callfunc = (KifCallFunction*)kifGlobals->Topid(idthread);
	for (int i = 0; i < keys.size(); i++) {
		kval = keys[i]->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
		if (Errorid(idthread)) {
			kvect->Release();
			return Errorptrid(idthread);
		}
		kvect->Push(kval);
	}
	return kvect;
}

Exported KifElement* KifBaseMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	vector<KifElement*> clefs;
	typetocreate = kifMap;
	KifElement* kval;
	for (int i = 0; i < keys.size(); i++) {
		kval = keys[i]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		clefs.push_back(kval);
		if (typetocreate != kifMap) {
			if (kval->type == kifFloat)
				typetocreate = kifFloatMap;
			else
			if (kval->aNumber()) {
				if (typetocreate != kifFloatMap)
					typetocreate = kifIntegerMap;
			}
			else
				typetocreate = kifMap;
		}
	}

	KifElement* kres = Selectmapaccordingly(kifcode, contextualpattern, (KifType)typetocreate);
	KifConst kconst(NULL, NULL, 0);
	kconst.kifcode = kifcode;
	kconst.type = kres->type;
	kconst.reference = 3000;

	for (int i = 0; i < clefs.size(); i++) {
		kval = values[i]->Exec(&kconst, dom, kifNULL, idthread, callfunc);
		kres->Push(clefs[i], kval);
		clefs[i]->Release();
	}
	return kres;
}

//----------------------------------------------------------------------------------
string KifMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<string, KifElement*>::iterator it;
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

string KifMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<string, KifElement*>::iterator it;
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

string KifMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<string, KifElement*>::iterator it;
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

KifElement* KifMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	hmap<string, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	hmap<string, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->Push(it->second);
	return kvect;
}

KifElement* KifMap::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifMap)
		return kifNULL;

	KifMap* A = (KifMap*)a;
	hmap<string, KifElement*>::iterator it;

	KifMap* ref = new KifMap(kifcode, NULL);
	KifMap* bref = (KifMap*)b;
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

KifElement* KifMap::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifMap)
		return kifNULL;

	KifMap* A = (KifMap*)a;
	hmap<string, KifElement*>::iterator it;

	KifMap* ref = this;
	if (autoself == false)
		ref = (KifMap*)a->Copy(NULL);

	KifMap* bref = (KifMap*)b;
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

KifElement* KifMap::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifMap)
		return kifNULL;

	KifMap* A = (KifMap*)a;
	hmap<string, KifElement*>::iterator it;
	hmap<string, KifElement*>::iterator itres;

	KifMap* ref = new KifMap(kifcode, NULL);
	KifMap* bref = (KifMap*)b;
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

Exported KifElement* KifMap::same(KifElement* a) {

	if (a->type != kifMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifMap* m = (KifMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<string, KifElement*>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second->same(values[it->first]) == kifFALSE)
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifMap::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifMap)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMap* ref = this;
	if (autoself == false)
		ref = (KifMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMap*)a;

	hmap<string, KifElement*>::iterator it;
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

KifElement* KifMap::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifMap)
		return a->minus(a, b, context, idthread,callfunc,autoself);

	KifMap* ref = this;
	if (autoself == false)
		ref = (KifMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMap*)a;

	hmap<string, KifElement*>::iterator it;
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

KifElement* KifMap::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifMap)
		return a->multiply(a, b, context, idthread,callfunc,autoself);

	KifMap* ref = this;
	if (autoself == false)
		ref = (KifMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMap*)a;

	hmap<string, KifElement*>::iterator it;
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

KifElement* KifMap::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifMap)
		return a->divide(a, b, context, idthread,callfunc,autoself);

	KifMap* ref = this;
	if (autoself == false)
		ref = (KifMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMap*)a;

	hmap<string, KifElement*>::iterator it;
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

KifElement* KifMap::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifMap)
		return a->mod(a, b, context, idthread,callfunc,autoself);

	KifMap* ref = this;
	if (autoself == false)
		ref = (KifMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMap*)a;

	hmap<string, KifElement*>::iterator it;
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

KifElement* KifMap::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifMap)
		return a->shiftright(a, b, context, idthread,callfunc,autoself);

	KifMap* ref = this;
	if (autoself == false)
		ref = (KifMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMap*)a;

	hmap<string, KifElement*>::iterator it;
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

KifElement* KifMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifMap)
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);

	KifMap* ref = this;
	if (autoself == false)
		ref = (KifMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMap*)a;

	hmap<string, KifElement*>::iterator it;
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

KifElement* KifMap::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifMap)
		return a->power(a, b, context, idthread,callfunc,autoself);

	KifMap* ref = this;
	if (autoself == false)
		ref = (KifMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMap*)a;

	hmap<string, KifElement*>::iterator it;
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

KifElement* KifMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorMap* iter = new KifIteratorMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

		ke = ke->Map(dom, this);
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

KifElement* KifMap::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<string, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

KifElement* KifMap::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Map::Declaration");
#endif
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifMap::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifMap::Methods(KifVectorString* fcts) {
#ifdef PROFILING
	profilingcall("Map::List");
#endif
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
}

KifElement* KifMap::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Map::Copy");
#endif
	KifMap* ke = new KifMap(kifcode, NULL);
	hmap<string, KifElement*>::iterator itx;
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

KifElement* KifMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

Exported void KifMap::Setreference(int inc) {
	reference += inc;
	Putpopped(0);
	hmap<string, KifElement*>::iterator itx;
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


KifElement* KifIteratorMap::Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val) {
#ifdef PROFILING
	profilingcall("IteratorMap::Execute");
#endif
	if (reverse == false) {
		if (it == values->end())
			return kifNULL;
		return it->second->Execute(contextualpattern, dom, val);
	}
	if (itr == values->Rend())
		return kifNULL;
	return itr->second->Execute(contextualpattern, dom, val);
}



static void resetMap(KifMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	hmap<string, KifElement*>& values = kmap->values;
	if (values.size() == 0)
		return;

	hmap<string, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}

void KifMap::Resetreference(int inc) {
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

void KifMap::Clear() {
	KifContainerLock _lock(this);
	hmap<string, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(reference + 1);
	values.clear();
}

void KifMap::Merge(KifElement* ke) {
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

KifElement* KifMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		KifElement* ke;
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			hmap<string, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				vect->Push(kifcode->Providestringraw(it->first));
			return vect;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifMap* kmap = new KifMap(kifcode, NULL);
			hmap<string, KifElement*>::iterator it;
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifMap* kmap = new KifMap(kifcode, NULL);
			hmap<string, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kmap->push(it->first, it->second);
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

void KifMap::Pop(KifElement* ke) {
	string s = ke->String();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return;

		hmap<string, KifElement*>::iterator itx = values.find(s);

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

Exported void KifMap::push(string key, KifElement* val) {
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
Exported void KifMap::Push(string& key, KifElement* val) {
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

Exported void KifMap::Push(char* key, KifElement* val) {
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

Exported void KifMap::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	string key = k->String();
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


//----------------------------------------------------------------------------------
Exported KifElement* KifMapString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifMapString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifMapString* kmap = (KifMapString*)ke;
		hmap<string, string>::iterator it;
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

KifElement* KifMapString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	hmap<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifMapString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	hmap<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


Exported void KifMapString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->String();
}


Exported void KifMapString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->String();
}

Exported void KifMapString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->String();
}

string KifMapString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<string, string>::iterator it;
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

string KifMapString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<string, string>::iterator it;
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

KifElement* KifMapString::Newiterator(bool rev) {
	KifIteratorMapString* iter = new KifIteratorMapString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifMapString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	string s = ke->String();
	hmap<string, string>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifMapString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifMapString) {
			KifMapString* kvect = (KifMapString*)ke;
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

KifElement* KifMapString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifMapString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifMapString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifMapString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hmap<string, string>::iterator it;
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
			KifMapString* kmap = new KifMapString(kifcode, NULL);
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

	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providestringraw(values[skey]);
}

Exported KifElement* KifMapString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hmap<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kmap->push(it->first, (KifElement*)kifcode->Providestring(it->second));
	return kmap;
}

KifElement* KifMapString::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifMapString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifMapString* m = (KifMapString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<string, string>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifMapString::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifMapString* ref = (KifMapString*)a->Copy(NULL);
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

KifElement* KifMapString::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifMapString* ref = new KifMapString(kifcode, NULL);
		KifMapString* A = (KifMapString*)a->Copy(NULL);
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

KifElement* KifMapString::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifMapString* ref = new KifMapString(kifcode, NULL);
		hmap<string, string>& val = ((KifMapString*)a)->values;
		string v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyString();
			if (val.find(v) == val.end())
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();

		hmap<string, string>::iterator it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnKey(it->first) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifMapString::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapString* ref = this;
	if (autoself == false)
		ref = (KifMapString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapString*)a;

	KifContainerLock _lock(this);

	hmap<string, string>::iterator it;
	if (b->type == kifList) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifList* bref = (KifList*)b;
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
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			if (bref->values.find(it->first) != bref->values.end()) {
				it->second += bref->values[it->first]->String();
			}
		}
		return ref;
	}

	if (b->type == kifMapString) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifMapString* bref = (KifMapString*)b;
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


KifElement* KifMapString::Copy(KifDomain* kp, KifElement* dom) {
	KifMapString* ke = new KifMapString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//----------------------------------------------------------------------------------
Exported KifElement* KifMapInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

KifElement* KifMapInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	hmap<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifMapInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hmap<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

void KifMapInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifMapInteger* kmap = (KifMapInteger*)ke;
		hmap<string, long>::iterator it;
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

Exported void KifMapInteger::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}


Exported void KifMapInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}


Exported void KifMapInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Integer();
}


string KifMapInteger::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<string, long>::iterator it;
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

string KifMapInteger::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<string, long>::iterator it;
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


KifElement* KifMapInteger::Newiterator(bool rev) {
	KifIteratorMapInteger* iter = new KifIteratorMapInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}


void KifMapInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	string s = ke->String();
	hmap<string, long>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifMapInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifMapInteger) {
			KifMapInteger* kvect = (KifMapInteger*)ke;
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


KifElement* KifMapInteger::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifMapInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifMapInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifMapInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hmap<string, long>::iterator it;
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
			KifMapInteger* kmap = new KifMapInteger(kifcode, NULL);
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

	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Provideinteger(values[skey]);
}

Exported KifElement* KifMapInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hmap<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kmap->push(it->first, (KifElement*)kifcode->Provideinteger(it->second));
	return kmap;
}

KifElement* KifMapInteger::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifMapInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifMapInteger* m = (KifMapInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<string, long>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifMapInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifMapInteger* ke = new KifMapInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifMapInteger::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapInteger::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapInteger::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapInteger::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapInteger::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapInteger::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapInteger::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	long v;
	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapInteger::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapInteger::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapInteger* ref = this;
	if (autoself == false)
		ref = (KifMapInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapInteger*)a;

	KifContainerLock _lock(this);
	hmap<string, long>::iterator it;
	long v;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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
Exported KifElement* KifMapFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifMapFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifMapFloat* kmap = (KifMapFloat*)ke;
		hmap<string, double>::iterator it;
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

KifElement* KifMapFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	hmap<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifMapFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	hmap<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


Exported void KifMapFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifMapFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values[key] = val->Float();
}

Exported void KifMapFloat::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	string key = k->String();
	KifContainerLock _lock(this);
	values[key] = val->Float();
}



string KifMapFloat::String() {
	KifContainerLock _lock(this);
	hmap<string, double>::iterator it;
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

string KifMapFloat::JSonString() {
	KifContainerLock _lock(this);
	hmap<string, double>::iterator it;
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

KifElement* KifMapFloat::Newiterator(bool rev) {
	KifIteratorMapFloat* iter = new KifIteratorMapFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifMapFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	string s = ke->String();
	hmap<string, double>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifMapFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifMapFloat) {
			KifMapFloat* kvect = (KifMapFloat*)ke;
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

KifElement* KifMapFloat::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifMapFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifMapFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifMapFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hmap<string, double>::iterator it;
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
			KifMapFloat* kmap = new KifMapFloat(kifcode, NULL);
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
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providefloat(values[skey]);
}

Exported KifElement* KifMapFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hmap<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kmap->push(it->first, (KifElement*)kifcode->Providefloat(it->second));
	return kmap;
}

KifElement* KifMapFloat::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifMapFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifMapFloat* m = (KifMapFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<string, double>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifMapFloat::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	KifContainerLock _lock(this);
	hmap<string, double>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapFloat::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	KifContainerLock _lock(this);
	hmap<string, double>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapFloat::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	KifContainerLock _lock(this);
	hmap<string, double>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapFloat::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	hmap<string, double>::iterator it;
	if (b->type == kifMapFloat) {
		KifMapFloat* bref = (KifMapFloat*)b;
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

KifElement* KifMapFloat::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	hmap<string, double>::iterator it;
	if (b->type == kifMapFloat) {
		KifMapFloat* bref = (KifMapFloat*)b;
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

KifElement* KifMapFloat::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	hmap<string, double>::iterator it;
	if (b->type == kifMapFloat) {
		KifMapFloat* bref = (KifMapFloat*)b;
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

KifElement* KifMapFloat::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	double v;
	hmap<string, double>::iterator it;
	if (b->type == kifMapFloat) {
		KifMapFloat* bref = (KifMapFloat*)b;
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

KifElement* KifMapFloat::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	long v;
	hmap<string, double>::iterator it;
	if (b->type == kifMapFloat) {
		KifMapFloat* bref = (KifMapFloat*)b;
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

KifElement* KifMapFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifMapFloat* ke = new KifMapFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifMapFloat::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	hmap<string, double>::iterator it;
	KifContainerLock _lock(this);

	if (b->type == kifMapFloat) {
		KifMapFloat* bref = (KifMapFloat*)b;
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

KifElement* KifMapFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	KifContainerLock _lock(this);
	hmap<string, double>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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

KifElement* KifMapFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloat*)a;

	KifContainerLock _lock(this);
	hmap<string, double>::iterator it;
	if (b->type == kifMapInteger) {
		KifMapInteger* bref = (KifMapInteger*)b;
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
string KifIteratorMapIntegerInteger::IteratorKeyString() {
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

Exported KifElement* KifMapIntegerInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

KifElement* KifMapIntegerInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hmap<long, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifMapIntegerInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hmap<long, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


void KifMapIntegerInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifMapIntegerInteger* kmap = (KifMapIntegerInteger*)ke;
		hmap<long, long>::iterator it;
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

Exported void KifMapIntegerInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = s_XLConvert(key);
	values[nb] = val->Integer();
}

Exported void KifMapIntegerInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = s_XLConvert(STR(key));
	values[nb] = val->Integer();
}

Exported void KifMapIntegerInteger::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;

	long nb = k->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->Integer();
}


string KifMapIntegerInteger::String() {
	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
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

KifElement* KifMapIntegerInteger::Newiterator(bool rev) {
	KifIteratorMapIntegerInteger* iter = new KifIteratorMapIntegerInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifMapIntegerInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	long s = ke->Integer();
	hmap<long, long>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifMapIntegerInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifMapIntegerInteger) {
			KifMapIntegerInteger* kvect = (KifMapIntegerInteger*)ke;
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

KifElement* KifMapIntegerInteger::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifMapIntegerInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifMapIntegerInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifMapIntegerInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hmap<long, long>::iterator it;
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
			KifMapIntegerInteger* kmap = new KifMapIntegerInteger(kifcode, NULL);
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
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Provideinteger(values[skey]);
}

Exported KifElement* KifMapIntegerInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hmap<long, long>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Provideinteger(it->second));
	}
	return kmap;
}

KifElement* KifMapIntegerInteger::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<long, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifMapIntegerInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifMapIntegerInteger* m = (KifMapIntegerInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<long, long>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifMapIntegerInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifMapIntegerInteger* ke = new KifMapIntegerInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifMapIntegerInteger::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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

KifElement* KifMapIntegerInteger::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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

KifElement* KifMapIntegerInteger::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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

KifElement* KifMapIntegerInteger::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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

KifElement* KifMapIntegerInteger::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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

KifElement* KifMapIntegerInteger::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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

KifElement* KifMapIntegerInteger::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	long v;
	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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

KifElement* KifMapIntegerInteger::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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

KifElement* KifMapIntegerInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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

KifElement* KifMapIntegerInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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

KifElement* KifMapIntegerInteger::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerInteger* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerInteger*)a;

	KifContainerLock _lock(this);
	hmap<long, long>::iterator it;
	long v;
	if (b->type == kifMapIntegerInteger) {
		KifMapIntegerInteger* bref = (KifMapIntegerInteger*)b;
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
string KifIteratorMapIntegerString::IteratorKeyString() {
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
Exported KifElement* KifMapIntegerString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifMapIntegerString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifMapIntegerString* kmap = (KifMapIntegerString*)ke;
		hmap<long, string>::iterator it;
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

KifElement* KifMapIntegerString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hmap<long, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifMapIntegerString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	hmap<long, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

Exported void KifMapIntegerString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = s_XLConvert(STR(key));
	values[nb] = val->String();
}

Exported void KifMapIntegerString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = s_XLConvert(key);
	values[nb] = val->String();
}

Exported void KifMapIntegerString::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long nb = k->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->String();
}


string KifMapIntegerString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<long, string>::iterator it;
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

string KifMapIntegerString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<long, string>::iterator it;
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

KifElement* KifMapIntegerString::Newiterator(bool rev) {
	KifIteratorMapIntegerString* iter = new KifIteratorMapIntegerString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifMapIntegerString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	long s = ke->Integer();
	hmap<long, string>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifMapIntegerString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifMapIntegerString) {
			KifMapIntegerString* kvect = (KifMapIntegerString*)ke;
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

KifElement* KifMapIntegerString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifMapIntegerString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifMapIntegerString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifMapIntegerString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hmap<long, string>::iterator it;
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
			KifMapIntegerString* kmap = new KifMapIntegerString(kifcode, NULL);
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
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providestringraw(values[skey]);
}

Exported KifElement* KifMapIntegerString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hmap<long, string>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Providestring(it->second));
	}
	return kmap;
}

KifElement* KifMapIntegerString::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<long, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifMapIntegerString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifMapIntegerString* m = (KifMapIntegerString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<long, string>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifMapIntegerString::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifMapIntegerString* ref = (KifMapIntegerString*)a->Copy(NULL);
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

KifElement* KifMapIntegerString::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifMapIntegerString* ref = new KifMapIntegerString(kifcode, NULL);
		KifMapIntegerString* A = (KifMapIntegerString*)a->Copy(NULL);
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

KifElement* KifMapIntegerString::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifMapIntegerString* ref = new KifMapIntegerString(kifcode, NULL);
		hmap<long, string>& val = ((KifMapIntegerString*)a)->values;
		long v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyInteger();
			if (val.find(v) == val.end())
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();

		hmap<long, string>::iterator it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnIntegerKey(it->first) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifMapIntegerString::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerString* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerString*)a;

	KifContainerLock _lock(this);

	hmap<long, string>::iterator it;
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

	if (b->type == kifMapIntegerString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifMapIntegerString* bref = (KifMapIntegerString*)b;
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


KifElement* KifMapIntegerString::Copy(KifDomain* kp, KifElement* dom) {
	KifMapIntegerString* ke = new KifMapIntegerString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//---------------------------------------------------------------------------------------------
string KifIteratorMapIntegerFloat::IteratorKeyString() {
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


Exported KifElement* KifMapIntegerFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

KifElement* KifMapIntegerFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hmap<long, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifMapIntegerFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	hmap<long, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


void KifMapIntegerFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifMapIntegerFloat* kmap = (KifMapIntegerFloat*)ke;
		hmap<long, double>::iterator it;
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

Exported void KifMapIntegerFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = s_XLConvert(key);
	values[nb] = val->Float();
}

Exported void KifMapIntegerFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	long nb = s_XLConvert(STR(key));
	values[nb] = val->Float();
}

Exported void KifMapIntegerFloat::Push(KifElement* k, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	long nb = k->Integer();
	KifContainerLock _lock(this);
	values[nb] = val->Float();
}


string KifMapIntegerFloat::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<long, double>::iterator it;
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

KifElement* KifMapIntegerFloat::Newiterator(bool rev) {
	KifIteratorMapIntegerFloat* iter = new KifIteratorMapIntegerFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifMapIntegerFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	long s = ke->Integer();
	hmap<long, double>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifMapIntegerFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifMapIntegerFloat) {
			KifMapIntegerFloat* kvect = (KifMapIntegerFloat*)ke;
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

KifElement* KifMapIntegerFloat::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifMapIntegerFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifMapIntegerFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifMapIntegerFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hmap<long, double>::iterator it;
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
			KifMapIntegerFloat* kmap = new KifMapIntegerFloat(kifcode, NULL);
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
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providefloat(values[skey]);
}

Exported KifElement* KifMapIntegerFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hmap<long, double>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%ld", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Providefloat(it->second));
	}
	return kmap;
}

KifElement* KifMapIntegerFloat::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<long, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifMapIntegerFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifMapIntegerFloat* m = (KifMapIntegerFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<long, double>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifMapIntegerFloat::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	hmap<long, double>::iterator it;
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

KifElement* KifMapIntegerFloat::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	hmap<long, double>::iterator it;

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

KifElement* KifMapIntegerFloat::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	hmap<long, double>::iterator it;

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

KifElement* KifMapIntegerFloat::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	hmap<long, double>::iterator it;
	if (b->type == kifMapIntegerFloat) {
		KifMapIntegerFloat* bref = (KifMapIntegerFloat*)b;
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

KifElement* KifMapIntegerFloat::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	hmap<long, double>::iterator it;
	if (b->type == kifMapIntegerFloat) {
		KifMapIntegerFloat* bref = (KifMapIntegerFloat*)b;
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

KifElement* KifMapIntegerFloat::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	hmap<long, double>::iterator it;
	if (b->type == kifMapIntegerFloat) {
		KifMapIntegerFloat* bref = (KifMapIntegerFloat*)b;
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

KifElement* KifMapIntegerFloat::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	double v;
	hmap<long, double>::iterator it;
	if (b->type == kifMapIntegerFloat) {
		KifMapIntegerFloat* bref = (KifMapIntegerFloat*)b;
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

KifElement* KifMapIntegerFloat::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	long v;
	hmap<long, double>::iterator it;
	if (b->type == kifMapIntegerFloat) {
		KifMapIntegerFloat* bref = (KifMapIntegerFloat*)b;
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

KifElement* KifMapIntegerFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifMapIntegerFloat* ke = new KifMapIntegerFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifMapIntegerFloat::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	hmap<long, double>::iterator it;
	KifContainerLock _lock(this);

	if (b->type == kifMapIntegerFloat) {
		KifMapIntegerFloat* bref = (KifMapIntegerFloat*)b;
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

KifElement* KifMapIntegerFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	hmap<long, double>::iterator it;

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

KifElement* KifMapIntegerFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapIntegerFloat* ref = this;
	if (autoself == false)
		ref = (KifMapIntegerFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapIntegerFloat*)a;

	KifContainerLock _lock(this);
	hmap<long, double>::iterator it;

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
string KifIteratorMapFloatInteger::IteratorKeyString() {
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

Exported KifElement* KifMapFloatInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

KifElement* KifMapFloatInteger::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hmap<double, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

KifElement* KifMapFloatInteger::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	hmap<double, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}


void KifMapFloatInteger::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifMapFloatInteger* kmap = (KifMapFloatInteger*)ke;
		hmap<double, long>::iterator it;
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

Exported void KifMapFloatInteger::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(key);
	values[nb] = val->Integer();
}

Exported void KifMapFloatInteger::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	double nb = convertfloat(STR(key));
	values[nb] = val->Integer();
}

Exported void KifMapFloatInteger::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->Integer();
}


string KifMapFloatInteger::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<double, long>::iterator it;
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

KifElement* KifMapFloatInteger::Newiterator(bool rev) {
	KifIteratorMapFloatInteger* iter = new KifIteratorMapFloatInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifMapFloatInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	double s = ke->Float();
	hmap<double, long>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifMapFloatInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifMapFloatInteger) {
			KifMapFloatInteger* kvect = (KifMapFloatInteger*)ke;
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

KifElement* KifMapFloatInteger::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifMapFloatInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifMapFloatInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifMapFloatInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hmap<double, long>::iterator it;
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
			KifMapFloatInteger* kmap = new KifMapFloatInteger(kifcode, NULL);
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
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Provideinteger(values[skey]);
}

Exported KifElement* KifMapFloatInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hmap<double, long>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%g", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Provideinteger(it->second));
	}
	return kmap;
}

KifElement* KifMapFloatInteger::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<double, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifMapFloatInteger::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifMapFloatInteger* m = (KifMapFloatInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<double, long>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifMapFloatInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifMapFloatInteger* ke = new KifMapFloatInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifMapFloatInteger::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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

KifElement* KifMapFloatInteger::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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

KifElement* KifMapFloatInteger::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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

KifElement* KifMapFloatInteger::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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

KifElement* KifMapFloatInteger::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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

KifElement* KifMapFloatInteger::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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

KifElement* KifMapFloatInteger::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	long v;
	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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

KifElement* KifMapFloatInteger::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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

KifElement* KifMapFloatInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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

KifElement* KifMapFloatInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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

KifElement* KifMapFloatInteger::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatInteger* ref = this;
	if (autoself == false)
		ref = (KifMapFloatInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatInteger*)a;

	KifContainerLock _lock(this);
	hmap<double, long>::iterator it;
	long v;
	if (b->type == kifMapFloatInteger) {
		KifMapFloatInteger* bref = (KifMapFloatInteger*)b;
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
string KifIteratorMapFloatString::IteratorKeyString() {
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

Exported KifElement* KifMapFloatString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifMapFloatString::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifMapFloatString* kmap = (KifMapFloatString*)ke;
		hmap<double, string>::iterator it;
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

KifElement* KifMapFloatString::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	hmap<double, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifMapFloatString::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	hmap<double, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}


Exported void KifMapFloatString::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = convertfloat(STR(key));
	KifContainerLock _lock(this);
	values[nb] = val->String();
}

Exported void KifMapFloatString::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = convertfloat(key);
	KifContainerLock _lock(this);
	values[nb] = val->String();
}

Exported void KifMapFloatString::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->String();
}


string KifMapFloatString::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<double, string>::iterator it;
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

string KifMapFloatString::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<double, string>::iterator it;
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

KifElement* KifMapFloatString::Newiterator(bool rev) {
	KifIteratorMapFloatString* iter = new KifIteratorMapFloatString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifMapFloatString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	double s = ke->Float();
	hmap<double, string>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifMapFloatString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifMapFloatString) {
			KifMapFloatString* kvect = (KifMapFloatString*)ke;
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

KifElement* KifMapFloatString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifMapFloatString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifMapFloatString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifMapFloatString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hmap<double, string>::iterator it;
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
			KifMapFloatString* kmap = new KifMapFloatString(kifcode, NULL);
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
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providestringraw(values[skey]);
}

Exported KifElement* KifMapFloatString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hmap<double, string>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%g", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Providestring(it->second));
	}
	return kmap;
}

KifElement* KifMapFloatString::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<double, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifMapFloatString::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifMapFloatString* m = (KifMapFloatString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<double, string>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifMapFloatString::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifMapFloatString* ref = (KifMapFloatString*)a->Copy(NULL);
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

KifElement* KifMapFloatString::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifMapFloatString* ref = new KifMapFloatString(kifcode, NULL);
		KifMapFloatString* A = (KifMapFloatString*)a->Copy(NULL);
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

KifElement* KifMapFloatString::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifMapFloatString* ref = new KifMapFloatString(kifcode, NULL);
		hmap<double, string>& val = ((KifMapFloatString*)a)->values;
		double v;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			v = itr->IteratorKeyFloat();
			if (val.find(v) == val.end())
				ref->values[v] = itr->IteratorValueString();
		}
		itr->Release();

		hmap<double, string>::iterator it;
		for (it = val.begin(); it != val.end(); it++) {
			if (b->ValueOnFloatKey(it->first) == kifNOELEMENT)
				ref->values[it->first] = it->second;
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifMapFloatString::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatString* ref = this;
	if (autoself == false)
		ref = (KifMapFloatString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatString*)a;

	KifContainerLock _lock(this);

	hmap<double, string>::iterator it;
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

	if (b->type == kifMapFloatString) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifMapFloatString* bref = (KifMapFloatString*)b;
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


KifElement* KifMapFloatString::Copy(KifDomain* kp, KifElement* dom) {
	KifMapFloatString* ke = new KifMapFloatString(kifcode, NULL);
	ke->values = values;
	return ke;
}

//---------------------------------------------------------------------------------------------
string KifIteratorMapFloatFloat::IteratorKeyString() {
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

KifElement* KifMapFloatFloat::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	hmap<double, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifMapFloatFloat::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	hmap<double, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->second);
	return kvect;
}

Exported KifElement* KifMapFloatFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifMapFloatFloat::Merge(KifElement* ke) {
	//Three cases:
	if (ke->type == type) {
		KifMapFloatFloat* kmap = (KifMapFloatFloat*)ke;
		hmap<double, double>::iterator it;
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

Exported void KifMapFloatFloat::Push(char* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = convertfloat(key);
	KifContainerLock _lock(this);
	values[nb] = val->Float();
}

Exported void KifMapFloatFloat::Push(string& key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = convertfloat(STR(key));
	KifContainerLock _lock(this);
	values[nb] = val->Float();
}

Exported void KifMapFloatFloat::Push(KifElement* key, KifElement* val) {
	if (val == kifRAISEERROR)
		return;
	double nb = key->Float();
	KifContainerLock _lock(this);
	values[nb] = val->Float();
}


string KifMapFloatFloat::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<double, double>::iterator it;
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

KifElement* KifMapFloatFloat::Newiterator(bool rev) {
	KifIteratorMapFloatFloat* iter = new KifIteratorMapFloatFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifMapFloatFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	double s = ke->Float();
	hmap<double, double>::iterator itx = values.find(s);

	if (itx == values.end()) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(kifErrorStrings[223]);
		return;
	}

	values.erase(itx);
}

bool KifMapFloatFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}
		if (ke->type == kifMapFloatFloat) {
			KifMapFloatFloat* kvect = (KifMapFloatFloat*)ke;
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

KifElement* KifMapFloatFloat::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifMapFloatFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifMapFloatFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

KifElement* KifMapFloatFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvector(this, contextualpattern);
			hmap<double, double>::iterator it;
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
			KifMapFloatFloat* kmap = new KifMapFloatFloat(kifcode, NULL);
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
	if (values.find(skey) == values.end()) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}
	return kifcode->Providefloat(values[skey]);
}

Exported KifElement* KifMapFloatFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	hmap<double, double>::iterator it;
	char buffer[50];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(buffer, 50, "%g", it->first);
		kmap->Push(buffer, (KifElement*)kifcode->Providefloat(it->second));
	}
	return kmap;
}

KifElement* KifMapFloatFloat::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<double, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

Exported KifElement* KifMapFloatFloat::same(KifElement* a) {

	if (a->type != type)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifMapFloatFloat* m = (KifMapFloatFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<double, double>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second != values[it->first])
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifMapFloatFloat::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	KifContainerLock _lock(this);
	hmap<double, double>::iterator it;
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

KifElement* KifMapFloatFloat::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	KifContainerLock _lock(this);
	hmap<double, double>::iterator it;

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

KifElement* KifMapFloatFloat::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	KifContainerLock _lock(this);
	hmap<double, double>::iterator it;

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

KifElement* KifMapFloatFloat::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	hmap<double, double>::iterator it;
	if (b->type == kifMapFloatFloat) {
		KifMapFloatFloat* bref = (KifMapFloatFloat*)b;
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

KifElement* KifMapFloatFloat::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	hmap<double, double>::iterator it;
	if (b->type == kifMapFloatFloat) {
		KifMapFloatFloat* bref = (KifMapFloatFloat*)b;
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

KifElement* KifMapFloatFloat::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	hmap<double, double>::iterator it;
	if (b->type == kifMapFloatFloat) {
		KifMapFloatFloat* bref = (KifMapFloatFloat*)b;
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

KifElement* KifMapFloatFloat::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	double v;
	hmap<double, double>::iterator it;
	if (b->type == kifMapFloatFloat) {
		KifMapFloatFloat* bref = (KifMapFloatFloat*)b;
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

KifElement* KifMapFloatFloat::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	long v;
	hmap<double, double>::iterator it;
	if (b->type == kifMapFloatFloat) {
		KifMapFloatFloat* bref = (KifMapFloatFloat*)b;
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

KifElement* KifMapFloatFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifMapFloatFloat* ke = new KifMapFloatFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifMapFloatFloat::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	hmap<double, double>::iterator it;
	KifContainerLock _lock(this);

	if (b->type == kifMapFloatFloat) {
		KifMapFloatFloat* bref = (KifMapFloatFloat*)b;
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

KifElement* KifMapFloatFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	KifContainerLock _lock(this);
	hmap<double, double>::iterator it;

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

KifElement* KifMapFloatFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifMapFloatFloat* ref = this;
	if (autoself == false)
		ref = (KifMapFloatFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifMapFloatFloat*)a;

	KifContainerLock _lock(this);
	hmap<double, double>::iterator it;

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

//-------------------------------------------------------------------------------


string KifIntegerMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<long, KifElement*>::iterator it;
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

string KifIntegerMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<long, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":" << it->second->JSonString();
	}
	res << "}";
	return res.str();
}

string KifIntegerMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<long, KifElement*>::iterator it;
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

KifElement* KifIntegerMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
	hmap<long, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifIntegerMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	hmap<long, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->Push(it->second);
	return kvect;
}

KifElement* KifIntegerMap::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifIntegerMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifIntegerMap)
		return kifNULL;

	KifIntegerMap* A = (KifIntegerMap*)a;
	hmap<long, KifElement*>::iterator it;

	KifIntegerMap* ref = new KifIntegerMap(kifcode, NULL);
	KifIntegerMap* bref = (KifIntegerMap*)b;
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

KifElement* KifIntegerMap::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifIntegerMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifIntegerMap)
		return kifNULL;

	KifIntegerMap* A = (KifIntegerMap*)a;
	hmap<long, KifElement*>::iterator it;

	KifIntegerMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerMap*)a->Copy(NULL);

	KifIntegerMap* bref = (KifIntegerMap*)b;

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

KifElement* KifIntegerMap::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifIntegerMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifIntegerMap)
		return kifNULL;

	KifIntegerMap* A = (KifIntegerMap*)a;
	hmap<long, KifElement*>::iterator it;
	hmap<long, KifElement*>::iterator itres;

	KifIntegerMap* ref = new KifIntegerMap(kifcode, NULL);
	KifIntegerMap* bref = (KifIntegerMap*)b;


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

Exported KifElement* KifIntegerMap::same(KifElement* a) {

	if (a->type != kifIntegerMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifIntegerMap* m = (KifIntegerMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<long, KifElement*>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second->same(values[it->first]) == kifFALSE)
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifIntegerMap::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifIntegerMap)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifIntegerMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerMap*)a;

	hmap<long, KifElement*>::iterator it;
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

KifElement* KifIntegerMap::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifIntegerMap)
		return a->minus(a, b, context, idthread,callfunc,autoself);

	KifIntegerMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerMap*)a;

	hmap<long, KifElement*>::iterator it;
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

KifElement* KifIntegerMap::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifIntegerMap)
		return a->multiply(a, b, context, idthread,callfunc,autoself);

	KifIntegerMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerMap*)a;

	hmap<long, KifElement*>::iterator it;
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

KifElement* KifIntegerMap::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifIntegerMap)
		return a->divide(a, b, context, idthread,callfunc,autoself);

	KifIntegerMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerMap*)a;

	hmap<long, KifElement*>::iterator it;
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

KifElement* KifIntegerMap::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifIntegerMap)
		return a->mod(a, b, context, idthread,callfunc,autoself);

	KifIntegerMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerMap*)a;

	hmap<long, KifElement*>::iterator it;
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

KifElement* KifIntegerMap::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifIntegerMap)
		return a->shiftright(a, b, context, idthread,callfunc,autoself);

	KifIntegerMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerMap*)a;

	hmap<long, KifElement*>::iterator it;
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

KifElement* KifIntegerMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifIntegerMap)
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);

	KifIntegerMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerMap*)a;

	hmap<long, KifElement*>::iterator it;
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

KifElement* KifIntegerMap::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifIntegerMap)
		return a->power(a, b, context, idthread,callfunc,autoself);

	KifIntegerMap* ref = this;
	if (autoself == false)
		ref = (KifIntegerMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifIntegerMap*)a;

	hmap<long, KifElement*>::iterator it;
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

KifElement* KifIntegerMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorIntegerMap* iter = new KifIteratorIntegerMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifIntegerMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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

KifElement* KifIntegerMap::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<long, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

KifElement* KifIntegerMap::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Map::Declaration");
#endif
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifIntegerMap::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}


void KifIntegerMap::Methods(KifVectorString* fcts) {
#ifdef PROFILING
	profilingcall("Map::List");
#endif
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
}

KifElement* KifIntegerMap::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Map::Copy");
#endif
	KifIntegerMap* ke = new KifIntegerMap(kifcode, NULL);
	hmap<long, KifElement*>::iterator itx;
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

KifElement* KifIntegerMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

Exported void KifIntegerMap::Setreference(int inc) {
	reference += inc;
	Putpopped(0);
	hmap<long, KifElement*>::iterator itx;
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


string KifIteratorIntegerMap::IteratorKeyString() {
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

KifElement* KifIteratorIntegerMap::Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val) {
	if (reverse == false) {
		if (it == values->end())
			return kifNULL;
		return it->second->Execute(contextualpattern, dom, val);
	}
	if (itr == values->Rend())
		return kifNULL;
	return itr->second->Execute(contextualpattern, dom, val);
}



static void resetMap(KifIntegerMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	hmap<long, KifElement*>& values = kmap->values;
	if (values.size() == 0)
		return;

	hmap<long, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}

void KifIntegerMap::Resetreference(int inc) {
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

void KifIntegerMap::Clear() {
	KifContainerLock _lock(this);
	hmap<long, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(reference + 1);
	values.clear();
}

void KifIntegerMap::Merge(KifElement* ke) {
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


KifElement* KifIntegerMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* ke;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			hmap<long, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				vect->Push(kifcode->Provideinteger(it->first));
			return vect;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifIntegerMap* kmap = new KifIntegerMap(kifcode, NULL);
			hmap<long, KifElement*>::iterator it;
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifIntegerMap* kmap = new KifIntegerMap(kifcode, NULL);
			hmap<long, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kmap->push(it->first, it->second);
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

void KifIntegerMap::Pop(KifElement* ke) {
	long s = ke->Integer();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return;

		hmap<long, KifElement*>::iterator itx = values.find(s);

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
Exported void KifIntegerMap::Push(string& key, KifElement* val) {
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

Exported void KifIntegerMap::Push(char* key, KifElement* val) {
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

Exported void KifIntegerMap::Push(KifElement* k, KifElement* val) {
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

void KifIntegerMap::push(long n, KifElement* val) {
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

string KifFloatMap::String() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<double, KifElement*>::iterator it;
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

string KifFloatMap::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<double, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		res << it->first << ":" << it->second->JSonString();
	}
	res << "}";
	return res.str();
}

string KifFloatMap::BaseString() {
	KifContainerLock _lock(this);
	stringstream res;
	hmap<double, KifElement*>::iterator it;
	res << "{";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sx = it->second->BaseString();
		res << it->first << ":" << sx;
	}
	res << "}";
	return res.str();
}

KifElement* KifFloatMap::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
	hmap<double, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->storevalue(it->first);
	return kvect;
}

KifElement* KifFloatMap::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	hmap<double, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->Push(it->second);
	return kvect;
}

KifElement* KifFloatMap::xorset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifFloatMap)
		return a->xorset(a, b, context, autoself);

	if (b->type != kifFloatMap)
		return kifNULL;

	KifFloatMap* A = (KifFloatMap*)a;
	hmap<double, KifElement*>::iterator it;

	KifFloatMap* ref = new KifFloatMap(kifcode, NULL);
	KifFloatMap* bref = (KifFloatMap*)b;
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

KifElement* KifFloatMap::orset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::orset");
#endif

	if (a->type != kifFloatMap)
		return a->orset(a, b, context, autoself);

	if (b->type != kifFloatMap)
		return kifNULL;

	KifFloatMap* A = (KifFloatMap*)a;
	hmap<double, KifElement*>::iterator it;

	KifFloatMap* ref = this;
	if (autoself == false)
		ref = (KifFloatMap*)a->Copy(NULL);

	KifFloatMap* bref = (KifFloatMap*)b;

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

KifElement* KifFloatMap::andset(KifElement* a, KifElement* b, KifElement* context, int idthread, KifCallFunction* callfunc, bool autoself) {
#ifdef PROFILING
	profilingcall("Map::andset");
#endif

	if (a->type != kifFloatMap)
		return a->andset(a, b, context, autoself);

	if (b->type != kifFloatMap)
		return kifNULL;

	KifFloatMap* A = (KifFloatMap*)a;
	hmap<double, KifElement*>::iterator it;
	hmap<double, KifElement*>::iterator itres;

	KifFloatMap* ref = new KifFloatMap(kifcode, NULL);
	KifFloatMap* bref = (KifFloatMap*)b;

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

Exported KifElement* KifFloatMap::same(KifElement* a) {

	if (a->type != kifFloatMap)
		return Mapcompare(this, a);

	KifContainerLock _lock(this);
	KifFloatMap* m = (KifFloatMap*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	hmap<double, KifElement*>::iterator it = m->values.begin();
	while (it != m->values.end()) {
		if (values.find(it->first) == values.end())
			return kifFALSE;
		if (it->second->same(values[it->first]) == kifFALSE)
			return kifFALSE;
		it++;
	}
	return kifTRUE;
}

KifElement* KifFloatMap::plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::plus");
#endif

	if (a->type != kifFloatMap)
		return a->plus(a, b, context, idthread,callfunc,autoself);

	KifFloatMap* ref = this;
	if (autoself == false)
		ref = (KifFloatMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatMap*)a;

	hmap<double, KifElement*>::iterator it;
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

KifElement* KifFloatMap::minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::minus");
#endif

	if (a->type != kifFloatMap)
		return a->minus(a, b, context, idthread,callfunc,autoself);

	KifFloatMap* ref = this;
	if (autoself == false)
		ref = (KifFloatMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatMap*)a;

	hmap<double, KifElement*>::iterator it;
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

KifElement* KifFloatMap::multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::multiply");
#endif

	if (a->type != kifFloatMap)
		return a->multiply(a, b, context, idthread,callfunc,autoself);

	KifFloatMap* ref = this;
	if (autoself == false)
		ref = (KifFloatMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatMap*)a;

	hmap<double, KifElement*>::iterator it;
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

KifElement* KifFloatMap::divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::divide");
#endif

	if (a->type != kifFloatMap)
		return a->divide(a, b, context, idthread,callfunc,autoself);

	KifFloatMap* ref = this;
	if (autoself == false)
		ref = (KifFloatMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatMap*)a;

	hmap<double, KifElement*>::iterator it;
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

KifElement* KifFloatMap::mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::mod");
#endif

	if (a->type != kifFloatMap)
		return a->mod(a, b, context, idthread,callfunc,autoself);

	KifFloatMap* ref = this;
	if (autoself == false)
		ref = (KifFloatMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatMap*)a;

	hmap<double, KifElement*>::iterator it;
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

KifElement* KifFloatMap::shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftright");
#endif

	if (a->type != kifFloatMap)
		return a->shiftright(a, b, context, idthread,callfunc,autoself);

	KifFloatMap* ref = this;
	if (autoself == false)
		ref = (KifFloatMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatMap*)a;

	hmap<double, KifElement*>::iterator it;
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

KifElement* KifFloatMap::shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::shiftleft");
#endif

	if (a->type != kifFloatMap)
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);

	KifFloatMap* ref = this;
	if (autoself == false)
		ref = (KifFloatMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatMap*)a;

	hmap<double, KifElement*>::iterator it;
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

KifElement* KifFloatMap::power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Map::power");
#endif

	if (a->type != kifFloatMap)
		return a->power(a, b, context, idthread,callfunc,autoself);

	KifFloatMap* ref = this;
	if (autoself == false)
		ref = (KifFloatMap*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifFloatMap*)a;

	hmap<double, KifElement*>::iterator it;
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

KifElement* KifFloatMap::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Map::Newiterator");
#endif
	KifIteratorFloatMap* iter = new KifIteratorFloatMap(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

bool KifFloatMap::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
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
			double n = 0;
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
			double n = 0;
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

KifElement* KifFloatMap::Vector(KifElement* contextualpattern, KifVector* v) {
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
	hmap<double, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		localstring.value = it->first;
		kvect->Push(&localstring);
	}
	return kvect;
}

KifElement* KifFloatMap::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Map::Declaration");
#endif
	if (TestkifFunctions(n, kifGlobals->kifMapFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifFloatMap::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsMap.find(s) == kifGlobals->kifInformationsMap.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsMap[s]);
}

void KifFloatMap::Methods(KifVectorString* fcts) {
#ifdef PROFILING
	profilingcall("Map::List");
#endif
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifMapFunctions.begin(); it != kifGlobals->kifMapFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
}

KifElement* KifFloatMap::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Map::Copy");
#endif
	KifFloatMap* ke = new KifFloatMap(kifcode, NULL);
	hmap<double, KifElement*>::iterator itx;
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

KifElement* KifFloatMap::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
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

Exported void KifFloatMap::Setreference(int inc) {
	reference += inc;
	Putpopped(0);
	hmap<double, KifElement*>::iterator itx;
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


string KifIteratorFloatMap::IteratorKeyString() {
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

KifElement* KifIteratorFloatMap::Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val) {
	if (reverse == false) {
		if (it == values->end())
			return kifNULL;
		return it->second->Execute(contextualpattern, dom, val);
	}
	if (itr == values->Rend())
		return kifNULL;
	return itr->second->Execute(contextualpattern, dom, val);
}

static void resetMap(KifFloatMap* kmap, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kmap);
	hmap<double, KifElement*>& values = kmap->values;
	if (values.size() == 0)
		return;

	hmap<double, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}

void KifFloatMap::Resetreference(int inc) {
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

void KifFloatMap::Clear() {
	KifContainerLock _lock(this);
	hmap<double, KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		itx->second->Resetreference(reference + 1);
	values.clear();
}

void KifFloatMap::Merge(KifElement* ke) {
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

KifElement* KifFloatMap::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* ke;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* vect = contextualpattern->Newinstance();
			hmap<double, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				vect->Push(kifcode->Providefloat(it->first));
			return vect;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some value must be replaced with their evaluation
		if (evaluate == true) {
			KifFloatMap* kmap = new KifFloatMap(kifcode, NULL);
			hmap<double, KifElement*>::iterator it;
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
			if (kind->key != kifZERO || kind->right != kifZERO) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(kifErrorStrings[223]);
				return kifNOELEMENT;
			}
			KifFloatMap* kmap = new KifFloatMap(kifcode, NULL);
			hmap<double, KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++)
				kmap->push(it->first, it->second);
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

void KifFloatMap::Pop(KifElement* ke) {
	double s = ke->Float();
	KifElement* kres = NULL;
	{
		KifContainerLock _lock(this);
		if (values.size() == 0)
			return;

		hmap<double, KifElement*>::iterator itx = values.find(s);

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
Exported void KifFloatMap::Push(string& key, KifElement* val) {
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

Exported void KifFloatMap::Push(char* key, KifElement* val) {
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

Exported void KifFloatMap::Push(KifElement* key, KifElement* val) {
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

void KifFloatMap::push(double n, KifElement* val) {
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

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


void KifMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<string, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifMapString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<string, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifMapInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<string, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifMapFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<string, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseString(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifIntegerMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<long, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifMapIntegerString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<long, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifMapIntegerInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<long, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifMapIntegerFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<long, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifFloatMap::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<double, KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifMapFloatString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<double, string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifMapFloatInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<double, long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------

void KifMapFloatFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int i = 0;
	KifElement* ke;
	hmap<double, double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, it->first);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

//--------------------------




