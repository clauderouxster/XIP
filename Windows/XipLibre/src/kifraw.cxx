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
filename   : kifraw.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language: External method implementation.
Programmer : Claude ROUX
Reviewer   :
*/

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//KIF RAW SECTION
//Below are the basic bodies of specific methods, whose bodies are different in the XIP version

#include "kif.h"
#include "kifversion.h"
#include "kifinit.h"
#include "kifcontainer.h"
#include "kifmaps.h"
#include "kiftreemaps.h"
#include "kifautomaton.h"
#include "fractalhmap.h"
#include "kifpmaps.h"

#ifdef WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#endif 

#ifdef UNIX
#include <unistd.h>
#include <float.h>
#include <dlfcn.h>
#endif

bool localevaluate(KifType typecontext, KifType thetype) {
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

#ifdef DOSOUTPUT
static bool dosoutput = true;
void setdosoutput(bool d) { dosoutput = d; }
#define conversion2dos(x) dosoutput?s_utf8_to_dos(USTR(x)):x
#endif


extern const char* kifErrorStrings[];
#ifdef _DEBUG
extern KifCode* currentkifcode;
#endif

#ifdef WIN32
int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

int GETCH(bool echoing);
KifDebugInfo* DebugPtr();
int KifLoadin(KifCode* kifcode, string path);
void KifInitFunctions(KifCode* kifcodeptr, KifFrame* kstart);
string replacestringwithautomaton(string &str, string& reg, string& rep);
extern KifElement* kifWAIT;
string KifIdString(short n);
string Whoami_kif();
extern KifGlobalThread* kifGlobals;
void KifEvaluateMetaString(string& thestr, string sub);

//===================================================================
#ifndef DBL_MAX
#define DBL_MAX         1.7976931348623158e+308 /* max value */
#endif

//===================================================================
bool kifrestrandom = false;
double localrandom(long mx) {
	if (!mx)
		mx = 1;
	static unsigned long x = 123456789;
	static unsigned long y = 362436069;
	static unsigned long z = 521288629;
	static long w = time(0);
	unsigned long t;
	if (kifrestrandom) {
		w = time(0);
		kifrestrandom = false;
	}

	t = x ^ (x << 11);
	x = y; y = z; z = w;
	w = w ^ (w >> 19) ^ (t ^ (t >> 8));
	return abs(w%mx);
}


//===================================================================
class KifComparison {
public:

	KifDomain* domain;
	KifFunction* compare;

	KifComparison(KifDomain* dom, KifFunction* comp) {
		domain = dom;
		compare = comp;
	}

	bool operator() (KifElement* i, KifElement* j) {
		return domain->applycomparatorfunction(i, j, compare)->Boolean();
	}
};

class KifComparisonString {
public:

	KifDomain* domain;
	KifFunction* compare;
	KifBaseString* li;
	KifBaseString* lj;

	KifComparisonString(KifDomain* dom, KifFunction* comp, KifBaseString* i, KifBaseString* j) {
		domain = dom;
		compare = comp;
		li = i;
		lj = j;
	}

	bool operator() (string i, string j) {
		li->value = i;
		lj->value = j;
		return domain->applycomparatorfunction(li, lj, compare)->Boolean();
	}
};

class KifComparisonInteger {
public:

	KifDomain* domain;
	KifFunction* compare;
	KifBaseInteger* li;
	KifBaseInteger* lj;

	KifComparisonInteger(KifDomain* dom, KifFunction* comp, KifBaseInteger* i, KifBaseInteger* j) {
		domain = dom;
		compare = comp;
		li = i;
		lj = j;
	}

	bool operator() (long i, long j) {
		li->value = i;
		lj->value = j;
		return domain->applycomparatorfunction(li, lj, compare)->Boolean();
	}
};

class KifComparisonFloat {
public:

	KifDomain* domain;
	KifFunction* compare;
	KifBaseFloat* li;
	KifBaseFloat* lj;

	KifComparisonFloat(KifDomain* dom, KifFunction* comp, KifBaseFloat* i, KifBaseFloat* j) {
		domain = dom;
		compare = comp;
		li = i;
		lj = j;
	}

	bool operator() (double i, double j) {
		li->value = i;
		lj->value = j;
		return domain->applycomparatorfunction(li, lj, compare)->Boolean();
	}
};

//===================================================================
//We remove this element everywhere in KifCode kifWaitstates
static hmap<string, vector<KifWaitState*> > kifWaitstates;

KifWaitStates::~KifWaitStates() {
	for (int i = 0; i < flags.size(); i++) {
		bool found = false;
		string flag = flags[i];
		if (kifWaitstates.find(flag) != kifWaitstates.end()) {
			vector<KifWaitState*>& kws = kifWaitstates[flag];
			vector<KifWaitState*>::iterator it;
			for (it = kws.begin(); it != kws.end(); it++) {
				if (*it == this) {
					found = true;
					break;
				}
			}
			if (found) {
				kws.erase(it);
				if (kws.empty())
					kifWaitstates.erase(flag);
			}
		}
	}
}

vector<KifWaitState*>* Freestate(KifCode* kifcode, string& flag) {
	ThreadLock _lock(kifSignal);
	if (kifWaitstates.find(flag) == kifWaitstates.end()) {
		string message = "This wait state does not exist: '";
		message += flag;
		message += "'";
		kifcode->Returnerror(message);
		return NULL;
	}
	return &kifWaitstates[flag];
}


//---------------------------------------------------------------------------------
KifElement* PreSetDebug(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	DebugPtr()->kifdebugging = true;
	DebugPtr()->kifoutoffunction = 0;
	int fileidx = callfunc->Fileindex();
	DebugPtr()->gotoline = callfunc->Evaluatethread(0, dom, idthread)->Integer();
	return kifTRUE;
}

KifElement* PreSetDebugFunction(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfunc = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* object = kifNULL;
	if (callfunc->Size() == 2)
		object = callfunc->Evaluatethread(1, dom, idthread);
	if (kfunc->isCall() || kfunc->isFunction()) {
		if (object != kifNULL)
		if (object->type == kifInstance)
			object = object->Thevariable(dom, kifGlobals->GetThreadid(), NULL);
		else
			object = object->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		kifGlobals->debuginfocurrent = &kifGlobals->debuginfos;
		DebugPtr()->Setkifdebug(kfunc, object);
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[258]);
}

KifElement* PreSetKifCodeDebugFunction(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kcode = callfunc->Evaluatethread(0, dom, idthread);
	if (kcode->type != kifKiF)
		return callfunc->kifcode->Returnerror(kifErrorStrings[260]);
	KifKiF* kkif = (KifKiF*)kcode;
	if (kkif->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[261]);
	KifCode* kifcode = kifGlobals->kifCodeVector[kkif->id];
	KifElement* kfunc = callfunc->Evaluatethread(1, dom, idthread);
	KifElement* object = callfunc->Evaluatethread(2, dom, idthread);
	if (kfunc->isCall() || kfunc->isFunction()) {
		if (object->type == kifInstance)
			object = object->Thevariable(dom, kifGlobals->GetThreadid(), NULL);
		else
			object = object->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		kifGlobals->debuginfocurrent = &kifGlobals->debuginfos;
		DebugPtr()->Setkifdebug(NULL, object);
		kifGlobals->debugfunction = kfunc;
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[258]);
}

KifElement* PreCloseKifCodeDebugFunction(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kcode = callfunc->Evaluatethread(0, dom, idthread);
	if (kcode->type != kifKiF)
		return callfunc->kifcode->Returnerror(kifErrorStrings[260]);
	KifKiF* kkif = (KifKiF*)kcode;
	if (kkif->id == -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[261]);
	DebugPtr()->Close();
	return kifTRUE;
}

KifElement* PreSetDebugData(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* object = callfunc->Evaluatethread(0, dom, idthread);
	if (object->type == kifInstance)
		object = object->Thevariable(dom, kifGlobals->GetThreadid(), NULL);
	else
		object = object->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	DebugPtr()->Setkifdata(object);
	return kifTRUE;
}

KifElement* PreDebugClose(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	DebugPtr()->Close();
	return kifTRUE;
}


KifElement* PreGetInfoStackOnId(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	int i = callfunc->Evaluatethread(0, dom, idthread)->Integer();
	KifMap* kmap = Selectmap(callfunc, contextualpattern);
	if (!kifGlobals->Getstackinfoid(callfunc->kifcode, i, kmap))
		return callfunc->kifcode->Returnerror(kifErrorStrings[259]);
	return kmap;
}

KifElement* PreTheStack(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	DebugPtr()->Stack(callfunc->kifcode);
	return DebugPtr()->debugstack;
}


KifElement* PreTheVariables(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (callfunc->Size() == 1) {
		string var = callfunc->Evaluatethread(0, dom, idthread)->String();
		DebugPtr()->Variables(var);
	}
	else
		DebugPtr()->Variables("");
	return DebugPtr()->debugvariables;
}

KifElement* PreCurrentFilename(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	return DebugPtr()->Currentfilename();
}

KifElement* PreErrorOnKey(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	kifGlobals->erroronkey = res->Boolean();
	return res;
}

KifElement* PreGarbageFrame(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	string sname = res->String();
	short name = -1;
	name = kifGlobals->KifStringId(sname);
	if (name == -1)
		return kifFALSE;
	int sz = 0;
	for (int i = 0; i < kifGlobals->garbage.size(); i++) {
		if (kifGlobals->garbage[i] != NULL) {
			if (kifGlobals->garbage[i]->type == kifDomain) {
				KifDomain* d = (KifDomain*)kifGlobals->garbage[i];
				if (d->Frame()->Name() == name) {
					bin_hash<KifElement*>::iterator it;
					for (it = d->declarations.begin(); it != d->declarations.end(); it++) {
						if (it->second != NULL) {
							KifElement* v = it->second->VariableValue();
							switch (v->type) {
							case kifTime:
							case kifDate:
							case kifInteger:
							case kifFraction:
							case kifFloat:
								sz += 1;
								break;
							default:
								sz += v->Size();
							}
						}
					}
				}
			}
		}
	}
	return callfunc->kifcode->Provideinteger(sz);
}

KifElement* PreGarbageSurvey(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	int i;
	int letype;
	map<short, int> v;
	map<short, int> tobecleaned;
	tobecleaned[kifVOID] = 0;
	for (i = 0; i<kifGlobals->garbage.size(); i++) {
		if (kifGlobals->garbage[i] != NULL) {
			letype = kifGlobals->garbage[i]->type;
			if (kifGlobals->garbage[i]->reference>0) {
				if (v.find(letype) == v.end())
					v[letype] = 1;
				else
					v[letype]++;
			}
			else {
				if (tobecleaned.find(letype) == tobecleaned.end())
					tobecleaned[letype] = 1;
				else
					tobecleaned[letype]++;
			}
		}
		else
			tobecleaned[kifVOID]++;
	}

	if (v.find(kifString) == v.end())
		v[kifString] = 0;


	for (i = 0; i<kifGlobals->stringlist.size(); i++) {
		if (kifGlobals->stringlist[i]->reference>0 || kifGlobals->stringlist[i]->popped)
			v[kifString]++;
	}

	if (v.find(kifInteger) == v.end())
		v[kifInteger] = 0;


	for (i = 0; i<kifGlobals->integerlist.size(); i++) {
		if (kifGlobals->integerlist[i]->reference>0 || kifGlobals->integerlist[i]->popped)
			v[kifInteger]++;
	}

	if (v.find(kifFloat) == v.end())
		v[kifFloat] = 0;

	for (i = 0; i<kifGlobals->floatlist.size(); i++) {
		if (kifGlobals->floatlist[i]->reference>0 || kifGlobals->floatlist[i]->popped)
			v[kifFloat]++;
	}

	KifElement* kpmi = Selectmapinteger(callfunc, contextualpattern);
	map<short, int>::iterator it;
	string n;
	char buff[10];
	for (it = v.begin(); it != v.end(); it++) {
		if (kifGlobals->kifStringType.find((KifType)it->first) != kifGlobals->kifStringType.end())
			n = kifGlobals->kifStringType[(KifType)it->first];
		else {
			sprintf_s(buff, 10, "%d", it->first);
			n = buff;
		}
		kpmi->storekey(n, (long)it->second);
	}

	for (it = tobecleaned.begin(); it != tobecleaned.end(); it++) {
		n = "clean_";
		if (kifGlobals->kifStringType.find((KifType)it->first) != kifGlobals->kifStringType.end())
			n += kifGlobals->kifStringType[(KifType)it->first];
		else {
			sprintf_s(buff, 10, "%d", it->first);
			n += buff;
		}
		kpmi->storekey(n, (long)it->second);
	}

	return kpmi;
}


KifElement* PreTestUTF8(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString)
		return kifTRUE;
	string str = kstr->String();
	if (s_is_utf8(USTR(str), str.size()) == true)
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreParentThreadId(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	int id = kifGlobals->GetThreadid();
	if (id == 0)
		return callfunc->kifcode->Provideinteger(0);
	id = kifGlobals->THR[id]->parentthreads;
	return callfunc->kifcode->Provideinteger(id);
}


KifElement* PreId(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	return callfunc->kifcode->Provideinteger(ke->idgarbage);
}

KifElement* PreDuplicate(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	return ke->Copy(dom);
}

KifElement* PreGCSize(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreGCSize");
#endif
	static KifMap kmap(NULL, NULL);
	static KifBaseInteger cursor(NULL, NULL, 0);
	static KifBaseInteger cursormax(NULL, NULL, 0);
	static KifBaseInteger garbage(NULL, NULL, 0);
	static KifBaseInteger empty(NULL, NULL, 0);
	static KifBaseInteger tobecleaned(NULL, NULL, 0);
	static KifBaseInteger popping(NULL, NULL, 0);

	static KifBaseInteger cursorstring(NULL, NULL, 0);
	static KifBaseInteger cursorinteger(NULL, NULL, 0);
	static KifBaseInteger cursorfloat(NULL, NULL, 0);

	static KifBaseInteger garbagestring(NULL, NULL, 0);
	static KifBaseInteger garbageinteger(NULL, NULL, 0);
	static KifBaseInteger garbagefloat(NULL, NULL, 0);

	static bool prepare = true;
	if (prepare == true) {
		kmap.values["cursor"] = &cursor;
		kmap.values["cursormax"] = &cursormax;
		kmap.values["garbage"] = &garbage;
		kmap.values["empty"] = &empty;
		kmap.values["to_be_cleaned"] = &tobecleaned;
		kmap.values["popping"] = &popping;
		kmap.values["cursorstring"] = &cursorstring;
		kmap.values["cursorinteger"] = &cursorinteger;
		kmap.values["cursorfloat"] = &cursorfloat;
		kmap.values["garbagestring"] = &garbagestring;
		kmap.values["garbageinteger"] = &garbageinteger;
		kmap.values["garbagefloat"] = &garbagefloat;
		prepare = false;
	}

	empty.value = 0;
	tobecleaned.value = 0;
	popping.value = 0;
	for (int i = 0; i < kifGlobals->garbage.taille; i++) {
		if (kifGlobals->garbage[i] == NULL)
			empty.value++;
		else
		if (kifGlobals->garbage[i]->reference == 0) {
			if (kifGlobals->garbage[i]->popped == 0)
				tobecleaned.value++;
			else
				popping.value++;
		}
	}

	for (int i = 0; i < kifGlobals->stringlist.size(); i++) {
		if (kifGlobals->stringlist[i] == NULL)
			empty.value++;
		else
		if (kifGlobals->stringlist[i]->reference == 0) {
			if (kifGlobals->stringlist[i]->popped)
				popping.value++;
		}
	}

	for (int i = 0; i < kifGlobals->integerlist.size(); i++) {
		if (kifGlobals->integerlist[i] == NULL)
			empty.value++;
		else
		if (kifGlobals->integerlist[i]->reference == 0) {
			if (kifGlobals->integerlist[i]->popped)
				popping.value++;
		}
	}

	for (int i = 0; i < kifGlobals->floatlist.size(); i++) {
		if (kifGlobals->floatlist[i] == NULL)
			empty.value++;
		else
		if (kifGlobals->floatlist[i]->reference == 0) {
			if (kifGlobals->floatlist[i]->popped)
				popping.value++;
		}
	}


	kmap.reference = 1024;
	cursor.reference = 1024;
	cursormax.reference = 1024;
	garbage.reference = 1024;
	garbagestring.reference = 1024;
	garbageinteger.reference = 1024;
	garbagefloat.reference = 1024;
	empty.reference = 1024;
	tobecleaned.reference = 1024;

	kifGlobals->garbaging = true;
	ThreadLock _lock(kifGarbaging);
	cursor.value = kifGlobals->cursor;
	cursormax.value = kifGlobals->cursormax;
	garbage.value = kifGlobals->garbage.taille;

	cursorstring.value = kifGlobals->cursorstring;
	cursorinteger.value = kifGlobals->cursorinteger;
	cursorfloat.value = kifGlobals->cursorfloat;

	garbagestring.value = kifGlobals->stringlist.size();
	garbageinteger.value = kifGlobals->integerlist.size();
	garbagefloat.value = kifGlobals->floatlist.size();

	kifGlobals->garbaging = false;
	return &kmap;
}


KifElement* PreExposed(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);

	if (ke->type == kifKiF || ke->type == kifLoader) {
		//We get the KiFCode object associated...
		int id = ((KifKiF*)ke)->id;
		KifCode* kcode = kifGlobals->kifCodeVector[id];
		if (kcode == NULL)
			return callfunc->kifcode->Returnerror(kifErrorStrings[247]);
		if (pattern->type == kifMap) {
			KifMap* kmap = Selectmap(callfunc, pattern);
			for (int i = 0; i < kifGlobals->garbage.size(); i++) {
				if (kifGlobals->garbage[i] != NULL && kifGlobals->garbage[i]->isFunction() && kifGlobals->garbage[i]->Private() == false) {
					id = ((KifFunction*)kifGlobals->garbage[i])->name;
					if (id != idsynchronous) {
						string name = KifIdString(id);
						kmap->Push(name, kifGlobals->garbage[i]);
					}
				}
			}
			return kmap;
		}
		KifVector* kvect = Selectvector(callfunc, pattern);
		for (int i = 0; i < kifGlobals->garbage.size(); i++) {
			if (kifGlobals->garbage[i] != NULL && kifGlobals->garbage[i]->isFunction() && kifGlobals->garbage[i]->Private() == false) {
				id = ((KifFunction*)kifGlobals->garbage[i])->name;
				if (id != idsynchronous)
					kvect->Push(kifGlobals->garbage[i]);
			}
		}
		return kvect;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[247]);
}


//--------------------------------------------------------
const int maxrange = 100000;

KifElement* PreRange(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* ksecond = callfunc->Evaluatethread(1, dom, idthread);
	KifElement* kinc = kifONE;
	KifElement* kvect;
	char typevar = kfirst->inCode();
	typevar |= ksecond->inCode();
	if (callfunc->Size() == 3) {
		kinc = callfunc->Evaluatethread(2, dom, idthread);
		typevar |= kinc->inCode();
	}

	if (typevar == ckifInteger) {
		long l = kfirst->Integer();
		long r = ksecond->Integer();
		long d = r - l + 1;
		long inc = kinc->Integer();
		if (inc == 0)
			return callfunc->kifcode->Returnerror(kifErrorStrings[225]);
		if (d<0)
			d *= -1;
		if (l>r && inc > 0)
			inc *= -1;

		if (d <= maxrange) {
			if (contextualpattern == kifNULL)
				kvect = new KifVectorInteger(dom->kifcode, NULL);
			else
				kvect = Selectavector(callfunc, contextualpattern);
			kvect->Reserve(d);
			switch (kvect->type) {
			case kifVectorByte:
				if (inc > 0) {
					for (long i = l; i <= r; i += inc)
						((KifVectorByte*)kvect)->values.push_back((uchar)i);
				}
				else {
					for (long i = l; i >= r; i += inc)
						((KifVectorByte*)kvect)->values.push_back((uchar)i);
				}
				return kvect;
			case kifVectorInteger:
				if (inc > 0) {
					for (long i = l; i <= r; i += inc)
						((KifVectorInteger*)kvect)->values.push_back(i);
				}
				else {
					for (long i = l; i >= r; i += inc)
						((KifVectorInteger*)kvect)->values.push_back(i);
				}
				return kvect;
			case kifVectorFloat:
				if (inc > 0) {
					for (long i = l; i <= r; i += inc)
						((KifVectorFloat*)kvect)->values.push_back(i);
				}
				else {
					for (long i = l; i >= r; i += inc)
						((KifVectorFloat*)kvect)->values.push_back(i);
				}
				return kvect;
			}
			KifBaseInteger loc(NULL, NULL);
			loc.kifcode = callfunc->kifcode;
			if (inc > 0) {
				for (long i = l; i <= r; i += inc) {
					loc.value = i;
					kvect->Push(&loc);
				}
			}
			else {
				for (long i = l; i >= r; i += inc) {
					loc.value = i;
					kvect->Push(&loc);
				}
			}
			return kvect;
		}
		return callfunc->kifcode->Returnerror(kifErrorStrings[227]);
	}

	if (typevar == ckifFloat || typevar == ckifIntFloat) {
		double l = kfirst->Float();
		double r = ksecond->Float();
		double inc = kinc->Float();
		if (inc == 0)
			return callfunc->kifcode->Returnerror(kifErrorStrings[225]);
		double d = (r - l) / inc;
		if (d<0)
			d *= -1;
		if (l>r && inc > 0)
			inc *= -1;

		if (d <= maxrange) {
			if (contextualpattern == kifNULL)
				kvect = new KifVectorFloat(dom->kifcode, NULL);
			else
				kvect = Selectavector(callfunc, contextualpattern);
			kvect->Reserve(d);
			switch (kvect->type) {
			case kifVectorByte:
				if (inc > 0) {
					for (double i = l; i <= r; i += inc)
						((KifVectorByte*)kvect)->values.push_back(i);
				}
				else {
					for (double i = l; i >= r; i += inc)
						((KifVectorByte*)kvect)->values.push_back(i);
				}
				return kvect;
			case kifVectorInteger:
				if (inc > 0) {
					for (double i = l; i <= r; i += inc)
						((KifVectorInteger*)kvect)->values.push_back(i);
				}
				else {
					for (double i = l; i >= r; i += inc)
						((KifVectorInteger*)kvect)->values.push_back(i);
				}
				return kvect;
			case kifVectorFloat:
				if (inc > 0) {
					for (double i = l; i <= r; i += inc)
						((KifVectorFloat*)kvect)->values.push_back(i);
				}
				else {
					for (double i = l; i >= r; i += inc)
						((KifVectorFloat*)kvect)->values.push_back(i);
				}
				return kvect;
			}
			KifBaseFloat loc(NULL, NULL);
			loc.kifcode = callfunc->kifcode;
			if (inc > 0) {
				for (double i = l; i <= r; i += inc) {
					loc.value = i;
					kvect->Push(&loc);
				}
			}
			else {
				for (double i = l; i >= r; i += inc) {
					loc.value = i;
					kvect->Push(&loc);
				}
			}
			return kvect;
		}
		return callfunc->kifcode->Returnerror(kifErrorStrings[227]);
	}

	if (typevar == ckifUString) {
		wstring l = kfirst->UString();
		wstring r = ksecond->UString();
		if (l.size() != 1 || r.size() != 1)
			return callfunc->kifcode->Returnerror(kifErrorStrings[230]);
		long inc = kinc->Integer();
		if (inc == 0)
			return callfunc->kifcode->Returnerror(kifErrorStrings[225]);
		char cl = l[0];
		char cr = r[0];
		int d = cr - cl;
		if (d<0)
			d *= -1;

		if (cl>cr && inc > 0)
			inc *= -1;

		kvect = Selectvectorustring(callfunc, contextualpattern);
		kvect->Reserve(d);
		if (inc > 0) {
			for (uchar i = cl; i <= cr; i += inc) {
				l = (char)i;
				kvect->storevalue(l);
			}
		}
		else {
			for (uchar i = cl; i >= cr; i += inc) {
				l = (char)i;
				kvect->storevalue(l);
			}
		}
		return kvect;
	}

	string l = kfirst->String();
	string r = ksecond->String();
	if (l.size() != 1 || r.size() != 1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[230]);
	long inc = kinc->Integer();
	if (inc == 0)
		return callfunc->kifcode->Returnerror(kifErrorStrings[225]);
	char cl = l[0];
	char cr = r[0];
	int d = cr - cl;
	if (d<0)
		d *= -1;

	if (cl>cr && inc > 0)
		inc *= -1;

	kvect = Selectvectorstring(callfunc, contextualpattern);
	kvect->Reserve(d);
	if (inc > 0) {
		for (uchar i = cl; i <= cr; i += inc) {
			l = (char)i;
			kvect->storevalue(l);
		}
	}
	else {
		for (uchar i = cl; i >= cr; i += inc) {
			l = (char)i;
			kvect->storevalue(l);
		}
	}
	return kvect;
}

