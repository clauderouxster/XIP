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
filename   : kifeditor.cxx
Date       : 09/09/2010
Purpose    : Debugger and editor for KiF
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
//#define KIF_DLL

#ifdef UNIX
#include <unistd.h>
#endif

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tabs.H>

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"
#include "kifmaps.h"
#include "kifautomaton.h"


static const char* kifdisplayln = KIFLTKSDEBUGGERTRINGS[0];
static const char* kifwhile = KIFLTKSDEBUGGERTRINGS[1];
static const char* kiffor = KIFLTKSDEBUGGERTRINGS[2];
static const char* kifswitch = KIFLTKSDEBUGGERTRINGS[3];
static const char* kifif = KIFLTKSDEBUGGERTRINGS[4];
static const char* kifelse = KIFLTKSDEBUGGERTRINGS[5];
static const char* kifelif = KIFLTKSDEBUGGERTRINGS[6];


#ifdef APPLE
#define FLCTRL FL_COMMAND
#define TESTCOMBI Fl::event_command
#define TAILLE_NORMALE FL_NORMAL_SIZE
#else
#define FLCTRL FL_CTRL
#define TESTCOMBI Fl::event_ctrl
#ifdef UNIX
#define TAILLE_NORMALE FL_NORMAL_SIZE-1
#else
#define TAILLE_NORMALE FL_NORMAL_SIZE
#endif
#endif

#ifdef C11
using std::map;
#endif

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif


//------------------------------------------------------------------------------------
int VirtualIndentation(string& codestr, const char* kifelse, const char* kifelif, const char* kifif);
void SetBlankSize(int sz);
int GetBlankSize();
//------------------------------------------------------------------------------------
Exported bool kifexplorexn(x_node* xn, string& var, string& type, int first);
static hmap<short, vector<string> > thevectorstrings;
static hmap<short, map<string, string> > thevectorinfos;
static hmap<string, short> kiftypes;
static hmap<short, short> equivalences;

static bool double_display = false;

class KifLocalEditorMain;
static KifLocalEditorMain* mainwindow;
static Fl_Double_Window* w_mainwindow;
static Fl_Group* maingroup;
static Fl_Tabs* tabwindows;

class KifLocalEditor;
static vector<KifLocalEditor*> editors;


static bool scomparestr(string e1, string e2) {
	if (e1 < e2)
		return(true);
	return(false);
}

void Updatesuggestion() {
	thevectorstrings.clear();
	thevectorinfos.clear();
	kiftypes.clear();
	equivalences.clear();


	hmap<short, hmap<short, bool> >::iterator it;
	for (it = kifGlobals->kifAllFunctions.begin(); it != kifGlobals->kifAllFunctions.end(); it++) {
		hmap<short, bool>& val = it->second;
		vector<string> methods;
		hmap<short, bool>::iterator itsub;
		for (itsub = val.begin(); itsub != val.end(); itsub++)
			methods.push_back(kifGlobals->KifIdString(itsub->first));

		sort(methods.begin(), methods.end(), scomparestr);
		thevectorstrings[it->first] = methods;
		thevectorinfos[it->first] = kifGlobals->kifAllInformation[it->first];
	}

	equivalences = kifGlobals->equivalences;
	map<string, KifType>::iterator itk;
	for (itk = kifGlobals->kifTypeString.begin(); itk != kifGlobals->kifTypeString.end(); itk++) {
		if (equivalences.find(itk->second) != equivalences.end())
			kiftypes[itk->first] = itk->second;
	}
}
//------------------------------------------------------------------------------------
//Implemented in FLTK...
void ResetGetEditor(KifCode* local);
void AddALoadingPath(string thelocalname);

const int EDITORMAXBUFFER = 250000;

static bool stopmainloop = false;
static bool waitabit = true;
void debug_callback(void*);
extern "C" {
	bool KifWindowMode();
	void FLTKLaunchWindowCall();
	void Fltkclean();
	void KifWarnDebugEditor(bool v);
}

string Removetrailing(string& res) {
	string chaine(res);
	int d, f;
	for (d = 0; d<chaine.size(); d++) {
		if ((uchar)chaine[d]>32)
			break;
	}

	for (f = chaine.size() - 1; f >= 0; f--) {
		if ((uchar)chaine[f] > 32)
			break;
	}
	int lg = f - d + 1;
	if (lg >= 1)
		chaine = chaine.substr(d, lg);
	else
		chaine = "";
	return chaine;
}

static bool comparestr(string e1, string e2) {
	if (e1 < e2)
		return(true);
	return(false);
}

class x_fltk_reading : public x_readstring {
public:

	x_fltk_reading(string& s) : x_readstring(s) {}

	virtual void loadtoken(bool keepspace = false) {
		unsigned char s, c;
		Initable();
		char ch[] = { 0, 0 };
		string token;
		stack.clear();
		stackln.clear();
		stacktype.clear();

		long locpos = Length();

		if (locpos != -1) {
			stack.reserve(locpos / 2);
			stackln.reserve(locpos / 2);
			stacktype.reserve(locpos / 2);
		}

		bool decimal = false;
		bool digit = false;
		bool exposant = false;
		long line = 0;
		char lastc = 0;
		s = get();
		locpos = tell();
		while (!eof()) {
			if (s == '/' && token == "") {
				s = get();
				if (s == '/') {
					//this is a comment
					token = "//";
					s = get();
					while (s != 10 && s != 13 && !eof()) {
						token += s;
						s = get();
					}

					stack.push_back(token);
					stackln.push_back(line);
					stacktype.push_back(9);
					token.clear();
					line++;
					lastc = s;
					s = get();
					locpos = tell();
					continue;
				}
				if (s == '@') {
					//this is a comment
					token = "/@";
					lastc = 0;
					while (1) {
						if (eof())
							break;
						s = get();
						if (s == '@') {
							token += s;
							s = get();
							token += s;
							if (s == '/')
								break;
						}
						else {
							if (s == 10 && lastc == 13)
								lastc = 0;
							else
							if (s == 10 || s == 13) {
								token += s;
								lastc = s;
								stack.push_back(token);
								stackln.push_back(line);
								stacktype.push_back(9);
								token.clear();
								line++;
							}
							else
								token += s;
						}
					}

					stack.push_back(token);
					stackln.push_back(line);
					stacktype.push_back(9);
					token.clear();
					s = get();
					locpos = tell();
					continue;
				}
				seek(locpos);
				s = '/';
			}

			c = s;
			if (token == "") {
				if (c == '@') {
					s = get();
					if (s == '"') {
						lastc = 0;
						token += s;
						c = get();
						while (!eof()) {
							if (c == 10 && lastc == 13)
								lastc = 0;
							else {
								if (c == 10 || c == 13) {
									lastc = c;
									line++;
								}
							}
							token += c;
							bool fd = false;
							if (c == '"')
								fd = true;
							c = get();
							if (fd && c == '@')
								break;
						}
						stack.push_back(token);
						stackln.push_back(line);
						stacktype.push_back(5);
						token.clear();
						s = get();
						locpos = tell();
						continue;
					}
					seek(locpos);
				}
				else {
					if (c == '"') {
						lastc = 0;
						token += c;
						c = get();
						while (c != '"' && !eof()) {
							if (c == 10 && lastc == 13)
								lastc = 0;
							else
							if (c == 10 || c == 13) {
								lastc = c;
								line++;
							}
							if (c == '\\') {
								token += c;
								c = get();
							}
							token += c;
							c = get();
						}
						token += '"';
						stack.push_back(token);
						stackln.push_back(line);
						stacktype.push_back(1);
						token.clear();
						s = get();
						locpos = tell();
						continue;
					}
					if (c == '\'') {
						token += c;
						c = get();
						while (c != '\'' && !eof()) {
							if (c == 10 && lastc == 13)
								lastc = 0;
							else
							if (c == 10 || c == 13) {
								lastc = c;
								line++;
							}
							token += c;
							c = get();
						}
						token += "'";
						stack.push_back(token);
						stackln.push_back(line);
						stacktype.push_back(2);
						token.clear();
						s = get();
						locpos = tell();
						continue;
					}
				}
			}
			if (strchr((char*)table, c)) {
				token += c;
				if (c == 'e' && decimal)
					exposant = true;
			}
			else {
				bool normaltoken = true;
				if (digit) {
					switch (c) {
					case 48:
					case 49:
					case 50:
					case 51:
					case 52:
					case 53:
					case 54:
					case 55:
					case 56:
					case 57:
						token += c;
						normaltoken = false;
						break;
					case '.':
						if (!decimal) {
							token += c;
							decimal = true;
							normaltoken = false;
						}
						break;
					case '-':
						if (exposant) {
							token += c;
							normaltoken = false;
						}
					}
				}
				else {
					if (c >= 48 && c <= 57) {
						normaltoken = false;
						if (token == "")
							digit = true;
						token += c;
					}
				}

				if (normaltoken) {
					if (token != "") {
						stack.push_back(token);
						if (digit)
							stacktype.push_back(3);
						else
							stacktype.push_back(4);
						stackln.push_back(line);
						token.clear();
						digit = false;
						exposant = false;
						decimal = false;
					}
					if (c > 32 || keepspace) {
						ch[0] = c;
						stack.push_back(ch);
						stackln.push_back(line);
						stacktype.push_back(0);
					}
					if (c == 10 && lastc == 13)
						lastc = 0;
					else
					if (c == 10 || c == 13) {
						lastc = c;
						line++;
					}
				}
			}
			s = get();
			locpos = tell();
		}

		if (token != "") {
			stack.push_back(token);
			if (digit)
				stacktype.push_back(3);
			else
				stacktype.push_back(4);
			stackln.push_back(line);
		}
	}
};


class x_xip_reading : public x_fltk_reading {
public:

	x_xip_reading(string& s) : x_fltk_reading(s) {}

	void loadtoken(bool keepspace = false) {
		unsigned char s, c;
		Initable();
		char ch[] = { 0, 0 };
		string token;
		stack.clear();
		stackln.clear();
		stacktype.clear();
		int lg = strlen((char*)table);
		table[lg - 1] = 0;

		long locpos = Length();

		if (locpos != -1) {
			stack.reserve(locpos / 2);
			stackln.reserve(locpos / 2);
			stacktype.reserve(locpos / 2);
		}

		bool decimal = false;
		bool digit = false;
		long line = 0;
		char lastc = 0;
		s = get();
		locpos = tell();
		while (!eof()) {
			if (s == '/' && token == "") {
				s = get();
				if (s == '/') {
					//this is a comment
					token = "//";
					s = get();
					token += s;
					while (s != 10 && s != 13 && !eof()) {
						s = get();
						token += s;
					}

					stack.push_back(token);
					stackln.push_back(line);
					stacktype.push_back(9);
					token.clear();
					line++;
					lastc = s;
					s = get();
					locpos = tell();
					continue;
				}

				token = "/";
				while (!eof() && s != '\\') {
					if (s == 10)
						line++;
					token += s;
					s = get();
				}
				token += "\\";
				stack.push_back(token);
				stackln.push_back(line);
				stacktype.push_back(9);
				token.clear();
				lastc = s;
				s = get();
				locpos = tell();
				continue;
			}

			c = s;
			if (c == '"' && token == "") {
				lastc = 0;
				token += c;
				c = get();
				while (c != '"' && !eof()) {
					if (c == 10 && lastc == 13)
						lastc = 0;
					else
					if (c == 10 || c == 13) {
						lastc = c;
						line++;
					}
					if (c == '\\') {
						token += c;
						c = get();
					}
					token += c;
					c = get();
				}
				token += '"';
				stack.push_back(token);
				stackln.push_back(line);
				stacktype.push_back(1);
				token.clear();
				s = get();
				locpos = tell();
				continue;
			}

			if (strchr((char*)table, c)) {
				token += c;
			}
			else {
				bool normaltoken = true;
				if (digit) {
					switch (c) {
					case 48:
					case 49:
					case 50:
					case 51:
					case 52:
					case 53:
					case 54:
					case 55:
					case 56:
					case 57:
						token += c;
						normaltoken = false;
						break;
					case '.':
						if (!decimal) {
							token += c;
							decimal = true;
							normaltoken = false;
						}
						break;
					}
				}
				else {
					if (c >= 48 && c <= 57) {
						normaltoken = false;
						if (token == "")
							digit = true;
						token += c;
					}
				}

				if (normaltoken) {
					if (token != "") {
						stack.push_back(token);
						if (digit)
							stacktype.push_back(3);
						else {
							if ((token[0] == '$' || token[0] == '#') && token.size() > 1 && token[1] >= 48 && token[1] <= 57)
								stacktype.push_back(6);
							else
								stacktype.push_back(4);
						}
						stackln.push_back(line);
						token.clear();
						digit = false;
						decimal = false;
					}
					if (c == '#' || c == '$') {
						token.clear();
						token += c;
					}
					else {
						if (c > 32 || keepspace) {
							ch[0] = c;
							stack.push_back(ch);
							stackln.push_back(line);
							stacktype.push_back(0);
						}
						if (c == 10 && lastc == 13)
							lastc = 0;
						else
						if (c == 10 || c == 13) {
							lastc = c;
							line++;
						}
					}
				}
			}
			s = get();
			locpos = tell();
		}

		if (token != "") {
			stack.push_back(token);
			if (digit)
				stacktype.push_back(3);
			else
				stacktype.push_back(4);
			stackln.push_back(line);
		}
	}
};


bool displaydebug(string vrs, string allvrs,string pile, string fname, long ln);

class FLTKLocalDebug :public KifDebugInfo {
public:
	string svariables;
    string allvariables;
	int laststacksize;
	string lastfunction;
	vector<string> pile;
	char** piles;
	bool donottest;
	bool enter;

	bool nextcodeline;
	bool theend;
	bool terminate;
	int strickthread;
	agnostring localline;
	int thelastline;

	bool shortname;


	map<string, map<long, bool> > breakpoints;
	vector<string> variableschoice;


	map<string, string> principales;
	map<string, string> fonctions;
	map<string, string> classes;

	void SelectionVariables(string& s) {
		variableschoice.clear();
		string sub;
		for (int i = 0; i < s.size(); i++) {
			if (s[i] != '\n')
				sub += s[i];
			else {
				variableschoice.push_back(sub);
				sub.clear();
			}
		}
	}

	int AddSelectionVariable(string& s) {
		variableschoice.push_back(s);
		return variableschoice.size() - 1;
	}

	void GetVariables(string& v) {
		v.clear();
		for (int i = 0; i < variableschoice.size(); i++) {
			v += variableschoice[i];
			v += Endl;
		}
	}

