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

#ifdef _DEBUG
#include <vld.h>
#endif

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

Exporting void Clearallcodes();

static const char* kifdisplayln = "println";
static const char* kifwhile = "while";
static const char* kiffor = "for";
static const char* kifswitch = "switch";
static const char* kifif = "if";
static const char* kifelse = "else";
static const char* kifelif = "elif";


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
			svariables = "//Local variables:\n";
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
			allvariables += "//Main variables:\n";
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
			allvariables += "//Frame variables:";
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
			allvariables += "//Function variables:";
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
	cerr << "Terminated" << endl;
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
		find_dlg(300, 105, 450, 120, "Find"),
		find_find(70, 10, 350, 25, "String:"),
		find_next(20, 70, 130, 25, "Next"),
		find_cancel(290, 70, 130, 25, "Cancel"),
		find_case(290, 40, 60, 25, "Case")	{

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
		replace_dlg(300, 105, 600, 130, "Replace"),
		replace_find(80, 10, 500, 25, "Find:"),
		replace_with(80, 40, 500, 25, "Replace:"),

		replace_all(10, 90, 170, 25, "Replace All"),
		replace_next(200, 90, 170, 25, "Replace Next"),
		replace_cancel(390, 90, 100, 25, "Cancel"),
		replace_case(520, 90, 100, 25, "Case")	{

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

	KifLocalEditor(KifLocalEditor* m, int x, int y, int h, int w, char* f) : Fl_Text_Editor(x, y, h, w, ""), o_linenumber(930, 35, 55, 20, ""), o_utf8(850, 35, 45, 20, "UTF8"), o_checkvariable(750, 35, 45, 20, "Suggest") {
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
            tabname="Noname";
	}

	void Loadfromfile(string pathname) {
		ifstream load(STR(pathname), modeOuverture);
		if (load.fail()) {
			code = "Creating a new file:";
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
		const char* val = fl_input("Line", "");
		if (val != NULL && strlen(val) > 0) {
			int line = atoi(val);
			Selectline(line, false);
		}
	}

	void SetBlanks() {
		int sz = GetBlankSize();
		char ch[10];
		sprintf_s(ch, 10, "%d", sz);
		const char* val = fl_input("Blanks Size", ch);
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
		const char* msg = "Two 'runs'.\n\tThe first runs in the main frame and CANNOT BE stopped.\n\tThe second runs in a thread and CAN BE stopped.";
		menubar = new Fl_Menu_Bar(2, 30, 450, 30);
		menubar->tooltip(msg);
        menubar->add("File/Save", FLCTRL + 's', Menu_CB, (void*)this);
        menubar->add("File/Save as", FLCTRL + FL_SHIFT + 's', Menu_CB, (void*)this);
#ifdef WIN32
		menubar->add("Edit/Undo", FLCTRL + 'z', Menu_CB, (void*)this);
		menubar->add("Edit/Redo", FLCTRL + FL_SHIFT + 'z', Menu_CB, (void*)this, 0x80);
#else
		menubar->add("Edit/Undo", FLCTRL + 'u', Menu_CB, (void*)this);
		menubar->add("Edit/Redo", FLCTRL + FL_SHIFT + 'u', Menu_CB, (void*)this, 0x80);
#endif
		menubar->add("Edit/List", FLCTRL + FL_SHIFT + 'l', Menu_CB, (void*)this);
		menubar->add("Edit/Matching", FLCTRL + 'j', Menu_CB, (void*)this);
		menubar->add("Edit/Indent", FLCTRL + 'i', Menu_CB, (void*)this);
		menubar->add("Edit/(Un)comment", FLCTRL + 'k', Menu_CB, (void*)this);
		menubar->add("Edit/Goto line", FLCTRL + 'l', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Set blanks", 0, Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Find", FLCTRL + 'f', Menu_CB, (void*)this);
		menubar->add("Edit/Replace", FLCTRL + 'h', Menu_CB, (void*)this);
		menubar->add("Edit/Next", FLCTRL + 'n', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Font larger", FLCTRL + 'y', Menu_CB, (void*)this);
		menubar->add("Edit/Font smaller", FLCTRL + FL_SHIFT + 'y', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Cut", FLCTRL + 'x', Menu_CB, (void*)this);
		menubar->add("Edit/Copy", FLCTRL + 'c', Menu_CB, (void*)this);
		menubar->add("Edit/Paste", FLCTRL + 'v', Menu_CB, (void*)this);
		rmain = menubar->add("Command/Run (main thread)", FLCTRL + 'r', Menu_CB, (void*)this);
		rthread = menubar->add("Command/Run (threaded)", FLCTRL + 'e', Menu_CB, (void*)this);
		menubar->add("Command/Stop", FL_ALT + 'b', Menu_CB, (void*)this, 0x80);
		rdebug = menubar->add("Command/Debug", FLCTRL + 'd', Menu_CB, (void*)this);
		menubar->add("Command/Toggle Breakpoint", FLCTRL + 'b', Menu_CB, (void*)this);
		menubar->add("Command/Remove All Breakpoints", FLCTRL + FL_SHIFT + 'b', Menu_CB, (void*)this);
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
	fl->text = "Choose";
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
			descript = "No description available";

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
		menubar->add("File/New", FLCTRL + FL_SHIFT + 'n', Menu_CB, (void*)this);    // userdata is always 'this'
		menubar->add("File/Open", FLCTRL + 'o', Menu_CB, (void*)this);    // userdata is always 'this'
		menubar->add("File/Save", FLCTRL + 's', Menu_CB, (void*)this, 0x80);
		menubar->add("File/Quit", FLCTRL + 'q', Menu_CB, (void*)this);
#ifdef WIN32
		menubar->add("Edit/Undo", FLCTRL + 'z', Menu_CB, (void*)this);
		menubar->add("Edit/Redo", FLCTRL + FL_SHIFT + 'z', Menu_CB, (void*)this, 0x80);
#else
		menubar->add("Edit/Undo", FLCTRL + 'u', Menu_CB, (void*)this);
		menubar->add("Edit/Redo", FLCTRL + FL_SHIFT + 'u', Menu_CB, (void*)this, 0x80);
#endif
		menubar->add("Edit/Cut", FLCTRL + 'x', Menu_CB, (void*)this);
		menubar->add("Edit/Copy", FLCTRL + 'c', Menu_CB, (void*)this);
		menubar->add("Edit/Paste", FLCTRL + 'v', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Mirror", FLCTRL + FL_SHIFT + 'm', Menu_CB, (void*)this, FL_MENU_TOGGLE);
		menubar->add("Edit/List", FLCTRL + FL_SHIFT + 'l', Menu_CB, (void*)this);
		menubar->add("Edit/Editor", FLCTRL + FL_SHIFT + 'e', Menu_CB, (void*)this);
		menubar->add("Edit/Clean", FLCTRL + 'k', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Font larger", FLCTRL + 'y', Menu_CB, (void*)this);
		menubar->add("Edit/Font smaller", FLCTRL + FL_SHIFT + 'y', Menu_CB, (void*)this);
	}

	void EvaluateCommand() {
		char picked[1024];
		menubar->item_pathname(picked, sizeof(picked)-1);
		Fl_File_Chooser* filebrowser;

        if (!strcmp(picked, "File/New")) {
			KifCreateWindow("", true);
			return;
		}


		if (!strcmp(picked, "File/Open")) {
			filebrowser = new Fl_File_Chooser(".", "*.kif|*.xip", Fl_File_Chooser::SINGLE, "Load your file");
			filebrowser->show();
			while (filebrowser->shown())
				Fl::wait();
			if (filebrowser->value() != NULL)
				local_filebrowser_callback_open(filebrowser, this);
			delete filebrowser;
			return;
		}

		if (!strcmp(picked, "File/Quit")) {
			Clearallcodes();
			if (kifcode != NULL) {
				delete kifcode;
				kifcode = NULL;
			}
			if (kifGlobals != NULL) {
				delete kifGlobals;
				kifGlobals = NULL;
			}
			local_close_callback(window(), NULL);
			return;
		}

		string codeindente;
		if (!strcmp(picked, "File/Save")) {
			if (filename == "") {
				filebrowser = new Fl_File_Chooser(".", "*.kif|*.xip", Fl_File_Chooser::CREATE, "Save your file");
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

		if (!strcmp(picked, "Edit/List")) {
			string codeindente;
			if (!xipyfile) {
				IndentCode(code, codeindente, 0, kifelse, kifelif, kifif);
				Display(codeindente, -1);
			}
			else
				Display(code, -1);
			return;
		}

		if (!strcmp(picked, "Edit/Editor")) {
			KifCreateWindow("");
			return;
		}

		if (!strcmp(picked, "Edit/Mirror")) {
			double_display = 1 - double_display;
			return;
		}

		if (!strcmp(picked, "Edit/Clean")) {
			Newkifcode();
			intermediarycode.clear();
			count = 0;
			code.clear();
			filename.clear();
			code = "cleaned\n";
			Display(code, -1);
			code.clear();
			main->textbuf->clear();
			return;
		}

		if (!strcmp(picked, "Edit/Font larger")) {
			int sz = localstyletable[0].size;
			sz += 1;
			for (int i = 0; i < nbelements; i++)
				localstyletable[i].size = sz;
			Fl::redraw();
			return;
		}

		if (!strcmp(picked, "Edit/Font smaller")) {
			int sz = localstyletable[0].size;
			if (sz <= 12)
				return;
			sz -= 1;
			for (int i = 0; i<nbelements; i++)
				localstyletable[i].size = sz;
			Fl::redraw();
			return;
		}

		if (!strcmp(picked, "Edit/Undo")) {
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

		if (!strcmp(picked, "Edit/Redo")) {
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

		if (!strcmp(picked, "Edit/Cut")) {
			Fl_Text_Editor::kf_cut(0, this);
		}

		if (!strcmp(picked, "Edit/Copy")) {
			Fl_Text_Editor::kf_copy(0, this);
		}

		if (!strcmp(picked, "Edit/Paste")) {
			Fl_Text_Editor::kf_paste(0, this);
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
				pathname = "none";
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
								*os << "Wrong instruction" << endl;
							}
							else {
								if (count == 0) {
									execution = true;
									*os << "\nrecorded\n";
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
                message << "File:'" << e->filename << "' not saved... Do you want to quit? ";
                asked=true;
                if (fl_ask(STR(message.str())) != 1)
                    return;
            }
        }
    }
    
    if (asked==false) {
        if (fl_ask("Do you want to quit?") != 1)
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
		debugwindow = new Fl_Double_Window(100, 100, 800, 700, "Debugger");
		currentline = new Fl_Output(600, 600, 40, 25, "Line");
		shortname = new Fl_Check_Button(670, 600, 60, 25, "Short value");
        localvariables = new KifDebugEditor(30, 15, 700, 135, "Locals");
        allvariables = new KifDebugEditor(30, 155, 700, 180, "Alls");
		stack = new KifDebugEditor(30, 340, 700, 250, "Stack");
		ofilename = new Fl_Output(100, 600, 400, 25, "Filename");
		nextline = new Fl_Return_Button(30, 650, 75, 25, "Next");
		go = new Fl_Button(115, 650, 75, 25, "Goto");
		infunction = new Fl_Button(200, 650, 75, 25, "In");
		outfunction = new Fl_Button(285, 650, 75, 25, "Out");
		stop = new Fl_Button(390, 650, 75, 25, "Stop");
		upToTheEnd = new Fl_Button(475, 650, 75, 25, "To End");

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
	if (!strcmp(picked, "File/Save")) {
		if (filename == "") {
			filebrowser = new Fl_File_Chooser(".", "*.kif|*.xip", Fl_File_Chooser::CREATE, "Save your file");
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

    if (!strcmp(picked, "File/Save as")) {
        filename="";
        filebrowser = new Fl_File_Chooser(".", "*.kif|*.xip", Fl_File_Chooser::CREATE, "Save your file as");
        filebrowser->show();
        while (filebrowser->shown())
            Fl::wait();
        if (filebrowser->value() != NULL)
            local_filebrowser_callback(filebrowser, this);
        delete filebrowser;
        return;
    }

	if (!strcmp(picked, "Edit/Find")) {
		FindWindow();
		return;
	}

	if (!strcmp(picked, "Edit/Replace")) {
		ReplaceWindow();
		return;
	}

	if (!strcmp(picked, "Edit/Goto line")) {
		Gotoline();
		return;
	}

	if (!strcmp(picked, "Edit/Set blanks")) {
		SetBlanks();
		return;
	}

	if (!strcmp(picked, "Edit/Font larger")) {
		int sz = localstyletable[0].size;
		sz += 1;
		for (int i = 0; i < nbelements; i++)
			localstyletable[i].size = sz;
		Fl::redraw();
		return;
	}

	if (!strcmp(picked, "Edit/List")) {
		Setvalue(main->code);
		Indente();
		return;
	}

	if (!strcmp(picked, "Edit/Font smaller")) {
		int sz = localstyletable[0].size;
		if (sz <= 12)
			return;
		sz -= 1;
		for (int i = 0; i < nbelements; i++)
			localstyletable[i].size = sz;
		Fl::redraw();
		return;
	}

	if (!strcmp(picked, "Edit/Next")) {
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

	if (!strcmp(picked, "Edit/Undo")) {
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

	if (!strcmp(picked, "Edit/Redo")) {
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

	if (!strcmp(picked, "Edit/Matching")) {
		int line = Findcorrespondance();
		if (line != -1)
			Selectline(line + 1, true);
		return;
	}

	//we need to get the selected lines
	if (!strcmp(picked, "Edit/(Un)comment")) {
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

	if (!strcmp(picked, "Edit/Indent")) {
		Indente();
		return;
	}

	if (!strcmp(picked, "Command/Run (main thread)")) {
		Deactivate();
		main->textbuf->clear();
		main->ExecuteKiF(this, Value(), main->Start(), false, false);
		return;
	}

	if (!strcmp(picked, "Command/Run (threaded)")) {
		Deactivate();
		main->textbuf->clear();
		main->ExecuteKiF(this, Value(), main->Start(), false, true);
		return;
	}

	if (!strcmp(picked, "Command/Debug")) {
		if (filename == "") {
			main->Display("Save this window first", -1);
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

	if (!strcmp(picked, "Command/Stop")) {
		termination(main->Kifcode());
		Reactivate();
		return;
	}

	if (!strcmp(picked, "Command/Toggle Breakpoint")) {
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

	if (!strcmp(picked, "Edit/Cut")) {
		Fl_Text_Editor::kf_cut(0, this);
	}

	if (!strcmp(picked, "Edit/Copy")) {
		Fl_Text_Editor::kf_copy(0, this);
	}

	if (!strcmp(picked, "Edit/Paste")) {
		Fl_Text_Editor::kf_paste(0, this);
	}

	if (!strcmp(picked, "Command/Remove All Breakpoints")) {
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
		localkeywords["is"] = true;
		localkeywords["<-"] = true;
		localkeywords["do"] = true;
		localkeywords["predicatevar"] = true;
		localkeywords["predicate"] = true;
		localkeywords["python"] = true;
		localkeywords["term"] = true;
		localkeywords["fst"] = true;
		localkeywords["transducer"] = true;
		localkeywords["parser"] = true;
		localkeywords["dependency"] = true;
		localkeywords["node"] = true;
		localkeywords["_garbagefunction"] = true;
		localkeywords["grammar"] = true;
		localkeywords["otherwise"] = true;
		localkeywords["_garbagesize"] = true;
		localkeywords["setdebugfunction"] = true;
		localkeywords["parentthreadid"] = true;
		localkeywords["imatrix"] = true;
		localkeywords["setdebugdata"] = true;
		localkeywords["sthrough"] = true;
		localkeywords["ithrough"] = true;
		localkeywords["fthrough"] = true;
		localkeywords["createserver"] = true;
		localkeywords["getstackinfo"] = true;
		localkeywords["waitonfalse"] = true;
		localkeywords["env"] = true;
		localkeywords["runadthread"] = true;
		localkeywords["synchronous"] = true;
		localkeywords["filebrowser"] = true;
		localkeywords["polynomial"] = true;
		localkeywords["printlnerr"] = true;
		localkeywords["printjlnerr"] = true;
		localkeywords["waitonjoined"] = true;
		localkeywords["myvariable"] = true;
		localkeywords["marginleft"] = true;
		localkeywords["exclusive"] = true;
		localkeywords["joined"] = true;
		localkeywords["wtreeitem"] = true;
		localkeywords["protected"] = true;
		localkeywords["attribute"] = true;
		localkeywords["riterator"] = true;
		localkeywords["fraction"] = true;
		localkeywords["printerr"] = true;
		localkeywords["printjerr"] = true;
		localkeywords["function"] = true;
		localkeywords["lambda"] = true;
		localkeywords["sequence"] = true;
		localkeywords["threadid"] = true;
		localkeywords["pathname"] = true;
		localkeywords["continue"] = true;
		localkeywords["iterator"] = true;
		localkeywords["instance"] = true;
		localkeywords["println"] = true;
		localkeywords["printjln"] = true;
		localkeywords["address"] = true;
		localkeywords["boolean"] = true;
		localkeywords["autorun"] = true;
		localkeywords["private"] = true;
		localkeywords["browser"] = true;
		localkeywords["wchoice"] = true;
		localkeywords["woutput"] = true;
		localkeywords["waiting"] = true;
		localkeywords["_version"] = true;
		localkeywords["matrix"] = true;
		localkeywords["wheely"] = true;
		localkeywords["wheelx"] = true;
		localkeywords["remote"] = true;
		localkeywords["minute"] = true;
		localkeywords["_gcsize"] = true;
		localkeywords["socket"] = true;
		localkeywords["sound"] = true;
		localkeywords["mp3"] = true;
		localkeywords["button"] = true;
		localkeywords["scroll"] = true;
		localkeywords["switch"] = true;
		localkeywords["xmldoc"] = true;
		localkeywords["editor"] = true;
		localkeywords["loadin"] = true;
		localkeywords["winput"] = true;
		localkeywords["slider"] = true;
		localkeywords["wgroup"] = true;
		localkeywords["image"] = true;
		localkeywords["sqlite"] = true;
		localkeywords["pflush"] = true;
		localkeywords["rawstring"] = true;
		localkeywords["string"] = true;
		localkeywords["ustring"] = true;
		localkeywords["tuple"] = true;
		localkeywords["common"] = true;
		localkeywords["random"] = true;
		localkeywords["bottom"] = true;
		localkeywords["vector"] = true;
		localkeywords["return"] = true;
		localkeywords["_return"] = true;
		localkeywords["thread"] = true;
		localkeywords["window"] = true;
		localkeywords["wcounter"] = true;
		localkeywords["wprogress"] = true;
		localkeywords["bitmap"] = true;
		localkeywords["second"] = true;
		localkeywords["strict"] = true;
		localkeywords["print"] = true;
		localkeywords["printj"] = true;
		localkeywords["catch"] = true;
		localkeywords["table"] = true;
		localkeywords["false"] = true;
		localkeywords["eigen"] = true;
		localkeywords["wtabs"] = true;
		localkeywords["frame"] = true;
		localkeywords["extension"] = true;
		localkeywords["while"] = true;
		localkeywords["start"] = true;
		localkeywords["stdin"] = true;
		localkeywords["not"] = true;
		localkeywords["absent"] = true;
		localkeywords["from"] = true;
		localkeywords["notin"] = true;
		localkeywords["pause"] = true;
		localkeywords["const"] = true;
		localkeywords["sleep"] = true;
		localkeywords["reset"] = true;
		localkeywords["range"] = true;
		localkeywords["break"] = true;
		localkeywords["raise"] = true;
		localkeywords["float"] = true;
		localkeywords["wtree"] = true;
		localkeywords["class"] = true;
		localkeywords["bool"] = true;
		localkeywords["self"] = true;
		localkeywords["auto"] = true;
		localkeywords["eval"] = true;
		localkeywords["true"] = true;
		localkeywords["fail"] = true;
		localkeywords["cast"] = true;
		localkeywords["this"] = true;
		localkeywords["getc"] = true;
		localkeywords["long"] = true;
		localkeywords["time"] = true;
		localkeywords["call"] = true;
		localkeywords["list"] = true;
		localkeywords["port"] = true;
		localkeywords["_exit"] = true;
		localkeywords["wfile"] = true;
		localkeywords["file"] = true;
		localkeywords["enum"] = true;
		localkeywords["hash"] = true;
		localkeywords["bits"] = true;
		localkeywords["empty"] = true;
		localkeywords["null"] = true;
		localkeywords["kill"] = true;
		localkeywords["byte"] = true;
		localkeywords["sloop"] = true;
		localkeywords["uloop"] = true;
		localkeywords["bloop"] = true;
		localkeywords["iloop"] = true;
		localkeywords["floop"] = true;
		localkeywords["int"] = true;
		localkeywords["isa"] = true;
		localkeywords["use"] = true;
		localkeywords["lib"] = true;
		localkeywords["top"] = true;
		localkeywords["for"] = true;
		localkeywords["box"] = true;
		localkeywords["try"] = true;
		localkeywords["bit"] = true;
		localkeywords["tree"] = true;

		//Haskell
		localkeywords["where"] = true;
		localkeywords["repeat"] = true;
		localkeywords["cycle"] = true;
		localkeywords["replicate"] = true;
		localkeywords["filter"] = true;
		localkeywords["take"] = true;
		localkeywords["takeWhile"] = true;
		localkeywords["zipWith"] = true;
		localkeywords["zip"] = true;
		localkeywords["flip"] = true;
		localkeywords["drop"] = true;
		localkeywords["dropWhile"] = true;
		localkeywords["foldr"] = true;
		localkeywords["foldl"] = true;
		localkeywords["foldr1"] = true;
		localkeywords["foldl1"] = true;
		localkeywords["scanr"] = true;
		localkeywords["scanl"] = true;
		localkeywords["scanr1"] = true;
		localkeywords["scanl1"] = true;

		localkeywords["set"] = true;
		localkeywords["sset"] = true;
		localkeywords["uset"] = true;
		localkeywords["iset"] = true;
		localkeywords["fset"] = true;

		localkeywords["fvector"] = true;
		localkeywords["svector"] = true;
		localkeywords["uvector"] = true;
		localkeywords["ivector"] = true;
		localkeywords["bvector"] = true;

		localkeywords["map"] = true;

		localkeywords["maps"] = true;
		localkeywords["mapf"] = true;
		localkeywords["mapi"] = true;

		localkeywords["fmap"] = true;
		localkeywords["imap"] = true;
		localkeywords["smap"] = true;

		localkeywords["sfmap"] = true;
		localkeywords["simap"] = true;
		localkeywords["ssmap"] = true;

		localkeywords["ismap"] = true;
		localkeywords["ifmap"] = true;
		localkeywords["iimap"] = true;

		localkeywords["fsmap"] = true;
		localkeywords["ffmap"] = true;
		localkeywords["fimap"] = true;

		localkeywords["treemap"] = true;

		localkeywords["treemaps"] = true;
		localkeywords["treemapf"] = true;
		localkeywords["treemapi"] = true;

		localkeywords["ftreemap"] = true;
		localkeywords["itreemap"] = true;
		localkeywords["streemap"] = true;

		localkeywords["sftreemap"] = true;
		localkeywords["sitreemap"] = true;
		localkeywords["sstreemap"] = true;

		localkeywords["istreemap"] = true;
		localkeywords["iftreemap"] = true;
		localkeywords["iitreemap"] = true;

		localkeywords["fstreemap"] = true;
		localkeywords["fftreemap"] = true;
		localkeywords["fitreemap"] = true;

		localkeywords["primemap"] = true;

		localkeywords["primemaps"] = true;
		localkeywords["primemapf"] = true;
		localkeywords["primemapi"] = true;

		localkeywords["fprimemap"] = true;
		localkeywords["iprimemap"] = true;
		localkeywords["sprimemap"] = true;

		localkeywords["sfprimemap"] = true;
		localkeywords["siprimemap"] = true;
		localkeywords["ssprimemap"] = true;

		localkeywords["isprimemap"] = true;
		localkeywords["ifprimemap"] = true;
		localkeywords["iiprimemap"] = true;

		localkeywords["fsprimemap"] = true;
		localkeywords["ffprimemap"] = true;
		localkeywords["fiprimemap"] = true;

		localkeywords["xml"] = true;
		localkeywords["if"] = true;
		localkeywords["case"] = true;
		localkeywords["let"] = true;
		localkeywords["in"] = true;
		localkeywords["into"] = true;
		localkeywords["on"] = true;
		localkeywords["default"] = true;
		localkeywords["curl"] = true;
		localkeywords["and"] = true;
		localkeywords["or"] = true;
		localkeywords["else"] = true;
		localkeywords["elif"] = true;
		localkeywords["with"] = true;
		localkeywords["_endl"] = true;
		localkeywords["a_first"] = true;
		localkeywords["a_last"] = true;
		localkeywords["a_prefix"] = true;
		localkeywords["a_change"] = true;
		localkeywords["a_insert"] = true;
		localkeywords["a_delete"] = true;
		localkeywords["a_switch"] = true;
		localkeywords["a_nocase"] = true;
		localkeywords["a_surface"] = true;
		localkeywords["a_full"] = true;
		localkeywords["a_split"] = true;
		localkeywords["a_skip"] = true;
		localkeywords["a_track"] = true;
		localkeywords["pred_none"] = true;
		localkeywords["pred_weight"] = true;
		localkeywords["pred_weightstack"] = true;
		localkeywords["automaton"] = true;
		localkeywords["π"] = true;
		localkeywords["mapff"] = true;
		localkeywords["primemapff"] = true;
		localkeywords["treemapff"] = true;
		localkeywords["mapfi"] = true;
		localkeywords["primemapfi"] = true;
		localkeywords["treemapfi"] = true;
		localkeywords["mapfs"] = true;
		localkeywords["primemapfs"] = true;
		localkeywords["treemapfs"] = true;
		localkeywords["mapif"] = true;
		localkeywords["primemapif"] = true;
		localkeywords["treemapif"] = true;
		localkeywords["mapii"] = true;
		localkeywords["primemapii"] = true;
		localkeywords["treemapii"] = true;
		localkeywords["mapis"] = true;
		localkeywords["primemapis"] = true;
		localkeywords["treemapis"] = true;
		localkeywords["mapsf"] = true;
		localkeywords["primemapsf"] = true;
		localkeywords["treemapsf"] = true;
		localkeywords["mapsi"] = true;
		localkeywords["primemapsi"] = true;
		localkeywords["treemapsi"] = true;
		localkeywords["mapss"] = true;
		localkeywords["primemapss"] = true;
		localkeywords["treemapss"] = true;
		localkeywords["mapuu"] = true;
		localkeywords["uumap"] = true;
		localkeywords["mapui"] = true;
		localkeywords["uimap"] = true;
		localkeywords["mapuf"] = true;
		localkeywords["ufmap"] = true;
		localkeywords["mapiu"] = true;
		localkeywords["iumap"] = true;
		localkeywords["mapfu"] = true;
		localkeywords["fumap"] = true;
		localkeywords["treemapuu"] = true;
		localkeywords["uutreemap"] = true;
		localkeywords["treemapui"] = true;
		localkeywords["uitreemap"] = true;
		localkeywords["treemapuf"] = true;
		localkeywords["uftreemap"] = true;
		localkeywords["treemapiu"] = true;
		localkeywords["iutreemap"] = true;
		localkeywords["treemapfu"] = true;
		localkeywords["futreemap"] = true;
		localkeywords["primemapuu"] = true;
		localkeywords["uuprimemap"] = true;
		localkeywords["primemapui"] = true;
		localkeywords["uiprimemap"] = true;
		localkeywords["primemapuf"] = true;
		localkeywords["ufprimemap"] = true;
		localkeywords["primemapiu"] = true;
		localkeywords["iuprimemap"] = true;
		localkeywords["primemapfu"] = true;
		localkeywords["fuprimemap"] = true;
		localkeywords["binmapi"] = true;
		localkeywords["binmapii"] = true;
		localkeywords["binmapif"] = true;
		localkeywords["binmapis"] = true;
		localkeywords["binmapiu"] = true;
		localkeywords["iibinmap"] = true;
		localkeywords["ifbinmap"] = true;
		localkeywords["isbinmap"] = true;
		localkeywords["iubinmap"] = true;

		kifreturnfromthread = NULL;
		stopmainloop = false;

        w_mainwindow = new Fl_Double_Window(50, 50, 1001, 850, "KIF");
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
