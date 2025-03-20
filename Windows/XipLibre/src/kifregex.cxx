/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the KiF library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : KiF
Version    : 1.00
filename   : kifregex.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language: System methods
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DL

#include "kif.h"
#include "kifregex.h"
#include "kifversion.h"

#ifdef MAVERICK
using std::regex;
using std::sregex_token_iterator;
using std::smatch;
using std::match_results;
using std::wregex;
using std::wsregex_token_iterator;
using std::wsmatch;
#elif WIN32
using std::regex;
using std::sregex_token_iterator;
using std::smatch;
using std::match_results;
#else
using boost::regex;
using boost::sregex_token_iterator;
using boost::smatch;
using boost::match_results;
#endif

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

short KifNewId(string n);
string KifIdString(short n);

#define STR(x) (char*)x.c_str()


KifElement* PreReplaceRgx(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kreg = callfunc->Evaluatethread(1, dom, idthread);

	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		wstring reg = kreg->UString();
		wstring rep = callfunc->Evaluatethread(2, dom, idthread)->UString();

		try {
			wregex pattern(reg);
			wstring res = regex_replace(str, pattern, rep);
			return callfunc->kifcode->Provideustring(res);			
		}
		catch (...) {
			string estr = "RGX(001):Wrong regular expression: ";
			estr += kreg->String();
			return callfunc->kifcode->Returnerror(estr);
		}
	}
	else {
		string str = kstr->String();
		string reg = kreg->String();
		string rep = callfunc->Evaluatethread(2, dom, idthread)->String();

		try {
			regex pattern(reg);
			string res = regex_replace(str, pattern, rep);
			return callfunc->kifcode->Providestring(res);
		}
		catch (...) {
			str = "RGX(001):Wrong regular expression: ";
			str += reg;
			return callfunc->kifcode->Returnerror(str);
		}
	}
}



KifElement* PreRegex(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	
	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kreg = callfunc->Evaluatethread(1, dom, idthread);

	if (kstr->type == kifUString) {
		const wsregex_token_iterator end;
		wstring str = kstr->UString();
		wstring reg = kreg->UString();
		try {
			wregex pattern(reg);


			if (contextualpattern->type == kifVector) {
				KifBaseUString localstring(NULL, NULL, 0);
				localstring.kifcode = callfunc->kifcode;

				KifElement* vect = Selectvectorustring(callfunc, contextualpattern);
				for (wsregex_token_iterator i(str.begin(), str.end(), pattern); i != end; ++i) {
					localstring.value = *i;
					vect->Push(&localstring);
				}
				return vect;
			}

			if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat || contextualpattern->isString()) {
				wsmatch result;
				if (regex_search(str, result, pattern) == true) {
					if (contextualpattern->isString())
						return callfunc->kifcode->Provideustring(result[0]);						
					return callfunc->kifcode->Provideinteger(result.position());
				}
				return kifNULL;
			}

			if (regex_match(str, pattern) == false)
				return kifFALSE;
		}
		catch (...) {
			string estr = "RGX(002):Wrong regular expression: ";
			estr += kreg->String();
			return callfunc->kifcode->Returnerror(estr);
		}
	}
	else {
		const sregex_token_iterator end;
		string str = kstr->String();
		string reg = kreg->String();
		try {
			regex pattern(reg);


			if (contextualpattern->type == kifVector) {
				KifBaseString localstring(NULL, NULL);
				localstring.kifcode = callfunc->kifcode;

				KifElement* vect = Selectvectorstring(callfunc, contextualpattern);
				for (sregex_token_iterator i(str.begin(), str.end(), pattern); i != end; ++i) {
					localstring.value = *i;
					vect->Push(&localstring);
				}
				return vect;
			}

			if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat || contextualpattern->type == kifString) {
				smatch result;
				if (regex_search(str, result, pattern) == true) {
					if (contextualpattern->type == kifString)
						return callfunc->kifcode->Providestringraw(result[0]);
					return callfunc->kifcode->Provideinteger(result.position());
				}
				return kifNULL;
			}

			if (regex_match(str, pattern) == false)
				return kifFALSE;
		}
		catch (...) {
			str = "RGX(002):Wrong regular expression: ";
			str += reg;
			return callfunc->kifcode->Returnerror(str);
		}
	}
	return kifTRUE;
}

KifElement* PreSplitRegex(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {

	KifElement* kstr = callfunc->Evaluatethread(0, dom, idthread);
	KifElement* kreg = callfunc->Evaluatethread(1, dom, idthread);

	if (kstr->type == kifUString) {
		wstring str = kstr->UString();
		wstring reg = kreg->UString();
		try {
			wregex pattern(reg);


			if (contextualpattern->type == kifVector) {
				KifBaseUString localstring(NULL, NULL, 0);
				localstring.kifcode = callfunc->kifcode;

				KifElement* vect = Selectvectorustring(callfunc, contextualpattern);
				match_results<wstring::const_iterator> result;
				if (regex_match(str, result, pattern) == true) {
					for (int i = 1; i < result.size(); i++) {
						localstring.value = result[i];
						vect->Push(&localstring);
					}
				}
				return vect;
			}
		}
		catch (...) {
			string estr = "RGX(003):Wrong regular expression: ";
			estr += kreg->String();
			return callfunc->kifcode->Returnerror(estr);
		}
	}
	else {
		string str = kstr->String();
		string reg = kreg->String();
		try {
			regex pattern(reg);


			if (contextualpattern->type == kifVector) {
				KifBaseString localstring(NULL, NULL);
				localstring.kifcode = callfunc->kifcode;

				KifElement* vect = Selectvectorstring(callfunc, contextualpattern);
				match_results<string::const_iterator> result;
				if (regex_match(str, result, pattern) == true) {
					for (int i = 1; i < result.size(); i++) {
						localstring.value = result[i];
						vect->Push(&localstring);
					}
				}
				return vect;
			}
		}
		catch (...) {
			str = "RGX(003):Wrong regular expression: ";
			str += reg;
			return callfunc->kifcode->Returnerror(str);
		}
	}
	return kifNULL;
}

extern KifGlobalThread* kifGlobals;

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifregex(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}
		kifcode->KifInitLib();

		return true;
	}

	Exporting bool kifregex_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifregex(kifcode, version) == false)
			return false;

		kifGlobals->KifCreatePredefined(kifcode, "regex", P_TWO, &PreRegex);
		kifGlobals->KifCreatePredefined(kifcode, "splitrgx", P_TWO, &PreSplitRegex);
		kifGlobals->KifCreatePredefined(kifcode, "replacergx", P_THREE, &PreReplaceRgx);

		InitMapBooleanString(kifGlobals->kifStringFunctions, "regex");
		InitMapBooleanString(kifGlobals->kifStringFunctions, "splitrgx");
		InitMapBooleanString(kifGlobals->kifStringFunctions, "replacergx");
		kifGlobals->kifReturnString[KifNewId("replacergx")] = true;

		return true;
	}
	Exporting bool kifregex_Terminate(KifCode* kifcode) {
		return true;
	}
}