	void RemoveVariables(int m) {
		variableschoice.erase(variableschoice.begin() + m, variableschoice.end());
	}

	bool TestVariable(string s) {
		if (s == "kifsys")
			return false;
		if (donottest || variableschoice.size() == 0)
			return true;
		for (int i = 0; i < variableschoice.size(); i++)
		if (variableschoice[i] == s)
			return true;
		return false;
	}

	FLTKLocalDebug() : KifDebugInfo() {
		thelastline = -1;
		shortname = true;
		piles = NULL;
	}

	void cleanall() {
		thelastline = -1;
		kiftabledebugfilename.clear();
		strickthread = -100;
		laststacksize = 0;
		localline.clear();
		lastfunction.clear();
		ClearFilename();
		breakpoints.clear();
		nextcodeline = false;
		enter = false;
		theend = false;
		kifdebugging = false;
		kifoutoffunction = 0;
		kiffunctioncount = 1;
		terminate = false;
	}

	void setdebugmode(bool v) {
		kifdebugging = v;
		nextcodeline = v;
	}

	void listebreakpoint(string fidx, vector<long>& v) {
		if (breakpoints.find(fidx) != breakpoints.end()) {
			map<long, bool>::iterator it;
			for (it = breakpoints[fidx].begin(); it != breakpoints[fidx].end(); it++)
				v.push_back(it->first);
		}
	}

	bool isbreakpoint(string fidx, long l) {
		if (breakpoints.find(fidx) != breakpoints.end()) {
			if (breakpoints[fidx].find(l) != breakpoints[fidx].end())
				return true;
		}
		return false;
	}

	int addbreakpoint(string fidx, long l) {
		if (breakpoints.find(fidx) != breakpoints.end()) {
			if (breakpoints[fidx].find(l) != breakpoints[fidx].end()) {
				breakpoints[fidx].erase(l);
				if (breakpoints[fidx].size() == 0)
					breakpoints.erase(fidx);
				if (breakpoints.size() == 0)
					kifdebugging = false;
				return -2;
			}
		}
		kifdebugging = true;
		nextcodeline = false;
		breakpoints[fidx][l] = true;
		return 1;
	}

	bool KifDebugging(KifCode* kcode, KifElement* ke, KifDomain* dom, KifCallFunction* callfunc) {
		//In multi-threading, it is better to control to have only one thread at a time accessing that stuff...
		if (theend) {
			kifdebugging = false;
			return true;
		}

		int id = kifGlobals->GetThreadid();

		ThreadLock _lock(kifInDebug);
		if (strickthread >= -1 && strickthread != id)
			return true;

		if (terminate) {
			terminate = false;
			kcode->Terminate();
			return true;
		}

		long line = ke->Line();
		if (line == -1)
			line = currentline;

		int fileidx = ke->Fileindex();
		string d_fname;
		if (fileidx != -1 && fileidx < kiftabledebugfilename.size())
			d_fname = kiftabledebugfilename[fileidx];
		else
			d_fname = CurrentFilename();


		if (!nextcodeline) {
			thelastline = -1;
			if (breakpoints.find(d_fname) == breakpoints.end() || breakpoints[d_fname].find(line) == breakpoints[d_fname].end())
				return true;
		}

		if (kifinfunction == true) {
			if (enter == false) {
				thelastline = -1;
				kifoutoffunction = kiffunctioncount - 1;
				return true;
			}
		}

		if (thelastline != -1 && thelastline == line)
			return true;

		thelastline = line;
		currentline = line;
		enter = false;
		nextcodeline = false;
		currentcallfunc = callfunc;
		currentdom = dom;
		currentkifcode = kcode;

		donottest = true;
		LoadVariables(currentkifcode, currentdom, currentcallfunc);

		LoadStack(id, line);

		string lapile;
		for (int i = pile.size() - 1; i >= 0; i--)
			lapile += pile[i];

#ifdef UNIX
		//synchronisation problem on UNIX, when the debugmode is launched from the menu
		if (waitabit) {
			sleep(1);
			waitabit = false;
		}
#endif
        displaydebug(svariables, allvariables,lapile, d_fname, line);
		return true;
	}

	void LoadStack(int id, long lg) {
		pile.clear();
		vector<Stackelement*>& elements = kifGlobals->THR[id]->stackelements;

		char ch[1024];
		char fn[1024];
		string fonc;
		char* pt;
		string ptt("BASE");

		for (int i = 0; i < elements.size(); i++) {
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
				if (fonc != "") {
					sprintf_s(ch, 1024, "[%d] %-30s\t(%ld:%s) (%d)\n", i, STR(fonc), lg, pt, id);
					ptt = fonc;
				}
				else
					sprintf_s(ch, 1024, "[%d] %-30s\t(%ld:%s) (%d)\n", i, STR(ptt), lg, pt, id);
			}
			else {
				if (fonc != "") {
					sprintf_s(ch, 1024, "[%d] %-30s\t(%ld) (%d)\n", i, STR(fonc), lg, id);
					ptt = fonc;
				}
				else
					sprintf_s(ch, 1024, "[%d] %-30s\t(%ld) (%d)\n", i, STR(ptt), lg, id);
			}
			pile.push_back(ch);
		}
	}

	void ReLoadVariables() {
		donottest = false;
		LoadVariables(currentkifcode, currentdom, currentcallfunc);
	}

	void SearchStack(int l) {
		int idx = pile.size() - l - 1;
		if (idx < 0)
			return;
		int id = kifGlobals->GetThreadid();
		KifDomain* dom = NULL;
		KifDomain* func = NULL;
		KifCode* kcode = currentkifcode;
		int idstack = idx;
		if (id == -1)
			idstack--;
		if (idstack >= 0) {
			ThreadLock _lock(kifStack);
			dom = kifGlobals->THR[id]->threadstacks[idstack * 2];
			func = kifGlobals->THR[id]->threadstacks[idstack * 2 + 1];
		}
		if (dom != NULL)
			kcode = dom->kifcode;
		LoadVariables(kcode, dom, func);
		string fname = Filename(kifGlobals->THR[id]->stackelements[idx]->file);
		long lg = kifGlobals->THR[id]->stackelements[idx]->line;
	}

	void Splitlocalline(map<string, string>& variables_in_line, map<string, bool>& bvariables_in_line) {
		vector<string> local_variables;
		localline.tokenize(local_variables, 0);
		int j = 0, ch = 0;
		uchar c;

		for (int i = 0; i<local_variables.size(); i++) {
			if (local_variables.size() >= 1)
				c = (unsigned char)local_variables[i][0];
			if (c == 95 || c>128 || c_is_alpha(USTR(local_variables[i]), j)) {
				variables_in_line[local_variables[i]].clear();
				bvariables_in_line[local_variables[i]] = false;
			}
		}
	}

	void LoadVariables(KifCode* ck, KifDomain* cd, KifDomain* ccf) {
		principales.clear();
		classes.clear();
		fonctions.clear();
		map<string, string> variables_in_line;
		map<string, bool> bvariables_in_line;
		Splitlocalline(variables_in_line, bvariables_in_line);

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
			if (shortname && localvalue.size() > 201) {
				localvalue = localvalue.substr(0, 200);
				localvalue += "...";
			}
			principales[varname] = localvalue;
			if (variables_in_line.find(varname) != variables_in_line.end()) {
				variables_in_line[varname] = localvalue;
				bvariables_in_line[varname] = true;
			}
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
				if (shortname && localvalue.size() > 201) {
					localvalue = localvalue.substr(0, 200);
					localvalue += "...";
				}
				if (classes.find(varname) == classes.end())
					classes[varname] = localvalue;
				if (variables_in_line.find(varname) != variables_in_line.end()) {
					variables_in_line[varname] = localvalue;
					bvariables_in_line[varname] = true;
				}
			}
		}
		if (ccf != NULL) {
			for (it = ccf->declarations.begin(); it != ccf->declarations.end(); it++) {
				varname = KifIdString(it->first);
				if (!TestVariable(varname))
					continue;
				localvalue = it->second->BaseString();
				if (shortname && localvalue.size() > 201) {
					localvalue = localvalue.substr(0, 200);
					localvalue += "...";
				}
				fonctions[varname] = localvalue;
				if (variables_in_line.find(varname) != variables_in_line.end()) {
					variables_in_line[varname] = localvalue;
					bvariables_in_line[varname] = true;
				}
			}
		}

		svariables.clear();
        allvariables.clear();
		map<string, string>::iterator itx;

		if (variables_in_line.size() != 0) {
			svariables = KIFLTKSDEBUGGERTRINGS[7];
			for (itx = variables_in_line.begin(); itx != variables_in_line.end(); itx++) {
				if (bvariables_in_line[itx->first] == false)
					continue;
				svariables += itx->first;
				svariables += " = ";
				svariables += itx->second;
				svariables += "\n";
			}
		}

		if (principales.size() != 0) {
			allvariables += KIFLTKSDEBUGGERTRINGS[8];
			for (itx = principales.begin(); itx != principales.end(); itx++) {
				if (itx->first.find("XIP_") != -1)
					continue;
				allvariables += itx->first;
				allvariables += " = ";
				allvariables += itx->second;
				allvariables += "\n";
			}
		}

		if (classes.size() != 0) {
			allvariables += "\n";
			allvariables += KIFLTKSDEBUGGERTRINGS[9];
			allvariables += "\n";
			for (itx = classes.begin(); itx != classes.end(); itx++) {
				allvariables += itx->first;
				allvariables += " = ";
				allvariables += itx->second;
				allvariables += "\n";
			}
		}

		if (fonctions.size() != 0) {
			allvariables += "\n";
			allvariables += KIFLTKSDEBUGGERTRINGS[10];
			allvariables += "\n";
			for (itx = fonctions.begin(); itx != fonctions.end(); itx++) {
				allvariables += itx->first;
				allvariables += " = ";
				allvariables += itx->second;
				allvariables += "\n";
			}
		}
	}

}
localdebugger;

Exporting void KifTerminateAllLibraries(KifCode* kifcodemain);


static void termination(KifCode* kcode) {
	cerr << KIFLTKSDEBUGGERTRINGS[11] << endl;
	localdebugger.terminate = true;
	if (kcode != NULL)
		kcode->Terminate();
	else {
		if (kifGlobals != NULL) {
			ThreadLock _lock(kifPrepareError);
			ThreadLock _lockxThreadLock(kifError);
			kifGlobals->executionbreak = 1;
		}
	}
}

static void nextcodeline(bool v) {
	localdebugger.nextcodeline = v;
}

static void getinto(bool v) {
	localdebugger.nextcodeline = v;
	localdebugger.enter = v;
}

static void setdebugmode(bool v) {
	localdebugger.setdebugmode(v);
	localdebugger.strickthread = -99;
}

bool isdebug() {
	return localdebugger.kifdebugging;
}

static void uptotheend() {
	localdebugger.theend = true;
}

static long debuggercurrentline() {
	return localdebugger.currentline;
}

int addbreakpoint(string& filename, long l) {
	return localdebugger.addbreakpoint(filename, l);
}

bool isbreakpoint(const char* c, long l) {
	return localdebugger.isbreakpoint(c, l);
}

static void breakpointlist(const char* c, vector<long>& v) {
	return localdebugger.listebreakpoint(c, v);
}

static void exitfunction() {
	localdebugger.kifoutoffunction = localdebugger.kiffunctioncount - 1;
	localdebugger.nextcodeline = true;
}

