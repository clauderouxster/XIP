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
filename   : kifmain.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language
Programmer : Claude ROUX
Reviewer   :
*/

/*
64,64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,252,7,0,0,0,0,0,0,255,31,0,0,0,0,0,128,255,63,
0,0,0,0,0,192,63,32,0,0,0,0,0,224,7,32,0,0,0,0,0,248,0,48,192,15,0,0,0,56,0,16,48,12,0,0,0,24,0,24,24,24,0,0,0,12,0,14,12,24,0,0,0,4,
0,0,6,56,0,0,0,4,0,0,3,56,0,0,0,4,0,0,1,56,0,0,0,4,0,128,1,56,0,0,0,4,0,192,0,56,0,0,0,4,0,96,0,24,0,0,0,12,0,96,0,24,0,0,0,12,0,48,0,
24,0,0,0,24,0,48,0,12,0,0,0,24,0,16,0,12,0,0,0,16,0,16,0,7,15,0,0,240,3,16,0,131,9,192,7,224,255,31,224,193,8,224,14,192,255,255,255,64,
0,112,28,0,0,2,60,64,0,120,56,0,0,3,0,64,0,120,56,0,0,1,0,64,12,120,48,124,128,1,0,66,4,120,48,198,128,64,0,194,7,120,48,207,192,192,199,
7,192,112,48,73,96,206,228,15,193,112,48,121,48,219,48,140,97,112,48,1,152,209,16,120,48,96,96,3,140,208,24,56,28,96,96,30,134,208,8,12,
14,192,192,0,130,217,8,6,7,192,195,0,3,222,152,1,3,128,131,129,1,208,240,192,1,0,15,195,32,216,0,224,0,0,28,110,16,12,0,124,0,0,56,60,48,
6,0,63,0,0,240,124,224,3,128,7,0,0,192,223,3,0,240,1,0,0,0,0,254,255,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,132,8,0,0,0,0,0,0,196,136,143,31,0,0,0,0,100,136,128,0,0,0,0,0,60,136,128,0,0,0,0,0,108,136,131,3,4,0,0,0,196,136,128,0,6,0,0,0,132,
136,128,0,3,0,0,0,132,136,128,128,1,0,0,0,0,128,128,224,0,0,0,0,254,255,255,127,0,0,0,0,254,255,255,31,0,0,0,0,0,0,0,0,0,0,0
*/

#include "kif.h"
#include "kifversion.h"
#include "conversion.h"
#include "kifcontainer.h"

#include <stdio.h>

#ifdef XIPPARSEUR
#include "parsbin.h"
extern VecteurDeParseurs gVecteurParseurs;
void ParseurKifInit(KifCode* kifcodeptr);
#endif

void KifSetKifDebuggingFunction(KifDebugInfo* debug);
void AddALoadingPath(string thelocalname);

extern Exported const char* KIFLTKSDEBUGGERTRINGS[];
extern Exported const char* KIFTHEINFOSTRINGS[];
extern Exported const char* kifdisplayln;
extern Exported const char* kifdisplay;
extern Exported const char* kifelse;
extern Exported const char* kifif;
extern Exported const char* kifelif;
extern Exported const char* kifwhile;
extern Exported const char* kiffor;
extern Exported const char* kifswitch;
void v_split_indent(string& thestr, vector<string>& v);

#ifdef WIN32
void winaffichage(string vrs, string pile, string fname, long ln);
#else
#endif

Exported void KifTerminateAllLibraries(KifCode* kifcodemain);

extern "C" {
#include "kifextern.h"

	bool KifWindowMode();

	void kifsocket_WindowsInit();
	void kiffltk_sessionInit();

	bool kifsys_KifInitialisationModule(KifCode* localkifcode, string version);
	bool kifsparse_KifInitialisationModule(KifCode* kifcode, string version);
	bool kifsqlite_KifInitialisationModule(KifCode* localkifcode, string version);
	bool kifsocket_KifInitialisationModule(KifCode* localkifcode, string version);
	bool xmldoc_KifInitialisationModule(KifCode* localkifcode, string version);
	bool kifltk_KifInitialisationModule(KifCode* localkifcode, string version);
	bool kifregex_KifInitialisationModule(KifCode* localkifcode, string version);
	void FLTKRedraw();

	bool kifltk_Terminate(KifCode* localkifcode);
	bool kifsocket_Terminate(KifCode* localkifcode);
#ifdef WITHCURL
	bool kifcurl_KifInitialisationModule(KifCode* localkifcode, string version);
	bool kifcurl_Terminate(KifCode* localkifcode);
#endif

#ifdef SOUND
	bool kifmp3_KifInitialisationModule(KifCode* localkifcode, string version);
#endif

#ifdef AOSOUND
	bool kifsound_KifInitialisationModule(KifCode* localkifcode, string version);
#endif

#ifdef MACUI    
	void appelaffichage(void* o);
	void surpile(char* svariables, long lg, char*);
	void preparationDebug(char* wnd);
	void* debugenregistre(char* letxt, char** pile, char* w, int idt);
#endif
}

static KifCode* kifinit = NULL;
void ResetGetEditor(KifCode* local);

