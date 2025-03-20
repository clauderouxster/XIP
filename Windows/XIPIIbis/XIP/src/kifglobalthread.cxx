/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2010 - 2011 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the KiF library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : KiF
Version    : 1.00
filename   : kifglobalthread.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language: GlobalThread methods
Programmer : Claude ROUX
Reviewer   :
*/

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------

#include "kif.h"
#include "kifinit.h"
#include "kifcontainer.h"
#include "kifpredicate.h"

#ifdef WIN32
#define _USE_MATH_DEFINES
#include <math.h>

#include <io.h>
#define DUP _dup
#define DUP2 _dup2
#define FILENO _fileno
#else
#include <unistd.h>
#define FILENO fileno
#define DUP dup
#define DUP2 dup2
#endif



//--------------------------------------------------------------------------------------------------------------------------
extern const char* kifErrorStrings[];
KIFCallHaskell calls[7];
//--------------------------------------------------------------------------------------------------------------------------
Exported void Cleanallerror() {
	for (int i = 0; i<kifGlobals->mxthreads; i++) {
		if (kifGlobals->THR[i] != NULL)
			kifGlobals->THR[i]->cleanerror();
	}
	kifGlobals->executionbreak = 0;
}


Exported void Cleanerror() {
	int id = kifGlobals->GetThreadid();
	if (kifGlobals->THR[id] != NULL)
		kifGlobals->THR[id]->cleanerror();
}

Exported string Errorstringid(int id) {
	if (kifGlobals->THR[id] != NULL) {
		if (kifGlobals->THR[id]->errorptr != NULL)
			return kifGlobals->THR[id]->errorptr->String();

	}
	return "";
}

Exported string Errorstring() {
	int id = kifGlobals->GetThreadid();
	if (kifGlobals->THR[id] != NULL) {
		if (kifGlobals->THR[id]->errorptr != NULL)
			return kifGlobals->THR[id]->errorptr->String();

	}
	return "";
}

Exported KifElement* Errorptr() {
	int id = kifGlobals->GetThreadid();
	if (kifGlobals->THR[id] != NULL) {
		if (kifGlobals->THR[id]->errorptr != NULL)
			return kifGlobals->THR[id]->errorptr;

	}
	return kifNULL;
}

Exported KifElement* Errorptrid(int id) {
	if (kifGlobals->THR[id] != NULL) {
		if (kifGlobals->THR[id]->errorptr != NULL)
			return kifGlobals->THR[id]->errorptr;

	}
	return kifNULL;
}

Exported bool Error() {
	int id = kifGlobals->GetThreadid();

	if (kifGlobals->executionbreak) {
		if (id == 0 && kifGlobals->executionbreak == 1) {
			if (kifGlobals->terminationfunction != NULL)
				(*kifGlobals->terminationfunction)();
			kifGlobals->executionbreak = 2;
		}
		return true;
	}

	if (kifGlobals->THR[id]->error)
		return true;

	return kifGlobals->Teststack(id);
}

Exported bool Errorid(int id) {

	if (kifGlobals->executionbreak) {
		if (id == 0 && kifGlobals->executionbreak == 1) {
			if (kifGlobals->terminationfunction != NULL)
				(*kifGlobals->terminationfunction)();
			kifGlobals->executionbreak = 2;
		}
		return true;
	}

	if (kifGlobals->THR[id]->error)
		return true;

	return kifGlobals->Teststack(id);
}

Exported void Seterror(bool v) {
	kifGlobals->THR[kifGlobals->GetThreadid()]->error = v;
}

void KifGlobalThread::CleanTriggerTables() {
	//In the case of lumbering threads, we need to terminate them...
	bin_hash<KifTable*>::iterator it;
	for (it = triggertable.begin(); it != triggertable.end(); it++)
		it->second->lock->Released();
	triggertable.clear();
}

//--------------------------------------------------------------------------------------------------------------------
static bin_hash<KifElement*> throughs;
//--------------------------------------------------------------------------------------------------------------------
KifElement* KifCode::KifParseVarThrough(x_node* xn, KifElement* kf) {
	short idtype = KifNewId(xn->nodes[0]->value);
	short id = KifNewId(xn->nodes[1]->value);
	kifGlobals->through = true;
	KifElement* var = CreateVariable(xn, id, kf, kifFULLSTRING);
	kifGlobals->through = false;
	KifElement* kv;
	if (throughs.find(id) == throughs.end()) {
		switch (idtype) {
		case idsthrough:
			kv = new KifString(NULL, var);
			break;
		case idithrough:
			kv = new KifInteger(NULL, var);
			break;
		case idfthrough:
			kv = new KifFloat(NULL, var);
			break;
		case idvthrough:
			kv = new KifVectorString(NULL, var);
			break;
		}
		throughs[id] = kv;
	}
	else {
		kv = throughs[id];
		var->Add(kv);
	}
	kv->kifcode = this;
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}
//--------------------------------------------------------------------------------------------------------------------
static FILE* localstreamerr = NULL;
static FILE* localstream = NULL;

void clear_output() {
	if (localstream != NULL) {
		fclose(localstream);
		localstream = NULL;
	}

	if (localstreamerr != NULL) {
		fclose(localstreamerr);
		localstreamerr = NULL;
	}
}
char redirect_output(KifCode* kifcode, string& filename, int output) {
	if (output == 1) {
		if (localstream != NULL)
			return -2;

		int o = DUP(output);

		localstream = fopen(STR(filename), "w");
		if (localstream == NULL)
			return -1;
		DUP2(FILENO(localstream), output);
		return o;
	}

	if (localstreamerr != NULL)
		return -2;

	int o = DUP(output);

	localstreamerr = fopen(STR(filename), "w");
	if (localstreamerr == NULL)
		return -1;
	DUP2(FILENO(localstreamerr), output);
	return o;

}