static void cleanbreakpoints() {
	localdebugger.breakpoints.clear();
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
class KifLocalEditor;
static bool fltkcompileandrun(KifLocalEditor* e, KifCode* local, string codeinit, ostringstream* os, bool display, bool asdebug, bool threadmode);
//---------------------------------------------------------------------------------------------------------
static bool comparewith(string& s, string with) {
	if (s.size() <= with.size())
		return false;
	for (int i = 0; i < with.size(); i++)
	if (s[i] != with[i])
		return false;
	return true;
}

static bool isinstruction(string& s) {
	char c = s[s.size() - 1];
	if (c == ')') {
		if (comparewith(s, kifwhile))
			return true;
		if (comparewith(s, kiffor))
			return true;
		if (comparewith(s, kifswitch))
			return true;
		if (comparewith(s, kifif))
			return true;
	}
	return false;
}

static void call_choice(Fl_Widget* w, void* x);
static void see_choice(Fl_Widget* w, void* x);

static void local_filebrowser_callback(Fl_File_Chooser *fc, void* data);
static void local_filebrowser_callback_open(Fl_File_Chooser *fc, void* data);

static Fl_Text_Display::Style_Table_Entry localstyletable[] = {     // Style table
	{ FL_BLACK, FL_COURIER, TAILLE_NORMALE }, // A - Plain
	{ FL_DARK_RED, FL_COURIER, TAILLE_NORMALE }, // B - numbers
	{ FL_DARK_GREEN, FL_COURIER, TAILLE_NORMALE }, // C - Comments
	{ FL_RED, FL_COURIER, TAILLE_NORMALE }, // D - Strings (double)
	{ FL_RED, FL_COURIER, TAILLE_NORMALE }, // E - Strings (simple)
	{ FL_BLACK, FL_COURIER, TAILLE_NORMALE }, // F - Keywords
	{ FL_BLUE, FL_COURIER, TAILLE_NORMALE }, // G - Keywords function, thread, if, switch,else,elif,try, catch
	{ FL_BLUE, FL_COURIER, TAILLE_NORMALE }, // H - Keywords preceded with a '.'
	{ FL_DARK_RED, FL_COURIER_ITALIC, TAILLE_NORMALE }, // I - Breakpoints
	{ FL_DARK1, FL_COURIER, TAILLE_NORMALE }, // J - Long comments
	{ FL_DARK_MAGENTA, FL_COURIER, TAILLE_NORMALE }, // K - XIP variables
	{ FL_DARK_BLUE, FL_COURIER, TAILLE_NORMALE }, // L - XIP features
	{ FL_DARK_RED, FL_COURIER, TAILLE_NORMALE } // M - console
};

static map<string, bool> localkeywords;

class e_modified {
public:

	string buff;
	int pos;
	int nb;
	char inserted;

	e_modified(int p, char i, int n, string& s) : buff(s) {
		pos = p;
		nb = n;
		inserted = i;
	}

	bool add(int p, int n, string& s) {
#ifdef WIN32
		unsigned char c = s.back();
#else
		unsigned char c = s[s.size() - 1];
#endif
		if (!inserted || c<'0' || (c>'9' && c<'A') || (c>'Z' && c < 'a'))
			return false;
		if (p == pos + nb) {
			nb += n;
			buff += s;
			return true;
		}
		return false;
	}
};


class KifEditorTextBuf : public Fl_Text_Buffer {
public:
	char* wbuffer;
	int internalsize;
	bool donotprocess;
	list<e_modified> doing;
	list<e_modified> undoing;


	KifEditorTextBuf() : Fl_Text_Buffer(EDITORMAXBUFFER) {
		internalsize = 1024;
		wbuffer = new char[1024];
		donotprocess = false;
		//canUndo(1);
	}

	~KifEditorTextBuf() {
		delete[] wbuffer;
	}

	void addinsertion(int p, int n, string s, int nDeleted) {
		if (doing.size() && doing.back().add(p, n, s))
			return;

		e_modified e(p, 1, n, s);
		if (nDeleted)
			e.inserted = 2;
		if (doing.size() >= 1000)
			doing.pop_front();
		doing.push_back(e);
	}

	void adddeletion(int p, int n, string s) {
		e_modified e(p, 0, n, s);
		if (doing.size() >= 1000)
			doing.pop_front();
		doing.push_back(e);
	}

	void insertlocaltext(int pos, string& a) {
		a = s_replacestrings(a, "\r\n", "\n");
		insert(pos, STR(a));
	}

	void clear() {
		donotprocess = true;
		text("");
	}

	void setstyle(int start, int end, uchar style) {
		int l = end - start;
		if (l >= internalsize) {
			delete[] wbuffer;
			internalsize = l + 100;
			wbuffer = new char[internalsize];
		}
		memset(wbuffer, style, l);
		wbuffer[l] = 0;
		replace(start, end, wbuffer);
	}

	//Extract a word from position s
	//s should be the last character of the word
	int wordfromend(int e) {
		int c = char_at(--e);
		while (e>=0 && c>32) {
			c = char_at(--e);
		}
		return e+1;
	}

	string subrange(int s, int e) {
		if (s > length() || e > length() || (e - s) <= 0)
			return "";
		string code;
		char* str = text_range(s, e);
		code = str;
		free(str);
		return code;
	}
};


struct Local_Style_Arg {
	KifLocalEditor* e;
	KifEditorTextBuf* s;
};

static void local_style_update(int pos,          // I - Position of update
	int        nInserted,    // I - Number of inserted chars
	int        nDeleted,     // I - Number of deleted chars
	int        nRestyled,    // I - Number of restyled chars
	const char *deletedText, // I - Text that was deleted
	void       *stylarg);

static void localdisplay(string s, void* o);
void KifSetKifDebuggingFunction(KifDebugInfo* debug);

static void local_number_callback(Fl_Widget *w, void *data);

static void local_close_callback(Fl_Widget *w, void *data);
static void find_next_button_callback(Fl_Widget *w, void *data);
static void find_cancel_button_callback(Fl_Widget *w, void *data);


static void Menu_CB(Fl_Widget*w, void*data);


class KifFindEditor : public Fl_Text_Editor {
public:
	KifEditorTextBuf* textbuf;

	KifFindEditor(int x, int y, int h, int w, char* f) : Fl_Text_Editor(x, y, h, w, f) {
		textbuf = new KifEditorTextBuf();
		buffer(textbuf);
	}

	void value(string& s) {
		textbuf->text(STR(s));
		textbuf->select(0, textbuf->length());
	}

	string value() {
		char* txt = textbuf->text();
		string s(txt);
		free(txt);
		return s;
	}
};




class KifLocalFind {
public:

	Fl_Window          find_dlg;
	Fl_Input           find_find;
	Fl_Return_Button   find_next;
	Fl_Button          find_cancel;
	Fl_Check_Button	   find_case;
	KifLocalEditor* editor;
	int start, end;
	bool found;


	KifLocalFind(KifLocalEditor* e, string& tofind) :
		find_dlg(300, 105, 450, 120, KIFLTKSDEBUGGERTRINGS[12]),
		find_find(70, 10, 350, 25, KIFLTKSDEBUGGERTRINGS[13]),
		find_next(20, 70, 130, 25, KIFLTKSDEBUGGERTRINGS[14]),
		find_cancel(290, 70, 130, 25, KIFLTKSDEBUGGERTRINGS[15]),
		find_case(290, 40, 60, 25, KIFLTKSDEBUGGERTRINGS[16])	{

		find_dlg.set_modal();
		find_next.callback(find_next_button_callback, (void*)this);
		find_cancel.callback(find_cancel_button_callback, (void*)this);
		if (tofind.size() > 0) {
			find_find.value(STR(tofind));
			find_find.position(0, tofind.size());
		}
		find_case.value(false);
		editor = e;
		find_dlg.show();
		start = 0;
		end = 0;
		found = true;
	}
};

static void replace_all_button_callback(Fl_Widget *w, void *data);
static void replace_next_button_callback(Fl_Widget *w, void *data);
static void replace_cancel_button_callback(Fl_Widget *w, void *data);
static void close_choice(Fl_Widget *w, void *data);

class KifLocalReplace {
public:

	Fl_Window           replace_dlg;
	Fl_Input            replace_find;
	Fl_Input			replace_with;
	Fl_Button			replace_all;
	Fl_Return_Button	replace_next;
	Fl_Button          replace_cancel;
	Fl_Check_Button	   replace_case;
	KifLocalEditor* editor;
	int start, end;
	bool found;


	KifLocalReplace(KifLocalEditor* e, string& tofind, string& toreplace) :
		replace_dlg(300, 105, 600, 130, KIFLTKSDEBUGGERTRINGS[17]),
		replace_find(80, 10, 500, 25, KIFLTKSDEBUGGERTRINGS[18]),
		replace_with(80, 40, 500, 25, KIFLTKSDEBUGGERTRINGS[19]),

		replace_all(10, 90, 170, 25, KIFLTKSDEBUGGERTRINGS[20]),
		replace_next(200, 90, 170, 25, KIFLTKSDEBUGGERTRINGS[21]),
		replace_cancel(390, 90, 100, 25, KIFLTKSDEBUGGERTRINGS[15]),
		replace_case(520, 90, 100, 25, KIFLTKSDEBUGGERTRINGS[16])	{

		replace_dlg.set_modal();
		replace_all.callback(replace_all_button_callback, (void*)this);
		replace_next.callback(replace_next_button_callback, (void*)this);
		replace_cancel.callback(replace_cancel_button_callback, (void*)this);
		if (tofind.size() > 0) {
			replace_find.value(STR(tofind));
			replace_find.position(0, tofind.size());
		}
		replace_with.value(STR(toreplace));
		replace_case.value(false);
		editor = e;
		replace_dlg.show();
		start = 0;
		end = 0;
		found = false;
	}
};

class KifSuggest {
public:
	Fl_Window localwnd;
	Fl_Choice wchoice;
	Fl_Menu_Item* flm;
	KifLocalEditor* edt;
	int nb;

	KifSuggest(KifLocalEditor* e, string& code, int x, int y, int, int, bool);
	~KifSuggest();
};

class KifChoiceItem {
public:
	KifSuggest* wnd;
	KifLocalEditor* editor;
	Fl_Menu_Item* fli;
	int pos;
	int idx;

	KifChoiceItem(KifLocalEditor* e, int i, int p, Fl_Menu_Item* f, KifSuggest* w) {
		editor = e;
		idx = i + 1;
		fli = f;
		wnd = w;
		pos = p;
	}
};


class KifLocalEditor : public Fl_Text_Editor {
public:
	KifEditorTextBuf* textbuf;
	string filename;
    string tabname;
	KifEditorTextBuf* stylebuf;
	Local_Style_Arg stylearg;
	KifLocalEditor* main;
	Fl_Menu_Bar* menubar;
	Fl_Menu_Item menuitems[3];
	string searchstring;
	string searchreplace;
	KifLocalFind* findwindow;
	KifLocalReplace* replacewindow;
	char xipyfile;
	bool utf8;
	int rmain, rthread, rdebug;
	int idwindow;

	int idwnd;
	string code;
	Fl_Input o_linenumber;
	Fl_Check_Button o_utf8;
	Fl_Check_Button o_checkvariable;
    Fl_Group* w;

	char lastop;
	bool issaved;
	bool lastcasesensitive;
	int nbelements;	
	vector<KifChoiceItem*> choicemenuitems;
	string str_choicemenuitems;

	hmap<string, string> declared_variables;

	void Testtypefile() {
		xipyfile = 0;
		if (filename != "" && filename.size() > 4) {
			if (filename.substr(filename.size() - 4, 4) == ".xip")
				xipyfile = 1;
			else
			if (filename.substr(filename.size() - 3, 3) == ".py")
				xipyfile = 2;
		}
        string sep;
#ifdef WIN32
        sep='\\';
#else
        sep='/';
#endif
        int ps=filename.rfind(sep,filename.size()-1);
        if (ps!=string::npos)
            tabname=filename.substr(ps+1,filename.size()-ps-1);
        else
            tabname=filename;
	}

	KifLocalEditor(KifLocalEditor* m, int x, int y, int h, int w, char* f) : Fl_Text_Editor(x, y, h, w, ""), o_linenumber(930, 35, 55, 20, ""), o_utf8(850, 35, 45, 20, "UTF8"), o_checkvariable(750, 35, 45, 20, KIFLTKSDEBUGGERTRINGS[22]) {
		o_linenumber.callback(local_number_callback, this);
		o_checkvariable.value(false);
		utf8 = true;
		idwindow = 0;
		o_utf8.value(true);
		lastcasesensitive = false;
		issaved = true;
		idwnd = 0;
		lastop = 0;
		menubar = NULL;
		filename = f;
		main = m;
		textbuf = new KifEditorTextBuf();
		stylebuf = new KifEditorTextBuf();
		buffer(textbuf);
		rmain = 0; rthread = 0; rdebug = 0;
		localstyletable[4].color = fl_rgb_color(130, 130, 240);
		localstyletable[7].color = fl_rgb_color(160, 100, 10);
		localstyletable[9].color = fl_rgb_color(135, 135, 135);
		localstyletable[11].color = fl_rgb_color(20, 30, 180);
		nbelements = sizeof(localstyletable) / sizeof(localstyletable[0]);
		highlight_data(stylebuf, localstyletable, nbelements, 'A', NULL, 0);
		stylearg.s = stylebuf;
		stylearg.e = this;
		textbuf->add_modify_callback(local_style_update, &stylearg);
		Testtypefile();
        if (tabname=="")
            tabname=KIFLTKSDEBUGGERTRINGS[23];
	}

	void Loadfromfile(string pathname) {
		ifstream load(STR(pathname), modeOuverture);
		if (load.fail()) {
			code = KIFLTKSDEBUGGERTRINGS[24];
			code += pathname;
			Display(code, -1);
			code.clear();
			return;
		}
		code.clear();
		string s;
		bool first = true;
		while (!load.eof()) {
			getline(load, s);
			if (first) {
				//we check the utf8 signature
				if (s.size() >= 3 && (uchar)(s[0]) == 239 && (uchar)(s[1]) == 187 && (uchar)(s[2]) == 191)
					s = s.substr(3, s.size() - 3);
				first = false;
			}
			code += s;
			code += '\n';
		}
		Setvalue(code);
		issaved = true;
	}

	bool Tosave() {
		return issaved;
	}

	virtual KifCode* Kifcode() {
		return main->Kifcode();
	}

	virtual KifCode* Kifcodemain() {
		return main->Kifcodemain();
	}


	virtual KifCode* Newkifcode() {
		return main->Newkifcode();
	}

	virtual int currentline(int pos = -1) {
		int linenum = -1;
		if (pos == -1)
			pos = insert_position();
		position_to_line(pos, &linenum);
		linenum += get_absolute_top_line_number();
		return linenum;
	}

	void Updateline() {
		int linenum = currentline();
		char ch[20];
		sprintf_s(ch, 20, "%d", linenum);
		o_linenumber.value(ch);
	}

	void Line(int pos) {
		int linenum = currentline();
		char ch[20];
		sprintf_s(ch, 20, "%d", linenum);
		o_linenumber.value(ch);
	}

	void Selectline(int line, bool sel) {
		int start = 0;
		int end = 0;
		while (line > 1) {
			end = line_end(start, 1);
			start = end + 1;
			line--;
		}
		if (sel == false) {
			insert_position(start);
			Updateline();
		}
		else {
			end = line_end(start, 1);
			insert_position(start);
			textbuf->select(start, end);
		}
		show_insert_position();
	}

	void Getline(int line, agnostring& localline) {
		int start = 0;
		int end = 0;
		while (line > 1) {
			end = line_end(start, 1);
			start = end + 1;
			line--;
		}
		end = line_end(start, 1);
		insert_position(start);
		textbuf->select(start, end);
		localline = textbuf->subrange(start, end);
		show_insert_position();
	}

	void Gotoline() {
		const char* val = fl_input(KIFLTKSDEBUGGERTRINGS[25], "");
		if (val != NULL && strlen(val) > 0) {
			int line = atoi(val);
			Selectline(line, false);
		}
	}

	void SetBlanks() {
		int sz = GetBlankSize();
		char ch[10];
		sprintf_s(ch, 10, "%d", sz);
		const char* val = fl_input(KIFLTKSDEBUGGERTRINGS[26], ch);
		if (val != NULL && strlen(val) > 0) {
			sz = atoi(val);
			SetBlankSize(sz);
		}
	}

	bool Find() {
		const char* val;
		lastop = 0;
		val = (const char*)searchstring.c_str();
		if (val != NULL && strlen(val) > 0) {
			lastop = 1;
			int start = insert_position();
			int end = start;
			if (textbuf->search_forward(start, val, &end, lastcasesensitive) == 1) {
				textbuf->select(end, end + searchstring.size());
				insert_position(end + searchstring.size());
				show_insert_position();
				Updateline();
				return true;
			}

			if (textbuf->search_forward(0, val, &end, lastcasesensitive) == 1) {
				textbuf->select(end, end + searchstring.size());
				insert_position(end + searchstring.size());
				show_insert_position();
				Updateline();
				return true;
			}
		}
		return false;
	}

	bool Findfirst(int& start, int& end, bool casesensitive) {
		if (searchstring.size() == 0)
			return false;
		lastcasesensitive = casesensitive;
		const char* tofind = (const char*)searchstring.c_str();
		start = insert_position();
		end = start;
		int l = searchstring.size();

		if (textbuf->search_forward(start, tofind, &end, casesensitive) == 1) {
			textbuf->select(end, end + l);
			start = end;
			end += l;
			insert_position(end);
			show_insert_position();
			Updateline();
			return true;
		}

		if (textbuf->search_forward(0, tofind, &end, casesensitive) == 1) {
			textbuf->select(end, end + l);
			start = end;
			end += l;
			insert_position(end);
			show_insert_position();
			Updateline();
			return true;
		}
		return false;
	}

	void Replaceone(int start, int end, bool casesensitive) {
		lastop = 0;
		if (searchstring.size() == 0)
			return;
		lastcasesensitive = casesensitive;
		const char* tofind = (const char*)searchstring.c_str();
		const char* toreplace = (const char*)searchreplace.c_str();
		int l = searchstring.size();
		int lrep = searchreplace.size();
		lastop = 2;
		if (textbuf->search_forward(start, tofind, &end, casesensitive) == 1) {
			textbuf->replace(end, end + l, toreplace);
			insert_position(end + lrep);
			show_insert_position();
		}
	}

	void Replaceall(bool casesensitive) {
		lastop = 0;
		if (searchstring.size() == 0)
			return;
		lastcasesensitive = casesensitive;
		const char* tofind = (const char*)searchstring.c_str();
		const char* toreplace = (const char*)searchreplace.c_str();
		int l = searchstring.size();
		int lrep = searchreplace.size();
		int start = 0;
		int end = 0;
		while (textbuf->search_forward(start, tofind, &end, casesensitive) == 1) {
			start = end + l;
			textbuf->replace(end, start, toreplace);
			start = end;
		}
	}

	void FindWindow() {
		findwindow = new KifLocalFind(this, searchstring);
	}

	void ReplaceWindow() {
		replacewindow = new KifLocalReplace(this, searchstring, searchreplace);
	}

	virtual void ExecuteKiF(KifLocalEditor* e, string thecode, int start, bool debugmode, bool threadmode) {}

	virtual string Value() {
		char* str = textbuf->text();
		string code = str;
		free(str);
		return code;
	}

	virtual void Display(string a, int end) {
		main->Display(a, end);
	}

	virtual void Findbreakpoints(map<string, vector<int> >& breaks) {
		if (filename == "")
			return;

		int mx = stylebuf->length();
		for (int i = 0; i < mx; i++) {
			if (stylebuf->byte_at(i) == 'I') {
				breaks[filename].push_back(i);
				i = line_end(i, 1);
			}
		}
	}

	void Savecontent() {
		string c = Value();
		utf8 = o_utf8.value();
		string s;
		if (!utf8) {
			s = conversion_utf8_to_latin(USTR(c));
			c = s;
		}

		ofstream save(STR(filename));
		save << c;
		save.close();

        SyntaxColor(0, 0);
		issaved = true;
	}

	short FindDeclarations(int end) {
		if (xipyfile == 1)
			return -1;

		string localcode = textbuf->subrange(0, end);

		x_readstring xr(localcode);
		xr.loadtoken();
		if (xr.stack.size() == 0 || xr.stacktype.back() != 4)
			return -1;

		string var = xr.stack.back();
		xr.stack.pop_back();
		xr.stackln.pop_back();
		xr.stacktype.pop_back();

		//D'abord on compte les '{}' pour savoir combien il en faut pour avoir une analyse complete...

		char fnc;		
		int j;
		int sz = xr.stack.size();
		vector<string> vars;
		int declaration = 0;
		for (j = 0; j < sz; j++) {
			if (xr.stacktype[j]==4) {
				if (xr.stack[j] == var) {
					int i = j - 1;
					if (i >= 0) {
						if (xr.stack[i][0] == ',') {
							i = j - 2;
							bool stop = false;
							while (i >= 0 && kiftypes.find(xr.stack[i]) == kiftypes.end()) {
								if (xr.stack[i][0] == ';') {
									stop = true;
									break;
								}
								i--;
							}
							if (i < 0 || stop)
								continue;
						}
						if (kiftypes.find(xr.stack[i]) != kiftypes.end()) {
							if (declaration)
								vars.push_back("#");
							vars.push_back(xr.stack[i]);
						}
					}
				}
				continue;
			}

			if (xr.stacktype[j] == 0) {
				fnc = xr.stack[j][0];
				switch (fnc) {
				case '(':
					declaration++;
					break;
				case ')':
					declaration--;
					break;
				case '{':
					if (vars.size() >= 2 && vars[vars.size() - 2] == "#") {
						vars[vars.size() - 2] = "{";
						break;
					}
					vars.push_back("{");
					break;
				case '}':
					while (vars.back() != "{")
						vars.pop_back();
					vars.pop_back();
				}
			}
		}

		if (vars.size()) {
			while (vars.size() && vars.back() == "{")
				vars.pop_back();			
			if (vars.size())
				return kiftypes[vars.back()];
		}

		return -1;
	}

	void SyntaxColor(int start, int max) {
		static char table[] = { '[', '{', '}', '#', '$' };
		static char table2[] = { ':', '=', ']' };
		int rastart, raend;
		string localcode;

		map<string, vector<int> > thebreaks;
		if (start == 0 && max == 0)  {
			Findbreakpoints(thebreaks);
			localcode = Value();
			rastart = 0;
			raend = stylebuf->length();
			stylebuf->setstyle(0, raend, 'A');
		}
		else {
			rastart = 0;
			raend = max;
			localcode = textbuf->subrange(start, max);
			stylebuf->setstyle(start, max, 'A');
		}

		x_fltk_reading* xr;
		if (xipyfile == 1)
			xr = new x_xip_reading(localcode);
		else
			xr = new x_fltk_reading(localcode);

		xr->loadtoken();
		int end = 0, l;
		char* v;
		int i, r;
		char nextcar;
		char nextisopen;

		for (i = 0; i<xr->stack.size(); i++) {
			l = xr->stack[i].size();
			v = (char*)xr->stack[i].c_str();
			nextisopen = 'F';
			if (xr->stacktype[i] == 4) {
				if (i>0 && xr->stack[i - 1] == ".")
					nextisopen = true;
				if (i + 1 < xr->stack.size()) {
					nextcar = xr->stack[i + 1][0];
					if (nextcar == '(')
						nextisopen = 'H';
					else
					if (xipyfile == 1) {
						if (strchr(table, nextcar))
							nextisopen = 'H';
						if (strchr(table2, nextcar))
							nextisopen = 'L';
					}
				}
			}
			
			r = textbuf->search_forward(start, (char*)v, &end, 0);
			start = end + l;
			if (r == 0)
				continue;
			switch (xr->stacktype[i]) {
			case 1:
				stylebuf->setstyle(end, start, 'D');
				break;
			case 2:
				stylebuf->setstyle(end, start, 'E');
				break;
			case 3:
				if (textbuf->byte_at(start) == '>')
					stylebuf->setstyle(end, start + 1, 'B');
				else
					stylebuf->setstyle(end, start, 'B');
				break;
			case 4:
				if (localkeywords.find(xr->stack[i]) != localkeywords.end())
					stylebuf->setstyle(end, start, 'G');
				else
					stylebuf->setstyle(end, start, nextisopen);
				break;
			case 5:
				stylebuf->setstyle(end, start, 'J');
				break;
			case 6:
				stylebuf->setstyle(end, start, 'K');
				break;
			case 9:
				stylebuf->setstyle(end, start, 'C');
				break;
			}
		}

		for (i = 0; i < thebreaks[filename].size(); i++) {
			start = thebreaks[filename][i];
			end = line_end(start, 1);
			stylebuf->setstyle(start, end, 'I');
		}
		redisplay_range(rastart, raend);
		delete xr;
	}

	void Indente() {
		if (xipyfile)
			return;
		string c = Value();
		code.clear();
		int pos = insert_position();
		IndentCode(c, code, 0, kifelse, kifelif, kifif);
		e_modified e(0, 3, c.size(), c);
		textbuf->doing.push_back(e);
		textbuf->text(code.c_str());
		textbuf->doing.pop_back();
		insert_position(pos);
		show_insert_position();
		Updateline();
		SyntaxColor(0, 0);
	}

	virtual int Start() {
		return -1;
	}

	virtual void Setvalue(string& c) {
		if (c == "")
			utf8 = true;
		else {
			c = s_replacestrings(c, "\r\n", "\n");
			utf8 = s_is_utf8(USTR(c), c.size());
			string s;
			if (!utf8) {
				s_latin_to_utf8(s, USTR(c));
				c = s;
			}
			textbuf->text(c.c_str());
		}

		o_utf8.value(utf8);
		SyntaxColor(0, 0);
	}

	void getcodewithnospace(int start, int end, string& c) {
		start = line_start(start);
		while (start > 0) {
			if ((uchar)textbuf->byte_at(start) > 32)
				break;
			start = line_start(start - 1);
		}

		c = textbuf->subrange(start, end);
	}

	int justwhite(string& lastline) {
		int compte = 0;
		for (int i = 0; i < lastline.size() - 1; i++) {
			if (lastline[i] != 32)
				return compte;
			compte++;
		}
		return compte;
	}

	//-------------------------------------------------
	virtual int key_callback_up(int e) {
		int key = Fl::event_key();
		int ekey = Fl::event_state();
		if (key == 65289 && (ekey & FL_CTRL) == FL_CTRL) { //ctrl+tab
			int idx = idwnd + 1;
			if (idx >= editors.size())
				idx = 0;
			tabwindows->value(editors[idx]->w);
			return 1;
		}

		if (xipyfile != 2) {
			const char* tkey = Fl::event_text();
			if (tkey[0] == '}') {
				int start = insert_position();
				start = line_start(start);
				int end = line_end(start, 1);
				code = textbuf->subrange(start, end);
				if (Removetrailing(code) == "}") {
					int startcolor = line_start(start - 1);
					int endcolor = line_end(startcolor, 1);
					getcodewithnospace(startcolor, endcolor, code);
					code += "\n";
					int blanc = VirtualIndentation(code, kifelse, kifelif, kifif);
					code = "";
					blanc -= 5;
					for (int i = 0; i < blanc; i++)
						code += " ";
					code += '}';
					textbuf->replace(start, end, STR(code));
				}
				return 1;
			}

			//we try to detect the kind of declaration that can be associated to this variable
			if (o_checkvariable.value() && tkey[0] == '.') {
				int abspos = insert_position();
				int end = abspos-1;
				if (thevectorstrings.size() == 0)
					Updatesuggestion();

				short thetype = FindDeclarations(end);
				if (thetype != -1 && equivalences.find(thetype) != equivalences.end()) {
					code = kifGlobals->kifStringType[(KifType)thetype];
					thetype = kifGlobals->equivalences[thetype];
					if (thevectorstrings.find(thetype) != thevectorstrings.end()) {
						int x, y;
						str_choicemenuitems = "";
						position_to_xy(abspos, &x, &y);
						KifSuggest* kf = new KifSuggest(this, code, x, y, abspos, thetype, false);
					}
				}
				return 1;
			}

			if (key == 65293) {//carriage return...
				
				Fl::lock();
				if (str_choicemenuitems != "") {
					str_choicemenuitems = "";
					Fl::unlock();
					return 1;
				}
				Fl::unlock();
				
				int start = insert_position(), startcolor, endcolor, end;
				start = line_start(start);
				end = line_end(start, 1);

				startcolor = line_start(start - 1);
				endcolor = line_end(startcolor, 1);
				SyntaxColor(startcolor, endcolor);
				getcodewithnospace(startcolor, end, code);
				int blanc = VirtualIndentation(code, kifelse, kifelif, kifif);
				code = "";
				for (int i = 0; i<blanc; i++)
					code += " ";

				if (code.size()>0) {
					textbuf->insertlocaltext(end, code);
					insert_position(end + code.size());
				}
				Updateline();
				return 1;
			}
		}
		else {
			if (key == 65293) {//carriage return...

				Fl::lock();
				if (str_choicemenuitems != "") {
					str_choicemenuitems = "";
					Fl::unlock();
					return 1;
				}
				Fl::unlock();

				int start = insert_position(), startcolor, endcolor, end;
				start = line_start(start);
				end = line_end(start, 1);

				startcolor = line_start(start - 1);
				endcolor = line_end(startcolor, 1);
				SyntaxColor(startcolor, endcolor);

				char cwhite = 32;
				int blanc = startcolor;
				uchar c = (uchar)textbuf->byte_at(blanc);
				while (c == 9 || c == 32) {
					cwhite = c;
					blanc++;
					c = (uchar)textbuf->byte_at(blanc);
				}
				code = "";
				for (int i = startcolor; i<blanc; i++)
					code += cwhite;
				if (code.size()>0) {
					textbuf->insertlocaltext(end, code);
					insert_position(end + code.size());
				}

				Updateline();
				return 1;
			}
		}
		Updateline();
		return Fl_Text_Editor::handle(e);
	}

	virtual bool ismain() {
		return false;
	}

	virtual int handle(int e) {
		switch (e) {
		case FL_RELEASE:
			Updateline();
			break;
		case FL_KEYDOWN:
			if (Fl::event_key() == 122 && TESTCOMBI())
				return 0;
			break;
		case FL_KEYUP:
			if (key_callback_up(e) == 1)
				return 1;
			break;
		}
		return Fl_Text_Editor::handle(e);
	}
	//-----------------------------------------------------

	virtual KifLocalEditor* Editor(string pathname) {
		return this;
	}

	virtual void removebreaks() {
		stylebuf->setstyle(0, stylebuf->length(), 'A');
		SyntaxColor(0, 0);
	}

	int Findcorrespondance() {
		int linepos = currentline() - 1;
		int start = line_start(insert_position());
		int end = line_end(start, 1);
		code = textbuf->subrange(start, end);
		bool search = false;
		int direction = 1;

		if (code.find("}") != string::npos) {
			code = textbuf->subrange(0, end);
			direction = -1;
			search = true;
		}
		else
		if (code.find("{") != string::npos) {
			code = textbuf->subrange(start, textbuf->length());
			direction = 1;
			search = true;
		}
		if (search == false)
			return -1;

		x_readstring xr(code);
		xr.loadtoken();
		int count = 0;
		int line = -1;
		if (direction == 1) {
			for (start = 0; xr.stack.size(); start++) {
				if (xr.stack[start] == "{")
					count++;
				else
				if (xr.stack[start] == "}") {
					count--;
					if (count == 0)
						return xr.stackln[start] + linepos;
				}
			}
		}
		else {
			for (start = xr.stack.size() - 1; start >= 0; start--) {
				if (xr.stack[start] == "}")
					count++;
				else
				if (xr.stack[start] == "{") {
					count--;
					if (count == 0)
						return xr.stackln[start];
				}
			}
		}
		return -1;
	}

	virtual void EvaluateCommand();


	virtual void Setfilename(string f) {
		filename = f;
		Testtypefile();
		w->label(STR(tabname));
	}

	virtual void Addmenu() {
		const char* msg = KIFLTKSDEBUGGERTRINGS[27];
		menubar = new Fl_Menu_Bar(2, 30, 450, 30);
		menubar->tooltip(msg);
        menubar->add(KIFLTKSDEBUGGERTRINGS[28], FLCTRL + 's', Menu_CB, (void*)this);
        menubar->add(KIFLTKSDEBUGGERTRINGS[29], FLCTRL + FL_SHIFT + 's', Menu_CB, (void*)this);
#ifdef WIN32
		menubar->add(KIFLTKSDEBUGGERTRINGS[30], FLCTRL + 'z', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[31], FLCTRL + FL_SHIFT + 'z', Menu_CB, (void*)this, 0x80);
#else
		menubar->add(KIFLTKSDEBUGGERTRINGS[30], FLCTRL + 'u', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[31], FLCTRL + FL_SHIFT + 'u', Menu_CB, (void*)this, 0x80);
#endif
		menubar->add(KIFLTKSDEBUGGERTRINGS[32], FLCTRL + FL_SHIFT + 'l', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[33], FLCTRL + 'j', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[34], FLCTRL + 'i', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[35], FLCTRL + 'k', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[36], FLCTRL + 'l', Menu_CB, (void*)this, 0x80);
		menubar->add(KIFLTKSDEBUGGERTRINGS[37], 0, Menu_CB, (void*)this, 0x80);
		menubar->add(KIFLTKSDEBUGGERTRINGS[38], FLCTRL + 'f', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[39], FLCTRL + 'h', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[40], FLCTRL + 'n', Menu_CB, (void*)this, 0x80);
		menubar->add(KIFLTKSDEBUGGERTRINGS[41], FLCTRL + 'y', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[42], FLCTRL + FL_SHIFT + 'y', Menu_CB, (void*)this, 0x80);
		menubar->add(KIFLTKSDEBUGGERTRINGS[43], FLCTRL + 'x', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[44], FLCTRL + 'c', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[45], FLCTRL + 'v', Menu_CB, (void*)this);
		rmain = menubar->add(KIFLTKSDEBUGGERTRINGS[46], FLCTRL + 'r', Menu_CB, (void*)this);
		rthread = menubar->add(KIFLTKSDEBUGGERTRINGS[47], FLCTRL + 'e', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[48], FL_ALT + 'b', Menu_CB, (void*)this, 0x80);
		rdebug = menubar->add(KIFLTKSDEBUGGERTRINGS[49], FLCTRL + 'd', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[50], FLCTRL + 'b', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[51], FLCTRL + FL_SHIFT + 'b', Menu_CB, (void*)this);
	}

	virtual void Deactivate() {
		Fl_Menu_Item* items = (Fl_Menu_Item*)menubar->menu();
		items[rthread].deactivate();
		items[rdebug].deactivate();
		items[rmain].deactivate();
	}

	virtual void Reactivate() {
		Fl_Menu_Item* items = (Fl_Menu_Item*)menubar->menu();
		items[rthread].activate();
		items[rdebug].activate();
		items[rmain].activate();
	}

	virtual KifLocalEditor* KifCreateWindow(string path, bool fullnew = false) {
		return this;
	}
};

//------------------------------------------------------------------------------------------------------------------
static void see_choice(Fl_Widget* w, void* x) {
	KifSuggest* sug = (KifSuggest*)x;
	cerr << "";
}

static void call_choice(Fl_Widget* w, void* x) {
	KifChoiceItem* fli = (KifChoiceItem*)x;
	KifLocalEditor* edt = fli->editor;
	Fl::lock();
	edt->str_choicemenuitems = fli->fli->text;
	int pos = edt->str_choicemenuitems.find("(");
	if (pos == -1)
		edt->str_choicemenuitems += "(";
	else
		edt->str_choicemenuitems = edt->str_choicemenuitems.substr(0, pos+1);
	pos = fli->pos;
	edt->insert(STR(edt->str_choicemenuitems)); 
	Fl::unlock();
	delete fli->wnd;
}

static void close_choice(Fl_Widget* w, void* x) {
	delete (KifSuggest*)x;
}

KifSuggest::KifSuggest(KifLocalEditor* e, string& code, int x, int y, int abspos, int thetype, bool info) : wchoice(30, 10, 170, 30, ""),
	localwnd(x + e->window()->x() + 10, y + e->window()->y() + 10, 220, 50, STR(code)) {

	edt = e;
	nb = thevectorstrings[thetype].size() + 2;
	flm = new Fl_Menu_Item[nb];
	Fl_Menu_Item* fl;

	fl = new Fl_Menu_Item;
	fl->labeltype_ = 0;
	fl->labelfont_ = 0;
	fl->labelsize_ = 12;
	fl->labeltype_ = 0;
	fl->labelcolor_ = FL_BLUE;
	fl->text = KIFLTKSDEBUGGERTRINGS[52];
	fl->shortcut_ = 0;
	fl->callback_ = 0;
	fl->user_data_ = 0;
	fl->flags = 0;
	flm[0] = *fl;

	int i;
	char* v;
	e->choicemenuitems.clear();
	KifChoiceItem* kci;
	string val;
	string descript;
	for (i = 0; i < thevectorstrings[thetype].size(); i++) {
		val = thevectorstrings[thetype][i];
		if (thevectorinfos[thetype].find(val) != thevectorinfos[thetype].end()) {
			descript = thevectorinfos[thetype][val];
			if (info)
				val = descript;
		}
		else
			descript = KIFLTKSDEBUGGERTRINGS[53];

		v = strdup(STR(val));
		if (v != NULL) {
			fl = new Fl_Menu_Item;
			fl->labeltype_ = 0;
			fl->labelfont_ = 0;
			fl->labelsize_ = 12;
			fl->labeltype_ = 0;
			fl->labelcolor_ = FL_BLACK;
			fl->text = v;
			fl->shortcut_ = 0;
			fl->callback_ = call_choice;
			kci = new KifChoiceItem(e, i, abspos, fl, this);
			fl->user_data((void*)kci);
			e->choicemenuitems.push_back(kci);
			fl->flags = 0;
			flm[i + 1] = *fl;
		}
	}

	i++;
	flm[i].text = 0;
	flm[i].shortcut_ = 0;
	flm[i].callback_ = NULL;
	flm[i].user_data_ = 0;
	flm[i].flags = 0;
	flm[i].labeltype_ = 0;
	flm[i].labelfont_ = 0;
	flm[i].labelsize_ = 12;
	flm[i].labelcolor_ = FL_BLACK;

	wchoice.menu(flm);
	wchoice.value(0);
	localwnd.callback(close_choice, (void*)this);
	localwnd.set_modal();
	Fl::focus(&wchoice);
	localwnd.end();
	localwnd.show();
	const Fl_Menu_Item* cfl = wchoice.menu()->pulldown(wchoice.x(), wchoice.y(), wchoice.w(), wchoice.h(), wchoice.mvalue(), &wchoice);
	if (!cfl || cfl->submenu()) {
		delete this;
		return;
	}
	wchoice.picked(cfl);
}

KifSuggest::~KifSuggest() {
	int i;
	for (i = 0; i < edt->choicemenuitems.size(); i++) {
		free((char*)edt->choicemenuitems[i]->fli->text);
		delete edt->choicemenuitems[i];
	}
	edt->choicemenuitems.clear();
	delete[] flm;
}

//------------------------------------------------------------------------------------------------------------------


static void local_style_update(int pos,          // I - Position of update
	int        nInserted,    // I - Number of inserted chars
	int        nDeleted,     // I - Number of deleted chars
	int        nRestyled,    // I - Number of restyled chars
	const char *deletedText, // I - Text that was deleted
	void       *stylarg) {     // I - Callback data



	KifEditorTextBuf *stylebuf = ((struct Local_Style_Arg*)stylarg)->s;
	KifLocalEditor* editor = ((struct Local_Style_Arg*)stylarg)->e;
	unsigned char currentstyle = 'A';

	// If this is just a selection change, just unselect the style buffer...
	if (nInserted == 0 && nDeleted == 0) {
		stylebuf->unselect();
		return;
	}

	KifEditorTextBuf *styletext = NULL;
	if (editor != NULL) {
		editor->issaved = false;
		styletext = editor->textbuf;
		if (styletext->donotprocess)
			styletext = NULL;
	}

	if (nDeleted > 0) {
		// Just delete characters in the style buffer...
		if (styletext != NULL) {
			styletext->adddeletion(pos, nDeleted, deletedText);
			styletext->undoing.clear();
		}
	}

	// Track changes in the text buffer...
	if (nInserted > 0) {
		// Insert characters into the style buffer...
		if (pos > 0) {
			char* cstyle = stylebuf->text_range(pos - 1, pos);
			currentstyle = cstyle[0];
			free(cstyle);
		}
		if (pos == 0 && styletext != NULL && nInserted == styletext->length())
			styletext = NULL;

		char* style = new char[nInserted + 1];
		memset(style, currentstyle, nInserted);
		style[nInserted] = '\0';
		stylebuf->replace(pos, pos + nDeleted, style);
		delete[] style;
		if (styletext) {
			styletext->addinsertion(pos, nInserted, styletext->subrange(pos, pos + nInserted), nDeleted);
			styletext->undoing.clear();
		}
	}
	else
		stylebuf->remove(pos, pos + nDeleted);
}

//-------------------------------------------------------------------------------------------------------
void find_next_button_callback(Fl_Widget *w, void *data) {
	KifLocalFind* r = (KifLocalFind*)data;
	if (r->found == false) {
		delete r;
		return;
	}

	r->editor->searchstring = r->find_find.value();
	r->editor->lastcasesensitive = r->find_case.value();
	if (r->editor->Find() == false)
		delete r;
}

void find_cancel_button_callback(Fl_Widget *w, void *data) {
	KifLocalFind* r = (KifLocalFind*)data;
	delete r;
}

void replace_all_button_callback(Fl_Widget *w, void *data) {
	KifLocalReplace* r = (KifLocalReplace*)data;
	r->editor->searchstring = r->replace_find.value();
	r->editor->searchreplace = r->replace_with.value();
	r->editor->Replaceall(r->replace_case.value());
	delete r;
}

void replace_next_button_callback(Fl_Widget *w, void *data) {
	KifLocalReplace* r = (KifLocalReplace*)data;
	r->editor->searchstring = r->replace_find.value();
	r->editor->searchreplace = r->replace_with.value();
	if (r->start == 0 && r->end == 0)
		r->found = r->editor->Findfirst(r->start, r->end, r->replace_case.value());

	if (r->found) {
		r->editor->Replaceone(r->start, r->end, r->replace_case.value());
		r->found = r->editor->Findfirst(r->start, r->end, r->replace_case.value());
	}
	if (r->found == false)
		delete r;
}

void replace_cancel_button_callback(Fl_Widget *w, void *data) {
	KifLocalReplace* r = (KifLocalReplace*)data;
	delete r;
}
//------------------------------------------------------------------------------------------------
class KifLocalEditorMain : public KifLocalEditor {
public:
	KifCode* kifcodemain;
	KifCode* kifcode;
	bool startingdisplay;
	int count;
	string intermediarycode;
	int pos;
	ostringstream* os;
	Fl_Output o_kif_version;
	int numnew;


	KifLocalEditorMain(KifCode* kifmain, string fn, int x, int y, int h, int w, char* txt) : KifLocalEditor(NULL, x, y, h, w, txt), o_kif_version(850, 5, 140, 20, "") {
		numnew = 0;
		o_checkvariable.value(false);
		os = NULL;
		kifcodemain = kifmain;
		char buff[100];
		sprintf_s(buff, 100, "kif: %s", KIFVERSION);
		o_kif_version.value(buff);
		pos = 100;
		main = this;
		count = 0;
		kifcode = NULL;
		//kifGlobals->displayfunction=localdisplay;
		//kifGlobals->displayobject=this;
	}

	~KifLocalEditorMain() {
		delete kifcode;
		if (os != NULL)
			delete os;
	}

	KifCode* Kifcode() {
		return kifcode;
	}

	KifCode* Kifcodemain() {
		return kifcodemain;
	}

	void Setfilename(string f) {
		Testtypefile();
	}

	void Deactivate() {}
	void Reactivate() {}

	void Addmenu() {
		menubar = new Fl_Menu_Bar(2, 30, 450, 30);
		menubar->add(KIFLTKSDEBUGGERTRINGS[54], FLCTRL + FL_SHIFT + 'n', Menu_CB, (void*)this);    // userdata is always 'this'
		menubar->add(KIFLTKSDEBUGGERTRINGS[55], FLCTRL + 'o', Menu_CB, (void*)this);    // userdata is always 'this'
		menubar->add(KIFLTKSDEBUGGERTRINGS[28], FLCTRL + 's', Menu_CB, (void*)this, 0x80);
		menubar->add(KIFLTKSDEBUGGERTRINGS[56], FLCTRL + 'q', Menu_CB, (void*)this);
#ifdef WIN32
		menubar->add(KIFLTKSDEBUGGERTRINGS[30], FLCTRL + 'z', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[31], FLCTRL + FL_SHIFT + 'z', Menu_CB, (void*)this, 0x80);
#else
		menubar->add(KIFLTKSDEBUGGERTRINGS[30], FLCTRL + 'u', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[31], FLCTRL + FL_SHIFT + 'u', Menu_CB, (void*)this, 0x80);
#endif
		menubar->add(KIFLTKSDEBUGGERTRINGS[57], FLCTRL + FL_SHIFT + 'm', Menu_CB, (void*)this, FL_MENU_TOGGLE);
		menubar->add(KIFLTKSDEBUGGERTRINGS[32], FLCTRL + FL_SHIFT + 'l', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[58], FLCTRL + FL_SHIFT + 'e', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[59], FLCTRL + 'k', Menu_CB, (void*)this, 0x80);
		menubar->add(KIFLTKSDEBUGGERTRINGS[41], FLCTRL + 'y', Menu_CB, (void*)this);
		menubar->add(KIFLTKSDEBUGGERTRINGS[42], FLCTRL + FL_SHIFT + 'y', Menu_CB, (void*)this);
	}

	void EvaluateCommand() {
		char picked[1024];
		menubar->item_pathname(picked, sizeof(picked)-1);
		Fl_File_Chooser* filebrowser;

        if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[54])) {
			KifCreateWindow("", true);
			return;
		}


		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[55])) {
			filebrowser = new Fl_File_Chooser(".", KIFLTKSDEBUGGERTRINGS[60], Fl_File_Chooser::SINGLE, KIFLTKSDEBUGGERTRINGS[61]);
			filebrowser->show();
			while (filebrowser->shown())
				Fl::wait();
			if (filebrowser->value() != NULL)
				local_filebrowser_callback_open(filebrowser, this);
			delete filebrowser;
			return;
		}

		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[56])) {
			local_close_callback(window(), NULL);
			return;
		}

		string codeindente;
		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[28])) {
			if (filename == "") {
				filebrowser = new Fl_File_Chooser(".", KIFLTKSDEBUGGERTRINGS[60], Fl_File_Chooser::CREATE, KIFLTKSDEBUGGERTRINGS[62]);
				filebrowser->show();
				while (filebrowser->shown())
					Fl::wait();
				if (filebrowser->value() != NULL)
					local_filebrowser_callback(filebrowser, this);
				delete filebrowser;
			}
			else
				Savecontent();
			return;
		}

		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[32])) {
			string codeindente;
			if (!xipyfile) {
				IndentCode(code, codeindente, 0, kifelse, kifelif, kifif);
				Display(codeindente, -1);
			}
			else
				Display(code, -1);
			return;
		}

		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[58])) {
			KifCreateWindow("");
			return;
		}

		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[57])) {
			double_display = 1 - double_display;
			return;
		}

		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[59])) {
			Newkifcode();
			intermediarycode.clear();
			count = 0;
			code.clear();
			filename.clear();
			code = KIFLTKSDEBUGGERTRINGS[63];
			Display(code, -1);
			code.clear();
			main->textbuf->clear();
			return;
		}

		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[41])) {
			int sz = localstyletable[0].size;
			sz += 1;
			for (int i = 0; i < nbelements; i++)
				localstyletable[i].size = sz;
			Fl::redraw();
			return;
		}

		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[42])) {
			int sz = localstyletable[0].size;
			if (sz <= 12)
				return;
			sz -= 1;
			for (int i = 0; i<nbelements; i++)
				localstyletable[i].size = sz;
			Fl::redraw();
			return;
		}

		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[30])) {
			//we remove the top of the list in textbuf doing
			if (textbuf->doing.size()>0) {
				textbuf->donotprocess = true;
				e_modified& e = textbuf->doing.back();
				if (!e.inserted) {
					textbuf->insert(e.pos, e.buff.c_str()); //removed, we reinsert it
					insert_position(e.pos + e.nb);
				}
				else {
					textbuf->remove(e.pos, e.pos + e.nb);
					insert_position(e.pos);
				}
				textbuf->undoing.push_back(e);
				textbuf->doing.pop_back();
				textbuf->donotprocess = false;
			}
			return;
		}

		if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[31])) {
			if (textbuf->undoing.size() > 0) {
				textbuf->donotprocess = true;
				e_modified& e = textbuf->undoing.back();
				if (e.inserted) {
					textbuf->insert(e.pos, e.buff.c_str()); //removed, we reinsert it
					insert_position(e.pos + e.nb);
				}
				else {
					textbuf->remove(e.pos, e.pos + e.nb);
					insert_position(e.pos);
				}
				textbuf->doing.push_back(e);
				textbuf->undoing.pop_back();
				textbuf->donotprocess = false;
			}
			return;
		}


	}


	void Findbreakpoints(map<string, vector<int> >& breaks) {
		for (int i = 1; i < editors.size(); i++) {
			if (editors[i] != NULL)
				editors[i]->Findbreakpoints(breaks);
		}
	}

	void removebreaks() {
		for (int i = 1; i < editors.size(); i++) {
			if (editors[i] != NULL)
				editors[i]->removebreaks();
		}
	}

	KifLocalEditor* Editor(string pathname) {
		if (pathname == "")
			return NULL;
#ifdef WIN32
		pathname = s_to_lower(pathname);
#endif
		for (int i = 1; i < editors.size(); i++) {
			if (editors[i] != NULL && editors[i]->filename == pathname)
				return editors[i];
		}
		return NULL;
	}

	//In this case, the buffer can wrap, the line position is much more complex then...
	//we cannot use position_to_line
	int currentline(int pos = -1) {
		int linenum = 1;
		if (pos == -1)
			pos = insert_position();

		if (pos > textbuf->length())
			pos = textbuf->length();

		for (int i = 0; i < pos; i++) {
			if (textbuf->byte_at(i) == '\n')
				linenum++;
		}
		return linenum;
	}

	KifLocalEditor* KifCreateWindow(string pathname, bool fullnew = false) {
		KifLocalEditor* e = Editor(pathname);
		if (e != NULL) {
			e->show();
			return e;
		}

		Fl_Group* w = new Fl_Group(0, 30, 1000, 800, STR(pathname));
        e = new KifLocalEditor(this, 1, 60, 999, 769, STR(pathname));
        e->w=w;
		e->idwnd = editors.size();
		editors.push_back(e);
		e->Addmenu();		
		if (!fullnew)
			e->Setvalue(code);
		e->Line(0);
		//w->size_range(10, 10, 0, 0);
		if (pathname == "")
			e->issaved = false;
		else {
			e->filename = pathname;
			e->Testtypefile();
			e->issaved = true;
		}

        w->label(STR(e->tabname));
        w->end();
        w->resizable(e);
		
        tabwindows->add(w);
        tabwindows->value(w);
        tabwindows->hide();
        tabwindows->show();
		return e;
	}

	int Start() {
		return textbuf->length();
	}

	KifCode* Newkifcode() {
		if (kifcode != NULL)
			delete kifcode;

		kifcode = Brandnewkifcode(kifcodemain);
		KifSetKifDebuggingFunction(&localdebugger);
		return kifcode;
	}

	void addfilenamepath(string& pathname) {
		if (pathname != "")
			AddALoadingPath(pathname);
	}

	void ExecuteKiF(KifLocalEditor* e, string thecode, int start, bool debugmode, bool threadmode) {
		string pathname = e->filename;
		bool asdebug = false;
		if (debugmode)
			asdebug = true;
		else {
			Newkifcode();
			if (pathname == "") {
				pathname = KIFLTKSDEBUGGERTRINGS[64];
				localdebugger.AddFilename(pathname, kifcode);
			}
			else
				localdebugger.AddFilename(pathname, kifcode);
		}

		addfilenamepath(pathname);
		window()->cursor(FL_CURSOR_WAIT);
		if (os != NULL)
			delete os;
		os = new ostringstream;
		*os << "\n";
		fltkcompileandrun(e, kifcode, thecode, os, false, asdebug, threadmode);
		if (os->str().size() > 1)
			Display(os->str(), start);
		code = thecode;
		window()->cursor(FL_CURSOR_DEFAULT);
	}

	void Display(string a, int end) {
		int start;

		if (end >= EDITORMAXBUFFER || (textbuf->length() + a.size()) > EDITORMAXBUFFER) {
			textbuf->text(STR(a));
			end = a.size();
		}
		else
		if (end == -1) {
			end = textbuf->length();
			textbuf->append(STR(a));
		}
		else
			textbuf->insertlocaltext(end, a);

		start = end;
		end = textbuf->length();
		stylebuf->setstyle(start, end, 'M');
		insert_position(end);
		show_insert_position();
		Updateline();
		redraw();
        tabwindows->value(mainwindow->w);
	}

	void Setvalue(string& c) {
		utf8 = s_is_utf8(USTR(c), c.size());
		string s;
		if (!utf8) {
			s_latin_to_utf8(s, USTR(c));
			code = s;
		}
		else
			code = c;
		o_utf8.value(utf8);
	}

	string Value() {
		return code;
	}

	int key_callback_up(int e) {
		int key = Fl::event_key();
		int ekey = Fl::event_state();
		if (key == 65289 && (ekey & FL_CTRL) == FL_CTRL) { //ctrl+tab
			int idx = idwnd + 1;
			if (idx >= editors.size())
				idx = 0;
			tabwindows->value(editors[idx]->w);
			return 1;
		}

		if (key == 65293) {//carriage return...
			int start = line_start(insert_position());
			int end = line_end(start, 1);
			string c = intermediarycode;
			int blanc = VirtualIndentation(c, kifelse, kifelif, kifif);
			string blank;
			for (int i = 0; i<blanc; i++)
				blank += " ";

			if (blank.size()>0) {
				textbuf->insertlocaltext(end, blank);
				insert_position(end + blank.size());
			}
			return 1;
		}
		return Fl_Text_Editor::handle(e);
	}

	int key_callback_down(int e) {
		textbuf->donotprocess = false;
		int key = Fl::event_key();
		if (key != 65293) {
			Updateline();
			return Fl_Text_Editor::handle(e);
		}

		if (os != NULL)
			delete os;
		os = new ostringstream;

		int start = 0;
		int end = 0;
		bool execution = false;
		bool display = false;
		bool add = false;
		textbuf->selection_position(&start, &end);
		string s;
		char c;
		if (start != end) {
			s = textbuf->selection_text();
			textbuf->select(end, end);
			s = Trim(s);
			if (s != "") {
				intermediarycode.clear();
				execution = true;
				c = s[s.size() - 1];
				if (c != ';')
					display = true;
				intermediarycode = s;
			}
		}
		else {
			start = insert_position();
			start = line_start(start);
			end = line_end(start, 1);
			s = textbuf->subrange(start, end);
			if (s.size() > 5 && s[s.size() - 1] == '}' && justwhite(s) != 0) {
				s[s.size() - 6] = '}';
				s[s.size() - 1] = ' ';
				textbuf->replace(start, end, STR(s));
			}
			s = Trim(s);
			SyntaxColor(start, end);
		}


		if (s != "") {
			if (!execution) {
				execution = false;
				c = s[s.size() - 1];
				if (isinstruction(s)) {
					intermediarycode += s;
					intermediarycode += "\n";
					add = true;
				}
				else {
					if (c == '{') {
						intermediarycode += s;
						intermediarycode += "\n";
						count++;
						add = true;
					}
					else {
						if (c == '}') {
							intermediarycode += s;
							intermediarycode += "\n";
							count--;
							add = true;
							if (count < 0) {
								intermediarycode.clear();
								count = 0;
								*os << KIFLTKSDEBUGGERTRINGS[65] << endl;
							}
							else {
								if (count == 0) {
									execution = true;
									*os << KIFLTKSDEBUGGERTRINGS[66];
								}
							}
						}
						else {
							if (c == ';') {
								intermediarycode += s;
								intermediarycode += "\n";
								add = true;
								execution = true;
							}
							else {
								intermediarycode += s;
								intermediarycode += "\n";
								execution = true;
								display = true;
							}
						}
					}
				}
			}
		}

		if (execution && !count) {
			if (add)
				code += intermediarycode;
			startingdisplay = true;
			if (kifcode == NULL) {
				Newkifcode();
				addfilenamepath(filename);
			}
			*os << "\n";
			fltkcompileandrun(NULL, kifcode, intermediarycode, os, display, false, false);
			intermediarycode.clear();
		}

		if (os->str().size() > 1)
			Display(os->str(), end);

		end = textbuf->length();
		if (end > 0)
			stylebuf->setstyle(end - 1, end, 'A');
		Updateline();
		return Fl_Text_Editor::handle(e);
	}

	int handle(int e) {
		switch (e) {
		case FL_KEYDOWN:
			if (Fl::event_key() == 122 && TESTCOMBI())
				return 0;
			if (key_callback_down(e) == 1)
				return 1;
			break;
		case FL_RELEASE:
			Updateline();
			break;
		case FL_KEYUP:
			return key_callback_up(e);
		}
		return Fl_Text_Editor::handle(e);
	}

	bool ismain() {
		return true;
	}

};
//--------------------------------------------------------------------------------------------
void Menu_CB(Fl_Widget*w, void*data) {
	KifLocalEditor* o = (KifLocalEditor*)data;
	o->EvaluateCommand();
}