//
KifElement* PrePushFirst(KifElement* kf, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->type == kifList) {
		//next parameters are anything
		KifElement* res;
		for (size_t i = 1; i < callfunc->Size(); i++) {
			res = callfunc->Evaluatethread(i, dom, idthread);
			if (res == kifNOELEMENT)
				continue;
			if (res == kifRAISEERROR)
				return res;
			((KifList*)kf)->PushFirst(res);
		}
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[39]);
}

KifElement* PreMerge(KifElement* kf, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf == kifNOELEMENT)
		return kifFALSE;
	int sz = callfunc->Size();
	for (int i = 1; i < sz; i++)
		kf->Merge(callfunc->Evaluatethread(i, dom, idthread));
	return kf;
}

KifElement* PrePredicate(KifElement* kf, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	kf = callfunc->Evaluatethread(0, dom, idthread);
	return kf->Predicate(dom);
}

KifElement* PreStore(KifElement* kf, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->type == kifTransducer) {
		if (callfunc->Size() == 1)
			return callfunc->kifcode->Returnerror("XTR(011): Missing parameters");
		
		if (((KifTransducer*)kf)->automaton == NULL)
			return kifFALSE;

		string name = callfunc->Evaluatethread(1, dom, idthread)->String();
		if (callfunc->Size() >= 3) {
			((KifTransducer*)kf)->automaton->normalize = callfunc->Evaluatethread(2, dom, idthread)->Boolean();
			if (callfunc->Size() == 4)
				((KifTransducer*)kf)->automaton->encoding_table = callfunc->Evaluatethread(3, dom, idthread)->Integer();
		}

		if (((KifTransducer*)kf)->automaton->store(name))
			return kifTRUE;

		return kifFALSE;
	}

	if (kf->type == kifAutomaton) {
		if (callfunc->Size() == 1)
			return callfunc->kifcode->Returnerror("XTR(011): Missing parameters");
		KifElement* ke = callfunc->Evaluatethread(1, dom, idthread);
		return ((KifAutomaton*)kf)->Store(ke, dom);
	}

	if (callfunc->Size() == 1)
		return kf->Store(NULL, dom, idthread);
	KifElement* db = callfunc->Evaluatethread(1, dom, idthread);
	return kf->Store(db, dom, idthread);
}

KifElement* PreRemove(KifElement* kf, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	kf = callfunc->Evaluatethread(0, dom, idthread);
	if (callfunc->Size() == 1)
		return kf->Remove(NULL, dom, idthread);

	KifElement* db = callfunc->Evaluatethread(0, dom, idthread);
	return kf->Remove(db, dom, idthread);
}

KifElement* PrePushLast(KifElement* kf, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->type == kifList) {
		//next parameters are anything
		KifElement* res;
		for (size_t i = 1; i < callfunc->Size(); i++) {
			res = callfunc->Evaluatethread(i, dom, idthread);
			if (res == kifNOELEMENT)
				continue;
			if (res == kifRAISEERROR)
				return res;
			((KifList*)kf)->PushLast(res);
		}
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[39]);
}

KifElement* PrePopFirst(KifElement* kf, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->type == kifList) {
		((KifList*)kf)->PopFirst();
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[41]);
}



KifElement* PrePopLast(KifElement* kf, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->type == kifList) {
		((KifList*)kf)->PopLast();
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[41]);
}

KifElement* PreFirst(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	return callfunc->Evaluatethread(0, dom, idthread)->First();
}


KifElement* PreGetc(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//Reading of one character
	int c = GETCH(false);
	if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
		return callfunc->kifcode->Provideinteger(c);
	string s;
	s = (uchar)c;
	return callfunc->kifcode->Providestring(s);
}


KifElement* PreComplement(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kv = callfunc->Evaluatethread(0, dom, idthread);
	if (kv->type == kifBit)
		return ((KifBit*)kv)->Complement();
	else
	if (kv->type == kifBits)
		return ((KifBits*)kv)->Complement();
	else
	if (kv->type == kifBoolean) {
		if (kv->Boolean() == false)
			return kifTRUE;
		return kifFALSE;
	}
	long v = kv->Integer();
	v = ~v;
	return callfunc->kifcode->Provideinteger(v);
}


KifElement* PreEvaluateMeta(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	static string reg("[\\\\[%d%d%d,x%x%x,u%X%X%X%X,?],&[%#%d(%d)(%d)(%d);,[%c,%C]+;],%%%X%X,u%X%X%X%X]");
	static vector<int> vectr;
	static automate metacodes(USTR(reg), (void*)1);

	string sub = callfunc->Evaluatethread(0, dom, idthread)->String();

	metacodes.rechercheindexes(USTR(sub), vectr);

	if (vectr.size() == 0)
		return callfunc->kifcode->Providestring(sub);

	size_t i = 0, previous = 0;
	string thestr;
	string s;
	for (i = 0; i < vectr.size(); i += 2) {
		if (previous < vectr[i])
			thestr += sub.substr(previous, vectr[i] - previous);
		previous = vectr[i] + vectr[i + 1];
		s = sub.substr(vectr[i], vectr[i + 1]);
		s_EvaluateMetaCharacters(s);
		thestr += s;
	}

	i = sub.size() - previous;
	if (i) {
		s = sub.substr(previous, i);
		thestr += s;
	}

	return callfunc->kifcode->Providestring(thestr);
}

KifElement* PreXipReplaceRgx(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//Replace an element with another

	string str = callfunc->Evaluatethread(0, dom, idthread)->String();
	string reg = callfunc->Evaluatethread(1, dom, idthread)->String();
	string rep = callfunc->Evaluatethread(2, dom, idthread)->String();

	string res = replacestringwithautomaton(str, reg, rep);
	return callfunc->kifcode->Providestring(res);
}


void XNBrowse(x_node* xn, KifVector* kf) {
	if (xn == NULL)
		return;
	KifPrimeMap* kmap = new KifPrimeMap(kf->kifcode, NULL);
	kf->Push(kmap);

	if (xn->nodes.size()) {
		string key = xn->token;
		if (xn->value != "")
			key += "_" + xn->value;
		KifVector* kvect = new KifVector(kf->kifcode, NULL);
		kmap->Push(key, kvect);
		for (size_t i = 0; i < xn->nodes.size(); i++) {
			XNBrowse(xn->nodes[i], kvect);
		}
	}
	else
		kmap->Push(xn->token, kf->kifcode->Providestring(xn->value));
}

KifElement* PreParse(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	string str = callfunc->Evaluatethread(0, dom, idthread)->String();
	x_readstring xr(str);
	xr.loadtoken();
	x_reading* previous = kifGlobals->bnfkif->fx;
	x_node* xn = kifGlobals->bnfkif->x_parsing(&xr, FULL, false);
	string message;
	if (xn == NULL) {
		if (kifGlobals->bnfkif->errornumber != -1) {
			string msg("EVL(100):");
			msg += kifGlobals->bnfkif->x_errormsg(kifGlobals->bnfkif->errornumber);
			DebugPtr()->currentline = kifGlobals->bnfkif->lineerror;
			char ch[20];
			sprintf_s(ch, 20, " (line:%d)", kifGlobals->bnfkif->lineerror);
			msg += ch;
			return callfunc->kifcode->Returnerror(msg);
		}
		return callfunc->kifcode->Returnerror(kifGlobals->GetThreadid(), kifErrorStrings[244]);
	}

	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	XNBrowse(xn, kvect);
	return kvect;
}



KifElement* PreLisp(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	string str = callfunc->Evaluatethread(0, dom, idthread)->String();
	string copen("(");
	string cclose(")");
	if (callfunc->Size() == 1)
		return callfunc->kifcode->EvaluateLisp(dom, str, copen, cclose);

	copen = callfunc->Evaluatethread(1, dom, idthread)->String();
	cclose = callfunc->Evaluatethread(2, dom, idthread)->String();

	if (copen.size() != 1 || cclose.size() != 1)
		return callfunc->kifcode->Returnerror("KIF(831): Wrong opening or closing character in LISP");
	return callfunc->kifcode->EvaluateLisp(dom, str, copen, cclose);
}

KifElement* PreTags(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	agnostring str(callfunc->Evaluatethread(0, dom, idthread)->String());
	string copen = callfunc->Evaluatethread(1, dom, idthread)->String();
	string cclose = callfunc->Evaluatethread(2, dom, idthread)->String();
	return callfunc->kifcode->EvaluateTags(dom, str, copen, cclose);
}


void convertbytepositiontochar(vector<long>& v, agnostring& s) {
	vector<long> vres;
	int i = 0;
	int r = 0;
	for (int j = 0; j < v.size(); j++) {
		c_bytetocharpositionidx(USTR(s), v[j], r, i);
		vres.push_back(r);
	}
	v = vres;
}

void convertbytepositiontochar(vector<double>& v, agnostring& s) {
	vector<double> vres;
	int i = 0;
	int r = 0;
	for (int j = 0; j < v.size(); j++) {
		c_bytetocharpositionidx(USTR(s), v[j], r, i);
		vres.push_back(r);
	}
	v = vres;
}


KifElement* PreXipRegex(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	agnostring str(callfunc->Evaluatethread(0, dom, idthread)->String());
	bool utf8 = str.isutf8();
	string reg = callfunc->Evaluatethread(1, dom, idthread)->String();
	automate a(USTR(reg), (void*)1);
	if (contextualpattern->aVectorContainer()) {
		KifElement* vect;

		vect = Selectavector(callfunc, contextualpattern);

		switch (vect->type) {
		case kifVectorInteger:
			a.rechercheindexes(USTR(str), ((KifVectorInteger*)vect)->values);
			if (utf8)
				convertbytepositiontochar(((KifVectorInteger*)vect)->values, str);
			return vect;
		case kifVectorFloat:
			a.rechercheindexes(USTR(str), ((KifVectorFloat*)vect)->values);
			if (utf8)
				convertbytepositiontochar(((KifVectorFloat*)vect)->values, str);
			return vect;
		case kifVectorString:
			a.rechercheavecextractiondansvecteur(USTR(str), ((KifVectorString*)vect)->values);
			return vect;
		}

		if (vect->type == kifVectorByte) {
			vector<long> vectr;
			a.rechercheindexes(USTR(str), vectr);
			if (utf8)
				convertbytepositiontochar(vectr, str);
			for (int i = 0; i < vectr.size(); i++)
				((KifVectorByte*)vect)->values.push_back((uchar)vectr[i]);
			return vect;
		}

		vector<string> vectr;
		a.rechercheavecextractiondansvecteur(USTR(str), vectr);
		KifBaseString localstring(NULL, NULL);
		localstring.kifcode = callfunc->kifcode;
		for (int iv = 0; iv < vectr.size(); iv++) {
			localstring.value = vectr[iv];
			vect->Push(&localstring);
		}
		return vect;
	}

	if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat || contextualpattern->type == kifString) {
		reg = "";
		int res = a.rechercheavecextraction(USTR(str), reg);
		if (contextualpattern->type == kifString)
			return callfunc->kifcode->Providestring(reg);
		return callfunc->kifcode->Provideinteger(res);
	}

	etat* e = a.recherche(USTR(str));
	if (e == NULL)
		return kifFALSE;
	return kifTRUE;
}


//test(value);
KifElement* PreTest(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//check an index value for a container
	return callfunc->Evaluatethread(0, dom, idthread)->Index(callfunc->Evaluatethread(1, dom, idthread));
}


KifElement* PreCount(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	int i = 0, nb = 0;

	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifTable) {
		KifTable* ktab = (KifTable*)kstr;
		for (i = 0; i < ktab->size; i++) {
			if (ktab->values[i] != kifNULL)
				nb++;
		}
		return callfunc->kifcode->Provideinteger(nb);
	}

	int mx = -1;
	if (callfunc->Size() >= 3) {
		i = callfunc->Evaluatethread(2, dom, idthread)->Integer();
		if (callfunc->Size() == 4)
			mx = callfunc->Evaluatethread(3, dom, idthread)->Integer();
	}

	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		if (str == L"")
			return kifZERO;
		wstring sub = callfunc->Evaluatethread(1, dom, idthread)->UString();

		i = str.find(sub, i);
		while (i != -1) {
			if (mx != -1 && i >= mx)
				break;
			nb++;
			i++;
			i = str.find(sub, i);
		}

		return callfunc->kifcode->Provideinteger(nb);
	}

	agnostring str(kstr->String());
	if (str == "")
		return kifZERO;

	string sub = callfunc->Evaluatethread(1, dom, idthread)->String();
	i = str.find(sub, i);
	while (i != -1) {
		if (mx != -1 && i >= mx)
			break;
		nb++;
		i++;
		i = str.find(sub, i);
	}

	return callfunc->kifcode->Provideinteger(nb);
}

KifElement* Searchinfile(KifBaseFile* kfile, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	agnostring s(callfunc->Evaluatethread(1, dom, idthread)->String());
	bool u = false;
	if (callfunc->Size() == 3)
		u = callfunc->Evaluatethread(2, dom, idthread)->Boolean();
	return kfile->Find(contextualpattern, s, u);
}


//find(s,i);
KifElement* PreFind(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kbasecall = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* ksub = callfunc->Evaluatethread(1, dom, idthread);
	if (kbasecall->isfile())
		return Searchinfile((KifBaseFile*)kbasecall, contextualpattern, dom, callfunc, idthread);

	//we search for a substring starting at position idx
	int i = 0;
	if (callfunc->Size() == 3)
		i = callfunc->Evaluatethread(2, dom, idthread)->Integer();

	if (contextualpattern->aVectorContainer()) {
		KifElement* kvect = Selectavector(callfunc, contextualpattern);
		int j;
		if (kbasecall->type == kifUString) {
			wstring str = kbasecall->UString();
			if (str == L"")
				return kvect;
			wstring sub = ksub->UString();
			vector<long> v;
			s_findall(str, sub, v);
			if (v.size()) {
				KifContainerLock _lock((KifEvaluate*)kvect);

				switch (kvect->type) {
				case kifVectorByte:
					for (j = 0; j < v.size(); j++) {
						if (v[j] >= i)
							((KifVectorByte*)kvect)->values.push_back((uchar)v[j]);
					}
					return kvect;
				case kifVectorInteger:
					for (j = 0; j < v.size(); j++) {
						if (v[j] >= i)
							((KifVectorInteger*)kvect)->values.push_back((uchar)v[j]);
					}
					return kvect;
				case kifVectorFloat:
					for (j = 0; j < v.size(); j++) {
						if (v[j] >= i)
							((KifVectorFloat*)kvect)->values.push_back((double)v[j]);
					}
					return kvect;
				}

				KifBaseInteger loc(NULL, NULL);
				loc.kifcode = callfunc->kifcode;
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i) {
						loc.value = v[j];
						kvect->Push(&loc);
					}
				}
			}
			return kvect;
		}

		string str = kbasecall->String();
		if (str == "")
			return kvect;

		string sub = ksub->String();
		vector<long> v;
		s_findall(str, sub, v);

		if (kbasecall->type == kifString) {
			int ps = 0, nb = 0;
			for (j = 0; j < v.size(); j++)
				v[j] = c_bytetocharpositionidx(USTR(str), v[j], nb, ps);
		}

		if (v.size()) {
			KifContainerLock _lock((KifEvaluate*)kvect);

			switch (kvect->type) {
			case kifVectorByte:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((KifVectorByte*)kvect)->values.push_back((uchar)v[j]);
				}
				return kvect;
			case kifVectorInteger:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((KifVectorInteger*)kvect)->values.push_back((uchar)v[j]);
				}
				return kvect;
			case kifVectorFloat:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((KifVectorFloat*)kvect)->values.push_back((double)v[j]);
				}
				return kvect;
			}

			KifBaseInteger loc(NULL, NULL);
			loc.kifcode = callfunc->kifcode;
			for (j = 0; j < v.size(); j++) {
				if (v[j] >= i) {
					loc.value = v[j];
					kvect->Push(&loc);
				}
			}
		}

		return kvect;
	}

	if (kbasecall->type == kifUString) {
		wstring str = kbasecall->UString();
		wstring sub = ksub->UString();
		i = s_find(str, sub, i);		
		if (i == wstring::npos)
			return kifMINUSONE;
		return callfunc->kifcode->Provideinteger(i);
	}

	string str = kbasecall->String();
	string sub = ksub->String();

	if (kbasecall->type == kifRawString)
		i = s_findbyte(str, sub, i);
	else
		i = s_find(str, sub, i);

	if (i == string::npos)
		return kifMINUSONE;

	return callfunc->kifcode->Provideinteger(i);
}

KifElement* PreRFind(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kbasecall = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* ksub = callfunc->Evaluatethread(1, dom, idthread);
	if (kbasecall->isfile())
		return Searchinfile((KifBaseFile*)kbasecall, contextualpattern, dom, callfunc, idthread);

	//we search for a substring starting at position idx
	int i = 0;	
	if (callfunc->Size() == 3)
		i = callfunc->Evaluatethread(2, dom, idthread)->Integer();

	if (contextualpattern->aVectorContainer()) {
		KifElement* kvect = Selectavector(callfunc, contextualpattern);
		int j;
		if (kbasecall->type == kifUString) {
			wstring str = kbasecall->UString();
			if (str == L"")
				return kvect;
			wstring sub = ksub->UString();
			vector<long> v;
			s_findall(str, sub, v);
			if (v.size()) {
				KifContainerLock _lock((KifEvaluate*)kvect);

				switch (kvect->type) {
				case kifVectorByte:
					for (j = 0; j < v.size(); j++) {
						if (v[j] >= i)
							((KifVectorByte*)kvect)->values.push_back((uchar)v[j]);
					}
					return kvect;
				case kifVectorInteger:
					for (j = 0; j < v.size(); j++) {
						if (v[j] >= i)
							((KifVectorInteger*)kvect)->values.push_back((uchar)v[j]);
					}
					return kvect;
				case kifVectorFloat:
					for (j = 0; j < v.size(); j++) {
						if (v[j] >= i)
							((KifVectorFloat*)kvect)->values.push_back((double)v[j]);
					}
					return kvect;
				}

				KifBaseInteger loc(NULL, NULL);
				loc.kifcode = callfunc->kifcode;
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i) {
						loc.value = v[j];
						kvect->Push(&loc);
					}
				}
			}
			return kvect;
		}

		string str = kbasecall->String();
		if (str == "")
			return kvect;

		string sub = ksub->String();
		vector<long> v;
		s_findall(str, sub, v);

		if (kbasecall->type == kifString) {
			int ps = 0, nb = 0;
			for (j = 0; j < v.size(); j++)
				v[j] = c_bytetocharpositionidx(USTR(str), v[j], nb, ps);
		}

		if (v.size()) {
			KifContainerLock _lock((KifEvaluate*)kvect);

			switch (kvect->type) {
			case kifVectorByte:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((KifVectorByte*)kvect)->values.push_back((uchar)v[j]);
				}
				return kvect;
			case kifVectorInteger:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((KifVectorInteger*)kvect)->values.push_back((uchar)v[j]);
				}
				return kvect;
			case kifVectorFloat:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((KifVectorFloat*)kvect)->values.push_back((double)v[j]);
				}
				return kvect;
			}

			KifBaseInteger loc(NULL, NULL);
			loc.kifcode = callfunc->kifcode;
			for (j = 0; j < v.size(); j++) {
				if (v[j] >= i) {
					loc.value = v[j];
					kvect->Push(&loc);
				}
			}
		}

		return kvect;
	}

	if (kbasecall->type == kifUString) {
		wstring str = kbasecall->UString();
		wstring sub = ksub->UString();
		i = s_rfind(str, sub, i);
		if (i == wstring::npos)
			return kifMINUSONE;
		return callfunc->kifcode->Provideinteger(i);
	}

	string str = kbasecall->String();
	string sub = ksub->String();
	if (kbasecall->type==kifRawString)
		i = s_rfindbyte(str, sub, i);
	else
		i = s_rfind(str, sub, i);

	if (i == string::npos)
		return kifMINUSONE;

	return callfunc->kifcode->Provideinteger(i);
}

//From a character to a byte position
KifElement* PreBytePosition(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	agnostring s(callfunc->Evaluatethread(0, dom, idthread)->String());
	return callfunc->kifcode->Provideinteger(s.chartobyteposition(callfunc->Evaluatethread(1, dom, idthread)->Integer()));
}

//From a byte position to a character
KifElement* PreCharacterPosition(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	agnostring s(callfunc->Evaluatethread(0, dom, idthread)->String());
	return callfunc->kifcode->Provideinteger(s.bytetocharposition(callfunc->Evaluatethread(1, dom, idthread)->Integer()));
}

KifElement* PreTokenize(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);

	if (kstr->type == kifUString) {
		wstring thestr = kstr->UString();
		short flag = 0;
		bool comma = false;
		bool separator = false;
		bool keepwithdigit = false;
		if (callfunc->Size() >= 2) {
			comma = callfunc->Evaluatethread(1, dom, idthread)->Boolean();
			if (comma)
				flag |= token_comma;
			if (callfunc->Size() >= 3) {
				separator = callfunc->Evaluatethread(2, dom, idthread)->Boolean();
				if (separator)
					flag |= token_separator;
				if (callfunc->Size() == 4) {
					keepwithdigit = callfunc->Evaluatethread(3, dom, idthread)->Boolean();
					if (keepwithdigit)
						flag |= token_keepwithdigit;
				}
			}
		}

		vector<wstring> v;
		vw_tokenize(v, thestr, flag);

		KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
		if (kvect->type == kifVectorUString) {
			KifContainerLock _lock((KifEvaluate*)kvect);
			((KifVectorUString*)kvect)->values = v;
		}
		else {
			KifBaseUString localstring(NULL, NULL, 0);
			localstring.kifcode = callfunc->kifcode;

			for (int i = 0; i < v.size(); i++) {
				localstring.value = v[i];
				kvect->Push(&localstring);
			}
		}
		return kvect;
	}

	agnostring thestr(kstr->String());
	bool comma = false;
	bool separator = false;
	bool keepwithdigit = false;
	if (callfunc->Size() >= 2) {
		comma = callfunc->Evaluatethread(1, dom, idthread)->Boolean();
		if (callfunc->Size() >= 3) {
			separator = callfunc->Evaluatethread(2, dom, idthread)->Boolean();
			if (callfunc->Size() == 4)
				keepwithdigit = callfunc->Evaluatethread(3, dom, idthread)->Boolean();
		}
	}

	vector<string> v;
	thestr.tokenize(v, comma, separator, keepwithdigit);

	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	if (kvect->type == kifVectorString) {
		KifContainerLock _lock((KifEvaluate*)kvect);
		((KifVectorString*)kvect)->values = v;
	}
	else {
		KifBaseString localstring(NULL, NULL);
		localstring.kifcode = callfunc->kifcode;

		for (int i = 0; i < v.size(); i++) {
			localstring.value = v[i];
			kvect->Push(&localstring);
		}
	}
	return kvect;
}