static void kifinitialsession() {
	kifsocket_WindowsInit();
	kiffltk_sessionInit();
	if (kifinit == NULL) {

		kifinit = new KifCode(10000);


		KifRecordTerminateModule("kifsocket_Terminate", kifsocket_Terminate);
		KifRecordMainLibraries("kifsocket_KifInitialisationModule", kifsocket_KifInitialisationModule);

		KifRecordMainLibraries("kifsparse_KifInitialisationModule", kifsparse_KifInitialisationModule);
		KifRecordMainLibraries("kifsys_KifInitialisationModule", kifsys_KifInitialisationModule);
		KifRecordMainLibraries("kifsqlite_KifInitialisationModule", kifsqlite_KifInitialisationModule);
		KifRecordMainLibraries("xmldoc_KifInitialisationModule", xmldoc_KifInitialisationModule);
		KifRecordMainLibraries("kifregex_KifInitialisationModule", kifregex_KifInitialisationModule);
#ifdef FLTKCODE	    
		KifRecordMainLibraries("kifltk_KifInitialisationModule", kifltk_KifInitialisationModule);
#endif

#ifdef SOUND
		KifRecordMainLibraries("kifmp3_KifInitialisationModule", kifmp3_KifInitialisationModule);
#endif

#ifdef AOSOUND
		KifRecordMainLibraries("kifsound_KifInitialisationModule", kifsound_KifInitialisationModule);
#endif

#ifdef WITHCURL
		KifRecordTerminateModule("kifcurl_Terminate", kifcurl_Terminate);
		KifRecordMainLibraries("kifcurl_KifInitialisationModule", kifcurl_KifInitialisationModule);
#endif
#ifdef MACUI
#ifdef KIFFRENCH
		setenv("KIFLIBS", "/Applications/KIFF.app/Contents/Resources", 1);
#else
		setenv("KIFLIBS", "/Applications/KIFE.app/Contents/Resources", 1);
#endif
#endif
		KifReloadLibraries(kifinit);
	}
}


class KifLocalDebug :public KifDebugInfo {
public:
	string d_name;
	string d_label;
	string d_fname;
	string svariables;
	int dernieretaillepile;
	string dernierefonction;
	vector<string> pile;
	char** piles;
	bool donottest;
	bool entre;

	bool suivant;
	bool fin;
	bool terminer;
	int strickthread;
	int lastline;

	map<string, map<long, bool> > arrets;
	vector<string> choixvariables;


	map<string, string> principales;
	map<string, string> fonctions;
	map<string, string> classes;

	void SelectionVariables(string& s) {
		choixvariables.clear();
		string sub;
		for (int i = 0; i<s.size(); i++) {
			if (s[i] != '\n')
				sub += s[i];
			else {
				choixvariables.push_back(sub);
				sub = "";
			}
		}
	}

	int AddSelectionVariable(string& s) {
		choixvariables.push_back(s);
		return choixvariables.size() - 1;
	}

	void GetVariables(string& v) {
		v = "";
		for (int i = 0; i<choixvariables.size(); i++) {
			v += choixvariables[i];
			v += Endl;
		}
	}

	void RemoveVariables(int m) {
		choixvariables.erase(choixvariables.begin() + m, choixvariables.end());
	}

	bool TestVariable(string s) {
		if (s == "kifsys")
			return false;
		if (donottest || choixvariables.size() == 0)
			return true;
		for (int i = 0; i<choixvariables.size(); i++)
		if (choixvariables[i] == s)
			return true;
		return false;
	}

	KifLocalDebug() : KifDebugInfo() {
		lastline = -1;
		piles = NULL;
	}

	void nettoietout() {
		lastline = -1;
		strickthread = -100;
		dernieretaillepile = 0;
		dernierefonction = "";
		ClearFilename();
		arrets.clear();
		suivant = false;
		entre = false;
		fin = false;
		kifdebugging = false;
		kifoutoffunction = 0;
		kiffunctioncount = 1;
		terminer = false;
	}

	void modedebug(bool v) {
		kifdebugging = v;
		suivant = v;
	}

	void listebreakpoint(string fidx, vector<long>& v) {
#ifdef WIN32
		fidx = s_to_lower(fidx);
#endif
		if (arrets.find(fidx) != arrets.end()) {
			map<long, bool>::iterator it;
			for (it = arrets[fidx].begin(); it != arrets[fidx].end(); it++)
				v.push_back(it->first);
		}
	}

	bool isbreakpoint(string fidx, long l) {
#ifdef WIN32
		fidx = s_to_lower(fidx);
#endif
		if (arrets.find(fidx) != arrets.end()) {
			if (arrets[fidx].find(l) != arrets[fidx].end())
				return true;
		}
		return false;
	}

	int ajoutebreakpoint(string fidx, long l) {
#ifdef WIN32
		fidx = s_to_lower(fidx);
#endif
		if (arrets.find(fidx) != arrets.end()) {
			if (arrets[fidx].find(l) != arrets[fidx].end()) {
				arrets[fidx].erase(l);
				if (arrets[fidx].size() == 0)
					arrets.erase(fidx);
				if (arrets.size() == 0)
					kifdebugging = false;
				return -2;
			}
		}
		kifdebugging = true;

		suivant = false;
		arrets[fidx][l] = true;
		return 1;
	}

