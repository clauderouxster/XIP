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
filename   : kiflibs.cxx
Date       : 09/09/2010
Purpose    : Loading programs
Programmer : Claude ROUX
Reviewer   :
*/

#include "kifversion.h"
const char KIFVERSION[] = "1.94 build 17";

#ifdef KIFPYTHON
#ifndef WIN32
#define Py_BUILD_CORE
#endif
#ifdef APPLE
#ifdef DYNAMICLIB
#include <crt_externs.h>
char **environ = *_NSGetEnviron();
#endif
#endif

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif
#endif

#include "kif.h"
#include "kifpredicate.h"
#include "kifcontainer.h"

#define STR(x) (char*)x.c_str()

extern "C" {
	void NormalisationNomFichier(char* nomFichier, char* tampon);
}

#ifdef WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#else
#include <dlfcn.h>
#endif 

KifElement* PrePop(KifElement* pattern, KifDomain* dom, KifCallFunction* callfunc, int idthread);
extern double kifinfinity;

#ifdef WIN32
char KIFOS[] = "WINDOWS";
#else
#ifdef APPLE
char KIFOS[] = "MACOS";
#else
#ifdef ARCH64
char KIFOS[] = "UNIX64";
#else
char KIFOS[] = "UNIX";
#endif
#endif
#endif

//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
KifElement* PostEval(KifCode* kifcode, string& code, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread);
//------------------------------------------------------------------------------------------------------------------------
#ifdef WIN32
#include <conio.h>
int GETCH(bool echoing) {
	ThreadLock _lock(kifInputchr);
	if (echoing == false)
		return _getch();
	int i = _getch();
	cout << i;
	return i;
}
#else
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#include <termios.h>
int GETCH(bool echoing) {
	ThreadLock _lock(kifInputchr);
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag &= ~ICANON;
	if (echoing == false)
		t.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
	int c = getchar_unlocked();
	t.c_lflag |= ICANON;
	if (echoing == false)
		t.c_lflag |= ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
	return c;
}
#endif 

//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

void AddALoadingPath(string thelocalname);

static void affichexn(x_node* xn, bool premier) {
	cerr << xn->token;
	if (xn->value != "")
		cerr << ":'" << xn->value << "'";
	if (xn->nodes.size()) {
		cerr << "[";
		for (int i = 0; i<xn->nodes.size(); i++)
			affichexn(xn->nodes[i], false);
		cerr << "]";
	}
	if (premier)
		cerr << endl;
}

typedef KifElement* (KifCode::*KifParseElement)(x_node*, KifElement*);

extern int kif_declencheur;

void KifFinalClean();

KifDebugInfo* DebugPtr();
bool TestkifFunctions(short n, hmap<short, bool>& kiffunc);
string& Trim(string& chaine);
char KifCheckParameters(KifFunction* kfunction, KifCallFunction* kcf, bool add, KifFunction** body);
int KifLoadin(KifCode* kifcode, string path);
void KiFSetdebug(bool d);

#ifndef WIN32
#include <signal.h>
#endif

//===================================================================
//Use when we create a KIFCASEFUNCTION
#define KIFCASEFUNCTION -9876

//===================================================================
//List of all KifCode in memory

bool TestKifFunctions(string& token, bool func) {
	if (token == "subfunc" || token == "subfuncbis")
		return true;
	if (func == false) {
		if (token == "indexes" || token == "interval")
			return true;
	}
	return false;
}

//-------------------------------------------------------------------
Exported bool KifRecordLibraries(string name, KifExternalModule module) {
	if (kifGlobals->kifLibraries.find(name) != kifGlobals->kifLibraries.end())
		return false;

	if (module == NULL)
		return true;

	kifGlobals->kifLibraries[name] = module;
	return true;
}

Exported bool KifRecordMainLibraries(string name, KifExternalModule module) {
	if (kifGlobals->kifMainLibraries.find(name) != kifGlobals->kifMainLibraries.end())
		return false;

	if (module == NULL)
		return true;

	kifGlobals->kifMainLibraries[name] = module;
	return true;
}

Exported bool KifRecordTerminateModule(string name, KifExternalTerminateModule module) {
	if (kifGlobals->kifTerminateLibraries.find(name) != kifGlobals->kifTerminateLibraries.end())
		return false;

	if (module == NULL)
		return true;

	kifGlobals->kifTerminateLibraries[name] = module;
	return true;
}
//---------------------------------------------------------------------

Exported string Whoami_kif() {
	string s;
	s = "KiF.";
	s += KIFVERSION;
	s += Endl;
	s += KIFLIBSTRINGS[277];
	s += Endl;
	s += KIFLIBSTRINGS[278];
	s += Endl;
	s += KIFLIBSTRINGS[279];
	s += Endl;
	return s;
}


#ifdef WIN32
bool KifCode::KifLoadModule(x_node* xn, string n, string& library_name) {
	if (n == "")
		return false;

	string name = Getfilename(n);
	HINSTANCE LoadMe;
	long line = 0;
	long end = 0;
	if (xn != NULL) {
		line = GetCurrentLine(0, xn);
		end = xn->end;
	}

	string lname = name;
	int pos = lname.find_last_of(SEP);
	string subname = lname;
	if (pos != -1) {
		subname = lname.substr(pos + 1, lname.size() - pos - 1);
		pos = subname.find(".");
		if (pos != -1)
			subname = subname.substr(0, pos);
	}

	string moduleinitname = subname;
	moduleinitname += "_KifInitialisationModule";

	string initname = "Init_";
	initname += subname;
	//If it has already been loaded, we return...
	library_name = lname;
	KifExternalModule LibInitEntryPoint;

	if (KifRecordMainLibraries(moduleinitname, NULL) == false) {
		LibInitEntryPoint = kifGlobals->kifMainLibraries[moduleinitname];
		(*LibInitEntryPoint)(this, KIFVERSION);
		return true;
	}

	LoadMe = LoadLibraryA(STR(lname));
	if (LoadMe == 0) {
		string kiflib;
		if (getenv("KIFLIBS") != NULL)
			kiflib = getenv("KIFLIBS");
		else {
			lname = KIFLIBSTRINGS[280];
			throw new KifRaiseError(lname, kifGlobals->current->name, line, end);
		}

		if (getenv("PATH") != NULL) {
			string path = "Path=";
			path += kiflib;
			path += ";";
			path += getenv("PATH");
			_putenv(STR(path));
		}

		kiflib += "\\";
		kiflib += subname;
		kiflib += ".dll";
		kiflib = Getfilename(kiflib);
		library_name = kiflib;
		LoadMe = LoadLibraryA(STR(kiflib));
	}
	// Check to see if the library was loaded successfully 

	if (LoadMe == 0) {
		DWORD err = GetLastError();
		lname = KIFLIBSTRINGS[189];
		lname += name;
		throw new KifRaiseError(lname, kifGlobals->current->name, line, end);
	}

	string terminatename = subname;
	terminatename += "_Terminate";
	KifExternalModule LibMainEntryPoint;

	KifExternalTerminateModule LibTerminateEntryPoint;

	LibMainEntryPoint = (KifExternalModule)GetProcAddress(LoadMe, STR(moduleinitname));
	LibInitEntryPoint = (KifExternalModule)GetProcAddress(LoadMe, STR(initname));
	LibTerminateEntryPoint = (KifExternalTerminateModule)GetProcAddress(LoadMe, STR(terminatename));

	if (LibMainEntryPoint == NULL || LibInitEntryPoint == NULL) {
		lname = KIFLIBSTRINGS[190];
		lname += name;
		throw new KifRaiseError(lname, kifGlobals->current->name, line, end);
	}

	KifRecordLibraries(initname, LibInitEntryPoint);
	KifRecordMainLibraries(moduleinitname, LibMainEntryPoint);
	kifGlobals->kifTerminateLibraries[terminatename] = LibTerminateEntryPoint;

	if ((*LibMainEntryPoint)(this, KIFVERSION) == false) {
		FreeLibrary(LoadMe);
		return false;
	}
	return true;
}
#else
#ifdef KIFNEEDCLOSELIBS
vector<void*> KIFLIBSTOCLOSE;

void KifCloseLibraries() {
	for (int x = 0; x<KIFLIBSTOCLOSE.size(); x++)
		dlclose(KIFLIBSTOCLOSE[x]);
}
#endif
bool KifCode::KifLoadModule(x_node* xn, string n, string& library_name) {
	string name = Getfilename(n);
	void* LoadMe;
	long line = 0;
	long end = 0;
	if (xn != NULL) {
		line = GetCurrentLine(0, xn);
		end = xn->end;
	}

	char lname[1024];
	strcpy(lname, STR(name));
	char* error;
	char* pt = strrchr(lname, '/');
	char buff[1024];
	bool addso = true;
	string basename;
	if (strstr(lname + strlen(lname) - 3, ".so"))
		addso = false;
	if (pt != NULL) {
		if (memcmp(pt + 1, "lib", 3)) {
			if (addso)
				sprintf(buff, "lib%s.so", pt + 1);
			else
				sprintf(buff, "lib%s", pt + 1);
		}
		else {
			if (addso)
				sprintf(buff, "%s.so", pt + 1);
			else
				strcpy(buff, pt + 1);
		}
		basename = buff;
		strcpy(pt + 1, buff);
	}
	else {
		if (memcmp(lname, "lib", 3)) {
			if (addso)
				sprintf(buff, "lib%s.so", lname);
			else
				sprintf(buff, "lib%s", lname);
		}
		else {
			if (addso)
				sprintf(buff, "%s.so", lname);
			else
				strcpy(buff, lname);
		}
		basename = buff;
		strcpy(lname, buff);
	}

	NormalisationNomFichier(buff, lname);
	strcpy(lname, buff);
	string subname = basename.substr(3, basename.size() - 6);

	string moduleinitname = subname;
	moduleinitname += "_KifInitialisationModule";

	string initname = "Init_";
	initname += subname;
	string terminatename = subname;
	terminatename += "_Terminate";

	KifExternalModule LibInitEntryPoint;
	library_name = lname;
	//If it has already been loaded, we return...
	if (KifRecordMainLibraries(moduleinitname, NULL) == false) {
		LibInitEntryPoint = kifGlobals->kifMainLibraries[moduleinitname];
		(*LibInitEntryPoint)(this, KIFVERSION);
		return true;
	}

	LoadMe = dlopen(lname, RTLD_LAZY);
	if (LoadMe == NULL) {
		string kiflib;
		if (getenv("KIFLIBS") != NULL)
			kiflib = getenv("KIFLIBS");
		else {
			basename = KIFLIBSTRINGS[280];
			throw new KifRaiseError(basename, kifGlobals->current->name, line, end);
		}

		string ldlibpath;
		if (getenv("LD_LIBRARY_PATH") != NULL) {
			ldlibpath = getenv("LD_LIBRARY_PATH");
			ldlibpath = ":" + ldlibpath;
		}

		ldlibpath = kiflib + ldlibpath;
		setenv("LD_LIBRARY_PATH", ldlibpath.c_str(), 1);

		ldlibpath = "";
		if (getenv("PATH") != NULL) {
			ldlibpath = getenv("PATH");
			ldlibpath = ":" + ldlibpath;
		}
		ldlibpath = kiflib + ldlibpath;
		setenv("PATH", ldlibpath.c_str(), 1);


		kiflib += "/";
		kiflib += basename;
		kiflib = Getfilename(kiflib);
		library_name = kiflib;
		LoadMe = dlopen(STR(kiflib), RTLD_LAZY);
	}

	// Check to see if the library was loaded successfully 

	if (LoadMe == NULL) {
		error = dlerror();
		string message = error;
		message += ": ";
		message += lname;
		cerr << message << Endl;
		throw new KifRaiseError(message, kifGlobals->current->name, line, end);
	}

#ifdef KIFNEEDCLOSELIBS
	KIFLIBSTOCLOSE.push_back(LoadMe);
#endif


	KifExternalModule LibMainEntryPoint;
	KifExternalTerminateModule LibTerminateEntryPoint;

	LibMainEntryPoint = (KifExternalModule)dlsym(LoadMe, STR(moduleinitname));
	LibInitEntryPoint = (KifExternalModule)dlsym(LoadMe, STR(initname));
	LibTerminateEntryPoint = (KifExternalTerminateModule)dlsym(LoadMe, STR(terminatename));

	if ((error = dlerror()) != NULL) {
		string message = error;
		message += ": ";
		message += name;
		cerr << message << Endl;
		throw new KifRaiseError(message, kifGlobals->current->name, line, end);
	}

	KifRecordLibraries(initname, LibInitEntryPoint);
	KifRecordMainLibraries(moduleinitname, LibMainEntryPoint);
	kifGlobals->kifTerminateLibraries[terminatename] = LibTerminateEntryPoint;

	if ((*LibMainEntryPoint)(this, KIFVERSION) == false) {
		dlclose(LoadMe);
		return false;
	}
	return true;
}
#endif


Exported KifCode* Brandnewkifcode(KifCode* kifcodemain) {
	KifRecreatekifGlobals(kifcodemain);
	KifCode* kifcode = kifcodemain->Newinstance();
	KifReloadLibraries(kifcode);
	kifcode->SetArguments();
	return kifcode;
}

Exported void KifTerminateAllLibraries(KifCode* kifcodemain) {
	hmap<string, KifExternalTerminateModule>::iterator term;
	for (term = kifGlobals->kifTerminateLibraries.begin(); term != kifGlobals->kifTerminateLibraries.end(); term++)
		(*term->second)(kifcodemain);
}

