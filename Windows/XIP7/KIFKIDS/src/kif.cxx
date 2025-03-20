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

#ifdef WIN32
#include "Windows.h"
#endif

#ifdef C11
#include <thread>
#else
#ifdef WIN32
#include <process.h>
#else
#include <pthread.h>
#endif
#endif

#include "kif.h"
#include "automate.h"
#include "kifcontainer.h"
#include "kifmaps.h"

extern const char* KIFLIBSTRINGS[];


#ifdef UNIX
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#include <termios.h>
#endif

//---------------------------------------------------------------------------------------------
#define NOVARIABLEINFUNCTION 10
#define SINGLEINSTRUCTION 15
#define VARIABLEINFUNCTION 20
bool KifDerivation(KifElement* frame, KifElement* derive, bool typetest);
//---------------------------------------------------------------------------------------------
static threadhandle kif_main_thread_id;

static bool init_main_thread = false;
bool isLong(KIFLONG v);

void KifSetMainThread() {
	if (init_main_thread)
		return;
	init_main_thread = true;
	kif_main_thread_id = _GETTHREADID();
}

bool KifIsMainThread() {
	if (init_main_thread == false)
		return false;
	threadhandle tid = _GETTHREADID();
	if (tid == kif_main_thread_id)
		return true;
	return false;
}

//-----------------------------------------------------------------------

KifGlobalThread* kifGlobals = NULL;
//---------------------------------------------------------------------
#ifdef C11

//This method is used to create a mutex that counts up and does not block
//this function is called when the main function has been interrupted
ThreadLock::ThreadLock(KifType i) {
	relax = NULL;
	garbaging = false;
	if (kifGlobals != NULL && kifGlobals->Test() == true) {
		if (i >= kifStack && i <= kifError)
			relax = new std::lock_guard<std::recursive_mutex>(kifGlobals->tableofcommonlocks[i]);
		else {
			if (kifGlobals->garbaging) {
				garbaging = true;
				relaxgarbage = new std::lock_guard<std::recursive_mutex>(kifGlobals->tableofcommonlocks[kifGarbaging]);

			}
			relax = new std::lock_guard<std::recursive_mutex>(kifGlobals->tableofcommonlocks[i]);
		}
	}
}

void ThreadLock::Initialise() {}


ThreadLock::~ThreadLock() {
	if (kifGlobals == NULL)
		return;

	if (garbaging)
		delete relaxgarbage;
	if (relax != NULL)
		delete relax;
}

Exported KifThreadLock::KifThreadLock(std::recursive_mutex* l, bool run, bool init) {
	lock = l;
	recursive = init;
	locked = run;
	idthread = 0;
	if (run)
		lock->lock();
	else
	if (lock == NULL)
		lock = new std::recursive_mutex();
}

Exported void KifThreadLock::Initialize() {
	lock = new std::recursive_mutex();
}

Exported void KifThreadLock::Locking() {
	lock->lock();
}

Exported void KifThreadLock::Unlocking() {
	lock->unlock();
}

Exported KifThreadLock::~KifThreadLock() {
	if (locked)
		lock->unlock();
	else
	if (lock != NULL)
		delete lock;
}

//--------------------------------------------------
Exported void LockedThread::Blocked() {
	std::unique_lock<std::mutex> lck(mtx);
	released = false;
	lock.wait(lck);
	released = true;
}

Exported void LockedThread::Released() {
	lock.notify_all();
}

//--------------------------------------------------
#else
#ifdef WIN32
Exported ThreadLock::ThreadLock(KifType i) {
	relax = -1;
	garbaging = false;
	if (kifGlobals != NULL && kifGlobals->Test() == true) {
		if (kifGlobals->updatethread == true) {
			kifGlobals->updatethread = false;
			Initialise();
		}
		relax = (int)i;
		if (kifGlobals->tableofcommonlocks[relax] == 0)
			kifGlobals->tableofcommonlocks[relax] = CreateMutex(NULL, FALSE, NULL);

		if (i >= kifStack && i <= kifError)
			WaitForSingleObject(kifGlobals->tableofcommonlocks[relax], INFINITE);
		else {
			//garbaging takes the step before any other actions...
			if (kifGlobals->garbaging) {
				garbaging = true;
				WaitForSingleObject(kifGlobals->tableofcommonlocks[kifGarbaging], INFINITE);
			}
			WaitForSingleObject(kifGlobals->tableofcommonlocks[relax], INFINITE);
		}
	}
}

Exported void ThreadLock::Initialise() {
	kifGlobals->tableofcommonlocks[kifGarbaging] = CreateMutex(NULL, FALSE, NULL);
}


Exported ThreadLock::~ThreadLock() {
	if (kifGlobals == NULL)
		return;

	if (relax != -1) {
		if (relax >= kifStack && relax <= kifError)
			ReleaseMutex(kifGlobals->tableofcommonlocks[relax]);
		else {
			if (garbaging)
				ReleaseMutex(kifGlobals->tableofcommonlocks[(int)kifGarbaging]);
			ReleaseMutex(kifGlobals->tableofcommonlocks[relax]);
		}
	}
}

Exported KifThreadLock::KifThreadLock(HANDLE l, bool run, bool init) {
	lock = l;
	locked = run;
	idthread = 0;
	if (run)
		WaitForSingleObject(lock, INFINITE);
}

Exported void KifThreadLock::Initialize() {
	lock = CreateMutex(NULL, FALSE, NULL);
}

Exported void KifThreadLock::Locking() {
	if (lock == NULL)
		lock = CreateMutex(NULL, FALSE, NULL);
	WaitForSingleObject(lock, INFINITE);
}

Exported void KifThreadLock::Unlocking() {
	ReleaseMutex(lock);
}

Exported KifThreadLock::~KifThreadLock() {
	if (locked)
		ReleaseMutex(lock);
	else
	if (lock != NULL)
		CloseHandle(lock);
}

Exported LockedThread::LockedThread() {
	blocked = false;
	lock = CreateEvent(NULL, FALSE, FALSE, NULL);
}

Exported LockedThread::~LockedThread() {
	CloseHandle(lock);
}

Exported void LockedThread::Blocked() {
	blocked = true;
	WaitForSingleObject(lock, INFINITE);
}

Exported void LockedThread::Released() {
	SetEvent(lock);
}


#else //For other OS

//This method is used to create a mutex that counts up and does not block
void KifInitialisationMutex(pthread_mutex_t* mut) {
	static bool initial = true;
	static pthread_mutexattr_t mattr;
	//We create it once for all
	if (initial == true) {
		pthread_mutexattr_init(&mattr);
		pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
		initial = false;
	}
	pthread_mutex_init(mut, &mattr);
}

//this function is called when the main function has been interrupted
void cleanThreads(int i) {
#if defined(WIN32) || defined(APPLE)
	hmap<threadhandle, int>::iterator it;
#else
	map<threadhandle, int>::iterator it;
#endif
	for (it = kifGlobals->threads.begin(); it != kifGlobals->threads.end(); it++)
		pthread_cancel(it->first);
	exit(-1);
}

ThreadLock::ThreadLock(KifType i) {
	relax = -1;
	garbaging = false;
	if (kifGlobals != NULL && kifGlobals->Test() == true) {
		if (kifGlobals->updatethread == true) {
			kifGlobals->updatethread = false;
			signal(SIGPIPE, cleanThreads);
			signal(SIGINT, cleanThreads);
			Initialise();
		}
		relax = (int)i;
		if (i >= kifStack && i <= kifError)
			pthread_mutex_lock(&kifGlobals->tableofcommonlocks[relax]);
		else {
			if (kifGlobals->garbaging) {
				garbaging = true;
				pthread_mutex_lock(&kifGlobals->tableofcommonlocks[kifGarbaging]);
			}
			pthread_mutex_lock(&kifGlobals->tableofcommonlocks[relax]);
		}
	}
}

void ThreadLock::Initialise() {
	for (int i = 0; i<200; i++)
		KifInitialisationMutex(&kifGlobals->tableofcommonlocks[i]);
}


ThreadLock::~ThreadLock() {
	if (kifGlobals == NULL)
		return;

	if (relax != -1) {
		if (relax >= kifStack && relax <= kifError)
			pthread_mutex_unlock(&kifGlobals->tableofcommonlocks[relax]);
		else {
			if (garbaging)
				pthread_mutex_unlock(&kifGlobals->tableofcommonlocks[(int)kifGarbaging]);
			pthread_mutex_unlock(&kifGlobals->tableofcommonlocks[relax]);
		}
	}
}

Exported KifThreadLock::KifThreadLock(pthread_mutex_t* l, bool run, bool init) {
	lock = l;
	recursive = init;
	locked = run;
	idthread = 0;
	if (run)
		pthread_mutex_lock(lock);
}

Exported void KifThreadLock::Initialize() {
	lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if (recursive)
		KifInitialisationMutex(lock);
}

Exported void KifThreadLock::Locking() {
	if (lock == NULL) {
		lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		if (recursive)
			KifInitialisationMutex(lock);
	}
	pthread_mutex_lock(lock);
}

Exported void KifThreadLock::Unlocking() {
	pthread_mutex_unlock(lock);
}


Exported KifThreadLock::~KifThreadLock() {
	if (locked)
		pthread_mutex_unlock(lock);
	else
	if (lock != NULL)
		free(lock);
}

#ifdef APPLE
//Mac OS only provides "named" semaphores...
int LockedThread::naming = 0;
Exported LockedThread::LockedThread() {
	blocked = false;
	sprintf(name, "/kif_%d", naming);
	naming++;
	lock = sem_open(name, O_CREAT, 0777, 0);
}

Exported LockedThread::~LockedThread() {
	sem_unlink(name);
	sem_destroy(lock);
}

Exported void LockedThread::Blocked() {
	blocked = true;
	sem_wait(lock);
}


Exported void LockedThread::Released() {
	sem_post(lock);
}
#else //Other UNIX systems
//In the other UNIX systems, we can use anonymous semaphores...
Exported LockedThread::LockedThread() {
	blocked = false;
	sem_init(&lock, 0, 0);
}

Exported LockedThread::~LockedThread() {
	sem_destroy(&lock);
}

Exported void LockedThread::Blocked() {
	blocked = true;
	sem_wait(&lock);
}

Exported void LockedThread::Released() {
	sem_post(&lock);
}
#endif

#endif
#endif

int kif_declencheur = 10000;

//-----------------------------GLOBAL DEBUGGING----------------------------
bool initkifglobals = false;

KifDebugInfo* DebugPtr() {
	if (kifGlobals == NULL) {
		kifGlobals = new KifGlobalThread();
		initkifglobals = true;
	}
	return kifGlobals->debuginfocurrent;
}

KifDebugInfo* DebugPtrInit(bool& deb) {
	if (kifGlobals == NULL) {
		kifGlobals = new KifGlobalThread();
		initkifglobals = true;
	}
	deb = kifGlobals->debuginfocurrent->kifdebugging;
	return kifGlobals->debuginfocurrent;
}

Exported void KifSetKifDebuggingFunction(KifDebugInfo* debug) {
	if (kifGlobals == NULL) {
		kifGlobals = new KifGlobalThread();
		initkifglobals = true;
	}
	kifGlobals->debuginfocurrent = debug;
}

void KiFSetdebug(bool d) {
	DebugPtr()->KiFSetdebug(d);
}

void KiFSetdebugmin(bool d) {
	DebugPtr()->KiFSetdebugmin(d);
}

//-----------------------------------------------------------------------------
KifInstructionStack::KifInstructionStack(int id, short n, KifElement* ke) {
	idthread = id;
	name = n;
	e = NULL;
	if (ke != NULL && ke->Line() != -1) {
		popping = true;
		DebugPtr()->currentline = ke->Line();
		DebugPtr()->currentfile = ke->Fileindex();
		e = new Stackelement(ke->Line(), ke->Fileindex(), name);
		kifGlobals->THR[idthread]->stackelements.push_back(e);
	}
	else
		popping = false;
}

KifInstructionStack::~KifInstructionStack() {
	if (popping) {
		kifGlobals->THR[idthread]->stackelements.pop_back();
		delete e;
	}
}

void KifInstructionStack::Push(KifElement* ke) {
	if (ke->Line() != -1) {
		popping = true;
		kifGlobals->debuginfocurrent->currentline = ke->Line();
		kifGlobals->debuginfocurrent->currentfile = ke->Fileindex();
		e = new Stackelement(ke->Line(), ke->Fileindex(), name);
		kifGlobals->THR[idthread]->stackelements.push_back(e);
	}
}

void KifInstructionStack::Modify(KifElement* ke) {
	if (popping == false) {
		Push(ke);
		return;
	}
	if (ke->Line() != -1) {
		kifGlobals->debuginfocurrent->currentline = ke->Line();
		kifGlobals->debuginfocurrent->currentfile = ke->Fileindex();
		e->file = ke->Fileindex();
		e->line = ke->Line();
		e->name = name;
	}
}

void Stackelement::Modify(KifElement* ke) {
	if (ke->Line() != -1) {
		kifGlobals->debuginfocurrent->currentline = ke->Line();
		kifGlobals->debuginfocurrent->currentfile = ke->Fileindex();
		file = ke->Fileindex();
		line = ke->Line();
	}
}


//-------------------------------------------------------------------------

//Index analysis for a string.
char RawStringIndexes(char* svalue, size_t sz, KifElement* left, KifDomain* dom, long& ileft, long& iright, int idthread, KifCallFunction* callfunc) {
	if (idthread == -1) {
		idthread = kifGlobals->GetThreadid();
		callfunc = (KifCallFunction*)kifGlobals->Topid(idthread);
	}


	KifElement* right = NULL;
	bool sleft = false;
	bool sright = false;
	if (left->type == kifIndex) {
		KifIndex* kind = (KifIndex*)left;
		sleft = kind->signkey;
		sright = kind->signright;
		left = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true)
			right = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	long subsize = -1;

	char* strleft;
	if (left->type == kifString) {
		string sub(left->String());
		//then we are looking for a substring
		if (sleft)
			strleft = strrstr(svalue, STR(sub), sz, sub.size());
		else
			strleft = strstr(svalue, STR(sub));

		if (strleft == NULL)
			return 0;

		ileft = strleft - svalue;
		subsize = sub.size();
	}
	else {
		ileft = left->Integer();
		if (ileft < 0)
			ileft += sz;
		if (ileft >= sz)
			return 0;
		if (ileft < 0 || ileft >= sz)
			return 0;
	}

	left->Release();
	//We return as a non interval
	if (right == NULL) {
		if (subsize != -1) {//unless the left index was a string...
			iright = ileft + subsize;
			return 2;
		}
		return 1;
	}

	if (right->type == kifString) {
		char* strright;
		string sub(right->String());
		if (sright)
			strright = strrstr(svalue, STR(sub), sz, sub.size());
		else {
			if (subsize != -1)
				strright = strstr(svalue + ileft + subsize, STR(sub));
			else
				strright = strstr(svalue + ileft, STR(sub));
		}
		if (strright == NULL)
			return 0;
		iright = (strright - svalue) + sub.size();
	}
	else {
		iright = right->Integer();

		if (iright < 0 || right == kifNULL)
			iright += sz;
		else {
			if (ileft != -1)
				iright += ileft;
		}
		if (iright > sz)
			iright = sz;
	}

	right->Release();
	if ((iright - ileft) <= 0)
		return 0;
	return 2;
}

char StringIndexes(string& svalue, KifElement* left, KifDomain* dom, int& ileft, int& iright, int idthread, KifCallFunction* callfunc) {
	if (idthread == -1) {
		idthread = kifGlobals->GetThreadid();
		callfunc = (KifCallFunction*)kifGlobals->Topid(idthread);
	}


	KifElement* right = NULL;
	bool sleft = false;
	bool sright = false;
	if (left->type == kifIndex) {
		KifIndex* kind = (KifIndex*)left;
		sleft = kind->signkey;
		sright = kind->signright;
		left = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true)
			right = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	int sz = svalue.size();
	int szc = size_c(svalue);
	int offset = -1;
	int subsize = -1;

	if (left->type == kifString) {
		string sub = left->String();
		//then we are looking for a substring
		if (sleft)
			ileft = s_rfindbyte(svalue, sub, sz);
		else
			ileft = s_findbyte(svalue, sub, 0);
		if (ileft == -1)
			return 0;
		offset = size_c(sub);
		subsize = sub.size();
	}
	else {
		ileft = left->Integer();
		if (ileft < 0)
			ileft += szc;
		if (ileft >= szc)
			return 0;
		ileft = c_chartobyteposition(USTR(svalue), ileft);
		if (ileft < 0 || ileft >= sz)
			return 0;
	}

	left->Release();
	//We return as a non interval
	if (right == NULL) {
		if (subsize != -1) {//unless the left index was a string...
			iright = ileft + subsize;
			return 2;
		}
		return 1;
	}

	if (right->type == kifString) {
		string sub = right->String();
		if (sright)
			iright = s_rfindbyte(svalue, sub, sz);
		else {
			if (subsize != -1)
				iright = s_findbyte(svalue, sub, ileft + subsize);
			else
				iright = s_findbyte(svalue, sub, ileft);
		}
		if (iright == -1)
			return 0;
		iright += sub.size();
	}
	else {
		iright = right->Integer();

		if (iright < 0 || right == kifNULL)
			iright += size_c(svalue);
		else {
			if (offset != -1) {
				int szoffset = ileft;
				szoffset = c_bytetocharposition(USTR(svalue), szoffset);
				iright += szoffset + offset;
			}
		}
		if (iright >= szc)
			iright = sz;
		else {
			iright = c_chartobyteposition(USTR(svalue), iright);
			if (iright > sz)
				iright = sz;
		}
	}
	right->Release();
	if ((iright - ileft) <= 0)
		return 0;
	return 2;
}
//-------------------------------------------------------------------------
//Index analysis for a string.
char StringIndexes(wstring& svalue, KifElement* left, KifDomain* dom, int& ileft, int& iright, int idthread, KifCallFunction* callfunc) {
	if (idthread == -1) {
		idthread = kifGlobals->GetThreadid();
		callfunc = (KifCallFunction*)kifGlobals->Topid(idthread);
	}


	KifElement* right = NULL;
	bool sleft = false;
	bool sright = false;
	if (left->type == kifIndex) {
		KifIndex* kind = (KifIndex*)left;
		sleft = kind->signkey;
		sright = kind->signright;
		left = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true)
			right = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	int sz = svalue.size();
	int offset = -1;

	if (left->isString()) {
		wstring sub = left->UString();
		//then we are looking for a substring
		if (sleft)
			ileft = s_rfind(svalue, sub, sz);
		else
			ileft = s_find(svalue, sub, 0);

		if (ileft == -1)
			return 0;
		offset = sub.size();
	}
	else {
		ileft = left->Integer();
		if (ileft < 0)
			ileft += sz;
		if (ileft >= sz)
			return 0;
	}

	left->Release();
	//We return as a non interval
	if (right == NULL) {
		if (offset != -1) {//unless the left index was a string...
			iright = ileft + offset;
			return 2;
		}
		return 1;
	}

	if (right->isString()) {
		wstring wr = right->UString();
		if (sright)
			iright = s_rfind(svalue, wr, sz);
		else {
			if (offset != -1)
				iright = s_find(svalue, wr, ileft + offset);
			else
				iright = s_find(svalue, wr, ileft);
		}
		if (iright == -1)
			return 0;
		iright += wr.size();
	}
	else {
		iright = right->Integer();

		if (iright < 0 || right == kifNULL)
			iright += svalue.size();
		else {
			if (offset != -1)
				iright += ileft + offset;
		}
		if (iright >= sz)
			iright = sz;
		else {
			if (iright>sz)
				iright = sz;
		}
	}
	right->Release();
	if ((iright - ileft) <= 0)
		return 0;
	return 2;
}
//------------------------------------------------------------------
Exported string KifExecute(int ipar, string name, vector<string>& parameters, ostringstream* os, bool debugkif);
Exported void KifDelete(bool, int ipar);
int KifLoadin(KifCode* kifcode, string path);
extern "C" {
	void NormalisationNomFichier(char* nomFichier, char* tampon);
}

string Getfilename(string n) {
	char buff[1024];
	NormalisationNomFichier(buff, (char*)n.c_str());
	return buff;
}


//----------------------------------------------------------------------------------------
#ifdef WIN32
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
int gettimeofday(struct timeval *tv, struct timezone *tz) {

	FILETIME ft;
	unsigned __int64 tmpres = 0;

	if (NULL != tv) {
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	return 0;
}
#endif


double plustime(timeval& tempsinitial, timeval& tempsfinal) {
	double init = (((unsigned long)tempsinitial.tv_sec) * 1000 + ((unsigned long)tempsinitial.tv_usec) / 1000.0) + 0.5;
	double final = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0) + 0.5;
	return(final + init);
}

double minustime(timeval& tempsfinal, timeval& tempsinitial) {
	double init = (((unsigned long)tempsinitial.tv_sec) * 1000 + ((unsigned long)tempsinitial.tv_usec) / 1000.0);
	double final = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
	return(final - init);
}

long timeminus(double init, timeval& tempsfinal) {
	double final = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
	return(final - init);
}

KifElement* PrePause(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kfirst = callfunc->Evaluatethread(0, dom, idthread);
	double res = kfirst->Float() * 1000;
	char car = 8;
	bool display = false;
	if (callfunc->Size() == 2)
		display = callfunc->Evaluatethread(1, dom, idthread)->Boolean();
	timeval tempsfinal;
	gettimeofday(&tempsfinal, NULL);
	double init = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);

	while (timeminus(init, tempsfinal) < res) {
		if (display) {
			cerr << "/" << car << "/" << car << "/" << car;
			cerr << "-" << car << "-" << car << "-" << car;
			cerr << "\\" << car << "\\" << car << "\\" << car;
		}
		gettimeofday(&tempsfinal, NULL);
	}
	return kifTRUE;
}


typedef bool(*PauseBack)(void*);
bool KifWaitingFor(double tm, PauseBack pb, void* data) {
	double init;
	double res;
	timeval tempsfinal;

	gettimeofday(&tempsfinal, NULL);
	init = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
	res = tm * 1000;

	double diff = 0;
	while (diff < res) {
		if (pb(data) == true)
			return true;
		gettimeofday(&tempsfinal, NULL);
		diff = timeminus(init, tempsfinal);
	}
	return false;
}

void KifWait(double tm) {
	double init;
	double res;
	timeval tempsfinal;

	gettimeofday(&tempsfinal, NULL);
	init = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
	res = tm * 1000;

	double diff = 0;
	while (diff < res) {
		gettimeofday(&tempsfinal, NULL);
		diff = timeminus(init, tempsfinal);
	}
}

#ifdef WIN32
Exported bool DirectoryCreation(char* noms, int pos) {
	char* pt = strchr(noms + pos, SEP);
	while (pt != NULL) {
		*pt = 0;
		CreateDirectoryA(noms, NULL);
		*pt = SEP;
		pt = strchr(pt + 1, SEP);
	}

	bool created = true;
	if (CreateDirectoryA(noms, NULL) == FALSE)
		created = false;

	return created;
}
#else
bool DirectoryCreation(char* noms, int pos) {
	char* pt = strchr(noms + pos, SEP);
	while (pt != NULL) {
		*pt = 0;
		mkdir(noms, S_IRWXU | S_IRWXG | S_IRWXO);
		*pt = SEP;
		pt = strchr(pt + 1, SEP);
	}

	bool created = true;
	if (mkdir(noms, S_IRWXU | S_IRWXG | S_IRWXO) != 0)
		created = false;
	return created;
}
#endif

#ifdef APPLE
extern "C" {
	int    gethostuuid(uuid_t id, const struct timespec *wait) {
		return -1;
	}
}
#endif


//===================================================================
string KifCode::Gets() {
	string s;
	getline(cin, s);
	return s;
}


inline string IntegerString(long value) {
	ostringstream is;
	is << value;
	return is.str();
}

inline string FloatString(double value) {
	ostringstream is;
	is << value;
	return is.str();
}


#ifdef _DEBUG
KifCode* currentkifcode;
#endif

typedef KifElement* (KifInstruction::*ExecuteInstruction)(KifElement* contextualpattern, KifDomain* dom, KifElement* ke);


bool TestkifFunctions(short n, hmap<short, bool>& kiffunc) {
	if (kiffunc.find(n) == kiffunc.end())
		return false;
	return true;
}


KifBoolean::KifBoolean(KifCode* klc, KifElement* kp, bool v, KifType ty) : value(v), KifElement(klc, kp, ty) {}

KifRaiseError::KifRaiseError(string mes, string file, long l, long r) {
	message = mes;
	char c[100];
	sprintf_s(c, 100, "%d", l);
	message += KIFKIFSTRINGS[0];
	message += c;
	message += ")";

	filename = file;
	left = l;
	right = r;
}

//===================================================================
bool PureKifInstance(KifElement* i) {
	if (i->type != kifInstance)
		return false;
	KifInstance* inst = (KifInstance*)i;
	if (inst->index != NULL)
		return false;
	if (inst->value != NULL) {
		if (inst->value->type == kifInstanceFunction)
			return false;
		if (inst->value->type == kifInstance)
			return PureKifInstance(inst->value);
	}
	return true;
}

inline bool compatibleArguments(KifElement* kparam, KifElement* karg) {
	if (karg->Simple())
		return true;

	//in this case, we do not want to duplicate anything, we wait for the rest of the code to take over...
	if (karg->type == kparam->type)
		return(false);

	return kifGlobals->compatible(kparam, karg);
}


inline bool compatibleArgument(short arg, short param) {
	return kifGlobals->compatible(arg, param);
}

static bool KifSameParameters(KifElement* arg, KifElement* param) {
	//for a self, anything goes...
	if (arg->type == kifSelf)
		return true;

	KifType typeparam = param->inType();
	if (typeparam == kifthis && arg->type == kifFrame)
		return true;

	//we have two specific cases: if the call is done with a function
	//or the parameter is called through a function or through an index, we cannot say anything
	//It will be defined dynamically
	if (kifGlobals->accetabletypeparams.check(typeparam) || param->hasIndex() || param->Function() || param->isACall())
		return false;

	//The simplest case, they both share the same type...
	if (typeparam == arg->type) {
		//However, if we are dealing with two frames...
		if (arg->type == kifFrame) {
			//then the parameter should be either the same frame or a subframe... 
			KifFrame* fparam = (KifFrame*)param->Linkedvalue();
			if (compatibleArgument(arg->Name(), fparam->Name()) == false) {
				//In this case, a function has been declared to transform this frame into another
				if (fparam->Declared(arg->Name()))
					return true;
				return false;
			}
			return true;
		}
		return true;
	}

	//Rattrapage
	if (param->aVectorContainer() && arg->aVectorContainer())
		return true;
	if (param->aMapContainer() && arg->aMapContainer())
		return true;
	if (param->aString() && arg->aString())
		return true;
	if (param->aNumber() && arg->aNumber())
		return true;

	return false;
}


static bool KifAnalyzeParameter(KifElement* arg, KifElement* param, bool strict) {
	//for a self, anything goes...
	if (arg->type == kifSelf)
		return true;

	//The simplest case. No strict equivalence, and arg is a simple type such as: integer, float, boolean or string
	if (strict == false && arg->Simple())
		return true;


	KifType typeparam = param->inType();

	//we have two specific cases: if the call is done with KIFKIFSTRINGS[1] or with KIFKIFSTRINGS[2]
	//then these two specific values are still valid...
	if (kifGlobals->accetabletypeparams.check(typeparam) || (typeparam == kifthis && arg->type == kifFrame) || param->hasIndex() || param->Function() || param->isACall())
		return true;

	//The simplest case, they both share the same type...
	if (typeparam == arg->type) {
		//However, if we are dealing with two frames...
		if (arg->type == kifFrame) {
			//then the parameter should be either the same frame or a subframe... 
			KifFrame* fparam = (KifFrame*)param->Linkedvalue();
			if (compatibleArgument(arg->Name(), fparam->Name()) == false) {
				//In this case, a function has been declared to transform this frame into another
				if (fparam->Declared(arg->Name()))
					return true;
				return false;
			}
			return true;
		}
		return true;
	}
	//If we have reached this part of the code, and arg is still a frame, then error->..
	if (arg->type == kifFrame)
		return false;

	//If we demand a strict checking of the paramater, we still have different cases
	//We are in this part of the code, in the case where the parameter is a mathematical formula...
	if (strict == true)
		return compatibleArgument(typeparam, arg->type);

	if (compatibleArgument(arg->type, typeparam))
		return true;

	return false;
}

bool KifCompareArgs(KifFunction* func, KifCallFunction* kcf) {
	long sza = func->arguments.size();
	long szp = kcf->parameters.size();

	if (sza == szp)
		return true;

	long counta = 1;
	long nb = 1;
	for (int i = 0; i < sza; i++) {
		KifElement* ke = func->arguments[i]->Initialisation();
		if (ke->type == kifInstructionINITIALISATION)
			counta |= (1 << nb);
		else
			counta = (1 << nb);
		nb++;
	}

	nb = 1 << szp;

	if ((counta&nb) != nb)
		return false;
	return true;
}

char KifCheckParameters(KifFunction* kfunction, KifCallFunction* kcf, bool add, KifFunction** body) {
	//If we have more than one function available, we go for the 
	//the call with the most compatible arguments...		
	int arg;
	if (kfunction->next == NULL) {
		if (kfunction->Unlimited() == false && KifCompareArgs(kfunction, kcf) == false)
			return 1;
		if (add == false)
			*body = kfunction;
		for (arg = 0; arg < kcf->parameters.size(); arg++) {
			if (KifAnalyzeParameter(kfunction->arguments[arg]->Linkedvalue(), kcf->parameters[arg], kfunction->strict) == false)
				return 2;
		}
		return 0;
	}

	int nbcomp = 0;
	KifFunction* kfnc = NULL;
	bool found;

	while (kfunction != NULL) {
		if (kfunction->Unlimited() == false && KifCompareArgs(kfunction, kcf) == false) {
			kfunction = kfunction->next;
			continue;
		}

		if (kfunction->arguments.size() == 0) {
			if (add)
				kcf->Setbody(kfunction);
			else
				*body = kfunction;
			return 0;
		}

		found = true;
		//We found one fully compatible function, no need to go further
		for (arg = 0; arg < kcf->parameters.size(); arg++) {
			if (KifSameParameters(kfunction->arguments[arg]->Linkedvalue(), kcf->parameters[arg]) == false) {
				found = false;
				break;
			}
		}

		if (found) {
			if (add)
				kcf->Setbody(kfunction);
			else
				*body = kfunction;
			return 0;
		}

		found = true;
		for (arg = 0; arg < kcf->parameters.size(); arg++) {
			if (KifAnalyzeParameter(kfunction->arguments[arg]->Linkedvalue(), kcf->parameters[arg], true) == false) {
				found = false;
				break;
			}
		}

		if (found) {
			nbcomp++;
			if (kfnc == NULL)
				kfnc = kfunction;
		}

		kfunction = kfunction->next;
	}

	//We found no compatible function, it is an error...
	if (!nbcomp)
		return 2;

	if (nbcomp > 1) {
		if (kcf->type == kifCallFunctionGeneral || kcf->type == kifCallExecuteThread)
			((KifCallFunctionGeneral*)kcf)->dynamicselection = LOWDYNAMIC; //In this case we have already tested the initial comparison between arguments...
	}

	if (add)
		kcf->Setbody(kfnc);
	else
		*body = kfnc;
	return 0;
}


//===================================================================
#ifdef WIN32
void termination_function() {
	char ch[1024];
	int id = kifGlobals->GetThreadid();
	for (int i = kifGlobals->THR[id]->stackelements.size() - 1; i >= 0; i--) {
		string mess = KIFKIFSTRINGS[3];
		if (kifGlobals->THR[id]->stackelements[i]->name != -1)
			mess = kifGlobals->kifIdString[kifGlobals->THR[id]->stackelements[i]->name];
		mess += ": ";
		long line = kifGlobals->THR[id]->stackelements[i]->line;
		string filename = DebugPtr()->Filename(kifGlobals->THR[id]->stackelements[i]->file);
		sprintf_s(ch, 1024, KIFKIFSTRINGS[4], line, STR(filename));
		sprintf_s(ch, 1024, KIFKIFSTRINGS[4], line, STR(filename));
		mess += ch;
		cerr << mess << endl;
	}
	exit(-1);
}
#endif

void KifCode::Terminate() {
	kifGlobals->Releaseallloquet();
	ThreadLock _lock(kifPrepareError);
	ThreadLock _lockxThreadLock(kifError);
	kifGlobals->executionbreak = 1;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------



//===================================================================

void KifCode::KifInitLib() {
	if (kifGlobals != gGlobals)
		kifGlobals = gGlobals;
}

Exported string KifFrame::Type() {
	KifContainerLock _lock(this);
	return kifGlobals->kifIdString[name];
}

inline bool KifFunction::CheckFunctionFrame(KifFrame* vframe) {
	if (frame == (KifElement*)vframe || top == vframe->declarations[name])
		return true;
	return false;
}


Exported void KifFunction::Setlimited(bool l) {
	nonlimited = l;
	string s = "_";
	s += kifGlobals->kifIdString[name];
	int id = KifNewId(s);
	declarations[id] = kifNULL;
}


Exported string KifFunction::Type() {
	return kifGlobals->kifStringType[type];
}

string KifVariableDeclaration::Type() {
	if (value == this)
		return kifGlobals->kifStringType[type];
	return value->Type();
}

string KifConst::Type() {
	if (value == this)
		return kifGlobals->kifStringType[type];
	return value->Type();
}

//===================================================================
KifConst::KifConst(KifCode* klc, KifElement* kp, short n, bool a) : KifCommonVariable(klc, kp, this, kifVOID) {
	isbreak = false;
	name = n;
	add = a;
	if (n == idnull || n == idnoelement)
		type = kifnull;
}

//===================================================================

KifString::KifString(KifCode* klc, KifElement* kp, string v, KifType t) : _locker(NULL, false), KifElement(klc, kp, t) {
	value = v;
}

long KifString::Size() {
	KifContainerLock _lock(this);
	return size_c(value);
}

long KifRawString::Integer() {
	if (value == NULL)
		return 0;
	return conversionintegerhexa((char*)value);
}

double KifRawString::Float() {
	if (value == NULL)
		return 0;
	return convertfloat((char*)value);
}

long KifString::Integer() {
	KifContainerLock _lock(this);
	if (value == "")
		return 0;
	return conversionintegerhexa(STR(value));
}

double KifString::Float() {
	KifContainerLock _lock(this);
	if (value == "")
		return 0;
	return convertfloat(STR(value));
}

//===================================================================
long KifUString::Size() {
	KifContainerLock _lock(this);
	return value.size();
}

long KifUString::Integer() {
	KifContainerLock _lock(this);
	if (value == L"")
		return 0;
	string v;
	sc_unicode_to_utf8(v, value);
	return conversionintegerhexa(STR(v));
}

double KifUString::Float() {
	KifContainerLock _lock(this);
	if (value == L"")
		return 0;
	string v;
	sc_unicode_to_utf8(v, value);
	return convertfloat(STR(v));
}

Exported string KifUString::Bytes() {
	KifContainerLock _lock(this);
	string buffer;
	s_unicode_to_utf8(buffer, value);
	return buffer;
}

wstring KifUString::UString() {
	KifContainerLock _lock(this);
	return value;
}
//===================================================================
long KifBaseFile::Size() {
	ThreadLock _lock(type);
	FILE* flocal;
	struct stat scible;
	int stcible = -1;
	long size = -1;
#ifdef WIN32
	fopen_s(&flocal, STR(filename), "rb");
#else
	flocal = fopen(STR(filename), "rb");
#endif

	if (flocal != NULL) {

#if  defined(WIN32) | defined(APPLE) | defined(XIPFSTAT64)
		stcible = fstat(flocal->_file, &scible);
#else
		stcible = fstat(flocal->_fileno, &scible);
#endif
		if (stcible >= 0)
			size = scible.st_size;
		fclose(flocal);
	}
	return size;
}

long KifBaseFile::Integer() {
	return Size();
}

double KifBaseFile::Float() {
	return Size();
}


Exported string KifBit::String() {
	ThreadLock _lock(type);
	static char dbin[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	static unsigned short masks[] = { 0xF000, 0xF00, 0xF0, 0xF };
	string s = "0x";
	unsigned short v;
	bool first = true;
	short shift;
	for (int sz = bitvector.size() - 1; sz >= 0; sz--) {
		shift = 12;
		for (int i = 0; i < 4; i++) {
			v = bitvector[sz] & masks[i];
			if (!first || v != 0) {
				v >>= shift;
				s += dbin[v];
				first = false;
			}
			shift -= 4;
		}
	}
	if (first)
		s += "0";
	return s;
}

Exported string KifBits::String() {
	static char dbin[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	static unsigned short masks[] = { 0xF000, 0xF00, 0xF0, 0xF };
	string s = "0x";
	unsigned short v;
	short shift;
	bool first = true;
	map<short, unsigned short>::reverse_iterator it;

	ThreadLock _lock(type);
	for (it = bitmap.rbegin(); it != bitmap.rend(); it++) {
		if (!first)
			s += "x";
		if (it->first == 0)
			s += "0";
		else {
			shift = 12;
			for (int i = 0; i < 4; i++) {
				v = it->first & masks[i];
				if (v != 0) {
					v >>= shift;
					s += dbin[v];
				}
				shift -= 4;
			}
		}
		shift = 12;
		s += "_";
		for (int i = 0; i < 4; i++) {
			v = it->second & masks[i];
			if (!first || v != 0) {
				v >>= shift;
				s += dbin[v];
				first = false;
			}
			shift -= 4;
		}
	}
	if (first)
		s += "0";
	return s;
}

Exported string KifDomain::BaseString() {
	KifContainerLock _lock(this);
	bin_hash<KifElement*>::iterator it;
	string n = "{";
	for (it = declarations.begin(); it != declarations.end(); it++) {
		if (n != "{")
			n += ",";
		n += ".";
		n += KifIdString(it->first);
		n += ":";
		if (it->second == this)
			n += "...";
		else
			n += it->second->BaseString();
	}
	n += "}";
	return n;
}


Exported string KifEnum::String() {
	threadblocinit;
	KifElement* ke = Exec(kifNULL, kifcode->kstart, kifNULL, idthread, callfunc);
	return ke->String();
}

string KifConst::String() {
	if (name == iduniversal)
		return "_";
	return "";
}

string KifConst::StringForDisplay() {
	if (kifGlobals->kifIdString.find(name) != kifGlobals->kifIdString.end())
		return kifGlobals->kifIdString[name];
	return "";
}

string KifInstruction::String() {
	stringstream v;
	bool par = false;
	if (instructions[0]->Action() >= kifPLUS && instructions[0]->Action() <= kifINTERSECTION) {
		v << "(";
		par = true;
	}
	v << instructions[0]->String();
	if (par)
		v << ")";
	if (action >= kifAFFECTATION && action <= kifMINUSMINUS) {
		v << kifGlobals->kifOperatorString[action];
		if (instructions.size() > 1) {
			par = false;
			if (instructions[1]->Action() >= kifPLUS && instructions[1]->Action() <= kifINTERSECTION) {
				if (action >= kifPLUS && action <= kifINTERSECTION) {
					v << "(";
					par = true;
				}
			}
			v << instructions[1]->String();
			if (par)
				v << ")";
		}
		return v.str();
	}
	return v.str();
}

//===================================================================
KifFrame::KifFrame(KifCode* klc, KifElement* kp, short n, KifType ktype) : KifDomainInstruction(klc, kp, n, ktype) {
	declared = false;
	parent = NULL;
	function = NULL;
	if (kp != NULL && kp->type == kifFrame)
		parent = (KifFrame*)kp;
	prive = false;
	if (klc == NULL)
		kifGlobals->frames[n] = this;
	else
		prive = true;
}

Exported bool KifStaticVariable::Setvalue(KifElement* k, KifElement* index, KifElement* dom) {
	if (init)
		kifcode->Returnerror(KIFLIBSTRINGS[250]);
	init = true;
	return value->Setvalue(k, index, dom);
}

Exported bool KifThroughVariable::Setvalue(KifElement* k, KifElement* index, KifElement* dom) {
	return value->Setvalue(k, index, dom);
}

Exported bool KifBits::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {

	if (idx != NULL) {
		int ikey = idx->Integer();
		long v = ke->Integer();
		if (v != 0 && v != 1) {
			kifcode->Returnerror(KIFKIFSTRINGS[5]);
			return true;
		}
		int bidx = Bitindex(ikey);
		int bpos = Bitposition(ikey, bidx);
		ThreadLock _lock(type);
		if (v == 0)
			bitmap[bidx] &= ~(1 << bpos);
		else
			bitmap[bidx] |= 1 << bpos;
		return true;
	}

	KifBits* kbit = Bits(ke);
	ThreadLock _lock(type);
	bitmap.clear();
	bitmap = kbit->bitmap;
	if (kbit != (KifBits*)ke)
		delete kbit;
	return true;
}


Exported bool KifFrame::Setvalue(KifElement* ke, KifElement* kkey, KifElement* dom) {
	KifContainerLock _lock(this);
	string key = kkey->String();
	short skey = KifNewId(key);
	if (declarations.check(skey)) {
		string message = KIFLIBSTRINGS[234];
		message += key;
		return kifcode->Returnerror(message);
	}

	if (!ke->Variable()) {
		KifVariable* var = new KifVariable(kifcode, NULL, ke, skey);
		ke = var;
	}

	ke->Setreference();
	declarations[skey] = ke;
	return true;
}


Exported KifElement* KifCallFunctionBreakpoint::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	DebugPtr()->kifdebug = true;
	DebugPtr()->kifdebugging = true;
	return kifTRUE;
}


Exported KifElement* KifFunctionSynchronous::Exec(KifElement* var, KifDomain* dom, KifElement* newvalue, int idthread, KifCallFunction* callfunc) {
	//var is our variable
	LockedThread* l = kifGlobals->AddLoquet(var->idgarbage);
	if (newvalue != NULL && newvalue->Boolean() == false) {
		//we use its idgarbage, which is unique as a way to trigger our semaphore
		//then we check, if we have any specific semaphore on this variable
		l->Released();
		return kifTRUE;
	}
	return kifTRUE;
}

Exported KifElement* KifFrame::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*)  {
	KifContainerLock _lock(this);

	if (val != NULL && !val->isConst()) {
		string key = val->String();
		short skey = KifStringId(key);
		if (skey == -1 || !declarations.check(skey)) {
			string message = KIFLIBSTRINGS[233];
			message += key;
			return kifcode->Returnerror(message);
		}

		return declarations[skey]->Returnvalue();
	}
	string message = KIFLIBSTRINGS[233];
	return kifcode->Returnerror(message);
}

Exported KifElement* KifFrame::Map(KifElement* context, KifMap* m) {
	KifContainerLock _lock(this);

	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, context);
	else
		kmap->Clear();
	bin_hash<KifElement*>::iterator it;
	short sn;
	for (it = declarations.begin(); it != declarations.end(); it++) {
		sn = it->first;
		if (sn <= idstdin)
			continue;
		if (kifGlobals->flfrom != -1 && (sn >= kifGlobals->flfrom && sn <= kifGlobals->flto))
			continue;
		string s = KifIdString(sn);
		if (it->second->isFunction())
			kmap->Push(s, kifcode->Providestringraw(KIFKIFSTRINGS[6]));
		else
		if (it->second->type == kifDomain) {
			KifDomain* d = (KifDomain*)it->second;
			if (d->frame != NULL) {
				string sx = KifIdString(d->frame->Name());
				kmap->Push(s, kifcode->Providestring(sx));
			}
			else
				kmap->Push(s, kifcode->Providestringraw(KIFKIFSTRINGS[7]));
		}
		else {
			string sx = it->second->String();
			if (sx == "")
				sx = kifGlobals->kifStringType[it->second->type];
			kmap->Push(s, kifcode->Providestring(sx));
		}
	}
	return kmap;
}

Exported string KifFrame::String() {
	KifMap* m = (KifMap*)Map(this);
	return m->String();
}

Exported void KifFrame::Pop(KifElement* kkey) {
	KifContainerLock _lock(this);

	string key = kkey->String();
	short skey = KifStringId(key);
	if (skey == -1) {
		string message = KIFLIBSTRINGS[233];
		message += key;
		kifcode->Returnerror(message);
	}
	KifElement* res = declarations[skey];
	declarations.erase(skey);
	res->Resetreference();
}

//===================================================================
KifDate::KifDate(KifCode* klc, KifElement* kp, time_t v) : KifElement(klc, kp, kifDate) {
	if (v == 0)
		time(&value);
	else
		value = v;
}
//===================================================================
//Constructor implementation for a few pre-defined types.

KifElement* KifDomain::constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	KifElement* func = frame->Declared(id_initial);

	postpone = false;
	//If such a function has been defined, we use it...
	if (func == NULL) {
		if (params == NULL || params == kifNULL)
			return kifFALSE;
		//else this is an initialisation with a variable
		params->Exec(this, dom, this, idthread, callfunc);
		return kifTRUE;
	}



	KifCallFunctionGeneral kfunc(NULL, NULL, id_initial, (KifFunction*)func);
	kfunc.kifcode = kifcode;
	KifInstruction* ki = NULL;
	if (params->isInstruction()) {
		ki = (KifInstruction*)params;
		for (int i = 0; i < ki->instructions.size(); i++) {
			params = ki->instructions[i]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			params->Setreference();
			kfunc.parameters.push_back(params);
		}
	}

	params = kfunc.Exec(kifNULL, this, kifNULL, idthread, callfunc);

	if (ki != NULL) {
		//we might need to clean our variables...
		for (int i = 0; i < ki->instructions.size(); i++)
			kfunc.parameters[i]->Resetreference();
	}
	return params;
}

KifElement* KifDomain::destructor() {
	KifContainerLock _lock(this);
	KifElement* func = frame->Declared(id_final);
	//If such a function has been defined, we use it...
	if (func == NULL)
		return kifFALSE;

	bool d = DebugPtr()->kifdebugging;
	DebugPtr()->kifdebugging = false;
	//KifCallFunctionLocal kfunc(kifcode, id_final, (KifFunction*)func);
	KifCallFunctionGeneral kfunc(NULL, NULL, id_final, (KifFunction*)func);
	kfunc.kifcode = kifcode;
	threadblocinit;
	kfunc.Exec(kifNULL, this, kifNULL, idthread, callfunc);
	DebugPtr()->kifdebugging = d;
	return kifTRUE;
}

Exported KifElement* KifBaseFile::constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
	if (params->InstructionSize() == 0)
		return this;

	if (params->InstructionSize() > 2)
		return kifcode->Returnerror(KIFLIBSTRINGS[245]);


	KifElement* kstr = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	string filename = kstr->String();
	kstr->Release();
	string reading(KIFKIFSTRINGS[8]);

	if (params->InstructionSize() == 2) {
		kstr = params->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		reading = kstr->String();
		kstr->Release();
	}

	if (reading == KIFKIFSTRINGS[8]) {
		if (!Openread(filename)) {
			string mess = KIFLIBSTRINGS[32];
			mess += filename;
			return kifcode->Returnerror(mess);
		}
	}
	else
	if (reading == KIFKIFSTRINGS[9]) {
		if (!Openwrite(filename)) {
			string mess = KIFLIBSTRINGS[35];
			mess += filename;
			return kifcode->Returnerror(mess);
		}
	}
	else
	if (reading == KIFKIFSTRINGS[10] || reading == "a") {
		if (!Openappend(filename)){
			string mess = KIFLIBSTRINGS[35];
			mess += filename;
			return kifcode->Returnerror(mess);
		}
	}
	else
		return kifcode->Returnerror(KIFLIBSTRINGS[245]);
	return this;
}

Exported KifElement* KifFraction::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	KIFLONG ib = b->Long();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	KIFLONG i = a->Long() % ib;
	return new KifLong(kifcode, NULL, i);
}


Exported KifElement* KifFraction::constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
	if (params->InstructionSize() == 0)
		return this;
	KifElement* kinit = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	if (params->InstructionSize() == 2) {
		numerator = kinit->Integer();
		denominator = params->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc)->Integer();
		Simplify();
	}
	else {
		if (kinit->type != kifFraction) {
			kinit = kinit->Fraction();
			numerator = kinit->N();
			denominator = kinit->D();
		}
		else {
			numerator = kinit->N();
			denominator = kinit->D();
			Simplify();
		}
	}
	kinit->Release();
	return this;
}

Exported KifElement* KifBit::constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
	if (params->InstructionSize() == 0)
		return this;
	//We have the size in bits of our special guest
	KifElement* kinit = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	int sz = kinit->Integer() - 1;
	kinit->Release();
	sz >>= 4;
	ThreadLock _lock(type);
	while (sz >= 0) {
		bitvector.push_back(0);
		sz--;
	}
	return this;
}

Exported KifElement* KifDate::constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
	if (params->InstructionSize() == 0)
		return this;

	KifElement* kinit = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

	if (kinit->type == kifDate) {
		memcpy((void*)&value, (void*)&((KifDate*)kinit)->value, sizeof(time_t));
		kinit->Release();
		return kifTRUE;
	}

	time_t x = 0;
	struct tm* temps = localtime(&x);

	int fulldate = 0;


	//Year
	long res = kinit->Integer();
	kinit->Release();

	if (res > 0) {
		temps->tm_year = res - 1900;
		fulldate = 100;
	}


	//Month
	if (params->InstructionSize() >= 2) {
		res = params->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc)->Integer();
		if (res > 0) {
			fulldate += 10;
			temps->tm_mon = res - 1;
		}
	}

	//Day
	if (params->InstructionSize() >= 3) {
		res = params->Instruction(2)->Exec(kifNULL, dom, kifNULL, idthread, callfunc)->Integer();
		if (res > 0) {
			fulldate += 1;
			temps->tm_mday = res;
		}
	}

	//Hour
	if (params->InstructionSize() >= 4) {
		res = params->Instruction(3)->Exec(kifNULL, dom, kifNULL, idthread, callfunc)->Integer();
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
	if (params->InstructionSize() >= 5) {
		res = params->Instruction(4)->Exec(kifNULL, dom, kifNULL, idthread, callfunc)->Integer();
		if (res >= 0 && res < 60)
			temps->tm_min = res;
	}

	//Second
	if (params->InstructionSize() >= 6) {
		res = params->Instruction(5)->Exec(kifNULL, dom, kifNULL, idthread, callfunc)->Integer();
		if (res >= 0 && res < 60)
			temps->tm_sec = res;
	}


	x = mktime(temps);
	if (x <= 0)
		return kifFALSE;
	value = x;
	return kifTRUE;

}

//--------------------------------------------------------------------------------------
KifElement* KifDate::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Time::Copy");
#endif
	KifElement* ke = new KifDate(kifcode, (KifElement*)kp, 0);
	return ke;
}

KifElement* KifDate::Newinstance() {
	KifDate* ke = new KifDate(kifcode, NULL);
	return ke;
}
//----------------------------------------------------------------------------------------

KifTime::KifTime(KifCode* klc, KifElement* kp) : KifElement(klc, kp, kifTime) {
	gettimeofday(&value, NULL);
}

KifElement* KifTime::Copy(KifDomain* kp, KifElement* dom) {
#ifdef PROFILING
	profilingcall("Time::Copy");
#endif
	KifElement* ke = new KifTime(kifcode, (KifElement*)kp);
	return ke;
}

KifElement* KifTime::Newinstance() {
	KifTime* ke = new KifTime(kifcode, NULL);
	return ke;
}

//===================================================================
KifInstance::KifInstance(KifCode* klc, KifElement* kp, string n, KifCommonVariable* d) : KifInstanceCommon(klc, kp, -1, d) {
	if (d != NULL)
		value = d->VariableValue();
	index = NULL;
	self = false;
	name = KifNewId(n);
}

KifVariableDeclaration::KifVariableDeclaration(KifCode* klc, KifElement* kp, string n, KifType kt) : KifCommonVariable(klc, kp, kifNULL, kt) {
	name = KifNewId(n);
	initialisation = kifNULL;
	function = NULL;
}


//===================================================================
KifCallFunctionExternal::KifCallFunctionExternal(KifCode* klc, KifElement* kp, short n, KifCallMethod* b) : KifCallFunctionWithClean(klc, kp, n) {
	body = b;
	type = kifCallFunctionExternal;
}
//===================================================================
bool KifFile::Write(string n) {
	ThreadLock _lock(type);
	if (mode < 2)
		return false;

	if (first) {
		if (signature)
			*thefile << (uchar)239 << (uchar)187 << (uchar)191;
		first = false;
	}

	thefile->write(STR(n), n.size());
	return true;
}

bool KifFile::WriteBin(int c) {
	ThreadLock _lock(type);
	if (c < 0 || c >= 256)
		return kifcode->Returnerror(KIFLIBSTRINGS[254]);
	if (mode < 2)
		return false;
	if (first) {
		if (signature)
			*thefile << (uchar)239 << (uchar)187 << (uchar)191;
		first = false;
	}
	*thefile << (char)c;
	return true;
}

long KifFile::get() {
	if (fileread != NULL)
		c = fgetc(fileread);
	else
		c = thefile->get();
	return c;
}

void KifFile::Unget(int c) {
	if (fileread != NULL)
		ungetc(c, fileread);
	else
		thefile->unget();
}

//239,187,191,
long KifFile::Get() {
	c = get();
	if (first) {
		if (signature) {
			if (c == 239) {
				c = get();
				if (c == 187) {
					c = get();
					if (c == 191)
						c = get();
					else {
						Unget(c);
						Unget(187);
						return 239;
					}
				}
				else {
					Unget(c);
					return 239;
				}
			}
		}
		first = false;
	}
	return c;
}

void KifFile::readnb(int nb, vector<unsigned char>& buffer) {
	if (EndOfFile()->Boolean() == true)
		return;

	ThreadLock _lock(type);
	fbuffer[0] = 0;

	int nbread, i;

	while (nb) {
		if (fileread != NULL) {
			if (!feof(fileread))
				return;
			nbread = fread(fbuffer, 1, 4095, fileread);
		}
		else {
			if (thefile->eof())
				return;
			thefile->read((char*)fbuffer, 4095);
			nbread = thefile->gcount();
		}

		if (nbread == 0)
			return;

		if (nb != -1) {
			if (nbread >= nb) {
				nbread = nb;
				nb = 0;
			}
			else
				nb -= nbread;
		}

		fbuffer[nbread] = 0;
		i = 0;
		if (first) {
			if (signature) {
				if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
					i = 3;
			}
			first = false;
		}
		for (; i < nbread; i++)
			buffer.push_back(fbuffer[i]);
	}
}

void KifFile::readnb(int nb, vector<long>& buffer) {
	if (EndOfFile()->Boolean() == true)
		return;

	ThreadLock _lock(type);
	fbuffer[0] = 0;

	int nbread, i;

	while (nb) {
		if (fileread != NULL) {
			if (!feof(fileread))
				return;
			nbread = fread(fbuffer, 1, 4095, fileread);
		}
		else {
			if (thefile->eof())
				return;
			thefile->read((char*)fbuffer, 4095);
			nbread = thefile->gcount();
		}

		if (nbread == 0)
			return;

		if (nb != -1) {
			if (nbread >= nb) {
				nbread = nb;
				nb = 0;
			}
			else
				nb -= nbread;
		}

		fbuffer[nbread] = 0;
		i = 0;
		if (first) {
			if (signature) {
				if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
					i = 3;
			}
			first = false;
		}
		for (; i < nbread; i++)
			buffer.push_back(fbuffer[i]);
	}
}

void KifFile::readnb(int nb, vector<double>& buffer) {
	if (EndOfFile()->Boolean() == true)
		return;

	ThreadLock _lock(type);
	fbuffer[0] = 0;

	int nbread, i;

	while (nb) {
		if (fileread != NULL) {
			if (!feof(fileread))
				return;
			nbread = fread(fbuffer, 1, 4095, fileread);
		}
		else {
			if (thefile->eof())
				return;
			thefile->read((char*)fbuffer, 4095);
			nbread = thefile->gcount();
		}

		if (nbread == 0)
			return;

		if (nb != -1) {
			if (nbread >= nb) {
				nbread = nb;
				nb = 0;
			}
			else
				nb -= nbread;
		}

		fbuffer[nbread] = 0;
		i = 0;
		if (first) {
			if (signature) {
				if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
					i = 3;
			}
			first = false;
		}
		for (; i < nbread; i++)
			buffer.push_back(fbuffer[i]);
	}
}

KifElement* KifFile::Read(int nb) {
	if (EndOfFile()->Boolean() == true)
		return kifNULL;

	ThreadLock _lock(type);
	fbuffer[0] = 0;

	int nbread, i;
	localstring->value = "";

	while (nb) {
		if (fileread != NULL) {
			if (!feof(fileread))
				return localstring;
			nbread = fread(fbuffer, 1, 4095, fileread);
		}
		else {
			if (thefile->eof())
				return localstring;
			thefile->read((char*)fbuffer, 4095);
			nbread = thefile->gcount();
		}

		if (nbread == 0)
			return localstring;

		if (nb != -1) {
			if (nbread >= nb) {
				nbread = nb;
				nb = 0;
			}
			else
				nb -= nbread;
		}

		fbuffer[nbread] = 0;
		i = 0;
		if (first) {
			if (signature) {
				if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
					i = 3;
			}
			first = false;
		}
		localstring->value += (char*)(fbuffer + i);
	}
	return localstring;
}

KifElement* KifFile::Readoneline() {
	if (EndOfFile()->Boolean() == true)
		return kifNULL;
	memset(fbuffer, 0, 4095);
	ThreadLock _lock(type);
	if (fileread != NULL) {
		fbuffer[0] = 0;
		fgets((char*)fbuffer, 4095, fileread);
		if (first) {
			if (signature) {
				if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
					memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
			}
			first = false;
		}
		localstring->value = (char*)fbuffer;
	}
	else {
		getline(*thefile, localstring->value);
		if (first) {
			if (signature) {
				if (localstring->value.size() >= 3) {
					fbuffer[0] = localstring->value[0];
					fbuffer[1] = localstring->value[1];
					fbuffer[2] = localstring->value[2];
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
						localstring->value = localstring->value.substr(3, localstring->value.size() - 3);
				}
				first = false;
			}
		}
		localstring->value += '\n';
	}
	return localstring;
}

KifElement* KifFile::Seek(long i) {
	if (fileread != NULL) {
		fseek(fileread, i, SEEK_SET);
		return kifTRUE;
	}
	if (thefile == NULL)
		return kifFALSE;
	thefile->clear();

	if (mode == 1)
		thefile->seekg(i, ios::beg);
	else {
		if (mode == 2)
			thefile->seekg(i, ios::beg);
		else
		if (mode == 3) {
			thefile->seekp(i, ios::beg);
			thefile->seekg(i, ios::beg);
		}
	}
	if (!i)
		first = true;

	return kifTRUE;
}

KifElement* KifFile::Tell() {
	long l = -1;
	if (fileread != NULL)
		l = ftell(fileread);
	else {
		if (thefile == NULL)
			return kifMINUSONE;

		if (mode == 1)
			l = thefile->tellg();
	}
	return kifcode->Provideinteger(l);
}

long KifFile::tell() {
	long l = -1;
	if (fileread != NULL)
		l = ftell(fileread);
	else {
		if (thefile == NULL)
			return l;
		if (mode == 1)
			l = thefile->tellg();
	}
	return l;
}

BULONG KifFile::getc(bool utf8) {
	if (!utf8)
		return get();

	unsigned char c = get();
	if (c < 0x0080)
		return c;

	unsigned char utf[] = { 0, 0, 0, 0, 0, 0 };
	BULONG code = c;
	if ((c & 0xE0) == 0xC0) {
		//We take two bytes
		utf[1] = get();
		if ((utf[1] & 0xC0) != 0x80) {
			Unget(utf[1]);
			return c;
		}
		code = (c ^ 0xc0) << 6;
		code |= utf[1] ^ 0x80;
		return code;
	}
	//we take three
	if ((c & 0xF0) == 0xE0) {
		utf[1] = get();
		if ((utf[1] & 0xC0) != 0x80) {
			Unget(utf[1]);
			return c;
		}
		utf[2] = get();
		if ((utf[2] & 0xC0) != 0x80) {
			Unget(utf[1]);
			Unget(utf[2]);
			return c;
		}
		code = (c ^ 0xE0) << 12;
		code |= (utf[1] ^ 0x80) << 6;
		code |= (utf[2] ^ 0x80);
		return code;
	}

	//we take four
	if ((c & 0xF8) == 0xF0) {
		utf[1] = get();
		if ((utf[1] & 0xC0) != 0x80) {
			Unget(utf[1]);
			return c;
		}
		utf[2] = get();
		if ((utf[2] & 0xC0) != 0x80) {
			Unget(utf[1]);
			Unget(utf[2]);
			return c;
		}
		utf[3] = get();
		if ((utf[3] & 0xC0) != 0x80) {
			Unget(utf[1]);
			Unget(utf[2]);
			Unget(utf[3]);
			return c;
		}
		code = (c ^ 0xF0) << 18;
		code |= (utf[1] ^ 0x80) << 12;
		code |= (utf[2] ^ 0x80) << 6;
		code |= (utf[3] ^ 0x80);
		return code;
	}

	//we take five
	if ((c & 0xFC) == 0xF8) {
		utf[1] = get();
		if ((utf[1] & 0xC0) != 0x80) {
			Unget(utf[1]);
			return c;
		}
		utf[2] = get();
		if ((utf[2] & 0xC0) != 0x80) {
			Unget(utf[1]);
			Unget(utf[2]);
			return c;
		}
		utf[3] = get();
		if ((utf[3] & 0xC0) != 0x80) {
			Unget(utf[1]);
			Unget(utf[2]);
			Unget(utf[3]);
			return c;
		}
		utf[4] = get();
		if ((utf[4] & 0xC0) != 0x80) {
			Unget(utf[1]);
			Unget(utf[2]);
			Unget(utf[3]);
			Unget(utf[4]);
			return c;
		}
		code = (c ^ 0xF8) << 24;
		code |= (utf[1] ^ 0x80) << 18;
		code |= (utf[2] ^ 0x80) << 12;
		code |= (utf[3] ^ 0x80) << 6;
		code |= (utf[4] ^ 0x80);
		return code;
	}

	//we take six
	if ((c & 0xFF) == 0xFC) {
		utf[1] = get();
		if ((utf[1] & 0xC0) != 0x80) {
			Unget(utf[1]);
			return c;
		}
		utf[2] = get();
		if ((utf[2] & 0xC0) != 0x80) {
			Unget(utf[1]);
			Unget(utf[2]);
			return c;
		}
		utf[3] = get();
		if ((utf[3] & 0xC0) != 0x80) {
			Unget(utf[1]);
			Unget(utf[2]);
			Unget(utf[3]);
			return c;
		}
		utf[4] = get();
		if ((utf[4] & 0xC0) != 0x80) {
			Unget(utf[1]);
			Unget(utf[2]);
			Unget(utf[3]);
			Unget(utf[4]);
			return c;
		}
		utf[5] = get();
		if ((utf[4] & 0xC0) != 0x80) {
			Unget(utf[1]);
			Unget(utf[2]);
			Unget(utf[3]);
			Unget(utf[4]);
			Unget(utf[5]);
			return c;
		}
		code = (c ^ 0xFC) << 30;
		code |= (utf[1] ^ 0x80) << 24;
		code |= (utf[2] ^ 0x80) << 18;
		code |= (utf[3] ^ 0x80) << 12;
		code |= (utf[4] ^ 0x80) << 6;
		code |= (utf[5] ^ 0x80);
		return code;
	}
	return c;
}


KifElement* KifFile::Find(KifElement* context, agnostring& sx, bool u) {
	if (EndOfFile()->Boolean() == true)
		return kifNULL;
	ThreadLock _lock(type);
	BULONG c;
	bool utf8 = false;
	if (u) {
		sx = sx.tolower();
		utf8 = sx.isutf8();
	}
	string& s = sx;
	int i = 0;
	long p = tell();
	KifElement* v = NULL;
	if (context->aContainer())
		v = Selectvectorinteger(kifNULL, context);
	c = getc(utf8);
	while (EndOfFile()->Boolean() == false) {
		if (utf8) {
			if (sx.end()) {
				if (v == NULL)
					return kifcode->Provideinteger(p);
				v->storevalue(p);
				sx.charpos = 0;
				sx.bytepos = 0;
			}
			c = c_to_lower(c);
			if (c != sx.nextcode()) {
				sx.charpos = 0;
				sx.bytepos = 0;
				p = tell();
			}
		}
		else {
			if (i >= s.size()) {
				if (v == NULL)
					return kifcode->Provideinteger(p);
				v->storevalue(p);
				i = 0;
			}
			if (u)
				c = c_to_lower(c);
			if ((uchar)c == (uchar)s[i])
				i++;
			else {
				i = 0;
				p = tell();
			}
		}
		c = getc(utf8);
	}
	if (v == NULL)
		return kifMINUSONE;
	return v;
}

//===================================================================
bool KifFileWide::Write(string n) {
	ThreadLock _lock(type);
	if (mode < 2)
		return false;

	w_u_char cw;

	if (first) {
		if (signature) {
			cw.cc[0] = 255;
			cw.cc[1] = 254;
			*thefile << cw.c;
		}
		first = false;
	}

	sc_utf8_to_unicode(localstring->value, STR(n));
	for (int i = 0; i < localstring->value.size(); i++) {
		c = localstring->value[i];
		*thefile << c;
	}
	return true;
}

bool KifFileWide::WriteBin(int n) {
	ThreadLock _lock(type);
	if (mode < 2)
		return false;
	if (first) {
		w_u_char cw;

		if (signature) {
			cw.cc[0] = 255;
			cw.cc[1] = 254;
			*thefile << c;
		}
		first = false;
	}
	c = n;
	*thefile << c;
	return true;
}

void KifFileWide::readnb(int nb, vector<unsigned char>& buffer) {
	if (EndOfFile()->Boolean() == true)
		return;

	ThreadLock _lock(type);
	fbuffer[0] = 0;

	int nbread, i, j;
	w_u_char wc;

	while (nb) {
		if (thefile->eof())
			return;
		thefile->read(fbuffer, 4095);
		nbread = thefile->gcount();

		if (nbread == 0)
			return;

		if (nb != -1) {
			if (nbread >= nb) {
				nbread = nb;
				nb = 0;
			}
			else
				nb -= nbread;
		}

		fbuffer[nbread] = 0;
		i = 0;
		if (first) {
			first = false;
			i = 1;
			w_u_char wc;
			wc.c = fbuffer[0];
			if (wc.cc[0] == 255 && wc.cc[1] == 254)
				signature = true;
		}
		for (i = 0; i < nbread; i++) {
			wc.c = fbuffer[i];
			for (j = 0; j < sizeof(wchar_t); j++)
				buffer.push_back(wc.cc[j]);
		}
	}
}

void KifFileWide::readnb(int nb, vector<double>& buffer) {
	if (EndOfFile()->Boolean() == true)
		return;

	ThreadLock _lock(type);
	fbuffer[0] = 0;

	int nbread, i;

	while (nb) {
		if (thefile->eof())
			return;
		thefile->read(fbuffer, 4095);
		nbread = thefile->gcount();

		if (nbread == 0)
			return;

		if (nb != -1) {
			if (nbread >= nb) {
				nbread = nb;
				nb = 0;
			}
			else
				nb -= nbread;
		}

		fbuffer[nbread] = 0;
		i = 0;
		if (first) {
			first = false;
			i = 1;
			w_u_char wc;
			wc.c = fbuffer[0];
			if (wc.cc[0] == 255 && wc.cc[1] == 254)
				signature = true;
		}
		for (i = 0; i < nbread; i++)
			buffer.push_back(fbuffer[i]);
	}
}

void KifFileWide::readnb(int nb, vector<long>& buffer) {
	if (EndOfFile()->Boolean() == true)
		return;

	ThreadLock _lock(type);
	fbuffer[0] = 0;

	int nbread, i;

	while (nb) {
		if (thefile->eof())
			return;
		thefile->read(fbuffer, 4095);
		nbread = thefile->gcount();

		if (nbread == 0)
			return;

		if (nb != -1) {
			if (nbread >= nb) {
				nbread = nb;
				nb = 0;
			}
			else
				nb -= nbread;
		}

		fbuffer[nbread] = 0;
		i = 0;
		if (first) {
			first = false;
			i = 1;
			w_u_char wc;
			wc.c = fbuffer[0];
			if (wc.cc[0] == 255 && wc.cc[1] == 254)
				signature = true;
		}
		for (i = 0; i < nbread; i++)
			buffer.push_back(fbuffer[i]);
	}
}


long KifFileWide::Get() {
	c = thefile->get();
	if (first) {
		w_u_char wc;
		wc.c = c;
		if (wc.cc[0] == 255 && wc.cc[1] == 254) {
			c = thefile->get();
			signature = true;
		}
		first = false;
	}
	return c;
}

KifElement* KifFileWide::Read(int nb) {
	if (EndOfFile()->Boolean() == true)
		return kifNULL;

	ThreadLock _lock(type);

	localstring->value = L"";
	fbuffer[0] = 0;

	int nbread, i;

	while (nb) {
		if (thefile->eof())
			return localstring;
		thefile->read(fbuffer, 4095);
		nbread = thefile->gcount();

		if (nbread == 0)
			return localstring;

		if (nb != -1) {
			if (nbread >= nb) {
				nbread = nb;
				nb = 0;
			}
			else
				nb -= nbread;
		}

		fbuffer[nbread] = 0;
		i = 0;
		if (first) {
			first = false;
			i = 1;
			w_u_char wc;
			wc.c = fbuffer[0];
			if (wc.cc[0] == 255 && wc.cc[1] == 254)
				signature = true;
		}
		localstring->value += (fbuffer + i);
	}

	return localstring;
}

KifElement* KifFileWide::Find(KifElement* context, agnostring& s, bool u) {
	if (EndOfFile()->Boolean() == true)
		return kifNULL;
	ThreadLock _lock(type);
	Get();
	long p = thefile->tellg();
	KifElement* v = NULL;
	if (context->aContainer())
		v = Selectvectorinteger(kifNULL, context);
	if (u)
		s = s.tolower();
	while (!thefile->eof()) {
		if (s.end()) {
			s.charpos = 0;
			s.bytepos = 0;
			if (v == NULL)
				return kifcode->Provideinteger(p);
			v->storevalue(p);
		}
		if (!u) {
			if (c != s.nextcode()) {
				p = thefile->tellg();
				s.charpos = 0;
				s.bytepos = 0;
			}
		}
		else {
			if (c_to_lower(c) != s.nextcode()) {
				p = thefile->tellg();
				s.charpos = 0;
				s.bytepos = 0;
			}
		}
		Get();
	}
	if (v == NULL)
		return kifMINUSONE;
	return v;
}

KifElement* KifFileWide::Readoneline() {
	if (EndOfFile()->Boolean() == true)
		return kifNULL;
	ThreadLock _lock(type);
	localstring->value = L"";
	Get();
	while (!thefile->eof() && c != 10 && c != 13) {
		localstring->value += c;
		Get();
	}

	localstring->value += L'\n';
	return localstring;
}

KifElement* KifFileWide::Seek(long i) {
	if (thefile == NULL)
		return kifFALSE;

	thefile->clear();

	if (mode == 1)
		thefile->seekg(i, std::wios::beg);
	else {
		if (mode == 2)
			thefile->seekg(i, std::wios::beg);
		else
		if (mode == 3) {
			thefile->seekp(i, std::wios::beg);
			thefile->seekg(i, std::wios::beg);
		}
	}
	if (!i)
		first = true;

	return kifTRUE;
}

KifElement* KifFileWide::Tell() {
	long l = -1;
	if (mode == 1)
		l = thefile->tellg();
	return kifcode->Provideinteger(l);
}

//===================================================================
//A variable is looked for, first in the function declarations, then in the domain, then among the global
//variables

KifElement* KifCode::Getbasic(short n, KifElement* dom) {
	KifElement* v;
	if (dom != NULL) {
		v = dom->Declaration(n);
		if (v != NULL)
			return v;
	}
	return kifGlobals->Get(n);
}

KifElement* KifCode::Getbasicfunction(short n, KifElement* dom) {
	KifElement* v;
	if (dom != NULL) {
		v = dom->Declarationfunction(n);
		if (v != NULL)
			return v;
	}
	return kifGlobals->Get(n);
}


KifElement* KifCode::Get(short n, KifDomain* dom) {
	KifElement* v;
	if (dom != NULL) {
		//we are trying to access a variable out of our scope in another kifcode area
		if (dom->kifcode == this || dom->kifcode->kstart != dom) {
			v = dom->Declared(n);
			if (v != NULL)
				return v;
		}
	}
	return kifGlobals->Get(n);
}

KifElement* KifCode::Getframe(short n, KifElement* dom) {
	KifElement* v;
	if (dom != NULL && dom->type == kifFrame) {
		v = dom->Declaration(n);
		if (v != NULL)
			return v;
	}
	return kifGlobals->Getframe(n);
}
//===================================================================

KifElement* KifDate::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Time::plus");
#endif
	if (autoself) {
		value += b->Integer();
		return this;
	}
	long i = a->Integer() + b->Integer();
	return new KifDate(kifcode, NULL, i);
}

KifElement* KifDate::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Time::minus");
#endif
	if (autoself) {
		value = difftime(value, b->Integer());
		return this;
	}
	time_t i = difftime(a->Integer(), b->Integer());
	return new KifDate(kifcode, NULL, i);
}

KifElement* KifTime::plus(KifElement* aa, KifElement* bb, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Time::minus");
#endif
	if (aa->type != kifTime && bb->type != kifTime)
		return kifNULL;

	KifTime* a = (KifTime*)aa;
	KifTime* b = (KifTime*)bb;

	return kifcode->Providefloat(plustime(a->value, b->value));
}

KifElement* KifTime::minus(KifElement* aa, KifElement* bb, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Time::minus");
#endif
	if (aa->type != kifTime && bb->type != kifTime)
		return kifNULL;

	KifTime* a = (KifTime*)aa;
	KifTime* b = (KifTime*)bb;
	return kifcode->Providefloat(minustime(a->value, b->value));
}
//===================================================================

//-----------------------------------------------------------------------------------------------
Exported KifElement* KifInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	double ib = b->Float();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	double iv = (double)value;
	if (autoself) {
		iv /= ib;
		value = iv;
		return this;
	}
	iv = a->Float() / ib;
	return kifcode->Providefloat(iv);
}

Exported KifElement* KifInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	long ib = b->Integer();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	if (autoself) {
		value %= ib;
		return this;
	}
	long i = a->Integer() % ib;
	return kifcode->Provideinteger(i);
}

Exported KifElement* KifInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself) {
		value = pow((double)value, b->Float());
		return this;
	}
	double i = pow(a->Float(), b->Float());
	return kifcode->Provideinteger(i);
}

Exported KifElement* KifByte::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	unsigned char ib = b->Byte();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	if (autoself) {
		value /= ib;
		return this;
	}
	unsigned char i = a->Byte() / ib;
	return new KifByte(kifcode, NULL, i);
}

Exported KifElement* KifByte::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	unsigned char ib = b->Byte();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	if (autoself) {
		value %= ib;
		return this;
	}
	unsigned char i = a->Byte() % ib;
	return new KifByte(kifcode, NULL, i);
}

Exported KifElement* KifByte::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself) {
		value = pow((double)value, b->Float());
		return this;
	}
	double i = pow(a->Float(), b->Float());
	return new KifByte(kifcode, NULL, i);
}

Exported KifElement* KifLong::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	double ib = b->Float();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	if (autoself) {
		value /= ib;
		return this;
	}
	double i = a->Float() / ib;
	return kifcode->Providefloat(i);
}

Exported KifElement* KifLong::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	KIFLONG ib = b->Long();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	if (autoself) {
		value %= ib;
		return this;
	}
	KIFLONG i = a->Long() % ib;
	return new KifLong(kifcode, NULL, i);
}

Exported KifElement* KifLong::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself) {
		value = pow((double)value, b->Float());
		return this;
	}
	double i = pow(a->Float(), b->Float());
	return new KifLong(kifcode, NULL, i);
}

Exported KifElement* KifShort::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	double ib = b->Float();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	if (autoself) {
		value /= ib;
		return this;
	}
	double i = a->Float() / ib;
	return kifcode->Providefloat(i);
}

Exported KifElement* KifShort::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	short ib = b->Short();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	if (autoself) {
		value %= ib;
		return this;
	}
	short i = a->Short() % ib;
	return new KifShort(kifcode, NULL, i);
}

Exported KifElement* KifShort::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself) {
		value = pow((double)value, b->Float());
		return this;
	}
	double i = pow(a->Float(), b->Float());
	return new KifShort(kifcode, NULL, (short)i);
}

Exported KifElement* KifFraction::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	double i = pow(a->Float(), b->Float());
	return kifcode->Provideinteger(i);
}

Exported KifElement* KifFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	double ib = b->Float();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	if (autoself) {
		value /= ib;
		return this;
	}
	double i = a->Float() / ib;
	return kifcode->Providefloat(i);
}

Exported KifElement* KifFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	long ib = b->Integer();
	if (ib == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	if (autoself) {
		value = (long)value%ib;
		return this;
	}
	long i = a->Integer() % ib;
	return kifcode->Provideinteger(i);
}

Exported KifElement* KifFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself) {
		value = pow(value, b->Float());
		return this;
	}
	double i = pow(a->Float(), b->Float());
	return kifcode->Providefloat(i);
}

//===================================================================

KifElement* KifIteratorString::IteratorKey() {
	return kifcode->Provideinteger(length);
}

KifElement* KifIteratorString::IteratorValue() {
	if (itx >= value.size())
		return kifNOELEMENT;
	ibase = itx;
	return kifcode->Providestringraw(c_char_get(USTR(value), ibase));
}

string KifIteratorString::IteratorValueString() {
	if (itx >= value.size())
		return "";
	ibase = itx;
	return c_char_get(USTR(value), ibase);
}
void KifIteratorString::Next() {
	c_char_get(USTR(value), itx);
	itx++;
	if (itx >= value.size())
		return;
	if (reverse)
		length--;
	else
		length++;
}

KifElement* KifIteratorString::End() {
	if (itx == value.size())
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifIteratorString::Begin() {
	itx = 0;
	length = 0;
	if (itx == value.size())
		return kifFALSE;

	if (reverse == true) {
		value = s_char_reverse(value, length);
		length--;
	}
	return kifTRUE;
}
//===================================================================

KifElement* KifIteratorUString::IteratorKey() {
	return kifcode->Provideinteger(itx);
}

KifElement* KifIteratorUString::IteratorValue() {
	if (itx >= value.size())
		return kifNOELEMENT;
	return kifcode->Provideustring(value[itx]);
}

string KifIteratorUString::IteratorValueString() {
	if (itx >= value.size())
		return "";
	return c_unicode_to_utf8(value[itx]);
}

wstring KifIteratorUString::IteratorValueUString() {
	if (itx >= value.size())
		return L"";
	wchar_t w[] = { value[itx], 0 };
	return w;
}

void KifIteratorUString::Next() {
	if (reverse) {
		itx--;
		return;
	}
	itx++;
}

KifElement* KifIteratorUString::End() {
	if (reverse) {
		if (itx <= 0)
			return kifTRUE;
	}
	else {
		if (itx >= value.size())
			return kifTRUE;
	}
	return kifFALSE;
}

KifElement* KifIteratorUString::Begin() {
	itx = 0;
	if (itx == value.size())
		return kifFALSE;

	return kifTRUE;
}

//===================================================================
KifElement* KifIteratorFile::IteratorKey() {
	return &thetell;
}

KifElement* KifIteratorFile::IteratorValue() {
	return value->Local();
}

void KifIteratorFile::Next() {
	KifElement* e = value->Tell();
	thetell.value = e->Integer();
	e->Release();
	value->Readoneline();
}

KifElement* KifIteratorFile::End() {
	if (value == NULL || value->mode != 1)
		return kifTRUE;
	return value->EndOfFile();
}

KifElement* KifIteratorFile::Begin() {
	if (value == NULL || value->mode != 1)
		return kifFALSE;
	value->Seek(0);
	thetell.value = 0;
	value->Readoneline();
	return kifTRUE;
}

//===================================================================

bool KifFile::Openappend(string n) {
	ThreadLock _lock(type);
	if (thefile != NULL)
		return false;
	n = Getfilename(n);
#ifdef WIN32
	thefile = new fstream((char*)n.c_str(), ios::app | ios::binary);
#else
	thefile = new fstream((char*)n.c_str(), ios::out | ios::app | ios::binary);
#endif
	if (thefile->fail()) {
		raz();
		return false;
	}
	filename = n;
	mode = 3;
	return true;
}

KifElement* KifBaseFile::Newiterator(bool rev) {
	return new KifIteratorFile(kifcode, NULL, this);
}

KifElement* KifRawString::Newiterator(bool rev) {
	KifIteratorString* iter = new KifIteratorString(kifcode, NULL);
	iter->reverse = rev;
	if (value != NULL)
		iter->value = (char*)value;
	return iter;
}

KifElement* KifString::Newiterator(bool rev) {
	KifIteratorString* iter = new KifIteratorString(kifcode, NULL);
	iter->reverse = rev;
	iter->value = value;
	return iter;
}

KifElement* KifUString::Newiterator(bool rev) {
	KifIteratorUString* iter = new KifIteratorUString(kifcode, NULL);
	iter->reverse = rev;
	iter->value = value;
	return iter;
}


KifElement* KifBit::Newiterator(bool rev) {
	KifIteratorBit* iter = new KifIteratorBit(kifcode, NULL);
	iter->reverse = rev;
	iter->values = &bitvector;
	iter->maxvalue = bitvector.size() << 4;
	return iter;
}

KifElement* KifBits::Newiterator(bool rev) {
	KifIteratorBits* iter = new KifIteratorBits(kifcode, NULL);
	iter->reverse = rev;
	iter->values = &bitmap;
	iter->maxvalue = Size();
	return iter;
}


Exported long KifBits::Size() {
	ThreadLock _lock(type);
	long sz = -1;
	map<short, unsigned short>::iterator it;
	for (it = bitmap.begin(); it != bitmap.end(); it++)
	if (it->first > sz)
		sz = it->first;
	return (sz + 1) << 4;
}

//===================================================================
//===================================================================
//-------------------------------------Add--------------------------------------------------------

void KifFunction::Add(KifElement* ke) {
	if (choice == 0)
		arguments.push_back(ke);
	else
	if (adding)
		instructions.push_back(ke);
}


Exported KifType KifInstance::inType() {
	if (index != NULL && index->function != NULL)
		return index->function->inType();
	if (value != NULL) {
		if (value->isInstance())
			return value->inType();
		return value->theType();
	}
	return theType();
}

KifType KifCallFunctionPredefined::inType() {
	if (body != NULL) {
		short nm = body->name;
		if (kifGlobals->kifMathFunctions.find(nm) != kifGlobals->kifMathFunctions.end())
			return kifFloat;
		if (kifGlobals->kifReturnString.find(nm) != kifGlobals->kifReturnString.end())
			return kifString;
		if (kifGlobals->kifReturnInt.find(nm) != kifGlobals->kifReturnInt.end())
			return kifInteger;
		if (kifGlobals->kifReturnFloat.find(nm) != kifGlobals->kifReturnFloat.end())
			return kifFloat;
	}

	return kifAny;
}

KifType KifInstanceFunction::inType() {
	if (function == NULL)
		return kifAny;
	return function->inType();
}


void KifInstruction::Addparent(KifElement* p) {
	parent = p;
}

void KifObjectInstruction::Addinstruction(int i, KifElement* ke) {
	if (i < instructions.size())
		instructions[i] = ke;
	else
		instructions.push_back(ke);
}


void KifObjectInstruction::Add(KifElement* ke) {
	if (adding)
		instructions.push_back(ke);
}


void KifCallFunction::Add(KifElement* ke) {
	if (add == 1)
		parameters.push_back(ke);
	else
	if (add == 2)
		function = (KifCallFunction*)ke;
}

//-------------------------------------Add--------------------------------------------------------

Exported void KifInstruction::Set(short op, KifElement* kf, bin_hash<KifElement*>* stacks) {
	action = (KifOperator)op;
}

void addvectorvalue(vector<short>& v, short s) {
	for (int i = 0; i < v.size(); i++)
	if (v[i] == s)
		return;
	v.push_back(s);
}

void KifFrame::Sharedeclaration(KifFrame* k) {
	KifContainerLock _lock(this);
	bin_hash<KifElement*>::iterator it;
	bool ad = false;
	if (k->ordered.size() != 0)
		ad = true;
	kifGlobals->compatibilities[k->name].set(name);
	for (it = declarations.begin(); it != declarations.end(); it++) {
		k->declarations[it->first] = it->second;
		if (!ad)
			k->ordered.push_back(it->first);
		else
			addvectorvalue(k->ordered, it->first);
	}
}


Exported void KifFrame::Set(short s, KifElement* k, bin_hash<KifElement*>* stacks) {
	KifContainerLock _lock(this);
	Setdeclaration(s, k, stacks);
	ordered.push_back(s);
}

Exported void KifFrame::Setstring(string sx, KifElement* k, bin_hash<KifElement*>* stacks) {
	KifContainerLock _lock(this);

	short s = KifNewId(sx);
	Setdeclaration(s, k, stacks);
	ordered.push_back(s);
}

Exported KifElement* KifFrame::Getstring(string sx) {
	KifContainerLock _lock(this);
	short s = KifNewId(sx);
	return Declaration(s);
}


short KifFrame::Newfield(string n, KifElement* val) {
	KifContainerLock _lock(this);
	short name = KifNewId(n);
	KifFrameDeclaration* var = new KifFrameDeclaration(kifcode, this, name, this);
	var->Add(val);
	Set(name, var, NULL);
	return name;
}


void KifCallFunction::SetParameters(vector<KifElement*>& vect, bin_hash<KifElement*>* stacks) {
	int it;
	for (it = 0; it < vect.size(); it++) {
		KifElement* ke = vect[it];
		short thename = ke->Name();
		if (ke->type == kifInstance)
			Setdeclaration(thename, ((KifInstance*)ke)->value, stacks);
		else {
			if (thename == -1)
				Setdeclaration(kifGlobals->kifStringId.size() + it + 1, ke, stacks);
			else
				Setdeclaration(thename, ke, stacks);
		}
		parameters.push_back(ke);
	}
}


Exported bool KifDate::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	long x = ke->Integer();
	if (x <= 0)
		return false;
	value = x;
	return true;
}

Exported bool KifTime::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	if (ke->type != kifTime)
		return false;
	value = ((KifTime*)ke)->value;
	return true;
}

Exported bool KifInteger::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	value = ke->Integer();
	return true;
}

Exported bool KifByte::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	value = ke->Byte();
	return true;
}

Exported bool KifFloat::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	value = ke->Float();
	return true;
}

void KifLoopString::callfunction() {
	threadblocinit;
	KifDomain* dom = kifGlobals->PreviousTopid(idthread);
	if (dom == NULL)
		dom = kifcode->kstart;

	KifCallFunctionGeneral kfunc(NULL, NULL, function->Name(), (KifFunction*)function);
	kfunc.kifcode = kifcode;
	KifElement* ki = kifcode->Provideinteger(position);
	ki->Setreference();
	kfunc.parameters.push_back(this);
	kfunc.parameters.push_back(ki);
	KifElement* ke = kfunc.Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	ke->Release();
	ki->Resetreference();
}

void KifLoopUString::callfunction() {
	threadblocinit;
	KifDomain* dom = kifGlobals->PreviousTopid(idthread);
	if (dom == NULL)
		dom = kifcode->kstart;
	KifCallFunctionGeneral kfunc(NULL, NULL, function->Name(), (KifFunction*)function);
	kfunc.kifcode = kifcode;
	KifElement* ki = kifcode->Provideinteger(position);
	ki->Setreference();
	kfunc.parameters.push_back(this);
	kfunc.parameters.push_back(ki);
	KifElement* ke = kfunc.Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	ke->Release();
	ki->Resetreference();
}

void KifLoopInteger::callfunction() {
	threadblocinit;
	KifDomain* dom = kifGlobals->PreviousTopid(idthread);
	if (dom == NULL)
		dom = kifcode->kstart;
	KifCallFunctionGeneral kfunc(NULL, NULL, function->Name(), (KifFunction*)function);
	kfunc.kifcode = kifcode;
	KifElement* ki = kifcode->Provideinteger(position);
	ki->Setreference();
	kfunc.parameters.push_back(this);
	kfunc.parameters.push_back(ki);
	KifElement* ke = kfunc.Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	ke->Release();
	ki->Resetreference();
}

void KifLoopFloat::callfunction() {
	threadblocinit;
	KifDomain* dom = kifGlobals->PreviousTopid(idthread);
	if (dom == NULL)
		dom = kifcode->kstart;
	KifCallFunctionGeneral kfunc(NULL, NULL, function->Name(), (KifFunction*)function);
	kfunc.kifcode = kifcode;
	KifElement* ki = kifcode->Provideinteger(position);
	ki->Setreference();
	kfunc.parameters.push_back(this);
	kfunc.parameters.push_back(ki);
	KifElement* ke = kfunc.Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	ke->Release();
	ki->Resetreference();
}


Exported bool KifLoopString::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	if (ke->aVectorContainer()) {
		position = 0;
		interval.clear();
		for (int i = 0; i < ke->Size(); i++)
			interval.push_back(ke->getstring(i));
		value = interval[0];
		return true;
	}

	if (ke->type == type) {
		KifLoopString* kl = (KifLoopString*)ke;
		interval = kl->interval;
		value = kl->value;
		position = kl->position;
		return true;
	}

	if (interval.size() == 0) {
		position = 0;
		value = "";
		return true;
	}

	position = ke->Integer();
	if (position >= interval.size())
		position = position % interval.size();
	else
	if (position<0)
		position = (interval.size() + position) % interval.size();
	value = interval[position];
	return true;
}

Exported bool KifLoopUString::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	if (ke->aVectorContainer()) {
		position = 0;
		interval.clear();
		for (int i = 0; i < ke->Size(); i++)
			interval.push_back(ke->getustring(i));
		value = interval[0];
		return true;
	}

	if (ke->type == type) {
		KifLoopUString* kl = (KifLoopUString*)ke;
		interval = kl->interval;
		value = kl->value;
		position = kl->position;
		return true;
	}

	if (interval.size() == 0) {
		position = 0;
		value = L"";
		return true;
	}

	position = ke->Integer();
	if (position >= interval.size())
		position = position % interval.size();
	else
	if (position<0)
		position = (interval.size() + position) % interval.size();

	value = interval[position];
	return true;
}

Exported bool KifLoopInteger::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	if (ke->aVectorContainer()) {
		position = 0;
		interval.clear();
		for (int i = 0; i < ke->Size(); i++)
			interval.push_back(ke->getinteger(i));
		value = interval[0];
		return true;
	}

	if (ke->type == type) {
		KifLoopInteger* kl = (KifLoopInteger*)ke;
		interval = kl->interval;
		value = kl->value;
		position = kl->position;
		return true;
	}

	if (interval.size() == 0) {
		position = 0;
		value = 0;
		return true;
	}

	position = ke->Integer();
	if (position >= interval.size())
		position = position % interval.size();
	else
	if (position<0)
		position = (interval.size() + position) % interval.size();
	value = interval[position];
	return true;
}

Exported bool KifLoopFloat::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	if (ke->aVectorContainer()) {
		position = 0;
		interval.clear();
		for (int i = 0; i < ke->Size(); i++)
			interval.push_back(ke->getfloat(i));
		value = interval[0];
		return true;
	}

	if (ke->type == type) {
		KifLoopFloat* kl = (KifLoopFloat*)ke;
		interval = kl->interval;
		value = kl->value;
		position = kl->position;
		return true;
	}

	if (interval.size() == 0) {
		position = 0;
		value = 0;
		return true;
	}

	position = ke->Integer();
	if (position >= interval.size())
		position = position % interval.size();
	else
	if (position<0)
		position = (interval.size() + position) % interval.size();
	value = interval[position];
	return true;
}


KifElement* KifLoopFloat::Vector(KifElement* dom, KifVector* v) {
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();

	for (int i = 0; i < interval.size(); i++)
		kvect->storevalue(interval[i]);
	return kvect;
}

KifElement* KifLoopInteger::Vector(KifElement* dom, KifVector* v) {
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();

	for (int i = 0; i < interval.size(); i++)
		kvect->storevalue(interval[i]);
	return kvect;
}

KifElement* KifLoopString::Vector(KifElement* dom, KifVector* v) {
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();

	for (int i = 0; i < interval.size(); i++)
		kvect->storevalue(interval[i]);
	return kvect;
}

KifElement* KifLoopUString::Vector(KifElement* dom, KifVector* v) {
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();

	for (int i = 0; i < interval.size(); i++)
		kvect->storevalue(interval[i]);
	return kvect;
}

Exported bool KifString::Boolean() {
	KifContainerLock _lock(this);
	if (value == "")
		return false;
	return true;
}

Exported unsigned char KifUString::Byte() {
	KifContainerLock _lock(this);
	if (value.size() == 0)
		return 0;
	return value[0];
}

Exported bool KifUString::Boolean() {
	KifContainerLock _lock(this);
	if (value == L"")
		return false;
	return true;
}

Exported string KifBoolean::String() {
	if (value)
		return KIFKIFSTRINGS[11];
	return KIFKIFSTRINGS[12];
}

Exported bool KifBoolean::Setvalue(KifElement* ke, KifElement* index, KifElement* dom) {
	value = ke->Boolean();
	return true;
}

Exported bool KifIterator::Setvalue(KifElement* k, KifElement* idx, KifElement* dom) {
	ThreadLock _lock(type);
	if (value != NULL && value->type == kifIteration)
		value->Resetreference();
	KifCode* kcode = k->kifcode;
	k->kifcode = kifcode;
	value = k->Newiterator(reverse);
	k->kifcode = kcode;
	value->Setreference();
	return true;
}

Exported bool KifIterator::Putvalue(KifElement* k) {
	ThreadLock _lock(type);
	if (value != NULL && value->type == kifIteration)
		value->Resetreference();
	KifCode* kcode = k->kifcode;
	k->kifcode = kifcode;
	value = k->Newiterator(reverse);
	k->kifcode = kcode;
	value->Setreference();
	return true;
}

Exported KifElement* KifSelf::Copy(KifDomain* kp, KifElement* dom) {
	return new KifSelf(kifcode, kp, kifNULL);
}

bool KifSelf::Setvalue(KifElement* ke, KifElement* prev, KifElement* dom) {
	KifContainerLock _lock(this);

	if (clean) {
		if (ke != value) {
			ke->Setreference();
			value->Resetreference(clean);
			clean = 1;
			value = ke;
		}
	}
	else {
		ke->Setreference();
		value = ke;
		clean = 1;
	}
	return true;
}

Exported KifElement* KifSelfFrame::Copy(KifDomain* kp, KifElement* dom) {
	return new KifSelfFrame(kifcode, kp, kifNULL, frame);
}

bool KifSelfFrame::Setvalue(KifElement* ke, KifElement* prev, KifElement* dom) {
	KifContainerLock _lock(this);

	if (frame != ke->Frame() && KifDerivation(frame, ke->Frame(), false) == false) {
		kifcode->Returnerror(KIFLIBSTRINGS[155]);
		return true;
	}

	if (clean) {
		if (ke != value) {
			ke->Setreference();
			value->Resetreference(clean);
			clean = 1;
			value = ke;
		}
	}
	else {
		ke->Setreference();
		value = ke;
		clean = 1;
	}
	return true;
}


KifSelf::KifSelf(KifCode* klc, KifElement* kp, KifElement* v) : _locker(NULL, false), localvariable(false), clean(0), parameter(kifNULL), KifCommonVariable(klc, kp, v, kifSelf) {}

Exported void KifSelf::Setreference(int inc) {
	reference += inc;
	Putpopped(0);
	if (clean) {
		value->Setreference(inc);
		clean++;
	}
}

Exported void KifSelf::Resetreference(int inc) {
	if (clean) {
		value->Resetreference(inc);
		clean--;
	}
	if (!clean)
		value = kifNULL;
	KifElement::Resetreference(inc);
}


bool KifVariable::Setvalue(KifElement* v, KifElement* idx, KifElement* dom) {
	if (value == v)
		return true;

	callfunction(value, v, (KifDomain*)dom, -1, NULL);

	if (value->type == kifDomain && v->type == kifFrame) {
		KifContainerLock _lock((KifDomain*)value);

		//In the case of a postpone variable, then this new affectation replaces any
		//delayed initialisation...
		//Then we have either a new instance, if k is a kifFrame
		//we need to check if we can use it...
		KifDomain* vdom = (KifDomain*)value;
		//If the value is postponed, it means that it has been created within a frame
		//which is waiting for its initialisation to take place...
		//In that case, we do not create a new object...
		if (!value->Postpone() && vdom->frame == v) {
			value = v->Copy(NULL, v);
			value->Setframe(v);
			vdom->Resetreference(reference);
			value->Setreference(reference);
			//value->Incrementreference();
		}
		return false;
	}

	if (value->reference > reference) {
		KifElement* nv = value->Newinstance();
		//In the other cases, we create a new instance and we set our value with k
		value->Resetreference(reference);
		value = nv;
		value->Setvalue(v, idx, dom);
		if (reference > 1)
			value->Setreference(reference - 1);
		value->reference++;
	}
	else
		value->Setvalue(v, idx, dom);

	v->Putpopped(0);
	return true;
}

bool KifVariableDeclaration::Setvalue(KifElement* k, KifElement* idx, KifElement* dom) {
	if (value->reference > 1) {

		//Simpler case, value and k have the same type		
		if (k->type == value->type && value->type != kifDomain) {
			value->Resetreference();
			k->Setreference();
			value = k;
			return true;
		}

		if (value->type == kifDomain && k->type == kifFrame) {
			//Simpler case, value and k have the same type
			//Then we have either a new instance, if k is a kifFrame
			//we need to check if we can use it...
			KifDomain* vdom = (KifDomain*)value;
			if (vdom->frame == k) {
				value = k->Copy(NULL, k);
				value->Setframe(k);
				//Simpler case, value and k have the same type
				for (int i = 0; i < reference; i++) {
					vdom->Resetreference();
					value->Setreference();
				}
			}
			return false;
		}
		value->Resetreference();
		//In the other cases, we create a new instance and we set our value with k
		value = value->Newinstance();
		value->Setreference();
		value->Setvalue(k, idx, dom);
	}
	else
		value->Setvalue(k, idx, dom);
	return true;
}

//------------------------------------Couple management--------------------------------------------------------
vector<KifCouple*> KifCouple::localgarbage;
int KifCouple::ilocalgarbage = 0;

KifCouple::KifCouple(KifCode* klc, KifElement* rec, KifIndex* i, KifDomain* dom, KifElement* f) : indexes(i), recipient(rec), domain(dom), function(f), KifElement(NULL, NULL, kifCouple) {
	igarbagerecipient = rec->Ingarbage();
	kifcode = klc;
	idgarbage = localgarbage.size();
	ilocalgarbage = idgarbage;
	localgarbage.push_back(this);
	returnvalue = false;
}

KifCouple::KifCouple(KifElement* rec, KifIndex* i, KifDomain* dom, KifElement* f) : indexes(i), recipient(rec), domain(dom), function(f), KifElement(NULL, NULL, kifCouple) {
	igarbagerecipient = rec->Ingarbage();
	returnvalue = false;
}

void KifCouple::Release() {
	//local definition, we do not need to clean it...
	if (idgarbage == -1)
		return;

	popped = 0;
	if (kifGlobals->Test()) {
		ThreadLock _lock(kifCouple);
		if (idgarbage < ilocalgarbage)
			ilocalgarbage = idgarbage;
	}
	else {
		if (idgarbage < ilocalgarbage)
			ilocalgarbage = idgarbage;
	}
}

void KifCouple::Resetreference(int inc) {
	reference -= inc;
	if (reference <= 0)
		Release();
}

//-------------------------------------------------------------------------------------------------------------------
bool KifCouple::Setvalue(KifElement* k, KifElement* idx, KifElement* dom) {
	//We simply replace, the current element with k
	threadblocinit;

	KifElement* key;

	if (indexes->function != NULL && indexes->function->type == kifIndex) {
		KifIndex* kidx = indexes;
		KifElement* klocal = recipient;
		vector<KifElement*> stack;

		while (kidx->function != NULL && kidx->function->type == kifIndex) {
			stack.push_back(kidx);
			stack.push_back(klocal);
			klocal = klocal->Exec(kifNULL, domain, kidx, idthread, callfunc);
			stack.push_back(klocal);
			kidx = (KifIndex*)kidx->function;
		}

		if (kidx->interval == false && !kidx->signkey)
			key = kidx->key->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
		else
			key = kidx;

		callfunction(key, k, domain, idthread, callfunc);

		klocal->Setvalue(k, key, dom);

		for (int i = stack.size() - 1; i >= 0; i -= 3) {
			klocal = stack[i];
			if (!klocal->popped || klocal->reference)
				break;
			stack[i - 1]->Setvalue(klocal, stack[i - 2], dom);
		}
		key->Release();
		Release();
		return true;
	}

	if (indexes->interval == false && !indexes->signkey)
		key = indexes->key->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	else
		key = indexes;

	callfunction(key, k, domain, idthread, callfunc);

	recipient->Setvalue(k, key, dom);
	key->Release();
	Release();
	return true;
}

//we need to build the last value/idx
KifElement* KifCouple::EvaluateKey(KifElement* k, KifElement* idx, KifElement* dom, int idthread, KifCallFunction* callfunc) {

	//Specific case, where the function is also an index reference...
	if (indexes->function != NULL && indexes->function->type == kifIndex)
		return kifcode->Returnerror(KIFKIFSTRINGS[13]);

	KifElement* key;

	if (indexes->interval == false)
		key = indexes->key->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	else
		key = indexes;

	callfunction(key, k, domain, idthread, callfunc);

	KifIndex* kind = new KifIndex(NULL, NULL, false);
	kind->kifcode = kifcode;
	kind->key = key->Atom();
	kind->key->Setreference();
	key->Release();
	key = new KifCouple(recipient, kind, (KifDomain*)dom, NULL);
	key->kifcode = kifcode;
	return key;
}

//In this case, we use the VALUE as the type
string KifCouple::Type() {
	threadblocinit;

	ThreadLock _lock(type);
	if (indexes->interval == true)
		return "";

	KifElement* key = indexes->key->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	KifElement* value = recipient->Exec(this, domain, key, idthread, callfunc);
	string v = value->Type();
	value->Release();
	key->Release();
	return v;
}
//===================================================================

KifElement* KifRawString::Map(KifElement* dom, KifMap* m) {
	string smap((char*)value);
	kifGlobals->PushStack((KifDomain*)dom);
	KifElement* ke = kifcode->EvaluateMap(dom, smap);
	kifGlobals->PopStack();
	if (ke->type != kifMap)
		return kifNULL;
	threadblocinit;
	ke = ke->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
	return ke;
}


KifElement* KifRawString::Vector(KifElement* dom, KifVector* v) {
	string svect((char*)value);
	kifGlobals->PushStack((KifDomain*)dom);
	KifElement* ke = kifcode->EvaluateVector(dom, svect);
	kifGlobals->PopStack();
	if (!ke->aVectorContainer())
		return kifNULL;
	threadblocinit;
	ke = ke->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
	return ke;
}


//===================================================================
bool KifString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;
	string s;
	if (ke->type > kifnull)
		s = ke->String();

	if (idx == NULL || idx->isConst()) {
		KifContainerLock _lock(this);
		value = s;
	}
	else {
		int ileft, iright;
		KifContainerLock _lock(this);
		char res = StringIndexes(value, idx, (KifDomain*)dom, ileft, iright, -1, NULL);

		if (res == 0) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return true;
		}

		if (res == 1) {
			iright = ileft;
			iright = c_char_next(USTR(value), iright);
		}
		else
			iright = iright - ileft;
		value.erase(ileft, iright);
		if (s != "")
			value.insert(ileft, s);
	}
	return true;
}

//===================================================================
bool KifUString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;
	wstring s;
	if (ke->type > kifnull)
		s = ke->UString();

	if (idx == NULL || idx->isConst()) {
		KifContainerLock _lock(this);
		value = s;
	}
	else {
		int ileft, iright;
		KifContainerLock _lock(this);
		char res = StringIndexes(value, idx, (KifDomain*)dom, ileft, iright, -1, NULL);

		if (res == 0) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return true;
		}

		if (res == 1)
			iright = ileft + 1;
		else
			iright = iright - ileft;

		value.erase(ileft, iright);
		if (s != L"")
			value.insert(ileft, s);
	}
	return true;
}

KifElement* KifString::Map(KifElement* dom, KifMap* m) {
	string smap;
	{
		KifContainerLock _lock(this);
		smap = value;
	}
	kifGlobals->PushStack((KifDomain*)dom);
	KifElement* ke = kifcode->EvaluateMap(dom, smap);
	kifGlobals->PopStack();
	if (ke->type != kifMap)
		return kifNULL;
	threadblocinit;
	ke = ke->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
	return ke;
}


KifElement* KifString::Vector(KifElement* dom, KifVector* v) {
	string svect;
	{
		KifContainerLock _lock(this);
		svect = value;
	}
	kifGlobals->PushStack((KifDomain*)dom);
	KifElement* ke = kifcode->EvaluateVector(dom, svect);
	kifGlobals->PopStack();
	if (!ke->aVectorContainer())
		return kifNULL;
	threadblocinit;
	ke = ke->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
	return ke;
}

KifElement* KifUString::Map(KifElement* dom, KifMap* m) {
	string smap;
	{
		KifContainerLock _lock(this);
		smap = String();
	}
	kifGlobals->PushStack((KifDomain*)dom);
	KifElement* ke = kifcode->EvaluateMap(dom, smap);
	kifGlobals->PopStack();
	if (ke->type != kifMap)
		return kifNULL;
	threadblocinit;
	ke = ke->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
	return ke;
}


KifElement* KifUString::Vector(KifElement* dom, KifVector* v) {
	string svect;
	{
		KifContainerLock _lock(this);
		svect = String();
	}
	kifGlobals->PushStack((KifDomain*)dom);
	KifElement* ke = kifcode->EvaluateVector(dom, svect);
	kifGlobals->PopStack();
	if (!ke->aVectorContainer())
		return kifNULL;
	threadblocinit;
	ke = ke->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
	return ke;
}

//===================================================================
KifElement* KifCallFunction::applyfunction(KifElement* a) {
	//we protect a from being deleted
	a->reference++;
	KifElement* ret;
	threadblocinit;
	if (a->type == kifDomain)
		ret = function->Exec(kifNULL, (KifDomain*)a, a, idthread, callfunc);
	else
		ret = function->Exec(kifNULL, this, a, idthread, callfunc);
	a->reference--;
	if (a != ret) {
		//we now protect ret from being deleted (it could belong to a, if a is a temporaty container)
		ret->Setreference();
		a->Release();
		ret->Putpopped(1);
		ret->Resetreference();
		ret->Putpopped(1);
	}
	else
		a->Putpopped(1);
	return ret;
}


Exported void KifCouple::callfunction(KifElement* a, KifElement* b, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (function != NULL) {
		KifFunction* func = function->Functioncall(dom);
		short name = func->Name();
		if (func->Size() != 2) {
			string message = KIFLIBSTRINGS[195];
			message += kifGlobals->kifIdString[name];
			kifcode->Returnerror(message);
			return;
		}

		//KifCallFunctionLocal kfunc(kifcode, name, (KifFunction*)func);
		KifCallFunctionGeneral kfunc(NULL, NULL, name, (KifFunction*)func);
		kfunc.kifcode = kifcode;
		kfunc.parameters.push_back(a);
		kfunc.parameters.push_back(b);
		a->reference++;
		b->reference++;

		if (idthread == -1) {
			idthread = kifGlobals->GetThreadid();
			callfunc = (KifCallFunction*)kifGlobals->Topid(idthread);
		}

		kfunc.Exec(kifNULL, func, kifNULL, idthread, callfunc);
		a->reference--;
		b->reference--;
	}
}

KifElement* KifElement::Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern) {
	return Selectvector(callfunc, contextualpattern);
}

KifElement* KifElement::Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern) {
	return Selectvector(callfunc, contextualpattern);
}

KifElement* KifElement::applyfunc(KifElement* a, KifElement* b, KifDomain* func) {
	if (func == NULL)
		return kifcode->Returnerror(KIFLIBSTRINGS[232]);

	short name = func->Name();
	if (func->Size() != 2) {
		string message = KIFLIBSTRINGS[195];
		message += kifGlobals->kifIdString[name];
		return kifcode->Returnerror(message);
	}
	threadblocinit;
	//KifCallFunctionLocal kfunc(kifcode, name, (KifFunction*)func);
	KifCallFunctionGeneral kfunc(NULL, NULL, name, (KifFunction*)func);
	kfunc.kifcode = kifcode;
	kfunc.parameters.push_back(a);
	kfunc.parameters.push_back(b);
	a->reference++;
	b->reference++;
	KifElement* ret = kfunc.Exec(kifNULL, func, kifNULL, idthread, callfunc);
	a->reference--;
	b->reference--;
	return ret;
}

KifElement* KifElement::applyfuncvariable(KifVariable* var, KifElement* a, KifElement* b, KifDomain* func, int idthread, KifCallFunction* callfunc) {
	if (func == NULL)
		return kifcode->Returnerror(KIFLIBSTRINGS[232]);

	//In the case of an object, we prefere avoiding applying the function, which might be applied on a field from that variable 
	if (var->Returnvalue()->type == kifDomain)
		return kifTRUE;

	if (func->IsSynchronous()) {
		func->Exec(var, NULL, b, idthread, callfunc);
		return kifTRUE;
	}

	short name = func->Name();
	if (func->Size() != 2) {
		string message = KIFLIBSTRINGS[195];
		message += kifGlobals->kifIdString[name];
		return kifcode->Returnerror(message);
	}
	//KifCallFunctionLocal kfunc(kifcode, name, (KifFunction*)func);
	KifCallFunctionGeneral kfunc(NULL, NULL, name, (KifFunction*)func);
	kfunc.kifcode = kifcode;
	kfunc.parameters.push_back(a);
	kfunc.parameters.push_back(b);
	a->reference++;
	b->reference++;
	KifElement* ret = kfunc.Exec(kifNULL, func, kifNULL, idthread, callfunc);
	a->reference--;
	b->reference--;
	return ret;
}

KifElement* KifElement::applyfuncinframe(KifVariable* var, KifElement* dom, KifElement* a, KifElement* b, KifDomain* func, int idthread, KifCallFunction* callfunc) {
	if (func == NULL)
		return kifcode->Returnerror(KIFLIBSTRINGS[232]);

	if (func->IsSynchronous()) {
		func->Exec(var, NULL, b, idthread, callfunc);
		return kifTRUE;
	}

	KifElement* arg0 = func->Argument(0);
	short name = func->Name();
	KifVariable* vardom = NULL;
	//KifCallFunctionLocal kfunc(kifcode, name, (KifFunction*)func);
	KifCallFunctionGeneral kfunc(NULL, NULL, name, (KifFunction*)func);
	kfunc.kifcode = kifcode;

	if (func->Size() == 3) {
		if (arg0 == NULL || arg0->Name() == -1) {
			string message = KIFLIBSTRINGS[195];
			message += kifGlobals->kifIdString[name];
			return kifcode->Returnerror(message);
		}
		vardom = new KifVariable(NULL, NULL, dom, arg0->Name());
		vardom->kifcode = kifcode;
		kfunc.parameters.push_back(vardom);
	}
	else
	if (func->Size() != 2) {
		string message = KIFLIBSTRINGS[195];
		message += kifGlobals->kifIdString[name];
		return kifcode->Returnerror(message);
	}


	kfunc.parameters.push_back(a);
	kfunc.parameters.push_back(b);
	if (dom != NULL)
		dom->reference++;
	a->reference++;
	b->reference++;
	KifElement* ret = kfunc.Exec(kifNULL, func, kifNULL, idthread, callfunc);
	if (vardom != NULL)
		delete vardom;
	if (dom != NULL)
		dom->reference--;
	a->reference--;
	b->reference--;
	return ret;
}

KifElement* KifDomain::applycomparatorfunction(KifElement* a, KifElement* b, KifFunction* func) {
	KifContainerLock _lock(this);
	short name = func->Name();
	if (func->Size() != 2) {
		string message = KIFLIBSTRINGS[195];
		message += kifGlobals->kifIdString[name];
		return kifcode->Returnerror(message);
	}

	threadblocinit;

	KifCallFunctionGeneral kfunc(NULL, NULL, func->Name(), func);
	kfunc.Setline(func->Line(), func->Fileindex());
	kfunc.kifcode = kifcode;

	a->reference++;
	b->reference++;

	kfunc.parameters.push_back(a);
	kfunc.parameters.push_back(b);

	KifElement* ret = kfunc.Exec(kifNULL, (KifDomain*)this, kifNULL, idthread, callfunc);
	a->reference--;
	b->reference--;
	return ret;
}

KifElement* KifDomain::Framedefined(short methodname, KifElement* contextualpattern, KifDomain* dom) {
	KifContainerLock _lock(this);
	KifElement* func = frame->Declared(methodname);
	if (func == NULL)
		return kifNULL;

	//KifCallFunctionLocal kfunc(kifcode, methodname, (KifFunction*)func);
	KifCallFunctionGeneral kfunc(NULL, NULL, methodname, (KifFunction*)func);
	kfunc.kifcode = kifcode;
	threadblocinit;
	return kfunc.Exec(contextualpattern, this, kifNULL, idthread, callfunc);
}
//--------------------------------------------------------------------------------------


//This function applies delayed _initial functions to frame elements...
void KifDomain::ApplyDelayedInitial(int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	//The difference between delayed and postpone is the following:
	//delayed means that frame elements belonging to the current frame need to be initialised
	//postpone means that the local _initial method for the current frame has not been called yet...
	delayed--;
	bin_hash<KifElement*>::iterator it;
	KifElement* xe;
	KifElement* ke;
	for (it = declarations.begin(); it != declarations.end(); it++) {
		//the initialisation method is attached to frame description of that element
		xe = ((KifDomain*)frame)->declarations[it->first];
		ke = it->second->Returnvalue();
		if (ke->Postpone()) {
			ke->constructor(this, xe->Initialisation(), idthread, callfunc);
			//Do we still have elements to initialize?
			if (ke->Delayed())
				ke->ApplyDelayedInitial(idthread, callfunc);
		}
	}
}


KifElement* KifDomain::Copy(KifDomain* dom, KifElement* decl) {
	KifContainerLock _lock(this);

	bin_hash<KifElement*>::iterator it;
	KifDomain* kFrameInstanciation = new KifDomain(kifcode, dom, decl->Name(), kifDomain);
	kFrameInstanciation->frame = frame;

	KifElement* xe;

	for (it = declarations.begin(); it != declarations.end(); it++) {
		xe = it->second->Atom();
		kFrameInstanciation->Setdeclaration(it->first, xe, NULL);
	}

	return kFrameInstanciation;
}

KifElement* KifFrame::Copy(KifDomain* dom, KifElement* decl) {
	int idthread = kifGlobals->GetThreadid();
	return Create(dom, decl, idthread, (KifCallFunction*)kifGlobals->Topid(idthread));
}

KifElement* KifFrame::Create(KifDomain* dom, KifElement* decl, int idthread, KifCallFunction* callfunc) {
	KifDomain* kFrameInstanciation = new KifDomain(kifcode, dom, decl->Name(), kifDomain);
	kFrameInstanciation->frame = this;

	KifElement* func;
	if (decl != kifNULL) {
		func = decl->Returnfunction();
		if (func == NULL)
			func = kFrameInstanciation->frame->Returnfunction();
	}
	else
		func = dom->Returnfunction();

	kFrameInstanciation->Disableadding();
	bool removefunction;
	KifElement* ke;
	KifElement* xe;

	for (int i = 0; i < ordered.size(); i++) {
		xe = declarations[ordered[i]];
		if (xe->isFunction())
			continue;

		removefunction = false;
		if (xe->Returnfunction() == NULL && func != NULL) {
			removefunction = true;
			xe->Setfunction(func);
		}

		//If we are dealing with a frame element, then
		//we will delay the call to the associate _initial function
		//xe should have the postpone flag set to true
		if (xe->Postpone())
			//kFrameInstanciation is our new Domain element. A Domain element is an instanciation of a particular frame declaration
			//This element receives a delayed flag, which is general to all frames within this frame.
			kFrameInstanciation->Setdelayed();

		if (xe->Linkedvalue() == this) {
			ke = new KifSelfFrame(kifcode, NULL, xe);
			kFrameInstanciation->Setdeclaration(ordered[i], ke, NULL);
		}
		else
			ke = xe->Create(kFrameInstanciation, xe, idthread, callfunc);

		//We keep a track of the variable frame
		ke->Setframe(kFrameInstanciation);
		if (removefunction == true)
			xe->Setfunction(NULL);
	}

	return kFrameInstanciation;
}

KifElement* KifDomain::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);

	if (frame->Declared(idvector) != NULL)
		return Framedefined(idvector, kifNULL, this);

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();
	bin_hash<KifElement*>::iterator it;
	KifVariable* var;
	KifElement* katom;
	KifElement* x;

	for (it = declarations.begin(); it != declarations.end(); it++) {
		x = it->second;
		if (x->type == kifVariable) {
			var = (KifVariable*)x;
			if (var->value->type == kifDomain)
				katom = var->value->Vector(this);
			else {
				katom = var->Atom();
				if (var->value == katom)
					katom = var->value;
			}
			kvect->Push(katom);
		}
	}
	return kvect;
}

KifElement* KifDomain::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	if (frame->Declared(idmap) != NULL)
		return Framedefined(idmap, kifNULL, this);

	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = new KifMap(kifcode, NULL);
	else
		kmap->Clear();

	bin_hash<KifElement*>::iterator it;
	KifElement* x;
	KifVariable* var;
	KifElement* katom;

	for (it = declarations.begin(); it != declarations.end(); it++) {
		x = it->second;
		if (x->type == kifVariable) {
			var = (KifVariable*)x;
			if (var->value->type == kifDomain)
				katom = var->value->Map(this);
			else
				katom = var->Atom();

			string skey = kifGlobals->kifIdString[it->first];
			kmap->Push(skey, katom);
		}
	}
	return kmap;
}

//--------------------------------------------------------------------------------------


bool KifDerivation(KifElement* frame, KifElement* derive, bool typetest) {
	if (frame->type != kifFrame || derive->type != kifFrame)
		return typetest;

	KifFrame* derivation = (KifFrame*)derive;
	while (derivation != NULL) {
		if (frame == derivation)
			return true;
		derivation = derivation->parent;
	}
	return false;
}

KifElement* KifDomain::applyoperation(short na, KifElement* a, KifElement* b) {
	KifContainerLock _lock(this);
	KifElement* func = frame->Declared(na);
	if (func == NULL)
		return kifNULL;

	if (a->type != kifDomain || KifDerivation(frame, a->Frame(), false) == false) {
		a = ((KifDomain*)a)->Framedefined(frame->Name(), this, (KifDomain*)a);

		if (a == kifNULL || frame != a->Frame())
			return kifcode->Returnerror(KIFLIBSTRINGS[242]);
	}

	if (b->type != kifDomain || KifDerivation(frame, b->Frame(), false) == false) {
		b = ((KifDomain*)b)->Framedefined(frame->Name(), this, (KifDomain*)b);

		if (b == kifNULL || frame != b->Frame())
			return kifcode->Returnerror(KIFLIBSTRINGS[242]);
	}

	//If such a function has been defined, we use it...
	//KifCallFunctionLocal kfunc(kifcode, na, (KifFunction*)func);
	KifCallFunctionGeneral kfunc(NULL, NULL, na, (KifFunction*)func);
	kfunc.kifcode = kifcode;
	kfunc.parameters.push_back(a);
	kfunc.parameters.push_back(b);
	threadblocinit;
	return kfunc.Exec(kifNULL, this, kifNULL, idthread, callfunc);
}


KifElement* KifDomain::applycomparison(short na, KifElement* a) {
	KifContainerLock _lock(this);
	KifElement* func = frame->Declared(na);
	if (func == NULL)
		return kifFALSE;

	if (a->type != kifDomain || KifDerivation(frame, a->Frame(), false) == false) {
		a = ((KifDomain*)a)->Framedefined(frame->Name(), this, (KifDomain*)a);

		if (a == kifNULL || frame != a->Frame())
			return kifcode->Returnerror(KIFLIBSTRINGS[242]);
	}


	//If such a function has been defined, we use it...
	//KifCallFunctionLocal kfunc(kifcode, na, (KifFunction*)func);
	KifCallFunctionGeneral kfunc(NULL, NULL, na, (KifFunction*)func);
	kfunc.kifcode = kifcode;
	//We add our parameters
	kfunc.parameters.push_back(a);
	threadblocinit;
	return kfunc.Exec(kifNULL, this, kifNULL, idthread, callfunc);
}

KifElement* KifDomain::applyincrement(short na) {
	KifElement* func = frame->Declared(na);
	if (func == NULL)
		return kifFALSE;

	KifContainerLock _lock(this);
	//If such a function has been defined, we use it...
	//KifCallFunctionLocal kfunc(kifcode, na, (KifFunction*)func);
	KifCallFunctionGeneral kfunc(NULL, NULL, na, (KifFunction*)func);
	kfunc.kifcode = kifcode;
	threadblocinit;
	return kfunc.Exec(kifNULL, this, kifNULL, idthread, callfunc);
}

KifElement* KifFile::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	if (EndOfFile() == kifTRUE)
		return kifFALSE;
	agnostring s(a->String());
	if (s == "") {
		if (!idx)
			return kifFALSE;
		return kifNULL;
	}
	KifElement* e = Find(contextualpattern, s, false);
	if (e == kifMINUSONE || e->Size() == 0) {
		if (!idx)
			return kifFALSE;
		return kifNULL;
	}
	return e;
}

KifElement* KifFile::whilein(KifElement* var) {
#ifdef PROFILING
	profilingcall("File::in");
#endif
	if (EndOfFile() == kifTRUE)
		return kifFALSE;
	switch (localtype) {
	case kifByte:
		c = Get();
		((KifByte*)local)->value = c;
		break;
	case kifInteger:
		c = Get();
		((KifInteger*)local)->value = c;
		break;
	case kifFloat:
		c = Get();
		local->inValue(c);
		break;
	case kifString:
		fbuffer[0] = 0;
		fgets((char*)fbuffer, 4095, fileread);
		if (first) {
			if (signature) {
				if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
					memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
			}
			first = false;
		}
		((KifString*)local)->value = (char*)fbuffer;
		break;
	case kifUString:
		fbuffer[0] = 0;
		fgets((char*)fbuffer, 4095, fileread);
		if (first) {
			if (signature) {
				if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
					memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
			}
			first = false;
		}
		sc_utf8_to_unicode(((KifUString*)local)->value, (char*)fbuffer);
		break;
	default:
		if (var->type == kifVectorInteger) {
			c = (uchar)thefile->get();
			var->storevalue((long)c);
		}
		else {
			fbuffer[0] = 0;
			fgets((char*)fbuffer, 4095, fileread);
			if (first) {
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
						memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
				}
				first = false;
			}
			localstring->value = (char*)fbuffer;
			if (var->type == kifVectorString || var->type == kifVectorUString)
				var->storevalue(localstring->value);
			else
			if (var->aVectorContainer())
				var->Push(localstring);
			else
				var->Setvalue(localstring);
		}
	}
	return kifTRUE;
}

KifElement* KifFileWide::in(KifElement* var, KifElement* a, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	if (EndOfFile() == kifTRUE)
		return kifFALSE;
	agnostring s(a->String());
	if (s == "") {
		if (!idx)
			return kifFALSE;
		return kifNULL;
	}
	KifElement* e = Find(contextualpattern, s, false);
	if (e == kifMINUSONE || e->Size() == 0) {
		if (!idx)
			return kifFALSE;
		return kifNULL;
	}
	return e;
}

KifElement* KifFileWide::whilein(KifElement* var) {
#ifdef PROFILING
	profilingcall("File::in");
#endif
	if (EndOfFile() == kifTRUE)
		return kifFALSE;
	switch (localtype) {
	case kifByte:
		Get();
		((KifByte*)local)->value = c;
		break;
	case kifInteger:
		Get();
		((KifInteger*)local)->value = c;
		break;
	case kifFloat:
		Get();
		local->inValue(c);
		break;
	case kifString:
		Readoneline();
		((KifString*)local)->value = localstring->String();
		break;
	case kifUString:
		Readoneline();
		((KifUString*)local)->value = localstring->value;
		break;
	default:
		if (var->type == kifVectorInteger) {
			Get();
			var->storevalue((long)c);
		}
		else {
			Readoneline();
			if (var->type == kifVectorString || var->type == kifVectorUString)
				var->storevalue(localstring->value);
			else
			if (var->aVectorContainer())
				var->Push(localstring);
			else
				var->Setvalue(localstring);
		}
	}
	return kifTRUE;
}

KifElement* KifStdin::in(KifElement* var, KifElement* a, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	return kifFALSE;
}

KifElement* KifStdin::whilein(KifElement* var) {
#ifdef PROFILING
	profilingcall("File::in");
#endif
	if (EndOfFile() == kifTRUE)
		return kifFALSE;
	switch (localtype) {
	case kifByte:
		c = (uchar)is->get();
		((KifByte*)local)->value = c;
		break;
	case kifInteger:
		c = (uchar)is->get();
		((KifInteger*)local)->value = c;
		break;
	case kifFloat:
		c = (uchar)is->get();
		local->inValue(c);
		break;
	case kifUString:
	case kifString:
		getline(*is, ((KifString*)local)->value, '\n');
		break;
	default:
		if (var->type == kifVectorInteger) {
			c = (uchar)is->get();
			var->storevalue((long)c);
		}
		else {
			getline(*is, localstring->value, '\n');
			if (var->type == kifVectorString || var->type == kifVectorUString)
				var->storevalue(localstring->value);
			else
			if (var->aVectorContainer())
				var->Push(localstring);
			else
				var->Setvalue(localstring);
		}
	}
	return kifTRUE;
}

KifElement* KifDomain::in(KifElement* a, KifElement* b, KifDomain* kexfunc, KifElement* contextualpattern, bool idx) {
	if (kexfunc != NULL)
		return applyfunc(a, b, kexfunc);


	KifElement* func = frame->Declared(idin);
	//If such a function has been defined, we use it...
	if (func != NULL) {
		//KifCallFunctionLocal kfunc(kifcode, idin, (KifFunction*)func);
		KifCallFunctionGeneral kfunc(NULL, NULL, idin, (KifFunction*)func);
		kfunc.kifcode = kifcode;
		//We add our parameters
		kfunc.parameters.push_back(a);
		threadblocinit;
		return kfunc.Exec(kifNULL, this, kifNULL, idthread, callfunc);
	}
	return kifNOELEMENT;
}


KifElement* KifString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	string svalue;
	{
		KifContainerLock _lock(this);
		svalue = value;
	}
	if (kfunc == NULL) {
		string s1 = a->String();
		if (contextualpattern->type == kifBoolean && idx == false && b == this) {
			if (strstr(STR(svalue), STR(s1)) != NULL)
				return kifTRUE;
			return kifFALSE;
		}

		if (!b->isString())
			return b->in(a, b, kfunc, contextualpattern, true);


		string s2 = b->String();

		if (contextualpattern->aContainer()) {
			if (idx == false) {
				if (s1 == s2)
					return kifTRUE;
				return kifFALSE;
			}
		}


		int rs;
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* kvect = Selectvectorinteger(this, contextualpattern);
			vector<long> v;
			s_findall(s2, s1, v);
			int ps = 0, nb = 0;

			int i;
			for (i = 0; i < v.size(); i++)
				v[i] = c_bytetocharpositionidx(USTR(s2), v[i], nb, ps);

			KifContainerLock _lock((KifEvaluate*)kvect);

			switch (kvect->type) {
			case kifVectorByte:
				for (i = 0; i < v.size(); i++)
					((KifVectorByte*)kvect)->values.push_back((uchar)v[i]);
				return kvect;
			case kifVectorInteger:
				((KifVectorInteger*)kvect)->values = v;
				return kvect;
			case kifVectorFloat:
				for (i = 0; i < v.size(); i++)
					((KifVectorFloat*)kvect)->values.push_back((double)v[i]);
				return kvect;
			}

			KifBaseInteger locint(NULL, NULL);
			locint.kifcode = kifcode;
			for (i = 0; i < v.size(); i++) {
				locint.value = v[i];
				kvect->Push(&locint);
			}

			return kvect;
		}

		rs = s_find(s2, s1, 0);

		if (rs != -1) {
			if (contextualpattern->type == kifString)
				return kifcode->Providestring(s1);

			if (idx == false)
				return kifTRUE;

			return kifcode->Provideinteger(rs);
		}

		if (contextualpattern->type == kifString)
			return kifNOELEMENT;

		if (idx == false)
			return kifFALSE;
		if (contextualpattern->aContainer())
			return kifNOELEMENT;
		return kifMINUSONE;
	}
	else
		return applyfunc(a, b, kfunc);
}
//--------------------------------------------------------------------------------------
KifElement* KifUString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	wstring svalue;
	{
		KifContainerLock _lock(this);
		svalue = value;
	}
	if (kfunc == NULL) {
		wstring s1 = a->UString();
		if (contextualpattern->type == kifBoolean && idx == false && b == this) {
			if (wcsstr(WSTR(svalue), WSTR(s1)) != NULL)
				return kifTRUE;
			return kifFALSE;
		}

		if (!b->isString())
			return b->in(a, b, kfunc, contextualpattern, true);


		wstring s2 = b->UString();

		if (contextualpattern->aContainer()) {
			if (idx == false) {
				if (s1 == s2)
					return kifTRUE;
				return kifFALSE;
			}
		}

		int rs;
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* kvect = Selectvectorinteger(this, contextualpattern);
			vector<long> v;
			s_findall(s2, s1, v);
			int ps = 0, nb = 0;
			int i;

			KifContainerLock _lock((KifEvaluate*)kvect);
			switch (kvect->type) {
			case kifVectorByte:
				for (i = 0; i < v.size(); i++)
					((KifVectorByte*)kvect)->values.push_back((uchar)v[i]);
				return kvect;
			case kifVectorInteger:
				((KifVectorInteger*)kvect)->values = v;
				return kvect;
			case kifVectorFloat:
				for (i = 0; i < v.size(); i++)
					((KifVectorFloat*)kvect)->values.push_back((double)v[i]);
				return kvect;
			}

			KifBaseInteger locint(NULL, NULL);
			locint.kifcode = kifcode;
			for (i = 0; i < v.size(); i++) {
				locint.value = v[i];
				kvect->Push(&locint);
			}

			return kvect;
		}

		rs = s_find(s2, s1, 0);

		if (rs != -1) {
			if (contextualpattern->type == kifString)
				return kifcode->Provideustring(s1);

			if (idx == false)
				return kifTRUE;

			return kifcode->Provideinteger(rs);
		}

		if (contextualpattern->type == kifString)
			return kifNOELEMENT;

		if (idx == false)
			return kifFALSE;
		if (contextualpattern->aContainer())
			return kifNOELEMENT;
		return kifMINUSONE;
	}
	else
		return applyfunc(a, b, kfunc);
}

//--------------------------------------------------------------------------------------

Exported void KifDomain::SetInstanceslocal(KifElement* dom, bin_hash<KifElement*>* stacks) {
	KifContainerLock _lock(this);
	bin_hash<KifElement*>::iterator it;
	dom->Disableadding();
	KifElement* xe;
	for (it = declarations.begin(); it != declarations.end(); it++) {
		xe = it->second;
		if (xe->isFunction() || xe->reference >= KIF_PROTECT_REFERENCE || xe->idgarbage <= 0)
			continue;
		xe = xe->Copy(this, xe);
		dom->Setdeclaration(it->first, xe, stacks);
	}
}


void KifDomain::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror(KIFKIFSTRINGS[14]);
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;

	KifContainerLock _lock(this);
	reference += inc;
	Putpopped(0);
	bin_hash<KifElement*>::iterator it;
	for (it = declarations.begin(); it != declarations.end(); it++) {
		if (it->second != NULL)
			it->second->Setreference(inc);
	}
	idgarbage = id;
}

void KifDomain::Setpopped(int v, bool force) {
	KifContainerLock _lock(this);
	if (reference <= 0 || force) {
		Putpopped(v);
		bin_hash<KifElement*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++) {
			if (it->second != NULL)
				it->second->Setpopped(v, force);
		}
	}
}


Exported void KifIterator::Resetreference(int inc) {
	if (value != NULL && value->type == kifIteration) {
		if (popped)
			value->Putpopped(1);
		value->Resetreference(inc);
	}
	KifElement::Resetreference(inc);
}

void KifDomain::Clean() {
	{
		KifContainerLock _lock(this);
		bin_hash<KifElement*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++) {
			if (it->second != NULL)
				it->second->Clean();
		}
		declarations.clear();
	}
	KifElement::Resetreference();
}

void KifDomain::Clear() {
	KifContainerLock _lock(this);
	bin_hash<KifElement*>::iterator it;
	for (it = declarations.begin(); it != declarations.end(); it++) {
		if (it->second != NULL)
			it->second->Resetreference(reference);
	}
	declarations.clear();
}

Exported void KifElement::Resetreference(int inc) {
	reference -= inc;
	if (reference <= 0) {
		if (popped == 1)
			popped = 0;
		else {
			if (idgarbage != -1) {
				if (kifGlobals->Test()) {
					ThreadLock _lock(kifGarbage);
					kifGlobals->garbage.vecteur[idgarbage] = NULL;
					kifGlobals->empties.push_back(idgarbage);
				}
				else {
					kifGlobals->garbage.vecteur[idgarbage] = NULL;
					kifGlobals->empties.push_back(idgarbage);
				}
			}
			delete this;
		}
	}
}

void KifElement::Forcecleanreference() {
	reference--;
	if (reference <= 0) {
		if (popped == 1)
			popped = 0;
		else {
			if (idgarbage != -1) {
				if (kifGlobals->Test()) {
					ThreadLock _lock(kifGarbage);
					kifGlobals->garbage.vecteur[idgarbage] = NULL;
					kifGlobals->empties.push_back(idgarbage);
				}
				else {
					kifGlobals->garbage.vecteur[idgarbage] = NULL;
					kifGlobals->empties.push_back(idgarbage);
				}
			}
			delete this;
		}
	}
}

void KifDomain::Resetreference(int inc) {
	{
		KifContainerLock _lock(this);
		reference -= inc;
		if (reference <= 0 && !popped)
			destructor();

		bin_hash<KifElement*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++) {
			if (it->second != NULL) {
				if (popped)
					it->second->Putpopped(1);
				it->second->Resetreference(inc);
			}
		}
		if (reference <= 0 && !popped)
			declarations.clear();
	}
	KifElement::Resetreference(0);
}


Exported void KifDomain::GetInstances(KifDomain* dom) {
	bin_hash<KifElement*>::iterator it;
	Clear();
	Disableadding();
	KifContainerLock _lock(this);
	for (it = dom->declarations.begin(); it != dom->declarations.end(); it++) {
		it->second->Setreference();
		declarations[it->first] = it->second;
	}
	dom->declarations.clear();
}



bool KifDomain::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (this == ke)
		return true;

	if (frame == kifNULL || ke == kifNULL) {
		kifcode->Returnerror(KIFLIBSTRINGS[155]);
		return true;
	}

	KifContainerLock _lock(this);

	KifElement* func = frame->Declared(idindex);
	if (idx != NULL && idx != kifNULL && func != NULL) {
		threadblocinit;
		//We set a value through an index...
		KifElement* left = idx;
		if (idx->type == kifIndex) {
			KifIndex* kind = (KifIndex*)idx;
			left = kind->key->Exec(kifNULL, this, kifNULL, idthread, callfunc);
			if (kind->interval == true) {
				left->Release();
				kifcode->Returnerror(KIFLIBSTRINGS[155]);
				return true;
			}
		}

		//If such a function has been defined, we use it...
		//KifCallFunctionLocal kfunc(kifcode, idindex, (KifFunction*)func);
		KifCallFunctionGeneral kfunc(NULL, NULL, idindex, (KifFunction*)func);
		kfunc.kifcode = kifcode;
		//We add our parameters
		kfunc.parameters.push_back(left);
		kfunc.parameters.push_back(ke);
		return kfunc.Exec(kifNULL, this, kifNULL, idthread, callfunc);
	}

	//We rebuild an object from a map...
	if (ke->type == kifMap) {
		//each key of the map should be a variable in our frame
		bin_hash<KifElement*> keys;
		hmap<string, KifElement*>::iterator it;
		short id;
		KifMap* kmap = (KifMap*)ke;
		for (it = kmap->values.begin(); it != kmap->values.end(); it++) {
			if (kifGlobals->kifStringId.find(it->first) == kifGlobals->kifStringId.end()) {
				kifcode->Returnerror(KIFLIBSTRINGS[155]);
				return true;
			}
			id = kifGlobals->kifStringId[it->first];
			if (!declarations.check(id)) {
				kifcode->Returnerror(KIFLIBSTRINGS[155]);
				return true;
			}
			keys[id] = it->second;
		}

		bin_hash<KifElement*>::iterator itk;
		for (itk = keys.begin(); itk != keys.end(); itk++)
			declarations[itk->first]->Setvalue(itk->second);
		return true;
	}

	if (ke->type != kifDomain) {
		kifcode->Returnerror(KIFLIBSTRINGS[155]);
		return true;
	}


	if (KifDerivation(frame, ke->Frame(), false) == false) {
		//Maybe we have a translation from one frame into another
		ke = ((KifDomain*)ke)->Framedefined(frame->Name(), this, (KifDomain*)ke);

		if (ke == kifNULL || frame != ke->Frame()) {
			kifcode->Returnerror(KIFLIBSTRINGS[155]);
			return true;
		}
		//If it is an instance, which has been created into our cast function
		if (ke->reference <= 0) {
			//we simple vampirize it.
			GetInstances((KifDomain*)ke);
			return true;
		}
	}

	Clear();
	ke->SetInstances(this, NULL);
	return true;
}

bool KifCall::Setvalue(KifElement* val, KifElement* idx, KifElement* dom) {
	ThreadLock _lock(type);
	if (val == kifNULL) {
		body = NULL;
		domain = NULL;
		return true;
	}

	if (val->type == kifCall) {
		body = ((KifCall*)val)->body;
		domain = ((KifCall*)val)->domain;
		return true;
	}

	if (val->type != kifFunction) {
		int i = kifGlobals->KifStringId(val->String());
		if (i == -1)
			kifcode->Returnerror(KIFLIBSTRINGS[156]);
		val = kifcode->kstart->Declaration(i);
		if (val == NULL || val->type != kifFunction) {
			kifcode->Returnerror(KIFLIBSTRINGS[156]);
			return true;
		}
	}
	body = (KifFunction*)val;
	domain = (KifDomain*)dom;
	return true;
}

//===================================================================
Exported string KifFunction::String() {
	return kifGlobals->kifIdString[name];
}

Exported string KifCall::String() {
	ThreadLock _lock(type);
	if (body != NULL)
		return body->String();
	return "";
}

Exported string KifCall::BaseString() {
	ThreadLock _lock(type);
	if (body != NULL)
		return body->BaseString();
	return "";
}

string KifInteger::String() {
	char buffer[50];
	sprintf_s(buffer, 50, "%ld", value);
	return buffer;
}

string KifByte::String() {
	string s(" ");
	s[0] = value;
	return s;
}

string KifLong::String() {
	char buffer[50];
	sprintf_s(buffer, 50, "%lld", value);
	return buffer;
}

string KifShort::String() {
	char buffer[10];
	sprintf_s(buffer, 10, "%d", value);
	return buffer;
}

string KifFraction::String() {
	char buffer[100];
	sprintf_s(buffer, 100, "%lld/%lld", numerator, denominator);
	return buffer;
}

string KifFloat::String() {
	char buffer[50];
	sprintf_s(buffer, 50, "%g", value);
	return buffer;
}

string KifDate::String() {
	char buffer[100];
	struct tm* temps = localtime(&value);
	strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", temps);
	return buffer;
}

string KifTime::String() {
	char buffer[100];
	double timeval = Float() / 1000000UL;
	sprintf_s(buffer, 100, "%fs", timeval);
	return buffer;
}

//===================================================================

void KifFrame::Run(int from) {
	size_t i;

	kifGlobals->executionbreak = 0;
	int maxid = instructions.size();
	int idthread = kifGlobals->GetThreadid();
	KifInstructionStack _stacking(idthread);
	bool debugging;
	KifDebugInfo* debugptr = DebugPtrInit(debugging);

	//We look for the first instruction to execute
	KifElement* ke;
	for (i = from; i < maxid; i++) {
		ke = instructions[i];
		if (ke->Variable() || EXECUTABLE(ke->type)) {
			_stacking.Modify(ke);
			if (debugging)
				debugging = debugptr->Debugging(kifcode, this, ke, this, NULL);
			ke = ke->Exec(kifNULL, this, kifNULL, idthread, NULL);
			ke->Release();
			if (Errorid(idthread) == true)
				break;
		}
	}

	//In the case of lumbering threads, we need to terminate them...
	kifGlobals->CleanTriggerTables();
}

KifElement* KifFrame::RunThread(int from) {
	if (kifcode->function == NULL) {
		size_t i;
		int maxid = instructions.size();
		kifcode->function = new KifFunctionSequence(kifcode, idstart);
		for (i = from; i < maxid; i++) {
			KifElement* ke = instructions[i];
			if (ke->Variable() || EXECUTABLE(ke->type))
				kifcode->function->instructions.push_back(ke);
		}
	}

	KifCallFunctionThread* kcf = new KifCallFunctionThread(NULL, NULL, id_run_as_thread, kifcode->function);
	kcf->kifcode = kifcode;
	kcf->reference = 1000;
	kcf->Exec(kifNULL, this, kifNULL, 0, NULL);
	return kcf;
}

//===================================================================
//DWORD WINAPI KifCallExecuteThread::Call(LPVOID arg) { we use _beginthread now... no need for this type of call

#ifdef C11
#ifdef WIN32
void __stdcall Call(KifCallExecuteThread* kfuncthread) {
#else
void Call(KifCallExecuteThread* kfuncthread) {
#endif
#else
#ifdef WIN32
unsigned __stdcall KifCallExecuteThread::Call(void* arg) {
	set_terminate(termination_function);
	KifCallExecuteThread* kfuncthread = (KifCallExecuteThread*)arg;
#else
void* KifCallExecuteThread::Call(void* arg) {
	KifCallExecuteThread* kfuncthread = (KifCallExecuteThread*)arg;
	//actually the id that is computed by pthread_create is not always the same as pthread_self
	kfuncthread->tid = pthread_self();
#endif
#endif

#ifdef C11
	{
		//we need to synchronize with the affectation of thid when the thread is created...
		//otherwise, we might go through without a proper value for it...
		ThreadLock _lstr(kifStack);
		kfuncthread->tid = std::this_thread::get_id();
	}
#endif

	if (kfuncthread->body->Joined()) {
		ThreadLock _lock(kifCallFunctionThread);
		kfuncthread->joinpos = kifGlobals->THR[kfuncthread->parentthreadid]->joinedloquets.size();
		if (kfuncthread->joinpos >= kifGlobals->maxjoined) {
			kfuncthread->kifcode->Returnerror(KIFKIFSTRINGS[15]);
			kfuncthread->joinpos = -1;
		}
		else
#if defined(C11) || defined(WIN32)
			kifGlobals->THR[kfuncthread->parentthreadid]->joinedloquets.push_back(kfuncthread->thid);
#else
			kifGlobals->THR[kfuncthread->parentthreadid]->joinedloquets.push_back(kfuncthread->tid);
#endif
	}

	kifGlobals->SetThreadid(kfuncthread->tid, kfuncthread->threadid);

	kfuncthread->Launch();
	bool reclaim = true;
	if (kfuncthread->body->Joined()) {
		ThreadLock _lock(kifCallFunctionThread);
		kifGlobals->THR[kfuncthread->parentthreadid]->joinedloquets[kfuncthread->joinpos] = NULL;
		reclaim = false;
	}

	//We need to clean the pending locks if any
	{
		ThreadLock _lock(kifLock);
		//We clean the locks as well
		hmap<string, KifThreadLock*>::iterator itlock;
		KifThreadLock* ktlt;
		for (itlock = kifGlobals->locktables.begin(); itlock != kifGlobals->locktables.end(); itlock++) {
			ktlt = itlock->second;
			if (ktlt->idthread == kfuncthread->threadid)
				ktlt->Unlocking();
		}
	}

	if (kfuncthread->name == id_run_as_thread) {
		kifGlobals->CleanTriggerTables();
		kifGlobals->endofexecution = true;
		if (kifGlobals->terminationfunction != NULL)
			(*kifGlobals->terminationfunction)();
	}

#ifdef C11
	kifGlobals->THR[kfuncthread->threadid]->localthread = kfuncthread->thid;
	delete kfuncthread;
#else
#ifdef WIN32	
	CloseHandle(kfuncthread->thid);
	delete kfuncthread;
	_endthreadex(0);
	return 0;
#else
	pthread_t tid = kfuncthread->tid;
	delete kfuncthread;
	//We need to detach it, in order to reclaim its space, once it has terminated...
#ifdef APPLE
	//In the case of APPLE, with a potential pthread_join, we do not detach it, otherwise it crashes
	if (reclaim)
		pthread_detach(tid);
#else
	pthread_detach(tid);
#endif
	pthread_exit(NULL);
	return 0;
#endif
#endif
}

Exported void KifCallExecuteThread::Launch() {
	//We use Execution, since we already know the arguments    
	Execution();

	//We can now clean our variables...
	kifGlobals->EraseThreadid(tid);
	ThreadLock _lock(kifStack);
	kifGlobals->THR[threadid]->clear(true);
	if (domain != kifcode->kstart)
		domain->Resetreference();
	Resetreference();
}

//This method is called from within a KifCallExecuteThread Launch or Execute...
Exported KifElement* KifThread::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* value, int idthread, KifCallFunction* callfunc) {

	//If it is a protected thread, then we set the lock in
	if (klock != NULL)
		klock->Locking();
	else
		dom->Lockframe(exclusive);
	KifElement* ret;
	KifTable* table = NULL;

	//We need to check if we in a waiton configuration
	if (kifGlobals->triggertable.find(name) != kifGlobals->triggertable.end()) {
		{
			ThreadLock _lock(kifTable);
			table = kifGlobals->triggertable[name];
		}

		table->lock->Blocked();
		if (Errorid(idthread)) {
			{
				ThreadLock _lock(kifTable);
				kifGlobals->triggerfunctions[table->idgarbage].erase(name);
			}
			if (klock != NULL)
				klock->Unlocking();

			else
				dom->Unlockframe(exclusive);
			return kifNULL;
		}
	}

	ret = KifFunction::Exec(contextualpattern, dom, value, idthread, callfunc);

	if (table != NULL) {
		ThreadLock _lock(kifTable);
		kifGlobals->triggerfunctions[table->idgarbage].erase(name);
	}

	if (klock != NULL)
		klock->Unlocking();

	else
		dom->Unlockframe(exclusive);

	return ret;
}

void KifCallFunction::Initialise(KifCallFunction* kt) {
	Setselfinsert(kt->Selfinsert());
	line = kt->line;
	adding = kt->adding;
	function = kt->function;
	declarations = kt->declarations;;
	parameters = kt->parameters;
}

Exported KifElement* KifCallReturn::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (parameters.size() != 1)
		return kifGlobals->THR[idthread]->Setreturnvalue(kifNULL);

	KifElement* res = parameters[0]->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	//For streaming only
	if (callfunc != NULL)
		callfunc->Setreturnvalue(res);
	return kifGlobals->THR[idthread]->Setreturnvalue(res);
}

KifElement* KifCallFunctionThread::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
#ifdef UNIX
	static bool init = false;
	static size_t stacksz = 0;
	static pthread_attr_t attr;
	if (init == false) {
		pthread_attr_init(&attr);
		pthread_attr_getstacksize(&attr, &stacksz);
		init = true;
	}
#endif
	//First we initialise our thread with the current element
	kifGlobals->Threadmode();

	ThreadLock* _lockstack = new ThreadLock(kifStack);

	int localthreadid = kifGlobals->Setthreadid(idthread);
	if (localthreadid == -1) {
		delete _lockstack;
		return kifcode->Returnerror(KIFKIFSTRINGS[16]);
	}
	KifCallExecuteThread* kifcall = new KifCallExecuteThread(NULL, this, body, -1);
	if (val != NULL && val->Action() == kifSTREAM) {
		kifcall->streamvalue = contextualpattern;
		kifcall->istreamvalue = contextualpattern->Ingarbage();
	}

	kifcall->threadid = localthreadid;
	kifcall->parentthreadid = idthread;
	kifcall->kifcode = kifcode;
	kifcall->Setreference();
	kifcall->context = contextualpattern;

	if (dom != kifcode->kstart) {
		kifcall->domain = (KifDomain*)dom->Newinstance();
		kifcall->domain->declarations = dom->declarations;
		if (kifcall->domain->declarations.check(PREDICATEDICO))
			kifcall->domain->declarations.erase(PREDICATEDICO);
		kifcall->domain->Setreference();
	}
	else
		kifcall->domain = dom;
	kifcall->value = val;
	delete _lockstack;

	//We initialise our function with the right parameters, before they are erased
	kifcall->Arguments(callfunc);

	if (Errorid(idthread) == true || dom->Failed())
		return Errorptrid(idthread);

	//Then we launch our function
	_lockstack = new ThreadLock(kifStack);

#ifdef C11
	kifcall->thid = new std::thread(Call, kifcall);
	if (kifcall->thid == NULL)
		return kifcode->Returnerror(KIFLIBSTRINGS[211]);
#else
#ifdef WIN32
	//kifcall->thid=CreateThread(NULL,0,&KifCallExecuteThread::Call,(LPVOID)kifcall,0,&kifcall->tid);
	kifcall->thid = (HANDLE)_beginthreadex(NULL, 0, KifCallExecuteThread::Call, (void*)kifcall, 0, (UINT*)&kifcall->tid);
	if (kifcall->thid == NULL)
		return kifcode->Returnerror(KIFLIBSTRINGS[211]);
#else
	pthread_attr_setstacksize(&attr, stacksz << 2);
	int rc = pthread_create(&kifcall->tid, &attr, &KifCallExecuteThread::Call, (void*)kifcall);
	if (rc != 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[211]);
#endif
#endif
	delete _lockstack;

	kifGlobals->Loopthreadid(kifcall->threadid);

	return kifcode->Provideinteger(kifcall->threadid);
}

KifElement* KifThis::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*) {
	if (dom->type != kifDomain) {
		dom = kifGlobals->Beforetop();
		if (dom == NULL)
			return kifcode->Returnerror(KIFLIBSTRINGS[235]);
	}

	return dom;
}

KifElement* KifIndex::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc) {
	if (function == NULL && ke->Returnfunction() == NULL)
		return ke->Exec(contextualpattern, dom, this, idthread, callfunc);

	KifCouple couple(ke, this, dom, NULL);
	return couple.Exec(contextualpattern, dom, this, idthread, callfunc);
}

KifElement* KifCouple::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* kres, int idthread, KifCallFunction* callfunc) {
	kres = recipient->Exec(contextualpattern, domain, indexes, idthread, callfunc);

	if (indexes->function == NULL) {
		Release();
		if (returnvalue && kres == kifNOELEMENT) {
			if (kifGlobals->basevalues.find(recipient->type) == kifGlobals->basevalues.end())
				return kifNOELEMENT;
			return kifGlobals->basevalues[recipient->type]->Atom(true);
		}
		return kres;
	}

	KifIndex* kidx = indexes;
	KifElement* klocal;

	if (kidx->function->type == kifIndex) {
		//Double indexes...
		kidx = (KifIndex*)indexes->function;
		klocal = kres;
		while (kidx != NULL) {
			//In this case, what we try to do is first to get the element with this index
			klocal = klocal->Exec(kifNULL, domain, kidx, idthread, callfunc);
			if (klocal != kres) {
				kres->Release();
				kres = klocal;
			}
			if (kidx->function != NULL && kidx->function->type == kifIndex)
				kidx = (KifIndex*)kidx->function;
			else
				break;
		}
	}

	if (kidx->function != NULL) {
		if (kres->type == kifDomain)
			kres = kidx->function->Exec(contextualpattern, (KifDomain*)kres, kres, idthread, callfunc);
		else
			kres = kidx->function->Exec(contextualpattern, domain, kres, idthread, callfunc);
		Release();
		if (returnvalue && kres == kifNOELEMENT) {
			if (kifGlobals->basevalues.find(kidx->function->type) == kifGlobals->basevalues.end())
				return kifNOELEMENT;
			return kifGlobals->basevalues[kidx->function->type]->Atom(true);
		}
	}
	else
		Release();
	return kres;
}

Exported KifElement* KifDomain::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	if (idx == NULL || idx->isConst())
		return this;

	KifContainerLock _lock(this);

	KifElement* left = idx;
	KifElement* right = NULL;
	short ididx = idindex;

	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;
		left = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true) {
			ididx = idinterval;
			right = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		}
	}

	KifElement* func = frame->Declared(ididx);
	if (func == NULL) {
		left->Release();
		if (right != NULL)
			right->Release();
		return this;
	}

	//If such a function has been defined, we use it...
	//KifCallFunctionLocal kfunc(kifcode, ididx, (KifFunction*)func);
	KifCallFunctionGeneral kfunc(NULL, NULL, ididx, (KifFunction*)func);
	kfunc.kifcode = kifcode;
	//We add our parameters
	kfunc.parameters.push_back(left);
	if (right != NULL)
		kfunc.parameters.push_back(right);
	func = kfunc.Exec(kifNULL, this, kifNULL, idthread, callfunc);
	left->Release();
	if (right != NULL)
		right->Release();
	return func;
}

//-------------------------------------------------------------------------------------------------------
Exported KifElement* KifRawString::constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
	if (params->InstructionSize() == 0)
		return this;

	if (params->InstructionSize() != 1)
		return kifcode->Returnerror(KIFLIBSTRINGS[266]);


	params = params->Instruction(0)->Exec(kifTypeSTRING, dom, kifNULL, idthread, callfunc);
	if (params->isString()) {
		string v = params->String();
		stringsize = v.size();
		buffersize = stringsize + 1;
		value = new unsigned char[buffersize];
		strcpy_s((char*)value, buffersize, STR(v));
	}
	else {
		buffersize = params->Integer();
		value = new unsigned char[buffersize];
		memset(value, 0, buffersize);
	}
	params->Release();
	return this;
}

bool KifRawString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;


	if (value == NULL) {
		kifcode->Returnerror(KIFKIFSTRINGS[17]);
		return true;
	}

	string s;
	if (ke->type > kifnull)
		s = ke->String();

	if (s.size() == 0) {
		memset((char*)value, 0, buffersize);
		stringsize = 0;
		return true;
	}

	if (idx == NULL || idx->isConst()) {
		if (buffersize <= s.size()) {
			kifcode->Returnerror(KIFKIFSTRINGS[17]);
			return true;
		}
		strcpy((char*)value, STR(s));
		stringsize = s.size();
	}
	else {
		long ileft = -1, iright = -1;
		char res = RawStringIndexes((char*)value, stringsize, idx, (KifDomain*)dom, ileft, iright, -1, NULL);

		if (res == 0) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return true;
		}

		if (res == 1)
			iright = ileft + 1;

		//the size is:
		int computesize = stringsize - (iright - ileft) + s.size();
		if (computesize > buffersize) {
			kifcode->Returnerror(KIFKIFSTRINGS[17]);
			return true;
		}

		stringsize = computesize;
		s += (char*)value + iright;
		value[ileft] = 0;
		strcat((char*)value, STR(s));
		return true;
	}
	return true;
}

KifElement* KifRawString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	if (value == NULL) {
		if (idx == false)
			return kifFALSE;
		return kifMINUSONE;
	}

	if (kfunc == NULL) {
		string s1 = a->String();
		if (contextualpattern->type == kifBoolean && idx == false && b == this) {
			if (strstr((char*)value, STR(s1)) != NULL)
				return kifTRUE;
			return kifFALSE;
		}

		if (!b->isString())
			return b->in(a, b, kfunc, contextualpattern, true);


		string s2 = b->String();

		if (contextualpattern->aContainer()) {
			if (idx == false) {
				if (s1 == s2)
					return kifTRUE;
				return kifFALSE;
			}
		}

		int rs;
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* kvect = Selectvectorinteger(this, contextualpattern);
			vector<long> v;
			s_findall(s2, s1, v);
			int ps = 0, nb = 0;

			int i;

			KifContainerLock _lock((KifEvaluate*)kvect);

			switch (kvect->type) {
			case kifVectorByte:
				for (i = 0; i < v.size(); i++)
					((KifVectorByte*)kvect)->values.push_back((uchar)v[i]);
				return kvect;
			case kifVectorInteger:
				((KifVectorInteger*)kvect)->values = v;
				return kvect;
			case kifVectorFloat:
				for (i = 0; i < v.size(); i++)
					((KifVectorFloat*)kvect)->values.push_back((double)v[i]);
				return kvect;
			}

			KifBaseInteger locint(NULL, NULL);
			locint.kifcode = kifcode;
			for (i = 0; i < v.size(); i++) {
				locint.value = v[i];
				kvect->Push(&locint);
			}

			return kvect;
		}

		rs = s_findbyte(s2, s1, 0);

		if (rs != -1) {
			if (contextualpattern->type == kifString)
				return kifcode->Providestring(s1);

			if (idx == false)
				return kifTRUE;
			return kifcode->Provideinteger(rs);
		}

		if (contextualpattern->type == kifString)
			return kifNOELEMENT;

		if (idx == false)
			return kifFALSE;
		if (contextualpattern->aContainer())
			return kifNOELEMENT;
		return kifMINUSONE;
	}
	else
		return applyfunc(a, b, kfunc);
}

Exported void KifRawString::Clear() {
	if (value != NULL) {
		memset(value, 0, buffersize);
		stringsize = 0;
	}
}

Exported KifElement* KifRawString::Atom(bool forced) {
	if (reference == 0 && popped && !forced)
		return this;

	KifRawString* v = new KifRawString(kifcode, NULL);
	if (value != NULL) {
		v->value = new unsigned char[buffersize];
		v->buffersize = buffersize;
		v->stringsize = stringsize;
		strcpy((char*)v->value, (char*)value);
	}
	return v;
}

Exported void KifRawString::Push(KifElement* ke) {
	if (value == NULL) {
		kifcode->Returnerror(KIFKIFSTRINGS[17]);
		return;
	}

	string s = ke->String();
	if ((stringsize + s.size()) >= buffersize) {
		kifcode->Returnerror(KIFKIFSTRINGS[17]);
		return;
	}

	for (long i = 0; i < s.size(); i++)
		value[stringsize + i] = s[i];
	stringsize += s.size();
}

void KifRawString::storevalue(string s) {
	if (value == NULL) {
		kifcode->Returnerror(KIFKIFSTRINGS[17]);
		return;
	}

	if ((stringsize + s.size()) >= buffersize) {
		kifcode->Returnerror(KIFKIFSTRINGS[17]);
		return;
	}
	for (long i = 0; i < s.size(); i++)
		value[stringsize + i] = s[i];
	stringsize += s.size();
}

void KifRawString::storevalue(char* k) {
	if (value == NULL) {
		kifcode->Returnerror(KIFKIFSTRINGS[17]);
		return;
	}
	int ksz = strlen(k);
	if ((stringsize + ksz) >= buffersize) {
		kifcode->Returnerror(KIFKIFSTRINGS[17]);
		return;
	}
	for (long i = 0; i < ksz; i++)
		value[stringsize + i] = k[i];

	stringsize += ksz;
}

string KifRawString::getstring(int i) {
	if (value == NULL) {
		kifcode->Returnerror(KIFKIFSTRINGS[17]);
		return "";
	}

	if (i < 0 || i >= stringsize) {
		kifcode->Returnerror(KIFKIFSTRINGS[17]);
		return "";
	}

	char c[] = { (char)value[i], 0 };
	return c;
}

Exported KifElement* KifRawString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (value == NULL)
		return kifNULL;

	if (autoself) {
		string add = b->String();
		long sz = add.size();
		if ((stringsize + sz) >= buffersize)
			return kifcode->Returnerror(KIFKIFSTRINGS[17]);

		if (sz == 1)
			value[stringsize] = add[0];
		else {
			for (int i = 0; i < sz; i++)
				value[stringsize + i] = add[i];
		}
		stringsize += sz;
		return this;
	}

	string add = a->String() + b->String();
	return kifcode->Providestring(add);
}

Exported KifElement* KifRawString::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (value == NULL)
		return kifNULL;

	string add = b->String();
	int nb = add.size();

	if (autoself) {
		if (nb > stringsize)
			return this;

		char* pt = strstr((char*)value, STR(add));
		if (pt == NULL)
			return this;
		strcpy(pt, pt + nb);
		stringsize -= nb;
		return this;
	}

	string v = a->String();
	if (nb > stringsize)
		return kifcode->Providestring(v);

	int pos = v.rfind(add);
	if (pos == string::npos)
		return kifcode->Providestring(v);
	add = v.substr(0, pos);
	pos += nb;
	add += v.substr(pos, v.size() - pos);
	return kifcode->Providestring(add);
}

KifElement* KifRawString::Exec(KifElement* right, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	if (idx == NULL || idx->isConst() || right == idx)
		return this;

	if (value == NULL)
		return kifNULL;

	long ileft = -1, iright = -1;

	//char res = StringIndexes(v, idx, dom, ileft, iright, idthread, callfunc);
	char res = RawStringIndexes((char*)value, stringsize, idx, dom, ileft, iright, idthread, callfunc);

	if (res == 0) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return kifNOELEMENT;
	}

	if (res == 1) {
		char v[] = { (char)value[ileft], 0 };
		return kifcode->Providestringraw(v);
	}

	if (iright == stringsize)
		return kifcode->Providestringraw((char*)value + ileft);

	uchar c = value[iright];
	value[iright] = 0;
	idx = kifcode->Providestringraw((char*)value + ileft);
	value[iright] = c;
	return idx;
}

void KifRawString::Pop(KifElement* ke) {
	int i = ke->Integer();
	if (value == NULL || !stringsize)
		return;

	if (i == -1) {
		i = value[stringsize - 1] = 0;
		stringsize--;
		return;
	}

	if (i >= stringsize || i < 0)
		return;

	stringsize--;
	strcpy_s((char*)value + i, i, (char*)value + i + 1);
}

KifElement* KifRawString::Last() {
	if (value == NULL || !stringsize)
		return kifNULL;

	char c[] = { (char)value[stringsize - 1], 0 };
	return kifcode->Providestringraw(c);
}

Exported void KifRawString::Clean() {
	reference--;
	if (value != NULL)
		memset(value, 0, buffersize);
	if (reference <= 0) {
		if (idgarbage != -1 && idgarbage < kifGlobals->cursorstring)
			kifGlobals->cursorstring = idgarbage;
	}
}

Exported string KifRawString::BaseString() {
	if (value == NULL || stringsize == 0)
		return "''";
	string res;
	sstringing(res, (char*)value);
	return res;
}

Exported string KifRawString::JSonString() {
	if (value == NULL)
		return "";
	string res;
	string s((char*)value);
	jstringing(res, s);
	return res;
}

Exported unsigned char KifRawString::Byte() {
	if (value == NULL)
		return 0;
	return value[0];
}

Exported KifElement* KifRawString::less(KifElement* a) {
	string s = a->String();
	if (value == NULL) {
		if (s.size() == 0)
			return kifFALSE;
		return kifTRUE;
	}

	string v((char*)value);
	if (v < s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifRawString::more(KifElement* a) {
	string s = a->String();
	if (value == NULL) {
		if (s.size() == 0)
			return kifFALSE;
		return kifTRUE;
	}

	string v((char*)value);
	if (v > s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifRawString::same(KifElement* a) {
	string s = a->String();
	if (value == NULL) {
		if (s.size() == 0)
			return kifTRUE;
		return kifFALSE;
	}

	string v((char*)value);
	if (v == s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifRawString::different(KifElement* a) {
	string s = a->String();
	if (value == NULL) {
		if (s.size() == 0)
			return kifFALSE;
		return kifTRUE;
	}

	string v((char*)value);
	if (v != s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifRawString::lessequal(KifElement* a) {
	string s = a->String();
	if (value == NULL)
		return kifTRUE;

	string v((char*)value);
	if (v <= s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifRawString::moreequal(KifElement* a) {
	string s = a->String();
	if (value == NULL) {
		if (s.size() == 0)
			return kifTRUE;
		return kifFALSE;
	}

	string v((char*)value);
	if (v >= s)
		return kifTRUE;
	return kifFALSE;
}


//-----------------------------------------------------------------------------------------------------
Exported void KifString::Clear() {
	KifContainerLock _lock(this);
	value = "";
}

Exported KifElement* KifString::Atom(bool forced) {
	if (reference == 0 && popped && !forced)
		return this;
	KifContainerLock _lock(this);
	return kifcode->Providestring(value);
}

Exported KifElement* KifString::Copy(KifDomain* kp, KifElement* dom) {
	KifContainerLock _lock(this);
	return kifcode->Providestring(value);
}

Exported void KifString::Push(KifElement* ke) {
	KifContainerLock _lock(this);
	value += ke->String();
}

void KifString::storevalue(string s) {
	KifContainerLock _lock(this);
	value += s;
}

void KifString::storevalue(char* k) {
	KifContainerLock _lock(this);
	value += k;
}

string KifString::getstring(int i) {
	KifContainerLock _lock(this);
	return c_char_index(value, i);
}

void KifString::Pop(KifElement* ke) {
	int i = ke->Integer();
	KifContainerLock _lock(this);
	if (value.size() == 0)
		return;
	if (i == -1)
		i = value.size() - 1;
	else
	if (i >= value.size() || i < 0)
		return;
	c_char_index_remove(value, i);
}

KifElement* KifString::Last() {
	string res;
	{
		KifContainerLock _lock(this);
		if (value.size() == 0)
			return kifNULL;
		res = c_char_index(value, value.size() - 1);
	}
	return kifcode->Providestring(res);
}

Exported void KifString::Clean() {
	KifContainerLock _lock(this);
	reference--;
	value = "";
	if (reference <= 0) {
		if (idgarbage != -1 && idgarbage < kifGlobals->cursorstring)
			kifGlobals->cursorstring = idgarbage;
	}
}

//-----------------------------------------------------------------------------------------------------
Exported void KifUString::Clear() {
	KifContainerLock _lock(this);
	value = L"";
}

Exported KifElement* KifUString::Atom(bool forced) {
	if (reference == 0 && popped && !forced)
		return this;
	KifContainerLock _lock(this);
	return kifcode->Provideustring(value);
}

Exported KifElement* KifUString::Copy(KifDomain* kp, KifElement* dom) {
	KifContainerLock _lock(this);
	return kifcode->Provideustring(value);
}

Exported void KifUString::Push(KifElement* ke) {
	KifContainerLock _lock(this);
	value += ke->UString();
}

void KifUString::storevalue(wstring v) {
	KifContainerLock _lock(this);
	value += v;
}

void KifUString::storevalue(wchar_t v) {
	KifContainerLock _lock(this);
	value += v;
}

void KifUString::storevalue(string s) {
	KifContainerLock _lock(this);
	wstring sx;
	sc_utf8_to_unicode(sx, STR(s));
	value += sx;
}

void KifUString::storevalue(char* k) {
	KifContainerLock _lock(this);
	wstring sx;
	s_utf8_to_unicode(sx, k);
	value += sx;
}

string KifUString::getstring(int i) {
	KifContainerLock _lock(this);
	return c_unicode_to_utf8(value[i]);
}

wstring KifUString::getustring(int i) {
	KifContainerLock _lock(this);
	wchar_t w[] = { value[i], 0 };
	return w;
}

void KifUString::Pop(KifElement* ke) {
	int i = ke->Integer();
	KifContainerLock _lock(this);
	if (value.size() == 0)
		return;
	if (i == -1)
		i = value.size() - 1;
	else
	if (i >= value.size() || i < 0)
		return;
	value.erase(i);
}

KifElement* KifUString::Last() {
	{
		KifContainerLock _lock(this);
		if (value.size() == 0)
			return kifNULL;
	}
	return kifcode->Provideustring(value[value.size() - 1]);
}

Exported void KifUString::Clean() {
	KifContainerLock _lock(this);
	reference--;
	value = L"";
}

string KifBaseUString::BaseString() {
	string res;
	string v = String();
	stringing(res, v);
	return res;
}

string KifBaseUString::String() {
	string s;
	s_unicode_to_utf8(s, value);
	return s;
}

string KifBaseUString::JSonString() {
	string res;
	string v = String();
	jstringing(res, v);
	return res;
}

string KifUString::BaseString() {
	KifContainerLock _lock(this);
	string res;
	string v = String();
	stringing(res, v);
	return res;
}

string KifUString::String() {
	string s;
	KifContainerLock _lock(this);
	s_unicode_to_utf8(s, value);
	return s;
}

string KifUString::JSonString() {
	KifContainerLock _lock(this);
	string res;
	string v = String();
	jstringing(res, v);
	return res;
}

Exported KifElement* KifUString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	wstring s = b->UString();

	if (autoself) {
		KifContainerLock _lock(this);
		value += s;
		return this;
	}
	s = a->UString() + s;
	return kifcode->Provideustring(s);
}

Exported KifElement* KifUString::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	wstring s = b->UString();
	int nb = s.size();

	if (autoself) {
		KifContainerLock _lock(this);
		int pos = value.rfind(s);
		if (pos == wstring::npos)
			return this;
		s = value.substr(0, pos);
		pos += nb;
		s += value.substr(pos, value.size() - pos);
		value = s;
		return this;
	}

	wstring v = a->UString();
	int pos = v.rfind(s);
	if (pos == string::npos)
		return kifcode->Provideustring(v);
	s = v.substr(0, pos);
	pos += nb;
	s += v.substr(pos, v.size() - pos);
	return kifcode->Provideustring(s);
}

Exported KifElement* KifUString::less(KifElement* a) {
	wstring s = a->UString();
	KifContainerLock _lock(this);
	if (value<s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifUString::more(KifElement* a) {
	wstring s = a->UString();
	KifContainerLock _lock(this);
	if (value>s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifUString::same(KifElement* a) {
	wstring s = a->UString();
	KifContainerLock _lock(this);
	if (value == s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifUString::different(KifElement* a) {
	wstring s = a->UString();
	KifContainerLock _lock(this);
	if (value != s)
		return kifTRUE;
	return kifFALSE;
}
Exported KifElement* KifUString::lessequal(KifElement* a) {
	wstring s = a->UString();
	KifContainerLock _lock(this);
	if (value <= s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifUString::moreequal(KifElement* a) {
	wstring s = a->UString();
	KifContainerLock _lock(this);
	if (value >= s)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifUString::Exec(KifElement* right, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	if (idx == NULL || idx->isConst() || right == idx)
		return this;

	int ileft, iright;
	KifContainerLock _lock(this);
	char res = StringIndexes(value, idx, dom, ileft, iright, idthread, callfunc);

	if (res == 0) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return kifNOELEMENT;
	}

	if (res == 1)
		return kifcode->Provideustring(value[ileft]);

	return kifcode->Provideustring(value.substr(ileft, iright - ileft));
}

//-----------------------------------------------------------------------------------------------------
Exported string KifBaseString::BaseString() {
	string res;
	stringing(res, value);
	return res;
}

Exported string KifString::BaseString() {
	KifContainerLock _lock(this);
	string res;
	stringing(res, value);
	return res;
}

Exported string KifString::String() {
	KifContainerLock _lock(this);
	return value;
}

Exported string KifString::JSonString() {
	KifContainerLock _lock(this);
	string res;
	jstringing(res, value);
	return res;
}

Exported unsigned char KifString::Byte() {
	KifContainerLock _lock(this);
	if (value.size() == 0)
		return 0;
	return value[0];
}

Exported KifElement* KifString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	string s = b->String();

	if (autoself) {
		KifContainerLock _lock(this);
		value += s;
		return this;
	}
	s = a->String() + s;
	return kifcode->Providestring(s);
}

Exported KifElement* KifString::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	string s = b->String();
	int nb = s.size();

	if (autoself) {
		KifContainerLock _lock(this);
		int pos = value.rfind(s);
		if (pos == string::npos)
			return this;
		s = value.substr(0, pos);
		pos += nb;
		s += value.substr(pos, value.size() - pos);
		value = s;
		return this;
	}

	string v = a->String();
	int pos = v.rfind(s);
	if (pos == string::npos)
		return kifcode->Providestring(v);
	s = v.substr(0, pos);
	pos += nb;
	s += v.substr(pos, v.size() - pos);
	return kifcode->Providestring(s);
}

Exported KifElement* KifString::less(KifElement* a) {
	string s = a->String();
	KifContainerLock _lock(this);
	if (value<s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifString::more(KifElement* a) {
	string s = a->String();
	KifContainerLock _lock(this);
	if (value>s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifString::same(KifElement* a) {
	string s = a->String();
	KifContainerLock _lock(this);
	if (value == s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifString::different(KifElement* a) {
	string s = a->String();
	KifContainerLock _lock(this);
	if (value != s)
		return kifTRUE;
	return kifFALSE;
}
Exported KifElement* KifString::lessequal(KifElement* a) {
	string s = a->String();
	KifContainerLock _lock(this);
	if (value <= s)
		return kifTRUE;
	return kifFALSE;
}

Exported KifElement* KifString::moreequal(KifElement* a) {
	string s = a->String();
	KifContainerLock _lock(this);
	if (value >= s)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifString::Exec(KifElement* right, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	if (idx == NULL || idx->isConst() || right == idx)
		return this;

	int ileft, iright;
	KifContainerLock _lock(this);
	char res = StringIndexes(value, idx, dom, ileft, iright, idthread, callfunc);

	if (res == 0) {
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return kifNOELEMENT;
	}

	if (res == 1)
		return kifcode->Providestringraw(c_char_get(USTR(value), ileft));

	return kifcode->Providestringraw(value.substr(ileft, iright - ileft));
}

Exported KifElement* KifBit::Vector(KifElement* ke, KifVector* vv) {
	KifVector* kvect = vv;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();

	unsigned short v;
	ThreadLock _lock(type);
	for (int i = 0; i < bitvector.size(); i++) {
		v = bitvector[i];
		for (int j = 0; j < 16; j++) {
			if ((v & 1) == 1)
				kvect->Push(kifcode->Provideinteger(1));
			else
				kvect->Push(kifcode->Provideinteger(0));
			v >>= 1;
		}
	}
	return kvect;
}

Exported KifElement* KifBit::Map(KifElement* ke, KifMap* m) {
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = new KifMap(kifcode, NULL);
	else
		kmap->Clear();
	char buffer[100];
	unsigned short v;
	int nb = 0;
	ThreadLock _lock(type);

	for (int i = 0; i < bitvector.size(); i++) {
		v = bitvector[i];
		for (int j = 0; j < 16; j++) {
			sprintf_s(buffer, 100, "%d", nb);
			if ((v & 1) == 1)
				kmap->Push(buffer, kifcode->Provideinteger(1));
			else
				kmap->Push(buffer, kifcode->Provideinteger(0));
			v >>= 1;
			nb++;
		}
	}
	return kmap;
}

Exported KifElement* KifBits::Vector(KifElement* ke, KifVector* v) {
	ThreadLock _lock(type);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();

	map<short, unsigned short>::iterator it;
	for (it = bitmap.begin(); it != bitmap.end(); it++) {
		kvect->Push(kifcode->Provideinteger(it->first));
		kvect->Push(new KifLong(kifcode, NULL, it->second));
	}
	return kvect;
}


Exported KifElement* KifBits::Map(KifElement* ke, KifMap* m) {
	ThreadLock _lock(type);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = new KifMap(kifcode, NULL);
	else
		kmap->Clear();
	char buffer[100];
	map<short, unsigned short>::iterator it;
	unsigned short v;
	int nb = 0;
	for (it = bitmap.begin(); it != bitmap.end(); it++) {
		v = it->second;
		nb = it->first << 4;
		for (int j = 0; j < 16; j++) {
			sprintf_s(buffer, 100, "%d", nb);
			if ((v & 1) == 1)
				kmap->Push(buffer, kifcode->Provideinteger(1));
			else
				kmap->Push(buffer, kifcode->Provideinteger(0));
			v >>= 1;
			nb++;
		}
	}
	return kmap;
}

Exported KifElement* KifBit::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	long sz = b->Integer();
	KifBit* kv = new KifBit(kifcode, NULL);
	unsigned short v;
	register int i;

	int firstelement = Bitindex(sz);
	int firstposition = Bitposition(sz, firstelement);

	ThreadLock _lock(type);
	for (i = 0; i <= firstelement; i++)
		kv->bitvector.push_back(0);

	for (i = 0; i < bitvector.size(); i++) {
		v = bitvector[i];
		for (int j = 0; j < 16; j++) {
			if ((v & 1) == 1)
				kv->bitvector[firstelement] |= 1 << firstposition;
			firstposition++;
			if (firstposition == 16) {
				firstposition = 0;
				firstelement++;
				kv->bitvector.push_back(0);
			}
			v >>= 1;
		}
	}

	if (autoself) {
		bitvector.clear();
		for (i = 0; i < kv->bitvector.size(); i++)
			bitvector.push_back(kv->bitvector[i]);
		kv->Resetpopped();
		return this;
	}

	return kv;
}


Exported KifElement* KifBit::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	long sz = (bitvector.size() << 4) - b->Integer() - 1;
	if (sz < 0)
		return kifNULL;
	KifBit* kv = new KifBit(kifcode, NULL);
	unsigned short v;
	register int i;

	int firstelement = sz >> 4;
	int firstposition = Bitposition(sz, firstelement);

	ThreadLock _lock(type);

	for (i = 0; i <= bitvector.size(); i++)
		kv->bitvector.push_back(0);

	for (i = bitvector.size() - 1; i >= 0; i--) {
		v = bitvector[i];
		for (int j = 0; j < 16; j++) {
			if ((v & 32768) == 32768)
				kv->bitvector[firstelement] |= 1 << firstposition;
			firstposition--;
			if (firstposition < 0) {
				firstposition = 15;
				firstelement--;
				if (firstelement < 0)
					break;
			}
			v <<= 1;
		}
		if (firstelement < 0)
			break;
	}

	if (autoself) {
		bitvector.clear();
		for (i = 0; i < kv->bitvector.size(); i++)
			bitvector.push_back(kv->bitvector[i]);
		kv->Resetpopped();
		return this;
	}

	return kv;
}


Exported KifElement* KifBit::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	ThreadLock _lock(type);
	if (idx == NULL || idx->isConst()) {
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* kvect = Selectvectorinteger(this, contextualpattern);
			unsigned short v;
			for (int i = 0; i < bitvector.size(); i++) {
				v = bitvector[i];
				for (int j = 0; j < 16; j++) {
					if ((v & 1) == 1)
						kvect->storevalue((long)1);
					else
						kvect->storevalue((long)0);
					v >>= 1;
				}
			}
			return kvect;
		}

		if (contextualpattern->aMapContainer()) {
			KifElement* kmap = Selectmapinteger(this, contextualpattern);
			unsigned short v;
			long nb = 0;
			for (int i = 0; i < bitvector.size(); i++) {
				v = bitvector[i];
				for (int j = 0; j < 16; j++) {
					if ((v & 1) == 1)
						kmap->storekey(nb, (long)1);
					else
						kmap->storekey(nb, (long)0);
					v >>= 1;
					nb++;
				}
			}
			return kmap;
		}
		return this;
	}

	KifElement* key = NULL;
	KifElement* keyright = NULL;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true)
			keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	int szbits = bitvector.size() << 4;
	register int ikey = key->Integer();
	key->Release();
	if (ikey < 0)
		ikey += szbits;

	if (ikey < 0 || ikey >= szbits)
		return kifNULL;

	int bidx = Bitindex(ikey);
	int bpos = Bitposition(ikey, bidx);

	if (keyright == NULL) {
		bool v = bitvector[bidx] & (1 << bpos);
		if (v)
			return kifTRUE;
		return kifFALSE;
	}

	int iright = keyright->Integer();
	if (iright <= 0 || keyright == kifNULL)
		iright += szbits;
	else
	if (iright >= szbits)
		iright = szbits;
	keyright->Release();
	int bidxr = Bitindex(iright);
	int bposr = Bitposition(iright, bidxr);

	KifBit* kbits = new KifBit(kifcode, NULL);
	for (ikey = bidx; ikey <= bidxr; ikey++) {
		kbits->bitvector.push_back(0);
		if (ikey > bidx && ikey < bidxr)
			kbits->bitvector[ikey] = bitvector[ikey];
		else {
			if (ikey == bidx) {
				//we keep only the bits starting at bpos
				bpos = ~(1 << bpos) - 1;
				kbits->bitvector[ikey] = bitvector[ikey] & bpos;
			}
			if (ikey == bidxr) {
				//up to bposr
				bposr = (1 << bposr) - 1;
				kbits->bitvector[ikey] = bitvector[ikey] & bposr;
			}
		}
	}
	return kbits;
}

Exported KifElement* KifBits::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	ThreadLock _lock(type);
	map<short, unsigned short>::iterator it;
	unsigned short v;
	if (idx == NULL || idx->isConst()) {
		if (contextualpattern->aVectorContainer() || contextualpattern->type == kifList) {
			KifElement* kvect = Selectvectorinteger(this, contextualpattern);
			for (it = bitmap.begin(); it != bitmap.end(); it++) {
				v = it->second;
				for (int j = 0; j < 16; j++) {
					if ((v & 1) == 1)
						kvect->storevalue((long)1);
					else
						kvect->storevalue((long)0);
					v >>= 1;
				}
			}
			return kvect;
		}

		if (contextualpattern->aMapContainer()) {
			KifElement* kmap = Selectmapinteger(this, contextualpattern);
			long nb = 0;
			for (it = bitmap.begin(); it != bitmap.end(); it++) {
				v = it->second;
				for (int j = 0; j < 16; j++) {
					if ((v & 1) == 1)
						kmap->storekey(nb, (long)1);
					else
						kmap->storekey(nb, (long)0);
					v >>= 1;
					nb++;
				}
			}
			return kmap;
		}

		return this;
	}

	KifElement* key;
	KifElement* keyright = NULL;
	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true)
			keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	int szbits = Size();
	register int ikey = key->Integer();
	key->Release();
	if (ikey < 0)
		ikey += szbits;

	if (ikey < 0 || ikey >= szbits)
		return kifNULL;

	int bidx = Bitindex(ikey);
	int bpos = Bitposition(ikey, bidx);
	if (bitmap.find(bidx) == bitmap.end())
		return kifFALSE;
	if (keyright == NULL) {
		KIFLONG v = bitmap[bidx] & (KIFLONGONE << bpos);
		if (v)
			return kifTRUE;
		return kifFALSE;
	}

	int iright = keyright->Integer();
	if (iright <= 0 || keyright == kifNULL)
		iright += szbits;
	else
	if (iright >= szbits)
		iright = szbits;
	keyright->Release();
	int bidxr = Bitindex(iright);
	int bposr = Bitposition(iright, bidxr);

	KifBits* kbits = new KifBits(kifcode, NULL);
	for (it = bitmap.begin(); it != bitmap.end(); it++) {
		if (ikey > bidx && ikey < bidxr)
			kbits->bitmap[ikey] = it->second;
		else {
			if (ikey == bidx) {
				//we keep only the bits starting at bpos
				bpos = ~(KIFLONGONE << bpos) - 1;
				kbits->bitmap[ikey] = it->second&bpos;
			}
			if (ikey == bidxr) {
				//up to bposr
				bposr = (KIFLONGONE << bposr) - 1;
				kbits->bitmap[ikey] = it->second&bposr;
			}
		}
	}
	return kbits;
}

KifElement* KifEnum::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifVector* kvect = Selectvector(this, contextualpattern);

	for (int i = 0; i < values.size(); i++)
		kvect->Push(values[i]->Exec(contextualpattern, dom, kifNULL, idthread, callfunc));
	return kvect;
}


KifElement* KifVariableDeclaration::Copy(KifDomain* dom, KifElement* vartype) {
	threadblocinit;
	return Create(dom, vartype, idthread, callfunc);
}

KifElement* KifStaticVariable::Create(KifDomain* dom, KifElement* vartype, int idthread, KifCallFunction* callfunc) {
	dom->Set(name, this, NULL);

	if (value->initialconstructor() == false) {
		if (value->type != kifFrame) {
			if (initialisation != kifNULL) {
				//This case is the following: we are creating a variable in a method which was launched from
				//within a frame, and this variable needs to access the frame's variables...
				if CALLFUNCTION(dom) {
					KifDomain* domframe = kifGlobals->Beforetopid(idthread);
					if (domframe != NULL && domframe->type == kifDomain)
						dom = domframe;
				}
				init = false;
				initialisation->Exec(value, dom, this, idthread, callfunc);
			}
		}
	}
	return this;
}

KifElement* KifThroughVariable::Create(KifDomain* dom, KifElement* vartype, int idthread, KifCallFunction* callfunc) {
	dom->Set(name, this, NULL);

	if (value->initialconstructor() == false) {
		if (value->type != kifFrame) {
			if (initialisation != kifNULL) {
				//This case is the following: we are creating a variable in a method which was launched from
				//within a frame, and this variable needs to access the frame's variables...
				if CALLFUNCTION(dom) {
					KifDomain* domframe = kifGlobals->Beforetopid(idthread);
					if (domframe != NULL && domframe->type == kifDomain)
						dom = domframe;
				}
				initialisation->Exec(value, dom, this, idthread, callfunc);
			}
		}
	}
	return this;
}


KifElement* KifVariableDeclaration::Create(KifDomain* dom, KifElement* vartype, int idthread, KifCallFunction* callfunc) {
	KifElement* ke = value->type == kifFrame ? value->Create(NULL, vartype, idthread, callfunc) : value->Copy(NULL, vartype);
	//We look for the initial function
	ke->constructor(dom, initialisation, idthread, callfunc);
	if (eval) {
		switch (eval) {
		case 1:
			ke->Incrementreference();
			ke = new KifVariable(kifcode, NULL, ke, name);
			dom->Set(name, ke, NULL);
			return ke;
		case 2:
			ke->Incrementreference();
			ke = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			dom->Set(name, ke, NULL);
			return ke;
		case 3:
			ke->Incrementreference();
			ke = new KifVariable(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			return ke;
		case 4:
			ke->Incrementreference();
			ke = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			return ke;
		case 11:
			ke->Incrementreference();
			ke = new KifVariable(kifcode, NULL, ke, name);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			return ke;
		case 12:
			ke->Incrementreference();
			ke = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			return ke;
		case 13:
			ke->Incrementreference();
			ke = new KifVariable(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			return ke;
		case 14:
			ke->Incrementreference();
			ke = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			return ke;
		case 21:
			ke->Incrementreference();
			ke = new KifVariable(kifcode, NULL, ke, name);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			return ke;
		case 22:
			ke->Incrementreference();
			ke = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			return ke;
		case 23:
			ke->Incrementreference();
			ke = new KifVariable(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			return ke;
		case 24:
			ke->Incrementreference();
			ke = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			return ke;
		case 200:
			dom->Set(name, ke, NULL);
			return ke;
		case 210:
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			return ke;
		case 220:
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			return ke;
		}
	}
	KifVariable* kvar = NULL;
	if (ke->Basic()) {
		if (Returnfunction() == NULL) {
			eval = 1;
			kvar = new KifVariable(kifcode, NULL, ke, name);
		}
		else {
			kvar = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			eval = 2;
		}

		if (vartype != NULL) {
			eval += 2;
			kvar->Setprivate(vartype->Private());
		}
		ke->Incrementreference();
		ke = kvar;
	}
	else
		eval = 200;

	dom->Set(name, ke, NULL);

	if (value->initialconstructor() == false) {
		if (value->type != kifFrame) {
			if (initialisation != kifNULL) {
				//This case is the following: we are creating a variable in a method which was launched from
				//within a frame, and this variable needs to access the frame's variables...
				if CALLFUNCTION(dom) {
					KifDomain* domframe = kifGlobals->Beforetopid(idthread);
					if (domframe != NULL && domframe->type == kifDomain && dom != domframe) {
						dom = domframe;
						eval += 20;
					}
					else
						eval += 10;
				}
				else
					eval += 10;
				initialisation->Exec(value, dom, ke, idthread, callfunc);
			}
		}
	}
	return ke;
}

KifElement* KifFrameDeclaration::Create(KifDomain* dom, KifElement* vartype, int idthread, KifCallFunction* callfunc) {
	KifElement* ke = value->type == kifFrame ? value->Create(NULL, vartype, idthread, callfunc) : value->Copy(NULL, vartype);
	if (eval) {
		ke->Incrementreference();
		ke->constructor(dom, initialisation, idthread, callfunc);
		switch (eval) {
		case 100:
			dom->Set(name, ke, NULL);
			return ke;
		case 101:
			ke = new KifVariableFrame(kifcode, NULL, ke, name);
			dom->Set(name, ke, NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 102:
			ke = new KifVariableFrameFunction(kifcode, NULL, ke, Returnfunction(), name);
			dom->Set(name, ke, NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 103:
			ke = new KifVariableFrame(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 104:
			ke = new KifVariableFrameFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 110:
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			return ke;
		case 111:
			ke = new KifVariableFrame(kifcode, NULL, ke, name);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			ke->Setfunction(Returnfunction());
			return ke;
		case 112:
			ke = new KifVariableFrameFunction(kifcode, NULL, ke, Returnfunction(), name);
			dom->Set(name, ke, NULL);
			ke->Setframe(dom);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			ke->Setframe(NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 113:
			ke = new KifVariableFrame(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			ke->Setfunction(Returnfunction());
			return ke;
		case 114:
			ke = new KifVariableFrameFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			ke->Setframe(dom);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			ke->Setframe(NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 120:
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			return ke;
		case 121:
			ke = new KifVariableFrame(kifcode, NULL, ke, name);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			ke->Setfunction(Returnfunction());
			return ke;
		case 122:
			ke = new KifVariableFrameFunction(kifcode, NULL, ke, Returnfunction(), name);
			dom->Set(name, ke, NULL);
			ke->Setframe(dom);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			ke->Setframe(NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 123:
			ke = new KifVariableFrame(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			ke->Setfunction(Returnfunction());
			return ke;
		case 124:
			ke = new KifVariableFrameFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			ke->Setframe(dom);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			ke->Setframe(NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		}
	}
	//If we have frames to initialise, we might postpone them...	
	//We look for the initial function
	if (Postpone() == false) {
		//At this level, we are at the top frame initialisation...
		ke->reference++;
		ke->constructor(dom, initialisation, idthread, callfunc);
		ke->reference--;
		//If the initialisation has been delayed, then we trigger it
		//Frame initialisation is done from the mother down to the children...
		if (ke->Delayed())
			ke->ApplyDelayedInitial(idthread, callfunc);
		else
			eval = 100;
	}
	else
		ke->Setpostpone(true); //otherwise, we will call the _initial function later...

	KifVariable* kvar = NULL;
	KifElement* ret = ke;
	bool addframreference = false;
	if (ke->Basic()) {
		//TOP variables belong to only one single frame, which cannot be duplicated
		if (dom == kifcode->kstart) {
			if (Returnfunction() == NULL) {
				eval = 0;
				kvar = new KifVariable(kifcode, NULL, ke, name);
			}
			else {
				eval = 0;
				kvar = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			}
		}
		else {
			if (Returnfunction() == NULL) {
				if (eval)
					eval = 101;
				kvar = new KifVariableFrame(kifcode, NULL, ke, name);
			}
			else {
				kvar = new KifVariableFrameFunction(kifcode, NULL, ke, Returnfunction(), name);
				addframreference = true;
				if (eval)
					eval = 102;
			}
		}
		ke->Incrementreference();
		if (vartype != NULL) {
			kvar->Setprivate(vartype->Private());
			if (eval)
				eval += 2;
		}
		ret = kvar;
	}

	dom->Set(name, ret, NULL);

	if (ke->initialconstructor() == false) {
		if (initialisation != kifNULL) {
			if (addframreference == true)
				kvar->Setframe(dom);
			//This case is the following: we are creating a variable in a method which was launched from
			//within a frame, and this variable needs to access the frame's variables...
			if CALLFUNCTION(dom) {
				ke = kifGlobals->Beforetopid(idthread);
				if (ke != NULL && ke->type == kifDomain) {
					dom = (KifDomain*)ke;
					if (eval)
						eval += 20;
				}
				else {
					if (eval)
						eval += 10;
				}
			}
			else {
				if (eval)
					eval += 10;
			}

			initialisation->Exec(value, dom, ret, idthread, callfunc);
			if (addframreference == true)
				kvar->Setframe(NULL);
		}
	}
	if (kvar != NULL)
		kvar->Setfunction(Returnfunction());
	return ret;
}

KifElement* KifVariableDeclaration::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc) {
	dom->Disableadding();

	if (eval) {
		if (eval == 255) //the variable was predeclared for some reasons such as a loadin
			return dom->Declaration(name);

		ke = value->type == kifFrame ? value->Create(NULL, this, idthread, callfunc) : value->Copy(NULL, this);
		//We look for the initial function
		ke->constructor(dom, initialisation, idthread, callfunc);
		switch (eval) {
		case 3:
			ke->Incrementreference();
			ke = new KifVariable(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			return ke;
		case 4:
			ke->Incrementreference();
			ke = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			return ke;
		case 13:
			ke->Incrementreference();
			ke = new KifVariable(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			return ke;
		case 14:
			ke->Incrementreference();
			ke = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			return ke;
		case 23:
			ke->Incrementreference();
			ke = new KifVariable(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			return ke;
		case 24:
			ke->Incrementreference();
			ke = new KifVariableFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			return ke;
		case 103:
			ke->Incrementreference();
			ke = new KifVariableFrame(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 104:
			ke->Incrementreference();
			ke = new KifVariableFrameFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 113:
			ke->Incrementreference();
			ke = new KifVariableFrame(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			ke->Setfunction(Returnfunction());
			return ke;
		case 114:
			ke->Incrementreference();
			ke = new KifVariableFrameFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			ke->Setframe(dom);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			ke->Setframe(NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 123:
			ke->Incrementreference();
			ke = new KifVariableFrame(kifcode, NULL, ke, name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			ke->Setfunction(Returnfunction());
			return ke;
		case 124:
			ke->Incrementreference();
			ke = new KifVariableFrameFunction(kifcode, NULL, ke, Returnfunction(), name);
			ke->Setprivate(privatevariable);
			dom->Set(name, ke, NULL);
			ke->Setframe(dom);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			ke->Setframe(NULL);
			ke->Setfunction(Returnfunction());
			return ke;
		case 200:
			dom->Set(name, ke, NULL);
			return ke;
		case 210:
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, dom, ke, idthread, callfunc);
			return ke;
		case 220:
			dom->Set(name, ke, NULL);
			initialisation->Exec(value, kifGlobals->Beforetopid(idthread), ke, idthread, callfunc);
			return ke;
		}
	}

	if (type == kifVariableDeclaration && value->type != kifFrame) {
		if (initialisation != kifNULL) {
			//if we are within a frame, then dom should point to that frame
			ke = kifGlobals->Beforetopid(idthread);
			if (ke != NULL && ke->type == kifDomain && dom == kifGlobals->Top())
				initialisation->Exec(value, (KifDomain*)ke, value, idthread, callfunc);
			else
				initialisation->Exec(value, dom, value, idthread, callfunc);
		}
		Setreference();
		return value;
	}

	return Create(dom, this, idthread, callfunc);
}

KifElement* KifFunction::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	int maxid = instructions.size();

	if (choice < 10) {
		ThreadLock _lock(kifFunction);
		if (choice < 10) {
			choice = NOVARIABLEINFUNCTION;
			for (int i = 0; i < maxid; i++) {
				if (instructions[i]->Variable()) {
					choice = VARIABLEINFUNCTION;
					break;
				}
			}
			if (choice == NOVARIABLEINFUNCTION && instructions.size() == 1)
				choice = SINGLEINSTRUCTION;
		}
	}


	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	if (debugging) {
		//Debugging part...
		DebugPtr()->kiffunctioncount++;
		DebugPtr()->kifinfunction = true;
		DebugPtr()->Debugging(kifcode, this, this, dom, callfunc);
		DebugPtr()->kifinfunction = false;
	}

	KifInstructionStack _stacking(idthread, name);
	KifElement* returnval = kifNULL;
	KifElement* ke;

	if (choice == SINGLEINSTRUCTION) {
		ke = instructions[0];
		_stacking.Modify(ke);
		if (debugging)
			debugging = DebugPtr()->Debugging(kifcode, this, ke, dom, callfunc);
		returnval = ke->Exec(contextualpattern, dom, val, idthread, callfunc);

		if (returnval->isBreak())
			return returnval;

		if (returnval->Returned())
			returnval = returnval->Returnvalue();
		else {
			returnval->Release();
			returnval = kifNULL;
		}
		if (Errorid(idthread))
			returnval = Errorptrid(idthread);
		if (debugging)
			DebugPtr()->kiffunctioncount--;
		return returnval;
	}

	if (choice == NOVARIABLEINFUNCTION) {
		for (int i = 0; i < maxid; i++) {
			ke = instructions[i];
			_stacking.Modify(ke);
			if (debugging)
				debugging = DebugPtr()->Debugging(kifcode, this, ke, dom, callfunc);
			returnval = ke->Exec(contextualpattern, dom, val, idthread, callfunc);
			if (returnval->isBreak())
				break;
			if (returnval->Returned()) {
				returnval = returnval->Returnvalue();
				break;
			}
			returnval->Release();
			returnval = kifNULL;

			if (Errorid(idthread)) {
				returnval = Errorptrid(idthread);
				break;
			}
		}
		if (debugging)
			DebugPtr()->kiffunctioncount--;
		return returnval;
	}

	vector<short> stacks;
	for (int i = 0; i < maxid; i++) {
		ke = instructions[i];
		_stacking.Modify(ke);
		if (debugging)
			debugging = DebugPtr()->Debugging(kifcode, this, ke, dom, callfunc);

		if (ke->Variable()) {
			stacks.push_back(ke->Name());
			ke->Exec(contextualpattern, callfunc, val, idthread, callfunc);
		}
		else {
			returnval = ke->Exec(contextualpattern, dom, val, idthread, callfunc);
			if (returnval->isBreak())
				break;
			if (returnval->Returned()) {
				returnval = returnval->Returnvalue();
				break;
			}
			returnval->Release();
			returnval = kifNULL;
		}

		if (Errorid(idthread)) {
			returnval = Errorptrid(idthread);
			break;
		}
	}

	maxid = stacks.size();
	for (int i = 0; i < maxid; i++) {
		callfunc->declarations[stacks[i]]->Resetreference();
		callfunc->declarations.erase(stacks[i]);
		callfunc->Cleantracks(stacks[i]);
	}

	if (debugging)
		DebugPtr()->kiffunctioncount--;
	return returnval;
}

//We only use this structure to execute a piece of global code into a thread, see KifFrame::RunThread
KifElement* KifFunctionSequence::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	bool debugging;

	KifElement* ke;
	KifDebugInfo* debugptr = DebugPtrInit(debugging);
	KifInstructionStack _stacking(idthread, name);
	size_t maxid = instructions.size();
	for (int i = 0; i < maxid; i++) {
		ke = instructions[i];
		_stacking.Modify(ke);
		if (debugging)
			debugging = debugptr->Debugging(kifcode, this, ke, dom, NULL);
		ke->Exec(contextualpattern, dom, val, idthread, callfunc)->Release();
		if (Errorid(idthread))
			return Errorptrid(idthread);
	}

	return kifTRUE;
}

KifElement* KifPolynomial::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	bool debugging;

	string keyval;

	bin_hash<KifElement*>::iterator it;
	for (it = callfunc->declarations.begin(); it != callfunc->declarations.end(); it++) {
		keyval += "_";
		keyval += it->second->String();
	}

	KifElement* returnval = ReturnResult(keyval);
	if (returnval != NULL)
		return returnval;

	returnval = kifNULL;
	bin_hash<KifElement*>* stacks = NULL;

	KifElement* ke;
	//Debugging part...
	KifDebugInfo* debugptr = DebugPtrInit(debugging);

	if (debugging) {
		debugptr->kiffunctioncount++;
		debugptr->kifinfunction = true;
		debugptr->Debugging(kifcode, this, this, dom, callfunc);
		debugptr->kifinfunction = false;
	}

	KifInstructionStack _stacking(idthread, name);
	size_t maxid = instructions.size();
	for (int i = 0; i < maxid; i++) {
		ke = instructions[i];
		_stacking.Modify(ke);
		if (debugging)
			debugging = debugptr->Debugging(kifcode, this, ke, dom, callfunc);
		if (ke->Variable()) {
			short thename = ke->Name();
			//If this variable has already been declared
			if (stacks == NULL)
				stacks = new bin_hash<KifElement*>;
			(*stacks)[thename] = callfunc->Declaration(thename);
			if (debugging)
				debugging = debugptr->Debugging(kifcode, this, ke, dom, callfunc);
			ke->Exec(contextualpattern, callfunc, val, idthread, callfunc);
		}
		else {
			returnval = ke->Exec(contextualpattern, dom, val, idthread, callfunc);
			if (returnval->Returned()) {
				returnval = returnval->Returnvalue();
				break;
			}
			returnval->Release();
			returnval = kifNULL;
		}
		if (Errorid(idthread))
			return Errorptrid(idthread);
	}


	if (stacks != NULL) {
		callfunc->Resetdeclaration(*stacks);
		delete stacks;
	}

	debugptr->kiffunctioncount--;
	StoreResult(keyval, returnval);
	return returnval;
}

long KifError::Integer() {
	if (value == "")
		return 0;
	long i = conversionintegerhexa(STR(value));
	return i;
}


string KifError::String() {
	string message = Endl;
	message += value;
	message += Endl;
	for (int i = 0; i < stackmessages.size(); i++) {
		message += stackmessages[i];
		message += Endl;
	}

	return message;
}

KifElement* KifError::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	return kifGlobals->THR[idthread]->Setreturnvalue(kifNULL);
}

KifElement* KifCall::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	//If we are called from outside
	if (body == NULL)
		return kifNULL;
	//KifCallFunctionLocal kfunc(kifcode, id_call, body);
	KifCallFunctionGeneral kfunc(NULL, NULL, id_call, body);
	kfunc.kifcode = kifcode;
	//We add our parameters
	for (int i = 1; i < ((KifCallFunctionPredefined*)dom)->parameters.size(); i++)
		kfunc.parameters.push_back(((KifCallFunctionPredefined*)dom)->parameters[i]);
	return kfunc.Exec(kifNULL, domain, kifNULL, idthread, callfunc);
}


KifElement* KifCallFunctionCall::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	KifCall* call = (KifCall*)kcall->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	if (call->body == NULL)
		return kifcode->Returnerror(KIFLIBSTRINGS[232]);
	body = call->body;
	dynamicselection = NODYNAMIC;
	if (body->next != NULL)
		dynamicselection = FULLDYNAMIC;
	return KifCallFunctionGeneral::Exec(contextualpattern, dom, val, idthread, callfunc);
}

KifElement* KifCallFunctionExternal::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
#ifdef _DEBUG
	string nm = KifIdString(name);
#endif

	if (val->Declaration(name) == NULL)
		return kifcode->Returnerror(KIFLIBSTRINGS[256]);


	KifCallFunctionExternal* localfunc = this;
	if (idthread) {
		localfunc = new KifCallFunctionExternal(NULL, NULL, name, body);
		localfunc->kifcode = kifcode;
		localfunc->parameters = parameters;
	}


	//This the case, when we use an external description in a dynamic library
	localfunc->clean = true;
	localfunc->caller = callfunc;

	KifElement* ke = body->Exec(contextualpattern, dom, val, idthread, localfunc);

	localfunc->Toclean(ke);

	if (localfunc != this)
		delete localfunc;

	if (function != NULL)
		ke = applyfunction(ke);

	if (Errorid(idthread))
		return Errorptrid(idthread);
	return ke;
}


KifElement* KifCallFunctionPredefined::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	KifElement* ke;

	if (val->Yourowntype() != -1) {
		//In this case, a Predifined method has been replaced with a function from an external library
		//This case occurs when a call is done from a map or a vector element, which was assigned the wrong
		//element due to lack of information at compile time
		if (val->Declaration(name) == NULL) {
			string err = KIFLIBSTRINGS[38];
			err += kifGlobals->kifIdString[name];
			return kifcode->Returnerror(err);
		}
		KifCallMethod kcm(name, P_FULL);
		KifCallFunctionExternal externalcall(NULL, NULL, name, &kcm);
		externalcall.kifcode = kifcode;
		externalcall.clean = true;
		//We keep all parameters except first one
		for (int ipar = 1; ipar < parameters.size(); ipar++)
			externalcall.parameters.push_back(parameters[ipar]);
		ke = kcm.Exec(contextualpattern, dom, val, idthread, &externalcall);
		externalcall.Toclean(ke);
		if (function != NULL)
			ke = applyfunction(ke);
		return ke;
	}

	KifCallFunctionPredefined* localfunc = this;
	//To handle thread calls... We need to protect our parameters...
	if (idthread) {
		localfunc = new KifCallFunctionPredefined(NULL, NULL, name, body);
		localfunc->kifcode = kifcode;
		localfunc->parameters = parameters;
	}

	localfunc->clean = true;
	localfunc->caller = callfunc;
	if (selfinsert == true) {
		val = val->Returnvalue();
		if (val->Simple()) {
			localfunc->parameters[0] = val;
			ke = (*body->function)(contextualpattern, dom, localfunc, idthread);
		}
		else {
			KifSelf kbloc(NULL, NULL, val);
			kbloc.kifcode = kifcode;
			localfunc->parameters[0] = &kbloc;
			ke = (*body->function)(contextualpattern, dom, localfunc, idthread);
		}
	}
	else
		ke = (*body->function)(contextualpattern, dom, localfunc, idthread);

	//We get rid of the temporary values created during the evaluation of the parameters.
	localfunc->Toclean(ke);
	if (localfunc != this)
		delete localfunc;

	if (function != NULL)
		ke = applyfunction(ke);
	return ke;
}


Exported KifElement* KifCallExecuteThread::Arguments(KifCallFunction* callfunc) {
	if (Errorid(threadid))
		return Errorptrid(threadid);

	KifElement* karg;

	currentbody = body;

	if (currentbody == NULL) {
		//Then we are into a vector or a map..., we need to find out which 
		//function to use
		karg = domain->Frame()->Declaration(name);
		if (karg == NULL || karg->isFunction() == false || KifCheckParameters((KifFunction*)karg, this, false, &currentbody) != 0) {
			string message = KIFLIBSTRINGS[231];
			message += "'";
			message += kifGlobals->kifIdString[name];
			message += KIFKIFSTRINGS[18];
			if (domain->Type() == "")
				message += value->Type();
			else
				message += domain->Type();
			message += "'";
			return kifcode->Returnerror(message);
		}
	}
	else  {
		//We set the parameters
		if (dynamicselection == FULLDYNAMIC && currentbody->Unlimited() == false && KifCheckParameters(body->top, this, false, &currentbody)) {
			string message = KIFLIBSTRINGS[158];
			message += kifGlobals->kifIdString[name];
			return kifcode->Returnerror(message);
		}
	}


	size_t i;
	bool err = false;
	KifElement* kparam;
	//The parameters are expressions in most encompassing frame, not the local one...
	KifDomain* anciendom = kifGlobals->Beforetopid(threadid);
	if (anciendom == NULL)
		anciendom = kifcode->kstart;
	if (callfunc == NULL)
		callfunc = (KifCallFunction*)kifGlobals->Topid(threadid);
	short na;
	int sz = parameters.size();
	KifElement* arg;
	while (currentbody != NULL) {
		for (i = 0; i < sz; i++) {
			//We expect some arguments in the function declaration
			arg = currentbody->arguments[i];
			//the name of this argument
			//We then compute our parameter
			na = arg->Name();
			karg = arg->Linkedvalue();
			kparam = parameters[i]->Exec(karg, anciendom, kifNULL, threadid, callfunc);

			//self is always transparent
			if (karg->type == kifSelf)
				keep(na, kparam);
			else {//Simple is: Float, integer, string or boolean
				//In this case, the bridges from one structure to another are rather limited...
				if (karg->type == kifFrame && kparam->type == kifDomain) {
					//if a frame is a subframe of another frame
					if (compatibleArgument(karg->Name(), kparam->Frame()->Name()) == false) {
						//If we have a cast from one frame into another, then we use it
						kparam = ((KifDomain*)kparam)->Framedefined(karg->Name(), karg, (KifDomain*)kparam);
						//We still check that we have a proper translation
						if (kparam == NULL || karg != kparam->Frame()) {
							err = true;
							if (kparam != NULL)
								kparam->Release();
							break;
						}
					}
					keep(na, kparam);
				}
				else {
					if (compatibleArguments(kparam, karg) == true) {
						keep(na, kifNULL);
						KifElement* newarg = arg->Create(this, arg, threadid, callfunc);
						//Particular case. Parameters cannot be associated with functions
						newarg->Setfunction(NULL);
						newarg->Setvalue(kparam);
					}
					else {
						if (karg->type != kparam->type) {
							err = true;
							kparam->Release();
							break;
						}
						keep(na, kparam);
					}
				}
			}
			kparam->Release();
		}

		if (err && dynamicselection != NODYNAMIC && currentbody->next != NULL) {
			currentbody = currentbody->next;
			while (currentbody != NULL && currentbody->Unlimited() == false && KifCompareArgs(currentbody, this) == false)
				currentbody = currentbody->next;
			if (currentbody == NULL)
				break;

			err = false;
			bin_hash<KifElement*>::iterator it;
			for (it = declarations.begin(); it != declarations.end(); it++)
				it->second->Resetreference();
			declarations.clear();
			continue;
		}
		break;
	}

	//We might have an unlimited number of arguments...
	if (currentbody->Unlimited() == true) {
		string s = "_";
		s += kifGlobals->kifIdString[name];
		na = KifNewId(s);
		KifVector* kvect = new KifVector(kifcode, NULL);
		keep(na, kvect);
		for (; i < parameters.size(); i++) {
			kparam = parameters[i]->Exec(karg, anciendom, kifNULL, threadid, callfunc);
			kvect->Push(kparam);
			kparam->Release();
		}
	}

	if (sz < currentbody->arguments.size()) {
		//If we have some predeclared arguments in our function
		for (; sz < currentbody->arguments.size(); sz++) {
			arg = currentbody->arguments[sz];
			//the name of this argument
			//We then compute our parameter
			na = arg->Name();
			keep(na, kifNULL);
			arg->Create(this, arg, threadid, callfunc);
		}
	}

	if (err) {
		string message = KIFLIBSTRINGS[159];
		message += kifGlobals->kifIdString[name];
		bin_hash<KifElement*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++)
			it->second->Resetreference();
		declarations.clear();
		return kifcode->Returnerror(message);
	}
	return currentbody;
}

Exported void KifCallExecuteThread::Execution() {
	if (Errorid(threadid))
		return;

	returnvalue = kifNULL;
	KifElement* karg;

	d_pop ppthread(threadid, this, domain);

	karg = currentbody->Exec(context, domain, value, threadid, this);

	if (function != NULL) {
		karg = applyfunction(karg);
		if (returnvalue != kifNULL)
			returnvalue = karg;
	}

	if (streamvalue != NULL) {
		if (streamvalue->type == kifCouple) {
			if (streamvalue->Kifexist())
				streamvalue->Setvalue(returnvalue);
			else
				kifcode->Returnerror(KIFKIFSTRINGS[19]);

			((KifCouple*)streamvalue)->indexes->key->Resetreference();
			delete ((KifCouple*)streamvalue)->indexes;
			delete streamvalue;
		}
		else {
			if (Kifexist())
				streamvalue->Setvalue(returnvalue);
			else
				kifcode->Returnerror(KIFKIFSTRINGS[19]);
		}
	}

	if (karg != returnvalue)
		karg->Release();

	returnvalue->Release();
	bin_hash<KifElement*>::iterator it;
	for (it = declarations.begin(); it != declarations.end(); it++)
		it->second->Resetreference();
}

KifElement* KifCallFunctionGeneral::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	KifElement* karg;
#ifdef _DEBUG
	string functionname = kifGlobals->kifIdString[name];
#endif
	KifFunction* bd = body;
	size_t sz = parameters.size();

	if (_eval != 2) {
		if (bd == NULL) {
			//Then we are into a vector or a map..., we need to find out which 
			//function to use
			karg = dom->Frame()->Declared(name);
			_eval = 1;
			if (karg == NULL || karg->isFunction() == false || KifCheckParameters((KifFunction*)karg, this, false, &bd) != 0) {
				string message = KIFLIBSTRINGS[231];
				message += "'";
				message += kifGlobals->kifIdString[name];
				message += KIFKIFSTRINGS[18];
				if (dom->Type() == "")
					message += val->Type();
				else
					message += dom->Type();
				message += "'";
				return kifcode->Returnerror(message);
			}
		}
		else  {
			_eval = 2;
			if (!bd->isFunction() || (dynamicselection == FULLDYNAMIC && bd->Unlimited() == false && KifCheckParameters(body->top, this, false, &bd))) {
				string message = KIFLIBSTRINGS[158];
				message += kifGlobals->kifIdString[name];
				return kifcode->Returnerror(message);
			}
		}
	}

	size_t i;
	bool err = false;
	KifElement* kparam;
	KifElement* newarg;
	//The parameters are expressions in most encompassing frame, not the local one...
	KifDomain* anciendom = kifGlobals->Beforetopid(idthread);
	if (anciendom == NULL)
		anciendom = kifcode->kstart;

	if (callfunc == NULL)
		callfunc = (KifCallFunction*)kifGlobals->Topid(idthread);

	KifCallFunction* localfunc = this;
	if (idthread || declarations.size()) {
		localfunc = new KifCallFunction(NULL, NULL, name);
		localfunc->kifcode = kifcode;
	}

	short na;
	KifElement* arg;
	while (bd != NULL) {
		for (i = 0; i < sz; i++) {
			//We expect some arguments in the function declaration
			arg = bd->arguments[i];
			//the name of this argument
			//We then compute our parameter
			na = arg->Name();
			//string nnn=kifGlobals->kifIdString[na];
			karg = arg->Linkedvalue();
			kparam = parameters[i]->Exec(karg, anciendom, kifNULL, idthread, callfunc);

			//self is always transparent
			if (karg->type == kifSelf)
				localfunc->keep(na, kparam);
			else {//Simple is: Float, integer, string or boolean
				//In this case, the bridges from one structure to another are rather limited...
				if (karg->type == kifFrame && kparam->type == kifDomain) {
					//if a frame is a subframe of another frame
					if (compatibleArgument(karg->Name(), kparam->Frame()->Name()) == false) {
						//If we have a cast from one frame into another, then we use it
						kparam = ((KifDomain*)kparam)->Framedefined(karg->Name(), karg, (KifDomain*)kparam);
						//We still check that we have a proper translation
						if (kparam == NULL || karg != kparam->Frame()) {
							err = true;
							if (kparam != NULL)
								kparam->Release();
							break;
						}
					}
					localfunc->keep(na, kparam);
				}
				else {
					if (compatibleArguments(kparam, karg) == true) {
						localfunc->keep(na, kifNULL);
						newarg = arg->Create(localfunc, arg, idthread, callfunc);
						//Particular case. Parameters cannot be associated with functions
						newarg->Setfunction(NULL);
						newarg->Setvalue(kparam);
					}
					else {
						if (karg->type != kparam->type) {
							err = true;
							kparam->Release();
							break;
						}
						localfunc->keep(na, kparam);
					}
				}
			}
			kparam->Release();
		}

		if (err && dynamicselection != NODYNAMIC && bd->next != NULL) {
			bd = bd->next;
			while (bd != NULL && bd->Unlimited() == false && KifCompareArgs(bd, this) == false)
				bd = bd->next;
			if (bd == NULL)
				break;

			bin_hash<KifElement*>::iterator it;
			for (it = localfunc->declarations.begin(); it != localfunc->declarations.end(); it++)
				it->second->Resetreference();

			if (localfunc != this) {
				delete localfunc;
				localfunc = new KifCallFunction(NULL, NULL, name);
			}
			else
				declarations.clear();
			err = false;
			continue;
		}
		break;
	}

	//We might have an unlimited number of arguments...
	if (!err && bd->Unlimited() == true) {
		string s = "_";
		s += kifGlobals->kifIdString[name];
		na = KifNewId(s);
		KifVector* kvect = new KifVector(kifcode, NULL);
		localfunc->keep(na, kvect);
		for (; i < sz; i++) {
			kparam = parameters[i]->Exec(karg, anciendom, kifNULL, idthread, callfunc);
			kvect->Push(kparam);
			kparam->Release();
		}
	}

	if (sz < bd->arguments.size()) {
		//If we have some predeclared arguments in our function
		for (; sz < bd->arguments.size(); sz++) {
			arg = bd->arguments[sz];
			//the name of this argument
			//We then compute our parameter
			na = arg->Name();
			localfunc->keep(na, kifNULL);
			arg->Create(localfunc, arg, idthread, callfunc);
		}
	}

	if (err == true) {
		string message = KIFLIBSTRINGS[159];
		message += kifGlobals->kifIdString[name];
		karg = kifcode->Returnerror(message);
	}
	else {
		if (dom->Failed())
			karg = kifNOELEMENT;
		else {
			kifGlobals->PushFunctionStackid(idthread, localfunc, dom);
			karg = bd->Exec(contextualpattern, dom, val, idthread, localfunc);
			karg->Setpopped(1);
			kifGlobals->PopFunctionStack(idthread);
			if (function != NULL)
				karg = applyfunction(karg);
		}
	}

	bin_hash<KifElement*>::iterator it;
	for (it = localfunc->declarations.begin(); it != localfunc->declarations.end(); it++)
		it->second->Resetreference();

	if (localfunc != this)
		delete localfunc;
	else
		declarations.clear();

	char kpopped = karg->popped;
	karg->popped = 10;
	kifGlobals->Garbage();
	karg->popped = kpopped;
	return karg;
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//--- HASKELL BODIES
//-------------------------------------------------------------------------------------------------------------------------

class itemEnv {
public:
	KifIteration* iter;
	KifElement* cleaning;
	KifElement* variable;
	itemEnv* next;
	KifElement* key;
	KifElement* kmap;

	KifDomain* dom;
	KifElement* context;
	KifCallFunction* callfunc;
	int idthread;

	int idx;
	bool reset;
	char itervar;

	itemEnv(int i, KifElement* c, KifDomain* d, int idt, KifCallFunction* cl) {
		context = c;
		dom = d;
		callfunc = cl;
		idthread = idt;

		idx = i + 1; //to point to the local instruction
		iter = NULL;
		cleaning = NULL;
		variable = NULL;
		reset = false;
		next = NULL;
		itervar = 0;
		kmap = kifNULL;
	}

	~itemEnv() {
		kmap->Release();
		if (next != NULL)
			delete next;
	}

	inline void setvariable(KifElement* v) {
		variable = v;
		if (variable->VariableValue()->type == kifIterator) {
			variable = variable->VariableValue();
			itervar = 1;
		}
		else
		if (variable->type == kifBaseVector)
			itervar = 2;
		else
		if (variable->type == kifBaseMap) {
			itervar = 3;
			kmap = new KifMap(v->kifcode, NULL);
			kmap->popped = 1;
		}
	}

	inline bool resetvalue(KifElement* v) {
		reset = v->Boolean();
		if (v == kifZERO || v == kifONE)
			return true;
		return false;
	}

	void clean() {
		if (cleaning != NULL)
			cleaning->Release();
		if (iter != NULL)
			iter->Release();
		if (itervar == 1)
			((KifIterator*)variable)->value = kifNULL;
		if (next != NULL)
			next->clean();
	}

	inline void set(bool init = false) {
		if (!itervar) {
			if (cleaning->aMapContainer())
				key = iter->IteratorKey();
			else
				key = iter->IteratorValue();
			variable->Setvalue(key);
			key->Release();
			if (next)
				next->set(init);
			return;
		}

		if (itervar == 1) {
			if (init)
				((KifIterator*)variable)->value = iter;
			if (next)
				next->set(init);
			return;
		}


		if (itervar == 2) {
			key = iter->IteratorValue();
			key->Match((KifBaseVector*)variable, dom, idthread, callfunc);
		}
		else {
			key = iter->IteratorKey();
			KifElement* val = iter->IteratorValue();
			kmap->Push(key, val);
			val->Release();
			kmap->Match((KifBaseMap*)variable, dom, idthread, callfunc);
			kmap->Clear();
		}

		key->Release();

		if (kifGlobals->THR[idthread]->error) {
			Errorptrid(idthread);
			return;
		}
		if (next)
			next->set(init);
	}

	void Next() {
		iter->Next();
		if (next != NULL)
			next->Next();
	}

	bool Begin() {
		if (iter->Begin() == kifFALSE)
			return false;
		if (next != NULL)
			return next->Begin();
		return true;
	}

	bool End() {
		if (iter->End() == kifTRUE)
			return true;
		if (next != NULL)
			return next->End();
		return false;
	}

	bool update(KifDomainLambda* kframe, KifDomain* dom) {
		if (reset) { //this iterator depends on a variable from other iterators...
			cleaning->Release();
			threadblocinit;
			cleaning = kframe->instructions[idx]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			iter->Release();
			iter = (KifIteration*)cleaning->Newiterator(false);
			if (next)
				next->update(kframe, dom);
			return true;
		}
		else
		if (next)
			return next->update(kframe, dom);
		return false;
	}
};

class applyEnv {
public:
	vector<itemEnv*> items;
	bool stop;

	applyEnv() {
		stop = false;
	}

	~applyEnv() {
		for (int i = 0; i < items.size(); i++) {
			items[i]->clean();
			delete items[i];
		}
	}

	//--------------updates-------------------

	inline void next(int c) {
		items[c]->Next();
	}

	inline bool end(int c) {
		if (items[c]->End()) {
			if (!c)
				stop = true;
			return true;
		}
		return false;
	}

	inline bool begin(int c) {
		return items[c]->Begin();
	}

	inline void set(int c, bool init = false) {
		items[c]->set(init);
	}

	inline bool update(int c, KifDomainLambda* kframe, KifCallFunctionHaskell* kthis) {
		return items[c]->update(kframe, kthis);
	}

	bool evaluate(int& current, KifDomainLambda* kframe, KifCallFunctionHaskell* kthis) {
		bool skip = false;
		for (; current < items.size(); current++) {
			bool rst = update(current, kframe, kthis);
			if (begin(current))
				set(current, rst);
			else
				skip = true;
		}
		current--;
		return skip;
	}
};

class applyEnvShort {
public:
	KifElement* klist;
	KifIteration* iter;
	KifElement* init;
	KifElement* ret;
	KifElement* var;

	applyEnvShort() {
		klist = NULL;
		iter = NULL;
		init = NULL;
		ret = NULL;
		var = NULL;
	}

	~applyEnvShort() {
		if (ret != NULL)
			ret->Setpopped(1);
		if (klist != NULL)
			klist->Release();
		if (iter != NULL)
			iter->Release();
		if (init)
			init->Release();
	}

	void begin(bool d) {
		iter = (KifIteration*)klist->Newiterator(d);
	}
};

//-------------------------------------------------------------------------------------------------------------------------

KifElement* KifCallFunctionHaskell::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ret, int idthread, KifCallFunction* callfunc) {
	if (Errorid(idthread))
		return Errorptr();

	kifGlobals->THR[idthread]->prologstack++;

	KifDomain* anciendom = kifGlobals->Beforetopid(idthread);
	if (anciendom == NULL)
		anciendom = kifcode->kstart;

	int i;
	int sz = parameters.size();
	KifElement** params = NULL;
	if (sz) {
		params = new KifElement*[sz + 1];

		for (i = 0; i < sz; i++) {
			params[i] = parameters[i]->Exec(kifNULL, anciendom, kifNULL, idthread, callfunc);
			params[i]->Setreference();
		}
	}

	bool addcallfunc = false;
	KifCallFunctionHaskell* localfunc = this;
	if (callfunc != NULL && callfunc->declarations.size())
		addcallfunc = true;

	if (addcallfunc || idthread || declarations.size())
		localfunc = new KifCallFunctionHaskell(this);
	else
		Initialize();

	kifGlobals->PushFunctionStackid(idthread, localfunc, dom);
	KifFunctionLambda* bd = body;
	short n, nm;

	while (bd != NULL) {
		if (bd->arguments.size() != sz) {
			bd = (KifFunctionLambda*)bd->next;
			continue;
		}

		if (addcallfunc)
			localfunc->declarations = callfunc->declarations;

		localfunc->frame = bd->frame;
		if (sz)
			ret = localfunc->Apply(bd, params, contextualpattern, dom, idthread);
		else
			ret = localfunc->KIFHASKELL(contextualpattern, dom, bd, idthread);

		ret->Putpopped(1);
		for (n = 0; n < localfunc->created.size(); n++) {
			nm = localfunc->created[n];
			if (nm != -1 && localfunc->declarations[nm] != NULL)
				localfunc->declarations[nm]->Resetreference();
		}
		ret->Putpopped(0);
		localfunc->created.clear();
		localfunc->declarations.clear();
		if (ret != kifRAISEERROR && ret != kifBREAK) {
			ret->Setpopped(1);
			break;
		}

		ret = kifNULL;
		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			ret = Errorptr();
			break;
		}

		bd = (KifFunctionLambda*)bd->next;
	}

	kifGlobals->PopFunctionStack(idthread);
	if (sz) {
		for (i = 0; i < sz; i++)
			params[i]->Resetreference();
		delete[] params;
	}
	if (localfunc != this)
		delete localfunc;

	kifGlobals->THR[idthread]->prologstack--;

	return ret;
}

//-------------------------------------------------------------------------------------------------------------------------

KifElement* KifCallFunctionHaskell::Apply(KifFunctionLambda* bd, KifElement** params, KifElement* contextualpattern, KifDomain* dom, int idthread) {
	//The parameters are expressions in most encompassing frame, not the local one...
	for (int i = 0; i < bd->arguments.size(); i++) {
		//We expect some arguments in the function declaration
		KifElement* arg = bd->arguments[i];
		//string nnn=kifGlobals->kifIdString[na];
		//particular case where karg is BaseVector in a haskell expression (only case)
		//we need to split the expression... parameters[i] is also a vector
		if (arg->type == kifBaseVector) {
			if (params[i]->Split((KifBaseVector*)arg, this) == kifRAISEERROR)
				return kifRAISEERROR;
			continue;
		}

		if (arg->type == kifBaseMap) {
			if (params[i]->Split((KifBaseMap*)arg, this) == kifRAISEERROR)
				return kifRAISEERROR;
			continue;
		}

		short na = arg->Name();
		if (na == -1) {//this is not a variable... We compare the value with our kparam
			if (arg->same(params[i]) == kifFALSE)
				return kifRAISEERROR;
			continue;
		}

		keep(na, params[i]);
	}

	return KIFHASKELL(contextualpattern, dom, bd, idthread);
}

//-------------------------------------------------------------------------------------------------------------------------

Exported void KifCallFunctionHaskell::Set(short n, KifElement* ke, bin_hash<KifElement*>* stacks) {
	created.push_back(n);
	if (kifGlobals->Test() == false) {
		ke->Incrementreference();
		declarations[n] = ke;
		return;
	}
	KifContainerLock _lock(this);
	ke->Incrementreference();
	declarations[n] = ke;

}

void KifCallFunctionHaskell::keep(short n, KifElement* k) {
	declarations[n] = k;
	k->Setreference();
	created.push_back(n);
}
//-------------------------------------------------------------------------------------------------------------------------

KifElement* KifElement::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	KifElement* returnval;
	KifIteration* iter = (KifIteration*)Newiterator(false);
	KifIteration* itrloc;
	KifElement* key;
	KifElement* val;
	KifElement* kmap = kifNULL;

	char choice = 0;

	if (additer)
		choice = 1;
	else
	if (var->type == kifBaseVector)
		choice = 3;
	else
	if (var->type == kifBaseMap) {
		kmap = new KifMap(kifcode, NULL);
		kmap->popped = 1;
		choice = 2;
	}

	for (iter->Begin(); iter->End() == kifFALSE; iter->Next()) {
		switch (choice) {
		case 0:
			key = iter->IteratorValue();
			var->Setvalue(key);
			key->Release();
			break;
		case 1:
			((KifIterator*)var)->value = iter;
			break;
		case 2:
			key = iter->IteratorKey();
			val = iter->IteratorValue();
			kmap->Push(key, val);
			val->Release();
			key->Release();
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
			break;
		case 3:
			key = iter->IteratorValue();
			key->Match((KifBaseVector*)var, dom, idthread, clf);
			if (kifGlobals->THR[idthread]->error) {
				if (kcont != contextualpattern)
					kcont->Release();
				iter->Release();
				key->Release();
				return Errorptrid(idthread);
			}
			key->Release();
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (additer)
			((KifIterator*)var)->value = kifNULL;

		if (returnval->isBreak()) {
			iter->Release();
			kmap->Release();
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			if (kcont != contextualpattern)
				kcont->Release();
			iter->Release();
			kmap->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (kcont == kifNULL) {
				//then we have an issue here, we need to decide which structure to create...
				if (returnval->aMapContainer())
					kcont = new KifMap(kifcode, NULL);
				else
					kcont = new KifVector(kifcode, NULL);
			}

			if (kcont->aMapContainer() && returnval->aMapContainer()) {
				itrloc = (KifIteration*)returnval->Newiterator(false);
				for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
					key = itrloc->IteratorKey();
					val = itrloc->IteratorValue();
					kcont->Push(key, val);
					key->Release();
					val->Release();
				}
				itrloc->Release();
			}
			else
				kcont->Push(returnval);
			returnval->Release();
		}
	}
	iter->Release();

	kmap->Release();
	return kcont;
}

KifElement* KifReplicateVector::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	int mx = nb->Integer();
	for (int itx = 0; itx < mx; itx++)
		kcont->Push(value);
	Clear();
	return kcont;
}

KifElement* KifRawString::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	char testcontainer = 1;

	KifElement* key;
	KifElement* val;
	KifIteration* itrloc;

	KifElement* returnval;
	agnostring str(value);
	str.begin();
	KifBaseString kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;

	long counter = 0;

	KifElement* kmap = kifNULL;
	if (var->type == kifBaseMap) {
		kmap = new KifMapIntegerString(kifcode, NULL);
		kmap->popped = 1;
	}
	else
		var->Setvalue(&kstr);

	while (!str.end()) {
		kstr.value = str.next();
		if (var->type == kifBaseMap) {
			kmap->storekey(counter++, kstr.value);
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (returnval->isBreak()) {
			var->Setvalue(kifNULL);
			kmap->Release();
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			kmap->Release();
			var->Setvalue(kifNULL);
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (testcontainer == 1) {
				if (kcont == kifNULL) {
					//then we have an issue here, we need to decide which structure to create...
					if (returnval->aMapContainer()) {
						kcont = new KifMap(kifcode, NULL);
						testcontainer = 3;
					}
					else {
						if (returnval->isString()) {
							kcont = kifcode->Providestringraw("");
							testcontainer = 0;
						}
						else {
							kcont = new KifVector(kifcode, NULL);
							testcontainer = 2;
						}
					}
				}
				else {
					testcontainer = 2;
					if (returnval->aContainer()) {
						if (returnval->aMapContainer()) {
							if (!kcont->aMapContainer()) {
								kcont->Release();
								kcont = new KifMap(kifcode, NULL);
							}
							testcontainer = 3;
						}
						else {
							if (!kcont->aVector()) {
								kcont->Release();
								kcont = new KifVector(kifcode, NULL);
							}
						}
					}
					else
					if (kcont->type == kifString)
						testcontainer = 0;
					else
					if (kcont->aMapContainer())
						testcontainer = 3;
				}
			}

			switch (testcontainer) {
			case 0:
				((KifString*)kcont)->value += returnval->String();
				break;
			case 2:
				kcont->Push(returnval);
				break;
			case 3:
				if (kcont->aMapContainer()) {
					itrloc = (KifIteration*)returnval->Newiterator(false);
					for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
						key = itrloc->IteratorKey();
						val = itrloc->IteratorValue();
						kcont->Push(key, val);
						key->Release();
						val->Release();
					}
					itrloc->Release();
				}
			}
			returnval->Release();
		}
	}

	kmap->Release();
	var->Setvalue(kifNULL);
	return kcont;
}

KifElement* KifUString::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread,
	KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {

	char testcontainer = 1;

	KifElement* key;
	KifElement* val;
	KifIteration* itrloc;

	KifElement* returnval;
	KifBaseUString kstr(NULL, NULL, 0);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;

	long counter = 0;

	KifElement* kmap = kifNULL;
	if (var->type == kifBaseMap) {
		kmap = new KifMapIntegerString(kifcode, NULL);
		kmap->popped = 1;
	}
	else
		var->Setvalue(&kstr);

	for (int i = 0; i < value.size(); i++) {
		kstr.value[0] = value[i];
		if (var->type == kifBaseMap) {
			kmap->storekey(counter++, kstr.String());
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (returnval->isBreak()) {
			var->Setvalue(kifNULL);
			kmap->Release();
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			kmap->Release();
			var->Setvalue(kifNULL);
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (testcontainer == 1) {
				if (kcont == kifNULL) {
					//then we have an issue here, we need to decide which structure to create...
					if (returnval->aMapContainer()) {
						kcont = new KifMap(kifcode, NULL);
						testcontainer = 3;
					}
					else {
						if (returnval->isString()) {
							kcont = kifcode->Provideustring(L"");
							testcontainer = 0;
						}
						else {
							kcont = new KifVector(kifcode, NULL);
							testcontainer = 2;
						}
					}
				}
				else {
					testcontainer = 2;
					if (returnval->aContainer()) {
						if (returnval->aMapContainer()) {
							if (!kcont->aMapContainer()) {
								kcont->Release();
								kcont = new KifMap(kifcode, NULL);
							}
							testcontainer = 3;
						}
						else {
							if (!kcont->aVector()) {
								kcont->Release();
								kcont = new KifVector(kifcode, NULL);
							}
						}
					}
					else
					if (kcont->type == kifUString)
						testcontainer = 0;
					else
					if (kcont->aMapContainer())
						testcontainer = 3;
				}
			}

			switch (testcontainer) {
			case 0:
				((KifUString*)kcont)->value += returnval->UString();
				break;
			case 2:
				kcont->Push(returnval);
				break;
			case 3:
				if (kcont->aMapContainer()) {
					itrloc = (KifIteration*)returnval->Newiterator(false);
					for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
						key = itrloc->IteratorKey();
						val = itrloc->IteratorValue();
						kcont->Push(key, val);
						key->Release();
						val->Release();
					}
					itrloc->Release();
				}
			}
			returnval->Release();
		}
	}

	kmap->Release();
	var->Setvalue(kifNULL);
	return kcont;
}

KifElement* KifString::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	char testcontainer = 1;

	KifElement* key;
	KifElement* val;
	KifIteration* itrloc;

	KifElement* returnval;
	agnostring str(value);
	str.begin();
	KifBaseString kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;

	long counter = 0;

	KifElement* kmap = kifNULL;
	if (var->type == kifBaseMap) {
		kmap = new KifMapIntegerString(kifcode, NULL);
		kmap->popped = 1;
	}
	else
		var->Setvalue(&kstr);

	while (!str.end()) {
		kstr.value = str.next();
		if (var->type == kifBaseMap) {
			kmap->storekey(counter++, kstr.value);
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (returnval->isBreak()) {
			var->Setvalue(kifNULL);
			kmap->Release();
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			kmap->Release();
			var->Setvalue(kifNULL);
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (testcontainer == 1) {
				if (kcont == kifNULL) {
					//then we have an issue here, we need to decide which structure to create...
					if (returnval->aMapContainer()) {
						kcont = new KifMap(kifcode, NULL);
						testcontainer = 3;
					}
					else {
						if (returnval->isString()) {
							kcont = kifcode->Providestringraw("");
							testcontainer = 0;
						}
						else {
							kcont = new KifVector(kifcode, NULL);
							testcontainer = 2;
						}
					}
				}
				else {
					testcontainer = 2;
					if (returnval->aContainer()) {
						if (returnval->aMapContainer()) {
							if (!kcont->aMapContainer()) {
								kcont->Release();
								kcont = new KifMap(kifcode, NULL);
							}
							testcontainer = 3;
						}
						else {
							if (!kcont->aVector()) {
								kcont->Release();
								kcont = new KifVector(kifcode, NULL);
							}
						}
					}
					else
					if (kcont->type == kifString)
						testcontainer = 0;
					else
					if (kcont->aMapContainer())
						testcontainer = 3;
				}
			}

			switch (testcontainer) {
			case 0:
				((KifString*)kcont)->value += returnval->String();
				break;
			case 2:
				kcont->Push(returnval);
				break;
			case 3:
				if (kcont->aMapContainer()) {
					itrloc = (KifIteration*)returnval->Newiterator(false);
					for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
						key = itrloc->IteratorKey();
						val = itrloc->IteratorValue();
						kcont->Push(key, val);
						key->Release();
						val->Release();
					}
					itrloc->Release();
				}
			}
			returnval->Release();
		}
	}

	kmap->Release();
	var->Setvalue(kifNULL);
	return kcont;
}

KifElement* KifVector::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	KifElement* returnval;
	KifElement* key;
	KifElement* val;
	KifIteration* itrloc;

	KifElement* kmap = kifNULL;
	if (var->type == kifBaseMap) {
		kmap = new KifIntegerMap(kifcode, NULL);
		kmap->popped = 1;
	}

	for (int i = 0; i < values.size(); i++) {
		switch (var->type) {
		case kifBaseMap:
			((KifIntegerMap*)kmap)->push((long)i, values[i]);
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
			break;
		case  kifBaseVector:
			values[i]->Match((KifBaseVector*)var, dom, idthread, clf);
			if (kifGlobals->THR[idthread]->error) {
				if (kcont != contextualpattern)
					kcont->Release();
				return Errorptrid(idthread);
			}
			break;
		default:
			var->Setvalue(values[i]);
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (returnval->isBreak()) {
			if (var->type != kifBaseVector)
				var->Setvalue(kifNULL);
			kmap->Release();
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			kmap->Release();
			if (var->type != kifBaseVector)
				var->Setvalue(kifNULL);
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (kcont == kifNULL) {
				//then we have an issue here, we need to decide which structure to create...
				if (returnval->aMapContainer())
					kcont = new KifMap(kifcode, NULL);
				else
					kcont = new KifVector(kifcode, NULL);
			}

			if (kcont->aMapContainer() && returnval->aMapContainer()) {
				itrloc = (KifIteration*)returnval->Newiterator(false);
				for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
					key = itrloc->IteratorKey();
					val = itrloc->IteratorValue();
					kcont->Push(key, val);
					key->Release();
					val->Release();
				}
				itrloc->Release();
			}
			else
				kcont->Push(returnval);
			returnval->Release();
		}
	}

	kmap->Release();
	if (var->type != kifBaseVector)
		var->Setvalue(kifNULL);
	return kcont;
}

KifElement* KifEvaluateMap::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	KifElement* returnval;
	KifIteration* itr = (KifIteration*)Newiterator(false);
	KifIteration* itrloc;
	KifElement* key;
	KifElement* val;
	KifElement* kmap = kifNULL;
	if (var->type == kifBaseMap) {
		kmap = Newinstance();
		kmap->popped = 1;
	}

	for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
		key = itr->IteratorKey();

		if (var->type == kifBaseMap) {
			val = itr->IteratorValue();
			kmap->Push(key, val);
			key->Release();
			val->Release();
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
		}
		else {
			var->Setvalue(key);
			key->Release();
		}

		if (kifGlobals->THR[idthread]->error) {
			itr->Release();
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptrid(idthread);
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (returnval->isBreak()) {
			if (var->type != kifBaseMap)
				var->Setvalue(kifNULL);
			itr->Release();
			kmap->Release();
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			if (var->type != kifBaseMap)
				var->Setvalue(kifNULL);
			if (kcont != contextualpattern)
				kcont->Release();
			itr->Release();
			kmap->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (kcont == kifNULL) {
				//then we have an issue here, we need to decide which structure to create...
				if (returnval->aMapContainer())
					kcont = new KifMap(kifcode, NULL);
				else
					kcont = new KifVector(kifcode, NULL);
			}

			if (kcont->aMapContainer() && returnval->aMapContainer()) {
				itrloc = (KifIteration*)returnval->Newiterator(false);
				for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
					key = itrloc->IteratorKey();
					val = itrloc->IteratorValue();
					kcont->Push(key, val);
					key->Release();
					val->Release();
				}
				itrloc->Release();
			}
			else
				kcont->Push(returnval);
			returnval->Release();
		}
	}

	if (var->type != kifBaseMap)
		var->Setvalue(kifNULL);

	kmap->Release();
	itr->Release();
	return kcont;
}

KifElement* KifVectorUString::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	char testcontainer = 1;

	KifElement* key;
	KifElement* val;
	KifIteration* itrloc;

	KifElement* kmap = kifNULL;

	KifElement* returnval;
	KifBaseUString kstr(NULL, NULL, 0);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;

	if (var->type == kifBaseMap) {
		kmap = new KifMapIntegerString(kifcode, NULL);
		kmap->popped = 1;
	}
	else
		var->Setvalue(&kstr);

	string sconv;
	for (int i = 0; i < values.size(); i++) {
		kstr.value = values[i];
		if (var->type == kifBaseMap) {
			sc_unicode_to_utf8(sconv, kstr.value);
			kmap->storekey((long)i, sconv);
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (returnval->isBreak()) {
			kmap->Release();
			var->Setvalue(kifNULL);
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			kmap->Release();
			var->Setvalue(kifNULL);
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (testcontainer == 1) {
				if (kcont == kifNULL) {
					//then we have an issue here, we need to decide which structure to create...
					if (returnval->aMapContainer()) {
						kcont = new KifMap(kifcode, NULL);
						testcontainer = 3;
					}
					else {
						if (returnval->isString()) {
							kcont = new KifVectorUString(kifcode, NULL);
							testcontainer = 0;
						}
						else {
							kcont = new KifVector(kifcode, NULL);
							testcontainer = 2;
						}
					}
				}
				else {
					testcontainer = 2;
					if (returnval->aContainer()) {
						if (returnval->aMapContainer()) {
							if (!kcont->aMapContainer()) {
								kcont->Release();
								kcont = new KifMap(kifcode, NULL);
							}
							testcontainer = 3;
						}
						else {
							if (!kcont->aVector()) {
								kcont->Release();
								kcont = new KifVector(kifcode, NULL);
							}
						}
					}
					else {
						if (kcont->type == kifVectorUString)
							testcontainer = 0;
						else
						if (kcont->aMapContainer())
							testcontainer = 3;
					}
				}
			}

			switch (testcontainer) {
			case 0:
				((KifVectorUString*)kcont)->values.push_back(returnval->UString());
				break;
			case 2:
				kcont->Push(returnval);
				break;
			case 3:
				if (kcont->aMapContainer()) {
					itrloc = (KifIteration*)returnval->Newiterator(false);
					for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
						key = itrloc->IteratorKey();
						val = itrloc->IteratorValue();
						kcont->Push(key, val);
						key->Release();
						val->Release();
					}
					itrloc->Release();
				}
			}
			returnval->Release();
		}
	}

	kmap->Release();
	var->Setvalue(kifNULL);
	return kcont;
}

KifElement* KifVectorString::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	char testcontainer = 1;

	KifElement* key;
	KifElement* val;
	KifIteration* itrloc;

	KifElement* kmap = kifNULL;

	KifElement* returnval;
	KifBaseString kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;

	if (var->type == kifBaseMap) {
		kmap = new KifMapIntegerString(kifcode, NULL);
		kmap->popped = 1;
	}
	else
		var->Setvalue(&kstr);

	for (int i = 0; i < values.size(); i++) {
		kstr.value = values[i];
		if (var->type == kifBaseMap) {
			kmap->storekey((long)i, kstr.value);
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (returnval->isBreak()) {
			kmap->Release();
			var->Setvalue(kifNULL);
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			kmap->Release();
			var->Setvalue(kifNULL);
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (testcontainer == 1) {
				if (kcont == kifNULL) {
					//then we have an issue here, we need to decide which structure to create...
					if (returnval->aMapContainer()) {
						kcont = new KifMap(kifcode, NULL);
						testcontainer = 3;
					}
					else {
						if (returnval->isString()) {
							kcont = new KifVectorString(kifcode, NULL);
							testcontainer = 0;
						}
						else {
							kcont = new KifVector(kifcode, NULL);
							testcontainer = 2;
						}
					}
				}
				else {
					testcontainer = 2;
					if (returnval->aContainer()) {
						if (returnval->aMapContainer()) {
							if (!kcont->aMapContainer()) {
								kcont->Release();
								kcont = new KifMap(kifcode, NULL);
							}
							testcontainer = 3;
						}
						else {
							if (!kcont->aVector()) {
								kcont->Release();
								kcont = new KifVector(kifcode, NULL);
							}
						}
					}
					else {
						if (kcont->type == kifVectorString)
							testcontainer = 0;
						else
						if (kcont->aMapContainer())
							testcontainer = 3;
					}
				}
			}

			switch (testcontainer) {
			case 0:
				((KifVectorString*)kcont)->values.push_back(returnval->String());
				break;
			case 2:
				kcont->Push(returnval);
				break;
			case 3:
				if (kcont->aMapContainer()) {
					itrloc = (KifIteration*)returnval->Newiterator(false);
					for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
						key = itrloc->IteratorKey();
						val = itrloc->IteratorValue();
						kcont->Push(key, val);
						key->Release();
						val->Release();
					}
					itrloc->Release();
				}
			}
			returnval->Release();
		}
	}

	kmap->Release();
	var->Setvalue(kifNULL);
	return kcont;
}

KifElement* KifVectorByte::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	char testcontainer = 1;

	KifElement* key;
	KifElement* val;
	KifIteration* itrloc;
	KifElement* kmap = kifNULL;

	KifElement* returnval;
	KifByte kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;

	if (var->type == kifBaseMap) {
		kmap = new KifMapIntegerInteger(kifcode, NULL);
		kmap->popped = 1;
	}
	else
		var->Setvalue(&kstr);

	for (int i = 0; i < values.size(); i++) {
		kstr.value = values[i];
		if (var->type == kifBaseMap) {
			kmap->storekey((long)i, (long)kstr.value);
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (returnval->isBreak()) {
			kmap->Release();
			var->Setvalue(kifNULL);
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			kmap->Release();
			var->Setvalue(kifNULL);
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (testcontainer == 1) {
				if (kcont == kifNULL) {
					//then we have an issue here, we need to decide which structure to create...
					if (returnval->aMapContainer()) {
						kcont = new KifMap(kifcode, NULL);
						testcontainer = 3;
					}
					else {
						if (returnval->aNumber()) {
							kcont = new KifVectorByte(kifcode, NULL);
							testcontainer = 0;
						}
						else {
							kcont = new KifVector(kifcode, NULL);
							testcontainer = 2;
						}
					}
				}
				else {
					testcontainer = 2;
					if (returnval->aContainer()) {
						if (returnval->aMapContainer()) {
							if (!kcont->aMapContainer()) {
								kcont->Release();
								kcont = new KifMap(kifcode, NULL);
							}
							testcontainer = 3;
						}
						else {
							if (!kcont->aVector()) {
								kcont->Release();
								kcont = new KifVector(kifcode, NULL);
							}
						}
					}
					else {
						if (kcont->type == kifVectorByte)
							testcontainer = 0;
						else
						if (kcont->aMapContainer())
							testcontainer = 3;
					}
				}
			}

			switch (testcontainer) {
			case 0:
				((KifVectorByte*)kcont)->values.push_back(returnval->Byte());
				break;
			case 2:
				kcont->Push(returnval);
				break;
			case 3:
				if (kcont->aMapContainer()) {
					itrloc = (KifIteration*)returnval->Newiterator(false);
					for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
						key = itrloc->IteratorKey();
						val = itrloc->IteratorValue();
						kcont->Push(key, val);
						key->Release();
						val->Release();
					}
					itrloc->Release();
				}
			}
			returnval->Release();
		}
	}

	kmap->Release();
	var->Setvalue(kifNULL);
	return kcont;
}

KifElement* KifVectorInteger::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	char testcontainer = 1;

	KifElement* key;
	KifElement* val;
	KifIteration* itrloc;
	KifElement* kmap = kifNULL;

	KifElement* returnval;
	KifBaseInteger kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;

	if (var->type == kifBaseMap) {
		kmap = new KifMapIntegerInteger(kifcode, NULL);
		kmap->popped = 1;
	}
	else
		var->Setvalue(&kstr);

	for (int i = 0; i < values.size(); i++) {
		kstr.value = values[i];
		if (var->type == kifBaseMap) {
			kmap->storekey((long)i, kstr.value);
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (returnval->isBreak()) {
			kmap->Release();
			var->Setvalue(kifNULL);
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			kmap->Release();
			var->Setvalue(kifNULL);
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (testcontainer == 1) {
				if (kcont == kifNULL) {
					//then we have an issue here, we need to decide which structure to create...
					if (returnval->aMapContainer()) {
						kcont = new KifMap(kifcode, NULL);
						testcontainer = 3;
					}
					else {
						if (returnval->aNumber()) {
							kcont = new KifVectorInteger(kifcode, NULL);
							testcontainer = 0;
						}
						else {
							kcont = new KifVector(kifcode, NULL);
							testcontainer = 2;
						}
					}
				}
				else {
					testcontainer = 2;
					if (returnval->aContainer()) {
						if (returnval->aMapContainer()) {
							if (!kcont->aMapContainer()) {
								kcont->Release();
								kcont = new KifMap(kifcode, NULL);
							}
							testcontainer = 3;
						}
						else {
							if (!kcont->aVector()) {
								kcont->Release();
								kcont = new KifVector(kifcode, NULL);
							}
						}
					}
					else {
						if (kcont->type == kifVectorInteger)
							testcontainer = 0;
						else
						if (kcont->aMapContainer())
							testcontainer = 3;
					}
				}
			}

			switch (testcontainer) {
			case 0:
				((KifVectorInteger*)kcont)->values.push_back(returnval->Integer());
				break;
			case 2:
				kcont->Push(returnval);
				break;
			case 3:
				if (kcont->aMapContainer()) {
					itrloc = (KifIteration*)returnval->Newiterator(false);
					for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
						key = itrloc->IteratorKey();
						val = itrloc->IteratorValue();
						kcont->Push(key, val);
						key->Release();
						val->Release();
					}
					itrloc->Release();
				}
			}
			returnval->Release();
		}
	}

	kmap->Release();
	var->Setvalue(kifNULL);
	return kcont;
}

KifElement* KifVectorFloat::Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer)  {
	char testcontainer = 1;

	KifElement* key;
	KifElement* val;
	KifIteration* itrloc;

	KifElement* kmap = kifNULL;

	KifElement* returnval;
	KifBaseFloat kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;

	if (var->type == kifBaseMap) {
		kmap = new KifMapIntegerFloat(kifcode, NULL);
		kmap->popped = 1;
	}
	else
		var->Setvalue(&kstr);

	for (int i = 0; i < values.size(); i++) {
		kstr.value = values[i];
		if (var->type == kifBaseMap) {
			kmap->storekey((long)i, kstr.value);
			kmap->Match((KifBaseMap*)var, dom, idthread, clf);
			kmap->Clear();
		}

		if (bd->choice == SINGLEINSTRUCTION) {
			returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
			if (returnval->Returned())
				returnval = returnval->Returnvalue();
		}
		else
			returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

		if (returnval->isBreak()) {
			kmap->Release();
			var->Setvalue(kifNULL);
			return kcont;
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			kmap->Release();
			var->Setvalue(kifNULL);
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptrid(idthread);
		}

		if (returnval != kifNULL) {
			if (testcontainer == 1) {
				if (kcont == kifNULL) {
					//then we have an issue here, we need to decide which structure to create...
					if (returnval->aMapContainer()) {
						kcont = new KifMap(kifcode, NULL);
						testcontainer = 3;
					}
					else {
						if (returnval->aNumber()) {
							kcont = new KifVectorFloat(kifcode, NULL);
							testcontainer = 0;
						}
						else {
							kcont = new KifVector(kifcode, NULL);
							testcontainer = 2;
						}
					}
				}
				else {
					testcontainer = 2;
					if (returnval->aContainer()) {
						if (returnval->aMapContainer()) {
							if (!kcont->aMapContainer()) {
								kcont->Release();
								kcont = new KifMap(kifcode, NULL);
							}
							testcontainer = 3;
						}
						else {
							if (!kcont->aVector()) {
								kcont->Release();
								kcont = new KifVector(kifcode, NULL);
							}
						}
					}
					else {
						if (kcont->type == kifVectorFloat)
							testcontainer = 0;
						else
						if (kcont->aMapContainer())
							testcontainer = 3;
					}
				}
			}

			switch (testcontainer) {
			case 0:
				((KifVectorFloat*)kcont)->values.push_back(returnval->Float());
				break;
			case 2:
				kcont->Push(returnval);
				break;
			case 3:
				if (kcont->aMapContainer()) {
					itrloc = (KifIteration*)returnval->Newiterator(false);
					for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
						key = itrloc->IteratorKey();
						val = itrloc->IteratorValue();
						kcont->Push(key, val);
						key->Release();
						val->Release();
					}
					itrloc->Release();
				}
			}
			returnval->Release();
		}
	}

	kmap->Release();
	var->Setvalue(kifNULL);
	return kcont;
}

//-------------------------------------------------------------------------------------------------------------------------

KifElement* KifElement::FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool direct) {
	KifElement* returnval;
	KifIteration* iter = (KifIteration*)Newiterator(direct);
	bool first = false;
	KifElement* key;

	if (init != kifNOELEMENT) {
		accu->Setvalue(init);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else
				kcont->Push(init);
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1

	for (iter->Begin(); iter->End() == kifFALSE; iter->Next()) {
		key = iter->IteratorValue();
		if (first) {
			returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			var->Setvalue(key);
			key->Release();
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

			if (returnval->isBreak()) {
				accu = returnval;
				break;
			}

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != NULL)
					kcont->Release();
				return Errorptrid(idthread);
			}
		}

		if (returnval != kifNULL) {
			accu->Setvalue(returnval);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, returnval);
				else
					kcont->Push(returnval);
			}
			returnval->Release();
		}
	}
	iter->Release();
	if (kcont != NULL)
		return kcont;
	return accu->Returnvalue();
}

KifElement* KifUString::FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf,
	KifFunctionLambda* bd, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool direct) {

	KifElement* returnval;

	KifBaseUString kstr(NULL, NULL, 0);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;
	var->Setvalue(&kstr);
	bool first = false;
	if (init != kifNOELEMENT) {
		accu->Setvalue(init);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else {
				if (kcont->aString())
					kcont->plus(kcont, init, kcont, true);
				else
					kcont->Push(init);
			}
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1

	bool loop = true;
	int i = 0;
	if (direct)
		i = value.size() - 1;

	while (loop) {
		if (direct)
			kstr.value = value[i--];
		else
			kstr.value = value[i++];

		if (first) {
			returnval = kstr.Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

			if (returnval->isBreak()) {
				accu = returnval;
				break;
			}

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != NULL)
					kcont->Release();
				var->Setvalue(kifNULL);
				return Errorptrid(idthread);
			}
		}

		if (returnval != kifNULL) {
			accu->Setvalue(returnval);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, init);
				else {
					if (kcont->aString())
						((KifString*)kcont)->value += returnval->String();
					else
						kcont->Push(returnval);
				}
			}
			returnval->Release();
		}

		if (direct) {
			if (i < 0)
				break;
		}
		else {
			if (i >= value.size())
				break;
		}
	}
	var->Setvalue(kifNULL);
	if (kcont != NULL)
		return kcont;
	return accu->Returnvalue();
}

KifElement* KifRawString::FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool direct) {

	KifElement* returnval;
	agnostring str(value);
	str.begin();
	KifBaseString kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;
	var->Setvalue(&kstr);
	bool first = false;
	if (init != kifNOELEMENT) {
		accu->Setvalue(init);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else {
				if (kcont->aString())
					kcont->plus(kcont, init, kcont, true);
				else
					kcont->Push(init);
			}
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1

	bool loop = true;
	vector<string>* vstr = NULL;
	int i = 0;
	if (direct) {
		vstr = new vector<string>;
		while (!str.end())
			vstr->push_back(str.next());
		i = vstr->size() - 1;
	}

	while (loop) {
		if (direct)
			kstr.value = vstr->at(i--);
		else
			kstr.value = str.next();

		if (first) {
			returnval = kstr.Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

			if (returnval->isBreak()) {
				accu = returnval;
				break;
			}

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != NULL)
					kcont->Release();
				if (direct)
					delete vstr;
				var->Setvalue(kifNULL);
				return Errorptrid(idthread);
			}
		}

		if (returnval != kifNULL) {
			accu->Setvalue(returnval);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, init);
				else {
					if (kcont->aString())
						((KifString*)kcont)->value += returnval->String();
					else
						kcont->Push(returnval);
				}
			}
			returnval->Release();
		}

		if (direct) {
			if (i < 0)
				break;
		}
		else {
			if (!str.end())
				break;
		}
	}
	var->Setvalue(kifNULL);
	if (direct)
		delete vstr;
	if (kcont != NULL)
		return kcont;
	return accu->Returnvalue();
}

KifElement* KifString::FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool direct) {

	KifElement* returnval;
	agnostring str(value);
	str.begin();
	KifBaseString kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;
	var->Setvalue(&kstr);
	bool first = false;
	if (init != kifNOELEMENT) {
		accu->Setvalue(init);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else {
				if (kcont->aString())
					((KifString*)kcont)->value += init->String();
				else
					kcont->Push(init);
			}
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1

	bool loop = true;
	vector<string>* vstr = NULL;
	int i = 0;
	if (direct) {
		vstr = new vector<string>;
		while (!str.end())
			vstr->push_back(str.next());
		i = vstr->size() - 1;
	}

	while (loop) {
		if (direct)
			kstr.value = vstr->at(i--);
		else
			kstr.value = str.next();

		if (first) {
			returnval = kstr.Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

			if (returnval->isBreak()) {
				accu = returnval;
				break;
			}

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != NULL)
					kcont->Release();
				if (direct)
					delete vstr;
				var->Setvalue(kifNULL);
				return Errorptrid(idthread);
			}
		}

		if (returnval != kifNULL) {
			accu->Setvalue(returnval);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, init);
				else {
					if (kcont->aString())
						((KifString*)kcont)->value += returnval->String();
					else
						kcont->Push(returnval);
				}
			}
			returnval->Release();
		}

		if (direct) {
			if (i < 0)
				break;
		}
		else {
			if (!str.end())
				break;
		}
	}
	var->Setvalue(kifNULL);
	if (direct)
		delete vstr;
	if (kcont != NULL)
		return kcont;
	return accu->Returnvalue();
}

KifElement* KifVector::FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool direct) {
	KifElement* returnval;
	bool first = false;
	if (init != kifNOELEMENT) {
		accu->Setvalue(init);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else
				kcont->Push(init);
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1
	KifElement* key;
	int i;
	if (direct)
		i = values.size() - 1;
	else
		i = 0;
	bool loop = true;
	while (loop) {
		key = values[i];
		if (first) {
			returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			var->Setvalue(key);
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

			if (returnval->isBreak()) {
				accu = returnval;
				break;
			}

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != NULL)
					kcont->Release();
				return Errorptrid(idthread);
			}
		}

		if (returnval != kifNULL) {
			accu->Setvalue(returnval);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, returnval);
				else
					kcont->Push(returnval);
			}
			returnval->Release();
		}

		if (direct) {
			if (--i < 0)
				break;
		}
		else {
			if (++i >= values.size())
				break;
		}
	}
	if (kcont != NULL)
		return kcont;
	return accu->Returnvalue();
}

KifElement* KifVectorString::FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool direct) {

	KifElement* returnval;
	bool first = false;
	if (init != kifNOELEMENT) {
		accu->Setvalue(init);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else
				kcont->Push(init);
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1
	bool loop = true;
	int i = 0;
	if (direct)
		i = values.size() - 1;
	KifBaseString kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;
	var->Setvalue(&kstr);

	while (loop) {
		kstr.value = values[i];
		if (first) {
			returnval = kstr.Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

			if (returnval->isBreak()) {
				accu = returnval;
				break;
			}

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != NULL)
					kcont->Release();
				var->Setvalue(kifNULL);
				return Errorptrid(idthread);
			}
		}

		if (returnval != kifNULL) {
			accu->Setvalue(returnval);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, returnval);
				else
					kcont->Push(returnval);
			}
			returnval->Release();
		}

		if (direct) {
			if (--i < 0)
				break;
		}
		else {
			if (++i >= values.size())
				break;
		}
	}
	var->Setvalue(kifNULL);
	if (kcont != NULL)
		return kcont;
	return accu->Returnvalue();
}

KifElement* KifVectorUString::FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool direct) {

	KifElement* returnval;
	bool first = false;
	if (init != kifNOELEMENT) {
		accu->Setvalue(init);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else
				kcont->Push(init);
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1
	bool loop = true;
	int i = 0;
	if (direct)
		i = values.size() - 1;
	KifBaseUString kstr(NULL, NULL, 0);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;
	var->Setvalue(&kstr);

	while (loop) {
		kstr.value = values[i];
		if (first) {
			returnval = kstr.Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

			if (returnval->isBreak()) {
				accu = returnval;
				break;
			}

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != NULL)
					kcont->Release();
				var->Setvalue(kifNULL);
				return Errorptrid(idthread);
			}
		}

		if (returnval != kifNULL) {
			accu->Setvalue(returnval);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, returnval);
				else
					kcont->Push(returnval);
			}
			returnval->Release();
		}

		if (direct) {
			if (--i < 0)
				break;
		}
		else {
			if (++i >= values.size())
				break;
		}
	}
	var->Setvalue(kifNULL);
	if (kcont != NULL)
		return kcont;
	return accu->Returnvalue();
}

KifElement* KifVectorByte::FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool direct) {
	KifElement* returnval;
	bool first = false;
	if (init != kifNOELEMENT) {
		accu->Setvalue(init);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else
				kcont->Push(init);
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1
	bool loop = true;
	int i = 0;
	if (direct)
		i = values.size() - 1;
	KifByte kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;
	var->Setvalue(&kstr);

	while (loop) {
		kstr.value = values[i];
		if (first) {
			returnval = kstr.Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

			if (returnval->isBreak()) {
				accu = returnval;
				break;
			}

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != NULL)
					kcont->Release();
				var->Setvalue(kifNULL);
				return Errorptrid(idthread);
			}
		}

		if (returnval != kifNULL) {
			accu->Setvalue(returnval);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, returnval);
				else
					kcont->Push(returnval);
			}
			returnval->Release();
		}

		if (direct) {
			if (--i < 0)
				break;
		}
		else {
			if (++i >= values.size())
				break;
		}
	}

	var->Setvalue(kifNULL);
	if (kcont != NULL)
		return kcont;
	return accu->Returnvalue();
}

KifElement* KifVectorInteger::FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool direct) {
	KifElement* returnval;
	bool first = false;
	if (init != kifNOELEMENT) {
		accu->Setvalue(init);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else
				kcont->Push(init);
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1
	bool loop = true;
	int i = 0;
	if (direct)
		i = values.size() - 1;
	KifBaseInteger kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;
	var->Setvalue(&kstr);

	while (loop) {
		kstr.value = values[i];
		if (first) {
			returnval = kstr.Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

			if (returnval->isBreak()) {
				accu = returnval;
				break;
			}

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != NULL)
					kcont->Release();
				var->Setvalue(kifNULL);
				return Errorptrid(idthread);
			}
		}

		if (returnval != kifNULL) {
			accu->Setvalue(returnval);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, returnval);
				else
					kcont->Push(returnval);
			}
			returnval->Release();
		}

		if (direct) {
			if (--i < 0)
				break;
		}
		else {
			if (++i >= values.size())
				break;
		}
	}

	var->Setvalue(kifNULL);
	if (kcont != NULL)
		return kcont;
	return accu->Returnvalue();
}

KifElement* KifVectorFloat::FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool direct) {
	KifElement* returnval;
	bool first = false;
	if (init != kifNOELEMENT) {
		accu->Setvalue(init);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else
				kcont->Push(init);
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1
	bool loop = true;
	int i = 0;
	if (direct)
		i = values.size() - 1;
	KifBaseFloat kstr(NULL, NULL);
	kstr.reference = 10000;
	kstr.kifcode = kifcode;
	var->Setvalue(&kstr);

	while (loop) {
		kstr.value = values[i];
		if (first) {
			returnval = kstr.Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, clf);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, clf);

			if (returnval->isBreak()) {
				accu = returnval;
				break;
			}

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != NULL)
					kcont->Release();
				var->Setvalue(kifNULL);
				return Errorptrid(idthread);
			}
		}

		if (returnval != kifNULL) {
			accu->Setvalue(returnval);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, returnval);
				else
					kcont->Push(returnval);
			}
			returnval->Release();
		}

		if (direct) {
			if (--i < 0)
				break;
		}
		else {
			if (++i >= values.size())
				break;
		}
	}
	var->Setvalue(kifNULL);
	if (kcont != NULL)
		return kcont;
	return accu->Returnvalue();
}

KifElement* KifCallFunctionHaskell::KIFHASKELL0(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda* bd, int idthread) {
	KifDomainLambda* ldomain = bd->lambdadomain;

	if (ldomain->local) {
		bin_hash<KifElement*>::iterator it;
		for (it = ldomain->declarations.begin(); it != ldomain->declarations.end(); it++) {
			if (it->second->Frame() == kifMINUSONE)
				//This is a where variable...
				it->second->Create(this, kifNULL, idthread, this);
		}
	}

	KifElement* returnval;

	//When sz==0, it means that we are evaluating a simple function without any reference to a boolean expression or a vector
	if (bd->choice == SINGLEINSTRUCTION) {
		returnval = bd->instructions[0]->Exec(contextualpattern, dom, kifNULL, idthread, this);
		if (returnval->Returned())
			returnval = returnval->Returnvalue();
	}
	else
		returnval = bd->Exec(contextualpattern, dom, kifNULL, idthread, this);

	if (returnval == kifNULL)
		return kifRAISEERROR;
	return returnval;
}


KifElement* KifCallFunctionHaskell::KIFHASKELL2(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda* bd, int idthread) {
	KifDomainLambda* ldomain = bd->lambdadomain;

	if (ldomain->local) {
		bin_hash<KifElement*>::iterator it;
		for (it = ldomain->declarations.begin(); it != ldomain->declarations.end(); it++) {
			if (it->second->Frame() == kifMINUSONE)
				//This is a where variable...
				it->second->Create(this, kifNULL, idthread, this);
		}
	}

	applyEnvShort aes;

	aes.init = kifTRUE;

	aes.klist = ldomain->instructions[1]->Exec(kifNULL, dom, kifNULL, idthread, this);
	if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error)
		return kifNULL;

	if (aes.klist->Size() == 0 && !aes.klist->Infinite())
		return kifTRUE;

	aes.var = ldomain->instructions[0]->Create(this, kifNULL, idthread, this);

	if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error)
		return Errorptrid(idthread);
	if (ldomain->name == 1) //name==1 is a AND
		return aes.klist->FilterHaskell(contextualpattern, dom, idthread, this, bd, aes.var, NULL, kifTRUE, kifNULL, false);
	//else it is a OR...
	return aes.klist->FilterHaskell(contextualpattern, dom, idthread, this, bd, aes.var, NULL, kifFALSE, kifNULL, false);
}

KifElement* KifCallFunctionHaskell::KIFHASKELL3(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda* bd, int idthread) {
	KifDomainLambda* ldomain = bd->lambdadomain;
	if (ldomain->local) {
		bin_hash<KifElement*>::iterator it;
		for (it = ldomain->declarations.begin(); it != ldomain->declarations.end(); it++) {
			if (it->second->Frame() == kifMINUSONE)
				//This is a where variable...
				it->second->Create(this, kifNULL, idthread, this);
		}
	}

	KifElement* kcont = kifNULL;

	applyEnvShort aes;

	aes.klist = ldomain->instructions[1]->Exec(kifNULL, dom, kifNULL, idthread, this);

	if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error)
		return kifNULL;

	if ((contextualpattern->aString() || contextualpattern->aContainer()) && contextualpattern != aes.klist) {
		kcont = contextualpattern;
		kcont->Clear();
	}

	if (aes.klist->Size() == 0 && !aes.klist->Infinite())
		return kcont;

	if (ldomain->instructions[0]->type == kifBaseVector) {
		if (aes.klist->aValueContainer())
			return kifcode->Returnerror(KIFLIBSTRINGS[269]);
		aes.var = ldomain->instructions[0];
	}
	else {
		if (ldomain->instructions[0]->type == kifBaseMap) {
			//if (!aes.klist->aMapContainer() && !aes.klist->isfile())
			//	return kifcode->Returnerror(KIFLIBSTRINGS[271]);
			aes.var = ldomain->instructions[0];
		}
		else
			aes.var = ldomain->instructions[0]->Create(this, kifNULL, idthread, this);
	}

	if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error)
		return Errorptrid(idthread);

	bool additer = false;
	if (aes.var->VariableValue()->type == kifIterator) {
		additer = true;
		aes.var = aes.var->VariableValue();
		return aes.klist->KifElement::Haskell(contextualpattern, dom, idthread, this, bd, aes.var, kcont, additer);
	}

	return aes.klist->Haskell(contextualpattern, dom, idthread, this, bd, aes.var, kcont, additer);
}

KifElement* KifCallFunctionHaskell::KIFHASKELL5(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda* bd, int idthread) {
	KifDomainLambda* ldomain = bd->lambdadomain;

	if (ldomain->local) {
		bin_hash<KifElement*>::iterator it;
		for (it = ldomain->declarations.begin(); it != ldomain->declarations.end(); it++) {
			if (it->second->Frame() == kifMINUSONE)
				//This is a where variable...
				it->second->Create(this, kifNULL, idthread, this);
		}
	}

	KifElement* kcont = NULL;

	KifElement* key;
	bool testcontainer = false;

	applyEnvShort aes;

	aes.init = ldomain->instructions[3]->Exec(kifNULL, this, kifNULL, idthread, this);

	aes.klist = ldomain->instructions[1]->Exec(kifNULL, dom, kifNULL, idthread, this);
	if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error)
		return kifNULL;

	if (aes.klist->Size() == 0 && !aes.klist->Infinite()) {
		kcont = aes.init;
		aes.init = NULL;
		return kcont;
	}

	aes.var = ldomain->instructions[0]->Create(this, kifNULL, idthread, this);
	KifElement* accu = ldomain->instructions[2]->Create(this, kifNULL, idthread, this);

	if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error)
		return Errorptrid(idthread);

	key = ldomain->instructions[4];
	bool direct = key->Boolean();

	if (key == kifZERO || key == kifONE) // this is a scan
		kcont = aes.klist->Newinstance();

	if (kcont != NULL) {
		testcontainer = kcont->aValueContainer();
		if (!kcont->aContainer() && !kcont->aString()) {
			kcont->Release();
			kcont = new KifVector(kifcode, NULL);
		}
		aes.klist->FilterHaskell(contextualpattern, dom, idthread, this, bd, aes.var, kcont, accu, aes.init, direct);
		return kcont;
	}

	aes.ret = aes.klist->FilterHaskell(contextualpattern, dom, idthread, this, bd, aes.var, kcont, accu, aes.init, direct);
	return aes.ret;
}

KifElement* KifCallFunctionHaskell::KIFHASKELLN(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda* bd, int idthread) {
	KifDomainLambda* ldomain = bd->lambdadomain;

	if (ldomain->local) {
		bin_hash<KifElement*>::iterator it;
		for (it = ldomain->declarations.begin(); it != ldomain->declarations.end(); it++) {
			if (it->second->Frame() == kifMINUSONE)
				//This is a where variable...
				it->second->Create(this, kifNULL, idthread, this);
		}
	}

	KifElement* kcont = NULL;

	KifElement* returnval;

	KifElement* key;
	KifElement* val;
	bool testcontainer = false;


	bool skip = false;

	applyEnv a;

	//The frame contains:
	// i: an iterator variable, which will be assigned with the current iterator value
	// i+1: the container
	// i+2: true if the container depends on an iterator variable, and in that case, this variable should be reset during the process. false, one single value is enough
	itemEnv* lastev;

	for (int i = 0; i < haskelltype; i += 3) {
		itemEnv* ev = new itemEnv(i, contextualpattern, dom, idthread, this);

		if (ev->resetvalue(ldomain->instructions[i + 2]))
			lastev->next = ev;
		else
			a.items.push_back(ev);
		lastev = ev;

		KifElement* klist = ldomain->instructions[i + 1]->Exec(kifNULL, dom, kifNULL, idthread, this);
		if (ldomain->instructions[i]->type == kifBaseVector) {
			if (klist->aValueContainer())
				return kifcode->Returnerror(KIFLIBSTRINGS[269]);
			ev->setvariable(ldomain->instructions[i]);
		}
		else {
			if (ldomain->instructions[i]->type == kifBaseMap) {
				//if (!klist->aMapContainer() && !klist->isfile()) {
				//	if (kcont != NULL && kcont != contextualpattern)
				//		kcont->Release();
				//	return kifcode->Returnerror(KIFLIBSTRINGS[271]);
				//}
				ev->setvariable(ldomain->instructions[i]);
			}
			else
				ev->setvariable(ldomain->instructions[i]->Create(this, kifNULL, idthread, this));
		}

		if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
			if (!klist->aContainer() && !klist->aString())
				return kifNULL;
			return Errorptrid(idthread);
		}

		ev->cleaning = klist;

		//We create a recipient variable based on the type either of the recipient variable (contextualpattern)
		//on the type of this first element.
		if (!i) {
			if ((contextualpattern->aString() || contextualpattern->aContainer()) && contextualpattern != klist)
				kcont = contextualpattern;
			else
				kcont = kifNULL;
		}
		else {
			if (kcont == klist)
				kcont = kifNULL;
		}

		//All our iterators are created and set on first element...
		ev->iter = (KifIteration*)klist->Newiterator(false);
		ev->iter->kifcode = kifcode;
		if (ev->Begin())
			ev->set(true);
		else {
			skip = true;
			if (!i)
				a.stop = true;
		}
	}

	if (kcont->aContainer() != 0)
		kcont->Clear();

	int current = a.items.size() - 1;

	testcontainer = kcont->aValueContainer();
	KifIteration* itrloc;
	while (!a.stop) {
		if (!skip) {
			if (bd->choice == SINGLEINSTRUCTION) {
				returnval = bd->instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, this);
				if (returnval->Returned())
					returnval = returnval->Returnvalue();
			}
			else
				returnval = bd->Exec(kifNULL, dom, kifNULL, idthread, this);

			if (returnval->isBreak())
				break;

			if (kifGlobals->executionbreak || kifGlobals->THR[idthread]->error) {
				if (kcont != contextualpattern)
					kcont->Release();
				kcont = Errorptrid(idthread);
				break;
			}

			if (returnval != kifNULL) {
				if (kcont == kifNULL) {
					//then we have an issue here, we need to decide which structure to create...
					if (returnval->aMapContainer())
						kcont = new KifMap(kifcode, NULL);
					else
						kcont = new KifVector(kifcode, NULL);
					testcontainer = false;
				}

				if (testcontainer) {
					if (returnval->aContainer()) {
						if (returnval->aMapContainer()) {
							if (!kcont->aMapContainer()) {
								kcont->Release();
								kcont = new KifMap(kifcode, NULL);
							}
						}
						else {
							if (!kcont->aVector()) {
								kcont->Release();
								kcont = new KifVector(kifcode, NULL);
							}
						}
					}
					testcontainer = false;
				}

				if (kcont->aMapContainer()) {
					itrloc = (KifIteration*)returnval->Newiterator(false);
					for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
						key = itrloc->IteratorKey();
						val = itrloc->IteratorValue();
						kcont->Push(key, val);
						key->Release();
						val->Release();
					}
					itrloc->Release();
				}
				else
					kcont->Push(returnval);
				returnval->Release();
			}
		}
		else
			skip = false;

		//We traverse the deepest iterators
		a.next(current);
		if (a.end(current)) {
			//if we have reached the end
			current--;
			while (current >= 0) {
				//then we also move the previous iterator and so on...
				a.next(current);
				if (!a.end(current)) {
					a.set(current);
					current++;
					break;
				}
				current--;
			}
			if (current >= 0)
				skip = a.evaluate(current, ldomain, this);
		}
		else
			a.set(current);
	}
	return kcont;
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------

KifElement* KifInstance::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;

	if (newinstance == kinKnown)
		return current_value->Returnvalue();


	KifElement* v = NULL;
	bool keep = false;
	if (current_value == NULL) {
		switch (newinstance) {
		case kinFunction:
			if (callfunc == NULL)
				v = kifGlobals->Topid(idthread)->Declared(name);
			else
				v = callfunc->Declared(name);
			break;
		case kinFrame:
			v = dom->Declared(name);
			if (v == NULL) {
				dom = kifGlobals->Beforetopid(idthread);
				v = dom->Declared(name);
			}
			break;
		case kinGlobal:
			v = kifcode->kstart->Declared(name);
			current_value = v;
			keep = true;
			break;
		case kinConstant:
			v = kifGlobals->kconstants->Declared(name);
			current_value = v;
			keep = true;
			break;
		default:
			//An External Global variable (from XIP)
			if (declaration->type == kifVariableDeclaration) {
				if (index != NULL) {
					if (declaration->value->Returnfunction() == NULL) {
						if (index->function == NULL)
							return declaration->value->Returnvalue()->Exec(contextualpattern, dom, index, idthread, callfunc);

						if (index->function->type != kifIndex) {
							v = declaration->value->Returnvalue()->Exec(contextualpattern, dom, index, idthread, callfunc);

							if (v->type == kifDomain)
								return index->function->Exec(contextualpattern, (KifDomain*)v, v, idthread, callfunc);

							return index->function->Exec(contextualpattern, dom, v, idthread, callfunc);
						}
					}

					KifCouple couple(declaration->value->Returnvalue(), index, dom, declaration->value->Returnfunction());
					return couple.Exec(contextualpattern, dom, index, idthread, callfunc);
				}
				return declaration->value->Returnvalue();
			}
			KifDomain* stackdom = kifGlobals->Beforetopid(idthread);

			if (dom != stackdom)
				v = dom->Declared(name);

			if (v == NULL) {
				KifDomain* func = kifGlobals->Top();
				if (func != NULL)
					v = func->Declared(name);
				if (v == NULL) {
					if (stackdom != NULL)
						v = stackdom->Declared(name);
					if (v == NULL && dom != kifcode->kstart) {
						v = kifcode->kstart->Declared(name);
						keep = true;
						current_value = v;
					}
					else
					if (stackdom == kifcode->kstart) {
						current_value = v;
						keep = true;
					}
					else
						newinstance = kinFrame;
				}
				else
					newinstance = kinFunction;
			}
			else
			if (dom == kifcode->kstart) {
				current_value = v;
				keep = true;
			}
			else
				newinstance = kinFrame;
		}

		if (v == NULL) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		if (v->Localvariable()) {
			current_value = NULL;
			keep = false;
		}
	}
	else
		v = current_value;


	if (value->isInstance()) {
		v = v->Returnvalue();
		if (v->isDomain() == false) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		return value->Exec(contextualpattern, (KifDomain*)v, val, idthread, callfunc);
	}


	if (index != NULL) {
		if (v->Returnfunction() == NULL) {
			if (index->function == NULL)
				return v->Returnvalue()->Exec(contextualpattern, dom, index, idthread, callfunc);

			if (index->function->type != kifIndex) {
				v = v->Returnvalue()->Exec(contextualpattern, dom, index, idthread, callfunc);

				if (v->type == kifDomain)
					return index->function->Exec(contextualpattern, (KifDomain*)v, v, idthread, callfunc);

				return index->function->Exec(contextualpattern, dom, v, idthread, callfunc);
			}
		}

		KifCouple couple(v->Returnvalue(), index, dom, v->Returnfunction());
		return couple.Exec(contextualpattern, dom, index, idthread, callfunc);
	}

	if (keep == true)
		newinstance = kinKnown;

	return v->Returnvalue();
}


KifElement* KifInstance::Thevariable(KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;

	if (newinstance == kinKnown)
		return current_value;


	KifElement* v = NULL;
	bool keep = false;
	if (current_value == NULL) {
		switch (newinstance) {
		case kinFunction:
			if (callfunc == NULL)
				v = kifGlobals->Topid(idthread)->Declared(name);
			else
				v = callfunc->Declared(name);
			break;
		case kinFrame:
			v = dom->Declared(name);
			if (v == NULL) {
				dom = kifGlobals->Beforetopid(idthread);
				v = dom->Declared(name);
			}
			break;
		case kinGlobal:
			v = kifcode->kstart->Declared(name);
			current_value = v;
			keep = true;
			break;
		case kinConstant:
			v = kifGlobals->kconstants->Declared(name);
			current_value = v;
			keep = true;
			break;
		default:
			//An External Global variable (from XIP)
			if (declaration->type == kifVariableDeclaration) {
				if (index != NULL)
					return kifGlobals->providecouple(declaration->value->Returnvalue(), index, dom, declaration->value->Returnfunction());
				return declaration;
			}
			KifDomain* stackdom = kifGlobals->Beforetopid(idthread);

			if (dom != stackdom)
				v = dom->Declared(name);

			if (v == NULL) {
				KifDomain* func = kifGlobals->Top();
				if (func != NULL)
					v = func->Declared(name);
				if (v == NULL) {
					if (stackdom != NULL)
						v = stackdom->Declared(name);
					if (v == NULL && dom != kifcode->kstart) {
						v = kifcode->kstart->Declared(name);
						keep = true;
						current_value = v;
					}
					else
					if (stackdom == kifcode->kstart) {
						current_value = v;
						keep = true;
					}
					else
						newinstance = kinFrame;
				}
				else
					newinstance = kinFunction;
			}
			else
			if (dom == kifcode->kstart) {
				keep = true;
				current_value = v;
			}
			else
				newinstance = kinFrame;
		}

		if (v == NULL) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		if (v->Localvariable()) {
			current_value = NULL;
			keep = false;
		}
	}
	else
		v = current_value;


	if (value->isInstance()) {
		v = v->Returnvalue();
		if (v->isDomain() == false) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		return value->Thevariable((KifDomain*)v, idthread, callfunc);
	}


	if (index != NULL)
		return kifGlobals->providecouple(v->Returnvalue(), index, dom, v->Returnfunction());

	if (keep == true)
		newinstance = kinKnown;

	return v;
}

KifElement* KifInstanceFunction::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;

	KifElement* v = NULL;
	KifDomain* stackdom;

	if (current_value == NULL) {
		switch (newinstance) {
		case kinFunction:
			if (callfunc == NULL)
				v = kifGlobals->Topid(idthread)->Declared(name);
			else
				v = callfunc->declarations[name];
			break;
		case kinFrame:
			v = dom->Declared(name);
			if (v == NULL) {
				dom = kifGlobals->Beforetopid(idthread);
				v = dom->declarations[name];
			}
			break;
		case kinGlobal:
			v = kifcode->kstart->declarations[name];
			current_value = v;
			break;
		case kinConstant:
			v = kifGlobals->kconstants->declarations[name];
			current_value = v;
			break;
		default:
			stackdom = kifGlobals->Beforetopid(idthread);

			if (dom != stackdom)
				v = dom->Declared(name);

			if (v == NULL) {
				KifDomain* func = kifGlobals->Top();
				if (func != NULL)
					v = func->Declared(name);
				if (v == NULL) {
					if (stackdom != NULL)
						v = stackdom->Declared(name);
					if (v == NULL && dom != kifcode->kstart) {
						v = kifcode->kstart->Declared(name);
						current_value = v;
					}
					else {
						if (stackdom == kifcode->kstart)
							current_value = v;
						else
							newinstance = kinFrame;
					}
				}
				else
					newinstance = kinFunction;
			}
			else
			if (dom == kifcode->kstart)
				current_value = v;
			else
				newinstance = kinFrame;
		}

		if (v == NULL) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		if (v->Localvariable())
			current_value = NULL;
	}
	else
		v = current_value;

	if (v == NULL) {
		string msg = KIFKIFSTRINGS[20];
		msg += kifGlobals->kifIdString[name];
		return kifcode->Returnerror(msg);
	}

	v = v->Returnvalue();

	//Selfinsert indicates that the function will a predefined one
	if (function->Selfinsert() == true) {
		//In this case, we use the value as the value itself
		if (v->Linkedvalue() != kifNULL)
			v = v->Linkedvalue();
		return v->ApplySelfInsertFunction(function, contextualpattern, dom, idthread, callfunc);
	}

	//This is a function that is called from within a frame
	KifCallFunctionGeneral* localfunction = (KifCallFunctionGeneral*)function;
	//external declaration within another file
	if (v->type == kifKiF || v->type == kifLoader) {
		v = v->Declaration(function->name);
		if (v != NULL && v->isFunction()) {
			if (KifCheckParameters((KifFunction*)v, function, false, &localfunction->body) == 0)
				return function->Exec(contextualpattern, dom, val, idthread, callfunc);
		}
		string mess = KIFKIFSTRINGS[21] + kifGlobals->kifIdString[function->Name()];
		return kifcode->Returnerror(mess);
	}

	//We detect the sub frame declarations

	if (v->isDomain() == false) {
		string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[function->Name()];
		return kifcode->Returnerror(mess);
	}
	//if the function belongs to the same frame, we simply execute it
	KifFunction* bd = localfunction->body;
	if (bd != NULL) {
		//if might be a copy from a derivation
		if (bd->CheckFunctionFrame((KifFrame*)v->Frame()))
			return function->Exec(contextualpattern, (KifDomain*)v, val, idthread, callfunc);
		localfunction->body = NULL;
	}

	//the body is NULL, we will find the correct function body on the fly
	v = function->Exec(contextualpattern, (KifDomain*)v, val, idthread, callfunc);
	//else, we had kept a track of the previous body	
	localfunction->body = bd;
	return v;
}


KifElement* KifInstanceInFunction::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;

	switch (newinstance) {
	case kinKnown:
		return callfunc->declarations[name]->Returnvalue();
	case kinCall:
		return kifGlobals->Topid(idthread)->declarations[name]->Returnvalue();
	case kinPreviousCall:
		return kifGlobals->PreviousTopid(idthread)->declarations[name]->Returnvalue();
	}

	KifElement* v = NULL;
	if (callfunc == NULL) {
		newinstance = kinCall;
		v = kifGlobals->Topid(idthread)->Declared(name);
		if (v == NULL) {
			newinstance = kinPreviousCall;
			v = kifGlobals->PreviousTopid(idthread)->Declared(name);
			if (v == NULL) {
				string msg = KIFKIFSTRINGS[20];
				msg += kifGlobals->kifIdString[name];
				return kifcode->Returnerror(msg);
			}
		}
	}
	else {
		newinstance = kinKnown;
		v = callfunc->Declared(name);
		if (v == NULL) {
			newinstance = kinCall;
			v = kifGlobals->Topid(idthread)->Declared(name);
			if (v == NULL) {
				newinstance = kinPreviousCall;
				v = kifGlobals->PreviousTopid(idthread)->Declared(name);
				if (v == NULL) {
					string msg = KIFKIFSTRINGS[20];
					msg += kifGlobals->kifIdString[name];
					return kifcode->Returnerror(msg);
				}
			}
		}
	}

	if (value->isInstance()) {
		newinstance = kinFunction;
		v = v->Returnvalue();
		if (v->isDomain() == false) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		return value->Exec(contextualpattern, (KifDomain*)v, val, idthread, callfunc);
	}


	if (index != NULL) {
		newinstance = kinFunction;
		if (index->function == NULL && v->Returnfunction() == NULL)
			return v->Returnvalue()->Exec(contextualpattern, dom, index, idthread, callfunc);
		KifCouple couple(v->Returnvalue(), index, dom, v->Returnfunction());
		return couple.Exec(contextualpattern, dom, index, idthread, callfunc);
	}

	return v->Returnvalue();
}


KifElement* KifInstanceInFunction::Thevariable(KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;

	switch (newinstance) {
	case kinKnown:
		return callfunc->declarations[name];
	case kinCall:
		return kifGlobals->Topid(idthread)->declarations[name];
	case kinPreviousCall:
		return kifGlobals->PreviousTopid(idthread)->declarations[name];
	}

	KifElement* v = NULL;
	if (callfunc == NULL) {
		newinstance = kinCall;
		v = kifGlobals->Topid(idthread)->Declared(name);
		if (v == NULL) {
			newinstance = kinPreviousCall;
			v = kifGlobals->PreviousTopid(idthread)->Declared(name);
			if (v == NULL) {
				string msg = KIFKIFSTRINGS[20];
				msg += kifGlobals->kifIdString[name];
				return kifcode->Returnerror(msg);
			}
		}
	}
	else {
		newinstance = kinKnown;
		v = callfunc->Declared(name);
		if (v == NULL) {
			newinstance = kinCall;
			v = kifGlobals->Topid(idthread)->Declared(name);
			if (v == NULL) {
				newinstance = kinPreviousCall;
				v = kifGlobals->PreviousTopid(idthread)->Declared(name);
				if (v == NULL) {
					string msg = KIFKIFSTRINGS[20];
					msg += kifGlobals->kifIdString[name];
					return kifcode->Returnerror(msg);
				}
			}
		}
	}

	if (value->isInstance()) {
		newinstance = kinFunction;
		v = v->Returnvalue();
		if (v->isDomain() == false) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		return value->Thevariable((KifDomain*)v, idthread, callfunc);
	}

	if (index != NULL) {
		newinstance = kinFunction;
		return kifGlobals->providecouple(v->Returnvalue(), index, dom, v->Returnfunction());
	}

	return v;
}

KifElement* KifInstanceInFrame::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;

	switch (newinstance) {
	case kinKnown:
		return dom->declarations[name]->Returnvalue();
	case kinCallFrame:
		return kifGlobals->Beforetopid(idthread)->declarations[name]->Returnvalue();
	case kinPreviousCallFrame:
		return kifGlobals->PreviousBeforetopid(idthread)->declarations[name]->Returnvalue();
	}

	KifElement* v = dom->Declared(name);
	newinstance = kinKnown;
	if (v == NULL) {
		dom = kifGlobals->Beforetopid(idthread);
		v = dom->Declared(name);
		newinstance = kinCallFrame;
		if (v == NULL) {
			dom = kifGlobals->PreviousBeforetopid(idthread);
			newinstance = kinPreviousCallFrame;
			v = dom->Declared(name);
			if (v == NULL) {
				string msg = KIFKIFSTRINGS[20];
				msg += kifGlobals->kifIdString[name];
				return kifcode->Returnerror(msg);
			}
		}
	}

	if (value->isInstance()) {
		newinstance = kinFrame;
		v = v->Returnvalue();
		if (v->isDomain() == false) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		return value->Exec(contextualpattern, (KifDomain*)v, val, idthread, callfunc);
	}

	if (index != NULL) {
		newinstance = kinFrame;
		if (index->function == NULL && v->Returnfunction() == NULL)
			return v->Returnvalue()->Exec(contextualpattern, dom, index, idthread, callfunc);
		KifCouple couple(v->Returnvalue(), index, dom, v->Returnfunction());
		return couple.Exec(contextualpattern, dom, index, idthread, callfunc);
	}

	return v->Returnvalue();
}


KifElement* KifInstanceInFrame::Thevariable(KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;
	switch (newinstance) {
	case kinKnown:
		return dom->declarations[name];
	case kinCallFrame:
		return kifGlobals->Beforetopid(idthread)->declarations[name];
	case kinPreviousCallFrame:
		return kifGlobals->PreviousBeforetopid(idthread)->declarations[name];
	}

	KifElement* v = dom->Declared(name);
	newinstance = kinKnown;
	if (v == NULL) {
		dom = kifGlobals->Beforetopid(idthread);
		v = dom->Declared(name);
		newinstance = kinCallFrame;
		if (v == NULL) {
			dom = kifGlobals->PreviousBeforetopid(idthread);
			newinstance = kinPreviousCallFrame;
			v = dom->Declared(name);
			if (v == NULL) {
				string msg = KIFKIFSTRINGS[20];
				msg += kifGlobals->kifIdString[name];
				return kifcode->Returnerror(msg);
			}
		}
	}

	if (value->isInstance()) {
		newinstance = kinFrame;
		v = v->Returnvalue();
		if (v->isDomain() == false) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		return value->Thevariable((KifDomain*)v, idthread, callfunc);
	}


	if (index != NULL) {
		newinstance = kinFrame;
		return kifGlobals->providecouple(v->Returnvalue(), index, dom, v->Returnfunction());
	}

	return v;
}

KifElement* KifInstanceGlobal::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;
	if (newinstance == kinKnown)
		return current_value->Returnvalue();

	KifElement* v = kifcode->kstart->declarations[name];
	if (v == NULL) {
		string msg = KIFKIFSTRINGS[20];
		msg += kifGlobals->kifIdString[name];
		return kifcode->Returnerror(msg);
	}

	if (value->isInstance()) {
		v = v->Returnvalue();
		if (v->isDomain() == false) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		return value->Exec(contextualpattern, (KifDomain*)v, val, idthread, callfunc);
	}


	if (index != NULL) {
		if (v->Returnfunction() == NULL) {
			if (index->function == NULL)
				return v->Returnvalue()->Exec(contextualpattern, dom, index, idthread, callfunc);

			if (index->function->type != kifIndex) {
				v = v->Returnvalue()->Exec(contextualpattern, dom, index, idthread, callfunc);

				if (v->type == kifDomain)
					return index->function->Exec(contextualpattern, (KifDomain*)v, v, idthread, callfunc);

				return index->function->Exec(contextualpattern, dom, v, idthread, callfunc);
			}
		}

		KifCouple couple(v->Returnvalue(), index, dom, v->Returnfunction());
		return couple.Exec(contextualpattern, dom, index, idthread, callfunc);
	}

	//If we are dealing with a loadin, then the variable has not been created yet...
	if (callfunc != NULL && callfunc->name == idloadin) {
		val = v->Exec(contextualpattern, dom, val, 0, callfunc);
		v->Seteval(255);
		v = val;
		dom->Enableadding();
	}


	if (!v->Localvariable()) {
		newinstance = kinKnown;
		current_value = v;
	}
	return v->Returnvalue();
}


KifElement* KifInstanceGlobal::Thevariable(KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;
	if (newinstance == kinKnown)
		return current_value;


	KifElement* v = kifcode->kstart->declarations[name];
	if (v == NULL) {
		string msg = KIFKIFSTRINGS[20];
		msg += kifGlobals->kifIdString[name];
		return kifcode->Returnerror(msg);
	}

	if (value->isInstance()) {
		v = v->Returnvalue();
		if (v->isDomain() == false) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		return value->Thevariable((KifDomain*)v, idthread, callfunc);
	}


	if (index != NULL)
		return kifGlobals->providecouple(v->Returnvalue(), index, dom, v->Returnfunction());

	if (!v->Localvariable()) {
		newinstance = kinKnown;
		current_value = v;
	}
	return v;
}

KifElement* KifInstanceConstant::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;
	if (newinstance == kinKnown)
		return current_value->Returnvalue();


	KifElement* v = kifGlobals->kconstants->Declared(name);
	if (v == NULL) {
		string msg = KIFKIFSTRINGS[20];
		msg += kifGlobals->kifIdString[name];
		return kifcode->Returnerror(msg);
	}

	if (value->isInstance()) {
		v = v->Returnvalue();
		if (v->isDomain() == false) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		return value->Exec(contextualpattern, (KifDomain*)v, val, idthread, callfunc);
	}


	if (index != NULL) {
		if (v->Returnfunction() == NULL) {
			if (index->function == NULL)
				return v->Returnvalue()->Exec(contextualpattern, dom, index, idthread, callfunc);

			if (index->function->type != kifIndex) {
				v = v->Returnvalue()->Exec(contextualpattern, dom, index, idthread, callfunc);

				if (v->type == kifDomain)
					return index->function->Exec(contextualpattern, (KifDomain*)v, v, idthread, callfunc);

				return index->function->Exec(contextualpattern, dom, v, idthread, callfunc);
			}
		}

		KifCouple couple(v->Returnvalue(), index, dom, v->Returnfunction());
		return couple.Exec(contextualpattern, dom, index, idthread, callfunc);
	}

	newinstance = kinKnown;
	current_value = v;
	return v->Returnvalue();
}


KifElement* KifInstanceConstant::Thevariable(KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (kifGlobals->executionbreak)
		return kifRAISEERROR;
	if (newinstance == kinKnown)
		return current_value;


	KifElement* v = kifGlobals->kconstants->Declared(name);
	if (v == NULL) {
		string msg = KIFKIFSTRINGS[20];
		msg += kifGlobals->kifIdString[name];
		return kifcode->Returnerror(msg);
	}

	if (value->isInstance()) {
		v = v->Returnvalue();
		if (v->isDomain() == false) {
			string mess = KIFLIBSTRINGS[183] + kifGlobals->kifIdString[name];
			return kifcode->Returnerror(mess);
		}
		return value->Thevariable((KifDomain*)v, idthread, callfunc);
	}


	if (index != NULL)
		return kifGlobals->providecouple(v->Returnvalue(), index, dom, v->Returnfunction());

	newinstance = kinKnown;
	current_value = v;
	return v;
}

//-------------------------------------------------------------------------------------------

Exported KifElement* KifPredefined::Exec(KifElement* a, KifDomain* b, KifElement* c, int idthread, KifCallFunction* d) {
	return (*function)(a, b, d, idthread);
}

KifElement* KifElement::ApplySelfInsertFunction(KifCallFunction* f,
	KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	return f->Exec(contextualpattern, dom, this, idthread, callfunc);
}

KifElement* KifBasic::ApplySelfInsertFunction(KifCallFunction* f, KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (f->type == kifCallFunctionExternal)
		return f->Exec(contextualpattern, dom, this, idthread, callfunc);

	//if we are here, it is because a library definition name matches a predefined name...
	//In that case the call to this library method should replace the call to the predefined name
	int idvalue = f->Name();
	KifElement* kfunc = Declaration(idvalue);
	if (kfunc == NULL) {
		string mess = KIFKIFSTRINGS[22] + kifGlobals->kifIdString[idvalue];
		return kifcode->Returnerror(mess);
	}
	if (kfunc->type == kifPredefined) {
		//a little bit of explanation. We are forced to hide the current variable as a Self
		//otherwise, we will run through the reinterpretation of this function as a local function
		//to our external library object. In this case, we are trying to execute functions such as size or string
		KifSelf voila(NULL, NULL, this);
		voila.kifcode = kifcode;
		return f->Exec(contextualpattern, dom, &voila, idthread, callfunc);
	}

	//Since it is a self insert function, we need to skip the first parameter...
	KifCallFunctionWithClean call(NULL, NULL, idvalue);
	call.kifcode = f->kifcode;
	for (int i = 1; i < f->Size(); i++)
		call.parameters.push_back(f->parameters[i]);
	return ExecMethod(idvalue, contextualpattern, dom, idthread, &call);
}

KifElement* KifDomain::ApplySelfInsertFunction(KifCallFunction* f, KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	int idvalue = f->Name();
	KifElement* func = frame->Declared(idvalue);
	if (func == NULL)
		return f->Exec(contextualpattern, dom, this, idthread, callfunc);

	if (func->isFunction() == false) {
		string mess = KIFKIFSTRINGS[23] + kifGlobals->kifIdString[idvalue];
		return kifcode->Returnerror(mess);
	}
	KifCallFunctionGeneral call(NULL, NULL, idvalue, NULL);
	call.kifcode = kifcode;
	if (KifCheckParameters((KifFunction*)func, &call, false, &call.body) == 0) {
		for (int i = 1; i < f->Size(); i++)
			call.parameters.push_back(f->parameters[i]);
		return call.Exec(contextualpattern, this, kifNULL, idthread, callfunc);
	}
	string mess = KIFKIFSTRINGS[23] + kifGlobals->kifIdString[idvalue];
	return kifcode->Returnerror(mess);

}

//-----------------------------------------------------------------------------------------------------------------
//Instruction methods
KifInstruction::KifInstruction(KifCode* klc, KifElement* kp, KifOperator kop) : KifObjectInstruction(klc, NULL, kifInstruction) {
	action = kop;
	negation = false;
	parent = kp;
	if (kp != NULL)
		kp->Add(this);
}

//-----------------------------------------------------------------------------------------------------------------
KifElement* KifInstructionINITIALISATION::Exec(KifElement* context, KifDomain* dom, KifElement* var, int idthread, KifCallFunction* callfunc) {
	//first, we get the target element where to store our result
	KifElement* val = var->Returnvalue();
	KifElement* result = kifNULL;
	if (instructions.size() != 0)
		result = instructions[0];

	if (result->type == kifSelf) {
		//One single horrible case... var is a KifCall
		//then we might expect the initialisation to be done with a function
		if (val->type != kifCall)
			return kifcode->Returnerror(KIFLIBSTRINGS[160]);
	}


	if (result != kifNULL) {
		switch (result->type) {
		case kifInteger:
		case kifFloat:
		case kifBoolean:
		case kifString:
		case kifLong:
		case kifUString:
			var->Setvalue(result, NULL, dom);
			return kifTRUE;
		}

		//In the case of a function attached to a variable, we do not want to loose the initial value for maps and vectors
		if (var->Function() == false)
			val->Setevaluate(true);

		result = result->Exec(val, dom, kifNULL, idthread, callfunc);

		val->Setevaluate(false);

		if (Errorid(idthread)) {
			result->Release();
			return Errorptr();
		}

		if (result == kifNOELEMENT)
			var->Setvalue(kifNULL, NULL, dom);
		else {
			if (negation) {
				bool vbool = result->Boolean();
				result->Release();
				if (vbool)
					result = kifFALSE;
				else
					result = kifTRUE;
			}
			var->Setvalue(result, NULL, dom);
			result->Release();
		}
	}

	return kifTRUE;
}

KifElement* KifInstructionAFFECTATION::Exec(KifElement* context, KifDomain* dom, KifElement* var, int idthread, KifCallFunction* callfunc) {
	KifElement* result = instructions[0];
	var = kifNULL;
	KifElement* val = kifNULL;
	bool container = false;
	if (result->type != kifBaseVector && result->type != kifBaseMap) {
		if (result->type == kifInstance) {
			var = result->Thevariable(dom, idthread, callfunc);
			//If the type is KIFKIFSTRINGS[24], we do not want the current value to change the behaviour
			//of storing a new element into the variable.
			if (var->type != kifSelf)
				val = var->Returnvalue();
		}
		else {
			var = result;
			val = var->Value(dom);
		}

		//In the case of a function attached to a variable, we do not want to loose the initial value for maps and vectors
		//Toassign is a way to avoid creating a new map or new vector, when in fact the placeholder is modifying itself...
		if (var->Function() == false) {
			if (val != NULL)
				val->Setevaluate(true);
		}
		else
			var->SetSynchronousLock();
	}
	else {
		container = true;
		var = result;
	}

	switch (instructions[1]->type) {
	case kifInteger:
	case kifFloat:
	case kifBoolean:
	case kifString:
	case kifLong:
	case kifUString:
		result = instructions[1];
		break;
	case kifCallFunctionThread:
		if (action == kifSTREAM) {
			if (val != NULL)
				val->Setevaluate(false);
			if (var->type == kifCouple) {
				result = var->EvaluateKey(kifNULL, NULL, dom, idthread, callfunc);
				var->Release();
			}
			else
				result = var;
			instructions[1]->Exec(result, dom, this, idthread, callfunc);
			var->ResetSynchronousLock();
			return kifTRUE;
		}
	default:
		result = instructions[1]->Exec(val, dom, kifNULL, idthread, callfunc);
	}

	if (container) {
		if (var->type == kifBaseVector) {
			if (!result->aVectorContainer())
				return kifcode->Returnerror(KIFLIBSTRINGS[268]);
			var = result->Match((KifBaseVector*)var, dom, idthread, callfunc);
			result->Release();
			if (var == kifNOELEMENT)
				return kifcode->Returnerror(KIFLIBSTRINGS[269]);
			return kifTRUE;
		}

		if (var->type == kifBaseMap) {
			if (!result->aMapContainer())
				return kifcode->Returnerror(KIFLIBSTRINGS[270]);
			var = result->Match((KifBaseMap*)var, dom, idthread, callfunc);
			result->Release();
			if (var == kifNOELEMENT)
				return kifcode->Returnerror(KIFLIBSTRINGS[271]);
			return kifTRUE;
		}
	}

	if (val != NULL)
		val->Setevaluate(false);

	if (Errorid(idthread)) {
		var->ResetSynchronousLock();
		return Errorptr();
	}

	if (result == kifNOELEMENT)
		result = kifNULL;
	else {
		if (negation) {
			bool vbool = result->Boolean();
			result->Release();
			if (vbool)
				result = kifFALSE;
			else
				result = kifTRUE;
		}
	}

	if (var->Setvalue(result, NULL, dom) == false) {
		//This is a specific KifSelf element, created in KifParseCall
		//The value is the FRAME definition (and not a frame instance)
		//Setvalue with this element triggers the creation of a new instance
		//which we must initialise. parameter stows the parameters of the _initial function
		//pointer.
		val = var->Returnvalue();
		KifSelf* self = (KifSelf*)instructions[1];
		//we call
		val->constructor(dom, self->parameter, idthread, callfunc);
		if (val->Delayed())
			val->ApplyDelayedInitial(idthread, callfunc);
		//then we also disable the delayed on the top frame variable...
		dom->Resetdelayed();
	}

	var->ResetSynchronousLock();
	result->Release();
	return kifTRUE;
}

KifElement* KifInstructionPLUSPLUS::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* var, int idthread, KifCallFunction* callfunc) {

	var = instructions[0];

	if (var->isInstance())
		var = var->Thevariable(dom, idthread, callfunc);
	else {
		var = var->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (dom->Failed())
			return kifRAISEERROR;
	}

	if (var->Function()) {
		var->SetSynchronousLock();
		KifElement* val = var->Returnvalue();
		val->plusplus();
		var->callfunction(val, val, dom, idthread, callfunc);
		var = val;
		var->ResetSynchronousLock();
	}
	else {
		var = var->Returnvalue();
		var = var->plusplus();
	}
	return var;
}

KifElement* KifInstructionMINUSMINUS::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* var, int idthread, KifCallFunction* callfunc) {

	var = instructions[0];

	if (var->isInstance())
		var = var->Thevariable(dom, idthread, callfunc);
	else {
		var = var->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (dom->Failed())
			return kifRAISEERROR;
	}

	if (var->Function()) {
		var->SetSynchronousLock();
		KifElement* val = var->Returnvalue();
		val->minusminus();
		var->callfunction(val, val, dom, idthread, callfunc);
		var = val;
		var->ResetSynchronousLock();
	}
	else {
		var = var->Returnvalue();
		var = var->minusminus();
	}
	return var;
}

KifElement* KifInstructionKIFIN::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc) {

	ke = instructions[0]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	KifElement* kf = instructions[1]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	KifElement* result = kifNULL;
	if (Errorid(idthread)) {
		ke->Release();
		kf->Release();
		return Errorptrid(idthread);
	}

	if (instructions.size() == 3) {
		KifFunction* func = instructions[2]->Functioncall(dom);
		if (func == NULL) {
			kf->Release();
			return kifcode->Returnerror(KIFLIBSTRINGS[264]);
		}
		result = contextualpattern->in(ke, kf, func, contextualpattern, false);
	}
	else
		result = contextualpattern->in(ke, kf, NULL, contextualpattern, false);
	ke->Release();
	kf->Release();
	if (contextualpattern->aContainer() && !result->Succeed())
		return kifNULL;
	bool neg = negation;
	if (action == kifNOTIN)
		neg = 1 - neg;

	if (neg) {
		bool vbool = result->Boolean();
		result->Release();
		if (vbool)
			return kifFALSE;
		return kifTRUE;
	}

	return result;
}

//--------------------------------LAMBDA AND HASKELL-------------------------------------------------
KifElement* OnListReturn(KifElement* contextualpattern, KifDomain* dom, KifElement* kfunction, KifElement* klist, int idthread, KifCallFunction* callfunc) {
	KifElement* kcont = NULL;

	if (contextualpattern != kifNULL && contextualpattern != klist) {
		kcont = contextualpattern;
		kcont->Clear();
	}

	KifFunction* thefunction = kfunction->Functioncall(dom);

	KifIteration* itr = (KifIteration*)klist->Newiterator(false);
	KifIterator vitr(NULL, NULL, false);
	vitr.kifcode = dom->kifcode;
	vitr.value = itr;
	vitr.reference = 1;
	KifElement* returnval;

	KifCallFunctionGeneral kfunc(NULL, NULL, thefunction->Name(), thefunction);
	kfunc.Setline(thefunction->Line(), thefunction->Fileindex());
	kfunc.kifcode = dom->kifcode;

	//We add our parameters
	kfunc.parameters.push_back(&vitr);
	kfunc.Set(thefunction->arguments[0]->Name(), &vitr, NULL);
	KifElement* key;
	KifElement* localvar = kifNULL;
	bool addlocal = false;
	if (thefunction->arguments.size() == 2) {
		KifFunctionDeclaration* kfd = (KifFunctionDeclaration*)thefunction->arguments[1];
		if (kfd->initialisation != NULL && kfd->initialisation->Action() == kifINITIALISATION) {
			KifElement* localval = kfd->initialisation->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			localvar = localval->Newinstance();
			localvar->Setvalue(localval);
			localvar->Setreference();
			localval->Release();
			kfunc.parameters.push_back(localvar);
			kfunc.Set(thefunction->arguments[1]->Name(), localvar, NULL);
		}
		else
			addlocal = true;
	}

	d_pop pp(idthread, &kfunc, dom);
	for (vitr.Begin(); vitr.End() != kifTRUE; vitr.Next()) {
		//values in this case are atomic... They cannot be a function as above, hence this local case
		//We expect a function with one single argument
		if (addlocal) {
			returnval = itr->IteratorValue();
			localvar = returnval->Newinstance();
			localvar->Setreference();
			addlocal = false;
			kfunc.parameters.push_back(localvar);
			kfunc.Set(thefunction->arguments[1]->Name(), localvar, NULL);
			returnval->Release();
		}

		returnval = thefunction->Exec(contextualpattern, dom, kifNULL, idthread, &kfunc);
		if (returnval == kifNOELEMENT)
			break;

		if (Errorid(idthread)) {
			localvar->Resetreference();
			itr->Release();
			if (kcont != contextualpattern)
				kcont->Release();
			return Errorptr();
		}

		if (returnval != kifNULL) {
			if (kcont == NULL)
				kcont = klist->Newinstance();
			if (kcont->aMapContainer()) {
				key = vitr.IteratorKey();
				kcont->Push(key, returnval);
				key->Release();
			}
			else
				kcont->Push(returnval);
			returnval->Release();
		}
	}
	itr->Release();
	localvar->Resetreference();
	if (kcont == NULL)
		return kifTRUE;
	return kcont;
}

KifElement* OnValueReturn(KifElement* contextualpattern, KifDomain* dom, KifElement* kfunction, KifElement* klist, int idthread, KifCallFunction* callfunc) {
	KifFunction* thefunction = kfunction->Functioncall(dom);

	KifIteration* itr = (KifIteration*)klist->Newiterator(false);
	KifIterator vitr(NULL, NULL, false);
	vitr.kifcode = dom->kifcode;
	vitr.value = itr;
	vitr.reference = 1;
	KifElement* returnval;
	KifElement* localvar = kifTRUE;

	KifCallFunctionGeneral kfunc(NULL, NULL, thefunction->Name(), thefunction);
	kfunc.Setline(thefunction->Line(), thefunction->Fileindex());
	kfunc.kifcode = dom->kifcode;
	kfunc.parameters.push_back(&vitr);
	kfunc.Set(thefunction->arguments[0]->Name(), &vitr, NULL);

	if (thefunction->arguments.size() == 2) {
		KifFunctionDeclaration* kfd = (KifFunctionDeclaration*)thefunction->arguments[1];
		if (kfd->initialisation != NULL && kfd->initialisation->Action() == kifINITIALISATION) {
			KifElement* localval = kfd->initialisation->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			localvar = localval->Newinstance();
			localvar->Setvalue(localval);
			localval->Release();
		}
		else
			localvar = contextualpattern->Newinstance();
		localvar->Setreference();
		//We add our parameter	
		kfunc.parameters.push_back(localvar);
		kfunc.Set(thefunction->arguments[1]->Name(), localvar, NULL);
	}

	d_pop pp(idthread, &kfunc, dom);
	for (vitr.Begin(); vitr.End() != kifTRUE; vitr.Next()) {
		//values in this case are atomic... They cannot be a function as above, hence this local case
		//We expect a function with one single argument
		returnval = thefunction->Exec(contextualpattern, dom, kifNULL, idthread, &kfunc);
		if (returnval == kifNOELEMENT)
			break;

		if (Errorid(idthread)) {
			if (localvar != kifTRUE)
				localvar->Resetreference();
			itr->Release();
			return Errorptr();
		}
	}

	itr->Release();
	if (localvar != kifTRUE) {
		localvar->reference--;
		localvar->Setpopped();
	}
	return localvar;
}


KifElement* KifInstructionKIFON::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* kfunc, int idthread, KifCallFunction* callfunc) {

	kfunc = instructions[0];
	if (!kfunc->isFunction()) {
		kfunc = kfunc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kfunc->type != kifCall && !kfunc->isFunction()) {
			kfunc->Release();
			return dom->kifcode->Returnerror(KIFKIFSTRINGS[25]);
		}
	}

	KifElement* klist = instructions[1]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

	if (Errorid(idthread))
		return Errorptrid(idthread);

	KifElement* result = kifNULL;
	if (contextualpattern->aContainer() || contextualpattern == kifNULL)
		result = OnListReturn(contextualpattern, dom, kfunc, klist, idthread, callfunc);
	else
		result = OnValueReturn(contextualpattern, dom, kfunc, klist, idthread, callfunc);

	kfunc->Release();
	klist->Release();
	if (negation) {
		bool vbool = result->Boolean();
		result->Release();
		if (vbool)
			return kifFALSE;
		return kifTRUE;
	}
	return result;
}


KifElement* KifInstructionCOMPARE::Exec(KifElement* context, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc) {
	ke = instructions[0];
	KifElement* result = instructions[1];
	KifElement* res;

	switch (ke->type) {
	case kifInteger:
	case kifFloat:
	case kifBoolean:
	case kifString:
	case kifLong:
	case kifUString:
		break;
	default: {
				 ke = ke->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
				 if (dom->Failed())
					 return kifRAISEERROR;
	}
	}

	switch (result->type) {
	case kifInteger:
	case kifFloat:
	case kifBoolean:
	case kifString:
	case kifLong:
	case kifUString:
		break;
	case kifInstance:
		result = ((KifInstance*)result)->Exec(ke, dom, kifNULL, idthread, callfunc);
		break;
	default:
		if (!result->aContainer()) {
			result = result->Exec(ke, dom, kifNULL, idthread, callfunc);
			if (dom->Failed())
				return kifRAISEERROR;
		}
	}

	char av = ke->inCode() | result->inCode();
	if (av <= 8) {
		res = kifFALSE;
		if (av == ckifInteger) {
			long v1 = ke->Integer();
			long v2 = result->Integer();
			switch (action) {
			case kifEQUAL:
				if (v1 == v2)
					res = kifTRUE;
				break;
			case kifSUPERIOR:
				if (v1 > v2)
					res = kifTRUE;
				break;
			case kifINFERIOR:
				if (v1 < v2)
					res = kifTRUE;
				break;
			case kifSUPEQU:
				if (v1 >= v2)
					res = kifTRUE;
				break;
			case kifINFEQU:
				if (v1 <= v2)
					res = kifTRUE;
				break;
			case kifDIFFERENT:
				if (v1 != v2)
					res = kifTRUE;
			}
			ke->Release();
			result->Release();
			if (negation) {
				if (res == kifFALSE)
					return kifTRUE;
				return kifFALSE;
			}
			return res;
		}
		if (av == ckifFloat || av == ckifIntFloat) {
			double v1 = ke->Float();
			double v2 = result->Float();
			switch (action) {
			case kifEQUAL:
				if (v1 == v2)
					res = kifTRUE;
				break;
			case kifSUPERIOR:
				if (v1 > v2)
					res = kifTRUE;
				break;
			case kifINFERIOR:
				if (v1 < v2)
					res = kifTRUE;
				break;
			case kifSUPEQU:
				if (v1 >= v2)
					res = kifTRUE;
				break;
			case kifINFEQU:
				if (v1 <= v2)
					res = kifTRUE;
				break;
			case kifDIFFERENT:
				if (v1 != v2)
					res = kifTRUE;
			}
			ke->Release();
			result->Release();
			if (negation) {
				if (res == kifFALSE)
					return kifTRUE;
				return kifFALSE;
			}
			return res;
		}
		if (av == ckifString) {
			string v1 = ke->String();
			string v2 = result->String();
			switch (action) {
			case kifEQUAL:
				if (v1 == v2)
					res = kifTRUE;
				break;
			case kifSUPERIOR:
				if (v1 > v2)
					res = kifTRUE;
				break;
			case kifINFERIOR:
				if (v1 < v2)
					res = kifTRUE;
				break;
			case kifSUPEQU:
				if (v1 >= v2)
					res = kifTRUE;
				break;
			case kifINFEQU:
				if (v1 <= v2)
					res = kifTRUE;
				break;
			case kifDIFFERENT:
				if (v1 != v2)
					res = kifTRUE;
			}
			ke->Release();
			result->Release();
			if (negation) {
				if (res == kifFALSE)
					return kifTRUE;
				return kifFALSE;
			}
			return res;
		}
		if (av == ckifUString) {
			wstring v1 = ke->UString();
			wstring v2 = result->UString();
			switch (action) {
			case kifEQUAL:
				if (v1 == v2)
					res = kifTRUE;
				break;
			case kifSUPERIOR:
				if (v1 > v2)
					res = kifTRUE;
				break;
			case kifINFERIOR:
				if (v1 < v2)
					res = kifTRUE;
				break;
			case kifSUPEQU:
				if (v1 >= v2)
					res = kifTRUE;
				break;
			case kifINFEQU:
				if (v1 <= v2)
					res = kifTRUE;
				break;
			case kifDIFFERENT:
				if (v1 != v2)
					res = kifTRUE;
			}
			ke->Release();
			result->Release();
			if (negation) {
				if (res == kifFALSE)
					return kifTRUE;
				return kifFALSE;
			}
			return res;
		}
	}

	switch (action) {
	case kifEQUAL:
		res = ke->same(result);
		break;
	case kifSUPERIOR:
		res = ke->more(result);
		break;
	case kifINFERIOR:
		res = ke->less(result);
		break;
	case kifSUPEQU:
		res = ke->moreequal(result);
		break;
	case kifINFEQU:
		res = ke->lessequal(result);
		break;
	case kifDIFFERENT:
		res = ke->different(result);
	}

	result->Release();
	ke->Release();

	if (negation == true) {
		bool vbool = res->Boolean();
		res->Release();
		if (vbool)
			return kifFALSE;
		return kifTRUE;
	}

	return res;
}

KifElement* KifInstructionOR::Exec(KifElement* context, KifDomain* dom, KifElement* result, int idthread, KifCallFunction* callfunc) {
	int maxid = instructions.size();
	for (int i = 0; i < maxid; i++) {
		result = instructions[i]->Exec(kifTRUE, dom, kifNULL, idthread, callfunc);
		if (result->Boolean() == true) {
			result->Release();
			return kifTRUE;
		}
		result->Release();
	}
	return kifFALSE;
}

KifElement* KifInstructionAND::Exec(KifElement* context, KifDomain* dom, KifElement* result, int idthread, KifCallFunction* callfunc) {
	int maxid = instructions.size();
	for (int i = 0; i < maxid; i++) {
		result = instructions[i]->Exec(kifTRUE, dom, kifNULL, idthread, callfunc);
		if (result->Boolean() == false) {
			result->Release();
			return kifFALSE;
		}
		result->Release();
	}
	return kifTRUE;
}


KifElement* KifInstructionAPPLYOPERATIONEQU::Exec(KifElement* context, KifDomain* dom, KifElement* var, int idthread, KifCallFunction* callfunc) {
	var = instructions[0]->Thevariable(dom, idthread, callfunc);

	var->SetSynchronousLock();

	KifElement* val = var->Returnvalue();

	KifElement* result = kifNULL;
	KifElement* ins;
	bool evaluate = val->Evaluate();
	val->Setevaluate(false);

	ins = instructions[1];
	switch (ins->type) {
	case kifInteger:
	case kifFloat:
	case kifBoolean:
	case kifLong:
	case kifString:
	case kifUString:
		result = ins;
		break;
	default:
		result = ins->Exec(val, dom, kifNULL, idthread, callfunc);
	}

	switch (action) {
	case kifINTERSECTIONEQU:
		ins = val->andset(val, result, val, true);
		break;
	case kifUNIONEQU:
		ins = val->orset(val, result, val, true);
		break;
	case kifXOREQU:
		ins = val->xorset(val, result, val, true);
		break;
	case kifPLUSEQU:
		ins = val->plus(val, result, val, true);
		break;
	case kifMINUSEQU:
		ins = val->minus(val, result, val, true);
		break;
	case kifMULTIPLYEQU:
		ins = val->multiply(val, result, val, true);
		break;
	case kifDIVIDEEQU:
		ins = val->divide(val, result, val, true);
		break;
	case kifMODEQU:
		ins = val->mod(val, result, val, true);
		break;
	case kifPOWEREQU:
		ins = val->power(val, result, val, true);
		break;
	case kifSHIFTRIGHTEQU:
		ins = val->shiftright(val, result, val, true);
		break;
	case kifSHIFTLEFTEQU:
		ins = val->shiftleft(val, result, val, true);
		break;
	default:
		ins = kifNULL;
	}

	if (var->Function()) {
		var->callfunction(val, val, dom, idthread, callfunc);
		var->ResetSynchronousLock();
	}

	val->Setevaluate(evaluate);
	result->Release();
	ins->Release();
	return val;
}

KifElement* KifInstructionMERGEEQU::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* var, int idthread, KifCallFunction* callfunc) {
	var = instructions[0]->Thevariable(dom, idthread, callfunc);

	var->SetSynchronousLock();

	KifElement* vres = var->Returnvalue();
	if (vres->type == kifCouple) {
		KifElement* vval = vres->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (vval == kifNOELEMENT || !vval->aVectorContainer()) {
			vval = new KifVector(kifcode, NULL);
			vres->Setvalue(vval);
		}
		vres->Release();
		vres = vval;
	}


	KifElement* ins;
	for (int i = 1; i < instructions.size(); i++) {
		ins = instructions[i];
		switch (ins->type) {
		case kifInteger:
		case kifFloat:
		case kifBoolean:
		case kifString:
		case kifUString:
			vres->Push(ins);
			break;
		default:
			ins = ins->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			if (kifGlobals->THR[idthread]->error) {
				ins->Release();
				vres->Release();
				if (var->Function())
					var->ResetSynchronousLock();

				return kifRAISEERROR;
			}

			if (ins == kifNOELEMENT || ins == kifNULL || (ins->aContainer() && ins->Size() == 0)) {
				ins->Release();
				continue;
			}

			vres->Merge(ins);
			ins->Release();
		}
	}

	if (var->Function()) {
		var->callfunction(vres, vres, dom, idthread, callfunc);
		var->ResetSynchronousLock();
	}

	return vres;
}

KifElement* KifInstructionMERGE::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* vres, int idthread, KifCallFunction* callfunc) {
	//the first element gives a hint at the final object
	VECTA<KifElement*> vals(instructions.size());
	int i;
	uchar tp = 0;
	uchar utp;
	for (i = 0; i < instructions.size(); i++) {
		vres = instructions[i];
		utp = vres->inCode();
		if (utp != ckifInstance && vres->Simple()) {
			tp |= utp;
			vals.push_back(vres);
		}
		else {
			vres = vres->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			if (kifGlobals->THR[idthread]->error) {
				vres->Release();
				for (i = 0; i < vals.size(); i++)
					vals[i]->Release();
				return kifRAISEERROR;
			}
			if (vres == kifNOELEMENT || vres == kifNULL || (vres->aContainer() && vres->Size() == 0)) {
				vres->Release();
				continue;
			}

			if (!i && vres->aMapContainer())
				tp = 255;
			tp |= vres->inCode();
			vals.push_back(vres);
		}
	}

	if (tp == 255) {
		if (contextualpattern->reference <= 1 && contextualpattern->aMapContainer() && contextualpattern->Evaluate()) {
			contextualpattern->Clear();
			vres = contextualpattern;
		}
		else
			vres = vals[0]->Newinstance();

	}
	else {
		if (contextualpattern->reference <= 1 && contextualpattern->aVectorContainer() && contextualpattern->Evaluate()) {
			contextualpattern->Clear();
			vres = contextualpattern;
		}
		else {
			switch (tp) {
			case ckifInteger:
				vres = new KifVectorInteger(kifcode, NULL);
				break;
			case ckifFloat:
			case ckifIntFloat:
				vres = new KifVectorFloat(kifcode, NULL);
				break;
			case ckifString:
				vres = new KifVectorString(kifcode, NULL);
				break;
			case ckifUString:
				vres = new KifVectorUString(kifcode, NULL);
				break;
			default:
				vres = new KifVector(kifcode, NULL);
			}
		}
	}

	for (i = 0; i < vals.size(); i++) {
		vres->Merge(vals[i]);
		vals[i]->Release();
	}
	return vres;
}

KifElement* KifInstructionADDTOLIST::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* v1, int idthread, KifCallFunction* callfunc) {
	//the first element gives a hint at the final object
	if (instructions.size() != 2)
		return kifcode->Returnerror(KIFKIFSTRINGS[26]);


	v1 = instructions[0]->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
	KifElement* v2 = instructions[1]->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (kifGlobals->THR[idthread]->error) {
		v1->Release();
		v2->Release();
		return kifRAISEERROR;
	}

	if (v1->aVectorContainer()) {
		v1->Merge(v2);
		v2->Release();
		return v1;
	}

	if (v2->aVectorContainer()) {
		v2->Insert(0, v1);
		v1->Release();
		return v2;
	}

	v1->Release();
	v2->Release();
	return kifcode->Returnerror(KIFKIFSTRINGS[27]);
}

KifElement* KifInstructionADDTOLISTEQU::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* var, int idthread, KifCallFunction* callfunc) {
	//the first element gives a hint at the final object
	if (instructions.size() != 2)
		return kifcode->Returnerror(KIFKIFSTRINGS[26]);

	var = instructions[0]->Thevariable(dom, idthread, callfunc);

	var->SetSynchronousLock();

	KifElement* v1 = var->Returnvalue();
	if (v1->type == kifCouple) {
		KifElement* vval = v1->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (vval == kifNOELEMENT || !vval->aVectorContainer()) {
			vval = new KifVector(kifcode, NULL);
			v1->Setvalue(vval);
		}
		v1->Release();
		v1 = vval;
	}

	KifElement* v2 = instructions[1]->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	if (v1->aVectorContainer()) {
		v1->Merge(v2);
		v2->Release();
		return v1;
	}

	if (v2->aVectorContainer()) {
		v2->Insert(0, v1);
		v1->Release();
		return v2;
	}
	v1->Release();
	v2->Release();
	return kifcode->Returnerror(KIFKIFSTRINGS[27]);
}

KifElement* KifInstructionAPPLYOPERATION::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	KifElement* current = instructions[0];
	int i, maxid = instructions.size();
	bool autoself = false;

	switch (current->type) {
	case kifInteger:
	case kifFloat:
	case kifBoolean:
	case kifLong:
	case kifString:
	case kifUString:
		val = current;
		break;
	default:
		val = current->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (val->popped == 1 && val->reference == 0 && val->type == contextualpattern->type) {
			autoself = true;
			contextualpattern = val;
		}
	}

	//A sub expression, its type is val then
	if (subcontext)
		contextualpattern = val;

	for (i = 1; i < maxid; i++) {
		current = instructions[i];
		switch (current->type) {
		case kifInteger:
		case kifFloat:
		case kifBoolean:
		case kifLong:
		case kifString:
		case kifUString:
			break;
		default:
			current = current->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		}

		switch (action) {
		case kifINTERSECTION:
			contextualpattern = contextualpattern->andset(val, current, contextualpattern, autoself);
			break;
		case kifUNION:
			contextualpattern = contextualpattern->orset(val, current, contextualpattern, autoself);
			break;
		case kifXOR:
			contextualpattern = contextualpattern->xorset(val, current, contextualpattern, autoself);
			break;
		case kifPLUS:
			contextualpattern = contextualpattern->plus(val, current, contextualpattern, autoself);
			break;
		case kifMINUS:
			contextualpattern = contextualpattern->minus(val, current, contextualpattern, autoself);
			break;
		case kifMULTIPLY:
			contextualpattern = contextualpattern->multiply(val, current, contextualpattern, autoself);
			break;
		case kifDIVIDE:
			contextualpattern = contextualpattern->divide(val, current, contextualpattern, autoself);
			break;
		case kifMOD:
			contextualpattern = contextualpattern->mod(val, current, contextualpattern, autoself);
			break;
		case kifPOWER:
			contextualpattern = contextualpattern->power(val, current, contextualpattern, autoself);
			break;
		case kifSHIFTRIGHT:
			contextualpattern = contextualpattern->shiftright(val, current, contextualpattern, autoself);
			break;
		case kifSHIFTLEFT:
			contextualpattern = contextualpattern->shiftleft(val, current, contextualpattern, autoself);
			break;
		}

		if (contextualpattern != val) {
			val->Release();
			val = contextualpattern;
		}

		autoself = true;
		current->Release();
	}

	return val;
}

KifElement* KifInstruction::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc) {
	KifElement* res = kifNULL;

	switch (action) {
	case kifBLOC:
		res = instructions[0]->Exec(kifNULL, dom, ke, idthread, callfunc);
		break;
	case kifBLOCBOOLEAN:
		switch (instructions[0]->type) {
		case kifInteger:
		case kifFloat:
		case kifBoolean:
			res = instructions[0];
			break;
		default:
			res = instructions[0]->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		}
		break;
	case kifMATCH:
		res = instructions[0]->Exec(kifNULL, dom, ke, idthread, callfunc);
		res = res->MatchPattern(instructions[1], dom, idthread, callfunc);
	}

	if (negation == true) {
		bool vbool = res->Boolean();
		res->Release();
		if (vbool)
			return kifFALSE;
		return kifTRUE;
	}

	return res;
}


//===================================================================

void KifInstructionSWITCH::Add(KifElement* ke) {
	if (adding)
		instructions.push_back(ke);
}

KifElement* KifInstructionSWITCH::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc) {

	//First our variable
	KifElement* var = instructions[0]->Exec(kifTRUE, dom, kifNULL, idthread, callfunc);
	//First case, we have only discrete values...
	if (usekeys) {
		//Then first, we get the string out of our variable...
		string s = var->String();
		if (keys.find(s) == keys.end()) {
			//we test if we have a default value...
			if (instructions[instructions.size() - 2] == kifDEFAULT) {
				return instructions.back()->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
			}
			return kifNULL;
		}
		return instructions[keys[s] + 1]->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
	}


	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	KifElement* result;

	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	//Then the instructions, the odd element is the value to compare with, the even element
	//the instruction to execute.
	int maxid = instructions.size();
	for (int i = 1; i < maxid; i += 2) {
		result = instructions[i]->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		ke = instructions[i + 1];
		if (_stacking != NULL) _stacking->Modify(ke);
		if (debugging && ke->type != kifInstructionSEQUENCE)
			debugging = DebugPtr()->Debugging(kifcode, this, ke, dom, (KifCallFunction*)kifGlobals->Topid(idthread));

		if (result == kifDEFAULT)
			return ke->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

		if (function == NULL) {
			if (var->same(result)->Boolean() == true) {
				result->Release();
				return ke->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
			}
		}
		else {
			if (applyfunc(var, result, (KifDomain*)function)->Boolean() == true) {
				result->Release();
				return ke->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
			}
		}
		result->Release();
		if (Errorid(idthread))
			return Errorptrid(idthread);
	}
	return kifNULL;
}

KifElement* KifInstructionIF::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc) {

	KifElement* result = instructions[0]->Exec(kifTRUE, dom, kifNULL, idthread, callfunc);

	if (Errorid(idthread)) {
		result->Release();
		return Errorptrid(idthread);
	}

	bool truevalue = result->Boolean();
	result->Release();

	if (truevalue != negation)
		result = instructions[1];
	else
	if (instructions.size() == 3)
		result = instructions[2];
	else
		return kifNULL;

	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(result);

	if (result->type != kifInstructionSEQUENCE && DebugPtr()->DebuggingMode())
		DebugPtr()->Debugging(kifcode, this, result, dom, (KifCallFunction*)kifGlobals->Topid(idthread));

	return result->Exec(contextualpattern, dom, ke, idthread, callfunc);
}


KifElement* KifInstructionUNTIL::Exec(KifElement* ktest, KifDomain* dom, KifElement* result, int idthread, KifCallFunction* callfunc) {

	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	ktest = instructions[0];
	KifElement* bloc = instructions[1];

	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);

	bool testing = true;
	do {
		if (debugging)
			debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

		result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

		if (Errorid(idthread)) {
			result->Release();
			return Errorptrid(idthread);
		}

		if (result->Returned())
			return result;

		if (result == kifBREAK)
			return kifTRUE;

		result->Release();
		result = ktest->Exec(kifTRUE, dom, kifNULL, idthread, callfunc);
		testing = result->Boolean();
		result->Release();
	} while (testing);

	return kifTRUE;
}

KifElement* KifInstructionWHILE::Exec(KifElement* ktest, KifDomain* dom, KifElement* result, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	result = instructions[0]->Exec(kifTRUE, dom, kifNULL, idthread, callfunc);

	if (result->Boolean() == false) {
		result->Release();
		return kifNULL;
	}

	ktest = instructions[0];
	KifElement* bloc = instructions[1];
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);

	while (result->Boolean()) {
		if (debugging)
			debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

		result->Release();
		result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

		if (Errorid(idthread)) {
			result->Release();
			return Errorptrid(idthread);
		}

		if (result->Returned())
			return result;

		if (result == kifBREAK)
			return kifTRUE;

		result->Release();
		result = ktest->Exec(kifTRUE, dom, kifNULL, idthread, callfunc);
	}
	result->Release();
	return kifTRUE;
}

//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------- Loops are pre-loaded to avoid modification drift in the orginal container

void KifElement::storeInLoop(KifElement** alls) {
	KifIterator it(NULL, NULL, false);
	it.kifcode = kifcode;
	ThreadLock _lock(type);
	it.Putvalue(this);
	it.Begin();
	KifElement* ke;
	int i = 0;
	while (it.End() == kifFALSE) {
		ke = it.IteratorValue();
		ke->Setreference();
		alls[i++] = ke;
		it.Next();
	}
	it.Clean();
}

void KifRawString::storeInLoop(KifElement** alls) {
	agnostring str(value);
	str.begin();
	KifString* k;

	int i = 0;
	while (!str.end()) {
		k = new KifBaseString(NULL, NULL, str.next());
		k->kifcode = kifcode;
		k->reference = 1;
		k->popped = 0;
		alls[i++] = k;
	}
}

void KifString::storeInLoop(KifElement** alls) {
	agnostring str(value);
	str.begin();
	KifString* k;

	int i = 0;
	while (!str.end()) {
		k = new KifBaseString(NULL, NULL, str.next());
		k->kifcode = kifcode;
		k->reference = 1;
		k->popped = 0;
		alls[i++] = k;
	}
}

void KifUString::storeInLoop(KifElement** alls) {
	KifUString* k;
	for (int i = 0; i < value.size(); i++) {
		k = new KifBaseUString(NULL, NULL, value[i]);
		k->kifcode = kifcode;
		k->reference = 1;
		k->popped = 0;
		alls[i++] = k;
	}
}

void KifEvaluate::storeInLoop(KifElement** alls) {
	KifIterator it(NULL, NULL, false);
	it.kifcode = kifcode;
	KifContainerLock _lock(this);

	it.Putvalue(this);
	it.Begin();
	KifElement* ke;
	int i = 0;
	while (it.End() == kifFALSE) {
		if (aMapContainer())
			ke = it.IteratorKey();
		else
			ke = it.IteratorValue();
		ke->Setreference();
		alls[i++] = ke;
		it.Next();
	}
	it.Clean();
}

void KifVector::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int sz = values.size();

	for (int i = 0; i < sz; i++) {
		values[i]->Setreference();
		alls[i] = values[i];
	}
}

void KifTable::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	for (int i = 0; i < size; i++) {
		values[i]->Setreference();
		alls[i] = values[i];
	}
}

void KifList::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	list<KifElement*>::iterator it;
	int i = 0;
	for (it = values.begin(); it != values.end(); it++) {
		(*it)->Setreference();
		alls[i++] = *it;
	}
}


void KifVectorString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int sz = values.size();

	KifElement* ke;
	for (int i = 0; i < sz; i++) {
		ke = new KifBaseString(NULL, NULL, values[i]);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i] = ke;
	}
}

void KifVectorUString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int sz = values.size();

	KifElement* ke;
	for (int i = 0; i < sz; i++) {
		ke = new KifBaseUString(NULL, NULL, values[i]);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i] = ke;
	}
}

void KifVectorByte::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int sz = values.size();

	KifElement* ke;
	for (int i = 0; i < sz; i++) {
		ke = new KifByte(NULL, NULL, values[i]);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i] = ke;
	}
}


void KifVectorInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int sz = values.size();

	KifElement* ke;
	for (int i = 0; i < sz; i++) {
		ke = new KifBaseInteger(NULL, NULL, values[i]);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i] = ke;
	}
}

void KifVectorFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);

	int sz = values.size();

	KifElement* ke;
	for (int i = 0; i < sz; i++) {
		ke = new KifBaseFloat(NULL, NULL, values[i]);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i] = ke;
	}
}

void KifSet::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);
	int i = 0;
	KifElement* ke;
	kifsetiterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = *it;
		ke->Setreference();
		alls[i++] = ke;
	}
}


void KifSetString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);
	int i = 0;
	KifElement* ke;
	std::set<string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseString(NULL, NULL, *it);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

void KifSetUString::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);
	int i = 0;
	KifElement* ke;
	std::set<wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseUString(NULL, NULL, *it);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

void KifSetInteger::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);
	int i = 0;
	KifElement* ke;
	std::set<long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseInteger(NULL, NULL, *it);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}

void KifSetFloat::storeInLoop(KifElement** alls) {
	KifContainerLock _lock(this);
	int i = 0;
	KifElement* ke;
	std::set<double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = new KifBaseFloat(NULL, NULL, *it);
		ke->kifcode = kifcode;
		ke->reference = 1;
		ke->popped = 0;
		alls[i++] = ke;
	}
}


KifBooleanLoop::KifBooleanLoop(KifElement* varout, KifElement* varin) : var(varout) {
	local_variable = NULL;
	if (varout->inType() == kifDomain) {
		local_variable = (KifDomain*)varout->Returnvalue();
		local_declarations = local_variable->declarations;
		local_variable->declarations.clear();
	}
	varout->Setreference();
	sz = varin->Size();
	alls = new KifElement*[sz + 1];
	varin->storeInLoop(alls);

	ite = 0;
}


KifElement* KifInstructionFORIN::Exec(KifElement* var, KifDomain* dom, KifElement* result, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	result = kifTRUE;

	result = instructions[0]->Instruction(0)->Thevariable(dom, idthread, callfunc);
	var = instructions[0]->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	//If it is a temporary variable, we need to protect it...
	var->Setreference();

	//very specific case, which involves a class instance on which we loop...
	//we do not want to lose its values

	KifBooleanLoop localloop(result, var);
	KifElement* bloc = instructions[1];
	KifLoopKey kloopkey(kifcode, &localloop, var->idgarbage);
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);

	while (localloop.Nextin(dom)) {
		if (debugging)
			debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

		result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

		if (Errorid(idthread) || result == kifBREAK) {
			result->Release();
			break;
		}

		if (result->Returned()) {
			var->Resetreference();
			return result;
		}

		result->Release();
	}

	var->Resetreference();
	return kifTRUE;
}

KifElement* KifInstructionFORVECTORIN::Exec(KifElement* var, KifDomain* dom, KifElement* dvector, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	dvector = kifTRUE;

	dvector = instructions[0]->Instruction(0);//the vector description
	var = instructions[0]->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc); //our vector in which we want to iterate...
	var->Setreference();
	if (var->type != kifVector)
		return kifcode->Returnerror(KIFLIBSTRINGS[268]);

	KifElement* bloc = instructions[1];
	KifElement* result;
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);
	KifVector* kvect = (KifVector*)var;
	for (int i = 0; i < kvect->values.size(); i++) {
		if (debugging)
			debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

		if (kvect->values[i]->Match((KifBaseVector*)dvector, dom, idthread, callfunc) == kifNOELEMENT) {
			var->Resetreference();
			return kifcode->Returnerror(KIFLIBSTRINGS[269]);
		}

		result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

		if (Errorid(idthread) || result == kifBREAK) {
			result->Release();
			break;
		}

		if (result->Returned()) {
			var->Resetreference();
			return result;
		}

		result->Release();

	}

	var->Resetreference();

	return kifTRUE;
}

KifElement* KifInstructionFORMAPIN::Exec(KifElement* var, KifDomain* dom, KifElement* dmap, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	dmap = kifTRUE;

	dmap = instructions[0]->Instruction(0);//the vector description
	var = instructions[0]->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc); //our vector in which we want to iterate...
	var->Setreference();
	if (!var->aMapContainer())
		return kifcode->Returnerror(KIFLIBSTRINGS[268]);

	KifElement* bloc = instructions[1];
	KifElement* result;
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);
	KifElement* key;
	KifElement* val;
	KifElement* kmap = var->Newinstance();
	kmap->popped = 1;

	KifIteration* itr = (KifIteration*)var->Newiterator(false);
	for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
		if (debugging)
			debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

		key = itr->IteratorKey();
		val = itr->IteratorValue();
		kmap->Push(key, val);
		key->Release();
		val->Release();
		if (kmap->Match((KifBaseMap*)dmap, dom, idthread, callfunc) == kifNOELEMENT) {
			var->Resetreference();
			itr->Release();
			kmap->Clear();
			kmap->Release();
			return kifcode->Returnerror(KIFLIBSTRINGS[269]);
		}
		kmap->Clear();

		result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

		if (Errorid(idthread) || result == kifBREAK) {
			result->Release();
			break;
		}

		if (result->Returned()) {
			var->Resetreference();
			itr->Release();
			kmap->Release();
			return result;
		}

		result->Release();
	}

	itr->Release();
	kmap->Release();
	var->Resetreference();
	return kifTRUE;
}

Exported KifElement* KifInstructionFORINRANGE::ExecuteShort(KifShort* var, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	KifElement* result = Instruction(0)->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	short start = result->Short();
	result->Release();
	result = Instruction(0)->Instruction(2)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	short limit = result->Short();
	result->Release();
	result = Instruction(0)->Instruction(3)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	short increment = result->Short();
	result->Release();

	if (increment == 0)
		return kifFALSE;

	short i;

	KifElement* bloc;

	bloc = instructions[1];
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);

	if (increment<0) {
		for (i = start; i>limit; i += increment) {
			var->value = i;
			if (debugging)
				debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

			result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

			if (Errorid(idthread) || result == kifBREAK) {
				result->Release();
				break;
			}

			if (result->Returned())
				return result;

			result->Release();
		}
	}
	else {
		for (i = start; i < limit; i += increment) {
			var->value = i;
			if (debugging)
				debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

			result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

			if (Errorid(idthread) || result == kifBREAK) {
				result->Release();
				break;
			}

			if (result->Returned())
				return result;

			result->Release();
		}
	}

	return kifTRUE;
}

Exported KifElement* KifInstructionFORINRANGE::ExecuteInteger(KifInteger* var, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	KifElement* result = Instruction(0)->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	long start = result->Integer();
	result->Release();
	result = Instruction(0)->Instruction(2)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	long limit = result->Integer();
	result->Release();
	result = Instruction(0)->Instruction(3)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	long increment = result->Integer();
	result->Release();

	if (increment == 0)
		return kifFALSE;

	long i;

	KifElement* bloc;

	bloc = instructions[1];
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);

	if (increment<0) {
		for (i = start; i>limit; i += increment) {
			var->value = i;
			if (debugging)
				debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

			result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

			if (Errorid(idthread) || result == kifBREAK) {
				result->Release();
				break;
			}

			if (result->Returned())
				return result;

			result->Release();
		}
	}
	else {
		for (i = start; i < limit; i += increment) {
			var->value = i;
			if (debugging)
				debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

			result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

			if (Errorid(idthread) || result == kifBREAK) {
				result->Release();
				break;
			}

			if (result->Returned())
				return result;

			result->Release();
		}
	}

	return kifTRUE;
}

Exported KifElement* KifInstructionFORINRANGE::ExecuteFloat(KifFloat* var, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	KifElement* result = Instruction(0)->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	double start = result->Float();
	result->Release();
	result = Instruction(0)->Instruction(2)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	double limit = result->Float();
	result->Release();
	result = Instruction(0)->Instruction(3)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	double increment = result->Float();
	result->Release();
	double i;

	if (increment == 0)
		return kifFALSE;

	KifElement* bloc;

	bloc = instructions[1];
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);

	if (increment<0) {
		for (i = start; i>limit; i += increment) {
			var->value = i;
			if (debugging)
				debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

			result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

			if (Errorid(idthread) || result == kifBREAK) {
				result->Release();
				break;
			}

			if (result->Returned())
				return result;

			result->Release();
		}
	}
	else {
		for (i = start; i < limit; i += increment) {
			var->value = i;
			if (debugging)
				debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

			result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

			if (Errorid(idthread) || result == kifBREAK) {
				result->Release();
				break;
			}

			if (result->Returned())
				return result;

			result->Release();
		}
	}

	return kifTRUE;
}

Exported KifElement* KifInstructionFORINRANGE::ExecuteLong(KifLong* var, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	KifElement* result = Instruction(0)->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	KIFLONG start = result->Long();
	result->Release();
	result = Instruction(0)->Instruction(2)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	KIFLONG limit = result->Long();
	result->Release();
	result = Instruction(0)->Instruction(3)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	KIFLONG increment = result->Long();
	result->Release();

	if (increment == 0)
		return kifFALSE;

	KIFLONG i;

	KifElement* bloc;

	bloc = instructions[1];
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);

	if (increment<0) {
		for (i = start; i>limit; i += increment) {
			var->value = i;
			if (debugging)
				debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

			result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

			if (Errorid(idthread) || result == kifBREAK) {
				result->Release();
				break;
			}

			if (result->Returned())
				return result;

			result->Release();
		}
	}
	else {
		for (i = start; i < limit; i += increment) {
			var->value = i;
			if (debugging)
				debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

			result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

			if (Errorid(idthread) || result == kifBREAK) {
				result->Release();
				break;
			}

			if (result->Returned())
				return result;

			result->Release();
		}
	}

	return kifTRUE;
}


KifElement* KifInstructionFORINRANGE::Exec(KifElement* var, KifDomain* dom, KifElement* result, int idthread, KifCallFunction* callfunc) {

	result = instructions[0]->Instruction(0)->Thevariable(dom, idthread, callfunc);
	var = ((KifVariable*)result)->value;
	//If it is a temporary variable, we need to protect it...
	switch (var->type) {
	case kifInteger:
		return ExecuteInteger((KifInteger*)var, dom, idthread, callfunc);
	case kifFloat:
		return ExecuteFloat((KifFloat*)var, dom, idthread, callfunc);
	case kifLong:
		return ExecuteLong((KifLong*)var, dom, idthread, callfunc);
	case kifShort:
		return ExecuteShort((KifShort*)var, dom, idthread, callfunc);
	}
	return kifTRUE;
}


KifElement* KifInstructionFILEIN::Exec(KifElement* var, KifDomain* dom, KifElement* result, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;


	result = kifTRUE;

	var = instructions[0]->Instruction(0)->Thevariable(dom, idthread, callfunc);
	KifBaseFile* kfile = (KifBaseFile*)((KifInstance*)Instruction(0)->Instruction(1))->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	kfile->initwhile(var);
	result = kfile->whilein(var);
	if (result->Boolean() == false)
		return kifNULL;

	KifElement* bloc = instructions[1];
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);

	while (result->Boolean() != false) {
		if (debugging)
			debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

		result = bloc->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

		if (Errorid(idthread)) {
			result->Release();
			return Errorptrid(idthread);
		}

		if (result->Returned())
			return result;

		if (result == kifBREAK)
			return kifTRUE;

		result->Release();
		result = kfile->whilein(var);
	}

	return kifTRUE;
}

KifElement* KifInstructionSEQUENCE::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	bin_hash<KifElement*>* stacks = NULL;
	KifElement* res = kifTRUE;
	KifDomain* localdom = dom;
	//id_run_as_thread is the name of the encapsulating thread function which is launched with KIFKIFSTRINGS[28] (see KifFrame::RunThread below)
	//In that case, we should not consider this function as one. The referent frame is then dom
	if (callfunc != NULL && callfunc->Name() != id_run_as_thread)
		localdom = callfunc;
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();

	int maxid = instructions.size();
	for (size_t i = 0; i < maxid; i++) {
		ke = instructions[i];

		if (_stacking != NULL) _stacking->Modify(ke);

		if (debugging)
			debugging = DebugPtr()->Debugging(kifcode, this, ke, dom, callfunc);

		if (ke->Variable()) {
			if (stacks == NULL)
				stacks = new bin_hash<KifElement*>;
			(*stacks)[ke->Name()] = localdom->Declaration(ke->Name());
			ke->Exec(contextualpattern, localdom, kifNULL, idthread, callfunc)->Setlocalvariable();
		}
		else
			res = ke->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

		if (res->Returned())
			break;

		if (Errorid(idthread)) {
			res->Release();
			res = Errorptrid(idthread);
			break;
		}

		if (res == kifBREAK || res == kifCONTINUE)
			break;
		res->Release();
		res = kifTRUE;
	}

	//We then clean the variables that were created locally...
	//We forbid at the compiling level, the fact that a local variable would share the same name
	//as another variable declared above this sequence, this in order to simplify the process
	//of creating and removing these local variables... Futhermore, a SEQUENCE cannot call itself recursively
	//so we do not need to keep a complex track of which was what...

	if (stacks != NULL) {
		localdom->Resetdeclaration(*stacks);
		delete stacks;
	}
	//TO keep the Returnvalue protected, we need to call Protect... Returned() already do the same
	res->Protect();
	return res;
}

KifElement* KifInstructionTRY::Exec(KifElement* ke, KifDomain* dom, KifElement* res, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	int last = instructions.size() - 1;
	bool catchbloc = false;
	if (instructions[last]->Action() == kifCATCHBLOC) {
		last--;
		catchbloc = true;
	}

	for (size_t i = 0; i < last; i++) {
		ke = instructions[i];
		if (debugging && ke->type != kifInstructionSEQUENCE)
			debugging = DebugPtr()->Debugging(kifcode, this, ke, dom, callfunc);

		res = ke->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

		if (Errorid(idthread)) {
			res->Release();
			instructions[last]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			res = kifFALSE;
			if (catchbloc)
				res = instructions[last + 1]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			return res;
		}

		if (res->Returned())
			return res;

		if (res == kifBREAK || res == kifCONTINUE)
			return res;
		res->Release();
	}

	return kifTRUE;
}

KifElement* KifInstructionFOR::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* var, int idthread, KifCallFunction* callfunc) {
	bool debugging = kifGlobals->debuginfocurrent->kifdebugging;

	KifElement* bloc;

	//Initialisation

	KifElement* test = kifTRUE;


	//Initialisation
	instructions[0]->Exec(kifTRUE, dom, kifNULL, idthread, callfunc);
	//then test
	test = instructions[1]->Exec(kifTRUE, dom, kifNULL, idthread, callfunc);

	var = instructions[2];
	KifOperator act = var->Action();
	if (act == kifPLUSPLUS || act == kifMINUSMINUS) {
		var = instructions[2]->Instruction(0)->Thevariable(dom, idthread, callfunc);
		var = var->Returnvalue();
	}

	//A simple test, which does not require any complex comparison
	KifElement* res = kifNULL;
	bloc = instructions[3];
	Stackelement* _stacking = kifGlobals->THR[idthread]->Getlastelement();
	if (_stacking != NULL) _stacking->Modify(bloc);

	while (test->Boolean()) {
		//then the body		
		if (debugging)
			debugging = DebugPtr()->Debugging(kifcode, this, bloc, dom, callfunc);

		res = bloc->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

		if (Errorid(idthread)) {
			res->Release();
			return Errorptrid(idthread);
		}

		if (res->Returned())
			return res;

		if (res == kifBREAK)
			return kifTRUE;

		res->Release();

		switch (act) {
		case kifPLUSPLUS:
			var->plusplus();
			break;
		case kifMINUSMINUS:
			var->minusminus();
			break;
		default:
			var->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		}

		//then the test again
		test = instructions[1]->Exec(kifTRUE, dom, kifNULL, idthread, callfunc);
	}

	return kifTRUE;
}

//===================================================================

void KifCode::Run(int from) {
#ifdef WIN32
	set_terminate(termination_function);
#endif
	kstart->Run(from);
}


Exported string KifElement::Type() {
	return kifGlobals->kifStringType[type];
}

Exported KifFraction* KifElement::Fraction() {
	KifBaseFloat f(NULL, NULL, Float());
	f.kifcode = kifcode;
	return f.Fraction();
}

Exported KifFraction* KifInteger::Fraction() {
	return new KifFraction(kifcode, NULL, value, 1);
}

Exported KifFraction* KifByte::Fraction() {
	return new KifFraction(kifcode, NULL, value, 1);
}

Exported KifFraction* KifShort::Fraction() {
	return new KifFraction(kifcode, NULL, value, 1);
}

Exported KifFraction* KifLong::Fraction() {
	return new KifFraction(kifcode, NULL, value, 1);
}

Exported KifFraction* KifFloat::Fraction() {
	KIFLONG denom = (long)value;
	if (denom == value)
		return new KifFraction(kifcode, NULL, denom, 1);
	double mantissa = value;
	//nb contains the number of digits of value...
	short nb = log10(fabs(value));
	KIFLONG num = 1;
	//We limit the calculus to 29 digits after the point.
	//The mantissa of a double is encoded on 52 bits, which is roughly 15 digits...
	while (nb < 15 && mantissa != (long)mantissa)  {
		mantissa *= 10;
		nb++;
		num *= 10;
	}
	denom = mantissa;
	//In the case of a full mantissa, we use another version of the transformation
	//namely we try to approximate with the smallest fraction possible...
	char ddenom = 0;
	char dnum = 0;
	if (nb == 15) {
		KIFLONG p1 = computeEuclidian(denom, num);
		KIFLONG p2 = computeEuclidian(denom + 1, num);
		if (p1 < p2) {
			ddenom = 1;
			dnum = 0;
			p1 = p2;
		}
		p2 = computeEuclidian(denom, num + 1);
		if (p1 < p2) {
			ddenom = 0;
			dnum = 1;
			p1 = p2;
		}
		p2 = computeEuclidian(denom - 1, num);
		if (p1 < p2) {
			ddenom = -1;
			dnum = 0;
			p1 = p2;
		}
		p2 = computeEuclidian(denom, num - 1);
		if (p1 < p2) {
			ddenom = 0;
			dnum = -1;
		}
		denom += ddenom;
		num += dnum;
	}

	KifFraction* f = new KifFraction(kifcode, NULL, denom, num);
	f->FractionSimplify(true);
	return f;
}


//-------------------------------------------------------------------------------------------
Exported KifElement* KifInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself) {
		value <<= b->Integer();
		return this;
	}
	KIFLONG v = a->Long() << b->Integer();
	if (isLong(v))
		return new KifLong(kifcode, NULL, v);
	return kifcode->Provideinteger(v);
}


Exported KifElement* KifInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself) {
		value *= b->Integer();
		return this;
	}
	if (b->type == kifFloat || b->type == kifFraction) {
		double v = a->Float()*b->Float();
		return kifcode->Providefloat(v);
	}

	KIFLONG v = a->Integer()*b->Integer();
	if (isLong(v))
		return new KifLong(kifcode, NULL, v);
	return kifcode->Provideinteger(v);
}

Exported KifElement* KifLong::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself) {
		value *= b->Long();
		return this;
	}

	if (b->type == kifFloat) {
		double i = a->Float()*b->Float();
		return kifcode->Providefloat(i);
	}

	KIFLONG i = a->Long()*b->Long();
	return new KifLong(kifcode, NULL, i);
}

Exported KifElement* KifShort::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself) {
		value *= b->Short();
		return this;
	}

	if (b->type == kifFloat) {
		double i = a->Float()*b->Float();
		return kifcode->Providefloat(i);
	}

	short i = a->Short()*b->Short();
	return new KifShort(kifcode, NULL, i);
}

//----------------------------------------------------------------------------------
Exported KifElement* KifCouple::plusplus() {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;
	KifElement* ke = Exec(this, domain, kifNULL, idthread, callfunc);
	ke->plusplus();
	if (!ke->reference)
		Setvalue(ke);
	return ke;
}

Exported KifElement* KifCouple::minusminus() {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;

	KifElement* ke = Exec(this, domain, kifNULL, idthread, callfunc);
	ke->minusminus();

	if (!ke->reference)
		Setvalue(ke);
	return ke;
}

Exported KifElement* KifCouple::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;
	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	v = v->plus(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

Exported KifElement* KifCouple::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;

	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);

	v = v->minus(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

Exported KifElement* KifCouple::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;
	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);

	v = v->orset(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

Exported KifElement* KifCouple::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;
	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);

	v = v->andset(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

Exported KifElement* KifCouple::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;
	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);

	v = v->xorset(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

Exported KifElement* KifCouple::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;
	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);

	v = v->multiply(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

Exported KifElement* KifCouple::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;
	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);

	v = v->divide(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

Exported KifElement* KifCouple::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;
	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);

	v = v->mod(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

Exported KifElement* KifCouple::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;
	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);

	v = v->shiftleft(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

Exported KifElement* KifCouple::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;
	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);

	v = v->shiftright(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

Exported KifElement* KifCouple::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (recipient->type == kifTuple)
		return kifcode->Returnerror(KIFKIFSTRINGS[29]);

	threadblocinit;
	returnvalue = true;

	KifElement* v = a->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
	b = b->Exec(kifNULL, domain, kifNULL, idthread, callfunc);

	v = v->power(v, b, context, autoself);

	if (!v->reference && autoself)
		a->Setvalue(v);

	b->Release();
	return v;
}

char KifIndex::Indexes(int sz, KifDomain* dom, int& ileft, int& iright) {
	threadblocinit;

	KifElement* keyleft = key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	KifElement* keyright = NULL;
	if (interval == true)
		keyright = right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

	ileft = keyleft->Integer();
	keyleft->Release();
	if (keyright != NULL) {
		iright = keyright->Integer();
		keyright->Release();
	}

	if (ileft < 0)
		ileft += sz;

	if (ileft < 0 || ileft >= sz)
		return 0;

	if (keyright == NULL)
		return 1;

	if (iright <= 0 || keyright == kifNULL) {
		iright += sz;
		if (iright<ileft)
			return 0;
	}
	else {
		if (iright>sz)
			return 0;
	}
	return 2;
}



