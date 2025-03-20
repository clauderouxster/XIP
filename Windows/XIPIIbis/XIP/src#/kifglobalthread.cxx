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


//--------------------------------------------------------------------------------------------------------------------------
extern const char* kifErrorStrings[];
//--------------------------------------------------------------------------------------------------------------------------
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
	case kifVectorString:
		((KifVectorString*)ke)->values.reserve(ksz->Integer());
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
	hmap<short, KifElement*>::iterator it;
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
	hmap<short, LockedThread*>::iterator it;
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

	if (kconstants->declarations.find(n) != kconstants->declarations.end())
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

	hmap<short, KifElement*>::iterator it;
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
		kifStringType[(KifType)kifLastType] = name;
		kifTypeString[name] = (KifType)kifLastType;
		kifExternalTypes[name] = func;
		return (KifType)kifLastType;
	}
	if (error)
		return kifVOID;

	return kifTypeString[name];
}

void KifGlobalThread::KifAssociateType(string name, KifExternalParse func, KifType kt) {
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

KifGlobalThread::KifGlobalThread(void* d) {
#ifndef C11
#ifdef WIN32
	for (int i = 0; i<locktablesize; i++)
		tableofcommonlocks[i] = 0;
#endif
#endif
	mxthreads = 0;
	resizethreads(1000);
	KifPredicate::nb = 0;
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

	kifStringType[kifPredicateVariableInstance] = "predicateinstance";
	kifStringType[kifLoader] = "_loader";
	kifStringType[kifVariable] = "variable";
	kifStringType[kifDomain] = "domain";
	kifStringType[kifString] = "string";
	kifStringType[kifTime] = "time";
	kifStringType[kifAutomaton] = "automaton";
	kifStringType[kifVOID] = "available";
	kifStringType[kifInteger] = "int";
	kifStringType[kifByte] = "byte";
	kifStringType[kifLong] = "long";
	kifStringType[kifFraction] = "fraction";
	kifStringType[kifBit] = "bit";
	kifStringType[kifBits] = "bits";
	kifStringType[kifDate] = "date";
	kifStringType[kifCall] = "call";

	kifStringType[kifFloat] = "float";
	kifStringType[kifBoolean] = "boolean";
	kifStringType[kifSelf] = "self";

	kifStringType[kifMap] = "map";
	kifStringType[kifIntegerMap] = "mapi";
	kifStringType[kifFloatMap] = "mapf";
	kifStringType[kifMapIntegerString] = "ismap";
	kifStringType[kifMapIntegerFloat] = "ifmap";
	kifStringType[kifMapIntegerInteger] = "iimap";
	kifStringType[kifMapFloatString] = "fsmap";
	kifStringType[kifMapFloatFloat] = "ffmap";
	kifStringType[kifMapFloatInteger] = "fimap";
	kifStringType[kifMapString] = "smap";
	kifStringType[kifMapFloat] = "fmap";
	kifStringType[kifMapInteger] = "imap";

	kifStringType[kifTreeMap] = "treemap";
	kifStringType[kifIntegerTreeMap] = "treemapi";
	kifStringType[kifFloatTreeMap] = "treemapf";
	kifStringType[kifTreeMapIntegerString] = "istreemap";
	kifStringType[kifTreeMapIntegerFloat] = "iftreemap";
	kifStringType[kifTreeMapIntegerInteger] = "iitreemap";
	kifStringType[kifTreeMapFloatString] = "fstreemap";
	kifStringType[kifTreeMapFloatFloat] = "fftreemap";
	kifStringType[kifTreeMapFloatInteger] = "fitreemap";
	kifStringType[kifTreeMapString] = "streemap";
	kifStringType[kifTreeMapFloat] = "ftreemap";
	kifStringType[kifTreeMapInteger] = "itreemap";

	kifStringType[kifPrimeMap] = "primemap";
	kifStringType[kifIntegerPrimeMap] = "primemapi";
	kifStringType[kifFloatPrimeMap] = "primemapf";
	kifStringType[kifPrimeMapIntegerString] = "isprimemap";
	kifStringType[kifPrimeMapIntegerFloat] = "ifprimemap";
	kifStringType[kifPrimeMapIntegerInteger] = "iiprimemap";
	kifStringType[kifPrimeMapFloatString] = "fsprimemap";
	kifStringType[kifPrimeMapFloatFloat] = "ffprimemap";
	kifStringType[kifPrimeMapFloatInteger] = "fiprimemap";
	kifStringType[kifPrimeMapString] = "sprimemap";
	kifStringType[kifPrimeMapFloat] = "fprimemap";
	kifStringType[kifPrimeMapInteger] = "iprimemap";

	kifStringType[kifSet] = "set";
	kifStringType[kifSetString] = "sset";
	kifStringType[kifSetFloat] = "fset";
	kifStringType[kifSetInteger] = "iset";

	kifStringType[kifVectorString] = "svector";
	kifStringType[kifVectorFloat] = "fvector";
	kifStringType[kifVectorInteger] = "ivector";
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

	kifTypeString["available"] = kifVOID;
	kifTypeString["predicateinstance"] = kifPredicateVariableInstance;
	kifTypeString["domain"] = kifDomain;
	kifTypeString["string"] = kifString;
	kifTypeString["automaton"] = kifAutomaton;
	kifTypeString["_loader"] = kifLoader;
	kifTypeString["variable"] = kifVariable;
	kifTypeString["int"] = kifInteger;
	kifTypeString["long"] = kifLong;
	kifTypeString["fraction"] = kifFraction;
	kifTypeString["bit"] = kifBit;
	kifTypeString["bits"] = kifBits;
	kifTypeString["byte"] = kifByte;
	kifTypeString["date"] = kifDate;
	kifTypeString["call"] = kifCall;
	kifTypeString["float"] = kifFloat;
	kifTypeString["boolean"] = kifBoolean;
	kifTypeString["self"] = kifSelf;
	kifTypeString["let"] = kifSelf;
	kifTypeString["time"] = kifTime;

	kifTypeString["map"] = kifMap;
	kifTypeString["maps"] = kifMap;
	kifTypeString["mapf"] = kifFloatMap;
	kifTypeString["mapi"] = kifIntegerMap;
	kifTypeString["ismap"] = kifMapIntegerString;
	kifTypeString["iimap"] = kifMapIntegerInteger;
	kifTypeString["ifmap"] = kifMapIntegerFloat;
	kifTypeString["fsmap"] = kifMapFloatString;
	kifTypeString["fimap"] = kifMapFloatInteger;
	kifTypeString["ffmap"] = kifMapFloatFloat;

	kifTypeString["smap"] = kifMapString;
	kifTypeString["imap"] = kifMapInteger;
	kifTypeString["fmap"] = kifMapFloat;
	kifTypeString["ssmap"] = kifMapString;
	kifTypeString["simap"] = kifMapInteger;
	kifTypeString["sfmap"] = kifMapFloat;

	kifTypeString["treemap"] = kifTreeMap;
	kifTypeString["treemaps"] = kifTreeMap;
	kifTypeString["treemapf"] = kifFloatTreeMap;
	kifTypeString["treemapi"] = kifIntegerTreeMap;

	kifTypeString["istreemap"] = kifTreeMapIntegerString;
	kifTypeString["iitreemap"] = kifTreeMapIntegerInteger;
	kifTypeString["iftreemap"] = kifTreeMapIntegerFloat;
	kifTypeString["fstreemap"] = kifTreeMapFloatString;
	kifTypeString["fitreemap"] = kifTreeMapFloatInteger;
	kifTypeString["fftreemap"] = kifTreeMapFloatFloat;

	kifTypeString["streemap"] = kifTreeMapString;
	kifTypeString["itreemap"] = kifTreeMapInteger;
	kifTypeString["ftreemap"] = kifTreeMapFloat;
	kifTypeString["sstreemap"] = kifTreeMapString;
	kifTypeString["sitreemap"] = kifTreeMapInteger;
	kifTypeString["sftreemap"] = kifTreeMapFloat;

	kifTypeString["primemap"] = kifPrimeMap;
	kifTypeString["primemaps"] = kifPrimeMap;
	kifTypeString["primemapf"] = kifFloatPrimeMap;
	kifTypeString["primemapi"] = kifIntegerPrimeMap;

	kifTypeString["isprimemap"] = kifPrimeMapIntegerString;
	kifTypeString["iiprimemap"] = kifPrimeMapIntegerInteger;
	kifTypeString["ifprimemap"] = kifPrimeMapIntegerFloat;
	kifTypeString["fsprimemap"] = kifPrimeMapFloatString;
	kifTypeString["fiprimemap"] = kifPrimeMapFloatInteger;
	kifTypeString["ffprimemap"] = kifPrimeMapFloatFloat;

	kifTypeString["sprimemap"] = kifPrimeMapString;
	kifTypeString["iprimemap"] = kifPrimeMapInteger;
	kifTypeString["fprimemap"] = kifPrimeMapFloat;
	kifTypeString["ssprimemap"] = kifPrimeMapString;
	kifTypeString["siprimemap"] = kifPrimeMapInteger;
	kifTypeString["sfprimemap"] = kifPrimeMapFloat;

	kifTypeString["sset"]=kifSetString;
	kifTypeString["fset"]=kifSetFloat;
	kifTypeString["iset"] = kifSetInteger;
	kifTypeString["set"] = kifSet;

	kifTypeString["vector"] = kifVector;
	kifTypeString["table"] = kifTable;
	kifTypeString["predicate"] = kifPredicate;
	kifTypeString["term"] = kifPredicateTerm;
	kifTypeString["svector"] = kifVectorString;
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
	kifTypes["let"] = &KifCode::KifParseVarSelf;
	kifTypes["int"] = &KifCode::KifParseVarInt;
	kifTypes["byte"] = &KifCode::KifParseVarByte;
	kifTypes["long"] = &KifCode::KifParseVarLong;
	kifTypes["fraction"] = &KifCode::KifParseVarFraction;
	kifTypes["bit"] = &KifCode::KifParseVarBit;
	kifTypes["bits"] = &KifCode::KifParseVarBits;
	kifTypes["date"] = &KifCode::KifParseVarDate;
	kifTypes["time"] = &KifCode::KifParseVarTime;
	kifTypes["call"] = &KifCode::KifParseVarCall;
	kifTypes["string"] = &KifCode::KifParseVarString;
	kifTypes["automaton"] = &KifCode::KifParseVarAutomaton;
	kifTypes["predicate"] = &KifCode::KifParseVarPredicate;
	kifTypes["predicatevar"] = &KifCode::KifParseVarPredicate;
	kifTypes["term"] = &KifCode::KifParseVarPredicateTerm;
	kifTypes["float"] = &KifCode::KifParseVarFloat;
	kifTypes["vector"] = &KifCode::KifParseVarVector;
	kifTypes["table"] = &KifCode::KifParseVarTable;
	kifTypes["enum"] = &KifCode::KifParseVarEnum;
	kifTypes["list"] = &KifCode::KifParseVarList;


	kifTypes["set"] = &KifCode::KifParseVarSet;
	kifTypes["sset"] = &KifCode::KifParseVarSetString;
	kifTypes["iset"] = &KifCode::KifParseVarSetInteger;
	kifTypes["fset"] = &KifCode::KifParseVarSetFloat;

	kifTypes["map"] = &KifCode::KifParseVarMap;
	kifTypes["maps"] = &KifCode::KifParseVarMap;
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

	kifTypes["treemap"] = &KifCode::KifParseVarTreeMap;
	kifTypes["treemaps"] = &KifCode::KifParseVarTreeMap;
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

	kifTypes["primemap"] = &KifCode::KifParseVarPrimeMap;
	kifTypes["primemaps"] = &KifCode::KifParseVarPrimeMap;
	kifTypes["primemapi"] = &KifCode::KifParseVarIntegerPrimeMap;
	kifTypes["primemapf"] = &KifCode::KifParseVarFloatPrimeMap;

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

	kifTypes["svector"] = &KifCode::KifParseVarVectorString;
	kifTypes["ivector"] = &KifCode::KifParseVarVectorInteger;
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
	kifFunctions["dicopredicate"] = &KifCode::KifParseDico;

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
	kifFunctions["switch"] = &KifCode::KifParseSwitch;
	kifFunctions["trycatch"] = &KifCode::KifParseTryCatch;
	kifFunctions["testswitch"] = &KifCode::KifParseTestSwitch;
	kifFunctions["testelif"] = &KifCode::KifParseTest;
	kifFunctions["loop"] = &KifCode::KifParseWhile;
	kifFunctions["doloop"] = &KifCode::KifParseUntil;
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

	compatibilities[kifDomain].set(kifDomain);
	compatibilities[kifDomain].set(kifFrame);
	compatibilities[kifFrame].set(kifDomain);
	compatibilities[kifFrame].set(kifFrame);
	compatibilities[kifSubframe].set(kifDomain);
	compatibilities[kifSubframe].set(kifFrame);

	compatibilities[kifDomain].set(kifthis);
	compatibilities[kifFrame].set(kifthis);
	compatibilities[kifSubframe].set(kifthis);

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

	KifType vectortypes[] = { kifVector, kifVectorString, kifVectorInteger, kifVectorFloat, kifVectorPtr, kifBaseVector, kifTable, kifBaseVectorMerge, kifLASTYPE };

	KifType maptypes[] = { kifMap, kifMapString, kifMapInteger, kifMapFloat,
		kifIntegerMap, kifMapIntegerString, kifMapIntegerInteger, kifMapIntegerFloat,
		kifFloatMap, kifMapFloatString, kifMapFloatInteger, kifMapFloatFloat,

		kifTreeMap, kifTreeMapString, kifTreeMapInteger, kifTreeMapFloat,
		kifIntegerTreeMap, kifTreeMapIntegerString, kifTreeMapIntegerInteger, kifTreeMapIntegerFloat,
		kifFloatTreeMap, kifTreeMapFloatString, kifTreeMapFloatInteger, kifTreeMapFloatFloat,

		kifPrimeMap, kifPrimeMapString, kifPrimeMapInteger, kifPrimeMapFloat,
		kifIntegerPrimeMap, kifPrimeMapIntegerString, kifPrimeMapIntegerInteger, kifPrimeMapIntegerFloat,
		kifFloatPrimeMap, kifPrimeMapFloatString, kifPrimeMapFloatInteger, kifPrimeMapFloatFloat,
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
	InitMapBooleanString(kifPredefinedFunctions, "_id", false);

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


	InitMapBooleanString(kifTreeFunctions, "json");
	InitMapBooleanString(kifTreeFunctions, "string");
	InitMapBooleanString(kifTreeFunctions, "methods",false);
	InitMapBooleanString(kifTreeFunctions, "infos",false);
	InitMapBooleanString(kifTreeFunctions, "float");
	InitMapBooleanString(kifTreeFunctions, "boolean");
	InitMapBooleanString(kifTreeFunctions, "int");
	InitMapBooleanString(kifTreeFunctions, "long");
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
	InitMapBooleanString(kifStringFunctions, "float");
	InitMapBooleanString(kifStringFunctions, "boolean");
	InitMapBooleanString(kifStringFunctions, "vector");
	InitMapBooleanString(kifStringFunctions, "map");

	InitMapBooleanString(kifStringFunctions, "hash", false);
	InitMapBooleanString(kifStringFunctions, "size");
	InitMapBooleanString(kifStringFunctions, "get");
	InitMapBooleanString(kifStringFunctions, "reverse");
	InitMapBooleanString(kifStringFunctions, "ord", false);
	InitMapBooleanString(kifStringFunctions, "bytes", false);
	InitMapBooleanString(kifStringFunctions, "format");
	InitMapBooleanString(kifStringFunctions, "fill");
	InitMapBooleanString(kifStringFunctions, "padding");

	InitMapBooleanString(kifStringFunctions, "parse");
	InitMapBooleanString(kifStringFunctions, "lisp");
	InitMapBooleanString(kifStringFunctions, "tags");
	InitMapBooleanString(kifStringFunctions, "regexip");
	InitMapBooleanString(kifStringFunctions, "evaluate");
	InitMapBooleanString(kifStringFunctions, "html");
	InitMapBooleanString(kifStringFunctions, "replaceregexip");
	InitMapBooleanString(kifStringFunctions, "levenshtein");
	InitMapBooleanString(kifStringFunctions, "editdistance");
	InitMapBooleanString(kifStringFunctions, "replace");
	InitMapBooleanString(kifStringFunctions, "getstd");
	InitMapBooleanString(kifStringFunctions, "geterr");
	InitMapBooleanString(kifStringFunctions, "split");
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
	InitMapBooleanString(kifStringFunctions, "isvowel");
	InitMapBooleanString(kifStringFunctions, "ispunctuation");
	InitMapBooleanString(kifStringFunctions, "isdigit");
	InitMapBooleanString(kifStringFunctions, "extract");
	InitMapBooleanString(kifStringFunctions, "isupper");
	InitMapBooleanString(kifStringFunctions, "islower");
	InitMapBooleanString(kifStringFunctions, "upper");
	InitMapBooleanString(kifStringFunctions, "deaccentuate");
	InitMapBooleanString(kifStringFunctions, "lower");
	InitMapBooleanString(kifStringFunctions, "trim");
	InitMapBooleanString(kifStringFunctions, "trimleft");
	InitMapBooleanString(kifStringFunctions, "trimright");
	InitMapBooleanString(kifStringFunctions, "pop");
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
	InitMapBooleanString(kifTimeFunctions, "float");
	InitMapBooleanString(kifTimeFunctions, "boolean");

	//"log","ln","exp","sqrt", "tan","sin","cos","atan","@pi","@e",abs
	//Mathematics Function
	kifMathFunctions[KifNewId("log")] = log10;
	kifMathFunctions[KifNewId("abs")] = fabs;
	kifMathFunctions[KifNewId("ln")] = log;
	kifMathFunctions[KifNewId("exp")] = exp;
	kifMathFunctions[KifNewId("sqrt")] = sqrt;
	kifMathFunctions[KifNewId("tan")] = tan;
	kifMathFunctions[KifNewId("sin")] = sin;
	kifMathFunctions[KifNewId("cos")] = cos;
	kifMathFunctions[KifNewId("atan")] = atan;
	kifMathFunctions[KifNewId("factors")] = fabs;


	InitMapBooleanString(kifNumericalFunctions, "isa",false);
	InitMapBooleanString(kifNumericalFunctions, "type",false);
	InitMapBooleanString(kifNumericalFunctions, "methods",false);
	InitMapBooleanString(kifNumericalFunctions, "infos",false);
	InitMapBooleanString(kifNumericalFunctions, "chr", false);

	InitMapBooleanString(kifNumericalFunctions, "string");
	InitMapBooleanString(kifNumericalFunctions, "json");
	InitMapBooleanString(kifNumericalFunctions, "partof");
	InitMapBooleanString(kifNumericalFunctions, "fraction");
	InitMapBooleanString(kifNumericalFunctions, "format");
	InitMapBooleanString(kifNumericalFunctions, "int");
	InitMapBooleanString(kifNumericalFunctions, "long");
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
	kifReturnString[KifNewId("trim")] = true;
	kifReturnString[KifNewId("trimleft")] = true;
	kifReturnString[KifNewId("trimright")] = true;
	kifReturnString[KifNewId("pop")] = true;
	kifReturnString[KifNewId("last")] = true;
	kifReturnString[KifNewId("infos")] = true;
	kifReturnString[KifNewId("type")] = true;
	kifReturnString[KifNewId("string")] = true;
	kifReturnString[KifNewId("format")] = true;

	kifReturnInt[idint] = true;
	kifReturnInt[idlong] = true;
	kifReturnInt[idsize] = true;

	kifReturnInt[KifNewId("count")] = true;
	kifReturnInt[KifNewId("find")] = true;
	kifReturnInt[KifNewId("rfind")] = true;
	kifReturnInt[KifNewId("levenshtein")] = true;
	kifReturnInt[KifNewId("editdistance")] = true;

	kifReturnFloat[KifNewId("float")] = true;
}

void KifGlobalThread::InitialisationInitStructs(int trig, KifCode* kifcodeptr) {
	equivalences[kifString] = kifString;
	equivalences[kifTime] = kifTime;
	equivalences[kifAutomaton] = kifAutomaton;
	equivalences[kifInteger] = kifInteger;
	equivalences[kifByte] = kifInteger;
	equivalences[kifLong] = kifInteger;
	equivalences[kifFraction] = kifInteger;
	equivalences[kifBit] = kifInteger;
	equivalences[kifBits] = kifInteger;
	equivalences[kifFloat] = kifInteger;
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
	equivalences[kifMapString] = kifMap;
	equivalences[kifMapFloat] = kifMap;
	equivalences[kifMapInteger] = kifMap;
	equivalences[kifTreeMap] = kifMap;
	equivalences[kifIntegerTreeMap] = kifMap;
	equivalences[kifFloatTreeMap] = kifMap;
	equivalences[kifTreeMapIntegerString] = kifMap;
	equivalences[kifTreeMapIntegerFloat] = kifMap;
	equivalences[kifTreeMapIntegerInteger] = kifMap;
	equivalences[kifTreeMapFloatString] = kifMap;
	equivalences[kifTreeMapFloatFloat] = kifMap;
	equivalences[kifTreeMapFloatInteger] = kifMap;
	equivalences[kifTreeMapString] = kifMap;
	equivalences[kifTreeMapFloat] = kifMap;
	equivalences[kifTreeMapInteger] = kifMap;
	equivalences[kifPrimeMap] = kifMap;
	equivalences[kifIntegerPrimeMap] = kifMap;
	equivalences[kifFloatPrimeMap] = kifMap;
	equivalences[kifPrimeMapIntegerString] = kifMap;
	equivalences[kifPrimeMapIntegerFloat] = kifMap;
	equivalences[kifPrimeMapIntegerInteger] = kifMap;
	equivalences[kifPrimeMapFloatString] = kifMap;
	equivalences[kifPrimeMapFloatFloat] = kifMap;
	equivalences[kifPrimeMapFloatInteger] = kifMap;
	equivalences[kifPrimeMapString] = kifMap;
	equivalences[kifPrimeMapFloat] = kifMap;
	equivalences[kifPrimeMapInteger] = kifMap;
	equivalences[kifVector] = kifVector;
	equivalences[kifTable] = kifVector;
	equivalences[kifVectorString] = kifVector;
	equivalences[kifVectorFloat] = kifVector;
	equivalences[kifVectorInteger] = kifVector;
	equivalences[kifBaseVector] = kifVector;
	equivalences[kifList] = kifList;
	equivalences[kifFile] = kifFile;
	equivalences[kifFileWide] = kifFile;
	equivalences[kifSet] = kifSet;
	equivalences[kifSetString] = kifSet;
	equivalences[kifSetFloat] = kifSet;
	equivalences[kifSetInteger] = kifSet;

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


	KifCreatePredefined(kifcodeptr, "gcsize", P_FULL, &PreGCSize);
	KifCreatePredefined(kifcodeptr, "hash", P_ONE, &PreHash);
	KifCreatePredefined(kifcodeptr, "next", P_ONE, &PreNext);

	KifCreatePredefined(kifcodeptr, "extract", P_ATLEASTFOUR, &PreExtract);

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



	KifCreatePredefined(kifcodeptr, "log", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "ln", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "abs", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "exp", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "sqrt", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "tan", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "sin", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "cos", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "atan", P_ONE, &PreMath);
	KifCreatePredefined(kifcodeptr, "factors", P_ONE, &PrePrimefactors);

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
	KifCreatePredefined(kifcodeptr, "add", P_TWO | P_THREE, &PreAddword);
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

	KifCreatePredefined(kifcodeptr, "version", P_NONE, &PreVersion);
	KifCreatePredefined(kifcodeptr, "print", P_FULL, &PrePrint);
	KifCreatePredefined(kifcodeptr, "exit", P_ONE | P_NONE, &PreExit);
	KifCreatePredefined(kifcodeptr, "pflush", P_FULL, &PrePrintFlush);
	KifCreatePredefined(kifcodeptr, "println", P_FULL, &PrePrintLN);
	KifCreatePredefined(kifcodeptr, "printj", P_ONE | P_TWO | P_THREE, &PrePrintJoin);
	KifCreatePredefined(kifcodeptr, "printjerr", P_ONE | P_TWO | P_THREE, &PrePrintJoinErr);
	KifCreatePredefined(kifcodeptr, "printjln", P_ONE | P_TWO | P_THREE, &PrePrintJoinLN);
	KifCreatePredefined(kifcodeptr, "printjlnerr", P_ONE| P_TWO | P_THREE, &PrePrintJoinErrLN);
	KifCreatePredefined(kifcodeptr, "erroronkey", P_ONE, &PreErrorOnKey);
	KifCreatePredefined(kifcodeptr, "predicatedump", P_NONE | P_ONE, &PrePredicateDump);
	KifCreatePredefined(kifcodeptr, "findall", P_NONE | P_ONE, &PrePredicateDump);
	KifCreatePredefined(kifcodeptr, "retractall", P_NONE | P_ONE, &PreRetractAll);
	KifCreatePredefined(kifcodeptr, "garbagesurvey", P_NONE, &PreGarbageSurvey);
	KifCreatePredefined(kifcodeptr, "garbageframe", P_ONE, &PreGarbageFrame);
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
	KifCreatePredefined(kifcodeptr, "insert", P_THREE, &PreInsert);
	KifCreatePredefined(kifcodeptr, "query", P_ATLEASTTWO, &PreQuery);
	KifCreatePredefined(kifcodeptr, "store", P_ONE | P_TWO, &PreStore);
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
	KifCreatePredefined(kifcodeptr, "replaceregexip", P_THREE, &PreXipReplaceRgx);
	KifCreatePredefined(kifcodeptr, "replace", P_THREE, &PreReplace);
	KifCreatePredefined(kifcodeptr, "levenshtein", P_TWO, &PreLevenshtein);
	KifCreatePredefined(kifcodeptr, "editdistance", P_TWO | P_FOUR, &PreLevenshtein);
	KifCreatePredefined(kifcodeptr, "_trace", P_TWO, &PreTrace);
	KifCreatePredefined(kifcodeptr, "_predicatemode", P_ONE|P_TWO|P_THREE, &PreMode);
	KifCreatePredefined(kifcodeptr, "split", P_THREE | P_TWO | P_ONE, &PreSplit);
	KifCreatePredefined(kifcodeptr, "tokenize", P_ONE | P_TWO | P_THREE | P_FOUR, &PreTokenize);
	KifCreatePredefined(kifcodeptr, "stokenize", P_ONE|P_TWO, &PreSTokenize);
	KifCreatePredefined(kifcodeptr, "find", P_THREE | P_TWO, &PreFind);
	KifCreatePredefined(kifcodeptr, "count", P_FOUR | P_THREE | P_TWO, &PreCount);
	KifCreatePredefined(kifcodeptr, "test", P_TWO, &PreTest);
	KifCreatePredefined(kifcodeptr, "rfind", P_THREE | P_TWO, &PreRFind);
	KifCreatePredefined(kifcodeptr, "removefirst", P_TWO, &PreRemovefirst);
	KifCreatePredefined(kifcodeptr, "removelast", P_TWO, &PreRemovelast);
	KifCreatePredefined(kifcodeptr, "ngrams", P_TWO, &PreNGRAMS);
	KifCreatePredefined(kifcodeptr, "isutf8", P_ONE, &PreTestUTF8);
	KifCreatePredefined(kifcodeptr, "utf8", P_ONE, &PreLatinToUTF8);
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

void KifGlobalThread::KifInitInformations() {
	kifInformationsPredicate["weight"] = "weight(): Returns the weight of this predicate.";
	kifInformationsPredicate["query"] = "query(predicate,var1,var2...): This query is analyzed as a predicate.";
	kifInformationsPredicate["store"] = "store(sqlite db): Store a predicate in the database. If db is omitted then store it in the knowledge base";
	kifInformationsPredicate["remove"] = "remove(sqlite db): Remove a predicate from the database. If db is omitted then remove it from the knowledge base";

	kifInformationsString["hash"] = "hash(): Return the hash value of a string.";
	kifInformationsString["json"] = "json(): Return the json encoded version of a string.";
	kifInformationsString["byteposition"] = "byteposition(int pos): convert a character position into a byte position (especially useful in UTF8 strings)";
	kifInformationsString["charposition"] = "charposition(int pos): convert a byte position into a character position.";
	kifInformationsString["size"] = "size(): Return the size of the string.";
	kifInformationsString["evaluate"] = "evaluate(): evaluate the meta-characters within a string and return the evaluated string.";
	kifInformationsString["clear"] = "clear(): Clean the content of a string.";
	kifInformationsString["tokenize"] = "tokenize(bool comma,bool separator): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number.";
	kifInformationsString["stokenize"] = "stokenize(map keeps): Segment a string into words and punctuations, with a keep.";
	kifInformationsString["split"] = "split(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters";
	kifInformationsString["removefirst"] = "removefirst(int nb): remove the first nb characters of a string";
	kifInformationsString["removelast"] = "removelast(int nb): remove the last nb characters of a string";
	kifInformationsString["fill"] = "fill(int nb,string c): create a string of nb characters c";
	kifInformationsString["padding"] = "padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional";
	kifInformationsString["format"] = "format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.";
	kifInformationsString["ord"] = "ord(): return the ASCII code of the first character, or a list of all ASCII code if the recipient is a vector";
	kifInformationsString["left"] = "left(int nb): return the first nb characters of a string";
	kifInformationsString["byteposition(int pos)"] = "Convert a character position into a byte position";
	kifInformationsString["charposition(int pos)"] = "Convert a byte position into a character position";
	kifInformationsString["right"] = "right(int nb): return the last nb characters of a string";
	kifInformationsString["mid"] = "mid(int pos,int nb): return the nb characters starting at position pos of a string";
	kifInformationsString["reverse"] = "reverse(): reverse the string";
	kifInformationsString["pop"] = "pop(i): remove character at position i";
	kifInformationsString["pop"] = "pop(): remove last character";
	kifInformationsString["last"] = "last(): return last character";
	kifInformationsString["insert"] = "insert(i,s): insert the string s at i. If i is -1, then insert s between each character in the input string";
	kifInformationsString["trim"] = "trim(): remove the trailing characters";
	kifInformationsString["trimright"] = "trimright(): remove the trailing characters on the right";
	kifInformationsString["trimleft"] = "trimleft(): remove the trailing characters on the left";
	kifInformationsString["deaccentuate"] = "deaccentuate(): Remove the accents from accented characters";
	kifInformationsString["upper"] = "upper(): Return the string in upper characters";
	kifInformationsString["lower"] = "lower(): Return the string in lower characters";
	kifInformationsString["islower"] = "islower(): Test if a string only contains lowercase characters";
	kifInformationsString["isupper"] = "isupper(): Test if a string only contains uppercase characters";
	kifInformationsString["isalpha"] = "isalpha(): Test if a string only contains only alphabetical characters";
	kifInformationsString["isconsonant"] = "isconsonant(): Test if a string only contains consonants";
	kifInformationsString["isvowel"] = "isvowel(): Test if a string only contains only vowels";
	kifInformationsString["ispunctuation"] = "ispunctuation(): Test if the character is a punctuation";
	kifInformationsString["extract"] = "extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings";
	kifInformationsString["isdigit"] = "isdigit(): Test if a string only contains digits";
	kifInformationsString["latin"] = "latin(): convert an UTF8 string in LATIN";
	kifInformationsString["utf8"] = "utf8(): convert a LATIN string into UTF8";
	kifInformationsString["dos"] = "dos(): convert a string into DOS encoding";
	kifInformationsString["dostoutf8"] = "dostoutf8(): convert a DOS string into UTF8";
	kifInformationsString["isutf8"] = "isutf8(): Return true is the string is encoded in UTF8";
	kifInformationsString["ngrams"] = "ngrams(int r): Return a vector of all ngrams of rank r";
	kifInformationsString["count"] = "count(string sub,int pos,int mx): Count the number of substrings starting at position pos, ending at mx";
	kifInformationsString["find"] = "find(string sub,int pos): Return the position of substring sub starting at position pos";
	kifInformationsString["rfind"] = "rfind(string sub,int pos): Return the position of substring sub backward starting at position pos";
	kifInformationsString["replaceregexip"] = "replaceregexip(sub,str): Replace the substrings matching sub with str";
	kifInformationsString["levenshtein"] = "levenshtein(string s): Return the edit distance with 's' according to Levenshtein algorithm";
	kifInformationsString["editdistance"] = "editdistance(string s): Return the edit distance with 's'";
	kifInformationsString["html"] = "html(): Return the string into an HTML compatible string or as a vector of strings";
	kifInformationsString["regexip"] = "regexip(sub): Find the substrings matching sub";
	kifInformationsString["replace"] = "replace(sub,str): Replace the substrings matching sub with str";
	kifInformationsString["getstd"] = "getstd(bool): catch or release the standard output";
	kifInformationsString["geterr"] = "geterr(bool): catch or release the error output";
	kifInformationsString["replacergx"] = "replacergx(rgx,str): Replace the substrings matching rgx with str";
	kifInformationsString["splitrgx"] = "splitrgx(rgx): Split string with regular expression rgx. Return a vector of substrings.";
	kifInformationsString["regex"] = "regex(rgx): Test if the regular expression rgx applies to string|vector=regex(rgx): Return all substrings matching rgx|string=regex(rgx): Return the substring matching rgx in the string|int=regex(rgx): Return the position of the substring matching rgx in the string";
	kifInformationsString["parse"] = "parse(): Parse a string as a piece of code and returns the evaluation as a vector.";
	kifInformationsString["lisp"] = "lisp(): Parse a string as a parenthetic expressions, which is returned as a vector.|lisp(string o,string c): Parse a string as a parenthetic expressions, where the opening and closing characters are provided.";
	kifInformationsString["tags"] = "tags(string o,string c): Parse a string as a parenthetic expression, where the opening and closing strings are provided.";
	kifInformationsInt["factors"] = "factors(): return the decomposition in prime factors";
	kifInformationsFloat["factors"] = "factors(): return the decomposition in prime factors";
	kifInformationsInt["log"] = "log(): return the log base 10 of the value";
	kifInformationsFloat["log"] = "log(): return the log base 10 of the value";
	kifInformationsInt["ln"] = "ln(): return the neperian log";
	kifInformationsInt["abs"] = "abs(): return the absolute value of a number";
	kifInformationsFloat["abs"] = "abs(): return the absolute value of a number";
	kifInformationsFloat["ln"] = "ln(): return the neperian log";
	kifInformationsInt["exp"] = "exp(): return the exponential";
	kifInformationsFloat["exp"] = "exp(): return the exponential";
	kifInformationsInt["sqrt"] = "sqrt(): return the square root";
	kifInformationsFloat["sqrt"] = "sqrt(): return the square root";
	kifInformationsInt["invert"] = "invert(): Invert the denominator and the numerator of a fraction";
	kifInformationsFloat["invert"] = "invert(): Invert the denominator and the numerator of a fraction";
	kifInformationsInt["partof"] = "partof(bits x): Return true if the bits of x are all in the current bits";
	kifInformationsFloat["partof"] = "partof(bits x): Return true if the bits of x are all in the current bits";
	kifInformationsInt["d"] = "d(): Denominator of a fraction (with a parameter: set the value, without parameter return the value)";
	kifInformationsInt["infos"] = "infos(string): infos about a method";
	kifInformationsInt["methods"] = "methods(): Return a vector of all available methods";
	kifInformationsInt["simplify"] = "simplify(): Simplification";
	kifInformationsFloat["simplify"] = "simplify(): Simplification";
	kifInformationsInt["n"] = "n(): Numerator of a fraction (with a parameter: set the value, without parameter return the value)";
	kifInformationsInt["nd"] = "nd(int n,int d): Set the Numerator and the Denominator of a fraction";
	kifInformationsInt["tan"] = "tan(): tangent";
	kifInformationsFloat["tan"] = "tan(): tangent";
	kifInformationsInt["chr"] = "chr(): return the character whose code is the value of the variable";
	kifInformationsFloat["chr"] = "chr(): return the character whose code is the value of the variable";
	kifInformationsInt["sin"] = "sin(): sinus";
	kifInformationsFloat["sin"] = "sin(): sinus";
	kifInformationsInt["cos"] = "cos(): cosinus";
	kifInformationsFloat["cos"] = "cos(): cosinus";
	kifInformationsInt["atan"] = "atan(): arc tangent";
	kifInformationsFloat["atan"] = "atan(): arc tangent";
	kifInformationsInt["#"] = "#(): return the bit complement";
	kifInformationsFloat["#"] = "#(): return the bit complement";
	kifInformationsInt["format"] = "format(string form): return a string formatted according to the pattern in form. (this format is the same as the sprintf format in C++)";
	kifInformationsFloat["format"] = "format(string form): return a string formatted according to the pattern in form. (this format is the same as the sprintf format in C++)";
	kifInformationsString["get"] = "get(): Read a string from keyboard";
	kifInformationsString["bytes"] = "bytes(): Return the string as a vector of bytes";
	kifInformationsInt["get"] = "get(): Read a number from keyboard";
	kifInformationsFloat["get"] = "get(): Read a number from keyboard";

	kifInformationsVector["resize"] = "resize(nb): Resize a table";
	kifInformationsVector["move"] = "move(int pfrom,int pto): move the element in pfrom to pto.";
	kifInformationsVector["push"] = "push(a): add a to the vector";
	kifInformationsVector["editdistance"] = "editdistance(s): Return the edit distance with 's'";
	kifInformationsVector["flatten"] = "flatten(): merge all the sub-vectors elements into the current vector";
	kifInformationsVector["merge"] = "merge(v): merge the elements from v into the container";
	kifInformationsVector["bytes"] = "bytes(): Return the string matching a vector of bytes. Each value should be between 0..255";
	kifInformationsVector["totext"] = "totext(): Return the string matching a vector of bytes. Each value should be between 0..255";
	kifInformationsVector["test"] = "test(i): test if i is a valid slot in the vector";
	kifInformationsVector["pop"] = "pop(int i): remove the ith element from the vector and return it.";
	kifInformationsVector["pop"] = "pop(): remove the last element from the vector and return it.";
	kifInformationsVector["last"] = "last(): return the last element of the vector";
	kifInformationsVector["reverse"] = "reverse(): reverse the order of the elements in the vector";
	kifInformationsVector["join"] = "join(string sep): concatenate each element in the vector in a string where each element is separated from the others with sep";
	kifInformationsVector["reserve"] = "reserve(int sz): Reserve sz elements in a vector";
	kifInformationsVector["insert"] = "insert(i,x): insert the element x at position i";
	kifInformationsVector["sum"] = "sum(): Sum each element with the others";
	kifInformationsVector["product"] = "product(): Multiply each element with the others";
	kifInformationsVector["predicate"] = "predicate(): Create a new predicate of the form [head,param1,param2..]";
	kifInformationsVector["clear"] = "clear(): clean the vector";
	kifInformationsVector["min"] = "min(): return the minimum value in a container";
	kifInformationsVector["max"] = "max(): return the maximum value in a container";
	kifInformationsVector["sort"] = "sort(compare): sort the content of the vector according to compare function. compare is optional";
	kifInformationsVector["shuffle"] = "shuffle(): reshuffle the values";
	kifInformationsVector["permute"] = "permute(): permute the values in a vector. Return false, when all permutations have been explored.";
	kifInformationsVector["unique"] = "unique(): remove duplicates";
	kifInformationsVector["sortint"] = "sortint(bool order): sort the content of the vector forcing elements to be ints.";
	kifInformationsVector["sortfloat"] = "sortfloat(bool order): sort the content of the vector forcing elements to be floats.";
	kifInformationsVector["sortlong"] = "sortlong(bool order): sort the content of the vector forcing elements to be longs.";
	kifInformationsVector["sortstring"] = "sortstring(bool order): sort the content of the vector forcing elements to be strings.";
	kifInformationsVector["apply"] = "apply(a,b,c): apply all XIP rules stored in the vector, passing a,b,c etc. as parameters.";
	kifInformationsList["pushfirst"] = "pushfirst(a): add a to the beginning of the list";
	kifInformationsList["editdistance"] = "editdistance(s): Return the edit distance with 's'";
	kifInformationsList["flatten"] = "flatten(): merge all the sub-vectors elements into the current vector";
	kifInformationsList["test"] = "test(i): test if i is a valid slot in the list";
	kifInformationsList["permute"] = "permute(): permute the values in a vector. Return false, when all permutations have been explored.";
	kifInformationsList["shuffle"] = "shuffle(): reshuffle the values";
	kifInformationsList["unique"] = "unique(): remove duplicates";
	kifInformationsList["merge"] = "merge(v): merge the elements from v into the container";
	kifInformationsList["pushlast"] = "pushlast(a): add a at the end of the list";
	kifInformationsList["popfirst"] = "popfirst(): remove the first element from the list and return it.";
	kifInformationsList["min"] = "min(): return the minimum value in a container";
	kifInformationsVector["max"] = "max(): return the maximum value in a container";
	kifInformationsList["poplast"] = "poplast(): remove the last element from the list and return it.";
	kifInformationsList["last"] = "last(): return the last element of the list";
	kifInformationsList["first"] = "first(): return the first element of the list";
	kifInformationsList["reverse"] = "reverse(): reverse the order of the elements in the list";
	kifInformationsList["join"] = "join(string sep): concatenate each element in the list in a string where each element is separated from the others with sep";
	kifInformationsList["insert"] = "insert(i,x): insert the element x at position i";
	kifInformationsList["sum"] = "sum(): Sum each element with the others";
	kifInformationsList["product"] = "product(): Multiply each element with the others";
	kifInformationsList["apply"] = "apply(a,b,c): apply all XIP rules stored in the list, passing a,b,c etc. as parameters.";
	kifInformationsList["clear"] = "clear(): clean the list";

	kifInformationsMap["invert"] = "invert(): Invert keys and values in a map";
	kifInformationsMap["sum"] = "sum(): Sum each element with the others";
	kifInformationsMap["join"] = "join(string keysep,string sep): concatenate each element in the map in a string where each key is appended with a keysep and each element is separated from the others with sep";
	kifInformationsMap["product"] = "product(): Multiply each element with the others";
	kifInformationsMap["apply"] = "apply(a,b,c): apply all XIP rules stored in the list, passing a,b,c etc. as parameters.";
	kifInformationsMap["merge"] = "merge(v): merge the elements from v into the container";
	kifInformationsMap["predicate"] = "predicate(): Create a new predicate of the form {'name':name,0:param1,1:param2..]";
	kifInformationsMap["test"] = "test(i): test if i is a valid key in the map";
	kifInformationsMap["editdistance"] = "editdistance(s): Return the edit distance with 's'";
	kifInformationsMap["pop"] = "pop(string key): remove the elements matching key";
	kifInformationsMap["keys"] = "keys(): returns the map keys as a vector";
	kifInformationsMap["values"] = "values(): return the values as a vector";
	kifInformationsMap["items"] = "items(): return a vector of key:value";
	kifInformationsMap["min"] = "min(): return the minimum value in a container";
	kifInformationsMap["max"] = "max(): return the maximum value in a container";
	kifInformationsMap["clear"] = "clear(): clean the map";
	kifInformationsMap["evaluation"] = "evaluation(): evaluate the memory footprint of a primemap";
	kifInformationsMap["seeds"] = "seeds(ivector iv): set the primemap seed keys";

	kifInformationsTree["tree"] = "tree(value): create a tree node, with value as a value. Value can have any types";
	kifInformationsTree["editdistance"] = "editdistance(s): Return the edit distance with 's'";
	kifInformationsTree["sister"] = "sister()|sister(tree n): return the sister node, or add a sister node";
	kifInformationsTree["previous"] = "previous|previous(tree n): return the previous node or add n as the previous node";
	kifInformationsTree["daughter"] = "daughter()|daughter(tree n): return the first child node or add n as the first child node";
	kifInformationsTree["mother"] = "mother()|mother(tree n): return the mother node or test if the current node is a parent of n";
	kifInformationsTree["lastnode"] = "lastnode()|lastnode(tree n): return the last node or test if the current node is the last child of n";
	kifInformationsTree["isolate"] = "isolate(): Extract the current node from its tree";
	kifInformationsTree["prune"] = "prune(): Delete the current sub-tree from the global tree";
	kifInformationsTree["depth"] = "depth(): Return the depth of the node in the tree";
	kifInformationsIterator["value"] = "value(): return the value of the current element";
	kifInformationsIterator["key"] = "key(): return the key of the current element";
	kifInformationsIterator["valuetype"] = "valuetype(): return the value type of the current element";
	kifInformationsIterator["valueisa"] = "valueisa(string type): test the type of the current element";
	kifInformationsIterator["begin"] = "begin(): initialiaze the iterator with the beginning of the collection";
	kifInformationsIterator["next"] = "next(): next element in the collection";
	kifInformationsIterator["setvalue"] = "setvalue(value): Set a new value to the current iterator placeholder.";
	kifInformationsIterator["end"] = "end(): return true when the end of the collection is reached";
	kifInformationsIterator["nend"] = "nend(): return true while the end of the collection has not been reached (~end())";
	kifInformationsIterator["apply"] = "apply(a,b,c): apply a XIP rule or a function";
	kifInformationsDate["setdate"] = "setdate(year,month,day,hour,min,sec): set a time variable";
	kifInformationsDate["date"] = "date(): return the date as a string";
	kifInformationsDate["year"] = "year(): return the year as an integer";
	kifInformationsDate["month"] = "month(): return the month as an integer";
	kifInformationsDate["day"] = "day(): return the day as an integer";
	kifInformationsDate["yearday"] = "yearday(): return the year day as an integer between 0-365";
	kifInformationsDate["weekday"] = "weekday(): return the week day as an integer between 0-6, 0 is Sunday.";
	kifInformationsDate["hour"] = "hour(): return the hour as an integer";
	kifInformationsDate["minute"] = "minute(): return the minutes as an integer";
	kifInformationsDate["second"] = "second(): return the seconds as an integer";
	kifInformationsTime["reset"] = "reset (): reinitialize a time variable";
	kifInformationsFile["openappend"] = "openappend(string filename): open a file in append writing mode";
	kifInformationsFile["openwrite"] = "openwrite(string filename): open a file in write mode";
	kifInformationsFile["find"] = "find(string str,bool uppercase): Find a string in a file";
	kifInformationsFile["openread"] = "openread(string filename): open a file in read mode";
	kifInformationsFile["setsignature"] = "setsignature(bool s): Set the signature mode";
	kifInformationsFile["getsignature"] = "getsignature(): Return the signature mode";
	kifInformationsFile["write"] = "write(string s1,string s2,): write strings in the file";
	kifInformationsFile["writebin"] = "writebin(int s1,int s2,): write character code in a file";
	kifInformationsFile["get"] = "get(): read one character from the file";
	kifInformationsFile["unget"] = "unget(): return one character to the stream";
	kifInformationsFile["unget"] = "unget(nb): return nb character to the stream";
	kifInformationsFile["read"] = "read(): read a line from a file";
	kifInformationsFile["readln"] = "readln(): read a line from a file";
	kifInformationsFile["seek"] = "seek(int p): position the file cursor at p";
	kifInformationsFile["tell"] = "tell(): return the position of the file cursor";
	kifInformationsFile["eof"] = "eof(): return true when the end of file is reached";

	kifInformationsAutomaton["load"] = "load(string kifpathname,string featchar): Load a word file. Each line should contain one single word. featchar defines the character which starts features, it is optional.";
	kifInformationsAutomaton["loadcompact"] = "loadcompact(string kifpathname): Load a file stored in a compact format.";
	kifInformationsAutomaton["loadlemma"] = "loadlemma(string kifpathname): Load a word and lemma file. The even line contains a surface form and the odd lines the lemma and features.";
	kifInformationsAutomaton["model"] = "model(function): Set the model function which is used to traverse the automaton";
	kifInformationsAutomaton["look"] = "look(word): Traverses the automaton and returns the most probable readings accoding to the model function";
	kifInformationsAutomaton["spotfinalstate"] = "spotfinalstate(bool): Detect final states in automaton while in 'look'";
	kifInformationsAutomaton["settransformationmap"] = "settransformationmap(map m): Set the transformation map to set the weights when traversing the automaton with editdistance";
	kifInformationsAutomaton["store"] = "store(string pathname): Store the file into a compact format";
	kifInformationsAutomaton["size"] = "size(): Returns the number of words stored in the automaton";
	kifInformationsAutomaton["dump"] = "dump(string path): Dump the automaton content into a file";
	kifInformationsAutomaton["setcodefeature"] = "setcodefeature(string character): Use this character to detect the features while adding words";
	kifInformationsAutomaton["editdistance"] = "editdistance(string word,int threshold,int flags): Finds words whose edit distance to 'word' is inferior to the threshold. flags values are: 1: action on first character. 2: change character. 4: delete character. 8: insert character.";
	kifInformationsAutomaton["clear"] = "clear(): Clears the automaton";
	kifInformationsAutomaton["add"] = "add(string word): add a word to the automaton. add(string word,string lemma): add a word and its lemma to the automaton.";

	kifInformationsKif["load"] = "load(string kifpathname): Load a KiF program";
	kifInformationsKif["compile"] = "compile(string kifpathname): Compile a KiF program as a string. Return an handle on the first instruction to execute ";
	kifInformationsKif["run"] = "run(int i): run a program from instruction i (returned by compile)";
	kifInformationsKif["kstart"] = "kstart(): Return the declared variables as a map";
	kifInformationsKif["runasthread"] = "runasthread(int i): run a program from instruction i (returned by compile) in a thread";
	kifInformationsKif["runend"] = "runend(): Return true if the programme execution has been completed";
	kifInformationsKif["debugfunction"] = "debugfunction(function,object): set the debug function that will be called during the execution";
	kifInformationsKif["debugclose"] = "debugclose(): Remove the debug mode initiated with debugfunction";
	kifInformationsKif["open"] = "open(): open a KiF session";
	kifInformationsKif["clean"] = "clean(): close a KiF session";
	kifInformationsKif["name"] = "name(): Return the pathname of the KiF file";
	kifInformationsKif["exposed"] = "exposed(): Return the list of methods exposed by KiF";
	kifInformationsKif["_loader"] = "_loader: A kif variable (of the type described here), which stores a pointer to the loading program.";
}

bool KifGlobalThread::KifCreatePredefined(KifCode* kcode, string name, unsigned int arity, Predefined func, bool tree) {
	int idname = KifNewId(name);
	if (tree == false) {
		if (kifBaseFunctions.find(idname) != kifBaseFunctions.end())
			return false;

		kifBaseFunctions[idname] = new KifPredefined(NULL, arity, func, idname);
		kifBaseFunctions[idname]->kifcode = kcode;
	}
	else {
		if (kifBaseTreeFunctions.find(idname) != kifBaseTreeFunctions.end())
			return false;

		kifBaseTreeFunctions[idname] = new KifPredefined(NULL, arity, func, idname);
		kifBaseTreeFunctions[idname]->kifcode = kcode;
	}

	return true;
}

void KifGlobalThread::RecordExternalFunction(short idname, unsigned int arity) {
	if (kifExternalFunctions.find(idname) == kifExternalFunctions.end()) {
		KifCallMethod* kcm = new KifCallMethod(idname, arity);
		kifExternalFunctions[idname] = kcm;
	}
	else
		kifExternalFunctions[idname]->arity |= arity;
}

bool KifGlobalThread::TestExternalArity(int id, int nbparams) {
	if (kifExternalFunctions.find(id) == kifExternalFunctions.end())
		return false;

	unsigned int arity = 1 << nbparams;
	unsigned int expected = kifExternalFunctions[id]->arity;
	if ((expected&arity) != arity)
		return false;
	return true;
}

KifElement* KifGlobalThread::KifBaseFunctionTest(short name) {
	if (kifBaseFunctions.find(name) != kifBaseFunctions.end())
		return kifBaseFunctions[name];
	if (kifExternalFunctions.find(name) != kifExternalFunctions.end())
		return kifExternalFunctions[name];
	if (kifBaseTreeFunctions.find(name) != kifBaseTreeFunctions.end())
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
	for (i = 0; i<mxthreads; i++) {
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
	hmap<short, KifCallMethod*>::iterator itfuncs;
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

	for (cursorstring = 0; cursorstring<stringlist.size(); cursorstring++)
		delete stringlist[cursorstring];

	for (cursorinteger = 0; cursorinteger<integerlist.size(); cursorinteger++)
		delete integerlist[cursorinteger];

	for (cursorfloat = 0; cursorfloat<floatlist.size(); cursorfloat++)
		delete floatlist[cursorfloat];

	for (int i = 0; i<garbage.size(); i++) {
		if (garbage[i] != NULL)
			delete garbage[i];
	}

	hmap<short, KifPredefined*>::iterator ifunc;
	for (ifunc = kifGlobals->kifBaseFunctions.begin(); ifunc != kifGlobals->kifBaseFunctions.end(); ifunc++)
		delete ifunc->second;

	for (ifunc = kifGlobals->kifBaseTreeFunctions.begin(); ifunc != kifGlobals->kifBaseTreeFunctions.end(); ifunc++)
		delete ifunc->second;

	hmap<short, LockedThread*>::iterator it;
	for (it = loquets.begin(); it != loquets.end(); it++) {
		if (it->second != NULL)
			delete it->second;
	}
	loquets.clear();

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

	int idthread = kifGlobals->GetThreadid();
	//Only the main thread can do garbaging
	if (idthread)
		return;

	if (garbagefunction != NULL)
		garbagefunction->Execute(kifNULL, NULL);

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