	bool KifDebugging(KifCode* kcode, KifElement* ke, KifDomain* dom, KifCallFunction* callfunc) {
		//In multi-threading, it is better to control to have only one thread at a time accessing that stuff...    
		if (fin || kifGlobals->executionbreak) {
			kifdebugging = false;
			return true;
		}

		int id = kifGlobals->GetThreadid();

#ifdef MACUI
		void* o;
		{
#endif
			ThreadLock _lock(kifInDebug);
			if (strickthread >= -1 && strickthread != id)
				return true;

			if (terminer) {
				terminer = false;
				kcode->Terminate();
				return true;
			}

			long line = ke->Line();
			if (line == -1)
				line = currentline;

			if (ke->Fileindex() != -1)
				d_fname = kifGlobals->debuginfocurrent->kiftabledebugfilename[ke->Fileindex()];
			else
				d_fname = kifGlobals->debuginfocurrent->CurrentFilename();


			if (!suivant) {
				lastline = -1;
				if (arrets.find(d_fname) == arrets.end() || arrets[d_fname].find(line) == arrets[d_fname].end())
					return true;
			}

			if (kifinfunction == true) {
				if (entre == false) {
					kifoutoffunction = kiffunctioncount - 1;
					lastline = -1;
					return true;
				}
			}


			if (lastline != -1 && lastline == line)
				return true;

			currentline = line;
			lastline = line;
			entre = false;
			suivant = false;
			currentcallfunc = callfunc;
			currentdom = dom;
			currentkifcode = kcode;
			d_name = KIFLIBSTRINGS[615];
			if (callfunc != NULL) {
				d_name = KIFLIBSTRINGS[617];
				d_name += kifGlobals->kifIdString[callfunc->Name()];
			}
			else
			if (dom != dom->kifcode->kstart) {
				d_name = KIFLIBSTRINGS[616];
				d_name += kifGlobals->kifIdString[dom->frame->Name()];
			}

			if (kifGlobals->kifLabelIdx.find(line) != kifGlobals->kifLabelIdx.end())
				d_label = kifGlobals->kifLabelIdx[line].label;

			donottest = true;
			ChargeVariables(currentkifcode, currentdom, currentcallfunc);

			ChargePile(id, line);

#ifdef MACUI
			if (piles != NULL)
				delete[] piles;
			piles = new char*[pile.size() + 1];
			int szpile = pile.size();
			for (int i = 0; i<szpile; i++)
				piles[szpile - i - 1] = STR(pile[i]);
			piles[szpile] = NULL;
			o = debugenregistre(STR(svariables), piles, STR(d_fname), id);
		}
		appelaffichage(o);
#endif
#ifdef WIN32
		string lapile;
		for (int i = pile.size() - 1; i >= 0; i--) {
			lapile += pile[i];
		}
		winaffichage(svariables, lapile, d_fname, line);
#endif
		return true;
	}

	void ChargePile(int id, long lg) {
		pile.clear();
		vector<Stackelement*>& elements = kifGlobals->THR[id]->stackelements;

		char ch[1024];
		char fn[1024];
		string fonc;
		char* pt;
		string ptt("BASE");

		for (int i = 0; i<elements.size(); i++) {
			lg = elements[i]->line;
			fonc = kifGlobals->KifIdString(elements[i]->name);
			string filename = Filename(elements[i]->file);
			if (filename != "") {
				strcpy(fn, STR(filename));
				pt = strrchr(fn, '/');
				if (pt != NULL)
					pt++;
				else
					pt = fn;
#ifdef WIN32
				if (fonc != "") {
					sprintf(ch, "[%d] %-30s\t(%ld:%s) (%d)\r\n", i, STR(fonc), lg, pt, id + 1);
					ptt = fonc;
				}
				else
					sprintf(ch, "[%d] %-30s\t(%ld:%s) (%d)\r\n", i, STR(ptt), lg, pt, id + 1);
#else
				if (fonc != "") {
					sprintf(ch, "[%d] %-30s\t(%ld:%s) (%d)\n", i, STR(fonc), lg, pt, id + 1);
					ptt = fonc;
				}
				else
					sprintf(ch, "[%d] %-30s\t(%ld:%s) (%d)\n", i, STR(ptt), lg, pt, id + 1);
#endif
			}
			else {
#ifdef WIN32
				if (fonc != "") {
					sprintf(ch, "[%d] %-30s\t(%ld) (%d)\r\n", i, STR(fonc), lg, id + 1);
					ptt = fonc;
				}
				else
					sprintf(ch, "[%d] %-30s\t(%ld) (%d)\r\n", i, STR(ptt), lg, id + 1);
#else
				if (fonc != "") {
					sprintf(ch, "[%d] %-30s\t(%ld) (%d)\n", i, STR(fonc), lg, id + 1);
					ptt = fonc;
				}
				else
					sprintf(ch, "[%d] %-30s\t(%ld) (%d)\n", i, STR(ptt), lg, id + 1);
#endif
			}
			pile.push_back(ch);
		}
	}

	void RechargeVariables() {
		donottest = false;
		ChargeVariables(currentkifcode, currentdom, currentcallfunc);
	}

	void CherchePile(int l) {
		int idx = pile.size() - l - 1;
		if (idx<0)
			return;
		int id = kifGlobals->GetThreadid();
		KifDomain* dom = NULL;
		KifDomain* func = NULL;
		KifCode* kcode = currentkifcode;
		int idstack = idx;
		if (id == 0)
			idstack--;
		if (idstack >= 0) {
			dom = kifGlobals->THR[id]->threadstacks[idstack * 2];
			func = kifGlobals->THR[id]->threadstacks[idstack * 2 + 1];
		}
		if (dom != NULL)
			kcode = dom->kifcode;
		ChargeVariables(kcode, dom, func);
		string fname = Filename(kifGlobals->THR[id]->stackelements[idx]->file);
		long lg = kifGlobals->THR[id]->stackelements[idx]->line;
#ifdef MACUI
		surpile(STR(svariables), lg, STR(fname));
#endif
	}