Exported void KifRecreatekifGlobals(KifCode* kifcodemain) {
	KifTerminateAllLibraries(kifcodemain);

	int trig = 10000;
	ostream* os = &cout;
	vector<string> arguments;
	hmap<string, KifExternalModule> kifLibraries_store;
	hmap<string, KifExternalModule> kifMainLibraries_store;
	hmap<string, KifExternalTerminateModule> kifTerminateLibraries_store;

	if (kifGlobals != NULL) {
		os = kifGlobals->os;
		trig = kifGlobals->trigger;
		arguments = kifGlobals->arguments;
		kifLibraries_store = kifGlobals->kifLibraries;
		kifMainLibraries_store = kifGlobals->kifMainLibraries;
		kifTerminateLibraries_store = kifGlobals->kifTerminateLibraries;
		delete kifGlobals;
	}

	kifGlobals = new KifGlobalThread;
	kifGlobals->trigger = trig;
	kifGlobals->os = os;
	kifGlobals->Initialisation(trig, kifcodemain);
	kifGlobals->kifCodeVector.push_back(kifcodemain);

	kifGlobals->arguments = arguments;
	kifGlobals->kifLibraries = kifLibraries_store;
	kifGlobals->kifMainLibraries = kifMainLibraries_store;
	kifGlobals->kifTerminateLibraries = kifTerminateLibraries_store;
	kifcodemain->gGlobals = kifGlobals;
	//we need to rebuild kstart that has been lost in delete kifGlobals...
	kifcodemain->Updatekstart();
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//We use this class to deactivate temporarily the instance sharing...
//In certain cases, we need to keep different instances apart as in Haskell formulae...
class derecording {
public:
	bool rec;
	KifCode* kifcode;
	hmap<long, KifElement*> localinstances;

	derecording(KifCode* k) {
		kifcode = k;
		localinstances = kifcode->instances;

		rec = kifcode->recordinstance;
		kifcode->recordinstance = false;
	}

	~derecording() {
		kifcode->recordinstance = rec;
		kifcode->instances = localinstances;
	}
};

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

Exported void KifReloadLibraries(KifCode* kifcode) {
	KifExternalModule LibInitEntryPoint;
	hmap<string, KifExternalModule>::iterator it;
	for (it = kifGlobals->kifMainLibraries.begin(); it != kifGlobals->kifMainLibraries.end(); it++) {
		LibInitEntryPoint = it->second;
		(*LibInitEntryPoint)(kifcode, KIFVERSION);
	}
}

KifElement* KifCode::KifTerminateVariable(x_node* xn, KifElement* kv, KifElement* var, KifElement* kf) {
	short name = KifNewId(xn->nodes[1]->value);
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(name, var, NULL);
	return var;
}

KifCode* KifElement::KifCurrent() {
	return kifcode;
}


bool KifCode::KifDeclaredFunctionTest(short name) {
	if (kifDeclaredFunction.find(name) != kifDeclaredFunction.end())
		return true;
	return false;
}

KifElement* KifCode::KifParseNegation(x_node* xn, KifElement* kf) {
	kf->Setnegation(true);
	return kf;
}

KifInstruction* KifCode::KifCreateInstruction(KifElement* kf, KifOperator op) {
	KifInstruction* res;
	switch (op) {
	case kifPLUSEQU:
	case kifMINUSEQU:
	case kifMULTIPLYEQU:
	case kifDIVIDEEQU:
	case kifMODEQU:
	case kifPOWEREQU:
	case kifSHIFTLEFTEQU:
	case kifSHIFTRIGHTEQU:
	case kifUNIONEQU:
	case kifXOREQU:
	case kifINTERSECTIONEQU:
		res = new KifInstructionAPPLYOPERATIONEQU(this, kf);
		break;
	case kifSTREAM:
	case kifAFFECTATION:
		res = new KifInstructionAFFECTATION(this, kf);
		break;
	case kifINITIALISATION:
		res = new KifInstructionINITIALISATION(this, kf);
		break;
	case kifEQUAL:
	case kifDIFFERENT:
	case kifINFERIOR:
	case kifSUPERIOR:
	case kifSUPEQU:
	case kifINFEQU:
		res = new KifInstructionCOMPARE(this, kf);
		break;
	case kifPLUS:
	case kifMINUS:
	case kifMULTIPLY:
	case kifDIVIDE:
	case kifMOD:
	case kifPOWER:
	case kifSHIFTLEFT:
	case kifSHIFTRIGHT:
	case kifUNION:
	case kifXOR:
	case kifINTERSECTION:
		res = new KifInstructionAPPLYOPERATION(this, kf);
		break;
	case kifPLUSPLUS:
		res = new KifInstructionPLUSPLUS(this, kf);
		break;
	case kifMINUSMINUS:
		res = new KifInstructionMINUSMINUS(this, kf);
		break;
	case kifOR:
		res = new KifInstructionOR(this, kf);
		break;
	case kifAND:
		res = new KifInstructionAND(this, kf);
		break;
	case kifMERGE:
		res = new KifInstructionMERGE(this, kf);
		break;
	case kifMERGEEQU:
		res = new KifInstructionMERGEEQU(this, kf);
		break;
	case kifADD:
		res = new KifInstructionADDTOLIST(this, kf);
		break;
	case kifADDEQU:
		res = new KifInstructionADDTOLISTEQU(this, kf);
		break;
	case kifNOTIN:
	case kifIN:
		res = new KifInstructionKIFIN(this, kf);
		break;
	case kifON:
		res = new KifInstructionKIFON(this, kf);
		break;
	default:
		res = new KifInstruction(this, kf);
	}

	res->Setaction(op);
	return res;
}

KifInstruction* KifCode::CloningInstruction(KifInstruction* ki) {
	if (ki->action == kifBLOC || ki->action == kifBLOCBOOLEAN || ki->type != kifInstruction)
		return ki;

	KifInstruction* k = KifCreateInstruction(NULL, ki->Action());
	ki->Clone(k);
	ki->Release();
	return k;
}

KifElement* KifCode::KifParseBoolean(x_node* xn, KifElement* kf) {
	KifElement* kbloc;
	KifElement* ke;

	if (xn->nodes.size() == 1) {
		//It is a test on a function or on a single variable...
		kbloc = KifCreateInstruction(NULL, kifBLOCBOOLEAN);
		kbloc->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
		KifBrowse(xn->nodes[0], kbloc);
		//The test on negation stems from the fact, that negation will be tested through kifBLOCBOOLEAN in KifInstruction::Exec
		//otherwise, we do not need it...
		if (kbloc->Action() == kifBLOCBOOLEAN && kbloc->InstructionSize() == 1 && !kbloc->Negation()) {
			//In this case, we can get rid of it...
			ke = kbloc->Instruction(0);
			kbloc->Release();
			kf->Add(ke);
			return ke;
		}

		kbloc = CloningInstruction((KifInstruction*)kbloc);
		kf->Add(kbloc);
		return kbloc;
	}
	//Else, we have two expressions and an operator
	//If our operator is new to our test
	ke = kf;
	KifOperator op = kifGlobals->kifOperators[xn->nodes[1]->value];
	if (op != kf->Action()) {
		if (kf->Action() == kifBLOCBOOLEAN)
			ke->Setaction(op);
		else {
			ke = KifCreateInstruction(kf, op);
			ke->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
		}
	}

	kbloc = KifCreateInstruction(NULL, kifBLOCBOOLEAN);
	kbloc->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
	KifBrowse(xn->nodes[0], kbloc);
	if (kbloc->Action() == kifBLOCBOOLEAN && kbloc->InstructionSize() == 1 && !kbloc->Negation()) {
		//In this case, we can get rid of it...
		KifElement* k = kbloc->Instruction(0);
		kbloc->Release();
		kbloc = k;
	}
	else
		kbloc = CloningInstruction((KifInstruction*)kbloc);

	ke->Add(kbloc);

	if (xn->nodes.size() == 3)
		KifBrowse(xn->nodes[2], ke);
	else
		KifBrowse(xn->nodes[1], kbloc);
	return ke;
}

KifElement* KifCode::KifParseTestSwitch(x_node* xn, KifElement* kf) {
	if (xn->nodes[0]->token == "default")
		kf->Add(kifDEFAULT);
	else
		KifBrowse(xn->nodes[0], kf);
	hmap<long, KifElement*> localinstances;
	localinstances = instances;

	KifDomain* ktrue = new KifInstructionSEQUENCE(this, kf, idsequence);
	ktrue->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
	kifGlobals->PushStack(ktrue);
	KifBrowse(xn->nodes[1], ktrue);
	kifGlobals->PopStack();
	if (ktrue->InstructionSize() == 1 && !ktrue->Instruction(0)->Variable()) {
		kf->SetInstruction(kf->InstructionSize() - 1, ktrue->Instruction(0));
		ktrue->InstructionClear();
		ktrue->Release();
	}
	instances = localinstances;

	return ktrue;
}

KifElement* KifCode::KifParseSwitch(x_node* xn, KifElement* kf) {
	//We create a IF section
	KifElement* kswitch = new KifInstructionSWITCH(this, kf);
	kswitch->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
	KifBrowse(xn->nodes[0], kswitch);
	int i = 1;
	if (xn->nodes[1]->token == "depend") {
		i++;
		long line = GetCurrentLine(0, xn->nodes[1]);
		string funcname = xn->nodes[1]->nodes[0]->value;
		short idf = KifNewId(funcname);
		KifElement* kfunc = NULL;
		kfunc = Getbasic(idf, kf);
		//We have a WITH description
		if (kfunc == NULL) {
			string message = KIFLIBSTRINGS[178] + funcname;
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		if (kfunc->isFunction()) {
			if (kfunc->Size() != 2) {
				string message = KIFLIBSTRINGS[281];
				throw new KifRaiseError(message, kifGlobals->current->name, kifGlobals->current_start, kifGlobals->current_end);
			}

			kswitch->Setfunction(kfunc);
		}
		else {
			if (!kfunc->isCall()) {
				string message = KIFLIBSTRINGS[282] + kifGlobals->kifIdString[kfunc->Name()] + "'";
				throw new KifRaiseError(message, kifGlobals->current->name, kifGlobals->current_start, kifGlobals->current_end);
			}
			KifInstance* ki;
			int idfunc = kfunc->Name();
			if (kifGlobals->kconstants->declarations.check(idfunc)) {
				ki = new KifInstanceConstant(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
				ki->Setinstancetype(kinConstant);
			}
			else {
				if (kfunc->type == kifFunctionDeclaration) {
					ki = new KifInstanceInFunction(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
					ki->Setinstancetype(kinFunction);
				}
				else {
					if (kfunc->type == kifFrameDeclaration) {
						KifElement* currentdeclaration = FindFrameForSequence(kfunc, xn, -1);
						if (currentdeclaration == kstart) {
							ki = new KifInstanceGlobal(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
							ki->Setinstancetype(kinGlobal);
						}
						else {
							ki = new KifInstanceInFrame(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
							ki->Setinstancetype(kinFrame);
						}
					}
					else
						ki = new KifInstance(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
				}
			}
			kswitch->Setfunction(ki);
		}
	}

	for (; i<xn->nodes.size(); i++)
		KifBrowse(xn->nodes[i], kswitch);
	KifInstructionSWITCH* ks = (KifInstructionSWITCH*)kswitch;
	if (ks->function == NULL) {
		bool onlybasevalue = true;
		for (i = 1; i<ks->instructions.size(); i += 2) {
			if (ks->instructions[i] == kifDEFAULT)
				break;

			if (ks->instructions[i]->Basevalue() == false) {
				onlybasevalue = false;
				break;
			}
			ks->keys[ks->instructions[i]->String()] = i;
		}
		ks->usekeys = onlybasevalue;
	}
	return kswitch;
}

KifElement* KifCode::KifParseTryCatch(x_node* xn, KifElement* kf) {
	KifElement* ke = new KifInstructionTRY(this, kf);
	ke->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
	string name;
	KifElement* declaration;
	KifInstance* ki;

	long line = GetCurrentLine(0, xn);
	short id;
	if (xn->nodes.size() != 1 && xn->nodes[1]->token == "word") {
		name = xn->nodes[1]->value;
		id = KifNewId(name);
		declaration = Getbasic(id, kf);

		if (declaration == NULL || declaration->XIP()) {
			string message = KIFLIBSTRINGS[283] + name + "'";
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}

		if (!declaration->Variable()) {
			if (xn->nodes.size() == 1 && (kifAllTypes[id] == true || declaration->isFunction())) {
				KifElement* ke = new KifSelf(this, kf, declaration);
				ke->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
				return ke;
			}
			string message = KIFLIBSTRINGS[284] + name + "'";
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}

		KifInstruction* kaff = KifCreateInstruction(ke, kifAFFECTATION);
		kaff->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
		if (kifGlobals->kconstants->declarations.check(id)) {
			ki = new KifInstanceConstant(this, kaff, id, (KifCommonVariable*)declaration);
			ki->Setinstancetype(kinConstant);
		}
		else {
			if (declaration->type == kifFunctionDeclaration) {
				ki = new KifInstanceInFunction(this, kaff, id, (KifCommonVariable*)declaration);
				ki->Setinstancetype(kinFunction);
			}
			else {
				if (declaration->type == kifFrameDeclaration) {
					KifElement* currentdeclaration = FindFrameForSequence(declaration, xn, -1);
					if (currentdeclaration == kstart) {
						ki = new KifInstanceGlobal(this, kaff, id, (KifCommonVariable*)declaration);
						ki->Setinstancetype(kinGlobal);
					}
					else {
						ki = new KifInstanceInFrame(this, kaff, id, (KifCommonVariable*)declaration);
						ki->Setinstancetype(kinFrame);
					}
				}
				else
					ki = new KifInstance(this, kaff, id, (KifCommonVariable*)declaration);
			}
		}
		ki->Setstringvalue(name);
		kaff->Add(kifNULL);
	}

	KifBrowse(xn->nodes[0], ke);


	KifElement* kfunc = kifGlobals->kifBaseFunctions[idcatch];
	KifCallFunction* kcf = new KifCallFunctionPredefined(this, ke, idcatch, (KifPredefined*)kfunc);
	kcf->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);

	if (xn->nodes.size() != 1) {
		if (xn->nodes[1]->token == "word") {
			if (kifGlobals->kconstants->declarations.check(id)) {
				ki = new KifInstanceConstant(this, kcf, id, (KifCommonVariable*)declaration);
				ki->Setinstancetype(kinConstant);
			}
			else {
				if (declaration->type == kifFunctionDeclaration) {
					ki = new KifInstanceInFunction(this, kcf, id, (KifCommonVariable*)declaration);
					ki->Setinstancetype(kinFunction);
				}
				else
				if (declaration->type == kifFrameDeclaration) {
					KifElement* currentdeclaration = FindFrameForSequence(declaration, xn, -1);
					if (currentdeclaration == kstart) {
						ki = new KifInstanceGlobal(this, kcf, id, (KifCommonVariable*)declaration);
						ki->Setinstancetype(kinGlobal);
					}
					else {
						ki = new KifInstanceInFrame(this, kcf, id, (KifCommonVariable*)declaration);
						ki->Setinstancetype(kinFrame);
					}
				}
			}
			ki->Setstringvalue(name);
			ki->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
			if (xn->nodes.size() == 3) {
				if (xn->nodes[2]->token == "blocs")  {
					KifElement* kbloc = new KifInstructionCATCH(this, ke);
					kbloc->Setline(linereference + GetCurrentLine(0, xn->nodes[2]), kifGlobals->debuginfocurrent->currentfile);
					//Instruction
					KifBrowse(xn->nodes[2], kbloc);
				}
			}
		}
		else {
			if (xn->nodes[1]->token == "blocs")  {
				KifElement* kbloc = new KifInstructionCATCH(this, ke);
				kbloc->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
				//Instruction
				KifBrowse(xn->nodes[1], kbloc);
			}
		}
	}
	kcf->add = 0;
	return ke;
}


KifElement* KifCode::KifParseTest(x_node* xn, KifElement* kf) {
	KifInstructionIF* ktest = new KifInstructionIF(this, kf);
	ktest->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
	KifBrowse(xn->nodes[0], ktest);
	KifElement* nxt = ktest->instructions[0];

	//Small improvement, when we have only one element of test
	//It is BOOLEANBLOC, with only one element...
	//We can get rid of it and push it to the top...
	if (nxt->Action() == kifBLOCBOOLEAN && nxt->InstructionSize() == 1) {
		//we push the negation up then... otherwise it would be lost
		ktest->negation = nxt->Negation();
		nxt = nxt->Instruction(0);
		ktest->instructions[0]->Release();
		ktest->instructions[0] = nxt;
	}

	hmap<long, KifElement*> localinstances;
	localinstances = instances;
	KifDomain* ktrue = new KifInstructionSEQUENCE(this, ktest, idsequence);
	kifGlobals->PushStack(ktrue);
	ktrue->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	KifBrowse(xn->nodes[1], ktrue);
	kifGlobals->PopStack();
	if (ktrue->InstructionSize() == 1 && !ktrue->Instruction(0)->Variable()) {
		nxt = ktrue->Instruction(0);
		ktest->SetInstruction(1, nxt);
		ktrue->InstructionClear();
		ktrue->Release();
	}
	instances = localinstances;

	if (xn->nodes.size() == 3) {
		KifDomain* kfalse = new KifInstructionSEQUENCE(this, ktest, idsequence);
		kfalse->Setline(linereference + GetCurrentLine(0, xn->nodes[2]), kifGlobals->debuginfocurrent->currentfile);
		kifGlobals->PushStack(kfalse);
		KifBrowse(xn->nodes[2], kfalse);
		kifGlobals->PopStack();
		if (kfalse->InstructionSize() == 1 && !kfalse->Instruction(0)->Variable()) {
			nxt = kfalse->Instruction(0);
			ktest->SetInstruction(2, nxt);
			kfalse->InstructionClear();
			kfalse->Release();
		}
		instances = localinstances;
	}

	return ktest;
}

KifElement* KifCode::KifParseUntil(x_node* xn, KifElement* kf) {
	KifInstructionUNTIL* kuntil = new KifInstructionUNTIL(this, kf);
	kuntil->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	KifBrowse(xn->nodes[1], kuntil);
	KifDomain* kseq = new KifInstructionSEQUENCE(this, kuntil, idsequence);
	hmap<long, KifElement*> localinstances;
	localinstances = instances;
	kseq->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
	kifGlobals->PushStack(kseq);
	KifBrowse(xn->nodes[0], kseq);
	kifGlobals->PopStack();
	if (kseq->InstructionSize() == 1 && !kseq->Instruction(0)->Variable()) {
		kuntil->SetInstruction(1, kseq->Instruction(0));
		kseq->InstructionClear();
		kseq->Release();
	}

	instances = localinstances;
	KifElement* kinst = kuntil->instructions[1];
	if (kinst->type == kifInstructionSEQUENCE && kinst->InstructionSize() == 1)
		kuntil->instructions[1] = kinst->Instruction(0);

	return kuntil;
}

KifElement* KifCode::KifParseWhile(x_node* xn, KifElement* kf) {
	KifInstructionWHILE* kwhile = new KifInstructionWHILE(this, kf);
	kwhile->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
	KifBrowse(xn->nodes[0], kwhile);
	hmap<long, KifElement*> localinstances;
	localinstances = instances;

	KifDomain* kseq = new KifInstructionSEQUENCE(this, kwhile, idsequence);
	kseq->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kifGlobals->PushStack(kseq);
	KifBrowse(xn->nodes[1], kseq);
	kifGlobals->PopStack();
	if (kseq->InstructionSize() == 1 && !kseq->Instruction(0)->Variable()) {
		kwhile->SetInstruction(1, kseq->Instruction(0));
		kseq->InstructionClear();
		kseq->Release();
	}

	instances = localinstances;

	KifElement* kinst = kwhile->instructions[1];
	if (kinst->type == kifInstructionSEQUENCE && kinst->InstructionSize() == 1)
		kwhile->instructions[1] = kinst->Instruction(0);

	return kwhile;
}

KifElement* KifCode::KifParseForIn(x_node* xn, KifElement* kf) {

	KifObjectInstruction* kforin;
	KifInstruction ktemp(NULL, NULL, kifBLOCLOOPIN);
	KifBrowse(xn->nodes[1], &ktemp);
	KifElement* kcontainer = ktemp.instructions[0];
	bool popping = false;
	KifElement* kref = kf;
	KifDomain* kbase = NULL;
	if (xn->nodes[0]->nodes.size() == 2 && xn->nodes[0]->token == "declarationfor") {
		kbase = new KifInstructionSEQUENCE(this, kf, idsequence);
		xn->nodes[0]->token = "declaration";
		KifBrowse(xn->nodes[0], kbase);
		kifGlobals->PushStack(kbase);
		popping = true;
		kref = kbase;
	}

	if (kcontainer->type == kifInstance && ((KifInstance*)kcontainer)->value->isfile())
		kforin = new KifInstructionFILEIN(this, kref);
	else {
		if (xn->nodes[1]->token == "arange")
			kforin = new KifInstructionFORINRANGE(this, kref);
		else {
			if (xn->nodes[0]->token == "valvectortail")
				kforin = new KifInstructionFORVECTORIN(this, kref);
			else {
				if (xn->nodes[0]->token == "valmaptail")
					kforin = new KifInstructionFORMAPIN(this, kref);
				else
					kforin = new KifInstructionFORIN(this, kref);
			}
		}
	}

	kforin->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
	KifElement* kin = KifCreateInstruction(kforin, kifBLOCLOOPIN);
	if (kbase == NULL)
		KifBrowse(xn->nodes[0], kin);
	else
		KifBrowse(xn->nodes[0]->nodes[1], kin);

	if (kforin->type == kifInstructionFORINRANGE) {
		if (kin->Instruction(0)->type != kifInstance) {
			string message = KIFLIBSTRINGS[285];
			throw new KifRaiseError(message, kifGlobals->current->name, kforin->line, xn->end);
		}

		KifType tyvar = ((KifInstance*)kin->Instruction(0))->value->type;
		if (tyvar != kifInteger && tyvar != kifFloat && tyvar != kifLong) {
			string message = KIFLIBSTRINGS[286];
			throw new KifRaiseError(message, kifGlobals->current->name, kforin->line, xn->end);
		}

		kin->Add(ktemp.instructions[0]);
		kin->Add(ktemp.instructions[1]);
		kin->Add(ktemp.instructions[2]);
	}
	else
		kin->Add(kcontainer);


	//We then compile the instruction bloc
	hmap<long, KifElement*> localinstances;
	localinstances = instances;
	KifDomain* ktrue = new KifInstructionSEQUENCE(this, kforin, idsequence);
	ktrue->Setline(linereference + GetCurrentLine(0, xn->nodes[2]), kifGlobals->debuginfocurrent->currentfile);
	kifGlobals->PushStack(ktrue);
	KifBrowse(xn->nodes[2], ktrue);
	kifGlobals->PopStack();
	if (popping)
		kifGlobals->PopStack();
	if (ktrue->InstructionSize() == 1 && !ktrue->Instruction(0)->Variable()) {
		kforin->SetInstruction(1, ktrue->Instruction(0));
		ktrue->InstructionClear();
		ktrue->Release();
	}
	instances = localinstances;
	return kforin;
}

KifElement* KifCode::KifParseBlocFor(x_node* xn, KifElement* kf) {
	hmap<long, KifElement*> localinstances;
	localinstances = instances;

	KifDomain* kbloc = new KifInstructionSEQUENCE(this, kf, idsequence);
	for (int i = 0; i<xn->nodes.size(); i++)
		KifBrowse(xn->nodes[i], kbloc);
	instances = localinstances;
	return kbloc;
}

KifElement* KifCode::KifParseFor(x_node* xn, KifElement* kf) {
	KifDomain* kbase = NULL;
	KifInstructionFOR* kfor;
	bool popping = false;
	if (xn->nodes[0]->token == "multideclaration") {
		kbase = new KifInstructionSEQUENCE(this, kf, idsequence);
		KifBrowse(xn->nodes[0], kbase);
		kifGlobals->PushStack(kbase);
		kfor = new KifInstructionFOR(this, kbase);
		popping = true;
	}
	else
		kfor = new KifInstructionFOR(this, kf);

	kfor->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
	//Initialisation
	if (!popping)
		KifBrowse(xn->nodes[0], kfor);
	else
		kfor->Add(kifTRUE);

	//Test
	KifBrowse(xn->nodes[1], kfor);
	//Increment
	KifBrowse(xn->nodes[2], kfor);
	hmap<long, KifElement*> localinstances;
	localinstances = instances;

	KifDomain* kbloc = new KifInstructionSEQUENCE(this, kfor, idsequence);
	kbloc->Setline(linereference + GetCurrentLine(0, xn->nodes[3]), kifGlobals->debuginfocurrent->currentfile);
	//Instruction
	kifGlobals->PushStack(kbloc);
	KifBrowse(xn->nodes[3], kbloc);
	kifGlobals->PopStack();
	if (popping)
		kifGlobals->PopStack();

	if (kbloc->InstructionSize() == 1 && !kbloc->Instruction(0)->Variable()) {
		kfor->SetInstruction(3, kbloc->Instruction(0));
		kbloc->InstructionClear();
		kbloc->Release();
	}

	instances = localinstances;
	return kfor;
}

//The parameter list is a two nodes tree, where the second node is always a parameter
//hence a recursive analysis
void KifCode::ComputeParameters(x_node* xn, KifCallFunction* kcf) {
	KifInstruction kbloc(NULL, NULL, kifBLOC);
	kbloc.kifcode = this;
	kbloc.type = kifInstructionPARAMETER;
	KifBrowse(xn->nodes[0], &kbloc);
	kcf->Add(kbloc.instructions[0]);

	if (xn->nodes.size() == 2)
		ComputeParameters(xn->nodes[1], kcf);
}

KifElement* KifCode::KifParseUniqueCall(x_node* xn, KifElement* kf) {
	string& name = xn->value;
	//Looking if it is known as function
	KifElement* kcf;

	if (name == KIFLIBSTRINGS[287])
		kcf = new KifCallFunctionBreakOuContinue(this, kf, kifBREAK);
	else
	if (name == KIFLIBSTRINGS[288])
		kcf = new KifCallFunctionBreakOuContinue(this, kf, kifCONTINUE);
	else
	if (name == KIFLIBSTRINGS[289] || name == KIFLIBSTRINGS[290])
		kcf = new KifReturn(this, kf);
	else
	if (name == "kifbreakpoint")
		kcf = new KifCallFunctionBreakpoint(this, kf);
	else {
		KifElement* kfunc = kifGlobals->kifBaseFunctions[idkifmain];
		kcf = new KifCallFunctionPredefined(this, kf, idkifmain, (KifPredefined*)kfunc);
		((KifCallFunctionPredefined*)kcf)->add = 0;
		kf->Initvalue(kcf);
	}


	kcf->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
	return kcf;
}

KifElement* KifCode::KifNonLimited(x_node* xn, KifElement* kf) {
	kf->Setlimited(true);
	return kf;
}

KifElement* KifCode::KifParseCall(x_node* xn, KifElement* kf) {
	string name = xn->nodes[0]->value;
	short id = KifNewId(name);
	short idframe = -1;
	short fullid = -1;
	string params;

	if (xn->token == "predicatecall")
		params = "predicateparameters";
	else
		params = "parameters";

	//It could be a predicate, then two cases as part of an expression or as a call to evaluation
	if (predicates.find(id) != predicates.end()) {
		//then it is a PredicateInstance
		if (kf->type != kifPredicateRuleElement) {
			predicatevariables.clear();
			KifInstructionLaunch* kbloc = new KifInstructionLaunch(this, kf);
			kf = kbloc;
		}

		KifPredicate* kx = predicates[id];
		kx = new KifPredicate(this, kf, id);
		if (xn->nodes[1]->token == "expression") {
			KifInstruction kbloc(NULL, NULL, kifBLOC);
			KifBrowse(xn->nodes[1], &kbloc);
			kx->weight = kbloc.instructions[0];
		}

		if (xn->nodes.back()->token == params)
			ComputePredicateParameters(xn->nodes.back(), kx);
		return kx;
	}

	//It could be a term
	if (terms.find(id) != terms.end() || kf->type == kifInstructionPARAMETERPredicate) {
		KifPredicateTerm* kx = new KifPredicateTerm(this, kf, id, name);
		if (terms.find(id) == terms.end())
			terms[id] = name;
		if (xn->nodes.back()->token == params)
			ComputePredicateParameters(xn->nodes.back(), kx);
		return kx;
	}

	if (xn->nodes[0]->nodes.size() != 1) {
		string framename = xn->nodes[0]->nodes[0]->nodes[0]->value;
		name = xn->nodes[0]->nodes[1]->value;
		idframe = KifNewId(framename);
		framename += "::";
		framename += name;
		fullid = KifNewId(framename);
		if (fullid == id)
			//In this case, name was previously the concatenation of frame::name
			id = KifNewId(name);
	}

	bool basictype = false;
	bool getbasefunction = false;
	long line = GetCurrentLine(0, xn);
	KifElement* res;

	if (name == KIFLIBSTRINGS[291]) {
		//very specific case, to set the stack size from within
		if (xn->nodes.size() == 2) {
			KifCallFunction call(NULL, NULL, idloadin);
			KifInstruction kxf(NULL, NULL);
			kxf.kifcode = NULL;
			KifBrowse(xn->nodes[1], &kxf);
			res = kxf.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
			long sz = res->Integer();
			kifGlobals->stacklimit = sz;
		}
		else {
			string message = KIFLIBSTRINGS[292];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		return kf;
	}

	if (name == KIFLIBSTRINGS[293]) {
		//very specific case, to set the stack size from within
		if (xn->nodes.size() == 2) {
			KifCallFunction call(NULL, NULL, idloadin);
			KifInstruction kxf(NULL, NULL);
			kxf.kifcode = NULL;
			KifBrowse(xn->nodes[1], &kxf);
			res = kxf.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
			long sz = res->Integer();
			kifGlobals->maxjoined = sz;
			kifGlobals->THR[0]->joinedloquets.reserve(sz);
		}
		else {
			string message = KIFLIBSTRINGS[294];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		return kf;
	}

	if (name == KIFLIBSTRINGS[295]) {
		//very specific case, to set the stack size from within
		if (xn->nodes.size() == 2) {
			KifCallFunction call(NULL, NULL, idloadin);
			KifInstruction kxf(NULL, NULL);
			kxf.kifcode = NULL;
			KifBrowse(xn->nodes[1], &kxf);
			res = kxf.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
			kifGlobals->resizethreads(res->Integer());
		}
		else {
			string message = KIFLIBSTRINGS[296];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		return kf;
	}

	if (name == KIFLIBSTRINGS[297]) {
		//very specific case, to set the stack size from within
		if (xn->nodes.size() == 2) {
			KifCallFunction call(NULL, NULL, idloadin);
			KifInstruction kxf(NULL, NULL);
			kxf.kifcode = NULL;
			KifBrowse(xn->nodes[1], &kxf);
			res = kxf.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
			kifGlobals->doubledisplay = res->Boolean();
		}
		else {
			string message = KIFLIBSTRINGS[298];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		return kf;
	}

	if (name == KIFLIBSTRINGS[299]) {
		//very specific case, to set the garbage size from within
		if (xn->nodes.size() == 2) {
			KifCallFunction call(NULL, NULL, idloadin);
			KifInstruction kxf(NULL, NULL);
			kxf.kifcode = NULL;
			KifBrowse(xn->nodes[1], &kxf);
			res = kxf.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
			long sz = res->Integer();
			if (sz>kif_declencheur) {
				kifGlobals->trigger = sz;
				kifGlobals->garbage.ajuste(sz);
			}
		}
		else {
			string message = KIFLIBSTRINGS[300];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		return kf;
	}

	if (name == KIFLIBSTRINGS[301]) {
		if (xn->nodes.size() == 2) {
			KifElement* kfunc = NULL;
			name = xn->nodes[1]->nodes[0]->nodes[0]->value;
			id = KifNewId(name);
			kfunc = Getbasic(id, kf);
			//We have a WITH description
			if (kfunc == NULL || kfunc->type != kifFunction) {
				string message = KIFLIBSTRINGS[178] + name;
				long line = GetCurrentLine(0, xn);
				throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
			}
			kifGlobals->garbagefunction = (KifFunction*)kfunc;
		}
		else {
			string message = KIFLIBSTRINGS[302];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		return kf;
	}

	if (name == KIFLIBSTRINGS[303]) {
		if (xn->nodes.size() != 2 || xn->nodes[1]->nodes.size() != 2) {
			string message = KIFLIBSTRINGS[304];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		KifCallFunction call(NULL, NULL, idloadin);
		KifInstruction kxn(NULL, NULL);
		KifBrowse(xn->nodes[1]->nodes[0], &kxn);
		string e_name = kxn.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call)->String();
		KifInstruction kxv(NULL, NULL);
		KifBrowse(xn->nodes[1]->nodes[1], &kxv);
		string e_val = kxv.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call)->String();
#ifdef WIN32
		e_name += "=";
		e_name += e_val;
		_putenv(STR(e_name));
#else
		setenv(e_name.c_str(), e_val.c_str(), 1);
#endif
		return kf;
	}

	if (name == KIFLIBSTRINGS[305]) {
		//very specific case, where we load our lib on the fly
		if (xn->nodes.size() == 2) {
			KifInstruction kxf(NULL, NULL);
			kxf.kifcode = NULL;

			KifElement* pathlib = kifNULL;
			KifElement* platform;
			KifElement* arg;
			bool load = true;
			KifCallFunction call(NULL, NULL, idloadin);
			if (xn->nodes[1]->nodes.size() == 2) {
				KifInstruction kxl(NULL, NULL);
				kxl.kifcode = NULL;
				if (xn->nodes[1]->nodes[1]->nodes.size() == 2) {
					KifBrowse(xn->nodes[1]->nodes[0], &kxf);
					arg = kxf.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
					KifInstruction kxp(NULL, NULL);
					kxp.kifcode = NULL;
					KifBrowse(xn->nodes[1]->nodes[1]->nodes[0], &kxp);
					platform = kxp.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
					if (arg->String() == platform->String()) {
						KifBrowse(xn->nodes[1]->nodes[1]->nodes[1], &kxl);
						pathlib = kxl.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
					}
					else
						load = false;
				}
				else {
					KifBrowse(xn->nodes[1]->nodes[0], &kxf);
					platform = kxf.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
					if (platform->String() != KIFOS)
						load = false;
					else {
						KifBrowse(xn->nodes[1]->nodes[1], &kxl);
						pathlib = kxl.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
					}
				}
			}
			else {
				KifBrowse(xn->nodes[1], &kxf);
				pathlib = kxf.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
			}

			if (load) {
				string libname;
				KifLoadModule(xn, pathlib->String(), libname);
			}
		}
		else {
			string message = KIFLIBSTRINGS[306];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		return kf;
	}

	if (name == KIFLIBSTRINGS[307]) {
		//very specific case, where we load a KiF program on the fly
		if (xn->nodes.size() == 2) {
			res = kifTRUE;
			//We can have a test as a second argument...
			if (xn->nodes[1]->nodes.size() == 2) {
				KifInstruction ktst(NULL, NULL, kifBLOC);
				ktst.kifcode = this;
				KifBrowse(xn->nodes[1]->nodes[1], &ktst);
				KifInstruction* klocaltest = CloningInstruction(&ktst);
				res = klocaltest->Exec(kifTRUE, kstart, kifNULL, 0, NULL);
				//If the element was not cloned, we cannot remove it...
				if (klocaltest != &ktst)
					klocaltest->Release();
			}

			if (res->Boolean()) {
				KifCallFunction call(NULL, NULL, idloadin);
				KifInstruction kxf(NULL, NULL);
				kxf.kifcode = NULL;
				KifBrowse(xn->nodes[1]->nodes[0], &kxf);
				res = kxf.instructions[0]->Exec(kifNULL, kstart, kifNULL, 0, &call);
				KifLoadin(this, res->String());
				res->Release();
			}
		}
		else {
			string message = KIFLIBSTRINGS[308];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		return kf;
	}

	//Particular case, where the function is a predefined function linked to an instance
	if (kifGlobals->KifBaseFunctionTest(id) != NULL) {
		if (kf->type == kifInstanceFunction)
			basictype = true;
		else
		if (kf->type == kifIndex || CALLFUNCTION(kf)) {
			basictype = true;
			getbasefunction = true;
		}
		else
			getbasefunction = true;
	}

	//Particular case, where a different frame was provided through a frame::function call
	KifElement* kframe = kf;
	KifElement* kfunc = NULL;
	if (idframe != -1 && kf->Frame() != NULL) {
		KifElement* localframe = kf->Frame();
		kfunc = Getbasic(fullid, localframe);
		if (kfunc == NULL) {
			kframe = Getbasic(idframe, kf);
			kfunc = Getbasic(id, kframe);
			localframe->Set(fullid, kfunc, NULL);
		}
	}
	else
		//Looking if it is known as a function
		kfunc = Getbasicfunction(id, kframe);

	if (kifAllTypes[id] == true && kfunc != NULL && kfunc->type == kifFrame) {
		//This is a very specific case, where we create a new instance
		//for a frame. kfunc is the frame definition, which will be used
		//to create a new instance in Setvalue.
		KifSelf* ke = new KifSelf(this, kframe, kfunc);
		ke->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
		KifInstruction* ki = KifCreateInstruction(ke, kifINITIALISATION);
		ki->Setline(ke->Line(), kifGlobals->debuginfocurrent->currentfile);
		//if we have some parameters
		if (xn->nodes.size() == 2)
			KifBrowse(xn->nodes[1], ki);

		ke->parameter = ki;
		return ke;
	}

	if (kfunc == NULL && getbasefunction == true) {
		if (basictype == false && kifGlobals->kifMethods.find(id) != kifGlobals->kifMethods.end()) {
			if (xn->token != "haskellcall") {
				string message = KIFLIBSTRINGS[282] + name + "'";
				throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
			}
		}

		kfunc = kifGlobals->KifBaseFunctionTest(id);
	}

	if (name == KIFLIBSTRINGS[289])
		kfunc = new KifCallReturn(this, kf, id);

	if (kfunc == NULL) {
		KifElement* lv = kf->Linkedvalue();
		if (lv == NULL || lv->type != kifFrame || ((KifFrame*)lv)->declared == false) {

			string message = KIFLIBSTRINGS[282] + name + "'";
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
	}

	if (kfunc == kifTRUE)
		kfunc = NULL;

	if (idframe != -1)
		id = fullid;

	KifCallFunction* kcf = NULL;
	if (kfunc != NULL) {
		if (kfunc->type == kifVariable) {
			kfunc = kfunc->Returnvalue();
			if (kfunc != NULL && kfunc->type == kifCall)
				kcf = new KifCallFunctionCall(this, kf, id, kfunc);
			else {
				string message = KIFLIBSTRINGS[309];
				message += name;
				throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
			}
		}
		else {
			if (name == KIFLIBSTRINGS[289])
				kcf = (KifCallFunction*)kfunc;
			else
			if (kfunc->Linkedvalue() != NULL && kfunc->Linkedvalue()->type == kifCall)
				kcf = new KifCallFunctionCall(this, kf, id, kfunc);
			else {
				kfunc->Setused();
				if (kfunc->type == kifPredefined)
					kcf = new KifCallFunctionPredefined(this, kf, id, (KifPredefined*)kfunc);
				else
				if (kfunc->type == kifCallMethod)
					kcf = new KifCallFunctionExternal(this, kf, id, (KifCallMethod*)kfunc);
				else
				if (kfunc->type == kifThread)
					kcf = new KifCallFunctionThread(this, kf, id, (KifFunction*)kfunc);
			}
		}
	}

	if (kcf == NULL) {
		if (kfunc != NULL && kfunc->type == kifFunctionLambda)
			kcf = new KifCallFunctionHaskell(this, kf, id, (KifFunctionLambda*)kfunc);
		else
			kcf = new KifCallFunctionGeneral(this, kf, id, (KifFunction*)kfunc);
	}

	kcf->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);

	int minusarity = 0;
	//We check again to see if a specific function name was not overloaded
	if (basictype == true && kfunc != NULL && kfunc->type == kifPredefined) {
		if (xn->token != "haskellcall" && kf->type != kifCallFunctionHaskell) {
			kcf->Setselfinsert(true);
			kcf->parameters.push_back(kifNULL);
			minusarity = 1;
		}
	}
	else
	if (kfunc != NULL && kfunc->type == kifCallMethod)
		kcf->Setselfinsert(true);

	kf->Initvalue(kcf);
	if (xn->nodes.size() >= 2) {
		int last = 1;
		if (xn->nodes[last]->token == params) {
			ComputeParameters(xn->nodes[last], kcf);
			last++;
		}
		if (xn->nodes.size()>last) {
			if (TestKifFunctions(xn->nodes[last]->token, false)) {
				kcf->add = 2;
				KifBrowse(xn->nodes[last], kcf);
			}
		}
	}

	if (kfunc != NULL && kfunc->isFunction()) {
		//if we have a call:  var.FRAME::Function, then we record this FRAME::Function as a new function
		//within the frame...

		char ret = KifCheckParameters((KifFunction*)kfunc, kcf, true, NULL);
		if (ret == 1) {
			string message = KIFLIBSTRINGS[158];
			message += name;
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		else
		if (ret == 2) {
			string message = KIFLIBSTRINGS[310];
			message += name;
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}

	}

	if (kfunc != NULL) {
		int nbparams = kcf->parameters.size();
		unsigned int arity = 1 << nbparams;
		if (kfunc->type == kifPredefined) {
			unsigned int expected = ((KifPredefined*)kfunc)->arity;
			if ((expected&arity) != arity && kifGlobals->TestExternalArity(id, nbparams - minusarity) == false) {
				string message = KIFLIBSTRINGS[257];
				message += name;
				throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
			}
		}
		else {
			if (kfunc->type == kifCallMethod) {
				unsigned int expected = ((KifCallMethod*)kfunc)->arity;
				if ((expected&arity) != arity) {
					string message = KIFLIBSTRINGS[257];
					message += name;
					throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
				}
			}
		}
	}

	kcf->add = 0;
	return kcf;
}

KifElement* KifCode::KifParseIndexes(x_node* xn, KifElement* kf) {
	KifIndex* ki = new KifIndex(this, kf, false);
	ki->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
	kf->Initvalue(ki);
	//KifInstruction* kidx=new KifInstruction(this,ki,kifBLOC);
	KifInstruction kidx(NULL, NULL, kifBLOC);
	kidx.kifcode = this;

	if (xn->nodes[0]->token == "minus") {
		ki->signkey = true;
		KifBrowse(xn->nodes[0]->nodes[0], &kidx);
	}
	else
		KifBrowse(xn->nodes[0], &kidx);


	ki->Add(kidx.instructions[0]);
	if (xn->nodes.size() == 2)
		KifBrowse(xn->nodes[1], ki);
	return ki;
}

KifElement* KifCode::KifParseInterval(x_node* xn, KifElement* kf) {
	string ty = kf->Type();
	//if (ty!=KIFLIBSTRINGS[311] && ty!=KIFLIBSTRINGS[312]) {
	//	string message=KIFLIBSTRINGS[313]+kf->Name()+KIFLIBSTRINGS[314]+ty;
	//	throw new KifRaiseError(message,kifGlobals->current->name,line,xn->end);
	//}

	int xsz = xn->nodes.size();


	KifIndex* ki = new KifIndex(this, kf, true);
	ki->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
	kf->Initvalue(ki);

	if (TestKifFunctions(xn->nodes[xsz - 1]->token, false))
		xsz--;

	if (xsz == 1) {
		ki->interval = true;
		ki->Add(kifZERO);
		ki->Add(kifNULL);
	}
	else {
		bool beforesep = true;
		for (int i = 0; i < xsz; i++) {
			if (xn->nodes[i]->token == "sep") {
				beforesep = false;
				if (ki->key == NULL)
					ki->key = kifNULL;
				continue;
			}

			KifInstruction kidx(NULL, NULL, kifBLOC);
			kidx.kifcode = this;
			if (xn->nodes[i]->token == "minus") {
				if (beforesep)
					ki->signkey = true;
				else
					ki->signright = true;
				KifBrowse(xn->nodes[i]->nodes[0], &kidx);
			}
			else
				KifBrowse(xn->nodes[i], &kidx);
			ki->Add(kidx.instructions[0]);
		}
		if (!beforesep && ki->right == NULL)
			ki->right = kifNULL;
	}

	//If we have a call
	if (xsz != xn->nodes.size()) {
		ki->interval = false;
		KifBrowse(xn->nodes[xsz], ki);
		ki->interval = true;
	}

	return ki;
}

KifElement* KifCode::KifParseOperator(x_node* xn, KifElement* kf) {
	KifOperator op = kifGlobals->kifOperators[xn->value];
	kf->Set(op, kf, NULL);
	return kf;
}

KifElement* KifCode::KifParseBloc(x_node* xn, KifElement* kf) {
	if (kf->Action() != kifBLOCBOOLEAN) {
		KifInstruction* ki = KifCreateInstruction(NULL, kifBLOC);
		ki->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);

		for (size_t i = 0; i<xn->nodes.size(); i++)
			KifBrowse(xn->nodes[i], ki);

		if (ki->action == kifBLOC && ki->instructions.size() == 1) {
			KifElement* ke = ki->instructions[0];
			if (CALLFUNCTION(ke) || ke->isInstruction()) {
				kf->Add(ke);
				ki->Release();
				return ke;
			}
		}
		else
			ki = CloningInstruction(ki);

		kf->Add(ki);
		return ki;
	}

	for (size_t i = 0; i<xn->nodes.size(); i++)
		KifBrowse(xn->nodes[i], kf);
	return kf;
}

KifElement* KifCode::KifParseComparisonPredicate(x_node* xn, KifElement* kf) {
	//The first parameter is the operator
	KifOperator op = kifGlobals->kifOperators[xn->nodes[1]->value];
	KifElement* ki = KifCreateInstruction(kf, op);
	for (size_t i = 0; i<xn->nodes.size(); i++)
		KifBrowse(xn->nodes[i], ki);
	return ki;
}

KifElement* KifCode::KifParseComparison(x_node* xn, KifElement* kf) {
	//The first parameter is the operator
	KifElement* ki = kf;
	KifOperator op = kifGlobals->kifOperators[xn->nodes[0]->value];
	if (kf->Action() == kifBLOCBOOLEAN || kf->Action() == kifBLOC)
		ki->Setaction(op);
	else
	if (kf->Action() >= kifPLUS && kf->Action() <= kifINTERSECTION) {
		KifElement* kparent = kf->Parent();
		while (kparent != NULL) {
			if (kparent->Action() == kifBLOCBOOLEAN || kparent->Action() == kifBLOC) {
				kparent->Setaction(op);
				ki = kparent;
				break;
			}
			kparent = kparent->Parent();
		}
		if (kparent == NULL) {
			kparent = kf->Parent();
			ki = KifCreateInstruction(kparent, op);
		}
	}
	else
	if (kf->Action() == kifPREDICATEELEMENT) {
		ki = KifCreateInstruction(NULL, op);
		((KifInstruction*)ki)->instructions.push_back(((KifPredicateRuleElement*)kf)->instructions[0]);
		((KifPredicateRuleElement*)kf)->instructions[0] = ki;
	}
	else
		ki = KifCreateInstruction(kf, op);
	//ki->Setline(linereference+GetCurrentLine(0,xn),kifGlobals->debuginfocurrent->currentfile);
	for (size_t i = 0; i<xn->nodes.size(); i++)
		KifBrowse(xn->nodes[i], ki);

	if (ki->isInstruction() && ki->InstructionSize() == 2 && (ki->Instruction(1)->Basevalue() || ki->Instruction(1)->Instancetype() == kinConstant)) {
		//Then we invert the comparison to force with the const to compare with itself
		//We do not want 0==empty for instance to be true...
		KifInstruction* kinst = (KifInstruction*)ki;
		kf = kinst->instructions[1];
		if (kf->Name() == idnoelement) {
			kinst->instructions.pop_back();
			kinst->instructions.insert(kinst->instructions.begin(), kf);
		}
	}

	return ki;
}

KifElement* KifCode::KifParsePlusPlus(x_node* xn, KifElement* kf) {
	KifOperator op = kifGlobals->kifOperators[xn->value];
	kf->Setaction(op);
	return kf;
}


KifElement* KifCode::KifParseOperation(x_node* xn, KifElement* kf) {
	//The first parameter is the operator	
	KifOperator op = kifGlobals->kifOperators[xn->nodes[0]->value];
	//The second parameter is the rest of the operation
	//kf is the TOP instruction
	KifInstruction* ki;
	if (kf->Action() == kifBLOC) {
		//We are in a new bloc, which is our current element
		//In this case, we create a new level
		ki = KifCreateInstruction(NULL, op);
		ki->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
		//It becomes the new element
		ki->Add(kf->Instruction(0));
		kf->Addinstruction(0, ki);
		ki->Addparent(kf);
		KifBrowse(xn->nodes[1], ki);
		ki->Subcontext(true);
		return ki;
	}

	if (kf->Action() == op) {
		KifBrowse(xn->nodes[1], kf);
		return kf;
	}

	//In this case, the operator is not the same
	//we still have two cases: if it is
	//if it is a PLUS or a MINUS, we reset the top node with the new information
	//It has to be in the middle of an operation
	if (kifGlobals->kifOperatorMath[kf->Action()] == true) {
		if (op == kifPLUS || op == kifMINUS || op == kifMERGE) {
			KifElement* kloop = kf;
			KifOperator kact = kloop->Action();
			while (kloop != NULL && kifGlobals->kifOperatorMath[kact] == true && kact != kifPLUS && kact != kifMINUS && kact != kifMERGE) {
				kloop = kloop->Parent();
				if (kloop != NULL)
					kact = kloop->Action();
			}

			if (kloop == NULL || kifGlobals->kifOperatorMath[kloop->Action()] == false)
				kloop = kf;
			else {
				if (kloop->Action() == op) {
					KifBrowse(xn->nodes[1], kloop);
					return kloop;
				}
			}

			ki = KifCreateInstruction(NULL, kloop->Action());
			ki->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
			for (int i = 0; i<kloop->InstructionSize(); i++)
				ki->instructions.push_back(kloop->Instruction(i));
			kloop->Setaction(op);
			kloop->InstructionClear();
			kloop->Addinstruction(0, ki);
			ki->Addparent(kf);
			KifBrowse(xn->nodes[1], kloop);
			return kloop;
		}
	}

	//we create a new level
	ki = KifCreateInstruction(NULL, op);
	ki->Add(kf->Lastinstruction());
	kf->Addinstruction(kf->InstructionSize() - 1, ki);
	ki->Addparent(kf);
	KifBrowse(xn->nodes[1], ki);

	//we can merge in this case the content of ki into kf
	if (ki->Action() == kf->Action()) {
		int sz = kf->InstructionSize() - 1;
		kf->InstructionRemove(sz);
		for (int i = 0; i<ki->InstructionSize(); i++)
			kf->Addinstruction(i + sz, ki->Instruction(i));
		ki->Release();
	}
	return kf;
}

KifElement* KifCode::KifParseOperationIn(x_node* xn, KifElement* kf) {
	//The first parameter is the operator
	KifInstruction* kinst;
	KifOperator kcurrentop = kifGlobals->kifOperators[xn->nodes[0]->token];
	if (kf->Action() == kifBLOCBOOLEAN) {
		kinst = (KifInstruction*)kf;
		kinst->action = kcurrentop;
	}
	else {
		KifElement* last;
		//In this case, we replace the previous last element in kf with this one
		KifOperator kact = kf->Action();
		if (kact == kifAFFECTATION || kact == kifINITIALISATION || (kact >= kifPLUSEQU && kact <= kifINTERSECTIONEQU)) {
			last = kf->InstructionRemoveLast();
			kinst = KifCreateInstruction(kf, kcurrentop);
			kinst->instructions.push_back(last);
			kinst->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
		}
		else {
			if (kf->Parent() != NULL) {
				//In this case, we need to keep the full instruction			
				last = kf->Parent()->InstructionRemoveLast();
				kinst = KifCreateInstruction(kf->Parent(), kcurrentop);
				kinst->instructions.push_back(kf);
				kf->Addparent(kinst);
				kinst->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
			}
			else {
				last = kf->InstructionRemoveLast();
				kinst = KifCreateInstruction(kf, kcurrentop);
				if (last != NULL)
					kinst->instructions.push_back(last);
				kinst->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
			}
		}
	}

	//if (kinst->action == kifNOTIN) {
	//	kinst->action = kifIN;
	//	kinst->negation = true;
	//}

	KifBrowse(xn->nodes[1], kinst);
	if (xn->nodes.size() == 3) {
		KifElement* kfunc = NULL;
		string& name = xn->nodes[2]->value;
		short id = KifNewId(name);
		kfunc = Getbasic(id, kf);
		//We have a WITH description
		if (kfunc == NULL) {
			string message = KIFLIBSTRINGS[178] + name;
			long line = GetCurrentLine(0, xn);
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		if (kfunc->isFunction())
			kinst->Add(kfunc);
		else {
			if (!kfunc->isCall()) {
				string message = KIFLIBSTRINGS[282] + kifGlobals->kifIdString[kfunc->Name()] + "'";
				throw new KifRaiseError(message, kifGlobals->current->name, kifGlobals->current_start, kifGlobals->current_end);
			}
			KifInstance* ki;
			int idfunc = kfunc->Name();
			if (kifGlobals->kconstants->declarations.check(idfunc)) {
				ki = new KifInstanceConstant(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
				ki->Setinstancetype(kinConstant);
			}
			else {
				if (kfunc->type == kifFunctionDeclaration) {
					ki = new KifInstanceInFunction(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
					ki->Setinstancetype(kinFunction);
				}
				else {
					if (kfunc->type == kifFrameDeclaration) {
						KifElement* currentdeclaration = FindFrameForSequence(kfunc, xn, -1);
						if (currentdeclaration == kstart) {
							ki = new KifInstanceGlobal(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
							ki->Setinstancetype(kinGlobal);
						}
						else {
							ki = new KifInstanceInFrame(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
							ki->Setinstancetype(kinFrame);
						}
					}
					else
						ki = new KifInstance(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
				}
			}
			kinst->Add(ki);
		}
	}
	return kinst;
}

KifElement* KifCode::KifParseSubFunc(x_node* xn, KifElement* kf) {
	string name = xn->nodes[0]->nodes[0]->value;
	if (xn->nodes[0]->nodes[0]->nodes.size() != 1)
		name = xn->nodes[0]->nodes[0]->nodes[1]->value;
	short id = KifNewId(name);
	KifElement* declaration = kf->Declarationfunction(id);
	if (declaration == NULL)
	if CALLFUNCTION(kf)
		declaration = kifGlobals->KifBaseFunctionTest(id);

	if (declaration == NULL || declaration->Private()) {
		KifElement* lv = kf->Linkedvalue();
		if (lv != NULL && lv->type == kifFrame && ((KifFrame*)lv)->declared == true)
			return KifBrowse(xn->nodes[0], kf);
		string message = KIFLIBSTRINGS[282] + name + "'";
		throw new KifRaiseError(message, kifGlobals->current->name, kifGlobals->current_start, kifGlobals->current_end);
	}


	derecording rec(this);
	declaration = KifBrowse(xn->nodes[0], kf);
	return declaration;
}

KifElement* KifCode::KifParseSubVar(x_node* xn, KifElement* kf) {
	hmap<long, KifElement*> localinstances = instances;
	string& name = xn->nodes[0]->nodes[0]->value;
	short id = KifNewId(name);
	KifElement* declaration = kf->Declaration(id);
	if (declaration == NULL || declaration->Private()) {
		string message = KIFLIBSTRINGS[284] + name + "'";
		throw new KifRaiseError(message, kifGlobals->current->name, kifGlobals->current_start, kifGlobals->current_end);
	}

	derecording rec(this);
	declaration = KifBrowse(xn->nodes[0], kf);
	return declaration;
}


KifElement* KifCode::KifParseNegativeVariable(x_node* xn, KifElement* kf) {

	KifInstruction* ki = KifCreateInstruction(kf, kifMULTIPLY);
	ki->Add(kifMINUSONE);
	KifBrowse(xn->nodes[0], ki);
	return ki;
}

//A variable is a complex structure involving: expression, crochets
KifElement* KifCode::KifParseVariable(x_node* xn, KifElement* kf) {
	string name = xn->nodes[0]->value;
	short id = KifNewId(name);

	//Used in 
	if (id == iduniversal) {
		kf->Add(kifUNIVERSAL);
		return kifUNIVERSAL;
	}

	KifElement* declaration = NULL;
	long line = GetCurrentLine(0, xn);
	if (kf->type == kifEnum) {
		KifVariableDeclaration* var = Createstaticvariable(name, kf, xn);
		//then we create its value
		return new KifBaseInteger(this, var, ((KifEnum*)kf)->values.size() - 1);
	}

	//specific case, this is itself...
	if (name == KIFLIBSTRINGS[315]) {
		if (xn->nodes.size() != 1) {
			string message = KIFLIBSTRINGS[316];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		kf->Add(new KifThis(this, NULL));
		return kf;
	}

	declaration = Getbasic(id, kf);

	if (declaration == NULL) {
		string lname = s_to_lower(name);
		short idlname = KifNewId(lname);
		declaration = Getbasic(idlname, kf);
		if (declaration == NULL || declaration->XIP() == false) {
			string message = KIFLIBSTRINGS[284] + name + "'";
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
	}

	if (!declaration->Variable()) {
		if (xn->nodes.size() == 1 && declaration->isFunction()) {
			KifElement* ke = new KifSelf(this, kf, declaration);
			ke->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
			return ke;
		}
		if (declaration->type != kifVariable) {
			string message = KIFLIBSTRINGS[284] + name + "'";
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
	}
	KifOrigin org = kinUnknown;
	if (kifGlobals->kconstants->declarations.check(id))
		org = kinConstant;
	else {
		if (declaration->type == kifFunctionDeclaration)
			org = kinFunction;
		else {
			if (declaration->type == kifFrameDeclaration) {
				KifElement* currentdeclaration = FindFrameForSequence(declaration, xn, -1);
				if (currentdeclaration == kstart)
					org = kinGlobal;
				else
					org = kinFrame;
			}
		}
	}

	KifElement* ki;
	if (declaration->isValidVariable()) {
		if (xn->nodes.size()>1 && TestKifFunctions(xn->nodes[1]->token, true))  {
			ki = new KifInstanceFunction(this, kf, id, (KifCommonVariable*)declaration);
			ki->Setinstancetype(org);
			ki->Setstringvalue(xn->value);
			ki->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
		}
		else {
			long keyid = 1;
			keyid <<= org + 16;
			keyid |= id;
			if (recordinstance && xn->nodes.size() == 1 && instances.find(keyid) != instances.end()) {
				ki = instances[keyid];
				kf->Add(ki);
			}
			else {
				switch (org) {
				case kinFunction:
					ki = new KifInstanceInFunction(this, kf, id, (KifCommonVariable*)declaration);
					break;
				case kinFrame:
					ki = new KifInstanceInFrame(this, kf, id, (KifCommonVariable*)declaration);
					break;
				case kinGlobal:
					ki = new KifInstanceGlobal(this, kf, id, (KifCommonVariable*)declaration);
					break;
				case kinConstant:
					ki = new KifInstanceConstant(this, kf, id, (KifCommonVariable*)declaration);
					break;
				default:
					ki = new KifInstance(this, kf, id, (KifCommonVariable*)declaration);
				}

				ki->Setinstancetype(org);
				ki->Setstringvalue(xn->value);
				ki->Setline(linereference + GetCurrentLine(0, xn->nodes[0]), kifGlobals->debuginfocurrent->currentfile);
				if (xn->nodes.size() == 1)
					instances[keyid] = ki;
			}
		}
	}
	else {
		string message = KIFLIBSTRINGS[317] + name + "'";
		throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
	}

	kf->Initvalue(ki);
	KifElement* ret = NULL;
	for (size_t i = 1; i<xn->nodes.size(); i++) {
		KifElement* retloc = KifBrowse(xn->nodes[i], ki);
		if (retloc != NULL)
			ret = retloc;
	}
	if (ret == NULL)
		ki->Setself();
	return ki;
}

KifElement* KifCode::KifParseIncrement(x_node* xn, KifElement* kf) {
	KifInstruction* ki = KifCreateInstruction(kf, kifNONE);
	ki->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
	for (size_t i = 0; i<xn->nodes.size(); i++)
		KifBrowse(xn->nodes[i], ki);
	return ki;
}

KifElement* KifCode::KifParseAffectation(x_node* xn, KifElement* kf) {
	KifInstruction* ki = KifCreateInstruction(NULL, kifAFFECTATION);
	ki->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);


	for (size_t i = 0; i<xn->nodes.size(); i++)
		KifBrowse(xn->nodes[i], ki);

	if (ki->Action() != kifAFFECTATION) {
		KifInstruction* k = KifCreateInstruction(kf, ki->Action());
		ki->Clone(k);
		ki->Release();
		ki = k;
	}
	else
		kf->Add(ki);
	return ki;
}


//Hexa-decimal value
KifElement* KifCode::KifParseAXNumber(x_node* xn, KifElement* kf) {
	string& name = xn->value;
	KifElement* kv;
	KIFLONG v = conversionintegerhexa(STR(name));
	if (v> ((1 << 8) - 1))
		kv = new KifBaseLong(this, kf, v);
	else
		kv = new KifBaseInteger(this, kf, v);
	kv->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
	return kv;
}

bool isLong(KIFLONG v) {
	KIFLONG u = abs((KIFLONG)v) >> 32;
	if (!u)
		return false;
	return true;
}

KifElement* KifCode::KifParseANumber(x_node* xn, KifElement* kf) {
	string& name = xn->value;
	KifElement* kv;
	if (name.find(".") == -1) {
		KIFLONG v = conversionintegerhexa(STR(name));
		if (isLong(v))
			kv = new KifBaseLong(this, kf, v);
		else
			kv = new KifBaseInteger(this, kf, v);
	}
	else
		kv = new KifBaseFloat(this, kf, convertfloat(STR(name)));

	kv->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
	return kv;
}

KifElement* KifCode::KifParseABoolean(x_node* xn, KifElement* kf) {
	if (xn->value == KIFLIBSTRINGS[318]) {
		kf->Add(kifTRUE);
		return kifTRUE;
	}
	kf->Add(kifFALSE);
	return kifFALSE;
}

//-----------------------------------------------------------------------------------------------


void KifEvaluateMetaString(string& thestr, string sub) {
	static string reg("[\\\\?");
	static vector<int> vectr;
	static automate metacodes(USTR(reg), (void*)1);

	if (sub.find("\\") == -1) {
		thestr = sub;
		return;
	}

	size_t i = 0, previous = 0;
	string s;
	metacodes.rechercheindexes(USTR(sub), vectr);

	if (vectr.size() == 0) {
		thestr = sub;
		return;
	}

	for (i = 0; i<vectr.size(); i += 2) {
		if (previous<vectr[i])
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
}

KifElement* KifCode::KifParseAString(x_node* xn, KifElement* kf) {
	string thestr;

	if (xn->token == "astring" || xn->token == "punct")
		thestr = xn->value;
	else
	if (xn->token == "astringdouble")
		KifEvaluateMetaString(thestr, xn->value.substr(1, xn->value.size() - 2));
	else
		thestr = xn->value.substr(1, xn->value.size() - 2);

	KifString* kv = new KifBaseString(this, kf, thestr);

	kv->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
	return kv;
}
//-----------------------------------------------------------------------------------------------

KifElement* KifCode::FindFrameForSequence(KifElement* ktop, x_node* xn, short id) {
	KifType currenttype = ktop->type;
	KifElement* kbefore = ktop;
	if (currenttype == kifFrameDeclaration) {
		kbefore = ktop->Frame();
		currenttype = kbefore->type;
	}
	if (currenttype == kifInstructionSEQUENCE) {
		//we check if a variable of the same name was not already declared at a previous level...		
		int i = kifGlobals->THR[0]->threadstacks.size() - 2;
		//We look for the first non SEQUENCE bloc, which might be embedded one into the others... 
		while (i >= 0 && kifGlobals->THR[0]->threadstacks[i]->type == kifInstructionSEQUENCE)
			i--;
		if (i >= 0)
			kbefore = kifGlobals->THR[0]->threadstacks[i];
		if (kbefore != NULL)  {
			//This level defines the type of this variable as a function or a domain related variable...
			if (id != -1 && kbefore->Declaration(id) != NULL) {
				long line = GetCurrentLine(0, xn);
				string message = KIFLIBSTRINGS[319];
				message += kifGlobals->kifIdString[id];
				message += KIFLIBSTRINGS[320];
				throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
			}
		}
	}
	return kbefore;
}

KifElement* KifCode::CreateVariable(x_node* xn, short id, KifElement* kf, KifInstructionType instype) {
	KifElement* ktop = kifGlobals->Top();
	KifElement* var;
	long line = GetCurrentLine(0, xn);
	//If it is a global variable, instance is unique
	if (kf->Declaration(id) != NULL) {
		string message = KIFLIBSTRINGS[319];
		message += kifGlobals->kifIdString[id];
		message += KIFLIBSTRINGS[320];
		throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
	}

	KifType currenttype = kifVOID;
	if (ktop != NULL) {
		KifElement* kbefore = FindFrameForSequence(ktop, xn, id);
		currenttype = kbefore->type;
	}

	if (kifGlobals->common == true && currenttype != kifFrame) {
		string message = KIFLIBSTRINGS[321];
		message += kifGlobals->kifIdString[id];
		throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
	}

	if (kifGlobals->constant)
		var = new KifStaticVariable(this, kf, id);
	else
	if (kifGlobals->through)
		var = new KifThroughVariable(this, kf, id);
	else
	if (currenttype == kifVOID)
		var = new KifVariableDeclaration(this, kf, id);
	else
		//Otherwise, we keep a track of that specific frame
	if (currenttype == kifFrame) {
		if (kifGlobals->common)
			var = new KifFrameDeclarationCommon(this, kf, id, ktop);
		else
			var = new KifFrameDeclaration(this, kf, id, ktop);
	}
	else
		var = new KifFunctionDeclaration(this, kf, id, ktop);

	var->Setprivate(kifGlobals->prive);

	var->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
	if (xn->nodes.size() >= 3) {
		for (int nxt = 2; nxt<xn->nodes.size(); nxt++) {
			if (xn->nodes[nxt]->token == "depend") {
				string funcname = xn->nodes[nxt]->nodes[0]->value;
				short idf = KifNewId(funcname);
				KifElement* kfunc = NULL;
				kfunc = Getbasic(idf, kf);
				//We have a WITH description
				if (kfunc == NULL) {
					string message = KIFLIBSTRINGS[178] + funcname;
					throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
				}
				if (kfunc->isFunction())
					var->Setfunction(kfunc);
				else {
					if (!kfunc->isCall()) {
						string message = KIFLIBSTRINGS[282] + kifGlobals->kifIdString[kfunc->Name()] + "'";
						throw new KifRaiseError(message, kifGlobals->current->name, kifGlobals->current_start, kifGlobals->current_end);
					}
					KifInstance* ki;
					int idfunc = kfunc->Name();
					if (kifGlobals->kconstants->declarations.check(idfunc)) {
						ki = new KifInstanceConstant(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
						ki->Setinstancetype(kinConstant);
					}
					else {
						if (kfunc->type == kifFunctionDeclaration) {
							ki = new KifInstanceInFunction(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
							ki->Setinstancetype(kinFunction);
						}
						else {
							if (kfunc->type == kifFrameDeclaration) {
								KifElement* currentdeclaration = FindFrameForSequence(kfunc, xn, -1);
								if (currentdeclaration == kstart) {
									ki = new KifInstanceGlobal(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
									ki->Setinstancetype(kinGlobal);
								}
								else {
									ki = new KifInstanceInFrame(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
									ki->Setinstancetype(kinFrame);
								}
							}
							else
								ki = new KifInstance(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
						}
					}
					var->Setfunction(ki);
				}
			}
			else {
				if (xn->nodes[nxt]->token != "declarationlist") {
					if (instype == kifENUM) {
						KifInstruction ki(NULL, var, kifINITIALISATION);
						ki.instructiontype = instype;
						KifBrowse(xn->nodes[nxt], &ki);
					}
					else {
						KifInstruction* ki = KifCreateInstruction(var, kifINITIALISATION);
						ki->Setline(linereference + GetCurrentLine(0, xn->nodes[nxt]), kifGlobals->debuginfocurrent->currentfile);
						var->Setinitialisation(ki);
						ki->instructiontype = instype;
						KifBrowse(xn->nodes[nxt], ki);

						//we cannot keep it as a value...
						ki->instructiontype = kifNOTYPE;
					}
				}
			}
		}
	}

	return var;
}

KifElement* KifCode::KifParseVarFrame(x_node* xn, KifElement* kf) {
	string& type = xn->nodes[0]->value;
	short id = KifNewId(type);
	string& name = xn->nodes[1]->value;
	short idname = KifNewId(name);
	KifElement* ke;
	ke = Getframe(id, kf);
	if (ke == NULL) {
		string message = KIFLIBSTRINGS[322] + type + "'";
		long line = GetCurrentLine(0, xn);
		throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
	}
	KifElement* var = CreateVariable(xn, idname, kf, kifNOTYPE);
	var->SetVariableValue(ke);
	//In the case of a frame element defined within a frame, we add the postpone flag
	//which delays if necessary, the call to the _initial frame function for that variable frame...
	if (kf != kstart && kf->type == kifFrame)
		var->Setpostpone(true);
	kf->Set(idname, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarSelf(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifSelf* kv = new KifSelf(this, var, kifNULL);
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarCall(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifCall* kv = new KifCall(this, var, NULL);
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarDate(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifDate* kv = new KifDate(this, var);

	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTime(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTime* kv = new KifTime(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}


KifElement* KifCode::KifParseVarBit(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifBit* kv = new KifBit(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarBits(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifBits* kv = new KifBits(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}


KifElement* KifCode::KifParseVarFraction(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifFraction* kv = new KifFraction(this, var, 0, 1, true);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarKifKiF(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifKiF* kv = new KifKiF(this, var);
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarLong(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLINTEGER);
	KifLong* kv = new KifLong(this, var, 0);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarShort(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLINTEGER);
	KifShort* kv = new KifShort(this, var, 0);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarByte(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifByte* kv = new KifByte(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarInt(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLINTEGER);
	KifInteger* kv = new KifBaseInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}
//--------------------------------------------------------------------------------------------------
KifElement* KifCode::KifParseVarString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLSTRING);
	KifString* kv = new KifBaseString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLSTRING);
	KifBaseUString* kv = new KifBaseUString(this, var, 0);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarRawString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLSTRING);
	KifRawString* kv = new KifRawString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}
//--------------------------------------------------------------------------------------------------
KifElement* KifCode::KifParseVarPredicate(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifElement* kv;
	if (xn->nodes[0]->value == KIFLIBSTRINGS[323])
		kv = new KifPredicateVar(this, var, id);
	else {
		kv = new KifPredicateFunction(this, var, var->Robfunction(kf), id);
		predicates[id] = (KifPredicateFunction*)kv;
	}

	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarPredicateTerm(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	terms[id] = xn->nodes[1]->value;
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifPredicateTerm* kv = new KifPredicateTerm(this, var, id, terms[id]);
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLFLOAT);
	KifFloat* kv;
	kv = new KifBaseFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarLoopString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLSTRING);
	KifLoopString* kv;
	kv = new KifLoopString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarLoopUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLSTRING);
	KifLoopUString* kv;
	kv = new KifLoopUString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarLoopBoolean(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLINTEGER);
	KifLoopBoolean* kv;
	kv = new KifLoopBoolean(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}


KifElement* KifCode::KifParseVarLoopInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLINTEGER);
	KifLoopInteger* kv;
	kv = new KifLoopInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarLoopFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifFULLFLOAT);
	KifLoopFloat* kv;
	kv = new KifLoopFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTuple(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTuple* kv = new KifTuple(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}


KifElement* KifCode::KifParseVarBoolean(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifBoolean* kv = new KifBoolean(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarFile(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifFile* kv = new KifFile(this, var);
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarFileWide(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifFileWide* kv = new KifFileWide(this, var);
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarVector(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifVector* kv;
	if (kifGlobals->evaluation) {
		kv = new KifVector(this, NULL);
		var->Add(kv);
	}
	else
		kv = new KifVector(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarTable(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTable* kv;
	if (kifGlobals->evaluation) {
		kv = new KifTable(this, NULL);
		var->Add(kv);
	}
	else
		kv = new KifTable(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarEnum(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifENUM);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarList(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifList* kv;
	if (kifGlobals->evaluation) {
		kv = new KifList(this, NULL);
		var->Add(kv);
	}
	else
		kv = new KifList(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarVectorString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifVectorString* kv = new KifVectorString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarVectorUString(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifVectorUString* kv = new KifVectorUString(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarVectorInteger(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifVectorInteger* kv = new KifVectorInteger(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarVectorByte(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifVectorByte* kv = new KifVectorByte(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarVectorFloat(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifVectorFloat* kv = new KifVectorFloat(this, var);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}


KifElement* KifCode::KifParseVarTree(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifTree* kv = new KifTree(this, var, kifNULL);
	if (!kv->testfunction(var->Returnfunction()))
		Returnerror(KIFLIBSTRINGS[281]);

	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}


KifElement* KifCode::KifParseVarIterator(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifIterator* kv = new KifIterator(this, var, false);
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseVarRIterator(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifIterator* kv = new KifIterator(this, var, true);
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

KifElement* KifCode::KifParseMultiDeclaration(x_node* xn, KifElement* kf) {
	string& type = xn->nodes[0]->value;
	bool oldprive = kifGlobals->prive;
	bool oldcommon = kifGlobals->common;
	bool oldconstant = kifGlobals->constant;
	if (xn->nodes[0]->nodes.size() && xn->nodes[0]->nodes[0]->token == "feature") {
		string& s = xn->nodes[0]->nodes[0]->nodes[0]->value;
		if (s == KIFLIBSTRINGS[324]) {
			kifGlobals->prive = true;
			if (xn->nodes[0]->nodes[0]->nodes.size() != 1)
				kifGlobals->common = true;
		}
		else
		if (s == KIFLIBSTRINGS[325])
			kifGlobals->common = true;
		else
		if (s == KIFLIBSTRINGS[326])
			kifGlobals->constant = true;
		type = xn->nodes[0]->nodes[1]->value;
	}

	KifElement* element = NULL;
	int last = xn->nodes.size() - 1;
	bool recall = false;

	if (xn->nodes[last]->token == "declarationlist") {
		x_node* xnew = new x_node();
		xnew->token = xn->nodes[0]->token;
		xnew->value = type;
		xn->nodes[last]->nodes.insert(xn->nodes[last]->nodes.begin(), xnew);
		recall = true;
	}

	if (kifGlobals->kifTypes.find(type) != kifGlobals->kifTypes.end()) {
		KifParseElement kpe = kifGlobals->kifTypes[type];
		element = (this->*kpe)(xn, kf);
	}
	else
	if (kifGlobals->kifExternalTypes.find(type) != kifGlobals->kifExternalTypes.end()) {
		KifExternalParse kext = kifGlobals->kifExternalTypes[type];
		element = (*kext)(this, xn, kf);
	}
	else
		KifParseVarFrame(xn, kf);

	if (recall)
		KifParseMultiDeclaration(xn->nodes[last], kf);
	kifGlobals->prive = oldprive;
	kifGlobals->common = oldcommon;
	kifGlobals->constant = oldconstant;

	if (kifGlobals->THR[0]->errorptr != NULL) {
		string message = kifGlobals->THR[0]->errorptr->String();
		long line = GetCurrentLine(0, xn);
		throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
	}
	return element;
}


KifElement* KifCode::KifParseDeclaration(x_node* xn, KifElement* kf) {
	string& type = xn->nodes[0]->value;
	bool oldprive = kifGlobals->prive;
	if (xn->nodes[0]->nodes[0]->token == "private") {
		kifGlobals->prive = true;
		type = xn->nodes[0]->nodes[1]->value;
	}
	KifElement* element = NULL;
	if (kifGlobals->kifTypes.find(type) != kifGlobals->kifTypes.end()) {
		KifParseElement kpe = kifGlobals->kifTypes[type];
		element = (this->*kpe)(xn, kf);
	}
	else
	if (kifGlobals->kifExternalTypes.find(type) != kifGlobals->kifExternalTypes.end()) {
		KifExternalParse kext = kifGlobals->kifExternalTypes[type];
		element = (*kext)(this, xn, kf);
	}
	else
		KifParseVarFrame(xn, kf);
	kifGlobals->prive = oldprive;
	return element;
}

KifElement* KifCode::KifParseList(x_node* xn, KifElement* kf) {
	//KifInstruction* kbloc=new KifInstruction(this,kf,kifBLOC);
	if (kf->type == kifEnum) {
		//In this case, we create for each value, a variable, whose value depends on its position
		//in the enumeration
		KifBrowse(xn->nodes[0], kf);
		if (xn->nodes.size() == 2)
			KifBrowse(xn->nodes[1], kf);
		return kf;
	}


	if (xn->token == "merging" || xn->token == "valtail") {
		//we create a sub-vector
		KifBaseVector* kbv = new KifBaseVectorMerge(this, NULL);
		KifBrowse(xn->nodes[0], kbv);
		kf->Push(kbv);
		return kbv;
	}

	KifInstruction kbloc(NULL, NULL, kifBLOC);
	kbloc.kifcode = this;
	KifBrowse(xn->nodes[0], &kbloc);
	KifElement* ke = kbloc.instructions[0];
	kf->Push(ke);
	if (xn->nodes.size() == 2)
		KifBrowse(xn->nodes[1], kf);
	return ke;
}


KifElement* KifCode::KifParseIntentionDoubleVector(x_node* xn, KifElement* kf) {
	//Different types of expressions: [..x,y]
	KifInfinitiveVector* kinfvect;
	if (xn->nodes[0]->token == "intentionsep") {
		kinfvect = new KifInfinitiveVector(this, kf, -1);
		kinfvect->compute = true;
		//The initial value
		KifBrowse(xn->nodes[2], kinfvect);
		//The next value from which the step will be computed...
		KifBrowse(xn->nodes[1], kinfvect);
		return kinfvect;
	}

	//[x,y..]
	if (xn->nodes.back()->token == "intentionsep") {
		kinfvect = new KifInfinitiveVector(this, kf, 1);
		kinfvect->compute = true;
		//The initial value
		KifBrowse(xn->nodes[0], kinfvect);
		//The next value from which the step will be computed...
		KifBrowse(xn->nodes[1], kinfvect);
		return kinfvect;
	}

	//[x,y..z]
	x_node* nop = new x_node;
	nop->start = xn->start;
	nop->end = xn->end;
	nop->token = "regularcall";
	x_node* nfunc = new x_node;
	nop->nodes.push_back(nfunc);

	nfunc->token = "functioncall";
	nfunc->value = KIFLIBSTRINGS[327];
	nfunc->start = xn->start;
	nfunc->end = xn->end;
	x_node* nfuncsub = new x_node;
	nfuncsub->token = "word";
	nfuncsub->value = KIFLIBSTRINGS[327];
	nfunc->nodes.push_back(nfuncsub);

	x_node* param = new x_node;
	param->token = "parameters";
	param->nodes.push_back(xn->nodes[0]);
	nop->nodes.push_back(param);

	x_node* param2 = new x_node;
	param2->token = "parameters";
	param->nodes.push_back(param2);
	param2->nodes.push_back(xn->nodes[3]);

	KifElement* kret = KifBrowse(nop, kf);
	kret->Setadd(1);

	KifInstruction* ki = KifCreateInstruction(kret, kifMINUS);
	KifBrowse(xn->nodes[1], ki);
	int idord = KifNewId(KIFLIBSTRINGS[328]);
	KifElement* kfunc;
	KifCallFunctionPredefined* kcf;
	if (ki->instructions[0]->aString()) {
		kfunc = kifGlobals->kifBaseFunctions[idord];
		kcf = new KifCallFunctionPredefined(this, NULL, idord, (KifPredefined*)kfunc);
		kcf->parameters.push_back(ki->instructions[0]);
		ki->instructions[0] = kcf;
	}
	KifBrowse(xn->nodes[0], ki);
	if (ki->instructions[1]->aString()) {
		kfunc = kifGlobals->kifBaseFunctions[idord];
		kcf = new KifCallFunctionPredefined(this, NULL, idord, (KifPredefined*)kfunc);
		kcf->parameters.push_back(ki->instructions[1]);
		ki->instructions[1] = kcf;
	}

	param->nodes.clear();
	param2->nodes.clear();
	param->nodes.push_back(param2);
	delete nop;
	return kf;

}

KifElement* KifCode::KifParseIntentionVector(x_node* xn, KifElement* kf) {
	//We will rewrite this instruction into a call to KIFLIBSTRINGS[327]...
	x_node* nstep = NULL;
	if (xn->nodes.back()->token == "step") {
		nstep = xn->nodes.back();
		xn->nodes.pop_back();
	}

	if (xn->nodes.size() == 3) {
		x_node* nop = new x_node;
		nop->start = xn->start;
		nop->end = xn->end;
		nop->token = "regularcall";
		x_node* nfunc = new x_node;
		nop->nodes.push_back(nfunc);

		nfunc->token = "functioncall";
		nfunc->value = KIFLIBSTRINGS[327];
		nfunc->start = xn->start;
		nfunc->end = xn->end;
		x_node* nfuncsub = new x_node;
		nfuncsub->token = "word";
		nfuncsub->value = KIFLIBSTRINGS[327];
		nfunc->nodes.push_back(nfuncsub);

		x_node* param = new x_node;
		param->token = "parameters";
		param->nodes.push_back(xn->nodes[0]);
		nop->nodes.push_back(param);

		x_node* param2 = new x_node;
		param2->token = "parameters";
		param->nodes.push_back(param2);
		param2->nodes.push_back(xn->nodes[2]);
		x_node* param3 = NULL;
		if (nstep != NULL) {
			//we add the step
			param3 = new x_node;
			param3->token = "parameters";
			param3->nodes.push_back(nstep);
			param2->nodes.push_back(param3);
		}

		KifBrowse(nop, kf);
		param->nodes.clear();
		param2->nodes.clear();
		param->nodes.push_back(param2);
		if (param3 != NULL)
			param2->nodes.push_back(param3);
		delete nop;
		return kf;
	}

	KifElement* kinfvect;
	if (xn->nodes[0]->token == "intentionsep") {
		kinfvect = new KifInfinitiveVector(this, kf, -1);
		KifBrowse(xn->nodes[1], kinfvect);
	}
	else {
		kinfvect = new KifInfinitiveVector(this, kf, 1);
		KifBrowse(xn->nodes[0], kinfvect);
	}

	if (nstep != NULL) {
		KifBrowse(nstep, kinfvect);
		delete nstep;
	}

	return kinfvect;
}

KifElement* KifCode::KifParseJSONVector(x_node* xn, KifElement* kf) {
	KifVector* kvect = new KifVector(this, kf);
	for (int i = 0; i < xn->nodes.size(); i++)
		KifBrowse(xn->nodes[i], kvect);

	return kvect;
}

KifElement* KifCode::KifParseValTuple(x_node* xn, KifElement* kf) {
	int i;

	KifTuple* ktuple = new KifTuple(this, kf);
	KifBaseVector kvect(NULL, NULL);
	kvect.kifcode = this;

	for (i = 0; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->nodes.size() == 1)
			KifBrowse(xn->nodes[i], &kvect);
		else {
			KifInstruction ki(NULL, NULL);
			ki.kifcode = this;
			KifBrowse(xn->nodes[i], &ki);
			kvect.Add(ki.instructions[0]);
		}
	}

	ktuple->build(kvect.values.size());
	for (i = 0; i < kvect.values.size(); i++)
		ktuple->values[i] = kvect.values[i];

	kvect.values.clear();
	return ktuple;
}

KifElement* KifCode::KifParseValVector(x_node* xn, KifElement* kf) {
	int i;

	if (kf != NULL && kf->Instructiontype() == kifENUM) {
		//In this case, each value will be declared as a variable
		KifEnum* ken = new KifEnum(this, kf->Parent());
		ken->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);
		for (i = 0; i<xn->nodes.size(); i++)
			KifBrowse(xn->nodes[i], ken);
		return ken;
	}

	KifBaseVector* kvect = new KifBaseVector(this, kf);

	if (xn->token == "valpredicatevector" || xn->token == "valvectortail") {
		for (i = 0; i<xn->nodes.size(); i++)
			KifBrowse(xn->nodes[i], kvect);
	}
	else {
		for (i = 0; i<xn->nodes.size(); i++) {
			if (xn->nodes[i]->nodes.size() == 1)
				KifBrowse(xn->nodes[i], kvect);
			else {
				KifInstruction ki(NULL, NULL);
				ki.kifcode = this;
				KifBrowse(xn->nodes[i], &ki);
				kvect->Add(ki.instructions[0]);
			}
		}
	}
	kvect->evaluate = false;
	for (i = 0; i<kvect->values.size(); i++) {
		if (kvect->values[i]->type<kifString || kvect->values[i]->type>kifBoolean) {
			kvect->evaluate = true;
			break;
		}
	}

	return kvect;
}
KifElement* KifCode::KifParseDico(x_node* xn, KifElement* kf) {
	KifInstruction kbloc(NULL, NULL, kifBLOC);
	kbloc.kifcode = this;

	//First the key
	KifBrowse(xn->nodes[0], &kbloc);
	KifElement* key = kbloc.instructions[0];


	kbloc.instructions.clear();

	//then the value
	KifBrowse(xn->nodes[1], &kbloc);
	KifElement* val = kbloc.instructions[0];

	KifBaseMap* kmap = (KifBaseMap*)kf;
	kmap->keys.push_back(key);
	kmap->values.push_back(val);
	if (xn->nodes.size() == 3)
		KifBrowse(xn->nodes[2], kf);
	return kf;
}

KifElement* KifCode::KifParseJSONDico(x_node* xn, KifElement* kf) {
	KifInstruction kbloc(NULL, NULL, kifBLOC);
	kbloc.kifcode = this;

	//First the key
	KifBrowse(xn->nodes[0], &kbloc);
	string key = kbloc.instructions[0]->String();

	kbloc.instructions[0]->Release();
	kbloc.instructions.clear();

	//then the value
	KifBrowse(xn->nodes[1], &kbloc);
	KifElement* val = kbloc.instructions[0];

	kf->Push(key, val);
	return kf;
}


KifElement* KifCode::KifParseValMap(x_node* xn, KifElement* kf) {
	KifBaseMap* kmap;

	if (xn->token == "maptail" || xn->token == "mapmerging") {
		kmap = (KifBaseMap*)kf;
		kmap->keys.push_back(new KifConst(this, NULL, idpipe));
		KifInstruction kbloc(NULL, NULL, kifBLOC);
		kbloc.kifcode = this;
		KifBrowse(xn->nodes[0], &kbloc);
		kmap->values.push_back(kbloc.instructions[0]);
		return kf;
	}

	kmap = new KifBaseMap(this, kf, false);
	for (int i = 0; i<xn->nodes.size(); i++)
		KifBrowse(xn->nodes[i], kmap);
	return kmap;
}

KifElement* KifCode::KifParseJSONMap(x_node* xn, KifElement* kf) {
	KifMap* kmap = new KifMap(this, kf);
	for (int i = 0; i<xn->nodes.size(); i++)
		KifBrowse(xn->nodes[i], kmap);
	return kmap;
}


//label detection
KifElement* KifCode::KifParseLabel(x_node* xn, KifElement* kf) {
	string& name = xn->nodes[0]->value;
	KifLabel lab;
	lab.line = gGlobals->bnfkif->x_linenumber(xn->start + 1);
	lab.fileindex = kifGlobals->debuginfocurrent->currentfile;
	lab.label = name;
	if (kifGlobals->kifLabels.find(name) != kifGlobals->kifLabels.end()) {
		string message = KIFLIBSTRINGS[329] + name + "'";
		throw new KifRaiseError(message, kifGlobals->current->name, lab.line, xn->end);
	}
	kifGlobals->kifLabels[name] = lab;
	kifGlobals->kifLabelIdx[lab.line] = lab;
	return kifTRUE;
}

void KifCode::ComputePredicateParameters(x_node* xn, KifElement* kcf) {
	KifInstruction kbloc(NULL, NULL, kifBLOC);
	kbloc.kifcode = this;
	kbloc.type = kifInstructionPARAMETERPredicate;
	KifBrowse(xn->nodes[0], &kbloc);

	kcf->Add(kbloc.instructions[0]);

	if (xn->nodes.size() == 2)
		ComputePredicateParameters(xn->nodes[1], kcf);
}

KifElement* KifCode::KifParsePredicateDatabase(x_node* xn, KifElement* kf) {
	KifBrowse(xn->nodes[0], kf);
	return kf;
}

KifPredicateContainer* KifCode::Predicatecontainer() {
	KifDomain* ktop = kifGlobals->Top();
	if (ktop == NULL)
		ktop = kstart;

	KifPredicateContainer* kpcont;
	if (!ktop->declarations.check(KIFPREDICATEENTREE)) {
		kpcont = new KifPredicateContainer(this);
		ktop->Set(KIFPREDICATEENTREE, kpcont, NULL);
	}
	else
		kpcont = (KifPredicateContainer*)ktop->declarations[KIFPREDICATEENTREE];
	return kpcont;
}



KifElement* KifCode::KifParsePredicateDCGVariable(string& name, KifElement* kcf, bool param) {
	//if (param)
	//	kcf = new KifInstructionPARAMETER(this, kcf);

	KifPredicateVariable* var;
	if (predicatevariables.find(name) == predicatevariables.end()) {
		var = new KifPredicateVariable(this, kcf, KifPredicate::nb++, name);
		predicatevariables[name] = var;
	}
	else {
		var = predicatevariables[name];
		kcf->Add(var);
	}
	return var;
}

//This is a DCG rule. The first element is the head of the rule. The other elements might be:
//a dcgfinal : [.]
//a predicate or a word...

KifElement* KifCode::KifParsePredicateDCG(x_node* xn, KifElement* kf) {

	//the container is where the rules are stored... 
	KifPredicateContainer* kpcont = Predicatecontainer();
	//We extract our predicate head name
	string sname = xn->nodes[0]->nodes[0]->value;
	short name = KifNewId(sname);
	if (predicates.find(name) == predicates.end())
		predicates[name] = new KifPredicateFunction(this, NULL, NULL, name);
	//We compute the inner variables: from S0 to Sn
	int endidx = xn->nodes.size() - 1;
	//The last element, if it is a final block should not be counted in...
	if (xn->nodes.back()->token == "finaldcg")
		endidx--;

	//We create our rule:
	predicatevariables.clear();
	KifPredicateRule* krule = new KifPredicateRule(this, kf, name);
	KifPredicate* kcf = new KifPredicate(this, NULL, name);
	krule->head = kcf;
	KifPredicateRuleElement* kblocelement = NULL;
	KifInstruction kcount(NULL, NULL, kifBLOC);
	x_node* nsub;
	KifPredicateRuleElement* kpredelement = NULL;
	char buff[] = { '?', '?', 'A', 0 };

	//Two cases: First it is a rule of the sort: det --> [a]...
	if (xn->nodes[1]->token == "finaltoken") {
		//Terminal rule		
		KifBaseVector* kvect = new KifBaseVector(this, kcf);
		//We add our value...
		KifBrowse(xn->nodes[1], kvect);
		sname = "";
		int i;
		int nb = 0;
		//we try to compute the number of dcgword in the middle... to generate a sequence of variable from ??X0 to ??Xnb
		for (i = 2; i < xn->nodes.size(); i++) {
			if (xn->nodes[2]->token == "dcgword")
				nb++;
			else
				break;
		}

		if (kvect->values.size()) {
			KifBaseVectorMerge* kmerge = new KifBaseVectorMerge(this, kvect);
			//Basically, we have: det --> [a]. which is transformed into det([a|?X],?X,...)
			sname = buff;
			KifParsePredicateDCGVariable(sname, kmerge, false); //this part is [a|?X]
			if (nb) {
				buff[2] += nb;
				sname = buff;
			}
			KifParsePredicateDCGVariable(sname, kcf, true); //this one is simply ?X...
		}
		else
			kcf->Add(kvect); //In that case, we do not want a variable... det([],[],...).

		//If we have some parameters, it is finally time to handle them, at the level of the root predicate...
		if (xn->nodes[0]->nodes.size() >= 2) {
			//we create an instruction of the sort: det([a|?X],?X...).
			if (xn->nodes[0]->nodes[1]->token == "expression") {
				KifBrowse(xn->nodes[0]->nodes[1], &kcount);
				kcf->weight = kcount.instructions[0];
				kcount.instructions.clear();
				if (xn->nodes[0]->nodes.size() == 3)
					ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
			}
			else
				ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
		}

		for (int i = 2; i < xn->nodes.size(); i++) {
			//If we have a "finaldcg" object, then we create a specific predicate suite
			//which is added to kblocelement...
			if (xn->nodes[i]->token == "finaldcg") {
				kpredelement = new KifPredicateRuleElement(this, kpredelement);
				KifBrowse(xn->nodes.back(), kpredelement);
				if (kblocelement == NULL)
					krule->Addtail(kpcont, kpredelement);
				else
					krule->Addtail(kpcont, kblocelement);
				return krule;
			}

			//We have a rule: det --> [a], tst...
			nsub = xn->nodes[i];
			//Then we analyze each element, which should have the same form as the head...
			name = KifNewId(nsub->nodes[0]->value);
			kpredelement = new KifPredicateRuleElement(this, kpredelement);
			if (kblocelement == NULL)
				kblocelement = kpredelement;
			kcf = new KifPredicate(this, kpredelement, name);
			if (sname == "")
				kcf->Add(kvect); //in that case, we automatically transmit [], we do not have a ?X variable to deal with
			else {
				buff[2] = 'A' + i - 2;
				sname = buff;//previous variable
				KifParsePredicateDCGVariable(sname, kcf, true); //we add the ?X variable... It is the rest of the list...
			}

			buff[2] = 'A' + i - 1;
			sname = buff;
			KifParsePredicateDCGVariable(sname, kcf, true); //then we control variable...

			if (nsub->nodes.size() >= 2) {//we add the other variables, which might part of the description of the predicate....
				if (nsub->nodes[1]->token == "expression") {
					KifBrowse(nsub->nodes[1], &kcount);
					kcf->weight = kcount.instructions[0];
					kcount.instructions.clear();
					if (nsub->nodes.size() == 3)
						ComputePredicateParameters(nsub->nodes.back(), kcf);
				}
				else
					ComputePredicateParameters(nsub->nodes.back(), kcf);
			}
		}

		if (kblocelement == NULL)
			((KifPredicateRule*)krule)->addfinal(kpcont);
		else
			krule->Addtail(kpcont, kblocelement);
		return krule;
	}

	kblocelement = new KifPredicateRuleElement(this, NULL);

	//the head...
	//We will implement the head as: predicate(??S0,??Sn...) where n is the number of elements in our expressions
	// p --> sn,vp,pp. here n is 3... p(??S0,??S3...)	
	sname = buff;
	//We add our first variable
	KifParsePredicateDCGVariable(sname, kcf, true);
	buff[2] = 'A' + endidx;
	sname = buff;
	//and our following one...
	KifParsePredicateDCGVariable(sname, kcf, true);

	//If we have some parameters, it is finally time to handle them, at the level of the root predicate...
	if (xn->nodes[0]->nodes.size() >= 2) {
		if (xn->nodes[0]->nodes[1]->token == "expression") {
			KifBrowse(xn->nodes[0]->nodes[1], &kcount);
			kcf->weight = kcount.instructions[0];
			kcount.instructions.clear();
			if (xn->nodes[0]->nodes.size() == 3)
				ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
		}
		else
			ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
	}

	for (int i = 1; i <= endidx; i++) {
		nsub = xn->nodes[i];
		//Then we analyze each element, which should have the same form as the head...
		name = KifNewId(nsub->nodes[0]->value);
		if (kpredelement == NULL)
			kpredelement = kblocelement;
		else
			kpredelement = new KifPredicateRuleElement(this, kpredelement);

		kcf = new KifPredicate(this, kpredelement, name);
		buff[2] = 'A' + i - 1;
		sname = buff;
		KifParsePredicateDCGVariable(sname, kcf, true);
		buff[2] = 'A' + i;
		sname = buff;
		KifParsePredicateDCGVariable(sname, kcf, true);
		if (nsub->nodes.size() >= 2) {
			if (nsub->nodes[1]->token == "expression") {
				KifBrowse(nsub->nodes[1], &kcount);
				kcf->weight = kcount.instructions[0];
				kcount.instructions.clear();
				if (nsub->nodes.size() == 3)
					ComputePredicateParameters(nsub->nodes.back(), kcf);
			}
			else
				ComputePredicateParameters(nsub->nodes.back(), kcf);
		}
	}

	if (xn->nodes.back()->token == "finaldcg") {
		if (kpredelement == NULL)
			KifBrowse(xn->nodes.back(), kblocelement);
		else {
			kpredelement = new KifPredicateRuleElement(this, kpredelement);
			KifBrowse(xn->nodes.back(), kpredelement);
		}
	}

	krule->Addtail(kpcont, kblocelement);
	return krule;
}

//A call to a predicate expression
//We have three possibilities, which must be taken into account:
//1)  predicate(x,y):-true; in this case, this predicate is appended to the knowledge base
//2)  predicate(x,y):-false; in this case, it is removed from the knwoledge base
//3) predicate(x,y) :-  expression, then we must evaluate the expression to add this predicate
KifElement* KifCode::KifParsePredicateDefinition(x_node* xn, KifElement* kf) {
	//We create an expression
	//If it is a boolean value (either true or false)
	KifPredicateContainer* kpcont = Predicatecontainer();

	string sname = xn->nodes[0]->nodes[0]->value;
	short name = KifNewId(sname);
	if (predicates.find(name) == predicates.end())
		predicates[name] = new KifPredicateFunction(this, NULL, NULL, name);

	predicatevariables.clear();
	if (xn->token == "predicatefact" || xn->nodes[1]->token == "abool") {
		KifElement* kbloc = new KifInstructionPredicate(this, NULL, name);
		KifPredicateKnowledgeBase* kcf = new KifPredicateKnowledgeBase(this, kbloc, name);
		if (xn->nodes[0]->nodes[1]->token == "expression") {
			KifInstruction kbloc(NULL, NULL, kifBLOC);
			KifBrowse(xn->nodes[0]->nodes[1], &kbloc);
			kcf->weight = kbloc.instructions[0];
		}
		if (xn->nodes[0]->nodes.back()->token == "predicateparameters")
			ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
		//We check if there are some PredicateVariable in the parameters
		if (xn->token == "predicatefact" || xn->nodes[1]->value == "true") {
			bool keepasknowledge = true;
			if (kpcont->rules.find(name) != kpcont->rules.end())
				keepasknowledge = false;
			else
				//if there is a predicate in the formula, then it cannot be an instance, it is a goal
			if (kcf->unifiedDeclaration(NULL) == false)
				keepasknowledge = false;

			if (keepasknowledge == false) {
				kbloc->Release();
				kbloc = new KifPredicateRule(this, kf, name);
				((KifPredicateRule*)kbloc)->addfinal(kpcont);
				KifPredicate* kfx = new KifPredicate(this, NULL, name);
				kfx->weight = kcf->weight;
				kcf->weight = kifONE;
				for (int j = 0; j<kcf->parameters.size(); j++)
					kfx->parameters.push_back(kcf->parameters[j]);
				kcf->parameters.clear();
				kcf->Release();
				((KifPredicateRule*)kbloc)->head = kfx;
				return kbloc;
			}
			//We want to add our value to the knowlegde base		
			kcf->add = true;
		}
		kf->Add(kbloc);
		return kbloc;
	}
	else {
		if (xn->nodes[1]->token == "predicatedatabase") {
			KifElement* kbloc = new KifInstructionPredicate(this, kf, name);
			KifPredicateDatabase* kcf = new KifPredicateDatabase(this, kbloc, NULL, name);
			if (xn->nodes[0]->nodes[1]->token == "expression") {
				KifInstruction kbloc(NULL, NULL, kifBLOC);
				KifBrowse(xn->nodes[0]->nodes[1], &kbloc);
				kcf->weight = kbloc.instructions[0];
			}
			if (xn->nodes[0]->nodes.back()->token == "predicateparameters")
				ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
			if (xn->nodes[1]->nodes[0]->value == KIFLIBSTRINGS[330])
				kcf->store = _PREDSTORE;
			else
			if (xn->nodes[1]->nodes[0]->value == KIFLIBSTRINGS[331])
				kcf->store = _PREDREMOVE;

			kcf->adddb = true;
			return KifBrowse(xn->nodes[1]->nodes[1], kcf);
		}
	}

	KifPredicateRule* kbloc = new KifPredicateRule(this, kf, name);
	KifPredicate* kcf = new KifPredicate(this, NULL, name);
	if (xn->nodes[0]->nodes[1]->token == "expression") {
		KifInstruction kbloc(NULL, NULL, kifBLOC);
		KifBrowse(xn->nodes[0]->nodes[1], &kbloc);
		kcf->weight = kbloc.instructions[0];
	}
	if (xn->nodes[0]->nodes.back()->token == "predicateparameters")
		ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
	kbloc->head = kcf;

	KifPredicateRuleElement* kblocelement = new KifPredicateRuleElement(this, NULL);
	//Else we need to browse our structure...
	KifBrowse(xn->nodes[1], kblocelement);
	kbloc->Addtail(kpcont, kblocelement);
	return kbloc;
}


KifElement* KifCode::KifParsePredicateCut(x_node* xn, KifElement* kf) {
	if (xn->token == "cut")
		kf->Add(kifCUT);
	else
		kf->Add(kifFAIL);
	return kf;
}

KifElement* KifCode::KifParsePredicateExpression(x_node* xn, KifElement* kf) {
	//This is where we analyse our structure...
	KifPredicateRuleElement* kbloc = new KifPredicateRuleElement(this, kf);
	if (xn->nodes[0]->token == "negation") {
		kbloc->negation = true;
		if (xn->nodes[1]->token == "expressions") {
			KifInstruction klocbloc(NULL, NULL, kifBLOC);
			KifBrowse(xn->nodes[1], &klocbloc);
			KifInstruction* kint = CloningInstruction((KifInstruction*)klocbloc.instructions[0]);
			kbloc->Add(kint);
		}
		else
			KifBrowse(xn->nodes[1], kbloc);
		if (xn->nodes.size() == 4) {
			if (xn->nodes[2]->value == ";")
				kbloc->disjunction = true;
			KifBrowse(xn->nodes[3], kbloc);
		}
	}
	else {
		if (xn->nodes[0]->token == "expressions") {
			KifInstruction klocbloc(NULL, NULL, kifBLOC);
			KifBrowse(xn->nodes[0], &klocbloc);
			KifInstruction* kint = CloningInstruction((KifInstruction*)klocbloc.instructions[0]);
			kbloc->Add(kint);
		}
		else
			KifBrowse(xn->nodes[0], kbloc);
		if (xn->nodes.size() == 3) {
			if (xn->nodes[1]->value == ";")
				kbloc->disjunction = true;
			KifBrowse(xn->nodes[2], kbloc);
		}
	}
	return kbloc;
}

KifElement* KifCode::KifParsePredicateVariable(x_node* xn, KifElement* kf) {

	string& name = xn->value;
	KifPredicateVariable* var;
	short idname;

	if (xn->nodes[0]->token == "goal")
		return KifBrowse(xn->nodes[0], kf);

	if (predicatevariables.find(name) == predicatevariables.end()) {
		idname = KifPredicate::nb++;
		var = new KifPredicateVariable(this, kf, idname, name);
		predicatevariables[name] = var;
	}
	else {
		var = predicatevariables[name];
		kf->Add(var);
	}
	return var;
}

KifElement* KifCode::KifParsePredicateTerm(x_node* xn, KifElement* kf) {
	string name("_");
	short idname = iduniversal;
	if (xn->token == "term") {
		name = xn->value;
		name = name.substr(0, name.size() - 2);
		idname = KifNewId(name);
	}

	KifPredicateTerm* var = new KifPredicateTerm(this, kf, idname, name);
	if (xn->nodes.back()->token == "predicateparameters")
		ComputePredicateParameters(xn->nodes.back(), var);
	return var;
}

KifElement* KifCode::KifParsePredicateAssert(x_node* xn, KifElement* kf) {
	short id = idassertz;
	if (xn->nodes[0]->token == "assertweight")
		id = idponder;
	else {
		if (xn->nodes[0]->token == "assertcommandafter") {
			if (xn->nodes[0]->value == KIFLIBSTRINGS[332])
				id = idassertdb;
		}
		else {
			if (xn->nodes[0]->token == "assertcommandbefore")
				id = idasserta;
			else {
				if (xn->nodes[0]->token == "retractcommand")
				if (xn->nodes[0]->value == KIFLIBSTRINGS[333])
					id = idretractdb;
				else
					id = idretract;
			}
		}
	}

	KifElement* kbloc = new KifPredicateKnowledgeBaseFunction(this, kf, id);
	if (id == idassertdb || id == idretractdb) {
		if (xn->nodes.size() != 3)  {
			long line = GetCurrentLine(0, xn);
			string message = KIFLIBSTRINGS[334];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		KifBrowse(xn->nodes[1], kbloc);
	}
	else {
		if (xn->nodes.size() != 2)  {
			long line = GetCurrentLine(0, xn);
			string message = KIFLIBSTRINGS[335];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
	}
	KifBrowse(xn->nodes.back(), kbloc);
	return kbloc;
}

//This expression is used to test a predicate...
KifElement* KifCode::KifParsePredicateHead(x_node* xn, KifElement* kf) {
	short name = KifNewId(xn->nodes[0]->value);
	KifPredicate* kcf = new KifPredicate(this, NULL, name);
	if (xn->nodes[1]->token == "expression") {
		KifInstruction kbloc(NULL, NULL, kifBLOC);
		KifBrowse(xn->nodes[1], &kbloc);
		kcf->weight = kbloc.instructions[0];
	}

	if (xn->nodes.back()->token == "predicateparameters")
		ComputePredicateParameters(xn->nodes.back(), kcf);
	kf->Add(kcf);
	return kcf;
}


//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//--- HASKELL and LAMBDA
//-------------------------------------------------------------------------------------------------------------------------

bool KifCode::KifCheckVariable(x_node* xn, KifElement* kf) {
	if (xn == NULL)
		return false;

	if (xn->token == "variable") {
		short id = KifNewId(xn->nodes[0]->value);
		if (kf->Declared(id) != NULL)
			return true;
		return false;
	}

	for (size_t i = 0; i<xn->nodes.size(); i++)
	if (KifCheckVariable(xn->nodes[i], kf) == true)
		return true;

	return false;
}

bool KifCheckingVariableName(x_node* xn, KifElement* kf) {
	if (xn == NULL)
		return false;

	if (xn->token == "variable") {
		short id = KifNewId(xn->nodes[0]->value);
		if (kf->Name() == id)
			return true;
		return false;
	}

	for (size_t i = 0; i<xn->nodes.size(); i++)
	if (KifCheckingVariableName(xn->nodes[i], kf) == true)
		return true;

	return false;
}

void KifBrowseVariableVector(x_node* xn, KifElement* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable") {
		short id = KifNewId(xn->nodes[0]->value);
		if (kf->Declared(id) == NULL) {
			KifElement* var = new KifFunctionDeclaration(kf->kifcode, kf, id, kifMINUSONE);
			kf->Set(id, var, NULL);
			var = new KifSelf(kf->kifcode, var, kifNULL);
		}
		return;
	}

	for (size_t i = 0; i<xn->nodes.size(); i++)
		KifBrowseVariableVector(xn->nodes[i], kf);
}

void KifBrowseVariableMap(x_node* xn, KifElement* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable") {
		short id = KifNewId(xn->nodes[0]->value);
		if (kf->Declared(id) == NULL) {
			KifElement* var = new KifFunctionDeclaration(kf->kifcode, kf, id, kifMINUSONE);
			kf->Set(id, var, NULL);
			var = new KifSelf(kf->kifcode, var, kifNULL);
		}
		return;
	}

	for (size_t i = 0; i<xn->nodes.size(); i++)
		KifBrowseVariableMap(xn->nodes[i], kf);
}



void KifCode::KifBrowseVariable(x_node* xn, KifElement* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable") {
		short id = KifNewId(xn->nodes[0]->value);
		if (kf->Declared(id) == NULL) {
			KifElement* var = new KifFunctionDeclaration(this, kf, id, kf);
			kf->Set(id, var, NULL);
			var = new KifSelf(this, var, kifNULL);
		}
		return;
	}

	for (size_t i = 0; i<xn->nodes.size(); i++)
		KifBrowseVariable(xn->nodes[i], kf);
}

KifElement* KifCode::KifParseVarHaskellLetMin(x_node* xn, KifElement* kf) {
	//We do some makeup in order to transform a letmin into a let
	x_node* n = new x_node;
	n->token = "letkeyword";
	n->value = KIFLIBSTRINGS[336];
	xn->token = "let";
	xn->nodes.insert(xn->nodes.begin(), n);
	KifBrowse(xn, kf);//then we can compile it as if it was a known let structure...
	return kf;
}

KifElement* KifCode::KifParseHaskellExpression(x_node* xn, KifElement* kf) {
	int id;
	KifElement* var;
	if (xn->nodes[0]->token == "word") {
		id = KifNewId(xn->nodes[0]->value);
		if (kf->Declared(id) != NULL) {
			//In this case, we do not need to return an error
			//It is a case construction within a function...
			long line = GetCurrentLine(0, xn);
			string message = KIFLIBSTRINGS[319];
			message += kifGlobals->kifIdString[id];
			message += KIFLIBSTRINGS[320];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}

		var = new KifFunctionDeclaration(this, NULL, id, kf);
		kf->Set(id, var, NULL);
		kf = new KifSelf(this, var, kifNULL);
		return var;
	}
	else
		var = KifBrowse(xn->nodes[0], kf);
	return var;
}

KifElement* KifCode::KifParseHaskellVector(x_node* xn, KifElement* kf) {

	KifBaseVector* kvect = new KifBaseVector(this, kf);
	char ch = kf->Choice(); //Only the top Haskell Vector can be an argument
	kf->Setchoice(1);
	KifElement* var;
	for (int i = 0; i<xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "haskelltail") {
			KifBaseVectorMerge* kbv = new KifBaseVectorMerge(this, NULL);
			kvect->values.push_back(kbv);
			var = KifBrowse(xn->nodes[i]->nodes[0], kf);
			kbv->values.push_back(var);
			break;
		}
		var = KifBrowse(xn->nodes[i], kf);
		var->Setreference();
		kvect->values.push_back(var);
	}
	kf->Setchoice(ch);
	return kvect;
}

KifElement* KifCode::KifParseHaskellMap(x_node* xn, KifElement* kf) {

	KifBaseMap* kmap = new KifBaseMap(this, kf, false);
	char ch = kf->Choice(); //Only the top Haskell Vector can be an argument
	kf->Setchoice(1);
	KifElement* var;
	for (int i = 0; i<xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "haskellmaptail") {
			kmap->keys.push_back(new KifConst(this, NULL, idpipe));
			var = KifBrowse(xn->nodes[i], kf);
			kmap->values.push_back(var);
			break;
		}
		var = KifBrowse(xn->nodes[i]->nodes[0], kf);
		kmap->keys.push_back(var);
		var = KifBrowse(xn->nodes[i]->nodes[1], kf);
		kmap->values.push_back(var);
	}
	kf->Setchoice(ch);
	return kmap;
}


KifElement* KifCode::KifParseLambdaFunction(x_node* xn, KifElement* kf) {
	int idname = KifNewId("&lambdahaskell;");
	KifFunction* kfunc = new KifFunction(this, NULL, idname);
	KifElement* var;
	for (int i = 0; i<xn->nodes.size() - 1; i++) {
		if (xn->nodes[i]->token == "word") {
			idname = KifNewId(xn->nodes[i]->value);
			var = new KifFunctionDeclaration(this, kfunc, idname, kfunc);
			kfunc->Set(idname, var, NULL);
			var = new KifSelf(this, var, kifNULL);
		}
		else
			KifBrowse(xn->nodes[i], kfunc);
	}
	kfunc->choice = 1;
	var = new KifCallReturn(this, kfunc, kifGlobals->idreturn);
	kifGlobals->PushStack(kfunc);
	KifInstruction* kbloc = KifCreateInstruction(NULL, kifBLOC);
	KifBrowse(xn->nodes.back(), kbloc);

	if (kbloc->action == kifBLOC && kbloc->instructions.size() == 1) {
		var->Add(kbloc->instructions[0]);
		kbloc->instructions.clear();
		kbloc->Release();
	}
	else {
		kbloc = CloningInstruction(kbloc);
		var->Add(kbloc);
	}

	kifGlobals->PopStack();
	return kfunc;
}

//this case which is associated to "hcompose" can only be called from KifParseHaskellFunctionCall
//In the other cases (see Filter and Map below), we simply jump to definition itself
//of the corresponding calls (filtering, mapping or zipping)...
KifElement* KifCode::KifParseHaskellCompose(x_node* xn, KifElement* kbase) {
	if (kbase->type != kifCallFunctionHaskell) {
		KifCallFunctionHaskell* kf = new KifCallFunctionHaskell(this, kbase);
		kf->Init(NULL);
		return KifBrowse(xn->nodes[0], kf);
	}
	return KifBrowse(xn->nodes[0], kbase);
}


KifElement* KifCode::KifParseHaskellIn(x_node* xn, KifElement* kf) {
	KifInstruction kbloc(NULL, NULL, kifBLOC);
	kbloc.kifcode = this;

	//we must feed our return section with this value...
	KifBrowse(xn->nodes[0], &kbloc);
	kf->Add(kbloc.instructions[0]);
	return kf;
}


KifElement* KifCode::KifParseHaskellWhere(x_node* xn, KifElement* kf) {
	KifElement* var;
	KifInstruction* kres;
	int idname;
	KifCallFunctionHaskell* kint = (KifCallFunctionHaskell*)kf;
	kint->body->lambdadomain->local = true;
	for (int i = 0; i<xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "word") {
			idname = KifNewId(xn->nodes[i]->value);
			if (kint->body->lambdadomain->declarations.check(idname)) {
				string message = KIFLIBSTRINGS[319];
				message += xn->nodes[i]->value;
				message += KIFLIBSTRINGS[337];
				throw new KifRaiseError(message, kifGlobals->current->name, GetCurrentLine(0, xn), xn->end);
			}

			var = new KifFunctionDeclaration(this, NULL, idname, kifMINUSONE);
			kint->body->lambdadomain->Set(idname, var, NULL);
			kres = KifCreateInstruction(NULL, kifINITIALISATION);
			KifBrowse(xn->nodes[i + 1], kres);
			var->Setinitialisation(kres);
			var = new KifSelf(this, var, kifNULL);
			i++;
		}
		else
			KifBrowse(xn->nodes[i], kint);
	}
	return kf;
}

//-------------------------------------------------------------------------------------------------------------------------

KifElement* KifCode::KifParseReturnValue(x_node* xn, KifFunctionLambda* kf, char adding) {
	kf->choice = 1;
	if (adding == 1 && kf->instructions.size() != 0 && kf->instructions.back()->type == kifInstructionIF)
		return NULL;

	if (kf->instructions.size() == 0 || (kf->instructions.back()->type != kifInstructionIF && kf->instructions.back()->Name() != kifGlobals->idreturn))
		return new KifCallReturn(this, kf, kifGlobals->idreturn);


	x_node nvar;
	nvar.token = "variable";
	nvar.value = "&return;";
	nvar.start = xn->start;
	nvar.end = xn->end;
	x_node* nname = new x_node;
	nvar.nodes.push_back(nname);
	nname->token = "word";
	nname->value = "&return;";
	kf->choice = 1;

	KifElement* returnstatement = kf->instructions.back();
	kf->instructions.pop_back();
	KifCallReturn* kcallret;
	if (returnstatement->type == kifInstructionIF)
		kcallret = (KifCallReturn*)returnstatement->Instruction(1);
	else
		kcallret = (KifCallReturn*)returnstatement;

	if (adding == 2) {
		KifCallReturn* k = new KifCallReturn(this, NULL, kcallret->Name());
		k->parameters.push_back(kcallret->parameters[0]);
		kcallret = k;
	}


	KifInstruction* ki = NULL;
	//We create our variable if necessary...
	if (!kf->declarations.check(kifGlobals->idreturnvariable)) {
		KifElement* kret = kcallret->parameters[0];
		kcallret->parameters.clear();

		KifElement* var = new KifFunctionDeclaration(this, kf, kifGlobals->idreturnvariable, kf);
		kf->Set(kifGlobals->idreturnvariable, var, NULL);
		var = new KifSelf(this, var, kifNULL);
		ki = KifCreateInstruction(NULL, kifAFFECTATION);
		KifBrowse(&nvar, ki);
		//We create an accumulator value in the function instructions...
		ki->Add(kret);
		//Its value will be set by the next instructions after the call to KifParseReturnValue
		kf->Add(ki);
		//We transform our return statatement into a return(&return;);
		kcallret->add = 1;
		KifBrowse(&nvar, kcallret);
	}

	if (adding == 1) {
		ki = KifCreateInstruction(NULL, kifAFFECTATION);
		KifBrowse(&nvar, ki);
		kf->Add(ki);
	}
	kf->Add(returnstatement);
	return ki;
}
//-------------------------------------------------------------------------------------------------------------------------

KifElement* KifCode::KifParseHaskellFunctionCall(x_node* xn, KifElement* kf) {
	//We rebuild a complete tree, in order to benefit from the regular parsing of a function call
	x_node* nop = new x_node;
	nop->start = xn->start;
	nop->end = xn->end;
	if (xn->nodes[0]->token == "operator") {
		vector<x_node*> nodes;
		nodes.push_back(nop);
		nop->token = "expressions";
		nop->nodes.push_back(xn->nodes[1]);
		x_node* noper;
		x_node* prev = nop;
		int i;
		for (i = 2; i < xn->nodes.size(); i++) {
			noper = new x_node;
			nodes.push_back(noper);
			if (prev == nop) {
				noper->token = "operation";
				prev->nodes.push_back(noper);
			}
			else {
				noper->token = "expressions";
				noper->nodes.push_back(prev->nodes[1]);
				prev->nodes[1] = noper;
				noper->nodes.push_back(new x_node);
				noper = noper->nodes.back();
				nodes.push_back(noper);
				noper->token = "operation";
			}
			noper->nodes.push_back(xn->nodes[0]);
			noper->nodes.push_back(xn->nodes[i]);
			prev = noper;
		}
		KifElement* kcall = KifBrowse(nop, kf);
		for (i = 0; i < nodes.size(); i++) {
			nodes[i]->nodes.clear();
			delete nodes[i];
		}
		return kcall;

	}
	nop->token = "haskellcall";
	x_node* nfunc = new x_node;
	nfunc->token = "functioncall";
	nfunc->value = xn->nodes[0]->value;
	nfunc->nodes.push_back(xn->nodes[0]);
	nfunc->start = xn->start;
	nfunc->end = xn->end;
	nop->nodes.push_back(nfunc);
	x_node* param;
	x_node* prev = NULL;
	vector<x_node*> v;
	//The parameters is a recursive structure...
	for (int i = 1; i<xn->nodes.size(); i++) {
		param = new x_node;
		param->token = "parameters";
		param->nodes.push_back(xn->nodes[i]);
		if (prev == NULL)
			nop->nodes.push_back(param);
		else
			prev->nodes.push_back(param);
		v.push_back(param);
		prev = param;
	}

	KifElement* kcall = KifBrowse(nop, kf);
	nfunc->nodes.clear();
	for (int i = 0; i<v.size(); i++) {
		v[i]->nodes.clear();
		delete v[i];
	}

	nop->nodes.pop_back();
	delete nop;
	return kcall;
}
//-------------------------------------------------------------------------------------------------------------------------

KifElement* KifCode::KifParseHaskellBoolean(x_node* xn, KifElement* kbase) {
	KifCallFunctionHaskell* kf;
	if (kbase->type == kifCallFunctionHaskell)
		kf = (KifCallFunctionHaskell*)kbase;
	else {
		kf = new KifCallFunctionHaskell(this, kbase);
		kf->Init(NULL);
	}
	KifFunctionLambda* kfunc = kf->body;
	KifDomainLambda* lambdadom = kfunc->lambdadomain;
	kifGlobals->PushStack(kfunc);
	kifGlobals->PushStack(lambdadom);

	char buff[50];
	sprintf_s(buff, 50, "&%s;", xn->nodes[0]->value.c_str());
	//we need first to create a variable...
	x_node nvar;
	nvar.token = "variable";
	nvar.value = buff;
	nvar.start = xn->start;
	nvar.end = xn->end;
	x_node* nname = new x_node;
	nvar.nodes.push_back(nname);
	nname->token = "word";
	nname->value = buff;

	KifBrowseVariable(&nvar, lambdadom);
	KifBrowse(xn->nodes[2], lambdadom);

	kfunc->choice = 1;
	KifInstructionIF* ktest = new KifInstructionIF(this, kfunc);

	if (xn->nodes[1]->token == "comparison" || xn->nodes[1]->token == "operationin") {
		KifInstruction* ki = KifCreateInstruction(NULL, kifBLOC);
		KifBrowse(&nvar, ki); // we add our variable to compare with
		KifBrowse(xn->nodes[1], ki); // we add our comparison operator with its value...

		if (ki->action == kifBLOC && ki->instructions.size() == 1) {
			ktest->Add(ki->instructions[0]);
			ki->Release();
		}
		else {
			ki = CloningInstruction(ki);
			ktest->Add(ki);
		}
	}
	else {
		if (xn->nodes[1]->token == "hboollambda") {
			KifElement* kprevious = KifParseLambdaFunction(xn->nodes[1], kfunc);
			KifCallFunctionGeneral* kcall = new KifCallFunctionGeneral(this, ktest, kprevious->Name(), (KifFunction*)kprevious);
			KifBrowse(&nvar, kcall);
		}
		else {
			x_node* nop = new x_node;
			nop->start = xn->start;
			nop->end = xn->end;
			nop->token = "haskellcall";
			x_node* nfunc = new x_node;
			nfunc->token = "functioncall";
			nfunc->value = xn->nodes[1]->nodes[0]->value;
			nfunc->start = xn->start;
			nfunc->end = xn->end;
			nfunc->nodes.push_back(xn->nodes[1]->nodes[0]);
			nop->nodes.push_back(nfunc);
			x_node* param = new x_node;
			param->token = "parameters";
			nop->nodes.push_back(param);
			param->nodes.push_back(&nvar);

			x_node* prev = param;
			vector<x_node*> v;
			v.push_back(param);
			for (int i = 1; i<xn->nodes[1]->nodes.size(); i++) {
				param = new x_node;
				param->token = "parameters";
				param->nodes.push_back(xn->nodes[1]->nodes[i]);
				prev->nodes.push_back(param);
				v.push_back(param);
				prev = param;
			}

			KifBrowse(nop, ktest);
			for (int i = 0; i<v.size(); i++) {
				v[i]->nodes.clear();
				delete v[i];
			}
			nfunc->nodes.clear();
			nop->nodes.pop_back();
			delete nop;
		}
	}

	if (xn->nodes[0]->value == KIFLIBSTRINGS[338]) {
		//Whenever a value is not true, we break
		ktest->Add(kifTRUE);
		ktest->Add(kifBREAKFALSE);
	}
	else {
		ktest->Add(kifBREAKTRUE);
		lambdadom->name = 2; //the name 2 corresponds to a OR
	}

	kifGlobals->PopStack();
	kifGlobals->PopStack();
	return kfunc;

}

//Description of the compiling:
//0 is either foldl or foldr
//1 is the lambda function
//2 is the initial parameter (except if it is a folding1, then it is kifNULL)
//3 is the list to be folded
//4 if the function is a scan or a fold, and whether it is a left or right one...
//This case will have 5 parameters and will correspond to the matching section of sz==5 in KIFHASKELL
KifElement* KifCode::KifParseHaskellFold(x_node* xn, KifElement* kbase) {
	KifCallFunctionHaskell* kf;
	if (kbase->type == kifCallFunctionHaskell)
		kf = (KifCallFunctionHaskell*)kbase;
	else {
		kf = new KifCallFunctionHaskell(this, kbase);
		kf->Init(NULL);
	}
	KifFunctionLambda* kfunc = kf->body;
	KifDomainLambda* lambdadom = kfunc->lambdadomain;
	kifGlobals->PushStack(kfunc);
	kifGlobals->PushStack(lambdadom);

	char buff[50];
	sprintf_s(buff, 50, "&%s;", xn->nodes[0]->value.c_str());
	//we need first to create a variable...
	x_node nvar;
	nvar.token = "variable";
	nvar.value = buff;
	nvar.start = xn->start;
	nvar.end = xn->end;
	x_node* nname = new x_node;
	nvar.nodes.push_back(nname);
	nname->token = "word";
	nname->value = buff;

	//The &folding; <- expressions part...
	KifBrowseVariable(&nvar, lambdadom);
	if (xn->token == "folding")
		KifBrowse(xn->nodes[3], lambdadom);
	else
		KifBrowse(xn->nodes[2], lambdadom);

	//The initial value for the lambda function
	x_node accuvar;
	accuvar.token = "variable";
	accuvar.value = "&iaccu;";
	accuvar.start = xn->start;
	accuvar.end = xn->end;
	x_node* iname = new x_node;
	accuvar.nodes.push_back(iname);
	iname->token = "word";
	iname->value = "&iaccu;";

	KifBrowseVariable(&accuvar, lambdadom);
	if (xn->token == "folding")
		KifBrowse(xn->nodes[2], lambdadom);
	else
		lambdadom->Add(kifNOELEMENT);

	//The iterator direction
	KifElement* choice;
	string act = xn->nodes[0]->nodes[0]->token;
	if (act[0] == 'l') {
		if (act[1] == 's')
			choice = kifZERO; //scanning from left to right
		else
			choice = kifFALSE; //folding from left to right
	}
	else {
		if (act[1] == 's')
			choice = kifONE; //scanning right to left
		else
			choice = kifTRUE; //folding right to left
	}

	lambdadom->Add(choice);

	kfunc->choice = 1;
	KifElement* kret = KifParseReturnValue(xn, kfunc);

	if (xn->nodes[1]->token == "operator") {
		KifOperator op = kifGlobals->kifOperators[xn->nodes[1]->value];
		KifInstruction* ki = KifCreateInstruction(kret, op);
		KifBrowse(&accuvar, ki);
		KifBrowse(&nvar, ki);
	}
	else {
		KifCallFunctionGeneral* kcall;
		if (xn->nodes[1]->token == "hlambda") {
			KifElement* kprevious = KifParseLambdaFunction(xn->nodes[1], kfunc);
			kcall = new KifCallFunctionGeneral(this, kret, kprevious->Name(), (KifFunction*)kprevious);
			if (choice->Boolean() == false) {
				KifBrowse(&accuvar, kcall);
				KifBrowse(&nvar, kcall);
			}
			else {
				KifBrowse(&nvar, kcall);
				KifBrowse(&accuvar, kcall);
			}
		}
		else {
			x_node* nop = new x_node;
			nop->start = xn->start;
			nop->end = xn->end;
			nop->token = "haskellcall";
			x_node* nfunc = new x_node;
			nfunc->token = "functioncall";
			nfunc->value = xn->nodes[1]->nodes[0]->value;
			nfunc->nodes.push_back(xn->nodes[1]->nodes[0]);
			nfunc->start = xn->start;
			nfunc->end = xn->end;
			nop->nodes.push_back(nfunc);
			x_node* param = new x_node;
			param->token = "parameters";
			nop->nodes.push_back(param);
			if (choice->Boolean() == true)
				param->nodes.push_back(&nvar);
			else
				param->nodes.push_back(&accuvar);
			x_node* param2 = new x_node;
			param2->token = "parameters";
			param->nodes.push_back(param2);
			if (choice->Boolean() == true)
				param2->nodes.push_back(&accuvar);
			else
				param2->nodes.push_back(&nvar);

			x_node* prev = param2;
			vector<x_node*> v;
			v.push_back(param);
			v.push_back(param2);
			for (int i = 1; i<xn->nodes[1]->nodes.size(); i++) {
				param = new x_node;
				param->token = "parameters";
				param->nodes.push_back(xn->nodes[1]->nodes[i]);
				prev->nodes.push_back(param);
				v.push_back(param);
				prev = param;
			}

			KifBrowse(nop, kret);
			for (int i = 0; i<v.size(); i++) {
				v[i]->nodes.clear();
				delete v[i];
			}
			nop->nodes.pop_back();
			nfunc->nodes.clear();
			delete nop;
		}
	}

	kifGlobals->PopStack();
	kifGlobals->PopStack();
	return kfunc;

}
//-------------------------------------------------------------------------------------------------------------------------

KifElement* KifCode::KifParseHaskellMapping(x_node* xn, KifElement* kbase) {
	KifCallFunctionHaskell* kf;
	if (kbase->type == kifCallFunctionHaskell)
		kf = (KifCallFunctionHaskell*)kbase;
	else {
		kf = new KifCallFunctionHaskell(this, kbase);
		kf->Init(NULL);
	}
	KifFunctionLambda* kfunc = kf->body;
	KifDomainLambda* lambdadom = kfunc->lambdadomain;

	x_node nvar;
	nvar.token = "variable";
	nvar.value = "&common;";
	nvar.start = xn->start;
	nvar.end = xn->end;
	x_node* nname = new x_node;
	nvar.nodes.push_back(nname);
	nname->token = "word";
	nname->value = "&common;";

	kifGlobals->PushStack(kfunc);
	kifGlobals->PushStack(lambdadom);

	KifElement* kret;
	//We parse our KIFLIBSTRINGS[339] value
	kf->add = 3;
	KifBrowse(xn->nodes[1], kf);
	//We are dealing with a simple return function
	//The value is stored in a variable, which is one step before the return statement
	//We store the calculus in a intermediary variable, whose name is &return; and which is created
	//with KifParseReturnValue function...
	if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
		kret = kfunc->instructions.back()->Parameter(0);
		if (kret->type == kifCallFunctionHaskell) {
			kifGlobals->PopStack();
			kifGlobals->PopStack();
			kf = (KifCallFunctionHaskell*)kret;
			kfunc = kf->body;
			kfunc->choice = 1;
			lambdadom = kfunc->lambdadomain;
			kifGlobals->PushStack(kfunc);
			kifGlobals->PushStack(lambdadom);
		}
	}

	//No composition
	if (lambdadom->instructions.size() == 1) {
		kret = lambdadom->instructions[0];
		KifCallFunctionHaskell* kinit = NULL;
		if (kret->Initialisation()->Instruction(0)->type == kifCallFunctionHaskell) {
			kinit = (KifCallFunctionHaskell*)kret->Initialisation()->Instruction(0);
			//we look for the most embedded call...
			if (kinit->body->lambdadomain->instructions.size() == 0 && kinit->body->Instruction(0)->Parameter(0)->type == kifCallFunctionHaskell)
				kinit = (KifCallFunctionHaskell*)kinit->body->Instruction(0)->Parameter(0);
		}

		if (kinit != NULL && kinit->body->lambdadomain->instructions.size() != 0) {
			//First we copy all our substructures into our main structure...
			kf->body->lambdadomain->instructions = kinit->body->lambdadomain->instructions;
			kf->body->lambdadomain->declarations = kinit->body->lambdadomain->declarations;
			kf->body->lambdadomain->local = kinit->body->lambdadomain->local;
			kf->body->arguments = kinit->body->arguments;
			kf->body->declarations = kinit->body->declarations;
			kf->body->instructions = kinit->body->instructions;
			kf->declarations = kinit->declarations;
			char adding = 0;
			if (kf != kbase) {
				adding = 2;
				kbase->Add(kf);
			}
			KifElement* kvar = kret;
			kvar->Seteval(200); //we deactivate the initialisation for this variable from which we stole its main structures...
			//if adding is two, then the return section in kinit will be duplicated and left intact...
			kret = KifParseReturnValue(xn, kfunc, adding);
			if (kret != NULL) {
				nvar.value = "&return;";
				nname->value = "&return;";
			}
		}
		else {
			lambdadom->instructions.clear();
			//we need first to create a variable...
			KifBrowseVariable(&nvar, lambdadom);
			lambdadom->Add(kret);
			lambdadom->Add(kifFALSE);

			kret = KifParseReturnValue(xn, kfunc, 1);

			//This variable becomes our current variable...
			if (kf != kbase)
				kbase->Add(kf);
		}
	}
	else {
		kret = KifParseReturnValue(xn, kfunc, 1);
		if (kret == NULL) {
			//Then this is a filter..., A if is detected as a last element in the function instructions set...
			//We create a new KifCallFunctionHaskell, in which we copy all that was computed with xn->nodes[1]
			KifCallFunctionHaskell* kcf = new KifCallFunctionHaskell(this, NULL);
			kcf->Init(NULL);
			kcf->body->lambdadomain->instructions = lambdadom->instructions;
			kcf->body->lambdadomain->declarations = lambdadom->declarations;
			kcf->body->lambdadomain->local = lambdadom->local;
			kcf->body->declarations = kfunc->declarations;
			kcf->body->instructions = kfunc->instructions;

			//We clear our current structure, in which we create a CALL to that filter...
			kcf->declarations = kf->declarations;
			lambdadom->instructions.clear();
			lambdadom->declarations.clear();
			lambdadom->local = false;
			kfunc->instructions.clear();
			kfunc->declarations.clear();

			//The second element, which is the list on which we compute, is then stored
			//in our structure...
			KifBrowseVariable(&nvar, lambdadom);
			lambdadom->Add(kcf);
			lambdadom->Add(kifFALSE);

			kret = KifParseReturnValue(xn, kfunc, 1);
		}
		else {
			nvar.value = "&return;";
			nname->value = "&return;";
		}
	}

	//The next
	//The return value, with either an operation
	if (xn->nodes[0]->token == "inverted") {
		KifOperator op = kifGlobals->kifOperators[xn->nodes[0]->nodes[1]->value];
		KifInstruction* ki = KifCreateInstruction(kret, op);
		KifBrowse(xn->nodes[0]->nodes[0], ki);
		KifBrowse(&nvar, ki);
	}
	else {
		if (xn->nodes[0]->token == "operator") {
			KifOperator op = kifGlobals->kifOperators[xn->nodes[0]->value];
			KifInstruction* ki = KifCreateInstruction(kret, op);
			KifBrowse(&nvar, ki);
			KifBrowse(&nvar, ki);
		}
		else {
			if (xn->nodes[0]->token == "operation") {
				KifInstruction* ki = KifCreateInstruction(NULL, kifBLOC);
				KifBrowse(&nvar, ki);
				KifBrowse(xn->nodes[0], ki);

				if (ki->action == kifBLOC && ki->instructions.size() == 1) {
					kret->Add(ki->instructions[0]);
					ki->Release();
				}
				else {
					ki = CloningInstruction(ki);
					kret->Add(ki);
				}
			}
			else {
				if (xn->nodes[0]->token == "hlambda") {
					KifElement* kprevious = KifParseLambdaFunction(xn->nodes[0], kfunc);
					KifCallFunctionGeneral* kcall = new KifCallFunctionGeneral(this, kret, kprevious->Name(), (KifFunction*)kprevious);
					KifBrowse(&nvar, kcall);
				}
				else { // a function call
					x_node* nop = new x_node;
					nop->start = xn->start;
					nop->end = xn->end;
					nop->token = "haskellcall";
					x_node* nfunc = new x_node;
					nfunc->token = "functioncall";
					nfunc->value = xn->nodes[0]->nodes[0]->value;
					nfunc->nodes.push_back(xn->nodes[0]->nodes[0]);
					nfunc->start = xn->start;
					nfunc->end = xn->end;
					nop->nodes.push_back(nfunc);
					x_node* param = new x_node;
					param->token = "parameters";
					nop->nodes.push_back(param);
					param->nodes.push_back(&nvar);
					x_node* prev = param;
					vector<x_node*> v;
					v.push_back(param);
					for (int i = 1; i<xn->nodes[0]->nodes.size(); i++) {
						param = new x_node;
						param->token = "parameters";
						param->nodes.push_back(xn->nodes[0]->nodes[i]);
						prev->nodes.push_back(param);
						v.push_back(param);
						prev = param;
					}

					KifBrowse(nop, kret);
					nfunc->nodes.clear();
					for (int i = 0; i<v.size(); i++) {
						v[i]->nodes.clear();
						delete v[i];
					}
					nop->nodes.pop_back();
					delete nop;
				}
			}
		}
	}

	kifGlobals->PopStack();
	kifGlobals->PopStack();
	return kfunc;
}

//-------------------------------------------------------------------------------------------------------------------------

KifElement* KifCode::KifParseHaskellFilter(x_node* xn, KifElement* kbase) {
	KifCallFunctionHaskell* kf;
	KifFunctionLambda* kfunc;
	KifDomainLambda* lambdadom;
	if (kbase->type == kifCallFunctionHaskell) {
		kf = (KifCallFunctionHaskell*)kbase;
	}
	else {
		kf = new KifCallFunctionHaskell(this, NULL);
		kf->Init(NULL);
	}

	KifElement* kret = NULL;
	kfunc = kf->body;
	kfunc->choice = 1;
	lambdadom = kfunc->lambdadomain;

	x_node nvar;
	nvar.token = "variable";
	nvar.value = "&common;";
	nvar.start = xn->start;
	nvar.end = xn->end;
	x_node* nname = new x_node;
	nvar.nodes.push_back(nname);
	nname->token = "word";
	nname->value = "&common;";

	kifGlobals->PushStack(kfunc);
	kifGlobals->PushStack(lambdadom);

	kf->add = 3;
	KifBrowse(xn->nodes[2], kf);
	if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
		kret = kfunc->instructions.back()->Parameter(0);
		if (kret->type == kifCallFunctionHaskell) {
			kifGlobals->PopStack(); //A call to a Haskell function
			kifGlobals->PopStack();
			kf = (KifCallFunctionHaskell*)kret;
			kfunc = kf->body;
			kfunc->choice = 1;
			lambdadom = kfunc->lambdadomain;
			kifGlobals->PushStack(kfunc);
			kifGlobals->PushStack(lambdadom);
		}
	}

	if (lambdadom->instructions.size() == 1) {
		kret = lambdadom->instructions[0];
		KifCallFunctionHaskell* kinit = NULL;
		if (kret->Initialisation()->Instruction(0)->type == kifCallFunctionHaskell) {
			kinit = (KifCallFunctionHaskell*)kret->Initialisation()->Instruction(0);
			//we look for the most embedded call...
			if (kinit->body->lambdadomain->instructions.size() == 0 && kinit->body->Instruction(0)->Parameter(0)->type == kifCallFunctionHaskell)
				kinit = (KifCallFunctionHaskell*)kinit->body->Instruction(0)->Parameter(0);
		}

		if (kinit != NULL && kinit->body->lambdadomain->instructions.size() != 0) {
			//First we copy all our substructures into our main structure...
			kf->body->lambdadomain->instructions = kinit->body->lambdadomain->instructions;
			kf->body->lambdadomain->declarations = kinit->body->lambdadomain->declarations;
			kf->body->lambdadomain->local = kinit->body->lambdadomain->local;
			kf->body->arguments = kinit->body->arguments;
			kf->body->declarations = kinit->body->declarations;
			kf->body->instructions = kinit->body->instructions;
			kf->declarations = kinit->declarations;
			char adding = 0;
			if (kf != kbase) {
				adding = 2;
				kbase->Add(kf);
			}
			KifElement* kvar = kret;
			kvar->Seteval(200); //we deactivate the initialisation for this variable from which we stolen its main structures...
			//if adding is two, then the return section in kinit will be duplicated and left intact...
			kret = KifParseReturnValue(xn, kfunc, adding);
			if (kret != NULL) {
				nvar.value = "&return;";
				nname->value = "&return;";
			}
		}
		else {
			lambdadom->instructions.clear();
			//we need first to create a variable...
			KifBrowseVariable(&nvar, lambdadom);
			lambdadom->Add(kret);
			lambdadom->Add(kifFALSE);
			//This variable becomes our current variable...

			kret = KifParseReturnValue(xn, kfunc);
			KifBrowse(&nvar, kret);
			if (kf != kbase)
				kbase->Add(kf);
		}
	}
	else {
		KifParseReturnValue(xn, kfunc);
		nvar.value = "&return;";
		nname->value = "&return;";
	}

	x_node nvardrop;

	if (xn->nodes[0]->value == KIFLIBSTRINGS[340]) {
		nvardrop.token = "variable";
		nvardrop.value = "&drop;";
		nvardrop.start = xn->start;
		nvardrop.end = xn->end;
		x_node* nnamedrop = new x_node;
		nvardrop.nodes.push_back(nnamedrop);
		nnamedrop->token = "word";
		nnamedrop->value = "&drop;";
		int iddrop = KifNewId(nvardrop.value);
		KifElement* var = new KifFunctionDeclaration(this, NULL, iddrop, kifMINUSONE);
		lambdadom->Set(iddrop, var, NULL);
		var = new KifBoolean(this, var, true);
		lambdadom->local = true;
	}

	//We remove the last instruction, to insert it into our test
	kret = kfunc->instructions.back();
	kfunc->instructions.pop_back();
	KifInstructionIF* ktest;

	//The return statement should be removed and replaced
	if (xn->nodes[0]->value == KIFLIBSTRINGS[341] || xn->nodes[0]->value == KIFLIBSTRINGS[342]) {//In that case, we need to count the number of elements that were used so far...
		//First we need to declare a variable which will be used as a counter...
		nvar.value = "&counter;"; //Our counter
		nname->value = "&counter;";
		int idcounter = KifNewId(nvar.value);
		KifElement* var = new KifFunctionDeclaration(this, NULL, idcounter, kifMINUSONE);
		lambdadom->Set(idcounter, var, NULL);
		var = new KifBaseInteger(this, var);
		lambdadom->local = true;
		//We add a PLUSPLUS to increment our value...
		KifInstruction* ki = KifCreateInstruction(kfunc, kifPLUSPLUS);
		KifBrowse(&nvar, ki);
		//Then we need to add our test
		ktest = new KifInstructionIF(this, kfunc);
		if (xn->nodes[0]->value == KIFLIBSTRINGS[342])
			ki = KifCreateInstruction(ktest, kifSUPERIOR);
		else
			ki = KifCreateInstruction(ktest, kifINFEQU);
		KifBrowse(&nvar, ki);
		KifBrowse(xn->nodes[1], ki);
	}
	else {
		ktest = new KifInstructionIF(this, kfunc);
		//The only difference is that we process a Boolean expression in a filter
		if (xn->nodes[1]->token == "comparison" || xn->nodes[1]->token == "operationin") {
			KifInstruction* ki = KifCreateInstruction(NULL, kifBLOC);
			KifBrowse(&nvar, ki); // we add our variable to compare with
			KifBrowse(xn->nodes[1], ki); // we add our comparison operator with its value...

			if (ki->action == kifBLOC && ki->instructions.size() == 1) {
				ktest->Add(ki->instructions[0]);
				ki->Release();
			}
			else {
				ki = CloningInstruction(ki);
				ktest->Add(ki);
			}
		}
		else {
			if (xn->nodes[1]->token == "hboollambda") {
				KifElement* kprevious = KifParseLambdaFunction(xn->nodes[1], kfunc);
				KifCallFunctionGeneral* kcall = new KifCallFunctionGeneral(this, ktest, kprevious->Name(), (KifFunction*)kprevious);
				KifBrowse(&nvar, kcall);
			}
			else {
				x_node* nop = new x_node;
				nop->start = xn->start;
				nop->end = xn->end;
				nop->token = "haskellcall";
				x_node* nfunc = new x_node;
				nfunc->token = "functioncall";
				nfunc->value = xn->nodes[1]->nodes[0]->value;
				nfunc->start = xn->start;
				nfunc->end = xn->end;
				nfunc->nodes.push_back(xn->nodes[1]->nodes[0]);
				nop->nodes.push_back(nfunc);
				x_node* param = new x_node;
				param->token = "parameters";
				nop->nodes.push_back(param);
				param->nodes.push_back(&nvar);

				x_node* prev = param;
				vector<x_node*> v;
				v.push_back(param);
				for (int i = 1; i<xn->nodes[1]->nodes.size(); i++) {
					param = new x_node;
					param->token = "parameters";
					param->nodes.push_back(xn->nodes[1]->nodes[i]);
					prev->nodes.push_back(param);
					v.push_back(param);
					prev = param;
				}

				KifBrowse(nop, ktest);
				for (int i = 0; i<v.size(); i++) {
					v[i]->nodes.clear();
					delete v[i];
				}
				nfunc->nodes.clear();
				nop->nodes.pop_back();
				delete nop;
			}
		}
	}

	if (xn->nodes[0]->value == KIFLIBSTRINGS[340]) {
		//Then, in that case, when the test is positive, we return kifNULL else the value
		//First we modify the test in ktest...
		//We need to use a Boolean (&drop;) which will be set to false, when the test will be true...
		KifInstruction* ki = KifCreateInstruction(NULL, kifAND);
		KifBrowse(&nvardrop, ki);
		ki->Add(ktest->instructions[0]);
		ktest->instructions[0] = ki;
		KifCallReturn* kretdrop = new KifCallReturn(this, ktest, kifGlobals->idreturn);
		kretdrop->add = 1;
		kretdrop->Add(kifNULL);
		//We need now a sequence of instructions
		KifInstructionSEQUENCE* kseq = new KifInstructionSEQUENCE(this, ktest, -1);
		//the variable &drop; is set to false
		ki = KifCreateInstruction(kseq, kifAFFECTATION);
		KifBrowse(&nvardrop, ki);
		ki->Add(kifFALSE);
		//we add our return value...
		kseq->Add(kret);
	}
	else {
		ktest->Add(kret); //We add the value to return if test is positive
		if (xn->nodes[0]->value != KIFLIBSTRINGS[343] && xn->nodes[0]->value != KIFLIBSTRINGS[342])
			kret = new KifCallFunctionBreakOuContinue(this, ktest, kifBREAK);
	}
	kifGlobals->PopStack();
	kifGlobals->PopStack();
	return kfunc;

}

//-------------------------------------------------------------------------------------------------------------------------
KifElement* KifCode::KifParseHaskellFlip(x_node* xn, KifElement* kbase) {
	KifCallFunctionHaskell* kf;
	if (kbase->type == kifCallFunctionHaskell) {
		kf = (KifCallFunctionHaskell*)kbase;
	}
	else {
		kf = new KifCallFunctionHaskell(this, NULL);
		kf->Init(NULL);
	}
	KifFunctionLambda* kfunc = kf->body;
	KifDomainLambda* lambdadom = kfunc->lambdadomain;
	kifGlobals->PushStack(kfunc);
	kifGlobals->PushStack(lambdadom);

	int i;
	KifInstruction kvar(NULL, NULL, kifBLOC);
	kvar.kifcode = this;


	kfunc->choice = 1;
	KifElement* kret = KifParseReturnValue(xn, kfunc);

	if (xn->nodes[0]->token == "operator") {
		KifOperator op = kifGlobals->kifOperators[xn->nodes[0]->value];
		KifInstruction* ki = KifCreateInstruction(NULL, op);
		KifBrowse(xn->nodes[2], ki);
		KifBrowse(xn->nodes[1], ki);
		kret->Add(ki);
	}
	else {
		if (xn->nodes[0]->token == "hlambda") {
			KifElement* kprevious = KifParseLambdaFunction(xn->nodes[0], kfunc);
			KifCallFunctionGeneral* kcall = new KifCallFunctionGeneral(this, kret, kprevious->Name(), (KifFunctionLambda*)kprevious);
			KifBrowse(xn->nodes[2], kcall);
			KifBrowse(xn->nodes[1], kcall);
		}
		else {
			x_node* nop = new x_node;
			nop->start = xn->start;
			nop->end = xn->end;
			nop->token = "haskellcall";
			x_node* nfunc = new x_node;
			nfunc->token = "functioncall";
			nfunc->value = xn->nodes[0]->nodes[0]->value;
			nfunc->start = xn->start;
			nfunc->end = xn->end;
			nfunc->nodes.push_back(xn->nodes[0]->nodes[0]);
			nop->nodes.push_back(nfunc);
			x_node* prev = NULL;
			x_node* param;
			vector<x_node*> v;
			for (i = 2; i >= 1; i--) {
				param = new x_node;
				param->token = "parameters";
				param->nodes.push_back(xn->nodes[i]);
				v.push_back(param);
				if (prev == NULL)
					nop->nodes.push_back(param);
				else
					prev->nodes.push_back(param);
				prev = param;
			}

			KifBrowse(nop, kret);
			nfunc->nodes.clear();
			for (i = 0; i < v.size(); i++) {
				v[i]->nodes.clear();
				delete v[i];
			}

			nop->nodes.pop_back();
			delete nop;
		}
	}
	kifGlobals->PopStack();
	kifGlobals->PopStack();
	return kfunc;
}

KifElement* KifCode::KifParseHaskellZip(x_node* xn, KifElement* kbase) {
	KifCallFunctionHaskell* kf;
	if (kbase->type == kifCallFunctionHaskell) {
		kf = (KifCallFunctionHaskell*)kbase;
	}
	else {
		kf = new KifCallFunctionHaskell(this, NULL);
		kf->Init(NULL);
	}
	KifFunctionLambda* kfunc = kf->body;
	KifDomainLambda* lambdadom = kfunc->lambdadomain;
	kifGlobals->PushStack(kfunc);
	kifGlobals->PushStack(lambdadom);

	vector<x_node*> nvars;
	char buff[50];
	int first = 1;
	int i;
	if (xn->token == "pairing")
		first = 0;

	for (i = first; i<xn->nodes.size(); i++) {
		//we need first to create a variable...
		x_node* nvar = new x_node;
		nvar->start = xn->start;
		nvar->end = xn->end;
		nvar->token = "variable";
		sprintf_s(buff, 50, "&zipping%d", i);
		nvar->value = buff;
		x_node* nname = new x_node;
		nvar->nodes.push_back(nname);
		nname->token = "word";
		nname->value = buff;

		KifBrowseVariable(nvar, lambdadom);
		KifBrowse(xn->nodes[i], lambdadom);
		if (i == first)
			lambdadom->Add(kifFALSE);
		else
			lambdadom->Add(kifZERO);
		nvars.push_back(nvar);
	}

	kfunc->choice = 1;
	KifElement* kret = KifParseReturnValue(xn, kfunc);

	if (!first) {
		KifBaseVector* kvect = new KifBaseVector(this, kret);
		kvect->evaluate = true;
		for (i = 0; i<nvars.size(); i++) {
			KifBrowse(nvars[i], kvect);
			delete nvars[i];
		}
	}
	else {
		if (xn->nodes[0]->token == "operator") {
			KifOperator op = kifGlobals->kifOperators[xn->nodes[0]->value];
			KifInstruction* ki = KifCreateInstruction(NULL, op);
			for (i = 0; i < nvars.size(); i++) {
				KifBrowse(nvars[i], ki);
				delete nvars[i];
			}
			kret->Add(ki);
		}
		else {
			if (xn->nodes[0]->token == "hlambda") {
				KifElement* kprevious = KifParseLambdaFunction(xn->nodes[0], kfunc);
				KifCallFunctionGeneral* kcall = new KifCallFunctionGeneral(this, kret, kprevious->Name(), (KifFunctionLambda*)kprevious);
				for (i = 0; i < nvars.size(); i++) {
					KifBrowse(nvars[i], kcall);
					delete nvars[i];
				}
			}
			else {
				x_node* nop = new x_node;
				nop->start = xn->start;
				nop->end = xn->end;
				nop->token = "haskellcall";
				x_node* nfunc = new x_node;
				nfunc->token = "functioncall";
				nfunc->value = xn->nodes[0]->nodes[0]->value;
				nfunc->start = xn->start;
				nfunc->end = xn->end;
				nfunc->nodes.push_back(xn->nodes[0]->nodes[0]);
				nop->nodes.push_back(nfunc);
				x_node* prev = NULL;
				x_node* param;
				vector<x_node*> v;
				for (i = 0; i < nvars.size(); i++) {
					param = new x_node;
					param->token = "parameters";
					v.push_back(param);
					param->nodes.push_back(nvars[i]);
					if (!i)
						nop->nodes.push_back(param);
					else
						prev->nodes.push_back(param);
					prev = param;
				}
				for (i = 1; i < xn->nodes[0]->nodes.size(); i++) {
					param = new x_node;
					param->token = "parameters";
					param->nodes.push_back(xn->nodes[0]->nodes[i]);
					v.push_back(param);
					prev->nodes.push_back(param);
					prev = param;
				}
				KifBrowse(nop, kret);
				nfunc->nodes.clear();
				for (i = 0; i<v.size(); i++) {
					v[i]->nodes.clear();
					delete v[i];
				}

				nop->nodes.pop_back();
				delete nop;
			}
		}
	}
	kifGlobals->PopStack();
	kifGlobals->PopStack();
	return kfunc;
}

//----------------------------------------------------------------------------------------------------

KifElement* KifCode::KifParseHaskellCycling(x_node* xn, KifElement* kbase) {//Cycling in a list...
	KifCallFunctionHaskell* kf;
	if (kbase->type == kifCallFunctionHaskell)
		kf = (KifCallFunctionHaskell*)kbase;
	else {
		kf = new KifCallFunctionHaskell(this, kbase);
		kf->Init(NULL);
	}
	KifFunctionLambda* kfunc = kf->body;
	KifDomainLambda* lambdadom = kfunc->lambdadomain;

	x_node nvar;
	nvar.token = "variable";
	nvar.value = "&common;";
	nvar.start = xn->start;
	nvar.end = xn->end;
	x_node* nname = new x_node;
	nvar.nodes.push_back(nname);
	nname->token = "word";
	nname->value = "&common;";

	kifGlobals->PushStack(kfunc);
	kifGlobals->PushStack(lambdadom);

	KifElement* kret;
	//We parse our KIFLIBSTRINGS[339] value
	kf->add = 3;
	KifBrowse(xn->nodes[1], kf);
	//We are dealing with a simple return function
	//The value is stored in a variable, which is one step before the return statement
	//We store the calculus in a intermediary variable, whose name is &return; and which is created
	//with KifParseReturnValue function...
	if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
		kret = kfunc->instructions.back()->Parameter(0);
		if (kret->type == kifCallFunctionHaskell) {
			kifGlobals->PopStack();
			kifGlobals->PopStack();
			kf = (KifCallFunctionHaskell*)kret;
			kfunc = kf->body;
			kfunc->choice = 1;
			lambdadom = kfunc->lambdadomain;
			kifGlobals->PushStack(kfunc);
			kifGlobals->PushStack(lambdadom);
		}
	}

	//No composition
	if (lambdadom->instructions.size() == 1) {
		kret = lambdadom->instructions[0];
		KifCallFunctionHaskell* kinit = NULL;
		if (kret->Initialisation()->Instruction(0)->type == kifCallFunctionHaskell) {
			kinit = (KifCallFunctionHaskell*)kret->Initialisation()->Instruction(0);
			//we look for the most embedded call...
			if (kinit->body->lambdadomain->instructions.size() == 0 && kinit->body->Instruction(0)->Parameter(0)->type == kifCallFunctionHaskell)
				kinit = (KifCallFunctionHaskell*)kinit->body->Instruction(0)->Parameter(0);
		}

		if (kinit != NULL && kinit->body->lambdadomain->instructions.size() != 0) {
			//First we copy all our substructures into our main structure...
			kf->body->lambdadomain->instructions = kinit->body->lambdadomain->instructions;
			kf->body->lambdadomain->declarations = kinit->body->lambdadomain->declarations;
			kf->body->lambdadomain->local = kinit->body->lambdadomain->local;
			kf->body->arguments = kinit->body->arguments;
			kf->body->declarations = kinit->body->declarations;
			kf->body->instructions = kinit->body->instructions;
			kf->declarations = kinit->declarations;
			char adding = 0;
			if (kf != kbase) {
				adding = 2;
				kbase->Add(kf);
			}
			KifElement* kvar = kret;
			kvar->Seteval(200); //we deactivate the initialisation for this variable from which we stolen its main structures...
			//if adding is two, then the return section in kinit will be duplicated and left intact...
			kret = KifParseReturnValue(xn, kfunc, adding);
			if (kret != NULL) {
				nvar.value = "&return;";
				nname->value = "&return;";
			}
		}
		else {
			lambdadom->instructions.clear();
			//we need first to create a variable...
			KifBrowseVariable(&nvar, lambdadom);
			lambdadom->Add(kret);
			lambdadom->Add(kifFALSE);

			kret = KifParseReturnValue(xn, kfunc, 1);

			//This variable becomes our current variable...
			if (kf != kbase)
				kbase->Add(kf);
		}
	}
	else {
		kret = KifParseReturnValue(xn, kfunc, 1);
		if (kret == NULL) {
			//Then this is a filter..., A if is detected as a last element in the function instructions set...
			//We create a new KifCallFunctionHaskell, in which we copy all that was computed with xn->nodes[1]
			KifCallFunctionHaskell* kcf = new KifCallFunctionHaskell(this, NULL);
			kcf->Init(NULL);
			kcf->body->lambdadomain->instructions = lambdadom->instructions;
			kcf->body->lambdadomain->declarations = lambdadom->declarations;
			kcf->body->lambdadomain->local = lambdadom->local;
			kcf->body->declarations = kfunc->declarations;
			kcf->body->instructions = kfunc->instructions;

			//We clear our current structure, in which we create a CALL to that filter...
			kcf->declarations = kf->declarations;
			lambdadom->instructions.clear();
			lambdadom->declarations.clear();
			lambdadom->local = false;
			kfunc->instructions.clear();
			kfunc->declarations.clear();

			//The second element, which is the list on which we compute, is then stored
			//in our structure...
			KifBrowseVariable(&nvar, lambdadom);
			lambdadom->Add(kcf);
			lambdadom->Add(kifFALSE);

			kret = KifParseReturnValue(xn, kfunc, 1);
		}
		else {
			nvar.value = "&return;";
			nname->value = "&return;";
		}
	}

	//Now we modify lambdadom a little bit...
	if (xn->nodes[0]->value == KIFLIBSTRINGS[344])
		lambdadom->instructions[1] = new KifCycleVector(this, NULL, lambdadom->instructions[1], true);
	else
	if (xn->nodes[0]->value == KIFLIBSTRINGS[345])
		lambdadom->instructions[1] = new KifCycleVector(this, NULL, lambdadom->instructions[1], false);
	else {
		lambdadom->instructions[1] = new KifReplicateVector(this, NULL, lambdadom->instructions[1]);
		KifBrowse(xn->nodes[0], lambdadom->instructions[1]);
	}

	//we simply return our value...
	KifBrowse(&nvar, kret);
	kifGlobals->PopStack();
	kifGlobals->PopStack();
	return kfunc;
}
//-------------------------------------------------------------------------------------------------------------------------
KifElement* KifCode::KifParseHaskellCase(x_node* xn, KifElement* kf) {
	KifElement* kres = NULL;
	KifInstructionIF* ktest = NULL;
	KifInstruction* thetest;
	KifElement* kfirst;
	KifFunction* kfunc = NULL;
	KifCallReturn* kret;

	for (int i = 1; i<xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "haskellcaseotherwise")
			break;

		if (xn->nodes[i]->nodes[0]->token == "valvectortail") {
			//In this case, we need to create a function, in which everything is going to get executed
			//We might need to declare variables, hence this function declaration...
			if (kfunc == NULL) {
				kfunc = new KifFunction(this, kf, KIFCASEFUNCTION);
				kfunc->adding = true;
				kifGlobals->PushStack(kfunc);
			}
			KifBrowseVariableVector(xn->nodes[i]->nodes[0], kfunc);
		}

		if (xn->nodes[i]->nodes[0]->token == "valmaptail") {
			//In this case, we need to create a function, in which everything is going to get executed
			//We might need to declare variables, hence this function declaration...
			if (kfunc == NULL) {
				kfunc = new KifFunction(this, kf, KIFCASEFUNCTION);
				kfunc->adding = true;
				kifGlobals->PushStack(kfunc);
			}
			KifBrowseVariableMap(xn->nodes[i]->nodes[0], kfunc);
		}
	}

	for (int i = 1; i<xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "haskellcaseotherwise") {
			if (kfunc == NULL)
				KifBrowse(xn->nodes[i]->nodes[0], ktest); //then the value
			else {//In the case of a function we need to officially return the value...
				kret = new KifCallReturn(this, ktest, kifGlobals->idreturn);
				thetest = KifCreateInstruction(NULL, kifBLOC);
				KifBrowse(xn->nodes[i]->nodes[0], thetest);

				if (thetest->action == kifBLOC && thetest->instructions.size() == 1) {
					kret->Add(thetest->instructions[0]);
					thetest->instructions.clear();
					thetest->Release();
				}
				else {
					thetest = CloningInstruction(thetest);
					kret->Add(thetest);
				}
			}
			break;
		}
		//we create a test
		ktest = new KifInstructionIF(this, kres);
		if (kres == NULL)
			kfirst = ktest;
		//It should a be match between two values, it will be a match
		thetest = KifCreateInstruction(ktest, kifMATCH);
		KifBrowse(xn->nodes[0], thetest);
		KifBrowse(xn->nodes[i]->nodes[0], thetest); //the comparison
		if (kfunc == NULL)
			KifBrowse(xn->nodes[i]->nodes[1], ktest); //then the value
		else {//In the case of a function we need to officially return the value...
			kret = new KifCallReturn(this, ktest, kifGlobals->idreturn);
			thetest = KifCreateInstruction(NULL, kifBLOC);
			KifBrowse(xn->nodes[i]->nodes[1], thetest);
			if (thetest->action == kifBLOC && thetest->instructions.size() == 1) {
				kret->Add(thetest->instructions[0]);
				thetest->instructions.clear();
				thetest->Release();
			}
			else {
				thetest = CloningInstruction(thetest);
				kret->Add(thetest);
			}
		}
		kres = ktest; //then add the following case as a else in the expression
	}

	if (kfunc == NULL) //No function, we can add our structure directly...
		kf->Add(kfirst);
	else {
		//We then transform the declarations into instructions
		bin_hash<KifElement*>::iterator it;
		for (it = kfunc->declarations.begin(); it != kfunc->declarations.end(); it++) {
			if (it->first != KIFCASEFUNCTION)
				kfunc->instructions.push_back(it->second);
		}
		kfunc->instructions.push_back(kfirst);
		kifGlobals->PopStack();
	}

	return kf;
}
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//--- MAIN COMPILING METHOD FOR HASKELL AND LAMBDA
//-------------------------------------------------------------------------------------------------------------------------


KifElement* KifCode::KifParseLambda(x_node* xn, KifElement* kf) {
	char buff[100];
	sprintf_s(buff, 100, "&lambda%d;", kifGlobals->kifStringId.size());
	int idname = KifNewId(buff);
	KifFunction* kfunc = new KifFunction(this, kf, idname);
	kifGlobals->PushStack(kfunc);
	KifBrowse(xn->nodes[0], kfunc);
	kfunc->choice = 1;
	KifBrowse(xn->nodes[1], kfunc);
	kifGlobals->PopStack();
	return kfunc;
}

KifElement* KifCode::KifParseEvalLambda(x_node* xn, KifElement* kf) {
	KifInstruction kint(NULL, NULL);
	kint.kifcode = this;
	KifBrowse(xn->nodes[0], &kint);
	KifBrowse(xn->nodes[1], &kint);
	kf->Add(kint.instructions[0]);
	kf = kint.instructions[0];
	return kf;
}

KifElement* KifCode::KifParseHaskell(x_node* xn, KifElement* kf) {
	//We deactivate temporarily the instance recording...
	derecording rec(this);

	KifCallFunctionHaskell* kint = NULL;

	if (kf->type == kifCallFunctionHaskell)
		kint = (KifCallFunctionHaskell*)kf;


	//hmetafunctions: filter, map, zipWith, takeWhile, scan, fold
	if (xn->nodes.size() && xn->nodes[0]->token == "hmetafunctions") {
		if (kint == NULL)
			kint = new KifCallFunctionHaskell(this, kf);
		kint->Init(NULL);
		return KifBrowse(xn->nodes[0]->nodes[0], kint);
	}

	short idname;
	int i;
	KifFunctionLambda* kfuncbase = NULL;

	int first = 0;

	KifElement* kprevious = NULL;

	if (xn->nodes[0]->token == "haskell") {
		string name = xn->nodes[0]->nodes[0]->value;
		idname = KifNewId(name);
		long line = GetCurrentLine(0, xn);
		if (kifGlobals->KifBaseFunctionTest(idname) != NULL && kifGlobals->KifConversion(name) == false) {
			if (kifGlobals->kifMethods.find(idname) != kifGlobals->kifMethods.end()) {
				string message = KIFLIBSTRINGS[346] + name + "'";
				throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
			}
		}

		kprevious = kf->Declaration(idname);
		if (kprevious != NULL) {
			if (kprevious->type != kifFunctionLambda) {
				string message = KIFLIBSTRINGS[347] + name + "'";
				throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
			}

			kfuncbase = new KifFunctionLambda(this, NULL, idname);
			//we keep the top function as the reference in the declarations list (which will be available throughout the stack)
			kfuncbase->declarations[idname] = kprevious;
			while (((KifFunction*)kprevious)->next != NULL)
				kprevious = ((KifFunction*)kprevious)->next;
			((KifFunction*)kprevious)->next = kfuncbase;
		}
		else {
			kfuncbase = new KifFunctionLambda(this, NULL, idname);
			kf->Set(idname, kfuncbase, NULL);
		}


		kfuncbase->choice = 0;
		short id;
		KifElement* var;


		first = 1;
		for (i = 1; i<xn->nodes[0]->nodes.size(); i++) {
			if (xn->nodes[0]->nodes[i]->token == "word") {
				id = KifNewId(xn->nodes[0]->nodes[i]->value);
				var = new KifFunctionDeclaration(this, kfuncbase, id, kfuncbase);
				kfuncbase->Set(id, var, NULL);
				var = new KifSelf(this, var, kifNULL);
			}
			else {//it is a haskellvector...
				KifBrowse(xn->nodes[0]->nodes[i], kfuncbase);
			}
		}
		kifGlobals->PushStack(kfuncbase);
		kfuncbase->choice = 1;
		if (kint == NULL) //it is a function declaration, we create it out of the main loop...
			kint = new KifCallFunctionHaskell(this, NULL);
		kint->Init(kfuncbase);
		if (xn->nodes.size() == 2) {//we do not have a range or a Boolean expression			
			if (xn->nodes[1]->token == "hmetafunctions") {
				KifBrowse(xn->nodes[1], kint);
				kifGlobals->PopStack();
				return kf;
			}

			KifElement* ki = KifParseReturnValue(xn, kfuncbase);
			KifInstruction kbloc(NULL, NULL, kifBLOC);
			kbloc.kifcode = this;

			KifBrowse(xn->nodes[1], &kbloc); //compiling a 'guard' section
			ki->Add(kbloc.instructions[0]);

			//In that case, we do not need anything else...
			kifGlobals->PopStack();
			return kint;
		}
	}

	//It is a direct call then
	if (kint == NULL)
		kint = new KifCallFunctionHaskell(this, kf);

	kint->Init(NULL);
	KifFunctionLambda* kfunc = kint->body;
	KifDomainLambda* lambdadom = kfunc->lambdadomain;

	kint->Setline(linereference + GetCurrentLine(1, xn), kifGlobals->debuginfocurrent->currentfile);
	kint->body->Setline(linereference + GetCurrentLine(1, xn), kifGlobals->debuginfocurrent->currentfile);
	kint->body->choice = 1;

	bool addreturn = false;
	KifElement* kret = NULL;
	kifGlobals->PushStack(kint);
	kifGlobals->PushStack(lambdadom);
	kifGlobals->PushStack(kfunc);

	//Mapping implementation equivalent to:   < operation with &mapping; | &mapping; <- expressions >;

	vector<KifElement*> vars;
	bool conjunction = false;
	int inc = 1;
	string tok;
	//We handle these expressions first, as they declared variables for every other expressions...
	for (i = first; i<xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "letmin") {
			addreturn = true;
			inc = 0;
		}

		if (xn->nodes[i]->token == "whereexpression") {
			KifBrowse(xn->nodes[i], kint);
			kint->created.clear();
		}
	}

	for (i = first + inc; i<xn->nodes.size(); i++) {
		tok = xn->nodes[i]->token;
		if (tok == "whereexpression")
			continue;

		if (tok == "hoperator") {
			conjunction = false;
			if (xn->nodes[i]->value == ";")
				conjunction = true;
			continue;
		}

		if (tok == "letmin") {
			KifBrowse(xn->nodes[i], kfunc);
			vars.push_back(kfunc->instructions.back());
			continue;
		}

		if (tok == "hinexpression") {
			for (int j = 0; j<vars.size(); j++) {
				if (KifCheckingVariableName(xn->nodes[i], vars[j]))
					vars[j]->Seteval(0);
				vars.clear();
			}
			kret = KifParseReturnValue(xn, kfunc, 1);
			KifBrowse(xn->nodes[i], kret);
			continue;
		}

		if (tok == "booleanexpression") {
			kfunc->choice = 1;
			//We add our return statement with our variable...
			KifElement* ktest = new KifInstructionIF(this, kfunc);
			KifBrowse(xn->nodes[i], ktest);
			KifElement* kobj = ktest;
			bool neg = ktest->Instruction(0)->Negation();
			KifElement* nxt;
			while (kobj != NULL && kobj->InstructionSize() == 1) {
				if (kobj->Negation())
					neg = true;
				nxt = kobj;
				kobj = (KifObject*)kobj->Instruction(0);
				if (nxt != ktest) {
					nxt->InstructionClear();
					nxt->Release();
				}
			}
			if (kobj != ktest->Instruction(0))
				((KifObjectInstruction*)ktest)->instructions[0] = kobj;
			ktest->Setnegation(neg);

			//In order to add it to the test return on true
			kret = new KifCallReturn(this, ktest, kifGlobals->idreturn);
			continue;
		}


		if (tok == "range") {
			if (xn->nodes[i]->nodes[0]->token == "let") {
				if (xn->nodes[i]->nodes[0]->nodes[0]->value == KIFLIBSTRINGS[348]) {
					x_node* val = xn->nodes[i]->nodes[0]->nodes[2];
					xn->nodes[i]->nodes[0]->nodes.pop_back();
					KifBrowse(xn->nodes[i]->nodes[0], lambdadom); //an iterator variable...
					KifBrowse(val, lambdadom); //The container
					if (KifCheckVariable(val, lambdadom)) {
						if (conjunction)
							lambdadom->Add(kifONE);
						else
							lambdadom->Add(kifTRUE);
					}
					else {
						if (conjunction)
							lambdadom->Add(kifZERO);
						else
							lambdadom->Add(kifFALSE);
					}
				}
				else {
					if (xn->nodes[i]->nodes[0]->nodes[1]->token == "valvectortail") {
						int lst = kfunc->instructions.size();
						KifBrowseVariableVector(xn->nodes[i]->nodes[0]->nodes[1], kfunc);
						for (; lst<kfunc->instructions.size(); lst++)
							vars.push_back(kfunc->instructions[lst]);

						delete xn->nodes[i]->nodes[0]->nodes[0];
						xn->nodes[i]->nodes[0]->nodes.erase(xn->nodes[i]->nodes[0]->nodes.begin());
						xn->nodes[i]->nodes[0]->token = "affectation";
						KifBrowse(xn->nodes[i]->nodes[0], kfunc);
					}
					else {
						if (xn->nodes[i]->nodes[0]->nodes[1]->token == "valmaptail") {
							int lst = kfunc->instructions.size();
							KifBrowseVariableMap(xn->nodes[i]->nodes[0]->nodes[1], kfunc);
							for (; lst < kfunc->instructions.size(); lst++)
								vars.push_back(kfunc->instructions[lst]);

							delete xn->nodes[i]->nodes[0]->nodes[0];
							xn->nodes[i]->nodes[0]->nodes.erase(xn->nodes[i]->nodes[0]->nodes.begin());
							xn->nodes[i]->nodes[0]->token = "affectation";
							KifBrowse(xn->nodes[i]->nodes[0], kfunc);
						}
						else {
							KifBrowse(xn->nodes[i]->nodes[0], kfunc); //a let, a simple variable affectation...
							vars.push_back(kfunc->instructions.back());
						}
					}
				}
				continue;
			}

			if (xn->nodes[i]->nodes[0]->token == "valvectortail") {
				lambdadom->adding = false;
				lambdadom->local = true;
				KifBrowseVariableVector(xn->nodes[i]->nodes[0], lambdadom);
				lambdadom->adding = true;
				KifBrowse(xn->nodes[i]->nodes[0], lambdadom);
			}
			else {
				if (xn->nodes[i]->nodes[0]->token == "valmaptail") {
					lambdadom->adding = false;
					lambdadom->local = true;
					KifBrowseVariableMap(xn->nodes[i]->nodes[0], lambdadom);
					lambdadom->adding = true;
					KifBrowse(xn->nodes[i]->nodes[0], lambdadom);
				}
				else
					KifBrowseVariable(xn->nodes[i]->nodes[0], lambdadom);
			}

			if (xn->nodes[i]->nodes[1]->token == "hmetafunctions") {
				KifCallFunctionHaskell* klocal = new KifCallFunctionHaskell(this, lambdadom);
				klocal->Init(NULL);
				KifBrowse(xn->nodes[i]->nodes[1], klocal);
			}
			else
				KifBrowse(xn->nodes[i]->nodes[1], lambdadom);
			if (KifCheckVariable(xn->nodes[i]->nodes[1], lambdadom)) {
				if (conjunction)
					lambdadom->Add(kifONE);
				else
					lambdadom->Add(kifTRUE);
			}
			else {
				if (conjunction)
					lambdadom->Add(kifZERO);
				else
					lambdadom->Add(kifFALSE);
			}
		}
	}

	if (!addreturn) {
		if (kret == NULL)
			//we create one, whose value is returned by our return(value) that we add to the end of the function
			kret = KifParseReturnValue(xn, kfunc);

		if (kret != NULL) {
			//when a variable is requested in the KIFLIBSTRINGS[289] structure, then it has to be reactivated, if it had been deactivated
			//as in <[a,b] | let a=[1..10], let b= filter (<4) a>... a is deactivated (eval is set to 200)
			//see KifVariableDeclaration::Create to see why we set eval to 200. It corresponds to a simple storage in a frame
			//without a call to initialisation. When reset to 0, then the initialisation can be activated again...
			//With this mechanism, we do not need to get rid of the specific code associated to such a variable...
			for (i = 0; i<vars.size(); i++) {
				if (KifCheckingVariableName(xn->nodes[first], vars[i]))
					vars[i]->Seteval(0);
			}
			KifInstruction kidxx(NULL, NULL, kifBLOC);
			kidxx.kifcode = this;
			KifBrowse(xn->nodes[first], &kidxx);
			kret->Add(kidxx.instructions[0]);
		}
	}

	kifGlobals->PopStack();
	kifGlobals->PopStack();
	kifGlobals->PopStack();
	if (first)
		//In that case, we need to copy onto the current function (see KIFAPPLY) all the variables that were declared
		//in funcbase, for them to be accessible from the code...			
		kifGlobals->PopStack();
	return kint;
}


//We preload our libraries...
bool KifCode::CheckUse(x_node* xn, KifElement* kf) {
	while (xn->nodes.size()) {
		if (xn->token == "regularcall") {
			if (xn->nodes[0]->token == "functioncall"	&& xn->nodes[0]->value == "use") {
				KifBrowse(xn, kf);
				xn->token[0] = '_';
				return true;
			}
		}
		xn = xn->nodes[0];
	}
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//We check for the existence of functions in this bloc in order to predeclare them
KifElement* KifCode::KifParsePredeclareFunctions(x_node* xn, KifElement* kf) {
	hmap<size_t, bool> skip;
	size_t i;
	KifElement* s;

	if (kf->type == kifFrame) {
		x_node* xend;
		x_node declaration_ending;
		declaration_ending.token = "declarationending";
		declaration_ending.value = ";";
		int last;
		//First we check all the functions present in the blocs and we predeclare them
		for (i = 0; i < xn->nodes.size(); i++) {

			if (CheckUse(xn->nodes[i], kf))
				continue;

			if (xn->nodes[i]->token == "bloc") {
				x_node* nsub = xn->nodes[i]->nodes[0];
				if (nsub->token == "predicatedefinition" || nsub->token == "predicatefact") {
					if (nsub->nodes[0]->token == "predicate") {
						short id = KifNewId(nsub->nodes[0]->nodes[0]->value);
						if (predicates.find(id) == predicates.end())
							predicates[id] = new KifPredicateFunction(this, NULL, NULL, id);
					}
				}
				continue;
			}

			if (xn->nodes[i]->token == "function" || xn->nodes[i]->token == "frame") {
				last = xn->nodes[i]->nodes.size() - 1;
				xend = xn->nodes[i]->nodes[last];
				if (xend->token == "declarationending") {
					//in this case, we do not need to take these predeclarations into account anymore...
					skip[i] = true;
					if ((kf->type != kifFrame || kf == kstart) && xn->nodes[i]->token == "function")
						continue;

					//we modify the value as a hint of a predeclared function for the actual building of that function
					xn->nodes[i]->nodes[last]->value = ".";
				}
				else
					xn->nodes[i]->nodes[last] = &declaration_ending;

				try {
					s = KifBrowse(xn->nodes[i], kf);
				}
				catch (KifRaiseError* m) {
					xn->nodes[i]->nodes[last] = xend;
					throw m;
				}
				xn->nodes[i]->nodes[last] = xend;
				//In this case, this an actual predeclaration from within a frame... We use a specific encoding
				//to isolate it later on... This can only happen in a frame...
				if (skip.find(i) != skip.end()) {
					((KifFunction*)s)->choice = 3;
					xn->nodes[i]->nodes[last]->value = ";";
				}
			}
		}
	}

	KifElement* ke = NULL;
	for (i = 0; i < xn->nodes.size(); i++) {
		if (skip.find(i) != skip.end())
			continue;
		s = KifBrowse(xn->nodes[i], kf);
		if (s != NULL)
			ke = s;
	}
	return ke;

}
//-------------------------------------------------------------------------------------------------------------------------
//Function creation
KifElement* KifCode::KifParseFunction(x_node* xn, KifElement* kf) {
	hmap<long, KifElement*> localinstances;
	short idname;
	string name = xn->nodes[1]->value;
	long line = GetCurrentLine(0, xn);
	if (kf->type == kifFunction) {
		string message = KIFLIBSTRINGS[349] + name + "'";
		throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
	}

	localinstances = instances;
	idname = KifNewId(name);

	if (kifGlobals->KifBaseFunctionTest(idname) != NULL && kifGlobals->KifConversion(name) == false) {
		if (kifGlobals->kifMethods.find(idname) != kifGlobals->kifMethods.end()) {
			instances = localinstances;
			string message = KIFLIBSTRINGS[346] + name + "'";
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
	}

	KifElement* kprevious = kf->Declaration(idname);
	if (kf != kstart)
		kifDeclaredFunction[idname] = true;

	int last = xn->nodes.size() - 1;


	KifFunction* kfunc = NULL;
	bool autorun = false;
	bool privatefunction = false;
	bool strictfunction = false;
	bool joinfunction = false;

	KifFunction* predeclared = NULL;

	if (kprevious != NULL) {
		if (kprevious->isFunction() == false) {
			string message = KIFLIBSTRINGS[319];
			message += name;
			message += KIFLIBSTRINGS[337];
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}

		//When value==".", it is an actual pre-declaration in a frame otherwise it is a type declaration scan... See KifParsePredeclareFunctions
		if (xn->nodes[last]->token == "declarationending" && xn->nodes[last]->value == ";") {
			kfunc = (KifFunction*)kprevious;
			while (kfunc != NULL) {
				if (kfunc->choice == 3) {
					predeclared = kfunc;
					break;
				}
				kfunc = kfunc->next;
			}
			kfunc = NULL;
		}

		if (predeclared == NULL) {
			//To cope with predeclarations, we need to use these predeclarations in the order in which they were found
			//When a predeclaration has been used, it is marked as declared (choice==1), we then jump to the next one
			while (kprevious != NULL && kprevious->Alreadydeclared())
				kprevious = ((KifFunction*)kprevious)->next;
		}
	}


	//Two cases:
	//If this is the first implementation of that function OR NO predeclaration had been issued OR it is not a function, then we create a new function
	if (kprevious == NULL || xn->nodes[last]->token == "declarationending") {

		string typefunction;
		int protection = 0;
		vector<x_node*>& xsub = xn->nodes[0]->nodes;
		int si = 0;
		if (xsub[si]->value == KIFLIBSTRINGS[350]) {
			joinfunction = true;
			si++;
		}

		if (xsub[si]->value == KIFLIBSTRINGS[351]) {
			protection = 1;
			si++;
		}

		if (xsub[si]->value == KIFLIBSTRINGS[352]) {
			protection = 2;
			si++;
		}

		if (xsub[si]->value == KIFLIBSTRINGS[324]) {
			privatefunction = true;
			si++;
		}
		if (xsub[si]->value == KIFLIBSTRINGS[353]) {
			strictfunction = true;
			si++;
		}
		if (xsub[si]->token == "functionsort")
			typefunction = xsub[si]->value;

		if (typefunction == KIFLIBSTRINGS[354])
			kfunc = new KifPolynomial(this, kf, idname);
		else
		if (typefunction == KIFLIBSTRINGS[355])
			kfunc = new KifThread(this, kf, idname, joinfunction, protection);
		else {
			if (protection != 0)
				kfunc = new KifProtectedFunction(this, kf, idname, protection);
			else
				kfunc = new KifFunction(this, kf, idname);
			if (typefunction == KIFLIBSTRINGS[356])
				autorun = true;
		}
	}

	//If we already have an implementation for that function, either it is a predeclaration, then we simply use it
	//Or this predeclaration is NOT a function
	if (kprevious != NULL && xn->nodes[last]->token != "declarationending") {
		if (kprevious->isFunction()) {
			if (kprevious->Predeclared()) {
				kfunc = (KifFunction*)kprevious; //we use the predeclaration as our new function
				kprevious = NULL;
			}//else, we will add this new function to a previous declaration...
			else
			if (kprevious->isUsed() == true)
				kprevious = NULL;
			else //if it has been implemented in the mother frame already, then it should not be attached to that set of functions
			if (kf != kprevious->Frame())
				kprevious = NULL;
		}
		else
			kprevious = NULL;
	}

	if (kfunc == NULL) {
		instances = localinstances;
		string message = KIFLIBSTRINGS[357] + name + "'";
		throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
	}

	kfunc->privatefunction = privatefunction;
	kfunc->strict = strictfunction;
	kifGlobals->PushStack(kfunc);
	kfunc->Setline(linereference + GetCurrentLine(1, xn), kifGlobals->debuginfocurrent->currentfile);

	if (xn->nodes[last]->token == "declarationending") {
		kfunc->choice = 0;
		//we process the arguments, if they are available
		if (last != 2)
			KifBrowse(xn->nodes[2], kfunc);
		kfunc->choice = 2;
	}
	else {
		if (xn->nodes[2]->token == "instruction") {
			kfunc->choice = 1;
			KifBrowse(xn->nodes[2], kfunc);
		}
		else {
			//If we have a predeclaration, then the arguments have already been parsed...
			if (kfunc->Predeclared() == false) {
				kfunc->choice = 0;
				KifBrowse(xn->nodes[2], kfunc);
			}
			kfunc->choice = 1;
			KifBrowse(xn->nodes[3], kfunc);
		}
	}

	if (kprevious != NULL) {
		bool found = false;
		//We need to consume it, an actual pre-declared function in a frame...
		if (predeclared != NULL) {
			if (predeclared->arguments.size() == kfunc->arguments.size()) {
				found = true;
				for (int arg = 0; arg < predeclared->arguments.size(); arg++) {
					if (predeclared->arguments[arg]->Linkedvalue()->type != kfunc->arguments[arg]->Linkedvalue()->type) {
						found = false;
						break;
					}
				}

				if (found) {
					if (kf->Declaration(idname) == NULL)
						kf->Set(idname, predeclared, NULL);
					predeclared->choice = 2;
					predeclared->declarations = kfunc->declarations;
					predeclared->arguments = kfunc->arguments;
					kfunc = predeclared;
				}
				else {
					string message = KIFLIBSTRINGS[358] + name + "'";
					throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
				}
			}
		}

		if (found == false)
			kprevious->Addfunction(kfunc);
	}
	else {
		if (kf->Declaration(idname) == NULL)
			kf->Set(idname, kfunc, NULL);
	}

	kifGlobals->PopStack();
	if (autorun && loader == NULL) {
		if (kfunc->arguments.size() != 0) {
			instances = localinstances;
			string message = KIFLIBSTRINGS[359] + name + "'";
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		if (kf == kstart || kf->type != kifFrame)
			new KifCallFunctionGeneral(this, kf, idname, kfunc);
		kfunc->autorun = true;
	}
	instances = localinstances;
	return kfunc;
}

//---------------------------------------------------------------------------------------------------------------
//This function will be call in case of an extension
KifElement* PreExtensions(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* ke = callfunc->Evaluatethread(0, dom, idthread);
	//callfunc contains the name of the function...
	int idtype = kifGlobals->equivalences[ke->type];
	KifElement* kfr;

	if (!callfunc->kifcode->kstart->declarations.check(idtype))
		return callfunc->kifcode->Returnerror(KIFLIBSTRINGS[360]);

	kfr = callfunc->kifcode->kstart->declarations[idtype];
	KifFrame* kframe = (KifFrame*)kfr;
	KifDomain* kdom = new KifDomain(NULL, kifNULL, idtype, kifDomain);
	kdom->kifcode = callfunc->kifcode;

	//We compute the name of the variable...
	idtype = kifGlobals->extensionvar[idtype];

	KifElement* kvar = kframe->declarations[idtype];
	kvar = kvar->Create(kdom, kvar, idthread, callfunc);
	kvar->Setframe(kdom);
	((KifSelf*)kvar)->value = ke;

	//We need now to get access to the function
	if (!kframe->declarations.check(callfunc->Name()))
		return callfunc->kifcode->Returnerror(KIFLIBSTRINGS[361]);

	kvar = kframe->declarations[callfunc->Name()];
	if (kvar->type != kifFunction) {
		kdom->Release();
		return callfunc->kifcode->Returnerror(KIFLIBSTRINGS[361]);
	}

	KifFunction* kfunc = (KifFunction*)kvar;
	KifCallFunctionGeneral kcf(NULL, NULL, kfunc->Name(), kfunc);
	for (int i = 0; i < kfunc->arguments.size(); i++) {
		ke = callfunc->Evaluatethread(i + 1, dom, idthread);
		kcf.keep(kfunc->arguments[i]->Name(), ke);
	}

	kifGlobals->PushFunctionStackid(idthread, &kcf, kdom);
	ke = kfunc->Exec(contextualpattern, kdom, kifNULL, idthread, &kcf);
	kifGlobals->PopFunctionStack(idthread);
	kdom->Release();
	return ke;
}

KifElement* KifCode::KifParseExtension(x_node* xn, KifElement* kf) {
	//We create an extension of a current type...
	//The name is the next parameter
	hmap<long, KifElement*> localinstances;
	localinstances = instances;

	int pos = 0;
	string nametype = xn->nodes[pos]->value;
	long line = GetCurrentLine(0, xn);

	string message;

	short idname;
	if (kifGlobals->kifTypeString.find(nametype) == kifGlobals->kifTypeString.end()) {
		message = KIFLIBSTRINGS[362] + nametype + "'";
		throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
	}

	idname = kifGlobals->kifTypeString[nametype];
	KifElement* ke = NULL;

	//We create a temporary frame to store the functions...
	KifFrame* kframe = new KifFrame(this, NULL, idname);
	kstart->Set(idname, kframe, NULL);

	//we first create a global variable, whose name is _name
	string name("_");
	name += nametype;
	x_node xself;
	xself.token = "multideclaration";
	x_node xtype;
	xtype.token = "type";
	xtype.value = KIFLIBSTRINGS[363];
	x_node xname;
	xname.token = "word";
	xname.value = name;
	xself.nodes.push_back(&xtype);
	xself.nodes.push_back(&xname);

	kifGlobals->PushStack(kframe);
	ke = KifBrowse(&xself, kframe);
	kifGlobals->extensionvar[idname] = ke->Name();
	xself.nodes.clear();
	KifBrowse(xn->nodes[pos + 1], kframe);
	for (int i = 0; i < kframe->instructions.size(); i++) {
		ke = kframe->instructions[i];
		if (ke->isFunction()) {
			name = KifIdString(ke->Name());
			if (kifGlobals->KifCreatePredefined(this, name, P_ATLEASTONE, &PreExtensions) == false) {
				kifGlobals->PopStack();
				instances = localinstances;
				message = KIFLIBSTRINGS[364] + name + "'";
				throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
			}

			switch (idname) {
			case kifString:
				InitMapBooleanString(kifGlobals->kifStringFunctions, name);
				break;
			case kifAutomaton:
				InitMapBooleanString(kifGlobals->kifAutomatonFunctions, name);
				break;
			case kifDate:
				InitMapBooleanString(kifGlobals->kifDateFunctions, name);
				break;
			case kifTime:
				InitMapBooleanString(kifGlobals->kifTimeFunctions, name);
				break;
			case kifFile:
				InitMapBooleanString(kifGlobals->kifFileFunctions, name);
				break;
			case kifInteger:
				InitMapBooleanString(kifGlobals->kifNumericalFunctions, name);
				break;
			case kifVector:
				InitMapBooleanString(kifGlobals->kifVectorFunctions, name);
				break;
			case kifList:
				InitMapBooleanString(kifGlobals->kifListFunctions, name);
				break;
			case kifMap:
				InitMapBooleanString(kifGlobals->kifMapFunctions, name);
				break;
			case kifSet:
				InitMapBooleanString(kifGlobals->kifSetFunctions, name);
				break;
			case kifTree:
				InitMapBooleanString(kifGlobals->kifTreeFunctions, name);
				break;
			default:
				kifGlobals->PopStack();
				instances = localinstances;
				message = KIFLIBSTRINGS[365] + nametype + "'";
				throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
			}
		}
	}

	kifGlobals->PopStack();
	instances = localinstances;
	return kf;
}

//---------------------------------------------------------------------------------------------------------------

KifElement* KifCode::KifParseFrame(x_node* xn, KifElement* kf) {
	//We create a frame
	//The name is the next parameter
	hmap<long, KifElement*> localinstances;
	localinstances = instances;

	bool privated = false;
	int pos = 0;
	if (xn->nodes[0]->token == "private") {
		privated = true;
		pos = 1;
	}
	string name = xn->nodes[pos]->value;
	short idname = KifNewId(name);
	KifFrame* kframe = NULL;
	KifElement* ke = NULL;

	if (kifGlobals->frames.find(idname) != kifGlobals->frames.end()) {
		ke = kifGlobals->frames[idname];
		if (privated) {
			instances = localinstances;
			long line = GetCurrentLine(0, xn);
			string message = KIFLIBSTRINGS[366] + name;
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
	}
	else
		ke = kf->Declaration(idname);

	if (ke == NULL) {
		if (privated)
			kframe = new KifFrame(this, kf, idname);
		else {
			kframe = new KifFrame(NULL, kf, idname);
			kframe->kifcode = this;
		}
	}
	else {
		if (ke->type == kifFrame) {
			kframe = (KifFrame*)ke;
			//if (kframe->declared==false)
			//	kframe=NULL;
		}
	}
	if (kframe == NULL) {
		instances = localinstances;
		long line = GetCurrentLine(0, xn);
		string message = KIFLIBSTRINGS[367] + name;
		throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
	}

	if (xn->nodes[pos + 1]->token == "declarationending") {
		instances = localinstances;
		kframe->declared = true;
		kf->Set(kframe->name, kframe, NULL);
		return kframe;
	}


	kifGlobals->PushStack(kframe);
	kframe->Setline(linereference + GetCurrentLine(0, xn), kifGlobals->debuginfocurrent->currentfile);

	//If it is a sub-frame definition
	if (kf->type == kifFrame && kstart != kf)
		//We copy all our declarations in it
		//These declarations, will be replaced by local ones if necessary
		kf->Sharedeclaration(kframe);
	//We then record this new Frame in our instructions list
	kifAllTypes[kframe->name] = true;
	//We also store it at the TOP level, so that others can have access to it...
	kstart->Set(kframe->name, kframe, NULL);
	if (xn->nodes[pos + 1]->token == "depend") {
		string funcname = xn->nodes[pos + 1]->nodes[0]->value;
		short idf = KifNewId(funcname);
		KifElement* kfunc = NULL;
		kfunc = Getbasic(idf, kf);
		//We have a WITH description
		if (kfunc == NULL) {
			instances = localinstances;
			long line = GetCurrentLine(0, xn);
			string message = KIFLIBSTRINGS[178] + funcname;
			throw new KifRaiseError(message, kifGlobals->current->name, line, xn->end);
		}
		if (kfunc->isFunction())
			kframe->Setfunction(kfunc);
		else {
			if (!kfunc->isCall()) {
				instances = localinstances;
				string message = KIFLIBSTRINGS[282] + kifGlobals->kifIdString[kfunc->Name()] + "'";
				throw new KifRaiseError(message, kifGlobals->current->name, kifGlobals->current_start, kifGlobals->current_end);
			}
			KifInstance* ki;
			int idfunc = kfunc->Name();
			if (kifGlobals->kconstants->declarations.check(idfunc)) {
				ki = new KifInstanceConstant(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
				ki->Setinstancetype(kinConstant);
			}
			else {
				if (kfunc->type == kifFunctionDeclaration) {
					ki = new KifInstanceInFunction(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
					ki->Setinstancetype(kinFunction);
				}
				else {
					if (kfunc->type == kifFrameDeclaration) {
						KifElement* currentdeclaration = FindFrameForSequence(kfunc, xn, -1);
						if (currentdeclaration == kstart) {
							ki = new KifInstanceGlobal(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
							ki->Setinstancetype(kinGlobal);
						}
						else {
							ki = new KifInstanceInFrame(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
							ki->Setinstancetype(kinFrame);
						}
					}
					else
						ki = new KifInstance(this, NULL, kfunc->Name(), (KifCommonVariable*)kfunc);
				}
			}
			kframe->Setfunction(ki);
		}

		KifBrowse(xn->nodes[pos + 2], kframe);
	}
	else
		KifBrowse(xn->nodes[pos + 1], kframe);
	kifGlobals->PopStack();
	kframe->declared = false;
	instances = localinstances;
	return kframe;
}

KifElement* KifCode::KifBrowse(x_node* xn, KifElement* kf) {
	if (xn == NULL)
		return NULL;

	kifGlobals->debuginfocurrent->currentline = linereference + GetCurrentLine(0, xn);

	if (kifGlobals->kifFunctions.find(xn->token) != kifGlobals->kifFunctions.end())
		return (this->*kifGlobals->kifFunctions[xn->token])(xn, kf);

	KifElement* ke = NULL;
	for (size_t i = 0; i<xn->nodes.size(); i++) {
		KifElement* s = KifBrowse(xn->nodes[i], kf);
		if (s != NULL)
			ke = s;
	}
	return ke;
}

void KifCode::Loadfile(string n, KifCode* loader) {
	string filename = Getfilename(n);
	int previous = kifGlobals->debuginfocurrent->currentfile;
	if (DebugPtr()->kifdebugging == true) {
		ifstream fkif(STR(filename), modeOuverture);
		string s;
		int lref = 1;
		while (!fkif.eof()) {
			getline(fkif, s);
			if (s != "") {
				string strim = s;
				Trim(strim);
				if (strim != "")
					DebugPtr()->kiftabledebug[kifGlobals->debuginfocurrent->currentfile][lref] = s;
			}
			lref++;
		}
	}

	DebugPtr()->AddFilename(n, this);
	x_readfile xf(STR(filename));
	Load(&xf);
	kifGlobals->debuginfocurrent->currentfile = previous;
	Setloader(loader);
}

void KifCode::Loadfile(ifstream* f, string n, long pos, KifCode* loader) {

	string filename = Getfilename(n);
	if (f->fail()) {
		delete f;
		string compiled = filename + ".kif";
		f = new ifstream(STR(compiled), modeOuverture);
		if (f->fail()) {
			delete f;
			string message = KIFLIBSTRINGS[368];
			message += filename;
			throw new KifRaiseError(message, filename, -1, -1);
		}
		delete f;
		f = NULL;
	}


	string s;
	DebugPtr()->AddFilename(filename, this);
	long l = 1;
	delete f;
	if (DebugPtr()->kifdebugging == true) {
		f = new ifstream(STR(filename), modeOuverture);
		f->seekg(pos);
		while (!f->eof()) {
			getline(*f, s);
			if (s != "") {
				string strim = s;
				Trim(strim);
				if (strim != "")
					DebugPtr()->kiftabledebug[kifGlobals->debuginfocurrent->currentfile][linereference + l] = s;
			}
			l += 1;
		}
		delete f;
	}

	Setloader(loader);
	x_readfile xf(STR(filename), pos);
	Load(&xf);

	int i, j;
	kifGlobals->basegarbage = kifGlobals->garbage.size();
	kifGlobals->trigger += kifGlobals->basegarbage;
	for (j = 0; j<locals.size(); j++) {
		i = locals[j];
		if (kifGlobals->garbage[i] != NULL) {
			if (kifGlobals->garbage[i]->Basevalue()) {
				kifGlobals->garbage[i]->reference = KIF_PROTECT_REFERENCE;
				kifGlobals->garbage[i]->popped = staticval;
			}
			else
			if (kifGlobals->garbage[i]->reference == 0)
				kifGlobals->garbage[i]->reference = 1;
		}
	}
}

KifElement* KifCode::EvaluateLisp(KifElement* dom, string& s, string& o, string& c) {
	ThreadLock _lock(kifVector);

	x_readstring xr(s);
	kifGlobals->current = &xr;
	xr.readstrings = false;
	xr.loadtoken();
	gGlobals->bnfkif->initialize(&xr);

	string lret;
	x_node* xn = new x_node;

	gGlobals->bnfkif->Y_var_0 = o[0];
	gGlobals->bnfkif->Y_var_1 = c[0];

	if (gGlobals->bnfkif->m_lisp(lret, &xn) != 1)
		return Returnerror(KIFLIBSTRINGS[244]);

	KifElement* kret = new KifBaseVector(this, NULL);
	kifGlobals->evaluation = true;
	try {
		KifBrowse(xn, kret);
	}
	catch (KifRaiseError* m) {
		lret = m->message;
		kret->Release();
		kret = Returnerror(kifGlobals->GetThreadid(), lret);
		kifGlobals->debuginfocurrent->currentline = m->left;
		delete m;
	}
	kifGlobals->evaluation = false;
	delete xn;
	return kret;
}

//we check the presence of specific non alpha characters
static string normalizeopen(agnostring& s, string& ch) {
	if (s_is_alpha(ch))
		return ch;

	agnostring o(ch);
	string res;
	string a;
	o.begin();
	int i = 0;
	while (!o.end()) {
		a = o.next();
		i = 0;
		if (c_is_alpha(USTR(a), i) || (a[0] >= 48 && a[0] <= 57))
			res += a;
		else
			res += "_";
	}

	while (s.find(res, 0) != -1)
		res += "_";
	s = s.replace(o, res);
	return res;
}

KifElement* KifCode::EvaluateTags(KifElement* dom, agnostring& s, string& o, string& c) {
	ThreadLock _lock(kifVector);

	o = normalizeopen(s, o);
	c = normalizeopen(s, c);

	x_readstring xr(s);
	kifGlobals->current = &xr;
	xr.readstrings = false;
	xr.loadtoken();
	gGlobals->bnfkif->initialize(&xr);

	string lret;
	x_node* xn = new x_node;

	gGlobals->bnfkif->VS_var_2 = o;
	gGlobals->bnfkif->VS_var_3 = c;

	if (gGlobals->bnfkif->m_tag(lret, &xn) != 1)
		return Returnerror(KIFLIBSTRINGS[244]);

	KifElement* kret = new KifBaseVector(this, NULL);
	kifGlobals->evaluation = true;
	try {
		KifBrowse(xn, kret);
	}
	catch (KifRaiseError* m) {
		lret = m->message;
		kret->Release();
		kret = Returnerror(kifGlobals->GetThreadid(), lret);
		kifGlobals->debuginfocurrent->currentline = m->left;
		delete m;
	}
	kifGlobals->evaluation = false;
	delete xn;
	return kret;
}

KifElement* KifCode::EvaluateVector(KifElement* dom, string& s) {
	ThreadLock _lock(kifVector);

	x_readstring xr(s);
	kifGlobals->current = &xr;

	xr.loadtoken();
	gGlobals->bnfkif->initialize(&xr);

	string lret;
	x_node* xn = new x_node;
	if (gGlobals->bnfkif->m_jvector(lret, &xn) != 1)
		return Returnerror(KIFLIBSTRINGS[244]);

	KifElement* kret = kifNULL;
	kifGlobals->evaluation = true;
	try {
		kret = KifBrowse(xn, NULL);
	}
	catch (KifRaiseError* m) {
		lret = m->message;
		kret = Returnerror(kifGlobals->GetThreadid(), lret);
		kifGlobals->debuginfocurrent->currentline = m->left;
		delete m;
	}
	kifGlobals->evaluation = false;
	delete xn;
	return kret;
}

KifElement* KifCode::EvaluatePredicateParameter(KifElement* dom, string& s) {
	ThreadLock _lock(kifPredicate);

	x_readstring xr(s);
	kifGlobals->current = &xr;

	xr.loadtoken();
	gGlobals->bnfkif->initialize(&xr);

	string lret;
	x_node* xn = new x_node;
	if (gGlobals->bnfkif->m_predicateparameters(lret, &xn) != 1)
		return Returnerror(KIFLIBSTRINGS[244]);

	KifElement* kret = kifNULL;
	kifGlobals->evaluation = true;
	try {
		kret = KifBrowse(xn, NULL);
	}
	catch (KifRaiseError* m) {
		lret = m->message;
		kret = Returnerror(kifGlobals->GetThreadid(), lret);
		kifGlobals->debuginfocurrent->currentline = m->left;
		delete m;
	}
	kifGlobals->evaluation = false;
	delete xn;
	return kret;
}

KifElement* KifCode::EvaluateMap(KifElement* dom, string& s) {
	ThreadLock _lock(kifMap);

	x_readstring xr(s);
	kifGlobals->current = &xr;

	xr.loadtoken();
	gGlobals->bnfkif->initialize(&xr);

	string lret;
	x_node* xn = new x_node;
	if (gGlobals->bnfkif->m_jmap(lret, &xn) != 1)
		return Returnerror(KIFLIBSTRINGS[244]);

	KifElement* kret = kifNULL;
	kifGlobals->evaluation = true;
	try {
		kret = KifBrowse(xn, NULL);
	}
	catch (KifRaiseError* m) {
		delete xn;
		lret = m->message;
		kret = Returnerror(kifGlobals->GetThreadid(), lret);
		kifGlobals->debuginfocurrent->currentline = m->left;
		delete m;
		kifGlobals->evaluation = false;
		return kret;
	}
	kifGlobals->evaluation = false;
	delete xn;
	return kret;
}

KifElement* KifCode::EvaluateJSON(KifElement* dom, string& s) {
	ThreadLock _lock(kifString);

	x_readstring xr(s);
	kifGlobals->current = &xr;

	xr.loadtoken();
	gGlobals->bnfkif->initialize(&xr);

	string lret;
	x_node* xn = new x_node;
	if (gGlobals->bnfkif->m_jexpression(lret, &xn) != 1)
		return Returnerror(KIFLIBSTRINGS[244]);

	KifElement* kret = kifNULL;
	int idthread = kifGlobals->GetThreadid();
	kifGlobals->evaluation = true;
	try {
		kret = KifBrowse(xn, NULL);
	}
	catch (KifRaiseError* m) {
		delete xn;
		lret = m->message;
		kret = Returnerror(idthread, lret);
		kifGlobals->debuginfocurrent->currentline = m->left;
		delete m;
		kifGlobals->evaluation = false;
		return kret;
	}
	kifGlobals->evaluation = false;
	if (dom == NULL)
		dom = kstart;
	delete xn;
	return kret;
}

KifElement* KifCode::Evaluate(KifElement* kf, string& sbase, bool commandline) {
	string s;
	if (commandline == true) {
		kifGlobals->PushStack(kstart);
		s = KIFLIBSTRINGS[369];
		gGlobals->bnfkif->baseline = 1;
	}
	s += sbase;

	x_readstring xr(s);
	kifGlobals->current = &xr;
	xr.loadtoken();

	x_node* xn = gGlobals->bnfkif->x_parsing(&xr, FULL, false);
	string message;
	if (xn == NULL) {
		if (gGlobals->bnfkif->errornumber != -1) {
			string msg("EVL(100):");
			int lsterr = -1;
			for (int er = gGlobals->bnfkif->errornumbers.size() - 1; er >= 0; er--) {
				if (gGlobals->bnfkif->lineerrors[er] != gGlobals->bnfkif->lineerror)
					break;
				if (lsterr == gGlobals->bnfkif->errornumbers[er])
					continue;
				if (lsterr != -1) {
					msg += " ";
					msg += KIFLIBSTRINGS[370];
					msg += " ";
				}

				lsterr = gGlobals->bnfkif->errornumbers[er];
				msg += gGlobals->bnfkif->x_errormsg(lsterr);
			}
			DebugPtr()->currentline = gGlobals->bnfkif->lineerror;
			char ch[20];
			sprintf_s(ch, 20, KIFLIBSTRINGS[371], gGlobals->bnfkif->lineerror);
			msg += ch;
			return Returnerror(msg);
		}
		return Returnerror(kifGlobals->GetThreadid(), KIFLIBSTRINGS[244]);
	}

	instances.clear();
	KifElement* kret = kifNULL;
	try {
		kret = KifBrowse(xn, kf);
	}
	catch (KifRaiseError* m) {
		if (commandline == true)
			kifGlobals->PopStack();
		delete xn;
		string message = m->message;
		kret = Returnerror(kifGlobals->GetThreadid(), message);
		kifGlobals->debuginfocurrent->currentline = m->left;
		delete m;
		return kret;
	}

	if (commandline == true)
		kifGlobals->PopStack();

	for (int i = 0; i<kifGlobals->garbage.dernier; i++) {
		if (kifGlobals->garbage[i] != NULL) {
			if (kifGlobals->garbage[i]->Basevalue()) {
				kifGlobals->garbage[i]->reference = KIF_PROTECT_REFERENCE;
				kifGlobals->garbage[i]->popped = staticval;
			}
			else
			if (kifGlobals->garbage[i]->reference == 0)
				kifGlobals->garbage[i]->reference = 1;
		}
	}
	delete xn;
	return kret;
}

Exported void KifCode::Setbaseline(int i) {
	gGlobals->bnfkif->baseline = i;
}

void KifCode::Load(x_reading* xr) {
	kifGlobals->current = xr;
	AddALoadingPath(xr->name);

	xr->loadtoken();
	x_reading* previous = gGlobals->bnfkif->fx;
	x_node* xn = gGlobals->bnfkif->x_parsing(xr, FULL);
	if (xn == NULL) {
		long line = gGlobals->bnfkif->lineerror;
		string message = KIFLIBSTRINGS[372];
		if (xr->error()) {
			message = KIFLIBSTRINGS[373];
			message += xr->name;
		}
		else
		if (gGlobals->bnfkif->errornumber != -1)
			message += gGlobals->bnfkif->x_errormsg(gGlobals->bnfkif->errornumber);
		else
			message += gGlobals->bnfkif->labelerror;
		throw new KifRaiseError(message, kifGlobals->current->name, line, gGlobals->bnfkif->lineerror);
	}
#ifndef _DEBUG
	try {
#endif
		bool addto = addtolocal;
		addtolocal = true;
		kifGlobals->PushStack(kstart);
		KifBrowse(xn, kstart);
		kifGlobals->PopStack();
		addtolocal = addto;
#ifndef _DEBUG
	}
	catch (KifRaiseError* m) {
		delete xn;
		gGlobals->bnfkif->fx = previous;
		throw m;
	}
#endif
	gGlobals->bnfkif->fx = previous;
	delete xn;
}


KifElement* KifCode::Find(string& name) {
	short idname;
	hmap<string, short>::iterator iter = kifGlobals->kifStringId.find(name);
	if (iter == kifGlobals->kifStringId.end()) {
		string val = s_to_lower(name);
		iter = kifGlobals->kifStringId.find(val);
		if (iter == kifGlobals->kifStringId.end())
			return NULL;
		idname = iter->second;
	}
	else
		idname = iter->second;

	return kstart->declarations.search(idname);
}


KifElement* KifCode::Findraw(string& name) {
	hmap<string, short>::iterator iter = kifGlobals->kifStringId.find(name);
	if (iter == kifGlobals->kifStringId.end())
		return NULL;
	return  kstart->declarations.search(iter->second);
}

KifElement* KifSearch(string name) {
	if (kifGlobals == NULL)
		return NULL;
	KifElement* ret = NULL;
	for (int i = 0; i<kifGlobals->kifCodeVector.size(); i++) {
		if (kifGlobals->kifCodeVector[i] != NULL) {
			ret = kifGlobals->kifCodeVector[i]->Find(name);
			if (ret != NULL)
				return ret;
		}
	}
	return NULL;
}


KifElement* KifSearchraw(string name) {
	if (kifGlobals == NULL)
		return NULL;
	KifElement* ret = NULL;
	for (int i = 0; i<kifGlobals->kifCodeVector.size(); i++) {
		if (kifGlobals->kifCodeVector[i] != NULL) {
			ret = kifGlobals->kifCodeVector[i]->Findraw(name);
			if (ret != NULL)
				return ret;
		}
	}
	return NULL;
}


KifElement* KifCode::Execute(KifFunction* func, vector<KifElement*>& parameters) {
	KifCallFunctionGeneral kcf(NULL, NULL, func->Name(), func);
	kcf.reference = 1000;
	kcf.Setline(func->Line(), func->Fileindex());
	kcf.kifcode = this;
	kcf.parameters = parameters;
	kifGlobals->executionbreak = 0;
	return kcf.Exec(kifNULL, kstart, kifNULL, kifGlobals->GetThreadid(), NULL);
}

int maxc(int i, int j) {
	if (i>j)
		return i;
	return j;
}

void KifCode::FinalGarbage() {
	ThreadLock _lock(kifPrepareError);
	ThreadLock _lockerr(kifError);

	bin_hash<KifElement*>::iterator it;
	KifElement* ke;
	for (it = kstart->declarations.begin(); it != kstart->declarations.end(); it++) {
		ke = it->second;
		if (ke != NULL)
			ke->popped = false;
	}

	kstart->popped = false;
	kstart->Resetreference();
	int i;
	for (i = 0; i<locals.size(); i++) {
		int j = locals[i];
		if (kifGlobals->garbage[j] != NULL) {
			delete kifGlobals->garbage[j];
			kifGlobals->garbage[j] = NULL;
		}
	}

	predicates.clear();
	terms.clear();

	kifGlobals->cursorstring = -1;
	KifElement* k;

	//usually the largest of all structures...
	for (i = 0; i<kifGlobals->stringlist.size(); i++) {
		k = (KifString*)kifGlobals->stringlist[i];
		if (k->kifcode == this) {
			k->kifcode = NULL;
			((KifString*)k)->value = "";
			k->popped = 0;
			k->reference = 0;
		}
		if (kifGlobals->cursorstring == -1 && k->reference == 0 && k->popped == 0)
			kifGlobals->cursorstring = i;
	}

	int maxi = maxc(kifGlobals->garbage.size(), kifGlobals->integerlist.size());
	maxi = maxc(maxi, kifGlobals->floatlist.size());
	kifGlobals->cursorinteger = -1;
	kifGlobals->cursorfloat = -1;

	for (i = 0; i<maxi; i++) {
		if (i<kifGlobals->garbage.size() && kifGlobals->garbage.vecteur[i] != NULL && kifGlobals->garbage.vecteur[i]->kifcode == this) {
			delete kifGlobals->garbage.vecteur[i];
			kifGlobals->garbage.vecteur[i] = NULL;
		}

		if (i<kifGlobals->integerlist.size()) {
			k = (KifInteger*)kifGlobals->integerlist[i];
			if (k->kifcode == this) {
				k->popped = 0;
				k->reference = 0;
			}
			if (kifGlobals->cursorinteger == -1 && k->reference == 0 && k->popped == 0)
				kifGlobals->cursorinteger = i;
		}

		if (i<kifGlobals->floatlist.size()) {
			k = (KifFloat*)kifGlobals->floatlist[i];
			if (k->kifcode == this) {
				k->popped = 0;
				k->reference = 0;
			}
			if (kifGlobals->cursorfloat == -1 && k->reference == 0 && k->popped == 0)
				kifGlobals->cursorfloat = i;
		}
	}
	bin_hash<KifFrame*>::iterator itframe;
	for (itframe = kifGlobals->frames.begin(); itframe != kifGlobals->frames.end(); itframe++)
		delete itframe->second;
	kifGlobals->frames.clear();
	//kifGlobals->garbage_collector(0);
}



KifCode::~KifCode() {
	if (idkifmain == 0)
		kifGlobals->executionbreak = 2;

	if (kifGlobals->kstd != NULL && 	kifGlobals->kstd->kifcode == this)
		kifGlobals->kstd = NULL;
	if (kifGlobals->kerr != NULL && 	kifGlobals->kerr->kifcode == this)
		kifGlobals->kerr = NULL;

	kifGlobals->kifCodeVector[idkifmain] = NULL;
	if (function != NULL) {
		function->instructions.clear();
		delete function;
	}

	hmap<string, int>::iterator it;
	for (it = kifGlobals->kifCodePaths.begin(); it != kifGlobals->kifCodePaths.end(); it++) {
		if (it->second == idkifmain)
			it->second = -1;
	}

	hmap<string, KifThreadLock*>::iterator itlock;
	for (itlock = kifGlobals->locktables.begin(); itlock != kifGlobals->locktables.end(); itlock++)
		delete itlock->second;
	kifGlobals->locktables.clear();

	if (idkifmain == 0) {
		//If it is the first module, then we clean also our libraries...
		KifTerminateAllLibraries(this);
		KifFinalClean();
	}
	else
		FinalGarbage();
}


Exported bool Garbaging() {
	return kifGlobals->garbaging;
}

void KifCode::ClearArguments() {
	KifConst* var = (KifConst*)kifGlobals->kconstants->Declaration(idargs);
	KifVectorString* vargs = (KifVectorString*)var->value;
	vargs->values.clear();
}


void KifCode::AddArguments(vector<string>& args) {
	KifConst* var = (KifConst*)kifGlobals->kconstants->Declaration(idargs);
	KifVectorString* vargs = (KifVectorString*)var->value;
	for (int i = 0; i<args.size(); i++)
		vargs->values.push_back(args[i]);
}


void KifCode::SetArguments() {
	KifConst* var = (KifConst*)kifGlobals->kconstants->Declaration(idargs);
	KifVectorString* vargs = (KifVectorString*)var->value;
	for (int i = 0; i<kifGlobals->arguments.size(); i++)
		vargs->values.push_back(kifGlobals->arguments[i]);
}

void KifCode::InitArguments(vector<string>& args) {
	kifGlobals->arguments = args;
	KifConst* var = (KifConst*)kifGlobals->kconstants->Declaration(idargs);
	KifVectorString* vargs = (KifVectorString*)var->value;
	vargs->values = args;
}


KifElement* KifCode::KifInitVariable(x_node* xn, KifElement* kf) {
	short name = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, name, kf, kifNOTYPE);
	return var;
}


KifVariableDeclaration* KifCode::Createvariable(string name, KifElement* kf, x_node* xn) {
	short idname = KifNewId(name);
	if (kstart->declarations.check(idname)) {
		string message = KIFLIBSTRINGS[319];
		message += name;
		message += KIFLIBSTRINGS[337];
		if (xn == NULL)
			throw new KifRaiseError(message, kifGlobals->current->name, -1, -1);
		else
			throw new KifRaiseError(message, kifGlobals->current->name, GetCurrentLine(0, xn), xn->end);
	}
	KifVariableDeclaration* var = new KifVariableDeclaration(this, NULL, idname);
	kstart->Set(idname, var, NULL);
	return var;
}

KifStaticVariable* KifCode::Createstaticvariable(string name, KifElement* kf, x_node* xn) {
	short idname = KifNewId(name);
	if (kifGlobals->kconstants->declarations.check(idname)) {
		KifElement* v = kifGlobals->kconstants->declarations[idname];
		if (v->type != kifStaticVariable) {
			string message = KIFLIBSTRINGS[319];
			message += name;
			message += KIFLIBSTRINGS[337];
			if (xn == NULL)
				throw new KifRaiseError(message, kifGlobals->current->name, -1, -1);
			else
				throw new KifRaiseError(message, kifGlobals->current->name, GetCurrentLine(0, xn), xn->end);
		}
		return (KifStaticVariable*)v;
	}

	KifStaticVariable* var = new KifStaticVariable(this, kf, idname);
	kifGlobals->kconstants->Set(idname, var, NULL);

	return var;
}

Exported void KifCode::Setloader(KifCode* k) {
	if (k == NULL)
		return;
	if (loader == NULL)
		loader = k;
	KifElement* kifvar = kstart->Declaration(idloader);
	KifKiF* kifkif = (KifKiF*)kifvar->Returnvalue();
	kifkif->id = k->idkifmain;
}


Exported long KifCode::GetCurrentLine(int i, x_node* xn) {
	if (gGlobals->bnfkif->fx == NULL)
		return -1;

	kifGlobals->current_start = gGlobals->bnfkif->x_linenumber(xn->start + i);
	kifGlobals->current_end = xn->end;
	return kifGlobals->current_start;
}

extern bool initkifglobals;

KifCode::KifCode(int trig, void* d) {
	if (kifGlobals == NULL) {
		kifGlobals = new KifGlobalThread(d);
		initkifglobals = true;
	}
	else
	if (kifGlobals->data == NULL)
		kifGlobals->data = d;

	if (initkifglobals) {
		initkifglobals = false;
		kifGlobals->Initialisation(trig, this);
		kifGlobals->os = &cout;
	}

	weightfinalfunction = NULL;
	weightaiguillagefunction = NULL;
	checkprobabilities = PRED_NOACTION;
	gGlobals = kifGlobals;
	recordinstance = true;
	addtolocal = false;
	master = false;
	session = false;

	function = NULL;
	loader = NULL;
	ObjectInitialisation(trig);
	kstart->declarations[idpop] = new KifPredefined(this, P_ONE | P_TWO | P_THREE, &PrePop, idpop);
	kifGlobals->UpdateFail(this);
}

Exported short KifNewId(string n) {
	return kifGlobals->KifNewId(n);
}

Exported short KifStringId(string n) {
	return kifGlobals->KifStringId(n);
}

Exported string KifIdString(short n) {
	return kifGlobals->KifIdString(n);
}

Exported void KifRemoveId(short n) {
	kifGlobals->KifRemoveId(n);
}

void KifFinalClean() {
	if (kifGlobals != NULL) {
		delete kifGlobals;
		kifGlobals = NULL;
	}
}


void KifCleaning() {
	for (int i = 0; i<kifGlobals->mxthreads; i++) {
		if (kifGlobals->THR[i] != NULL) {
			delete kifGlobals->THR[i];
			kifGlobals->THR[i] = NULL;
		}
	}
	KifFinalClean();
}


int KifTestPath(string path) {
	if (kifGlobals->kifCodePaths.find(path) == kifGlobals->kifCodePaths.end() || kifGlobals->kifCodePaths[path] == -1)
		return -1;
	return kifGlobals->kifCodePaths[path];
}

void SetKifPath(string path, int id) {
	char localpath[4096];
#ifdef WIN32
	_fullpath(localpath, STR(path), 4096);
#else
	realpath(STR(path), localpath);
#endif
	kifGlobals->kifCodePaths[localpath] = id;
}

int KifLoadin(KifCode* kifcode, string path) {
	int previous = kifGlobals->debuginfocurrent->currentfile;
	DebugPtr()->AddFilename(path, kifcode);

	ifstream* fkif;
	if (DebugPtr()->kifdebugging == true) {
		fkif = new ifstream(STR(path), modeOuverture);
		string s;
		int lref = 1;
		while (!fkif->eof()) {
			getline(*fkif, s);
			if (s != "") {
				string strim = s;
				Trim(strim);
				if (strim != "")
					DebugPtr()->kiftabledebug[kifGlobals->debuginfocurrent->currentfile][lref] = s;
			}
			lref++;
		}
		delete fkif;
	}


	x_reading* precurrent = kifGlobals->current;
	bnf_kif* prebnfkif = kifGlobals->bnfkif;
	bnf_kif localbnf;
	kifGlobals->bnfkif = &localbnf;
	int from = kifcode->kstart->InstructionSize();
	fkif = new ifstream(STR(path), modeOuverture);
	kifGlobals->bnfkif->baseline = 1;
	bool autorundeactivated = false;
	if (kifcode->loader == NULL) {//we do not want autorun in the program to be executed...
		kifcode->loader = kifcode;
		autorundeactivated = true;
	}

	try {
		kifcode->Loadfile(fkif, path, 0, NULL);
	}
	catch (KifRaiseError* m) {
		if (autorundeactivated)
			kifcode->loader = NULL;
		kifGlobals->bnfkif = prebnfkif;
		kifGlobals->current = precurrent;
		throw m;
	}

	if (autorundeactivated)
		kifcode->loader = NULL;

	SetKifPath(path, kifcode->idkifmain);
	for (int i = from; i<kifcode->kstart->instructions.size(); i++) {
		if (kifcode->kstart->instructions[i]->Basevalue()) {
			kifcode->kstart->instructions[i]->reference = 5000;
			kifcode->kstart->instructions[i]->popped = staticval;
		}
		else
		if (kifcode->kstart->instructions[i]->reference == 0)
			kifcode->kstart->instructions[i]->reference = 1;
	}
	if (kifGlobals->THR[0]->error)
		return -1;

	kifGlobals->bnfkif = prebnfkif;
	kifGlobals->current = precurrent;
	kifGlobals->debuginfocurrent->currentfile = previous;
	return kifcode->idkifmain;
}

Exported KifCode* KifCode::Newinstance() {
	KifInitLib();
	return new KifCode(kifGlobals->trigger, kifGlobals->data);
}


Exported int KifCode::KifLoad(vector<string>& paths, bool debugkif, bool newinstance) {
	//On initialise une valeur par defaut du declencheur 
	//de garbage, pour que celui-ci puisse s'effectuer
	//a chaque fin d'execution
	KifCode* kifcode = this;
	if (newinstance)
		kifcode = Newinstance();
	bool deb = DebugPtr()->kifdebugging;
	DebugPtr()->kifdebugging = false;
	kifcode->SetArguments();
	register int i;
	for (i = 0; i<paths.size(); i++) {
		string fname = Getfilename(paths[i]);
		ifstream* fkif = new ifstream(STR(fname), modeOuverture);
		if (fkif->fail()) {
			string message = KIFLIBSTRINGS[373];
			message += fname;
			Returnerror(message);
			return kifcode->idkifmain;
		}
		try {
			kifGlobals->bnfkif->baseline = 1;
			kifcode->Loadfile(fkif, fname, 0, this);
			SetKifPath(fname, kifcode->idkifmain);
		}
		catch (KifRaiseError* m) {
			Returnerror(kifGlobals->GetThreadid(), m->message);
			kifGlobals->debuginfocurrent->currentline = m->left;
			delete m;
			return kifcode->idkifmain;
		}
	}

	kifcode->Run();
	DebugPtr()->kifdebugging = deb;
	if (kifGlobals->THR[0]->error)
		return -1;
	return kifcode->idkifmain;
}

int KifCode::KifLoadPath(string& path, bool debugkif) {
	char localpath[4096];
#ifdef WIN32
	_fullpath(localpath, STR(path), 4096);
#else
	realpath(STR(path), localpath);
#endif

	ThreadLock _lock(kifLoading);
	int ikif = KifTestPath(localpath);
	if (ikif == -1) {
		vector<string> paths;
		paths.push_back(localpath);
		ikif = KifLoad(paths, debugkif, false);
	}
	return ikif;
}


Exported int KifRunProgram(string fname, string arguments) {


	KifCode* kifcode = new KifCode(1000, NULL);

	if (arguments.size() != 0) {
		vector<string> args;
		v_split(arguments, " ", args);
		kifcode->ClearArguments();
		kifcode->AddArguments(args);
	}

	kifGlobals->threadmode = true;

	fname = Getfilename(fname);
	ifstream* fkif = new ifstream(STR(fname), modeOuverture);
	if (fkif->fail()) {
		string value = KIFLIBSTRINGS[374];
		value += fname;
		kifcode->Returnerror(value);
		return -1;
	}

	try {
		kifGlobals->bnfkif->baseline = 1;
		kifcode->Loadfile(fkif, fname, 0);
	}
	catch (KifRaiseError* m) {
		string lret = m->message;
		kifcode->Returnerror(kifGlobals->GetThreadid(), lret);
		kifGlobals->debuginfocurrent->currentline = m->left;
		delete m;
		return -2;
	}

	kifcode->Run();
	if (Error() == true)
		return -3;

	return kifcode->idkifmain;
}

Exported string KifExecute(int ikif, string name, vector<string>& parameters, ostringstream* os, bool debugkif) {
	KifCode* kifcode = kifGlobals->kifCodeVector[ikif];
	string valeur;

	if (kifcode == NULL) {
		valeur = KIFLIBSTRINGS[178];
		valeur += name;
		kifcode->Returnerror(valeur);
		return valeur;
	}

	ostream* old_os = kifGlobals->os;
	if (os != NULL)
		kifGlobals->os = os;
	KiFSetdebug(debugkif);

	KifElement* proc_ke = KifSearchraw(name);
	if (proc_ke == NULL || proc_ke->type != kifFunction) {
		valeur = KIFLIBSTRINGS[178];
		valeur += name;
		kifcode->Returnerror(valeur);
		return valeur;
	}

	vector<KifElement*> params;
	int i;
	KifElement* resultat;
	for (i = 0; i<parameters.size(); i++) {
		resultat = kifcode->Providestring(parameters[i]);
		resultat->Setreference();
		params.push_back(resultat);
	}

	KifFunction* proc = (KifFunction*)proc_ke;
	resultat = kifcode->Execute(proc, params);

	for (i = 0; i<params.size(); i++)
		params[i]->Resetreference();

	if (Error()) {
		valeur = resultat->String();
		return valeur;
	}

	valeur = resultat->String();
	kifGlobals->Clean();
	kifGlobals->os = old_os;
	return valeur;
}

Exported KifElement* KifExecuteFunction(KifCode* kifcode, string name, vector<KifElement*>& params) {
	string valeur;

	KifElement* proc_ke = KifSearchraw(name);
	if (proc_ke == NULL || proc_ke->type != kifFunction) {
		valeur = KIFLIBSTRINGS[178];
		valeur += name;
		return kifcode->Returnerror(valeur);
	}

	KifFunction* proc = (KifFunction*)proc_ke;
	KifElement* resultat = kifcode->Execute(proc, params);

	for (int i = 0; i<params.size(); i++)
		params[i]->Resetreference();

	return resultat;
}

Exported void KifDelete(bool session, int ipar) {
	KifCode* kifcode;
	{
		ThreadLock _lock(kifLoading);
		kifcode = kifGlobals->kifCodeVector[ipar];
		if (kifcode == NULL)
			return;
		if (session == true)
			kifcode->session = true;
	}

	delete kifcode;
}

Exported void KifSetSize(int threshold) {
	kif_declencheur = threshold;
	kifGlobals->trigger = kif_declencheur;
}


Exported KifType KifAddNewType(string name, KifExternalParse func, bool error) {
	return kifGlobals->KifAddNewType(name, func, error);
}

Exported KifType KifNewType() {
	kifGlobals->kifLastType++;
	return (KifType)kifGlobals->kifLastType;
}


#ifdef _DEBUG
extern KifCode* currentkifcode;
#endif

//===================================================================
KifElement::KifElement(KifCode* klc, KifElement* kp, KifType k) {
	kifcode = klc;
	type = k;
	popped = 1;
	reference = 0;
	if (klc != NULL) {
		if (kifGlobals->Test()) {
			ThreadLock _lock(kifGarbage);
			kifGlobals->addtogarbage(this);
		}
		else
			kifGlobals->addtogarbage(this);
		if (klc->addtolocal)
			klc->Locals(idgarbage);
	}
	else
		idgarbage = -1;
	if (kp != NULL)
		kp->Add(this);
}

KifPosition::KifPosition(KifCode* klc, KifElement* kp, KifType t) : line(-1), fileidx(0), KifElement(klc, kp, t) {
	if (kifcode != NULL) {
		line = kifGlobals->debuginfocurrent->currentline;
		fileidx = kifGlobals->debuginfocurrent->currentfile;
	}
}

Exported void KifElement::Clean() {
	reference--;
	if (reference <= 0) {
		if (idgarbage != -1) {
			ThreadLock* _lock = new ThreadLock(kifGarbage);
			kifGlobals->garbage.affecte(idgarbage, NULL);
			if (idgarbage<kifGlobals->cursor)
				kifGlobals->cursor = idgarbage;
			delete _lock;
			delete this;
		}
	}
}

string KifElement::IteratorKeyString() {
	KifElement* ke = IteratorKey();
	string n = ke->String();
	ke->Release();
	return n;
}

long KifElement::IteratorKeyInteger() {
	KifElement* ke = IteratorKey();
	long n = ke->Integer();
	ke->Release();
	return n;
}

double KifElement::IteratorKeyFloat() {
	KifElement* ke = IteratorKey();
	double n = ke->Float();
	ke->Release();
	return n;
}

string KifElement::IteratorValueString() {
	KifElement* ke = IteratorValue();
	string n = ke->String();
	ke->Release();
	return n;
}

long KifElement::IteratorValueInteger() {
	KifElement* ke = IteratorValue();
	long n = ke->Integer();
	ke->Release();
	return n;
}

double KifElement::IteratorValueFloat() {
	KifElement* ke = IteratorValue();
	double n = ke->Float();
	ke->Release();
	return n;
}

KIFLONG KifElement::pureeuclidian(KIFLONG a, KIFLONG b) {
	if (b == 0)
		return 0;
	if (a<b)
		return 1;
	KIFLONG remain = a%b;
	if (remain == 0)
		return b;
	return pureeuclidian(b, remain);
}

KIFLONG KifElement::computeEuclidian(KIFLONG a, KIFLONG b) {
	if (a>b)
		return pureeuclidian(a, b);
	return pureeuclidian(b, a);
}

Exported KifElement* KifElement::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	if (b->type == type)
		return kifNULL;
	if (kfunc == NULL)
		return b->in(a, b, NULL, contextualpattern, true);
	return applyfunc(a, b, kfunc);
}

void KifDomain::Lockframe(bool exclusive) {
	if (exclusive)
		_locker.Locking();
}

void KifDomain::Unlockframe(bool exclusive) {
	if (exclusive)
		_locker.Unlocking();
}

Exported void KifDomain::Setdeclaration(string n, KifElement* ke, bin_hash<KifElement*>* stacks) {
	KifContainerLock _lock(this);
	short id = KifNewId(n);
	Setdeclaration(id, ke, stacks);
}

Exported void KifDomain::Setdeclaration(short n, KifElement* ke, bin_hash<KifElement*>* stacks) {
	if (kifGlobals->Test() == false && stacks == NULL) {
		ke->Incrementreference();
		declarations[n] = ke;
		return;
	}

	KifContainerLock _lock(this);
	if (stacks != NULL) {
		KifElement* kdec = NULL;
		if (declarations.check(n)) {
			kdec = declarations[n];
			if (kdec == ke)
				return;
			(*stacks)[n] = kdec;
			ke->Setreference();
			declarations[n] = ke;
		}
		return;
	}

	ke->Incrementreference();
	declarations[n] = ke;
}


Exported void KifDomain::Set(short n, KifElement* ke, bin_hash<KifElement*>* stacks) {
	if (kifGlobals->Test() == false && stacks == NULL) {
		ke->Incrementreference();
		declarations[n] = ke;
		return;
	}
	KifContainerLock _lock(this);
	if (stacks != NULL) {
		KifElement* kdec = NULL;
		if (declarations.check(n)) {
			kdec = declarations[n];
			if (kdec == ke)
				return;
			(*stacks)[n] = kdec;
			ke->Setreference();
			declarations[n] = ke;
		}
		return;
	}

	ke->Incrementreference();
	declarations[n] = ke;
}

Exported void KifDomain::Resetdeclaration(bin_hash<KifElement*>& stacks) {
	if (stacks.size() == 0)
		return;
	bin_hash<KifElement*>::iterator it;
	short n;
	for (it = stacks.begin(); it != stacks.end(); it++) {
		n = it->first;
		KifContainerLock _lock(this);
		if (declarations.check(n))
			declarations[n]->Resetreference();

		if (it->second == NULL)
			declarations.erase(n);
		else
			declarations[n] = it->second;
	}
}

Exported void KifDomain::SetInstances(KifElement* dom, bin_hash<KifElement*>* stacks) {
	bin_hash<KifElement*>::iterator it;
	dom->Disableadding();
	KifElement* xe;
	for (it = declarations.begin(); it != declarations.end(); it++) {
		xe = it->second->Copy(this, it->second);
		dom->Setdeclaration(it->first, xe, stacks);
	}
}

Exported KifElement* KifDomain::same(KifElement* a) {
	if (a == kifNULL) {
		if (declarations.size() == 0)
			return kifTRUE;
		return kifFALSE;
	}

	if (a->Frame() != frame)
		return kifFALSE;
	KifElement* func = Declaration(idequal);

	if (func == NULL) {
		if (a == this)
			return kifTRUE;
		return kifFALSE;
	}
	return applycomparison(idequal, a);
}

Exported KifElement* KifDomain::different(KifElement* a) {
	if (a->Frame() != frame)
		return kifTRUE;

	KifElement* func = Declaration(iddifferent);
	if (func == NULL) {
		if (a != this)
			return kifTRUE;
		return kifFALSE;
	}
	return applycomparison(iddifferent, a);
}
//===================================================================
KifElement* KifElement::Kifpredefined(short n) {



	if (TestkifFunctions(n, kifGlobals->kifPredefinedFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifElement::Kifpredefined(string n) {



	short id = KifNewId(n);
	if (TestkifFunctions(id, kifGlobals->kifPredefinedFunctions) == true)
		return kifGlobals->kifBaseFunctions[id];
	return NULL;
}

//===================================================================
KifElement* KifSelf::Declaration(short n) {
	KifElement* kret = kifGlobals->KifBaseFunctionTest(n);
	if (kret != NULL)
		return kret;
	if (kifcode->KifDeclaredFunctionTest(n) == true)
		return kifTRUE;

	string message = KIFLIBSTRINGS[375];
	message += kifGlobals->kifIdString[n];
	throw new KifRaiseError(message, kifGlobals->current->name, kifGlobals->current_start, kifGlobals->current_end);
	return NULL;
}

KifElement* KifBit::Declaration(short n) {
	if (n == kifGlobals->kifStringId["#"] || n == kifGlobals->kifStringId[KIFLIBSTRINGS[376]])
		return kifGlobals->kifBaseFunctions[n];
	if (TestkifFunctions(n, kifGlobals->kifPredefinedFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifBits::Declaration(short n) {
	if (n == kifGlobals->kifStringId["#"] || n == kifGlobals->kifStringId[KIFLIBSTRINGS[376]])
		return kifGlobals->kifBaseFunctions[n];
	if (TestkifFunctions(n, kifGlobals->kifPredefinedFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifIndex::Declaration(short n) {
	KifElement* ke = kifGlobals->KifBaseFunctionTest(n);
	if (ke != NULL)
		return ke;
	if (kifcode->KifDeclaredFunctionTest(n) == true)
		return kifTRUE;

	string message = KIFLIBSTRINGS[377];
	message += kifGlobals->kifIdString[n];
	throw new KifRaiseError(message, kifGlobals->current->name, kifGlobals->current_start, kifGlobals->current_end);
	return NULL;
}

Exported KifElement* KifInstanceCommon::Declarationfunction(short name) {
	if (declaration->value->type == kifDomain)
		return ((KifDomain*)declaration->value)->frame->Declaration(name);
	return declaration->value->Declaration(name);
}


KifElement* KifDomain::Declaration(short s) {
	if (s == idthis)
		return this;

	if (s == idmethods)
		return kifGlobals->kifBaseFunctions[s];

	KifContainerLock _lock(this);
	if (!declarations.check(s))
		return NULL;
	return declarations[s];
}


KifElement* KifDomain::Declared(short s) {
	if (s == idthis)
		return this;

	if (s == idmethods)
		return kifGlobals->kifBaseFunctions[s];

	KifContainerLock _lock(this);
	if (!declarations.check(s))
		return NULL;
	return declarations[s];
}


KifElement* KifCallFunctionPredefined::Declaration(short n) {
	KifElement* ret = kifGlobals->KifBaseFunctionTest(n);
	if (ret != NULL)
		return ret;
	if (kifcode->KifDeclaredFunctionTest(n) == true)
		return kifTRUE;
	return NULL;
}

KifElement* KifCallFunction::Declaration(short s) {
	KifContainerLock _lock(this);
	bin_hash<KifElement*>::iterator iter = declarations.find(s);
	if (iter == declarations.end())
		return NULL;
	return iter->second;
}


KifElement* KifRawString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifStringFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifStringFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifUString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifStringFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifFloat::Declaration(short n) {
	if (kifGlobals->kifMathFunctions.find(n) != kifGlobals->kifMathFunctions.end() || TestkifFunctions(n, kifGlobals->kifNumericalFunctions) == true || n == idget)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}


KifElement* KifInteger::Declaration(short n) {
	if (kifGlobals->kifMathFunctions.find(n) != kifGlobals->kifMathFunctions.end() || TestkifFunctions(n, kifGlobals->kifNumericalFunctions) == true || n == idget)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifByte::Declaration(short n) {
	if (kifGlobals->kifMathFunctions.find(n) != kifGlobals->kifMathFunctions.end() || TestkifFunctions(n, kifGlobals->kifNumericalFunctions) == true || n == idget)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifShort::Declaration(short n) {
	if (kifGlobals->kifMathFunctions.find(n) != kifGlobals->kifMathFunctions.end() || TestkifFunctions(n, kifGlobals->kifNumericalFunctions) == true || n == idget)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}


KifElement* KifLong::Declaration(short n) {
	if (kifGlobals->kifMathFunctions.find(n) != kifGlobals->kifMathFunctions.end() || TestkifFunctions(n, kifGlobals->kifNumericalFunctions) == true || n == idget)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifFraction::Declaration(short n) {
	if (kifGlobals->kifMathFunctions.find(n) != kifGlobals->kifMathFunctions.end() || TestkifFunctions(n, kifGlobals->kifNumericalFunctions) == true || n == idget)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifDate::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifDateFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifTime::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifTimeFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}


KifElement* KifKiF::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifKiFFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	if (id != -1) {
		KifCode* kifcode = kifGlobals->kifCodeVector[id];
		if (kifcode != NULL) {
			KifElement* res = kifcode->kstart->Declared(n);
			if (res == NULL || res->Private())
				return NULL;
			return res;
		}
	}
	else
		return kifTRUE;
	return NULL;
}

KifElement* KifBaseFile::Declaration(short n) {

	if (TestkifFunctions(n, kifGlobals->kifFileFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifIterator::Declaration(short n) {



	if (TestkifFunctions(n, kifGlobals->kifIteratorFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

//===================================================================
Exported KifElement* KifRawString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationString.find(s) == kifGlobals->kifInformationString.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationString[s]);
}

Exported KifElement* KifString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationString.find(s) == kifGlobals->kifInformationString.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationString[s]);
}

Exported KifElement* KifUString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationString.find(s) == kifGlobals->kifInformationString.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationString[s]);
}

Exported KifElement* KifInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationInt.find(s) == kifGlobals->kifInformationInt.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationInt[s]);
}

Exported KifElement* KifByte::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationInt.find(s) == kifGlobals->kifInformationInt.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationInt[s]);
}

Exported KifElement* KifShort::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationInt.find(s) == kifGlobals->kifInformationInt.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationInt[s]);
}

Exported KifElement* KifLong::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationInt.find(s) == kifGlobals->kifInformationInt.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationInt[s]);
}

Exported KifElement* KifFraction::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationInt.find(s) == kifGlobals->kifInformationInt.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationInt[s]);
}

Exported KifElement* KifBit::Information(KifElement* kstr) {
	return kifcode->Providestring(kifGlobals->kifInformationInt["#"]);
}

Exported KifElement* KifBits::Information(KifElement* kstr) {
	return kifcode->Providestring(kifGlobals->kifInformationInt["#"]);
}


Exported KifElement* KifFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationFloat.find(s) == kifGlobals->kifInformationFloat.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationFloat[s]);
}



Exported KifElement* KifIterator::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationIterator.find(s) == kifGlobals->kifInformationIterator.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationIterator[s]);
}


Exported KifElement* KifDate::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationDate.find(s) == kifGlobals->kifInformationDate.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationDate[s]);
}

Exported KifElement* KifTime::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationTime.find(s) == kifGlobals->kifInformationTime.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationTime[s]);
}

Exported KifElement* KifBaseFile::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationFile.find(s) == kifGlobals->kifInformationFile.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationFile[s]);
}


Exported KifElement* KifKiF::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationKif.find(s) == kifGlobals->kifInformationKif.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationKif[s]);
}

//===================================================================

void KifRawString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifStringFunctions.begin(); it != kifGlobals->kifStringFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

void KifString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifStringFunctions.begin(); it != kifGlobals->kifStringFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

void KifUString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifStringFunctions.begin(); it != kifGlobals->kifStringFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

void KifFloat::Methods(KifVectorString* fcts) {
	hmap<short, mathFunc>::iterator itx;
	for (itx = kifGlobals->kifMathFunctions.begin(); itx != kifGlobals->kifMathFunctions.end(); itx++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[itx->first]));
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifNumericalFunctions.begin(); it != kifGlobals->kifNumericalFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}


void KifInteger::Methods(KifVectorString* fcts) {



	hmap<short, mathFunc>::iterator itx;
	for (itx = kifGlobals->kifMathFunctions.begin(); itx != kifGlobals->kifMathFunctions.end(); itx++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[itx->first]));
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifNumericalFunctions.begin(); it != kifGlobals->kifNumericalFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

void KifByte::Methods(KifVectorString* fcts) {



	hmap<short, mathFunc>::iterator itx;
	for (itx = kifGlobals->kifMathFunctions.begin(); itx != kifGlobals->kifMathFunctions.end(); itx++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[itx->first]));
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifNumericalFunctions.begin(); it != kifGlobals->kifNumericalFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}


void KifShort::Methods(KifVectorString* fcts) {
	hmap<short, mathFunc>::iterator itx;
	for (itx = kifGlobals->kifMathFunctions.begin(); itx != kifGlobals->kifMathFunctions.end(); itx++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[itx->first]));
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifNumericalFunctions.begin(); it != kifGlobals->kifNumericalFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}


void KifLong::Methods(KifVectorString* fcts) {
	hmap<short, mathFunc>::iterator itx;
	for (itx = kifGlobals->kifMathFunctions.begin(); itx != kifGlobals->kifMathFunctions.end(); itx++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[itx->first]));
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifNumericalFunctions.begin(); it != kifGlobals->kifNumericalFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}


void KifFraction::Methods(KifVectorString* fcts) {
	hmap<short, mathFunc>::iterator itx;
	for (itx = kifGlobals->kifMathFunctions.begin(); itx != kifGlobals->kifMathFunctions.end(); itx++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[itx->first]));
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifNumericalFunctions.begin(); it != kifGlobals->kifNumericalFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

void KifBit::Methods(KifVectorString* fcts) {
	fcts->Push(kifcode->Providestringraw("#"));
}

void KifBits::Methods(KifVectorString* fcts) {
	fcts->Push(kifcode->Providestringraw("#"));
}


void KifDate::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifDateFunctions.begin(); it != kifGlobals->kifDateFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}


void KifTime::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifTimeFunctions.begin(); it != kifGlobals->kifTimeFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

Exported void KifKiF::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifKiFFunctions.begin(); it != kifGlobals->kifKiFFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

void KifBaseFile::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifFileFunctions.begin(); it != kifGlobals->kifFileFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
}

void KifIterator::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifIteratorFunctions.begin(); it != kifGlobals->kifIteratorFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));
}

void KifDomain::Methods(KifVectorString* fcts) {
	bin_hash<KifElement*>::iterator iter;
	for (iter = ((KifFrame*)frame)->declarations.begin(); iter != ((KifFrame*)frame)->declarations.end(); iter++) {
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[iter->first]));
	}
	fcts->Push(kifcode->Providestringraw(KIFLIBSTRINGS[378]));
	fcts->Push(kifcode->Providestringraw(KIFLIBSTRINGS[311]));
	fcts->Push(kifcode->Providestringraw(KIFLIBSTRINGS[379]));
}

//===================================================================
void KifInitFunctions(KifCode* kifcodeptr) {

	kifGlobals->KifInitInformation();

	kifGlobals->gRAISEERROR->kifcode = kifcodeptr;
	kifGlobals->gNULL->kifcode = kifcodeptr;
	kifGlobals->gNOELEMENT->kifcode = kifcodeptr;
	kifGlobals->gUNIVERSAL->kifcode = kifcodeptr;
	kifGlobals->gTRUE->kifcode = kifcodeptr;
	kifGlobals->gFALSE->kifcode = kifcodeptr;
	kifGlobals->gMINUSONE->kifcode = kifcodeptr;
	kifGlobals->gDEFAULT->kifcode = kifcodeptr;
	kifGlobals->gBREAK->kifcode = kifcodeptr;
	kifGlobals->gBREAKPOINT->kifcode = kifcodeptr;
	kifGlobals->gCONTINUE->kifcode = kifcodeptr;
	kifGlobals->gNEG->kifcode = kifcodeptr;
	kifGlobals->gTypeSTRING->kifcode = kifcodeptr;
	kifGlobals->gTypeINTEGER->kifcode = kifcodeptr;
	kifGlobals->gTypeFLOAT->kifcode = kifcodeptr;
	kifGlobals->gTypeLONG->kifcode = kifcodeptr;
	kifGlobals->gTypeFRACTION->kifcode = kifcodeptr;
	kifGlobals->gTypeBOOLEAN->kifcode = kifcodeptr;
}

void KifCode::Updatekstart() {

	kstart = new KifBaseFrame(this, NULL, idstart);
	KifVariableDeclaration* var = new KifVariableDeclaration(this, NULL, idloader);
	KifKiF* kifkif = new KifKiF(this, var, kifLoader);
	kstart->Set(idloader, var, NULL);

	var = new KifVariableDeclaration(this, NULL, idstart);
	var->value = kstart;
	kstart->Set(idstart, var, NULL);

	if (kifGlobals->kconstants == NULL) {
		char buffer[4096];
#ifdef WIN32
		_fullpath(buffer, ".", 4096);
#else
		realpath(".", buffer);
#endif

		kifGlobals->kconstants = new KifBaseFrame(this, NULL, idstart);

		KifConst* args = new KifConst(this, NULL, idargs, true);
		KifVectorString* vargs = new KifVectorString(this, args);
		kifGlobals->kconstants->Set(idargs, args, NULL);

		args = new KifConst(this, NULL, idpath, true);

		vargs = new KifVectorString(this, args);
		kifGlobals->kconstants->Set(idpath, args, NULL);
		vargs->values.push_back(buffer);
		buffer[0] = 0;

		string sn;

		KifBaseString* sep;
		int id;
		id = KifNewId(KIFLIBSTRINGS[380]);
		args = new KifConst(this, NULL, id, true);
		sep = new KifBaseString(this, args);
		kifGlobals->kconstants->Set(id, args, NULL);


		args = new KifConst(this, NULL, idsep, true);
		sn = SEP;
		sep = new KifBaseString(this, args, sn);
		kifGlobals->kconstants->Set(idsep, args, NULL);

		args = new KifConst(this, NULL, idendl, true);
		sn = Endl;
		sep = new KifBaseString(this, args, sn);
		kifGlobals->kconstants->Set(idendl, args, NULL);

		args = new KifConst(this, NULL, idOS, true);
		sep = new KifBaseString(this, args, KIFOS);
		kifGlobals->kconstants->Set(idOS, args, NULL);

		var = new KifVariableDeclaration(this, NULL, idstdin);
		KifStdin* std = new KifStdin(this, var);
		kifGlobals->kconstants->Set(idstdin, var, NULL);

		kifGlobals->kconstants->Set(idtrue, kifTRUE, NULL);
		kifGlobals->kconstants->Set(idfalse, kifFALSE, NULL);
		kifGlobals->kconstants->Set(idnull, kifNULL, NULL);
		kifGlobals->kconstants->Set(idnoelement, kifNOELEMENT, NULL);
		kifGlobals->kconstants->Set(iduniversal, kifUNIVERSAL, NULL);

		args = new KifConst(this, NULL, id_pi, true);
		KifBaseFloat* kval = new KifBaseFloat(this, args, M_PI);
		kifGlobals->kconstants->Set(id_pi, args, NULL);

		args = new KifConst(this, NULL, idpichar, true);
		kval = new KifBaseFloat(this, args, M_PI);
		kifGlobals->kconstants->Set(idpichar, args, NULL);

		args = new KifConst(this, NULL, id_e, true);
		kval = new KifBaseFloat(this, args, M_E);
		kifGlobals->kconstants->Set(id_e, args, NULL);


		KifBaseInteger* kint;

		id = KifNewId(KIFLIBSTRINGS[381]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 1);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[382]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 2);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[383]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 4);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[384]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 8);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[385]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 32);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[386]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 16);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[387]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 64);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[388]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 32);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[389]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 64);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[390]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 128);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[391]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 256);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[392]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 512);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[393]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 1024);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[394]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 0);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[395]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 1);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[396]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 2);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[397]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 4);
		kifGlobals->kconstants->Set(id, args, NULL);

		id = KifNewId(KIFLIBSTRINGS[398]);
		args = new KifConst(this, NULL, id, true);
		kint = new KifBaseInteger(this, args, 8);
		kifGlobals->kconstants->Set(id, args, NULL);

		kcallsyncronous = new KifFunctionSynchronous(this, NULL, idsynchronous);
		kifGlobals->kconstants->declarations[idsynchronous] = kcallsyncronous;

		KifInitFunctions(this);

	}
}

KifElement* KifVariableDeclaration::Robfunction(KifElement* kbase) {
	if (function != NULL && kbase != NULL) {
		if (kbase->testfunction(function) == false)
			kifcode->Returnerror(KIFLIBSTRINGS[281]);
	}
	KifElement* func = function;
	function = NULL;
	return func;
}

KifElement* KifFrame::Robfunction(KifElement* kbase) {
	KifElement* func = function;
	if (func != NULL) {
		if (func->Size() != 3)
			kifcode->Returnerror(KIFLIBSTRINGS[281]);
	}
	function = NULL;
	return func;
}

Exported void KifCode::ObjectInitialisation(int trig) {
	inittableutf8();
#ifdef _DEBUG
	currentkifcode = this;
#endif
	kcallsyncronous = NULL;
	kifGlobals->AddKifCode(this);
	linereference = 0;
	insidecall = 0;
	Updatekstart();
}
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------

void KifDebugInfo::AddFilename(string& filename, KifCode* kifcode) {
#ifdef WIN32
	filename = s_to_lower(filename);
#endif

	for (int i = 0; i<kiftabledebugfilename.size(); i++)
	if (kiftabledebugfilename[i] == filename) {
		kifGlobals->debuginfocurrent->currentfile = i;
		currentfile = i;
		return;
	}
	currentfile = kiftabledebugfilename.size();
	kiftabledebugfilename.push_back(filename);
}

void KifDebugInfo::Setkifdata(KifElement* object) {
	debugdata = object;
}

void KifDebugInfo::Setkifdebug(KifElement* f, KifElement* object) {
	debugfunction = f;
	debugstack = new KifVector(NULL, NULL);
	debugstack->Setreference();
	debugvariables = new KifMap(NULL, NULL);
	debugvariables->Setreference();
	debugdata = object;
	kifdebug = true;
	kifdebugging = true;
	kifinfunction = false;
	debuginfunction = true;
}

void KifDebugInfo::Close() {
	debugfunction = NULL;
	if (debugstack != NULL) {
		debugstack->Clear();
		debugvariables->Clear();
		debugstack->Resetreference();
		debugvariables->Resetreference();
	}
	debugdata = NULL;
	kifdebug = false;
	kifdebugging = false;
	kifinfunction = false;
	debuginfunction = false;
}

int Stringsplit(string thestr, string thesplitter, vector<string>& vect) {
	size_t pos = 0;
	size_t found = 0;

	while (pos != -1) {
		found = thestr.find(thesplitter, pos);
		if (found != -1) {
			string n = thestr.substr(pos, found - pos);
			if (n != "")
				vect.push_back(n);
			pos = found + thesplitter.size();
		}
		else
			break;
	}
	string s = thestr.substr(pos, thestr.size() - pos);
	if (s != "")
		vect.push_back(s);
	return vect.size();
}



void DisplayClean(string& s, char* buffer, int& compte) {
	string blanc;
	compte += strlen(buffer);
	while (compte>0) {
		cout << (char)8;
		compte--;
		blanc += " ";
	}
	cout << buffer << s << blanc;
	for (int i = 0; i<blanc.size(); i++)
		cout << (char)8;
	compte = s.size();
}


void KifDebugInfo::Stack(KifCode* kifcode) {

	debugstack->Clear();
	debugstack->kifcode = kifcode;
	kifGlobals->GetStackInfos(kifcode, debugstack);
}

KifElement* KifDebugInfo::Currentfilename() {
	string fname = kifGlobals->debuginfocurrent->Filename(kifGlobals->debuginfocurrent->currentfile);
	return kifGlobals->kifCodeVector[0]->Providestring(fname);
}

void KifDebugInfo::Variables(string var) {

	kifdebugging = false;

	//First the function variable
	debugvariables->Clear();
	debugvariables->kifcode = currentkifcode;

	if (!debuginfunction || currentdom == NULL)
		return;

	bin_hash<KifElement*>::iterator it;
	string n;
	string nm = KIFLIBSTRINGS[399];
	KifBaseString localstring(NULL, NULL);
	localstring.kifcode = currentkifcode;
	short sn;
	string varname;
	for (it = currentkifcode->kstart->declarations.begin(); it != currentkifcode->kstart->declarations.end(); it++) {
		sn = it->first;
		if (sn <= idstdin)
			continue;
		if (kifGlobals->flfrom != -1 && (sn >= kifGlobals->flfrom && sn <= kifGlobals->flto))
			continue;
		if (!it->second->Variable())
			continue;
		varname = KifIdString(sn);
		if (var != "" && varname != var)
			continue;
		n = nm;
		n += "::";
		n += varname;
		localstring.value = it->second->BaseString();
		debugvariables->Push(n, &localstring);
	}
	if (currentdom != currentdom->kifcode->kstart) {
		nm = KIFLIBSTRINGS[400];
		nm += kifGlobals->kifIdString[currentdom->frame->Name()];
		for (it = currentdom->declarations.begin(); it != currentdom->declarations.end(); it++) {
			sn = it->first;
			if (sn <= idstdin)
				continue;
			if (kifGlobals->flfrom != -1 && (sn >= kifGlobals->flfrom && sn <= kifGlobals->flto))
				continue;
			if (!it->second->Variable())
				continue;
			varname = KifIdString(sn);
			if (var != "" && varname != var)
				continue;
			n = nm;
			n += "::";
			n += varname;
			localstring.value = it->second->BaseString();
			if (debugvariables->values.find(n) == debugvariables->values.end())
				debugvariables->Push(n, &localstring);
		}
	}
	if (currentcallfunc != NULL) {
		nm = KIFLIBSTRINGS[401];
		nm += kifGlobals->kifIdString[currentcallfunc->Name()];
		for (it = currentcallfunc->declarations.begin(); it != currentcallfunc->declarations.end(); it++) {
			varname = KifIdString(it->first);
			if (var != "" && varname != var)
				continue;
			n = nm;
			n += "::";
			n += varname;
			localstring.value = it->second->BaseString();
			debugvariables->Push(n, &localstring);
		}
	}
	kifdebugging = true;
}

bool KifDebugInfo::KifDebugging(KifCode* kifcode, KifElement* ke, KifDomain* dom, KifCallFunction* callfunc) {
	//If a debug function has been provided then we call it, with all the necessary information

	KifElement* debugf = NULL;
	KifElement** ptrdebugf;

	currentcallfunc = NULL;
	currentdom = NULL;

	if (kifGlobals->debugfunction != NULL) {
		debugf = kifGlobals->debugfunction;
		ptrdebugf = &kifGlobals->debugfunction;
	}
	else
	if (debugfunction != NULL) {
		debugf = debugfunction;
		ptrdebugf = &debugfunction;
	}


	if (debugf != NULL) {
		currentcallfunc = callfunc;
		currentdom = dom;
		currentkifcode = kifcode;
		kifdebugging = false;
		KifFunction* func = debugf->Functioncall(dom);
		KifCallFunctionGeneral kfunc(NULL, NULL, func->Name(), func);
		kfunc.kifcode = kifcode;
		KifElement* topname;
		KifElement* linenum;
		KifString* label;
		KifString* kfname;
		string nm = KIFLIBSTRINGS[399];
		if (callfunc != NULL) {
			nm = KIFLIBSTRINGS[401];
			nm += kifGlobals->kifIdString[callfunc->Name()];
		}
		else
		if (dom != dom->kifcode->kstart) {
			nm = KIFLIBSTRINGS[400];
			nm += kifGlobals->kifIdString[dom->frame->Name()];
		}

		long line = ke->Line();
		string fname = kifGlobals->debuginfocurrent->Filename(currentfile);

		topname = kifcode->Providestring(nm);
		topname->Setreference();
		kfname = kifcode->Providestring(fname);
		kfname->Setreference();
		linenum = kifcode->Provideinteger(line);
		linenum->Setreference();
		label = kifcode->Providestringraw("");
		label->Setreference();
		if (kifGlobals->kifLabelIdx.find(line) != kifGlobals->kifLabelIdx.end())
			label->value = kifGlobals->kifLabelIdx[line].label;
		kfunc.parameters.push_back(kfname);
		kfunc.parameters.push_back(topname);
		kfunc.parameters.push_back(linenum);
		kfunc.parameters.push_back(label);
		if (debugdata != NULL)
			kfunc.parameters.push_back(debugdata);
		else
			kfunc.parameters.push_back(kifNULL);

		KifElement* ret = kfunc.Exec(kifNULL, dom, kifNULL, kifGlobals->GetThreadid(), callfunc);

		kifdebugging = true;
		linenum->Resetreference();
		label->Resetreference();
		topname->Setreference();
		kfname->Setreference();
		if (ret->Boolean() == false)
			kifcode->Terminate();
		return true;
	}
	return false;
}

bool KifDebugInfo::Debugging(KifCode* kifcode, KifObject* func, KifElement* ke, KifDomain* dom, KifCallFunction* callfunc) {
	if (kifdebugging == false)
		return true;

	if (kifoutoffunction != 0) {
		if (kifoutoffunction != kiffunctioncount)
			return true;
		kifoutoffunction = 0;
	}
	if (KifDebugging(kifcode, ke, dom, callfunc))
		return true;

	ThreadLock _lock(kifInDebug);
	//If we are debugging in thread, we must not do anything
	if (debuginfunction)
		return true;

	if (gotoline == -10) {
		cout << KIFLIBSTRINGS[402] << Endl;
		gotoline = -5;
	}

	long cline = ke->Line();
	int fileidx = ke->Fileindex();

	if (!kifdebug) {
		if (kifbreakpoints.find(fileidx) == kifbreakpoints.end() || kifbreakpoints[fileidx].find(cline) == kifbreakpoints[fileidx].end())
			return true;
		if (gotoline == cline && kifgotofile == fileidx)
			gotoline = -5;
	}

	kifdebug = false;

	string s;
	if (kiftabledebug.find(fileidx) != kiftabledebug.end()) {
		if (kiftabledebug[fileidx].find(ke->Line()) != kiftabledebug[fileidx].end()) {
			s = kiftabledebug[fileidx][ke->Line()];
			s = Trim(s);
		}
	}

	if (func->Name() != -1 && func->Name() != idsequence)
		cout << "[" << kifGlobals->kifIdString[func->Name()] << "]" << cline << "> " << s << Endl;
	else
		cout << cline << "> " << s << Endl;

	s = "x";
	int compte;
	KifElement* v;
	register int j;
	register short id;
	char buffer[1024];
	while (s != "") {

		if (watches.size()) {
			for (j = 0; j<watches.size(); j++) {
				id = KifNewId(watches[j]);
				v = kifcode->Get(id, dom);
				if (v == NULL || v == kifNULL)
					continue;
				v = v->Returnvalue();
				cout << watches[j] << ": " << v->String() << Endl;
			}
		}

		sprintf_s(buffer, 20, "%d> ", cline);
		cout << buffer;
		compte = strlen(buffer);
		int c;
		s = "";
		int hist = history.size() - 1;
		do {
			c = GETCH(false);
#ifdef WIN32
			if (c == 224) {
				c = GETCH(false);
				s = "";
				DisplayClean(s, buffer, compte);
				compte = 0;

				if (hist<0) {
					hist = 0;
					continue;
				}
				if (hist >= history.size()) {
					hist = history.size() - 1;
					continue;
				}
				if (c == 72) {//up
					s = history[hist];
					hist--;
					cout << s;
					compte = s.size();
					continue;
				}
				if (c == 80) {//down
					s = history[hist];
					hist++;
					cout << s;
					compte = s.size();
					continue;
				}
				continue;
			}
#endif
			if (c == ',' || c == '.') {
				if (s == "" || s[0] != '$') {
					s = "";
					DisplayClean(s, buffer, compte);
					compte = 0;

					if (hist<0) {
						hist = 0;
						continue;
					}
					if (hist >= history.size()) {
						hist = history.size() - 1;
						continue;
					}
					if (c == ',') {//up
						s = history[hist];
						hist--;
					}
					if (c == '.') {//down
						s = history[hist];
						hist++;
					}
					cout << s;
					compte = s.size();
					continue;
				}
			}

			if (c >= 32) {
				s += (char)c;
				cout << (char)c;
				compte++;
			}
			else
			if (c == 8) {
				if (s != "") {
					cout << (char)c << " " << (char)c;
					s = s.substr(0, s.length() - 1);
				}
				c = 33;
			}
		} while (c >= 32);

		if (s == "$") {
			cout << Endl << KIFLIBSTRINGS[403] << Endl;
			kifdebugging = false;
			kifdebug = false;
			kifendofdebug = true;
			return false;
		}

		if (s == "$$") {
			cout << Endl << KIFLIBSTRINGS[403] << Endl;
			exit(-1);
		}

		if (s == "H") {
			cout << Endl;
			for (j = 0; j<history.size(); j++)
				cout << j << ":" << history[j] << Endl;
			continue;
		}

		if (s != "" && s[0] == '!') {
			j = atoi(STR(s) + 1);
			if (j<0 || j >= history.size()) {
				s = "";
				DisplayClean(s, buffer, compte);
				compte = 0;
				continue;
			}
			s = history[j];
			compte = s.size();
			DisplayClean(s, buffer, compte);
		}

		if (s != "")
			history.push_back(s);
		else
			kifdebug = true;

		if (s == "h") {
			cout << Endl << Endl << KIFLIBSTRINGS[404] << Endl;
			cout << KIFLIBSTRINGS[405] << Endl;
			cout << KIFLIBSTRINGS[406] << Endl;
			cout << KIFLIBSTRINGS[407] << Endl;
			cout << KIFLIBSTRINGS[408] << Endl;
			cout << KIFLIBSTRINGS[409] << Endl;
			cout << KIFLIBSTRINGS[410] << Endl;
			cout << KIFLIBSTRINGS[411] << Endl;
			cout << KIFLIBSTRINGS[412] << Endl;
			cout << KIFLIBSTRINGS[413] << Endl;
			cout << KIFLIBSTRINGS[414] << Endl;
			cout << KIFLIBSTRINGS[415] << Endl;
			cout << KIFLIBSTRINGS[416] << Endl;
			cout << KIFLIBSTRINGS[417] << Endl;
			cout << KIFLIBSTRINGS[418] << Endl;
			cout << KIFLIBSTRINGS[419] << Endl;
			cout << "$variable" << Endl;
			cout << "$expression;" << Endl << Endl;
			continue;
		}


		if (s == "i") {
			cout << Endl;
			kifdebug = true;
			return true;
		}


		if (s == "o") {
			cout << Endl;
			kifoutoffunction = kiffunctioncount - 1;
			kifdebug = true;
			return false;
		}

		if (s != "" && s[0] == 'w') {
			cout << Endl;
			string var;
			if (s == KIFLIBSTRINGS[420] || s.size()<3) {
				for (j = 0; j<watches.size(); j++)
					cout << watches[j] << Endl;
				continue;
			}
			var = s.substr(2, s.length() - 2);
			bool found = false;
			for (j = 0; j<watches.size(); j++) {
				if (watches[j] == var) {
					found = true;
					break;
				}
			}
			if (!found)
				watches.push_back(var);
			continue;
		}

		if (s != "" && s[0] == 'r') {
			cout << Endl;
			string var;
			if (s == KIFLIBSTRINGS[421] || s.size()<3) {
				for (j = 0; j<watches.size(); j++)
					cout << watches[j] << Endl;
				continue;
			}
			var = s.substr(2, s.length() - 2);
			for (j = 0; j<watches.size(); j++) {
				if (watches[j] == var) {
					watches.erase(watches.begin() + j);
					break;
				}
			}
			continue;
		}

		vector<string> positions;
		if (s != "" && (s[0] == 'g' || s[0] == 'b' || s[0] == 't')) {
			cout << Endl;
			int nb = Stringsplit(s, " ", positions);
			int filecurrent = fileidx;
			if (nb == 3) {
				j = atoi(STR(positions[1]));
				filecurrent = atoi(STR(positions[2]));
			}
			else {
				if (nb == 2) {
					string sk = "#" + positions[1];
					if (kifGlobals->kifLabels.find(sk) == kifGlobals->kifLabels.end()) {
						if (positions[1][0]<'0' || positions[1][0]>'9') {
							cerr << KIFLIBSTRINGS[422] << sk << endl;
							continue;
						}
						j = atoi(STR(positions[1]));
					}
					else {
						j = kifGlobals->kifLabels[sk].line;
						filecurrent = kifGlobals->kifLabels[sk].fileindex;
					}
				}
			}

			if (s[0] == 'b') {
				if (s == "b")  {
					map<int, map<long, bool> >::iterator itk;
					for (itk = kifbreakpoints.begin(); itk != kifbreakpoints.end(); itk++) {
						map<long, bool>::iterator it;
						for (it = itk->second.begin(); it != itk->second.end(); it++) {
							if (it->second)
								cout << KIFLIBSTRINGS[423] << it->first << KIFLIBSTRINGS[424] << itk->first << " [" << kiftabledebugfilename[itk->first] << "]" << Endl;
							else
								cout << KIFLIBSTRINGS[423] << it->first << KIFLIBSTRINGS[424] << itk->first << Endl;
						}
					}
				}
				else
				if (j == -1) {
					kifbreakpoints.clear();
					cout << KIFLIBSTRINGS[425] << Endl;
				}
				else {
					bool toadd = false;
					if (kifbreakpoints.find(filecurrent) == kifbreakpoints.end()) {
						toadd = true;
						map<long, bool> v;
						kifbreakpoints[filecurrent] = v;
					}
					else
					if (kifbreakpoints[filecurrent].find(j) == kifbreakpoints[filecurrent].end())
						toadd = true;

					if (toadd)
						kifbreakpoints[filecurrent][j] = true;
					else {
						kifbreakpoints[filecurrent].erase(j);
						if (kifbreakpoints[filecurrent].empty())
							kifbreakpoints.erase(filecurrent);
						cout << KIFLIBSTRINGS[426] << Endl;
					}
				}
				continue;
			}
			if (s == "g")
				gotoline = -5;
			else {
				gotoline = j;
				kifgotofile = filecurrent;
			}

			return true;
		}

		int mxj, ln;
		map<long, string>::iterator mx = --kiftabledebug[fileidx].end();
		long maxline = mx->first;
		if (s == "f") {
			cout << Endl;
			for (j = 0; j<kiftabledebugfilename.size(); j++)
				cout << j << ": " << kiftabledebugfilename[j] << Endl;
		}
		else
		if (s == "s") {
			cout << Endl;
			int idthread = kifGlobals->GetThreadid();
			for (int j = kifGlobals->THR[id]->stackelements.size() - 1; j >= 0; j--) {
				string mess = KIFLIBSTRINGS[427];
				if (kifGlobals->THR[id]->stackelements[j]->name != -1)
					mess = kifGlobals->kifIdString[kifGlobals->THR[id]->stackelements[j]->name];
				mess += ": ";
				long line = kifGlobals->THR[id]->stackelements[j]->line;
				string filename = kiftabledebugfilename[kifGlobals->THR[idthread]->stackelements[j]->file];
				sprintf_s(buffer, 1023, KIFLIBSTRINGS[428], line, STR(filename));
				mess += buffer;
				cout << mess << Endl;
			}
		}
		else
		if (s != "" && s[0] == 'l') {
			cout << Endl;
			mxj = Stringsplit(s, " ", positions);

			switch (mxj) {
			case 1:
				ln = 0;
				mxj = maxline;
				break;
			case 2:
				ln = atoi(STR(positions[1]));
				mxj = maxline;
				break;
			default:
				ln = atoi(STR(positions[1]));
				mxj = atoi(STR(positions[2]));
			}
			if (mxj>maxline)
				mxj = maxline;
			while (ln <= mxj) {
				if (kiftabledebug[fileidx].find(ln) == kiftabledebug[fileidx].end()) {
					ln++;
					continue;
				}
				s = kiftabledebug[fileidx][ln];
				cout << ln << "> " << kiftabledebug[fileidx][ln] << Endl;
				ln++;
			}
		}
		else
		if (s != "" && s[0] == 'n') {
			cout << Endl;
			mxj = 1;
			if (s != "n")
				mxj = atoi(STR(s) + 2);
			ln = cline;
			j = 0;
			if ((cline + mxj)>maxline)
				mxj = maxline - cline;

			while (j <= mxj) {
				if (kiftabledebug[fileidx].find(ln) == kiftabledebug[fileidx].end()) {
					ln++;
					j++;
					continue;
				}
				s = kiftabledebug[fileidx][ln];
				cout << ln << "> " << kiftabledebug[fileidx][ln] << Endl;
				ln++;
				j++;
			}
		}
		else
		if (s != "" && s[0] == '$') {
			cout << Endl;
			s = s.substr(1, s.size() - 1);
			if (s[s.size() - 1] != ';') {
				if (kifGlobals->kifStringId.find(s) == kifGlobals->kifStringId.end()) {
					cout << KIFLIBSTRINGS[429] << Endl;
					continue;
				}
				v = kifcode->Get(kifGlobals->kifStringId[s], dom);
				if (v == NULL || v == kifNULL) {
					cout << KIFLIBSTRINGS[429] << Endl;
					continue;
				}
				v = v->Returnvalue();
				cout << v->String() << Endl;
			}
			else {
				KifElement* kres;
				kres = PostEval(kifcode, s, kifTypeSTRING, dom, callfunc, 0);
				if (kres == NULL)
					cout << KIFLIBSTRINGS[430] << Endl;
				else
					cout << kres->String() << Endl;
			}
		}
	}
	char car = 8;
	while (compte>0) {
		cout << car;
		compte--;
	}
	s = "                   ";
	cout << s;
	compte = s.size();
	while (compte >= 0) {
		cout << car;
		compte--;
	}
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------

Exported KifElement* KifKiF::constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
	if (params->InstructionSize() == 0)
		return this;


	KifElement* kinit = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	name = kinit->String();
	kinit->Release();

	KifCode* knew = kifcode->Newinstance();
	id = knew->idkifmain;
	short ret = knew->KifLoadPath(name, DebugPtr()->kifdebugging);
	if (ret == -1)
		return kifRAISEERROR;

	return kifcode->Provideinteger(id);
}

KifElement* KifKiF::Map(KifElement* dom, KifMap* m) {
	ThreadLock _lock(type);
	if (id != -1) {
		KifFrame* kdom = kifGlobals->kifCodeVector[id]->kstart;
		KifMap* kmap = m;
		if (kmap == NULL)
			kmap = new KifMap(kifcode, NULL);
		else
			kmap->Clear();
		bin_hash<KifElement*>::iterator it;
		KifElement* x;
		KifElement* val;

		for (it = kdom->declarations.begin(); it != kdom->declarations.end(); it++) {
			x = it->second;
			short sn = x->Name();
			val = x->Returnvalue();
			if (sn != -1 && val != NULL) {
				if (sn <= idstdin)
					continue;
				if (kifGlobals->flfrom != -1 && (sn >= kifGlobals->flfrom && sn <= kifGlobals->flto))
					continue;

				string nm = kifGlobals->kifIdString[sn];
				kmap->Push(nm, val);
			}
		}
		return kmap;
	}
	return kifNULL;
}

KifKiF::~KifKiF() {
	if (type == kifKiF)
		KifDelete(kifcode->session, id);

	if (kifGlobals != NULL) {
		ThreadLock _lock(kifLoading);
		if (kifGlobals->kifCodePaths.find(name) != kifGlobals->kifCodePaths.end())
			kifGlobals->kifCodePaths.erase(name);
	}
}

//PYTHON SECTION
#ifdef KIFPYTHON
#if PY_MINOR_VERSION==4
#define Py_ssize_t int
#define OLDPYTHON
#endif



extern "C" {
	static PyObject* PythonKifError;


	bool PythonVersString(PyObject* po, string& s) {
		if (PyString_Check(po) == 1) {
			s = PyString_AsString(po);
			return true;
		}

#ifdef HAVE_WCHAR_H
		if (PyUnicode_Check(po)) {
			Py_ssize_t sz = PyUnicode_GetSize(po);
			Py_UNICODE* ibuff = PyUnicode_AsUnicode(po);
			for (int i = 0; i<sz; i++)
				s += s_unicode_to_utf8(ibuff[i]);
			return true;
		}
#else
		if (PyUnicode_Check(po)) {
			Py_ssize_t sz = PyUnicode_GET_DATA_SIZE(po);
			const char* sbuff = PyUnicode_AS_DATA(po);
			for (Py_ssize_t i = 0; i<sz; i++) {
				if (sbuff[i])
					s += sbuff[i];
			}
			return true;
		}
#endif
		return false;
	}


	static PyObject* kif_version(PyObject* self, PyObject* args) {
		return PyString_FromString(KIFVERSION);
	}

	static PyObject* kif_load(PyObject* self, PyObject* args) {
		char* filename;
		char* pargs;
		int mapping = 0;
		int nbelements = PyTuple_Size(args);
		if (nbelements == 2) {
			if (!PyArg_ParseTuple(args, "ss", &filename, &pargs)) {
				PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
				return PyInt_FromLong(-1);
			}
		}
		else {
			if (!PyArg_ParseTuple(args, "ssi", &filename, &pargs, &mapping)) {
				PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
				return PyInt_FromLong(-1);
			}
		}

		int kifhandler = KifRunProgram(filename, pargs);
		if (kifhandler == -1)
			return PyInt_FromLong(-1);

		if (mapping == 1) {
			stringstream code;
			KifCode* kifcode = kifGlobals->kifCodeVector[kifhandler];
			bin_hash<KifElement*>::iterator it;
			KifElement* kfunc;
			string name;
			char var;
			for (it = kifcode->kstart->declarations.begin(); it != kifcode->kstart->declarations.end(); it++) {
				kfunc = it->second;
				if (kfunc != NULL && kfunc->isFunction()) {
					name = kifGlobals->kifIdString[it->first];
					code << "def " << name << "(";
					var = 97;
					int i;
					for (i = 0; i<kfunc->Size(); i++) {
						if (i)
							code << ",";
						code << var;
						var++;
					}
					code << "):\n\treturn kif.kif(" << kifhandler << ",'" << name << "'";
					var = 97;
					for (i = 0; i<kfunc->Size(); i++) {
						code << "," << var;
						var++;
					}
					code << ")\n";
				}
			}
			if (code.str().size() != 0)
				PyRun_SimpleString(code.str().c_str());
		}

		return PyInt_FromLong(kifhandler);
	}

	KifElement* pythonkifclone(KifCode* kifcode, PyObject* po) {
		if (PyBool_Check(po) == 1) {
			if (po == Py_True)
				return new KifBoolean(kifcode, NULL, true);
			return new KifBoolean(kifcode, NULL, false);
		}

		if (PyInt_Check(po) == 1) {
			long l = PyLong_AsLong(po);
			return kifcode->Provideinteger(l);
		}

		if (PyLong_Check(po) == 1) {
			KIFLONG l = PyLong_AsLong(po);
			return new KifLong(kifcode, NULL, l);
		}

		if (PyFloat_Check(po) == 1) {
			double d = PyFloat_AsDouble(po);
			return kifcode->Providefloat(d);
		}

		if (PyString_Check(po) == 1) {
			char* s = PyString_AsString(po);
			return kifcode->Providestringraw(s);
		}

		PyObject* pelement;
		KifElement* e;
		if (PyList_Check(po) == 1) {
			KifVector* kvect = new KifVector(kifcode, NULL);
			int tail = PyList_GET_SIZE(po);
			for (int i = 0; i<tail; i++) {
				//Puis chacun des objets, kj est l'index de l'element
				pelement = PyList_GetItem(po, i);
				e = pythonkifclone(kifcode, pelement);
				if (e == kifRAISEERROR) {
					kvect->Release();
					return kifRAISEERROR;
				}
				kvect->Push(e);
			}
			return kvect;
		}

		if (PyDict_Check(po) == 1) {
			KifMap* kmap = new KifMap(kifcode, NULL);
			Py_ssize_t pos = 0;
			PyObject* key;
			KifElement* k;
			string kval;
			while (PyDict_Next(po, &pos, &key, &pelement)) {
				if (key != NULL && pelement != NULL) {
					k = pythonkifclone(kifcode, key);
					if (k == kifRAISEERROR) {
						kmap->Release();
						return kifRAISEERROR;
					}
					e = pythonkifclone(kifcode, pelement);
					if (e == kifRAISEERROR) {
						kmap->Release();
						return kifRAISEERROR;
					}
					kval = k->String();
					kmap->Push(kval, e);
					k->Release();
				}
			}
			return kmap;
		}

#ifdef HAVE_WCHAR_H
		if (PyUnicode_Check(po)) {
			Py_ssize_t sz = PyUnicode_GetSize(po);
			Py_UNICODE* ibuff = PyUnicode_AsUnicode(po);
			string s;
			for (int i = 0; i<sz; i++)
				s += s_unicode_to_utf8(ibuff[i]);
			return kifcode->Providestringraw(s);
		}
#else
		if (PyUnicode_Check(po)) {
			Py_ssize_t sz = PyUnicode_GET_DATA_SIZE(po);
			const char* sbuff = PyUnicode_AS_DATA(po);
			string s;
			for (Py_ssize_t i = 0; i<sz; i++) {
				if (sbuff[i])
					s += sbuff[i];
			}
			return kifcode->Providestring(s);
		}
#endif
		PyObject* perr = PyObject_Str(po);
		if (perr != NULL) {
			char* s = PyString_AsString(perr);
			e = kifcode->Providestringraw(s);
			Py_DECREF(perr);
			return e;
		}

		return kifRAISEERROR;
	}

	PyObject* kifclone(KifElement* resultat, bool relache) {
		if (resultat->type == kifBoolean) {
			PyObject* res;
			if (resultat == kifTRUE)
				res = Py_True;
			res = Py_False;
			if (relache)
				resultat->Release();
			return res;
		}

		if (resultat->type == kifInteger || resultat->type == kifLong) {
			long l = resultat->Integer();
			if (relache)
				resultat->Release();
			return PyInt_FromLong(l);
		}

		if (resultat->type == kifFloat || resultat->type == kifFraction) {
			double d = resultat->Float();
			if (relache)
				resultat->Release();
			return PyFloat_FromDouble(d);
		}

		PyObject* pcourant;
		PyObject* liste;

		if (resultat->aVectorContainer() || resultat->type == kifList || resultat->type == kifTable) {
			int i = 0;
			liste = PyList_New(0);
			KifIteration* itr = (KifIteration*)resultat->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
				pcourant = kifclone(itr->IteratorValue(), false);
				PyList_Insert(liste, i, pcourant);
				Py_DECREF(pcourant);
				i++;
			}
			itr->Release();
			if (relache)
				resultat->Release();
			return liste;
		}

		agnostring value;
		if (resultat->aMapContainer()) {
			liste = PyDict_New();
			KifIteration* itr = (KifIteration*)resultat->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
				pcourant = kifclone(itr->IteratorValue(), false);
				value = itr->IteratorKeyString();
				PyDict_SetItemString(liste, STR(value), pcourant);
				Py_DECREF(pcourant);
			}
			itr->Release();
			if (relache)
				resultat->Release();
			return liste;
		}

		value = resultat->String();
		if (relache)
			resultat->Release();
#ifdef OLDPYTHON
		if (value.isutf8())
			return PyUnicode_DecodeUTF8(value.c_str(), value.size(), NULL);
#else
		if (value.isutf8())
			return PyUnicode_FromString(value.c_str());
#endif
		return PyString_FromString(STR(value));
	}


	static PyObject* kif_run(PyObject* self, PyObject* args) {
		int nbelements = PyTuple_Size(args);

		if (nbelements<2) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters, at least two parameters are required: kif handler and function name");
			return PyInt_FromLong(-1);
		}

		PyObject* pelement;
		pelement = PyTuple_GetItem(args, 0);
		if (!PyInt_Check(pelement)) {
			PyErr_SetString(PyExc_AttributeError, "Expecting a kif handler");
			return PyInt_FromLong(-2);
		}

		int kifhandler = PyInt_AsLong(pelement);

		pelement = PyTuple_GetItem(args, 1);
		if (!PyString_Check(pelement)) {
			PyErr_SetString(PyExc_AttributeError, "Expecting a string");
			return PyInt_FromLong(-3);
		}

		char* functionname = PyString_AsString(pelement);

		vector<KifElement*> params;
		KifElement* ke;
		KifCode* kifcode = kifGlobals->kifCodeVector[kifhandler];
		if (kifcode == NULL) {
			PyErr_SetString(PyExc_AttributeError, "This kif handler does not match an existing KiF program");
			return PyInt_FromLong(-4);
		}

		for (int i = 2; i<nbelements; i++) {
			pelement = PyTuple_GetItem(args, i);
			ke = pythonkifclone(kifcode, pelement);
			if (ke == kifRAISEERROR) {
				for (i = 0; i<params.size(); i++)
					params[i]->Release();
				PyErr_SetString(PyExc_AttributeError, "No mapping for this python element");
				return PyInt_FromLong(-5);
			}
			params.push_back(ke);
		}

		KifElement* resultat = KifExecuteFunction(kifcode, functionname, params);
		if (Error()) {
			string value = Errorstring();
			PyErr_SetString(PyExc_AttributeError, STR(value));
			Cleanerror();
			return PyInt_FromLong(-6);
		}

		return kifclone(resultat, true);
	}

	static PyMethodDef NoeudMethode[] = {
		{ "whoami", kif_version, METH_VARARGS, "whoami() return the current kif version" },
		{ "load", kif_load, METH_VARARGS, "load(filename,arguments) load a KiF program and return a kif handler" },
		{ "run", kif_run, METH_VARARGS, "run(khandler,function_name,arguments) Execute a KiF function, the argument should be a vector of strings" },
		{ NULL, NULL, 0, NULL }
	};


	Exported void initkif() {
		PyObject* module = Py_InitModule("kif", NoeudMethode);
		PyObject* pDict = PyModule_GetDict(module);
		PythonKifError = PyErr_NewException("kif.error", NULL, NULL);
		PyDict_SetItemString(pDict, "error", PythonKifError);
	}
}
#endif
