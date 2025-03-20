/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the XIP library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : XIP
Version    : 10.00
filename   : kif.h
Date       : 09/09/2010
Purpose    : Implementation of the KiF language
Programmer : Claude ROUX
Reviewer   :
*/


#ifndef i_kifbase
#define i_kifbase

#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <locale>

using std::stringstream;
using std::string;
using std::map;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::list;
using std::ios;
using std::ostream;
using std::istream;
using std::fstream;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::ostringstream;
using std::min;
using std::max;
using std::pair;
using std::size_t;
using std::ptrdiff_t;


#ifdef KIF_HASH_MAP
#ifdef WIN32 
#include <unordered_map>
#define hmap unordered_map
using std::unordered_map;
#define Rend()	rend()
#define Rbegin()	rbegin()
#else
#ifdef MAVERICK
#include <unordered_map>
#define hmap unordered_map
using std::unordered_map;
#else
#include <tr1/unordered_map>
#define hmap std::tr1::unordered_map
#endif
#define Rend()	end()
#define Rbegin()	begin()
#endif
#else
#define hmap map
#define Rend()	rend()
#define Rbegin()	rbegin()
#endif



#define KIFPREDICATEENTREE -1000

const short locktablesize = 500;
const short idnull = 0;
const short idthis = 1;
const short idsequence = 2;
const short idcatch = 3;
const short idself = 4;
const short idin = 5;
const short id_call = 6;
const short id_initial = 7;
const short idsize = 8;
const short idinferior = 9;
const short idsuperior = 10;
const short idequal = 11;
const short iddifferent = 12;
const short idinferiorequal = 13;
const short idsuperiorequal = 14;
const short idmethods = 15;
const short idvector = 16;
const short idmap = 17;
const short idapply = 18;
const short idsynchronous = 19;
const short idget = 20;
const short idstring = 21;
const short idint = 22;
const short idlong = 23;
const short idfloat = 24;
const short idboolean = 25;
const short idfraction = 26;
const short idorset = 27;
const short idandset = 28;
const short idxorset = 29;
const short idplusplus = 30;
const short idminusminus = 31;
const short idplus = 32;
const short idminus = 33;
const short idmultiply = 34;
const short iddivide = 35;
const short idmod = 36;
const short idshiftright = 37;
const short idshiftleft = 38;
const short idpower = 39;
const short idinterval = 40;
const short idindex = 41;
const short idtrue = 42;
const short idfalse = 43;

const short idpath = 44;
const short idargs = 45;
const short idOS = 46;
const short idstart = 47;
const short idsep = 48;
const short idloader = 49;
const short idpop = 50;

const short id_pi = 51;
const short idpichar = 52;
const short id_e = 53;

const short idstdin = 54;

const short idfor = 55;
const short idwhile = 56;
const short idtry = 57;
const short idswitch = 58;
const short idif = 59;

const short id_final = 60;

const short id_run_as_thread = 61;
const short idloadin = 62;
const short idendl = 63;
const short idnoelement = 64;

const short idsmap = 65;
const short idimap = 66;
const short idfmap = 67;
const short idsvector = 68;
const short idivector = 69;
const short idfvector = 70;

const short idthecut = 71;
const short idtrace = 72;
const short idfail = 73;
const short iduniversal = 74;
const short idasserta = 75;
const short idassertz = 76;
const short idassertdb = 77;
const short idretract = 78;
const short idretractdb = 79;
const short idponder = 80;
const short idpipe = 81;
const short idpredicatemode = 82;


const short KIF_PROTECT_REFERENCE = 1000;

#define kifUNIVERSAL kifGlobals->gUNIVERSAL
#define kifNOELEMENT kifGlobals->gNOELEMENT
#define kifNULL kifGlobals->gNULL
#define kifTRUE kifGlobals->gTRUE
#define kifRAISEERROR kifGlobals->gRAISEERROR
#define kifFALSE kifGlobals->gFALSE
#define kifMINUSONE kifGlobals->gMINUSONE
#define kifONE kifGlobals->gONE
#define kifDEFAULT kifGlobals->gDEFAULT
#define kifBREAK kifGlobals->gBREAK
#define kifBREAKFALSE kifGlobals->gBREAKFALSE
#define kifBREAKTRUE kifGlobals->gBREAKTRUE
#define kifBREAKPOINT kifGlobals->gBREAKPOINT
#define kifCONTINUE kifGlobals->gCONTINUE
#define kifNEG kifGlobals->gNEG
#define kifZERO kifGlobals->gZERO
#define kifFAIL kifGlobals->gfail
#define kifCUT kifGlobals->gcut

#define kifEMPTYSTRING kifGlobals->gEMPTYSTRING
#define kifTypeSTRING kifGlobals->gTypeSTRING
#define kifTypeLONG kifGlobals->gTypeLONG
#define kifTypeFRACTION kifGlobals->gTypeFRACTION
#define kifTypeINTEGER kifGlobals->gTypeINTEGER
#define kifTypeFLOAT kifGlobals->gTypeFLOAT
#define kifTypeBOOLEAN kifGlobals->gTypeBOOLEAN

#define _PREDGET 0
#define _PREDSTORE 1
#define _PREDREMOVE 2

#define _PREDNONE 0
#define _PREDTESTED 1
#define _PREDLOADED 2

#define PREDICATEDICO -999

const unsigned int P_NONE = 1 << 0;
const unsigned int P_ONE = 1 << 1;
const unsigned int P_TWO = 1 << 2;
const unsigned int P_THREE = 1 << 3;
const unsigned int P_FOUR = 1 << 4;
const unsigned int P_FIVE = 1 << 5;
const unsigned int P_SIX = 1 << 6;
const unsigned int P_SEVEN = 1 << 7;
const unsigned int P_EIGHT = 1 << 8;
const unsigned int P_NINE = 1 << 9;
const unsigned int P_TEN = 1 << 10;
const unsigned int P_ELEVEN = 1 << 11;
const unsigned int P_TWELVE = 1 << 12;
const unsigned int P_THIRTEEN = 1 << 13;
const unsigned int P_FOURTEEN = 1 << 14;
const unsigned int P_FIFTEEN = 1 << 15;

const unsigned int P_FULL = -1;

const unsigned int P_ATLEASTONE = P_FULL^P_NONE;
const unsigned int P_ATLEASTTWO = P_ATLEASTONE^P_ONE;
const unsigned int P_ATLEASTTHREE = P_ATLEASTTWO^P_TWO;
const unsigned int P_ATLEASTFOUR = P_ATLEASTTHREE^P_THREE;
const unsigned int P_ATLEASTFIVE = P_ATLEASTFOUR^P_FOUR;
const unsigned int P_ATLEASTSIX = P_ATLEASTFIVE^P_FIVE;
const unsigned int P_ATLEASTSEVEN = P_ATLEASTSIX^P_SIX;
const unsigned int P_ATLEASTEIGHT = P_ATLEASTSEVEN^P_SEVEN;
const unsigned int P_ATLEASTNINE = P_ATLEASTEIGHT^P_EIGHT;
const unsigned int P_ATLEASTTEN = P_ATLEASTNINE^P_NINE;
const unsigned int P_ATLEASTELEVEN = P_ATLEASTTEN^P_TEN;
const unsigned int P_ATLEASTTWELVE = P_ATLEASTELEVEN^P_ELEVEN;
const unsigned int P_ATLEASTTHIRTEEN = P_ATLEASTTWELVE^P_TWELVE;
const unsigned int P_ATLEASTFOURTEEN = P_ATLEASTTHIRTEEN^P_THIRTEEN;
const unsigned int P_ATLEASTFIFTEEN = P_ATLEASTFOURTEEN^P_FOURTEEN;

void KifSetMainThread();
bool KifIsMainThread();

#ifdef C11
#include <thread>
#include <mutex>
#include <condition_variable>
#define threadhandle std::thread::id
#define THREADPTR std::thread*
#define _GETTHREADID() std::this_thread::get_id()
#define KIFLONG long long
#define KIFULONG unsigned long long
#else
#ifdef WIN32
#define threadhandle DWORD
#define KIFLONG __int64
#define KIFULONG unsigned __int64
#define THREADPTR HANDLE
#define _GETTHREADID() GetCurrentThreadId()
#else
#include <pthread.h>
#include <semaphore.h>
#define threadhandle pthread_t
#define THREADPTR pthread_t
#define KIFLONG long long
#define KIFULONG unsigned long long
#define _GETTHREADID() pthread_self()
#endif
#endif

const KIFULONG KIFLONGONE = 1;

#include "kifvecte.h"
#include "xipvecta.h"
#include "conversion.h"

#define kifNOTINITIALIZED -100
#define ckifInteger 1
#define ckifFloat 2
#define ckifIntFloat 3
#define ckifString 4
#define ckifInstance 8
#define ckifInstruction 16

#define kifNoOwner -2
#define kifStringOwner 1
#define kifIntegerOwner 2
#define kifFloatOwner 4
#define kifVectorOwner 8
#define kifListOwner 16
#define kifMapOwner 32
#define kifGarbageOwner 64
#define staticval 2

Exported short KifNewId(string n);
Exported short KifStringId(string n);
Exported string KifIdString(short n);
Exported void KifRemoveId(short n);

Exported bool Garbaging();
Exported void InitMapBooleanString(hmap<short, bool>& themap, string s, bool method = true);

#include "localisations.h"

typedef double(*mathFunc)(double);

class KifCallExecuteThread;
class KifAutomaton;
class KifDomain;
class KifLoopKey;
class KifBaseFrame;
class KifInstruction;
class KifGlobalThread;
class KifString;
class KifCouple;
class KifIndex;
class KifInteger;
class KifFloat;
class KifChild;
class KifComputeElement;
class KifRoot;
class KifContainerLock;
class KifVector;
class KifMap;
class KifList;
class KifElement;
class KifPredicateVariable;
class KifComputeValue;
class KifPredicateVariableInstance;
class KifFunction;
class KifFunctionLambda;
class KifCallFunction;
class KifPredefined;
class x_node;
class x_reading;
class KifCallMethod;
class KifCallFunctionGeneral;
class KifVariable;
class KifTree;
class TestFonctionXML;
class VariableXML;
class KifXipRule;
class VectTestFonction;
class KifVariableDeclaration;
class KifPredicate;
class KifPredicateFunction;
class KifPredicateRule;
class KifPredicateTerm;
class KifStaticVariable;
class KifDomainInstruction;
class KifVectorString;
class KifDomainLambda;
//We define the basic KIF element, which is KifObject
class KifCode;
class KifError;
class KifBoolean;
class KifObject;
class KifFrame;
class KifFraction;
class bnf_kif;
class KifPredicateContainer;
class KifReturnValue;
class KifBaseVector;
class KifBaseMap;
class KifCallFunctionHaskell;

#ifdef WIN32
#define Exported __declspec(dllexport)
#define Imported __declspec(dllimport)
#include <WinSock.h>
#else
#define Exported
#define Imported
#endif

KifVector* Selectvector(KifElement* kref, KifElement* context);
KifElement* Selectavector(KifElement* kref, KifElement* context);
KifMap* Selectmap(KifElement* kref, KifElement* context);
KifElement* Selectamap(KifElement* kref, KifElement* context);
KifElement* Selectvectorstring(KifElement* kref, KifElement* context);
KifElement* Selectmapstring(KifElement* kref, KifElement* context);
KifElement* Selectvectorinteger(KifElement* kref, KifElement* context);
KifElement* Selectmapinteger(KifElement* kref, KifElement* context);
KifElement* Selectvectorfloat(KifElement* kref, KifElement* context);
KifElement* Selectmapfloat(KifElement* kref, KifElement* context);

extern Exported const char* KIFSTRINGS[];
extern Exported const char* KIFRAWSTRINGS[];
extern Exported const char* KIFLIBSTRINGS[];
extern Exported const char* LIBKIFSTRINGS[];

string Getfilename(string n);


typedef void(*Kifterminate)();
typedef KifElement* (*Predefined)(KifElement*, KifDomain*, KifCallFunction*, int);

//Needed to use external libraries
typedef KifElement* (*KifExternalParse)(KifCode*, x_node*, KifElement*);
typedef bool(*KifExternalModule)(KifCode*, string);
typedef bool(*KifExternalTerminateModule)(KifCode*);


Exported bool Error();
Exported bool Errorid(int id);
Exported void Seterror(bool v);
Exported KifElement* Errorptr();
Exported KifElement* Errorptrid(int id);
Exported string Errorstring();
Exported string KifErrorstring(int id);
Exported void Cleanerror();
Exported void Cleanallerror();


class KifRaiseError {
public:

	string message;
	string filename;
	long left;
	long right;

	KifRaiseError(string mes, string file = "", long l = 0, long r = 0);
};

typedef enum {
	kifVOID, kifnull, kifthis, kifInDebug, kifAny, kifBaseString, kifStringPtr,
	kifString, kifInteger, kifFloat, kifPredicateContainer, kifTable,
	kifList,
	kifVector, kifVectorString, kifVectorInteger, kifVectorFloat,

	kifMap, kifMapString, kifMapInteger, kifMapFloat,
	kifIntegerMap, kifMapIntegerString, kifMapIntegerInteger, kifMapIntegerFloat,
	kifFloatMap, kifMapFloatString, kifMapFloatInteger, kifMapFloatFloat,

	kifTreeMap, kifTreeMapString, kifTreeMapInteger, kifTreeMapFloat,
	kifIntegerTreeMap, kifTreeMapIntegerString, kifTreeMapIntegerInteger, kifTreeMapIntegerFloat,
	kifFloatTreeMap, kifTreeMapFloatString, kifTreeMapFloatInteger, kifTreeMapFloatFloat,

	kifPrimeMap, kifPrimeMapString, kifPrimeMapInteger, kifPrimeMapFloat,
	kifIntegerPrimeMap, kifPrimeMapIntegerString, kifPrimeMapIntegerInteger, kifPrimeMapIntegerFloat,
	kifFloatPrimeMap, kifPrimeMapFloatString, kifPrimeMapFloatInteger, kifPrimeMapFloatFloat,

	kifSet, kifSetString, kifSetInteger, kifSetFloat,

	kifBoolean, kifID, kifLocalDomain, kifPredicateLocalInstruction, kifPredicateVariableInvolved, kifPredicateCut, kifFinalPredicate, kifPredicateVariable,
	kifPredicateVariableInstance, kifPredicateTerm, kifByte, kifBit, kifBits, kifPredicate, kifPredicateKnowledgeBase, kifPredicateDatabase,
	kifLong, kifFraction, kifLock, kifNamedLoquet, kifPrint, kifPString, kifPInteger, kifPFloat, kifInfinitiveVector, kifCycleVector, kifReplicateVector,
	kifCouple, kifDomain, kifPredicateDomain, kifDomainLambda, kifXML, kifXMLDOC, kifLoquet, kifLoquetThread, kifFrameDefined,
	kifVariableDeclaration, kifFrameDeclaration, kifFunctionDeclaration, kifFunctionLambda, kifStaticVariable,
	kifFile, kifFileWide, kifIndex, kifPredefined, kifSelf, kifLoader, kifPredicateRule, kifPredicateRuleElement,
	kifFrame, kifSubframe, kifIterator, kifVectorPtr, kifMapPtr, kifBaseMap, kifBaseVector, kifBaseVectorMerge, kifBaseMapMerge,
	kifVariable, kifPrepareError, kifIteration, kifCall, kifDate, kifTime,
	kifCallMethod, kifLOOPITERATOR, kifKiF, kifTree, kifTemporary, kifGlobalStack,
	kifStack, kifLocal, kifLoading, kifThreading, kifSignal, kifThreadId, kifInputchr, kifGarbage, kifGarbaging, kifError,
	kifInstruction, kifInstructionPARAMETER, kifInstructionIF,
	kifFST, kifNTM, kifParser, kifXipNode, kifGraph, kifXipDependency, kifXipGeneration, kifXMLNodePtr,
	kifInstructionSWITCH, kifInstructionTRY, kifInstructionWHILE, kifInstructionUNTIL,
	kifInstructionINITIALISATION, kifInstructionAFFECTATION, kifInstructionAPPLYOPERATION, kifInstructionAPPLYOPERATIONEQU, kifInstructionCOMPARE,
	kifInstructionPLUSPLUS, kifInstructionMINUSMINUS, kifInstructionAND, kifInstructionOR, kifInstructionMERGEEQU, kifInstructionMERGE,
	kifInstructionADDTOLIST, kifInstructionADDTOLISTEQU, kifInstructionKIFIN, kifInstructionKIFON,
	kifInstructionFORINRANGE, kifInstructionFORIN, kifInstructionFORVECTORIN, kifInstructionFORMAPIN, kifDISPLAY,
	kifInstructionFOR, kifInstructionPredicate, kifInstructionSEQUENCE, kifXPathExpression, kifInstanceFunction,
	kifCallFunction, kifCallFunctionBCR, kifCallExecuteThreadExternal, kifCallFunctionExternal, kifCallExecuteThreadPredefined, kifCallFunctionPredefined,
	kifCallFunctionHaskell, kifCallExecuteThread, kifCallFunctionThread, kifCallFunctionCall, kifCallFunctionGeneral,
	kifDebuggingLock, kifInstance, kifFunction, kifPolynomial, kifProtectedFunction, kifThread, kifEnum, kifAutomaton, kifReturnValue, kifLASTYPE
} KifType;


typedef enum {
	PRED_NOACTION = 0, PRED_WEIGHT = 1, PRED_WEIGHTSTACK = 2, PRED_NORMALIZE_WEIGHT = 4, PRED_RANDOMIZE = 8
} KifPredicateMode;


#define EXECUTABLE(xt) (xt>=kifInstruction && xt<=kifInstance)
#define CALLFUNCTION(xt) (xt->type>=kifCallFunction && xt->type<=kifCallFunctionGeneral)

//typedef enum  {kifNOTYPE,kifEQU,kifFULLINTEGER,kifFULLFLOAT,kifFULLSTRING} KifInstructionType;
#define kifNOTYPE 0
#define kifEQU 1
#define kifFULLINTEGER 2
#define kifFULLFLOAT 3
#define kifFULLSTRING 4
#define kifENUM 5
#define KifInstructionType char

typedef enum { kinUnknown, kinKnown, kinFrame, kinFunction, kinGlobal, kinConstant, kinCall, kinPreviousCall, kinCallFrame, kinPreviousCallFrame } KifOrigin;

const long szlonglong = 5;

class kifcompatibletype {
public:

	hmap<long, unsigned long> types;
	hmap<long, unsigned long>::iterator it;

	long bitindex(long position) {
		return position >> szlonglong; //16 bits
	}

	long bitposition(long position, long bitidx) {
		return (position - (bitidx << szlonglong));
	}

	void values(vector<KifType>& v);

	void clear() {
		types.clear();
	}

	void set(long i) {
		long rg = bitindex(i);
		long pos = bitposition(i, rg);
		unsigned long x = 1;
		x <<= pos;
		if (types.find(rg) == types.end())
			types[rg] = x;
		else
			types[rg] |= x;
	}

	bool check(long i) {
		long rg = bitindex(i);
		if (types.find(rg) == types.end())
			return false;
		long pos = bitposition(i, rg);
		unsigned long x = 1;
		x <<= pos;
		if ((x&types[rg]) == x)
			return true;
		return false;
	}

	bool compare(kifcompatibletype& kf) {
		for (it = types.begin(); it != types.end(); it++) {
			if (kf.types.find(it->first) == kf.types.end())
				return false;
			if ((types[it->first] & kf.types[it->first]) != types[it->first])
				return false;
		}
		return true;
	}
};


typedef enum  {
	kifNONE,
	kifINITIALISATION,

	kifAFFECTATION,
	kifSTREAM,
	kifPLUSEQU,
	kifMINUSEQU,
	kifMULTIPLYEQU,
	kifDIVIDEEQU,
	kifMODEQU,
	kifPOWEREQU,
	kifSHIFTLEFTEQU,
	kifSHIFTRIGHTEQU,
	kifMERGEEQU,
	kifADDEQU,
	kifUNIONEQU,
	kifXOREQU,
	kifINTERSECTIONEQU,

	kifEQUAL,
	kifSUPERIOR,
	kifINFERIOR,
	kifSUPEQU,
	kifINFEQU,
	kifDIFFERENT,

	kifPLUS,
	kifMINUS,
	kifMULTIPLY,
	kifDIVIDE,
	kifMOD,
	kifPOWER,
	kifSHIFTLEFT,
	kifSHIFTRIGHT,
	kifMERGE,
	kifADD,
	kifUNION,
	kifXOR,
	kifINTERSECTION,

	kifPLUSPLUS,
	kifMINUSMINUS,

	kifIN,
	kifNOTIN,
	kifON,

	kifMATCH,

	kifBLOC,
	kifBLOCLOOPIN,
	kifFILEIN,
	kifBLOCBOOLEAN,
	kifPARAMETER,
	kifSEQUENCE,
	kifOR,
	kifAND,
	kifIF,
	kifTRY,
	kifSWITCH,
	kifWHILE,
	kifFOR,
	kifCATCHBLOC,
	kifPREDICATEELEMENT,
	kifLASTDECLARATION
} KifOperator;



//------------------------------------------------------------------------------------------
#ifdef C11
class ThreadLock {
public:

	bool garbaging;
	std::lock_guard<std::recursive_mutex>* relax;
	std::lock_guard<std::recursive_mutex>* relaxgarbage;
	Exported void Initialise();
	ThreadLock(KifType i);
	~ThreadLock();
};

#define kifmutex(x,y) std::recursive_mutex y; std::recursive_mutex* x=&y
#define externkifmutex(x,y) extern std::recursive_mutex y; extern std::recursive_mutex* x
#define KifCreateMutex(x)
#define KifDeleteMutex(x)

class KifThreadLock {
public:
	std::recursive_mutex* lock;
	bool locked;
	int idthread;
	bool recursive;

	Exported KifThreadLock(std::recursive_mutex* l, bool run = true, bool init = true);
	Exported void Initialize();
	Exported void Locking();
	Exported void Unlocking();
	Exported ~KifThreadLock();
};

class LockedThread {
public:
	std::condition_variable lock;
	std::mutex mtx;
	bool released;

	Exported LockedThread() { released = false; }

	Exported ~LockedThread() {}
	Exported void Blocked();
	Exported void Released();
};
#else
#ifdef WIN32
class ThreadLock {
public:
	bool garbaging;
	int relax;
	Exported void Initialise();
	Exported ThreadLock(KifType i);

	Exported ~ThreadLock();
};


#define kifmutex(x,y) HANDLE x=NULL
#define externkifmutex(x,y) extern HANDLE x
#define KifCreateMutex(x) x=CreateMutex(NULL,FALSE,NULL)
#define KifDeleteMutex(x) CloseHandle(x)

class KifThreadLock {
public:
	HANDLE lock;
	bool locked;
	int idthread;

	Exported KifThreadLock(HANDLE l, bool run = true, bool init = true);
	Exported void Initialize();
	Exported void Locking();
	Exported void Unlocking();
	Exported ~KifThreadLock();
};

class LockedThread {
public:
	HANDLE lock;
	bool blocked;
	Exported LockedThread();
	Exported ~LockedThread();
	Exported void Blocked();
	Exported void Released();
};


#else
class ThreadLock {
public:

	bool garbaging;
	int relax;
	Exported void Initialise();
	ThreadLock(KifType i);
	~ThreadLock();
};

void KifInitialisationMutex(pthread_mutex_t* mut);
#define kifmutex(x,y) pthread_mutex_t y; pthread_mutex_t* x=&y
#define externkifmutex(x,y) extern pthread_mutex_t y; extern pthread_mutex_t* x
#define KifCreateMutex(x) KifInitialisationMutex(x)
#define KifDeleteMutex(x)

class KifThreadLock {
public:
	pthread_mutex_t* lock;
	bool locked;
	int idthread;
	bool recursive;

	Exported KifThreadLock(pthread_mutex_t* l, bool run = true, bool init = true);
	Exported void Initialize();
	Exported void Locking();
	Exported void Unlocking();
	Exported ~KifThreadLock();
};


#ifdef APPLE
class LockedThread {
public:
	static int naming;
	char name[50];
	sem_t* lock;
	bool blocked;

	Exported LockedThread();
	Exported ~LockedThread();
	Exported void Blocked();
	Exported void Released();
};
#else
class LockedThread {
public:
	sem_t lock;
	bool blocked;

	Exported LockedThread();
	Exported ~LockedThread();
	Exported void Blocked();
	Exported void Released();
};
#endif
#endif
#endif

class ThreadLockElement {
public:

	KifThreadLock* _lock;
	ThreadLockElement(KifThreadLock& l) {
		_lock = &l;
		_lock->Locking();
	}
	~ThreadLockElement() {
		_lock->Unlocking();
	}
};

//------------------------------------------------------------------------------------------
class KifWaitState {
public:
	LockedThread* loquet;
	string lastcast;
	bool killed;
	KifWaitState() {
		loquet = NULL;
		killed = false;
	}

	virtual bool multiple() {
		return false;
	}
	virtual ~KifWaitState() {}
};


class KifWaitStates : public KifWaitState {
public:
	vector<string> flags;
	KifCode* kifcode;

	KifWaitStates(KifCode* kcode) : kifcode(kcode), KifWaitState() {}

	bool multiple() {
		return true;
	}

	~KifWaitStates();
};

//This class implements all the elements that are necessary to debug a program
//It is possible to derive this class to implement a new Debugging function
class KifDebugInfo {
public:
	//the list of variables, which are surveyed by the debugger
	vector<string> watches;
	//The history of commands executed so far in the debugger
	vector<string> history;
	//When the command "o" is processed (out of function), then this variable is set to true
	//It is used to get out from a given function

	int kifoutoffunction;
	int kiffunctioncount;
	//debug mode
	bool kifdebugging;
	//temporary debug mode. 
	bool kifdebug;
	//a list of breakpoint indexed on the file index, with a map on the line index
	map<int, map<long, bool> > kifbreakpoints;
	//the file in which the breakpoint has been set
	int kifgotofile;
	//When a goto is processed, the system executes the code up to that specific line and the specific kifgotofile
	long gotoline;
	//To debug a function from within (commmand "i")
	bool kifinfunction;
	//To stop debugging, the Setdebug are then invalid
	bool kifendofdebug;
	bool debuginfunction;
	int currentfile;
	long currentline;
	KifElement* debugfunction;
	KifVector* debugstack;
	KifMap* debugvariables;
	KifElement* debugdata;
	KifDomain* currentdom;
	KifCode* currentkifcode;
	KifCallFunction* currentcallfunc;

	//A table which contains the actual code line from the KiF file indexed on their position as a line in the files
	hmap<int, map<long, string> > kiftabledebug;
	//Table of filenames.
	vector<string> kiftabledebugfilename;


	string Filename(int i) {
		if (i >= kiftabledebugfilename.size())
			return "";
		return kiftabledebugfilename[i];
	}

	string CurrentFilename() {
		if (currentfile >= kiftabledebugfilename.size())
			return "";
		return kiftabledebugfilename[currentfile];
	}

	int Pathname(string c) {
		for (int i = 0; i<kiftabledebugfilename.size(); i++)
		if (kiftabledebugfilename[i] == c)
			return i;
		return -1;
	}