void local_number_callback(Fl_Widget *w, void *data) {
	KifLocalEditor* local = (KifLocalEditor*)data;
	Fl_Input* d = (Fl_Input*)w;
	const char* val = d->value();
	if (val != NULL && strlen(val) > 0) {
		int line = atoi(val);
		local->Selectline(line, true);
	}
}

void local_close_callback(Fl_Widget *w, void *data) {
    if (data==NULL) {
        stopmainloop = true;
        delete w;
        return;
    }
    
    KifLocalEditor* e = (KifLocalEditor*)data;
    KifLocalEditorMain* em = (KifLocalEditorMain*)e->main;
    bool asked=false;
    
    for (int i = 1; i < editors.size(); i++) {
        e = editors[i];
        if (e != NULL) {
            if (e->Tosave() == false) {
                ostringstream message;
                message << KIFLTKSDEBUGGERTRINGS[67] << e->filename << KIFLTKSDEBUGGERTRINGS[68];
                asked=true;
                if (fl_ask(STR(message.str())) != 1)
                    return;
            }
        }
    }
    
    if (asked==false) {
        if (fl_ask(KIFLTKSDEBUGGERTRINGS[69]) != 1)
            return;
    }
    
    stopmainloop = true;
    delete w;
}

static void localdisplay(string s, void* o) {
	KifLocalEditorMain* e = (KifLocalEditorMain*)o;
	if (s == "")
		return;
	if (e->startingdisplay) {
		e->startingdisplay = false;
		s = "\n" + s;
	}
	s = s_replacestrings(s, "\r\n", "\n");
	e->Display(s, -1);
}