KifElement* PreSTokenize(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring thestr = kstr->UString();
		KifElement* keeps = NULL;

		if (callfunc->Size() == 2) {
			keeps = callfunc->Evaluatethread(1, dom, idthread);
			if (!keeps->aContainer())
				return callfunc->kifcode->Returnerror("KIF(665): Expecting a container in STOKENIZE");
		}

		vector<wstring> v;
		map<wstring, bool> k;
		if (keeps != NULL) {
			KifIteration* itr = (KifIteration*)keeps->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				k[itr->IteratorValueUString()] = true;
			itr->Release();
		}
		s_tokenize(v, thestr, k);

		KifElement* kvect = Selectvectorustring(callfunc, contextualpattern);
		if (kvect->type == kifVectorUString) {
			KifContainerLock _lock((KifEvaluate*)kvect);
			((KifVectorUString*)kvect)->values = v;
		}
		else {
			KifBaseUString localstring(NULL, NULL, 0);
			localstring.kifcode = callfunc->kifcode;

			for (int i = 0; i < v.size(); i++) {
				localstring.value = v[i];
				kvect->Push(&localstring);
			}
		}
		return kvect;
	}

	agnostring thestr(kstr->String());
	KifElement* keeps = NULL;

	if (callfunc->Size() == 2) {
		keeps = callfunc->Evaluatethread(1, dom, idthread);
		if (!keeps->aContainer())
			return callfunc->kifcode->Returnerror("KIF(665): Expecting a container in STOKENIZE");
	}

	vector<string> v;
	map<string, bool> k;
	if (keeps != NULL) {
		KifIteration* itr = (KifIteration*)keeps->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
			k[itr->IteratorValueString()] = true;
		itr->Release();
	}
	thestr.stokenize(v, k);

	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	if (kvect->type == kifVectorString) {
		KifContainerLock _lock((KifEvaluate*)kvect);
		((KifVectorString*)kvect)->values = v;
	}
	else {
		KifBaseString localstring(NULL, NULL);
		localstring.kifcode = callfunc->kifcode;

		for (int i = 0; i < v.size(); i++) {
			localstring.value = v[i];
			kvect->Push(&localstring);
		}
	}
	return kvect;
}

KifElement* PreSplit(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//Third parameter can be a vector

	KifElement* kvect = NULL;
	KifElement* khead = callfunc->Evaluatethread(0, dom, idthread);
	if (khead->type == kifUString) {
		wstring localvalue;
		wstring thestr = khead->UString();
		wstring thesplitter;
		if (callfunc->Size() != 1)
			thesplitter = callfunc->Evaluatethread(1, dom, idthread)->UString();

		//First parameter is a string to split
		if (callfunc->Size() == 3)
			kvect = callfunc->Evaluatethread(2, dom, idthread);
		else {
			if (contextualpattern->type == kifVectorUString || !contextualpattern->aVectorContainer())
				kvect = Selectvectorustring(callfunc, contextualpattern);
			else {
				kvect = Selectacontainer(callfunc, contextualpattern);
			}
		}

		if (callfunc->Size() == 1) {
			wchar_t c;
			//we split along space characters...
			for (int i = 0; i < thestr.size(); i++) {
				c = thestr[i];
				if (c <= 32) {
					if (thesplitter.size()) {
						kvect->storevalue(thesplitter);
						thesplitter = L"";
					}
				}
				else {
					thesplitter += thestr[i];
				}
			}

			if (thesplitter.size())
				kvect->storevalue(thesplitter);
			return kvect;
		}


		//Second parameter is the splitter string
		if (thesplitter == L"") {
			if (kvect->type == kifVectorByte) {
				for (int i = 0; i < thestr.size(); i++)
					kvect->storevalue((long)thestr[i]);
				return kvect;
			}

			if (kvect->type == kifVectorInteger) {
				for (int i = 0; i < thestr.size(); i++)
					kvect->storevalue((long)thestr[i]);
				return kvect;
			}

			for (int i = 0; i < thestr.size(); i++)
				kvect->storevalue(thestr[i]);

			return kvect;
		}

		size_t pos = 0;
		size_t found = 0;
		while (pos != string::npos) {
			found = s_find(thestr, thesplitter, pos);
			if (found != string::npos) {
				localvalue = thestr.substr(pos, found - pos);
				if (localvalue != L"")
					kvect->storevalue(localvalue);
				pos = found + thesplitter.size();
			}
			else
				break;
		}

		localvalue = thestr.substr(pos, thestr.size() - pos);
		if (localvalue != L"")
			kvect->storevalue(localvalue);

		return kvect;
	}

	string localvalue;
	string thestr = khead->String();
	string thesplitter;
	if (callfunc->Size() != 1)
		thesplitter = callfunc->Evaluatethread(1, dom, idthread)->String();

	//First parameter is a string to split
	if (callfunc->Size() == 3)
		kvect = callfunc->Evaluatethread(2, dom, idthread);
	else {
		if (contextualpattern->type == kifVectorString || !contextualpattern->aVectorContainer())
			kvect = Selectvectorstring(callfunc, contextualpattern);
		else {
			kvect = Selectacontainer(callfunc, contextualpattern);
		}
	}

	if (callfunc->Size() == 1) {
		unsigned char c;
		//we split along space characters...
		for (int i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (c <= 32) {
				if (thesplitter.size()) {
					kvect->storevalue(thesplitter);
					thesplitter = "";
				}
			}
			else {
				thesplitter += thestr[i];
			}
		}

		if (thesplitter.size())
			kvect->storevalue(thesplitter);
		return kvect;
	}


	//Second parameter is the splitter string
	if (thesplitter == "") {
		if (kvect->type == kifVectorByte) {
			for (int i = 0; i < thestr.size(); i++)
				kvect->storevalue((long)thestr[i]);
			return kvect;
		}

		agnostring athestr(thestr);
		athestr.begin();
		if (kvect->type == kifVectorInteger) {
			while (!athestr.end())
				kvect->storevalue((long)athestr.nextcode());
			return kvect;
		}

		while (!athestr.end())
			kvect->storevalue(athestr.next());
		return kvect;
	}

	size_t pos = 0;
	size_t found = 0;
	while (pos != string::npos) {
		found = s_findbyte(thestr, thesplitter, pos);
		if (found != string::npos) {
			localvalue = thestr.substr(pos, found - pos);
			if (localvalue != "")
				kvect->storevalue(localvalue);
			pos = found + thesplitter.size();
		}
		else
			break;
	}

	localvalue = thestr.substr(pos, thestr.size() - pos);
	if (localvalue != "")
		kvect->storevalue(localvalue);

	return kvect;
}

KifElement* PreSplitKeepEmptyStrings(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//Third parameter can be a vector

	KifElement* kvect = NULL;
	KifElement* khead = callfunc->Evaluatethread(0, dom, idthread);
	if (khead->type == kifUString) {
		wstring localvalue;
		wstring thestr = khead->UString();
		wstring thesplitter;
		if (callfunc->Size() != 1)
			thesplitter = callfunc->Evaluatethread(1, dom, idthread)->UString();

		//First parameter is a string to split
		if (callfunc->Size() == 3)
			kvect = callfunc->Evaluatethread(2, dom, idthread);
		else {
			if (contextualpattern->type == kifVectorUString || !contextualpattern->aVectorContainer())
				kvect = Selectvectorustring(callfunc, contextualpattern);
			else {
				kvect = Selectacontainer(callfunc, contextualpattern);
			}
		}

		if (callfunc->Size() == 1) {
			wchar_t c;
			//we split along space characters...
			for (int i = 0; i < thestr.size(); i++) {
				c = thestr[i];
				if (c <= 32) {
					kvect->storevalue(thesplitter);
					thesplitter = L"";
				}
				else {
					thesplitter += thestr[i];
				}
			}

			kvect->storevalue(thesplitter);
			return kvect;
		}


		//Second parameter is the splitter string
		if (thesplitter == L"") {
			if (kvect->type == kifVectorByte) {
				for (int i = 0; i < thestr.size(); i++)
					kvect->storevalue((long)thestr[i]);
				return kvect;
			}

			if (kvect->type == kifVectorInteger) {
				for (int i = 0; i < thestr.size(); i++)
					kvect->storevalue((long)thestr[i]);
				return kvect;
			}

			for (int i = 0; i < thestr.size(); i++)
				kvect->storevalue(thestr[i]);

			return kvect;
		}

		size_t pos = 0;
		size_t found = 0;
		while (pos != string::npos) {
			found = s_find(thestr, thesplitter, pos);
			if (found != string::npos) {
				localvalue = thestr.substr(pos, found - pos);
				kvect->storevalue(localvalue);
				pos = found + thesplitter.size();
			}
			else
				break;
		}

		localvalue = thestr.substr(pos, thestr.size() - pos);
		if (localvalue != L"")
			kvect->storevalue(localvalue);

		return kvect;
	}

	string localvalue;
	string thestr = khead->String();
	string thesplitter;
	if (callfunc->Size() != 1)
		thesplitter = callfunc->Evaluatethread(1, dom, idthread)->String();

	//First parameter is a string to split
	if (callfunc->Size() == 3)
		kvect = callfunc->Evaluatethread(2, dom, idthread);
	else {
		if (contextualpattern->type == kifVectorString || !contextualpattern->aVectorContainer())
			kvect = Selectvectorstring(callfunc, contextualpattern);
		else {
			kvect = Selectacontainer(callfunc, contextualpattern);
		}
	}

	if (callfunc->Size() == 1) {
		unsigned char c;
		//we split along space characters...
		for (int i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (c <= 32) {
				if (thesplitter.size()) {
					kvect->storevalue(thesplitter);
					thesplitter = "";
				}
			}
			else {
				thesplitter += thestr[i];
			}
		}

		if (thesplitter.size())
			kvect->storevalue(thesplitter);
		return kvect;
	}


	//Second parameter is the splitter string
	if (thesplitter == "") {
		if (kvect->type == kifVectorByte) {
			for (int i = 0; i < thestr.size(); i++)
				kvect->storevalue((long)thestr[i]);
			return kvect;
		}

		agnostring athestr(thestr);
		athestr.begin();
		if (kvect->type == kifVectorInteger) {
			while (!athestr.end())
				kvect->storevalue((long)athestr.nextcode());
			return kvect;
		}

		while (!athestr.end())
			kvect->storevalue(athestr.next());
		return kvect;
	}

	size_t pos = 0;
	size_t found = 0;
	while (pos != string::npos) {
		found = s_findbyte(thestr,thesplitter, pos);
		if (found != string::npos) {
			localvalue = thestr.substr(pos, found - pos);
			kvect->storevalue(localvalue);
			pos = found + thesplitter.size();
		}
		else
			break;
	}

	localvalue = thestr.substr(pos, thestr.size() - pos);
	if (localvalue != "")
		kvect->storevalue(localvalue);

	return kvect;
}

KifElement* PreNGRAMS(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	int num = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	if (num <= 0)
		return Selectvectorstring(callfunc, contextualpattern);

	int i, j, mx;
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	if (ke->aVectorContainer()) {
		string u;
		string sep;
		if (callfunc->Size() == 3)
			sep = callfunc->Evaluatethread(2, dom, idthread)->String();

		KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
		mx = ke->Size() - num + 1;
		for (i = 0; i < mx; i++) {
			u = "";
			for (j = i; j < i + num; j++) {
				if (j != i)
					u += sep;
				u += ke->getstring(j);
			}
			kvect->storevalue(u);
		}
		return kvect;
	}

	if (ke->type == kifUString) {
		wstring  s = ke->UString();
		wstring u;
		ke = Selectvectorustring(callfunc, contextualpattern);

		mx = s.size() - num + 1;
		for (i = 0; i < mx; i++) {
			u = L"";
			for (j = i; j < i + num; j++)
				u += s[j];
			ke->storevalue(u);
		}
		return ke;
	}

	agnostring s(ke->String());
	ke = Selectvectorstring(callfunc, contextualpattern);
	string u;

	if (s.isutf8()) {
		vector<string> v;
		s.begin();
		while (!s.end())
			v.push_back(s.next());
		mx = v.size() - num + 1;
		for (i = 0; i < mx; i++) {
			u = "";
			for (j = i; j < i + num; j++)
				u += v[j];
			ke->storevalue(u);
		}
		return ke;
	}
	string& sb = (string&)s;
	mx = s.size() - num + 1;
	for (i = 0; i < mx; i++) {
		u = "";
		for (j = i; j < i + num; j++)
			u += sb[j];
		ke->storevalue(u);
	}
	return ke;
}

KifElement* PreHash(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	static const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	string s = ke->String();
	if (s == "")
		return kifNULL;
	long myhash = coll.hash(s.data(), s.data() + s.length());
	return callfunc->kifcode->Provideinteger(myhash);
}

KifElement* PreTrigger(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ktab = callfunc->Evaluatethread(0, dom, idthread);
	KifTable* ktable = (KifTable*)ktab;
	ktable->functionloop = callfunc->Evaluatethread(1, dom, idthread)->Boolean();
	hmap<short, KifCallFunctionThread*> threads = kifGlobals->triggerfunctions[ktable->idgarbage];

	ktable->lock->Released();

	//we need each thread to have terminated...
	while (true) {
		ThreadLock _lock(kifTable);
		if (kifGlobals->triggerfunctions[ktable->idgarbage].size() == 0)
			break;
	}

	hmap<short, KifCallFunctionThread*>::iterator it;
	if (!ktable->functionloop) {
		ThreadLock _lock(kifTable);
		for (it = threads.begin(); it != threads.end(); it++)
			it->second->Forcecleanreference();
		kifGlobals->triggerfunctions.erase(ktable->idgarbage);
		kifGlobals->triggertable.erase(ktable->idgarbage);
	}
	else {
		//we reinject them
		{
			ThreadLock _lock(kifTable);
			kifGlobals->triggerfunctions[ktable->idgarbage] = threads;
		}
		//and we relaunch them
		for (it = threads.begin(); it != threads.end(); it++)
			it->second->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	return kifTRUE;
}


KifElement* PreWaitOn(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ktab = callfunc->Evaluatethread(0, dom, idthread);
	KifTable* ktable = (KifTable*)ktab;

	if (ktable->lock == NULL)
		ktable->lock = new LockedThread;

	ThreadLock _lock(kifTable);

	if (kifGlobals->triggerfunctions.find(ktable->idgarbage) != kifGlobals->triggerfunctions.end())
		return callfunc->kifcode->Returnerror("KIF(550): The thread list has already been implemented.");

	KifElement* kfunc;
	KifCallFunctionThread* kcf;
	vector<KifThread*> vthreads;
	vector<KifElement*> vparams;
	vector<KifCallFunctionThread*> threads;
	int i;
	for (i = 1; i < callfunc->Size(); i++) {
		kfunc = callfunc->Evaluatethread(i, dom, idthread);
		if (kfunc->type != kifThread)
			break;
		vthreads.push_back((KifThread*)kfunc);
	}

	if (vthreads.size() == 0)
		return callfunc->kifcode->Returnerror("KIF(551): Expecting threads as arguments.");

	for (; i < callfunc->Size(); i++) {
		kfunc = callfunc->Evaluatethread(i, dom, idthread);
		vparams.push_back(kfunc);
	}

	for (int t = 0; t < vthreads.size(); t++) {
		kfunc = vthreads[t];
		if (kifGlobals->triggerfunctions[ktable->idgarbage].find(kfunc->Name()) != kifGlobals->triggerfunctions[ktable->idgarbage].end()) {
			for (i = 0; i < threads.size(); i++)
				threads[i]->Forcecleanreference();
			kifGlobals->triggerfunctions.erase(ktable->idgarbage);
			string message = "KIF(552): This thread has already been inserted into the thread list";
			message += ": ";
			message += KifIdString(kfunc->Name());
			return callfunc->kifcode->Returnerror(message);
		}

		kcf = new KifCallFunctionThread(callfunc->kifcode, NULL, kfunc->Name(), (KifFunction*)kfunc);
		kcf->parameters.push_back(ktable);
		for (int p = 0; p < vparams.size(); p++)
			kcf->parameters.push_back(vparams[p]);

		kcf->reference = 1;
		kifGlobals->triggertable[kfunc->Name()] = ktable;
		kifGlobals->triggerfunctions[ktable->idgarbage][kfunc->Name()] = kcf;
		threads.push_back(kcf);
	}

	for (i = 0; i < threads.size(); i++)
		threads[i]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

	return kifTRUE;
}

KifElement* PreAddToWaitOn(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ktab = callfunc->Evaluatethread(0, dom, idthread);
	KifTable* ktable = (KifTable*)ktab;

	ThreadLock _lock(kifTable);
	if (ktable->lock == NULL || kifGlobals->triggerfunctions.find(ktable->idgarbage) == kifGlobals->triggerfunctions.end())
		return callfunc->kifcode->Returnerror("KIF(553): The thread list should be implemented with 'waiton' first.");

	KifElement* kfunc;
	//we extract a reference element from the thread list...
	KifCallFunctionThread* kreference = kifGlobals->triggerfunctions[ktable->idgarbage].begin()->second;

	KifCallFunctionThread* kcf;
	vector<KifThread*> vthreads;
	vector<KifCallFunctionThread*> threads;
	int i;
	for (i = 1; i < callfunc->Size(); i++) {
		kfunc = callfunc->Evaluatethread(i, dom, idthread);
		if (kfunc->type != kifThread) {
			//Important: we release the threads waiting to be triggered... 
			ktable->lock->Released();
			return callfunc->kifcode->Returnerror("KIF(551): Expecting threads as arguments.");
		}
		vthreads.push_back((KifThread*)kfunc);
	}

	for (int t = 0; t < vthreads.size(); t++) {
		kfunc = vthreads[t];
		if (kifGlobals->triggerfunctions[ktable->idgarbage].find(kfunc->Name()) != kifGlobals->triggerfunctions[ktable->idgarbage].end()) {
			//Important: we release the threads waiting to be triggered... 
			ktable->lock->Released();
			string message = "KIF(552): This thread has already been inserted into the thread list";
			message += ": ";
			message += KifIdString(kfunc->Name());
			return callfunc->kifcode->Returnerror(message);
		}

		kcf = new KifCallFunctionThread(callfunc->kifcode, NULL, kfunc->Name(), (KifFunction*)kfunc);
		kcf->parameters = kreference->parameters;
		kcf->reference = 1;
		kifGlobals->triggertable[kfunc->Name()] = ktable;
		kifGlobals->triggerfunctions[ktable->idgarbage][kfunc->Name()] = kcf;
		threads.push_back(kcf);
	}

	for (i = 0; i < threads.size(); i++)
		threads[i]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

	return kifTRUE;
}

KifElement* PreSetSeeds(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kvect = callfunc->Evaluatethread(1, dom, idthread);
	if (!kvect->aVectorContainer())
		return callfunc->kifcode->Returnerror("KIF(654): Second parameter should be a vector in 'seeds'");

	unsigned short* p = new unsigned short[kvect->Size() + 2];
	KifIteration* itr = (KifIteration*)kvect->Newiterator(false);
	int i = 0;
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		p[i] = itr->IteratorValueInteger();
		i++;
	}
	p[i] = 0;
	itr->Release();

	if (ke->Setseeds(p))
		return kifTRUE;
	delete[] p;
	return kifFALSE;
}

KifElement* PreEvaluationMap(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	return ke->EvaluationMap();
}

KifElement* PreConvertVector(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	if (ke->isString())
		return ke->Vector(dom);
	return ke->Vector(contextualpattern);
}

KifElement* PreConvertMap(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	if (ke->isString())
		return ke->Map(dom);
	return ke->Map(contextualpattern);
}

KifElement* PreJSon(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	return callfunc->kifcode->Providestringraw(ke->JSonString());
}


KifElement* PreConvertString(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	return callfunc->kifcode->Providestringraw(ke->String());
}

KifElement* PreConvertFloat(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	return callfunc->kifcode->Providefloat(ke->Float());
}

KifElement* PreConvertFraction(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	return ke->Fraction();
}


KifElement* PreConvertInteger(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	return callfunc->kifcode->Provideinteger(ke->Integer());
}

KifElement* PreConvertLong(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	return new KifLong(callfunc->kifcode, NULL, ke->Long());
}

KifElement* PreConvertBoolean(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	if (ke->Boolean())
		return kifTRUE;
	return kifFALSE;
}


KifElement* PreMethods(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	if (kvect->type != kifVectorString)
		kvect = new KifVectorString(callfunc->kifcode, NULL);
	ke->Methods((KifVectorString*)kvect);
	return kvect;
}

KifElement* PreInfos(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kstr = callfunc->Evaluatethread(1, dom, idthread);
	return ke->Information(kstr);
}

KifElement* PreUTF8ToLatin(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	agnostring thestr(callfunc->Evaluatethread(0, dom, idthread)->String());
	return callfunc->kifcode->Providestringraw(thestr.utf8tolatin());
}

KifElement* PreLatinToUTF8(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	agnostring thestr(callfunc->Evaluatethread(0, dom, idthread)->String());
	if (callfunc->Size() == 1) {
		if (contextualpattern->type == kifUString)
			return callfunc->kifcode->Provideustring(thestr.latintounicode());
		return callfunc->kifcode->Providestringraw(thestr.latintoutf8());
	}

	int table = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	if (contextualpattern->type == kifUString)
		return callfunc->kifcode->Provideustring(thestr.latintounicode(table));

	return callfunc->kifcode->Providestringraw(thestr.latintoutf8(table));
}

KifElement* PreToDOS(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	agnostring thestr(callfunc->Evaluatethread(0, dom, idthread)->String());
	return callfunc->kifcode->Providestringraw(thestr.utf8todos());
}

KifElement* PreDOSToUTF8(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	agnostring thestr(callfunc->Evaluatethread(0, dom, idthread)->String());
	return callfunc->kifcode->Providestringraw(thestr.dostoutf8());
}


KifElement* PreLeft(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	int nbchar = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	if (kstr->type == kifUString) {
		wstring thestr = kstr->UString();
		thestr = s_left(thestr, nbchar);
		return callfunc->kifcode->Provideustring(thestr);
	}
	agnostring thestr(kstr->String());
	//Second parameter is a position
	return callfunc->kifcode->Providestringraw(thestr.left(nbchar));
}

KifElement* PreRight(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	int nbchar = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	if (kstr->type == kifUString) {
		wstring thestr = kstr->UString();
		thestr = s_right(thestr, nbchar);
		return callfunc->kifcode->Provideustring(thestr);
	}

	agnostring thestr(kstr->String());
	return callfunc->kifcode->Providestringraw(thestr.right(nbchar));
}

KifElement* PreMid(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	//Second parameter is a position
	long pos = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	//Third parameter is a number of characters
	int nbchar = callfunc->Evaluatethread(2, dom, idthread)->Integer();
	if (kstr->type == kifUString) {
		wstring thestr = kstr->UString();
		thestr = s_middle(thestr, pos, nbchar);
		return callfunc->kifcode->Provideustring(thestr);
	}
	agnostring thestr(kstr->String());
	return callfunc->kifcode->Providestringraw(thestr.extract(pos, nbchar));
}