	void AddFilename(string& n, KifCode* kifcode);
	void ClearFilename() {
		kiftabledebugfilename.clear();
	}
	KifDebugInfo() {
		debuginfunction = false;
		debugfunction = NULL;
		debugstack = NULL;
		debugvariables = NULL;
		kifoutoffunction = 0;
		kiffunctioncount = 1;
		kifdebug = false;
		kifgotofile = 0;
		gotoline = -10;
		kifdebugging = false;
		kifinfunction = false;
		kifendofdebug = true;
		currentfile = 0;
		currentline = 0;
		currentdom = NULL;
		currentcallfunc = NULL;
	}

	KifElement* Currentfilename();
	virtual void Stack(KifCode*);
	virtual void Variables(string);

	void Setkifdebug(KifElement* f, KifElement* obj);
	void Setkifdata(KifElement* obj);
	void Close();
	virtual bool KifDebugging(KifCode* kifcode, KifElement* ke, KifDomain* dom, KifCallFunction* callfunc);
	void KiFSetdebug(bool d) {
		kifdebugging = d;
		kifdebug = d;
		kifendofdebug = 1 - d;
		gotoline = -10;
		kifinfunction = false;
		kifoutoffunction = 0;
		kiffunctioncount = 1;
		kifgotofile = 0;
		currentfile = 0;
		currentline = 0;
	}

	void KiFSetdebugmin(bool d) {
		kifdebugging = d;
		kifdebug = d;
		kifgotofile = 0;
		kifinfunction = false;
		currentfile = 0;
		currentline = 0;
		kifoutoffunction = 0;
		kiffunctioncount = 1;
	}

	bool DebugInfunction() {
		return kifinfunction;
	}

	bool DebuggingMode() {
		return kifdebugging;
	}

	void SetDebuggingMode(bool v) {
		if (kifendofdebug == false)
			kifdebugging = v;
	}

	void Setdebugmode(bool v) {
		kifdebug = v;
	}

	//This method can be derived to implement a different debugger...
	virtual bool Debugging(KifCode* kifcode, KifObject* func, KifElement* ke, KifDomain* dom, KifCallFunction* callfunc);
};

Exported KifType KifAddNewType(string name, KifExternalParse func, bool error = true);

KifElement* KifSearch(string name);
KifElement* KifSearchraw(string name);
KifDebugInfo* DebugPtrInit(bool& deb);

//------------------------------------------------------------------------------------------

class KifCode {
public:
	hmap<string, KifPredicateVariable*> predicatevariables;
	hmap<long, KifElement*> instances;
	hmap<short, bool> predicatedatabase;
	hmap<short, KifPredicateFunction*> predicates;
	hmap<short, KifPredicateTerm*> terms;
	hmap<short, bool> kifAllTypes;
	hmap<short, bool> kifDeclaredFunction;

	vector<int> locals;

	KifFunction* function;
	KifElement* weightfinalfunction;
	KifElement* weightaiguillagefunction;
	KifBaseFrame* kstart;
	KifFunction* kcallsyncronous;
	KifCode* loader;

	long linereference;

	int idkifmain;
	int insidecall;

	unsigned char checkprobabilities;

	bool addtolocal;
	bool session;
	bool recordinstance;
	bool master;

	//-------------------
	bool predicateTrace(short n);
	//-------------------

	int KifLoadPath(string& path, bool debugkif);
	Exported int KifLoad(vector<string>& paths, bool debugkif);
	virtual Exported KifCode* Newinstance();

	void Locals(int i) {
		ThreadLock _lock(kifLocal);
		locals.push_back(i);
	}

	Exported void Setbaseline(int i);

	KifGlobalThread* gGlobals;

	string Gets();

	VECTE<KifCode*>* kifsvector;
	Exported void KifInitLib();

	void FinalGarbage();
	void Terminate();

	Exported KifVariableDeclaration* Createvariable(string name, KifElement* kf = NULL, x_node* xn = NULL);
	Exported KifStaticVariable* Createstaticvariable(string name, KifElement* kf = NULL, x_node* xn = NULL);
	Exported long GetCurrentLine(int inc, x_node*);

	Exported vector<KifWaitState*>* Freestate(string& flag);
	Exported void Setloader(KifCode* k);
	Exported void ObjectInitialisation(int);
	Exported void Updatekstart();

	KifInstruction* KifCreateInstruction(KifElement* kf, KifOperator op);
	KifInstruction* CloningInstruction(KifInstruction* ki);

	KifPredicateContainer* Predicatecontainer();
	Exported KifElement* EvaluatePredicateParameter(KifElement*, string& s);
	Exported KifElement* EvaluateLisp(KifElement*, string& s, string& o, string& c);
	Exported KifElement* EvaluateTags(KifElement*, agnostring& s, string& o, string& c);
	Exported KifElement* EvaluateVector(KifElement*, string& s);
	Exported KifElement* EvaluateMap(KifElement*, string& s);
	Exported KifElement* EvaluateJSON(KifElement*, string& s);
	bool KifLoadModule(x_node* xn, string name, string& library_name);
	Exported void SetArguments();
	void AddArguments(vector<string>& args);
	void ClearArguments();

	Exported void InitArguments(vector<string>& args);
	KifElement* KifParseVarKifKiF(x_node* xn, KifElement* kf);
	bool KifDeclaredFunctionTest(short name);
	Exported KifElement* KifInitVariable(x_node* xn, KifElement* base);
	Exported KifElement* KifTerminateVariable(x_node* xn, KifElement* local, KifElement* variable, KifElement* base);
	KifElement* KifParseDico(x_node* xn, KifElement* kf);
	KifElement* KifParseJSONDico(x_node* xn, KifElement* kf);
	KifElement* KifParseList(x_node* xn, KifElement* kf);
	KifElement* KifParseValVector(x_node* xn, KifElement* kf);
	KifElement* KifParseJSONVector(x_node* xn, KifElement* kf);
	KifElement* KifParseIntentionVector(x_node* xn, KifElement* kf);
	KifElement* KifParseIntentionDoubleVector(x_node* xn, KifElement* kf);

	KifElement* KifParsePredicateDefinition(x_node* xn, KifElement* kf);
	KifElement* KifParsePredicateDCGVariable(string& n, KifElement* kf, bool);
	KifElement* KifParsePredicateDCG(x_node* xn, KifElement* kf);
	KifElement* KifParsePredicateCut(x_node* xn, KifElement* kf);
	KifElement* KifParsePredicateExpression(x_node* xn, KifElement* kf);
	KifElement* KifParsePredicateHead(x_node* xn, KifElement* kf);
	KifElement* KifParsePredicateAssert(x_node* xn, KifElement* kf);
	KifElement* KifParsePredicateVariable(x_node* xn, KifElement* kf);
	KifElement* KifParsePredicateTerm(x_node* xn, KifElement* kf);
	void ComputePredicateParameters(x_node* xn, KifElement* kcf);