static void local_filebrowser_callback(Fl_File_Chooser *fc, void* data) {
	KifLocalEditor* e = (KifLocalEditor*)data;
	e->filename = fc->value();
#ifdef WIN32
	e->filename = s_replacestrings(e->filename, "/", "\\");
	e->filename = s_to_lower(e->filename);
#endif
	e->Testtypefile();
	bool add = true;
	if (e->filename.size()>4) {
		add = false;
		string sub = e->filename.substr(e->filename.size() - 4, 4);
		if (sub != ".kif")
			add = true;
	}
	if (add)
		e->filename += ".kif";

	e->Setfilename(e->filename);
	e->Savecontent();
}

static void local_filebrowser_callback_open(Fl_File_Chooser *fc, void* data) {
	if (!fc->shown()) {
		KifLocalEditor* e = (KifLocalEditor*)data;
		e->filename = fc->value();
#ifdef WIN32
		for (int i = 0; i < e->filename.size(); i++) {
			if (e->filename[i] == '/')
				e->filename[i] = '\\';
		}
		e->filename = s_to_lower(e->filename);
#endif
		e = e->KifCreateWindow(e->filename);
		e->Loadfromfile(e->filename);
	}
}

//---------------------------------------------------------------------------------------------------------------------------
///----------------------------------------------------------------------DEBUG SECTION---------------------------------------
void NextLine(Fl_Widget *w, void *data);
void Shortname(Fl_Widget *w, void *data);
void Gotonextbreak(Fl_Widget *w, void *data);
void StopExecution(Fl_Widget *w, void *data);
void Gotoend(Fl_Widget *w, void *data);
void Getin(Fl_Widget *w, void *data);
void Getout(Fl_Widget *w, void *data);