KifElement* PreIsLower(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		if (s_is_lower(str))
			return kifTRUE;
		return kifFALSE;
	}
	agnostring thestr(kstr->String());
	if (thestr.islower())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreIsUpper(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		if (s_is_upper(str))
			return kifTRUE;
		return kifFALSE;
	}
	agnostring thestr(kstr->String());
	if (thestr.isupper())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreIsEmoji(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->aNumber()) {
		BULONG c = kstr->Long();
		if (c_is_emoji(c))
			return kifTRUE;
		return kifFALSE;
	}

	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		if (s_is_emoji(str))
			return kifTRUE;
		return kifFALSE;
	}
	agnostring thestr(kstr->String());
	if (thestr.isemoji())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreEmoji(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	string res;

	if (kstr->aNumber()) {
		BULONG c = kstr->Long();
		res = Emoji(c);
	}
	else {
		if (kstr->type == kifUString) {
			wstring str = kstr->UString();
			res = Emoji(str);
		}
		else {
			res = kstr->String();
			res = Emoji(res);
		}
	}

	return callfunc->kifcode->Providestring(res);
}



KifElement* PreIsDigit(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	agnostring thestr(callfunc->Evaluatethread(0, dom, idthread)->String());
	if (thestr.isdigit())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreIsAlpha(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		if (s_is_alpha(str))
			return kifTRUE;
		return kifFALSE;
	}

	agnostring thestr(kstr->String());
	if (thestr.isalpha())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreIsConsonants(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		if (s_is_consonant(str))
			return kifTRUE;
		return kifFALSE;
	}
	agnostring thestr(kstr->String());
	if (thestr.isconsonant())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreIsVowels(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		if (s_is_vowel(str))
			return kifTRUE;
		return kifFALSE;
	}
	agnostring thestr(kstr->String());
	if (thestr.isvowel())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreDeaccentuate(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		return callfunc->kifcode->Provideustring(s_deaccentuate(str));
	}
	agnostring thestr(kstr->String());
	return callfunc->kifcode->Providestringraw(thestr.deaccentuate());
}


KifElement* PreIsPunctuation(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		if (s_is_punctuation(str))
			return kifTRUE;
		return kifFALSE;
	}
	agnostring thestr(kstr->String());
	if (thestr.ispunctuation())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreLower(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		return callfunc->kifcode->Provideustring(s_to_lower(str));
	}
	agnostring thestr(kstr->String());
	return callfunc->kifcode->Providestringraw(thestr.tolower());
}

KifElement* PreUpper(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		return callfunc->kifcode->Provideustring(s_to_upper(str));
	}
	agnostring thestr(kstr->String());
	return callfunc->kifcode->Providestringraw(thestr.toupper());
}


KifElement* PreTrim(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		return callfunc->kifcode->Provideustring(Trim(str));
	}
	agnostring thestr(kstr->String());
	return callfunc->kifcode->Providestringraw(thestr.trim());
}

KifElement* PreTrimright(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		return callfunc->kifcode->Provideustring(Trimright(str));
	}
	agnostring thestr(kstr->String());
	return callfunc->kifcode->Providestringraw(thestr.trimright());
}

KifElement* PreTrimleft(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		return callfunc->kifcode->Provideustring(Trimleft(str));
	}
	agnostring thestr(kstr->String());
	return callfunc->kifcode->Providestringraw(thestr.trimleft());
}

KifElement* PreName(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	short name = res->Name();
	if (kifGlobals->kifIdString.find(name) == kifGlobals->kifIdString.end())
		return callfunc->kifcode->Providestringraw("");
	return callfunc->kifcode->Providestring(kifGlobals->kifIdString[name]);
}

KifElement* PreType(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat) {
		int r = contextualpattern->type;
		return callfunc->kifcode->Provideinteger(r);
	}
	return callfunc->kifcode->Providestringraw(res->Type());
}

KifElement* PreRandom(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	long mx = 100;
	if (callfunc->Size() == 1)
		mx = callfunc->Evaluatethread(0, dom, idthread)->Float();
	return callfunc->kifcode->Providefloat(localrandom(mx));
}


//------------------TIME ROUTINES------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

KifElement* PreResetTime(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a time
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifTime)
		return callfunc->kifcode->Returnerror(kifErrorStrings[219]);
	KifTime* ktime = (KifTime*)timeinfo;
	gettimeofday(&ktime->value, NULL);
	return ktime;
}

KifElement* PreSetDate(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreSetDate");
#endif


	//First parameter is a time
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[14]);

	KifDate* ktime = (KifDate*)timeinfo;
	struct tm* temps;
	time_t x = 0;
	temps = localtime(&x);

	int fulldate = 0;
	long res;
	//Year
	if (callfunc->Size() >= 2) {
		res = callfunc->Evaluatethread(1, dom, idthread)->Integer();
		if (res > 0) {
			temps->tm_year = res - 1900;
			fulldate = 100;
		}
	}

	//Month
	if (callfunc->Size() >= 3) {
		res = callfunc->Evaluatethread(2, dom, idthread)->Integer();
		if (res > 0) {
			fulldate += 10;
			temps->tm_mon = res - 1;
		}
	}

	//Day
	if (callfunc->Size() >= 4) {
		res = callfunc->Evaluatethread(3, dom, idthread)->Integer();
		if (res > 0) {
			fulldate += 1;
			temps->tm_mday = res;
		}
	}
	//Hour
	if (callfunc->Size() >= 5) {
		res = callfunc->Evaluatethread(4, dom, idthread)->Integer();
		if (res >= 0 && res < 24) {
			//reference is 1AM
			if (fulldate == 0)
				res += 2;
			else
			if (fulldate == 1)
				res += 1;
			temps->tm_hour = res;
		}
	}

	//Minute
	if (callfunc->Size() >= 6) {
		res = callfunc->Evaluatethread(5, dom, idthread)->Integer();
		if (res >= 0 && res < 60)
			temps->tm_min = res;
	}

	//Second
	if (callfunc->Size() >= 7) {
		res = callfunc->Evaluatethread(6, dom, idthread)->Integer();
		if (res >= 0 && res < 60)
			temps->tm_sec = res;
	}


	x = mktime(temps);
	if (x <= 0)
		return kifFALSE;
	ktime->value = x;
	return kifTRUE;
}

KifElement* PreDate(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreDate");
#endif

	//First parameter is a string
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifDate)
		callfunc->kifcode->Returnerror(kifErrorStrings[16]);
	string d = timeinfo->String();
	return callfunc->kifcode->Providestring(d);
}


KifElement* PreYear(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreYear");
#endif

	//First parameter is a string
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[18]);

	struct tm* temps = localtime(&((KifDate*)timeinfo)->value);
	long vl;

	if (callfunc->Size() == 2) {
		vl = callfunc->Evaluatethread(1, dom, idthread)->Integer();
		if (vl < 100)
			return callfunc->kifcode->Returnerror(kifErrorStrings[272]);
		temps->tm_year = vl - 1900;
		((KifDate*)timeinfo)->value = mktime(temps);
		return kifTRUE;
	}

	vl = temps->tm_year + 1900;
	return callfunc->kifcode->Provideinteger(vl);
}

KifElement* PreMonth(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter is a string
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[24]);

	long vl;
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value);
	if (callfunc->Size() == 2) {
		vl = callfunc->Evaluatethread(1, dom, idthread)->Integer();
		if (vl < 1 || vl>12)
			return callfunc->kifcode->Returnerror(kifErrorStrings[273]);
		temps->tm_mon = vl - 1;
		((KifDate*)timeinfo)->value = mktime(temps);
		return kifTRUE;
	}
	vl = temps->tm_mon + 1;
	return callfunc->kifcode->Provideinteger(vl);
}

KifElement* PreDay(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[20]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value);
	long vl;

	if (callfunc->Size() == 2) {
		vl = callfunc->Evaluatethread(1, dom, idthread)->Integer();
		if (vl < 1 || vl>31)
			return callfunc->kifcode->Returnerror(kifErrorStrings[274]);
		temps->tm_mday = vl;
		((KifDate*)timeinfo)->value = mktime(temps);
		return kifTRUE;
	}

	vl = temps->tm_mday;
	return callfunc->kifcode->Provideinteger(vl);
}

KifElement* PreHour(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[22]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value);

	if (callfunc->Size() == 2) {
		long vl = callfunc->Evaluatethread(1, dom, idthread)->Integer();
		if (vl < 0 || vl>23)
			return callfunc->kifcode->Returnerror(kifErrorStrings[275]);
		temps->tm_hour = vl;
		((KifDate*)timeinfo)->value = mktime(temps);
		return kifTRUE;
	}

	return callfunc->kifcode->Provideinteger(temps->tm_hour);
}

KifElement* PreMinute(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[22]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value);
	if (callfunc->Size() == 2) {
		long vl = callfunc->Evaluatethread(1, dom, idthread)->Integer();
		if (vl < 0 || vl>59)
			return callfunc->kifcode->Returnerror(kifErrorStrings[276]);
		temps->tm_min = vl;
		((KifDate*)timeinfo)->value = mktime(temps);
		return kifTRUE;
	}
	return callfunc->kifcode->Provideinteger(temps->tm_min);
}

KifElement* PreSecond(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[22]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value);
	if (callfunc->Size() == 2) {
		long vl = callfunc->Evaluatethread(1, dom, idthread)->Integer();
		if (vl < 0 || vl>59)
			return callfunc->kifcode->Returnerror(kifErrorStrings[276]);
		temps->tm_sec = vl;
		((KifDate*)timeinfo)->value = mktime(temps);
		return kifTRUE;
	}
	return callfunc->kifcode->Provideinteger(temps->tm_sec);
}

KifElement* PreWeekDay(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[20]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value);
	long vl = temps->tm_wday;
	return callfunc->kifcode->Provideinteger(vl);
}

KifElement* PreYearDay(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* timeinfo = callfunc->Evaluatethread(0, dom, idthread);
	if (timeinfo->type != kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[20]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value);
	long vl = temps->tm_yday;
	return callfunc->kifcode->Provideinteger(vl);
}

//-----------------------------------------------------------------------------------------------------

KifElement* PreMin(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter is a string
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	int sz = callfunc->Size();
	KifElement* knext;
	if (sz == 1 && kfirst->aContainer()) {
		KifIteration* itr = (KifIteration*)kfirst->Newiterator(false);
		bool first = true;
		kfirst = kifNULL;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			knext = itr->IteratorValue();
			if (first) {
				kfirst = knext;
				first = false;
			}
			else
			if (kfirst->more(knext)->Boolean()) {
				kfirst->Release();
				kfirst = knext;
			}
			else
				knext->Release();
		}
		itr->Release();
		return kfirst;
	}

	for (int i = 1; i < sz; i++) {
		knext = callfunc->Evaluatethread(i, dom, idthread);
		if (kfirst->more(knext)->Boolean()) {
			kfirst->Release();
			kfirst = knext;
		}
		else
			knext->Release();
	}
	return kfirst;
}

KifElement* PreMax(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	int sz = callfunc->Size();
	KifElement* knext;
	if (sz == 1 && kfirst->aContainer()) {
		KifIteration* itr = (KifIteration*)kfirst->Newiterator(false);
		bool first = true;
		kfirst = kifNULL;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			knext = itr->IteratorValue();
			if (first) {
				kfirst = knext;
				first = false;
			}
			else
			if (kfirst->less(knext)->Boolean()) {
				kfirst->Release();
				kfirst = knext;
			}
			else
				knext->Release();
		}
		itr->Release();
		return kfirst;
	}

	for (int i = 1; i < sz; i++) {
		knext = callfunc->Evaluatethread(i, dom, idthread);
		if (kfirst->less(knext)->Boolean()) {
			kfirst->Release();
			kfirst = knext;
		}
		else
			knext->Release();
	}
	return kfirst;
}


KifElement* PreCatch(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (kifGlobals->executionbreak)
		return kifFALSE;

	if (Errorid(idthread) == false)
		return kifFALSE;
	if (callfunc->Size() != 1) {
		kifGlobals->THR[idthread]->cleanerror();
		return kifTRUE;
	}
	string resstr = Errorstring();
	kifGlobals->THR[idthread]->cleanerror();
	KifElement* val;
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	if (res->Returnvalue()->type == kifString) {
		val = callfunc->kifcode->Providestring(resstr);
		res->Setvalue(val);
	}
	else {
		//We take the first three characters
		//we look for the first digit
		string resex;
		for (int i = 0; i < resstr.size(); i++) {
			while (i < resstr.size() && resstr[i] >= '0' && resstr[i] <= '9') {
				resex += resstr[i];
				i++;
			}
			if (resex != "")
				break;
		}
		//Which we transform into a number
		val = callfunc->kifcode->Provideinteger(atol(STR(resex)));
		res->Setvalue(val);
	}
	return kifTRUE;
}

KifElement* PreRaise(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreRaise");
#endif
	if (Error())
		return kifFALSE;

	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	string resstr = res->String();
	return callfunc->kifcode->Returnerror(resstr);
}

KifElement* PreTypeIs(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* typ = callfunc->Evaluatethread(1, dom, idthread);
	string resstr = res->Type();
	string typstr = typ->String();
	if (resstr == typstr)
		return kifTRUE;
	return kifFALSE;
}

KifElement* PrePrimefactors(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifVectorInteger* kvect = new KifVectorInteger(callfunc->kifcode, NULL);
	long n = callfunc->Evaluatethread(0, dom, idthread)->Integer();
	long dec = 2;
	long ni = n;
	while (n > 1 && (dec*dec) <= ni) {
		if (n%dec == 0) {
			kvect->values.push_back(dec);
			n /= dec;
		}
		else
			dec++;
	}
	if (n > 1)
		kvect->values.push_back(n);
	return kvect;
}

KifElement* PrePrime(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	long n = callfunc->Evaluatethread(0, dom, idthread)->Integer();
	if (!n)
		return kifFALSE;
	if (n == 2 || n == 3 || n == 5 || n == 7 || n == 11)
		return kifTRUE;

	if (!n || (n % 2) == 0 || (n % 3) == 0 || (n % 5) == 0 || (n % 7) == 0 || (n % 11) == 0)
		return kifFALSE;

	long dec = 13;
	long ni = n;

	while (n > 1 && (dec*dec) <= ni) {
		if (n%dec == 0)
			return kifFALSE;
		dec += 4;
		if (n%dec == 0)
			return kifFALSE;
		dec += 2;
		if (n%dec == 0)
			return kifFALSE;
		dec += 2;
		if (n%dec == 0)
			return kifFALSE;
		dec += 2;
	}

	return kifTRUE;
}

KifElement* PreBase(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	static vector<wstring> caracs;
	static hash_bin<wchar_t, int> mcaracs;
	wstring w;
	long n, b = -1;

	KifElement* ke = callfunc->Evaluatethread(1, dom, idthread);
	//We can reset the chararacter list...
	if (ke->aVectorContainer()) {
		caracs.clear();
		mcaracs.clear();
		b = ke->Size();
		for (n = 0; n < b; n++) {
			w = ke->getustring(n);
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
		}
		if (b)
			return callfunc->kifcode->Provideinteger(b);
	}

	if (caracs.size() == 0) {
		w = L"0";
		for (n = 0; n < 10; n++) {
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
			w[0]++;
		}
		w = L"A";
		for (n = 10; n < 36; n++) {
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
			w[0]++;
		}
		w = L"a";
		for (n = 36; n < 62; n++) {
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
			w[0]++;
		}
		w = L"#";
		mcaracs[w[0]] = caracs.size();
		caracs.push_back(w);
		w = L"@";
		mcaracs[w[0]] = caracs.size();
		caracs.push_back(w);
		w = L"";
		if (!b)
			return kifTRUE;
	}

	b = ke->Integer();
	ke = callfunc->Evaluatethread(0, dom, idthread);

	if (b > caracs.size()) {
		char message[1024];
		sprintf_s(message, 1024, "KIF(510): base too large, max is: %d", caracs.size());
		return callfunc->kifcode->Returnerror(message);
	}

	if (ke->isString()) {
		w = ke->UString();
		long v = 0;
		wchar_t wc;
		for (n = 0; n < w.size(); n++) {
			wc = w[n];
			if (mcaracs.check(wc) == false || mcaracs[wc] >= b)
				return callfunc->kifcode->Returnerror("KIF(511): Cannot analyze this string in this base.");

			v *= b;
			v += mcaracs[wc];
		}
		return callfunc->kifcode->Provideinteger(v);
	}

	n = ke->Integer();

	while (n) {
		w = caracs[n%b] + w;
		n /= b;
	}
	
	return callfunc->kifcode->Provideustring(w);
}


inline bool is_number(double x) {
	return (x == x);
}

inline bool is_finite(double x) {
	return (x <= DBL_MAX && x >= -DBL_MAX);
}


KifElement* PreMath(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kbase = callfunc->Evaluatethread(0, dom, idthread);
	double v;
	if (kbase->aContainer()) {
		if (kbase->aVectorContainer()) {
			KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
			long sz = kbase->Size();
			for (long i = 0; i < sz; i++) {
				v = kifGlobals->kifMathFunctions[callfunc->Name()](kbase->getfloat(i));
				if (!is_finite(v))
					return callfunc->kifcode->Returnerror("KIF(801): Infinite value");

				if (!is_number(v))
					return callfunc->kifcode->Returnerror("KIF(802): Not a number");
				kvect->storevalue(v);
			}
			return kvect;
		}

		if (kbase->type == kifList) {
			KifList* klist = (KifList*)kbase;
			KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
			list<KifElement*>::iterator it;
			for (it = klist->values.begin(); it != klist->values.end(); it++) {
				v = kifGlobals->kifMathFunctions[callfunc->Name()]((*it)->Float());
				if (!is_finite(v))
					return callfunc->kifcode->Returnerror("KIF(801): Infinite value");

				if (!is_number(v))
					return callfunc->kifcode->Returnerror("KIF(802): Not a number");
				kvect->storevalue(v);
			}
			return kvect;
		}
		return kbase;
	}

	v = kbase->Float();
	v = kifGlobals->kifMathFunctions[callfunc->Name()](v);
	if (!is_finite(v))
		return callfunc->kifcode->Returnerror("KIF(801): Infinite value");

	if (!is_number(v))
		return callfunc->kifcode->Returnerror("KIF(802): Not a number");

	return callfunc->kifcode->Providefloat(v);
}

KifElement* PrePadding(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kvstr = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* res = callfunc->Evaluatethread(1, dom, idthread);
	KifElement* kval = callfunc->Evaluatethread(2, dom, idthread);
	bool direction = true;
	if (callfunc->Size() == 4)
		direction = callfunc->Evaluatethread(3, dom, idthread)->Boolean();

	if (kvstr->type == kifUString) {
		KifUString* kstr = (KifUString*)kvstr;
		wstring sval = kval->UString();
		long sz = res->Integer() - kstr->value.size();
		if (sz <= 0)
			return kvstr;

		wstring sres;
		wstring s;
		while (sz > 0) {
			s += sval;
			sz--;
		}
		sres = s;
		if (direction)
			kstr->value += sres;
		else
			kstr->value = sres + kstr->value;

		return kstr;
	}

	KifString* kstr = (KifString*)kvstr;
	string sval = kval->String();
	long sz = res->Integer() - kstr->value.size();
	if (sz <= 0)
		return kvstr;

	string sres;
	if (sval != "" && sval.size() == 1) {
		char* s = (char*)malloc(sz + 1);
		char c = ' ';
		c = sval[0];
		memset(s, c, sz);
		s[sz] = 0;
		sres = s;
		free(s);
	}
	else {
		string s;
		while (sz > 0) {
			s += sval;
			sz--;
		}
		sres = s;
	}
	if (direction)
		kstr->value += sres;
	else
		kstr->value = sres + kstr->value;

	return kstr;
}

KifElement* PreFill(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kvstr = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* res = callfunc->Evaluatethread(1, dom, idthread);
	KifElement* kval = callfunc->Evaluatethread(2, dom, idthread);
	int sz = res->Integer();
	if (sz <= 0)
		return callfunc->kifcode->Providestringraw("");

	if (kvstr->type == kifUString) {
		KifUString* kstr = (KifUString*)kvstr;
		wstring sval = kval->UString();
		kstr->value = L"";
		while (sz > 0) {
			kstr->value += sval;
			sz--;
		}
		return kstr;
	}

	KifString* kstr = (KifString*)kvstr;
	string sval = kval->String();
	if (sval != "" && sval.size() == 1) {
		char* s = (char*)malloc(sz + 1);
		char c = ' ';
		c = sval[0];
		memset(s, c, sz);
		s[sz] = 0;
		kstr->value = s;
		free(s);
	}
	else {
		string s;
		while (sz > 0) {
			s += sval;
			sz--;
		}
		kstr->value = s;
	}
	return kstr;
}

KifElement* PreOrd(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	wstring s = callfunc->Evaluatethread(0, dom, idthread)->UString();
	if (s.size() >= 1) {
		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return callfunc->kifcode->Provideinteger((int)s[0]);

		if (contextualpattern->aVectorContainer() || s.size() > 1) {
			KifElement* kvect = Selectvectorinteger(callfunc, contextualpattern);
			if (kvect->type == kifVectorInteger) {
				KifContainerLock _lock((KifEvaluate*)kvect);
				for (int i = 0; i < s.size(); i++)
					((KifVectorInteger*)kvect)->values.push_back((long)s[i]);
			}
			else {
				KifBaseInteger loc(NULL, NULL);
				loc.kifcode = callfunc->kifcode;
				for (int i = 0; i < s.size(); i++) {
					loc.value = (long)s[i];
					kvect->Push(&loc);
				}
			}
			return kvect;
		}

		return callfunc->kifcode->Provideinteger((int)s[0]);
	}

	return kifNULL;
}

KifElement* PreHTMLEncoding(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	agnostring s(callfunc->Evaluatethread(0, dom, idthread)->String());
	string val;
	KifElement* res;
	vector<long> values;
	s.unicodes(values);
	if (contextualpattern->isString()) {
		res = callfunc->kifcode->Providestringraw("");
		for (int i = 0; i < values.size(); i++) {
			if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
				c_HTMLcode(val, values[i]);
			else
				val = (char)values[i];
			((KifString*)res)->value += val;
		}
		return res;
	}

	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	for (int i = 0; i < values.size(); i++) {
		if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
			c_HTMLcode(val, values[i]);
		else
			val = (char)values[i];
		kvect->storevalue(val);
	}
	return kvect;
}

KifElement* PreXMLEncoding(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	agnostring s(callfunc->Evaluatethread(0, dom, idthread)->String());
	string val;
	KifElement* res;
	vector<long> values;
	s.unicodes(values);
	if (contextualpattern->isString()) {
		res = callfunc->kifcode->Providestringraw("");
		for (int i = 0; i < values.size(); i++) {
			if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
				c_XMLcode(val, values[i]);
			else
				val = (char)values[i];
			((KifString*)res)->value += val;
		}
		return res;
	}

	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	for (int i = 0; i < values.size(); i++) {
		if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
			c_XMLcode(val, values[i]);
		else
			val = (char)values[i];
		kvect->storevalue(val);
	}
	return kvect;
}


void Flattening(KifElement* ret, KifElement* res) {
	KifIteration* itr = (KifIteration*)res->Newiterator(false);
	KifElement* kval;
	for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
		kval = itr->IteratorValue();
		if (kval->aContainer())
			Flattening(ret, kval);
		else
			ret->Push(kval);
	}
	itr->Release();
}

KifElement* PreFlatten(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	if (res->type != kifVector && res->type != kifList)
		return res;
	KifElement* ret = res->Newinstance();
	Flattening(ret, res);
	return ret;
}