	KifElement* KifParseHaskellCycling(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellMapping(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellMap(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellFilter(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellZip(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellFlip(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellFold(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellBoolean(x_node* xn, KifElement* kf);
	KifElement* KifParseVarHaskellLetMin(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellCase(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellIn(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellWhere(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellVector(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellExpression(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellFunctionCall(x_node* xn, KifElement* kf);
	void KifBrowseVariable(x_node* xn, KifElement* kf);
	bool KifCheckVariable(x_node* xn, KifElement* kf);

	KifElement* KifParseValList(x_node* xn, KifElement* kf);
	KifElement* KifParsePlusPlus(x_node* xn, KifElement* kf);
	KifElement* KifParseValMap(x_node* xn, KifElement* kf);
	KifElement* KifParseJSONMap(x_node* xn, KifElement* kf);
	KifElement* KifParseVarFrame(x_node* xn, KifElement* kf);
	KifElement* KifParseAString(x_node* xn, KifElement* kf);
	KifElement* KifParseANumber(x_node* xn, KifElement* kf);
	KifElement* KifParseAXNumber(x_node* xn, KifElement* kf);
	KifElement* KifParseOperation(x_node* xn, KifElement* kf);
	KifElement* KifParseOperationIn(x_node* xn, KifElement* kf);
	KifElement* KifParseOperator(x_node* xn, KifElement* kf);
	KifElement* KifParsePredicateDatabase(x_node* xn, KifElement* kf);
	KifElement* KifParseVarSelf(x_node* xn, KifElement* kf);
	KifElement* KifParseVarEnum(x_node* xn, KifElement* kf);
	KifElement* KifParseVarDate(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTime(x_node* xn, KifElement* kf);
	KifElement* KifParseVarInt(x_node* xn, KifElement* kf);
	KifElement* KifParseVarLong(x_node* xn, KifElement* kf);
	KifElement* KifParseVarFraction(x_node* xn, KifElement* kf);
	KifElement* KifParseVarBit(x_node* xn, KifElement* kf);
	KifElement* KifParseVarBits(x_node* xn, KifElement* kf);
	KifElement* KifParseVarCall(x_node* xn, KifElement* kf);
	KifElement* KifParseVarByte(x_node* xn, KifElement* kf);
	KifElement* KifParseSubFunc(x_node* xn, KifElement* kf);
	KifElement* KifParseSubVar(x_node* xn, KifElement* kf);
	KifElement* KifParseIncrement(x_node* xn, KifElement* kf);
	KifElement* KifParseVarAutomaton(x_node* xn, KifElement* kf);
	KifElement* KifParseVarString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPredicate(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPredicateTerm(x_node* xn, KifElement* kf);
	KifElement* KifParseVarFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarVector(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTable(x_node* xn, KifElement* kf);
	KifElement* KifParseVarVectorString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarVectorFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarVectorInteger(x_node* xn, KifElement* kf);
	KifElement* KifParseVarList(x_node* xn, KifElement* kf);

	KifElement* KifParseVarSet(x_node* xn, KifElement* kf);
	KifElement* KifParseVarSetString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarSetInteger(x_node* xn, KifElement* kf);
	KifElement* KifParseVarSetFloat(x_node* xn, KifElement* kf);


	KifElement* KifParseVarMap(x_node* xn, KifElement* kf);
	KifElement* KifParseVarFloatMap(x_node* xn, KifElement* kf);
	KifElement* KifParseVarIntegerMap(x_node* xn, KifElement* kf);
	KifElement* KifParseVarMapString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarMapFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarMapInteger(x_node* xn, KifElement* kf);
	KifElement* KifParseVarMapIntegerFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarMapIntegerString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarMapIntegerInteger(x_node* xn, KifElement* kf);
	KifElement* KifParseVarMapFloatFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarMapFloatString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarMapFloatInteger(x_node* xn, KifElement* kf);

	KifElement* KifParseVarTreeMap(x_node* xn, KifElement* kf);
	KifElement* KifParseVarIntegerTreeMap(x_node* xn, KifElement* kf);
	KifElement* KifParseVarFloatTreeMap(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTreeMapString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTreeMapFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTreeMapInteger(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTreeMapIntegerFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTreeMapIntegerString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTreeMapIntegerInteger(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTreeMapFloatFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTreeMapFloatString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTreeMapFloatInteger(x_node* xn, KifElement* kf);

	KifElement* KifParseVarPrimeMap(x_node* xn, KifElement* kf);
	KifElement* KifParseVarIntegerPrimeMap(x_node* xn, KifElement* kf);
	KifElement* KifParseVarFloatPrimeMap(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPrimeMapString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPrimeMapFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPrimeMapInteger(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPrimeMapIntegerFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPrimeMapIntegerString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPrimeMapIntegerInteger(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPrimeMapFloatFloat(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPrimeMapFloatString(x_node* xn, KifElement* kf);
	KifElement* KifParseVarPrimeMapFloatInteger(x_node* xn, KifElement* kf);

	KifElement* KifParseComparison(x_node* xn, KifElement* kf);
	KifElement* KifParseComparisonPredicate(x_node* xn, KifElement* kf);
	KifElement* KifParseVarBoolean(x_node* xn, KifElement* kf);
	KifElement* KifParseVarFile(x_node* xn, KifElement* kf);
	KifElement* KifParseVarFileWide(x_node* xn, KifElement* kf);
	KifElement* KifParseVariable(x_node* xn, KifElement* kf);
	KifElement* KifParseDeclaration(x_node* xn, KifElement* kf);
	KifElement* KifParseMultiDeclaration(x_node* xn, KifElement* kf);
	KifElement* KifParseIndexes(x_node* xn, KifElement* kf);
	KifElement* KifParseInterval(x_node* xn, KifElement* kf);
	KifElement* KifParseCall(x_node* xn, KifElement* kf);
	KifElement* KifNonLimited(x_node* xn, KifElement* kf);
	KifElement* KifParseUniqueCall(x_node* xn, KifElement* kf);
	KifElement* KifParseBloc(x_node* xn, KifElement* kf);
	KifElement* KifParseExpressions(x_node* xn, KifElement* kf);
	KifElement* KifParseTryCatch(x_node* xn, KifElement* kf);
	KifElement* KifParseTest(x_node* xn, KifElement* kf);
	KifElement* KifParseUntil(x_node* xn, KifElement* kf);
	KifElement* KifParseWhile(x_node* xn, KifElement* kf);
	KifElement* KifParseFor(x_node* xn, KifElement* kf);
	KifElement* KifParseBlocFor(x_node* xn, KifElement* kf);
	KifElement* KifParseForIn(x_node* xn, KifElement* kf);
	KifElement* KifParseBoolean(x_node* xn, KifElement* kf);
	KifElement* KifParseABoolean(x_node* xn, KifElement* kf);
	KifElement* KifParseVarTree(x_node* xn, KifElement* kf);
	KifElement* KifParseVarIterator(x_node* xn, KifElement* kf);
	KifElement* KifParseVarRIterator(x_node* xn, KifElement* kf);
	KifElement* KifParseNegation(x_node* xn, KifElement* kf);
	KifElement* KifParseSwitch(x_node* xn, KifElement* kf);
	KifElement* KifParseTestSwitch(x_node* xn, KifElement* kf);
	void ComputeParameters(x_node* xn, KifCallFunction* kcf);
	//Function creation
	KifElement* KifParseEvalLambda(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskell(x_node* xn, KifElement* kf);
	KifElement* KifParseLambda(x_node* xn, KifElement* kf);
	KifElement* KifParseLambdaFunction(x_node* xn, KifElement* kf);
	KifElement* KifParseLabel(x_node* xn, KifElement* kf);
	KifElement* KifParseHaskellCompose(x_node* xn, KifElement* kf);
	KifElement* KifParseReturnValue(x_node* xn, KifFunctionLambda* kf, char add = false);
	KifElement* KifParseFunction(x_node* xn, KifElement* kf);
	KifElement* KifParseAffectation(x_node* xn, KifElement* kf);
	KifElement* KifParseFrame(x_node* frame, KifElement* kf);
	KifElement* KifParseExtension(x_node* frame, KifElement* kf);
	KifElement* CreateVariable(x_node* xn, short name, KifElement* kf, KifInstructionType instype);
	KifElement* KifBrowse(x_node* xn, KifElement* kf);

	Exported void Loadfile(string filename, KifCode* loader = NULL);
	Exported void Loadfile(ifstream* fichier, string filename, long pos, KifCode* loader = NULL);
	Exported KifElement* Evaluate(KifElement*, string&, bool commandline);
	KifElement* FindFrameForSequence(KifElement* ktop, x_node* xn, short);

	Exported KifCode(int trig, void* d = NULL);
	bool Debugging(KifObject* func, KifElement* ke, KifDomain* dom, KifCallFunction*);

	Exported KifElement* Returnerror(string message);
	Exported KifElement* Returnerror(int id, string message);


	Exported KifElement* Get(short n, KifDomain* dom);
	Exported KifElement* Getbasic(short n, KifElement*);
	Exported KifElement* Getbasicfunction(short n, KifElement*);
	KifElement* Getframe(short n, KifElement* dom);

	void Load(x_reading* xr);
	Exported void Run(int from = 0);
	Exported KifElement* Execute(KifFunction*, vector<KifElement*>& paramaters);
	KifElement* Find(string& name);
	KifElement* Findraw(string& name);

	Exported KifString* Providestring(string& z);
	Exported KifString* Providestringraw(string z);
	Exported KifInteger* Provideinteger(long val);
	Exported KifFloat* Providefloat(double val);
	Exported KifVector* Providevector();
	Exported KifMap* Providemap();
	Exported KifList* Providelist();

	Exported ~KifCode();
};

//------------------------------------------------------------------------------------------
struct KifLabel {
	long line;
	int fileindex;
	string label;
};

class Stackelement {
public:
	int line;
	int file;
	int name;

	Stackelement(int l, int f, int n) {
		line = l;
		file = f;
		name = n;
	}

	void Modify(KifElement* ke);
};

class Kifthreadmessage {
public:
	virtual bool Clean() { return true; }
};

class Kifthreadmessages {
public:
	vector<Kifthreadmessage*> messages;
	void push(Kifthreadmessage* msg) {
		messages.push_back(msg);
	}

	~Kifthreadmessages() {
		for (int i = 0; i<messages.size(); i++) {
			if (messages[i]->Clean())
				delete messages[i];
		}
	}

	void clear() {
		for (int i = 0; i<messages.size(); i++) {
			if (messages[i]->Clean())
				delete messages[i];
		}
	}
};

//------------------------------------------------------------------
//------------------------------------------------------------------
class threaditems {
public:
	bool used;
	int id;
	int prologstack;
	vector<KifDomain*> threadstacks;
	int parentthreads;
	Kifthreadmessages threadmessages;
	KifError* errorptr;
	bool error;
	VECTE<THREADPTR> joinedloquets;
	vector<Stackelement*> stackelements;
	KifReturnValue* returnvalue;
#ifdef C11
	std::thread* localthread;
#endif

	threaditems(int i, int mxj);

	~threaditems();

	KifElement* Setreturnvalue(KifElement* ke);
	void clear(bool display);
	void cleanerror();
	inline size_t sum() { return(stackelements.size() + prologstack); }

	Stackelement* Getlastelement() {
		if (stackelements.size() == 0)
			return NULL;
		return stackelements.back();
	}
};
//------------------------------------------------------------------
typedef KifElement* (KifCode::*KifParseElement)(x_node*, KifElement*);
typedef void(*KifDisplayFunction)(string s, void* object);
//------------------------------------------------------------------

class KifGlobalThread {
public:
	hmap<short, kifcompatibletype> compatibilities;
	hmap<short, KifFrame*> frames;
	hmap<short, LockedThread*> loquets;
	hmap<string, short> kifStringId;
	hmap<short, string> kifIdString;
	hmap<string, KifThreadLock*> locktables;

	hmap<string, int> kifCodePaths;
	hmap<string, KifLabel> kifLabels;
	hmap<long, KifLabel> kifLabelIdx;

	hmap<string, KifExternalModule> kifLibraries;
	hmap<string, KifExternalModule> kifMainLibraries;
	hmap<string, KifExternalTerminateModule> kifTerminateLibraries;

	hmap<short, bool> kifTreeFunctions;
	hmap<short, bool> kifPredicateFunctions;
	hmap<short, bool> kifStringFunctions;
	hmap<short, bool> kifAutomatonFunctions;
	hmap<short, bool> kifDateFunctions;
	hmap<short, bool> kifTimeFunctions;
	hmap<short, bool> kifKiFFunctions;
	hmap<short, bool> kifFileFunctions;
	hmap<short, bool> kifIteratorFunctions;
	hmap<short, bool> kifNumericalFunctions;

	hmap<short, bool> kifVectorFunctions;
	hmap<short, bool> kifListFunctions;
	hmap<short, bool> kifMapFunctions;
	hmap<short, bool> kifSetFunctions;

	hmap<short, short> equivalences;

	hmap<string, KifParseElement> kifFunctions;
	hmap<string, KifParseElement> kifTypes;
	hmap<string, KifOperator> kifOperators;
#ifdef WIN32
	hmap<KifOperator, string> kifOperatorString;
#else
	hmap<short, string> kifOperatorString;
#endif

	hmap<short, bool> kifReturnString;
	hmap<short, bool> kifReturnInt;
	hmap<short, bool> kifReturnFloat;

	hmap<short, KifPredefined*> kifBaseFunctions;
	hmap<short, KifCallMethod*> kifExternalFunctions;
	hmap<short, KifPredefined*> kifBaseTreeFunctions;
	hmap<short, bool> kifPredefinedFunctions;
	hmap<short, bool> kifMethods;
	hmap<short, mathFunc> kifMathFunctions;
	hmap<short, vector<KifPredicate*> > knowledgebase;
	hmap<string, vector<KifPredicate*> > knowledgebasekeys;
	hmap<int, vector<KifLoopKey*> > loopkeys;
	hmap<int, bool> setthreads;

#if defined(WIN32) || defined(APPLE)
	hmap<threadhandle, int> threads;
#else
	map<threadhandle, int> threads;
#endif

	map<string, KifExternalParse> kifExternalTypes;
	map<string, vector<KifWaitState*> > kifWaitstates;
	map<KifType, string> kifStringType;
	map<string, KifType> kifTypeString;
	map<string, bool> kifConversionNames;
	map<KifType, short> kifIdType;
	map<string, string> kifInformationsPredicate;
	map<string, string> kifInformationsString;
	map<string, string> kifInformationsInt;
	map<string, string> kifInformationsFloat;
	map<string, string> kifInformationsVector;
	map<string, string> kifInformationsList;
	map<string, string> kifInformationsMap;
	map<string, string> kifInformationsTree;
	map<string, string> kifInformationsIterator;
	map<string, string> kifInformationsDate;
	map<string, string> kifInformationsTime;
	map<string, string> kifInformationsFile;
	map<string, string> kifInformationsKif;
	map<string, string> kifInformationsAutomaton;

	vector<string> arguments;
	vector<KifString*> stringlist;
	vector<KifFloat*> floatlist;
	vector<KifInteger*> integerlist;


#ifdef C11
	std::recursive_mutex tableofcommonlocks[locktablesize];
#else
#ifdef WIN32
	HANDLE tableofcommonlocks[locktablesize];
#else
	pthread_mutex_t tableofcommonlocks[locktablesize];
#endif
#endif

	char buffer[4096];

	VECTE<KifCode*> kifCodeVector;
	VECTE<KifElement*> garbage;
	VECTE<int> empties;
	VECTE<int> sempties;
	VECTE<int> iempties;
	VECTE<int> fempties;

	kifcompatibletype accetabletypeparams;
	LockedThread threadloquet;
	Kifterminate terminationfunction;
	KifDisplayFunction displayfunction;

	ostream* os;
	bnf_kif* bnfkif;
	KifBaseFrame* kconstants;
	threaditems** THR;
	x_reading* current;
	void* data;
	KifElement* debugfunction;
	void* displayobject;
	KifDebugInfo* debuginfocurrent;

	KifElement* gRAISEERROR;
	KifElement* gNULL;
	KifElement* gNOELEMENT;
	KifElement* gUNIVERSAL;
	KifElement* gTRUE;
	KifElement* gFALSE;
	KifElement* gMINUSONE;
	KifElement* gONE;
	KifElement* gZERO;
	KifElement* gEMPTYSTRING;
	KifElement* gDEFAULT;
	KifElement* gBREAK;
	KifElement* gBREAKTRUE;
	KifElement* gBREAKFALSE;
	KifElement* gBREAKPOINT;
	KifElement* gCONTINUE;
	KifElement* gNEG;

	KifElement* gTypeSTRING;
	KifElement* gTypeLONG;
	KifElement* gTypeFRACTION;
	KifElement* gTypeINTEGER;
	KifElement* gTypeFLOAT;
	KifElement* gTypeBOOLEAN;

	KifDebugInfo debuginfos;
	KifElement* kstd;
	KifElement* kerr;

	KifPredicateFunction* gcut;
	KifPredicateFunction* gfail;

	long current_start;
	long current_end;

	int lastfileindex;
	int maxjoined;
	int mxthreads;
	int basegarbage;
	int idreturnvariable;
	int idreturn;
	int stacklimit;
	int kifLastType;
	int trigger;
	int cursor;
	int cursormax;
	int cursorstring;
	int sizestring;
	int cursorfloat;
	int sizefloat;
	int cursorinteger;
	int sizeinteger;

	short flfrom;
	short flto;

	char executionbreak;

	bool threadmode;
	bool doubledisplay;
	bool erroronkey;
	bool garbaging;
	bool prive;
	bool evaluation;
	bool common;
	bool constant;
	bool compile;
	bool endofexecution;
	bool cleandata;
	bool updatethread;
	bool nogarbage;

	//--------------------------------------
	bool compatible(KifElement* a, KifElement* b);
	bool compatible(short a, short b);

	//-----------------------
	//Thread management

	void resizethreads(int t);

	inline bool Teststack(int id) {
		if ((THR[id]->sum())>stacklimit) {
			Stackerror(id);
			return true;
		}
		return false;
	}

	//-------------------

	//Debugging variables...
	void AddArguments(vector<string>& args) {
		for (int i = 0; i<args.size(); i++)
			arguments.push_back(args[i]);
	}

	inline KifElement* Garbagefind(int i) {
		ThreadLock _lock(kifGarbage);
		return garbage[i];
	}

	inline bool Garbageexist(int i, KifElement* w) {
		ThreadLock _lock(kifGarbage);
		if (garbage[i] == w)
			return true;
		return false;
	}

	string Currentfilename();


	LockedThread* GetLoquet(int idvar);
	LockedThread* AddLoquet(int idvar);
	void Releaseallloquet();


	~KifGlobalThread();


	void UpdateFail(KifCode* klc);

	KifElement* Stackerror(int idthread);

	bool KifConversion(string n);
	bool Getstackinfoid(KifCode*, int i, KifMap* kmap);


	bool KifCreatePredefined(KifCode*, string name, unsigned int arity, Predefined func, bool tree = false);
	KifElement* KifBaseFunctionTest(short name);
	void AddKifCode(KifCode* k);

	Exported void Clean();

	Exported bool Trigger();

	Exported void ResetGarbageElement(KifElement* d, int i);

	bool kifOperatorMath[kifLASTDECLARATION + 1];

	void TerminateAllLibraries(KifCode*);

	void KifInitInformations();

	Exported void addtogarbage(KifElement*);
	Exported void garbage_collector(int from);
	Exported void Garbage();

	void GetStackInfos(KifCode*, KifVector* vect);
	Exported KifString* providestring(string& z);

	Exported KifInteger* provideinteger(long val);

	Exported KifFloat* providefloat(double val);

	KifElement* Provide(KifType ktype);

	KifFunction* garbagefunction;
	Exported KifCouple* providecouple(KifElement* rec, KifIndex* i, KifDomain* dom, KifElement* f);

	void Setterminationfunction(Kifterminate t) {
		terminationfunction = t;
	}

	KifGlobalThread(void* d = NULL);

	KifElement* Get(short);
	KifElement* Getframe(short);
	void RecordExternalFunction(short idname, unsigned int arity);
	bool TestExternalArity(int id, int arity);

	inline string KifIdString(short n) {
		hmap<short, string>::iterator it = kifIdString.find(n);
		if (it != kifIdString.end())
			return it->second;
		return "";
	}

	inline short KifNewId(string n) {
		hmap<string, short>::iterator it = kifStringId.find(n);
		if (it == kifStringId.end()) {
			int id = kifStringId.size();
			kifStringId[n] = id;
			kifIdString[id] = n;
			compatibilities[id].set(id);
			return id;
		}
		return it->second;
	}

	inline short KifStringId(string n) {
		hmap<string, short>::iterator it = kifStringId.find(n);
		if (it == kifStringId.end())
			return -1;
		return it->second;
	}

	inline void KifRemoveId(short id) {
		hmap<short, string>::iterator it = kifIdString.find(id);
		if (it != kifIdString.end()) {
			kifStringId.erase(it->second);
			kifIdString.erase(id);
			compatibilities.erase(id);
		}
	}

	Exported int GetThreadid();
	Exported void SetThreadid(threadhandle tid, int idthread);
	Exported void Loopthreadid(int idthread);
	Exported void EraseThreadid(threadhandle tid);

	void Setthreadmessage(int id, Kifthreadmessage* m) {
		THR[id]->threadmessages.push(m);
	}

	void addloopkey(int id, KifLoopKey*);
	void removeloopkey(int id);
	KifLoopKey* getloopkey(int id);


	Exported int Setthreadid(int);
	KifElement* Returnerror(int id, KifError*);
	KifType KifAddNewType(string& name, KifExternalParse func, bool error = true);
	void KifAssociateType(string name, KifExternalParse func, KifType kt);
	bool TestExternalType(string name);

	void Initialisation(int, KifCode*);
	void InitPredefined(KifCode* kifcodeptr);
	void InitialisationIDs(KifCode* kifcodeptr);
	void InitialisationIDString(KifCode* kifcodeptr);
	void InitialisationInitMaps(KifCode* kifcodeptr);
	void InitialisationInitStructs(int trig, KifCode* kifcodeptr);

	KifElement* SearchStackForLambda();
	void SearchStackForLambdaDeclaration(KifCallFunctionGeneral*);

	Exported inline int PushFunctionStack(KifDomain* func, KifDomain* dom) {
		int id = GetThreadid();
		THR[id]->threadstacks.push_back(dom);
		THR[id]->threadstacks.push_back(func);
		return id;
	}

	Exported inline int PushFunctionStackid(int id, KifDomain* func, KifDomain* dom) {
		THR[id]->threadstacks.push_back(dom);
		THR[id]->threadstacks.push_back(func);
		return id;
	}

	Exported inline void PopFunctionStack(int id) {
		THR[id]->threadstacks.pop_back();
		THR[id]->threadstacks.pop_back();
	}

	Exported inline void PushStack(KifDomain* ke) {
		THR[GetThreadid()]->threadstacks.push_back(ke);
	}

	Exported inline void PopStack() {
		THR[GetThreadid()]->threadstacks.pop_back();
	}

	inline KifDomain* Top() {
		int id = GetThreadid();
		if (THR[id]->threadstacks.empty())
			return NULL;

		return THR[id]->threadstacks.back();
	}

	inline KifDomain* Topid(int id) {
		if (id == -1)
			id = GetThreadid();
		if (THR[id]->threadstacks.empty())
			return NULL;

		return THR[id]->threadstacks.back();
	}

	inline KifDomain* PreviousTopid(int id) {
		if (id == -1)
			id = GetThreadid();
		int sz = THR[id]->threadstacks.size();
		if (THR[id]->threadstacks.empty() || sz <= 2)
			return NULL;

		return THR[id]->threadstacks[sz - 2];
	}

	inline KifDomain* Beforetop() {
		int id = GetThreadid();
		int sz = THR[id]->threadstacks.size();
		if (sz <= 1)
			return NULL;
		return THR[id]->threadstacks[sz - 2];
	}

	inline KifDomain* Beforetopid(int id) {
		if (id == -1)
			id = GetThreadid();
		int sz = THR[id]->threadstacks.size();
		if (sz <= 1)
			return NULL;
		return THR[id]->threadstacks[sz - 2];
	}

	inline KifDomain* PreviousBeforetopid(int id) {
		if (id == -1)
			id = GetThreadid();
		int sz = THR[id]->threadstacks.size();
		if (sz <= 3)
			return NULL;
		return THR[id]->threadstacks[sz - 4];
	}

	inline KifDomain* Topthread(int id) {
		return THR[id]->threadstacks.back();
	}

	inline KifDomain* Beforetopthread(int id) {
		int sz = THR[id]->threadstacks.size();
		if (sz <= 1)
			return NULL;
		return THR[id]->threadstacks[sz - 2];
	}

	inline bool Test() {
		return threadmode;
	}

	inline void Threadmode() {
		threadmode = true;
	}


	bool storePredicate(KifDomain*, KifPredicate*, bool last);
	bool testPredicate(KifDomain*, KifPredicate*);
	char isaValidPredicate(KifDomain*, KifPredicate*, hmap<short, vector<KifPredicateRule*> >&);
	bool GetPredicates(KifDomain* dom, KifPredicate* p, vector<KifPredicate*>& v, bool cut);
	bool RemovePredicates(KifDomain*, KifPredicate* p);
	bool RemoveThePredicate(KifDomain*, KifPredicate* p);
};
extern KifGlobalThread* kifGlobals;

//==========================================================================================================================
//==========================================================================================================================
//==========================================================================================================================
class KifElement {
public:
	KifCode* kifcode;
	int idgarbage;
	short reference;
	KifType type;
	char popped;

	KifElement(KifCode* klc, KifElement* kp, KifType k = kifAny);

	//For a certain number of types: float, integer, string, vector and map, we need to ensure a duplication of these
	//elements in the case of a function call in which these elements will be hard-defined: myfunc([1,2,3]); would be a good
	//example. This function is used at compile time and set the value statique to popped. This value is only used in
	//KifInstructionPARAMETER::Execute.
	virtual bool Statique() {
		return false;
	}

	KifElement* Predicate(KifDomain* dom);

	virtual double Sum() {
		return Float();
	}
	virtual double Product() {
		return Float();
	}

	virtual bool aMapContainer() {
		return false;
	}

	virtual bool aVectorContainer() {
		return false;
	}

	virtual Exported bool isConst() {
		return false;
	}

	virtual KifElement* Unique() {
		return this;
	}

	virtual Exported KifElement* Declaration(short s) {
		return NULL;
	}

	virtual Exported KifElement* Store(KifElement*, KifDomain*, int) {
		return kifNULL;
	}

	virtual Exported KifElement* Remove(KifElement*, KifDomain*, int) {
		return kifNULL;
	}

	virtual bool IterInitialisation() {
		return false;
	}
	virtual void IterInitialize(bool v) {}

	virtual Exported KifElement* Declarationfunction(short s) {
		return Declaration(s);
	}

	virtual KifOrigin Instancetype() {
		return kinUnknown;
	}

	virtual KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	virtual KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);
	virtual bool Setseeds(unsigned short* p) { return false; }
	virtual KifElement* EvaluationMap() {
		return kifNULL;
	}

	virtual Exported void Setdeclaration(short n, KifElement*, hmap<short, KifElement*>* stacks) {}
	virtual Exported void Resetdeclaration(hmap<short, KifElement*>& stacks) {}

	virtual bool Simple() {
		return false;
	}

	virtual bool isACall() {
		return false;
	}

	virtual bool isInstruction() {
		return false;
	}

	virtual unsigned int EditDistance(KifElement*);
	virtual KifElement* Getvalues(KifDomain* dom, bool duplicate);
	virtual bool Insertvalue(KifDomain* dom, KifElement* v, hmap<short, KifElement*>& kept) {
		return true;
	}

	virtual bool Basevalue() {
		return false;
	}

	virtual int Garbager() {
		return 0;
	}

	virtual Exported void Sort(bool direction) {}

	virtual bool Succeed() {
		return true;
	}

	virtual void Shuffle() {}
	virtual bool Permute() { return false; }

	virtual Exported KifElement* Declared(short s) {
		return NULL;
	}

	virtual Exported void Insert(int idx, KifElement* ke) {}

	virtual bool aVector() {
		return false;
	}

	virtual bool aString() {
		return false;
	}

	virtual bool aValueVector() {
		return false;
	}

	virtual bool aValueMap() {
		return false;
	}

	virtual bool aValueContainer() {
		return false;
	}

	virtual void Reserve(int i) {}

	virtual KifRoot* Root() {
		return NULL;
	}

	virtual bool aContainer() {
		return false;
	}

	virtual bool aNumber() {
		return false;
	}

	virtual double theValue() {
		return 0;
	}

	virtual KifElement* Split(KifBaseVector* params, KifCallFunction*) {
		return kifRAISEERROR;
	}

	virtual KifElement* Split(KifBaseMap* params, KifCallFunction*) {
		return kifRAISEERROR;
	}

	virtual KifElement* Match(KifBaseVector* params, KifDomain* dom, int, KifCallFunction*);

	virtual KifElement* Match(KifBaseMap* params, KifDomain* dom, int, KifCallFunction*) {
		return kifFALSE;
	}

	virtual KifElement* MatchPattern(KifElement* params, KifDomain* dom, int, KifCallFunction*);

	virtual void inValue(double v) {}

	virtual Exported void callfunction(KifElement* a, KifElement* b, KifDomain* dom) {}
	virtual bool Basic() {
		return false;
	}

	virtual bool Negation() {
		return false;
	}

	virtual void Setdisjunction(bool v) {}

	virtual bool Disjunction() {
		return false;
	}

	virtual void Setfail(bool v) {}

	bool Failed() { return false; }

	virtual KifInstructionType Instructiontype() {
		return kifNOTYPE;
	}

	virtual KifFunction* Functioncall(KifDomain*) {
		return NULL;
	}

	virtual KifElement* Parameter(int i) {
		return kifNULL;
	}

	virtual bool aPredicateVariable() {
		return false;
	}

	virtual bool hasbeenSuccesfull() {
		return false;
	}
	virtual void setSucessfull(bool v) {}

	virtual bool isToMerge() {
		return false;
	}

	virtual void storeInLoop(KifElement** alls);
	virtual bool unified(KifDomain* dom, KifElement* a);

	virtual bool unifiedDeclaration(KifDomain* dom) {
		return true;
	}

	virtual bool isBreak() {
		return false;
	}

	virtual hmap<short, KifPredicateVariableInstance*>* Dico() {
		return NULL;
	}

	virtual bool isfile() {
		return false;
	}

	virtual void Lockframe(bool exclusive) {}
	virtual void Unlockframe(bool exclusive) {}
	virtual bool IsSynchronous() {
		return false;
	}
	virtual void SetSynchronousLock() {}
	virtual void ResetSynchronousLock() {}
	virtual void Setlocalvariable() {}
	virtual bool isString() {
		return false;
	}

	virtual bool Localvariable() {
		return false;
	}

	virtual Exported string Type();

	virtual Exported KifType inType() {
		return theType();
	}

	virtual char inCode() {
		return ckifInstance;
	}

	virtual Exported KifType theType() {
		return type;
	}

	virtual Exported int Depth() {
		return -1;
	}

	virtual void Protect() {}

	void Putpopped(char v) {
		if (popped != 2)
			popped = v;
	}

	Exported void Incrementreference() {
		reference++;
		Putpopped(0);
	}

	virtual bool Kifexist() {
		return true;
	}

	virtual void Items(KifVector*) {}

	virtual void Setself() {};

	virtual void Setinstancetype(KifOrigin v) {}

	virtual Exported void Setreference(int inc = 1) {
		reference += inc;
		Putpopped(0);
	}

	Exported void Resetpopped() {
		Putpopped(0);
	}

	virtual KifElement* VariableValue() {
		return kifNULL;
	}

	virtual void  SetVariableValue(KifElement*) {}
	virtual bool isValidVariable() {
		return false;
	}

	virtual Exported void Setpopped(int v = 1, bool force = false) {
		if (reference <= 0 || force)
			Putpopped(v);
	}

	virtual bool Alreadydeclared() {
		return false;
	}

	virtual bool Predeclared() {
		return false;
	}

	virtual bool isFunction() {
		return false;
	}

	virtual bool isCall() {
		return false;
	}

	virtual bool isUsed() {
		return true;
	}

	virtual void Setused() {}

	virtual void Addfunction(KifFunction* kfunc) {}
	virtual bool Subframe(KifFrame*) {
		return false;
	}

	virtual Exported void Resetreference(int inc = 1);
	virtual int Ingarbage() {
		return idgarbage;
	}

	virtual void Release() {
		if (reference == 0) {
			popped = false;
			Resetreference();
		}
	}

	virtual Exported KifElement* Inverse() {
		return kifNULL;
	}

	virtual Exported void Clean();


	virtual Exported void Clear() {}


	virtual void GetRoot(KifInstruction* r) {}


	virtual Exported void Next() {}

	virtual Exported KifElement* End() {
		return kifTRUE;
	}

	virtual KifElement* Initialisation() {
		return kifNULL;
	}

	virtual void Setselfinsert(bool) {};
	virtual bool Selfinsert() {
		return false;
	}

	virtual void SetInstruction(int i, KifElement*) {}

	virtual int InstructionSize() {
		return 0;
	}
	virtual void Setnegation(bool v) {}

	virtual Exported int Threadid() {
		return 0;
	}

	virtual Exported int Parentthreadid() {
		return 0;
	}

	virtual Exported bool Unlimited() {
		return false;
	}

	virtual Exported void Setlimited(bool l) {}


	virtual KifElement* Instruction(int i) {
		return kifNULL;
	}

	virtual KifElement* Frominstruction(int i) {
		return this;
	}

	virtual KifElement* InstructionRemoveLast() {
		return kifNULL;
	}

	virtual void InstructionRemove(int nb) {}

	virtual KifElement* Lastinstruction() {
		return kifNULL;
	}

	virtual string Keyvalue() { return "*"; }
	virtual void Setstringvalue(string& k) {}

	virtual bool Autorun() {
		return false;
	}

	virtual void Addinstruction(int i, KifElement* ke) {}
	virtual void InstructionClear() {}

	virtual Exported KifElement* Begin() {
		return kifNULL;
	}

	virtual char Indexes(int sz, KifDomain*, int& left, int& right) {
		return -1;
	}

	virtual Exported void IteratorSetvalue(KifElement* ke) {}

	virtual Exported KifElement* IteratorKey() {
		return kifNULL;
	}

	virtual Exported KifElement* IteratorValue() {
		return kifNULL;
	}

	virtual string IteratorKeyString();
	virtual long IteratorKeyInteger();
	virtual double IteratorKeyFloat();

	virtual long IteratorValueInteger();

	virtual string IteratorValueString();

	virtual double IteratorValueFloat();

	virtual Exported long Size() { return 0; }

	virtual KifElement* Parent() {
		return kifNULL;
	}

	virtual void Addparent(KifElement* p) {}

	//------------------------TREE---------------------------------
	virtual Exported KifElement* TreeChild() {
		return kifNULL;
	}

	virtual Exported KifElement* TreePrevious() {
		return kifNULL;
	}

	virtual Exported KifElement* TreeParent() {
		return kifNULL;
	}
	virtual Exported KifElement* TreeLast() {
		return kifNULL;
	}
	virtual Exported KifElement* TreeNext() {
		return kifNULL;
	}

	virtual Exported KifElement* TreeAddchild(KifTree* t) {
		return kifFALSE;
	}
	virtual Exported KifElement* TreeAddnext(KifTree* t) {
		return kifFALSE;
	}
	virtual Exported KifElement* TreeAddprevious(KifTree* t) {
		return kifFALSE;
	}
	virtual Exported KifElement* TreeRemove() {
		return kifFALSE;
	}

	Exported KifElement* TreeExtract(KifTree* with) {
		return kifNULL;
	}

	virtual Exported bool Private() {
		return false;
	}
	virtual void Setprivate(bool v) {}

	virtual Exported KifElement* TreeValue() { return kifNULL; }

	//--------------END TREE---------------------------------
	virtual void storekey(long k, long v);
	virtual void storekey(long k, double v);
	virtual void storekey(long k, string v);
	virtual void storekey(double k, long v);
	virtual void storekey(double k, double v);
	virtual void storekey(double k, string v);
	virtual void storekey(string k, long v);
	virtual void storekey(string k, double v);
	virtual void storekey(string k, string v);
	virtual void storekey(char* k, long v);
	virtual void storekey(char* k, double v);
	virtual void storekey(char* k, string v);

	virtual void storevalue(long v);
	virtual void storevalue(double v);
	virtual void storevalue(string v);
	virtual void storevalue(char* k);

	virtual long getinteger(int i) {
		return ValueOnIndex(i)->Integer();
	}

	virtual double getfloat(int i) {
		return ValueOnIndex(i)->Float();
	}

	virtual string getstring(int i) {
		return ValueOnIndex(i)->String();
	}

	virtual Exported void Push(KifElement*);
	virtual Exported void Push(string&, KifElement*) {}
	virtual Exported void Push(char*, KifElement*) {}
	virtual Exported void Push(KifElement*, KifElement*) {}
	virtual Exported void Pop(KifElement* ke);
	virtual Exported void Merge(KifElement* ke) {}
	virtual Exported void PushFirst(KifElement*);
	virtual Exported KifElement* PopFirst(KifElement* ke);
	virtual Exported void PushLast(KifElement*);
	virtual Exported KifElement* PopLast(KifElement* ke);
	virtual Exported KifElement* Last();
	virtual Exported KifElement* First();

	virtual Exported KifElement* Argument(int i) { return NULL; }
	virtual Exported void Save(string& filename, string& ty) {}
	virtual Exported void Add(KifElement*) {}

	virtual KifElement* Values(string s) { return kifNOELEMENT; }

	virtual KifElement* Value(KifDomain* dom) {
		return kifNOELEMENT;
	}

	virtual KifElement* Index(KifElement* s) { return kifFALSE; }
	virtual KifElement* ValueOnIndex(size_t i) { return kifNOELEMENT; };

	virtual KifElement* ValueOnKey(string k) { return kifNOELEMENT; };
	virtual KifElement* ValueOnIntegerKey(long k) { return kifNOELEMENT; };
	virtual KifElement* ValueOnFloatKey(double k) { return kifNOELEMENT; };

	virtual string ValueOnKeyString(string k, bool& a);
	virtual long ValueOnKeyInteger(string k, bool& a);
	virtual double ValueOnKeyFloat(string k, bool& a);

	virtual string ValueOnKeyIString(long k, bool& a);
	virtual long ValueOnKeyIInteger(long k, bool& a);
	virtual double ValueOnKeyIFloat(long k, bool& a);

	virtual string ValueOnKeyFString(double k, bool& a);
	virtual long ValueOnKeyFInteger(double k, bool& a);
	virtual double ValueOnKeyFFloat(double k, bool& a);

	virtual bool Predefinedtype() { return false; }
	virtual void Toassign(bool v) {}
	virtual bool Assigning(KifType) { return false; }
	virtual void Setevaluate(bool v) {}
	virtual bool Evaluate() { return false; }
	virtual void Setfunction(KifElement* f) {}
	virtual KifElement* Returnfunction() { return NULL; }
	virtual KifElement* Returnobject() { return kifNULL; }
	virtual KifElement* Robfunction(KifElement* kbase) { return NULL; }
	virtual bool testfunction(KifElement* kfunc) {
		if (kfunc != NULL && kfunc->isFunction())  {
			if (kfunc->Size() == 2) {//regular function
				if (kfunc->Argument(0)->VariableValue()->inType() != type || kfunc->Argument(1)->VariableValue()->inType() != type)
					return false;
				return true;
			}

			if (kfunc->Size() == 3) {//within a frame
				KifType ty = kfunc->Argument(0)->VariableValue()->inType();
				if (ty != kifFrame && ty != kifDomain)
					return false;
				if (kfunc->Argument(1)->VariableValue()->inType() != type || kfunc->Argument(2)->VariableValue()->inType() != type)
					return false;
				return true;
			}
			return false;
		}
		return true;
	}

	virtual void CleanPointer() {}
	virtual long Line() { return -1; }
	virtual void Setline(long l, int fidx) {}
	virtual int Fileindex() { return -1; }
	virtual bool Function() { return false; }
	virtual Exported void Methods(KifVectorString* fcts) {}
	virtual Exported KifElement* Information(KifElement*) {
		return kifNULL;
	}
	virtual KifElement* Variable(KifDomain* dom) {
		return NULL;
	}
	virtual KifElement* apply(KifCallFunctionGeneral* kfunc, KifElement* c, KifDomain* dom, KifCallFunction* callfunc) { return kifNULL; }
	virtual bool Variable() { return false; }
	virtual bool XIP() { return false; }
	virtual Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*) { return kifNULL; }
	virtual Exported bool initialconstructor() { return false; }
	virtual Exported KifElement* Newinstance() { return NULL; }
	virtual Exported KifElement* Newiterator(bool rev) { return kifNULL; }
	virtual KifElement* Returnvalue() { return this; }
	virtual bool Returned() {
		return false;
	}
	virtual KifElement* EvaluateKey(KifElement* k, KifElement* idx, KifElement* dom) { return this; }
	virtual Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL) { return kifNULL; }
	virtual inline Exported KifElement* ExecuteRaw(KifElement* contextualpattern, KifDomain* dom) {
		return Execute(contextualpattern, dom, kifNULL);
	}
	virtual inline Exported KifElement* ExecRaw(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
		return Execute(contextualpattern, dom, kifNULL);
	}

	inline virtual KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
		return Execute(contextualpattern, dom, val);
	}

	virtual bool isInstance() {
		return false;
	}

	virtual Exported KifElement* EvaluePredicateVariables(KifElement* context, KifDomain* dom);

	virtual Exported KifElement* ExtractPredicateVariables(KifElement* contextualpattern, KifDomain* dom, KifElement* val, KifElement* e, bool root);

	virtual KifElement* ApplySelfInsertFunction(KifCallFunctionGeneral* f, KifElement* contextualpattern, KifDomain* dom, int, KifCallFunction*);

	virtual Exported void Set(short s, KifElement* k, hmap<short, KifElement*>* stacks) {}
	virtual void Setinitialisation(KifElement* ke) {};
	virtual void Seteval(int v) {};

	virtual Exported KifElement* Copy(KifDomain*, KifElement* dom = kifNULL) { return this; }
	virtual Exported KifElement* Create(KifDomain* kp, KifElement* dom, int, KifCallFunction*) {
		return Copy(kp, dom);
	}

	virtual Exported KifElement* DuplicateHardCodedValue(KifDomain*) { return this; }

	virtual Exported KifElement* Atom(bool forced = false) {
		if (popped == staticval || forced)
			return Copy(NULL);
		return this;
	}
	virtual bool isDomain() { return false; }
	virtual Exported short Name() { return -1; }
	virtual Exported void Setname(short n) {}
	virtual Exported KifElement*  Linkedvalue() { return kifNULL; }
	virtual KifElement*  Frame() { return NULL; }
	virtual KifDomainLambda*  Thedomain() { return NULL; }
	virtual void  Setframe(KifElement* fr) {}
	virtual void Disableadding() {}
	virtual Exported string String() { return ""; }
	virtual Exported string StringForDisplay() {
		return String();
	}

	virtual Exported string BaseString() {
		return String();
	}

	virtual Exported string JSonString() {
		return String();
	}

	virtual Exported long Integer() { return 0; }
	virtual Exported unsigned char Byte() {
		return (unsigned char)Integer();
	}
	virtual Exported KIFLONG Long() {
		return Integer();
	}
	virtual Exported double Float() { return 0; }
	virtual Exported KifFraction* Fraction();
	virtual Exported bool Boolean() { return false; }
	virtual Exported KifElement* Vector(KifElement*, KifVector* itself = NULL) {
		return kifNULL;
	}
	virtual Exported KifElement* Map(KifElement*, KifMap* itself = NULL) {
		return kifNULL;
	}

	virtual Exported bool Setvalue(KifElement* k, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL) {
		return true;
	}
	virtual void Initvalue(KifElement* k) {}
	virtual void Setreturnvalue(KifElement* k) {}
	virtual Exported void Sharedeclaration(KifFrame* k) {}
	virtual Exported void SetInstances(KifElement* dom, hmap<short, KifElement*>* stacks) {}
	virtual Exported void GetInstances(KifDomain* dom) {}
	virtual void* Node() { return NULL; }

	virtual void Setpostpone(bool v) {}
	virtual bool Postpone() { return false; }
	virtual short Delayed() { return false; }
	virtual void Setdelayed() {}
	virtual void ApplyDelayedInitial(int idthread, KifCallFunction*) {}

	Exported KifElement* applyfunc(KifElement* a, KifElement* b, KifDomain* kfunc);
	Exported KifElement* applyfuncvariable(KifVariable*, KifElement* a, KifElement* b, KifDomain* kfunc);
	Exported KifElement* applyfuncinframe(KifVariable*, KifElement* dom, KifElement* a, KifElement* b, KifDomain* kfunc);



	KIFLONG pureeuclidian(KIFLONG a, KIFLONG b);

	KIFLONG computeEuclidian(KIFLONG a, KIFLONG b);

	virtual Exported KifElement* Euclidian() {
		return kifNULL;
	}

	virtual Exported KifElement* FractionSimplify(bool) {
		return this;
	}

	virtual Exported KIFLONG D() {
		return 1;
	}

	virtual Exported KIFLONG N() {
		return Long();
	}

	virtual Exported void ND(KIFLONG n, KIFLONG d) {
		double f = (double)n / (double)d;
		Setvalue((KifElement*)kifcode->Providefloat(f));
	}

	virtual Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);
	virtual bool Infinite() {
		return false;
	}

	virtual void Setadd(int i) {}

	virtual Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a == this)
			return kifNULL;
		return a->orset(a, b, context, autoself);
	}
	virtual Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a == this)
			return kifNULL;
		return a->xorset(a, b, context, autoself);
	}
	virtual Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a == this)
			return kifNULL;
		return a->andset(a, b, context, autoself);
	}

	Exported virtual KifElement* plusplus() {
		return kifNULL;
	}
	Exported virtual KifElement* minusminus() {
		return kifNULL;
	}

	virtual Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a == this)
			return kifNULL;
		return a->plus(a, b, context, idthread,callfunc,autoself);
	}
	virtual Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a == this)
			return kifNULL;
		return a->minus(a, b, context, idthread,callfunc,autoself);
	}
	virtual Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a == this)
			return kifNULL;
		return a->multiply(a, b, context, idthread,callfunc,autoself);
	}
	virtual Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a == this)
			return kifNULL;
		return a->divide(a, b, context, idthread,callfunc,autoself);
	}
	virtual Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a == this)
			return kifNULL;
		return a->mod(a, b, context, idthread,callfunc,autoself);
	}
	virtual Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a == this)
			return kifNULL;
		return a->shiftright(a, b, context, idthread,callfunc,autoself);
	}
	virtual Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a == this)
			return kifNULL;
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);
	}
	virtual Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a == this)
			return kifNULL;
		return a->power(a, b, context, idthread,callfunc,autoself);
	}

	virtual Exported KifElement* less(KifElement* a) {
		return kifFALSE;
	}
	virtual Exported KifElement* more(KifElement* a) {
		return kifFALSE;
	}

	virtual Exported KifElement* same(KifElement* a) {
		if (a == this)
			return kifTRUE;
		return kifFALSE;
	}

	virtual Exported KifElement* different(KifElement* a) {
		if (same(a) == kifFALSE)
			return kifTRUE;
		return kifFALSE;
	}

	virtual Exported KifElement* lessequal(KifElement* a) {
		return kifFALSE;
	}
	virtual Exported KifElement* moreequal(KifElement* a) {
		return kifFALSE;
	}

	virtual void Setaction(KifOperator a) {}
	virtual KifOperator Action() {
		return kifNONE;
	}

	virtual KifElement* ExecuteMethod(short name, KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		return kifNULL;
	}

	virtual KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		return ExecuteMethod(name, contextualpattern, domain, callfunc);
	}

	Exported KifElement* Kifpredefined(short n);
	Exported KifElement* Kifpredefined(string n);
	Exported KifCode* KifCurrent();
	virtual ~KifElement() {}
	//If you define your own library, you might need to overload this one
	virtual int Yourowntype() {
		return -1;
	}

	virtual KifElement* Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, bool);
	virtual KifElement* FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool);

	virtual void Setchoice(char v) {}
	virtual char Choice() { return 0; }

};