	void ChargeVariables(KifCode* ck, KifDomain* cd, KifDomain* ccf) {
		principales.clear();
		classes.clear();
		fonctions.clear();

		bin_hash<KifElement*>::iterator it;
		short sn;
		string varname;
		string localvalue;
		for (it = ck->kstart->declarations.begin(); it != ck->kstart->declarations.end(); it++) {
			sn = it->first;
			if (sn <= idretract)
				continue;
			if (kifGlobals->flfrom != -1 && (sn >= kifGlobals->flfrom && sn <= kifGlobals->flto))
				continue;
			if (!it->second->Variable())
				continue;
			varname = KifIdString(sn);
			if (!TestVariable(varname))
				continue;
			localvalue = it->second->BaseString();
			principales[varname] = localvalue;
		}
		if (cd != NULL && cd != cd->kifcode->kstart) {
			for (it = cd->declarations.begin(); it != cd->declarations.end(); it++) {
				sn = it->first;
				if (sn <= idretract)
					continue;
				if (kifGlobals->flfrom != -1 && (sn >= kifGlobals->flfrom && sn <= kifGlobals->flto))
					continue;
				if (!it->second->Variable())
					continue;
				varname = KifIdString(sn);
				if (!TestVariable(varname))
					continue;
				localvalue = it->second->BaseString();
				if (classes.find(varname) == classes.end())
					classes[varname] = localvalue;
			}
		}
		if (ccf != NULL) {
			for (it = ccf->declarations.begin(); it != ccf->declarations.end(); it++) {
				varname = KifIdString(it->first);
				if (!TestVariable(varname))
					continue;
				localvalue = it->second->BaseString();
				fonctions[varname] = localvalue;
			}
		}

		svariables = "";
		map<string, string>::iterator itx;
		if (principales.size() != 0) {
			svariables += "Variables principales:";
			svariables += Endl;
			for (itx = principales.begin(); itx != principales.end(); itx++) {
				if (itx->first.find("XIP_") != -1)
					continue;
				svariables += itx->first;
				svariables += " = ";
				svariables += itx->second;
				svariables += Endl;
			}
		}
		if (classes.size() != 0) {
			svariables += Endl;
			svariables += "Variables de la classe:";
			svariables += Endl;
			for (itx = classes.begin(); itx != classes.end(); itx++) {
				svariables += itx->first;
				svariables += " = ";
				svariables += itx->second;
				svariables += Endl;
			}
		}
		if (fonctions.size() != 0) {
			svariables += Endl;
			svariables += "Variables de la fonction:";
			svariables += Endl;
			for (itx = fonctions.begin(); itx != fonctions.end(); itx++) {
				svariables += itx->first;
				svariables += " = ";
				svariables += itx->second;
				svariables += Endl;
			}
		}
	}

};


KifDebugInfo* DebugPtr();

static KifLocalDebug mondebug;
static bool encours = false;
#ifdef XIPPARSEUR
static KifCodeParseur* localkifcode = NULL;
#else
static KifCode* localkifcode = NULL;
#endif
static ostringstream* os = NULL;
static string conversion;
static string msgerreur;
static string codes;
static string codeindente;
static string lastpathname;
static string leslignes;
static int lastmsg;
static int firstinstruction;
static bool firstinit = true;
static string lescurrentesvariables;
static vector<string> lesprevious;
static string forwhileif;
static LockedThread* loquet = NULL;
static LockedThread* wloquet = NULL;
static ThreadLock* klock = NULL;