KifElement* PreBytes(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	int i;
	long v;

	if (res->type == kifVectorByte) {
		KifVectorByte* kvect = (KifVectorByte*)res;
		KifContainerLock _lock(kvect);
		char* buff = new char[kvect->values.size() + 1];
		for (i = 0; i < kvect->values.size(); i++)
			buff[i] = kvect->values[i];

		buff[i] = 0;
		res = callfunc->kifcode->Providestringraw(buff);
		delete[] buff;
		return res;
	}

	if (res->type == kifVectorInteger) {
		KifVectorInteger* kvect = (KifVectorInteger*)res;
		KifContainerLock _lock(kvect);
		char* buff = new char[kvect->values.size() + 1];
		for (i = 0; i < kvect->values.size(); i++) {
			v = kvect->values[i];
			if (v<0 || v>255)
				buff[i] = 32;
			else
				buff[i] = v;
		}
		buff[i] = 0;
		res = callfunc->kifcode->Providestringraw(buff);
		delete[] buff;
		return res;
	}

	if (res->aContainer()) {
		//We transform the vector into a string
		char* buff = new char[res->Size() + 1];
		i = 0;
		KifIteration* itr = (KifIteration*)res->Newiterator(false);
		for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
			v = itr->IteratorValueInteger();
			if (v<0 || v>255)
				buff[i] = 32;
			else
				buff[i] = v;
			i++;
		}
		buff[i] = 0;
		res = callfunc->kifcode->Providestringraw(buff);
		delete[] buff;
		itr->Release();
		return res;
	}

	if (res->type == kifUString) {
		wstring s = res->UString();
		size_t sz = s.size();
		if (sz >= 1) {
			switch (contextualpattern->type) {
			case kifInteger:
				return callfunc->kifcode->Provideinteger(s[0]);
			case kifFloat:
				return callfunc->kifcode->Providefloat(s[0]);
			case kifByte:
				return new KifByte(callfunc->kifcode, NULL, (uchar)s[0]);
			}
			int j;
			if (contextualpattern->aVectorContainer() || sz > 1) {
				w_u_char c;
				KifElement* kvect = Selectavector(callfunc, contextualpattern);
				switch (kvect->type) {
				case kifVectorByte:
					for (i = 0; i < s.size(); i++) {
						c.c = s[i];
						for (j = 0; j < sizeof(wchar_t); j++)
							((KifVectorByte*)kvect)->values.push_back(c.cc[j]);
					}
					return kvect;
				case kifVectorInteger:
					for (i = 0; i < s.size(); i++)
						((KifVectorInteger*)kvect)->values.push_back(s[i]);
					return kvect;
				case kifVectorFloat:
					for (i = 0; i < s.size(); i++)
						((KifVectorFloat*)kvect)->values.push_back(s[i]);
					return kvect;
				}

				KifInteger loc(NULL, NULL);
				loc.kifcode = callfunc->kifcode;
				loc.reference = 1;

				for (i = 0; i < s.size(); i++) {
					loc.value = s[i];
					kvect->Push(&loc);
				}
				return kvect;
			}

			return new KifByte(callfunc->kifcode, NULL, s[0]);
		}
		return kifNULL;
	}
	string s = res->Bytes();
	int sz = size_c(STR(s));
	if (sz >= 1) {
		switch (contextualpattern->type) {
		case kifInteger:
			return callfunc->kifcode->Provideinteger(s[0]);
		case kifFloat:
			return callfunc->kifcode->Providefloat(s[0]);
		case kifByte:
			return new KifByte(callfunc->kifcode, NULL, s[0]);
		}

		if (contextualpattern->aVectorContainer() || sz > 1) {
			KifElement* kvect = Selectavector(callfunc, contextualpattern);
			switch (kvect->type) {
			case kifVectorByte:
				for (i = 0; i < s.size(); i++)
					((KifVectorByte*)kvect)->values.push_back(s[i]);
				return kvect;
			case kifVectorInteger:
				for (i = 0; i < s.size(); i++)
					((KifVectorInteger*)kvect)->values.push_back((uchar)s[i]);
				return kvect;
			case kifVectorFloat:
				for (i = 0; i < s.size(); i++)
					((KifVectorFloat*)kvect)->values.push_back((uchar)s[i]);
				return kvect;
			}

			KifInteger loc(NULL, NULL);
			loc.kifcode = callfunc->kifcode;
			loc.reference = 1;
			for (i = 0; i < s.size(); i++) {
				loc.value = (uchar)s[i];
				kvect->Push(&loc);
			}
			return kvect;
		}

		return new KifByte(callfunc->kifcode, NULL, s[0]);
	}

	return kifNULL;
}

KifElement* PreChr(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreChr");
#endif
	agnostring s(callfunc->Evaluatethread(0, dom, idthread)->Long());
	return callfunc->kifcode->Providestringraw(s);
}


KifElement* PreKill(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreKill");
#endif

	ThreadLock _lock(kifSignal);
	vector<KifWaitState*>* kws;
	vector<KifWaitState*>::iterator it;
	if (callfunc->Size() == 0) {
		hmap<string, vector<KifWaitState*> >::iterator itm;
		for (itm = kifWaitstates.begin(); itm != kifWaitstates.end(); itm++) {
			kws = &itm->second;
			for (it = kws->begin(); it != kws->end(); it++) {
				if ((*it)->loquet != NULL) {
					(*it)->killed = true;
					(*it)->loquet->Released();
				}
			}
			kws->clear();
		}
		kifWaitstates.clear();
		return kifTRUE;
	}

	if (callfunc->Size() != 1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[197]);

	KifElement* kflag = callfunc->Evaluatethread(0, dom, idthread);
	string flag = kflag->String();
	kws = Freestate(callfunc->kifcode, flag);
	if (kws == NULL)
		return kifNULL;
	for (it = kws->begin(); it != kws->end(); it++) {
		if ((*it)->loquet != NULL) {
			(*it)->killed = true;
			(*it)->loquet->Released();
		}
	}
	kws->clear();
	kifWaitstates.erase(flag);
	return kifTRUE;
}


KifElement* PreCast(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreCast");
#endif
	vector<KifWaitState*>* kws;
	vector<KifWaitState*>::iterator it;

	if (callfunc->Size() == 0) {
		ThreadLock _lock(kifSignal);
		hmap<string, vector<KifWaitState*> >::iterator itm;
		//First we duplicate it...
		for (itm = kifWaitstates.begin(); itm != kifWaitstates.end(); itm++) {
			kws = &itm->second;
			for (it = kws->begin(); it != kws->end(); it++) {
				if ((*it)->loquet != NULL)
					(*it)->loquet->Released();
			}
			kws->clear();
		}

		kifWaitstates.clear();
		return kifTRUE;
	}

	if (callfunc->Size() != 1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[198]);

	KifElement* kflag = callfunc->Evaluatethread(0, dom, idthread);
	string flag = kflag->String();

	ThreadLock _lock(kifSignal);
	kws = Freestate(callfunc->kifcode, flag);
	if (kws == NULL)
		return kifNULL;

	//We need to duplicate the vector beforehand, to take into account any specific modification
	//while applying the rest of the functions.
	for (it = kws->begin(); it != kws->end(); it++) {
		(*it)->lastcast = flag;
		if ((*it)->loquet != NULL)
			(*it)->loquet->Released();
	}
	kws->clear();
	kifWaitstates.erase(flag);
	return kifTRUE;
}

KifElement* PreWaitOnJoin(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (kifGlobals->THR[idthread]->joinedloquets.size() == 0)
		return kifFALSE;

	VECTE<THREADPTR>& v = kifGlobals->THR[idthread]->joinedloquets;
	bool alive = true;

#ifdef C11
	std::thread* t = NULL;
	while (alive) {
		alive = false;
		for (int i = 0; i < kifGlobals->maxjoined; i++) {
			if (Errorid(idthread)) {
				alive = false;
				break;
			}

			t = NULL;
			{
				ThreadLock _lock(kifCallFunctionThread);
				if (v.vecteur[i] != NULL) {
					t = v.vecteur[i];
					alive = true;
				}
			}
			if (t != NULL)
				t->join();
		}
	}
#else
#ifdef WIN32
	while (alive) {
		alive = false;
		for (int i = 0; i<kifGlobals->maxjoined; i++) {
			if (Errorid(idthread)) {
				alive = false;
				break;
			}

			ThreadLock _lock(kifCallFunctionThread);
			if (v.vecteur[i] != NULL) {
				i--;
				alive = true;
			}
		}
	}
#else
	while (alive) {
		alive = false;
		for (int i = 0; i<kifGlobals->maxjoined; i++) {
			if (Errorid(idthread)) {
				alive = false;
				break;
			}
			if (v.vecteur[i] != NULL) {
				pthread_join(v.vecteur[i], NULL);
				alive = true;
			}
		}
	}
#endif
#endif

	v.clear();
	return kifTRUE;
}

KifElement* PreWaitOnFalse(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	LockedThread* l = NULL;
	//first we get our variable back
	KifElement* var = callfunc->parameters[0];
	if (var->type != kifInstance)
		callfunc->kifcode->Returnerror("KIF(877): Expecting a variable");
	var = var->Thevariable(dom, kifGlobals->GetThreadid(), NULL);

	int id = var->idgarbage;
	var = var->Returnvalue();
	//If we wait on a frame object, then the modification is on one of the variables of that object
	if (var->type == kifDomain) {
		KifDomain* vdom = (KifDomain*)var;
		bin_hash<KifElement*>::iterator it;
		for (it = vdom->declarations.begin(); it != vdom->declarations.end(); it++) {
			if (it->second->isFunction())
				continue;
			l = kifGlobals->GetLoquet(it->second->idgarbage);
			if (l != NULL)
				break;
		}
	}
	else
		l = kifGlobals->GetLoquet(id);

	if (l == NULL)
		return kifFALSE;

	//we wait until our variable changes its status
	l->Blocked();
	return kifTRUE;
}

KifElement* PreLockRec(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//we create our lock
	KifThreadLock* ktl;
	KifElement* klock = callfunc->Evaluatethread(0, dom, idthread);
	string lock = klock->String();
	{
		ThreadLock _lock(kifLock);
		if (kifGlobals->locktables.find(lock) == kifGlobals->locktables.end()) {
			ktl = new KifThreadLock(NULL, false);
			kifGlobals->locktables[lock] = ktl;
		}
		else
			ktl = kifGlobals->locktables[lock];
	}
	ktl->Locking();
	ktl->idthread = idthread;
	return kifTRUE;
}


KifElement* PreLock(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//we create our lock
	KifThreadLock* ktl;
	KifElement* klock = callfunc->Evaluatethread(0, dom, idthread);
	string lock = klock->String();
	{
		ThreadLock _lock(kifLock);
		if (kifGlobals->locktables.find(lock) == kifGlobals->locktables.end()) {
			ktl = new KifThreadLock(NULL, false, false);
			kifGlobals->locktables[lock] = ktl;
		}
		else
			ktl = kifGlobals->locktables[lock];
	}
	ktl->Locking();
	ktl->idthread = idthread;
	return kifTRUE;
}

KifElement* PreUnlock(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifThreadLock* ktl;
	KifElement* klock = callfunc->Evaluatethread(0, dom, idthread);
	string lock = klock->String();
	{
		ThreadLock _lock(kifLock);
		if (kifGlobals->locktables.find(lock) == kifGlobals->locktables.end()) {
			string message = kifErrorStrings[240];
			message += lock;
			return callfunc->kifcode->Returnerror(message);
		}
		ktl = kifGlobals->locktables[lock];
	}
	ktl->idthread = 0;
	ktl->Unlocking();
	return kifTRUE;
}



KifElement* PreWait(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreWait");
#endif
	KifElement* kflag = callfunc->Evaluatethread(0, dom, idthread);
	string flag = kflag->String();

	if (flag != "") {
		vector<string> flags;
		flags.push_back(flag);
		ThreadLock* lock = new ThreadLock(kifSignal);
		LockedThread loquet;
		KifWaitState kws;
		kws.loquet = &loquet;
		kifWaitstates[flag].push_back(&kws);
		//we keep a track of all labels...
		//But only one semaphore is set...
		for (int i = 1; i < callfunc->Size(); i++) {
			flag = callfunc->Evaluatethread(i, dom, idthread)->String();
			flags.push_back(flag);
			kifWaitstates[flag].push_back(&kws);
		}
		delete lock;

		loquet.Blocked();
		lock = new ThreadLock(kifSignal);
		if (kws.killed)
			callfunc->Setreturnvalue(kifTRUE);
		//cleaning
		for (int i = 0; i < flags.size(); i++) {
			if (kifWaitstates.find(flags[i]) != kifWaitstates.end()) {
				vector<KifWaitState*>& kwsloops = kifWaitstates[flags[i]];
				for (int j = kwsloops.size() - 1; j >= 0; j--) {
					if (kwsloops[j] == &kws)
						kwsloops.erase(kwsloops.begin() + j);
				}
				if (kwsloops.empty())
					kifWaitstates.erase(flags[i]);
			}
		}
		flag = kws.lastcast;
		delete lock;
	}
	return callfunc->kifcode->Providestring(flag);
}

KifElement* PreWaiting(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreWait");
#endif
	ThreadLock _lock(kifSignal);
	KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
	vector<KifWaitState*>::iterator it;
	hmap<string, vector<KifWaitState*> >::iterator itm;
	//First we duplicate it...
	for (itm = kifWaitstates.begin(); itm != kifWaitstates.end(); itm++)
		kvect->storevalue(itm->first);
	return kvect;
}

KifElement* PreVersion(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	return callfunc->kifcode->Providestringraw(Whoami_kif());
}

KifElement* PreFractionSimplify(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	return callfunc->Evaluatethread(0, dom, idthread)->FractionSimplify(false);
}

KifElement* PreEuclidian(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	return callfunc->Evaluatethread(0, dom, idthread)->Euclidian();
}

KifElement* PreNumeratorDenominator(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfract = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* knum = callfunc->Evaluatethread(1, dom, idthread);
	KifElement* kden = callfunc->Evaluatethread(2, dom, idthread);
	KIFLONG den = kden->Long();
	if (den == 0)
		return callfunc->kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
	kfract->ND(knum->Long(), den);
	return kfract;
}

KifElement* PreFractionSwitch(KifElement* contextual, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->aMapContainer()) {
		KifElement* res;
		if (contextual != NULL && contextual != kf && contextual->reference <= 1 && contextual != callfunc && contextual->aMapContainer() && contextual->Evaluate()) {
			res = contextual;
			res->Clear();
		}
		else
			res = kf->Newinstance();

		KifElement* key;
		KifElement* value;
		KifIteration* itr = (KifIteration*)kf->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			key = itr->IteratorKey();
			value = itr->IteratorValue();
			res->Push(value, key);
			value->Release();
			key->Release();
		}

		itr->Release();
		return res;
	}

	if (kf->type != kifFraction)
		kf = kf->Fraction();

	KIFLONG val = ((KifFraction*)kf)->denominator;
	((KifFraction*)kf)->denominator = ((KifFraction*)kf)->numerator;
	((KifFraction*)kf)->numerator = val;

	return kf;
}

KifElement* PrePartOf(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* ksecond = callfunc->Evaluatethread(1, dom, idthread);

	if (kfirst->type == kifBits && ksecond->type == kifBits) {
		KifBits* k1 = (KifBits*)kfirst;
		KifBits* k2 = (KifBits*)ksecond;

		if (k2->bitmap.size() > k1->bitmap.size())
			return kifFALSE;

		map<short, unsigned short>::iterator it;

		for (it = k2->bitmap.begin(); it != k2->bitmap.end(); it++) {
			if (k1->bitmap.find(it->first) == k1->bitmap.end())
				return kifFALSE;
			if ((it->second&k1->bitmap[it->first]) != it->second)
				return kifFALSE;
		}

		return kifTRUE;
	}

	if (kfirst->type == kifBit && ksecond->type == kifBit) {
		KifBit* k1 = (KifBit*)kfirst;
		KifBit* k2 = (KifBit*)ksecond;

		if (k2->bitvector.size() > k1->bitvector.size())
			return kifFALSE;

		int i;

		for (i = 0; i < k2->bitvector.size(); i++) {
			if ((k2->bitvector[i] & k1->bitvector[i]) != k2->bitvector[i])
				return kifFALSE;
		}

		return kifTRUE;
	}

	return callfunc->kifcode->Returnerror("BITS(105): Wrong type of parameter in PartOf");
}


KifElement* PreDenominator(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (callfunc->Size() == 1)
		return new KifLong(callfunc->kifcode, NULL, kf->D());

	KifElement* kval = callfunc->Evaluatethread(1, dom, idthread);
	KIFLONG val = kval->Long();
	if (val == 0)
		return callfunc->kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");

	((KifFraction*)kf)->denominator = val;
	((KifFraction*)kf)->Simplify();
	return kval;
}

KifElement* PreNumerator(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (callfunc->Size() == 1) {
		KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
		return new KifLong(callfunc->kifcode, NULL, kf->N());
	}
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kval = callfunc->Evaluatethread(1, dom, idthread);
	((KifFraction*)kf)->numerator = kval->Long();
	((KifFraction*)kf)->Simplify();
	return kval;
}

KifElement* PreGetSTD(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//Reading
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	bool catchoutput = callfunc->Evaluatethread(1, dom, idthread)->Boolean();
	if (catchoutput) {
		if (kifGlobals->kstd != NULL)
			return callfunc->kifcode->Returnerror("KIF(543): Cannot intercept the current standard output");
		kifGlobals->kstd = kstr;
		return kifTRUE;
	}
	else {
		if (kifGlobals->kstd == kstr)
			kifGlobals->kstd = NULL;
	}
	return kifTRUE;
}

KifElement* PreGetERR(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//Reading
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	bool catchoutput = callfunc->Evaluatethread(1, dom, idthread)->Boolean();
	if (catchoutput) {
		if (kifGlobals->kstd != NULL)
			return callfunc->kifcode->Returnerror("KIF(544): Cannot intercept the current error output");
		kifGlobals->kerr = kstr;
		kstr->Setreference();
		return kifTRUE;
	}
	else {
		if (kifGlobals->kerr == kstr) {
			kifGlobals->kerr->Resetreference();
			kifGlobals->kerr = NULL;
		}
	}
	return kifTRUE;
}

void PreLocalPrint(string s) {
	ThreadLock _lock(kifPrint);
	KifString* kval = (KifString*)kifGlobals->kstd;

	if (kval == NULL) {
		if (kifGlobals->displayfunction == NULL)
			*kifGlobals->os << s;
		else
			kifGlobals->displayfunction(s, kifGlobals->displayobject);
		if (kifGlobals->doubledisplay) {
#ifdef DOSOUTPUT
			string sdos = conversion2dos(s);
			cout << sdos;
#else
			cout << s;
#endif
		}
	}
	else
		kval->value += s;
}


KifElement* PreEmojis(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {	
	KifTreeMapIntegerString* kmap = new KifTreeMapIntegerString(callfunc->kifcode, NULL);
	l_emojis(kmap->values);
	return kmap;
}

KifElement* PrePrint(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = kifNULL;
	KifString* kval = (KifString*)kifGlobals->kstd;
	string todisplay;
#ifdef DOSOUTPUT
	bool convert = false;
	if (kifGlobals->os == &cout)
		convert = true;
#endif

	for (int i = 0; i < callfunc->Size(); i++) {
		res = callfunc->Evaluatethread(i, dom, idthread);
		if (Errorid(idthread))
			return Errorptr();
		todisplay += res->StringForDisplay();
	}

	ThreadLock _lock(kifPrint);
	if (kval == NULL) {
		if (kifGlobals->displayfunction == NULL) {
#ifdef DOSOUTPUT
			if (convert) {
				string sdos = conversion2dos(todisplay);
				*kifGlobals->os << sdos;
			}
			else
				*kifGlobals->os << todisplay;
#else
			*kifGlobals->os << todisplay;
#endif
		}
		else
			kifGlobals->displayfunction(todisplay, kifGlobals->displayobject);
		if (kifGlobals->doubledisplay) {
#ifdef DOSOUTPUT
			string sdos = conversion2dos(todisplay);
			cout << sdos;
#else
			cout << todisplay;
#endif
		}
	}
	else
		kval->value += todisplay;

	return kifTRUE;
}


KifElement* PrePrintLN(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = kifNULL;
	KifString* kval = (KifString*)kifGlobals->kstd;
	string todisplay;
#ifdef DOSOUTPUT
	bool convert = false;
	if (kifGlobals->os == &cout)
		convert = true;
#endif

	for (int i = 0; i < callfunc->Size(); i++) {
		if (i)
			todisplay += " ";
		res = callfunc->Evaluatethread(i, dom, idthread);
		if (Errorid(idthread))
			return Errorptr();
		todisplay += res->StringForDisplay();
	}
	todisplay += Endl;

	ThreadLock _lock(kifPrint);
	if (kval == NULL) {
		if (kifGlobals->doubledisplay) {
#ifdef DOSOUTPUT
			string sdos = conversion2dos(todisplay);
			cout << sdos;
#else
			cout << todisplay;
#endif
		}
		if (kifGlobals->displayfunction == NULL) {
#ifdef DOSOUTPUT
			if (convert) {
				string sdos = conversion2dos(todisplay);
				*kifGlobals->os << sdos;
			}
			else
				*kifGlobals->os << todisplay;
#else
			*kifGlobals->os << todisplay;
#endif
		}
		else
			kifGlobals->displayfunction(todisplay, kifGlobals->displayobject);
	}
	else
		kval->value += todisplay;

	return kifTRUE;
}


void toDisplayJN(string& todisplay, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First the container...
	KifElement* kcont = callfunc->Evaluatethread(0, dom, idthread);
	string sep = Endl;
	if (callfunc->Size() > 1)
		sep = callfunc->Evaluatethread(1, dom, idthread)->String();

	if (kcont->aContainer() == false) {
		todisplay = kcont->String();
		todisplay += sep;
	}
	else {
		bool beg = true;
		if (kcont->aVectorContainer()) {
			int it;
			for (it = 0; it < kcont->Size(); it++) {
				if (beg == false)
					todisplay += sep;
				beg = false;
				todisplay += kcont->getstring(it);
			}
		}
		else {
			KifIteration* itr;
			if (kcont->aMapContainer() && callfunc->Size() == 3) {
				itr = (KifIteration*)kcont->Newiterator(false);
				string valsep = callfunc->Evaluatethread(2, dom, idthread)->String();
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					if (beg == false)
						todisplay += valsep;
					beg = false;
					todisplay += itr->IteratorKeyString();
					todisplay += sep;
					todisplay += itr->IteratorValueString();
				}
				itr->Release();
			}
			else {
				itr = (KifIteration*)kcont->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
					if (beg == false)
						todisplay += sep;
					beg = false;
					todisplay += itr->IteratorValueString();
				}
				itr->Release();
			}
		}
	}
}

KifElement* PrePrintJoin(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifString* kval = (KifString*)kifGlobals->kstd;
	string todisplay;
#ifdef DOSOUTPUT
	bool convert = false;
	if (kifGlobals->os == &cout)
		convert = true;
#endif


	toDisplayJN(todisplay, dom, callfunc, idthread);


	ThreadLock _lock(kifPrint);
	if (kval == NULL) {
		if (kifGlobals->doubledisplay) {
#ifdef DOSOUTPUT
			string sdos = conversion2dos(todisplay);
			cout << sdos;
#else
			cout << todisplay;
#endif
		}
		if (kifGlobals->displayfunction == NULL) {
#ifdef DOSOUTPUT
			if (convert) {
				string sdos = conversion2dos(todisplay);
				*kifGlobals->os << sdos;
			}
			else
				*kifGlobals->os << todisplay;
#else
			*kifGlobals->os << todisplay;
#endif
		}
		else
			kifGlobals->displayfunction(todisplay, kifGlobals->displayobject);
	}
	else
		kval->value += todisplay;

	return kifTRUE;
}

KifElement* PrePrintJoinErr(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifString* kval = (KifString*)kifGlobals->kerr;
	string todisplay;

	toDisplayJN(todisplay, dom, callfunc, idthread);

	ThreadLock _lock(kifPrint);
	if (kval == NULL) {
#ifdef DOSOUTPUT
		todisplay = conversion2dos(todisplay);
#endif
		cerr << todisplay;
	}
	else
		kval->value += todisplay;

	return kifTRUE;
}