class KifEvaluate : public KifElement {
public:
	KifThreadLock _locker;
	bool evaluate;

	void Toassign(bool v) {
		evaluate = v;
	}

	void Setevaluate(bool v) {
		evaluate = v;
	}

	bool Evaluate() {
		return evaluate;
	}

	bool Assigning(KifType t) {
		if (type == t)
			return evaluate;

		if (t == kifAny && type >= kifVector && type <= kifVectorFloat)
			return evaluate;
		if (type == kifVector && t >= kifVectorString && t <= kifVectorFloat)
			return evaluate;
		if (type == kifMap && t >= kifMapString && t <= kifMapFloatFloat)
			return evaluate;
		if (type == kifTreeMap && t >= kifTreeMapString && t <= kifTreeMapFloatFloat)
			return evaluate;
		if (type == kifPrimeMap && t >= kifPrimeMapString && t <= kifPrimeMapFloatFloat)
			return evaluate;
		return false;
	}

	KifEvaluate(KifCode* klc, KifElement* kp, KifType ty) : _locker(NULL, false), evaluate(false), KifElement(klc, kp, ty) {}

	bool testfunction(KifElement* kfunc) {
		if (kfunc != NULL && kfunc->isFunction())  {
			if (kfunc->Size() == 2)
				return true;
			if (kfunc->Size() == 3) {//within a frame
				KifType ty = kfunc->Argument(0)->VariableValue()->inType();
				if (ty != kifFrame && ty != kifDomain)
					return false;
				return true;
			}
			return false;
		}
		return true;
	}

	bool aContainer() {
		return true;
	}

	bool Basic() {
		return true;
	}

	bool Statique() {
		return true;
	}

	virtual unsigned int EditDistance(KifElement*);
	virtual void storeInLoop(KifElement** alls);
	virtual KifElement* apply(KifCallFunctionGeneral* kfunc, KifElement* c, KifDomain* dom, KifCallFunction* callfunc);
	KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);

	Exported virtual KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported virtual KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported virtual KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported virtual KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported virtual KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported virtual KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported virtual KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported virtual KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported virtual KifElement* xorset(KifElement* a, KifElement* b, KifElement* context,int idthread,KifCallFunction* callfunc, bool autoself);
	Exported virtual KifElement* orset(KifElement* a, KifElement* b, KifElement* context,int idthread,KifCallFunction* callfunc, bool autoself);
	Exported virtual KifElement* andset(KifElement* a, KifElement* b, KifElement* context,int idthread,KifCallFunction* callfunc, bool autoself);

	virtual Exported KifElement* less(KifElement* a);
	virtual Exported KifElement* more(KifElement* a);
	virtual Exported KifElement* lessequal(KifElement* a);
	virtual Exported KifElement* moreequal(KifElement* a);

};

class KifEvaluateMap : public KifEvaluate {
public:

	unsigned int EditDistance(KifElement*);
	KifEvaluateMap(KifCode* klc, KifElement* kp, KifType ty) : KifEvaluate(klc, kp, ty) {}
	virtual KifElement* apply(KifCallFunctionGeneral* kfunc, KifElement* c, KifDomain* dom, KifCallFunction* callfunc);
	KifElement* Match(KifBaseMap* params, KifDomain* dom, int, KifCallFunction*);
	KifElement* Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell* clf, KifFunctionLambda* bd, KifElement* var, KifElement* kcont, bool additer);
	KifElement* Split(KifBaseMap* params, KifCallFunction*);
	KifElement* MatchPattern(KifElement* params, KifDomain* dom, int, KifCallFunction*);
};

class KifEvaluateMapValue : public KifEvaluateMap {
public:

	KifEvaluateMapValue(KifCode* klc, KifElement* kp, KifType ty) : KifEvaluateMap(klc, kp, ty) {}
	KifElement* apply(KifCallFunctionGeneral* kfunc, KifElement* c, KifDomain* dom, KifCallFunction* callfunc);

};

class KifPredicateContainer : public KifElement {
public:
	hmap<short, vector<KifPredicateRule*> > rules;

	KifPredicateContainer(KifCode* klc) : KifElement(klc, NULL, kifPredicateContainer) {}

};

class KifPosition : public KifElement {
public:
	long line;
	int fileidx;
	void Setline(long l, int fidx) {
		line = l;
		fileidx = fidx;
	}

	long Line() {
		return line;
	}

	int Fileindex() {
		return fileidx;
	}

	KifPosition(KifCode* klc, KifElement* kp, KifType t);
};


class KifCommonVariable : public KifPosition {
public:
	KifElement* value;

	KifCommonVariable(KifCode* klc, KifElement* kp, KifElement* v, KifType t) : value(v), KifPosition(klc, kp, t) {}
	KifElement* VariableValue() {
		return value;
	}

	void  SetVariableValue(KifElement* v) {
		value = v;
	}

	bool isValidVariable() {
		return true;
	}
};


class KifCallMethod : public KifElement {
public:
	short name;
	unsigned int arity;

	KifCallMethod(short n, unsigned int a) : name(n), arity(a), KifElement(NULL, NULL, kifCallMethod) {}

	KifCallMethod() : KifElement(NULL, NULL, kifCallMethod) {}
	void Setname(short n) {
		name = n;
	}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* domain, KifElement* value, int idthread, KifCallFunction* func) {
		return value->ExecMethod(name, contextualpattern, domain, idthread, func);
	}
	Exported KifElement* Copy(KifDomain*, KifElement* ke = kifNULL) {
		return this;
	}
};

class KifBasic : public KifElement {
public:

	KifBasic(KifCode* klc, KifElement* kp, KifType k) : KifElement(klc, kp, k) {}
	//Return the predefined function bodies: type, list or isa	
	//What happens when a KifPerso element is evaluated
	virtual Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* idx = kifNULL) {
		return this;
	}

	virtual bool testfunction(KifElement* kfunc) {
		return true;
	}

	virtual KifElement* Declared(short s) {
		return Declaration(s);
	}

	//You can also overload Yourowntype for a better detection of your own new types...
	virtual int Yourowntype() { //this method returns -1 by default, unless you are in a new defined type
		return 1;
	}

	KifElement* ApplySelfInsertFunction(KifCallFunctionGeneral* f, KifElement* contextualpattern, KifDomain* dom, int, KifCallFunction*);

	bool Basic() {
		return true;
	}

};


class KifSelf : public KifCommonVariable {
public:
	KifThreadLock _locker;
	KifElement* parameter;
	bool localvariable;
	int clean;

	KifSelf(KifCode* klc, KifElement* kp, KifElement* v);
	bool Variable() {
		return true;
	}

	void Setlocalvariable() {
		localvariable = true;
	}

	bool Localvariable() {
		return localvariable;
	}

	Exported void Setreference(int inc = 1);
	Exported void Resetreference(int inc = 1);

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL) {
		return value;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*) {
		return value;
	}

	virtual Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	virtual Exported bool Setvalue(KifElement* ke, KifElement* index, KifElement* dom,int idthread=0,KifCallFunction* callfunc=NULL);
	KifElement* Returnvalue() {
		return value;
	}
	Exported KifElement* Declaration(short n);

	Exported string String() {
		if (value != NULL && value != this)
			return value->String();
		return("");
	}

	Exported string BaseString() {
		if (value != NULL && value != this)
			return value->BaseString();
		return("");
	}

};

class KifSelfFrame : public KifSelf {
public:
	KifElement* frame;
	KifSelfFrame(KifCode* klc, KifElement* kp, KifElement* v, KifElement* fr = NULL) : KifSelf(klc, kp, v) {
		frame = fr;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);
	Exported bool Setvalue(KifElement* ke, KifElement* index, KifElement* dom,int idthread=0,KifCallFunction* callfunc=NULL);

	KifElement* Frame() {
		return frame;
	}

	void  Setframe(KifElement* fr) {
		frame = fr->Frame();
	}
};

class KifEnum : public KifElement {
public:
	vector<KifElement*> values;

	KifEnum(KifCode* klc, KifElement* kp) : KifElement(klc, kp, kifEnum) {}
	Exported void Add(KifElement* ke) {
		values.push_back(ke);
	}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	Exported string String();

	Exported long Integer() {
		return values.size();
	}
	Exported double Float() {
		return values.size();
	}

};



class KifDate : public KifElement {
public:
	time_t value;

	Exported KifDate(KifCode* klc, KifElement* kp, time_t v = 0);

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return new KifDate(kifcode, NULL, value);
	}
	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);
	Exported bool initialconstructor() { return true; }

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);


	bool Basic() {
		return true;
	}

	char inCode() {
		return ckifFloat;
	}

	bool Simple() {
		return true;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		return this;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return this;
	}

	bool Predefinedtype() { return true; }
	Exported bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);

	Exported KifElement* Newinstance();

	Exported string String();
	Exported long Integer() {
		return value;
	}
	Exported double Float() {
		return value;
	}

	Exported bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (kfunc == NULL) {
			if (b->type == kifInteger)
				return b->same(a);
			return b->in(a, b, NULL, contextualpattern, true);
		}
		else
			return applyfunc(a, b, kfunc);
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);


	Exported KifElement* less(KifElement* a) {
		if (value<a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* more(KifElement* a) {
		if (value>a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* same(KifElement* a) {
		if (value == a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* different(KifElement* a) {
		if (value != a->Integer())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* lessequal(KifElement* a) {
		if (value <= a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* moreequal(KifElement* a) {
		if (value >= a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported long Size() {
		return value;
	}
};

class KifTime : public KifElement {
public:
	timeval value;

	Exported KifTime(KifCode* klc, KifElement* kp);

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		KifTime* tim = new KifTime(kifcode, NULL);
		tim->value = value;
		return tim;
	}
	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	bool Basic() {
		return true;
	}
	bool Simple() {
		return true;
	}

	char inCode() {
		return ckifFloat;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		return this;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return this;
	}

	bool Predefinedtype() { return true; }
	Exported bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);

	Exported KifElement* Newinstance();

	Exported string String();

	Exported double Float() {
		double mt = (((unsigned long)value.tv_sec) * 1000 + ((unsigned long)value.tv_usec) / 1000.0) + 0.5;
		return mt;
	}

	Exported long Integer() {
		return Float();
	}

	Exported bool Boolean() {
		return true;
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (kfunc == NULL) {
			if (b->type == kifInteger)
				return b->same(a);
			return b->in(a, b, NULL, contextualpattern, true);
		}
		else
			return applyfunc(a, b, kfunc);
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);


	Exported KifElement* less(KifElement* a) {
		double mt = (((unsigned long)value.tv_sec) * 1000 + ((unsigned long)value.tv_usec) / 1000.0) + 0.5;
		if (mt<a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* more(KifElement* a) {
		if (Float()>a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* same(KifElement* a) {
		if (Float() == a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* different(KifElement* a) {
		if (Float() != a->Float())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* lessequal(KifElement* a) {
		if (Float() <= a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* moreequal(KifElement* a) {
		if (Float() >= a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported long Size() {
		return Integer();
	}
};

class KifInteger : public KifElement {
public:
	long value;
	KifInteger(KifCode* klc, KifElement* kp, long v = 0, KifType t = kifInteger) : value(v), KifElement(klc, kp, t) {}

	Exported virtual KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;
		return kifcode->Provideinteger(value);
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return kifcode->Provideinteger(value);
	}

	bool Succeed() {
		if (value == -1)
			return false;
		return true;
	}

	Exported KifFraction* Fraction();

	bool aNumber() {
		return true;
	}

	void storevalue(long v) {
		value = v;
	}

	void storevalue(double v) {
		value = (long)v;
	}

	char inCode() {
		return ckifInteger;
	}

	virtual int Garbager() {
		return kifInteger;
	}

	virtual int Ingarbage() {
		return -100;
	}

	virtual Exported void Resetreference(int inc = 1);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	bool Basic() {
		return true;
	}
	bool Simple() {
		return true;
	}

	bool Statique() {
		return true;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		return this;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return this;
	}

	bool Predefinedtype() { return true; }
	virtual Exported bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);

	Exported KifElement* Newinstance() {
		return kifcode->Provideinteger(0);
	}

	Exported string String();

	Exported long Integer() {
		return value;
	}
	Exported double Float() {
		return value;
	}
	Exported bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (kfunc == NULL) {
			if (b->type == kifInteger)
				return b->same(a);
			return b->in(a, b, NULL, contextualpattern, true);
		}
		else
			return applyfunc(a, b, kfunc);
	}

	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value &= b->Integer();
			return this;
		}
		long i = a->Integer() & b->Integer();
		return kifcode->Provideinteger(i);

	}

	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value |= b->Integer();
			return this;
		}
		long i = a->Integer() | b->Integer();
		return kifcode->Provideinteger(i);

	}

	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value ^= b->Integer();
			return this;
		}
		long i = a->Integer() ^ b->Integer();
		return kifcode->Provideinteger(i);

	}

	Exported KifElement* plusplus() {
		value++;
		return this;
	}
	Exported KifElement* minusminus() {
		value--;
		return this;
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value += b->Integer();
			return this;
		}
		long i = a->Integer() + b->Integer();
		return kifcode->Provideinteger(i);

	}
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value -= b->Integer();
			return this;
		}
		long i = a->Integer() - b->Integer();
		return kifcode->Provideinteger(i);
	}

	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value >>= b->Integer();
			return this;
		}
		long i = a->Integer() >> b->Integer();
		return kifcode->Provideinteger(i);
	}

	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* less(KifElement* a) {
		if (value<a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* more(KifElement* a) {
		if (value>a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* same(KifElement* a) {
		if (value == a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* different(KifElement* a) {
		if (value != a->Integer())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* lessequal(KifElement* a) {
		if (value <= a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* moreequal(KifElement* a) {
		if (value >= a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported long Size() {
		return sizeof(value);
	}
};

const short kintsz = sizeof(long) >> 1;
const short kdoublesz = sizeof(double) >> 1;
const short klongsz = sizeof(KIFLONG) >> 1;

union kdouble {
	double v;
	unsigned short t[kdoublesz];
	kdouble() {
		v = 0;
		memset(t, 0, kdoublesz);
	}
};


union kinteger {
	long v;
	unsigned short t[kintsz];
	kinteger() {
		v = 0;
		memset(t, 0, kintsz);
	}
};

union klong {
	KIFLONG v;
	unsigned short t[klongsz];
	klong() {
		v = 0;
		memset(t, 0, klongsz);
	}
};


class KifByte : public KifElement {
public:
	unsigned char value;
	KifByte(KifCode* klc, KifElement* kp, unsigned char v = 0, KifType t = kifByte) : value(v), KifElement(klc, kp, t) {}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;
		return new KifByte(kifcode, NULL, value);
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return new KifByte(kifcode, NULL, value);
	}

	//A little bit weird, but it is only used to produce a string within a container.
	bool aString() {
		return true;
	}

	bool Succeed() {
		if (value == -1)
			return false;
		return true;
	}

	Exported KifFraction* Fraction();

	virtual int Garbager() {
		return kifInteger;
	}

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	bool Basic() {
		return true;
	}

	bool Simple() {
		return true;
	}

	bool Statique() {
		return true;
	}

	char inCode() {
		return ckifInteger;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		return this;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return this;
	}

	bool Predefinedtype() { return true; }
	virtual Exported bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);

	Exported KifElement* Newinstance() {
		return new KifByte(kifcode, NULL);
	}

	Exported string String();

	Exported unsigned char Byte() {
		return value;
	}

	Exported long Integer() {
		return value;
	}
	Exported double Float() {
		return value;
	}
	Exported bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (kfunc == NULL) {
			if (b->type == kifByte)
				return b->same(a);
			return b->in(a, b, NULL, contextualpattern, true);
		}
		else
			return applyfunc(a, b, kfunc);
	}

	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value &= b->Integer();
			return this;
		}
		unsigned char i = a->Integer() & b->Integer();
		return new KifByte(kifcode, NULL, i);

	}

	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value |= b->Integer();
			return this;
		}
		unsigned char i = a->Integer() | b->Integer();
		return kifcode->Provideinteger(i);

	}

	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value ^= b->Integer();
			return this;
		}
		unsigned char i = a->Integer() ^ b->Integer();
		return kifcode->Provideinteger(i);

	}

	Exported KifElement* plusplus() {
		value++;
		return this;
	}
	Exported KifElement* minusminus() {
		value--;
		return this;
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value += b->Integer();
			return this;
		}
		unsigned char i = a->Integer() + b->Integer();
		return new KifByte(kifcode, NULL, i);

	}
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value -= b->Integer();
			return this;
		}
		unsigned char i = a->Integer() - b->Integer();
		return new KifByte(kifcode, NULL, i);
	}
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value *= b->Integer();
			return this;
		}
		unsigned char i = a->Integer()*b->Integer();
		return new KifByte(kifcode, NULL, i);
	}

	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value <<= b->Integer();
			return this;
		}
		unsigned char i = a->Integer() << b->Integer();
		return new KifByte(kifcode, NULL, i);
	}


	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value >>= b->Integer();
			return this;
		}
		unsigned char i = a->Integer() >> b->Integer();
		return new KifByte(kifcode, NULL, i);
	}

	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* less(KifElement* a) {
		if (value<a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* more(KifElement* a) {
		if (value>a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* same(KifElement* a) {
		if (value == a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* different(KifElement* a) {
		if (value != a->Integer())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* lessequal(KifElement* a) {
		if (value <= a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* moreequal(KifElement* a) {
		if (value >= a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	Exported long Size() {
		return sizeof(value);
	}
};

class KifBit : public KifElement {
public:
	vector<unsigned short> bitvector;

	KifBit(KifCode* klc, KifElement* kp) : KifElement(klc, kp, kifBit) {}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		KifBit* kbit = new KifBit(kifcode, NULL);
		kbit->bitvector = bitvector;
		return kbit;
	}

	Exported KifElement* Newiterator(bool);
	Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);
	Exported bool initialconstructor() { return true; }

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return Atom(true);
	}
	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	bool Basic() {
		return true;
	}
	bool Simple() {
		return true;
	}

	bool Predefinedtype() { return true; }

	KifBit* Bits(KifElement* a, KifBit* loc) {
		if (a->type == kifBit)
			return (KifBit*)a;

		KifBit* kbit;
		int mx = bitvector.size();
		if (loc == NULL)
			kbit = new KifBit(NULL, NULL);
		else {
			kbit = loc;
			loc->bitvector.clear();
		}
		kbit->kifcode = kifcode;
		int sz;

		if (a->type == kifLong) {
			klong kl;
			kl.v = a->Long();
			for (sz = 0; sz<klongsz; sz++) {
				if (sz >= mx)
					break;
				kbit->bitvector.push_back(kl.t[sz]);
			}
			for (; sz<mx; sz++)
				kbit->bitvector.push_back(0);
			return kbit;
		}

		if (a->type == kifFloat || a->type == kifFraction) {
			kdouble kd;
			kd.v = a->Float();
			for (sz = 0; sz<kdoublesz; sz++) {
				if (sz >= mx)
					break;
				kbit->bitvector.push_back(kd.t[sz]);
			}
			for (; sz<mx; sz++)
				kbit->bitvector.push_back(0);
			return kbit;
		}

		kinteger kl;
		kl.v = a->Integer();
		for (sz = 0; sz<kintsz; sz++) {
			if (sz >= mx)
				break;
			kbit->bitvector.push_back(kl.t[sz]);
		}
		for (; sz<mx; sz++)
			kbit->bitvector.push_back(0);
		return kbit;
	}

	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);

	int Bitindex(long position) {
		return position >> 4; //16 bits
	}

	int Bitposition(long position, int bitidx) {
		return (position - (bitidx << 4));
	}

	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL) {
		if (idx != NULL) {
			int ikey = idx->Integer();
			long v = ke->Integer();
			if (v != 0 && v != 1) {
				kifcode->Returnerror("KIF(413): Incompatible value");
				return true;
			}
			int bidx = Bitindex(ikey);
			if (bidx >= bitvector.size()) {
				kifcode->Returnerror("KIF(412): Index out of bound");
				return true;
			}

			int bpos = Bitposition(ikey, bidx);
			if (v == 0)
				bitvector[bidx] &= ~(1 << bpos);
			else
				bitvector[bidx] |= 1 << bpos;
			return true;
		}

		Bits(ke, this);
		return true;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		int idthread = kifGlobals->GetThreadid();
		return Exec(contextualpattern, dom, ke, idthread, (KifCallFunction*)kifGlobals->Topid(idthread));
	}

	Exported KIFLONG Long() {
		klong kl;
		for (int sz = 0; sz<bitvector.size() && sz<klongsz; sz++)
			kl.t[sz] = bitvector[sz];
		return kl.v;
	}

	Exported long Integer() {
		kinteger kl;
		for (int sz = 0; sz<bitvector.size() && sz<kintsz; sz++)
			kl.t[sz] = bitvector[sz];
		return kl.v;
	}

	Exported double Float() {
		kdouble kl;
		for (int sz = 0; sz<bitvector.size() && sz<kdoublesz; sz++)
			kl.t[sz] = bitvector[sz];
		return kl.v;
	}

	Exported bool Boolean() {
		long v = Integer();
		if (v == 0)
			return false;
		return true;
	}
	Exported string String();


	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (kfunc == NULL) {
			if (b->type == kifBit)
				return b->same(a);
			return b->in(a, b, NULL, contextualpattern, true);
		}
		else
			return applyfunc(a, b, kfunc);
	}


	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		KifBit* kbit = Bits(b, NULL);
		int sz, i;
		KifBit* ka;
		KifBit* kv;

		if (autoself) {
			kv = this;
			ka = this;
		}
		else {
			ka = Bits(a, NULL);
			kv = new KifBit(kifcode, NULL);
		}

		//we keep the shortest
		sz = ka->bitvector.size();
		if (sz>kbit->bitvector.size())
			sz = kbit->bitvector.size();

		for (i = 0; i<sz; i++) {
			if (i >= kv->bitvector.size())
				kv->bitvector.push_back(ka->bitvector[i] & kbit->bitvector[i]);
			else
				kv->bitvector[i] = ka->bitvector[i] & kbit->bitvector[i];
		}

		while (i<kv->bitvector.size()) {
			kv->bitvector[i] = 0;
			i++;
		}

		if (kbit != (KifBit*)b)
			delete kbit;

		if (ka != this && ka != (KifBit*)a)
			delete ka;
		return kv;
	}

	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		KifBit* kbit = Bits(b, NULL);
		int sz, i;
		KifBit* ka;
		KifBit* kv;

		if (autoself) {
			kv = this;
			ka = this;
		}
		else {
			ka = Bits(a, NULL);
			kv = new KifBit(kifcode, NULL);
		}

		//we keep the shortest
		sz = ka->bitvector.size();
		if (sz>kbit->bitvector.size())
			sz = kbit->bitvector.size();

		for (i = 0; i<sz; i++) {
			if (i >= kv->bitvector.size())
				kv->bitvector.push_back(ka->bitvector[i] | kbit->bitvector[i]);
			else
				kv->bitvector[i] = ka->bitvector[i] | kbit->bitvector[i];
		}

		while (i<kbit->bitvector.size()) {
			if (i >= kv->bitvector.size())
				kv->bitvector.push_back(kbit->bitvector[i]);
			else
				kv->bitvector[i] = 0;
			i++;
		}

		if (kbit != (KifBit*)b)
			delete kbit;

		if (ka != this && ka != (KifBit*)a)
			delete ka;
		return kv;
	}

	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		KifBit* kbit = Bits(b, NULL);
		int sz, i;
		KifBit* ka;
		KifBit* kv;

		if (autoself) {
			kv = this;
			ka = this;
		}
		else {
			ka = Bits(a, NULL);
			kv = new KifBit(kifcode, NULL);
		}

		//we keep the shortest
		sz = ka->bitvector.size();
		if (sz>kbit->bitvector.size())
			sz = kbit->bitvector.size();

		for (i = 0; i<sz; i++) {
			if (i >= kv->bitvector.size())
				kv->bitvector.push_back(ka->bitvector[i] ^ kbit->bitvector[i]);
			else
				kv->bitvector[i] = ka->bitvector[i] ^ kbit->bitvector[i];
		}

		while (i<kv->bitvector.size()) {
			kv->bitvector[i] = 0;
			i++;
		}

		if (kbit != (KifBit*)b)
			delete kbit;

		if (ka != this && ka != (KifBit*)a)
			delete ka;
		return kv;
	}

	Exported KifElement* Complement() {
		KifBit* kbit = new KifBit(kifcode, NULL);
		unsigned short v;
		for (int i = 0; i<bitvector.size(); i++) {
			v = ~bitvector[i];
			kbit->bitvector.push_back(v);
		}
		return kbit;
	}

	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* same(KifElement* a) {
		if (a->type != kifBit)
			return kifFALSE;
		KifBit* kbit = (KifBit*)a;
		if (bitvector.size() != kbit->bitvector.size())
			return kifFALSE;
		for (int i = bitvector.size() - 1; i >= 0; i--) {
			if (bitvector[i] != kbit->bitvector[i])
				return kifFALSE;
		}
		return kifTRUE;
	}

	Exported long Size() {
		return bitvector.size() << 4;
	}
};


class KifBits : public KifElement {
public:
	map<short, unsigned short> bitmap;

	KifBits(KifCode* klc, KifElement* kp) : KifElement(klc, kp, kifBits) {}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		KifBits* kbit = new KifBits(kifcode, NULL);
		kbit->bitmap = bitmap;
		return kbit;
	}

	Exported KifElement* Newiterator(bool);

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return Atom(true);
	}

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);
	//Exported KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,int idthread,KifCallFunction * callfunc,bool autoself);
	//Exported KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,int idthread,KifCallFunction * callfunc,bool autoself);

	bool Basic() {
		return true;
	}
	bool Simple() {
		return true;
	}

	bool Predefinedtype() {
		return true;
	}

	KifBits* Bits(KifElement* a) {
		if (a->type == kifBits)
			return (KifBits*)a;

		KifBits* kbit = new KifBits(NULL, NULL);
		kbit->kifcode = kifcode;
		int sz;
		if (a->type == kifLong) {
			klong kl;
			kl.v = a->Long();
			for (sz = 0; sz<klongsz; sz++)
			if (kl.t[sz])
				kbit->bitmap[sz] = kl.t[sz];
			return kbit;
		}

		if (a->type == kifFloat || a->type == kifFraction) {
			kdouble kd;
			kd.v = a->Float();
			for (sz = 0; sz<kdoublesz; sz++)
			if (kd.t[sz] != 0)
				kbit->bitmap[sz] = kd.t[sz];
			return kbit;
		}

		kinteger kl;
		kl.v = a->Integer();
		for (sz = 0; sz<kintsz; sz++) {
			if (kl.t[sz] != 0)
				kbit->bitmap[sz] = kl.t[sz];
		}

		return kbit;
	}


	void Compact() {
		ThreadLock _lock(type);
		map<short, unsigned short>::iterator it;
		vector<short> keys;
		for (it = bitmap.begin(); it != bitmap.end(); it++)
		if (it->second == 0)
			keys.push_back(it->first);
		for (int i = 0; i<keys.size(); i++) {
			it = bitmap.find(keys[i]);
			bitmap.erase(it);
		}
	}

	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);

	int Bitindex(long position) {
		return position >> 4; //16 bits
	}

	int Bitposition(long position, int bitidx) {
		return (position - (bitidx << 4));
	}

	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		int idthread = kifGlobals->GetThreadid();
		return Exec(contextualpattern, dom, ke, idthread, (KifCallFunction*)kifGlobals->Topid(idthread));
	}

	Exported KIFLONG Long() {
		klong kl;
		map<short, unsigned short>::iterator it;
		for (it = bitmap.begin(); it != bitmap.end(); it++) {
			if (it->first >= 0 && it->first<klongsz)
				kl.t[it->first] = it->second;
		}
		return kl.v;
	}

	Exported long Integer() {
		kinteger kl;
		map<short, unsigned short>::iterator it;
		for (it = bitmap.begin(); it != bitmap.end(); it++) {
			if (it->first >= 0 && it->first<kintsz)
				kl.t[it->first] = it->second;
		}
		return kl.v;
	}

	Exported double Float() {
		kdouble kl;
		map<short, unsigned short>::iterator it;
		for (it = bitmap.begin(); it != bitmap.end(); it++) {
			if (it->first >= 0 && it->first<kdoublesz)
				kl.t[it->first] = it->second;
		}
		return kl.v;
	}

	Exported bool Boolean() {
		long v = Integer();
		if (v == 0)
			return false;
		return true;
	}

	Exported string String();

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (kfunc == NULL) {
			if (b->type == kifBits)
				return b->same(a);
			return b->in(a, b, NULL, contextualpattern, true);
		}
		else
			return applyfunc(a, b, kfunc);
	}


	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		KifBits* kbit = Bits(b);
		KifBits* ka;
		KifBits* kv;

		if (autoself) {
			kv = this;
			ka = this;
		}
		else {
			ka = Bits(a);
			kv = new KifBits(kifcode, NULL);
		}

		map<short, unsigned short>::iterator it;
		for (it = ka->bitmap.begin(); it != ka->bitmap.end(); it++) {
			if (kbit->bitmap.find(it->first) != kbit->bitmap.end())
				kv->bitmap[it->first] = kbit->bitmap[it->first] & it->second;
		}

		if (kbit != (KifBits*)b)
			delete kbit;

		if (ka != this && ka != (KifBits*)a)
			delete ka;
		kv->Compact();
		return kv;
	}

	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		KifBits* kbit = Bits(b);
		KifBits* ka;
		KifBits* kv;

		if (autoself) {
			kv = this;
			ka = this;
		}
		else {
			ka = Bits(a);
			kv = new KifBits(kifcode, NULL);
		}

		map<short, unsigned short>::iterator it;
		for (it = ka->bitmap.begin(); it != ka->bitmap.end(); it++) {
			if (kbit->bitmap.find(it->first) != kbit->bitmap.end())
				kv->bitmap[it->first] = kbit->bitmap[it->first] | it->second;
			else
				kv->bitmap[it->first] = it->second;
		}

		for (it = kbit->bitmap.begin(); it != kbit->bitmap.end(); it++) {
			if (ka->bitmap.find(it->first) == ka->bitmap.end())
				kv->bitmap[it->first] = it->second;
		}

		if (kbit != (KifBits*)b)
			delete kbit;

		if (ka != this && ka != (KifBits*)a)
			delete ka;
		return kv;
	}

	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		KifBits* kbit = Bits(b);
		KifBits* ka;
		KifBits* kv;

		if (autoself) {
			kv = this;
			ka = this;
		}
		else {
			ka = Bits(a);
			kv = new KifBits(kifcode, NULL);
		}

		map<short, unsigned short>::iterator it;
		for (it = ka->bitmap.begin(); it != ka->bitmap.end(); it++) {
			if (kbit->bitmap.find(it->first) != kbit->bitmap.end())
				kv->bitmap[it->first] = kbit->bitmap[it->first] ^ it->second;
			else
				kv->bitmap[it->first] = it->second;
		}

		for (it = kbit->bitmap.begin(); it != kbit->bitmap.end(); it++) {
			if (ka->bitmap.find(it->first) == ka->bitmap.end())
				kv->bitmap[it->first] = it->second;
		}

		if (kbit != (KifBits*)b)
			delete kbit;

		if (ka != this && ka != (KifBits*)a)
			delete ka;
		kv->Compact();
		return kv;
	}

	Exported KifElement* Complement() {
		KifBits* kbit = new KifBits(kifcode, NULL);
		map<short, unsigned short>::iterator it;
		for (it = bitmap.begin(); it != bitmap.end(); it++)
			kbit->bitmap[it->first] = ~it->second;
		kbit->Compact();
		return kbit;
	}

	Exported KifElement* same(KifElement* a) {
		if (a->type != kifBit)
			return kifFALSE;
		KifBits* kbit = (KifBits*)a;
		if (bitmap.size() != kbit->bitmap.size())
			return kifFALSE;
		map<short, unsigned short>::iterator it;
		for (it = bitmap.begin(); it != bitmap.end(); it++) {
			if (kbit->bitmap.find(it->first) == kbit->bitmap.end())
				return kifFALSE;
			if (kbit->bitmap[it->first] != it->second)
				return kifFALSE;
		}

		for (it = kbit->bitmap.begin(); it != kbit->bitmap.end(); it++) {
			if (bitmap.find(it->first) == kbit->bitmap.end())
				return kifFALSE;
		}
		return kifTRUE;
	}

	Exported long Size();
};

