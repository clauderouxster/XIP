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
extern const char* KIFLIBSTRINGS[];
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

		localstream = fopen(STR(filename), KIFGLOBALTHREAD[0]);
		if (localstream == NULL)
			return -1;
		DUP2(FILENO(localstream), output);
		return o;
	}

	if (localstreamerr != NULL)
		return -2;

	int o = DUP(output);

	localstreamerr = fopen(STR(filename), KIFGLOBALTHREAD[0]);
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
		int idcurrent = KifNewId(KIFGLOBALTHREAD[1]);
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
		return callfunc->kifcode->Returnerror(KIFGLOBALTHREAD[2]);
	if (output == -1)
		return callfunc->kifcode->Returnerror(KIFGLOBALTHREAD[3]);
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
		return callfunc->kifcode->Returnerror(KIFGLOBALTHREAD[4]);
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

	string err(KIFGLOBALTHREAD[5]);
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
			return callfunc->kifcode->Returnerror(KIFLIBSTRINGS[223]);
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
	KifError* err = new KifError(NULL, KIFGLOBALTHREAD[6]);
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
		errmsg << KIFGLOBALTHREAD[7] << id << ") " << errorptr->String() << Endl;
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
			message += KIFGLOBALTHREAD[8];
			message += " ";
			message += ch;
		}
		message += " ";
		message += KIFGLOBALTHREAD[9]; 
		message += " " + filename;
	}

	err->value = message;
	THR[id]->error = true;
	THR[id]->errorptr = err;
	for (int i = THR[id]->stackelements.size() - 1; i >= 0; i--) {
		string mess = KIFGLOBALTHREAD[10];
		if (THR[id]->stackelements[i]->name != -1)
			mess = kifIdString[THR[id]->stackelements[i]->name];
		mess += ": ";
		line = THR[id]->stackelements[i]->line;
		filename = debuginfocurrent->Filename(THR[id]->stackelements[i]->file);
		if (filename != "")
			sprintf_s(ch, 1024, KIFGLOBALTHREAD[11], line, STR(filename));
		else
			sprintf_s(ch, 1024, KIFGLOBALTHREAD[12], line);
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
		localstring.value = KIFGLOBALTHREAD[10];
		if (THR[id]->stackelements[i]->name != -1)
			localstring.value = kifIdString[THR[id]->stackelements[i]->name];
		localstring.value += ": ";
		line = THR[id]->stackelements[i]->line;
		string filename = debuginfocurrent->Filename(THR[id]->stackelements[i]->file);
		sprintf_s(ch, 1024, KIFGLOBALTHREAD[13], line, STR(filename));
		sprintf_s(ch, 1024, KIFGLOBALTHREAD[13], line, STR(filename));
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
	KifNewId(KIFGLOBALTHREAD[14]);
	KifNewId(KIFGLOBALTHREAD[15]);
	KifNewId(KIFGLOBALTHREAD[16]);
	KifNewId(KIFGLOBALTHREAD[17]);
	KifNewId(KIFGLOBALTHREAD[18]);
	KifNewId(KIFGLOBALTHREAD[9]);
	KifNewId("_call");
	KifNewId(KIFGLOBALTHREAD[19]);
	KifNewId(KIFGLOBALTHREAD[20]);
	KifNewId("<");
	KifNewId(">");
	KifNewId("==");
	KifNewId("!=");
	KifNewId("<=");
	KifNewId(">=");
	KifNewId(KIFGLOBALTHREAD[21]);
	KifNewId(KIFGLOBALTHREAD[22]);
	KifNewId(KIFGLOBALTHREAD[23]);
	KifNewId(KIFGLOBALTHREAD[24]);
	KifNewId(KIFGLOBALTHREAD[25]);
	KifNewId(KIFGLOBALTHREAD[26]);
	KifNewId(KIFGLOBALTHREAD[27]);
	KifNewId(KIFGLOBALTHREAD[28]);
	KifNewId(KIFGLOBALTHREAD[29]);
	KifNewId(KIFGLOBALTHREAD[30]);
	KifNewId(KIFGLOBALTHREAD[31]);
	KifNewId(KIFGLOBALTHREAD[32]);
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
	KifNewId(KIFGLOBALTHREAD[33]);
	KifNewId(KIFGLOBALTHREAD[34]);

	KifNewId(KIFGLOBALTHREAD[35]);
	KifNewId("_args");
	KifNewId("_OS");
	KifNewId(KIFGLOBALTHREAD[36]);
	KifNewId("_sep");
	KifNewId(KIFGLOBALTHREAD[37]);
	KifNewId(KIFGLOBALTHREAD[38]);

	KifNewId("_pi");
	KifNewId("π");
	KifNewId("_e");
	KifNewId(KIFGLOBALTHREAD[39]);

	KifNewId(KIFGLOBALTHREAD[40]);
	KifNewId(KIFGLOBALTHREAD[41]);
	KifNewId(KIFGLOBALTHREAD[42]);
	KifNewId(KIFGLOBALTHREAD[43]);
	KifNewId(KIFGLOBALTHREAD[44]);

	KifNewId(KIFGLOBALTHREAD[45]);
	KifNewId(KIFGLOBALTHREAD[46]);
	KifNewId(KIFGLOBALTHREAD[47]);
	KifNewId(KIFGLOBALTHREAD[48]);
	KifNewId(KIFGLOBALTHREAD[49]);

	KifNewId(KIFGLOBALTHREAD[50]);
	KifNewId(KIFGLOBALTHREAD[51]);
	KifNewId(KIFGLOBALTHREAD[52]);
	KifNewId(KIFGLOBALTHREAD[53]);
	KifNewId(KIFGLOBALTHREAD[54]);
	KifNewId(KIFGLOBALTHREAD[55]);
	KifNewId("!");
	KifNewId("_trace");
	KifNewId(KIFGLOBALTHREAD[56]);
	KifNewId("_");
	KifNewId("!asserta");
	KifNewId("!assertz");
	KifNewId(KIFGLOBALTHREAD[57]);
	KifNewId("!retract");
	KifNewId(KIFGLOBALTHREAD[58]);
	KifNewId(KIFGLOBALTHREAD[59]);
	KifNewId("%pipe%");
	KifNewId(KIFGLOBALTHREAD[60]);
	KifNewId(KIFGLOBALTHREAD[61]);
	KifNewId(KIFGLOBALTHREAD[62]);
	KifNewId(KIFGLOBALTHREAD[63]);

	KifNewId(KIFGLOBALTHREAD[64]);
	KifNewId(KIFGLOBALTHREAD[65]);
	KifNewId(KIFGLOBALTHREAD[66]);
	KifNewId(KIFGLOBALTHREAD[67]);

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

	idreturn = KifNewId(KIFGLOBALTHREAD[68]);
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

	kifStringType[kifPredicate] = KIFGLOBALTHREAD[69];
	kifStringType[kifPredicateVariableInstance] = KIFGLOBALTHREAD[70];
	kifStringType[kifLoader] = KIFGLOBALTHREAD[37];
	kifStringType[kifVariable] = KIFGLOBALTHREAD[71];
	kifStringType[kifDomain] = KIFGLOBALTHREAD[72];
	kifStringType[kifString] = KIFGLOBALTHREAD[27];
	kifStringType[kifUString] = KIFGLOBALTHREAD[73];
	kifStringType[kifTuple] = KIFGLOBALTHREAD[74];
	kifStringType[kifRawString] = KIFGLOBALTHREAD[75];
	kifStringType[kifTime] = KIFGLOBALTHREAD[76];
	kifStringType[kifAutomaton] = KIFGLOBALTHREAD[77];	
	kifStringType[kifTransducer] = KIFGLOBALTHREAD[78];
	kifStringType[kifGrammar] = KIFGLOBALTHREAD[79];	
	kifStringType[kifVOID] = KIFGLOBALTHREAD[80];
	kifStringType[kifInteger] = KIFGLOBALTHREAD[28];
	kifStringType[kifByte] = KIFGLOBALTHREAD[81];
	kifStringType[kifLong] = KIFGLOBALTHREAD[29];
	kifStringType[kifShort] = KIFGLOBALTHREAD[61];
	kifStringType[kifFraction] = KIFGLOBALTHREAD[32];
	kifStringType[kifBit] = KIFGLOBALTHREAD[82];
	kifStringType[kifBits] = KIFGLOBALTHREAD[83];
	kifStringType[kifDate] = KIFGLOBALTHREAD[84];
	kifStringType[kifCall] = KIFGLOBALTHREAD[85];

	kifStringType[kifFloat] = KIFGLOBALTHREAD[30];
	kifStringType[kifLoopString] = KIFGLOBALTHREAD[86];
	kifStringType[kifLoopUString] = KIFGLOBALTHREAD[87];
	kifStringType[kifLoopBoolean] = KIFGLOBALTHREAD[88];
	kifStringType[kifLoopInteger] = KIFGLOBALTHREAD[89];
	kifStringType[kifLoopFloat] = KIFGLOBALTHREAD[90];
	kifStringType[kifBoolean] = KIFGLOBALTHREAD[31];
	kifStringType[kifSelf] = KIFGLOBALTHREAD[18];


	kifStringType[kifTuple] = KIFGLOBALTHREAD[74];

	kifStringType[kifMap] = KIFGLOBALTHREAD[23];
	kifStringType[kifUStringMap] = KIFGLOBALTHREAD[91];
	kifStringType[kifIntegerMap] = KIFGLOBALTHREAD[92];
	kifStringType[kifFloatMap] = KIFGLOBALTHREAD[93];
	kifStringType[kifMapIntegerString] = KIFGLOBALTHREAD[94];
	kifStringType[kifMapIntegerFloat] = KIFGLOBALTHREAD[95];
	kifStringType[kifMapIntegerInteger] = KIFGLOBALTHREAD[96];
	kifStringType[kifMapFloatString] = KIFGLOBALTHREAD[97];
	kifStringType[kifMapFloatFloat] = KIFGLOBALTHREAD[98];
	kifStringType[kifMapFloatInteger] = KIFGLOBALTHREAD[99];
	kifStringType[kifMapUStringUString] = KIFGLOBALTHREAD[100];
	kifStringType[kifMapUStringInteger] = KIFGLOBALTHREAD[101];
	kifStringType[kifMapUStringFloat] = KIFGLOBALTHREAD[102];
	kifStringType[kifMapStringString] = KIFGLOBALTHREAD[103];
	kifStringType[kifMapStringFloat] = KIFGLOBALTHREAD[104];
	kifStringType[kifMapStringInteger] = KIFGLOBALTHREAD[105];

	kifStringType[kifMapIntegerUString] = KIFGLOBALTHREAD[106];
	kifStringType[kifMapFloatUString] = KIFGLOBALTHREAD[107];

	kifStringType[kifTreeMap] = KIFGLOBALTHREAD[108];
	kifStringType[kifUStringTreeMap] = KIFGLOBALTHREAD[109];
	kifStringType[kifIntegerTreeMap] = KIFGLOBALTHREAD[110];
	kifStringType[kifFloatTreeMap] = KIFGLOBALTHREAD[111];
	kifStringType[kifTreeMapIntegerString] = KIFGLOBALTHREAD[112];
	kifStringType[kifTreeMapIntegerFloat] = KIFGLOBALTHREAD[113];
	kifStringType[kifTreeMapIntegerInteger] = KIFGLOBALTHREAD[114];
	kifStringType[kifTreeMapFloatString] = KIFGLOBALTHREAD[115];
	kifStringType[kifTreeMapFloatFloat] = KIFGLOBALTHREAD[116];
	kifStringType[kifTreeMapFloatInteger] = KIFGLOBALTHREAD[117];
	kifStringType[kifTreeMapStringString] = KIFGLOBALTHREAD[118];
	kifStringType[kifTreeMapUStringUString] = KIFGLOBALTHREAD[119];
	kifStringType[kifTreeMapUStringInteger] = KIFGLOBALTHREAD[120];
	kifStringType[kifTreeMapUStringFloat] = KIFGLOBALTHREAD[121];

	kifStringType[kifTreeMapIntegerUString] = KIFGLOBALTHREAD[122];
	kifStringType[kifTreeMapFloatUString] = KIFGLOBALTHREAD[123];

	kifStringType[kifTreeMapStringFloat] = KIFGLOBALTHREAD[124];
	kifStringType[kifTreeMapStringInteger] = KIFGLOBALTHREAD[125];

	kifStringType[kifPrimeMap] = KIFGLOBALTHREAD[126];
	kifStringType[kifUStringPrimeMap] = KIFGLOBALTHREAD[127];
	kifStringType[kifIntegerPrimeMap] = KIFGLOBALTHREAD[128];
	
	kifStringType[kifIntegerBinMap] = KIFGLOBALTHREAD[129];
	kifStringType[kifBinMapIntegerInteger] = KIFGLOBALTHREAD[130];
	kifStringType[kifBinMapIntegerFloat] = KIFGLOBALTHREAD[131];
	kifStringType[kifBinMapIntegerString] = KIFGLOBALTHREAD[132];
	kifStringType[kifBinMapIntegerUString] = KIFGLOBALTHREAD[133];

	kifStringType[kifFloatPrimeMap] = KIFGLOBALTHREAD[134];
	kifStringType[kifPrimeMapIntegerString] = KIFGLOBALTHREAD[135];
	kifStringType[kifPrimeMapIntegerFloat] = KIFGLOBALTHREAD[136];
	kifStringType[kifPrimeMapIntegerInteger] = KIFGLOBALTHREAD[137];
	kifStringType[kifPrimeMapFloatString] = KIFGLOBALTHREAD[138];
	kifStringType[kifPrimeMapFloatFloat] = KIFGLOBALTHREAD[139];
	kifStringType[kifPrimeMapFloatInteger] = KIFGLOBALTHREAD[140];
	kifStringType[kifPrimeMapStringString] = KIFGLOBALTHREAD[141];
	kifStringType[kifPrimeMapStringFloat] = KIFGLOBALTHREAD[142];
	kifStringType[kifPrimeMapStringInteger] = KIFGLOBALTHREAD[143];

	kifStringType[kifPrimeMapUStringUString] = KIFGLOBALTHREAD[144];
	kifStringType[kifPrimeMapUStringInteger] = KIFGLOBALTHREAD[145];
	kifStringType[kifPrimeMapUStringFloat] = KIFGLOBALTHREAD[146];
	kifStringType[kifPrimeMapIntegerUString] = KIFGLOBALTHREAD[147];
	kifStringType[kifPrimeMapFloatUString] = KIFGLOBALTHREAD[148];


	kifStringType[kifSet] = KIFGLOBALTHREAD[149];
	kifStringType[kifSetString] = KIFGLOBALTHREAD[150];
	kifStringType[kifSetUString] = KIFGLOBALTHREAD[151];
	kifStringType[kifSetFloat] = KIFGLOBALTHREAD[152];
	kifStringType[kifSetInteger] = KIFGLOBALTHREAD[153];

	kifStringType[kifVectorString] = KIFGLOBALTHREAD[53];
	kifStringType[kifVectorUString] = KIFGLOBALTHREAD[154];
	kifStringType[kifVectorFloat] = KIFGLOBALTHREAD[55];
	kifStringType[kifVectorInteger] = KIFGLOBALTHREAD[54];
	kifStringType[kifVectorByte] = KIFGLOBALTHREAD[155];
	kifStringType[kifVector] = KIFGLOBALTHREAD[22];
	kifStringType[kifBaseVector] = KIFGLOBALTHREAD[22];
	kifStringType[kifTable] = KIFGLOBALTHREAD[156];
	kifStringType[kifPredicate] = KIFGLOBALTHREAD[69];
	kifStringType[kifPredicateTerm] = KIFGLOBALTHREAD[157];
	kifStringType[kifList] = KIFGLOBALTHREAD[158];
	kifStringType[kifIterator] = KIFGLOBALTHREAD[159];
	kifStringType[kifFile] = KIFGLOBALTHREAD[160];
	kifStringType[kifFileWide] = KIFGLOBALTHREAD[161];

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

	kifTypeString[KIFGLOBALTHREAD[69]] = kifPredicate;

	kifTypeString[KIFGLOBALTHREAD[80]] = kifVOID;
	kifTypeString[KIFGLOBALTHREAD[70]] = kifPredicateVariableInstance;
	kifTypeString[KIFGLOBALTHREAD[72]] = kifDomain;
	kifTypeString[KIFGLOBALTHREAD[75]] = kifRawString;
	kifTypeString[KIFGLOBALTHREAD[27]] = kifString;
	kifTypeString[KIFGLOBALTHREAD[73]] = kifUString;
	kifTypeString[KIFGLOBALTHREAD[74]] = kifTuple;
	kifTypeString[KIFGLOBALTHREAD[77]] = kifAutomaton;
	kifTypeString[KIFGLOBALTHREAD[78]] = kifTransducer;
	kifTypeString[KIFGLOBALTHREAD[79]] = kifGrammar;
	kifTypeString[KIFGLOBALTHREAD[37]] = kifLoader;
	kifTypeString[KIFGLOBALTHREAD[71]] = kifVariable;
	kifTypeString[KIFGLOBALTHREAD[28]] = kifInteger;
	kifTypeString[KIFGLOBALTHREAD[29]] = kifLong;
	kifTypeString[KIFGLOBALTHREAD[61]] = kifShort;
	kifTypeString[KIFGLOBALTHREAD[32]] = kifFraction;
	kifTypeString[KIFGLOBALTHREAD[82]] = kifBit;
	kifTypeString[KIFGLOBALTHREAD[83]] = kifBits;
	kifTypeString[KIFGLOBALTHREAD[81]] = kifByte;
	kifTypeString[KIFGLOBALTHREAD[84]] = kifDate;
	kifTypeString[KIFGLOBALTHREAD[85]] = kifCall;
	kifTypeString[KIFGLOBALTHREAD[30]] = kifFloat;
	kifTypeString[KIFGLOBALTHREAD[86]] = kifLoopString;
	kifTypeString[KIFGLOBALTHREAD[87]] = kifLoopUString;
	kifTypeString[KIFGLOBALTHREAD[88]] = kifLoopBoolean;
	kifTypeString[KIFGLOBALTHREAD[89]] = kifLoopInteger;
	kifTypeString[KIFGLOBALTHREAD[90]] = kifLoopFloat;
	kifTypeString[KIFGLOBALTHREAD[31]] = kifBoolean;
	kifTypeString[KIFGLOBALTHREAD[162]] = kifBoolean;
	kifTypeString[KIFGLOBALTHREAD[18]] = kifSelf;
	kifTypeString[KIFGLOBALTHREAD[163]] = kifSelf;
	kifTypeString[KIFGLOBALTHREAD[164]] = kifSelf;
	kifTypeString[KIFGLOBALTHREAD[76]] = kifTime;

	kifTypeString[KIFGLOBALTHREAD[23]] = kifMap;
	kifTypeString[KIFGLOBALTHREAD[165]] = kifMap;
	kifTypeString[KIFGLOBALTHREAD[91]] = kifUStringMap;
	kifTypeString[KIFGLOBALTHREAD[93]] = kifFloatMap;
	kifTypeString[KIFGLOBALTHREAD[92]] = kifIntegerMap;
	kifTypeString[KIFGLOBALTHREAD[166]] = kifMapIntegerString;
	kifTypeString[KIFGLOBALTHREAD[167]] = kifMapIntegerInteger;
	kifTypeString[KIFGLOBALTHREAD[168]] = kifMapIntegerFloat;
	kifTypeString[KIFGLOBALTHREAD[169]] = kifMapFloatString;
	kifTypeString[KIFGLOBALTHREAD[170]] = kifMapFloatInteger;
	kifTypeString[KIFGLOBALTHREAD[171]] = kifMapFloatFloat;

	kifTypeString[KIFGLOBALTHREAD[50]] = kifMapStringString;
	kifTypeString[KIFGLOBALTHREAD[51]] = kifMapStringInteger;
	kifTypeString[KIFGLOBALTHREAD[52]] = kifMapStringFloat;
	kifTypeString[KIFGLOBALTHREAD[172]] = kifMapStringString;
	kifTypeString[KIFGLOBALTHREAD[173]] = kifMapStringInteger;
	kifTypeString[KIFGLOBALTHREAD[174]] = kifMapStringFloat;
	kifTypeString[KIFGLOBALTHREAD[94]] = kifMapIntegerString;
	
	kifTypeString[KIFGLOBALTHREAD[175]] = kifMapUStringUString;
	kifTypeString[KIFGLOBALTHREAD[176]] = kifMapUStringInteger;
	kifTypeString[KIFGLOBALTHREAD[177]] = kifMapUStringFloat;

	kifTypeString[KIFGLOBALTHREAD[178]] = kifMapIntegerUString;
	kifTypeString[KIFGLOBALTHREAD[177]] = kifMapFloatUString;
	kifTypeString[KIFGLOBALTHREAD[106]] = kifMapIntegerUString;
	kifTypeString[KIFGLOBALTHREAD[107]] = kifMapFloatUString;

	kifTypeString[KIFGLOBALTHREAD[96]] = kifMapIntegerInteger;
	kifTypeString[KIFGLOBALTHREAD[95]] = kifMapIntegerFloat;
	kifTypeString[KIFGLOBALTHREAD[97]] = kifMapFloatString;
	kifTypeString[KIFGLOBALTHREAD[99]] = kifMapFloatInteger;
	kifTypeString[KIFGLOBALTHREAD[98]] = kifMapFloatFloat;
	kifTypeString[KIFGLOBALTHREAD[103]] = kifMapStringString;
	kifTypeString[KIFGLOBALTHREAD[100]] = kifMapUStringUString;
	kifTypeString[KIFGLOBALTHREAD[101]] = kifMapUStringInteger;
	kifTypeString[KIFGLOBALTHREAD[102]] = kifMapUStringFloat;
	kifTypeString[KIFGLOBALTHREAD[105]] = kifMapStringInteger;
	kifTypeString[KIFGLOBALTHREAD[104]] = kifMapStringFloat;

	kifTypeString[KIFGLOBALTHREAD[108]] = kifTreeMap;
	kifTypeString[KIFGLOBALTHREAD[179]] = kifTreeMap;
	kifTypeString[KIFGLOBALTHREAD[109]] = kifUStringTreeMap;
	kifTypeString[KIFGLOBALTHREAD[111]] = kifFloatTreeMap;
	kifTypeString[KIFGLOBALTHREAD[110]] = kifIntegerTreeMap;

	kifTypeString[KIFGLOBALTHREAD[180]] = kifTreeMapIntegerString;
	kifTypeString[KIFGLOBALTHREAD[181]] = kifTreeMapIntegerInteger;
	kifTypeString[KIFGLOBALTHREAD[182]] = kifTreeMapIntegerFloat;
	kifTypeString[KIFGLOBALTHREAD[183]] = kifTreeMapFloatString;
	kifTypeString[KIFGLOBALTHREAD[184]] = kifTreeMapFloatInteger;
	kifTypeString[KIFGLOBALTHREAD[185]] = kifTreeMapFloatFloat;

	kifTypeString[KIFGLOBALTHREAD[186]] = kifTreeMapStringString;
	kifTypeString[KIFGLOBALTHREAD[187]] = kifTreeMapStringInteger;
	kifTypeString[KIFGLOBALTHREAD[188]] = kifTreeMapStringFloat;
	kifTypeString[KIFGLOBALTHREAD[189]] = kifTreeMapStringString;
	kifTypeString[KIFGLOBALTHREAD[190]] = kifTreeMapStringInteger;
	kifTypeString[KIFGLOBALTHREAD[191]] = kifTreeMapStringFloat;
	kifTypeString[KIFGLOBALTHREAD[192]] = kifTreeMapUStringUString;
	kifTypeString[KIFGLOBALTHREAD[193]] = kifTreeMapUStringInteger;
	kifTypeString[KIFGLOBALTHREAD[194]] = kifTreeMapUStringFloat;
	
	kifTypeString[KIFGLOBALTHREAD[195]] = kifTreeMapIntegerUString;
	kifTypeString[KIFGLOBALTHREAD[194]] = kifTreeMapFloatUString;
	kifTypeString[KIFGLOBALTHREAD[122]] = kifTreeMapIntegerUString;
	kifTypeString[KIFGLOBALTHREAD[123]] = kifTreeMapFloatUString;

	kifTypeString[KIFGLOBALTHREAD[112]] = kifTreeMapIntegerString;
	kifTypeString[KIFGLOBALTHREAD[114]] = kifTreeMapIntegerInteger;
	kifTypeString[KIFGLOBALTHREAD[113]] = kifTreeMapIntegerFloat;
	kifTypeString[KIFGLOBALTHREAD[115]] = kifTreeMapFloatString;
	kifTypeString[KIFGLOBALTHREAD[117]] = kifTreeMapFloatInteger;
	kifTypeString[KIFGLOBALTHREAD[116]] = kifTreeMapFloatFloat;
	kifTypeString[KIFGLOBALTHREAD[118]] = kifTreeMapStringString;
	kifTypeString[KIFGLOBALTHREAD[119]] = kifTreeMapUStringUString;
	kifTypeString[KIFGLOBALTHREAD[120]] = kifTreeMapUStringInteger;
	kifTypeString[KIFGLOBALTHREAD[121]] = kifTreeMapUStringFloat;
	kifTypeString[KIFGLOBALTHREAD[125]] = kifTreeMapStringInteger;
	kifTypeString[KIFGLOBALTHREAD[124]] = kifTreeMapStringFloat;

	kifTypeString[KIFGLOBALTHREAD[126]] = kifPrimeMap;
	kifTypeString[KIFGLOBALTHREAD[196]] = kifPrimeMap;
	kifTypeString[KIFGLOBALTHREAD[127]] = kifUStringPrimeMap;
	kifTypeString[KIFGLOBALTHREAD[134]] = kifFloatPrimeMap;
	kifTypeString[KIFGLOBALTHREAD[128]] = kifIntegerPrimeMap;
	
	kifTypeString[KIFGLOBALTHREAD[129]] = kifIntegerBinMap;
	kifTypeString[KIFGLOBALTHREAD[130]] = kifBinMapIntegerInteger;
	kifTypeString[KIFGLOBALTHREAD[131]] = kifBinMapIntegerFloat;
	kifTypeString[KIFGLOBALTHREAD[132]] = kifBinMapIntegerString;
	kifTypeString[KIFGLOBALTHREAD[133]] = kifBinMapIntegerUString;
	kifTypeString[KIFGLOBALTHREAD[197]] = kifBinMapIntegerInteger;
	kifTypeString[KIFGLOBALTHREAD[198]] = kifBinMapIntegerFloat;
	kifTypeString[KIFGLOBALTHREAD[199]] = kifBinMapIntegerString;
	kifTypeString[KIFGLOBALTHREAD[200]] = kifBinMapIntegerUString;

	kifTypeString[KIFGLOBALTHREAD[144]] = kifPrimeMapUStringUString;
	kifTypeString[KIFGLOBALTHREAD[201]] = kifPrimeMapUStringUString;
	kifTypeString[KIFGLOBALTHREAD[145]] = kifPrimeMapUStringInteger;
	kifTypeString[KIFGLOBALTHREAD[202]] = kifPrimeMapUStringInteger;
	kifTypeString[KIFGLOBALTHREAD[146]] = kifPrimeMapUStringFloat;
	kifTypeString[KIFGLOBALTHREAD[203]] = kifPrimeMapUStringFloat;
	kifTypeString[KIFGLOBALTHREAD[147]] = kifPrimeMapIntegerUString;
	kifTypeString[KIFGLOBALTHREAD[204]] = kifPrimeMapIntegerUString;
	kifTypeString[KIFGLOBALTHREAD[148]] = kifPrimeMapFloatUString;
	kifTypeString[KIFGLOBALTHREAD[205]] = kifPrimeMapFloatUString;

	kifTypeString[KIFGLOBALTHREAD[206]] = kifPrimeMapIntegerString;
	kifTypeString[KIFGLOBALTHREAD[207]] = kifPrimeMapIntegerInteger;
	kifTypeString[KIFGLOBALTHREAD[208]] = kifPrimeMapIntegerFloat;
	kifTypeString[KIFGLOBALTHREAD[209]] = kifPrimeMapFloatString;
	kifTypeString[KIFGLOBALTHREAD[210]] = kifPrimeMapFloatInteger;
	kifTypeString[KIFGLOBALTHREAD[211]] = kifPrimeMapFloatFloat;

	kifTypeString[KIFGLOBALTHREAD[212]] = kifPrimeMapStringString;
	kifTypeString[KIFGLOBALTHREAD[213]] = kifPrimeMapStringInteger;
	kifTypeString[KIFGLOBALTHREAD[214]] = kifPrimeMapStringFloat;
	kifTypeString[KIFGLOBALTHREAD[215]] = kifPrimeMapStringString;
	kifTypeString[KIFGLOBALTHREAD[216]] = kifPrimeMapStringInteger;
	kifTypeString[KIFGLOBALTHREAD[217]] = kifPrimeMapStringFloat;
	kifTypeString[KIFGLOBALTHREAD[135]] = kifPrimeMapIntegerString;
	kifTypeString[KIFGLOBALTHREAD[137]] = kifPrimeMapIntegerInteger;
	kifTypeString[KIFGLOBALTHREAD[136]] = kifPrimeMapIntegerFloat;
	kifTypeString[KIFGLOBALTHREAD[138]] = kifPrimeMapFloatString;
	kifTypeString[KIFGLOBALTHREAD[140]] = kifPrimeMapFloatInteger;
	kifTypeString[KIFGLOBALTHREAD[139]] = kifPrimeMapFloatFloat;
	kifTypeString[KIFGLOBALTHREAD[141]] = kifPrimeMapStringString;
	kifTypeString[KIFGLOBALTHREAD[143]] = kifPrimeMapStringInteger;
	kifTypeString[KIFGLOBALTHREAD[142]] = kifPrimeMapStringFloat;

	kifTypeString[KIFGLOBALTHREAD[150]]=kifSetString;
	kifTypeString[KIFGLOBALTHREAD[151]] = kifSetUString;
	kifTypeString[KIFGLOBALTHREAD[152]]=kifSetFloat;
	kifTypeString[KIFGLOBALTHREAD[153]] = kifSetInteger;
	kifTypeString[KIFGLOBALTHREAD[149]] = kifSet;

	kifTypeString[KIFGLOBALTHREAD[22]] = kifVector;
	kifTypeString[KIFGLOBALTHREAD[156]] = kifTable;
	kifTypeString[KIFGLOBALTHREAD[69]] = kifPredicate;
	kifTypeString[KIFGLOBALTHREAD[157]] = kifPredicateTerm;
	kifTypeString[KIFGLOBALTHREAD[53]] = kifVectorString;
	kifTypeString[KIFGLOBALTHREAD[154]] = kifVectorUString;
	kifTypeString[KIFGLOBALTHREAD[155]] = kifVectorByte;
	kifTypeString[KIFGLOBALTHREAD[54]] = kifVectorInteger;
	kifTypeString[KIFGLOBALTHREAD[55]] = kifVectorFloat;
	kifTypeString[KIFGLOBALTHREAD[158]] = kifList;
	kifTypeString[KIFGLOBALTHREAD[159]] = kifIterator;
	kifTypeString[KIFGLOBALTHREAD[160]] = kifFile;
	kifTypeString[KIFGLOBALTHREAD[161]] = kifFileWide;

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

	kifTypes[KIFGLOBALTHREAD[218]] = &KifCode::KifParseVarKifKiF;
	kifTypes[KIFGLOBALTHREAD[18]] = &KifCode::KifParseVarSelf;
	kifTypes[KIFGLOBALTHREAD[163]] = &KifCode::KifParseVarSelf;
	kifTypes[KIFGLOBALTHREAD[164]] = &KifCode::KifParseVarSelf;
	kifTypes[KIFGLOBALTHREAD[28]] = &KifCode::KifParseVarInt;
	kifTypes[KIFGLOBALTHREAD[81]] = &KifCode::KifParseVarByte;
	kifTypes[KIFGLOBALTHREAD[29]] = &KifCode::KifParseVarLong;
	kifTypes[KIFGLOBALTHREAD[61]] = &KifCode::KifParseVarShort;
	kifTypes[KIFGLOBALTHREAD[32]] = &KifCode::KifParseVarFraction;
	kifTypes[KIFGLOBALTHREAD[82]] = &KifCode::KifParseVarBit;
	kifTypes[KIFGLOBALTHREAD[83]] = &KifCode::KifParseVarBits;
	kifTypes[KIFGLOBALTHREAD[84]] = &KifCode::KifParseVarDate;
	kifTypes[KIFGLOBALTHREAD[76]] = &KifCode::KifParseVarTime;
	kifTypes[KIFGLOBALTHREAD[85]] = &KifCode::KifParseVarCall;
	kifTypes[KIFGLOBALTHREAD[75]] = &KifCode::KifParseVarRawString;
	kifTypes[KIFGLOBALTHREAD[27]] = &KifCode::KifParseVarString;
	kifTypes[KIFGLOBALTHREAD[73]] = &KifCode::KifParseVarUString;
	kifTypes[KIFGLOBALTHREAD[77]] = &KifCode::KifParseVarAutomaton;
	kifTypes[KIFGLOBALTHREAD[78]] = &KifCode::KifParseVarTransducer;
	kifTypes[KIFGLOBALTHREAD[79]] = &KifCode::KifParseVarGrammar;
	kifTypes[KIFGLOBALTHREAD[69]] = &KifCode::KifParseVarPredicate;
	kifTypes[KIFGLOBALTHREAD[219]] = &KifCode::KifParseVarPredicate;
	kifTypes[KIFGLOBALTHREAD[157]] = &KifCode::KifParseVarPredicateTerm;
	kifTypes[KIFGLOBALTHREAD[30]] = &KifCode::KifParseVarFloat;
	kifTypes[KIFGLOBALTHREAD[86]] = &KifCode::KifParseVarLoopString;
	kifTypes[KIFGLOBALTHREAD[87]] = &KifCode::KifParseVarLoopUString;
	kifTypes[KIFGLOBALTHREAD[88]] = &KifCode::KifParseVarLoopBoolean;
	kifTypes[KIFGLOBALTHREAD[89]] = &KifCode::KifParseVarLoopInteger;
	kifTypes[KIFGLOBALTHREAD[90]] = &KifCode::KifParseVarLoopFloat;
	kifTypes[KIFGLOBALTHREAD[22]] = &KifCode::KifParseVarVector;
	kifTypes[KIFGLOBALTHREAD[156]] = &KifCode::KifParseVarTable;
	kifTypes["enum"] = &KifCode::KifParseVarEnum;
	kifTypes[KIFGLOBALTHREAD[158]] = &KifCode::KifParseVarList;

	kifTypes[KIFGLOBALTHREAD[64]] = &KifCode::KifParseVarThrough;
	kifTypes[KIFGLOBALTHREAD[65]] = &KifCode::KifParseVarThrough;
	kifTypes[KIFGLOBALTHREAD[66]] = &KifCode::KifParseVarThrough;
	kifTypes[KIFGLOBALTHREAD[67]] = &KifCode::KifParseVarThrough;

	kifTypes[KIFGLOBALTHREAD[74]] = &KifCode::KifParseVarTuple;

	kifTypes[KIFGLOBALTHREAD[149]] = &KifCode::KifParseVarSet;
	kifTypes[KIFGLOBALTHREAD[150]] = &KifCode::KifParseVarSetString;
	kifTypes[KIFGLOBALTHREAD[151]] = &KifCode::KifParseVarSetUString;
	kifTypes[KIFGLOBALTHREAD[153]] = &KifCode::KifParseVarSetInteger;
	kifTypes[KIFGLOBALTHREAD[152]] = &KifCode::KifParseVarSetFloat;

	kifTypes[KIFGLOBALTHREAD[23]] = &KifCode::KifParseVarMap;
	kifTypes[KIFGLOBALTHREAD[165]] = &KifCode::KifParseVarMap;
	kifTypes[KIFGLOBALTHREAD[91]] = &KifCode::KifParseVarUStringMap;
	kifTypes[KIFGLOBALTHREAD[92]] = &KifCode::KifParseVarIntegerMap;
	kifTypes[KIFGLOBALTHREAD[93]] = &KifCode::KifParseVarFloatMap;

	kifTypes[KIFGLOBALTHREAD[50]] = &KifCode::KifParseVarMapString;
	kifTypes[KIFGLOBALTHREAD[51]] = &KifCode::KifParseVarMapInteger;
	kifTypes[KIFGLOBALTHREAD[52]] = &KifCode::KifParseVarMapFloat;
	kifTypes[KIFGLOBALTHREAD[172]] = &KifCode::KifParseVarMapString;
	kifTypes[KIFGLOBALTHREAD[173]] = &KifCode::KifParseVarMapInteger;
	kifTypes[KIFGLOBALTHREAD[174]] = &KifCode::KifParseVarMapFloat;

	kifTypes[KIFGLOBALTHREAD[166]] = &KifCode::KifParseVarMapIntegerString;
	kifTypes[KIFGLOBALTHREAD[167]] = &KifCode::KifParseVarMapIntegerInteger;
	kifTypes[KIFGLOBALTHREAD[168]] = &KifCode::KifParseVarMapIntegerFloat;
	kifTypes[KIFGLOBALTHREAD[169]] = &KifCode::KifParseVarMapFloatString;
	kifTypes[KIFGLOBALTHREAD[170]] = &KifCode::KifParseVarMapFloatInteger;
	kifTypes[KIFGLOBALTHREAD[171]] = &KifCode::KifParseVarMapFloatFloat;
	kifTypes[KIFGLOBALTHREAD[103]] = &KifCode::KifParseVarMapString;
	kifTypes[KIFGLOBALTHREAD[100]] = &KifCode::KifParseVarMapUStringUString;
	kifTypes[KIFGLOBALTHREAD[101]] = &KifCode::KifParseVarMapUStringInteger;
	kifTypes[KIFGLOBALTHREAD[102]] = &KifCode::KifParseVarMapUStringFloat;
	kifTypes[KIFGLOBALTHREAD[175]] = &KifCode::KifParseVarMapUStringUString;
	
	kifTypes[KIFGLOBALTHREAD[176]] = &KifCode::KifParseVarMapUStringInteger;
	kifTypes[KIFGLOBALTHREAD[177]] = &KifCode::KifParseVarMapUStringFloat;

	kifTypes[KIFGLOBALTHREAD[178]] = &KifCode::KifParseVarMapIntegerUString;
	kifTypes[KIFGLOBALTHREAD[220]] = &KifCode::KifParseVarMapFloatUString;
	kifTypes[KIFGLOBALTHREAD[106]] = &KifCode::KifParseVarMapIntegerUString;
	kifTypes[KIFGLOBALTHREAD[107]] = &KifCode::KifParseVarMapFloatUString;

	kifTypes[KIFGLOBALTHREAD[105]] = &KifCode::KifParseVarMapInteger;
	kifTypes[KIFGLOBALTHREAD[104]] = &KifCode::KifParseVarMapFloat;
	kifTypes[KIFGLOBALTHREAD[94]] = &KifCode::KifParseVarMapIntegerString;
	kifTypes[KIFGLOBALTHREAD[96]] = &KifCode::KifParseVarMapIntegerInteger;
	kifTypes[KIFGLOBALTHREAD[95]] = &KifCode::KifParseVarMapIntegerFloat;
	kifTypes[KIFGLOBALTHREAD[97]] = &KifCode::KifParseVarMapFloatString;
	kifTypes[KIFGLOBALTHREAD[99]] = &KifCode::KifParseVarMapFloatInteger;
	kifTypes[KIFGLOBALTHREAD[98]] = &KifCode::KifParseVarMapFloatFloat;

	kifTypes[KIFGLOBALTHREAD[108]] = &KifCode::KifParseVarTreeMap;
	kifTypes[KIFGLOBALTHREAD[179]] = &KifCode::KifParseVarTreeMap;
	kifTypes[KIFGLOBALTHREAD[109]] = &KifCode::KifParseVarUStringTreeMap;
	kifTypes[KIFGLOBALTHREAD[110]] = &KifCode::KifParseVarIntegerTreeMap;
	kifTypes[KIFGLOBALTHREAD[111]] = &KifCode::KifParseVarFloatTreeMap;

	kifTypes[KIFGLOBALTHREAD[186]] = &KifCode::KifParseVarTreeMapString;
	kifTypes[KIFGLOBALTHREAD[187]] = &KifCode::KifParseVarTreeMapInteger;
	kifTypes[KIFGLOBALTHREAD[188]] = &KifCode::KifParseVarTreeMapFloat;
	kifTypes[KIFGLOBALTHREAD[189]] = &KifCode::KifParseVarTreeMapString;
	kifTypes[KIFGLOBALTHREAD[190]] = &KifCode::KifParseVarTreeMapInteger;
	kifTypes[KIFGLOBALTHREAD[191]] = &KifCode::KifParseVarTreeMapFloat;

	kifTypes[KIFGLOBALTHREAD[180]] = &KifCode::KifParseVarTreeMapIntegerString;
	kifTypes[KIFGLOBALTHREAD[181]] = &KifCode::KifParseVarTreeMapIntegerInteger;
	kifTypes[KIFGLOBALTHREAD[182]] = &KifCode::KifParseVarTreeMapIntegerFloat;
	kifTypes[KIFGLOBALTHREAD[183]] = &KifCode::KifParseVarTreeMapFloatString;
	kifTypes[KIFGLOBALTHREAD[184]] = &KifCode::KifParseVarTreeMapFloatInteger;
	kifTypes[KIFGLOBALTHREAD[185]] = &KifCode::KifParseVarTreeMapFloatFloat;
	kifTypes[KIFGLOBALTHREAD[118]] = &KifCode::KifParseVarTreeMapString;
	kifTypes[KIFGLOBALTHREAD[119]] = &KifCode::KifParseVarTreeMapUStringUString;
	kifTypes[KIFGLOBALTHREAD[120]] = &KifCode::KifParseVarTreeMapUStringInteger;
	kifTypes[KIFGLOBALTHREAD[121]] = &KifCode::KifParseVarTreeMapUStringFloat;

	kifTypes[KIFGLOBALTHREAD[192]] = &KifCode::KifParseVarTreeMapUStringUString;
	kifTypes[KIFGLOBALTHREAD[193]] = &KifCode::KifParseVarTreeMapUStringInteger;
	kifTypes[KIFGLOBALTHREAD[194]] = &KifCode::KifParseVarTreeMapUStringFloat;

	kifTypes[KIFGLOBALTHREAD[195]] = &KifCode::KifParseVarTreeMapIntegerUString;
	kifTypes[KIFGLOBALTHREAD[221]] = &KifCode::KifParseVarTreeMapFloatUString;
	kifTypes[KIFGLOBALTHREAD[122]] = &KifCode::KifParseVarTreeMapIntegerUString;
	kifTypes[KIFGLOBALTHREAD[123]] = &KifCode::KifParseVarTreeMapFloatUString;

	kifTypes[KIFGLOBALTHREAD[125]] = &KifCode::KifParseVarTreeMapInteger;
	kifTypes[KIFGLOBALTHREAD[124]] = &KifCode::KifParseVarTreeMapFloat;
	kifTypes[KIFGLOBALTHREAD[112]] = &KifCode::KifParseVarTreeMapIntegerString;
	kifTypes[KIFGLOBALTHREAD[114]] = &KifCode::KifParseVarTreeMapIntegerInteger;
	kifTypes[KIFGLOBALTHREAD[113]] = &KifCode::KifParseVarTreeMapIntegerFloat;
	kifTypes[KIFGLOBALTHREAD[115]] = &KifCode::KifParseVarTreeMapFloatString;
	kifTypes[KIFGLOBALTHREAD[117]] = &KifCode::KifParseVarTreeMapFloatInteger;
	kifTypes[KIFGLOBALTHREAD[116]] = &KifCode::KifParseVarTreeMapFloatFloat;

	kifTypes[KIFGLOBALTHREAD[126]] = &KifCode::KifParseVarPrimeMap;
	kifTypes[KIFGLOBALTHREAD[196]] = &KifCode::KifParseVarPrimeMap;
	kifTypes[KIFGLOBALTHREAD[127]] = &KifCode::KifParseVarUStringPrimeMap;
	kifTypes[KIFGLOBALTHREAD[128]] = &KifCode::KifParseVarIntegerPrimeMap;
	kifTypes[KIFGLOBALTHREAD[134]] = &KifCode::KifParseVarFloatPrimeMap;

	kifTypes[KIFGLOBALTHREAD[129]] = &KifCode::KifParseVarIntegerBinMap;
	kifTypes[KIFGLOBALTHREAD[130]] = &KifCode::KifParseVarBinMapIntegerInteger;
	kifTypes[KIFGLOBALTHREAD[131]] = &KifCode::KifParseVarBinMapIntegerFloat;
	kifTypes[KIFGLOBALTHREAD[132]] = &KifCode::KifParseVarBinMapIntegerString;
	kifTypes[KIFGLOBALTHREAD[133]] = &KifCode::KifParseVarBinMapIntegerUString;
	kifTypes[KIFGLOBALTHREAD[197]] = &KifCode::KifParseVarBinMapIntegerInteger;
	kifTypes[KIFGLOBALTHREAD[198]] = &KifCode::KifParseVarBinMapIntegerFloat;
	kifTypes[KIFGLOBALTHREAD[199]] = &KifCode::KifParseVarBinMapIntegerString;
	kifTypes[KIFGLOBALTHREAD[200]] = &KifCode::KifParseVarBinMapIntegerUString;


	kifTypes[KIFGLOBALTHREAD[212]] = &KifCode::KifParseVarPrimeMapString;
	kifTypes[KIFGLOBALTHREAD[213]] = &KifCode::KifParseVarPrimeMapInteger;
	kifTypes[KIFGLOBALTHREAD[214]] = &KifCode::KifParseVarPrimeMapFloat;
	kifTypes[KIFGLOBALTHREAD[215]] = &KifCode::KifParseVarPrimeMapString;
	kifTypes[KIFGLOBALTHREAD[216]] = &KifCode::KifParseVarPrimeMapInteger;
	kifTypes[KIFGLOBALTHREAD[217]] = &KifCode::KifParseVarPrimeMapFloat;

	kifTypes[KIFGLOBALTHREAD[206]] = &KifCode::KifParseVarPrimeMapIntegerString;
	kifTypes[KIFGLOBALTHREAD[207]] = &KifCode::KifParseVarPrimeMapIntegerInteger;
	kifTypes[KIFGLOBALTHREAD[208]] = &KifCode::KifParseVarPrimeMapIntegerFloat;
	kifTypes[KIFGLOBALTHREAD[209]] = &KifCode::KifParseVarPrimeMapFloatString;
	kifTypes[KIFGLOBALTHREAD[210]] = &KifCode::KifParseVarPrimeMapFloatInteger;
	kifTypes[KIFGLOBALTHREAD[211]] = &KifCode::KifParseVarPrimeMapFloatFloat;
	kifTypes[KIFGLOBALTHREAD[141]] = &KifCode::KifParseVarPrimeMapString;
	kifTypes[KIFGLOBALTHREAD[143]] = &KifCode::KifParseVarPrimeMapInteger;
	kifTypes[KIFGLOBALTHREAD[142]] = &KifCode::KifParseVarPrimeMapFloat;
	kifTypes[KIFGLOBALTHREAD[135]] = &KifCode::KifParseVarPrimeMapIntegerString;
	kifTypes[KIFGLOBALTHREAD[137]] = &KifCode::KifParseVarPrimeMapIntegerInteger;
	kifTypes[KIFGLOBALTHREAD[136]] = &KifCode::KifParseVarPrimeMapIntegerFloat;
	kifTypes[KIFGLOBALTHREAD[138]] = &KifCode::KifParseVarPrimeMapFloatString;
	kifTypes[KIFGLOBALTHREAD[140]] = &KifCode::KifParseVarPrimeMapFloatInteger;
	kifTypes[KIFGLOBALTHREAD[139]] = &KifCode::KifParseVarPrimeMapFloatFloat;

	kifTypes[KIFGLOBALTHREAD[144]] = &KifCode::KifParseVarPrimeMapUStringUString;
	kifTypes[KIFGLOBALTHREAD[145]] = &KifCode::KifParseVarPrimeMapUStringInteger;
	kifTypes[KIFGLOBALTHREAD[146]] = &KifCode::KifParseVarPrimeMapUStringFloat;
	kifTypes[KIFGLOBALTHREAD[147]] = &KifCode::KifParseVarPrimeMapIntegerUString;
	kifTypes[KIFGLOBALTHREAD[148]] = &KifCode::KifParseVarPrimeMapFloatUString;
	kifTypes[KIFGLOBALTHREAD[201]] = &KifCode::KifParseVarPrimeMapUStringUString;
	kifTypes[KIFGLOBALTHREAD[202]] = &KifCode::KifParseVarPrimeMapUStringInteger;
	kifTypes[KIFGLOBALTHREAD[203]] = &KifCode::KifParseVarPrimeMapUStringFloat;
	kifTypes[KIFGLOBALTHREAD[204]] = &KifCode::KifParseVarPrimeMapIntegerUString;
	kifTypes[KIFGLOBALTHREAD[205]] = &KifCode::KifParseVarPrimeMapFloatUString;

	kifTypes[KIFGLOBALTHREAD[53]] = &KifCode::KifParseVarVectorString;
	kifTypes[KIFGLOBALTHREAD[154]] = &KifCode::KifParseVarVectorUString;
	kifTypes[KIFGLOBALTHREAD[54]] = &KifCode::KifParseVarVectorInteger;
	kifTypes[KIFGLOBALTHREAD[155]] = &KifCode::KifParseVarVectorByte;
	kifTypes[KIFGLOBALTHREAD[55]] = &KifCode::KifParseVarVectorFloat;
	kifTypes[KIFGLOBALTHREAD[31]] = &KifCode::KifParseVarBoolean;
	kifTypes[KIFGLOBALTHREAD[162]] = &KifCode::KifParseVarBoolean;
	kifTypes[KIFGLOBALTHREAD[160]] = &KifCode::KifParseVarFile;
	kifTypes[KIFGLOBALTHREAD[161]] = &KifCode::KifParseVarFileWide;
	kifTypes[KIFGLOBALTHREAD[159]] = &KifCode::KifParseVarIterator;
	kifTypes[KIFGLOBALTHREAD[222]] = &KifCode::KifParseVarRIterator;
	kifTypes[KIFGLOBALTHREAD[223]] = &KifCode::KifParseVarTree;


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
	kifOperators[KIFGLOBALTHREAD[224]] = kifAFFECTATION;
	kifOperators[KIFGLOBALTHREAD[225]] = kifDIFFERENT;
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
	kifOperators[KIFGLOBALTHREAD[226]] = kifOR;
	kifOperators[KIFGLOBALTHREAD[227]] = kifAND;
	kifOperators[KIFGLOBALTHREAD[9]] = kifIN;
	kifOperators[KIFGLOBALTHREAD[228]] = kifON;
	kifOperators[KIFGLOBALTHREAD[229]] = kifNOTIN;

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
	kifOperatorString[kifIN] = KIFGLOBALTHREAD[9];
	kifOperatorString[kifON] = KIFGLOBALTHREAD[228];
	kifOperatorString[kifNOTIN] = KIFGLOBALTHREAD[229];


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
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[230],false);
	InitMapBooleanString(kifPredefinedFunctions, "json");
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[31]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[232],false);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[233],false);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[234], false);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[166], false);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[235], false);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[236]);
	InitMapBooleanString(kifPredefinedFunctions, "_trace");
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[60],false);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[20]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[237], false);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[238], false);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[239]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[22]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[23]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[240]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[62]);
	InitMapBooleanString(kifPredefinedFunctions, KIFGLOBALTHREAD[63]);
	InitMapBooleanString(kifPredefinedFunctions, "_id", false);

	InitMapBooleanString(kifGrammarFunctions, KIFGLOBALTHREAD[241]);
	InitMapBooleanString(kifGrammarFunctions, KIFGLOBALTHREAD[24]);
	InitMapBooleanString(kifGrammarFunctions, KIFGLOBALTHREAD[21], false);
	InitMapBooleanString(kifGrammarFunctions, KIFGLOBALTHREAD[231], false);

	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[241]);
	InitMapBooleanString(kifAutomatonFunctions, "dump");
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[242]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[243]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[244]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[245]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[246]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[247]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[20]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[248]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[249]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[239]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[250]);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifAutomatonFunctions, KIFGLOBALTHREAD[251]);

	InitMapBooleanString(kifTransducerFunctions, KIFGLOBALTHREAD[241]);
	InitMapBooleanString(kifTransducerFunctions, KIFGLOBALTHREAD[250]);
	InitMapBooleanString(kifTransducerFunctions, KIFGLOBALTHREAD[252]);
	InitMapBooleanString(kifTransducerFunctions, KIFGLOBALTHREAD[247]);
	InitMapBooleanString(kifTransducerFunctions, KIFGLOBALTHREAD[253]);
	InitMapBooleanString(kifTransducerFunctions, KIFGLOBALTHREAD[254]);
	InitMapBooleanString(kifTransducerFunctions, KIFGLOBALTHREAD[255]);
	InitMapBooleanString(kifTransducerFunctions, KIFGLOBALTHREAD[256]);


	InitMapBooleanString(kifTreeFunctions, "json");
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[31]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[23]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[22]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[257]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[249]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[258]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[259]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[260]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[261]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[262]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[263]);
	InitMapBooleanString(kifTreeFunctions, KIFGLOBALTHREAD[264]);

	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifStringFunctions, "json");
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[31]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[22]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[23]);

	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[265], false);
	
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[266], false);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[20]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[26]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[267]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[268], false);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[269], false);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[270]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[271]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[272]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[273]);

	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[22]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[274]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[275]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[38]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[276]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[277]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[278]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[279]);
	InitMapBooleanString(kifStringFunctions, "regexip");
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[280]);
	InitMapBooleanString(kifStringFunctions, "html");
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[281]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[282]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[283]);
	InitMapBooleanString(kifStringFunctions, "levenshtein");
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[249]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[284]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[285]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[286]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[287]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[288]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[289]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[290]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[291]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[292]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[293]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[294]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[295]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[296]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[297]);
	InitMapBooleanString(kifStringFunctions, "utf8");
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[298]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[299]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[300]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[301]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[302]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[303]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[304]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[305]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[306]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[307]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[308]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[309]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[310]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[311]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[312]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[313]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[314]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[315]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[316]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[317]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[318]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[319]);	
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[320]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[321]);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[232],false);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[233],false);
	InitMapBooleanString(kifStringFunctions, KIFGLOBALTHREAD[239]);


	InitMapBooleanString(kifPredicateFunctions, KIFGLOBALTHREAD[322]);
	InitMapBooleanString(kifPredicateFunctions, KIFGLOBALTHREAD[247]);
	InitMapBooleanString(kifPredicateFunctions, KIFGLOBALTHREAD[323]);
	InitMapBooleanString(kifPredicateFunctions, KIFGLOBALTHREAD[324]);



	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[20]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[249]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[279]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[325]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[267]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[326]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[321]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[275]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[297]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[327]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[69]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[269]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[328]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[329]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[330]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[331]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[332]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[38]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[320]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[237], false);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[238], false);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[333]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[334]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[335]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[336]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[337]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[338]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[339]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[340]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[341]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[342]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[239]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[24]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[232],false);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[233],false);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifVectorFunctions, "json");
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[23]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[22]);
	InitMapBooleanString(kifVectorFunctions, KIFGLOBALTHREAD[31]);

	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[20]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[325]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[249]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[267]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[334]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[335]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[321]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[329]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[343]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[38]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[344]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[345]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[346]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[336]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[327]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[341]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[330]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[342]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[237], false);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[238], false);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[331]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[24]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[320]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[347]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[239]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[232],false);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[233],false);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifListFunctions, "json");
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[22]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[23]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifListFunctions, KIFGLOBALTHREAD[31]);


	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[241]);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[348]);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[349]);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[350], false);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[351]);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[352]);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[353]);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[354]);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[355]);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[356]);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[357]);
	InitMapBooleanString(kifKiFFunctions, KIFGLOBALTHREAD[236]);

	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[20]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[325]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[358]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[249]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[22]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[23]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[331]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[333]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[239]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[341]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[330]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[342]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[38]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[327]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[69]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[232],false);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[359]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[360]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[361]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[238], false);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[237], false);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[233],false);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifMapFunctions, "json");
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[24]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[31]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[362]);
	InitMapBooleanString(kifMapFunctions, KIFGLOBALTHREAD[363]);

	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[20]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[249]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[325]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[275]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[327]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[269]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[328]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[330]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[331]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[38]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[237], false);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[238], false);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[341]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[342]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[239]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[232],false);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[233],false);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifSetFunctions, "json");
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[23]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[22]);
	InitMapBooleanString(kifSetFunctions, KIFGLOBALTHREAD[31]);

	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[292]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[364]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[365]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[366]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[367]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[368]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[369]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[26]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[370]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[371]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[372]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[373]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[374]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[375]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[376]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[377]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[378]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[20]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[232],false);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[233],false);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[379]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifFileFunctions, "json");
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifFileFunctions, KIFGLOBALTHREAD[31]);

	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[380]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[84]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[381]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[382]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[383]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[384]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[385]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[386]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[387]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[270]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[388]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[232],false);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[233],false);

	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifDateFunctions, "json");
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifDateFunctions, KIFGLOBALTHREAD[31]);

	InitMapBooleanString(kifTimeFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifTimeFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifTimeFunctions, KIFGLOBALTHREAD[233],false);

	InitMapBooleanString(kifTimeFunctions, KIFGLOBALTHREAD[389]);
	InitMapBooleanString(kifTimeFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifTimeFunctions, "json");
	InitMapBooleanString(kifTimeFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifTimeFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifTimeFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifTimeFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifTimeFunctions, KIFGLOBALTHREAD[31]);

	//abs,acos,acosh,asin,asinh,atan,atanh,cbrt,cos,cosh,erf,erfc,exp,exp2,expm1,floor,
	//lgamma,ln,log,log1p,log2,logb,nearbyint,rint,round,sin,sinh,sqrt,tan,tanh,tgamma,
	//trunc,"@pi","@e"
	//Mathematics Function
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[390])] = fabs;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[391])] = acos;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[392])] = acosh;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[393])] = asin;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[394])] = asinh;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[395])] = atan;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[396])] = atanh;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[397])] = cbrt;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[398])] = cos;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[399])] = cosh;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[400])] = erf;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[401])] = erfc;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[402])] = exp;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[403])] = exp2;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[404])] = expm1;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[405])] = floor;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[406])] = lgamma;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[407])] = log;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[408])] = log10;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[409])] = log1p;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[410])] = log2;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[411])] = logb;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[412])] = nearbyint;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[413])] = rint;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[414])] = round;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[415])] = sin;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[416])] = sinh;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[417])] = sqrt;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[418])] = tan;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[419])] = tanh;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[420])] = tgamma;
	kifMathFunctions[KifNewId(KIFGLOBALTHREAD[421])] = trunc;

	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[232],false);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[233],false);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[422], false);

	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[423], false);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[27]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[22]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[20]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[269], false);
	InitMapBooleanString(kifNumericalFunctions, "json");
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[424]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[32]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[425]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[426]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[271]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[270]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[427]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[428]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[429]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[430]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[28]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[82]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[29]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[61]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[30]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[31]);
	InitMapBooleanString(kifNumericalFunctions, "#", false);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[431]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[358]);
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[432]);
	InitMapBooleanString(kifNumericalFunctions, "d");
	InitMapBooleanString(kifNumericalFunctions, "n");
	InitMapBooleanString(kifNumericalFunctions, KIFGLOBALTHREAD[433]);


	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[434]);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[435]);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[436]);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[437]);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[438]);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[439]);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[440]);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[24]);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[441]);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[232],false);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[21],false);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[231],false);
	InitMapBooleanString(kifIteratorFunctions, KIFGLOBALTHREAD[233],false);

	kifConversionNames[KIFGLOBALTHREAD[27]] = true;
	kifConversionNames[KIFGLOBALTHREAD[29]] = true;
	kifConversionNames[KIFGLOBALTHREAD[61]] = true;
	kifConversionNames[KIFGLOBALTHREAD[28]] = true;
	kifConversionNames[KIFGLOBALTHREAD[31]] = true;
	kifConversionNames[KIFGLOBALTHREAD[30]] = true;
	kifConversionNames[KIFGLOBALTHREAD[22]] = true;
	kifConversionNames[KIFGLOBALTHREAD[23]] = true;

	//We only create these values ONCE

	//===================================================================	

	kifIdType[kifString] = KifNewId(KIFGLOBALTHREAD[27]);
	kifIdType[kifInteger] = KifNewId(KIFGLOBALTHREAD[28]);
	kifIdType[kifLong] = KifNewId(KIFGLOBALTHREAD[29]);
	kifIdType[kifFraction] = KifNewId(KIFGLOBALTHREAD[32]);
	kifIdType[kifBit] = KifNewId(KIFGLOBALTHREAD[82]);
	kifIdType[kifBits] = KifNewId(KIFGLOBALTHREAD[83]);
	kifIdType[kifByte] = KifNewId(KIFGLOBALTHREAD[81]);
	kifIdType[kifDate] = KifNewId(KIFGLOBALTHREAD[84]);
	kifIdType[kifCall] = KifNewId(KIFGLOBALTHREAD[85]);
	kifIdType[kifFloat] = KifNewId(KIFGLOBALTHREAD[30]);
	kifIdType[kifBoolean] = KifNewId(KIFGLOBALTHREAD[31]);
	kifIdType[kifSelf] = KifNewId(KIFGLOBALTHREAD[18]);
	kifIdType[kifMap] = KifNewId(KIFGLOBALTHREAD[23]);
	kifIdType[kifVector] = KifNewId(KIFGLOBALTHREAD[22]);
	kifIdType[kifTable] = KifNewId(KIFGLOBALTHREAD[156]);
	kifIdType[kifList] = KifNewId(KIFGLOBALTHREAD[158]);
	kifIdType[kifIterator] = KifNewId(KIFGLOBALTHREAD[159]);
	kifIdType[kifFile] = KifNewId(KIFGLOBALTHREAD[160]);
	kifIdType[kifFileWide] = KifNewId(KIFGLOBALTHREAD[161]);

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

	kifReturnString[KifNewId(KIFGLOBALTHREAD[283])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[284])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[294])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[295])] = true;
	kifReturnString[KifNewId("utf8")] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[299])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[300])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[296])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[298])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[301])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[302])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[303])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[304])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[305])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[314])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[315])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[316])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[317])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[318])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[319])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[38])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[320])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[231])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[233])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[27])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[86])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[87])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[270])] = true;
	kifReturnString[KifNewId(KIFGLOBALTHREAD[271])] = true;

	kifReturnInt[idint] = true;
	kifReturnInt[idlong] = true;
	kifReturnInt[idsize] = true;
	kifReturnInt[KifNewId(KIFGLOBALTHREAD[89])] = true;

	kifReturnInt[KifNewId(KIFGLOBALTHREAD[291])] = true;
	kifReturnInt[KifNewId(KIFGLOBALTHREAD[292])] = true;
	kifReturnInt[KifNewId(KIFGLOBALTHREAD[89])] = true;
	kifReturnInt[KifNewId(KIFGLOBALTHREAD[293])] = true;
	kifReturnInt[KifNewId("levenshtein")] = true;
	kifReturnInt[KifNewId(KIFGLOBALTHREAD[249])] = true;

	kifReturnFloat[KifNewId(KIFGLOBALTHREAD[30])] = true;
	kifReturnFloat[KifNewId(KIFGLOBALTHREAD[90])] = true;
	kifReturnFloat[KifNewId(KIFGLOBALTHREAD[427])] = true;
	kifReturnFloat[KifNewId(KIFGLOBALTHREAD[428])] = true;
	kifReturnFloat[KifNewId(KIFGLOBALTHREAD[429])] = true;
	kifReturnFloat[KifNewId(KIFGLOBALTHREAD[430])] = true;
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
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[257], P_ONE | P_TWO, &PreTreeNext, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[21], P_ONE, &PreMethods, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[231], P_TWO, &PreInfos, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[258], P_ONE | P_TWO, &PreTreePrevious, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[259], P_ONE | P_TWO, &PreTreeParent, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[260], P_ONE | P_TWO, &PreTreeChild, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[261], P_ONE | P_TWO, &PreTreeLast, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[262], P_ONE, &PreTreeDepth, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[263], P_ONE, &PreTreeRemove, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[264], P_ONE, &PreTreeIsolate, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[27], P_ONE, &PreConvertString, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[30], P_ONE, &PreConvertFloat, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[32], P_ONE, &PreConvertFraction, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[28], P_ONE, &PreConvertInteger, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[29], P_ONE, &PreConvertLong, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[31], P_ONE, &PreConvertBoolean, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[22], P_ONE, &PreConvertVector, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[23], P_ONE, &PreConvertMap, true);

	KifCreatePredefined(kifcodeptr, "_id", P_ONE, &PreId, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[325], P_ONE, &PreDuplicate);

	KifCreatePredefined(kifcodeptr, "_variables", P_NONE | P_ONE, &PreTheVariables, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[442], P_NONE, &PreTheStack, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[443], P_NONE, &PreCurrentFilename, true);


	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[266], P_ONE, &PreHash);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[434], P_ONE, &PreNext);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[311], P_ATLEASTFOUR, &PreExtract);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[265], P_TWO, &PreRedictectOutput);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[423], P_TWO, &PreRestateOutput);

	KifCreatePredefined(kifcodeptr, "json", P_ONE, &PreJSon);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[27], P_ONE, &PreConvertString);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[30], P_ONE, &PreConvertFloat);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[32], P_ONE, &PreConvertFraction);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[28], P_ONE, &PreConvertInteger);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[29], P_ONE, &PreConvertLong);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[31], P_ONE, &PreConvertBoolean);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[22], P_ONE, &PreConvertVector);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[23], P_ONE, &PreConvertMap);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[362], P_ONE, &PreEvaluationMap);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[363], P_TWO, &PreSetSeeds);


	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[62], P_FULL, &PreWaitOn);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[240], P_FULL, &PreAddToWaitOn);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[63], P_TWO, &PreTrigger);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[390], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[391], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[392], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[393], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[394], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[395], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[396], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[397], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[398], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[399], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[400], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[401], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[402], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[403], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[404], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[405], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[406], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[407], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[408], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[409], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[410], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[411], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[412], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[413], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[414], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[415], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[416], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[417], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[418], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[419], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[420], P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[421], P_ONE, &PreMath);	
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[425], P_ONE, &PrePrimefactors);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[426], P_ONE, &PrePrime);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[271], P_TWO, &PreBase);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[82], P_TWO, &PreGetBit);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[444], P_ONE | P_TWO, &PrePause);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[445], P_ONE, &PreSleep);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[446], P_ONE, &PreEval, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[447], P_ONE, &PreFromJSON, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[448], P_TWO, &PreRun);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[349], P_TWO, &PreRunAsThread);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[351], P_ONE, &PreRunEnd);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[350], P_ONE | P_NONE, &PreRunStart);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[355], P_ONE, &PreOpenSession);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[356], P_ONE, &PreCloseSession);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[354], P_TWO | P_THREE, &PreCompile);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[449], P_THREE | P_TWO, &PreRange);


	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[256], P_TWO, &PreProcess);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[254], P_FOUR | P_TWO, &PreLookup);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[255], P_TWO, &PreLookdown);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[252], P_FIVE | P_FOUR, &PreBuild);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[253], P_THREE | P_FOUR, &PreCompilergx);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[21], P_ONE, &PreMethods);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[231], P_TWO, &PreInfos);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[232], P_TWO, &PreTypeIs);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[233], P_ONE, &PreType);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[234], P_ONE, &PreIsContainer);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[166], P_ONE, &PreIsMap);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[235], P_ONE, &PreIsVector);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[236], P_ONE, &PreName);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[17], P_ONE | P_NONE, &PreCatch);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[450], P_ONE, &PreRaise);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[243], P_TWO, &PreSetmodel);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[244], P_TWO | P_THREE, &PreLook);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[245], P_TWO, &Prespotfinalstate);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[251], P_TWO, &PreSetpath);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[246], P_TWO | P_THREE, &PreLoadCompact);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[242], P_TWO | P_THREE, &PreLoadDouble);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[250], P_TWO | P_THREE | P_FOUR, &PreAddword);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[248], P_TWO, &PreSetcodefeat);
	KifCreatePredefined(kifcodeptr, "dump", P_TWO, &PreDump);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[241], P_TWO | P_THREE, &PreLoad);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[357], P_ONE, &PreExposed);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[267], P_ONE, &PreReverse);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[451], P_ONE, &PreLock);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[452], P_ONE, &PreUnlock);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[453], P_ONE, &PreLockRec);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[454], P_ATLEASTONE, &PreWait);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[455], P_ONE, &PreWaitOnFalse);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[456], P_NONE, &PreWaitOnJoin);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[457], P_NONE, &PreWaiting);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[458], P_ONE | P_NONE, &PreCast);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[459], P_ONE | P_NONE, &PreKill);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[460], P_ONE, &PreErrorOnKey);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[461], P_NONE, &PreGarbageSurvey);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[462], P_NONE, &PreGarbageFrame);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[463], P_FULL, &PreGCSize);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[464], P_NONE, &PreVersion);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[465], P_ONE | P_NONE, &PreExit);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[466], P_FULL, &PrePrint);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[467], P_FULL, &PrePrintFlush);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[468], P_FULL, &PrePrintLN);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[469], P_ONE | P_TWO | P_THREE, &PrePrintJoin);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[470], P_ONE | P_TWO | P_THREE, &PrePrintJoinErr);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[471], P_ONE | P_TWO | P_THREE, &PrePrintJoinLN);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[472], P_ONE| P_TWO | P_THREE, &PrePrintJoinErrLN);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[473], P_NONE | P_ONE, &PrePredicateDump);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[474], P_NONE | P_ONE, &PrePredicateDump);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[475], P_NONE | P_ONE, &PreRetractAll);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[476], P_FULL, &PrePrinterr);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[477], P_FULL, &PrePrinterrLN);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[366], P_TWO, &PreOpenReadfile);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[367], P_TWO, &PreSetSignature);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[368], P_ONE, &PreGetSignature);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[364], P_TWO, &PreOpenWritefile);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[365], P_TWO, &PreOpenAppendfile);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[478], P_NONE, &PreThreadId, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[479], P_NONE, &PreParentThreadId, true);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[480], P_ONE | P_TWO, &PreSetDebugFunction, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[352], P_THREE, &PreSetKifCodeDebugFunction);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[481], P_ONE, &PreCloseKifCodeDebugFunction);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[482], P_ONE, &PreSetDebugData, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[353], P_NONE, &PreDebugClose, true);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[483], P_ONE, &PreGetInfoStackOnId, true);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[276], P_ONE, &PreSizeBytes);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[369], P_ONE, &PreFlushFile);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[379], P_ONE, &PreEndOfFile);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[377], P_ONE | P_TWO, &PreRead);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[378], P_ONE, &PreReadOneLine);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[372], P_TWO, &PreSeek);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[373], P_ONE, &PreTell);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[374], P_ATLEASTTWO, &PreWrite);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[375], P_ATLEASTTWO, &PreWriteLN);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[376], P_ATLEASTTWO, &PreWriteBin);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[371], P_ONE, &PreClose);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[321], P_TWO | P_THREE, &PreInsert);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[322], P_ATLEASTTWO, &PreQuery);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[247], P_ONE | P_TWO | P_THREE| P_FOUR, &PreStore);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[324], P_ONE|P_TWO, &PrePredicateWeight);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[323], P_ONE | P_TWO, &PreRemove);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[275], P_ATLEASTTWO, &PrePush);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[279], P_TWO, &PreResize);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[326], P_THREE, &PreMoving);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[327], P_ATLEASTTWO, &PreMerge);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[69], P_ONE, &PrePredicate);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[343], P_ATLEASTTWO, &PrePushFirst);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[345], P_ATLEASTTWO, &PrePushLast);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[331], P_THREE | P_TWO, &PreJoin);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[332], P_TWO, &PreReserve);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[38], P_ONE | P_TWO | P_THREE, &PrePop);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[344], P_ONE, &PrePopLast);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[346], P_ONE, &PrePopFirst);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[320], P_ONE, &PreLast);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[347], P_ONE, &PreFirst);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[336], P_ONE, &PreUnique);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[333], P_TWO, &PreSort);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[334], P_ONE, &PrePermute);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[335], P_ONE, &PreShuffle);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[337], P_TWO, &PreSortInt);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[340], P_TWO, &PreSortLong);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[338], P_TWO, &PreSortFloat);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[339], P_TWO, &PreSortString);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[341], P_ONE, &PreSum);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[342], P_ONE, &PreProduct);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[239], P_ONE, &PreClear);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[360], P_ONE, &PreValues);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[361], P_ONE, &PreItems);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[359], P_ONE, &PreKeys);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[484], P_ONE, &PreInkey, true);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[431], P_ONE, &PreFractionSimplify);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[358], P_ONE, &PreFractionSwitch);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[424], P_TWO, &PrePartOf);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[433], P_ONE, &PreEuclidian);
	KifCreatePredefined(kifcodeptr, "d", P_TWO | P_ONE, &PreDenominator);
	KifCreatePredefined(kifcodeptr, "n", P_TWO | P_ONE, &PreNumerator);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[432], P_THREE, &PreNumeratorDenominator);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[237], P_ATLEASTONE, &PreMin);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[238], P_ATLEASTONE, &PreMax);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[422], P_ONE, &PreChr);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[268], P_ONE, &PreOrd);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[269], P_ONE, &PreBytes);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[328], P_ONE, &PreBytes);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[329], P_ONE, &PreFlatten);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[272], P_THREE, &PreFill);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[273], P_THREE | P_FOUR, &PrePadding);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[270], P_ATLEASTTWO, &PreFormat);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[427], P_ONE, &PreRadian);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[428], P_ONE, &PreEven);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[429], P_ONE, &PreOdd);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[430], P_ONE, &PreDegree);

	//Rules	
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[436], P_TWO, &PreSet);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[26], P_ONE | P_TWO, &PreGet);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[485], P_NONE, &PreGetc);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[285], P_TWO, &PreGetSTD);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[286], P_TWO, &PreGetERR);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[486], P_NONE | P_ONE, &PreRandom);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[24], P_ATLEASTONE, &PreApply);

	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[370], P_ONE | P_TWO, &PreUnGet);


	//String functions
	KifCreatePredefined(kifcodeptr, "#", P_ONE, &PreComplement);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[230], P_NONE | P_ONE, &PreCompatibilities);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[274], P_ONE | P_THREE, &PreParse);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[277], P_ONE | P_THREE, &PreLisp);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[278], P_THREE, &PreTags);
	KifCreatePredefined(kifcodeptr, "regexip", P_TWO, &PreXipRegex);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[280], P_ONE, &PreEvaluateMeta);
	KifCreatePredefined(kifcodeptr, "html", P_ONE, &PreHTMLEncoding);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[281], P_ONE, &PreHTMLEncoding);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[282], P_ONE, &PreXMLEncoding);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[283], P_THREE, &PreXipReplaceRgx);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[284], P_THREE, &PreReplace);
	KifCreatePredefined(kifcodeptr, "levenshtein", P_TWO | P_THREE, &PreLevenshtein);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[249], P_TWO | P_THREE | P_FOUR, &PreLevenshtein);
	KifCreatePredefined(kifcodeptr, "_trace", P_TWO, &PreTrace);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[60], P_ONE|P_TWO|P_THREE, &PreMode);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[287], P_THREE | P_TWO | P_ONE, &PreSplit);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[288], P_THREE | P_TWO | P_ONE, &PreSplitKeepEmptyStrings);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[289], P_ONE | P_TWO | P_THREE | P_FOUR, &PreTokenize);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[290], P_ONE|P_TWO, &PreSTokenize);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[292], P_THREE | P_TWO, &PreFind);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[291], P_FOUR | P_THREE | P_TWO | P_ONE, &PreCount);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[330], P_TWO, &PreTest);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[293], P_THREE | P_TWO, &PreRFind);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[294], P_TWO, &PreRemovefirst);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[295], P_TWO, &PreRemovelast);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[297], P_TWO | P_THREE, &PreNGRAMS);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[296], P_ONE, &PreTestUTF8);
	KifCreatePredefined(kifcodeptr, "utf8", P_ONE|P_TWO, &PreLatinToUTF8);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[298], P_ONE, &PreUTF8ToLatin);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[299], P_ONE, &PreToDOS);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[300], P_ONE, &PreDOSToUTF8);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[301], P_TWO, &PreLeft);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[302], P_TWO, &PreBytePosition);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[303], P_TWO, &PreCharacterPosition);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[304], P_TWO, &PreRight);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[305], P_THREE, &PreMid);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[313], P_ONE, &PreIsLower);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[312], P_ONE, &PreIsUpper);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[306], P_ONE, &PreIsAlpha);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[307], P_ONE, &PreIsConsonants);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[308], P_ONE, &PreIsVowels);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[309], P_ONE, &PreIsPunctuation);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[310], P_ONE, &PreIsDigit);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[316], P_ONE, &PreLower);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[314], P_ONE, &PreUpper);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[315], P_ONE, &PreDeaccentuate);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[317], P_ONE, &PreTrim);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[318], P_ONE, &PreTrimleft);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[319], P_ONE, &PreTrimright);

	//Time
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[380], P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX | P_SEVEN | P_EIGHT, &PreSetDate);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[389], P_ONE, &PreResetTime);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[84], P_ONE, &PreDate);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[381], P_ONE | P_TWO, &PreYear);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[382], P_ONE | P_TWO, &PreMonth);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[383], P_ONE | P_TWO, &PreDay);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[386], P_ONE | P_TWO, &PreHour);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[387], P_ONE | P_TWO, &PreMinute);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[388], P_ONE | P_TWO, &PreSecond);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[385], P_ONE, &PreWeekDay);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[384], P_ONE, &PreYearDay);

	//Iterator functions
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[438], P_ONE, &PreBegin);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[435], P_ONE, &PreEnd);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[437], P_ONE, &PreNend);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[439], P_ONE, &PreIteratorKey);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[440], P_ONE | P_TWO, &PreValue);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[441], P_ONE, &PreValueType);
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[487], P_TWO, &PreValueTypeIs);



	//Function for: string,map,vector
	KifCreatePredefined(kifcodeptr, KIFGLOBALTHREAD[20], P_ONE, &PreSize);

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
	kifInformationTransducer[KIFGLOBALTHREAD[252]] = KIFGLOBALTHREAD[488];
	kifInformationTransducer[KIFGLOBALTHREAD[247]] = KIFGLOBALTHREAD[489];
	kifInformationTransducer[KIFGLOBALTHREAD[253]] = KIFGLOBALTHREAD[490];
	kifInformationTransducer[KIFGLOBALTHREAD[241]] = KIFGLOBALTHREAD[491];
	kifInformationTransducer[KIFGLOBALTHREAD[250]] = KIFGLOBALTHREAD[492];
	kifInformationTransducer[KIFGLOBALTHREAD[254]] = KIFGLOBALTHREAD[493];
	kifInformationTransducer[KIFGLOBALTHREAD[255]] = KIFGLOBALTHREAD[494];
	kifInformationTransducer[KIFGLOBALTHREAD[256]] = KIFGLOBALTHREAD[495];


	kifInformationPredicate[KIFGLOBALTHREAD[324]] = KIFGLOBALTHREAD[496];
	kifInformationPredicate[KIFGLOBALTHREAD[322]] = KIFGLOBALTHREAD[497];
	kifInformationPredicate[KIFGLOBALTHREAD[247]] = KIFGLOBALTHREAD[498];
	kifInformationPredicate[KIFGLOBALTHREAD[323]] = KIFGLOBALTHREAD[499];

	kifInformationString[KIFGLOBALTHREAD[266]] = KIFGLOBALTHREAD[500];
	kifInformationString[KIFGLOBALTHREAD[279]] = KIFGLOBALTHREAD[501];
	kifInformationString["json"] = KIFGLOBALTHREAD[502];
	kifInformationString[KIFGLOBALTHREAD[302]] = KIFGLOBALTHREAD[503];
	kifInformationString[KIFGLOBALTHREAD[303]] = KIFGLOBALTHREAD[504];
	kifInformationString[KIFGLOBALTHREAD[20]] = KIFGLOBALTHREAD[505];
	kifInformationString[KIFGLOBALTHREAD[280]] = KIFGLOBALTHREAD[506];
	kifInformationString[KIFGLOBALTHREAD[239]] = KIFGLOBALTHREAD[507];
	kifInformationString[KIFGLOBALTHREAD[289]] = KIFGLOBALTHREAD[508];
	kifInformationString[KIFGLOBALTHREAD[290]] = KIFGLOBALTHREAD[509];
	kifInformationString[KIFGLOBALTHREAD[287]] = KIFGLOBALTHREAD[510];
	kifInformationString[KIFGLOBALTHREAD[288]] = KIFGLOBALTHREAD[511];
	kifInformationString[KIFGLOBALTHREAD[294]] = KIFGLOBALTHREAD[512];
	kifInformationString[KIFGLOBALTHREAD[295]] = KIFGLOBALTHREAD[513];
	kifInformationString[KIFGLOBALTHREAD[272]] = KIFGLOBALTHREAD[514];
	kifInformationString[KIFGLOBALTHREAD[273]] = KIFGLOBALTHREAD[515];
	kifInformationString[KIFGLOBALTHREAD[270]] = KIFGLOBALTHREAD[516];
	kifInformationString[KIFGLOBALTHREAD[268]] = KIFGLOBALTHREAD[517];
	kifInformationString[KIFGLOBALTHREAD[271]] = KIFGLOBALTHREAD[518];
	kifInformationString[KIFGLOBALTHREAD[301]] = KIFGLOBALTHREAD[519];
	kifInformationString[KIFGLOBALTHREAD[302]] = KIFGLOBALTHREAD[520];
	kifInformationString[KIFGLOBALTHREAD[303]] = KIFGLOBALTHREAD[521];
	kifInformationString[KIFGLOBALTHREAD[304]] = KIFGLOBALTHREAD[522];
	kifInformationString[KIFGLOBALTHREAD[305]] = KIFGLOBALTHREAD[523];
	kifInformationString[KIFGLOBALTHREAD[267]] = KIFGLOBALTHREAD[524];
	kifInformationString[KIFGLOBALTHREAD[38]] = KIFGLOBALTHREAD[525];
	kifInformationString[KIFGLOBALTHREAD[38]] = KIFGLOBALTHREAD[526];
	kifInformationString[KIFGLOBALTHREAD[320]] = KIFGLOBALTHREAD[527];
	kifInformationString[KIFGLOBALTHREAD[321]] = KIFGLOBALTHREAD[528];
	kifInformationString[KIFGLOBALTHREAD[317]] = KIFGLOBALTHREAD[529];
	kifInformationString[KIFGLOBALTHREAD[319]] = KIFGLOBALTHREAD[530];
	kifInformationString[KIFGLOBALTHREAD[318]] = KIFGLOBALTHREAD[531];
	kifInformationString[KIFGLOBALTHREAD[315]] = KIFGLOBALTHREAD[532];
	kifInformationString[KIFGLOBALTHREAD[314]] = KIFGLOBALTHREAD[533];
	kifInformationString[KIFGLOBALTHREAD[316]] = KIFGLOBALTHREAD[534];
	kifInformationString[KIFGLOBALTHREAD[313]] = KIFGLOBALTHREAD[535];
	kifInformationString[KIFGLOBALTHREAD[312]] = KIFGLOBALTHREAD[536];
	kifInformationString[KIFGLOBALTHREAD[306]] = KIFGLOBALTHREAD[537];
	kifInformationString[KIFGLOBALTHREAD[307]] = KIFGLOBALTHREAD[538];
	kifInformationString[KIFGLOBALTHREAD[308]] = KIFGLOBALTHREAD[539];
	kifInformationString[KIFGLOBALTHREAD[309]] = KIFGLOBALTHREAD[540];
	kifInformationString[KIFGLOBALTHREAD[311]] = KIFGLOBALTHREAD[541];
	kifInformationString[KIFGLOBALTHREAD[310]] = KIFGLOBALTHREAD[542];
	kifInformationString[KIFGLOBALTHREAD[298]] = KIFGLOBALTHREAD[543];
	kifInformationString["utf8"] = KIFGLOBALTHREAD[544];
	kifInformationString[KIFGLOBALTHREAD[299]] = KIFGLOBALTHREAD[545];
	kifInformationString[KIFGLOBALTHREAD[300]] = KIFGLOBALTHREAD[546];
	kifInformationString[KIFGLOBALTHREAD[296]] = KIFGLOBALTHREAD[547];
	kifInformationString[KIFGLOBALTHREAD[297]] = KIFGLOBALTHREAD[548];
	kifInformationString[KIFGLOBALTHREAD[291]] = KIFGLOBALTHREAD[549];
	kifInformationString[KIFGLOBALTHREAD[292]] = KIFGLOBALTHREAD[550];
	kifInformationString[KIFGLOBALTHREAD[276]] = KIFGLOBALTHREAD[551];
	kifInformationString[KIFGLOBALTHREAD[293]] = KIFGLOBALTHREAD[552];
	kifInformationString[KIFGLOBALTHREAD[283]] = KIFGLOBALTHREAD[553];
	kifInformationString["levenshtein"] = KIFGLOBALTHREAD[554];
	kifInformationString[KIFGLOBALTHREAD[249]] = KIFGLOBALTHREAD[555];
	kifInformationString["html"] = KIFGLOBALTHREAD[556];
	kifInformationString[KIFGLOBALTHREAD[281]] = KIFGLOBALTHREAD[557];
	kifInformationString[KIFGLOBALTHREAD[282]] = KIFGLOBALTHREAD[558];
	kifInformationString["regexip"] = KIFGLOBALTHREAD[559];
	kifInformationString[KIFGLOBALTHREAD[284]] = KIFGLOBALTHREAD[560];
	kifInformationString[KIFGLOBALTHREAD[285]] = KIFGLOBALTHREAD[561];
	kifInformationString[KIFGLOBALTHREAD[286]] = KIFGLOBALTHREAD[562];
	kifInformationString[KIFGLOBALTHREAD[563]] = KIFGLOBALTHREAD[564];
	kifInformationString[KIFGLOBALTHREAD[565]] = KIFGLOBALTHREAD[566];
	kifInformationString["regex"] = KIFGLOBALTHREAD[567];
	kifInformationString[KIFGLOBALTHREAD[274]] = KIFGLOBALTHREAD[568];
	kifInformationString[KIFGLOBALTHREAD[277]] = KIFGLOBALTHREAD[569];
	kifInformationString[KIFGLOBALTHREAD[278]] = KIFGLOBALTHREAD[570];
	kifInformationInt[KIFGLOBALTHREAD[425]] = KIFGLOBALTHREAD[571];
	kifInformationInt[KIFGLOBALTHREAD[426]] = KIFGLOBALTHREAD[572];
	kifInformationInt[KIFGLOBALTHREAD[271]] = KIFGLOBALTHREAD[573];
	kifInformationInt[KIFGLOBALTHREAD[82]] = KIFGLOBALTHREAD[574];
	kifInformationFloat[KIFGLOBALTHREAD[82]] = KIFGLOBALTHREAD[574];
	kifInformationFloat[KIFGLOBALTHREAD[425]] = KIFGLOBALTHREAD[571];
	kifInformationFloat[KIFGLOBALTHREAD[426]] = KIFGLOBALTHREAD[572];
	kifInformationFloat[KIFGLOBALTHREAD[271]] = KIFGLOBALTHREAD[573];
	kifInformationInt[KIFGLOBALTHREAD[358]] = KIFGLOBALTHREAD[575];
	kifInformationFloat[KIFGLOBALTHREAD[358]] = KIFGLOBALTHREAD[575];
	kifInformationInt[KIFGLOBALTHREAD[424]] = KIFGLOBALTHREAD[576];
	kifInformationFloat[KIFGLOBALTHREAD[424]] = KIFGLOBALTHREAD[576];
	kifInformationInt["d"] = KIFGLOBALTHREAD[577];
	kifInformationInt[KIFGLOBALTHREAD[231]] = KIFGLOBALTHREAD[578];
	kifInformationInt[KIFGLOBALTHREAD[21]] = KIFGLOBALTHREAD[579];
	kifInformationInt[KIFGLOBALTHREAD[431]] = KIFGLOBALTHREAD[580];
	kifInformationFloat[KIFGLOBALTHREAD[431]] = KIFGLOBALTHREAD[580];
	kifInformationInt["n"] = KIFGLOBALTHREAD[581];
	kifInformationInt[KIFGLOBALTHREAD[432]] = KIFGLOBALTHREAD[582];
	kifInformationInt[KIFGLOBALTHREAD[422]] = KIFGLOBALTHREAD[583];
	kifInformationFloat[KIFGLOBALTHREAD[422]] = KIFGLOBALTHREAD[583];
	kifInformationInt["#"] = KIFGLOBALTHREAD[584];
	kifInformationFloat["#"] = KIFGLOBALTHREAD[584];
	kifInformationInt[KIFGLOBALTHREAD[270]] = KIFGLOBALTHREAD[585];
	kifInformationFloat[KIFGLOBALTHREAD[270]] = KIFGLOBALTHREAD[585];
	kifInformationInt[KIFGLOBALTHREAD[427]] = KIFGLOBALTHREAD[586];
	kifInformationInt[KIFGLOBALTHREAD[428]] = KIFGLOBALTHREAD[587];
	kifInformationInt[KIFGLOBALTHREAD[429]] = KIFGLOBALTHREAD[588];
	kifInformationInt[KIFGLOBALTHREAD[430]] = KIFGLOBALTHREAD[589];
	kifInformationFloat[KIFGLOBALTHREAD[427]] = KIFGLOBALTHREAD[586];
	kifInformationFloat[KIFGLOBALTHREAD[430]] = KIFGLOBALTHREAD[589];
	kifInformationFloat[KIFGLOBALTHREAD[428]] = KIFGLOBALTHREAD[587];
	kifInformationFloat[KIFGLOBALTHREAD[429]] = KIFGLOBALTHREAD[588];
	kifInformationString[KIFGLOBALTHREAD[26]] = KIFGLOBALTHREAD[590];
	kifInformationString[KIFGLOBALTHREAD[269]] = KIFGLOBALTHREAD[591];
	kifInformationInt[KIFGLOBALTHREAD[26]] = KIFGLOBALTHREAD[592];
	kifInformationFloat[KIFGLOBALTHREAD[26]] = KIFGLOBALTHREAD[592];
	//Mathematical Functions
	kifInformationInt[KIFGLOBALTHREAD[390]] = KIFGLOBALTHREAD[593];
	kifInformationFloat[KIFGLOBALTHREAD[390]] = KIFGLOBALTHREAD[593];
	kifInformationInt[KIFGLOBALTHREAD[391]] = KIFGLOBALTHREAD[594];
	kifInformationFloat[KIFGLOBALTHREAD[391]] = KIFGLOBALTHREAD[594];
	kifInformationInt[KIFGLOBALTHREAD[392]] = KIFGLOBALTHREAD[595];
	kifInformationFloat[KIFGLOBALTHREAD[392]] = KIFGLOBALTHREAD[595];
	kifInformationInt[KIFGLOBALTHREAD[393]] = KIFGLOBALTHREAD[596];
	kifInformationFloat[KIFGLOBALTHREAD[393]] = KIFGLOBALTHREAD[596];
	kifInformationInt[KIFGLOBALTHREAD[394]] = KIFGLOBALTHREAD[597];
	kifInformationFloat[KIFGLOBALTHREAD[394]] = KIFGLOBALTHREAD[597];
	kifInformationInt[KIFGLOBALTHREAD[395]] = KIFGLOBALTHREAD[598];
	kifInformationFloat[KIFGLOBALTHREAD[395]] = KIFGLOBALTHREAD[598];
	kifInformationInt[KIFGLOBALTHREAD[396]] = KIFGLOBALTHREAD[599];
	kifInformationFloat[KIFGLOBALTHREAD[396]] = KIFGLOBALTHREAD[599];
	kifInformationInt[KIFGLOBALTHREAD[397]] = KIFGLOBALTHREAD[600];
	kifInformationFloat[KIFGLOBALTHREAD[397]] = KIFGLOBALTHREAD[600];
	kifInformationInt[KIFGLOBALTHREAD[398]] = KIFGLOBALTHREAD[601];
	kifInformationFloat[KIFGLOBALTHREAD[398]] = KIFGLOBALTHREAD[601];
	kifInformationInt[KIFGLOBALTHREAD[399]] = KIFGLOBALTHREAD[602];
	kifInformationFloat[KIFGLOBALTHREAD[399]] = KIFGLOBALTHREAD[602];
	kifInformationInt[KIFGLOBALTHREAD[400]] = KIFGLOBALTHREAD[603];
	kifInformationFloat[KIFGLOBALTHREAD[400]] = KIFGLOBALTHREAD[603];
	kifInformationInt[KIFGLOBALTHREAD[401]] = KIFGLOBALTHREAD[604];
	kifInformationFloat[KIFGLOBALTHREAD[401]] = KIFGLOBALTHREAD[604];
	kifInformationInt[KIFGLOBALTHREAD[402]] = KIFGLOBALTHREAD[605];
	kifInformationFloat[KIFGLOBALTHREAD[402]] = KIFGLOBALTHREAD[605];
	kifInformationInt[KIFGLOBALTHREAD[403]] = KIFGLOBALTHREAD[606];
	kifInformationFloat[KIFGLOBALTHREAD[403]] = KIFGLOBALTHREAD[606];
	kifInformationInt[KIFGLOBALTHREAD[404]] = KIFGLOBALTHREAD[607];
	kifInformationFloat[KIFGLOBALTHREAD[404]] = KIFGLOBALTHREAD[607];
	kifInformationInt[KIFGLOBALTHREAD[405]] = KIFGLOBALTHREAD[608];
	kifInformationFloat[KIFGLOBALTHREAD[405]] = KIFGLOBALTHREAD[608];
	kifInformationInt[KIFGLOBALTHREAD[406]] = KIFGLOBALTHREAD[609];
	kifInformationFloat[KIFGLOBALTHREAD[406]] = KIFGLOBALTHREAD[609];
	kifInformationInt[KIFGLOBALTHREAD[408]] = KIFGLOBALTHREAD[610];
	kifInformationFloat[KIFGLOBALTHREAD[408]] = KIFGLOBALTHREAD[610];
	kifInformationInt[KIFGLOBALTHREAD[407]] = KIFGLOBALTHREAD[611];
	kifInformationFloat[KIFGLOBALTHREAD[407]] = KIFGLOBALTHREAD[611];
	kifInformationInt[KIFGLOBALTHREAD[409]] = KIFGLOBALTHREAD[612];
	kifInformationFloat[KIFGLOBALTHREAD[409]] = KIFGLOBALTHREAD[612];
	kifInformationInt[KIFGLOBALTHREAD[410]] = KIFGLOBALTHREAD[613];
	kifInformationFloat[KIFGLOBALTHREAD[410]] = KIFGLOBALTHREAD[613];
	kifInformationInt[KIFGLOBALTHREAD[411]] = KIFGLOBALTHREAD[614];
	kifInformationFloat[KIFGLOBALTHREAD[411]] = KIFGLOBALTHREAD[614];
	kifInformationInt[KIFGLOBALTHREAD[412]] = KIFGLOBALTHREAD[615];
	kifInformationFloat[KIFGLOBALTHREAD[412]] = KIFGLOBALTHREAD[615];
	kifInformationInt[KIFGLOBALTHREAD[413]] = KIFGLOBALTHREAD[616];
	kifInformationFloat[KIFGLOBALTHREAD[413]] = KIFGLOBALTHREAD[616];
	kifInformationInt[KIFGLOBALTHREAD[414]] = KIFGLOBALTHREAD[617];
	kifInformationFloat[KIFGLOBALTHREAD[414]] = KIFGLOBALTHREAD[617];
	kifInformationInt[KIFGLOBALTHREAD[415]] = KIFGLOBALTHREAD[618];
	kifInformationFloat[KIFGLOBALTHREAD[415]] = KIFGLOBALTHREAD[618];
	kifInformationInt[KIFGLOBALTHREAD[416]] = KIFGLOBALTHREAD[619];
	kifInformationFloat[KIFGLOBALTHREAD[416]] = KIFGLOBALTHREAD[619];
	kifInformationInt[KIFGLOBALTHREAD[417]] = KIFGLOBALTHREAD[620];
	kifInformationFloat[KIFGLOBALTHREAD[417]] = KIFGLOBALTHREAD[620];
	kifInformationInt[KIFGLOBALTHREAD[418]] = KIFGLOBALTHREAD[621];
	kifInformationFloat[KIFGLOBALTHREAD[418]] = KIFGLOBALTHREAD[621];
	kifInformationInt[KIFGLOBALTHREAD[419]] = KIFGLOBALTHREAD[622];
	kifInformationFloat[KIFGLOBALTHREAD[419]] = KIFGLOBALTHREAD[622];
	kifInformationInt[KIFGLOBALTHREAD[420]] = KIFGLOBALTHREAD[623];
	kifInformationFloat[KIFGLOBALTHREAD[420]] = KIFGLOBALTHREAD[623];
	kifInformationInt[KIFGLOBALTHREAD[421]] = KIFGLOBALTHREAD[624];
	kifInformationFloat[KIFGLOBALTHREAD[421]] = KIFGLOBALTHREAD[624];

	kifInformationVector[KIFGLOBALTHREAD[279]] = KIFGLOBALTHREAD[625];
	kifInformationVector[KIFGLOBALTHREAD[297]] = KIFGLOBALTHREAD[626];
	kifInformationVector[KIFGLOBALTHREAD[326]] = KIFGLOBALTHREAD[627];
	kifInformationVector[KIFGLOBALTHREAD[275]] = KIFGLOBALTHREAD[628];
	kifInformationVector[KIFGLOBALTHREAD[249]] = KIFGLOBALTHREAD[629];
	kifInformationVector[KIFGLOBALTHREAD[329]] = KIFGLOBALTHREAD[630];
	kifInformationVector[KIFGLOBALTHREAD[327]] = KIFGLOBALTHREAD[631];
	kifInformationVector[KIFGLOBALTHREAD[269]] = KIFGLOBALTHREAD[632];
	kifInformationVector[KIFGLOBALTHREAD[328]] = KIFGLOBALTHREAD[633];
	kifInformationVector[KIFGLOBALTHREAD[330]] = KIFGLOBALTHREAD[634];
	kifInformationVector[KIFGLOBALTHREAD[38]] = KIFGLOBALTHREAD[635];
	kifInformationVector[KIFGLOBALTHREAD[38]] = KIFGLOBALTHREAD[636];
	kifInformationVector[KIFGLOBALTHREAD[320]] = KIFGLOBALTHREAD[637];
	kifInformationVector[KIFGLOBALTHREAD[267]] = KIFGLOBALTHREAD[638];
	kifInformationVector[KIFGLOBALTHREAD[331]] = KIFGLOBALTHREAD[639];
	kifInformationVector[KIFGLOBALTHREAD[332]] = KIFGLOBALTHREAD[640];
	kifInformationVector[KIFGLOBALTHREAD[321]] = KIFGLOBALTHREAD[641];
	kifInformationVector[KIFGLOBALTHREAD[341]] = KIFGLOBALTHREAD[642];
	kifInformationVector[KIFGLOBALTHREAD[342]] = KIFGLOBALTHREAD[643];
	kifInformationVector[KIFGLOBALTHREAD[69]] = KIFGLOBALTHREAD[644];
	kifInformationVector[KIFGLOBALTHREAD[239]] = KIFGLOBALTHREAD[645];
	kifInformationVector[KIFGLOBALTHREAD[237]] = KIFGLOBALTHREAD[646];
	kifInformationVector[KIFGLOBALTHREAD[238]] = KIFGLOBALTHREAD[647];
	kifInformationVector[KIFGLOBALTHREAD[333]] = KIFGLOBALTHREAD[648];
	kifInformationVector[KIFGLOBALTHREAD[335]] = KIFGLOBALTHREAD[649];
	kifInformationVector[KIFGLOBALTHREAD[334]] = KIFGLOBALTHREAD[650];
	kifInformationVector[KIFGLOBALTHREAD[336]] = KIFGLOBALTHREAD[651];
	kifInformationVector[KIFGLOBALTHREAD[337]] = KIFGLOBALTHREAD[652];
	kifInformationVector[KIFGLOBALTHREAD[338]] = KIFGLOBALTHREAD[653];
	kifInformationVector[KIFGLOBALTHREAD[340]] = KIFGLOBALTHREAD[654];
	kifInformationVector[KIFGLOBALTHREAD[339]] = KIFGLOBALTHREAD[655];
	kifInformationVector[KIFGLOBALTHREAD[24]] = KIFGLOBALTHREAD[656];
	kifInformationVector[KIFGLOBALTHREAD[62]] = KIFGLOBALTHREAD[657];
	kifInformationVector[KIFGLOBALTHREAD[240]] = KIFGLOBALTHREAD[658];
	kifInformationVector[KIFGLOBALTHREAD[63]] = KIFGLOBALTHREAD[659];

	kifInformationList[KIFGLOBALTHREAD[343]] = KIFGLOBALTHREAD[660];
	kifInformationList[KIFGLOBALTHREAD[249]] = KIFGLOBALTHREAD[629];
	kifInformationList[KIFGLOBALTHREAD[329]] = KIFGLOBALTHREAD[630];
	kifInformationList[KIFGLOBALTHREAD[330]] = KIFGLOBALTHREAD[661];
	kifInformationList[KIFGLOBALTHREAD[334]] = KIFGLOBALTHREAD[650];
	kifInformationList[KIFGLOBALTHREAD[335]] = KIFGLOBALTHREAD[649];
	kifInformationList[KIFGLOBALTHREAD[336]] = KIFGLOBALTHREAD[651];
	kifInformationList[KIFGLOBALTHREAD[327]] = KIFGLOBALTHREAD[631];
	kifInformationList[KIFGLOBALTHREAD[345]] = KIFGLOBALTHREAD[662];
	kifInformationList[KIFGLOBALTHREAD[346]] = KIFGLOBALTHREAD[663];
	kifInformationList[KIFGLOBALTHREAD[237]] = KIFGLOBALTHREAD[646];
	kifInformationList[KIFGLOBALTHREAD[238]] = KIFGLOBALTHREAD[647];
	kifInformationList[KIFGLOBALTHREAD[38]] = KIFGLOBALTHREAD[635];
	kifInformationList[KIFGLOBALTHREAD[38]] = KIFGLOBALTHREAD[636];
	kifInformationList[KIFGLOBALTHREAD[344]] = KIFGLOBALTHREAD[664];
	kifInformationList[KIFGLOBALTHREAD[320]] = KIFGLOBALTHREAD[665];
	kifInformationList[KIFGLOBALTHREAD[347]] = KIFGLOBALTHREAD[666];
	kifInformationList[KIFGLOBALTHREAD[267]] = KIFGLOBALTHREAD[667];
	kifInformationList[KIFGLOBALTHREAD[331]] = KIFGLOBALTHREAD[668];
	kifInformationList[KIFGLOBALTHREAD[321]] = KIFGLOBALTHREAD[641];
	kifInformationList[KIFGLOBALTHREAD[341]] = KIFGLOBALTHREAD[642];
	kifInformationList[KIFGLOBALTHREAD[342]] = KIFGLOBALTHREAD[643];
	kifInformationList[KIFGLOBALTHREAD[24]] = KIFGLOBALTHREAD[669];
	kifInformationList[KIFGLOBALTHREAD[239]] = KIFGLOBALTHREAD[670];

	kifInformationMap[KIFGLOBALTHREAD[358]] = KIFGLOBALTHREAD[671];
	kifInformationMap[KIFGLOBALTHREAD[341]] = KIFGLOBALTHREAD[642];
	kifInformationMap[KIFGLOBALTHREAD[331]] = KIFGLOBALTHREAD[672];
	kifInformationMap[KIFGLOBALTHREAD[342]] = KIFGLOBALTHREAD[643];
	kifInformationMap[KIFGLOBALTHREAD[24]] = KIFGLOBALTHREAD[669];
	kifInformationMap[KIFGLOBALTHREAD[327]] = KIFGLOBALTHREAD[631];
	kifInformationMap[KIFGLOBALTHREAD[69]] = KIFGLOBALTHREAD[673];
	kifInformationMap[KIFGLOBALTHREAD[330]] = KIFGLOBALTHREAD[674];
	kifInformationMap[KIFGLOBALTHREAD[249]] = KIFGLOBALTHREAD[629];
	kifInformationMap[KIFGLOBALTHREAD[38]] = KIFGLOBALTHREAD[675];
	kifInformationMap[KIFGLOBALTHREAD[359]] = KIFGLOBALTHREAD[676];
	kifInformationMap[KIFGLOBALTHREAD[360]] = KIFGLOBALTHREAD[677];
	kifInformationMap[KIFGLOBALTHREAD[361]] = KIFGLOBALTHREAD[678];
	kifInformationMap[KIFGLOBALTHREAD[237]] = KIFGLOBALTHREAD[646];
	kifInformationMap[KIFGLOBALTHREAD[238]] = KIFGLOBALTHREAD[647];
	kifInformationMap[KIFGLOBALTHREAD[239]] = KIFGLOBALTHREAD[679];
	kifInformationMap[KIFGLOBALTHREAD[362]] = KIFGLOBALTHREAD[680];
	kifInformationMap[KIFGLOBALTHREAD[363]] = KIFGLOBALTHREAD[681];

	kifInformationTree[KIFGLOBALTHREAD[223]] = KIFGLOBALTHREAD[682];
	kifInformationTree[KIFGLOBALTHREAD[249]] = KIFGLOBALTHREAD[629];
	kifInformationTree[KIFGLOBALTHREAD[257]] = KIFGLOBALTHREAD[683];
	kifInformationTree[KIFGLOBALTHREAD[258]] = KIFGLOBALTHREAD[684];
	kifInformationTree[KIFGLOBALTHREAD[260]] = KIFGLOBALTHREAD[685];
	kifInformationTree[KIFGLOBALTHREAD[259]] = KIFGLOBALTHREAD[686];
	kifInformationTree[KIFGLOBALTHREAD[261]] = KIFGLOBALTHREAD[687];
	kifInformationTree[KIFGLOBALTHREAD[264]] = KIFGLOBALTHREAD[688];
	kifInformationTree[KIFGLOBALTHREAD[263]] = KIFGLOBALTHREAD[689];
	kifInformationTree[KIFGLOBALTHREAD[262]] = KIFGLOBALTHREAD[690];
	kifInformationIterator[KIFGLOBALTHREAD[440]] = KIFGLOBALTHREAD[691];
	kifInformationIterator[KIFGLOBALTHREAD[439]] = KIFGLOBALTHREAD[692];
	kifInformationIterator[KIFGLOBALTHREAD[441]] = KIFGLOBALTHREAD[693];
	kifInformationIterator[KIFGLOBALTHREAD[487]] = KIFGLOBALTHREAD[694];
	kifInformationIterator[KIFGLOBALTHREAD[438]] = KIFGLOBALTHREAD[695];
	kifInformationIterator[KIFGLOBALTHREAD[434]] = KIFGLOBALTHREAD[696];
	kifInformationIterator[KIFGLOBALTHREAD[436]] = KIFGLOBALTHREAD[697];
	kifInformationIterator[KIFGLOBALTHREAD[435]] = KIFGLOBALTHREAD[698];
	kifInformationIterator[KIFGLOBALTHREAD[437]] = KIFGLOBALTHREAD[699];
	kifInformationIterator[KIFGLOBALTHREAD[24]] = KIFGLOBALTHREAD[700];
	kifInformationDate[KIFGLOBALTHREAD[380]] = KIFGLOBALTHREAD[701];
	kifInformationDate[KIFGLOBALTHREAD[84]] = KIFGLOBALTHREAD[702];
	kifInformationDate[KIFGLOBALTHREAD[381]] = KIFGLOBALTHREAD[703];
	kifInformationDate[KIFGLOBALTHREAD[382]] = KIFGLOBALTHREAD[704];
	kifInformationDate[KIFGLOBALTHREAD[383]] = KIFGLOBALTHREAD[705];
	kifInformationDate[KIFGLOBALTHREAD[384]] = KIFGLOBALTHREAD[706];
	kifInformationDate[KIFGLOBALTHREAD[385]] = KIFGLOBALTHREAD[707];
	kifInformationDate[KIFGLOBALTHREAD[386]] = KIFGLOBALTHREAD[708];
	kifInformationDate[KIFGLOBALTHREAD[387]] = KIFGLOBALTHREAD[709];
	kifInformationDate[KIFGLOBALTHREAD[388]] = KIFGLOBALTHREAD[710];
	kifInformationTime[KIFGLOBALTHREAD[389]] = KIFGLOBALTHREAD[711];
	kifInformationFile[KIFGLOBALTHREAD[365]] = KIFGLOBALTHREAD[712];
	kifInformationFile[KIFGLOBALTHREAD[364]] = KIFGLOBALTHREAD[713];
	kifInformationFile[KIFGLOBALTHREAD[371]] = KIFGLOBALTHREAD[714];
	kifInformationFile[KIFGLOBALTHREAD[292]] = KIFGLOBALTHREAD[715];
	kifInformationFile[KIFGLOBALTHREAD[366]] = KIFGLOBALTHREAD[716];
	kifInformationFile[KIFGLOBALTHREAD[367]] = KIFGLOBALTHREAD[717];
	kifInformationFile[KIFGLOBALTHREAD[368]] = KIFGLOBALTHREAD[718];
	kifInformationFile[KIFGLOBALTHREAD[374]] = KIFGLOBALTHREAD[719];
	kifInformationFile[KIFGLOBALTHREAD[375]] = KIFGLOBALTHREAD[720];
	kifInformationFile[KIFGLOBALTHREAD[376]] = KIFGLOBALTHREAD[721];
	kifInformationFile[KIFGLOBALTHREAD[26]] = KIFGLOBALTHREAD[722];
	kifInformationFile[KIFGLOBALTHREAD[370]] = KIFGLOBALTHREAD[723];
	kifInformationFile[KIFGLOBALTHREAD[370]] = KIFGLOBALTHREAD[724];
	kifInformationFile[KIFGLOBALTHREAD[377]] = KIFGLOBALTHREAD[725];
	kifInformationFile[KIFGLOBALTHREAD[378]] = KIFGLOBALTHREAD[726];
	kifInformationFile[KIFGLOBALTHREAD[372]] = KIFGLOBALTHREAD[727];
	kifInformationFile[KIFGLOBALTHREAD[373]] = KIFGLOBALTHREAD[728];
	kifInformationFile[KIFGLOBALTHREAD[379]] = KIFGLOBALTHREAD[729];

	kifInformationGrammar[KIFGLOBALTHREAD[241]] = KIFGLOBALTHREAD[730];
	kifInformationGrammar[KIFGLOBALTHREAD[24]] = KIFGLOBALTHREAD[731];

	kifInformationAutomaton[KIFGLOBALTHREAD[241]] = KIFGLOBALTHREAD[732];
	kifInformationAutomaton[KIFGLOBALTHREAD[246]] = KIFGLOBALTHREAD[733];
	kifInformationAutomaton[KIFGLOBALTHREAD[242]] = KIFGLOBALTHREAD[734];
	kifInformationAutomaton[KIFGLOBALTHREAD[243]] = KIFGLOBALTHREAD[735];
	kifInformationAutomaton[KIFGLOBALTHREAD[244]] = KIFGLOBALTHREAD[736];
	kifInformationAutomaton[KIFGLOBALTHREAD[245]] = KIFGLOBALTHREAD[737];
	kifInformationAutomaton[KIFGLOBALTHREAD[251]] = KIFGLOBALTHREAD[738];
	kifInformationAutomaton[KIFGLOBALTHREAD[247]] = KIFGLOBALTHREAD[739];
	kifInformationAutomaton[KIFGLOBALTHREAD[20]] = KIFGLOBALTHREAD[740];
	kifInformationAutomaton["dump"] = KIFGLOBALTHREAD[741];
	kifInformationAutomaton[KIFGLOBALTHREAD[248]] = KIFGLOBALTHREAD[742];
	kifInformationAutomaton[KIFGLOBALTHREAD[249]] = KIFGLOBALTHREAD[743];
	kifInformationAutomaton[KIFGLOBALTHREAD[239]] = KIFGLOBALTHREAD[744];
	kifInformationAutomaton[KIFGLOBALTHREAD[250]] = KIFGLOBALTHREAD[745];

	kifInformationKif[KIFGLOBALTHREAD[241]] = KIFGLOBALTHREAD[746];
	kifInformationKif[KIFGLOBALTHREAD[354]] = KIFGLOBALTHREAD[747];
	kifInformationKif[KIFGLOBALTHREAD[448]] = KIFGLOBALTHREAD[748];
	kifInformationKif[KIFGLOBALTHREAD[350]] = KIFGLOBALTHREAD[749];
	kifInformationKif[KIFGLOBALTHREAD[349]] = KIFGLOBALTHREAD[750];
	kifInformationKif[KIFGLOBALTHREAD[351]] = KIFGLOBALTHREAD[751];
	kifInformationKif[KIFGLOBALTHREAD[352]] = KIFGLOBALTHREAD[752];
	kifInformationKif[KIFGLOBALTHREAD[353]] = KIFGLOBALTHREAD[753];
	kifInformationKif[KIFGLOBALTHREAD[355]] = KIFGLOBALTHREAD[754];
	kifInformationKif[KIFGLOBALTHREAD[356]] = KIFGLOBALTHREAD[755];
	kifInformationKif[KIFGLOBALTHREAD[236]] = KIFGLOBALTHREAD[756];
	kifInformationKif[KIFGLOBALTHREAD[357]] = KIFGLOBALTHREAD[757];
	kifInformationKif[KIFGLOBALTHREAD[37]] = KIFGLOBALTHREAD[758];

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