KifElement* PrePrintJoinLN(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifString* kval = (KifString*)kifGlobals->kstd;
	string todisplay;
#ifdef DOSOUTPUT
	bool convert = false;
	if (kifGlobals->os == &cout)
		convert = true;
#endif

	toDisplayJN(todisplay, dom, callfunc, idthread);

	todisplay += Endl;
	ThreadLock _lock(kifPrint);
	if (kval == NULL) {
		if (kifGlobals->doubledisplay) {
#ifdef DOSOUTPUT
			string sdos = conversion2dos(todisplay);
			cout << sdos;
#else
			cout << todisplay;
#endif
		}
		if (kifGlobals->displayfunction == NULL) {
#ifdef DOSOUTPUT
			if (convert) {
				string sdos = conversion2dos(todisplay);
				*kifGlobals->os << sdos;
			}
			else
				*kifGlobals->os << todisplay;
#else
			*kifGlobals->os << todisplay;
#endif
		}
		else
			kifGlobals->displayfunction(todisplay, kifGlobals->displayobject);
	}
	else
		kval->value += todisplay;

	return kifTRUE;
}

KifElement* PrePrintJoinErrLN(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifString* kval = (KifString*)kifGlobals->kerr;
	string todisplay;

	toDisplayJN(todisplay, dom, callfunc, idthread);

	todisplay += Endl;

	ThreadLock _lock(kifPrint);
	if (kval == NULL) {
#ifdef DOSOUTPUT
		todisplay = conversion2dos(todisplay);
#endif
		cerr << todisplay;
	}
	else
		kval->value += todisplay;

	return kifTRUE;
}

KifElement* PrePrintFlush(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	kifGlobals->os->flush();
	return kifTRUE;
}

KifElement* PrePrinterr(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = kifNULL;
	KifString* kval = (KifString*)kifGlobals->kerr;
	string todisplay;

	for (int i = 0; i < callfunc->Size(); i++) {
		res = callfunc->Evaluatethread(i, dom, idthread);
		if (Errorid(idthread))
			return Errorptr();
		todisplay += res->StringForDisplay();
	}

	ThreadLock _lock(kifPrint);
	if (kval == NULL) {
#ifdef DOSOUTPUT
		todisplay = conversion2dos(todisplay);
#endif
		cerr << todisplay;
	}
	else
		kval->value += todisplay;

	return kifTRUE;
}

KifElement* PrePrinterrLN(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* res = kifNULL;
	KifString* kval = (KifString*)kifGlobals->kerr;

	string todisplay;
	for (int i = 0; i < callfunc->Size(); i++) {
		if (i)
			todisplay += " ";
		res = callfunc->Evaluatethread(i, dom, idthread);
		if (Errorid(idthread))
			return Errorptr();
		todisplay += res->StringForDisplay();
	}

	todisplay += Endl;

	ThreadLock _lock(kifPrint);
	if (kval == NULL) {
#ifdef DOSOUTPUT
		todisplay = conversion2dos(todisplay);
#endif
		cerr << todisplay;
	}
	else
		kval->value += todisplay;
	return kifTRUE;
}

//---------------------------------------------------------------------------------
// File handling functions
KifElement* PreSeek(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* ki = callfunc->Evaluatethread(1, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[162]);
	KifBaseFile* kfile = (KifBaseFile*)kf;
	return kfile->Seek(ki->Integer());
}

KifElement* PreTell(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[164]);
	KifBaseFile* kfile = (KifBaseFile*)kf;
	return kfile->Tell();
}


KifElement* PreWrite(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);

	KifElement* res = kifNULL;

	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[27]);
	KifBaseFile* kfile = (KifBaseFile*)kf;

	for (size_t i = 1; i < callfunc->Size(); i++) {
		res = callfunc->EvaluateString(i, dom, idthread);
		kfile->Write(res->String());
	}
	return kifTRUE;
}

KifElement* PreWriteLN(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[27]);
	KifBaseFile* kfile = (KifBaseFile*)kf;

	KifElement* res = kifNULL;
	string str;
	for (size_t i = 1; i<callfunc->Size(); i++) {
		if (i>1) {
			str = " ";
			kfile->Write(str);
		}
		str = callfunc->EvaluateString(i, dom, idthread)->String();		
		kfile->Write(str);
	}

	str = Endl;
	kfile->Write(str);
	return kifTRUE;
}

KifElement* PreWriteBin(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);

	KifBaseFile* kfile = (KifBaseFile*)kf;
	for (size_t i = 1; i < callfunc->Size(); i++) {
		kf = callfunc->Evaluatethread(i, dom, idthread);
		if (kf->type == kifVectorByte) {
			for (int i = 0; i < kf->Size(); i++)
				kfile->WriteBin(((KifVectorByte*)kf)->values[i]);
		}
		else {
			if (kf->aVectorContainer()) {
				for (int i = 0; i < kf->Size(); i++)
					kfile->WriteBin((uchar)kf->getinteger(i));
			}
			else
				kfile->WriteBin(kf->Byte());
		}
	}
	return kifTRUE;
}

KifElement* PreRead(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[28]);
	KifBaseFile* kfile = (KifBaseFile*)kf;
	int nb = -1;
	if (callfunc->Size() == 2)
		nb = callfunc->Evaluatethread(1, dom, idthread)->Integer();

	if (kfile->type == kifFileWide) {
		KifUString* line = ((KifFileWide*)kfile)->localstring;

		if (contextualpattern->isString()) {
			//we read all the text at once and store it into a string variable
			KifUString* res = callfunc->kifcode->Provideustring(0);
			if (nb == -1) {
				while (kfile->EndOfFile() != kifTRUE) {
					kfile->Readoneline();
					res->value += line->value;
				}
			}
			else {
				kfile->Read(nb);
				res->value = line->value;
			}
			return res;
		}

		long c;
		int counter = 0;
		KifElement* res = Selectavector(callfunc, contextualpattern);
		switch (res->type) {
		case kifVectorByte:
			kfile->readnb(nb, ((KifVectorByte*)res)->values);
			return res;
		case kifVectorInteger:
			kfile->readnb(nb, ((KifVectorInteger*)res)->values);
			return res;
		case kifVectorFloat:
			kfile->readnb(nb, ((KifVectorFloat*)res)->values);
			return res;
			return res;
		case kifVectorString:
		case kifVectorUString:
			while (kfile->EndOfFile() != kifTRUE) {
				kfile->Readoneline();
				res->storevalue(line->value);
				counter++;
				if (counter == nb)
					break;
			}
			return res;
		}

		while (kfile->EndOfFile() != kifTRUE) {
			kfile->Readoneline();
			res->Push(line);
			counter++;
			if (counter == nb)
				break;
		}
		return res;
	}

	KifBaseString* line = ((KifFile*)kfile)->localstring;

	if (contextualpattern->isString()) {
		//we read all the text at once and store it into a string variable
		KifString* res = callfunc->kifcode->Providestringraw("");
		if (nb == -1) {
			while (kfile->EndOfFile() != kifTRUE) {
				kfile->Readoneline();
				res->value += line->value;
			}
		}
		else {
			kfile->Read(nb);
			res->value = line->value;
		}
		return res;
	}

	long c;
	int counter = 0;
	KifElement* res = Selectavector(callfunc, contextualpattern);
	switch (res->type) {
	case kifVectorByte:
		kfile->readnb(nb, ((KifVectorByte*)res)->values);
		return res;
	case kifVectorInteger:
		kfile->readnb(nb, ((KifVectorInteger*)res)->values);
		return res;
	case kifVectorFloat:
		kfile->readnb(nb, ((KifVectorFloat*)res)->values);
		return res;
		return res;
	case kifVectorString:
	case kifVectorUString:
		while (kfile->EndOfFile() != kifTRUE) {
			kfile->Readoneline();
			res->storevalue(line->value);
			counter++;
			if (counter == nb)
				break;
		}
		return res;
	}

	while (kfile->EndOfFile() != kifTRUE) {
		kfile->Readoneline();
		res->Push(line);
		counter++;
		if (counter == nb)
			break;
	}
	return res;
}


KifElement* PreReadOneLine(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[29]);

	KifBaseFile* kfile = (KifBaseFile*)kf;
	return kfile->Readoneline();
}

//close(file)
KifElement* PreClose(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a file
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[30]);

	KifBaseFile* kfile = (KifBaseFile*)kf;
	kfile->Close();
	return kifTRUE;
}

//eof()
KifElement* PreEndOfFile(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a file
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[30]);

	KifBaseFile* kfile = (KifBaseFile*)kf;
	return kfile->EndOfFile();
}

//signature()
KifElement* PreSetSignature(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a file
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[30]);

	KifBaseFile* kfile = (KifBaseFile*)kf;
	bool sgn = callfunc->Evaluatethread(1, dom, idthread)->Boolean();
	kfile->Setsignature(sgn);
	return kifTRUE;
}

KifElement* PreGetSignature(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a file
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[30]);

	KifBaseFile* kfile = (KifBaseFile*)kf;
	return kfile->Getsignature();
}


//openread(file,name)
KifElement* PreOpenReadfile(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a file
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[30]);

	KifBaseFile* kfile = (KifBaseFile*)kf;

	//second parameter is a string
	KifElement* name = callfunc->Evaluatethread(1, dom, idthread);
	if (!kfile->Openread(name->String())) {
		string mess = kifErrorStrings[32] + name->String();
		return callfunc->kifcode->Returnerror(mess);
	}
	return kifTRUE;
}

//openappend(file,name)
KifElement* PreOpenAppendfile(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a file
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[30]);

	KifBaseFile* kfile = (KifBaseFile*)kf;
	//second parameter is a string
	KifElement* name = callfunc->Evaluatethread(1, dom, idthread);
	if (!kfile->Openappend(name->String())) {
		string mess = kifErrorStrings[35] + name->String();
		return callfunc->kifcode->Returnerror(mess);
	}
	return kifTRUE;
}


//openwrite(file,name)
KifElement* PreOpenWritefile(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a file
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[30]);

	KifBaseFile* kfile = (KifBaseFile*)kf;
	//second parameter is a string
	KifElement* name = callfunc->Evaluatethread(1, dom, idthread);
	if (!kfile->Openwrite(name->String())) {
		string mess = kifErrorStrings[35] + name->String();
		return callfunc->kifcode->Returnerror(mess);
	}
	return kifTRUE;
}

KifElement* PreFlushFile(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a file
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (!kf->isfile())
		return callfunc->kifcode->Returnerror(kifErrorStrings[30]);
	KifBaseFile* kfile = (KifBaseFile*)kf;

	if (kfile->Flush())
		return kifTRUE;

	return kifFALSE;
}
//--------------------------------------------------------------------------------------------------------------
KifElement* PreInsert(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	size_t idx;

	//First parameter should be a vector
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->aVectorContainer()) {
		if (callfunc->Size() != 3)
			return callfunc->kifcode->Returnerror(kifErrorStrings[37]);
		//second parameter is anything
		KifElement* kval = callfunc->Evaluatethread(2, dom, idthread);
		idx = callfunc->Evaluatethread(1, dom, idthread)->Integer();
		if (idx >= kf->Size())
			kf->Push(kval);
		else
			kf->Insert(idx, kval);
		return kifTRUE;
	}

	if (kf->type == kifList) {
		//second parameter is anything
		if (callfunc->Size() != 3)
			return callfunc->kifcode->Returnerror(kifErrorStrings[37]);
		KifElement* kval = callfunc->Evaluatethread(2, dom, idthread);
		idx = callfunc->Evaluatethread(1, dom, idthread)->Integer();
		if (idx >= ((KifList*)kf)->values.size())
			((KifList*)kf)->Push(kval);
		else
			((KifList*)kf)->Insert(idx, kval);
		return kifTRUE;
	}

	if (kf->isString()) {
		//second parameter is anything
		idx = -1;
		string val;
		agnostring ret(kf->String());
		if (callfunc->Size() == 3) {
			idx = callfunc->Evaluatethread(1, dom, idthread)->Integer();
			val = callfunc->Evaluatethread(2, dom, idthread)->String();
		}
		else
			val = callfunc->Evaluatethread(1, dom, idthread)->String();

		if (idx == -1)
			ret = ret.insertseparator(val);
		else {
			if (ret.insert(val, idx) == false)
				return kifNULL;
		}
		return callfunc->kifcode->Providestringraw(ret);
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[37]);
}

KifElement* PreJoin(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreJoin");
#endif
	string res;
	bool beg = true;
	string sep;

	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->aVectorContainer()) {
		if (callfunc->Size() != 2)
			return callfunc->kifcode->Returnerror(kifErrorStrings[165]);
		//The separator
		sep = callfunc->Evaluatethread(1, dom, idthread)->String();
		int it;
		for (it = 0; it < kf->Size(); it++) {
			if (beg == false)
				res += sep;
			beg = false;
			res += kf->getstring(it);
		}
		return callfunc->kifcode->Providestring(res);
	}

	if (kf->type == kifList) {
		if (callfunc->Size() != 2)
			return callfunc->kifcode->Returnerror(kifErrorStrings[165]);
		KifList* kvect = (KifList*)kf;
		//The separator
		sep = callfunc->Evaluatethread(1, dom, idthread)->String();
		list<KifElement*>::iterator it;
		for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
			if (beg == false)
				res += sep;
			beg = false;
			res += (*it)->String();
		}
		return callfunc->kifcode->Providestring(res);
	}

	if (kf->aMapContainer()) {
		if (callfunc->Size() != 3)
			return callfunc->kifcode->Returnerror(kifErrorStrings[165]);
		//The separator between keys
		string keysep = callfunc->Evaluatethread(1, dom, idthread)->String();
		//The separator between values
		sep = callfunc->Evaluatethread(2, dom, idthread)->String();
		KifIteration* itr = (KifIteration*)kf->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			if (beg == false)
				res += sep;
			beg = false;
			res += itr->IteratorKeyString();
			res += keysep;
			res += itr->IteratorValueString();
		}

		itr->Release();
		return callfunc->kifcode->Providestring(res);
	}

	if (kf->aContainer()) {
		sep = callfunc->Evaluatethread(1, dom, idthread)->String();
		KifIteration* it = (KifIteration*)kf->Newiterator(false);
		for (it->Begin(); it->End() == kifFALSE; it->Next()) {
			if (!beg)
				res += sep;
			else
				beg = false;
			res += it->IteratorValueString();
		}
		it->Release();
		return callfunc->kifcode->Providestring(res);
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[166]);
}

bool KifCompareInteger(KifElement* e1, KifElement* e2) {
	if (e1->Integer() < e2->Integer())
		return true;
	return false;
}

bool KifCompareLong(KifElement* e1, KifElement* e2) {
	if (e1->Long() < e2->Long())
		return true;
	return false;
}

bool KifCompareFloat(KifElement* e1, KifElement* e2) {
	if (e1->Float() < e2->Float())
		return true;
	return false;
}

bool KifCompareString(KifElement* e1, KifElement* e2) {
	if (e1->String()<e2->String())
		return true;
	return false;
}


bool KifInvCompareInteger(KifElement* e1, KifElement* e2) {
	if (e1->Integer()>e2->Integer())
		return true;
	return false;
}

bool KifInvCompareLong(KifElement* e1, KifElement* e2) {
	if (e1->Long() > e2->Long())
		return true;
	return false;
}

bool KifInvCompareFloat(KifElement* e1, KifElement* e2) {
	if (e1->Float() > e2->Float())
		return true;
	return false;
}

bool KifInvCompareString(KifElement* e1, KifElement* e2) {
	if (e1->String() > e2->String())
		return true;
	return false;
}

KifElement* PreResize(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	int sz = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	if (sz <= 0)
		return kifFALSE;
	if (kf->type == kifTable) {
		KifElement** values = new KifElement*[sz];
		KifTable* ktable = (KifTable*)kf;
		int mx = min(sz, ktable->size);
		int i;
		for (i = 0; i < mx; i++)
			values[i] = ktable->values[i];

		if (mx == sz) {
			for (i = mx; i < ktable->size; i++)
				ktable->values[i]->Resetreference(ktable->reference + 1);
		}
		else {
			//we loose all the other values
			for (i = mx; i < sz; i++)
				values[i] = kifNULL;
		}

		delete[] ktable->values;
		ktable->values = values;
		ktable->size = sz;
		return kifTRUE;
	}

	kf->Reserve(sz);
	return kifTRUE;
}

KifElement* PreSortInt(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->Size() <= 1)
		return kf;

	KifElement* order = callfunc->Evaluatethread(1, dom, idthread);
	//second parameter is an integer (the number of pop)
	if (kf->type == kifVectorInteger || kf->type == kifVectorFloat) {
		KifContainerLock _lock((KifEvaluate*)kf);
		kf->Sort(order->Boolean());
		return kf;
	}

	if (kf->aVector() == false)
		kf = kf->Vector(dom);

	KifVector* kvect = (KifVector*)kf;

	KifContainerLock _lock(kvect);
	if (order->Boolean() == false)
		sort(kvect->values.begin(), kvect->values.end(), KifCompareInteger);
	else
		sort(kvect->values.begin(), kvect->values.end(), KifInvCompareInteger);
	return kvect;
}

KifElement* PreSortFloat(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->Size() <= 1)
		return kf;

	KifElement* order = callfunc->Evaluatethread(1, dom, idthread);

	if (kf->type == kifVectorInteger || kf->type == kifVectorFloat) {
		KifContainerLock _lock((KifEvaluate*)kf);
		kf->Sort(order->Boolean());
		return kf;
	}

	//second parameter is an integer (the number of pop)
	if (kf->aVector() == false)
		kf = kf->Vector(dom);

	KifVector* kvect = (KifVector*)kf;
	KifContainerLock _lock(kvect);
	if (order->Boolean() == false)
		sort(kvect->values.begin(), kvect->values.end(), KifCompareFloat);
	else
		sort(kvect->values.begin(), kvect->values.end(), KifInvCompareFloat);
	return kvect;
}

KifElement* PreSortLong(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->Size() <= 1)
		return kf;

	KifElement* order = callfunc->Evaluatethread(1, dom, idthread);

	if (kf->type == kifVectorInteger || kf->type == kifVectorFloat) {
		KifContainerLock _lock((KifEvaluate*)kf);
		kf->Sort(order->Boolean());
		return kf;
	}

	//second parameter is an integer (the number of pop)
	if (kf->aVector() == false)
		kf = kf->Vector(dom);

	KifVector* kvect = (KifVector*)kf;
	KifContainerLock _lock(kvect);
	if (order->Boolean() == false)
		sort(kvect->values.begin(), kvect->values.end(), KifCompareLong);
	else
		sort(kvect->values.begin(), kvect->values.end(), KifInvCompareLong);
	return kvect;
}

KifElement* PreSortString(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->Size() <= 1)
		return kf;

	KifElement* order = callfunc->Evaluatethread(1, dom, idthread);

	if (kf->type == kifVectorString || kf->type == kifVectorUString) {
		KifContainerLock _lock((KifEvaluate*)kf);
		kf->Sort(order->Boolean());
		return kf;
	}

	//second parameter is an integer (the number of pop)
	if (kf->aVector() == false)
		kf = kf->Vector(dom);

	KifVector* kvect = (KifVector*)kf;
	KifContainerLock _lock(kvect);
	if (order->Boolean() == false)
		sort(kvect->values.begin(), kvect->values.end(), KifCompareString);
	else
		sort(kvect->values.begin(), kvect->values.end(), KifInvCompareString);
	return kvect;
}

KifElement* PreUnique(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	return kf->Unique();
}

KifElement* PrePermute(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (callfunc->Evaluatethread(0, dom, idthread)->Permute())
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreShuffle(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	callfunc->Evaluatethread(0, dom, idthread)->Shuffle();
	return kifTRUE;
}

KifElement* PreItems(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	kf->Items(kvect);
	return kvect;
}

KifElement* PreSort(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	//second parameter is an integer (the number of pop)
	if (!kf->aContainer())
		return kifFALSE;

	if (kf->Size() <= 1)
		return kf;

	if (kf->aMapContainer()) {
		vector<KifVector*> v;

		KifIteration* itr = (KifIteration*)kf->Newiterator(false);
		KifVector* m;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			m = new KifVector(NULL, NULL);
			m->kifcode = callfunc->kifcode;
			m->Push(itr->IteratorKey());
			m->Push(itr->IteratorValue());
			v.push_back(m);
		}
		itr->Release();

		int i;
		kf = callfunc->Evaluatethread(1, dom, idthread)->Functioncall(dom);
		KifElement* mp;
		if (contextualpattern->type >= kifPrimeMap && contextualpattern->type <= kifPrimeMapFloatUString) {
			mp = contextualpattern;
			mp->Clear();
		}
		else
			mp = new KifPrimeMap(callfunc->kifcode, NULL);
		if (kf != NULL && kf->isFunction()) {
			KifComparison kfcomp(dom, (KifFunction*)kf);
			sort(v.begin(), v.end(), kfcomp);
			for (i = 0; i < v.size(); i++)
				mp->Push(v[i]->values[0], v[i]->values[1]);
		}
		for (i = 0; i < v.size(); i++) {
			v[i]->Clear();
			delete v[i];
		}
		return mp;
	}


	if (kf->aVector()) {
		KifVector* kvect = (KifVector*)kf;
		//Second parameter should be function
		if (callfunc->Size() == 2) {
			kf = callfunc->Evaluatethread(1, dom, idthread)->Functioncall(dom);
			if (kf != NULL && kf->isFunction()) {
				KifComparison kfcomp(dom, (KifFunction*)kf);
				sort(kvect->values.begin(), kvect->values.end(), kfcomp);
				return kvect;
			}
		}
		else {
			//we use the first element as a trigger
			KifElement* kfirst = kvect->values[0];
			switch (kfirst->type) {
			case kifInteger:
				sort(kvect->values.begin(), kvect->values.end(), KifCompareInteger);
				break;
			case kifLong:
				sort(kvect->values.begin(), kvect->values.end(), KifCompareLong);
				break;
			case kifFraction:
			case kifFloat:
				sort(kvect->values.begin(), kvect->values.end(), KifCompareFloat);
				break;
			default: //else we only use strings for other types
				sort(kvect->values.begin(), kvect->values.end(), KifCompareString);
			}
			return kvect;
		}
		return kvect;
	}

	if (kf->type == kifVectorString && callfunc->Size() == 2) {
		KifVectorString* kvect = (KifVectorString*)kf;
		KifContainerLock _lock(kvect);
		kf = callfunc->Evaluatethread(1, dom, idthread)->Functioncall(dom);
		if (kf != NULL && kf->isFunction()) {
			KifBaseString li(NULL, NULL, "");
			KifBaseString lj(NULL, NULL, "");
			li.kifcode = dom->kifcode;
			lj.kifcode = dom->kifcode;
			KifComparisonString kfcomp(dom, (KifFunction*)kf, &li, &lj);
			sort(kvect->values.begin(), kvect->values.end(), kfcomp);
		}
		return kvect;
	}

	if (kf->type == kifVectorInteger && callfunc->Size() == 2) {
		KifVectorInteger* kvect = (KifVectorInteger*)kf;
		KifContainerLock _lock(kvect);
		kf = callfunc->Evaluatethread(1, dom, idthread)->Functioncall(dom);
		if (kf != NULL && kf->isFunction()) {
			KifBaseInteger li(NULL, NULL, 0);
			KifBaseInteger lj(NULL, NULL, 0);
			li.kifcode = dom->kifcode;
			lj.kifcode = dom->kifcode;
			KifComparisonInteger kfcomp(dom, (KifFunction*)kf, &li, &lj);
			sort(kvect->values.begin(), kvect->values.end(), kfcomp);
		}
		return kvect;
	}

	if (kf->type == kifVectorFloat && callfunc->Size() == 2) {
		KifVectorFloat* kvect = (KifVectorFloat*)kf;
		KifContainerLock _lock(kvect);
		kf = callfunc->Evaluatethread(1, dom, idthread)->Functioncall(dom);
		if (kf != NULL && kf->isFunction()) {
			KifBaseFloat li(NULL, NULL, 0);
			KifBaseFloat lj(NULL, NULL, 0);
			li.kifcode = dom->kifcode;
			lj.kifcode = dom->kifcode;
			KifComparisonFloat kfcomp(dom, (KifFunction*)kf, &li, &lj);
			sort(kvect->values.begin(), kvect->values.end(), kfcomp);
		}
		return kvect;
	}

	kf->Sort(true);
	return kf;
}