class KifLong : public KifElement {
public:
	KIFLONG value;
	KifLong(KifCode* klc, KifElement* kp, KIFLONG v = 0) : value(v), KifElement(klc, kp, kifLong) {}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return new KifLong(kifcode, NULL, value);
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return new KifLong(kifcode, NULL, value);
	}


	bool aNumber() {
		return true;
	}

	void storevalue(long v) {
		value = v;
	}

	void storevalue(double v) {
		value = v;
	}

	char inCode() {
		return ckifInteger;
	}


	Exported KifFraction* Fraction();

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	bool Basic() {
		return true;
	}

	bool Simple() {
		return true;
	}

	bool Statique() {
		return true;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		return this;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return this;
	}

	bool Predefinedtype() { return true; }
	Exported bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL) {
		value = ke->Long();
		return true;
	}

	Exported KifElement* Newinstance() {
		return new KifLong(kifcode, NULL, 0);
	}

	Exported string String();

	Exported KIFLONG Long() {
		return value;
	}

	Exported long Integer() {
		return (long)value;
	}
	Exported double Float() {
		return (double)value;
	}
	Exported bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (kfunc == NULL) {
			if (b->type == kifLong || b->type == kifInteger)
				return b->same(a);
			return b->in(a, b, NULL, contextualpattern, true);
		}
		else
			return applyfunc(a, b, kfunc);
	}

	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value &= b->Long();
			return this;
		}
		KIFLONG i = a->Long() & b->Long();
		return new KifLong(kifcode, NULL, i);
	}

	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value |= b->Long();
			return this;
		}
		KIFLONG i = a->Long() | b->Long();
		return new KifLong(kifcode, NULL, i);
	}

	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value ^= b->Long();
			return this;
		}
		KIFLONG i = a->Long() ^ b->Long();
		return new KifLong(kifcode, NULL, i);
	}

	Exported KifElement* plusplus() {
		value++;
		return this;
	}
	Exported KifElement* minusminus() {
		value--;
		return this;
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value += b->Long();
			return this;
		}
		KIFLONG i = a->Long() + b->Long();
		return new KifLong(kifcode, NULL, i);
	}

	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value -= b->Long();
			return this;
		}
		KIFLONG i = a->Long() - b->Long();
		return new KifLong(kifcode, NULL, i);
	}

	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value <<= b->Long();
			return this;
		}
		KIFLONG i = a->Long() << b->Long();
		return new KifLong(kifcode, NULL, i);
	}


	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value >>= b->Long();
			return this;
		}
		KIFLONG i = a->Long() >> b->Long();
		return new KifLong(kifcode, NULL, i);
	}

	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* less(KifElement* a) {
		if (value<a->Long())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* more(KifElement* a) {
		if (value>a->Long())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* same(KifElement* a) {
		if (value == a->Long())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* different(KifElement* a) {
		if (value != a->Long())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* lessequal(KifElement* a) {
		if (value <= a->Long())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* moreequal(KifElement* a) {
		if (value >= a->Long())
			return kifTRUE;
		return kifFALSE;
	}
	Exported long Size() {
		return sizeof(value);
	}
};


class KifFraction : public KifElement {
public:
	KIFLONG numerator;
	KIFLONG denominator;

	KifFraction(KifCode* klc, KifElement* kp, KIFLONG n, KIFLONG d, bool simplified = false) : numerator(n), denominator(d), KifElement(klc, kp, kifFraction) {
		if (simplified == false)
			Simplify();
	}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return new KifFraction(kifcode, NULL, numerator, denominator, true);
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return new KifFraction(kifcode, NULL, numerator, denominator, true);
	}

	bool aNumber() {
		return true;
	}

	char inCode() {
		return ckifFloat;
	}

	Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);
	Exported bool initialconstructor() { return true; }

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	bool Basic() {
		return true;
	}

	bool Simple() {
		return true;
	}

	bool Statique() {
		return true;
	}

	Exported KifFraction* Fraction() {
		return this;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		return this;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return this;
	}

	bool Predefinedtype() { return true; }

	Exported bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL) {
		if (ke->type != kifFraction)
			ke = ke->Fraction();

		if (ke->D() == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return true;
		}
		denominator = ke->D();
		numerator = ke->N();
		Simplify();
		return true;
	}

	Exported KifElement* Newinstance() {
		return kifcode->Provideinteger(0);
	}

	Exported string String();
	Exported long Integer() {
		if (denominator == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return 0;
		}
		double v = (double)numerator / (double)denominator;
		return ((long)v);
	}

	Exported double Float() {
		if (denominator == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return 0;
		}
		double v = (double)numerator / (double)denominator;
		return (v);
	}
	Exported bool Boolean() {
		if (denominator == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return false;
		}
		KIFLONG v = numerator / denominator;
		if (v == 0)
			return false;
		return true;
	}

	Exported void ND(KIFLONG n, KIFLONG d) {
		numerator = n;
		denominator = d;
		Simplify();

	}

	KIFLONG euclidian(KIFLONG a, KIFLONG b) {
		if (b == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return 0;
		}
		if (a<b)
			return 1;
		KIFLONG remain = a%b;
		if (remain == 0)
			return b;
		return euclidian(b, remain);
	}

	Exported KifElement* Euclidian() {
		if (denominator == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return 0;
		}
		KIFLONG res;
		if (denominator>numerator)
			res = euclidian(denominator, numerator);
		else
			res = euclidian(numerator, denominator);
		return kifcode->Provideinteger(res);
	}

	void Simplify() {
		if (denominator == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return;
		}
		if (denominator == 1 || numerator == 0)
			return;
		if (denominator == numerator) {
			numerator = 1;
			denominator = 1;
			return;
		}
		KIFLONG res;
		if (denominator>numerator)
			res = euclidian(denominator, numerator);
		else
			res = euclidian(numerator, denominator);
		if (res != 1)	{
			numerator /= res;
			denominator /= res;
		}
		if (numerator<0) {
			numerator *= -1;
			denominator *= -1;
		}
	}


	Exported KifElement* FractionSimplify(bool s) {
		if (denominator == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return 0;
		}
		if (denominator == 1 || numerator == 0)
			return this;
		if (denominator == numerator)
			return new KifFraction(kifcode, NULL, 1, 1, true);

		KIFLONG res;
		if (numerator<0) {
			numerator *= -1;
			denominator *= -1;
		}

		if (denominator>numerator)
			res = euclidian(denominator, numerator);
		else
			res = euclidian(numerator, denominator);

		if (res == 1)
			return this;

		if (s == true) {
			numerator /= res;
			denominator /= res;
			return this;
		}

		KIFLONG n = numerator / res;
		KIFLONG d = denominator / res;
		return new KifFraction(kifcode, NULL, n, d, true);
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (kfunc == NULL) {
			if (b->type == kifInteger || b->type == kifLong)
				return b->same(a);
			return b->in(a, b, NULL, contextualpattern, true);
		}
		else
			return applyfunc(a, b, kfunc);
	}

	Exported KIFLONG D() {
		return denominator;
	}

	Exported KIFLONG N() {
		return numerator;
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		KIFLONG dea = a->D();
		if (dea == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return kifFALSE;
		}
		KIFLONG nua = a->N();
		if (b->type != kifFraction)
			b = b->Fraction();

		KIFLONG nub = b->N();
		KIFLONG deb = b->D();
		if (deb == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return kifFALSE;
		}

		nub = b->N();
		if (deb != dea) {
			nua *= deb;
			nub *= dea;
			dea *= deb;
		}

		nua += nub;
		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new KifFraction(kifcode, NULL, nua, dea);
	}

	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		KIFLONG dea = a->D();
		if (dea == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return kifFALSE;
		}
		KIFLONG nua = a->N();
		if (b->type != kifFraction)
			b = b->Fraction();

		KIFLONG nub = b->N();
		KIFLONG deb = b->D();
		if (deb == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return kifFALSE;
		}

		nub = b->N();
		if (deb != dea) {
			nua *= deb;
			nub *= dea;
			dea *= deb;
		}

		nua -= nub;
		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new KifFraction(kifcode, NULL, nua, dea);
	}

	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		KIFLONG dea = a->D();
		if (dea == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return kifFALSE;
		}
		KIFLONG nua = a->N();

		if (b->type != kifFraction)
			b = b->Fraction();

		if (b->D() == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return kifFALSE;
		}

		nua *= b->N();
		dea *= b->D();

		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new KifFraction(kifcode, NULL, nua, dea);
	}

	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		KIFLONG dea = a->D();
		if (dea == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return kifFALSE;
		}
		KIFLONG nua = a->N();

		if (b->type != kifFraction)
			b = b->Fraction();

		KIFLONG deb = b->D();
		if (deb == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return kifFALSE;
		}
		nua *= deb;
		dea *= b->N();
		if (dea == 0) {
			kifcode->Returnerror("MATH(100): Error wrong fraction: denominator is 0");
			return kifFALSE;
		}

		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new KifFraction(kifcode, NULL, nua, dea);
	}

	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		KIFLONG i = a->Long() << b->Long();
		return new KifLong(kifcode, NULL, i);
	}


	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		KIFLONG i = a->Long() >> b->Long();
		return new KifLong(kifcode, NULL, i);
	}

	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* less(KifElement* a) {
		if (Float()<a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* more(KifElement* a) {
		if (Float()>a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* same(KifElement* a) {
		if (Float() == a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* different(KifElement* a) {
		if (Float() != a->Float())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* lessequal(KifElement* a) {
		if (Float() <= a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* moreequal(KifElement* a) {
		if (Float() >= a->Float())
			return kifTRUE;
		return kifFALSE;
	}

	Exported long Size() {
		return Integer();
	}
};



//Then KifString
class KifString : public KifElement {
public:
	KifThreadLock _locker;
	string value;

	KifString(KifCode* klc, KifElement* kp, string v = "", KifType t = kifString);


	bool aString() {
		return true;
	}

	unsigned int EditDistance(KifElement*);
	Exported virtual KifElement* Atom(bool forced = false);
	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);
	Exported void Clean();
	Exported void Clear();
	void storeInLoop(KifElement** alls);

	virtual int Garbager() {
		return kifString;
	}

	char inCode() {
		return ckifString;
	}

	bool Basic() {
		return true;
	}
	bool Simple() {
		return true;
	}
	bool Statique() {
		return true;
	}

	Exported void Push(KifElement* ke);

	Exported KifElement* Newiterator(bool);

	virtual int Ingarbage() {
		return -100;
	}

	virtual Exported void Resetreference(int inc = 1);

	bool isString() {
		return true;
	}

	Exported KifElement* Newinstance() {
		return kifcode->Providestringraw("");
	}

	void Pop(KifElement* ke);
	KifElement* Last();


	virtual Exported bool Setvalue(KifElement* ke, KifElement* idx = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);

	Exported long Size();

	bool Predefinedtype() { return true; }
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*);

	Exported virtual string BaseString();
	Exported virtual string String();
	Exported string JSonString();
	Exported unsigned char Byte();

	Exported long Integer();
	Exported double Float();
	Exported bool Boolean();


	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);


	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* less(KifElement* a);
	Exported KifElement* more(KifElement* a);
	Exported KifElement* same(KifElement* a);
	Exported KifElement* different(KifElement* a);
	Exported KifElement* lessequal(KifElement* a);
	Exported KifElement* moreequal(KifElement* a);

	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);

	KifElement* Haskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, bool);
	KifElement* FilterHaskell(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunctionHaskell*, KifFunctionLambda*, KifElement* var, KifElement* kcont, KifElement* accu, KifElement* init, bool);
};

class KifFloat : public KifElement {
public:
	double value;
	KifFloat(KifCode* klc, KifElement* kp, double v = 0, KifType t = kifFloat) : value(v), KifElement(klc, kp, t) {}
	Exported virtual KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return kifcode->Providefloat(value);
	}
	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return kifcode->Providefloat(value);
	}

	bool aNumber() {
		return true;
	}

	void storevalue(long v) {
		value = (double)v;
	}

	void storevalue(double v) {
		value = v;
	}

	virtual double theValue() {
		return value;
	}

	virtual void inValue(double v) {
		value = v;
	}

	virtual int Garbager() {
		return kifFloat;
	}

	char inCode() {
		return ckifFloat;
	}

	Exported KifFraction* Fraction();
	virtual int Ingarbage() {
		return -100;
	}

	virtual Exported void Resetreference(int inc = 1);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	bool Simple() {
		return true;
	}
	bool Statique() {
		return true;
	}

	bool Basic() {
		return true;
	}

	Exported KifElement* Newinstance() {
		return kifcode->Providefloat(0);
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		return this;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return this;
	}

	bool Predefinedtype() { return true; }

	Exported long Size() {
		return sizeof(value);
	}

	virtual Exported bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);
	Exported string String();
	Exported long Integer() {
		return value;
	}
	Exported double Float() {
		return value;
	}
	Exported bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}


	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (kfunc == NULL) {
			if (b->type == kifFloat)
				return b->same(a);
			return b->in(a, b, NULL, contextualpattern, true);
		}
		else
			return applyfunc(a, b, kfunc);
	}


	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value = (long)value & b->Integer();
			return this;
		}
		long i = a->Integer() & b->Integer();
		return kifcode->Provideinteger(i);

	}

	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value = (long)value | b->Integer();
			return this;
		}
		long i = a->Integer() | b->Integer();
		return kifcode->Provideinteger(i);

	}

	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (autoself) {
			value = (long)value^b->Integer();
			return this;
		}
		long i = a->Integer() ^ b->Integer();
		return kifcode->Provideinteger(i);
	}

	Exported KifElement* plusplus() {
		value++;
		return this;
	}
	Exported KifElement* minusminus() {
		value--;
		return this;
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value += b->Float();
			return this;
		}
		double i = a->Float() + b->Float();
		return kifcode->Providefloat(i);
	}

	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value -= b->Float();
			return this;
		}
		double i = a->Float() - b->Float();
		return kifcode->Providefloat(i);
	}

	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value *= b->Float();
			return this;
		}
		double i = a->Float()*b->Float();
		return kifcode->Providefloat(i);
	}

	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value = (long)value << b->Integer();
			return this;
		}
		double i = a->Integer() << b->Integer();
		return kifcode->Providefloat(i);
	}

	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (autoself) {
			value = (long)value >> b->Integer();
			return this;
		}
		double i = a->Integer() >> b->Integer();
		return kifcode->Providefloat(i);
	}

	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);

	Exported KifElement* less(KifElement* a) {
		if (value<a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* more(KifElement* a) {
		if (value>a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* same(KifElement* a) {
		if (value == a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* different(KifElement* a) {
		if (value != a->Float())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* lessequal(KifElement* a) {
		if (value <= a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* moreequal(KifElement* a) {
		if (value >= a->Float())
			return kifTRUE;
		return kifFALSE;
	}
};


class KifBaseString : public KifString {
public:
	KifBaseString(KifCode* klc, KifElement* kp, string v = "") : KifString(klc, kp, v) {}

	Exported KifElement* Atom(bool forced = false) {
		return kifcode->Providestring(value);
	}

	void Release() {}

	bool Basevalue() {
		return true;
	}
	Exported KifElement* DuplicateHardCodedValue(KifDomain* dom) {
		return kifcode->Providestring(value);
	}

	int Ingarbage() {
		return idgarbage;
	}

	Exported void Resetreference(int inc = 1) {}

	int Garbager() {
		return 0;
	}

	Exported string BaseString();
	Exported string String() {
		return value;
	}

};

class KifBaseInteger : public KifInteger {
public:
	KifBaseInteger(KifCode* klc, KifElement* kp, long v = 0) : KifInteger(klc, kp, v) {}
	bool Basevalue() {
		return true;
	}

	Exported KifElement* Atom(bool forced = false) {
		return kifcode->Provideinteger(value);
	}

	Exported KifElement* DuplicateHardCodedValue(KifDomain* dom) {
		return kifcode->Provideinteger(value);
	}

	int Ingarbage() {
		return idgarbage;
	}

	Exported void Resetreference(int inc = 1) {}

	void Release() {}

	int Garbager() {
		return 0;
	}

};

class KifBaseLong : public KifLong {
public:
	KifBaseLong(KifCode* klc, KifElement* kp, KIFLONG v = 0) : KifLong(klc, kp, v) {}
	bool Basevalue() {
		return true;
	}

	Exported KifElement* Atom(bool forced = false) {
		return new KifLong(kifcode, NULL, value);
	}

	Exported KifElement* DuplicateHardCodedValue(KifDomain* dom) {
		return new KifLong(kifcode, NULL, value);
	}

	int Ingarbage() {
		return idgarbage;
	}

	Exported void Resetreference(int inc = 1) {}

	void Release() {}

	int Garbager() {
		return 0;
	}

};

class KifBaseFloat : public KifFloat {
public:
	KifBaseFloat(KifCode* klc, KifElement* kp, double v = 0) : KifFloat(klc, kp, v) {}
	bool Basevalue() {
		return true;
	}

	Exported KifElement* Atom(bool forced = false) {
		return kifcode->Providefloat(value);
	}

	Exported KifElement* DuplicateHardCodedValue(KifDomain* dom) {
		return kifcode->Providefloat(value);
	}

	Exported void Resetreference(int inc = 1) {}

	int Ingarbage() {
		return idgarbage;
	}

	void Release() {}

	int Garbager() {
		return 0;
	}

};


class KifBoolean : public KifElement {
public:
	bool value;
	KifBoolean(KifCode* klc, KifElement* kp, bool v = false, KifType ty = kifBoolean);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		return this;
	}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return this;
	}

	bool Succeed() {
		return value;
	}

	Exported virtual bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);

	Exported KifElement* Newinstance() {
		return new KifBoolean(kifcode, NULL);
	}
	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return new KifBoolean(kifcode, NULL, value);
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return new KifBoolean(kifcode, (KifElement*)kp, value);
	}

	Exported KifElement* DuplicateHardCodedValue(KifDomain* dom) {
		return new KifBoolean(kifcode, NULL, value);
	}

	bool Basic() {
		return true;
	}
	bool Simple() {
		return true;
	}
	bool Statique() {
		return true;
	}

	char inCode() {
		return ckifInteger;
	}

	bool Predefinedtype() { return true; }

	Exported string String();

	Exported long Integer() {
		if (value)
			return 1;
		return 0;
	}
	Exported double Float() {
		if (value)
			return 1;
		return 0;
	}
	Exported bool Boolean() {
		return value;
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (b == this) {
			if (a->Boolean() == b->Boolean())
				return kifTRUE;
			return kifFALSE;
		}

		KifElement* res = b->in(a, b, kfunc, this, idx);
		if (res->Boolean() == false || res == kifNULL)
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		bool vb = b->Boolean();
		if (autoself) {
			if (vb)
				value = true;
			if (value)
				return kifTRUE;
			return kifFALSE;
		}

		bool va = a->Boolean();
		if (va || vb)
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		bool vb = b->Boolean();
		if (autoself) {
			value ^= vb;
			if (value)
				return kifTRUE;
			return kifFALSE;
		}
		bool va = a->Boolean();
		va ^= vb;
		if (va)
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		bool vb = b->Boolean();
		if (autoself) {
			value &= vb;
			if (value)
				return kifTRUE;
			return kifFALSE;
		}
		bool va = a->Boolean();
		va &= vb;
		if (va)
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		bool vb = b->Boolean();
		if (autoself) {
			value |= vb;
			if (value)
				return kifTRUE;
			return kifFALSE;
		}
		bool va = a->Boolean();
		va |= vb;
		if (va)
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		bool vb = b->Boolean();
		if (autoself) {
			if (value == vb) {
				value = false;
				return kifFALSE;
			}
			if (value)
				return kifTRUE;
			return kifFALSE;
		}

		bool va = a->Boolean();
		if (va == vb)
			return kifFALSE;

		if (va)
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* less(KifElement* a) {
		if (value<a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* more(KifElement* a) {
		if (value>a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* same(KifElement* a) {
		if (value == a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* different(KifElement* a) {
		if (value != a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* lessequal(KifElement* a) {
		if (value <= a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* moreequal(KifElement* a) {
		if (value >= a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}

};

class KifBaseBoolean : public KifBoolean {
public:

	KifBaseBoolean(KifCode* klc, KifElement* kp, bool v = false, KifType ty = kifBoolean) : KifBoolean(klc, kp, v, ty) {}
	Exported bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL) {
		return true;
	}
};

class KifError : public KifElement  {
public:
	vector<string> stackmessages;
	string value;
	bool execution;

	void Release() {}

	KifError(KifElement* kp, string message) : execution(true), value(message), KifElement(NULL, kp, kifError) {}

	Exported long Integer();
	Exported double Float() {
		return -1;
	}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	string String();
	Exported void Resetreference(int inc = 1) {}
};

class KifBaseFile : public KifElement {
public:
	string filename;
	KifElement* local;
	KifType localtype;
	KifBaseString* localstring;
	char mode;
	fstream* thefile;
	bool signature;
	bool first;

	KifBaseFile(KifCode* klc, KifElement* kp, KifType ty) : first(true), signature(false), thefile(NULL), localstring(new KifBaseString(NULL, NULL)), mode(0), KifElement(klc, kp, ty) {
		localstring->kifcode = kifcode;
	}

	virtual KifElement* EndOfFile() {
		return kifTRUE;
	}

	Exported KifElement* Newiterator(bool rev);

	Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);
	Exported bool initialconstructor() { return true; }

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	bool Basic() {
		return true;
	}

	virtual void raz() {
		thefile = NULL;
		first = true;
	}

	virtual void initwhile(KifElement* var) {}

	virtual bool Openread(string n) {
		ThreadLock _lock(type);
		if (thefile != NULL)
			return false;

		n = Getfilename(n);
		thefile = new fstream((char*)n.c_str(), ios::in | ios::binary);
		if (thefile->fail()) {
			raz();
			return false;
		}
		filename = n;
		mode = 1;
		return true;
	}

	bool Openwrite(string n) {
		ThreadLock _lock(type);
		if (thefile != NULL)
			return false;
		n = Getfilename(n);
		thefile = new fstream((char*)n.c_str(), ios::out | ios::binary);
		if (thefile->fail()) {
			raz();
			return false;
		}
		filename = n;
		mode = 2;
		return true;
	}

	bool Openappend(string n) {
		ThreadLock _lock(type);
		if (thefile != NULL)
			return false;
		n = Getfilename(n);
		thefile = new fstream((char*)n.c_str(), ios::in | ios::out | ios::ate | ios::binary);
		if (thefile->fail()) {
			raz();
			return false;
		}
		filename = n;
		mode = 3;
		return true;
	}

	virtual void Close() {}

	Exported string String() {
		return filename;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val) {
		return this;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return this;
	}

	Exported long Size();
	Exported long Integer();
	Exported double Float();
	virtual bool Write(string n) {
		return false;
	}

	virtual bool WriteBin(int c) {
		return false;
	}

	virtual long Get() {
		return -1;
	}

	virtual KifElement* Read(int nb) {
		return kifNULL;
	}

	virtual KifElement* Readoneline() {
		return kifNULL;
	}

	virtual KifElement* Find(KifElement*, agnostring& s, bool u) {
		return kifNULL;
	}

	virtual KifElement* Seek(long i) {
		return kifNULL;
	}
	virtual KifElement* Tell() {
		return kifNULL;
	}

	Exported void Clean() {
		if (reference <= 1)
			Close();
		KifElement::Resetreference();
	}

	Exported void Resetreference(int inc = 1) {
		if (reference <= 1)
			Close();
		KifElement::Resetreference(inc);
	}

	Exported void Clear() {
		Close();
	}

	virtual Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		return kifNULL;
	}
	virtual Exported KifElement* whilein(KifElement* var) {
		return kifNULL;
	}

	bool isfile() {
		return true;
	}

	bool Flush() {
		if (mode == 2) {
			thefile->flush();
			return true;
		}
		return false;
	}

	virtual void unget() {}
	virtual void clear() {}

	void Setsignature(bool s) {
		signature = s;
	}

	KifElement* Getsignature() {
		if (signature)
			return kifTRUE;
		return kifFALSE;
	}
};


class KifFile : public KifBaseFile {
public:

	FILE* fileread;
	unsigned char fbuffer[4096];
	unsigned char c;

	KifFile(KifCode* klc, KifElement* kp) : fileread(NULL), KifBaseFile(klc, kp, kifFile) {}


	virtual KifElement* EndOfFile() {
		if (!mode || mode == 2)
			return kifTRUE;
		if (fileread != NULL) {
			if (feof(fileread))
				return kifTRUE;
			return kifFALSE;
		}
		if (thefile == NULL)
			return kifTRUE;
		if (thefile->eof())
			return kifTRUE;
		return kifFALSE;
	}

	void unget() {
		thefile->unget();
	}

	void Unget(int);

	void clear() {
		thefile->clear();
	}

	long get();
	void raz() {
		fileread = NULL;
		thefile = NULL;
		first = true;
	}

	virtual void initwhile(KifElement* var) {
		local = var->Returnvalue();
		localtype = local->type;

		delete thefile;
		thefile = NULL;
		first = true;
#ifdef WIN32
		fopen_s(&fileread, (char*)filename.c_str(), "rb");
#else
		fileread = fopen((char*)filename.c_str(), "rb");
#endif

		if (var->inType() == kifVector)
			local = localstring;
	}


	void Close() {
		ThreadLock _lock(kifFile);
		if (thefile != NULL)
			delete thefile;
		if (fileread != NULL)
			fclose(fileread);
		raz();
		mode = 0;
	}

	bool Write(string n);
	bool WriteBin(int c);
	long Get();
	BULONG getc(bool utf8);
	KifElement* Read(int nb);
	KifElement* Readoneline();
	KifElement* Find(KifElement*, agnostring& s, bool u);


	~KifFile() {
		Close();
		delete localstring;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return new KifFile(kifcode, NULL);
	}

	KifElement* Seek(long i);
	KifElement* Tell();
	long tell();

	virtual Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);
	virtual Exported KifElement* whilein(KifElement* var);
};

union w_u_char {
	wchar_t c;
	unsigned char cc[sizeof(wchar_t)];
};

class KifFileWide : public KifBaseFile {
public:

	w_u_char c;
	wstring w;

	KifFileWide(KifCode* klc, KifElement* kp) : KifBaseFile(klc, kp, kifFileWide) {}

	KifElement* EndOfFile() {
		if (!mode || mode == 2)
			return kifTRUE;
		if (thefile == NULL)
			return kifTRUE;
		if (thefile->eof())
			return kifTRUE;
		return kifFALSE;
	}

	void initwhile(KifElement* var) {
		local = var->Returnvalue();
		localtype = local->type;
		if (var->inType() == kifVector)
			local = localstring;
	}


	void Close() {
		ThreadLock _lock(kifFileWide);
		if (thefile != NULL)
			delete thefile;
		raz();
		mode = 0;
	}

	void unget() {
		thefile->unget();
	}

	void clear() {
		thefile->clear();
	}

	bool Write(string n);
	bool WriteBin(int c);
	long Get();
	KifElement* Read(int nb);
	KifElement* Find(KifElement*, agnostring& s, bool u);
	KifElement* Readoneline();


	~KifFileWide() {
		Close();
		delete localstring;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return new KifFileWide(kifcode, NULL);
	}

	KifElement* Seek(long i);
	KifElement* Tell();

	virtual Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);
	virtual Exported KifElement* whilein(KifElement* var);
};

class KifStdin : public KifFile {
public:
	istream* is;

	KifStdin(KifCode* klc, KifElement* kp) : KifFile(klc, kp) {
		is = &cin;
		filename = "_in";
		mode = 1;
	}

	KifElement* EndOfFile() {
		if (is->eof())
			return kifTRUE;
		return kifFALSE;
	}

	void initwhile(KifElement* var) {
		local = var->Returnvalue();
		localtype = local->type;

		if (var->inType() == kifVector)
			local = localstring;
	}


	bool Openread(string n) {
		ThreadLock _lock(kifFile);
		if (thefile != NULL)
			return false;
		return true;
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);
	Exported KifElement* whilein(KifElement* var);
};


class KifIndex : public KifElement {
public:
	KifElement* key;
	KifElement* function;
	KifElement* right;
	bool interval;
	bool signkey;
	bool signright;

	KifIndex(KifCode* klc, KifElement* kp, bool interv) : signkey(false), signright(false), interval(interv), key(NULL), function(NULL), right(NULL), KifElement(klc, kp, kifIndex) {}

	char Indexes(int sz, KifDomain*, int& left, int& right);

	Exported KifElement* Declaration(short s);

	Exported void Add(KifElement* ke) {
		if (key == NULL)
			key = ke;
		else
		if (interval == true)
			right = ke;
		else
			function = ke;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = NULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*);

	Exported string String() {
		if (key == NULL)
			return "";
		return key->String();
	}

	Exported long Integer() {
		if (key == NULL)
			return 0;
		return key->Integer();
	}

	Exported double Float() {
		if (key == NULL)
			return 0;
		return key->Float();
	}

	Exported bool Boolean() {
		if (key == NULL)
			return false;
		return key->Boolean();
	}

};


class KifVariable : public KifCommonVariable {
public:
	short name;
	bool privatevariable;
	bool localvariable;

	KifVariable(KifCode* klc, KifElement* kp, KifElement* v, short n) : name(n), privatevariable(false), localvariable(false), KifCommonVariable(klc, kp, v, kifVariable) {}

	//In this case, we try to avoid a bad accident, by destroying a variable that should not be destroyed this way at the end of an affectation for instance...
	void ResetOnNull() {}

	void Setprivate(bool v) {
		privatevariable = v;
	}

	void Setlocalvariable() {
		localvariable = true;
	}

	bool Localvariable() {
		return localvariable;
	}

	Exported bool Private() {
		return privatevariable;
	}

	Exported short Name() {
		return name;
	}
	bool Variable() {
		return true;
	}

	Exported KifType inType() {
		return value->inType();
	}


	Exported KifElement* Atom(bool forced = false) {
		return value->Atom(forced);
	}

	virtual Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifVariable* var = new KifVariable(kifcode, (KifElement*)kp, value->Copy(kp), name);
		if (value != var->value)
			var->value->Setreference();
		return var;
	}
	virtual Exported void callfunction(KifElement* a, KifElement* b, KifDomain* dom) {}
	Exported bool Setvalue(KifElement* k, KifElement* index, KifElement* dom,int idthread=0,KifCallFunction* callfunc=NULL);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = NULL) {
		return value;
	}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return value;
	}
	KifElement* Returnvalue() {
		return value;
	}



	Exported void Clean() {
		KifElement::Resetreference();
		//value->Clean();
	}

	Exported void Clear() {
		//value->Clear();
	}

	Exported void Setreference(int inc = 1) {
		reference += inc;
		popped = 0;
		value->Setreference(inc);
	}

	Exported void Setpopped(int v = 1, bool force = false) {
		if (reference <= 0 || force) {
			Putpopped(v);
			value->Setpopped(v, force);
		}
	}

	Exported void Resetreference(int inc = 1) {
		if (popped)
			value->Putpopped(1);
		value->Resetreference(inc);
		KifElement::Resetreference(inc);
	}

	Exported string BaseString() {
		return value->BaseString();
	}


	Exported string String() {
		return value->String();
	}
	Exported long Integer() {
		return value->Integer();
	}
	Exported double Float() {
		return value->Float();
	}
	Exported bool Boolean() {
		return value->Boolean();
	}



	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return value->plus(value, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return value->minus(value, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return value->multiply(value, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return value->divide(value, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return value->mod(value, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return value->shiftright(value, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return value->shiftleft(value, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return value->power(value, b, context, idthread,callfunc,autoself);
	}

	Exported KifElement* less(KifElement* a) {
		return value->less(a);
	}

	Exported KifElement* more(KifElement* a) {
		return value->more(a);
	}

	Exported KifElement* same(KifElement* a) {
		return value->same(a);
	}

	Exported KifElement* different(KifElement* a) {
		return value->different(a);
	}

	Exported KifElement* lessequal(KifElement* a) {
		return value->lessequal(a);
	}

	Exported KifElement* moreequal(KifElement* a) {
		return value->moreequal(a);
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		if (kfunc == NULL)
			return value->in(a, b, NULL, this, idx);
		return applyfunc(a, b, kfunc);
	}
};

class KifVariableFunction : public KifVariable {
public:
	KifElement* function;
	KifVariableFunction(KifCode* klc, KifElement* kp, KifElement* v, KifElement* func, short n) : function(func), KifVariable(klc, kp, v, n) {}

	bool Function() {
		if (function == NULL)
			return false;
		return true;
	}

	void SetSynchronousLock() {
		if (function != NULL)
			function->SetSynchronousLock();
	}

	void ResetSynchronousLock() {
		if (function != NULL)
			function->ResetSynchronousLock();
	}

	virtual Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifVariableFunction* var = new KifVariableFunction(kifcode, (KifElement*)kp, value->Copy(kp), function, name);
		if (value != var->value)
			var->value->Setreference();
		return var;
	}
	KifElement* Returnfunction() {
		return function;
	}
	KifElement* Robfunction() {
		KifElement* func = function;
		function = NULL;
		return func;
	}
	Exported void callfunction(KifElement* a, KifElement* b, KifDomain* dom) {
		applyfuncvariable(this, a, b, (KifDomain*)function->Functioncall(dom));
	}
};

class KifVariableFrame : public KifVariable {
public:
	KifDomain* frame;

	KifVariableFrame(KifCode* klc, KifElement* kp, KifElement* v, short n) : frame(NULL), KifVariable(klc, kp, v, n) {}
	void  Setframe(KifElement* fr) {
		frame = (KifDomain*)fr;
	}

	KifElement* Frame() {
		return (KifElement*)frame;
	}
};

class KifVariableFrameFunction : public KifVariableFrame {
public:
	KifElement* function;

	KifVariableFrameFunction(KifCode* klc, KifElement* kp, KifElement* v, KifElement* func, short n) : function(func), KifVariableFrame(klc, kp, v, n) {}

	bool Function() {
		if (function == NULL)
			return false;
		return true;
	}

	virtual Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifVariableFrameFunction* var = new KifVariableFrameFunction(kifcode, (KifElement*)kp, value->Copy(kp), function, name);
		if (value != var->value)
			var->value->Setreference();
		return var;
	}
	KifElement* Returnfunction() {
		return function;
	}
	KifElement* Robfunction() {
		KifElement* func = function;
		function = NULL;
		return func;
	}

	Exported void callfunction(KifElement* a, KifElement* b, KifDomain* dom) {
		applyfuncinframe(this, (KifElement*)frame, a, b, (KifDomain*)function->Functioncall(dom));
	}

};

class KifCouple : public KifElement {
public:
	static vector<KifCouple*> localgarbage;
	static int ilocalgarbage;
	KifIndex* indexes;
	KifElement* recipient;
	KifElement* lastrecipient;
	KifIndex* lastindexes;
	KifDomain* domain;
	KifElement* function;
	int igarbagerecipient;
	bool keeplastrecipient;

	KifCouple(KifCode* klc, KifElement* rec, KifIndex* i, KifDomain* dom, KifElement* f);
	KifCouple(KifElement* rec, KifIndex* i, KifDomain* dom, KifElement* f);


	static KifCouple* Providecouple(KifElement* rec, KifIndex* i, KifDomain* dom, KifElement* f);
	void Initialization(KifElement* rec, KifIndex* i, KifDomain* dom, KifElement* f) {
		keeplastrecipient = false;
		igarbagerecipient = -1;
		recipient = rec;
		lastrecipient = NULL;
		lastindexes = NULL;
		indexes = i;
		domain = dom;
		function = f;
	}

	bool Kifexist() {
		return kifcode->gGlobals->Garbageexist(igarbagerecipient, recipient);
	}

	void Release();
	void Resetreference(int inc = 1);
	KifElement* EvaluateKey(KifElement* k, KifElement* idx, KifElement* dom);
	Exported bool Setvalue(KifElement* k, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = NULL);
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*);

	Exported string Type();

	Exported void callfunction(KifElement* a, KifElement* b, KifDomain* dom);

	Exported string BaseString() {
		KifElement* ke = Execute(this, domain, kifNULL);
		return ke->BaseString();
	}

	Exported string String() {
		KifElement* ke = Execute(this, domain, kifNULL);
		return ke->String();
	}
	Exported long Integer() {
		KifElement* ke = Execute(this, domain, kifNULL);
		return ke->Integer();
	}
	Exported double Float() {
		KifElement* ke = Execute(this, domain, kifNULL);
		return ke->Float();
	}
	Exported bool Boolean() {
		KifElement* ke = Execute(this, domain, kifNULL);
		return ke->Boolean();
	}

	Exported KifElement* plusplus();

	Exported KifElement* minusminus();

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context,int idthread,KifCallFunction* callfunc, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context,int idthread,KifCallFunction* callfunc, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context,int idthread,KifCallFunction* callfunc, bool autoself);

	Exported KifElement* less(KifElement* b) {
		KifElement* a = Execute(kifNULL, domain, kifNULL);
		if (b->type == kifCouple)
			b = b->Execute(kifNULL, domain, kifNULL);
		return a->less(b);
	}
	Exported KifElement* more(KifElement* b) {
		KifElement* a = Execute(kifNULL, domain, kifNULL);
		if (b->type == kifCouple)
			b = b->Execute(kifNULL, domain, kifNULL);
		return a->more(b);
	}
	Exported KifElement* same(KifElement* b) {
		KifElement* a = Execute(kifNULL, domain, kifNULL);
		if (b->type == kifCouple)
			b = b->Execute(kifNULL, domain, kifNULL);
		return a->same(b);
	}
	Exported KifElement* different(KifElement* b) {
		KifElement* a = Execute(kifNULL, domain, kifNULL);
		if (b->type == kifCouple)
			b = b->Execute(kifNULL, domain, kifNULL);
		return a->different(b);
	}

	Exported KifElement* lessequal(KifElement* b) {
		KifElement* a = Execute(kifNULL, domain, kifNULL);
		if (b->type == kifCouple)
			b = b->Execute(kifNULL, domain, kifNULL);
		return a->lessequal(b);
	}
	Exported KifElement* moreequal(KifElement* b) {
		KifElement* a = Execute(kifNULL, domain, kifNULL);
		if (b->type == kifCouple)
			b = b->Execute(kifNULL, domain, kifNULL);
		return a->moreequal(b);
	}
};

class KifConst : public KifCommonVariable {
public:
	short name; //can be a frame as well
	bool add;
	bool isbreak;

	KifConst(KifCode* klc, KifElement* kp, short n, bool a = false);

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return this;
	}

	void Release() {};

	Exported bool isConst() {
		return true;
	}

	bool Succeed() {
		return false;
	}

	bool unifiedDeclaration(KifDomain* dom) {
		return false;
	}

	bool isBreak() {
		return isbreak;
	}

	Exported void Setreference(int inc = 1) {}
	Exported void Setpopped(int v = 1, bool force = false) {}
	Exported void Clean() {}
	Exported void Resetreference(int inc = 1) {}
	Exported bool Setvalue(KifElement* k, KifElement* index, KifElement* dom,int idthread=0,KifCallFunction* callfunc=NULL) { return false; }
	Exported KifElement* Newinstance() {
		return this;
	}

	string String();
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL) {
		return this;
	}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*) {
		return this;
	}

	Exported string Type();

	Exported void Add(KifElement* ke) {
		if (add) {
			value = ke;
			value->Setreference();
			add = false;
		}
	}
	Exported short Name() {
		return name;
	}

	virtual KifElement* Returnvalue() {
		return value;
	}

	Exported string StringForDisplay();

	KifElement* Linkedvalue() {
		return value;
	}

	bool Variable() {
		return true;
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->plus(a, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->minus(a, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->multiply(a, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->divide(a, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->mod(a, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->shiftright(a, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->shiftleft(a, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->power(a, b, context, idthread,callfunc,autoself);
	}
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->orset(a, b, context, autoself);
	}

	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->xorset(a, b, context, autoself);
	}

	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (a->type <= kifnull)
			return kifNULL;
		return a->andset(a, b, context, autoself);
	}
};

class KifThis : public KifElement {
public:

	KifThis(KifCode* klc, KifElement* kp) : KifElement(klc, kp, kifthis) {}


	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifElement* ke = new KifThis(kifcode, (KifElement*)kp);
		return ke;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*);

};


//A basic variable declaration is of type: KifType
class KifVariableDeclaration : public KifCommonVariable {
public:
	short name; //can be a frame as well
	KifElement* initialisation;
	KifElement* function;
	bool privatevariable;
	unsigned char eval;

	KifVariableDeclaration(KifCode* klc, KifElement* kp, short n, KifType kt = kifVariableDeclaration) : eval(0),
		name(n), initialisation(kifNULL), function(NULL), privatevariable(false), KifCommonVariable(klc, kp, kifNULL, kt) {}

	KifVariableDeclaration(KifCode* klc, KifElement* kp, string n, KifType kt = kifVariableDeclaration);

	void Setprivate(bool v) {
		privatevariable = v;
	}
	Exported bool Private() {
		return privatevariable;
	}

	virtual Exported bool Setvalue(KifElement* k, KifElement* index, KifElement* dom,int idthread=0,KifCallFunction* callfunc=NULL);
	Exported string Type();
	void Setinitialisation(KifElement* ke) {
		if (ke != NULL)
			initialisation = ke;
	}

	void Seteval(int v) {
		eval = v;
	};

	virtual KifElement* Returnfunction() {
		return function;
	}

	KifElement* Robfunction(KifElement*);

	void Setfunction(KifElement* f) {
		function = f;
	}
	virtual Exported void Add(KifElement* ke) {
		value = ke;
		value->Setreference();
	}
	Exported short Name() {
		return name;
	}

	virtual KifElement* Returnvalue() {
		return value;
	}

	KifElement* Linkedvalue() {
		return value;
	}

	bool Variable() {
		return true;
	}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int, KifCallFunction*);
	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);
	virtual Exported KifElement* Create(KifDomain* kp, KifElement* dom, int, KifCallFunction*);
	KifElement* Initialisation() {
		return initialisation;
	}
};

class KifStaticVariable : public KifVariableDeclaration {
public:
	bool init;
	KifStaticVariable(KifCode* klc, KifElement* kp, short n) : KifVariableDeclaration(klc, kp, n) {
		init = false;
		type = kifStaticVariable;
	}
	Exported KifElement* Create(KifDomain* kp, KifElement* dom, int, KifCallFunction*);
	Exported bool Setvalue(KifElement* k, KifElement* index, KifElement* dom,int idthread=0,KifCallFunction* callfunc=NULL);
	Exported void Add(KifElement* ke) {
		init = true;
		value = ke;
		value->Setreference();
	}

};

class KifFrameDeclaration : public KifVariableDeclaration {
public:
	KifElement* frame;
	bool postpone;
	KifFrameDeclaration(KifCode* klc, KifElement* kp, short n, KifElement* d) : frame(d), postpone(false), KifVariableDeclaration(klc, kp, n, kifFrameDeclaration) {}

	void Setpostpone(bool v) {
		postpone = true;
	}

	bool Postpone() {
		return postpone;
	}

	KifElement* Domain() {
		return frame;
	}
	KifElement* Frame() {
		return frame;
	}
	Exported bool Setvalue(KifElement* k, KifElement* index, KifElement* dom,int idthread=0,KifCallFunction* callfunc=NULL) { return false; }
	KifElement* Returnvalue() { return kifNULL; }
	virtual Exported KifElement* Create(KifDomain* kp, KifElement* dom, int, KifCallFunction*);
};


class KifFunctionDeclaration : public KifVariableDeclaration {
public:
	KifElement* domain;
	KifFunctionDeclaration(KifCode* klc, KifElement* kp, short n, KifElement* d) : domain(d), KifVariableDeclaration(klc, kp, n, kifFunctionDeclaration) {}
	KifElement* Domain() {
		return domain;
	}
	KifElement* Frame() {
		return domain;
	}
};

//====================================================================================
class KifCall : public KifElement {
public:
	KifFunction* body;
	KifDomain* domain;


	KifCall(KifCode* klc, KifElement* kp, KifFunction* b) : body(b), domain(NULL), KifElement(klc, kp, kifCall) {}

	KifFunction* Functioncall(KifDomain* dom) {
		return body;
	}

	Exported KifElement* Atom(bool forced = false) {
		if (body == NULL)
			return kifNULL;
		return (KifElement*)body;
	}

	Exported string String();
	Exported string BaseString();

	bool isCall() {
		return true;
	}

	Exported bool Setvalue(KifElement* val, KifElement* idx, KifElement* dom,int idthread=0,KifCallFunction* callfunc=NULL);

	bool Variable() {
		return true;
	}

	bool Basic() {
		return true;
	}

	Exported KifElement* Newinstance() {
		return new KifCall(kifcode, NULL, NULL);
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction*);

	Exported KifElement* same(KifElement* a) {

		if (a == kifNULL) {
			if (body == NULL)
				return kifTRUE;
			return kifNULL;
		}

		if (a->type == kifCall) {
			if (body == ((KifCall*)a)->body)
				return kifTRUE;
			return kifFALSE;
		}
		if (a == (KifElement*)body)
			return kifTRUE;
		return kifFALSE;
	}
};
//=======================================================================================
class KifInstanceCommon : public KifPosition {
public:
	short name;
	KifCommonVariable* declaration;
	string keyvalue;
	KifOrigin newinstance;
	KifElement* current_value;

	KifInstanceCommon(KifCode* klc, KifElement* kp, short n, KifCommonVariable* d) :
		name(n), declaration(d), keyvalue(""), newinstance(kinUnknown), current_value(NULL), KifPosition(klc, kp, kifInstance) {}

	string String() {
		return keyvalue;
	}

	void Setinstancetype(KifOrigin v) {
		newinstance = v;
	}

	KifOrigin Instancetype() {
		return newinstance;
	}

	KifElement* Initialisation() {
		return declaration->Initialisation();
	}

	void Setinitialisation(KifElement* ke) {
		declaration->Setinitialisation(ke);
	}

	void Seteval(int v) {
		declaration->Seteval(v);
	}

	virtual void Initvalue(KifElement* ke) {}
	Exported short Name() { return name; }

	Exported KifElement* Declaration(short name) {
		return declaration->value->Declaration(name);
	}

	Exported KifElement* Declarationfunction(short name);

	Exported KifElement*  Linkedvalue() {
		return declaration->value;
	}

	string Keyvalue() {
		return keyvalue;
	}

	void Setstringvalue(string& k) {
		keyvalue = k;
	}
	Exported virtual string Type() {
		return "";
	}
};

//This class is used when a variable is in an expression
class KifInstance : public KifInstanceCommon {
public:
	//The domain is its type either in a frame or in a function
	KifElement* value;
	KifIndex* index;
	bool self;

	KifInstance(KifCode* klc, KifElement* kp, short n, KifCommonVariable* d) : value(NULL), index(NULL), self(false), KifInstanceCommon(klc, kp, n, d) {
		if (d != NULL)
			value = d->VariableValue();
	}

	KifInstance(KifCode* klc, KifElement* kp, string n, KifCommonVariable* d);

	bool isInstance() {
		return true;
	}

	virtual KifElement* Thevariable(KifDomain* dom, int idthread, KifCallFunction* callfunc);
	inline KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL) {
		int idthread = kifGlobals->GetThreadid();
		return Exec(contextualpattern, dom, val, idthread, (KifCallFunction*)kifGlobals->Topid(idthread));
	}
	virtual Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	KifFunction* Functioncall(KifDomain* dom) {
		KifElement* call = Execute(kifNULL, dom);
		return ((KifCall*)call)->body;
	}

	Exported string Type() {
		if (value != NULL)
			return value->Type();
		return "";
	}

	Exported KifType inType();

	void Initvalue(KifElement* ke) {
		if (ke->type == kifIndex)
			index = (KifIndex*)ke;
		else
			value = ke;
	}

	Exported KifElement* Linkedvalue() {
		if (value->type == kifInstance)
			return value->Linkedvalue();
		return value;
	}

	void Setself() {
		self = true;
	}
};

class KifInstanceInFunction : public KifInstance {
public:
	KifInstanceInFunction(KifCode* klc, KifElement* kp, short n, KifCommonVariable* d) : KifInstance(klc, kp, n, d) {}
	KifElement* Thevariable(KifDomain* dom, int idthread, KifCallFunction* callfunc);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

};

class KifInstanceInFrame : public KifInstance {
public:
	KifInstanceInFrame(KifCode* klc, KifElement* kp, short n, KifCommonVariable* d) : KifInstance(klc, kp, n, d) {}
	KifElement* Thevariable(KifDomain* dom, int idthread, KifCallFunction* callfunc);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

};

class KifInstanceGlobal : public KifInstance {
public:
	KifInstanceGlobal(KifCode* klc, KifElement* kp, short n, KifCommonVariable* d) : KifInstance(klc, kp, n, d) {}
	KifElement* Thevariable(KifDomain* dom, int idthread, KifCallFunction* callfunc);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

};

class KifInstanceConstant : public KifInstance {
public:
	KifInstanceConstant(KifCode* klc, KifElement* kp, short n, KifCommonVariable* d) : KifInstance(klc, kp, n, d) {}
	KifElement* Thevariable(KifDomain* dom, int idthread, KifCallFunction* callfunc);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

};


class KifInstanceFunction : public KifInstanceCommon {
public:
	KifCallFunctionGeneral* function;

	KifInstanceFunction(KifCode* klc, KifElement* kp, short n, KifCommonVariable* d) : function(NULL), KifInstanceCommon(klc, kp, n, d) {
		type = kifInstanceFunction;
	}

	inline KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL) {
		return Exec(contextualpattern, dom, val, kifGlobals->GetThreadid(), NULL);
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

	bool isInstance() {
		return true;
	}

	void Initvalue(KifElement* ke) {
		function = (KifCallFunctionGeneral*)ke;
	}

	Exported string Type() {
		return "kifCallFunction";
	}
	Exported KifType inType();

	KifElement*  Frame() {
		return declaration->value;
	}
};



class KifObject : public KifPosition {
public:

	bool adding;

	KifObject(KifCode* klc, KifElement* kp, KifType k = kifAny) : adding(true), KifPosition(klc, kp, k) {
		type = k;
	}

	void Disableadding() {
		adding = false;
	}
};


class KifObjectInstruction : public KifObject {
public:
	vector<KifElement*> instructions;
	KifInstructionType instructiontype;
	bool negation;

	KifObjectInstruction(KifCode* klc, KifElement* kp, KifType ktype = kifAny) : instructiontype(kifNOTYPE), negation(false), KifObject(klc, kp, ktype) {}

	void Setnegation(bool n) {
		negation = n;
	}

	bool Negation() {
		return negation;
	}

	void SetInstruction(int i, KifElement* ke) {
		instructions[i] = ke;
	}

	int InstructionSize() {
		return instructions.size();
	}

	KifInstructionType Instructiontype() {
		return instructiontype;
	}

	KifElement* Lastinstruction() {
		if (instructions.size() == 0)
			return kifNULL;
		return instructions[instructions.size() - 1];
	}

	KifElement* Instruction(int i) {
		return instructions[i];
	}

	KifElement* Frominstruction(int i) {
		return instructions[i];
	}

	void Addinstruction(int i, KifElement* ke);
	void InstructionClear() {
		instructions.clear();
	}


	void InstructionRemove(int nb) {
		if (nb>instructions.size())
			nb = instructions.size();
		while (nb>0) {
			instructions.pop_back();
			nb--;
		}
	}

	KifElement* InstructionRemoveLast() {
		if (instructions.size() == 0)
			return kifNULL;
		KifElement* ret = instructions[instructions.size() - 1];
		instructions.pop_back();
		return ret;
	}

	Exported virtual void Add(KifElement* k);
	virtual KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
};


class KifInstructionTRY : public KifObjectInstruction {
public:
	KifInstructionTRY(KifCode* klc, KifElement* kp) : KifObjectInstruction(klc, NULL, kifInstruction) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionTRY;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	short Name() {
		return idtry;
	}

};

class KifInstructionSWITCH : public KifObjectInstruction {
public:
	hmap<string, int> keys;
	bool usekeys;

	KifElement* function;
	KifInstructionSWITCH(KifCode* klc, KifElement* kp) : function(NULL), KifObjectInstruction(klc, NULL, kifInstruction) {
		usekeys = false;
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionSWITCH;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

	Exported void Setfunction(KifElement* f) {
		function = f;
	}
	short Name() {
		return idswitch;
	}

	Exported void Add(KifElement* k);
};

class KifInstructionIF : public KifObjectInstruction {
public:
	KifInstructionIF(KifCode* klc, KifElement* kp) : KifObjectInstruction(klc, NULL, kifInstruction) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionIF;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	short Name() {
		return idif;
	}

};

class KifInstruction : public KifObjectInstruction {
public:
	KifElement* parent;
	KifOperator action;
	bool negation;

	KifInstruction(KifCode* klc, KifElement* kp, KifOperator kop = kifNONE);


	Exported void Set(short s, KifElement* k, hmap<short, KifElement*>* stacks);
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

	bool Negation() {
		return negation;
	}

	bool isInstruction() {
		return true;
	}

	void Clone(KifInstruction* ki) {
		ki->instructions = instructions;
		ki->negation = negation;
		ki->parent = parent;
	}

	inline void Setaction(KifOperator a) {
		action = a;
	}

	inline KifOperator Action() {
		return action;
	}

	inline KifElement* Parent() {
		return parent;
	}

	void Setnegation(bool v) {
		negation = v;
	}

	void Addparent(KifElement* p);
	string String();
};


class KifInstructionINITIALISATION : public KifInstruction {
public:

	KifInstructionINITIALISATION(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionINITIALISATION;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionAFFECTATION : public KifInstruction {
public:

	KifInstructionAFFECTATION(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionAFFECTATION;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionAPPLYOPERATION : public KifInstruction {
public:

	KifInstructionAPPLYOPERATION(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionAPPLYOPERATION;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionAPPLYOPERATIONEQU : public KifInstruction {
public:

	KifInstructionAPPLYOPERATIONEQU(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionAPPLYOPERATIONEQU;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionCOMPARE : public KifInstruction {
public:

	KifInstructionCOMPARE(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionCOMPARE;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionPLUSPLUS : public KifInstruction {
public:

	KifInstructionPLUSPLUS(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionPLUSPLUS;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionMINUSMINUS : public KifInstruction {
public:

	KifInstructionMINUSMINUS(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionMINUSMINUS;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionOR : public KifInstruction {
public:

	KifInstructionOR(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionOR;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionAND : public KifInstruction {
public:

	KifInstructionAND(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionAND;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};


class KifInstructionMERGE : public KifInstruction {
public:

	KifInstructionMERGE(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionMERGE;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionMERGEEQU : public KifInstruction {
public:

	KifInstructionMERGEEQU(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionMERGEEQU;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionADDTOLIST : public KifInstruction {
public:

	KifInstructionADDTOLIST(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionADDTOLIST;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionADDTOLISTEQU : public KifInstruction {
public:

	KifInstructionADDTOLISTEQU(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionADDTOLISTEQU;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionKIFIN : public KifInstruction {
public:

	KifInstructionKIFIN(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionKIFIN;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionKIFON : public KifInstruction {
public:

	KifInstructionKIFON(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionKIFON;
	}
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};


//--------------------------------------------------------------------------------------------------
class KifInstructionPARAMETER : public KifInstruction {
public:
	KifInstructionPARAMETER(KifCode* klc, KifElement* kp) : KifInstruction(klc, NULL) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionPARAMETER;
	}

	bool unifiedDeclaration(KifDomain* dom);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	Exported KifElement* ExecuteRaw(KifElement* contextualpattern, KifDomain* dom);
	Exported KifElement* ExecRaw(KifElement* contextualpattern, KifDomain* dom, int idthread, KifCallFunction* callfunc);

	Exported KifElement* ExtractPredicateVariables(KifElement* contextualpattern, KifDomain* dom, KifElement* val, KifElement* e, bool root);
	Exported KifElement* EvaluePredicateVariables(KifElement* context, KifDomain* dom);
	string String() {
		return instructions[0]->String();
	}
};

class KifDomain : public KifObject {
public:

	KifThreadLock _locker;
	KifElement* frame;

	hmap<short, KifElement*> declarations;
	short name;
	short delayed;
	bool postpone;
	bool failed;

	KifDomain(KifCode* klc, KifElement* kp, short n, KifType t) : _locker(NULL, false), failed(false), name(n), frame(kifNULL), delayed(0), postpone(false), KifObject(klc, kp, t) {}

	void Setfail(bool v) {
		failed = v;
	}

	bool Failed() {
		return failed;
	}

	KifPredicateContainer* Predicatecontainer();

	void Lockframe(bool exclusive);

	void Unlockframe(bool exclusive);

	virtual bool Basic() {
		return true;
	}

	void Setpostpone(bool v) {
		postpone = v;
	}

	bool Postpone() {
		return postpone;
	}

	short Delayed() {
		return delayed;
	}

	virtual KifFunction* Body() {
		return NULL;
	}

	KifElement* ApplySelfInsertFunction(KifCallFunctionGeneral* f, KifElement* contextualpattern, KifDomain* dom, int, KifCallFunction*);

	void Setdelayed() {
		delayed++;
	}

	void Resetdelayed() {
		delayed--;
	}

	void ApplyDelayedInitial(int idthread, KifCallFunction*);

	void  Setframe(KifElement* fr) {
		frame = fr;
	}

	virtual KifFrame* ParentFrame() {
		return NULL;
	}

	KifElement* ExecuteMethod(short name, KifElement* contextualpattern, KifDomain* domain, KifCallFunction* func) {
		if (frame != NULL)
			return frame->ExecuteMethod(name, contextualpattern, this, func);
		return kifNULL;
	}

	Exported KifElement* Newinstance() {
		KifDomain* ke = new KifDomain(kifcode, NULL, name, type);
		ke->frame = frame;
		return ke;
	}

	virtual Exported KifElement* Declaration(short s);
	Exported KifElement* Declared(short s);

	Exported void Methods(KifVectorString* fcts);
	Exported void Setdeclaration(string n, KifElement* ke, hmap<short, KifElement*>* stacks);
	KifElement* applycomparatorfunction(KifElement* a, KifElement* b, KifFunction* func);

	Exported void Setdeclaration(short n, KifElement* ke, hmap<short, KifElement*>* stacks);
	Exported void Resetdeclaration(hmap<short, KifElement*>& stacks);
	virtual void Cleantracks(int nm) {}
	Exported void SetInstances(KifElement* dom, hmap<short, KifElement*>* stacks);


	Exported short Name() { return name; }

	virtual Exported string Type() {
		return frame->Type();
	}

	bool isDomain() { return true; }

	virtual Exported KifElement* Copy(KifDomain* dom, KifElement* decl = kifNULL);

	Exported void GetInstances(KifDomain* dom);
	Exported void SetInstanceslocal(KifElement* dom, hmap<short, KifElement*>* stacks);
	virtual Exported void Clean();
	virtual Exported void Clear();
	virtual Exported void Setreference(int inc = 1);
	virtual Exported void Setpopped(int v = 1, bool force = false);
	virtual Exported void Resetreference(int inc = 1);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);

	virtual Exported bool Setvalue(KifElement* ke, KifElement* idx = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);

	KifElement*  Frame() {
		return frame;
	}
	Exported KifElement* Framedefined(short met, KifElement* contextualpattern, KifDomain* dom);
	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);
	Exported KifElement* applycomparison(short, KifElement* a);
	KifElement* applyincrement(short na);
	Exported KifElement* applyoperation(short, KifElement* a, KifElement* b);
	Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);
	KifElement* destructor();
	Exported bool initialconstructor() { return true; }

	virtual Exported long Size() {
		KifElement* ke = Framedefined(idsize, kifTypeINTEGER, this);
		return ke->Integer();
	}

	Exported string BaseString();

	Exported string String() {
		KifElement* ke = Framedefined(idstring, kifTypeSTRING, this);
		return ke->String();
	}

	Exported KIFLONG Long() {
		KifElement* ke = Framedefined(idlong, kifTypeLONG, this);
		return ke->Long();
	}

	Exported long Integer() {
		KifElement* ke = Framedefined(idint, kifTypeINTEGER, this);
		return ke->Integer();
	}
	Exported double Float() {
		KifElement* ke = Framedefined(idfloat, kifTypeFLOAT, this);
		return ke->Float();
	}
	Exported bool Boolean() {
		KifElement* ke = Framedefined(idboolean, kifTypeBOOLEAN, this);
		return ke->Boolean();
	}

	Exported KifFraction* Fraction() {
		KifElement* ke = Framedefined(idfraction, kifTypeFRACTION, this);
		return ke->Fraction();
	}

	Exported KifElement* less(KifElement* a) {
		return applycomparison(idinferior, a);
	}
	Exported KifElement* more(KifElement* a) {
		return applycomparison(idsuperior, a);
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* different(KifElement* a);

	Exported KifElement* lessequal(KifElement* a) {
		return applycomparison(idinferiorequal, a);
	}
	Exported KifElement* moreequal(KifElement* a) {
		return applycomparison(idsuperiorequal, a);
	}

	KifElement* plusplus() {
		return applyincrement(idplusplus);
	}

	KifElement* minusminus() {
		return applyincrement(idminusminus);
	}

	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return applyoperation(idorset, a, b);
	}

	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return applyoperation(idxorset, a, b);
	}

	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		return applyoperation(idandset, a, b);
	}

	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return applyoperation(idplus, a, b);
	}

	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return applyoperation(idminus, a, b);
	}

	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return applyoperation(idmultiply, a, b);
	}

	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return applyoperation(iddivide, a, b);
	}

	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return applyoperation(idmod, a, b);
	}
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return applyoperation(idshiftright, a, b);
	}
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return applyoperation(idshiftleft, a, b);
	}
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, int idthread,KifCallFunction * callfunc,bool autoself) {
		return applyoperation(idpower, a, b);
	}

	virtual Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);

	virtual Exported void Set(short s, KifElement* k, hmap<short, KifElement*>* stacks);

	virtual bool Embedded() {
		return false;
	}

};


class KifFrameDeclarationCommon : public KifFrameDeclaration {
public:
	KifElement* variable;

	KifFrameDeclarationCommon(KifCode* klc, KifElement* kp, short n, KifElement* d) : variable(NULL), KifFrameDeclaration(klc, kp, n, d) {}

	Exported KifElement* Create(KifDomain* kp, KifElement* dom, int idthread, KifCallFunction* callfunc) {
		if (variable == NULL)
			variable = KifFrameDeclaration::Create(kp, dom, idthread, callfunc);
		else
			kp->Set(name, variable, NULL);

		variable->Setreference();
		return variable;
	}
};


class KifDomainInstruction : public KifDomain {
public:
	vector<KifElement*> instructions;

	KifDomainInstruction(KifCode* klc, KifElement* kp, short n, KifType t) : KifDomain(klc, kp, n, t) {}

	Exported virtual void Add(KifElement* ke) {
		if (adding)
			instructions.push_back(ke);
	}

	void SetInstruction(int i, KifElement* ke) {
		instructions[i] = ke;
	}

	int InstructionSize() {
		return instructions.size();
	}

	KifElement* Lastinstruction() {
		if (instructions.size() == 0)
			return kifNULL;
		return instructions[instructions.size() - 1];
	}

	void InstructionRemove(int nb) {
		if (nb>instructions.size())
			nb = instructions.size();
		while (nb>0) {
			instructions.pop_back();
			nb--;
		}
	}

	KifElement* InstructionRemoveLast() {
		if (instructions.size() == 0)
			return kifNULL;
		KifElement* ret = instructions[instructions.size() - 1];
		instructions.pop_back();
		return ret;
	}

	void InstructionClear() {
		instructions.clear();
	}

	KifElement* Instruction(int i) {
		return instructions[i];
	}

	KifElement* Frominstruction(int i) {
		return instructions[i];
	}

	void Addinstruction(int i, KifElement* ke) {
		if (i<instructions.size())
			instructions[i] = ke;
		else
			instructions.push_back(ke);
	}
};

class KifInstructionSEQUENCE : public KifDomainInstruction {
public:
	KifInstructionSEQUENCE(KifCode* klc, KifElement* kp, short n) : KifDomainInstruction(klc, kp, n, kifInstructionSEQUENCE) {}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifInstructionCATCH : public KifInstructionSEQUENCE {
public:
	KifInstructionCATCH(KifCode* klc, KifElement* kp) : KifInstructionSEQUENCE(klc, kp, idcatch) {}
	KifOperator Action() {
		return kifCATCHBLOC;
	}

};


//Next is the basic Kif instruction: KifFunction
class KifFunction : public KifDomainInstruction {
public:
	vector<KifElement*> arguments;
	KifFunction* next;
	KifFunction* top;
	char choice;
	bool autorun;
	bool privatefunction;
	bool strict;
	bool used;
	bool nonlimited;

	KifFunction(KifCode* klc, KifElement* kp, short n, KifType ktype = kifFunction) :
		next(NULL), choice(0), autorun(false), privatefunction(false), strict(false), used(false), nonlimited(false), KifDomainInstruction(klc, kp, n, ktype) {
		line = 0;
		top = this;
		declarations[name] = this;
		if (kp != NULL && kp->type == kifFrame)
			frame = kp;
	}

	Exported bool Unlimited() {
		return nonlimited;
	}

	Exported void Setlimited(bool l);

	Exported string String();
	Exported string Type();
	bool Autorun() {
		return autorun;
	}

	KifFunction* Functioncall(KifDomain*) {
		return this;
	}

	Exported bool Private() {
		return privatefunction;
	}
	bool Basic() {
		return false;
	}

	bool Alreadydeclared() {
		if (choice == 1)
			return true;
		return false;
	}

	bool Predeclared() {
		if (choice == 2)
			return true;
		return false;
	}

	void Setchoice(char v) {
		choice = v;
	}

	char Choice() {
		return choice;
	}

	virtual bool Joined() {
		return false;
	}

	bool CheckFunctionFrame(KifFrame* vframe);

	bool isFunction() {
		return true;
	}

	void Setused() {
		used = true;
	}

	bool isUsed() {
		return used;
	}

	void Addfunction(KifFunction* kfunc) {
		KifFunction* n = this;
		while (n->next != NULL)
			n = n->next;
		n->next = kfunc;
		kfunc->top = this;
	}

	Exported KifElement* Copy(KifDomain* dom, KifElement* decl = kifNULL) {
		return this;
	}

	Exported long Size() {
		return arguments.size();
	}

	Exported KifElement* Declaration(short s) {
		hmap<short, KifElement*>::iterator iter = declarations.find(s);
		if (iter == declarations.end())
			return NULL;
		return iter->second;
	}

	Exported void Add(KifElement* ke);

	Exported bool Setvalue(KifElement* ke, KifElement* idx = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL) {
		return true;
	}
	virtual Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	virtual Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

	Exported void Clean() {}
	Exported void Setreference(int inc = 1) {}
	Exported void Setpopped(int v = 1, bool force = false) {}
	Exported void Resetreference(int inc = 1) {}
	virtual void StoreResult(string& s, KifElement* res) {}
	virtual KifElement* ReturnResult(string& s) { return NULL; }
	KifElement* Argument(int i) {
		if (i >= 0 && i<arguments.size())
			return arguments[i];
		return NULL;
	}
};

class KifFunctionSequence : public KifFunction {
public:
	KifFunctionSequence(KifCode* klc, int n) : KifFunction(NULL, NULL, n, kifFunction) {
		kifcode = klc;
	}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
};

class KifFunctionSynchronous : public KifFunction {
public:
	bool unlock;
	KifThreadLock _locker;

	KifFunctionSynchronous(KifCode* klc, KifElement* kp, short n) : KifFunction(klc, kp, n, kifFunction), _locker(NULL, false) {
		unlock = false;
	}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	bool IsSynchronous() {
		return true;
	}

	void SetSynchronousLock() {
		unlock = false;
		if (kifGlobals->Test()) {
			unlock = true;
			_locker.Locking();
		}
	}

	void ResetSynchronousLock() {
		if (unlock)
			_locker.Unlocking();
	}
};

class KifPolynomial : public KifFunction {
public:
	hmap<string, KifElement*> results;
	KifPolynomial(KifCode* klc, KifElement* kp, short n) : KifFunction(klc, kp, n, kifPolynomial) {}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int, KifCallFunction*);
	void StoreResult(string& s, KifElement* res) {
		results[s] = res;
		res->Setreference();
	}

	KifElement* ReturnResult(string& s) {
		hmap<string, KifElement*>::iterator it = results.find(s);
		if (it != results.end())
			return it->second;
		return NULL;
	}
};

class KifReturnValue : public KifElement {
public:
	KifElement* value;

	KifReturnValue() : KifElement(NULL, NULL) {
		value = NULL;
	}

	bool Returned() {
		value->Putpopped(1);
		return true;
	}

	KifElement* Returnvalue() {
		return value;
	}

	void Protect() {
		value->Putpopped(1);
	}

	Exported void Resetreference(int inc = 1) {}
};

class KifReturn : public KifPosition {
public:

	KifReturn(KifCode* klc, KifElement* kp) : KifPosition(klc, kp, kifCallFunctionBCR) {}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL) {
		return kifGlobals->THR[kifGlobals->GetThreadid()]->Setreturnvalue(kifNULL);
	}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*) {
		return kifGlobals->THR[idthread]->Setreturnvalue(kifNULL);
	}
};

class KifCallFunctionBreakOuContinue : public KifPosition {
public:
	KifElement* returnvalue;

	KifCallFunctionBreakOuContinue(KifCode* klc, KifElement* kp, KifElement* r) : returnvalue(r), KifPosition(klc, kp, kifCallFunctionBCR) {}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL) {
		return returnvalue;
	}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*) {
		return returnvalue;
	}
};

class KifCallFunctionBreakpoint : public KifPosition {
public:

	KifCallFunctionBreakpoint(KifCode* klc, KifElement* kp) : KifPosition(klc, kp, kifCallFunctionBCR) {}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
};

class KifCallFunction : public KifDomain {
public:

	vector<KifElement*> parameters;
	KifCallFunction* function;
	char add;


	KifCallFunction(KifCode* klc, KifElement* kp, short n) : function(NULL), add(1), KifDomain(klc, kp, n, kifCallFunction) {
		line = -1;
	}

	virtual int Threadid() {
		return -1;
	}


	KifElement* Parameter(int i) {
		return parameters[i];
	}

	void Setadd(int i) {
		add = i;
	}

	string String() {
		string v = KifIdString(name);
		v += "(";
		for (int i = 0; i<parameters.size(); i++) {
			if (i)
				v += ",";
			v += parameters[i]->String();
		}
		v += ")";
		return v;
	}

	virtual void keep(short n, KifElement* k) {
		declarations[n] = k;
		k->Setreference();
	}

	void Initialise(KifCallFunction* kthread);


	Exported long Size() {
		return parameters.size();
	}

	bool Basic() {
		return false;
	}

	bool isACall() {
		return true;
	}

	virtual Exported KifElement* Declaration(short s);

	virtual KifElement* Evaluate(int param, KifDomain* domain) {
		return kifNULL;
	}

	virtual  KifElement* Evaluatethread(int param, KifDomain* domain, int idthread) {
		return kifNULL;
	}

	virtual KifElement* EvaluateString(int param, KifDomain* domain) {
		return kifNULL;
	}

	virtual void Toclean(KifElement* ke) {}

	virtual void Addtoclean(KifElement* ke) {}

	Exported void Add(KifElement* ke);

	void SetParameters(vector<KifElement*>& vect, hmap<short, KifElement*>* stacks);

	KifElement* Returnvalue() {
		return this;
	}

	Exported short Name() { return name; }
	Exported void Setreference(int inc = 1) {}
	Exported void Setpopped(int v = 1, bool force = false) {}
	Exported void Resetreference(int inc = 1) {};
	Exported void Clean() {}
	Exported KifElement* applyfunction(KifElement* a);
	virtual void Setbody(KifFunction* ke) {}
};

class KifCallReturn : public KifCallFunction {
public:

	KifCallReturn(KifCode* klc, KifElement* kp, short n) : KifCallFunction(klc, kp, n) {}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int, KifCallFunction*);
};


class KifCallFunctionGeneral : public KifCallFunction {
public:
	KifFunction* body;
	bool dynamicselection;
	char _eval;

	KifCallFunctionGeneral(KifCode* klc, KifElement* kp, short n, KifFunction* b) : body(b), KifCallFunction(klc, kp, n) {
		_eval = 0;
		type = kifCallFunctionGeneral;
		dynamicselection = false;
		if (kifcode == NULL && b != NULL && b->next != NULL)
			dynamicselection = true;
	}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	virtual Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	KifFunction* Body() {
		return body;
	}
	void Setbody(KifFunction* ke) {
		body = ke;
	}

	void SetInstruction(int i, KifElement* ke) {
		if (body != NULL)
			body->SetInstruction(i, ke);
	}

	int InstructionSize() {
		if (body == NULL)
			return 0;
		return body->InstructionSize();
	}

	KifElement* Lastinstruction() {
		if (body == NULL)
			return kifNULL;
		return body->Lastinstruction();
	}


	void InstructionRemove(int nb) {
		if (body != NULL)
			body->InstructionRemove(nb);
	}

	KifElement* InstructionRemoveLast() {
		if (body == NULL)
			return kifNULL;
		return body->InstructionRemoveLast();
	}

	KifElement* Instruction(int i) {
		if (body == NULL)
			return kifNULL;
		return body->Instruction(i);
	}

	KifElement* Frominstruction(int i) {
		if (body == NULL)
			return kifNULL;
		return body->Frominstruction(i);
	}

	void InstructionClear() {
		if (body != NULL)
			body->InstructionClear();
	}

	void Addinstruction(int i, KifElement* ke) {
		if (body != NULL)
			body->Addinstruction(i, ke);
	}

};


class KifCallFunctionCall : public KifCallFunctionGeneral {
public:
	KifInstance* kcall;

	KifCallFunctionCall(KifCode* klc, KifElement* kp, short n, KifElement* declaration) : kcall(new KifInstance(klc, NULL, n, (KifCommonVariable*)declaration)), KifCallFunctionGeneral(klc, kp, n, NULL) {
		type = kifCallFunctionCall;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);
};


class KifCallFunctionThread : public KifCallFunctionGeneral {
public:

	KifCallFunctionThread(KifCode* klc, KifElement* kp, short n, KifFunction* b) : KifCallFunctionGeneral(klc, kp, n, b) {
		type = kifCallFunctionThread;
	}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

};

class KifCallExecuteThread : public KifCallFunctionGeneral {
public:
#ifdef C11
	std::thread* thid;
	std::thread::id tid;
#else
#ifdef WIN32
	HANDLE thid;
	DWORD tid;
	//static DWORD WINAPI Call(LPVOID);
	static unsigned __stdcall Call(void*);
#else
	pthread_t tid;
	static void* Call(void*);
#endif
#endif
	int joinpos;
	KifElement* context;
	KifDomain* domain;
	KifElement* value;
	int parentthreadid;
	KifFunction* currentbody;
	KifElement* streamvalue;
	int istreamvalue;
	KifElement* returnvalue;
	int threadid;

	Exported void Launch();
	KifCallExecuteThread(KifCode* klc, KifCallFunctionGeneral* kthread, KifFunction* b, int idthread) : parentthreadid(-1),
		KifCallFunctionGeneral(klc, NULL, kthread->name, b) {
		istreamvalue = -1;
		returnvalue = kifNULL;
		streamvalue = NULL;
		Initialise(kthread);
		type = kifCallExecuteThread;
		threadid = idthread;
		joinpos = -1;
#ifdef C11
		thid = NULL;
#else
#ifdef WIN32
		thid = 0;
		tid = 0;
#else
		tid = NULL;
#endif
#endif

	}

	virtual int Threadid() {
		return threadid;
	}

	void Setreturnvalue(KifElement* k) {
		returnvalue = k;
	}

	bool Kifexist() {
		return kifcode->gGlobals->Garbageexist(istreamvalue, streamvalue);
	}

	Exported KifElement* Arguments(KifCallFunction*);
	Exported void Execution();

	Exported int Parentthreadid() {
		return parentthreadid;
	}
};

class KifCallFunctionLocal : public KifCallFunction {
public:
	KifFunction* body;
	KifCallFunctionLocal(KifCode* klc, short n, KifFunction* b) : KifCallFunction(NULL, NULL, n) {
		body = b;
		kifcode = klc;
	}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
};

class KifCallFunctionWithClean : public KifCallFunction {
public:
	bool clean;
	vector<KifElement*> toclean;
	bool selfinsert;
	KifCallFunction* caller;

	KifCallFunctionWithClean(KifCode* klc, KifElement* kp, short n) : selfinsert(false), KifCallFunction(klc, kp, n) {
		clean = false;
		caller = NULL;
	}

	Exported KifElement* Evaluate(int param, KifDomain* domain) {
		KifElement* ret = parameters[param];
		if (ret->type == kifInstructionPARAMETER)
			ret = ((KifInstructionPARAMETER*)ret)->instructions[0]->Execute(kifNULL, domain);
		else
			ret = ret->Execute(kifNULL, domain);
		if (clean && ret->reference == 0)
			toclean.push_back(ret);
		return ret;
	}

	Exported KifElement* Evaluatethread(int param, KifDomain* domain, int idthread) {
		KifElement* ret = parameters[param];

		if (ret->type == kifInstructionPARAMETER)
			ret = ((KifInstructionPARAMETER*)ret)->instructions[0]->Exec(kifNULL, domain, kifNULL, idthread, caller);
		else
			ret = ret->Exec(kifNULL, domain, kifNULL, idthread, caller);
		if (clean && ret->reference == 0)
			toclean.push_back(ret);
		return ret;
	}

	Exported KifElement* EvaluateString(int param, KifDomain* domain) {
		KifElement* ret = parameters[param];
		if (ret->type == kifInstructionPARAMETER)
			ret = ((KifInstructionPARAMETER*)ret)->instructions[0]->Execute(kifTypeSTRING, domain);
		else
			ret = ret->Execute(kifTypeSTRING, domain);
		if (clean && ret->reference == 0)
			toclean.push_back(ret);
		return ret;
	}

	void Addtoclean(KifElement* ke) {
		if (clean && ke->reference == 0)
			toclean.push_back(ke);
	}

	void Toclean(KifElement* ke) {
		if (clean) {
			for (int i = 0; i<toclean.size(); i++) {
				if (toclean[i] != ke)
					toclean[i]->Release();
			}
			toclean.clear();
		}
	}

	void Setselfinsert(bool v) {
		selfinsert = v;
	}

	bool Selfinsert() {
		return selfinsert;
	}

};


class KifCallFunctionPredefined : public KifCallFunctionWithClean {
public:
	KifPredefined* body;
	KifCallFunctionPredefined(KifCode* klc, KifElement* kp, short n, KifPredefined* b) : body(b), KifCallFunctionWithClean(klc, kp, n) {
		type = kifCallFunctionPredefined;
	}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int, KifCallFunction*);
	Exported KifElement* Declaration(short s);
	KifType inType();
};


class KifCallFunctionExternal : public KifCallFunctionWithClean {
public:
	KifCallMethod* body;
	KifCallFunctionExternal(KifCode* klc, KifElement* kp, short n, KifCallMethod* b);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int, KifCallFunction*);
};


//A User Based Variable: frame
class KifFrame : public KifDomainInstruction {
public:
	bool declared;
	KifFrame* parent;
	KifElement* function;
	vector<short> ordered;
	bool prive;

	Exported KifElement* Copy(KifDomain*, KifElement*);
	Exported KifElement* Create(KifDomain*, KifElement*, int, KifCallFunction*);
	KifFrame(KifCode* klc, KifElement* kp, short n, KifType ktype = kifFrame);

	Exported void Sharedeclaration(KifFrame* k);

	Exported void Setfunction(KifElement* f) {
		function = f;
	}
	KifElement* Returnfunction() {
		return function;
	}

	KifElement* Robfunction(KifElement* kbase);

	KifFrame* ParentFrame() {
		if (parent->parent == NULL)
			return NULL;
		return parent;
	}

	bool Subframe(KifFrame* sub) {
		//First case, we have a translation method from sub to this
		if (sub->Declared(name))
			return true;
		//Else we test whether it is a derivation of this
		while (sub->parent != NULL) {
			if (sub == this)
				return true;
			sub = sub->parent;
		}
		return false;
	}

	Exported void Pop(KifElement* key);

	Exported void Set(short, KifElement* k, hmap<short, KifElement*>* stacks);
	Exported void Setstring(string, KifElement* k, hmap<short, KifElement*>* stacks);
	Exported KifElement* Getstring(string);
	Exported void Run(int from);
	Exported KifElement* RunThread(int from);
	Exported short Newfield(string n, KifElement* val);
	bool Basic() {
		return false;
	}

	Exported KifElement* Getfield(short n, KifDomain* dom) {
		return kifcode->Get(n, dom);
	}
	Exported string String();
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported string Type();
	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread=0,KifCallFunction* callfunc=NULL);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
};

class KifBaseFrame : public KifFrame {
public:

	KifBaseFrame(KifCode* klc, KifElement* kp, short n, KifType ktype = kifFrame) : KifFrame(klc, kp, n, ktype) {}

	void Resetreference(int inc = 1) {}
};

//-------------------------------------------------------------------------------
class KifDomainLambda : public KifDomainInstruction {
public:
	bool local;

	KifDomainLambda(KifCode* klc, KifElement* kf) : KifDomainInstruction(klc, kf, 1, kifDomainLambda) {
		local = false;
	}
};

class KifFunctionLambda : public KifFunction {
public:
	KifDomainLambda* lambdadomain;
	char used;

	KifFunctionLambda(KifCode* klc, KifElement* kp, int n) : KifFunction(klc, kp, n, kifFunctionLambda) {
		lambdadomain = new KifDomainLambda(klc, NULL);
		used = 0;
	}

	KifDomainLambda*  Thedomain() {
		return lambdadomain;
	}

};


class KifCallFunctionHaskell : public KifCallFunction {
public:
	KifFunctionLambda* body;
	vector<short> created;
	short haskelltype;

	KifDomainLambda*  Thedomain() {
		return body->lambdadomain;
	}

	Exported void Set(short s, KifElement* k, hmap<short, KifElement*>* stacks);
	void keep(short n, KifElement* k);
	void Cleantracks(int nm) {
		for (int i = 0; i<created.size(); i++) {
			if (created[i] == nm) {
				created[i] = -1;
				return;
			}
		}
	}


	KifFunction* Body() {
		return body;
	}

	KifCallFunctionHaskell(KifCode* klc, KifElement* kp, short n = 1, KifFunctionLambda* b = NULL) : KifCallFunction(klc, kp, n) {
		body = b;
		type = kifCallFunctionHaskell;
		haskelltype = -1;
	}

	KifCallFunctionHaskell(KifCallFunctionHaskell* kcall) : KifCallFunction(NULL, NULL, kcall->name) {
		body = kcall->body;
		body->Setline(body->Line(), body->Fileindex());
		Setline(body->Line(), body->Fileindex());
		kifcode = kcall->kifcode;
		haskelltype = -1;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);
	KifElement* Apply(KifFunctionLambda* bd, KifElement** params, KifElement*, KifDomain*, KifElement*, int);
	KifElement* KIFHASKELL0(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda*, int idthread);
	KifElement* KIFHASKELL2(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda*, int idthread);
	KifElement* KIFHASKELL3(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda*, int idthread);
	KifElement* KIFHASKELL5(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda*, int idthread);
	KifElement* KIFHASKELLN(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda*, int idthread);

	inline KifElement* KIFHASKELL(KifElement* contextualpattern, KifDomain* dom, KifFunctionLambda* bd, int idthread) {
		haskelltype = (haskelltype == -1) ? body->lambdadomain->instructions.size() : haskelltype;

		switch (haskelltype) {
		case 0:
			return KIFHASKELL0(contextualpattern, dom, bd, idthread);
		case 2:
			return KIFHASKELL2(contextualpattern, dom, bd, idthread);
		case 3:
			return KIFHASKELL3(contextualpattern, dom, bd, idthread);
		case 5:
			return KIFHASKELL5(contextualpattern, dom, bd, idthread);
		default:
			return KIFHASKELLN(contextualpattern, dom, bd, idthread);
		}
	}


	void Init(KifElement* f) {
		if (body != NULL)
			return;

		if (f == NULL)
			body = new KifFunctionLambda(kifcode, NULL, 1);
		else
			body = (KifFunctionLambda*)f;
	}

	void Add(KifElement* ke) {
		switch (add) {
		case 1:
			parameters.push_back(ke);
			break;
		case 2:
			function = (KifCallFunction*)ke;
			break;
		case 3:
			body->lambdadomain->Add(ke);
			break;
		}
	}
};

//-------------------------------------------------------------------------------

class KifPredefined : public KifElement {
public:
	short name;
	unsigned int arity;
	Predefined function;
	KifPredefined(KifCode* klc, unsigned int a, Predefined f, short n) : name(n), arity(a), function(f), KifElement(klc, NULL, kifPredefined) {}
	Exported KifElement* Execute(KifElement* a, KifDomain* b, KifElement* c, KifCallFunction* d, int);
};
//====================================================================================
#ifdef C11
class KifContainerLock {
public:
	std::lock_guard<std::recursive_mutex>* g;

	KifContainerLock(KifAutomaton* d);
	KifContainerLock(KifSelf* d) {
		g = NULL;
		if (kifGlobals->Test() == true)
			g = new std::lock_guard<std::recursive_mutex>(*d->_locker.lock);

	}

	KifContainerLock(KifString* d)  {
		g = NULL;
		if (kifGlobals->Test() == true)
			g = new std::lock_guard<std::recursive_mutex>(*d->_locker.lock);

	}

	KifContainerLock(KifEvaluate* d) {
		g = NULL;
		if (kifGlobals->Test() == true)
			g = new std::lock_guard<std::recursive_mutex>(*d->_locker.lock);

	}

	KifContainerLock(KifDomain* d)  {
		g = NULL;
		if (kifGlobals->Test() == true)
			g = new std::lock_guard<std::recursive_mutex>(*d->_locker.lock);

	}

	~KifContainerLock() {
		if (g != NULL)
			delete g;
	}
};
#else
class KifContainerLock {
public:
	bool unlock;
	KifThreadLock* _locker;

	KifContainerLock(KifAutomaton* l);

	KifContainerLock(KifEvaluate* l) : _locker(&l->_locker) {
		unlock = false;
		if (kifGlobals->Test() == true) {
			unlock = true;
			_locker->Locking();
		}
	}

	KifContainerLock(KifSelf* l) : _locker(&l->_locker) {
		unlock = false;
		if (kifGlobals->Test() == true) {
			unlock = true;
			_locker->Locking();
		}
	}


	KifContainerLock(KifDomain* l) : _locker(&l->_locker) {
		unlock = false;
		if (kifGlobals->Test() == true) {
			unlock = true;
			_locker->Locking();
		}
	}


	KifContainerLock(KifString* l) : _locker(&l->_locker) {
		unlock = false;
		if (kifGlobals->Test() == true) {
			unlock = true;
			_locker->Locking();
		}
	}


	~KifContainerLock() {
		if (unlock)
			_locker->Unlocking();
	}
};

#endif

//====================================================================================
class KifIteration : public KifElement {
public:
	bool reverse;
	bool initialisation;
	KifIteration(KifCode* klc, KifElement* kp) : KifElement(klc, kp, kifIteration), initialisation(false), reverse(false) {}

	bool IterInitialisation() {
		return initialisation;
	}
	virtual void IterInitialize(bool v) {
		initialisation = v;
	}
};

class KifIterationContainer : public KifIteration {
public:
	KifContainerLock* lock;
	KifEvaluate* kbase;

	KifIterationContainer(KifCode* klc, KifElement* kp, KifEvaluate* k) : KifIteration(klc, kp), lock(NULL), kbase(k)  {}

	void Lock() {
		if (lock == NULL)
			lock = new KifContainerLock(kbase);
	}

	void Unlock() {
		if (lock != NULL)
			delete lock;
		lock = NULL;
	}

	~KifIterationContainer() {
		if (lock != NULL)
			delete lock;
	}

	void IterInitialize(bool v) {
		initialisation = v;
		if (v)
			Lock();
		else
			Unlock();
	}

};

class KifIterator : public KifElement {
public:

	KifElement* value;
	bool reverse;

	KifIterator(KifCode* klc, KifElement* kp, bool rev = false) : value(kifNULL), reverse(rev), KifElement(klc, kp, kifIterator) {}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL) { return this; }
	Exported KifElement* Newiterator(bool rev) { return this; }
	Exported void IteratorSetvalue(KifElement* ke) {
		if (value != NULL)
			value->IteratorSetvalue(ke);
	}

	Exported bool Setvalue(KifElement* k, KifElement* idx = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL);
	Exported bool Putvalue(KifElement* k);

	Exported KifElement* Newinstance() {
		return this;
	}

	bool Basic() {
		return true;
	}

	bool Simple() {
		return true;
	}

	Exported void Next() {
		if (value->End() == kifTRUE)
			return;
		value->Next();
	}

	Exported KifElement* plusplus() {
		value->Next();
		return kifTRUE;
	}

	Exported KifElement* End() {
		if (!value->IterInitialisation())
			return kifTRUE;

		if (value->End() == kifTRUE) {
			value->IterInitialize(false);
			return kifTRUE;
		}
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		value->IterInitialize(true);
		if (value->Begin() == kifFALSE) {
			value->IterInitialize(false);
			return kifFALSE;
		}
		return kifTRUE;
	}

	Exported KifElement* IteratorKey() {
		return value->IteratorKey();
	}

	Exported KifElement* IteratorValue() {
		return value->IteratorValue();
	}

	Exported string String() {
		if (!value->IterInitialisation())
			return "";
		return value->IteratorValue()->String();
	}

	Exported long Integer() {
		if (!value->IterInitialisation())
			return 0;
		return value->IteratorValue()->Integer();
	}

	Exported double Float() {
		if (!value->IterInitialisation())
			return 0;
		return value->IteratorValue()->Float();
	}

	Exported bool Boolean() {
		bool theend = 1 - value->End()->Boolean();
		return theend;
	}


	Exported KifElement* Declaration(short s);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);


	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return new KifIterator(kifcode, kp, reverse);
	}

	Exported KifElement* same(KifElement* a) {
		if (value->End()->Boolean() != a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* different(KifElement* a) {
		if (value->End()->Boolean() == a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}

	Exported void Resetreference(int inc = 1);

	Exported void Clear() {
		if (value != NULL && value->type == kifIteration)
			value->Resetreference();
	}

	Exported void Clean() {
		if (value != NULL && value->type == kifIteration)
			value->Resetreference();
		KifElement::Resetreference();
	}
};
//------------------------------------------------
class KifIteratorString : public KifIteration {
public:

	string value;
	int itx, ibase, length;

	KifIteratorString(KifCode* klc, KifElement* kp) : itx(0), ibase(0), length(0), KifIteration(klc, kp) {}

	Exported KifElement* IteratorKey();
	Exported KifElement* IteratorValue();
	Exported void Next();
	Exported KifElement* End();
	Exported KifElement* Begin();
};

class KifIteratorBit : public KifIteration {
public:
	int itx;
	int maxvalue;
	vector<unsigned short>* values;
	KifIteratorBit(KifCode* klc, KifElement* kp) : itx(0), maxvalue(0), KifIteration(klc, kp) {}


	Exported KifElement* IteratorKey() {
		return kifcode->Provideinteger(itx);
	}

	Exported KifElement* IteratorValue() {
		ThreadLock _lock(kifBit);
		int bidx = itx >> 4;
		int bpos = itx - (bidx << 4);
		short v = (*values)[bidx] & (1 << bpos);
		if (v == 0)
			return kifFALSE;
		return kifTRUE;
	}

	Exported void Next() {
		if (reverse == false)
			itx++;
		else
			itx--;
	}

	Exported KifElement* End() {
		ThreadLock _lock(kifBit);
		if (reverse == false) {
			if (itx == maxvalue)
				return kifTRUE;
		}
		else {
			if (itx<0)
				return kifTRUE;
		}
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		ThreadLock _lock(kifBit);
		if (reverse == false)
			itx = 0;
		else
			itx = maxvalue - 1;
		return kifTRUE;
	}
};

class KifIteratorBits : public KifIteration {
public:
	map<short, unsigned short>::iterator it;
	map<short, unsigned short>* values;
	int itx, maxvalue;

	KifIteratorBits(KifCode* klc, KifElement* kp) : itx(0), maxvalue(0), KifIteration(klc, kp) {}


	Exported KifElement* IteratorKey() {
		return kifcode->Provideinteger(itx);
	}

	Exported KifElement* IteratorValue() {
		ThreadLock _lock(kifBits);
		int bidx = itx >> 4;
		int bpos = itx - (bidx << 4);

		if (values->find(bidx) == values->end())
			return kifFALSE;

		short v = (*values)[bidx] & (KIFLONGONE << bpos);
		if (v == 0)
			return kifFALSE;
		return kifTRUE;
	}

	Exported void Next() {
		if (reverse == false)
			itx++;
		else
			itx--;
	}

	Exported KifElement* End() {
		ThreadLock _lock(kifBits);
		if (reverse == false) {
			if (itx == maxvalue)
				return kifTRUE;
		}
		else {
			if (itx<0)
				return kifTRUE;
		}
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		ThreadLock _lock(kifBits);
		if (reverse == false)
			itx = 0;
		else
			itx = maxvalue - 1;
		return kifTRUE;
	}
};

class KifIteratorFile : public KifIteration {
public:

	KifBaseInteger thetell;
	KifBaseFile* value;

	KifIteratorFile(KifCode* klc, KifElement* kp, KifBaseFile* f) : thetell(NULL, NULL, 0), KifIteration(klc, kp) {
		thetell.kifcode = kifcode;
		value = NULL;
		if (f->mode == 1)
			value = f;
	}



	Exported KifElement* IteratorKey();
	Exported KifElement* IteratorValue();
	Exported void Next();
	Exported KifElement* End();
	Exported KifElement* Begin();
};

class KifBooleanLoop {
public:
	KifElement** alls;
	int ite;
	KifElement* var;
	hmap<short, KifElement*> local_declarations;
	KifDomain* local_variable;
	int sz;

	KifBooleanLoop(KifElement* varout, KifElement* varin);

	Exported bool Nextin() {
		if (ite == sz)
			return false;
		var->Setvalue(alls[ite]);
		ite++;
		return true;
	}

	~KifBooleanLoop() {
		var->Resetreference();
		for (ite = 0; ite<sz; ite++)
			alls[ite]->Resetreference();
		if (local_variable != NULL) {
			//then we reinstate our initial domain value
			local_variable->declarations.clear();
			local_variable->declarations = local_declarations;
		}
		delete[] alls;
	}
};

class KifLoopKey : public KifElement {
public:
	int id;
	KifBooleanLoop* loop;

	KifLoopKey(KifCode* k, KifBooleanLoop* L, int i);
	KifElement* Key();
	~KifLoopKey();
};


class KifInstructionFOR : public KifObjectInstruction {
public:

	KifInstructionFOR(KifCode* klc, KifElement* kp) : KifObjectInstruction(klc, NULL, kifInstruction) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionFOR;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	short Name() {
		return idfor;
	}
};

class KifInstructionWHILE : public KifObjectInstruction {
public:
	KifInstructionWHILE(KifCode* klc, KifElement* kp) : KifObjectInstruction(klc, NULL, kifInstruction) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionWHILE;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	short Name() {
		return idwhile;
	}

};

class KifInstructionUNTIL : public KifObjectInstruction {
public:

	KifInstructionUNTIL(KifCode* klc, KifElement* kp) : KifObjectInstruction(klc, NULL, kifInstruction) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionUNTIL;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	short Name() {
		return idwhile;
	}

};


class KifInstructionFORIN : public KifObjectInstruction {
public:
	KifInstructionFORIN(KifCode* klc, KifElement* kp) : KifObjectInstruction(klc, NULL, kifInstruction) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionFORIN;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	short Name() {
		return idfor;
	}
};

class KifInstructionFORVECTORIN : public KifObjectInstruction {
public:
	KifInstructionFORVECTORIN(KifCode* klc, KifElement* kp) : KifObjectInstruction(klc, NULL, kifInstruction) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionFORVECTORIN;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	short Name() {
		return idfor;
	}
};

class KifInstructionFORMAPIN : public KifObjectInstruction {
public:

	KifInstructionFORMAPIN(KifCode* klc, KifElement* kp) : KifObjectInstruction(klc, NULL, kifInstruction) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionFORMAPIN;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	short Name() {
		return idfor;
	}
};

class KifInstructionFORINRANGE : public KifObjectInstruction {
public:
	KifInstructionFORINRANGE(KifCode* klc, KifElement* kp) : KifObjectInstruction(klc, NULL, kifInstruction) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionFORINRANGE;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	Exported KifElement* ExecuteInteger(KifInteger* contextualpattern, KifDomain* dom, int id, KifCallFunction* callfunc);
	Exported KifElement* ExecuteFloat(KifFloat* contextualpattern, KifDomain* dom, int id, KifCallFunction* callfunc);
	Exported KifElement* ExecuteLong(KifLong* contextualpattern, KifDomain* dom, int id, KifCallFunction* callfunc);

	short Name() {
		return idfor;
	}
};


class KifInstructionFILEIN : public KifObjectInstruction {
public:
	KifInstructionFILEIN(KifCode* klc, KifElement* kp) : KifObjectInstruction(klc, NULL, kifInstruction) {
		if (kp != NULL)
			kp->Add(this);
		type = kifInstructionFORIN;
	}


	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	short Name() {
		return idfor;
	}
};

class KifKiF : public KifElement {
public:
	int id;
	string name;

	KifKiF(KifCode* klc, KifElement* kp, KifType kt = kifKiF) : id(-1), KifElement(klc, kp, kt) {}

	virtual Exported KifElement* Declaration(short n);
	virtual Exported KifElement* Information(KifElement*);
	virtual Exported void Methods(KifVectorString*);
	virtual Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);
	Exported bool initialconstructor() { return true; }

	bool Simple() {
		return true;
	}

	bool Basic() {
		return true;
	}

	virtual Exported bool Setvalue(KifElement* k, KifElement* index = NULL, KifElement* dom = NULL,int idthread=0,KifCallFunction* callfunc=NULL) {
		if (k->type == kifKiF) {
			KifKiF* p = (KifKiF*)k;
			id = p->id;
		}
		else
			id = k->kifcode->idkifmain;

		return false;
	}

	Exported string String() {
		return name;
	}
	Exported long Integer() {
		return id;
	}

	Exported bool Boolean() {
		if (id == -1)
			return false;
		return true;
	}

	Exported void Clean() {}
	Exported void Clear() {}

	Exported KifElement* same(KifElement* a) {
		if (a == kifNULL) {
			if (id == -1)
				return kifTRUE;
			else
				return kifFALSE;
		}
		else
		if (name == a->String())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);

	~KifKiF();
};


class KifThread : public KifFunction {
public:
	KifThreadLock* klock;
	bool exclusive;
	bool joined;
	KifThread(KifCode* klc, KifElement* kp, short n, bool join = false, char protect = 0, KifType kt = kifThread) : klock(NULL), joined(join), KifFunction(klc, kp, n, kt) {
		exclusive = false;
		if (protect == 1)
			klock = new KifThreadLock(NULL, false);
		else
		if (protect == 2)
			exclusive = true;
	}

	bool Joined() {
		return joined;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);
	~KifThread() {
		if (klock != NULL)
			delete klock;
	}
};

class KifProtectedFunction : public KifThread {
public:
	KifProtectedFunction(KifCode* klc, KifElement* kp, short n, char protect) : KifThread(klc, kp, n, false, protect, kifProtectedFunction) {}

};

//------------------------------------------------------------------------------------------------------
inline void jstringing(string& res, string& value) {
	if (value.find("\\") != -1)
		value = c_creplacestring(value, "\\", "\\\\");

	if (value.find("\"") == -1) {
		res += "\"";
		res += value;
		res += "\"";
	}
	else {
		string v = c_creplacestring(value, "\"", "\\\"");
		res += "\"";
		res += v;
		res += "\"";
	}
}


inline void stringing(string& res, string& value) {
	if (value.find("'") != -1) {
		if (value.find("\"") != -1) {
			res += "@\"";
			res += value;
			res += "\"@";
		}
		else {
			res += "\"";
			res += value;
			res += "\"";
		}
	}
	else {
		res += "'";
		res += value;
		res += "'";
	}
}

inline void jstringing(stringstream& res, string& value) {
	if (value.find("\"") == -1)
		res << "\"" << value << "\"";
	else {
		string v = c_creplacestring(value, "\"", "\\\"");
		res << "'" << v << "'";
	}
}

inline void stringing(stringstream& res, string& value) {
	if (value.find("'") != -1) {
		if (value.find("\"") != -1)
			res << "@\"" << value << "\"@";
		else
			res << "\"" << value << "\"";
	}
	else
		res << "'" << value << "'";
}

//------------------------------------------------------------------------------------------------------
class d_pop {
public:
	int idstack;

	d_pop(KifCallFunction* f, KifDomain* dom) {
		idstack = kifGlobals->PushFunctionStack(f, dom);
	}

	d_pop(int threadid, KifDomain* f, KifDomain* dom) {
		idstack = kifGlobals->PushFunctionStackid(threadid, f, dom);
	}

	~d_pop() {
		kifGlobals->PopFunctionStack(idstack);
	}
};

class KifInstructionStack {
public:
	int idthread;
	bool popping;
	short name;
	Stackelement* e;

	KifInstructionStack(int idthread, short n = -1, KifElement* ke = NULL);
	~KifInstructionStack();
	void Push(KifElement* ke);
	void Modify(KifElement* ke);
};

inline bool Arity(KifCallMethod& met, int sz) {
	unsigned int arity = 1 << sz;
	if ((met.arity&arity) != arity)
		return false;
	return true;
}

#define KiFArityError(name) if (!Arity(kifexportedmethods[name],callfunc->Size())) {\
	string message = "KIF(262): Wrong number of parameters in: "; \
	message += kifGlobals->KifIdString(name); \
	return kifcode->Returnerror(message); }

#endif