class KifDebugEditor : public Fl_Text_Editor {
public:
	KifEditorTextBuf* textbuf;
	KifEditorTextBuf* stylebuf;
	Local_Style_Arg stylearg;

	KifDebugEditor(int x, int y, int h, int w, const char* f) : Fl_Text_Editor(x, y, h, w, "") {
		textbuf = new KifEditorTextBuf();
		stylebuf = new KifEditorTextBuf();
		buffer(textbuf);
		wrap_mode(WRAP_AT_BOUNDS, 0);
		int nbelements = sizeof(localstyletable) / sizeof(localstyletable[0]);
		highlight_data(stylebuf, localstyletable, nbelements, 'A', NULL, 0);
		stylearg.s = stylebuf;
		stylearg.e = NULL;
		textbuf->add_modify_callback(local_style_update, &stylearg);
	}

	void value(string& s) {
		textbuf->text(STR(s));
	}

	void SyntaxColor() {
		int start = 0, max = 0;
		int rastart, raend;
		char* texte = textbuf->text();
		string localcode = texte;
		free(texte);
		rastart = 0;
		raend = stylebuf->length();
		stylebuf->setstyle(0, raend, 'A');

		x_fltk_reading xr(localcode);
		xr.loadtoken();
		int end = 0, l;
		const char* v;
		int i;
		for (i = 0; i < xr.stack.size(); i++) {
			l = xr.stack[i].size();
			v = (const char*)xr.stack[i].c_str();
			textbuf->search_forward(start, v, &end, 0);
			start = end + l;
			switch (xr.stacktype[i]) {
			case 1:
				stylebuf->setstyle(end, start, 'D');
				break;
			case 2:
				stylebuf->setstyle(end, start, 'E');
				break;
			case 3:
				stylebuf->setstyle(end, start, 'B');
				break;
			case 4:
				if (localkeywords.find(xr.stack[i]) != localkeywords.end())
					stylebuf->setstyle(end, start, 'G');
				else {
					if ((i > 0 && xr.stack[i - 1] == ".") || (i < xr.stack.size() - 1 && xr.stack[i + 1] == "("))
						stylebuf->setstyle(end, start, 'H');
					else
						stylebuf->setstyle(end, start, 'F');
				}
				break;
			case 9:
				stylebuf->setstyle(end, start, 'C');
				break;
			}
		}

		redisplay_range(rastart, raend);
	}

};