extern "C" {

	const char* kifForWhileRegular() {
		if (forwhileif.size() == 0) {
			forwhileif = "[";
			forwhileif += kifwhile;
			forwhileif += "|";
			forwhileif += kiffor;
			forwhileif += "|";
			forwhileif += kifif;
			forwhileif += "|";
			forwhileif += kifswitch;
			forwhileif += "|";
			forwhileif += kifelif;
			forwhileif += "] \\(.+\\)";
		}
		return forwhileif.c_str();
	}

#ifdef MACUI
	void affichagedirect(char*);
	void affichageresultatmac(string s, void* object) {
		if (s.size() == 0)
			return;
		char* str = (char*)malloc(s.size() + 1);
		strcpy(str, s.c_str());
		affichagedirect(str);
	}
#endif

	void kifthreadlock() {
		klock = new ThreadLock(kifDebuggingLock);
	}

	void kifthreadunlock() {
		if (klock != NULL)
			delete klock;
		klock = NULL;
	}

	void kifBlockWindow() {
		if (wloquet == NULL)
			wloquet = new LockedThread;
		wloquet->Blocked();
	}

	void kifReleaseWindow() {
		if (wloquet != NULL) {
			wloquet->Released();
			delete wloquet;
		}
		wloquet = NULL;
	}

	void kifBlock() {
		if (loquet == NULL)
			loquet = new LockedThread;
		loquet->Blocked();
	}

	void kifRelease() {
		if (loquet != NULL) {
			loquet->Released();
			delete loquet;
		}
		loquet = NULL;
	}

	void kifsticktothread(int id) {
		mondebug.strickthread = id - 1;
	}

	int kifaddselection(string& s) {
		return mondebug.AddSelectionVariable(s);
	}

	void kifremove(int i) {
		mondebug.RemoveVariables(i);
	}

	void SelectionVariables(string& s) {
		mondebug.SelectionVariables(s);
	}

	void GetVariables(string& s) {
		mondebug.GetVariables(s);
	}

	void ReinitVariables(string& s) {
		mondebug.RechargeVariables();
		s = mondebug.svariables;
	}

	int KifCurrentThreadId() {
		return kifGlobals->GetThreadid() + 1;
	}

	const char* kifDisplayLocalVariable(const char* ch) {
		vector<string> v;
		bool added = false;
		lescurrentesvariables = ch;
		int pos = -1;
		v_tokenize(v, lescurrentesvariables, 0);
		int i;
		for (i = 0; i<lesprevious.size(); i++) {
			if (pos == -1)
				pos = kifaddselection(lesprevious[i]);
			else
				kifaddselection(lesprevious[i]);
		}
		lesprevious.clear();
		for (i = 0; i<v.size(); i++) {
			int j = 0, ch = 0;
			if (v.size() >= 1)
				ch = (unsigned char)v[i][0];
			if (ch == 95 || ch>128 || c_is_alpha(USTR(v[i]), j)) {
				if (pos == -1)
					pos = kifaddselection(v[i]);
				else
					kifaddselection(v[i]);
				added = true;
				lesprevious.push_back(v[i]);
			}
		}

		if (added) {
			lescurrentesvariables = "";
			ReinitVariables(lescurrentesvariables);
			kifremove(pos);
		}
		return lescurrentesvariables.c_str();
	}

	bool KifSetDisplayFunction(KifDisplayFunction f, void* o) {
		if (kifGlobals == NULL)
			return false;
		kifGlobals->displayfunction = f;
		kifGlobals->displayobject = o;
		return true;
	}


	bool windowmode() {
		return KifWindowMode();
	}


	void arretprg() {
		if (!encours)
			return;

		if (localkifcode != NULL) {
			localkifcode->Terminate();
			FLTKRedraw();
			KifTerminateAllLibraries(localkifcode);
		}
#ifdef MACUI
		encours = false;
#endif
	}

	KifElement* PreExitExtern(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
		arretprg();
		return kifTRUE;
	}

	void termination() {
		mondebug.terminer = true;
	}

	void lesuivant(bool v) {
		mondebug.suivant = v;
	}

	void entredans(bool v) {
		mondebug.suivant = v;
		mondebug.entre = v;
	}

	void modebug(bool v) {
		mondebug.modedebug(v);
	}

	bool isdebug() {
		return mondebug.kifdebugging;
	}

	void jusquefin() {
		mondebug.fin = true;
	}

	long laligne() {
		return mondebug.currentline;
	}

	int ajoutebreak(const char* c, long l) {
		return mondebug.ajoutebreakpoint(c, l);
	}

	bool isbreakpoint(const char* c, long l) {
		return mondebug.isbreakpoint(c, l);
	}

	void listebreakpoint(const char* c, vector<long>& v) {
		return mondebug.listebreakpoint(c, v);
	}

	void sortiefonction() {
		mondebug.kifoutoffunction = mondebug.kiffunctioncount - 1;
		mondebug.suivant = true;
	}

	void nettoyerpointarrets() {
		mondebug.arrets.clear();
	}

	bool kifencours() {
		if (kifGlobals != NULL) {
			ThreadLock _lock(kifAny);
			return encours;
		}
		return encours;
	}

	void kifEndencours() {
		if (kifGlobals != NULL) {
			ThreadLock _lock(kifAny);
			encours = false;
		}
		encours = false;
	}

	void kifpile(int l) {
		mondebug.CherchePile(l);
	}

#ifdef XIPPARSEUR
	void kifcreation() {
		KifRecreatekifGlobals(kifinit);
		localkifcode = new KifCodeParseur(NULL, 10000, NULL, os);
		localkifcode->master = true;
		KifReloadLibraries(localkifcode);
		ParseurKifInit(localkifcode);
	}
#else
	void kifcreation() {
		KifRecreatekifGlobals(kifinit);
		localkifcode = new KifCode(10000);
		KifReloadLibraries(localkifcode);
	}

#endif

	bool openkifsession() {
		if (encours)
			return false;
		if (localkifcode != NULL)
			return true;
		lastmsg = 0;
		kifEndencours();
		lesprevious.clear();

		if (firstinit)
			firstinit = false;

		kifinitialsession();

		if (os != NULL)
			delete os;
		os = new ostringstream;
		kifcreation();
		kifGlobals->executionbreak = 0;
		kifGlobals->doubledisplay = true;
		kifGlobals->os = os;
		localkifcode->session = true;
		KifSetKifDebuggingFunction(&mondebug);
		ResetGetEditor(localkifcode);
#ifdef MACUI
		kifGlobals->doubledisplay = false;
		KifSetDisplayFunction(affichageresultatmac, NULL);
#endif
		mondebug.terminer = false;
		if (kifGlobals != NULL)
			Cleanallerror();
		return true;
	}

	bool closekifsession() {
		if (encours)
			return false;
		codes = "";
		if (localkifcode != NULL)
			delete localkifcode;
		localkifcode = NULL;
		if (kifGlobals != NULL)
			kifGlobals->os = &cout;

		mondebug.nettoietout();
		return true;
	}

	bool closefinalkif() {
		kifEndencours();
		if (localkifcode != NULL)
			delete localkifcode;
		localkifcode = NULL;
		if (os != NULL)
			delete os;
		os = NULL;
		firstinit = true;
		if (kifinit != NULL) {
			kifinit->session = true;
			delete kifinit;
			kifGlobals = NULL;
			kifinit = NULL;
		}
		return true;
	}

	bool compileandrun(const char* codeinit) {
		if (codeinit == NULL)
			return false;
		string code;
		code = kifdisplayln;
		code += "(";
		code += codeinit;
		code += ");";

		KifCode* local = new KifCode(10000);

		int firstinst = local->kstart->instructions.size();
		int igarbage = kifGlobals->garbage.size();
		int ilocal = local->locals.size();
		int cursor = kifGlobals->cursor;

		bin_hash<KifElement*> stack;
		bin_hash<KifElement*>::iterator it;

		vector<KifDomain*> keepstack;
		int i;
		int id = -1;

		for (i = 0; i<kifGlobals->THR[id]->threadstacks.size(); i++)
			keepstack.push_back(kifGlobals->THR[id]->threadstacks[i]);
		kifGlobals->THR[id]->threadstacks.clear();

		int startlocal = local->locals.size();
		local->kstart->adding = true;
		kifGlobals->PushStack(local->kstart);
		code = conversion_latin_to_utf8(code);
		//We evaluate in kstart...
		kifGlobals->bnfkif->baseline = 1;
		local->Evaluate(local->kstart, code, false);
		kifGlobals->PopStack();
		if (Errorid(id) == true) {
			msgerreur = Errorstring();
			kifGlobals->THR[id]->cleanerror();
			kifGlobals->garbage.dernier = igarbage;
			kifGlobals->cursor = cursor;
			delete local;
			return false;
		}

		msgerreur = "";
		kifGlobals->doubledisplay = false;
		kifGlobals->endofexecution = false;
		bool toadd = localkifcode->kstart->adding;
		localkifcode->kstart->adding = false;
		local->kstart->Run(firstinst);
		localkifcode->kstart->adding = toadd;
		kifGlobals->doubledisplay = true;

		for (i = 0; i<keepstack.size(); i++)
			kifGlobals->THR[id]->threadstacks.push_back(keepstack[i]);

		//then from igarbage up to the end, we reference our elements
		if (Errorid(id) == true) {
			msgerreur = Errorstring();
			kifGlobals->THR[id]->cleanerror();
			delete local;
			return false;
		}

		delete local;
		return true;
	}

	const char* kifgetlineandfile(const char* msg, long* line, const char* cline, const char* dans) {
		conversion = msg;
		vector<string> v;
		string ligne;
		v_tokenize(v, conversion, token_period);
		conversion = "";
		for (int i = 0; i<v.size(); i++) {
			if (conversion != "" && ligne != "")
				break;

			if (v[i] == cline) {
				if (ligne == "") {
					i++;
					while (i<v.size() && (v[i][0]<48 || v[i][0]>57)) i++;
					if (i<v.size())
						ligne = v[i];
				}
			}
			else
			if (v[i] == dans && conversion == "") {
				i++;
				if (v[i] != ":")
					conversion = v[i];
			}
		}
		*line = atol(ligne.c_str());
		return conversion.c_str();
	}

	int compilekifsession(const char* codeinit) {
		if (localkifcode == NULL || codeinit == NULL)
			return -2;

		bool sauver = true;
		string code = codeinit;
		Trim(code);
		//		if (code[code.length() - 1] != ';' && code[code.length() - 1] != '}' && code[code.length() - 1] != '{') {
		if (code.find(";", 0) == string::npos &&  code.find("{", 0) == string::npos && code.find("}", 0) == string::npos) {
			code = kifdisplayln;
			code += "(";
			code += codeinit;
			code += ");";
			sauver = false;
		}
		else //On remet tous les blancs
			code = codeinit;

		msgerreur = "";
		int i;
		firstinstruction = localkifcode->kstart->instructions.size();
		int igarbage = kifGlobals->garbage.size();
		int ilocal = localkifcode->locals.size();
		int cursor = kifGlobals->cursor;

		bin_hash<KifElement*> stack;
		bin_hash<KifElement*>::iterator it;
		for (it = localkifcode->kstart->declarations.begin(); it != localkifcode->kstart->declarations.end(); it++)
			stack[it->first] = it->second;

		vector<KifDomain*> keepstack;
		int id = 0;

		for (i = 0; i<kifGlobals->THR[id]->threadstacks.size(); i++)
			keepstack.push_back(kifGlobals->THR[id]->threadstacks[i]);
		kifGlobals->THR[id]->threadstacks.clear();

		int startlocal = localkifcode->locals.size();
		localkifcode->kstart->adding = true;
		localkifcode->addtolocal = true;
		kifGlobals->PushStack(localkifcode->kstart);
		code = conversion_latin_to_utf8(code);

		kifGlobals->executionbreak = 0;
		kifGlobals->bnfkif->baseline = 1;
		localkifcode->Evaluate(localkifcode->kstart, code, false);
		kifGlobals->PopStack();
		localkifcode->addtolocal = false;
		for (i = 0; i<keepstack.size(); i++)
			kifGlobals->THR[id]->threadstacks.push_back(keepstack[i]);
		int j;
		//then from igarbage up to the end, we reference our elements
		if (Errorid(id) == true) {
			msgerreur = Errorstring();
			kifGlobals->THR[id]->cleanerror();
			kifGlobals->garbage.dernier = igarbage;
			kifGlobals->cursor = cursor;
			return -1;
		}

		for (i = startlocal; i<localkifcode->locals.size(); i++) {
			j = localkifcode->locals[i];
			if (kifGlobals->garbage[j] != NULL) {
				kifGlobals->garbage[j]->Setpopped();
				kifGlobals->garbage[j]->Setreference();
			}
		}

		if (sauver) {
			codes += code;
			codes += Endl;
		}
		return firstinstruction;

	}

	void kifsetarguments(string args) {
		if (localkifcode == NULL || args == "")
			return;
		vector<string> vargs;
		v_split(args, " ", vargs);
		localkifcode->InitArguments(vargs);
	}

	bool savekifsession(const char* filename) {
		if (filename == NULL) {
			msgerreur = "Aucun fichier en mémoire";
			return false;
		}
		if (codes == "") {
			msgerreur = "Aucun programme en mémoire";
			return false;
		}
		ofstream f(filename);
		f << STR(codes) << endl;
		return(true);
	}

	void kiffilename(const char* filename) {
		if (localkifcode == NULL)
			return;
		AddALoadingPath(filename);
		lastpathname = filename;
		DebugPtr()->AddFilename(lastpathname, localkifcode);
	}

	bool loadkifsession(const char* filename) {
		closekifsession();
		openkifsession();

		string c;
		if (filename == NULL) {
			msgerreur = "Aucun fichier en mémoire";
			return false;
		}

		ifstream* f = new ifstream(filename, modeOuverture);
		if (f->fail()) {
			msgerreur = "Fichier inconnu: ";
			msgerreur += filename;
			return false;
		}
		lastpathname = filename;
		codes = "";
		bool first = true;
		while (!f->eof()) {
			getline(*f, c);
			if (first) {
				if (c.size() >= 3 && (uchar)(c[0]) == 239 && (uchar)(c[1]) == 187 && (uchar)(c[2]) == 191)
					c = c.substr(3, c.size() - 3);
				first = false;
			}
			codes += c + Endl;
		}

		f->close();
		cr_normalise(codes);
		f->open(filename, modeOuverture);
		try {
			localkifcode->Setbaseline(1);
			localkifcode->Loadfile(f, filename, 0);
		}
		catch (KifRaiseError* m) {
			lastmsg = 1;
			msgerreur = m->message;
			return false;
		}

		localkifcode->Run();
		if (Error() == true) {
			msgerreur = Errorstring();
			Cleanerror();
		}
		lastmsg = 2;
		return true;
	}

	void infoerrorsbis(long* l, char* filename) {
		*l = kifGlobals->debuginfocurrent->currentline;
		string s = kifGlobals->debuginfocurrent->Filename(kifGlobals->debuginfocurrent->currentfile);
		filename[0] = 0;
		if (s.size())
			strcpy(filename, STR(s));
	}

	const char* infoerrors(long* l) {
		static string s;
		*l = kifGlobals->debuginfocurrent->currentline;
		s = kifGlobals->debuginfocurrent->Filename(kifGlobals->debuginfocurrent->currentfile);
		return STR(s);
	}

	bool loadkiffile(const char* filename) {
		closekifsession();
		openkifsession();

		string c;
		if (filename == NULL) {
			msgerreur = "Aucun fichier en mémoire";
			return false;
		}

		ifstream* f = new ifstream(filename, modeOuverture);
		if (f->fail()) {
			msgerreur = "Fichier inconnu: ";
			msgerreur += filename;
			return false;
		}
		codes = "";
		bool first = true;
		while (!f->eof()) {
			getline(*f, c);
			if (first) {
				if (c.size() >= 3 && (uchar)(c[0]) == 239 && (uchar)(c[1]) == 187 && (uchar)(c[2]) == 191)
					c = c.substr(3, c.size() - 3);
				first = false;
			}
			codes += c + Endl;
		}

		f->close();
		cr_normalise(codes);
		kiffilename(filename);
		return true;
	}

	char* lastloadedfile() {
		return STR(lastpathname);
	}

	char* theerrormsg() {
		Trim(msgerreur);
		if (msgerreur.size() != 0) {
			if (msgerreur.size()>1024) {
				int ps = msgerreur.find("\n", 1024);
				if (ps != -1)
					msgerreur = msgerreur.substr(0, ps + 1);
				else
					msgerreur = msgerreur.substr(0, 1024);
			}

			msgerreur = Endl + msgerreur;
			msgerreur += Endl;
		}
		return STR(msgerreur);
	}

	bool sessionactive() {
		if (localkifcode == NULL)
			return false;
		return true;
	}

	size_t ouvrante(string& ligne) {
		size_t p, pp;
		p = ligne.find("(", 0);
		pp = ligne.find("{", 0);
		p = min(p, pp);
		pp = ligne.find("[", 0);
		p = min(p, pp);

		if (p == string::npos) {
			p = ligne.find(" ", 0);
			if (p == string::npos)
				return 0;
		}
		return p;
	}

	int Findcomments(string& ligne) {
		int i;
		int mx = ligne.size();
		for (i = 0; i<mx; i++) {
			if (ligne[i] == '"') {
				i++;
				while (i<mx && ligne[i] != '"') {
					if (ligne[i] == '\\')
						i++;
					i++;
				}
			}
			else
			if (ligne[i] == '\'') {
				i++;
				while (i<mx && ligne[i] != '\'')
					i++;
			}
			else
			if (i<mx - 1 && ligne[i] == '/' && ligne[i + 1] == '/')
				return i;
		}
		return -1;
	}

	const char* lindentation(char* basecode, int blancs) {
		if (basecode == NULL) {
			codeindente = "";
			return codeindente.c_str();
		}

		string codestr = basecode;
		vector<string> vargs;
		vector <int> iblancs;
		cr_normalise(codestr);
		v_split_indent(codestr, vargs);
		codeindente = "";
		IndentationCode(codeindente, vargs, iblancs, blancs, true, kifelse, kifelif, kifif);
		Trimright(codeindente);
		if (codeindente.find("/@") != string::npos || codeindente.find("@\"") != string::npos)
			cr_normalise(codeindente);
		codeindente += "\n";
		return codeindente.c_str();
	}

	const char* indentation(char* basecode, string blanc) {
		string codestr = basecode;
		vector <int> iblancs;
		vector<string> vargs;
		cr_normalise(codestr);
		v_split_indent(codestr, vargs);
		codeindente = "";
		IndentationCode(codeindente, vargs, iblancs, blanc.size(), true, kifelse, kifelif, kifif);
		Trimright(codeindente);
		codeindente += "\n";
		if (codeindente.find("/@") != string::npos || codeindente.find("@\"") != string::npos)
			cr_normalise(codeindente);
		return codeindente.c_str();
	}

	void cleancodeindente() {
		codeindente = "";
	}

	int indentationvirtuel(string& codestr) {
		vector <int> iblancs;
		vector<string> vargs;
		cr_normalise(codestr);
		v_split_indent(codestr, vargs);
		codeindente = "";
		IndentationCode(codeindente, vargs, iblancs, 0, false, kifelse, kifelif, kifif);
		if (iblancs.size() == 0)
			return 0;
		return iblancs.back();
	}

	int indentationVirtuel(char* cr, char* acc) {
		if (cr == NULL)
			return 0;
		vector <int> iblancs;
		vector<string> vargs;
		string codestr(cr);
		cr_normalise(codestr);
		v_split_indent(codestr, vargs);
		if (vargs.size()>2) {
			codestr = vargs[vargs.size() - 2];
			if (codestr.find('}', 0) != string::npos || codestr.find(kifelse) != string::npos)
				*acc = 1;
		}
		codeindente = "";
		IndentationCode(codeindente, vargs, iblancs, 0, false, kifelse, kifelif, kifif);
		if (iblancs.size() == 0)
			return 0;
		return iblancs.back();
	}


	long kifgotoline(char* lecode, int nb, long* mx) {
		long ligne = 1;
		char* pt = strchr(lecode, '\n');
		long taille = 0;
		while (pt != NULL) {
			if (ligne == nb) {
				*pt = 0;
				*mx = size_c(lecode);
				*pt = '\n';
				return taille;
			}
			*pt = 0;
			taille += size_c(lecode) + 1;
			*pt = '\n';
			ligne++;
			lecode = pt + 1;
			pt = strchr(lecode, '\n');
		}
		return -1;
	}

	char* kifligneenligne(char* lecode, long* mx, char p) {
		if (p != 0) {
			*lecode = '\n';
			lecode++;
		}
		char* pt = strchr(lecode, '\n');
		if (pt != NULL) {
			*pt = 0;
			*mx = size_c(lecode);
		}
		return pt;
	}

	const char* LesLignes(char* lecode) {
		int lignes = 1;
		char buf[10];
		leslignes = "";
		char* pt = strchr(lecode, '\n');
		while (pt != NULL) {
#ifdef WIN32
			sprintf_s(buf, 10, "%d\r\n", lignes);
#else
			sprintf(buf, "%d\n", lignes);
#endif
			leslignes += buf;
			lignes++;
			lecode = pt + 1;
			pt = strchr(lecode, '\n');
		}
		return leslignes.c_str();
	}

	char* ligneparligne(char* lecode, char p) {
		if (p != 0) {
			*lecode = '\n';
			lecode++;
		}

		char* pt = strchr(lecode, '\n');
		if (pt == NULL)
			return NULL;
		*pt = 0;
		return pt;
	}

	int NbLignes(const char* lecode) {
		int lignes = 0;
		const char* pt = strchr(lecode, '\n');
		while (pt != NULL) {
			lignes++;
			lecode = pt + 1;
			pt = strchr(lecode, '\n');
		}
		return lignes;
	}

	bool runkifsession() {
		int i;
		if (localkifcode == NULL || encours)
			return true;
		encours = true;
		msgerreur = "";
		delete os;
		os = new ostringstream;
		kifGlobals->os = os;
		//kifGlobals->doubledisplay=true;
		kifGlobals->endofexecution = false;
		vector<KifDomain*> keepstack;
		int id = 0;

		int idname = KifNewId("exit");
		if (kifGlobals->kifBaseFunctions.find(idname) != kifGlobals->kifBaseFunctions.end())
			kifGlobals->kifBaseFunctions[idname]->function = &PreExitExtern;

		for (i = 0; i<kifGlobals->THR[id]->threadstacks.size(); i++)
			keepstack.push_back(kifGlobals->THR[id]->threadstacks[i]);
		kifGlobals->THR[id]->threadstacks.clear();

		bool toadd = localkifcode->kstart->adding;
		localkifcode->kstart->adding = false;

		kifGlobals->executionbreak = 0;

		localkifcode->kstart->Run(firstinstruction);

		localkifcode->kstart->adding = toadd;

		for (i = 0; i<keepstack.size(); i++)
			kifGlobals->THR[id]->threadstacks.push_back(keepstack[i]);

		kifGlobals->endofexecution = true;
		kifEndencours();

		kifGlobals->executionbreak = 2;

		if (Errorid(id) == true) {
			msgerreur = Errorstring();
			kifGlobals->THR[id]->cleanerror();
			return false;
		}

		return true;
	}

	long kiflignecourantedebug() {
		return mondebug.currentline;
	}

	void kifcleansession() {
		msgerreur = "";
		if (os != NULL) {
			delete os;
			os = new ostringstream;
			kifGlobals->os = os;
			kifGlobals->doubledisplay = true;
		}
	}

	char* displaykifsession() {
		conversion = "";
		if (msgerreur != "") {
			Trim(msgerreur);
			conversion += msgerreur;
			conversion += Endl;
		}
		if (os != NULL) {
			conversion += os->str();
			Trim(conversion);
			if (conversion != "")
				conversion += Endl;
			conversion = conversion_latin_to_utf8(conversion);
		}
		return STR(conversion);
	}

	char* lastkifdisplay() {
		if (lastmsg == 0)
			return NULL;
		if (lastmsg == 1)
			return theerrormsg();
		return displaykifsession();
	}

	char* listecodes() {
		codes = conversion_latin_to_utf8(USTR(codes));
		Trim(codes);
		codes = Endl + codes + Endl;
		return STR(codes);
	}

	const char* gethelp(const char* s) {
		static bool init = false;
		static map<string, string> dicos;
		string w;
		if (init == false) {
			int i = 0;
			string k;
			while (KIFTHEINFOSTRINGS[i][0] != 0) {
				k = KIFTHEINFOSTRINGS[i];
				w = KIFTHEINFOSTRINGS[i + 1];
				dicos[k] = w;
				i += 2;
			}
			init = true;
		}

		w = s;
		if (w[w.size() - 1] != '(')
			w += '(';
		if (dicos.find(w) == dicos.end())
			return NULL;
		return dicos[w].c_str();
	}

	const char* getkeywords() {
		codeindente = "";
		bool first = true;
		int i = 0;
		const unsigned char* wrd;

		while (KIFLTKSDEBUGGERTRINGS[i][0] != 0) {
			wrd = (const unsigned char*)KIFLTKSDEBUGGERTRINGS[i];
			if (wrd[0] >= 97 && wrd[0] != '\\' && wrd[0] != '/') {
				if (!first)
					codeindente += "|";
				first = false;
				codeindente += "\\b";
				codeindente += (char*)wrd;
				codeindente += "\\b";
			}
			i++;
		}
		codeindente += "|\\.[:word:]+\\(|\"|@|/|\\n|\\r|'";
		return codeindente.c_str();
	}

	const char* gettypes() {
		codeindente = "";
		bool first = true;
		if (kifGlobals == NULL)
			return codeindente.c_str();
		map<string, KifType>::iterator it;

		for (it = kifGlobals->kifTypeString.begin(); it != kifGlobals->kifTypeString.end(); it++) {
			if (!first)
				codeindente += "|";
			first = false;
			codeindente += "\\b";
			codeindente += it->first;
			codeindente += "\\b";
		}
		codeindente += "|";
		return codeindente.c_str();
	}

	char* convertir(char* res) {
		string conversion = conversion_latin_to_utf8(res);
		char* r = (char*)malloc(conversion.size() + 1);
		strcpy(r, STR(conversion));
		return r;
	}
}