KifElement* PreSum(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreSum");
#endif

	//First parameter should be a vector
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	//second parameter is an integer (the number of pop)
	double v = kf->Sum();
	return callfunc->kifcode->Providefloat(v);
}

KifElement* PreValues(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a map
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	return kf->Getvalues(callfunc, contextualpattern);
}


KifElement* PreKeys(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a map
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	return kf->Getkeys(callfunc, contextualpattern);
}


KifElement* PreInkey(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a map
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	kf = kifGlobals->getloopkey(kf->idgarbage);
	if (kf == NULL)
		return kifNOELEMENT;
	return ((KifLoopKey*)kf)->Key();
}

KifElement* PreProduct(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreProduct");
#endif

	//First parameter should be a vector
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->Size() == 0)
		return callfunc->kifcode->Providefloat(0);

	double v = kf->Product();
	return callfunc->kifcode->Providefloat(v);
}

//If it is a container, we return it, otherwise it will be a vector...
KifElement* Selectcontainer(KifElement* callfunc, KifElement* context) {
	if (context != NULL && !context->aValueContainer() && context->reference <= 1 && context != callfunc && context->Assigning(context->type)) {
		context->Clear();
		return context;
	}

	if (context->aMapContainer())
		return new KifMap(callfunc->kifcode, NULL);

	return new KifVector(callfunc->kifcode, NULL);
}



KifElement* PreApply(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a XIP rule
	KifElement* kfuncelement = callfunc->Evaluatethread(0, dom, idthread);

	if (kfuncelement->type == kifGrammar) {
		KifElement* kvect = Selectcontainer(callfunc, contextualpattern);

		KifElement* kval = callfunc->Evaluatethread(1, dom, idthread);
		if (kval->type == kifVectorUString)
			return ((KifGrammar*)kfuncelement)->Apply(idthread, kvect, dom, ((KifVectorUString*)kval)->values, false);

		//Else we build our vector out of the characters...
		if (kval->aVectorContainer()) {
			vector<wstring> v;
			for (int i = 0; i < kval->Size(); i++)
				v.push_back(kval->getustring(i));
			return ((KifGrammar*)kfuncelement)->Apply(idthread, kvect, dom, v, false);
		}

		wstring wstr = kval->UString();
		return ((KifGrammar*)kfuncelement)->Apply(idthread, kvect, dom, wstr, true);
	}

	if (kfuncelement->type == kifCall || kfuncelement->isFunction())
		return kfuncelement->Exec(contextualpattern, callfunc, kfuncelement, idthread, callfunc);

	KifElement* kf;
	if (kfuncelement->type == kifIterator) {
		kf = kfuncelement->IteratorValue();
		return kf->Exec(contextualpattern, callfunc, kf, idthread, callfunc);
	}


	KifCallFunctionGeneral* kfunclocal = NULL;
	if (callfunc->Size() >= 2) {
		kf = callfunc->Evaluatethread(1, dom, idthread);
		//then it is a simple parameter
		if (kf->type == kifCall || kf->isFunction()) {
			KifFunction* thefunction = kf->Functioncall(dom);
			kfunclocal = new KifCallFunctionGeneral(NULL, NULL, thefunction->Name(), thefunction);
			kfunclocal->Setline(thefunction->Line(), thefunction->Fileindex());
			kfunclocal->kifcode = dom->kifcode;
			kfunclocal->parameters.push_back(kifNULL);
			if (kfuncelement->aMapContainer()) {
				kfunclocal->parameters.push_back(kifNULL);
			}
			for (int i = 2; i < callfunc->Size(); i++)
				kfunclocal->parameters.push_back(callfunc->parameters[i]);
			kf = kfuncelement->apply(kfunclocal, contextualpattern, dom, callfunc);
			delete kfunclocal;
			return kf;
		}
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[49]);
}

KifElement* PreClear(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	if (kf->type == kifTime) {
		KifTime* ktime = (KifTime*)kf;
		gettimeofday(&ktime->value, NULL);
		return ktime;
	}

	//First parameter should be a vector
	kf->Clear();
	return kifTRUE;
}

KifElement* PreTreeRemove(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	return kfirst->TreeRemove();
}

KifElement* PreTreeIsolate(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	return kfirst->TreeExtract(NULL);
}

KifElement* PreExtract(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* pos = callfunc->Evaluatethread(1, dom, idthread);
	KifElement* from = callfunc->Evaluatethread(2, dom, idthread);
	if (kfirst->type == kifUString) {
		vector<wstring> v;
		int i;
		for (i = 3; i < callfunc->Size(); i++)
			v.push_back(callfunc->Evaluatethread(i, dom, idthread)->UString());
		wstring init = kfirst->UString();
		i = pos->Integer();
		wstring sfrom = from->UString();
		i = init.find(sfrom, i);
		int lgfrom = sfrom.size();
		KifElement* vect = Selectvectorustring(callfunc, contextualpattern);
		int nb, npos;

		while (i != -1) {
			npos = -1;
			for (int j = 0; j < v.size(); j++) {
				int spos = init.find(v[j], i + lgfrom);
				if (spos != -1 && (npos == -1 || spos < npos))
					npos = spos;
			}
			if (npos == -1)
				break;
			nb = npos - i - lgfrom;
			vect->storevalue(init.substr(i + lgfrom, nb));
			i = init.find(sfrom, npos + 1);
		}

		return vect;
	}

	vector<string> v;
	int i;
	for (i = 3; i < callfunc->Size(); i++)
		v.push_back(callfunc->Evaluatethread(i, dom, idthread)->String());
	string init = kfirst->String();
	i = pos->Integer();
	string sfrom = from->String();
	i = init.find(sfrom, i);
	int lgfrom = sfrom.size();
	KifElement* vect = Selectvectorstring(callfunc, contextualpattern);
	int nb, npos;

	while (i != -1) {
		npos = -1;
		for (int j = 0; j < v.size(); j++) {
			int spos = init.find(v[j], i + lgfrom);
			if (spos != -1 && (npos == -1 || spos < npos))
				npos = spos;
		}
		if (npos == -1)
			break;
		nb = npos - i - lgfrom;
		vect->storevalue(init.substr(i + lgfrom, nb));
		i = init.find(sfrom, npos + 1);
	}

	return vect;
}

KifElement* PreTreeNext(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (callfunc->Size() == 1)
		return kfirst->TreeNext();

	KifElement* kelement = callfunc->Evaluatethread(1, dom, idthread);
	return kfirst->TreeAddnext((KifTree*)kelement);
}


KifElement* PreTreePrevious(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (callfunc->Size() == 1)
		return kfirst->TreePrevious();

	KifElement* kelement = callfunc->Evaluatethread(1, dom, idthread);
	return kfirst->TreeAddprevious((KifTree*)kelement);
}

KifElement* PreTreeParent(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kval = kfirst->TreeParent();
	if (callfunc->Size() == 1)
		return kval;
	else {
		KifElement* kelement = callfunc->Evaluatethread(1, dom, idthread);
		if (kval == kelement)
			return kifTRUE;
	}
	return kifFALSE;
}

KifElement* PreTreeChild(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (callfunc->Size() == 1)
		return kfirst->TreeChild();

	KifElement* kelement = callfunc->Evaluatethread(1, dom, idthread);
	return kfirst->TreeAddchild((KifTree*)kelement);
}

KifElement* PreTreeLast(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kval = kfirst->TreeLast();
	if (callfunc->Size() == 1)
		return kval;
	else {
		KifElement* kelement = callfunc->Evaluatethread(1, dom, idthread);
		if (kval == kelement)
			return kifTRUE;
	}
	return kifFALSE;
}

KifElement* PreTreeDepth(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kroot = callfunc->Evaluatethread(0, dom, idthread);
	return callfunc->kifcode->Provideinteger(kroot->Depth());
}

KifElement* PreTreeAddChild(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kroot = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kelement = callfunc->Evaluatethread(1, dom, idthread);
	if (kelement->type != kifTree)
		return callfunc->kifcode->Returnerror(kifErrorStrings[201]);
	return kroot->TreeAddchild((KifTree*)kelement);
}

KifElement* PreTreeAddNext(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kroot = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kelement = callfunc->Evaluatethread(1, dom, idthread);
	if (kelement->type != kifTree)
		return callfunc->kifcode->Returnerror(kifErrorStrings[201]);
	return kroot->TreeAddnext((KifTree*)kelement);
}

KifElement* PreTreeAddPrevious(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kroot = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kelement = callfunc->Evaluatethread(1, dom, idthread);
	if (kelement->type != kifTree)
		return callfunc->kifcode->Returnerror(kifErrorStrings[201]);
	return kroot->TreeAddprevious((KifTree*)kelement);
}

KifElement* PreSize(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreSize");
#endif
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	long sz = kfirst->Size();
	KifElement* ksz = callfunc->kifcode->Provideinteger(sz);
	return ksz;
}

KifElement* PreReverse(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
#ifdef PROFILING
	profilingcall("PreReverse");
#endif
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);

	if (kfirst->aValueVector())
		return kfirst->Inverse();

	if (kfirst->aVector()) {
		KifVector* kvectres = new KifVector(callfunc->kifcode, NULL);
		for (int it = kfirst->Size() - 1; it >= 0; it--) {
			kvectres->Push(kfirst->ValueOnIndex(it));
		}
		return kvectres;
	}

	if (kfirst->type == kifList) {
		KifList* kvect = (KifList*)kfirst;
		list<KifElement*>::reverse_iterator it;
		KifList* kvectres = new KifList(callfunc->kifcode, NULL);
		for (it = kvect->values.rbegin(); it != kvect->values.rend(); it++) {
			kvectres->Push(*it);
		}
		return kvectres;
	}

	if (kfirst->type == kifUString) {
		wstring s = kfirst->UString();
		return callfunc->kifcode->Provideustring(s_revert(s));
	}

	agnostring s(kfirst->String());
	return callfunc->kifcode->Providestringraw(s.revert());
}


KifElement* PreRemovefirst(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* knb = callfunc->Evaluatethread(1, dom, idthread);
	int id = knb->Integer();
	if (kstr->type == kifUString) {
		wstring s = kstr->UString();
		//On retire en tete
		int iv = s.size();
		if (iv > 0 && id <= iv) {
			s = s_right(s, iv - id);
			return callfunc->kifcode->Provideustring(s);
		}
	}
	else {
		agnostring s(kstr->String());
		//On retire en tete
		int iv = s.sizec();

		if (iv > 0 && id <= iv) {
			s = s.right(iv - id);
			return callfunc->kifcode->Providestring(s);
		}
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[85]);
}

KifElement* PreRemovelast(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* knb = callfunc->Evaluatethread(1, dom, idthread);
	int id = knb->Integer();

	if (kstr->type == kifUString) {
		wstring s = kstr->UString();
		int iv = s.size();

		if (iv > 0 && id <= iv) {
			s = s_left(s, iv - id);
			return callfunc->kifcode->Provideustring(s);
		}
	}
	else {
		agnostring s(kstr->String());
		//On retire en tete
		int iv = s.sizec();

		if (iv > 0 && id <= iv) {
			s = s.left(iv - id);
			return callfunc->kifcode->Providestring(s);
		}
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[126]);
}

KifElement* PreUnGet(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	int nb = 1;
	if (callfunc->Size() == 2) {
		KifElement* knb = callfunc->Evaluatethread(1, dom, idthread);
		nb = knb->Integer();
	}


	if (kfirst->isfile()) {
		KifBaseFile* kfile = (KifBaseFile*)kfirst;
		if (kfile->mode == 1) {
			while (nb > 0) {
				kfile->unget();
				nb--;
			}
			kfile->clear();
			return kifTRUE;
		}
		return kifFALSE;
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[174]);
}

#ifdef WIN32
static bool errorsprintf = false;
void wrongSprintf(const wchar_t* expression,
	const wchar_t* function,
	const wchar_t* file,
	unsigned int line,
	uintptr_t pReserved) {
	errorsprintf = true;
}
#endif

KifElement* PreEven(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	long num = callfunc->Evaluatethread(0, dom, idthread)->Integer();
	if ((num % 2) == 0)
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreOdd(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	long num = callfunc->Evaluatethread(0, dom, idthread)->Integer();
	if ((num % 2) == 0)
		return kifFALSE;
	return kifTRUE;
}

KifElement* PreRadian(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	double num = callfunc->Evaluatethread(0, dom, idthread)->Float();
	num = M_PI*(num / 180);
	return callfunc->kifcode->Providefloat(num);
}

KifElement* PreDegree(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	double num = callfunc->Evaluatethread(0, dom, idthread)->Float();
	num = (num * 180) / M_PI;
	return callfunc->kifcode->Providefloat(num);
}


static bool checktimeformat(string& format, string& err) {
	static char tableformat[] = { 'a', 'A', 'b', 'B', 'c', 'd', 'H', 'I', 'j', 'm', 'M', 'p', 'S', 'U', 'w', 'W', 'x', 'X', 'y', 'Y', 'Z', 'z', 0 };
	vector<string> vs;
	v_split(format, "%", vs);	
	if (vs.size() == 0) {
		err += "empty";
		return false;
	}
	
	for (long i = 0; i < vs.size(); i++) {
		if (!strchr(tableformat, vs[i][0])) {
			err += "%";
			err += vs[i][0];
			return false;
		}
	}
	return true;
}

KifElement* PreFormat(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* knum = callfunc->Evaluatethread(0, dom, idthread);

	if (knum->type == kifUString) {
		wchar_t buffer[101];
		wstring sformat = knum->UString();
		//In this case, we might have more than two parameters...
		wstring e;
		buffer[0] = L'%';
		for (int i = callfunc->Size() - 1; i >= 1; i--) {
			e = callfunc->Evaluatethread(i, dom, idthread)->UString();
			swprintf_s(buffer + 1, 100, L"%d", i);
			sformat = s_replacestring(sformat, buffer, e);
		}
		return callfunc->kifcode->Provideustring(sformat);
	}

	char buffer[1025];
	if (knum->isString()) {
		string sformat = knum->String();
		//In this case, we might have more than two parameters...
		string e;
		buffer[0] = '%';
		for (int i = callfunc->Size() - 1; i >= 1; i--) {
			e = callfunc->Evaluatethread(i, dom, idthread)->String();
			sprintf_s(buffer + 1, 1024, "%d", i);
			sformat = s_replacestrings(sformat, buffer, e);
		}
		return callfunc->kifcode->Providestring(sformat);
	}

	KifElement* kformat = callfunc->Evaluatethread(1, dom, idthread);
	string sformat = kformat->String();
	if (knum->type == kifDate) {
#ifdef WIN32
		string err = "KIF(124):Incorrect date format specifier: ";
		if (!checktimeformat(sformat, err))
			return callfunc->kifcode->Returnerror(err);
#endif

		KifDate* ktime = (KifDate*)knum;
		struct tm* temps = localtime(&ktime->value);
		if (strftime(buffer, 1024, STR(sformat), temps) < 0)
			return callfunc->kifcode->Returnerror("KIF(124):Incorrect date format specifier");
		return callfunc->kifcode->Providestringraw(buffer);
	}

#ifdef WIN32
	_invalid_parameter_handler oldHandler = _set_invalid_parameter_handler(wrongSprintf);

	errorsprintf = false;
	switch (knum->type) {
	case kifInteger:
		sprintf_s(buffer, 1024, STR(sformat), knum->Integer());
		break;
	case kifLong:
		sprintf_s(buffer, 1024, STR(sformat), knum->Long());
		break;
	case kifFraction:
	case kifFloat:
		sprintf_s(buffer, 1024, STR(sformat), knum->Float());
		break;
	default:
		sprintf_s(buffer, 1024, STR(sformat), knum->Integer());
	}
	_set_invalid_parameter_handler(oldHandler);
	if (errorsprintf == true) {
		errorsprintf = false;
		return callfunc->kifcode->Returnerror("KIF(124):Incorrect format specifier or size too long");
	}
#else
	int spres;
	switch (knum->type) {
	case kifInteger:
		spres = sprintf_s(buffer, 100, STR(sformat), knum->Integer());
		break;
	case kifLong:
		spres = sprintf_s(buffer, 100, STR(sformat), knum->Long());
		break;
	case kifFraction:
	case kifFloat:
		spres = sprintf_s(buffer, 100, STR(sformat), knum->Float());
		break;
	default:
		spres = sprintf_s(buffer, 100, STR(sformat), knum->Integer());
	}
	if (spres<0)
		return callfunc->kifcode->Returnerror("KIF(124):Incorrect format specifier or size too long");
#endif
	return callfunc->kifcode->Providestringraw(buffer);
}

KifElement* PostEval(KifCode* kifcode, string& code, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//bool pushed=kifGlobals->Pushlocal(kifcode->kstart);
	int idstack = kifGlobals->PushFunctionStackid(idthread, dom, dom);
	KifInstructionSEQUENCE ki(NULL, NULL, idsequence);
	ki.kifcode = kifcode;
	//We deactivate the garbage temporary (to avoid problems)

	kifGlobals->nogarbage = true;
	kifcode->Setbaseline(1);
	{
		ThreadLock _lock(kifGarbage);
		kifcode->Evaluate(&ki, code, false);
	}
	//if (pushed)
	//	kifGlobals->PopStack();
	kifGlobals->PopFunctionStack(idstack);
	KifElement* ret = ki.Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
	//we reactivate it...
	kifGlobals->nogarbage = false;
	return ret;
}

KifElement* PreEval(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	string code = callfunc->Evaluatethread(0, dom, idthread)->String();
	return PostEval(callfunc->kifcode, code, contextualpattern, dom, callfunc, idthread);
}

KifElement* PreOpenSession(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifCode* kifcode = new KifCode(kifGlobals->trigger, kifGlobals->data);
	KifElement* kkif = callfunc->Evaluatethread(0, dom, idthread);
	if (((KifKiF*)kkif)->id != -1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[221]);
	((KifKiF*)kkif)->id = kifcode->idkifmain;
	kifcode->session = true;
	KifExternalModule LibInitEntryPoint;
	hmap<string, KifExternalModule>::iterator it;
	for (it = kifGlobals->kifLibraries.begin(); it != kifGlobals->kifLibraries.end(); it++) {
		LibInitEntryPoint = it->second;
		(*LibInitEntryPoint)(kifcode, KIFVERSION);
	}
	return kifTRUE;
}

KifElement* PreCloseSession(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kkif = callfunc->Evaluatethread(0, dom, idthread);
	int i = ((KifKiF*)kkif)->id;

	KifCode* kifcode = kifGlobals->kifCodeVector[i];
	if (kifcode == NULL || kifcode == callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);

	((KifKiF*)kkif)->id = -1;
	delete kifcode;

	return kifTRUE;
}

KifElement* PreCompile(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kkif = callfunc->Evaluatethread(0, dom, idthread);
	int i = ((KifKiF*)kkif)->id;

	KifCode* kifcode = kifGlobals->kifCodeVector[i];
	if (kifcode == NULL || kifcode == callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);

	int firstinstruction = kifcode->kstart->instructions.size();
	int igarbage = kifGlobals->garbage.size();
	int ilocal = kifcode->locals.size();
	int cursor = kifGlobals->cursor;
	KifElement* kfirst = callfunc->Evaluatethread(1, dom, idthread);
	string code = kfirst->String();
	KifElement* kname = kifNULL;
	if (callfunc->Size() == 3)
		kname = callfunc->Evaluatethread(2, dom, idthread);

	bin_hash<KifElement*> stack;
	bin_hash<KifElement*>::iterator it;
	for (it = kifcode->kstart->declarations.begin(); it != kifcode->kstart->declarations.end(); it++)
		stack[it->first] = it->second;

	vector<KifDomain*> keepstack;
	int id = kifGlobals->GetThreadid();
	for (i = 0; i < kifGlobals->THR[id]->threadstacks.size(); i++)
		keepstack.push_back(kifGlobals->THR[id]->threadstacks[i]);
	kifGlobals->THR[id]->threadstacks.clear();

	kifcode->kstart->adding = true;
	kifcode->addtolocal = true;
	kifGlobals->PushStack(kifcode->kstart);
	//We evaluate in kstart...
	kifcode->Setbaseline(1);
	kifcode->Evaluate(kifcode->kstart, code, false);
	kifGlobals->PopStack();
	kifcode->addtolocal = false;
	for (i = 0; i < keepstack.size(); i++)
		kifGlobals->THR[id]->threadstacks.push_back(keepstack[i]);
	int j;
	//then from igarbage up to the end, we reference our elements
	if (Errorid(id) == true) {
		string error = Errorstring();
		kifGlobals->THR[id]->cleanerror();
		for (i = firstinstruction; i < kifcode->kstart->instructions.size(); i++)
			kifcode->kstart->instructions.pop_back();
		kifcode->kstart->declarations.clear();
		for (it = stack.begin(); it != stack.end(); it++)
			kifcode->kstart->declarations[it->first] = it->second;

		for (i = kifcode->locals.size() - 1; i >= ilocal; i--) {
			j = kifcode->locals[i];
			kifcode->locals.pop_back();
			if (kifGlobals->garbage[j] != NULL) {
				delete kifGlobals->garbage[j];
				kifGlobals->garbage[j] = NULL;
			}
		}

		kifGlobals->garbage.dernier = igarbage;
		kifGlobals->cursor = cursor;
		return callfunc->kifcode->Returnerror(error);
	}

	for (i = 0; i < kifcode->locals.size(); i++) {
		j = kifcode->locals[i];
		if (kifGlobals->garbage[j] != NULL) {
			kifGlobals->garbage[j]->Setpopped();
			kifGlobals->garbage[j]->Setreference();
		}
	}

	if (kname != kifNULL) {
		code = kname->String();
		DebugPtr()->AddFilename(code, kifcode);
	}

	return callfunc->kifcode->Provideinteger(firstinstruction);
}

KifElement* PreRun(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kkif = callfunc->Evaluatethread(0, dom, idthread);
	int i = ((KifKiF*)kkif)->id;
	KifCode* kifcode = kifGlobals->kifCodeVector[i];
	if (kifcode == NULL || kifcode == callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);
	int firstinstruction = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	kifGlobals->endofexecution = false;
	vector<KifDomain*> keepstack;
	int id = kifGlobals->GetThreadid();
	for (i = 0; i < kifGlobals->THR[id]->threadstacks.size(); i++)
		keepstack.push_back(kifGlobals->THR[id]->threadstacks[i]);
	kifGlobals->THR[id]->threadstacks.clear();

	kifcode->kstart->Run(firstinstruction);

	for (i = 0; i < keepstack.size(); i++)
		kifGlobals->THR[id]->threadstacks.push_back(keepstack[i]);

	kifGlobals->endofexecution = true;
	return kifTRUE;
}

KifElement* PreRunAsThread(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kkif = callfunc->Evaluatethread(0, dom, idthread);
	int i = ((KifKiF*)kkif)->id;
	KifCode* kifcode = kifGlobals->kifCodeVector[i];
	if (kifcode == NULL || kifcode == callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);
	int firstinstruction = callfunc->Evaluatethread(1, dom, idthread)->Integer();
	vector<KifDomain*> keepstack;
	int id = kifGlobals->GetThreadid();
	for (i = 0; i < kifGlobals->THR[id]->threadstacks.size(); i++)
		keepstack.push_back(kifGlobals->THR[id]->threadstacks[i]);
	kifGlobals->THR[id]->threadstacks.clear();

	kifGlobals->endofexecution = false;
	kifcode->kstart->RunThread(firstinstruction);

	for (i = 0; i < keepstack.size(); i++)
		kifGlobals->THR[id]->threadstacks.push_back(keepstack[i]);

	return kifTRUE;
}


KifElement* PreRunEnd(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kkif = callfunc->Evaluatethread(0, dom, idthread);
	int i = ((KifKiF*)kkif)->id;
	KifCode* kifcode = kifGlobals->kifCodeVector[i];
	if (kifcode == NULL || kifcode == callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);
	if (kifGlobals->endofexecution)
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreRunStart(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	int i;
	if (callfunc->Size() == 0)
		i = callfunc->kifcode->idkifmain;
	else {
		KifElement* kkif = callfunc->Evaluatethread(0, dom, idthread);
		i = ((KifKiF*)kkif)->id;
	}
	KifCode* kifcode = kifGlobals->kifCodeVector[i];
	if (kifcode == NULL)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);
	return kifcode->kstart->Map(contextualpattern);
}


KifElement* PreSleep(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	int res = kfirst->Integer();
#ifdef WIN32
	Sleep(res);
#else
	sleep(res);
#endif
	return kifTRUE;
}



KifElement* PreBegin(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifIterator)
		return kfirst->Begin();
	return callfunc->kifcode->Returnerror(kifErrorStrings[132]);
}