#define FOREVER 1e05
static bool indebug = false;
static int debugstatus = -1;
static void displaythestrings();
static bool available = false;

static int Kifeditorrun() {
	int tt = 1;
	Fl::lock();
	while (!stopmainloop) {
		if (available)
			displaythestrings();
		tt = Fl::wait(FOREVER);
	}
	Fl::unlock();
	return 0;
}

void debugger_callback(Fl_Widget* wdg, void* data) {}

static void Pushaction(int c) {
	debugstatus = c;
	Fl::awake(debug_callback);
}


class KifDebugWindow {
public:

	Fl_Double_Window*    debugwindow;
    KifDebugEditor*      localvariables;
    KifDebugEditor*      allvariables;
	KifDebugEditor*      stack;
	Fl_Output*           currentline;
#if defined(WIN32) || defined(APPLE)
	Fl_Return_Button*   nextline;
#else
	Fl_Button*		   nextline;
#endif
	Fl_Button*		   stop;
	Fl_Button*		   go;
	Fl_Button*          upToTheEnd;
	Fl_Button*          infunction;
	Fl_Button*          outfunction;
	Fl_Check_Button*    shortname;
	Fl_Output*		   ofilename;
	string localstack;
	string variables;
    string allthevariables;
	char ch[50];
	KifLocalEditor* editor;
	int vcurrentline;
	LockedThread loquet;
	string vfilename;


	KifDebugWindow(KifLocalEditor* e)  {
		debugwindow = new Fl_Double_Window(100, 100, 800, 700, KIFLTKSDEBUGGERTRINGS[70]);
		currentline = new Fl_Output(600, 600, 40, 25, KIFLTKSDEBUGGERTRINGS[25]);
		shortname = new Fl_Check_Button(670, 600, 60, 25, KIFLTKSDEBUGGERTRINGS[71]);
        localvariables = new KifDebugEditor(30, 15, 700, 135, KIFLTKSDEBUGGERTRINGS[72]);
        allvariables = new KifDebugEditor(30, 155, 700, 180, KIFLTKSDEBUGGERTRINGS[73]);
		stack = new KifDebugEditor(30, 340, 700, 250, KIFLTKSDEBUGGERTRINGS[74]);
		ofilename = new Fl_Output(100, 600, 400, 25, KIFLTKSDEBUGGERTRINGS[75]);
		nextline = new Fl_Return_Button(30, 650, 75, 25, KIFLTKSDEBUGGERTRINGS[14]);
		go = new Fl_Button(115, 650, 75, 25, KIFLTKSDEBUGGERTRINGS[76]);
		infunction = new Fl_Button(200, 650, 75, 25, KIFLTKSDEBUGGERTRINGS[77]);
		outfunction = new Fl_Button(285, 650, 75, 25, KIFLTKSDEBUGGERTRINGS[78]);
		stop = new Fl_Button(390, 650, 75, 25, KIFLTKSDEBUGGERTRINGS[79]);
		upToTheEnd = new Fl_Button(475, 650, 75, 25, KIFLTKSDEBUGGERTRINGS[80]);

		shortname->value(true);
		debugwindow->callback(debugger_callback);
		nextline->callback(NextLine, (void*)this);
		go->callback(Gotonextbreak, (void*)this);
		shortname->callback(Shortname, (void*)this);
		stop->callback(StopExecution, (void*)this);
		upToTheEnd->callback(Gotoend, (void*)this);
		infunction->callback(Getin, (void*)this);
		outfunction->callback(Getout, (void*)this);

		debugwindow->end();
		debugwindow->resizable(localvariables);

		editor = e;
		vcurrentline = 0;
	}

	~KifDebugWindow() {
		delete debugwindow;
	}

	bool Endofexecution() {
		if (kifGlobals->endofexecution)
			return true;
		return false;
	}

    void update(KifLocalEditor* e, string filename, string thevariables, string allvars,string thestack, long thecurrentline) {
        editor = e;
        localstack = thestack;
        variables = thevariables;
        allthevariables=allvars;
        sprintf_s(ch, 50, "%d", thecurrentline);
        vcurrentline = thecurrentline;
        vfilename = filename;
        
        Fl::lock();
        ofilename->value(STR(vfilename));
        currentline->value(ch);
        stack->value(localstack);
        localvariables->value(variables);
        allvariables->value(allthevariables);

        localvariables->SyntaxColor();
        allvariables->SyntaxColor();
        Fl::unlock();
        
        Pushaction(1);
        loquet.Blocked();
    }

	void display() {
		editor->Getline(vcurrentline, localdebugger.localline);
        if (tabwindows->value()!=editor->w)
            tabwindows->value(editor->w);

		debugwindow->show();
		debugwindow->redraw();
	}

	void Release() {
		loquet.Released();
	}

	void show() {
		debugwindow->show();
	}

	void hide() {
		debugwindow->hide();
	}
};

static KifDebugWindow* old_current_debug_display = NULL;
static KifDebugWindow* current_debug_display = NULL;

//this method is used to display the content of the debugger in a window
bool displaydebug(string thevariables, string allvariables, string thestack, string thefilename, long thecurrentline) {
	if (current_debug_display == NULL || current_debug_display->editor == NULL) {
		return false;
	}
	KifLocalEditor* e = current_debug_display->editor->Editor(thefilename);
	if (e == NULL) {
		return false;
	}
	current_debug_display->update(e, thefilename, thevariables, allvariables, thestack, thecurrentline);
	return true;
}

void NextLine(Fl_Widget *w, void *data) {
	nextcodeline(true);
	if (current_debug_display->Endofexecution()) {
		current_debug_display->Release();
		Pushaction(0);
	}
	else
		current_debug_display->Release();
}

void Shortname(Fl_Widget *w, void *data) {
	localdebugger.shortname = ((Fl_Check_Button*)w)->value();
}


void Gotonextbreak(Fl_Widget *w, void *data) {
	nextcodeline(false);
	if (current_debug_display->Endofexecution()) {
		current_debug_display->Release();
		Pushaction(0);
	}
	else
		current_debug_display->Release();
}

void Getin(Fl_Widget *w, void *data) {
	getinto(true);
	current_debug_display->Release();
}

void Getout(Fl_Widget *w, void *data) {
	exitfunction();
	current_debug_display->Release();
}


void StopExecution(Fl_Widget *w, void *data) {
	termination(((KifDebugWindow*)data)->editor->Kifcode());
	current_debug_display->Release();
	Pushaction(0);
}


void Gotoend(Fl_Widget *w, void *data) {
	uptotheend();
	current_debug_display->Release();
	Pushaction(0);
}
//-------------------------------------------------------------------------------------
void KifLocalEditor::EvaluateCommand() {
	char picked[80];
	menubar->item_pathname(picked, sizeof(picked)-1);

	Fl_File_Chooser* filebrowser;
	// How to handle callbacks..

	string codeindente;
	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[28])) {
		if (filename == "") {
			filebrowser = new Fl_File_Chooser(".", KIFLTKSDEBUGGERTRINGS[60], Fl_File_Chooser::CREATE, KIFLTKSDEBUGGERTRINGS[62]);
			filebrowser->show();
			while (filebrowser->shown())
				Fl::wait();
			if (filebrowser->value() != NULL)
				local_filebrowser_callback(filebrowser, this);
			delete filebrowser;
		}
		else
			Savecontent();
		return;
	}

    if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[29])) {
        filename="";
        filebrowser = new Fl_File_Chooser(".", KIFLTKSDEBUGGERTRINGS[60], Fl_File_Chooser::CREATE, KIFLTKSDEBUGGERTRINGS[81]);
        filebrowser->show();
        while (filebrowser->shown())
            Fl::wait();
        if (filebrowser->value() != NULL)
            local_filebrowser_callback(filebrowser, this);
        delete filebrowser;
        return;
    }

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[38])) {
		FindWindow();
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[39])) {
		ReplaceWindow();
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[36])) {
		Gotoline();
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[37])) {
		SetBlanks();
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[41])) {
		int sz = localstyletable[0].size;
		sz += 1;
		for (int i = 0; i < nbelements; i++)
			localstyletable[i].size = sz;
		Fl::redraw();
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[32])) {
		Setvalue(main->code);
		Indente();
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[42])) {
		int sz = localstyletable[0].size;
		if (sz <= 12)
			return;
		sz -= 1;
		for (int i = 0; i < nbelements; i++)
			localstyletable[i].size = sz;
		Fl::redraw();
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[40])) {
		if (lastop == 1)
			Find();
		else {
			if (lastop == 2) {
				int start, end;
				if (Findfirst(start, end, lastcasesensitive))
					Replaceone(start, end, lastcasesensitive);
			}
		}

		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[30])) {
		//we remove the top of the list in textbuf doing
		if (textbuf->doing.size() > 0) {
			textbuf->donotprocess = true;
			e_modified& e = textbuf->doing.back();
			char ins = e.inserted;
			if (ins == 3) {//Full replacement
				string c = Value();
				e_modified nv(0, 3, c.size(), c);
				textbuf->text(STR(e.buff));
				textbuf->undoing.push_back(nv);
				textbuf->doing.pop_back();
				textbuf->donotprocess = false;
				return;
			}

			if (!ins) {
				textbuf->insert(e.pos, e.buff.c_str()); //removed, we reinsert it
				insert_position(e.pos + e.nb);
			}
			else {
				textbuf->remove(e.pos, e.pos + e.nb);
				insert_position(e.pos);
			}
			textbuf->undoing.push_back(e);
			textbuf->doing.pop_back();
			if (ins == 2) { //connected actions
				e_modified& eb = textbuf->doing.back();
				textbuf->insert(eb.pos, eb.buff.c_str()); //removed, we reinsert it
				insert_position(eb.pos + eb.nb);
				textbuf->undoing.push_back(eb);
				textbuf->doing.pop_back();
			}
			textbuf->donotprocess = false;
		}
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[31])) {
		if (textbuf->undoing.size() > 0) {
			textbuf->donotprocess = true;
			e_modified& e = textbuf->undoing.back();
			if (e.inserted == 3) {//Full replacement
				string c = Value();
				e_modified nv(0, 3, c.size(), c);
				textbuf->doing.push_back(nv);
				textbuf->text(STR(e.buff));
				textbuf->undoing.pop_back();
				textbuf->donotprocess = false;
				return;
			}

			if (e.inserted) {
				textbuf->insert(e.pos, e.buff.c_str()); //removed, we reinsert it
				insert_position(e.pos + e.nb);
			}
			else {
				textbuf->remove(e.pos, e.pos + e.nb);
				insert_position(e.pos);
			}
			textbuf->doing.push_back(e);
			textbuf->undoing.pop_back();
			if (textbuf->undoing.size()) {
				e_modified& eb = textbuf->undoing.back();
				if (eb.inserted == 2) {
					//second connected action
					textbuf->insert(eb.pos, eb.buff.c_str()); //removed, we reinsert it
					insert_position(eb.pos + eb.nb);
					textbuf->doing.push_back(eb);
					textbuf->undoing.pop_back();
				}
			}
			textbuf->donotprocess = false;
		}
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[33])) {
		int line = Findcorrespondance();
		if (line != -1)
			Selectline(line + 1, true);
		return;
	}

	//we need to get the selected lines
	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[35])) {
		int start, end;
		int i = textbuf->selection_position(&start, &end);
		string s;
		if (!i) {
			start = insert_position();
			s = textbuf->subrange(start, start + 2);
			if (s == "//")
				textbuf->remove(start, start + 2);
			else
				textbuf->insert(start, "//");
			return;
		}

		bool add = false;
		char* str = textbuf->selection_text();
		int sz = strlen(str);
		if (str[sz - 1] == 10) {
			str[sz - 1] = 0;
			sz--;
			add = true;
		}
		s = str;
		free(str);

		//we uncomment, if it is already commented
		if (s[0] == '/' && s[1] == '/') {
			s = s.substr(2, sz - 2);
			string rep;
			string sub;
			rep = 10;
			sub = 10;
			sub += "//";
			s = s_replacestring(s, sub, rep);
			if (add)
				s += 10;
			textbuf->replace_selection(STR(s));
			return;
		}
		s.insert(0, "//");
		for (i = 0; i < sz; i++) {
			if (s[i] == 10)
				s.insert(i + 1, "//");
		}
		if (add)
			s += 10;
		textbuf->replace_selection(STR(s));
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[34])) {
		Indente();
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[46])) {
		Deactivate();
		main->textbuf->clear();
		main->ExecuteKiF(this, Value(), main->Start(), false, false);
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[47])) {
		Deactivate();
		main->textbuf->clear();
		main->ExecuteKiF(this, Value(), main->Start(), false, true);
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[49])) {
		if (filename == "") {
			main->Display(KIFLTKSDEBUGGERTRINGS[82], -1);
			return;
		}
		Deactivate();
		map<string, vector<int> > thebreaks;
		int linenum = -1;
		main->Findbreakpoints(thebreaks);
		int start;
		localdebugger.cleanall();
		KifCode* kifcode = Newkifcode();
		setdebugmode(true);

		if (thebreaks.size() == 0) {
			start = insert_position();
			start = line_start(start);
			thebreaks[filename].push_back(start);
		}
		else
			localdebugger.AddFilename(filename, kifcode);

		map<string, vector<int> >::iterator it;
		for (it = thebreaks.begin(); it != thebreaks.end(); it++) {
			vector<int>& localbreaks = it->second;
			string fname = it->first;
			KifLocalEditor* e = main->Editor(fname);
			if (e == NULL)
				continue;
			bool first = true;
			for (int i = 0; i < localbreaks.size(); i++) {
				if (first) {
					localdebugger.AddFilename(fname, kifcode);
					first = false;
				}
				start = localbreaks[i];
				e->position_to_line(start, &linenum);
				linenum += e->get_absolute_top_line_number();
				addbreakpoint(fname, linenum);
			}
		}

		main->ExecuteKiF(this, Value(), main->Start(), true, false);
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[48])) {
		termination(main->Kifcode());
		Reactivate();
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[50])) {
		int start = insert_position();
		start = line_start(start);
		int end = line_end(start, 1);
		if (stylebuf->byte_at(start) == 'I') {
			stylebuf->setstyle(start, end, 'A');
			SyntaxColor(start, end);
		}
		else {
			stylebuf->setstyle(start, end, 'I');
			redisplay_range(start, end);
		}
		return;
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[43])) {
		Fl_Text_Editor::kf_cut(0, this);
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[44])) {
		Fl_Text_Editor::kf_copy(0, this);
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[45])) {
		Fl_Text_Editor::kf_paste(0, this);
	}

	if (!strcmp(picked, KIFLTKSDEBUGGERTRINGS[51])) {
		main->removebreaks();
		return;
	}
}
//--------------------------------------------------------------------------------------------