char restate_output(KifCode* kifcode, int o, int output) {
	if (output == 1) {
		if (localstream == NULL)
			return -1;

		fflush(stdout);
		fclose(localstream);
		localstream = NULL;
		DUP2(o, output);
		return 0;
	}

	if (localstreamerr == NULL)
		return -1;

	fflush(stdout);
	fclose(localstreamerr);
	localstreamerr = NULL;
	DUP2(o, output);
	return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------
void AddALoadingPath(string thelocalname) {
	KifConst* var = (KifConst*)kifGlobals->kconstants->Declaration(idpath);
	KifVectorString* vargs = (KifVectorString*)var->value;
#ifdef WIN32
	int seppos = thelocalname.rfind("\\");
#else
	int seppos = thelocalname.rfind("/");
#endif
	string lepath(".");
	string fname(thelocalname);
	if (seppos != -1) {
		lepath = thelocalname.substr(0, seppos);
		fname = thelocalname.substr(seppos + 1, thelocalname.size() - seppos - 1);
	}
	vargs->values.push_back(lepath);
	vargs->values.push_back(fname);

	//-----------------------------------------------
	if (vargs->values.size() <= 3) {
		//For the _current variable...
		string spath = lepath;

#ifdef WIN32
		if (spath[spath.size() - 1] != '\\')
			spath += '\\';
#else
		if (spath[spath.size() - 1] != '/')
			spath += '/';
#endif
		int idcurrent = KifNewId("_current");
		KifConst* varcurrent = (KifConst*)kifGlobals->kconstants->Declaration(idcurrent);
		((KifBaseString*)varcurrent->value)->value = spath;
	}
	//-----------------------------------------------
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void kifcompatibletype::values(vector<KifType>& v) {
	long res;
	int sz = sizeof(long)*8;
	for (it = types.begin(); it != types.end(); it++) {
		long ty = it->second;
		for (int i = sz- 1; i >= 0; i--) {
			if (!ty)
				break;
			if (ty < 0) {
				res = (it->first << szlonglong) + i;
				v.push_back((KifType)res);
			}
			ty <<= 1;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------
char redirect_output(KifCode* kifcode, string& filename, int output);
char restate_output(KifCode* kifcode, int o, int output);

union bitdouble {
	double v;
	unsigned char bits[sizeof(double)];
};


static inline double Radian(double num) {
	return(M_PI*(num / 180));
}

KifElement* PreGPSDistance(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	double a, b, c, d;
	double R = 6371;

	a = Radian(callfunc->Evaluatethread(0, dom, idthread)->Float());
	c = Radian(callfunc->Evaluatethread(1, dom, idthread)->Float());
	b = Radian(callfunc->Evaluatethread(2, dom, idthread)->Float());
	d = Radian(callfunc->Evaluatethread(3, dom, idthread)->Float());

	if (callfunc->Size() == 5)
		R = callfunc->Evaluatethread(4, dom, idthread)->Float();

	R = acos(cos(a)*cos(b)*cos(abs(c - d)) + sin(a)*sin(b)) * R;
	return callfunc->kifcode->Providefloat(R);
}

KifElement* PreGetBit(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kval = callfunc->Evaluatethread(0, dom, idthread);
	long ith = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	if (kval->type == kifByte) {
		unsigned char v = kval->Byte();
		if (ith <0 || ith >= 8)
			return kifFALSE;
		v &= 1 << ith;
		if (v == 0)
			return kifFALSE;
		return kifTRUE;
	}

	if (kval->type == kifFloat) {
		bitdouble b;
		b.v = kval->Float();
		int pbits = ith >> 3;
		if (pbits < 0 || pbits >= sizeof(double))
			return kifFALSE;
		unsigned char pbit = 1 << (ith - (pbits << 3));
		pbit &= b.bits[pbits];
		if (!pbit)
			return kifFALSE;
		return kifTRUE;
	}

	long v = kval->Integer();
	if (ith < 0 || ith >= sizeof(long)* 8)
		return kifFALSE;
	v &= 1 << ith;
	if (!v)
		return kifFALSE;
	return kifTRUE;
}

KifElement* PreRedictectOutput(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	string filename = callfunc->Evaluatethread(0, dom, idthread)->String();
	bool iserr = callfunc->Evaluatethread(1, dom, idthread)->Boolean();
	int output = 1;
	if (iserr)
		output = 2;
	output = redirect_output(callfunc->kifcode, filename, output);
	if (output == -2)
		return callfunc->kifcode->Returnerror("SYS(010): Stream has already been redirected");
	if (output == -1)
		return callfunc->kifcode->Returnerror("SYS(001): Cannot open output file");
	return callfunc->kifcode->Provideinteger(output);
}

KifElement* PreRestateOutput(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	int output = callfunc->Evaluatethread(0, dom, idthread)->Integer();
	bool iserr = callfunc->Evaluatethread(1, dom, idthread)->Boolean();
	if (iserr)
		output = restate_output(callfunc->kifcode, output, 2);
	else
		output = restate_output(callfunc->kifcode, output, 1);
	if (output == -1)
		return callfunc->kifcode->Returnerror("SYS(011): Stream not opened");
	return kifTRUE;
}

KifElement* PreCompatibilities(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	string key;
	string val;

	if (callfunc->Size() == 0) {
		hmap<short, kifcompatibletype>::iterator it;
		KifMap* kmap = new KifMap(callfunc->kifcode, NULL);
		KifVectorString* kvect;
		vector<KifType> v;				
		for (it = kifGlobals->compatibilities.begin(); it != kifGlobals->compatibilities.end(); it++) {
			key = kifGlobals->kifStringType[(KifType)it->first];
			if (key == "")
				continue;
			kvect = new KifVectorString(callfunc->kifcode, NULL);
			v.clear();
			it->second.values(v);
			for (int i = 0; i < v.size(); i++) {
				val = kifGlobals->kifStringType[v[i]];
				if (val != "")
					kvect->values.push_back(val);
			}

			if (kvect->values.size()>1)
				kmap->Push(key, kvect);
		}

		return kmap;
	}

	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	if (!ke->aMapContainer())
		return kifFALSE;

	string err("KIF(629): Unknwown type: ");
	KifType k, v;
	hmap<short, vector<KifType> > values;
	KifIteration* it = (KifIteration*)ke->Newiterator(true);	
	KifIteration* itv;
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		key = it->IteratorKeyString();
		if (kifGlobals->kifTypeString.find(key) == kifGlobals->kifTypeString.end()) {
			err += key;
			return callfunc->kifcode->Returnerror(err);
		}
		ke = it->IteratorValue();
		if (!ke->aVectorContainer())
			return kifFALSE;
		k = kifGlobals->kifTypeString[key];
		itv = (KifIteration*)ke->Newiterator(true);
		for (itv->Begin(); itv->End() == kifFALSE; itv->Next()) {
			val = itv->IteratorValueString();
			if (kifGlobals->kifTypeString.find(val) == kifGlobals->kifTypeString.end()) {
				err += val;
				return callfunc->kifcode->Returnerror(err);
			}

			v = kifGlobals->kifTypeString[val];
			values[k].push_back(v);
		}
		itv->Release();
	}
	it->Release();
	hmap<short, vector<KifType> >::iterator ivv;
	int i;

	for (ivv = values.begin(); ivv != values.end(); ivv++) {
		k = (KifType)ivv->first;
		//First we clean our compatibilities element
		kifGlobals->compatibilities[k].clear();
		//then we set our values back into it...
		for (i = 0; i<ivv->second.size();i++) 
			kifGlobals->compatibilities[k].set(ivv->second[i]);
	}

	return kifTRUE;
}

KifElement* PreSizeBytes(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	string code = callfunc->Evaluatethread(0, dom, idthread)->String();
	return callfunc->kifcode->Provideinteger(code.size());
}

KifElement* PreFromJSON(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	string code = callfunc->Evaluatethread(0, dom, idthread)->String();
	return callfunc->kifcode->EvaluateJSON(dom, code);
}

KifElement* PreIsContainer(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	if (res->aContainer())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreIsMap(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	if (res->aMapContainer())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreIsVector(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	if (res->aVectorContainer())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreReserve(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* ksz = callfunc->Evaluatethread(1, dom, idthread);
	switch (ke->type) {
	case kifVector:
		((KifVector*)ke)->values.reserve(ksz->Integer());
		return kifTRUE;
	case kifVectorByte:
		((KifVectorByte*)ke)->values.reserve(ksz->Integer());
		return kifTRUE;
	case kifVectorString:
		((KifVectorString*)ke)->values.reserve(ksz->Integer());
		return kifTRUE;
	case kifVectorUString:
		((KifVectorUString*)ke)->values.reserve(ksz->Integer());
		return kifTRUE;
	case kifVectorInteger:
		((KifVectorInteger*)ke)->values.reserve(ksz->Integer());
		return kifTRUE;
	case kifVectorFloat:
		((KifVectorFloat*)ke)->values.reserve(ksz->Integer());
		return kifTRUE;
	}
	return kifFALSE;
}


KifElement* PreMoving(KifElement* ke, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kvect = callfunc->Evaluatethread(0, dom, idthread);
	long movefrom = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	long moveto = callfunc->Evaluatethread(2, dom, idthread)->Integer();
	long sz = kvect->Size();

	if (movefrom < 0 || movefrom >= sz || moveto < 0 || moveto >= sz) {
		if (kifGlobals->erroronkey)
			return callfunc->kifcode->Returnerror(kifErrorStrings[223]);
		return kifNOELEMENT;
	}

	if (moveto == movefrom)
		return kvect;

	string s;
	wstring w;
	long v;
	double d;
	switch (kvect->type) {
	case kifVector:
		ke = ((KifVector*)kvect)->values[movefrom];
		if (movefrom>moveto) {
			((KifVector*)kvect)->values.erase(((KifVector*)kvect)->values.begin() + movefrom);
			((KifVector*)kvect)->values.insert(((KifVector*)kvect)->values.begin() + moveto, ke);
		}
		else {
			((KifVector*)kvect)->values.insert(((KifVector*)kvect)->values.begin() + moveto + 1, ke);
			((KifVector*)kvect)->values.erase(((KifVector*)kvect)->values.begin() + movefrom);
		}
		break;
	case kifVectorByte:
		v = ((KifVectorByte*)kvect)->values[movefrom];
		if (movefrom>moveto) {
			((KifVectorByte*)kvect)->values.erase(((KifVectorByte*)kvect)->values.begin() + movefrom);
			((KifVectorByte*)kvect)->values.insert(((KifVectorByte*)kvect)->values.begin() + moveto, (uchar)v);
		}
		else {
			((KifVectorByte*)kvect)->values.insert(((KifVectorByte*)kvect)->values.begin() + moveto + 1, (uchar)v);
			((KifVectorByte*)kvect)->values.erase(((KifVectorByte*)kvect)->values.begin() + movefrom);
		}
		break;
	case kifVectorString:
		s = ((KifVectorString*)kvect)->values[movefrom];
		if (movefrom>moveto) {
			((KifVectorString*)kvect)->values.erase(((KifVectorString*)kvect)->values.begin() + movefrom);
			((KifVectorString*)kvect)->values.insert(((KifVectorString*)kvect)->values.begin() + moveto, s);
		}
		else {
			((KifVectorString*)kvect)->values.insert(((KifVectorString*)kvect)->values.begin() + moveto + 1, s);
			((KifVectorString*)kvect)->values.erase(((KifVectorString*)kvect)->values.begin() + movefrom);
		}
		break;
	case kifVectorUString:
		w = ((KifVectorUString*)kvect)->values[movefrom];
		if (movefrom>moveto) {
			((KifVectorUString*)kvect)->values.erase(((KifVectorUString*)kvect)->values.begin() + movefrom);
			((KifVectorUString*)kvect)->values.insert(((KifVectorUString*)kvect)->values.begin() + moveto, w);
		}
		else {
			((KifVectorUString*)kvect)->values.insert(((KifVectorUString*)kvect)->values.begin() + moveto + 1, w);
			((KifVectorUString*)kvect)->values.erase(((KifVectorUString*)kvect)->values.begin() + movefrom);
		}
		break;
	case kifVectorInteger:
		v = ((KifVectorInteger*)kvect)->values[movefrom];
		if (movefrom>moveto) {
			((KifVectorInteger*)kvect)->values.erase(((KifVectorInteger*)kvect)->values.begin() + movefrom);
			((KifVectorInteger*)kvect)->values.insert(((KifVectorInteger*)kvect)->values.begin() + moveto, v);
		}
		else {
			((KifVectorInteger*)kvect)->values.insert(((KifVectorInteger*)kvect)->values.begin() + moveto + 1, v);
			((KifVectorInteger*)kvect)->values.erase(((KifVectorInteger*)kvect)->values.begin() + movefrom);
		}
		break;
	case kifVectorFloat:
		d = ((KifVectorFloat*)kvect)->values[movefrom];
		if (movefrom>moveto) {
			((KifVectorFloat*)kvect)->values.erase(((KifVectorFloat*)kvect)->values.begin() + movefrom);
			((KifVectorFloat*)kvect)->values.insert(((KifVectorFloat*)kvect)->values.begin() + moveto, d);
		}
		else {
			((KifVectorFloat*)kvect)->values.insert(((KifVectorFloat*)kvect)->values.begin() + moveto + 1, d);
			((KifVectorFloat*)kvect)->values.erase(((KifVectorFloat*)kvect)->values.begin() + movefrom);
		}
		break;
	}

	return kvect;
}

//----------------------GLOBAL STACK MANAGEMENT-------------------------------------------------
//----------------------THREADS MANAGEMENT-------------------------------------------------

extern char KIFOS[];

KifElement* PreGarbageFrame(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int);

Exported void KifGlobalThread::SetThreadid(threadhandle tid, int idthread) {
	ThreadLock _lock(kifThreadId);
	threads[tid] = idthread;
	setthreads[idthread] = true;
}

Exported void KifGlobalThread::Loopthreadid(int idthread) {
	while (true) {
		ThreadLock _lock(kifThreadId);
		if (setthreads.find(idthread) != setthreads.end()) {
			setthreads.erase(idthread);
			return;
		}
	}
}

Exported void KifGlobalThread::EraseThreadid(threadhandle tid) {
	ThreadLock _lock(kifThreadId);
	threads.erase(tid);
}

Exported int KifGlobalThread::GetThreadid() {
	if (Test() == false)
		return 0;

	threadhandle tid = _GETTHREADID();

	ThreadLock _lock(kifThreadId);
#if defined(WIN32) || defined(APPLE)
	hmap<threadhandle, int>::iterator it = threads.find(tid);
#else
	map<threadhandle, int>::iterator it = threads.find(tid);
#endif

	if (it == threads.end())
		return 0;
	return it->second;
}


KifElement* KifGlobalThread::SearchStackForLambda() {
	int id = GetThreadid();
	KifDomain* d = NULL;
	for (int i = THR[id]->threadstacks.size() - 1; i >= 0; i--) {
		d = THR[id]->threadstacks[i];
		if (d->type == kifFunctionLambda)
			return d;
	}
	return NULL;
}

void KifGlobalThread::SearchStackForLambdaDeclaration(KifCallFunctionGeneral* f) {
	int id = GetThreadid();
	KifDomain* d = NULL;
	bool first = true;
	bin_hash<KifElement*>::iterator it;
	for (int i = 1; i<THR[id]->threadstacks.size(); i += 2) {
		d = THR[id]->threadstacks[i];
		if (d->Body() != NULL && d->Body()->type == kifFunctionLambda) {
			if (first) {
				f->declarations = d->declarations;
				first = false;
			}
			else {
				for (it = d->declarations.begin(); it != d->declarations.end(); it++)
					f->declarations[it->first] = it->second;
			}
		}
	}
}


//-------------------------------------------------------------------------
//Stack management: to keep a track of all instructions

KifElement* KifGlobalThread::Stackerror(int idthread) {
	ThreadLock _lock(kifPrepareError);
	KifError* err = new KifError(NULL, "STK(888): Stack overflow");
	err->execution = false;
	return Returnerror(idthread, err);
}


//-----------------------------------------------------------------------------
LockedThread* KifGlobalThread::GetLoquet(int idvar) {
	ThreadLock _lock(kifLoquetThread);
	if (loquets.find(idvar) == loquets.end())
		return NULL;
	return loquets[idvar];
}

LockedThread* KifGlobalThread::AddLoquet(int idvar) {
	ThreadLock _lock(kifLoquetThread);
	if (loquets.find(idvar) == loquets.end())
		loquets[idvar] = new LockedThread;
	return loquets[idvar];
}

void KifGlobalThread::Releaseallloquet() {
	ThreadLock _lockq(kifLoquetThread);
	bin_hash<LockedThread*>::iterator it;
	for (it = loquets.begin(); it != loquets.end(); it++) {
		if (it->second != NULL)
			it->second->Released();
	}
}

//-----------------------------------------------------------------------------

threaditems::~threaditems() {
	clear(false);
	delete returnvalue;
#ifdef C11
	if (localthread != NULL) {
		if (localthread->joinable())
			localthread->detach();
		delete localthread;
	}
#endif

}

KifElement* threaditems::Setreturnvalue(KifElement* ke) {
	returnvalue->value = ke;
	ke->Putpopped(1);
	return returnvalue;
}

void threaditems::clear(bool display) {
	used = false;
	prologstack = 0;
	parentthreads = -1;
	threadstacks.clear();
	threadmessages.clear();
	joinedloquets.clear();
	error = false;
	if (errorptr != NULL) {
		ostringstream errmsg;
		errmsg << "(Thread:" << id << ") " << errorptr->String() << Endl;
		if (display) {
			ostream& os = *kifGlobals->os;
			os << errmsg.str();
			if (kifGlobals->doubledisplay)
				cerr << errmsg.str();
		}
		delete errorptr;
		errorptr = NULL;

		if (kifGlobals->THR[0]->errorptr == NULL)
			kifGlobals->THR[0]->errorptr = new KifError(NULL, errmsg.str());
		else
			kifGlobals->THR[0]->errorptr->value += errmsg.str();

		kifGlobals->THR[0]->error = true;
	}
}

void threaditems::cleanerror() {
	error = false;
	if (errorptr != NULL) {
		delete errorptr;
		errorptr = NULL;
	}
}

threaditems::threaditems(int i, int mxj) : joinedloquets(mxj) {
	returnvalue = new KifReturnValue;
	prologstack = 0;
	id = i;
	used = true;
	error = false;
	errorptr = NULL;
	parentthreads = NULL;
#ifdef C11
	localthread = NULL;
#endif

}

void KifGlobalThread::resizethreads(int t) {
	if (t<mxthreads)
		return;

	if (mxthreads == 0) {
		THR = new threaditems*[t];
		for (int i = 0; i<t; i++)
			THR[i] = NULL;
		mxthreads = t;
		return;
	}

	threaditems* th = THR[0];
	delete[] THR;
	THR = new threaditems*[t];
	for (int i = 0; i<t; i++)
		THR[i] = NULL;
	THR[0] = th;
	mxthreads = t;
}

string KifGlobalThread::Currentfilename() {
	return debuginfocurrent->Filename(debuginfocurrent->currentfile);
}

KifElement* KifGlobalThread::Returnerror(int id, KifError* err) {
	string filename;
	char ch[1024];
	long line;
	ThreadLock _lock(kifError);
	if (executionbreak)
		return kifNULL;

	if (THR[id]->error) {
		delete err;
		return kifRAISEERROR;
	}

	string message = err->value;
	line = debuginfocurrent->currentline;
	map<long, string>::reverse_iterator it;

	filename = debuginfocurrent->Filename(debuginfocurrent->currentfile);
	if (filename != "") {
		if (err->execution) {
			sprintf_s(ch, 1024, "%ld", line);
			message += " ";
			message += "at line:";
			message += " ";
			message += ch;
		}
		message += " ";
		message += "in"; 
		message += " " + filename;
	}

	err->value = message;
	THR[id]->error = true;
	THR[id]->errorptr = err;
	for (int i = THR[id]->stackelements.size() - 1; i >= 0; i--) {
		string mess = "Instruction";
		if (THR[id]->stackelements[i]->name != -1)
			mess = kifIdString[THR[id]->stackelements[i]->name];
		mess += ": ";
		line = THR[id]->stackelements[i]->line;
		filename = debuginfocurrent->Filename(THR[id]->stackelements[i]->file);
		if (filename != "")
			sprintf_s(ch, 1024, "line %ld in %s", line, STR(filename));
		else
			sprintf_s(ch, 1024, "line %ld", line);
		mess += ch;
		THR[id]->errorptr->stackmessages.push_back(mess);
	}

	return kifRAISEERROR;
}

KifElement* KifCode::Returnerror(string message) {
	ThreadLock _lock(kifPrepareError);
	KifError* err = new KifError(NULL, message);
	return kifGlobals->Returnerror(kifGlobals->GetThreadid(), err);
}

KifElement* KifCode::Returnerror(int id, string message) {
	ThreadLock _lock(kifPrepareError);
	KifError* err = new KifError(NULL, message);
	err->execution = false;
	return kifGlobals->Returnerror(id, err);
}

//----------------------------------------------------------------------------------------

KifElement* KifGlobalThread::Get(short n) {
	KifElement* v;

	int id = GetThreadid();
	vector<KifDomain*>& stack = THR[id]->threadstacks;
	for (int i = stack.size() - 1; i >= 0; i--) {
		v = stack[i]->Declared(n);
		if (v != NULL)
			return v;
	}

	if (kconstants->declarations.check(n))
		return kconstants->declarations[n];

	return NULL;
}

KifElement* KifGlobalThread::Getframe(short n) {
	KifElement* v;
	int id = GetThreadid();
	vector<KifDomain*>& stack = THR[id]->threadstacks;
	for (int i = stack.size() - 1; i >= 0; i--) {
		if (stack[i]->type != kifFrame)
			continue;
		v = stack[i]->Declared(n);
		if (v != NULL && v->type == kifFrame)
			return v;
	}
	return NULL;

}
//----------------------------------------------------------------------------------------
Exported KifCouple* KifGlobalThread::providecouple(KifElement* rec, KifIndex* idx, KifDomain* dom, KifElement* f) {
	if (Test()) {
		ThreadLock _lock(kifCouple);
		return KifCouple::Providecouple(rec, idx, dom, f);
	}
	return KifCouple::Providecouple(rec, idx, dom, f);
}

KifCouple* KifCouple::Providecouple(KifElement* rec, KifIndex* idx, KifDomain* dom, KifElement* f) {
	for (; ilocalgarbage<localgarbage.size(); ilocalgarbage++) {
		if (localgarbage[ilocalgarbage]->popped == 0) {
			localgarbage[ilocalgarbage]->Initialization(rec, idx, dom, f);
			localgarbage[ilocalgarbage]->popped = 1;
			return localgarbage[ilocalgarbage];
		}
	}
	return new KifCouple(dom->kifcode, rec, idx, dom, f);
}

//----------------------------------------------------------------------------------------
Exported int KifGlobalThread::Setthreadid(int idcaller) {
	int threadid = -1;
	for (int i = 1; i<mxthreads; i++) {
		if (THR[i] == NULL || THR[i]->used == false) {
			threadid = i;
			break;
		}
	}
	if (threadid == -1)
		return -1;

	if (THR[threadid] == NULL)
		THR[threadid] = new threaditems(threadid, kifGlobals->maxjoined);
	else {
		THR[threadid]->prologstack = 0;
		THR[threadid]->threadstacks.clear();
		THR[threadid]->threadmessages.clear();
		THR[threadid]->joinedloquets.clear();
		THR[threadid]->cleanerror();
		THR[threadid]->used = true;
	}
	THR[threadid]->threadstacks = THR[idcaller]->threadstacks;
	THR[threadid]->parentthreads = idcaller;
	return threadid;
}

void KifGlobalThread::GetStackInfos(KifCode* kifcode, KifVector* vect) {
	int id = GetThreadid();

	string fname = debuginfocurrent->Filename(debuginfocurrent->currentfile);
	long line;
	char ch[1025];
	KifBaseString localstring(NULL, NULL);
	localstring.kifcode = kifcode;
	for (int i = THR[id]->stackelements.size() - 1; i >= 0; i--) {
		localstring.value = "Instruction";
		if (THR[id]->stackelements[i]->name != -1)
			localstring.value = kifIdString[THR[id]->stackelements[i]->name];
		localstring.value += ": ";
		line = THR[id]->stackelements[i]->line;
		string filename = debuginfocurrent->Filename(THR[id]->stackelements[i]->file);
		sprintf_s(ch, 1024, "line %d in %s", line, STR(filename));
		sprintf_s(ch, 1024, "line %d in %s", line, STR(filename));
		localstring.value += ch;
		vect->Push(&localstring);
	}
}

bool KifGlobalThread::Getstackinfoid(KifCode* kifcode, int i, KifMap* kmap) {

	int id = GetThreadid();
	if (i >= THR[id]->stackelements.size())
		return false;
	KifDomain* dom;
	KifDomain* func;
	i *= 2;
	if (i >= THR[id]->threadstacks.size())
		return false;
	func = THR[id]->threadstacks[i];
	dom = THR[id]->threadstacks[i + 1];

	bin_hash<KifElement*>::iterator it;
	string n;
	KifBaseString localstring(NULL, NULL);
	localstring.kifcode = kifcode;
	for (it = kifcode->kstart->declarations.begin(); it != kifcode->kstart->declarations.end(); it++) {
		n = KifIdString(it->first);
		localstring.value = it->second->String();
		kmap->Push(n, &localstring);
	}
	if (dom != kifcode->kstart) {
		for (it = dom->declarations.begin(); it != dom->declarations.end(); it++) {
			n = KifIdString(it->first);
			localstring.value = it->second->String();
			kmap->Push(n, &localstring);
		}
	}

	for (it = func->declarations.begin(); it != func->declarations.end(); it++) {
		n = KifIdString(it->first);
		localstring.value = it->second->String();
		kmap->Push(n, &localstring);
	}
	return true;
}

bool KifGlobalThread::TestExternalType(string name) {
	if (kifExternalTypes.find(name) == kifExternalTypes.end())
		return false;
	return true;
}

KifType KifGlobalThread::KifAddNewType(string& name, KifExternalParse func, bool error) {
	if (kifExternalTypes.find(name) == kifExternalTypes.end()) {
		++kifLastType;
		KifType kt = (KifType)kifLastType;
		kifStringType[kt] = name;
		kifTypeString[name] = kt;
		kifExternalTypes[name] = func;
		equivalences[kt] = kt;
		return (KifType)kifLastType;
	}
	if (error)
		return kifVOID;

	return kifTypeString[name];
}

void KifGlobalThread::KifAssociateType(string name, KifExternalParse func, KifType kt) {
	equivalences[kt] = kt;
	kifStringType[kt] = name;
	kifTypeString[name] = kt;
	kifExternalTypes[name] = func;
}

//----------------------------------------------------------------------------------------
void KifGlobalThread::addloopkey(int id, KifLoopKey* ke) {
	ThreadLock _lock(kifLOOPITERATOR);
	loopkeys[id].push_back(ke);
}

KifLoopKey* KifGlobalThread::getloopkey(int id) {
	ThreadLock _lock(kifLOOPITERATOR);
	if (loopkeys.find(id) == loopkeys.end())
		return NULL;
	return loopkeys[id].back();
}

void KifGlobalThread::removeloopkey(int id) {
	ThreadLock _lock(kifLOOPITERATOR);
	loopkeys[id].pop_back();
	if (loopkeys[id].size() == 0)
		loopkeys.erase(id);
}

KifLoopKey::KifLoopKey(KifCode* k, KifBooleanLoop* L, int i) : KifElement(NULL, NULL, kifLOOPITERATOR) {
	id = i;
	kifcode = k;
	loop = L;
	kifGlobals->addloopkey(id, this);
}

KifElement* KifLoopKey::Key() {
	return kifcode->Provideinteger(loop->ite - 1);
}

KifLoopKey::~KifLoopKey() {
	kifGlobals->removeloopkey(id);
}
//----------------------------------------------------------------------------------------
void KifGlobalThread::TerminateAllLibraries(KifCode* kifcode) {
	KifExternalTerminateModule LibInitEntryPoint;
	hmap<string, KifExternalTerminateModule>::iterator it;
	for (it = kifGlobals->kifTerminateLibraries.begin(); it != kifGlobals->kifTerminateLibraries.end(); it++) {
		LibInitEntryPoint = it->second;
		(*LibInitEntryPoint)(kifcode);
	}
}

extern bool kifrestrandom;
void clear_output();

KifGlobalThread::KifGlobalThread(void* d) {
#ifndef C11
#ifdef WIN32
	for (int i = 0; i<locktablesize; i++)
		tableofcommonlocks[i] = 0;
#endif
#endif
	resetexternal = NULL;
	clear_output();
	mxthreads = 0;
	resizethreads(1000);
	KifPredicate::nb = PREDICATEDICO;
	maxjoined = 256;
	THR[0] = new threaditems(0, maxjoined);

	kifrestrandom = true;
	stacklimit = 500;
	data = d;
	cleandata = false;
	debugfunction = NULL;
	endofexecution = false;

	compile = false;
	evaluation = false;
	prive = false;
	common = false;
	through = false;
	constant = false;
	current = NULL;

	bnfkif = new bnf_kif;
	terminationfunction = NULL;
	executionbreak = 0;
	updatethread = true;
	displayfunction = NULL;
	displayobject = NULL;
	doubledisplay = false;
	flfrom = -1;
	flto = -1;
	garbaging = false;
	erroronkey = false;
	kstd = NULL;
	kerr = NULL;
	threadmode = false;
	kifLastType = kifLASTYPE;
	debuginfocurrent = &debuginfos;
	lastfileindex = 0;
	kconstants = NULL;
	gfail = NULL;
	gcut = NULL;
}


void KifGlobalThread::InitialisationIDs(KifCode* kifcodeptr) {
	static bool init = false;
	if (!init) {
		calls[0] = &KifCallFunctionHaskell::KIFHASKELL0;
		calls[1] = &KifCallFunctionHaskell::KIFHASKELL0;
		calls[2] = &KifCallFunctionHaskell::KIFHASKELL2;
		calls[3] = &KifCallFunctionHaskell::KIFHASKELL3;
		calls[4] = &KifCallFunctionHaskell::KIFHASKELL0;
		calls[5] = &KifCallFunctionHaskell::KIFHASKELL5;
		calls[6] = &KifCallFunctionHaskell::KIFHASKELLN;
		init = true;
	}

	//The order in which these elements are created should match their declaration as const short in kif.h
	KifNewId("null");
	KifNewId("this");
	KifNewId("sequence");
	KifNewId("catch");
	KifNewId("self");
	KifNewId("in");
	KifNewId("_call");
	KifNewId("_initial");
	KifNewId("size");
	KifNewId("<");
	KifNewId(">");
	KifNewId("==");
	KifNewId("!=");
	KifNewId("<=");
	KifNewId(">=");
	KifNewId("methods");
	KifNewId("vector");
	KifNewId("map");
	KifNewId("apply");
	KifNewId("synchronous");
	KifNewId("get");
	KifNewId("string");
	KifNewId("int");
	KifNewId("long");
	KifNewId("float");
	KifNewId("boolean");
	KifNewId("fraction");
	KifNewId("|");
	KifNewId("&");
	KifNewId("^");
	KifNewId("++");
	KifNewId("--");
	KifNewId("+");
	KifNewId("-");
	KifNewId("*");
	KifNewId("/");
	KifNewId("%");
	KifNewId(">>");
	KifNewId("<<");
	KifNewId("^^");
	KifNewId("[:]");
	KifNewId("[]");
	KifNewId("true");
	KifNewId("false");

	KifNewId("_paths");
	KifNewId("_args");
	KifNewId("_OS");
	KifNewId("_KIFMAIN");
	KifNewId("_sep");
	KifNewId("_loader");
	KifNewId("pop");

	KifNewId("_pi");
	KifNewId("π");
	KifNewId("_e");
	KifNewId("stdin");

	KifNewId("for");
	KifNewId("while");
	KifNewId("try");
	KifNewId("switch");
	KifNewId("if");

	KifNewId("_final");
	KifNewId("_run_as_thread");
	KifNewId("loadin");
	KifNewId("_endl");
	KifNewId("empty");

	KifNewId("smap");
	KifNewId("imap");
	KifNewId("fmap");
	KifNewId("svector");
	KifNewId("ivector");
	KifNewId("fvector");
	KifNewId("!");
	KifNewId("_trace");
	KifNewId("fail");
	KifNewId("_");
	KifNewId("!asserta");
	KifNewId("!assertz");
	KifNewId("!assertdb");
	KifNewId("!retract");
	KifNewId("!retractdb");
	KifNewId("!ponder");
	KifNewId("%pipe%");
	KifNewId("_predicatemode");
	KifNewId("short");
	KifNewId("waiton");
	KifNewId("trigger");

	KifNewId("sthrough");
	KifNewId("ithrough");
	KifNewId("fthrough");
	KifNewId("vthrough");

	KifNewId("idKIFPREDICATEENTREE");
	KifNewId("idPREDICATEDICO");

	gNULL = new KifConst(NULL, NULL, idnull);
	gUNIVERSAL = new KifConst(NULL, NULL, iduniversal);
	gNOELEMENT = new KifConst(NULL, NULL, idnoelement);

	gBREAK = new KifConst(NULL, NULL, KifNewId("_break"));
	gBREAKTRUE = new KifConst(NULL, NULL, KifNewId("_breaktrue"));
	gBREAKFALSE = new KifConst(NULL, NULL, KifNewId("_breakfalse"));

	gBREAKPOINT = new KifConst(NULL, NULL, KifNewId("_breakpoint"));

	gDEFAULT = new KifConst(NULL, NULL, KifNewId("_default"));

	gCONTINUE = new KifConst(NULL, NULL, KifNewId("_continue"));

	gNEG = new KifConst(NULL, NULL, KifNewId("_negation"));

	gMINUSONE = new KifBaseInteger(NULL, NULL, -1);
	gONE = new KifBaseInteger(NULL, NULL, 1);
	gZERO = new KifBaseInteger(NULL, NULL, 0);
	gEMPTYSTRING = new KifBaseString(NULL, NULL, "");

	gTRUE = new KifBaseBoolean(NULL, NULL, true);
	gFALSE = new KifBaseBoolean(NULL, NULL, false);


	gTRUE->popped = staticval;
	gFALSE->popped = staticval;
	gRAISEERROR = new KifConst(NULL, NULL, KifNewId("_RaiseError"));

	gRAISEERROR->kifcode = kifcodeptr;
	gNULL->kifcode = kifcodeptr;
	gNOELEMENT->kifcode = kifcodeptr;
	gUNIVERSAL->kifcode = kifcodeptr;
	gTRUE->kifcode = kifcodeptr;
	gFALSE->kifcode = kifcodeptr;
	gONE->kifcode = kifcodeptr;
	gMINUSONE->kifcode = kifcodeptr;
	gDEFAULT->kifcode = kifcodeptr;
	gBREAK->kifcode = kifcodeptr;
	gBREAKTRUE->kifcode = kifcodeptr;
	gBREAKFALSE->kifcode = kifcodeptr;
	gBREAKPOINT->kifcode = kifcodeptr;
	gCONTINUE->kifcode = kifcodeptr;
	gNEG->kifcode = kifcodeptr;

	gTypeSTRING = new KifBaseString(NULL, NULL);
	gTypeINTEGER = new KifInteger(NULL, NULL);
	gTypeFLOAT = new KifFloat(NULL, NULL);
	gTypeLONG = new KifLong(NULL, NULL);
	gTypeFRACTION = new KifFraction(NULL, NULL, 0, 1, true);
	gTypeBOOLEAN = new KifBoolean(NULL, NULL);

	gTypeSTRING->kifcode = kifcodeptr;
	gTypeINTEGER->kifcode = kifcodeptr;
	gTypeFLOAT->kifcode = kifcodeptr;
	gTypeLONG->kifcode = kifcodeptr;
	gTypeFRACTION->kifcode = kifcodeptr;
	gTypeBOOLEAN->kifcode = kifcodeptr;

	((KifConst*)gBREAKTRUE)->value = kifTRUE;
	((KifConst*)gBREAKFALSE)->value = kifFALSE;
	((KifConst*)gBREAK)->isbreak = true;
	((KifConst*)gBREAKTRUE)->isbreak = true;
	((KifConst*)gBREAKFALSE)->isbreak = true;

	idreturn = KifNewId("return");
	idreturnvariable = KifNewId("&return;");


}

void KifGlobalThread::InitialisationIDString(KifCode* kifcodeptr) {

	for (int i = 0; i <= kifLASTDECLARATION; i++)
		kifOperatorMath[i] = false;

	kifOperatorMath[kifPLUS] = true;
	kifOperatorMath[kifMINUS] = true;
	kifOperatorMath[kifMULTIPLY] = true;
	kifOperatorMath[kifDIVIDE] = true;
	kifOperatorMath[kifMOD] = true;
	kifOperatorMath[kifPOWER] = true;
	kifOperatorMath[kifUNION] = true;
	kifOperatorMath[kifXOR] = true;
	kifOperatorMath[kifINTERSECTION] = true;
	kifOperatorMath[kifSHIFTLEFT] = true;
	kifOperatorMath[kifSHIFTRIGHT] = true;

	kifStringType[kifPredicate] = "predicate";
	kifStringType[kifPredicateVariableInstance] = "predicateinstance";
	kifStringType[kifLoader] = "_loader";
	kifStringType[kifVariable] = "variable";
	kifStringType[kifDomain] = "domain";
	kifStringType[kifString] = "string";
	kifStringType[kifUString] = "ustring";
	kifStringType[kifTuple] = "tuple";
	kifStringType[kifRawString] = "rawstring";
	kifStringType[kifTime] = "time";
	kifStringType[kifAutomaton] = "automaton";	
	kifStringType[kifTransducer] = "transducer";
	kifStringType[kifGrammar] = "grammar";	
	kifStringType[kifVOID] = "available";
	kifStringType[kifInteger] = "int";
	kifStringType[kifByte] = "byte";
	kifStringType[kifLong] = "long";
	kifStringType[kifShort] = "short";
	kifStringType[kifFraction] = "fraction";
	kifStringType[kifBit] = "bit";
	kifStringType[kifBits] = "bits";
	kifStringType[kifDate] = "date";
	kifStringType[kifCall] = "call";

	kifStringType[kifFloat] = "float";
	kifStringType[kifLoopString] = "sloop";
	kifStringType[kifLoopUString] = "uloop";
	kifStringType[kifLoopBoolean] = "bloop";
	kifStringType[kifLoopInteger] = "iloop";
	kifStringType[kifLoopFloat] = "floop";
	kifStringType[kifBoolean] = "boolean";
	kifStringType[kifSelf] = "self";


	kifStringType[kifTuple] = "tuple";

	kifStringType[kifMap] = "map";
	kifStringType[kifUStringMap] = "mapu";
	kifStringType[kifIntegerMap] = "mapi";
	kifStringType[kifFloatMap] = "mapf";
	kifStringType[kifMapIntegerString] = "mapis";
	kifStringType[kifMapIntegerFloat] = "mapif";
	kifStringType[kifMapIntegerInteger] = "mapii";
	kifStringType[kifMapFloatString] = "mapfs";
	kifStringType[kifMapFloatFloat] = "mapff";
	kifStringType[kifMapFloatInteger] = "mapfi";
	kifStringType[kifMapUStringUString] = "mapuu";
	kifStringType[kifMapUStringInteger] = "mapui";
	kifStringType[kifMapUStringFloat] = "mapuf";
	kifStringType[kifMapStringString] = "mapss";
	kifStringType[kifMapStringFloat] = "mapsf";
	kifStringType[kifMapStringInteger] = "mapsi";

	kifStringType[kifMapIntegerUString] = "mapiu";
	kifStringType[kifMapFloatUString] = "mapfu";

	kifStringType[kifTreeMap] = "treemap";
	kifStringType[kifUStringTreeMap] = "treemapu";
	kifStringType[kifIntegerTreeMap] = "treemapi";
	kifStringType[kifFloatTreeMap] = "treemapf";
	kifStringType[kifTreeMapIntegerString] = "treemapis";
	kifStringType[kifTreeMapIntegerFloat] = "treemapif";
	kifStringType[kifTreeMapIntegerInteger] = "treemapii";
	kifStringType[kifTreeMapFloatString] = "treemapfs";
	kifStringType[kifTreeMapFloatFloat] = "treemapff";
	kifStringType[kifTreeMapFloatInteger] = "treemapfi";
	kifStringType[kifTreeMapStringString] = "treemapss";
	kifStringType[kifTreeMapUStringUString] = "treemapuu";
	kifStringType[kifTreeMapUStringInteger] = "treemapui";
	kifStringType[kifTreeMapUStringFloat] = "treemapuf";

	kifStringType[kifTreeMapIntegerUString] = "treemapiu";
	kifStringType[kifTreeMapFloatUString] = "treemapfu";

	kifStringType[kifTreeMapStringFloat] = "treemapsf";
	kifStringType[kifTreeMapStringInteger] = "treemapsi";

	kifStringType[kifPrimeMap] = "primemap";
	kifStringType[kifUStringPrimeMap] = "primemapu";
	kifStringType[kifIntegerPrimeMap] = "primemapi";
	
	kifStringType[kifIntegerBinMap] = "binmapi";
	kifStringType[kifBinMapIntegerInteger] = "binmapii";
	kifStringType[kifBinMapIntegerFloat] = "binmapif";
	kifStringType[kifBinMapIntegerString] = "binmapis";
	kifStringType[kifBinMapIntegerUString] = "binmapiu";

	kifStringType[kifFloatPrimeMap] = "primemapf";
	kifStringType[kifPrimeMapIntegerString] = "primemapis";
	kifStringType[kifPrimeMapIntegerFloat] = "primemapif";
	kifStringType[kifPrimeMapIntegerInteger] = "primemapii";
	kifStringType[kifPrimeMapFloatString] = "primemapfs";
	kifStringType[kifPrimeMapFloatFloat] = "primemapff";
	kifStringType[kifPrimeMapFloatInteger] = "primemapfi";
	kifStringType[kifPrimeMapStringString] = "primemapss";
	kifStringType[kifPrimeMapStringFloat] = "primemapsf";
	kifStringType[kifPrimeMapStringInteger] = "primemapsi";

	kifStringType[kifPrimeMapUStringUString] = "primemapuu";
	kifStringType[kifPrimeMapUStringInteger] = "primemapui";
	kifStringType[kifPrimeMapUStringFloat] = "primemapuf";
	kifStringType[kifPrimeMapIntegerUString] = "primemapiu";
	kifStringType[kifPrimeMapFloatUString] = "primemapfu";


	kifStringType[kifSet] = "set";
	kifStringType[kifSetString] = "sset";
	kifStringType[kifSetUString] = "uset";
	kifStringType[kifSetFloat] = "fset";
	kifStringType[kifSetInteger] = "iset";

	kifStringType[kifVectorString] = "svector";
	kifStringType[kifVectorUString] = "uvector";
	kifStringType[kifVectorFloat] = "fvector";
	kifStringType[kifVectorInteger] = "ivector";
	kifStringType[kifVectorByte] = "bvector";
	kifStringType[kifVector] = "vector";
	kifStringType[kifBaseVector] = "vector";
	kifStringType[kifTable] = "table";
	kifStringType[kifPredicate] = "predicate";
	kifStringType[kifPredicateTerm] = "term";
	kifStringType[kifList] = "list";
	kifStringType[kifIterator] = "iterator";
	kifStringType[kifFile] = "file";
	kifStringType[kifFileWide] = "wfile";

	kifStringType[kifAny] = "kifAny";
	kifStringType[kifCouple] = "kifCouple";
	kifStringType[kifCallFunction] = "kifCallFunction";
	kifStringType[kifFrameDeclaration] = "kifFrameDeclaration";
	kifStringType[kifFunctionDeclaration] = "kifFunctionDeclaration";
	kifStringType[kifInstance] = "kifInstance";
	kifStringType[kifIndex] = "kifIndex";
	kifStringType[kifPredefined] = "kifPredefined";
	kifStringType[kifVariableDeclaration] = "kifVariableDeclaration";
	kifStringType[kifFrame] = "kifFrame";
	kifStringType[kifFunction] = "kifFunction";
	kifStringType[kifProtectedFunction] = "kifProtectedFunction";
	kifStringType[kifInstruction] = "kifInstruction";

	kifTypeString["predicate"] = kifPredicate;

	kifTypeString["available"] = kifVOID;
	kifTypeString["predicateinstance"] = kifPredicateVariableInstance;
	kifTypeString["domain"] = kifDomain;
	kifTypeString["rawstring"] = kifRawString;
	kifTypeString["string"] = kifString;
	kifTypeString["ustring"] = kifUString;
	kifTypeString["tuple"] = kifTuple;
	kifTypeString["automaton"] = kifAutomaton;
	kifTypeString["transducer"] = kifTransducer;
	kifTypeString["grammar"] = kifGrammar;
	kifTypeString["_loader"] = kifLoader;
	kifTypeString["variable"] = kifVariable;
	kifTypeString["int"] = kifInteger;
	kifTypeString["long"] = kifLong;
	kifTypeString["short"] = kifShort;
	kifTypeString["fraction"] = kifFraction;
	kifTypeString["bit"] = kifBit;
	kifTypeString["bits"] = kifBits;
	kifTypeString["byte"] = kifByte;
	kifTypeString["date"] = kifDate;
	kifTypeString["call"] = kifCall;
	kifTypeString["float"] = kifFloat;
	kifTypeString["sloop"] = kifLoopString;
	kifTypeString["uloop"] = kifLoopUString;
	kifTypeString["bloop"] = kifLoopBoolean;
	kifTypeString["iloop"] = kifLoopInteger;
	kifTypeString["floop"] = kifLoopFloat;
	kifTypeString["boolean"] = kifBoolean;
	kifTypeString["bool"] = kifBoolean;
	kifTypeString["self"] = kifSelf;
	kifTypeString["auto"] = kifSelf;
	kifTypeString["let"] = kifSelf;
	kifTypeString["time"] = kifTime;

	kifTypeString["map"] = kifMap;
	kifTypeString["maps"] = kifMap;
	kifTypeString["mapu"] = kifUStringMap;
	kifTypeString["mapf"] = kifFloatMap;
	kifTypeString["mapi"] = kifIntegerMap;
	kifTypeString["ismap"] = kifMapIntegerString;
	kifTypeString["iimap"] = kifMapIntegerInteger;
	kifTypeString["ifmap"] = kifMapIntegerFloat;
	kifTypeString["fsmap"] = kifMapFloatString;
	kifTypeString["fimap"] = kifMapFloatInteger;
	kifTypeString["ffmap"] = kifMapFloatFloat;

	kifTypeString["smap"] = kifMapStringString;
	kifTypeString["imap"] = kifMapStringInteger;
	kifTypeString["fmap"] = kifMapStringFloat;
	kifTypeString["ssmap"] = kifMapStringString;
	kifTypeString["simap"] = kifMapStringInteger;
	kifTypeString["sfmap"] = kifMapStringFloat;
	kifTypeString["mapis"] = kifMapIntegerString;
	
	kifTypeString["uumap"] = kifMapUStringUString;
	kifTypeString["uimap"] = kifMapUStringInteger;
	kifTypeString["ufmap"] = kifMapUStringFloat;

	kifTypeString["iumap"] = kifMapIntegerUString;
	kifTypeString["ufmap"] = kifMapFloatUString;
	kifTypeString["mapiu"] = kifMapIntegerUString;
	kifTypeString["mapfu"] = kifMapFloatUString;

	kifTypeString["mapii"] = kifMapIntegerInteger;
	kifTypeString["mapif"] = kifMapIntegerFloat;
	kifTypeString["mapfs"] = kifMapFloatString;
	kifTypeString["mapfi"] = kifMapFloatInteger;
	kifTypeString["mapff"] = kifMapFloatFloat;
	kifTypeString["mapss"] = kifMapStringString;
	kifTypeString["mapuu"] = kifMapUStringUString;
	kifTypeString["mapui"] = kifMapUStringInteger;
	kifTypeString["mapuf"] = kifMapUStringFloat;
	kifTypeString["mapsi"] = kifMapStringInteger;
	kifTypeString["mapsf"] = kifMapStringFloat;

	kifTypeString["treemap"] = kifTreeMap;
	kifTypeString["treemaps"] = kifTreeMap;
	kifTypeString["treemapu"] = kifUStringTreeMap;
	kifTypeString["treemapf"] = kifFloatTreeMap;
	kifTypeString["treemapi"] = kifIntegerTreeMap;

	kifTypeString["istreemap"] = kifTreeMapIntegerString;
	kifTypeString["iitreemap"] = kifTreeMapIntegerInteger;
	kifTypeString["iftreemap"] = kifTreeMapIntegerFloat;
	kifTypeString["fstreemap"] = kifTreeMapFloatString;
	kifTypeString["fitreemap"] = kifTreeMapFloatInteger;
	kifTypeString["fftreemap"] = kifTreeMapFloatFloat;

	kifTypeString["streemap"] = kifTreeMapStringString;
	kifTypeString["itreemap"] = kifTreeMapStringInteger;
	kifTypeString["ftreemap"] = kifTreeMapStringFloat;
	kifTypeString["sstreemap"] = kifTreeMapStringString;
	kifTypeString["sitreemap"] = kifTreeMapStringInteger;
	kifTypeString["sftreemap"] = kifTreeMapStringFloat;
	kifTypeString["uutreemap"] = kifTreeMapUStringUString;
	kifTypeString["uitreemap"] = kifTreeMapUStringInteger;
	kifTypeString["uftreemap"] = kifTreeMapUStringFloat;
	
	kifTypeString["iutreemap"] = kifTreeMapIntegerUString;
	kifTypeString["uftreemap"] = kifTreeMapFloatUString;
	kifTypeString["treemapiu"] = kifTreeMapIntegerUString;
	kifTypeString["treemapfu"] = kifTreeMapFloatUString;

	kifTypeString["treemapis"] = kifTreeMapIntegerString;
	kifTypeString["treemapii"] = kifTreeMapIntegerInteger;
	kifTypeString["treemapif"] = kifTreeMapIntegerFloat;
	kifTypeString["treemapfs"] = kifTreeMapFloatString;
	kifTypeString["treemapfi"] = kifTreeMapFloatInteger;
	kifTypeString["treemapff"] = kifTreeMapFloatFloat;
	kifTypeString["treemapss"] = kifTreeMapStringString;
	kifTypeString["treemapuu"] = kifTreeMapUStringUString;
	kifTypeString["treemapui"] = kifTreeMapUStringInteger;
	kifTypeString["treemapuf"] = kifTreeMapUStringFloat;
	kifTypeString["treemapsi"] = kifTreeMapStringInteger;
	kifTypeString["treemapsf"] = kifTreeMapStringFloat;

	kifTypeString["primemap"] = kifPrimeMap;
	kifTypeString["primemaps"] = kifPrimeMap;
	kifTypeString["primemapu"] = kifUStringPrimeMap;
	kifTypeString["primemapf"] = kifFloatPrimeMap;
	kifTypeString["primemapi"] = kifIntegerPrimeMap;
	
	kifTypeString["binmapi"] = kifIntegerBinMap;
	kifTypeString["binmapii"] = kifBinMapIntegerInteger;
	kifTypeString["binmapif"] = kifBinMapIntegerFloat;
	kifTypeString["binmapis"] = kifBinMapIntegerString;
	kifTypeString["binmapiu"] = kifBinMapIntegerUString;
	kifTypeString["iibinmap"] = kifBinMapIntegerInteger;
	kifTypeString["ifbinmap"] = kifBinMapIntegerFloat;
	kifTypeString["isbinmap"] = kifBinMapIntegerString;
	kifTypeString["iubinmap"] = kifBinMapIntegerUString;

	kifTypeString["primemapuu"] = kifPrimeMapUStringUString;
	kifTypeString["uuprimemap"] = kifPrimeMapUStringUString;
	kifTypeString["primemapui"] = kifPrimeMapUStringInteger;
	kifTypeString["uiprimemap"] = kifPrimeMapUStringInteger;
	kifTypeString["primemapuf"] = kifPrimeMapUStringFloat;
	kifTypeString["ufprimemap"] = kifPrimeMapUStringFloat;
	kifTypeString["primemapiu"] = kifPrimeMapIntegerUString;
	kifTypeString["iuprimemap"] = kifPrimeMapIntegerUString;
	kifTypeString["primemapfu"] = kifPrimeMapFloatUString;
	kifTypeString["fuprimemap"] = kifPrimeMapFloatUString;

	kifTypeString["isprimemap"] = kifPrimeMapIntegerString;
	kifTypeString["iiprimemap"] = kifPrimeMapIntegerInteger;
	kifTypeString["ifprimemap"] = kifPrimeMapIntegerFloat;
	kifTypeString["fsprimemap"] = kifPrimeMapFloatString;
	kifTypeString["fiprimemap"] = kifPrimeMapFloatInteger;
	kifTypeString["ffprimemap"] = kifPrimeMapFloatFloat;

	kifTypeString["sprimemap"] = kifPrimeMapStringString;
	kifTypeString["iprimemap"] = kifPrimeMapStringInteger;
	kifTypeString["fprimemap"] = kifPrimeMapStringFloat;
	kifTypeString["ssprimemap"] = kifPrimeMapStringString;
	kifTypeString["siprimemap"] = kifPrimeMapStringInteger;
	kifTypeString["sfprimemap"] = kifPrimeMapStringFloat;
	kifTypeString["primemapis"] = kifPrimeMapIntegerString;
	kifTypeString["primemapii"] = kifPrimeMapIntegerInteger;
	kifTypeString["primemapif"] = kifPrimeMapIntegerFloat;
	kifTypeString["primemapfs"] = kifPrimeMapFloatString;
	kifTypeString["primemapfi"] = kifPrimeMapFloatInteger;
	kifTypeString["primemapff"] = kifPrimeMapFloatFloat;
	kifTypeString["primemapss"] = kifPrimeMapStringString;
	kifTypeString["primemapsi"] = kifPrimeMapStringInteger;
	kifTypeString["primemapsf"] = kifPrimeMapStringFloat;

	kifTypeString["sset"]=kifSetString;
	kifTypeString["uset"] = kifSetUString;
	kifTypeString["fset"]=kifSetFloat;
	kifTypeString["iset"] = kifSetInteger;
	kifTypeString["set"] = kifSet;

	kifTypeString["vector"] = kifVector;
	kifTypeString["table"] = kifTable;
	kifTypeString["predicate"] = kifPredicate;
	kifTypeString["term"] = kifPredicateTerm;
	kifTypeString["svector"] = kifVectorString;
	kifTypeString["uvector"] = kifVectorUString;
	kifTypeString["bvector"] = kifVectorByte;
	kifTypeString["ivector"] = kifVectorInteger;
	kifTypeString["fvector"] = kifVectorFloat;
	kifTypeString["list"] = kifList;
	kifTypeString["iterator"] = kifIterator;
	kifTypeString["file"] = kifFile;
	kifTypeString["wfile"] = kifFileWide;

	kifTypeString["kifAny"] = kifAny;
	kifTypeString["kifCouple"] = kifCouple;
	kifTypeString["kifCallFunction"] = kifCallFunction;
	kifTypeString["kifFrameDeclaration"] = kifFrameDeclaration;
	kifTypeString["kifFunctionDeclaration"] = kifFunctionDeclaration;
	kifTypeString["kifInstance"] = kifInstance;
	kifTypeString["kifIndex"] = kifIndex;
	kifTypeString["kifPredefined"] = kifPredefined;
	kifTypeString["kifVariableDeclaration"] = kifVariableDeclaration;
	kifTypeString["kifFrame"] = kifFrame;
	kifTypeString["kifFunction"] = kifFunction;
	kifTypeString["kifProtectedFunction"] = kifProtectedFunction;
	kifTypeString["kifInstruction"] = kifInstruction;

	kifTypes["kif"] = &KifCode::KifParseVarKifKiF;
	kifTypes["self"] = &KifCode::KifParseVarSelf;
	kifTypes["auto"] = &KifCode::KifParseVarSelf;
	kifTypes["let"] = &KifCode::KifParseVarSelf;
	kifTypes["int"] = &KifCode::KifParseVarInt;
	kifTypes["byte"] = &KifCode::KifParseVarByte;
	kifTypes["long"] = &KifCode::KifParseVarLong;
	kifTypes["short"] = &KifCode::KifParseVarShort;
	kifTypes["fraction"] = &KifCode::KifParseVarFraction;
	kifTypes["bit"] = &KifCode::KifParseVarBit;
	kifTypes["bits"] = &KifCode::KifParseVarBits;
	kifTypes["date"] = &KifCode::KifParseVarDate;
	kifTypes["time"] = &KifCode::KifParseVarTime;
	kifTypes["call"] = &KifCode::KifParseVarCall;
	kifTypes["rawstring"] = &KifCode::KifParseVarRawString;
	kifTypes["string"] = &KifCode::KifParseVarString;
	kifTypes["ustring"] = &KifCode::KifParseVarUString;
	kifTypes["automaton"] = &KifCode::KifParseVarAutomaton;
	kifTypes["transducer"] = &KifCode::KifParseVarTransducer;
	kifTypes["grammar"] = &KifCode::KifParseVarGrammar;
	kifTypes["predicate"] = &KifCode::KifParseVarPredicate;
	kifTypes["predicatevar"] = &KifCode::KifParseVarPredicate;
	kifTypes["term"] = &KifCode::KifParseVarPredicateTerm;
	kifTypes["float"] = &KifCode::KifParseVarFloat;
	kifTypes["sloop"] = &KifCode::KifParseVarLoopString;
	kifTypes["uloop"] = &KifCode::KifParseVarLoopUString;
	kifTypes["bloop"] = &KifCode::KifParseVarLoopBoolean;
	kifTypes["iloop"] = &KifCode::KifParseVarLoopInteger;
	kifTypes["floop"] = &KifCode::KifParseVarLoopFloat;
	kifTypes["vector"] = &KifCode::KifParseVarVector;
	kifTypes["table"] = &KifCode::KifParseVarTable;
	kifTypes["enum"] = &KifCode::KifParseVarEnum;
	kifTypes["list"] = &KifCode::KifParseVarList;

	kifTypes["sthrough"] = &KifCode::KifParseVarThrough;
	kifTypes["ithrough"] = &KifCode::KifParseVarThrough;
	kifTypes["fthrough"] = &KifCode::KifParseVarThrough;
	kifTypes["vthrough"] = &KifCode::KifParseVarThrough;

	kifTypes["tuple"] = &KifCode::KifParseVarTuple;

	kifTypes["set"] = &KifCode::KifParseVarSet;
	kifTypes["sset"] = &KifCode::KifParseVarSetString;
	kifTypes["uset"] = &KifCode::KifParseVarSetUString;
	kifTypes["iset"] = &KifCode::KifParseVarSetInteger;
	kifTypes["fset"] = &KifCode::KifParseVarSetFloat;

	kifTypes["map"] = &KifCode::KifParseVarMap;
	kifTypes["maps"] = &KifCode::KifParseVarMap;
	kifTypes["mapu"] = &KifCode::KifParseVarUStringMap;
	kifTypes["mapi"] = &KifCode::KifParseVarIntegerMap;
	kifTypes["mapf"] = &KifCode::KifParseVarFloatMap;

	kifTypes["smap"] = &KifCode::KifParseVarMapString;
	kifTypes["imap"] = &KifCode::KifParseVarMapInteger;
	kifTypes["fmap"] = &KifCode::KifParseVarMapFloat;
	kifTypes["ssmap"] = &KifCode::KifParseVarMapString;
	kifTypes["simap"] = &KifCode::KifParseVarMapInteger;
	kifTypes["sfmap"] = &KifCode::KifParseVarMapFloat;

	kifTypes["ismap"] = &KifCode::KifParseVarMapIntegerString;
	kifTypes["iimap"] = &KifCode::KifParseVarMapIntegerInteger;
	kifTypes["ifmap"] = &KifCode::KifParseVarMapIntegerFloat;
	kifTypes["fsmap"] = &KifCode::KifParseVarMapFloatString;
	kifTypes["fimap"] = &KifCode::KifParseVarMapFloatInteger;
	kifTypes["ffmap"] = &KifCode::KifParseVarMapFloatFloat;
	kifTypes["mapss"] = &KifCode::KifParseVarMapString;
	kifTypes["mapuu"] = &KifCode::KifParseVarMapUStringUString;
	kifTypes["mapui"] = &KifCode::KifParseVarMapUStringInteger;
	kifTypes["mapuf"] = &KifCode::KifParseVarMapUStringFloat;
	kifTypes["uumap"] = &KifCode::KifParseVarMapUStringUString;
	
	kifTypes["uimap"] = &KifCode::KifParseVarMapUStringInteger;
	kifTypes["ufmap"] = &KifCode::KifParseVarMapUStringFloat;

	kifTypes["iumap"] = &KifCode::KifParseVarMapIntegerUString;
	kifTypes["fumap"] = &KifCode::KifParseVarMapFloatUString;
	kifTypes["mapiu"] = &KifCode::KifParseVarMapIntegerUString;
	kifTypes["mapfu"] = &KifCode::KifParseVarMapFloatUString;

	kifTypes["mapsi"] = &KifCode::KifParseVarMapInteger;
	kifTypes["mapsf"] = &KifCode::KifParseVarMapFloat;
	kifTypes["mapis"] = &KifCode::KifParseVarMapIntegerString;
	kifTypes["mapii"] = &KifCode::KifParseVarMapIntegerInteger;
	kifTypes["mapif"] = &KifCode::KifParseVarMapIntegerFloat;
	kifTypes["mapfs"] = &KifCode::KifParseVarMapFloatString;
	kifTypes["mapfi"] = &KifCode::KifParseVarMapFloatInteger;
	kifTypes["mapff"] = &KifCode::KifParseVarMapFloatFloat;

	kifTypes["treemap"] = &KifCode::KifParseVarTreeMap;
	kifTypes["treemaps"] = &KifCode::KifParseVarTreeMap;
	kifTypes["treemapu"] = &KifCode::KifParseVarUStringTreeMap;
	kifTypes["treemapi"] = &KifCode::KifParseVarIntegerTreeMap;
	kifTypes["treemapf"] = &KifCode::KifParseVarFloatTreeMap;

	kifTypes["streemap"] = &KifCode::KifParseVarTreeMapString;
	kifTypes["itreemap"] = &KifCode::KifParseVarTreeMapInteger;
	kifTypes["ftreemap"] = &KifCode::KifParseVarTreeMapFloat;
	kifTypes["sstreemap"] = &KifCode::KifParseVarTreeMapString;
	kifTypes["sitreemap"] = &KifCode::KifParseVarTreeMapInteger;
	kifTypes["sftreemap"] = &KifCode::KifParseVarTreeMapFloat;

	kifTypes["istreemap"] = &KifCode::KifParseVarTreeMapIntegerString;
	kifTypes["iitreemap"] = &KifCode::KifParseVarTreeMapIntegerInteger;
	kifTypes["iftreemap"] = &KifCode::KifParseVarTreeMapIntegerFloat;
	kifTypes["fstreemap"] = &KifCode::KifParseVarTreeMapFloatString;
	kifTypes["fitreemap"] = &KifCode::KifParseVarTreeMapFloatInteger;
	kifTypes["fftreemap"] = &KifCode::KifParseVarTreeMapFloatFloat;
	kifTypes["treemapss"] = &KifCode::KifParseVarTreeMapString;
	kifTypes["treemapuu"] = &KifCode::KifParseVarTreeMapUStringUString;
	kifTypes["treemapui"] = &KifCode::KifParseVarTreeMapUStringInteger;
	kifTypes["treemapuf"] = &KifCode::KifParseVarTreeMapUStringFloat;

	kifTypes["uutreemap"] = &KifCode::KifParseVarTreeMapUStringUString;
	kifTypes["uitreemap"] = &KifCode::KifParseVarTreeMapUStringInteger;
	kifTypes["uftreemap"] = &KifCode::KifParseVarTreeMapUStringFloat;

	kifTypes["iutreemap"] = &KifCode::KifParseVarTreeMapIntegerUString;
	kifTypes["futreemap"] = &KifCode::KifParseVarTreeMapFloatUString;
	kifTypes["treemapiu"] = &KifCode::KifParseVarTreeMapIntegerUString;
	kifTypes["treemapfu"] = &KifCode::KifParseVarTreeMapFloatUString;

	kifTypes["treemapsi"] = &KifCode::KifParseVarTreeMapInteger;
	kifTypes["treemapsf"] = &KifCode::KifParseVarTreeMapFloat;
	kifTypes["treemapis"] = &KifCode::KifParseVarTreeMapIntegerString;
	kifTypes["treemapii"] = &KifCode::KifParseVarTreeMapIntegerInteger;
	kifTypes["treemapif"] = &KifCode::KifParseVarTreeMapIntegerFloat;
	kifTypes["treemapfs"] = &KifCode::KifParseVarTreeMapFloatString;
	kifTypes["treemapfi"] = &KifCode::KifParseVarTreeMapFloatInteger;
	kifTypes["treemapff"] = &KifCode::KifParseVarTreeMapFloatFloat;

	kifTypes["primemap"] = &KifCode::KifParseVarPrimeMap;
	kifTypes["primemaps"] = &KifCode::KifParseVarPrimeMap;
	kifTypes["primemapu"] = &KifCode::KifParseVarUStringPrimeMap;
	kifTypes["primemapi"] = &KifCode::KifParseVarIntegerPrimeMap;
	kifTypes["primemapf"] = &KifCode::KifParseVarFloatPrimeMap;

	kifTypes["binmapi"] = &KifCode::KifParseVarIntegerBinMap;
	kifTypes["binmapii"] = &KifCode::KifParseVarBinMapIntegerInteger;
	kifTypes["binmapif"] = &KifCode::KifParseVarBinMapIntegerFloat;
	kifTypes["binmapis"] = &KifCode::KifParseVarBinMapIntegerString;
	kifTypes["binmapiu"] = &KifCode::KifParseVarBinMapIntegerUString;
	kifTypes["iibinmap"] = &KifCode::KifParseVarBinMapIntegerInteger;
	kifTypes["ifbinmap"] = &KifCode::KifParseVarBinMapIntegerFloat;
	kifTypes["isbinmap"] = &KifCode::KifParseVarBinMapIntegerString;
	kifTypes["iubinmap"] = &KifCode::KifParseVarBinMapIntegerUString;


	kifTypes["sprimemap"] = &KifCode::KifParseVarPrimeMapString;
	kifTypes["iprimemap"] = &KifCode::KifParseVarPrimeMapInteger;
	kifTypes["fprimemap"] = &KifCode::KifParseVarPrimeMapFloat;
	kifTypes["ssprimemap"] = &KifCode::KifParseVarPrimeMapString;
	kifTypes["siprimemap"] = &KifCode::KifParseVarPrimeMapInteger;
	kifTypes["sfprimemap"] = &KifCode::KifParseVarPrimeMapFloat;

	kifTypes["isprimemap"] = &KifCode::KifParseVarPrimeMapIntegerString;
	kifTypes["iiprimemap"] = &KifCode::KifParseVarPrimeMapIntegerInteger;
	kifTypes["ifprimemap"] = &KifCode::KifParseVarPrimeMapIntegerFloat;
	kifTypes["fsprimemap"] = &KifCode::KifParseVarPrimeMapFloatString;
	kifTypes["fiprimemap"] = &KifCode::KifParseVarPrimeMapFloatInteger;
	kifTypes["ffprimemap"] = &KifCode::KifParseVarPrimeMapFloatFloat;
	kifTypes["primemapss"] = &KifCode::KifParseVarPrimeMapString;
	kifTypes["primemapsi"] = &KifCode::KifParseVarPrimeMapInteger;
	kifTypes["primemapsf"] = &KifCode::KifParseVarPrimeMapFloat;
	kifTypes["primemapis"] = &KifCode::KifParseVarPrimeMapIntegerString;
	kifTypes["primemapii"] = &KifCode::KifParseVarPrimeMapIntegerInteger;
	kifTypes["primemapif"] = &KifCode::KifParseVarPrimeMapIntegerFloat;
	kifTypes["primemapfs"] = &KifCode::KifParseVarPrimeMapFloatString;
	kifTypes["primemapfi"] = &KifCode::KifParseVarPrimeMapFloatInteger;
	kifTypes["primemapff"] = &KifCode::KifParseVarPrimeMapFloatFloat;

	kifTypes["primemapuu"] = &KifCode::KifParseVarPrimeMapUStringUString;
	kifTypes["primemapui"] = &KifCode::KifParseVarPrimeMapUStringInteger;
	kifTypes["primemapuf"] = &KifCode::KifParseVarPrimeMapUStringFloat;
	kifTypes["primemapiu"] = &KifCode::KifParseVarPrimeMapIntegerUString;
	kifTypes["primemapfu"] = &KifCode::KifParseVarPrimeMapFloatUString;
	kifTypes["uuprimemap"] = &KifCode::KifParseVarPrimeMapUStringUString;
	kifTypes["uiprimemap"] = &KifCode::KifParseVarPrimeMapUStringInteger;
	kifTypes["ufprimemap"] = &KifCode::KifParseVarPrimeMapUStringFloat;
	kifTypes["iuprimemap"] = &KifCode::KifParseVarPrimeMapIntegerUString;
	kifTypes["fuprimemap"] = &KifCode::KifParseVarPrimeMapFloatUString;

	kifTypes["svector"] = &KifCode::KifParseVarVectorString;
	kifTypes["uvector"] = &KifCode::KifParseVarVectorUString;
	kifTypes["ivector"] = &KifCode::KifParseVarVectorInteger;
	kifTypes["bvector"] = &KifCode::KifParseVarVectorByte;
	kifTypes["fvector"] = &KifCode::KifParseVarVectorFloat;
	kifTypes["boolean"] = &KifCode::KifParseVarBoolean;
	kifTypes["bool"] = &KifCode::KifParseVarBoolean;
	kifTypes["file"] = &KifCode::KifParseVarFile;
	kifTypes["wfile"] = &KifCode::KifParseVarFileWide;
	kifTypes["iterator"] = &KifCode::KifParseVarIterator;
	kifTypes["riterator"] = &KifCode::KifParseVarRIterator;
	kifTypes["tree"] = &KifCode::KifParseVarTree;


	kifFunctions["frame"] = &KifCode::KifParseFrame;
	kifFunctions["extension"] = &KifCode::KifParseExtension;
	kifFunctions["jmap"] = &KifCode::KifParseJSONMap;
	kifFunctions["valmap"] = &KifCode::KifParseValMap;
	kifFunctions["maptail"] = &KifCode::KifParseValMap;
	kifFunctions["valmaptail"] = &KifCode::KifParseValMap;
	kifFunctions["jvector"] = &KifCode::KifParseJSONVector;
	kifFunctions["valtuple"] = &KifCode::KifParseValTuple;
	kifFunctions["valvector"] = &KifCode::KifParseValVector;
	kifFunctions["valvectortail"] = &KifCode::KifParseValVector;
	kifFunctions["intentionvector"] = &KifCode::KifParseIntentionVector;
	kifFunctions["intentionwithdouble"] = &KifCode::KifParseIntentionDoubleVector;
	kifFunctions["onelisp"] = &KifCode::KifParseValVector;
	kifFunctions["onetag"] = &KifCode::KifParseValVector;
	kifFunctions["alist"] = &KifCode::KifParseList;
	kifFunctions["valtail"] = &KifCode::KifParseList;
	kifFunctions["dico"] = &KifCode::KifParseDico;
	kifFunctions["jdico"] = &KifCode::KifParseJSONDico;
	kifFunctions["dicoval"] = &KifCode::KifParseDico;
	kifFunctions["haskelldico"] = &KifCode::KifParseDico;

	//Haskell vector handling...
	kifFunctions["haskellcase"] = &KifCode::KifParseHaskellCase;
	kifFunctions["letmin"] = &KifCode::KifParseVarHaskellLetMin;
	kifFunctions["hinexpression"] = &KifCode::KifParseHaskellIn;
	kifFunctions["whereexpression"] = &KifCode::KifParseHaskellWhere;
	kifFunctions["haskellvector"] = &KifCode::KifParseHaskellVector;
	kifFunctions["haskellmap"] = &KifCode::KifParseHaskellMap;
	kifFunctions["haskellexpression"] = &KifCode::KifParseHaskellExpression;
	kifFunctions["haskellkeymap"] = &KifCode::KifParseHaskellExpression;
	kifFunctions["repeating"] = &KifCode::KifParseHaskellCycling;
	kifFunctions["cycling"] = &KifCode::KifParseHaskellCycling;
	kifFunctions["flipping"] = &KifCode::KifParseHaskellFlip;
	kifFunctions["mapping"] = &KifCode::KifParseHaskellMapping;
	kifFunctions["filtering"] = &KifCode::KifParseHaskellFilter;
	kifFunctions["taking"] = &KifCode::KifParseHaskellFilter;
	kifFunctions["zipping"] = &KifCode::KifParseHaskellZip;
	kifFunctions["pairing"] = &KifCode::KifParseHaskellZip;
	kifFunctions["folding"] = &KifCode::KifParseHaskellFold;
	kifFunctions["folding1"] = &KifCode::KifParseHaskellFold;
	kifFunctions["haskellbooling"] = &KifCode::KifParseHaskellBoolean;
	kifFunctions["hfunctioncall"] = &KifCode::KifParseHaskellFunctionCall;
	kifFunctions["hcompose"] = &KifCode::KifParseHaskellCompose;
	kifFunctions["let"] = &KifCode::KifParseMultiDeclaration;
	kifFunctions["hlambda"] = &KifCode::KifParseLambdaFunction;
	kifFunctions["evallambda"] = &KifCode::KifParseEvalLambda;
	kifFunctions["lambda"] = &KifCode::KifParseLambda;
	kifFunctions["telque"] = &KifCode::KifParseHaskell;


	//Predicate list reading...
	kifFunctions["apredicatelist"] = &KifCode::KifParseList;
	kifFunctions["merging"] = &KifCode::KifParseList;
	kifFunctions["alistnomerge"] = &KifCode::KifParseList;
	kifFunctions["valpredicatevector"] = &KifCode::KifParseValVector;
	kifFunctions["valmappredicate"] = &KifCode::KifParseValMap;
	kifFunctions["mapmerging"] = &KifCode::KifParseValMap;
	kifFunctions["predicatedico"] = &KifCode::KifParseDico;

	kifFunctions["fail"] = &KifCode::KifParsePredicateCut;
	kifFunctions["cut"] = &KifCode::KifParsePredicateCut;
	kifFunctions["dcg"] = &KifCode::KifParsePredicateDCG;
	kifFunctions["predicatefact"] = &KifCode::KifParsePredicateDefinition;
	kifFunctions["predicatedefinition"] = &KifCode::KifParsePredicateDefinition;
	kifFunctions["predicate"] = &KifCode::KifParsePredicateHead;
	kifFunctions["predicateexpression"] = &KifCode::KifParsePredicateExpression;
	kifFunctions["predicatevariable"] = &KifCode::KifParsePredicateVariable;
	kifFunctions["assertpredicate"] = &KifCode::KifParsePredicateAssert;
	kifFunctions["term"] = &KifCode::KifParsePredicateTerm;
	kifFunctions["tuple"] = &KifCode::KifParsePredicateTerm;


	kifFunctions["declaration"] = &KifCode::KifParseDeclaration;
	kifFunctions["multideclaration"] = &KifCode::KifParseMultiDeclaration;
	kifFunctions["function"] = &KifCode::KifParseFunction;
	kifFunctions["label"] = &KifCode::KifParseLabel;
	kifFunctions["autorun"] = &KifCode::KifParseFunction;
	kifFunctions["polynomial"] = &KifCode::KifParseFunction;
	kifFunctions["increment"] = &KifCode::KifParseIncrement;
	kifFunctions["affectation"] = &KifCode::KifParseAffectation;
	kifFunctions["affectationpredicate"] = &KifCode::KifParseAffectation;
	kifFunctions["operatoraffectation"] = &KifCode::KifParseOperator;
	kifFunctions["variable"] = &KifCode::KifParseVariable;
	kifFunctions["negated"] = &KifCode::KifParseNegativeVariable;
	kifFunctions["purevariable"] = &KifCode::KifParseVariable;
	kifFunctions["subfunc"] = &KifCode::KifParseSubFunc;
	kifFunctions["subfuncbis"] = &KifCode::KifParseSubFunc;
	kifFunctions["subvar"] = &KifCode::KifParseSubVar;
	kifFunctions["operator"] = &KifCode::KifParseOperator;
	kifFunctions["hoper"] = &KifCode::KifParseOperator;
	kifFunctions["comparator"] = &KifCode::KifParseOperator;
	kifFunctions["orand"] = &KifCode::KifParseOperator;
	kifFunctions["operation"] = &KifCode::KifParseOperation;
	kifFunctions["hoperation"] = &KifCode::KifParseOperation;
	kifFunctions["plusplus"] = &KifCode::KifParsePlusPlus;
	kifFunctions["operationin"] = &KifCode::KifParseOperationIn;
	kifFunctions["operationon"] = &KifCode::KifParseOperationIn;
	kifFunctions["comparison"] = &KifCode::KifParseComparison;
	kifFunctions["comparepredicate"] = &KifCode::KifParseComparisonPredicate;
	kifFunctions["predicatedatabase"] = &KifCode::KifParsePredicateDatabase;
	kifFunctions["abool"] = &KifCode::KifParseABoolean;
	kifFunctions["astring"] = &KifCode::KifParseAString;
	kifFunctions["punct"] = &KifCode::KifParseAString;
	kifFunctions["afullstring"] = &KifCode::KifParseAString;
	kifFunctions["astringdouble"] = &KifCode::KifParseAString;
	kifFunctions["astringsimple"] = &KifCode::KifParseAString;
	kifFunctions["anumber"] = &KifCode::KifParseANumber;
	kifFunctions["xnumber"] = &KifCode::KifParseAXNumber;
	kifFunctions["indexes"] = &KifCode::KifParseIndexes;
	kifFunctions["interval"] = &KifCode::KifParseInterval;
	kifFunctions["regularcall"] = &KifCode::KifParseCall;
	kifFunctions["purecall"] = &KifCode::KifParseCall;
	kifFunctions["haskellcall"] = &KifCode::KifParseCall;
	kifFunctions["predicatecall"] = &KifCode::KifParseCall;
	kifFunctions["nonlimited"] = &KifCode::KifNonLimited;
	kifFunctions["breakcall"] = &KifCode::KifParseUniqueCall;
	kifFunctions["returncall"] = &KifCode::KifParseUniqueCall;
	kifFunctions["breakpointcall"] = &KifCode::KifParseUniqueCall;
	kifFunctions["continuecall"] = &KifCode::KifParseUniqueCall;
	kifFunctions["optional"] = &KifCode::KifParseBloc;

	kifFunctions["test"] = &KifCode::KifParseTest;
	kifFunctions["localif"] = &KifCode::KifParseTest;
	kifFunctions["guard"] = &KifCode::KifParseTest;
	kifFunctions["testelif"] = &KifCode::KifParseTest;

	kifFunctions["switch"] = &KifCode::KifParseSwitch;
	kifFunctions["trycatch"] = &KifCode::KifParseTryCatch;
	kifFunctions["testswitch"] = &KifCode::KifParseTestSwitch;
	kifFunctions["loop"] = &KifCode::KifParseWhile;
	kifFunctions["doloop"] = &KifCode::KifParseUntil;
	kifFunctions["blocs"] = &KifCode::KifParsePredeclareFunctions;
	kifFunctions["for"] = &KifCode::KifParseFor;
	kifFunctions["blocfor"] = &KifCode::KifParseBlocFor;
	kifFunctions["forin"] = &KifCode::KifParseForIn;
	kifFunctions["negation"] = &KifCode::KifParseNegation;
	kifFunctions["booleanexpression"] = &KifCode::KifParseBoolean;
	kifFunctions["optionalboolean"] = &KifCode::KifParseBloc;

	//===================================================================
	kifOperators["=="] = kifEQUAL;
	kifOperators["<"] = kifINFERIOR;
	kifOperators[">"] = kifSUPERIOR;
	kifOperators[">="] = kifSUPEQU;
	kifOperators["<="] = kifINFEQU;
	kifOperators["="] = kifAFFECTATION;
	kifOperators["is"] = kifAFFECTATION;
	kifOperators["isnot"] = kifDIFFERENT;
	kifOperators["<>"] = kifDIFFERENT;
	kifOperators["~="] = kifDIFFERENT;
	kifOperators["!="] = kifDIFFERENT;
	kifOperators["+"] = kifPLUS;
	kifOperators["-"] = kifMINUS;
	kifOperators["*"] = kifMULTIPLY;
	kifOperators["/"] = kifDIVIDE;
	kifOperators["%"] = kifMOD;
	kifOperators["^^"] = kifPOWER;
	kifOperators["|"] = kifUNION;
	kifOperators["^"] = kifXOR;
	kifOperators["&"] = kifINTERSECTION;
	kifOperators["<<"] = kifSHIFTLEFT;
	kifOperators["<<<"] = kifSTREAM;
	kifOperators[">>"] = kifSHIFTRIGHT;
	kifOperators["+="] = kifPLUSEQU;
	kifOperators["++"] = kifPLUSPLUS;
	kifOperators["-="] = kifMINUSEQU;
	kifOperators["--"] = kifMINUSMINUS;
	kifOperators["&&&"] = kifMERGE;
	kifOperators["&&&="] = kifMERGEEQU;
	kifOperators["::"] = kifADD;
	kifOperators["::="] = kifADDEQU;
	kifOperators["*="] = kifMULTIPLYEQU;
	kifOperators["/="] = kifDIVIDEEQU;
	kifOperators["%="] = kifMODEQU;
	kifOperators["^^="] = kifPOWEREQU;
	kifOperators["<<="] = kifSHIFTLEFTEQU;
	kifOperators[">>="] = kifSHIFTRIGHTEQU;
	kifOperators["|="] = kifUNIONEQU;
	kifOperators["^="] = kifXOREQU;
	kifOperators["&="] = kifINTERSECTIONEQU;
	kifOperators["||"] = kifOR;
	kifOperators["&&"] = kifAND;
	kifOperators["or"] = kifOR;
	kifOperators["and"] = kifAND;
	kifOperators["in"] = kifIN;
	kifOperators["on"] = kifON;
	kifOperators["notin"] = kifNOTIN;

	kifOperatorString[kifEQUAL] = "==";
	kifOperatorString[kifINFERIOR] = "<";
	kifOperatorString[kifSUPERIOR] = ">";
	kifOperatorString[kifSUPEQU] = ">=";
	kifOperatorString[kifINFEQU] = "<=";
	kifOperatorString[kifAFFECTATION] = "=";
	kifOperatorString[kifDIFFERENT] = "<>";
	kifOperatorString[kifDIFFERENT] = "~=";
	kifOperatorString[kifDIFFERENT] = "!=";
	kifOperatorString[kifPLUS] = "+";
	kifOperatorString[kifMINUS] = "-";
	kifOperatorString[kifMULTIPLY] = "*";
	kifOperatorString[kifDIVIDE] = "/";
	kifOperatorString[kifMOD] = "%";
	kifOperatorString[kifPOWER] = "^^";
	kifOperatorString[kifUNION] = "|";
	kifOperatorString[kifXOR] = "^";
	kifOperatorString[kifINTERSECTION] = "&";
	kifOperatorString[kifSTREAM] = "<<<";
	kifOperatorString[kifSHIFTLEFT] = "<<";
	kifOperatorString[kifSHIFTRIGHT] = ">>";
	kifOperatorString[kifPLUSEQU] = "+=";
	kifOperatorString[kifPLUSPLUS] = "++";
	kifOperatorString[kifMINUSEQU] = "-=";
	kifOperatorString[kifMINUSMINUS] = "--";
	kifOperatorString[kifMULTIPLYEQU] = "*=";
	kifOperatorString[kifDIVIDEEQU] = "/=";
	kifOperatorString[kifMODEQU] = "%=";
	kifOperatorString[kifPOWEREQU] = "^^=";
	kifOperatorString[kifSHIFTLEFTEQU] = "<<=";
	kifOperatorString[kifSHIFTRIGHTEQU] = ">>=";
	kifOperatorString[kifUNIONEQU] = "|=";
	kifOperatorString[kifXOREQU] = "^=";
	kifOperatorString[kifINTERSECTIONEQU] = "&=";
	kifOperatorString[kifOR] = "||";
	kifOperatorString[kifAND] = "&&";
	kifOperatorString[kifIN] = "in";
	kifOperatorString[kifON] = "on";
	kifOperatorString[kifNOTIN] = "notin";


	accetabletypeparams.set(kifSelf);
	accetabletypeparams.set(kifAny);
	accetabletypeparams.set(kifnull);
	accetabletypeparams.set(kifVOID);
	accetabletypeparams.set(kifCallFunctionGeneral);
	accetabletypeparams.set(kifCallFunctionExternal);
	accetabletypeparams.set(kifCallFunctionThread);

	compatibilities[kifDomain].set(kifDomain);
	compatibilities[kifDomain].set(kifFrame);
	compatibilities[kifFrame].set(kifDomain);
	compatibilities[kifFrame].set(kifFrame);
	compatibilities[kifSubframe].set(kifDomain);
	compatibilities[kifSubframe].set(kifFrame);

	compatibilities[kifDomain].set(kifthis);
	compatibilities[kifFrame].set(kifthis);
	compatibilities[kifSubframe].set(kifthis);

	compatibilities[kifPredicateVariable].set(kifPredicateTerm);

	KifType numtypes[] = { kifInteger, kifLong, kifFraction, kifDate, kifTime, kifByte, kifBit, kifBits, kifBoolean, kifFloat, kifLASTYPE };
	KifType strtypes[] = { kifString, kifDate, kifTime, kifBaseString, kifStringPtr, kifLASTYPE };

	int xt = 0, yt;

	while (numtypes[xt] != kifLASTYPE) {
		yt = 0;
		while (numtypes[yt] != kifLASTYPE) {
			compatibilities[numtypes[xt]].set(numtypes[yt]);
			yt++;
		}
		xt++;
	}

	xt = 0;
	while (strtypes[xt] != kifLASTYPE) {
		yt = 0;
		while (strtypes[yt] != kifLASTYPE) {
			compatibilities[strtypes[xt]].set(strtypes[yt]);
			yt++;
		}
		xt++;
	}

	basevalues[kifVectorString] = kifEMPTYSTRING;
	basevalues[kifVectorUString] = kifEMPTYSTRING;
	basevalues[kifVectorByte] = kifZERO;
	basevalues[kifVectorInteger] = kifZERO;
	basevalues[kifVectorFloat] = kifZERO;

	basevalues[kifMapStringString] = kifEMPTYSTRING;
	basevalues[kifMapStringInteger] = kifZERO;
	basevalues[kifMapStringFloat] = kifZERO;
	basevalues[kifMapIntegerString] = kifEMPTYSTRING;
	basevalues[kifMapFloatString] = kifEMPTYSTRING;
	basevalues[kifMapUStringUString] = kifEMPTYSTRING;
	basevalues[kifMapUStringInteger] = kifZERO;
	basevalues[kifMapUStringFloat] = kifZERO;
	basevalues[kifMapIntegerUString] = kifEMPTYSTRING;
	basevalues[kifMapFloatUString] = kifEMPTYSTRING;
	basevalues[kifMapIntegerInteger] = kifZERO;
	basevalues[kifMapIntegerFloat] = kifZERO;
	basevalues[kifMapFloatInteger] = kifZERO;
	basevalues[kifMapFloatFloat] = kifZERO;

	basevalues[kifTreeMapStringString] = kifEMPTYSTRING;
	basevalues[kifTreeMapStringInteger] = kifZERO;
	basevalues[kifTreeMapStringFloat] = kifZERO;
	basevalues[kifTreeMapIntegerString] = kifEMPTYSTRING;
	basevalues[kifTreeMapFloatString] = kifEMPTYSTRING;
	basevalues[kifTreeMapUStringUString] = kifEMPTYSTRING;
	basevalues[kifTreeMapUStringInteger] = kifZERO;
	basevalues[kifTreeMapUStringFloat] = kifZERO;
	basevalues[kifTreeMapIntegerUString] = kifEMPTYSTRING;
	basevalues[kifTreeMapFloatUString] = kifEMPTYSTRING;
	basevalues[kifTreeMapIntegerInteger] = kifZERO;
	basevalues[kifTreeMapIntegerFloat] = kifZERO;
	basevalues[kifTreeMapFloatInteger] = kifZERO;
	basevalues[kifTreeMapFloatFloat] = kifZERO;

	basevalues[kifPrimeMapStringString] = kifEMPTYSTRING;
	basevalues[kifPrimeMapStringInteger] = kifZERO;
	basevalues[kifPrimeMapStringFloat] = kifZERO;
	basevalues[kifPrimeMapIntegerString] = kifEMPTYSTRING;
	basevalues[kifPrimeMapFloatString] = kifEMPTYSTRING;
	basevalues[kifPrimeMapUStringUString] = kifEMPTYSTRING;
	basevalues[kifPrimeMapUStringInteger] = kifZERO;
	basevalues[kifPrimeMapUStringFloat] = kifZERO;
	basevalues[kifPrimeMapIntegerUString] = kifEMPTYSTRING;
	basevalues[kifPrimeMapFloatUString] = kifEMPTYSTRING;
	basevalues[kifPrimeMapIntegerInteger] = kifZERO;
	basevalues[kifPrimeMapIntegerFloat] = kifZERO;
	basevalues[kifPrimeMapFloatInteger] = kifZERO;
	basevalues[kifPrimeMapFloatFloat] = kifZERO;
	
	basevalues[kifBinMapIntegerString] = kifEMPTYSTRING;
	basevalues[kifBinMapIntegerUString] = kifEMPTYSTRING;
	basevalues[kifBinMapIntegerInteger] = kifZERO;
	basevalues[kifBinMapIntegerFloat] = kifZERO;


	KifType vectortypes[] = { kifVector, kifVectorString, kifVectorUString, kifVectorInteger, kifVectorByte, kifVectorFloat, kifVectorPtr, kifBaseVector, kifTable, kifBaseVectorMerge, kifLASTYPE };

	KifType maptypes[] = { kifMap, kifMapStringString, kifMapStringInteger, kifMapStringFloat,
		kifIntegerMap, kifMapIntegerString, kifMapIntegerInteger, kifMapIntegerFloat,
		kifFloatMap, kifMapFloatString, kifMapFloatInteger, kifMapFloatFloat,
		kifUStringMap, kifMapUStringUString, kifMapUStringInteger, kifMapUStringFloat,
		kifMapIntegerUString, kifMapFloatUString,

		kifTreeMap, kifTreeMapStringString, kifTreeMapStringInteger, kifTreeMapStringFloat,
		kifIntegerTreeMap, kifTreeMapIntegerString, kifTreeMapIntegerInteger, kifTreeMapIntegerFloat,
		kifFloatTreeMap, kifTreeMapFloatString, kifTreeMapFloatInteger, kifTreeMapFloatFloat,
		kifUStringTreeMap, kifTreeMapUStringUString, kifTreeMapUStringInteger, kifTreeMapUStringFloat,
		kifTreeMapIntegerUString, kifTreeMapFloatUString,

		kifPrimeMap, kifPrimeMapStringString, kifPrimeMapStringInteger, kifPrimeMapStringFloat,
		kifIntegerPrimeMap, kifPrimeMapIntegerString, kifPrimeMapIntegerInteger, kifPrimeMapIntegerFloat,
		kifFloatPrimeMap, kifPrimeMapFloatString, kifPrimeMapFloatInteger, kifPrimeMapFloatFloat,
		kifUStringPrimeMap, kifPrimeMapUStringUString, kifPrimeMapUStringInteger, kifPrimeMapUStringFloat,
		kifPrimeMapIntegerUString, kifPrimeMapFloatUString,

		kifIntegerBinMap, kifBinMapIntegerInteger, kifBinMapIntegerFloat, kifBinMapIntegerString, kifBinMapIntegerUString,

		kifMapPtr, kifBaseMap, kifLASTYPE };

	xt = 0;
	while (vectortypes[xt] != kifLASTYPE) {
		yt = 0;
		while (vectortypes[yt] != kifLASTYPE) {
			compatibilities[vectortypes[xt]].set(vectortypes[yt]);
			yt++;
		}
		xt++;
	}
	xt = 0;
	while (maptypes[xt] != kifLASTYPE) {
		yt = 0;
		while (maptypes[yt] != kifLASTYPE) {
			compatibilities[maptypes[xt]].set(maptypes[yt]);
			yt++;
		}
		xt++;
	}
}

bool KifGlobalThread::compatible(KifElement* a, KifElement* b) {
	return compatibilities[b->type].check(a->type);
}

bool KifGlobalThread::compatible(short ta, short tb) {
	return compatibilities[tb].check(ta);
}

Exported void InitMapBooleanString(hmap<short, bool>& themap, string s, bool method) {
	short id = KifNewId(s);
	themap[id] = true;
	if (method)
		kifGlobals->kifMethods[id] = true;
}

void KifGlobalThread::InitialisationInitMaps(KifCode* kifcodeptr) {
	InitMapBooleanString(kifPredefinedFunctions, "_compatibilities",false);
	InitMapBooleanString(kifPredefinedFunctions, "json");
	InitMapBooleanString(kifPredefinedFunctions, "string");
	InitMapBooleanString(kifPredefinedFunctions, "int");
	InitMapBooleanString(kifPredefinedFunctions, "long");
	InitMapBooleanString(kifPredefinedFunctions, "short");
	InitMapBooleanString(kifPredefinedFunctions, "float");
	InitMapBooleanString(kifPredefinedFunctions, "boolean");
	InitMapBooleanString(kifPredefinedFunctions, "methods",false);
	InitMapBooleanString(kifPredefinedFunctions, "infos",false);
	InitMapBooleanString(kifPredefinedFunctions, "isa",false);
	InitMapBooleanString(kifPredefinedFunctions, "type",false);
	InitMapBooleanString(kifPredefinedFunctions, "iscontainer", false);
	InitMapBooleanString(kifPredefinedFunctions, "ismap", false);
	InitMapBooleanString(kifPredefinedFunctions, "isvector", false);
	InitMapBooleanString(kifPredefinedFunctions, "name");
	InitMapBooleanString(kifPredefinedFunctions, "_trace");
	InitMapBooleanString(kifPredefinedFunctions, "_predicatemode",false);
	InitMapBooleanString(kifPredefinedFunctions, "size");
	InitMapBooleanString(kifPredefinedFunctions, "min", false);
	InitMapBooleanString(kifPredefinedFunctions, "max", false);
	InitMapBooleanString(kifPredefinedFunctions, "clear");
	InitMapBooleanString(kifPredefinedFunctions, "vector");
	InitMapBooleanString(kifPredefinedFunctions, "map");
	InitMapBooleanString(kifPredefinedFunctions, "addtowaiton");
	InitMapBooleanString(kifPredefinedFunctions, "waiton");
	InitMapBooleanString(kifPredefinedFunctions, "trigger");
	InitMapBooleanString(kifPredefinedFunctions, "_id", false);

	InitMapBooleanString(kifGrammarFunctions, "load");
	InitMapBooleanString(kifGrammarFunctions, "apply");
	InitMapBooleanString(kifGrammarFunctions, "methods", false);
	InitMapBooleanString(kifGrammarFunctions, "infos", false);

	InitMapBooleanString(kifAutomatonFunctions, "load");
	InitMapBooleanString(kifAutomatonFunctions, "dump");
	InitMapBooleanString(kifAutomatonFunctions, "loadlemma");
	InitMapBooleanString(kifAutomatonFunctions, "model");
	InitMapBooleanString(kifAutomatonFunctions, "look");
	InitMapBooleanString(kifAutomatonFunctions, "spotfinalstate");
	InitMapBooleanString(kifAutomatonFunctions, "loadcompact");
	InitMapBooleanString(kifAutomatonFunctions, "store");
	InitMapBooleanString(kifAutomatonFunctions, "size");
	InitMapBooleanString(kifAutomatonFunctions, "setcodefeature");
	InitMapBooleanString(kifAutomatonFunctions, "editdistance");
	InitMapBooleanString(kifAutomatonFunctions, "clear");
	InitMapBooleanString(kifAutomatonFunctions, "add");
	InitMapBooleanString(kifAutomatonFunctions, "methods",false);
	InitMapBooleanString(kifAutomatonFunctions, "infos",false);
	InitMapBooleanString(kifAutomatonFunctions, "settransformationmap");

	InitMapBooleanString(kifTransducerFunctions, "load");
	InitMapBooleanString(kifTransducerFunctions, "add");
	InitMapBooleanString(kifTransducerFunctions, "build");
	InitMapBooleanString(kifTransducerFunctions, "store");
	InitMapBooleanString(kifTransducerFunctions, "compilergx");
	InitMapBooleanString(kifTransducerFunctions, "lookup");
	InitMapBooleanString(kifTransducerFunctions, "lookdown");
	InitMapBooleanString(kifTransducerFunctions, "process");


	InitMapBooleanString(kifTreeFunctions, "json");
	InitMapBooleanString(kifTreeFunctions, "string");
	InitMapBooleanString(kifTreeFunctions, "methods",false);
	InitMapBooleanString(kifTreeFunctions, "infos",false);
	InitMapBooleanString(kifTreeFunctions, "float");
	InitMapBooleanString(kifTreeFunctions, "boolean");
	InitMapBooleanString(kifTreeFunctions, "int");
	InitMapBooleanString(kifTreeFunctions, "long");
	InitMapBooleanString(kifTreeFunctions, "short");
	InitMapBooleanString(kifTreeFunctions, "map");
	InitMapBooleanString(kifTreeFunctions, "vector");
	InitMapBooleanString(kifTreeFunctions, "sister");
	InitMapBooleanString(kifTreeFunctions, "editdistance");
	InitMapBooleanString(kifTreeFunctions, "previous");
	InitMapBooleanString(kifTreeFunctions, "mother");
	InitMapBooleanString(kifTreeFunctions, "daughter");
	InitMapBooleanString(kifTreeFunctions, "lastnode");
	InitMapBooleanString(kifTreeFunctions, "depth");
	InitMapBooleanString(kifTreeFunctions, "prune");
	InitMapBooleanString(kifTreeFunctions, "isolate");

	InitMapBooleanString(kifStringFunctions, "string");
	InitMapBooleanString(kifStringFunctions, "json");
	InitMapBooleanString(kifStringFunctions, "int");
	InitMapBooleanString(kifStringFunctions, "long");
	InitMapBooleanString(kifStringFunctions, "short");
	InitMapBooleanString(kifStringFunctions, "float");
	InitMapBooleanString(kifStringFunctions, "boolean");
	InitMapBooleanString(kifStringFunctions, "vector");
	InitMapBooleanString(kifStringFunctions, "map");

	InitMapBooleanString(kifStringFunctions, "ioredirect", false);
	
	InitMapBooleanString(kifStringFunctions, "hash", false);
	InitMapBooleanString(kifStringFunctions, "size");
	InitMapBooleanString(kifStringFunctions, "get");
	InitMapBooleanString(kifStringFunctions, "reverse");
	InitMapBooleanString(kifStringFunctions, "ord", false);
	InitMapBooleanString(kifStringFunctions, "bytes", false);
	InitMapBooleanString(kifStringFunctions, "format");
	InitMapBooleanString(kifStringFunctions, "base");
	InitMapBooleanString(kifStringFunctions, "fill");
	InitMapBooleanString(kifStringFunctions, "padding");

	InitMapBooleanString(kifStringFunctions, "vector");
	InitMapBooleanString(kifStringFunctions, "parse");
	InitMapBooleanString(kifStringFunctions, "push");
	InitMapBooleanString(kifStringFunctions, "pop");
	InitMapBooleanString(kifStringFunctions, "sizeb");
	InitMapBooleanString(kifStringFunctions, "lisp");
	InitMapBooleanString(kifStringFunctions, "tags");
	InitMapBooleanString(kifStringFunctions, "resize");
	InitMapBooleanString(kifStringFunctions, "regexip");
	InitMapBooleanString(kifStringFunctions, "evaluate");
	InitMapBooleanString(kifStringFunctions, "html");
	InitMapBooleanString(kifStringFunctions, "tohtml");
	InitMapBooleanString(kifStringFunctions, "toxml");
	InitMapBooleanString(kifStringFunctions, "replaceregexip");
	InitMapBooleanString(kifStringFunctions, "levenshtein");
	InitMapBooleanString(kifStringFunctions, "editdistance");
	InitMapBooleanString(kifStringFunctions, "replace");
	InitMapBooleanString(kifStringFunctions, "getstd");
	InitMapBooleanString(kifStringFunctions, "geterr");
	InitMapBooleanString(kifStringFunctions, "split");
	InitMapBooleanString(kifStringFunctions, "splite");
	InitMapBooleanString(kifStringFunctions, "tokenize");
	InitMapBooleanString(kifStringFunctions, "stokenize");
	InitMapBooleanString(kifStringFunctions, "count");
	InitMapBooleanString(kifStringFunctions, "find");
	InitMapBooleanString(kifStringFunctions, "rfind");
	InitMapBooleanString(kifStringFunctions, "removefirst");
	InitMapBooleanString(kifStringFunctions, "removelast");
	InitMapBooleanString(kifStringFunctions, "isutf8");
	InitMapBooleanString(kifStringFunctions, "ngrams");
	InitMapBooleanString(kifStringFunctions, "utf8");
	InitMapBooleanString(kifStringFunctions, "latin");
	InitMapBooleanString(kifStringFunctions, "dos");
	InitMapBooleanString(kifStringFunctions, "dostoutf8");
	InitMapBooleanString(kifStringFunctions, "left");
	InitMapBooleanString(kifStringFunctions, "byteposition");
	InitMapBooleanString(kifStringFunctions, "charposition");
	InitMapBooleanString(kifStringFunctions, "right");
	InitMapBooleanString(kifStringFunctions, "mid");
	InitMapBooleanString(kifStringFunctions, "isalpha");
	InitMapBooleanString(kifStringFunctions, "isconsonant");
	InitMapBooleanString(kifStringFunctions, "isemoji");
	InitMapBooleanString(kifStringFunctions, "isvowel");
	InitMapBooleanString(kifStringFunctions, "ispunctuation");
	InitMapBooleanString(kifStringFunctions, "isdigit");
	InitMapBooleanString(kifStringFunctions, "extract");
	InitMapBooleanString(kifStringFunctions, "isupper");
	InitMapBooleanString(kifStringFunctions, "islower");
	InitMapBooleanString(kifStringFunctions, "upper");
	InitMapBooleanString(kifStringFunctions, "deaccentuate");
	InitMapBooleanString(kifStringFunctions, "emoji");
	InitMapBooleanString(kifStringFunctions, "lower");
	InitMapBooleanString(kifStringFunctions, "trim");
	InitMapBooleanString(kifStringFunctions, "trimleft");
	InitMapBooleanString(kifStringFunctions, "trimright");	
	InitMapBooleanString(kifStringFunctions, "last");
	InitMapBooleanString(kifStringFunctions, "insert");
	InitMapBooleanString(kifStringFunctions, "isa",false);
	InitMapBooleanString(kifStringFunctions, "methods",false);
	InitMapBooleanString(kifStringFunctions, "infos",false);
	InitMapBooleanString(kifStringFunctions, "type",false);
	InitMapBooleanString(kifStringFunctions, "clear");


	InitMapBooleanString(kifPredicateFunctions, "query");
	InitMapBooleanString(kifPredicateFunctions, "store");
	InitMapBooleanString(kifPredicateFunctions, "remove");
	InitMapBooleanString(kifPredicateFunctions, "weight");



	InitMapBooleanString(kifVectorFunctions, "size");
	InitMapBooleanString(kifVectorFunctions, "editdistance");
	InitMapBooleanString(kifVectorFunctions, "resize");
	InitMapBooleanString(kifVectorFunctions, "duplicate");
	InitMapBooleanString(kifVectorFunctions, "reverse");
	InitMapBooleanString(kifVectorFunctions, "move");
	InitMapBooleanString(kifVectorFunctions, "insert");
	InitMapBooleanString(kifVectorFunctions, "push");
	InitMapBooleanString(kifVectorFunctions, "ngrams");
	InitMapBooleanString(kifVectorFunctions, "merge");
	InitMapBooleanString(kifVectorFunctions, "predicate");
	InitMapBooleanString(kifVectorFunctions, "bytes");
	InitMapBooleanString(kifVectorFunctions, "totext");
	InitMapBooleanString(kifVectorFunctions, "flatten");
	InitMapBooleanString(kifVectorFunctions, "test");
	InitMapBooleanString(kifVectorFunctions, "join");
	InitMapBooleanString(kifVectorFunctions, "reserve");
	InitMapBooleanString(kifVectorFunctions, "pop");
	InitMapBooleanString(kifVectorFunctions, "last");
	InitMapBooleanString(kifVectorFunctions, "min", false);
	InitMapBooleanString(kifVectorFunctions, "max", false);
	InitMapBooleanString(kifVectorFunctions, "sort");
	InitMapBooleanString(kifVectorFunctions, "permute");
	InitMapBooleanString(kifVectorFunctions, "shuffle");
	InitMapBooleanString(kifVectorFunctions, "unique");
	InitMapBooleanString(kifVectorFunctions, "sortint");
	InitMapBooleanString(kifVectorFunctions, "sortfloat");
	InitMapBooleanString(kifVectorFunctions, "sortstring");
	InitMapBooleanString(kifVectorFunctions, "sortlong");
	InitMapBooleanString(kifVectorFunctions, "sum");
	InitMapBooleanString(kifVectorFunctions, "product");
	InitMapBooleanString(kifVectorFunctions, "clear");
	InitMapBooleanString(kifVectorFunctions, "apply");
	InitMapBooleanString(kifVectorFunctions, "isa",false);
	InitMapBooleanString(kifVectorFunctions, "methods",false);
	InitMapBooleanString(kifVectorFunctions, "infos",false);
	InitMapBooleanString(kifVectorFunctions, "type",false);
	InitMapBooleanString(kifVectorFunctions, "string");
	InitMapBooleanString(kifVectorFunctions, "json");
	InitMapBooleanString(kifVectorFunctions, "int");
	InitMapBooleanString(kifVectorFunctions, "long");
	InitMapBooleanString(kifVectorFunctions, "short");
	InitMapBooleanString(kifVectorFunctions, "float");
	InitMapBooleanString(kifVectorFunctions, "map");
	InitMapBooleanString(kifVectorFunctions, "vector");
	InitMapBooleanString(kifVectorFunctions, "boolean");

	InitMapBooleanString(kifListFunctions, "size");
	InitMapBooleanString(kifListFunctions, "duplicate");
	InitMapBooleanString(kifListFunctions, "editdistance");
	InitMapBooleanString(kifListFunctions, "reverse");
	InitMapBooleanString(kifListFunctions, "permute");
	InitMapBooleanString(kifListFunctions, "shuffle");
	InitMapBooleanString(kifListFunctions, "insert");
	InitMapBooleanString(kifListFunctions, "flatten");
	InitMapBooleanString(kifListFunctions, "pushfirst");
	InitMapBooleanString(kifListFunctions, "pop");
	InitMapBooleanString(kifListFunctions, "poplast");
	InitMapBooleanString(kifListFunctions, "pushlast");
	InitMapBooleanString(kifListFunctions, "popfirst");
	InitMapBooleanString(kifListFunctions, "unique");
	InitMapBooleanString(kifListFunctions, "merge");
	InitMapBooleanString(kifListFunctions, "sum");
	InitMapBooleanString(kifListFunctions, "test");
	InitMapBooleanString(kifListFunctions, "product");
	InitMapBooleanString(kifListFunctions, "min", false);
	InitMapBooleanString(kifListFunctions, "max", false);
	InitMapBooleanString(kifListFunctions, "join");
	InitMapBooleanString(kifListFunctions, "apply");
	InitMapBooleanString(kifListFunctions, "last");
	InitMapBooleanString(kifListFunctions, "first");
	InitMapBooleanString(kifListFunctions, "clear");
	InitMapBooleanString(kifListFunctions, "isa",false);
	InitMapBooleanString(kifListFunctions, "methods",false);
	InitMapBooleanString(kifListFunctions, "infos",false);
	InitMapBooleanString(kifListFunctions, "type",false);
	InitMapBooleanString(kifListFunctions, "string");
	InitMapBooleanString(kifListFunctions, "json");
	InitMapBooleanString(kifListFunctions, "int");
	InitMapBooleanString(kifListFunctions, "vector");
	InitMapBooleanString(kifListFunctions, "map");
	InitMapBooleanString(kifListFunctions, "long");
	InitMapBooleanString(kifListFunctions, "short");
	InitMapBooleanString(kifListFunctions, "float");
	InitMapBooleanString(kifListFunctions, "boolean");


	InitMapBooleanString(kifKiFFunctions, "load");
	InitMapBooleanString(kifKiFFunctions, "run");
	InitMapBooleanString(kifKiFFunctions, "methods",false);
	InitMapBooleanString(kifKiFFunctions, "infos",false);
	InitMapBooleanString(kifKiFFunctions, "runasthread");
	InitMapBooleanString(kifKiFFunctions, "kstart", false);
	InitMapBooleanString(kifKiFFunctions, "runend");
	InitMapBooleanString(kifKiFFunctions, "debugfunction");
	InitMapBooleanString(kifKiFFunctions, "debugclose");
	InitMapBooleanString(kifKiFFunctions, "compile");
	InitMapBooleanString(kifKiFFunctions, "open");
	InitMapBooleanString(kifKiFFunctions, "clean");
	InitMapBooleanString(kifKiFFunctions, "exposed");
	InitMapBooleanString(kifKiFFunctions, "name");

	InitMapBooleanString(kifMapFunctions, "size");
	InitMapBooleanString(kifMapFunctions, "duplicate");
	InitMapBooleanString(kifMapFunctions, "invert");
	InitMapBooleanString(kifMapFunctions, "editdistance");
	InitMapBooleanString(kifMapFunctions, "vector");
	InitMapBooleanString(kifMapFunctions, "map");
	InitMapBooleanString(kifMapFunctions, "join");
	InitMapBooleanString(kifMapFunctions, "sort");
	InitMapBooleanString(kifMapFunctions, "clear");
	InitMapBooleanString(kifMapFunctions, "sum");
	InitMapBooleanString(kifMapFunctions, "test");
	InitMapBooleanString(kifMapFunctions, "product");
	InitMapBooleanString(kifMapFunctions, "pop");
	InitMapBooleanString(kifMapFunctions, "merge");
	InitMapBooleanString(kifMapFunctions, "predicate");
	InitMapBooleanString(kifMapFunctions, "isa",false);
	InitMapBooleanString(kifMapFunctions, "keys");
	InitMapBooleanString(kifMapFunctions, "values");
	InitMapBooleanString(kifMapFunctions, "items");
	InitMapBooleanString(kifMapFunctions, "max", false);
	InitMapBooleanString(kifMapFunctions, "min", false);
	InitMapBooleanString(kifMapFunctions, "methods",false);
	InitMapBooleanString(kifMapFunctions, "infos",false);
	InitMapBooleanString(kifMapFunctions, "type",false);
	InitMapBooleanString(kifMapFunctions, "string");
	InitMapBooleanString(kifMapFunctions, "json");
	InitMapBooleanString(kifMapFunctions, "int");
	InitMapBooleanString(kifMapFunctions, "long");
	InitMapBooleanString(kifMapFunctions, "short");
	InitMapBooleanString(kifMapFunctions, "float");
	InitMapBooleanString(kifMapFunctions, "apply");
	InitMapBooleanString(kifMapFunctions, "boolean");
	InitMapBooleanString(kifMapFunctions, "evaluation");
	InitMapBooleanString(kifMapFunctions, "seeds");

	InitMapBooleanString(kifSetFunctions, "size");
	InitMapBooleanString(kifSetFunctions, "editdistance");
	InitMapBooleanString(kifSetFunctions, "duplicate");
	InitMapBooleanString(kifSetFunctions, "push");
	InitMapBooleanString(kifSetFunctions, "merge");
	InitMapBooleanString(kifSetFunctions, "bytes");
	InitMapBooleanString(kifSetFunctions, "totext");
	InitMapBooleanString(kifSetFunctions, "test");
	InitMapBooleanString(kifSetFunctions, "join");
	InitMapBooleanString(kifSetFunctions, "pop");
	InitMapBooleanString(kifSetFunctions, "min", false);
	InitMapBooleanString(kifSetFunctions, "max", false);
	InitMapBooleanString(kifSetFunctions, "sum");
	InitMapBooleanString(kifSetFunctions, "product");
	InitMapBooleanString(kifSetFunctions, "clear");
	InitMapBooleanString(kifSetFunctions, "isa",false);
	InitMapBooleanString(kifSetFunctions, "methods",false);
	InitMapBooleanString(kifSetFunctions, "infos",false);
	InitMapBooleanString(kifSetFunctions, "type",false);
	InitMapBooleanString(kifSetFunctions, "string");
	InitMapBooleanString(kifSetFunctions, "json");
	InitMapBooleanString(kifSetFunctions, "int");
	InitMapBooleanString(kifSetFunctions, "long");
	InitMapBooleanString(kifSetFunctions, "short");
	InitMapBooleanString(kifSetFunctions, "float");
	InitMapBooleanString(kifSetFunctions, "map");
	InitMapBooleanString(kifSetFunctions, "vector");
	InitMapBooleanString(kifSetFunctions, "boolean");

	InitMapBooleanString(kifFileFunctions, "find");
	InitMapBooleanString(kifFileFunctions, "openwrite");
	InitMapBooleanString(kifFileFunctions, "openappend");
	InitMapBooleanString(kifFileFunctions, "openread");
	InitMapBooleanString(kifFileFunctions, "setsignature");
	InitMapBooleanString(kifFileFunctions, "getsignature");
	InitMapBooleanString(kifFileFunctions, "flush");
	InitMapBooleanString(kifFileFunctions, "get");
	InitMapBooleanString(kifFileFunctions, "unget");
	InitMapBooleanString(kifFileFunctions, "close");
	InitMapBooleanString(kifFileFunctions, "seek");
	InitMapBooleanString(kifFileFunctions, "tell");
	InitMapBooleanString(kifFileFunctions, "write");
	InitMapBooleanString(kifFileFunctions, "writeln");
	InitMapBooleanString(kifFileFunctions, "writebin");
	InitMapBooleanString(kifFileFunctions, "read");
	InitMapBooleanString(kifFileFunctions, "readln");
	InitMapBooleanString(kifFileFunctions, "size");
	InitMapBooleanString(kifFileFunctions, "isa",false);
	InitMapBooleanString(kifFileFunctions, "methods",false);
	InitMapBooleanString(kifFileFunctions, "infos",false);
	InitMapBooleanString(kifFileFunctions, "type",false);
	InitMapBooleanString(kifFileFunctions, "eof");
	InitMapBooleanString(kifFileFunctions, "string");
	InitMapBooleanString(kifFileFunctions, "json");
	InitMapBooleanString(kifFileFunctions, "int");
	InitMapBooleanString(kifFileFunctions, "long");
	InitMapBooleanString(kifFileFunctions, "short");
	InitMapBooleanString(kifFileFunctions, "float");
	InitMapBooleanString(kifFileFunctions, "boolean");

	InitMapBooleanString(kifDateFunctions, "setdate");
	InitMapBooleanString(kifDateFunctions, "date");
	InitMapBooleanString(kifDateFunctions, "year");
	InitMapBooleanString(kifDateFunctions, "month");
	InitMapBooleanString(kifDateFunctions, "day");
	InitMapBooleanString(kifDateFunctions, "yearday");
	InitMapBooleanString(kifDateFunctions, "weekday");
	InitMapBooleanString(kifDateFunctions, "hour");
	InitMapBooleanString(kifDateFunctions, "minute");
	InitMapBooleanString(kifDateFunctions, "format");
	InitMapBooleanString(kifDateFunctions, "second");
	InitMapBooleanString(kifDateFunctions, "isa",false);
	InitMapBooleanString(kifDateFunctions, "methods",false);
	InitMapBooleanString(kifDateFunctions, "infos",false);
	InitMapBooleanString(kifDateFunctions, "type",false);

	InitMapBooleanString(kifDateFunctions, "string");
	InitMapBooleanString(kifDateFunctions, "json");
	InitMapBooleanString(kifDateFunctions, "int");
	InitMapBooleanString(kifDateFunctions, "long");
	InitMapBooleanString(kifDateFunctions, "short");
	InitMapBooleanString(kifDateFunctions, "float");
	InitMapBooleanString(kifDateFunctions, "boolean");

	InitMapBooleanString(kifTimeFunctions, "methods",false);
	InitMapBooleanString(kifTimeFunctions, "infos",false);
	InitMapBooleanString(kifTimeFunctions, "type",false);

	InitMapBooleanString(kifTimeFunctions, "reset");
	InitMapBooleanString(kifTimeFunctions, "string");
	InitMapBooleanString(kifTimeFunctions, "json");
	InitMapBooleanString(kifTimeFunctions, "int");
	InitMapBooleanString(kifTimeFunctions, "long");
	InitMapBooleanString(kifTimeFunctions, "short");
	InitMapBooleanString(kifTimeFunctions, "float");
	InitMapBooleanString(kifTimeFunctions, "boolean");

	//abs,acos,acosh,asin,asinh,atan,atanh,cbrt,cos,cosh,erf,erfc,exp,exp2,expm1,floor,
	//lgamma,ln,log,log1p,log2,logb,nearbyint,rint,round,sin,sinh,sqrt,tan,tanh,tgamma,
	//trunc,"@pi","@e"
	//Mathematics Function
	kifMathFunctions[KifNewId("abs")] = fabs;
	kifMathFunctions[KifNewId("acos")] = acos;
	kifMathFunctions[KifNewId("acosh")] = acosh;
	kifMathFunctions[KifNewId("asin")] = asin;
	kifMathFunctions[KifNewId("asinh")] = asinh;
	kifMathFunctions[KifNewId("atan")] = atan;
	kifMathFunctions[KifNewId("atanh")] = atanh;
	kifMathFunctions[KifNewId("cbrt")] = cbrt;
	kifMathFunctions[KifNewId("cos")] = cos;
	kifMathFunctions[KifNewId("cosh")] = cosh;
	kifMathFunctions[KifNewId("erf")] = erf;
	kifMathFunctions[KifNewId("erfc")] = erfc;
	kifMathFunctions[KifNewId("exp")] = exp;
	kifMathFunctions[KifNewId("exp2")] = exp2;
	kifMathFunctions[KifNewId("expm1")] = expm1;
	kifMathFunctions[KifNewId("floor")] = floor;
	kifMathFunctions[KifNewId("lgamma")] = lgamma;
	kifMathFunctions[KifNewId("ln")] = log;
	kifMathFunctions[KifNewId("log")] = log10;
	kifMathFunctions[KifNewId("log1p")] = log1p;
	kifMathFunctions[KifNewId("log2")] = log2;
	kifMathFunctions[KifNewId("logb")] = logb;
	kifMathFunctions[KifNewId("nearbyint")] = nearbyint;
	kifMathFunctions[KifNewId("rint")] = rint;
	kifMathFunctions[KifNewId("round")] = round;
	kifMathFunctions[KifNewId("sin")] = sin;
	kifMathFunctions[KifNewId("sinh")] = sinh;
	kifMathFunctions[KifNewId("sqrt")] = sqrt;
	kifMathFunctions[KifNewId("tan")] = tan;
	kifMathFunctions[KifNewId("tanh")] = tanh;
	kifMathFunctions[KifNewId("tgamma")] = tgamma;
	kifMathFunctions[KifNewId("trunc")] = trunc;

	InitMapBooleanString(kifNumericalFunctions, "isa",false);
	InitMapBooleanString(kifNumericalFunctions, "type",false);
	InitMapBooleanString(kifNumericalFunctions, "methods",false);
	InitMapBooleanString(kifNumericalFunctions, "infos",false);
	InitMapBooleanString(kifNumericalFunctions, "chr", false);

	InitMapBooleanString(kifNumericalFunctions, "isemoji");
	InitMapBooleanString(kifNumericalFunctions, "emoji");

	InitMapBooleanString(kifNumericalFunctions, "iorestate", false);
	InitMapBooleanString(kifNumericalFunctions, "string");
	InitMapBooleanString(kifNumericalFunctions, "vector");
	InitMapBooleanString(kifNumericalFunctions, "size");
	InitMapBooleanString(kifNumericalFunctions, "bytes", false);
	InitMapBooleanString(kifNumericalFunctions, "json");
	InitMapBooleanString(kifNumericalFunctions, "partof");
	InitMapBooleanString(kifNumericalFunctions, "fraction");
	InitMapBooleanString(kifNumericalFunctions, "factors");
	InitMapBooleanString(kifNumericalFunctions, "prime");
	InitMapBooleanString(kifNumericalFunctions, "base");
	InitMapBooleanString(kifNumericalFunctions, "format");
	InitMapBooleanString(kifNumericalFunctions, "radian");
	InitMapBooleanString(kifNumericalFunctions, "even");
	InitMapBooleanString(kifNumericalFunctions, "odd");
	InitMapBooleanString(kifNumericalFunctions, "degree");
	InitMapBooleanString(kifNumericalFunctions, "int");
	InitMapBooleanString(kifNumericalFunctions, "bit");
	InitMapBooleanString(kifNumericalFunctions, "long");
	InitMapBooleanString(kifNumericalFunctions, "short");
	InitMapBooleanString(kifNumericalFunctions, "float");
	InitMapBooleanString(kifNumericalFunctions, "boolean");
	InitMapBooleanString(kifNumericalFunctions, "#", false);
	InitMapBooleanString(kifNumericalFunctions, "simplify");
	InitMapBooleanString(kifNumericalFunctions, "invert");
	InitMapBooleanString(kifNumericalFunctions, "nd");
	InitMapBooleanString(kifNumericalFunctions, "d");
	InitMapBooleanString(kifNumericalFunctions, "n");
	InitMapBooleanString(kifNumericalFunctions, "euclidian");


	InitMapBooleanString(kifIteratorFunctions, "next");
	InitMapBooleanString(kifIteratorFunctions, "end");
	InitMapBooleanString(kifIteratorFunctions, "setvalue");
	InitMapBooleanString(kifIteratorFunctions, "nend");
	InitMapBooleanString(kifIteratorFunctions, "begin");
	InitMapBooleanString(kifIteratorFunctions, "key");
	InitMapBooleanString(kifIteratorFunctions, "value");
	InitMapBooleanString(kifIteratorFunctions, "apply");
	InitMapBooleanString(kifIteratorFunctions, "valuetype");
	InitMapBooleanString(kifIteratorFunctions, "isa",false);
	InitMapBooleanString(kifIteratorFunctions, "methods",false);
	InitMapBooleanString(kifIteratorFunctions, "infos",false);
	InitMapBooleanString(kifIteratorFunctions, "type",false);

	kifConversionNames["string"] = true;
	kifConversionNames["long"] = true;
	kifConversionNames["short"] = true;
	kifConversionNames["int"] = true;
	kifConversionNames["boolean"] = true;
	kifConversionNames["float"] = true;
	kifConversionNames["vector"] = true;
	kifConversionNames["map"] = true;

	//We only create these values ONCE

	//===================================================================	

	kifIdType[kifString] = KifNewId("string");
	kifIdType[kifInteger] = KifNewId("int");
	kifIdType[kifLong] = KifNewId("long");
	kifIdType[kifFraction] = KifNewId("fraction");
	kifIdType[kifBit] = KifNewId("bit");
	kifIdType[kifBits] = KifNewId("bits");
	kifIdType[kifByte] = KifNewId("byte");
	kifIdType[kifDate] = KifNewId("date");
	kifIdType[kifCall] = KifNewId("call");
	kifIdType[kifFloat] = KifNewId("float");
	kifIdType[kifBoolean] = KifNewId("boolean");
	kifIdType[kifSelf] = KifNewId("self");
	kifIdType[kifMap] = KifNewId("map");
	kifIdType[kifVector] = KifNewId("vector");
	kifIdType[kifTable] = KifNewId("table");
	kifIdType[kifList] = KifNewId("list");
	kifIdType[kifIterator] = KifNewId("iterator");
	kifIdType[kifFile] = KifNewId("file");
	kifIdType[kifFileWide] = KifNewId("wfile");

	kifIdType[kifAny] = KifNewId("kifAny");
	kifIdType[kifCouple] = KifNewId("kifCouple");
	kifIdType[kifCallFunction] = KifNewId("kifCallFunction");
	kifIdType[kifFrameDeclaration] = KifNewId("kifFrameDeclaration");
	kifIdType[kifFunctionDeclaration] = KifNewId("kifFunctionDeclaration");
	kifIdType[kifInstance] = KifNewId("kifInstance");
	kifIdType[kifIndex] = KifNewId("kifIndex");
	kifIdType[kifPredefined] = KifNewId("kifPredefined");
	kifIdType[kifVariableDeclaration] = KifNewId("kifVariableDeclaration");
	kifIdType[kifFrame] = KifNewId("kifFrame");
	kifIdType[kifFunction] = KifNewId("kifFunction");
	kifIdType[kifInstruction] = KifNewId("kifInstruction");

	kifReturnString[KifNewId("replaceregexip")] = true;
	kifReturnString[KifNewId("replace")] = true;
	kifReturnString[KifNewId("removefirst")] = true;
	kifReturnString[KifNewId("removelast")] = true;
	kifReturnString[KifNewId("utf8")] = true;
	kifReturnString[KifNewId("dos")] = true;
	kifReturnString[KifNewId("dostoutf8")] = true;
	kifReturnString[KifNewId("isutf8")] = true;
	kifReturnString[KifNewId("latin")] = true;
	kifReturnString[KifNewId("left")] = true;
	kifReturnString[KifNewId("byteposition")] = true;
	kifReturnString[KifNewId("charposition")] = true;
	kifReturnString[KifNewId("right")] = true;
	kifReturnString[KifNewId("mid")] = true;
	kifReturnString[KifNewId("upper")] = true;
	kifReturnString[KifNewId("deaccentuate")] = true;
	kifReturnString[KifNewId("lower")] = true;
	kifReturnString[KifNewId("emoji")] = true;
	kifReturnString[KifNewId("trim")] = true;
	kifReturnString[KifNewId("trimleft")] = true;
	kifReturnString[KifNewId("trimright")] = true;
	kifReturnString[KifNewId("pop")] = true;
	kifReturnString[KifNewId("last")] = true;
	kifReturnString[KifNewId("infos")] = true;
	kifReturnString[KifNewId("type")] = true;
	kifReturnString[KifNewId("string")] = true;
	kifReturnString[KifNewId("sloop")] = true;
	kifReturnString[KifNewId("uloop")] = true;
	kifReturnString[KifNewId("format")] = true;
	kifReturnString[KifNewId("base")] = true;

	kifReturnInt[idint] = true;
	kifReturnInt[idlong] = true;
	kifReturnInt[idsize] = true;
	kifReturnInt[KifNewId("iloop")] = true;

	kifReturnInt[KifNewId("count")] = true;
	kifReturnInt[KifNewId("find")] = true;
	kifReturnInt[KifNewId("iloop")] = true;
	kifReturnInt[KifNewId("rfind")] = true;
	kifReturnInt[KifNewId("levenshtein")] = true;
	kifReturnInt[KifNewId("editdistance")] = true;

	kifReturnFloat[KifNewId("float")] = true;
	kifReturnFloat[KifNewId("floop")] = true;
	kifReturnFloat[KifNewId("radian")] = true;
	kifReturnFloat[KifNewId("even")] = true;
	kifReturnFloat[KifNewId("odd")] = true;
	kifReturnFloat[KifNewId("degree")] = true;
}

void KifGlobalThread::InitialisationInitStructs(int trig, KifCode* kifcodeptr) {
	equivalences[kifString] = kifString;
	equivalences[kifUString] = kifString;
	equivalences[kifRawString] = kifString;
	equivalences[kifLoopString] = kifString;
	equivalences[kifLoopUString] = kifString;
	equivalences[kifTime] = kifTime;
	equivalences[kifAutomaton] = kifAutomaton;
	equivalences[kifGrammar] = kifGrammar;
	equivalences[kifInteger] = kifInteger;
	equivalences[kifByte] = kifInteger;
	equivalences[kifBoolean] = kifInteger;
	equivalences[kifLong] = kifInteger;
	equivalences[kifFraction] = kifInteger;
	equivalences[kifBit] = kifInteger;
	equivalences[kifBits] = kifInteger;
	equivalences[kifFloat] = kifInteger;
	equivalences[kifLoopBoolean] = kifInteger;
	equivalences[kifLoopInteger] = kifInteger;
	equivalences[kifLoopFloat] = kifInteger;
	equivalences[kifDate] = kifDate;
	equivalences[kifMap] = kifMap;
	equivalences[kifIntegerMap] = kifMap;
	equivalences[kifFloatMap] = kifMap;
	equivalences[kifMapIntegerString] = kifMap;
	equivalences[kifMapIntegerFloat] = kifMap;
	equivalences[kifMapIntegerInteger] = kifMap;
	equivalences[kifMapFloatString] = kifMap;
	equivalences[kifMapFloatFloat] = kifMap;
	equivalences[kifMapFloatInteger] = kifMap;
	equivalences[kifMapStringString] = kifMap;
	equivalences[kifMapStringFloat] = kifMap;
	equivalences[kifMapStringInteger] = kifMap;
	equivalences[kifTreeMap] = kifMap;
	equivalences[kifIntegerTreeMap] = kifMap;
	equivalences[kifFloatTreeMap] = kifMap;
	equivalences[kifTreeMapIntegerString] = kifMap;
	equivalences[kifTreeMapIntegerFloat] = kifMap;
	equivalences[kifTreeMapIntegerInteger] = kifMap;
	equivalences[kifTreeMapFloatString] = kifMap;
	equivalences[kifTreeMapFloatFloat] = kifMap;
	equivalences[kifTreeMapFloatInteger] = kifMap;
	equivalences[kifTreeMapStringString] = kifMap;
	equivalences[kifTreeMapStringFloat] = kifMap;
	equivalences[kifTreeMapStringInteger] = kifMap;
	equivalences[kifPrimeMap] = kifMap;
	equivalences[kifIntegerPrimeMap] = kifMap;
	equivalences[kifIntegerBinMap] = kifMap;
	equivalences[kifBinMapIntegerInteger] = kifMap;
	equivalences[kifBinMapIntegerFloat] = kifMap;
	equivalences[kifBinMapIntegerString] = kifMap;
	equivalences[kifBinMapIntegerUString] = kifMap;

	equivalences[kifFloatPrimeMap] = kifMap;
	equivalences[kifPrimeMapIntegerString] = kifMap;
	equivalences[kifPrimeMapIntegerFloat] = kifMap;
	equivalences[kifPrimeMapIntegerInteger] = kifMap;
	equivalences[kifPrimeMapFloatString] = kifMap;
	equivalences[kifPrimeMapFloatFloat] = kifMap;
	equivalences[kifPrimeMapFloatInteger] = kifMap;
	equivalences[kifPrimeMapStringString] = kifMap;
	equivalences[kifPrimeMapStringFloat] = kifMap;
	equivalences[kifPrimeMapStringInteger] = kifMap;
	equivalences[kifVector] = kifVector;
	equivalences[kifTable] = kifVector;
	equivalences[kifVectorString] = kifVector;
	equivalences[kifVectorUString] = kifVector;
	equivalences[kifVectorFloat] = kifVector;
	equivalences[kifVectorByte] = kifVector;
	equivalences[kifVectorInteger] = kifVector;
	equivalences[kifBaseVector] = kifVector;
	equivalences[kifList] = kifList;
	equivalences[kifFile] = kifFile;
	equivalences[kifFileWide] = kifFile;
	equivalences[kifSet] = kifSet;
	equivalences[kifSetString] = kifSet;
	equivalences[kifSetFloat] = kifSet;
	equivalences[kifSetInteger] = kifSet;

	kifAllFunctions[kifTree] = kifTreeFunctions;
	kifAllFunctions[kifPredicate] = kifPredicateFunctions;
	kifAllFunctions[kifString] = kifStringFunctions;
	kifAllFunctions[kifAutomaton] = kifAutomatonFunctions;
	kifAllFunctions[kifTransducer] = kifTransducerFunctions;
	kifAllFunctions[kifGrammar] = kifGrammarFunctions;
	kifAllFunctions[kifDate] = kifDateFunctions;
	kifAllFunctions[kifTime] = kifTimeFunctions;
	kifAllFunctions[kifFile] = kifFileFunctions;
	kifAllFunctions[kifIterator] = kifIteratorFunctions;
	kifAllFunctions[kifInteger] = kifNumericalFunctions;
	kifAllFunctions[kifVector] = kifVectorFunctions;
	kifAllFunctions[kifList] = kifListFunctions;
	kifAllFunctions[kifMap] = kifMapFunctions;
	kifAllFunctions[kifSet] = kifSetFunctions;

	sizeinstance = 0;
	cursorinstance = 0;

	sizestring = trig >> 4;
	stringlist.reserve(sizestring);
	sempties.ajuste(sizestring);
	KifString* kstr;
	for (cursorstring = 0; cursorstring<sizestring; cursorstring++) {
		kstr = new KifString(NULL, NULL, "");
		kstr->Putpopped(0);
		kstr->idgarbage = cursorstring;
		stringlist.push_back(kstr);
	}
	cursorstring = 0;

	sizeustring = trig >> 4;
	ustringlist.reserve(sizeustring);
	uempties.ajuste(sizeustring);
	KifUString* kustr;
	for (cursorustring = 0; cursorustring<sizeustring; cursorustring++) {
		kustr = new KifUString(NULL, NULL, L"");
		kustr->Putpopped(0);
		kustr->idgarbage = cursorustring;
		ustringlist.push_back(kustr);
	}
	cursorustring = 0;

	sizeinteger = sizestring;
	integerlist.reserve(sizeinteger);
	iempties.ajuste(sizeinteger);
	KifInteger* kint;
	for (cursorinteger = 0; cursorinteger<sizeinteger; cursorinteger++) {
		kint = new KifInteger(NULL, NULL, 0);
		kint->Putpopped(0);
		kint->idgarbage = cursorinteger;
		integerlist.push_back(kint);
	}
	cursorinteger = 0;

	sizefloat = sizestring;
	floatlist.reserve(sizefloat);
	fempties.ajuste(sizefloat);
	KifFloat* kfloat;
	for (cursorfloat = 0; cursorfloat<sizefloat; cursorfloat++) {
		kfloat = new KifFloat(NULL, NULL, 0);
		kfloat->Putpopped(0);
		kfloat->idgarbage = cursorfloat;
		floatlist.push_back(kfloat);
	}
	cursorfloat = 0;

	nogarbage = false;
	trigger = trig;
	garbage.ajuste(trigger);
	empties.ajuste(trigger);
	garbagefunction = NULL;
	basegarbage = 0;
	cursor = 0;
	cursormax = trigger;
}

void KifGlobalThread::InitPredefined(KifCode* kifcodeptr) {
	KifCreatePredefined(kifcodeptr, "sister", P_ONE | P_TWO, &PreTreeNext, true);
	KifCreatePredefined(kifcodeptr, "methods", P_ONE, &PreMethods, true);
	KifCreatePredefined(kifcodeptr, "infos", P_TWO, &PreInfos, true);
	KifCreatePredefined(kifcodeptr, "previous", P_ONE | P_TWO, &PreTreePrevious, true);
	KifCreatePredefined(kifcodeptr, "mother", P_ONE | P_TWO, &PreTreeParent, true);
	KifCreatePredefined(kifcodeptr, "daughter", P_ONE | P_TWO, &PreTreeChild, true);
	KifCreatePredefined(kifcodeptr, "lastnode", P_ONE | P_TWO, &PreTreeLast, true);
	KifCreatePredefined(kifcodeptr, "depth", P_ONE, &PreTreeDepth, true);
	KifCreatePredefined(kifcodeptr, "prune", P_ONE, &PreTreeRemove, true);
	KifCreatePredefined(kifcodeptr, "isolate", P_ONE, &PreTreeIsolate, true);
	KifCreatePredefined(kifcodeptr, "string", P_ONE, &PreConvertString, true);
	KifCreatePredefined(kifcodeptr, "float", P_ONE, &PreConvertFloat, true);
	KifCreatePredefined(kifcodeptr, "fraction", P_ONE, &PreConvertFraction, true);
	KifCreatePredefined(kifcodeptr, "int", P_ONE, &PreConvertInteger, true);
	KifCreatePredefined(kifcodeptr, "long", P_ONE, &PreConvertLong, true);
	KifCreatePredefined(kifcodeptr, "boolean", P_ONE, &PreConvertBoolean, true);
	KifCreatePredefined(kifcodeptr, "vector", P_ONE, &PreConvertVector, true);
	KifCreatePredefined(kifcodeptr, "map", P_ONE, &PreConvertMap, true);

	KifCreatePredefined(kifcodeptr, "_id", P_ONE, &PreId, true);
	KifCreatePredefined(kifcodeptr, "duplicate", P_ONE, &PreDuplicate);

	KifCreatePredefined(kifcodeptr, "_variables", P_NONE | P_ONE, &PreTheVariables, true);
	KifCreatePredefined(kifcodeptr, "_stack", P_NONE, &PreTheStack, true);
	KifCreatePredefined(kifcodeptr, "_filename", P_NONE, &PreCurrentFilename, true);


	KifCreatePredefined(kifcodeptr, "hash", P_ONE, &PreHash);
	KifCreatePredefined(kifcodeptr, "next", P_ONE, &PreNext);

	KifCreatePredefined(kifcodeptr, "extract", P_ATLEASTFOUR, &PreExtract);

	KifCreatePredefined(kifcodeptr, "ioredirect", P_TWO, &PreRedictectOutput);
	KifCreatePredefined(kifcodeptr, "iorestate", P_TWO, &PreRestateOutput);

	KifCreatePredefined(kifcodeptr, "json", P_ONE, &PreJSon);
	KifCreatePredefined(kifcodeptr, "string", P_ONE, &PreConvertString);
	KifCreatePredefined(kifcodeptr, "float", P_ONE, &PreConvertFloat);
	KifCreatePredefined(kifcodeptr, "fraction", P_ONE, &PreConvertFraction);
	KifCreatePredefined(kifcodeptr, "int", P_ONE, &PreConvertInteger);
	KifCreatePredefined(kifcodeptr, "long", P_ONE, &PreConvertLong);
	KifCreatePredefined(kifcodeptr, "boolean", P_ONE, &PreConvertBoolean);
	KifCreatePredefined(kifcodeptr, "vector", P_ONE, &PreConvertVector);
	KifCreatePredefined(kifcodeptr, "map", P_ONE, &PreConvertMap);
	KifCreatePredefined(kifcodeptr, "evaluation", P_ONE, &PreEvaluationMap);
	KifCreatePredefined(kifcodeptr, "seeds", P_TWO, &PreSetSeeds);


	KifCreatePredefined(kifcodeptr, "waiton", P_FULL, &PreWaitOn);
	KifCreatePredefined(kifcodeptr, "addtowaiton", P_FULL, &PreAddToWaitOn);
	KifCreatePredefined(kifcodeptr, "trigger", P_TWO, &PreTrigger);

	KifCreatePredefined(kifcodeptr, "abs", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "acos", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "acosh", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "asin", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "asinh", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "atan", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "atanh", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "cbrt", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "cos", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "cosh", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "erf", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "erfc", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "exp", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "exp2", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "expm1", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "floor", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "lgamma", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "ln", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "log", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "log1p", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "log2", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "logb", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "nearbyint", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "rint", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "round", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "sin", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "sinh", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "sqrt", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "tan", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "tanh", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "tgamma", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "trunc", P_ONE, &PreMath);	
	KifCreatePredefined(kifcodeptr, "factors", P_ONE, &PrePrimefactors);
	KifCreatePredefined(kifcodeptr, "prime", P_ONE, &PrePrime);
	KifCreatePredefined(kifcodeptr, "base", P_TWO, &PreBase);
	KifCreatePredefined(kifcodeptr, "bit", P_TWO, &PreGetBit);

	KifCreatePredefined(kifcodeptr, "GPSdistance", P_FOUR|P_FIVE, &PreGPSDistance);

	KifCreatePredefined(kifcodeptr, "pause", P_ONE | P_TWO, &PrePause);
	KifCreatePredefined(kifcodeptr, "sleep", P_ONE, &PreSleep);
	KifCreatePredefined(kifcodeptr, "eval", P_ONE, &PreEval, true);
	KifCreatePredefined(kifcodeptr, "fromjson", P_ONE, &PreFromJSON, true);
	KifCreatePredefined(kifcodeptr, "run", P_TWO, &PreRun);
	KifCreatePredefined(kifcodeptr, "runasthread", P_TWO, &PreRunAsThread);
	KifCreatePredefined(kifcodeptr, "runend", P_ONE, &PreRunEnd);
	KifCreatePredefined(kifcodeptr, "kstart", P_ONE | P_NONE, &PreRunStart);
	KifCreatePredefined(kifcodeptr, "open", P_ONE, &PreOpenSession);
	KifCreatePredefined(kifcodeptr, "clean", P_ONE, &PreCloseSession);
	KifCreatePredefined(kifcodeptr, "compile", P_TWO | P_THREE, &PreCompile);
	KifCreatePredefined(kifcodeptr, "range", P_THREE | P_TWO, &PreRange);


	KifCreatePredefined(kifcodeptr, "process", P_TWO, &PreProcess);
	KifCreatePredefined(kifcodeptr, "lookup", P_FOUR | P_TWO, &PreLookup);
	KifCreatePredefined(kifcodeptr, "lookdown", P_TWO, &PreLookdown);
	KifCreatePredefined(kifcodeptr, "build", P_FIVE | P_FOUR, &PreBuild);
	KifCreatePredefined(kifcodeptr, "compilergx", P_THREE | P_FOUR, &PreCompilergx);

	KifCreatePredefined(kifcodeptr, "methods", P_ONE, &PreMethods);
	KifCreatePredefined(kifcodeptr, "infos", P_TWO, &PreInfos);
	KifCreatePredefined(kifcodeptr, "isa", P_TWO, &PreTypeIs);
	KifCreatePredefined(kifcodeptr, "type", P_ONE, &PreType);
	KifCreatePredefined(kifcodeptr, "iscontainer", P_ONE, &PreIsContainer);
	KifCreatePredefined(kifcodeptr, "ismap", P_ONE, &PreIsMap);
	KifCreatePredefined(kifcodeptr, "isvector", P_ONE, &PreIsVector);
	KifCreatePredefined(kifcodeptr, "name", P_ONE, &PreName);
	KifCreatePredefined(kifcodeptr, "catch", P_ONE | P_NONE, &PreCatch);
	KifCreatePredefined(kifcodeptr, "raise", P_ONE, &PreRaise);

	KifCreatePredefined(kifcodeptr, "model", P_TWO, &PreSetmodel);
	KifCreatePredefined(kifcodeptr, "look", P_TWO | P_THREE, &PreLook);
	KifCreatePredefined(kifcodeptr, "spotfinalstate", P_TWO, &Prespotfinalstate);
	KifCreatePredefined(kifcodeptr, "settransformationmap", P_TWO, &PreSetpath);
	KifCreatePredefined(kifcodeptr, "loadcompact", P_TWO | P_THREE, &PreLoadCompact);
	KifCreatePredefined(kifcodeptr, "loadlemma", P_TWO | P_THREE, &PreLoadDouble);
	KifCreatePredefined(kifcodeptr, "add", P_TWO | P_THREE | P_FOUR, &PreAddword);
	KifCreatePredefined(kifcodeptr, "setcodefeature", P_TWO, &PreSetcodefeat);
	KifCreatePredefined(kifcodeptr, "dump", P_TWO, &PreDump);

	KifCreatePredefined(kifcodeptr, "load", P_TWO | P_THREE, &PreLoad);
	KifCreatePredefined(kifcodeptr, "exposed", P_ONE, &PreExposed);

	KifCreatePredefined(kifcodeptr, "reverse", P_ONE, &PreReverse);

	KifCreatePredefined(kifcodeptr, "block", P_ONE, &PreLock);
	KifCreatePredefined(kifcodeptr, "unlock", P_ONE, &PreUnlock);
	KifCreatePredefined(kifcodeptr, "lock", P_ONE, &PreLockRec);
	KifCreatePredefined(kifcodeptr, "wait", P_ATLEASTONE, &PreWait);
	KifCreatePredefined(kifcodeptr, "waitonfalse", P_ONE, &PreWaitOnFalse);
	KifCreatePredefined(kifcodeptr, "waitonjoined", P_NONE, &PreWaitOnJoin);
	KifCreatePredefined(kifcodeptr, "waiting", P_NONE, &PreWaiting);
	KifCreatePredefined(kifcodeptr, "cast", P_ONE | P_NONE, &PreCast);
	KifCreatePredefined(kifcodeptr, "kill", P_ONE | P_NONE, &PreKill);

	KifCreatePredefined(kifcodeptr, "_erroronkey", P_ONE, &PreErrorOnKey);
	KifCreatePredefined(kifcodeptr, "_garbagesurvey", P_NONE, &PreGarbageSurvey);
	KifCreatePredefined(kifcodeptr, "_garbageframe", P_NONE, &PreGarbageFrame);
	KifCreatePredefined(kifcodeptr, "_gcsize", P_FULL, &PreGCSize);
	KifCreatePredefined(kifcodeptr, "_version", P_NONE, &PreVersion);
	KifCreatePredefined(kifcodeptr, "_exit", P_ONE | P_NONE, &PreExit);

	KifCreatePredefined(kifcodeptr, "emojis", P_NONE, &PreEmojis);

	KifCreatePredefined(kifcodeptr, "print", P_FULL, &PrePrint);
	KifCreatePredefined(kifcodeptr, "pflush", P_FULL, &PrePrintFlush);
	KifCreatePredefined(kifcodeptr, "println", P_FULL, &PrePrintLN);
	KifCreatePredefined(kifcodeptr, "printj", P_ONE | P_TWO | P_THREE, &PrePrintJoin);
	KifCreatePredefined(kifcodeptr, "printjerr", P_ONE | P_TWO | P_THREE, &PrePrintJoinErr);
	KifCreatePredefined(kifcodeptr, "printjln", P_ONE | P_TWO | P_THREE, &PrePrintJoinLN);
	KifCreatePredefined(kifcodeptr, "printjlnerr", P_ONE| P_TWO | P_THREE, &PrePrintJoinErrLN);
	KifCreatePredefined(kifcodeptr, "predicatedump", P_NONE | P_ONE, &PrePredicateDump);
	KifCreatePredefined(kifcodeptr, "findall", P_NONE | P_ONE, &PrePredicateDump);
	KifCreatePredefined(kifcodeptr, "retractall", P_NONE | P_ONE, &PreRetractAll);
	KifCreatePredefined(kifcodeptr, "printerr", P_FULL, &PrePrinterr);
	KifCreatePredefined(kifcodeptr, "printlnerr", P_FULL, &PrePrinterrLN);
	KifCreatePredefined(kifcodeptr, "openread", P_TWO, &PreOpenReadfile);
	KifCreatePredefined(kifcodeptr, "setsignature", P_TWO, &PreSetSignature);
	KifCreatePredefined(kifcodeptr, "getsignature", P_ONE, &PreGetSignature);
	KifCreatePredefined(kifcodeptr, "openwrite", P_TWO, &PreOpenWritefile);
	KifCreatePredefined(kifcodeptr, "openappend", P_TWO, &PreOpenAppendfile);
	KifCreatePredefined(kifcodeptr, "threadid", P_NONE, &PreThreadId, true);
	KifCreatePredefined(kifcodeptr, "parentthreadid", P_NONE, &PreParentThreadId, true);

	KifCreatePredefined(kifcodeptr, "setdebugfunction", P_ONE | P_TWO, &PreSetDebugFunction, true);
	KifCreatePredefined(kifcodeptr, "debugfunction", P_THREE, &PreSetKifCodeDebugFunction);
	KifCreatePredefined(kifcodeptr, "debugclear", P_ONE, &PreCloseKifCodeDebugFunction);
	KifCreatePredefined(kifcodeptr, "setdebugdata", P_ONE, &PreSetDebugData, true);
	KifCreatePredefined(kifcodeptr, "debugclose", P_NONE, &PreDebugClose, true);
	KifCreatePredefined(kifcodeptr, "getstackinfo", P_ONE, &PreGetInfoStackOnId, true);

	KifCreatePredefined(kifcodeptr, "sizeb", P_ONE, &PreSizeBytes);

	KifCreatePredefined(kifcodeptr, "flush", P_ONE, &PreFlushFile);
	KifCreatePredefined(kifcodeptr, "eof", P_ONE, &PreEndOfFile);
	KifCreatePredefined(kifcodeptr, "read", P_ONE | P_TWO, &PreRead);
	KifCreatePredefined(kifcodeptr, "readln", P_ONE, &PreReadOneLine);
	KifCreatePredefined(kifcodeptr, "seek", P_TWO, &PreSeek);
	KifCreatePredefined(kifcodeptr, "tell", P_ONE, &PreTell);
	KifCreatePredefined(kifcodeptr, "write", P_ATLEASTTWO, &PreWrite);
	KifCreatePredefined(kifcodeptr, "writeln", P_ATLEASTTWO, &PreWriteLN);
	KifCreatePredefined(kifcodeptr, "writebin", P_ATLEASTTWO, &PreWriteBin);
	KifCreatePredefined(kifcodeptr, "close", P_ONE, &PreClose);
	KifCreatePredefined(kifcodeptr, "insert", P_TWO | P_THREE, &PreInsert);
	KifCreatePredefined(kifcodeptr, "query", P_ATLEASTTWO, &PreQuery);
	KifCreatePredefined(kifcodeptr, "store", P_ONE | P_TWO | P_THREE| P_FOUR, &PreStore);
	KifCreatePredefined(kifcodeptr, "weight", P_ONE|P_TWO, &PrePredicateWeight);
	KifCreatePredefined(kifcodeptr, "remove", P_ONE | P_TWO, &PreRemove);
	KifCreatePredefined(kifcodeptr, "push", P_ATLEASTTWO, &PrePush);
	KifCreatePredefined(kifcodeptr, "resize", P_TWO, &PreResize);
	KifCreatePredefined(kifcodeptr, "move", P_THREE, &PreMoving);
	KifCreatePredefined(kifcodeptr, "merge", P_ATLEASTTWO, &PreMerge);
	KifCreatePredefined(kifcodeptr, "predicate", P_ONE, &PrePredicate);
	KifCreatePredefined(kifcodeptr, "pushfirst", P_ATLEASTTWO, &PrePushFirst);
	KifCreatePredefined(kifcodeptr, "pushlast", P_ATLEASTTWO, &PrePushLast);
	KifCreatePredefined(kifcodeptr, "join", P_THREE | P_TWO, &PreJoin);
	KifCreatePredefined(kifcodeptr, "reserve", P_TWO, &PreReserve);
	KifCreatePredefined(kifcodeptr, "pop", P_ONE | P_TWO | P_THREE, &PrePop);
	KifCreatePredefined(kifcodeptr, "poplast", P_ONE, &PrePopLast);
	KifCreatePredefined(kifcodeptr, "popfirst", P_ONE, &PrePopFirst);
	KifCreatePredefined(kifcodeptr, "last", P_ONE, &PreLast);
	KifCreatePredefined(kifcodeptr, "first", P_ONE, &PreFirst);
	KifCreatePredefined(kifcodeptr, "unique", P_ONE, &PreUnique);
	KifCreatePredefined(kifcodeptr, "sort", P_TWO, &PreSort);
	KifCreatePredefined(kifcodeptr, "permute", P_ONE, &PrePermute);
	KifCreatePredefined(kifcodeptr, "shuffle", P_ONE, &PreShuffle);
	KifCreatePredefined(kifcodeptr, "sortint", P_TWO, &PreSortInt);
	KifCreatePredefined(kifcodeptr, "sortlong", P_TWO, &PreSortLong);
	KifCreatePredefined(kifcodeptr, "sortfloat", P_TWO, &PreSortFloat);
	KifCreatePredefined(kifcodeptr, "sortstring", P_TWO, &PreSortString);
	KifCreatePredefined(kifcodeptr, "sum", P_ONE, &PreSum);
	KifCreatePredefined(kifcodeptr, "product", P_ONE, &PreProduct);
	KifCreatePredefined(kifcodeptr, "clear", P_ONE, &PreClear);

	KifCreatePredefined(kifcodeptr, "values", P_ONE, &PreValues);
	KifCreatePredefined(kifcodeptr, "items", P_ONE, &PreItems);
	KifCreatePredefined(kifcodeptr, "keys", P_ONE, &PreKeys);
	KifCreatePredefined(kifcodeptr, "inkey", P_ONE, &PreInkey, true);

	KifCreatePredefined(kifcodeptr, "simplify", P_ONE, &PreFractionSimplify);
	KifCreatePredefined(kifcodeptr, "invert", P_ONE, &PreFractionSwitch);
	KifCreatePredefined(kifcodeptr, "partof", P_TWO, &PrePartOf);
	KifCreatePredefined(kifcodeptr, "euclidian", P_ONE, &PreEuclidian);
	KifCreatePredefined(kifcodeptr, "d", P_TWO | P_ONE, &PreDenominator);
	KifCreatePredefined(kifcodeptr, "n", P_TWO | P_ONE, &PreNumerator);
	KifCreatePredefined(kifcodeptr, "nd", P_THREE, &PreNumeratorDenominator);

	KifCreatePredefined(kifcodeptr, "min", P_ATLEASTONE, &PreMin);
	KifCreatePredefined(kifcodeptr, "max", P_ATLEASTONE, &PreMax);

	KifCreatePredefined(kifcodeptr, "chr", P_ONE, &PreChr);
	KifCreatePredefined(kifcodeptr, "ord", P_ONE, &PreOrd);
	KifCreatePredefined(kifcodeptr, "bytes", P_ONE, &PreBytes);
	KifCreatePredefined(kifcodeptr, "totext", P_ONE, &PreBytes);
	KifCreatePredefined(kifcodeptr, "flatten", P_ONE, &PreFlatten);
	KifCreatePredefined(kifcodeptr, "fill", P_THREE, &PreFill);
	KifCreatePredefined(kifcodeptr, "padding", P_THREE | P_FOUR, &PrePadding);
	KifCreatePredefined(kifcodeptr, "format", P_ATLEASTTWO, &PreFormat);
	KifCreatePredefined(kifcodeptr, "radian", P_ONE, &PreRadian);
	KifCreatePredefined(kifcodeptr, "even", P_ONE, &PreEven);
	KifCreatePredefined(kifcodeptr, "odd", P_ONE, &PreOdd);
	KifCreatePredefined(kifcodeptr, "degree", P_ONE, &PreDegree);

	//Rules	
	KifCreatePredefined(kifcodeptr, "setvalue", P_TWO, &PreSet);
	KifCreatePredefined(kifcodeptr, "get", P_ONE | P_TWO, &PreGet);
	KifCreatePredefined(kifcodeptr, "getc", P_NONE, &PreGetc);
	KifCreatePredefined(kifcodeptr, "getstd", P_TWO, &PreGetSTD);
	KifCreatePredefined(kifcodeptr, "geterr", P_TWO, &PreGetERR);
	KifCreatePredefined(kifcodeptr, "random", P_NONE | P_ONE, &PreRandom);
	KifCreatePredefined(kifcodeptr, "apply", P_ATLEASTONE, &PreApply);

	KifCreatePredefined(kifcodeptr, "unget", P_ONE | P_TWO, &PreUnGet);


	//String functions
	KifCreatePredefined(kifcodeptr, "#", P_ONE, &PreComplement);
	KifCreatePredefined(kifcodeptr, "_compatibilities", P_NONE | P_ONE, &PreCompatibilities);
	KifCreatePredefined(kifcodeptr, "parse", P_ONE | P_THREE, &PreParse);
	KifCreatePredefined(kifcodeptr, "lisp", P_ONE | P_THREE, &PreLisp);
	KifCreatePredefined(kifcodeptr, "tags", P_THREE, &PreTags);
	KifCreatePredefined(kifcodeptr, "regexip", P_TWO, &PreXipRegex);
	KifCreatePredefined(kifcodeptr, "evaluate", P_ONE, &PreEvaluateMeta);
	KifCreatePredefined(kifcodeptr, "html", P_ONE, &PreHTMLEncoding);
	KifCreatePredefined(kifcodeptr, "tohtml", P_ONE, &PreHTMLEncoding);
	KifCreatePredefined(kifcodeptr, "toxml", P_ONE, &PreXMLEncoding);
	KifCreatePredefined(kifcodeptr, "replaceregexip", P_THREE, &PreXipReplaceRgx);
	KifCreatePredefined(kifcodeptr, "replace", P_THREE, &PreReplace);
	KifCreatePredefined(kifcodeptr, "levenshtein", P_TWO | P_THREE, &PreLevenshtein);
	KifCreatePredefined(kifcodeptr, "editdistance", P_TWO | P_THREE | P_FOUR, &PreLevenshtein);
	KifCreatePredefined(kifcodeptr, "_trace", P_TWO, &PreTrace);
	KifCreatePredefined(kifcodeptr, "_predicatemode", P_ONE|P_TWO|P_THREE, &PreMode);
	KifCreatePredefined(kifcodeptr, "split", P_THREE | P_TWO | P_ONE, &PreSplit);
	KifCreatePredefined(kifcodeptr, "splite", P_THREE | P_TWO | P_ONE, &PreSplitKeepEmptyStrings);
	KifCreatePredefined(kifcodeptr, "tokenize", P_ONE | P_TWO | P_THREE | P_FOUR, &PreTokenize);
	KifCreatePredefined(kifcodeptr, "stokenize", P_ONE|P_TWO, &PreSTokenize);
	KifCreatePredefined(kifcodeptr, "find", P_THREE | P_TWO, &PreFind);
	KifCreatePredefined(kifcodeptr, "count", P_FOUR | P_THREE | P_TWO | P_ONE, &PreCount);
	KifCreatePredefined(kifcodeptr, "test", P_TWO, &PreTest);
	KifCreatePredefined(kifcodeptr, "rfind", P_THREE | P_TWO, &PreRFind);
	KifCreatePredefined(kifcodeptr, "removefirst", P_TWO, &PreRemovefirst);
	KifCreatePredefined(kifcodeptr, "removelast", P_TWO, &PreRemovelast);
	KifCreatePredefined(kifcodeptr, "ngrams", P_TWO | P_THREE, &PreNGRAMS);
	KifCreatePredefined(kifcodeptr, "isutf8", P_ONE, &PreTestUTF8);
	KifCreatePredefined(kifcodeptr, "utf8", P_ONE|P_TWO, &PreLatinToUTF8);
	KifCreatePredefined(kifcodeptr, "latin", P_ONE, &PreUTF8ToLatin);
	KifCreatePredefined(kifcodeptr, "dos", P_ONE, &PreToDOS);
	KifCreatePredefined(kifcodeptr, "dostoutf8", P_ONE, &PreDOSToUTF8);
	KifCreatePredefined(kifcodeptr, "left", P_TWO, &PreLeft);
	KifCreatePredefined(kifcodeptr, "byteposition", P_TWO, &PreBytePosition);
	KifCreatePredefined(kifcodeptr, "charposition", P_TWO, &PreCharacterPosition);
	KifCreatePredefined(kifcodeptr, "right", P_TWO, &PreRight);
	KifCreatePredefined(kifcodeptr, "mid", P_THREE, &PreMid);
	KifCreatePredefined(kifcodeptr, "islower", P_ONE, &PreIsLower);
	KifCreatePredefined(kifcodeptr, "isupper", P_ONE, &PreIsUpper);
	KifCreatePredefined(kifcodeptr, "isalpha", P_ONE, &PreIsAlpha);
	KifCreatePredefined(kifcodeptr, "isconsonant", P_ONE, &PreIsConsonants);
	KifCreatePredefined(kifcodeptr, "isvowel", P_ONE, &PreIsVowels);
	KifCreatePredefined(kifcodeptr, "ispunctuation", P_ONE, &PreIsPunctuation);
	KifCreatePredefined(kifcodeptr, "isdigit", P_ONE, &PreIsDigit);
	KifCreatePredefined(kifcodeptr, "lower", P_ONE, &PreLower);
	KifCreatePredefined(kifcodeptr, "upper", P_ONE, &PreUpper);
	KifCreatePredefined(kifcodeptr, "deaccentuate", P_ONE, &PreDeaccentuate);
	KifCreatePredefined(kifcodeptr, "isemoji", P_ONE, &PreIsEmoji);
	KifCreatePredefined(kifcodeptr, "emoji", P_ONE, &PreEmoji);
	KifCreatePredefined(kifcodeptr, "trim", P_ONE, &PreTrim);
	KifCreatePredefined(kifcodeptr, "trimleft", P_ONE, &PreTrimleft);
	KifCreatePredefined(kifcodeptr, "trimright", P_ONE, &PreTrimright);

	//Time
	KifCreatePredefined(kifcodeptr, "setdate", P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX | P_SEVEN | P_EIGHT, &PreSetDate);
	KifCreatePredefined(kifcodeptr, "reset", P_ONE, &PreResetTime);
	KifCreatePredefined(kifcodeptr, "date", P_ONE, &PreDate);
	KifCreatePredefined(kifcodeptr, "year", P_ONE | P_TWO, &PreYear);
	KifCreatePredefined(kifcodeptr, "month", P_ONE | P_TWO, &PreMonth);
	KifCreatePredefined(kifcodeptr, "day", P_ONE | P_TWO, &PreDay);
	KifCreatePredefined(kifcodeptr, "hour", P_ONE | P_TWO, &PreHour);
	KifCreatePredefined(kifcodeptr, "minute", P_ONE | P_TWO, &PreMinute);
	KifCreatePredefined(kifcodeptr, "second", P_ONE | P_TWO, &PreSecond);
	KifCreatePredefined(kifcodeptr, "weekday", P_ONE, &PreWeekDay);
	KifCreatePredefined(kifcodeptr, "yearday", P_ONE, &PreYearDay);

	//Iterator functions
	KifCreatePredefined(kifcodeptr, "begin", P_ONE, &PreBegin);
	KifCreatePredefined(kifcodeptr, "end", P_ONE, &PreEnd);
	KifCreatePredefined(kifcodeptr, "nend", P_ONE, &PreNend);
	KifCreatePredefined(kifcodeptr, "key", P_ONE, &PreIteratorKey);
	KifCreatePredefined(kifcodeptr, "value", P_ONE | P_TWO, &PreValue);
	KifCreatePredefined(kifcodeptr, "valuetype", P_ONE, &PreValueType);
	KifCreatePredefined(kifcodeptr, "valueisa", P_TWO, &PreValueTypeIs);



	//Function for: string,map,vector
	KifCreatePredefined(kifcodeptr, "size", P_ONE, &PreSize);

}

void KifGlobalThread::Initialisation(int trig, KifCode* kifcodeptr) {
	//std::locale::global(std::locale(""));
	InitialisationIDs(kifcodeptr);
	InitialisationIDString(kifcodeptr);
	InitialisationInitMaps(kifcodeptr);
	InitialisationInitStructs(trig, kifcodeptr);
	InitPredefined(kifcodeptr);
}

void KifGlobalThread::AddKifCode(KifCode* k) {
	ThreadLock _lock(kifLoading);
	for (int i = 0; i<kifCodeVector.size(); i++) {
		if (kifCodeVector[i] == NULL) {
			k->idkifmain = i;
			kifCodeVector[i] = k;
			return;
		}
	}
	k->idkifmain = kifCodeVector.size();
	kifCodeVector.ajoute(k);
}

void KifGlobalThread::KifInitInformation() {
	kifInformationTransducer["build"] = "build(string input,string output,bool norm,int latin): Build a transducer file out of a text file containing on the first line surface form, then on next line lemma+features. norm is used to make the transducer case incensitive. latin is the latin table encoding number.";
	kifInformationTransducer["store"] = "store(string output,bool norm,int encoding): Store a transducer into a file. The last two parameters are optional.";
	kifInformationTransducer["compilergx"] = "compilergx(string rgx,svector features,string filename): Build a transducer file out of regular expressions. filename is optional, the resulting automaton is stored in a file.";
	kifInformationTransducer["load"] = "load(string file): load a transducer file.";
	kifInformationTransducer["add"] = "add(container,bool norm,int encoding): transform a container (vector or map) into a transducer lexion. If the container is a vector, then it should have an even number of values.";
	kifInformationTransducer["lookup"] = "lookup(string wrd,int threshold, int flags): Lookup of a word using a transducer. The lookup can be constrained with a threshold and flags (a_insert, a_first, a_delete, a_switch, a_change).";
	kifInformationTransducer["lookdown"] = "lookdown(string wrd): Lookdown of a word using a transducer.";
	kifInformationTransducer["process"] = "process(string sentence): Analyse a sequence of words using a transducer.";


	kifInformationPredicate["weight"] = "weight(): Returns the weight of this predicate.";
	kifInformationPredicate["query"] = "query(predicate,var1,var2...): This query is analyzed as a predicate.";
	kifInformationPredicate["store"] = "store(sqlite db): Store a predicate in the database. If db is omitted then store it in the knowledge base";
	kifInformationPredicate["remove"] = "remove(sqlite db): Remove a predicate from the database. If db is omitted then remove it from the knowledge base";

	kifInformationString["hash"] = "hash(): Return the hash value of a string.";
	kifInformationString["resize"] = "resize(nb): Resize a rawstring.";
	kifInformationString["json"] = "json(): Return the json encoded version of a string.";
	kifInformationString["byteposition"] = "byteposition(int pos): convert a character position into a byte position (especially useful in UTF8 strings)";
	kifInformationString["charposition"] = "charposition(int pos): convert a byte position into a character position.";
	kifInformationString["size"] = "size(): Return the size of the string.";
	kifInformationString["evaluate"] = "evaluate(): evaluate the meta-characters within a string and return the evaluated string.";
	kifInformationString["clear"] = "clear(): Clean the content of a string.";
	kifInformationString["tokenize"] = "tokenize(bool comma,bool separator,bool concatenate): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token.";
	kifInformationString["stokenize"] = "stokenize(map keeps): Segment a string into words and punctuations, with a keep.";
	kifInformationString["split"] = "split(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters";
	kifInformationString["splite"] = "splite(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.";
	kifInformationString["removefirst"] = "removefirst(int nb): remove the first nb characters of a string";
	kifInformationString["removelast"] = "removelast(int nb): remove the last nb characters of a string";
	kifInformationString["fill"] = "fill(int nb,string c): create a string of nb characters c";
	kifInformationString["padding"] = "padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional";
	kifInformationString["format"] = "format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.";
	kifInformationString["ord"] = "ord(): return the ASCII code of the first character, or a list of all ASCII code if the recipient is a vector";
	kifInformationString["base"] = "base(int b): Return the value corresponding to the string in base b";
	kifInformationString["left"] = "left(int nb): return the first nb characters of a string";
	kifInformationString["byteposition"] = "byteposition(int pos): Convert a character position into a byte position";
	kifInformationString["charposition"] = "charposition(int pos): Convert a byte position into a character position";
	kifInformationString["right"] = "right(int nb): return the last nb characters of a string";
	kifInformationString["mid"] = "mid(int pos,int nb): return the nb characters starting at position pos of a string";
	kifInformationString["reverse"] = "reverse(): reverse the string";
	kifInformationString["pop"] = "pop(i): remove character at position i";
	kifInformationString["pop"] = "pop(): remove last character";
	kifInformationString["last"] = "last(): return last character";
	kifInformationString["insert"] = "insert(i,s): insert the string s at i. If i is -1, then insert s between each character in the input string";
	kifInformationString["trim"] = "trim(): remove the trailing characters";
	kifInformationString["trimright"] = "trimright(): remove the trailing characters on the right";
	kifInformationString["trimleft"] = "trimleft(): remove the trailing characters on the left";
	kifInformationString["deaccentuate"] = "deaccentuate(): Remove the accents from accented characters";
	kifInformationString["upper"] = "upper(): Return the string in upper characters";
	kifInformationString["lower"] = "lower(): Return the string in lower characters";
	kifInformationString["islower"] = "islower(): Test if a string only contains lowercase characters";
	kifInformationString["isupper"] = "isupper(): Test if a string only contains uppercase characters";
	kifInformationString["isalpha"] = "isalpha(): Test if a string only contains only alphabetical characters";
	kifInformationString["isconsonant"] = "isconsonant(): Test if a string only contains consonants";
	kifInformationString["isvowel"] = "isvowel(): Test if a string only contains only vowels";
	kifInformationString["ispunctuation"] = "ispunctuation(): Test if the character is a punctuation";
	kifInformationString["extract"] = "extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings";
	kifInformationString["isdigit"] = "isdigit(): Test if a string only contains digits";
	kifInformationString["latin"] = "latin(): convert an UTF8 string in LATIN";
	kifInformationString["utf8"] = "utf8(int table): convert a LATIN string into UTF8. table is optional, by default it is ISO/IEC 8859 part 1.";
	kifInformationString["dos"] = "dos(): convert a string into DOS encoding";
	kifInformationString["dostoutf8"] = "dostoutf8(): convert a DOS string into UTF8";
	kifInformationString["isutf8"] = "isutf8(): Return true is the string is encoded in UTF8";
	kifInformationString["ngrams"] = "ngrams(int r): Return a vector of all ngrams of rank r";
	kifInformationString["count"] = "count(string sub,int pos,int mx): Count the number of substrings starting at position pos, ending at mx";
	kifInformationString["find"] = "find(string sub,int pos): Return the position of substring sub starting at position pos";
	kifInformationString["sizeb"] = "sizeb(): Return the size in bytes of the string";
	kifInformationString["rfind"] = "rfind(string sub,int pos): Return the position of substring sub backward starting at position pos";
	kifInformationString["replaceregexip"] = "replaceregexip(sub,str): Replace the substrings matching sub with str";
	kifInformationString["levenshtein"] = "levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.";
	kifInformationString["editdistance"] = "editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.";
	kifInformationString["html"] = "html(): Return the string into an HTML compatible string or as a vector of strings";
	kifInformationString["tohtml"] = "tohtml(): Return the string into an HTML compatible string or as a vector of strings";
	kifInformationString["toxml"] = "toxml(): Return the string into an XML compatible string or as a vector of strings";
	kifInformationString["regexip"] = "regexip(sub): Find the substrings matching sub";
	kifInformationString["replace"] = "replace(sub,str): Replace the substrings matching sub with str";
	kifInformationString["getstd"] = "getstd(bool): catch or release the standard output";
	kifInformationString["geterr"] = "geterr(bool): catch or release the error output";
	kifInformationString["replacergx"] = "replacergx(rgx,str): Replace the substrings matching rgx with str";
	kifInformationString["splitrgx"] = "splitrgx(rgx): Split string with regular expression rgx. Return a vector of substrings.";
	kifInformationString["regex"] = "regex(rgx): Test if the regular expression rgx applies to string|vector=regex(rgx): Return all substrings matching rgx|string=regex(rgx): Return the substring matching rgx in the string|int=regex(rgx): Return the position of the substring matching rgx in the string";
	kifInformationString["parse"] = "parse(): Parse a string as a piece of code and returns the evaluation as a vector.";
	kifInformationString["lisp"] = "lisp(): Parse a string as a parenthetic expressions, which is returned as a vector.|lisp(string o,string c): Parse a string as a parenthetic expressions, where the opening and closing characters are provided.";
	kifInformationString["tags"] = "tags(string o,string c): Parse a string as a parenthetic expression, where the opening and closing strings are provided.";
	kifInformationInt["factors"] = "factors(): return the decomposition in prime factors";
	kifInformationInt["prime"] = "prime(): return true is the number is a prime";
	kifInformationInt["base"] = "base(int b): return a string representing a number in base b";
	kifInformationInt["bit"] = "bit(int i): return true if the ith bit is 1.";
	kifInformationFloat["bit"] = "bit(int i): return true if the ith bit is 1.";
	kifInformationFloat["factors"] = "factors(): return the decomposition in prime factors";
	kifInformationFloat["prime"] = "prime(): return true is the number is a prime";
	kifInformationFloat["base"] = "base(int b): return a string representing a number in base b";
	kifInformationInt["invert"] = "invert(): Invert the denominator and the numerator of a fraction";
	kifInformationFloat["invert"] = "invert(): Invert the denominator and the numerator of a fraction";
	kifInformationInt["partof"] = "partof(bits x): Return true if the bits of x are all in the current bits";
	kifInformationFloat["partof"] = "partof(bits x): Return true if the bits of x are all in the current bits";
	kifInformationInt["d"] = "d(): Denominator of a fraction (with a parameter: set the value, without parameter return the value)";
	kifInformationInt["infos"] = "infos(string): infos about a method";
	kifInformationInt["methods"] = "methods(): Return a vector of all available methods";
	kifInformationInt["simplify"] = "simplify(): Simplification";
	kifInformationFloat["simplify"] = "simplify(): Simplification";
	kifInformationInt["n"] = "n(): Numerator of a fraction (with a parameter: set the value, without parameter return the value)";
	kifInformationInt["nd"] = "nd(int n,int d): Set the Numerator and the Denominator of a fraction";
	kifInformationInt["chr"] = "chr(): return the character whose code is the value of the variable";
	kifInformationFloat["chr"] = "chr(): return the character whose code is the value of the variable";
	kifInformationInt["#"] = "#(): return the bit complement";
	kifInformationFloat["#"] = "#(): return the bit complement";
	kifInformationInt["format"] = "format(string form): return a string formatted according to the pattern in form. (this format is the same as the sprintf format in C++)";
	kifInformationFloat["format"] = "format(string form): return a string formatted according to the pattern in form. (this format is the same as the sprintf format in C++)";
	kifInformationInt["radian"] = "radian(): Convert a value in degree into a value in radian";
	kifInformationInt["even"] = "even(): Return true if the value is even.";
	kifInformationInt["odd"] = "odd(): Return true if the value is odd.";
	kifInformationInt["degree"] = "degree(): Convert a value in radian into a value in degree";
	kifInformationFloat["radian"] = "radian(): Convert a value in degree into a value in radian";
	kifInformationFloat["degree"] = "degree(): Convert a value in radian into a value in degree";
	kifInformationFloat["even"] = "even(): Return true if the value is even.";
	kifInformationFloat["odd"] = "odd(): Return true if the value is odd.";
	kifInformationString["get"] = "get(): Read a string from keyboard";
	kifInformationString["bytes"] = "bytes(): Return the string as a vector of bytes";
	kifInformationInt["get"] = "get(): Read a number from keyboard";
	kifInformationFloat["get"] = "get(): Read a number from keyboard";
	//Mathematical Functions
	kifInformationInt["abs"] = "abs(): absolute value";
	kifInformationFloat["abs"] = "abs(): absolute value";
	kifInformationInt["acos"] = "acos(): arc cosine";
	kifInformationFloat["acos"] = "acos(): arc cosine";
	kifInformationInt["acosh"] = "acosh(): area hyperbolic cosine";
	kifInformationFloat["acosh"] = "acosh(): area hyperbolic cosine";
	kifInformationInt["asin"] = "asin(): arc sine";
	kifInformationFloat["asin"] = "asin(): arc sine";
	kifInformationInt["asinh"] = "asinh(): area hyperbolic sine";
	kifInformationFloat["asinh"] = "asinh(): area hyperbolic sine";
	kifInformationInt["atan"] = "atan(): arc tangent";
	kifInformationFloat["atan"] = "atan(): arc tangent";
	kifInformationInt["atanh"] = "atanh(): area hyperbolic tangent";
	kifInformationFloat["atanh"] = "atanh(): area hyperbolic tangent";
	kifInformationInt["cbrt"] = "cbrt(): cubic root";
	kifInformationFloat["cbrt"] = "cbrt(): cubic root";
	kifInformationInt["cos"] = "cos(): cosine";
	kifInformationFloat["cos"] = "cos(): cosine";
	kifInformationInt["cosh"] = "cosh(): hyperbolic cosine";
	kifInformationFloat["cosh"] = "cosh(): hyperbolic cosine";
	kifInformationInt["erf"] = "erf(): error function";
	kifInformationFloat["erf"] = "erf(): error function";
	kifInformationInt["erfc"] = "erfc(): complementary error function";
	kifInformationFloat["erfc"] = "erfc(): complementary error function";
	kifInformationInt["exp"] = "exp(): exponential function";
	kifInformationFloat["exp"] = "exp(): exponential function";
	kifInformationInt["exp2"] = "exp2(): binary exponential function";
	kifInformationFloat["exp2"] = "exp2(): binary exponential function";
	kifInformationInt["expm1"] = "expm1(): exponential minus one";
	kifInformationFloat["expm1"] = "expm1(): exponential minus one";
	kifInformationInt["floor"] = "floor(): down value";
	kifInformationFloat["floor"] = "floor(): down value";
	kifInformationInt["lgamma"] = "lgamma(): log-gamma function";
	kifInformationFloat["lgamma"] = "lgamma(): log-gamma function";
	kifInformationInt["log"] = "log(): logarithm base 10";
	kifInformationFloat["log"] = "log(): logarithm base 10";
	kifInformationInt["ln"] = "ln(): natural logarithm";
	kifInformationFloat["ln"] = "ln(): natural logarithm";
	kifInformationInt["log1p"] = "log1p(): logarithm plus one";
	kifInformationFloat["log1p"] = "log1p(): logarithm plus one";
	kifInformationInt["log2"] = "log2(): binary logarithm";
	kifInformationFloat["log2"] = "log2(): binary logarithm";
	kifInformationInt["logb"] = "logb(): floating-point base logarithm";
	kifInformationFloat["logb"] = "logb(): floating-point base logarithm";
	kifInformationInt["nearbyint"] = "nearbyint(): to nearby integral value";
	kifInformationFloat["nearbyint"] = "nearbyint(): to nearby integral value";
	kifInformationInt["rint"] = "rint(): to integral value";
	kifInformationFloat["rint"] = "rint(): to integral value";
	kifInformationInt["round"] = "round(): to nearest";
	kifInformationFloat["round"] = "round(): to nearest";
	kifInformationInt["sin"] = "sin(): sine";
	kifInformationFloat["sin"] = "sin(): sine";
	kifInformationInt["sinh"] = "sinh(): hyperbolic sine";
	kifInformationFloat["sinh"] = "sinh(): hyperbolic sine";
	kifInformationInt["sqrt"] = "sqrt(): square root";
	kifInformationFloat["sqrt"] = "sqrt(): square root";
	kifInformationInt["tan"] = "tan(): tangent";
	kifInformationFloat["tan"] = "tan(): tangent";
	kifInformationInt["tanh"] = "tanh(): hyperbolic tangent";
	kifInformationFloat["tanh"] = "tanh(): hyperbolic tangent";
	kifInformationInt["tgamma"] = "tgamma(): gamma function";
	kifInformationFloat["tgamma"] = "tgamma(): gamma function";
	kifInformationInt["trunc"] = "trunc(): value";
	kifInformationFloat["trunc"] = "trunc(): value";

	kifInformationVector["resize"] = "resize(nb): Resize a table";
	kifInformationVector["ngrams"] = "ngrams(int r,string sep): Return a vector of all ngrams of rank r, strings are concatenated with 'sep'";
	kifInformationVector["move"] = "move(int pfrom,int pto): move the element in pfrom to pto.";
	kifInformationVector["push"] = "push(a): add a to the vector";
	kifInformationVector["editdistance"] = "editdistance(s): Return the edit distance with 's'";
	kifInformationVector["flatten"] = "flatten(): merge all the sub-vectors elements into the current vector";
	kifInformationVector["merge"] = "merge(v): merge the elements from v into the container";
	kifInformationVector["bytes"] = "bytes(): Return the string matching a vector of bytes. Each value should be between 0..255";
	kifInformationVector["totext"] = "totext(): Return the string matching a vector of bytes. Each value should be between 0..255";
	kifInformationVector["test"] = "test(i): test if i is a valid slot in the vector";
	kifInformationVector["pop"] = "pop(int i|string s): remove the ith element or the value s.";
	kifInformationVector["pop"] = "pop(): remove the last element from the vector and return it.";
	kifInformationVector["last"] = "last(): return the last element of the vector";
	kifInformationVector["reverse"] = "reverse(): reverse the order of the elements in the vector";
	kifInformationVector["join"] = "join(string sep): concatenate each element in the vector in a string where each element is separated from the others with sep";
	kifInformationVector["reserve"] = "reserve(int sz): Reserve sz elements in a vector";
	kifInformationVector["insert"] = "insert(i,x): insert the element x at position i";
	kifInformationVector["sum"] = "sum(): Sum each element with the others";
	kifInformationVector["product"] = "product(): Multiply each element with the others";
	kifInformationVector["predicate"] = "predicate(): Create a new predicate of the form [head,param1,param2..]";
	kifInformationVector["clear"] = "clear(): clean the vector";
	kifInformationVector["min"] = "min(): return the minimum value in a container";
	kifInformationVector["max"] = "max(): return the maximum value in a container";
	kifInformationVector["sort"] = "sort(compare): sort the content of the vector according to compare function. compare is optional";
	kifInformationVector["shuffle"] = "shuffle(): reshuffle the values";
	kifInformationVector["permute"] = "permute(): permute the values in a vector. Return false, when all permutations have been explored.";
	kifInformationVector["unique"] = "unique(): remove duplicates";
	kifInformationVector["sortint"] = "sortint(bool order): sort the content of the vector forcing elements to be ints.";
	kifInformationVector["sortfloat"] = "sortfloat(bool order): sort the content of the vector forcing elements to be floats.";
	kifInformationVector["sortlong"] = "sortlong(bool order): sort the content of the vector forcing elements to be longs.";
	kifInformationVector["sortstring"] = "sortstring(bool order): sort the content of the vector forcing elements to be strings.";
	kifInformationVector["apply"] = "apply(a,b,c): apply all functions stored in the vector, passing a,b,c etc. as parameters.";
	kifInformationVector["waiton"] = "waiton(t1,t2,...,p1,p2..,pn): Waiting for threads, with table as the first argument for of each thread. p1,p2..pn are optional variables that will be passed as next parameters to each thread.";
	kifInformationVector["addtowaiton"] = "addtowaiton(t1,t2,...,tn): Addind new threads to the thread waiting list.";
	kifInformationVector["trigger"] = "trigger(bool loop): Trigger the threads, if loop is true the function is put in waiting mode back.";

	kifInformationList["pushfirst"] = "pushfirst(a): add a to the beginning of the list";
	kifInformationList["editdistance"] = "editdistance(s): Return the edit distance with 's'";
	kifInformationList["flatten"] = "flatten(): merge all the sub-vectors elements into the current vector";
	kifInformationList["test"] = "test(i): test if i is a valid slot in the list";
	kifInformationList["permute"] = "permute(): permute the values in a vector. Return false, when all permutations have been explored.";
	kifInformationList["shuffle"] = "shuffle(): reshuffle the values";
	kifInformationList["unique"] = "unique(): remove duplicates";
	kifInformationList["merge"] = "merge(v): merge the elements from v into the container";
	kifInformationList["pushlast"] = "pushlast(a): add a at the end of the list";
	kifInformationList["popfirst"] = "popfirst(): remove the first element from the list and return it.";
	kifInformationList["min"] = "min(): return the minimum value in a container";
	kifInformationList["max"] = "max(): return the maximum value in a container";
	kifInformationList["pop"] = "pop(int i|string s): remove the ith element or the value s.";
	kifInformationList["pop"] = "pop(): remove the last element from the vector and return it.";
	kifInformationList["poplast"] = "poplast(): remove the last element from the list and return it.";
	kifInformationList["last"] = "last(): return the last element of the list";
	kifInformationList["first"] = "first(): return the first element of the list";
	kifInformationList["reverse"] = "reverse(): reverse the order of the elements in the list";
	kifInformationList["join"] = "join(string sep): concatenate each element in the list in a string where each element is separated from the others with sep";
	kifInformationList["insert"] = "insert(i,x): insert the element x at position i";
	kifInformationList["sum"] = "sum(): Sum each element with the others";
	kifInformationList["product"] = "product(): Multiply each element with the others";
	kifInformationList["apply"] = "apply(a,b,c): apply all functions stored in the list, passing a,b,c etc. as parameters.";
	kifInformationList["clear"] = "clear(): clean the list";

	kifInformationMap["invert"] = "invert(): Invert keys and values in a map";
	kifInformationMap["sum"] = "sum(): Sum each element with the others";
	kifInformationMap["join"] = "join(string keysep,string sep): concatenate each element in the map in a string where each key is appended with a keysep and each element is separated from the others with sep";
	kifInformationMap["product"] = "product(): Multiply each element with the others";
	kifInformationMap["apply"] = "apply(a,b,c): apply all functions stored in the list, passing a,b,c etc. as parameters.";
	kifInformationMap["merge"] = "merge(v): merge the elements from v into the container";
	kifInformationMap["predicate"] = "predicate(): Create a new predicate of the form {'name':name,0:param1,1:param2..]";
	kifInformationMap["test"] = "test(i): test if i is a valid key in the map";
	kifInformationMap["editdistance"] = "editdistance(s): Return the edit distance with 's'";
	kifInformationMap["pop"] = "pop(string key): remove the elements matching key";
	kifInformationMap["keys"] = "keys(): returns the map keys as a vector";
	kifInformationMap["values"] = "values(): return the values as a vector";
	kifInformationMap["items"] = "items(): return a vector of key:value";
	kifInformationMap["min"] = "min(): return the minimum value in a container";
	kifInformationMap["max"] = "max(): return the maximum value in a container";
	kifInformationMap["clear"] = "clear(): clean the map";
	kifInformationMap["evaluation"] = "evaluation(): evaluate the memory footprint of a primemap";
	kifInformationMap["seeds"] = "seeds(ivector iv): set the primemap seed keys";

	kifInformationTree["tree"] = "tree(value): create a tree node, with value as a value. Value can have any types";
	kifInformationTree["editdistance"] = "editdistance(s): Return the edit distance with 's'";
	kifInformationTree["sister"] = "sister()|sister(tree n): return the sister node, or add a sister node";
	kifInformationTree["previous"] = "previous|previous(tree n): return the previous node or add n as the previous node";
	kifInformationTree["daughter"] = "daughter()|daughter(tree n): return the first child node or add n as the first child node";
	kifInformationTree["mother"] = "mother()|mother(tree n): return the mother node or test if the current node is a parent of n";
	kifInformationTree["lastnode"] = "lastnode()|lastnode(tree n): return the last node or test if the current node is the last child of n";
	kifInformationTree["isolate"] = "isolate(): Extract the current node from its tree";
	kifInformationTree["prune"] = "prune(): Delete the current sub-tree from the global tree";
	kifInformationTree["depth"] = "depth(): Return the depth of the node in the tree";
	kifInformationIterator["value"] = "value(): return the value of the current element";
	kifInformationIterator["key"] = "key(): return the key of the current element";
	kifInformationIterator["valuetype"] = "valuetype(): return the value type of the current element";
	kifInformationIterator["valueisa"] = "valueisa(string type): test the type of the current element";
	kifInformationIterator["begin"] = "begin(): initialiaze the iterator with the beginning of the collection";
	kifInformationIterator["next"] = "next(): next element in the collection";
	kifInformationIterator["setvalue"] = "setvalue(value): Set a new value to the current iterator placeholder.";
	kifInformationIterator["end"] = "end(): return true when the end of the collection is reached";
	kifInformationIterator["nend"] = "nend(): return true while the end of the collection has not been reached (~end())";
	kifInformationIterator["apply"] = "apply(a,b,c): apply a function";
	kifInformationDate["setdate"] = "setdate(year,month,day,hour,min,sec): set a time variable";
	kifInformationDate["date"] = "date(): return the date as a string";
	kifInformationDate["year"] = "year(): return the year as an integer";
	kifInformationDate["month"] = "month(): return the month as an integer";
	kifInformationDate["day"] = "day(): return the day as an integer";
	kifInformationDate["yearday"] = "yearday(): return the year day as an integer between 0-365";
	kifInformationDate["weekday"] = "weekday(): return the week day as an integer between 0-6, 0 is Sunday.";
	kifInformationDate["hour"] = "hour(): return the hour as an integer";
	kifInformationDate["minute"] = "minute(): return the minutes as an integer";
	kifInformationDate["second"] = "second(): return the seconds as an integer";
	kifInformationTime["reset"] = "reset (): reinitialize a time variable";
	kifInformationFile["openappend"] = "openappend(string filename): open a file in append writing mode";
	kifInformationFile["openwrite"] = "openwrite(string filename): open a file in write mode";
	kifInformationFile["close"] = "close(): close a file";
	kifInformationFile["find"] = "find(string str,bool uppercase): Find a string in a file";
	kifInformationFile["openread"] = "openread(string filename): open a file in read mode";
	kifInformationFile["setsignature"] = "setsignature(bool s): Set the signature mode";
	kifInformationFile["getsignature"] = "getsignature(): Return the signature mode";
	kifInformationFile["write"] = "write(string s1,string s2,): write strings in the file";
	kifInformationFile["writeln"] = "writeln(string s1,string s2,): write strings in the file, terminated with a carriage return";
	kifInformationFile["writebin"] = "writebin(int s1,int s2,): write character code in a file";
	kifInformationFile["get"] = "get(): read one character from the file";
	kifInformationFile["unget"] = "unget(): return one character to the stream";
	kifInformationFile["unget"] = "unget(nb): return nb character to the stream";
	kifInformationFile["read"] = "read(): read a line from a file";
	kifInformationFile["readln"] = "readln(): read a line from a file";
	kifInformationFile["seek"] = "seek(int p): position the file cursor at p";
	kifInformationFile["tell"] = "tell(): return the position of the file cursor";
	kifInformationFile["eof"] = "eof(): return true when the end of file is reached";

	kifInformationGrammar["load"] = "load(svector rules): Load a grammar out of a svector. Each line must be a rule.";
	kifInformationGrammar["apply"] = "apply(string x): Apply the grammar to a string.";

	kifInformationAutomaton["load"] = "load(string kifpathname,string featchar): Load a word file. Each line should contain one single word. featchar defines the character which starts features, it is optional.";
	kifInformationAutomaton["loadcompact"] = "loadcompact(string kifpathname): Load a file stored in a compact format.";
	kifInformationAutomaton["loadlemma"] = "loadlemma(string kifpathname): Load a word and lemma file. The even line contains a surface form and the odd lines the lemma and features.";
	kifInformationAutomaton["model"] = "model(function): Set the model function which is used to traverse the automaton";
	kifInformationAutomaton["look"] = "look(word): Traverses the automaton and returns the most probable readings accoding to the model function";
	kifInformationAutomaton["spotfinalstate"] = "spotfinalstate(bool): Detect final states in automaton while in 'look'";
	kifInformationAutomaton["settransformationmap"] = "settransformationmap(map m): Set the transformation map to set the weights when traversing the automaton with editdistance";
	kifInformationAutomaton["store"] = "store(string pathname): Store the file into a compact format";
	kifInformationAutomaton["size"] = "size(): Returns the number of words stored in the automaton";
	kifInformationAutomaton["dump"] = "dump(string path): Dump the automaton content into a file";
	kifInformationAutomaton["setcodefeature"] = "setcodefeature(string character): Use this character to detect the features while adding words";
	kifInformationAutomaton["editdistance"] = "editdistance(string word,int threshold,int flags): Finds words whose edit distance to 'word' is inferior to the threshold. flags values are: 1: action on first character. 2: change character. 4: delete character. 8: insert character.";
	kifInformationAutomaton["clear"] = "clear(): Clears the automaton";
	kifInformationAutomaton["add"] = "add(string word): add a word to the automaton. add(string word,string lemma): add a word and its lemma to the automaton.";

	kifInformationKif["load"] = "load(string kifpathname): Load a KiF program";
	kifInformationKif["compile"] = "compile(string kifpathname): Compile a KiF program as a string. Return an handle on the first instruction to execute ";
	kifInformationKif["run"] = "run(int i): run a program from instruction i (returned by compile)";
	kifInformationKif["kstart"] = "kstart(): Return the declared variables as a map";
	kifInformationKif["runasthread"] = "runasthread(int i): run a program from instruction i (returned by compile) in a thread";
	kifInformationKif["runend"] = "runend(): Return true if the programme execution has been completed";
	kifInformationKif["debugfunction"] = "debugfunction(function,object): set the debug function that will be called during the execution";
	kifInformationKif["debugclose"] = "debugclose(): Remove the debug mode initiated with debugfunction";
	kifInformationKif["open"] = "open(): open a KiF session";
	kifInformationKif["clean"] = "clean(): close a KiF session";
	kifInformationKif["name"] = "name(): Return the pathname of the KiF file";
	kifInformationKif["exposed"] = "exposed(): Return the list of methods exposed by KiF";
	kifInformationKif["_loader"] = "_loader: A kif variable (of the type described here), which stores a pointer to the loading program.";

	kifAllInformation[kifTree] = kifInformationTree;
	kifAllInformation[kifPredicate] = kifInformationPredicate;
	kifAllInformation[kifString] = kifInformationString;
	kifAllInformation[kifAutomaton] = kifInformationAutomaton;
	kifAllInformation[kifTransducer] = kifInformationTransducer;
	kifAllInformation[kifGrammar] = kifInformationGrammar;
	kifAllInformation[kifDate] = kifInformationDate;
	kifAllInformation[kifTime] = kifInformationTime;
	kifAllInformation[kifFile] = kifInformationFile;
	kifAllInformation[kifIterator] = kifInformationIterator;
	kifAllInformation[kifInteger] = kifInformationInt;
	kifAllInformation[kifVector] = kifInformationVector;
	kifAllInformation[kifList] = kifInformationList;
	kifAllInformation[kifMap] = kifInformationMap;
	kifAllInformation[kifSet] = kifInformationVector;

}

bool KifGlobalThread::KifCreatePredefined(KifCode* kcode, string name, unsigned int arity, Predefined func, bool tree) {
	int idname = KifNewId(name);
	if (tree == false) {
		if (kifBaseFunctions.check(idname))
			return false;

		kifBaseFunctions[idname] = new KifPredefined(NULL, arity, func, idname);
		kifBaseFunctions[idname]->kifcode = kcode;
	}
	else {
		if (kifBaseTreeFunctions.check(idname))
			return false;

		kifBaseTreeFunctions[idname] = new KifPredefined(NULL, arity, func, idname);
		kifBaseTreeFunctions[idname]->kifcode = kcode;
	}

	return true;
}

void KifGlobalThread::RecordExternalFunction(short idname, unsigned int arity) {
	if (!kifExternalFunctions.check(idname)) {
		KifCallMethod* kcm = new KifCallMethod(idname, arity);
		kifExternalFunctions[idname] = kcm;
	}
	else
		kifExternalFunctions[idname]->arity |= arity;
}

bool KifGlobalThread::TestExternalArity(int id, int nbparams) {
	if (!kifExternalFunctions.check(id))
		return false;

	unsigned int arity = 1 << nbparams;
	unsigned int expected = kifExternalFunctions[id]->arity;
	if ((expected&arity) != arity)
		return false;
	return true;
}

KifElement* KifGlobalThread::KifBaseFunctionTest(short name) {
	if (kifBaseFunctions.check(name))
		return kifBaseFunctions[name];

	if (kifExternalFunctions.check(name))
		return kifExternalFunctions[name];
	if (kifBaseTreeFunctions.check(name))
		return kifBaseTreeFunctions[name];
	return NULL;
}

void KifGlobalThread::Clean() {
	garbage_collector(basegarbage);
}

void KifGlobalThread::Garbage() {
	if (Trigger() == true)
		garbage_collector(basegarbage);
}


KifGlobalThread::~KifGlobalThread() {
	int i;
	for (i = 0; i < mxthreads; i++) {
		if (THR[i] != NULL)
			delete THR[i];
	}
	delete[] THR;
	if (gfail != NULL)
		delete gfail;
	if (gcut != NULL)
		delete gcut;
	delete gRAISEERROR;
	delete gNULL;
	delete gMINUSONE;
	delete gONE;
	delete gZERO;
	delete gEMPTYSTRING;
	delete gDEFAULT;
	delete gTRUE;
	delete gFALSE;
	delete gBREAK;
	delete gBREAKTRUE;
	delete gBREAKFALSE;
	delete gBREAKPOINT;
	delete gCONTINUE;
	delete gNEG;
	delete gNOELEMENT;
	delete gUNIVERSAL;
	delete bnfkif;
	delete gTypeSTRING;
	delete gTypeINTEGER;
	delete gTypeFLOAT;
	delete gTypeLONG;
	delete gTypeFRACTION;
	delete gTypeBOOLEAN;


	hmap<string, KifThreadLock*>::iterator itlock;
	for (itlock = locktables.begin(); itlock != locktables.end(); itlock++)
		delete itlock->second;
	bin_hash<KifCallMethod*>::iterator itfuncs;
	for (itfuncs = kifExternalFunctions.begin(); itfuncs != kifExternalFunctions.end(); itfuncs++)
		delete itfuncs->second;
#ifndef C11
#ifdef WIN32
	for (i = 0; i<locktablesize; i++)
	if (tableofcommonlocks[i] != 0)
		CloseHandle(tableofcommonlocks[i]);
#endif
#endif

	for (i = 0; i<KifCouple::localgarbage.size(); i++)
		delete KifCouple::localgarbage[i];
	KifCouple::localgarbage.clear();
	KifCouple::ilocalgarbage = 0;

	for (cursorinstance = 0; cursorinstance<instancelist.size(); cursorinstance++)
		delete instancelist[cursorinstance];

	for (cursorstring = 0; cursorstring<stringlist.size(); cursorstring++)
		delete stringlist[cursorstring];

	for (cursorustring = 0; cursorustring<ustringlist.size(); cursorustring++)
		delete ustringlist[cursorustring];

	for (cursorinteger = 0; cursorinteger<integerlist.size(); cursorinteger++)
		delete integerlist[cursorinteger];

	for (cursorfloat = 0; cursorfloat<floatlist.size(); cursorfloat++)
		delete floatlist[cursorfloat];

	for (int i = 0; i<garbage.size(); i++) {
		if (garbage[i] != NULL)
			delete garbage[i];
	}

	bin_hash<KifPredefined*>::iterator ifunc;
	for (ifunc = kifGlobals->kifBaseFunctions.begin(); ifunc != kifGlobals->kifBaseFunctions.end(); ifunc++)
		delete ifunc->second;

	for (ifunc = kifGlobals->kifBaseTreeFunctions.begin(); ifunc != kifGlobals->kifBaseTreeFunctions.end(); ifunc++)
		delete ifunc->second;

	bin_hash<LockedThread*>::iterator it;
	for (it = loquets.begin(); it != loquets.end(); it++) {
		if (it->second != NULL)
			delete it->second;
	}
	loquets.clear();

	Reset_External();
}

Exported void KifGlobalThread::ResetGarbageElement(KifElement* d, int i) {
	ThreadLock* lock = NULL;
	if (kifGlobals->Test())
		lock = new ThreadLock(kifGarbage);

	if (i == -100)
		d->Resetreference();
	else
	if (garbage[i] != NULL)
		garbage[i]->Resetreference();

	if (lock != NULL)
		delete lock;
}

//Generic GARBAGE: clean all instances
void KifGlobalThread::garbage_collector(int from) {

	threadblocinit;
	//Only the main thread can do garbaging
	if (idthread)
		return;

	if (garbagefunction != NULL)
		garbagefunction->Exec(kifNULL, NULL, kifNULL, idthread, callfunc);

	garbaging = true;
	KifElement* ke;
	size_t it;

	ThreadLock _lock(kifGarbaging);


	//We only free elements from that specific thread, to avoid weird issues
	for (it = from; it<garbage.size(); it++) {
		ke = garbage[it];
		if (ke == NULL)
			continue;
		if (ke->popped)
			continue;
		if (ke->reference <= 0)
			ke->Clean();
	}

	cursormax = from;
	cursor = -1;

	int nbfree = garbage.taille - garbage.dernier - 1;
	for (it = from; it<garbage.size(); it++) {
		ke = garbage[it];
		if (ke == NULL) {
			nbfree++;
			if (cursor == -1)
				cursor = it;
			cursormax = it;
			continue;
		}

		if (ke->reference <= 0 && !ke->popped) {
			nbfree++;
			delete ke;
			if (cursor == -1)
				cursor = it;
			cursormax = it;
			garbage[it] = NULL;
		}
	}
	//if nbfree is less than 12 percent of the garbage, we need to increase it
	nbfree <<= 3;
	if (nbfree<garbage.taille) {
		garbage.ajuste(garbage.taille * 2);
		cursormax = garbage.taille;
	}
	garbaging = false;
}

void KifGlobalThread::addtogarbage(KifElement* ke) {

	if (empties.dernier>0) {
		ke->idgarbage = empties.vecteur[--empties.dernier];
		garbage.vecteur[ke->idgarbage] = ke;
		return;
	}

	while (cursor<cursormax && garbage.vecteur[cursor] != NULL) cursor++;
	if (cursor<cursormax) {
		garbage.vecteur[cursor] = ke;
		ke->idgarbage = cursor++;
		if (cursor>garbage.dernier)
			garbage.dernier = cursor;
	}
	else {
		cursor = garbage.size();
		ke->idgarbage = cursor;
		//if we are going to have an extension of the garbage
		//then we can delay the next garbage collecting
		garbage.ajuste(garbage.taille * 2);
		cursormax = garbage.taille;
		garbage.push_back(ke);
	}
}

bool KifGlobalThread::Trigger() {
	if (!nogarbage && cursor >= cursormax)
		return true;
	return false;
}

bool KifGlobalThread::KifConversion(string n) {
	if (kifConversionNames.find(n) != kifConversionNames.end())
		return true;
	return false;
}

//------------------------------------------------------------------------------------
//----------------------------------Specialized garbages------------------------------
//------------------------------------------------------------------------------------

Exported KifPredicateVariableInstance* KifCode::Provideinstance(short nb, string& z) {
	KifPredicateVariableInstance* ke;
	if (kifGlobals->Test()) {
		ThreadLock _lockp(kifPInstance);
		ke = kifGlobals->provideinstance(z);
	}
	else
		ke = kifGlobals->provideinstance(z);
	ke->kifcode = this;
	ke->name += nb;
	return ke;
}

Exported KifString* KifCode::Providestring(string& z) {
	KifString* ke;
	if (kifGlobals->Test()) {
		ThreadLock _lockp(kifPString);
		ke = kifGlobals->providestring(z);
	}
	else
		ke = kifGlobals->providestring(z);
	ke->kifcode = this;
	return ke;
}

Exported KifUString* KifCode::Provideustring(wstring z) {
	KifUString* ke;
	if (kifGlobals->Test()) {
		ThreadLock _lockp(kifPUString);
		ke = kifGlobals->provideustring(z);
	}
	else
		ke = kifGlobals->provideustring(z);
	ke->kifcode = this;
	return ke;
}

Exported KifUString* KifCode::Provideustring(wchar_t x) {
	KifUString* ke;
	wstring z;
	z = x;
	if (kifGlobals->Test()) {
		ThreadLock _lockp(kifPUString);
		ke = kifGlobals->provideustring(z);
	}
	else
		ke = kifGlobals->provideustring(z);
	ke->kifcode = this;
	return ke;
}

Exported KifString* KifCode::Providestringraw(string z) {
	KifString* ke;
	if (kifGlobals->Test()) {
		ThreadLock _lockp(kifPString);
		ke = kifGlobals->providestring(z);
	}
	else
		ke = kifGlobals->providestring(z);
	ke->kifcode = this;
	return ke;
}

Exported KifInteger* KifCode::Provideinteger(long val) {
	KifInteger* ke;
	if (kifGlobals->Test()) {
		ThreadLock _lockp(kifPInteger);
		ke = kifGlobals->provideinteger(val);
	}
	else
		ke = kifGlobals->provideinteger(val);
	ke->kifcode = this;
	return ke;
}

Exported KifFloat* KifCode::Providefloat(double val) {
	KifFloat* ke;
	if (kifGlobals->Test()) {
		ThreadLock _lockp(kifPFloat);
		ke = kifGlobals->providefloat(val);
	}
	else
		ke = kifGlobals->providefloat(val);
	ke->kifcode = this;
	return ke;
}
//------------------------------------------------------------------------------------
Exported void KifString::Resetreference(int inc) {
	reference -= inc;
	if (reference <= 0) {
		if (popped == 1)
			popped = 0;
		else {
			if (kifGlobals->Test()) {
				{
					KifContainerLock _lock(this);
					value = "";
				}
				if (idgarbage != -1) {
					ThreadLock _lockp(kifPString);
					kifGlobals->sempties.push_back(idgarbage);
				}
			}
			else {
				value = "";
				if (idgarbage != -1)
					kifGlobals->sempties.push_back(idgarbage);
			}
		}
	}
}

Exported void KifUString::Resetreference(int inc) {
	reference -= inc;
	if (reference <= 0) {
		if (popped == 1)
			popped = 0;
		else {
			if (kifGlobals->Test()) {
				{
					KifContainerLock _lock(this);
					value = L"";
				}
				if (idgarbage != -1) {
					ThreadLock _lockp(kifPUString);
					kifGlobals->uempties.push_back(idgarbage);
				}
			}
			else {
				value = L"";
				if (idgarbage != -1)
					kifGlobals->uempties.push_back(idgarbage);
			}
		}
	}
}

Exported void KifInteger::Resetreference(int inc) {
	reference -= inc;
	if (reference <= 0) {
		if (popped == 1)
			popped = 0;
		else {
			if (idgarbage != -1) {
				if (kifGlobals->Test()) {
					ThreadLock _lockp(kifPInteger);
					value = 0;
					kifGlobals->iempties.push_back(idgarbage);
				}
				else {
					value = 0;
					kifGlobals->iempties.push_back(idgarbage);					
				}
			}
		}
	}
}

Exported void KifFloat::Resetreference(int inc) {
	reference -= inc;
	if (reference <= 0) {
		if (popped == 1)
			popped = 0;
		else {
			if (idgarbage != -1) {
				if (kifGlobals->Test()) {
					ThreadLock _lockp(kifPFloat);
					value = 0;
					kifGlobals->fempties.push_back(idgarbage);
				}
				else {
					value = 0;
					kifGlobals->fempties.push_back(idgarbage);
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------

KifUString* KifGlobalThread::provideustring(wstring& str) {
	KifUString* ke;
	if (uempties.dernier>0) {
		ke = ustringlist[uempties.vecteur[--uempties.dernier]];
		ke->popped = 1;
		ke->reference = 0;
		ke->value = str;
		return ke;
	}

	if (cursorustring<ustringlist.size()) {
		ke = ustringlist[cursorustring++];
		ke->popped = 1;
		ke->reference = 0;
		ke->value = str;
		return ke;
	}

	sizeustring <<= 1;
	ustringlist.resize(sizeustring);
	for (int i = cursorustring; i<sizeustring; i++) {
		ke = new KifUString(NULL, NULL, L"");
		ke->popped = 0;
		ke->idgarbage = i;
		ustringlist[i] = ke;
	}
	ke = ustringlist[cursorustring];
	ke->popped = 1;
	cursorustring++;
	ke->value = str;
	return ke;
}

KifString* KifGlobalThread::providestring(string& str) {
	KifString* ke;
	if (sempties.dernier>0) {
		ke = stringlist[sempties.vecteur[--sempties.dernier]];
		ke->popped = 1;
		ke->reference = 0;
		ke->value = str;
		return ke;
	}

	if (cursorstring<stringlist.size()) {
		ke = stringlist[cursorstring++];
		ke->popped = 1;
		ke->reference = 0;
		ke->value = str;
		return ke;
	}

	sizestring <<= 1;
	stringlist.resize(sizestring);
	for (int i = cursorstring; i<sizestring; i++) {
		ke = new KifString(NULL, NULL, "");
		ke->popped = 0;
		ke->idgarbage = i;
		stringlist[i] = ke;
	}
	ke = stringlist[cursorstring];
	ke->popped = 1;
	cursorstring++;
	ke->value = str;
	return ke;
}


KifPredicateVariableInstance* KifGlobalThread::provideinstance(string& str) {
	KifPredicateVariableInstance* ke;
	if (insempties.dernier>0) {
		ke = instancelist[insempties.vecteur[--insempties.dernier]];
		ke->popped = 1;
		ke->reference = 0;
		ke->label = str;
		ke->name = ke->idgarbage;
		ke->value = kifNOELEMENT;
		ke->merge = false;
		return ke;
	}

	if (cursorinstance<instancelist.size()) {
		ke = instancelist[cursorinstance++];
		ke->popped = 1;
		ke->reference = 0;
		ke->label = str;
		ke->value = kifNOELEMENT;
		ke->merge = false;
		ke->name = ke->idgarbage;
		return ke;
	}

	sizeinstance += sizeinstance >> 2;
	instancelist.resize(sizeinstance);
	for (int i = cursorinstance; i<sizeinstance; i++) {
		ke = new KifPredicateVariableInstance(NULL, NULL, i, "");
		ke->popped = 0;
		ke->idgarbage = i;
		ke->name = i;
		instancelist[i] = ke;
	}
	ke = instancelist[cursorinstance];
	ke->name = cursorinstance;
	ke->popped = 1;
	cursorinstance++;
	ke->label = str;
	return ke;
}

KifInteger* KifGlobalThread::provideinteger(long val) {
	KifInteger* ke;
	if (iempties.dernier>0) {
		ke = integerlist[iempties.vecteur[--iempties.dernier]];
		ke->popped = 1;
		ke->reference = 0;
		ke->value = val;
		return ke;
	}

	if (cursorinteger<integerlist.size()) {
		ke = integerlist[cursorinteger++];
		ke->popped = 1;
		ke->reference = 0;
		ke->value = val;
		return ke;
	}

	sizeinteger <<= 1;
	integerlist.resize(sizeinteger);
	for (int i = cursorinteger; i<sizeinteger; i++) {
		ke = new KifInteger(NULL, NULL, 0);
		ke->popped = 0;
		ke->idgarbage = i;
		integerlist[i] = ke;
	}
	ke = integerlist[cursorinteger];
	ke->value = val;
	ke->popped = 1;
	cursorinteger++;
	return ke;
}

KifFloat* KifGlobalThread::providefloat(double val) {
	KifFloat* kfloat;
	if (fempties.dernier>0) {
		kfloat = floatlist[fempties.vecteur[--fempties.dernier]];
		kfloat->popped = 1;
		kfloat->reference = 0;
		kfloat->value = val;
		return kfloat;
	}

	if (cursorfloat<floatlist.size()) {
		kfloat = floatlist[cursorfloat++];
		kfloat->popped = 1;
		kfloat->reference = 0;
		kfloat->value = val;
		return kfloat;
	}

	sizefloat <<= 1;
	floatlist.resize(sizefloat);
	for (int i = cursorfloat; i<sizefloat; i++) {
		kfloat = new KifFloat(NULL, NULL, 0);
		kfloat->idgarbage = i;
		kfloat->popped = 0;
		floatlist[i] = kfloat;
	}
	kfloat = floatlist[cursorfloat];
	kfloat->value = val;
	kfloat->popped = 1;
	cursorfloat++;
	return kfloat;
}