KifElement* PreEnd(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifIterator)
		return kfirst->End();

	return callfunc->kifcode->Returnerror(kifErrorStrings[133]);
}

KifElement* PreNend(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifIterator) {
		if (kfirst->End()->Boolean() == true)
			return kifFALSE;
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[134]);
}

KifElement* PreIteratorKey(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifIterator) {
		KifElement* ke = kfirst->IteratorKey();
		if (ke == NULL)
			return kifNULL;
		return ke;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[135]);
}

KifElement* PreValue(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifIterator) {
		if (callfunc->Size() == 1)
			return kfirst->IteratorValue();
		KifElement* value = callfunc->Evaluatethread(1, dom, idthread);
		kfirst->IteratorSetvalue(value);
		return kifTRUE;
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[136]);
}

KifElement* PreValueType(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifIterator) {
		KifElement* ke = kfirst->IteratorValue();
		string resstr = ke->Type();
		return callfunc->kifcode->Providestring(resstr);
	}
	else {
		string resstr = kfirst->Type();
		return callfunc->kifcode->Providestring(resstr);
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[137]);
}

KifElement* PreValueTypeIs(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* typ = callfunc->Evaluatethread(1, dom, idthread);
	string resstr;
	if (kfirst->type == kifIterator) {
		KifElement* ke = kfirst->IteratorValue();
		resstr = ke->Type();
	}
	else
		resstr = kfirst->Type();

	string typstr = typ->String();
	if (resstr == typstr)
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreReplace(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//Replace an element with another
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);

	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		wstring reg = callfunc->Evaluatethread(1, dom, idthread)->UString();
		if (reg == L"")
			return callfunc->kifcode->Provideustring(str);

		wstring rep = callfunc->Evaluatethread(2, dom, idthread)->UString();

		str = s_replacestring(str, reg, rep);
		return callfunc->kifcode->Provideustring(str);
	}

	string str = kstr->String();
	string reg = callfunc->Evaluatethread(1, dom, idthread)->String();
	if (reg == "")
		return callfunc->kifcode->Providestring(str);

	string rep = callfunc->Evaluatethread(2, dom, idthread)->String();

	str = s_replacestring(str, reg, rep);
	return callfunc->kifcode->Providestring(str);
}

//--------------------------------Levenshtein: Edit Distance------------------------------------------------
KifElement* PreLevenshtein(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (callfunc->Size() == 2 || callfunc->Size() == 3) {
		KifElement* e1 = callfunc->Evaluatethread(0, dom, idthread);
		KifElement* e2 = callfunc->Evaluatethread(1, dom, idthread);
		if (e1->type == kifUString)
			return callfunc->kifcode->Provideinteger(e1->EditDistance(e2));

		bool bytecall = false;
		if (callfunc->Size() == 3)
			bytecall = callfunc->Evaluatethread(2, dom, idthread)->Boolean();

		if (!bytecall && e1->isString()) {
			KifBaseUString e(NULL, NULL, e1->UString());
			return callfunc->kifcode->Provideinteger(e.EditDistance(e2));
		}

		return callfunc->kifcode->Provideinteger(e1->EditDistance(e2));
	}

	KifElement* au = callfunc->Evaluatethread(0, dom, idthread);
	if (au->type != kifAutomaton)
		return callfunc->kifcode->Returnerror("KIF(635): This function must be called with an automaton variable");

	KifElement* e = callfunc->Evaluatethread(1, dom, idthread);
	KifElement* threshold = callfunc->Evaluatethread(2, dom, idthread);
	KifElement* flags = callfunc->Evaluatethread(3, dom, idthread);
	return ((KifAutomaton*)au)->editdistance(contextualpattern, dom, e, flags, threshold);
}
//-----------------------------------------Edit distance based on binary comparison ----------------------------
void affiche(uchar a, KIFLONG v) {
	cerr << a << "=";
	for (int i = 0; i < 64; i++) {
		if (v < 0)
			cerr << "1";
		else
			cerr << "0";
		v <<= 1;
	}
	cerr << endl;
}

KifElement* PreEditDistance(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	string s1 = callfunc->Evaluatethread(0, dom, idthread)->String();
	string s2 = callfunc->Evaluatethread(1, dom, idthread)->String();
	int nb = 0;
	KIFULONG characters[256];
	short pos[256];
	short bin[256];

	memset(characters, 0, sizeof(KIFULONG)* 256);
	memset(pos, 0, sizeof(short)* 256);
	memset(bin, 0, sizeof(short)* 256);

	//first we stow the characters from the two strings in characters...
	int sz1 = s1.size();
	int sz2 = s2.size();
	int l = max(sz1, sz2);
	int i;
	unsigned char a;
	KIFULONG v = 0;
	bool endofstr = false;

	for (i = 0; i < l; i++) {
		if (i < sz1 && i < sz2) {
			if (s1[i] == s2[i])
				continue;
		}
		else
			endofstr = true;

		if (i < sz1) {
			a = s1[i];
			if (!characters[a]) {
				pos[a] = i;
				characters[a] = 1;
				bin[a] = 0;
			}
			else {
				if (pos[a] < 0){
					bin[a] += i + pos[a];
					characters[a] |= 1 << bin[a];
				}
				else {
					characters[a] &= ~(1 << bin[a]);
					bin[a] += i - pos[a];
					characters[a] |= 1 << bin[a];
				}
				pos[a] = i;
			}
		}

		if (i<sz2) {
			a = s2[i];
			if (!characters[a]) {
				pos[a] = -1 * i;
				characters[a] = 1;
				bin[a] = 0;
			}
			else {
				if (pos[a]>0){
					bin[a] += i - pos[a];
					characters[a] |= 1 << bin[a];
				}
				else{ //we remove the last added bit...
					characters[a] &= ~(1 << bin[a]);
					bin[a] += i + pos[a];
					characters[a] |= 1 << bin[a];
				}
				pos[a] = i*-1;
			}
		}
	}

	nb = abs(sz1 - sz2);
	for (i = 32; i < 127; i++) {
		if (characters[i]) {
			v |= characters[i];
			affiche(i, characters[i]);
		}
	}
	affiche('V', v);
	return callfunc->kifcode->Provideinteger(nb);
}


//---------------------------------------------------------------------------------------------------------------

KifElement* PreNext(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	//First parameter should be a node
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->type == kifIterator) {
		kfirst->Next();
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[141]);
}

KifElement* PreLoad(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* filename;

	if (ke->type == kifTransducer) {
		filename = callfunc->Evaluatethread(1, dom, idthread);
		KifTransducer* ktr = (KifTransducer*)ke;
		if (ktr->automaton != NULL)
			delete ktr->automaton;
		ktr->automaton = new Kif_Automaton;
		string sname = filename->String();
		if (!ktr->automaton->load(sname))
			return kifFALSE;

		ktr->automaton->fillencoding(false);
		ktr->automaton->start.shuffle();
		return kifTRUE;
	}

	if (ke->type == kifKiF || ke->type == kifLoader) {
		if (callfunc->Size() != 2)
			return callfunc->kifcode->Returnerror(kifErrorStrings[129]);
		KifKiF* kpars = (KifKiF*)ke;
		if (kpars->id != -1)
			return callfunc->kifcode->Returnerror(kifErrorStrings[179]);
		filename = callfunc->Evaluatethread(1, dom, idthread);
		kpars->name = filename->String();
		kpars->name = Getfilename(kpars->name);
		KifCode* knew = callfunc->kifcode->Newinstance();
		kpars->id = knew->idkifmain;
		string error;
		short ret = knew->KifLoadPath(kpars->name, false);
		if (ret == -1) {
			error = kifErrorStrings[184];
			error += kpars->name;
			return callfunc->kifcode->Returnerror(error);
		}

		return callfunc->kifcode->Provideinteger(kpars->id);
	}

	if (ke->type == kifAutomaton) {
		string path = callfunc->Evaluatethread(1, dom, idthread)->String();
		string featchar;
		if (callfunc->Size() == 3)
			featchar = callfunc->Evaluatethread(2, dom, idthread)->String();
		if (((KifAutomaton*)ke)->load(path, featchar) == false)
			return kifFALSE;
		return kifTRUE;
	}

	if (ke->type == kifGrammar) {
		KifElement* kvect = callfunc->Evaluatethread(1, dom, idthread);
		string vrule = kvect->String();
		return ((KifGrammar*)ke)->Loader(vrule);
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[131]);
}

KifElement* PreLoadDouble(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	string path = callfunc->Evaluatethread(1, dom, idthread)->String();
	string codefeat;
	if (callfunc->Size() == 3)
		codefeat = callfunc->Evaluatethread(2, dom, idthread)->String();
	if (((KifAutomaton*)ke)->loaddouble(path, codefeat) == false)
		return kifFALSE;
	return kifTRUE;
}


KifElement* PreSetcodefeat(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	agnostring str(callfunc->Evaluatethread(1, dom, idthread)->String());
	return ((KifAutomaton*)ke)->setcodefeat(str);
}

KifElement* PreDump(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	string path(callfunc->Evaluatethread(1, dom, idthread)->String());
	((KifAutomaton*)ke)->dump(path);
	return kifTRUE;
}


KifElement* PreAddword(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {	
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	if (ke->type == kifTransducer) {
		KifTransducer* kau = (KifTransducer*)ke;
		if (kau->automaton == NULL)
			kau->automaton = new Kif_Automaton;

		ke = callfunc->Evaluatethread(1, dom, idthread);
		
		if (callfunc->Size() >= 3) {
			kau->automaton->normalize = callfunc->Evaluatethread(2, dom, idthread)->Boolean();
			if (callfunc->Size() == 4)
				kau->automaton->encoding_table = callfunc->Evaluatethread(3, dom, idthread)->Integer();
		}

		if (ke->type == kifMapStringString)
			kau->automaton->addmap(((KifMapStringString*)ke)->values);
		else {
			hmap<string, string> values;
			if (ke->aVectorContainer()) {
				if ((ke->Size() % 2) != 0)
					return callfunc->kifcode->Returnerror("XTR(010): Expecting a vector with an even number of values.");
				string s;
				string l;
				for (int i = 0; i < ke->Size(); i += 2) {
					s = ke->getstring(i);
					l = ke->getstring(i + 1);
					values[s] = l;
				}

			}
			else {
				if (ke->aMapContainer()) {
					KifIteration* itr = (KifIteration*)ke->Newiterator(false);
					for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
						values[itr->IteratorKeyString()] = itr->IteratorValueString();
					}
					itr->Release();
				}
			}
			kau->automaton->addmap(values);
		}

		return kifTRUE;
	}

	KifAutomaton* kau = (KifAutomaton*)ke;
	ke = callfunc->Evaluatethread(1, dom, idthread);
	if (ke->isString()) {
		agnostring str(ke->String());
		if (callfunc->Size() == 2)
			kau->addword(str);
		else {
			agnostring lemma(callfunc->Evaluatethread(2, dom, idthread)->String());
			kau->addworddouble(str, lemma);
		}
		return kifTRUE;
	}

	if (ke->aVectorContainer()) {
		for (int i = 0; i < ke->Size(); i++) {
			agnostring str(ke->getstring(i));
			kau->addword(str);
		}
		return kifTRUE;
	}

	if (ke->aMapContainer()) {
		KifIteration* itr = (KifIteration*)ke->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			agnostring k(itr->IteratorKeyString());
			agnostring v(itr->IteratorValueString());
			kau->addworddouble(k, v);
		}
		itr->Release();
		return kifTRUE;
	}

	return callfunc->kifcode->Returnerror("KIF(832): Argument cannot be processed in 'add' for 'automaton'.");
}


KifElement* PreLoadCompact(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kstr = callfunc->Evaluatethread(1, dom, idthread);
	string codefeat;
	if (callfunc->Size() == 3)
		codefeat = callfunc->Evaluatethread(2, dom, idthread)->String();

	return ((KifAutomaton*)ke)->loadcompact(kstr->String(), codefeat);
}

KifElement* Prespotfinalstate(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ka = callfunc->Evaluatethread(0, dom, idthread);
	bool v = callfunc->Evaluatethread(1, dom, idthread)->Boolean();
	((KifAutomaton*)ka)->spotfinalstate = v;
	return kifTRUE;
}

KifElement* PreLook(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ka = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kstr = callfunc->Evaluatethread(1, dom, idthread);
	KifElement* o = NULL;
	if (callfunc->Size() == 3)
		o = callfunc->Evaluatethread(2, dom, idthread);
	return ((KifAutomaton*)ka)->evaluate(pattern, dom, kstr, o);
}

KifElement* PreSetpath(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kmap = callfunc->Evaluatethread(1, dom, idthread);
	if (kmap->type != kifMap)
		return callfunc->kifcode->Returnerror("KIF(644): The structure should be a map of maps");
	return ((KifAutomaton*)ke)->setpath((KifMap*)kmap);
}

KifElement* PreSetmodel(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kstr = callfunc->Evaluatethread(1, dom, idthread);
	((KifAutomaton*)ke)->setmodel(kstr);
	return kifTRUE;
}

KifElement* PrePush(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	//second parameter is anything
	KifElement* res;
	for (size_t i = 1; i < callfunc->Size(); i++) {
		res = callfunc->Evaluatethread(i, dom, idthread);
		if (res == kifNOELEMENT)
			continue;
		if (res == kifRAISEERROR)
			return res;
		kf->Push(res);
	}
	return kifTRUE;
}

KifElement* PrePop(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);

	//second parameter is an integer (the number of pop)
	KifElement* i = NULL;
	int nb = 1;
	if (callfunc->Size() == 3)
		nb = callfunc->Evaluatethread(2, dom, idthread)->Integer();
	if (callfunc->Size() >= 2)
		i = callfunc->Evaluatethread(1, dom, idthread);
	else
		i = kifMINUSONE;

	while (nb > 0) {
		kf->Pop(i);
		nb--;
	}

	return kifTRUE;
}

KifElement* PreLast(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	//First parameter should be a vector
	KifElement* kf = callfunc->Evaluatethread(0, dom, idthread);
	//second parameter is an integer (the number of pop)

	if (kf->aContainer() || kf->isString())
		return kf->Last();
	return callfunc->kifcode->Returnerror(kifErrorStrings[45]);
}


KifElement* PreSet(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* value = callfunc->Evaluatethread(1, dom, idthread);
	kfirst->IteratorSetvalue(value);
	return kifTRUE;
}

KifElement* PreGet(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	if (kfirst->isfile()) {
		if (callfunc->Size() != 1)
			return callfunc->kifcode->Returnerror(kifErrorStrings[93]);
		KifBaseFile* kfile = (KifBaseFile*)kfirst;
		if (kfile->EndOfFile() == kifTRUE)
			return kifMINUSONE;

		long c = kfile->Get();
		if (pattern->type == kifInteger || pattern->type == kifFloat)
			return callfunc->kifcode->Provideinteger(c);

		return new KifByte(callfunc->kifcode, NULL, (unsigned char)c);
	}

	if (callfunc->Size() == 2) {
		string txt = callfunc->Evaluatethread(1, dom, idthread)->String();
#ifdef DOSOUTPUT
		txt = conversion2dos(txt);
#endif

		cout << STR(txt);
	}

	string srep = callfunc->kifcode->Gets();
	if (srep == "") {
		kfirst->Setvalue(kifNULL, NULL, dom);
		return kifFALSE;
	}

	KifElement* ke;
#ifdef DOSOUTPUT
	if (kfirst->isString() && dosoutput) {
		string txt = s_dos_to_utf8(USTR(srep));
		ke = callfunc->kifcode->Providestring(txt);
	}
	else
		ke = callfunc->kifcode->Providestring(srep);
#else
	ke = callfunc->kifcode->Providestring(srep);
#endif
	kfirst->Setvalue(ke, NULL, dom);
	ke->Release();
	return kifTRUE;
}

KifElement* PreThreadId(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	int id = kifGlobals->GetThreadid();
	return callfunc->kifcode->Provideinteger(id);
}


KifElement* PreExit(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	if (callfunc->Size() == 0)
		exit(-1);
	KifElement* res = callfunc->Evaluatethread(0, dom, idthread);
	exit(res->Integer());
	return res;
}


KifElement* PreBuild(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	string input = callfunc->Evaluatethread(1, dom, idthread)->String();
	string output = callfunc->Evaluatethread(2, dom, idthread)->String();
	bool norm = callfunc->Evaluatethread(3, dom, idthread)->Boolean();
	int latinencoding = 1;
	if (callfunc->Size() == 5)
		latinencoding = callfunc->Evaluatethread(4, dom, idthread)->Integer();

	KifTransducer* ktr = (KifTransducer*)kstr;

	if (ktr->automaton != NULL)
		delete ktr->automaton;

	ktr->automaton = new Kif_Automaton();
	if (compileAutomaton(*ktr->automaton, input, output, latinencoding, norm))
		return kifTRUE;

	return kifFALSE;
}

KifElement* PreProcess(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifTransducer* ktr = (KifTransducer*)ke;
	if (ktr->automaton == NULL)
		return kifNULL;
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	string words = callfunc->Evaluatethread(1, dom, idthread)->String();
	charReadString currentreader(words);
	currentreader.addoffsets = true;
	currentreader.begin();
	KifVectorString* kvs = new KifVectorString(callfunc->kifcode, NULL);
	while (!currentreader.end()) {
		if (ktr->automaton->process(currentreader, kvs->values, NON)) {
			kvect->Push(kvs);
			kvs = new KifVectorString(callfunc->kifcode, NULL);
		}
	}
	kvs->Release();
	return kvect;
}

KifElement* PreLookup(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifTransducer* ktr = (KifTransducer*)ke;
	if (ktr->automaton == NULL)
		return kifNULL;

	wstring word = callfunc->Evaluatethread(1, dom, idthread)->UString();
	int threshold = 0;
	short flags = 0;
	if (callfunc->Size() == 4) {
		threshold = callfunc->Evaluatethread(2, dom, idthread)->Integer();
		flags = callfunc->Evaluatethread(3, dom, idthread)->Integer();
	}

	if (contextualpattern->aContainer()) {
		KifElement* kvs = Selectvectorstring(callfunc, contextualpattern);

		if (kvs->type == kifVectorString) {
			ktr->automaton->up(word, ((KifVectorString*)kvs)->values, threshold, flags);			
		}
		else {
			vector<string> readings;
			ktr->automaton->up(word, readings, threshold, flags);
			for (int i = 0; i < readings.size(); i++)
				kvs->storevalue(readings[i]);
		}
		return kvs;
	}

	vector<string> readings;
	ktr->automaton->up(word, readings, threshold, flags);

	if (readings.size())
		return callfunc->kifcode->Providestringraw(readings[0]);
	return kifNULL;
}

KifElement* PreLookdown(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	KifTransducer* ktr = (KifTransducer*)ke;
	if (ktr->automaton == NULL)
		return kifNULL;

	wstring word = callfunc->Evaluatethread(1, dom, idthread)->UString();

	if (contextualpattern->aContainer()) {
		KifElement* kvs = Selectvectorstring(callfunc, contextualpattern);

		if (kvs->type == kifVectorString)
			ktr->automaton->down(word, ((KifVectorString*)kvs)->values);
		else {
			vector<string> readings;
			ktr->automaton->down(word, readings);
			for (int i = 0; i < readings.size(); i++)
				kvs->storevalue(readings[i]);
		}
		return kvs;
	}

	vector<string> readings;
	ktr->automaton->down(word, readings);

	if (readings.size())
		return callfunc->kifcode->Providestringraw(readings[0]);
	return kifNULL;
}

//Compile a Transducer regular expression...
KifElement* PreCompilergx(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifTransducer* kxiptrans = (KifTransducer*)callfunc->Evaluatethread(0, dom, idthread);

	agnostring rgx(callfunc->Evaluatethread(1, dom, idthread)->String());
	KifElement* kfeat = callfunc->Evaluatethread(2, dom, idthread);
	if (!kfeat->aVectorContainer())
		return callfunc->kifcode->Returnerror("XTR(001): We expect the second argument to be a vector.");

	if (kxiptrans->automaton == NULL)
		kxiptrans->automaton = new Kif_Automaton();

	//we first transform each of our features into indexes...
	vector<unsigned long> indexes;
	string s;
	for (int i = 0; i < kfeat->Size(); i++) {
		s = kfeat->getstring(i);
		if (s[0] != '\t')
			s = "\t" + s;
		indexes.push_back(kxiptrans->automaton->index(s));
	}

	if (!kxiptrans->automaton->start.parse(*kxiptrans->automaton, rgx, indexes))
		return callfunc->kifcode->Returnerror("XTR(002): Wrong regular expression");

	if (callfunc->Size() == 4) {
		string name = callfunc->Evaluatethread(3, dom, idthread)->String();
		kxiptrans->automaton->store(name);
	}

	return kifTRUE;
}


//===================================================================
//It is an XML document
#ifdef KIF_RAW

extern "C" {
	void NormalisationNomFichier(char* nomFichier, char* tampon) {
		//Tout d'abord on ramene tous les chemins a de l'UNIX
		int l = strlen(tampon);
		//On normalise les chemins
		for (int i = 0; i < l; i++) {
			if (tampon[i] == ANTISEP)
				tampon[i] = SEP;
		}

		//s'il y a une variable d'environnement
		char* vari = strchr(tampon, '$');
		nomFichier[0] = 0;
		while (vari) {
			char* reper = getenv(vari + 1);
			char* pt = strchr(vari + 1, SEP);

			if (pt != NULL)
				*pt = 0;

			//On recopie la partie qui precede la variable
			int lvar = vari - tampon;
			int lnom = strlen(nomFichier);
			memcpy(nomFichier + lnom, tampon, lvar);
			nomFichier[lvar + lnom] = 0;

			if (reper != NULL)
				strcat(nomFichier, reper);

			if (pt != NULL) {
				*pt = SEP;
				static char inter[1000];
				strcpy(inter, pt);
				strcpy(tampon, inter);
			}
			else
				tampon[0] = 0;
			vari = strchr(tampon, '$');
		}

		strcat(nomFichier, tampon);

	}
}

#endif