void whenallisfinished();
static KifElement* kifreturnfromthread = NULL;
static KifLocalEditor* currentexecutionwindow;

static vector<string> stringtodisplay;


void clearstringtodisplay() {
	ThreadLock _lock(kifPrint);
	stringtodisplay.clear();
	available = false;
}

static void addstringtodisplay(string& s) {
	ThreadLock _lock(kifPrint);
	stringtodisplay.push_back(s);
	available = true;
}

static void displaythestrings() {
	string res;
	{
		ThreadLock _lock(kifPrint);
		for (int i = 0; i < stringtodisplay.size(); i++)
			res += stringtodisplay[i];
		stringtodisplay.clear();
		available = false;
	}

	mainwindow->Display(res, -1);
}

static void display_results(string s, void* object) {
	addstringtodisplay(s);
}

static bool fltkcompileandrun(KifLocalEditor* e, KifCode* local, string codeinit, ostringstream* os, bool display, bool asdebug, bool threadmode) {
	string code;

	clearstringtodisplay();

	currentexecutionwindow = e;
	if (display) {
		code += kifdisplayln;
		code += "(";
		code += codeinit;
		code += ");";
	}
	else
		code = codeinit;

	Cleanallerror();
	kifGlobals->terminationfunction = NULL;

	kifGlobals->doubledisplay = double_display;
	kifGlobals->terminationfunction = NULL;
	kifGlobals->os = os;
	kifGlobals->displayfunction = NULL;

	int firstinst = local->kstart->instructions.size();
	int igarbage = kifGlobals->garbage.size();
	int ilocal = local->locals.size();
	int cursor = kifGlobals->cursor;

	bin_hash<KifElement*> stack;
	bin_hash<KifElement*>::iterator it;

	vector<KifDomain*> keepstack;
	int idthread = 0;
	if (kifreturnfromthread != kifreturnfromthread) {
		delete kifreturnfromthread;
		kifreturnfromthread = NULL;
	}

	ResetGetEditor(local);

	keepstack = kifGlobals->THR[idthread]->threadstacks;
	kifGlobals->THR[idthread]->threadstacks.clear();

	int startlocal = local->locals.size();
	local->kstart->adding = true;
	kifGlobals->PushStack(local->kstart);
	code = conversion_latin_to_utf8(code);
	//We evaluate in kstart...
	kifGlobals->bnfkif->baseline = 1;
	kifGlobals->debuginfocurrent->currentfile = 0;
	local->Evaluate(local->kstart, code, false);
	kifGlobals->PopStack();
	if (Errorid(idthread) == true) {
		if (e != NULL)
			e->Reactivate();

		*os << Errorstring();
		kifGlobals->THR[idthread]->cleanerror();
		kifGlobals->garbage.dernier = igarbage;
		kifGlobals->cursor = cursor;
		return false;
	}

	kifGlobals->endofexecution = false;
	if (KifWindowMode() == true) {
		asdebug = false;
		threadmode = false;
	}

	if (asdebug) {
		if (old_current_debug_display != NULL)
			current_debug_display = old_current_debug_display;
		else
			current_debug_display = new KifDebugWindow(mainwindow);

		current_debug_display->show();
		debugstatus = -1;
		kifGlobals->terminationfunction = whenallisfinished;
		kifGlobals->doubledisplay = false;
		kifGlobals->displayfunction = display_results;
		kifreturnfromthread = local->kstart->RunThread(firstinst);
	}
	else {
		indebug = false;
		localdebugger.cleanall();
		if (threadmode) {
			kifGlobals->doubledisplay = false;
			kifGlobals->displayfunction = display_results;
			kifGlobals->terminationfunction = whenallisfinished;
			kifreturnfromthread = local->kstart->RunThread(firstinst);
		}
		else {
			kifGlobals->doubledisplay = double_display;
			local->kstart->Run(firstinst);
			Updatesuggestion();

			kifGlobals->THR[idthread]->threadstacks = keepstack;
			if (e != NULL)
				e->Reactivate();

			ThreadLock _lock(kifError);
			kifGlobals->executionbreak = 2;
			kifGlobals->endofexecution = true;
		}
	}


	//then from igarbage up to the end, we reference our elements
	if (!threadmode) {
		if (Errorid(idthread) == true) {
			*os << Errorstring();
			kifGlobals->THR[idthread]->cleanerror();
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------
static void display_callback(void* str) {
	if (str != NULL) {
		string* s = (string*)str;
		mainwindow->Display(*s, -1);
		delete s;
	}
	mainwindow->textbuf->donotprocess = false;
}

void whenallisfinished() {
	kifGlobals->terminationfunction = NULL;
	string serr;

	if (currentexecutionwindow != NULL)
		currentexecutionwindow->Reactivate();
	currentexecutionwindow = NULL;
	if (Error() == true) {
		serr = Errorstring();
		if (kifGlobals->doubledisplay)
			*mainwindow->os << serr;
		else
			addstringtodisplay(serr);
	}

	Cleanallerror();
	if (kifGlobals->doubledisplay)
		serr = mainwindow->os->str();

	if (kifGlobals->doubledisplay)
		Fl::awake(display_callback, new string(serr));
	else
		Fl::awake(display_callback, NULL);

	ThreadLock _lock(kifError);
	kifGlobals->endofexecution = true;
	kifGlobals->executionbreak = 2;
	Updatesuggestion();
}

void debug_callback(void*) {
	if (debugstatus != -1 && current_debug_display != NULL) {
		if (debugstatus == 0) {
			old_current_debug_display = current_debug_display;
			current_debug_display->hide();
			current_debug_display = NULL;
		}
		else
			current_debug_display->display();
		Pushaction(-1);
	}
}

//------------------------------------------------------------------------------------------
extern "C" {
	Exporting bool Fltk_window_editor(KifCode* kifcode, string filename) {
		inittableutf8();
		localkeywords[KIFLTKSDEBUGGERTRINGS[83]] = true;
		localkeywords["<-"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[84]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[85]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[86]] = true;
		localkeywords["python"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[87]] = true;
		localkeywords["fst"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[88]] = true;
		localkeywords["parser"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[89]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[90]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[91]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[92]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[93]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[94]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[95]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[96]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[97]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[98]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[99]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[100]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[101]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[102]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[103]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[104]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[105]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[106]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[107]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[108]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[109]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[110]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[111]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[112]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[113]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[114]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[115]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[116]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[117]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[118]] = true;
		localkeywords["attribute"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[119]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[120]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[121]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[122]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[123]] = true;
		localkeywords["lambda"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[124]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[125]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[126]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[127]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[128]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[129]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[0]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[130]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[131]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[132]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[133]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[134]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[135]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[136]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[137]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[138]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[139]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[140]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[141]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[142]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[143]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[144]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[145]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[146]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[147]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[148]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[149]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[150]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[3]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[151]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[152]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[153]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[154]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[155]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[156]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[157]] = true;
		localkeywords["sqlite"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[158]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[159]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[160]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[161]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[162]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[163]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[164]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[165]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[166]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[167]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[168]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[169]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[170]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[171]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[172]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[173]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[174]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[175]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[176]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[177]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[178]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[179]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[180]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[181]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[182]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[183]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[184]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[1]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[185]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[186]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[187]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[188]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[189]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[190]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[191]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[192]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[193]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[194]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[195]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[196]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[197]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[198]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[199]] = true;
		localkeywords["class"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[200]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[201]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[202]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[203]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[204]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[205]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[206]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[207]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[208]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[209]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[210]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[211]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[212]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[213]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[214]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[215]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[216]] = true;
		localkeywords["enum"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[217]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[218]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[219]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[220]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[221]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[222]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[223]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[224]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[225]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[226]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[227]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[228]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[229]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[230]] = true;
		localkeywords["lib"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[231]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[2]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[232]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[233]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[234]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[235]] = true;

		//Haskell
		localkeywords[KIFLTKSDEBUGGERTRINGS[236]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[237]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[238]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[239]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[240]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[241]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[242]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[243]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[244]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[245]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[246]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[247]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[248]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[249]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[250]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[251]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[252]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[253]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[254]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[255]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[256]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[257]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[258]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[259]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[260]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[261]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[262]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[263]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[264]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[265]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[266]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[267]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[268]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[269]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[270]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[271]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[272]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[273]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[274]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[275]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[276]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[277]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[278]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[279]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[280]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[281]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[282]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[283]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[284]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[285]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[286]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[287]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[288]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[289]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[290]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[291]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[292]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[293]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[294]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[295]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[296]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[297]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[298]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[299]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[300]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[301]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[302]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[303]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[304]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[305]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[306]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[307]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[308]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[309]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[310]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[311]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[312]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[313]] = true;

		localkeywords[KIFLTKSDEBUGGERTRINGS[314]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[4]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[315]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[316]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[317]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[318]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[319]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[320]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[321]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[322]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[323]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[5]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[6]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[324]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[325]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[326]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[327]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[328]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[329]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[330]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[331]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[332]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[333]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[334]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[335]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[336]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[337]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[338]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[339]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[340]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[341]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[342]] = true;
		localkeywords["π"] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[343]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[344]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[345]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[346]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[347]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[348]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[349]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[350]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[351]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[352]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[353]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[354]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[355]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[356]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[357]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[358]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[359]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[360]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[361]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[362]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[363]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[364]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[365]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[366]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[367]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[368]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[369]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[370]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[371]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[372]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[373]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[374]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[375]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[376]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[377]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[378]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[379]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[380]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[381]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[382]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[383]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[384]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[385]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[386]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[387]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[388]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[389]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[390]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[391]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[392]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[393]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[394]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[395]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[396]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[397]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[398]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[399]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[400]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[401]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[402]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[403]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[404]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[405]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[406]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[407]] = true;
		localkeywords[KIFLTKSDEBUGGERTRINGS[408]] = true;

		kifreturnfromthread = NULL;
		stopmainloop = false;

        w_mainwindow = new Fl_Double_Window(50, 50, 1001, 850, KIFLTKSDEBUGGERTRINGS[409]);
		w_mainwindow->size_range(10, 10, 0, 0);
       
        tabwindows=new Fl_Tabs(0,0,1001,850,"");
        
        maingroup=new Fl_Group(0,30,1000,800,"Console");
		mainwindow = new KifLocalEditorMain(kifcode, filename, 1, 60, 999, 769, "Console");
		editors.push_back(mainwindow);

        mainwindow->w=maingroup;
		w_mainwindow->callback(local_close_callback, mainwindow);

		mainwindow->Addmenu();
		mainwindow->wrap_mode(3, -1);
        maingroup->end();
        
        tabwindows->end();
        tabwindows->hide();
        tabwindows->show();
        w_mainwindow->end();
        w_mainwindow->resizable(mainwindow);
        w_mainwindow->show();
		if (filename != "") {
			KifLocalEditor* elocal = mainwindow->KifCreateWindow(filename);
			elocal->Loadfromfile(filename);
		}
		Kifeditorrun();
		return true;
	}
}
