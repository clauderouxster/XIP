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
filename   : kifautomaton.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language
Programmer : Claude ROUX
Reviewer   :
*/


#include "kif.h"
#include "kifinit.h"
#include "kifcontainer.h"
#include "kifmaps.h"
#include "fractalhmap.h"
#include "kifpmaps.h"
#include "kifautomaton.h"

//----------------------------------------------------------------------------------------------------

extern const char* kifErrorStrings[];
bool TestkifFunctions(short n, hmap<short, bool>& kiffunc);

KifElement* KifCode::KifParseVarAutomaton(x_node* xn, KifElement* kf) {
	short id = KifNewId(xn->nodes[1]->value);
	KifElement* var = CreateVariable(xn, id, kf, kifNOTYPE);
	KifAutomaton* kv;
	kv = new KifAutomaton(this, var);
	kv->Setline(linereference + GetCurrentLine(0, xn->nodes[1]), kifGlobals->debuginfocurrent->currentfile);
	kf->Set(id, var, NULL);
	return var;
}

//----------------------------------------------------------------------------------------------------
const char kifarcstart = 1;
const char kifarcend = 2;
const char kifarcfeature = 4;
const char kifarcfeaturevalue = 8;
const char kifarcdouble = 16;
const char kifbeginarc = 32;
const char kifendarc = 64;
const char kifendfile = 128;


#ifdef C11
KifContainerLock::KifContainerLock(KifAutomaton* d) {
	g = NULL;
	if (kifGlobals->Test() == true)
		g = new std::lock_guard<std::recursive_mutex>(*d->_locker.lock);

}
#else
KifContainerLock::KifContainerLock(KifAutomaton* l) : _locker(&l->_locker) {
	unlock = false;
	if (kifGlobals != NULL && kifGlobals->Test() == true) {
		unlock = true;
		_locker->Locking();
	}
}
#endif

//adding a word to an automaton

void kifstate::addword(agnostring& w, int pos, KifAutomaton& a) {
	if (pos<w.size()) {
		if (!pos)
			status |= kifarcstart;
		BULONG c = w.nextcode(pos);
		if (c == a.codefeat) {
			status |= kifarcfeature | kifarcend;
			string b = w.substr(pos, w.size() - pos);
			if (a.features.find(b) == a.features.end()) {
				c = a.features.size();
				a.features[b] = c;
				a.ifeatures[c] = b;
			}
			else
				c = a.features[b];
			arcs[c].status |= kifarcfeaturevalue;
		}
		else
			arcs[c].addword(w, pos, a);
	}
	else
		status |= kifarcend;
}

void kifstate::addworddouble(agnostring& w, agnostring& lf, int posw, int posl, KifAutomaton& a) {
	status |= kifarcdouble;
	if (posw<w.size()) {
		if (!posw)
			status |= kifarcstart;
		BULONG cw = w.nextcode(posw);
		BULONG clf = 0;
		//we merge the bits of the two characters as a key
		if (posl<lf.size()) {
			clf = lf.nextcode(posl);
			cw |= clf << 32;
		}
		arcs[cw].addworddouble(w, lf, posw, posl, a);
	}
	else {
		status |= kifarcend;
		if (posl<lf.size()) {
			//if there is a remain, we store it as a feature...
			string b = lf.substr(posl, lf.size() - posl);
			BULONG c;
			if (a.features.find(b) == a.features.end()) {
				c = a.features.size();
				a.features[b] = c;
				a.ifeatures[c] = b;
			}
			else
				c = a.features[b];
			status |= kifarcfeature;
			arcs[c].status |= kifarcfeaturevalue | kifarcdouble;
		}
	}
}


void KifAutomaton::addword(agnostring& w) {
	KifContainerLock _lock(this);
	nbwords++;
	start.addword(w, 0, *this);
}

void KifAutomaton::addworddouble(agnostring& w, agnostring& lf) {
	KifContainerLock _lock(this);
	nbwords++;
	start.addworddouble(w, lf, 0, 0, *this);
}

void KifAutomaton::addWord(agnostring& w) {
	nbwords++;
	start.addword(w, 0, *this);
}

void KifAutomaton::addWorddouble(agnostring& w, agnostring& lf) {
	nbwords++;
	start.addworddouble(w, lf, 0, 0, *this);
}

void kifstate::dump(string upper, string lower, ofstream& dumping, KifAutomaton& a) {
	automap<BULONG, kifstate>::iterator it;
	if ((status&kifarcend) == kifarcend) {
		if ((status&kifarcfeature) == kifarcfeature) {
			agnostring b;
			for (it = arcs.begin(); it != arcs.end(); it++) {
				if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue) {
					if ((it->second.status&kifarcdouble) == kifarcdouble) {
						b = lower + a.ifeatures[it->first];
						dumping << upper << endl;
						dumping << b << endl;
					}
					else {
						b = upper + s_unicodetoutf8(a.codefeat) + a.ifeatures[it->first];
						dumping << b << endl;
					}
				}
			}
		}
		else {
			dumping << upper << endl;
			if ((status&kifarcdouble) == kifarcdouble)
				dumping << lower << endl;
		}
	}

	BULONG uc, lc;
	for (it = arcs.begin(); it != arcs.end(); it++) {
		if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
			continue;

		if ((it->second.status&kifarcdouble) == kifarcdouble) {
			uc = it->first & 0xFFFF;
			lc = it->first >> 32;
		}
		else {
			uc = it->first;
			lc = uc;
		}
		it->second.dump(upper + s_unicodetoutf8(uc), lower + s_unicodetoutf8(lc), dumping, a);
	}
}

void KifAutomaton::dump(string& path) {
	KifContainerLock _lock(this);
	ofstream dumping(STR(path));
	if (dumping.fail()) {
		kifcode->Returnerror("KIF(637): Cannot create file");
		return;
	}

	start.dump("", "", dumping, *this);
}


KifElement* KifAutomaton::setpath(KifMap* m) {
	paths.clear();
	usepaths = false;
	hmap<string, KifElement*>::iterator it;
	hmap<string, long>::iterator itimap;
	hmap<string, double>::iterator itfmap;
	hmap<string, KifElement*>::iterator itm;

	agnostring akey;
	long key;
	for (it = m->values.begin(); it != m->values.end(); it++) {
		if (it->second->aMapContainer() == false) {
			paths.clear();
			return kifcode->Returnerror("KIF(645): The map structure should only contain maps");
		}
		akey = it->first;
		key = akey.code(0);
		if (it->second->type == kifMapInteger) {
			for (itimap = ((KifMapInteger*)it->second)->values.begin(); itimap != ((KifMapInteger*)it->second)->values.end(); itimap++) {
				itimap = ((KifMapInteger*)it->second)->values.begin();
				akey = itimap->first;
				paths[key][akey.code(0)] = itimap->second;
			}
		}
		else {
			if (it->second->type == kifMapFloat) {
				for (itfmap = ((KifMapFloat*)it->second)->values.begin(); itfmap != ((KifMapFloat*)it->second)->values.end(); itfmap++) {
					akey = itfmap->first;
					paths[key][akey.code(0)] = itfmap->second;
				}
			}
			else {
				if (it->second->type == kifMap) {
					for (itm = ((KifMap*)it->second)->values.begin(); itm != ((KifMap*)it->second)->values.end(); itm++) {
						akey = itm->first;
						paths[key][akey.code(0)] = itm->second->Float();
					}
				}
				else {
					paths.clear();
					return kifcode->Returnerror("KIF(647): Cannot process this structure");
				}
			}
		}
	}
	usepaths = true;
	return kifTRUE;
}


bool KifAutomaton::load(string& path, string& featchar) {
	KifContainerLock _lock(this);
	char buff[2048];
	FILE* f = fopen(STR(path), "r");
	if (f == NULL) {
		kifcode->Returnerror("KIF(620): Cannot open file");
		return false;
	}
	pathname = path;
	agnostring s;
	if (featchar.size()>1) {
		kifcode->Returnerror("KIF(622): Feature character should be one single character or the empty string");
		return false;
	}
	if (featchar.size() == 1)
		c_utf8tounicode(USTR(featchar), codefeat);
	while (!feof(f)) {
		fgets(buff, 2048, f);
		s = buff;
		s = s.trim();
		if (s.size())
			addWord(s);
	}
	fclose(f);
	return true;
}

//two lines  to be loaded in pair
bool KifAutomaton::loaddouble(string& path, string& featchar) {
	KifContainerLock _lock(this);
	codefeat = 0;
	if (featchar.size() == 1)
		c_utf8tounicode(USTR(featchar), codefeat);

	char buff[2048];
	FILE* f = fopen(STR(path), "r");
	if (f == NULL)
		return false;
	pathname = path;
	agnostring s;
	agnostring lf;
	while (!feof(f)) {
		buff[0] = 0;
		fgets(buff, 2048, f);
		s = buff;
		s = s.trim();
		if (feof(f)) {
			fclose(f);
			if (s.size()) {
				kifcode->Returnerror("KIF(625): Surface+lemma should be in pairs, one on each line");
				return false;
			}
			return true;
		}
		buff[0] = 0;
		fgets(buff, 2048, f);
		lf = buff;
		lf = lf.trim();
		if (!s.size() || !lf.size()) {
			fclose(f);
			if (!s.size() && !lf.size())
				return true;
			kifcode->Returnerror("KIF(626): Missing value in 'LOADLEMMA'");
			return false;
		}
		addWorddouble(s, lf);
	}
	fclose(f);
	return true;
}



KifElement* KifAutomaton::setcodefeat(string& c) {
	if (codefeat == 0) {
		if (c.size() != 1)
			return kifcode->Returnerror("KIF(623): Feature character should be one single character");
		c_utf8tounicode(USTR(c), codefeat);
		return kifTRUE;
	}
	return kifcode->Returnerror("KIF(624): Clear the automaton first.");
}

KifElement* KifAutomaton::Execute(KifElement* pattern, KifDomain* dom, KifElement* idx) {
	return this;
}

bool KifAutomaton::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom,int idthread,KifCallFunction* callfunc) {
	return true;
}


KifAutomaton::~KifAutomaton() {}

KifElement* KifAutomaton::constructor(KifDomain* domloc, KifElement* params, int idthread, KifCallFunction* callfunc) {
	//If you do not have any parameters, then your function might return whatever you want... Here itself
	if (params->InstructionSize() == 0)
		return this;

	//Your first parameter is at position 0 etc...		
	object = params->Instruction(0); //would match i as in the example above: mynewtype toto(i,j);
	if (object->type == kifInstance)
		object = ((KifInstance*)object)->Thevariable(domloc,idthread, NULL);
	else
		object = object->Exec(kifNULL, domloc,kifNULL,idthread,callfunc);
	object->Setreference();
	return this;
}

#define action_first 1
#define action_change 2
#define action_delete 4
#define action_insert 8
#define action_switch 16
#define action_nocase 32
#define action_surface 64
#define action_full 128
#define action_split 256
#define action_skip 512
#define action_track 1024

#define isaction(a) (flags&a)==a
#define isnotaction(a) (flags&a)==0
#define popback if (isaction(action_track)) rs.actions->pop_back()

KifElement* KifAutomaton::editdistance(KifElement* contextualpattern, KifDomain* dom, KifElement* e, KifElement* kflags, KifElement* threshold) {
	KifContainerLock _lock(this);
	agnostring estr(e->String());
	agnostring lemma;
	agnostring surface;
	KifMapFloat* kfloatres = new KifMapFloat(kifcode, NULL);
	KifVector* splitwords = new KifVector(kifcode, NULL);
	KifMap* kactions = NULL;
	vector<hmap<int, string> >* actions = NULL;
	short flags = kflags->Integer();
	if (isaction(action_track)) {
		actions = new vector<hmap<int, string> >;
		kactions = new KifMap(kifcode, NULL);
	}

	kifstatres* rs = new kifstatres(kfloatres, splitwords, actions, kactions);
	threadowner = kifGlobals->GetThreadid();
	start.editdistance(dom, *this, estr, lemma, surface, 0, 0, threshold->Integer(), flags, *rs);
	delete rs;
	if (actions != NULL) {
		delete actions;
		hmap<string, double>::iterator it;
		vector<KifElement*> v;
		KifVector* kv;
		for (it = kfloatres->values.begin(); it != kfloatres->values.end(); it++) {
			v.clear();
			kv = (KifVector*)kactions->values[it->first];
			for (int i = 0; i<kv->Size(); i++) {
				if (kv->values[i]->Size() <= it->second)
					v.push_back(kv->values[i]);
				else
					kv->values[i]->Resetreference(2);
			}
			kv->values = v;
		}
		splitwords->Push(kactions);
	}
	kfloatres->Release();
	return splitwords;
}

void kifstatres::updateactions(string& k) {
	KifPrimeMapInteger* ke = new KifPrimeMapInteger(kactions->kifcode, NULL);
	hmap<int, string>::iterator it;
	for (int i = 0; i<actions->size(); i++) {
		it = (*actions)[i].begin();
		ke->values[it->second] = it->first;
	}
	KifVector* v;
	if (kactions->values.find(k) == kactions->values.end()) {
		v = new KifVector(kactions->kifcode, NULL);
		v->Setreference();
		kactions->values[k] = v;
	}
	else
		v = (KifVector*)kactions->values[k];

	v->values.push_back(ke);
	ke->Setreference(2);
}


//Flags:
// 1: enable 1st character evaluation
// 2: enable character change
// 4: enable character deletion
// 8: enable character insertion
//16: enable character switch
//32: enable case insensitive
//64: return surface forms
//128: return both lemma and surface forms
//256: split
//512: skip unknown characters.
//1024: keep track of modifications
inline static void addtoaction(int pos, char a, kifstatres& rs, BULONG c, BULONG uc, BULONG ul) {
	hmap<int, string> m;
	string s;
	s = a;
	s += "_";
	if (c != 0)
		s += s_unicodetoutf8(c);
	if (uc != 0) {
		s += ":";
		s += s_unicodetoutf8(uc);
		if (uc != ul) {
			s += "/";
			s += s_unicodetoutf8(ul);
		}
	}
	m[pos] = s;
	rs.actions->push_back(m);
}

bool kifstate::editdistance(KifDomain* d, KifAutomaton& a, agnostring& e,
	agnostring& lemma, agnostring& surface, int pos, double score, int threshold,
	short flags, kifstatres& rs) {
	//First we try gather the right information...
	//The list of all arc labels...
	if (score>threshold)
		return true;
	if (Errorid(a.threadowner))
		return false;

	agnostring blem;
	agnostring bsurf;
	bool pushed = false;
	automap<BULONG, kifstate>::iterator it;

	BULONG uc, ul;
	if (pos<e.size()) {
		double localsc;
		double sc = score + 1;
		int ipos = pos;
		BULONG c = e.nextcode(pos);

		if (isaction(action_split)) {
			if ((status&kifarcend) == kifarcend && surface.size()) {//In this case, we have reached a possible split point... 			
				agnostring ee(e.substr(ipos, e.size() - ipos));
				rs.wrds.push_back(surface + "!");
				a.start.editdistance(d, a, ee, blem, bsurf, 0, score + 1, threshold, flags, rs);
				rs.wrds.pop_back();
				pushed = true;
			}
		}

		//we skip unknown characters
		if ((isaction(action_skip)) && sc <= threshold) {
			bool launch = false;
			//if the character is unknown 
			if (arcs.find(c) == arcs.end()) {
				launch = true;
				if (arcs.size()>1) {
					for (it = arcs.begin(); it != arcs.end(); it++) {
						if ((it->second.status&kifarcdouble) != kifarcdouble)
							break;
						if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
							continue;

						//if it is a transducer, uc is part of the it->first...					
						uc = it->first & 0xFFFF;
						if (c == uc) {
							launch = false;
							break;
						}
					}
				}
			}
			if (launch) {
				//we do not modify the score...				
				if (isaction(action_split)) {
					if (pushed)
						rs.wrds.push_back(surface + "!");
					rs.wrds.push_back(s_unicodetoutf8(c));
					bool ret = editdistance(d, a, e, blem, bsurf, pos, score, threshold, flags, rs);
					if (pushed)
						rs.wrds.pop_back();
					rs.wrds.pop_back();
					if (ret == false)
						return false;
				}
				else {
					if (editdistance(d, a, e, lemma, surface, pos, score, threshold, flags, rs) == false)
						return false;
				}
			}
		}

		//In this case, we do not want the first character to be evaluated
		if (!ipos && isnotaction(action_first)) {
			if (arcs.find(c) != arcs.end()) {
				blem = s_unicodetoutf8(c);
				bsurf = blem;
				return arcs[c].editdistance(d, a, e, blem, bsurf, pos, score, threshold, flags, rs);
			}
			else {
				if (arcs.size()>1) {
					for (it = arcs.begin(); it != arcs.end(); it++) {
						if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
							continue;

						//if it is a trasducer, ul and uc are part of the it->first...
						if ((it->second.status&kifarcdouble) == kifarcdouble) {
							uc = it->first & 0xFFFF;
							ul = it->first >> 32;
						}
						else {
							uc = it->first;
							ul = uc;
						}

						blem = s_unicodetoutf8(ul);
						bsurf = s_unicodetoutf8(uc);
						if (c == uc) {//if it is the same...
							if (it->second.editdistance(d, a, e, blem, bsurf, pos, score, threshold, flags, rs) == false)
								return false;
						}
					}
				}
			}
			return true;
		}

		//we switch two characters next to each other
		if (isaction(action_switch)) {
			flags &= ~action_switch;
			int sz = e.sizec();
			int rpos = ipos;
			ipos = e.bytetocharposition(ipos);
			for (int i = ipos; i<sz - 1; i++) {
				blem = e.extract(ipos, i - ipos);
				blem += e[i + 1] + e[i];
				blem += e.extract(i + 2, sz - i + 2);
				if (isaction(action_track)) {
					int j = i;
					addtoaction(rpos, 's', rs, e.nextcode(j), 0, 0);
				}
				if (editdistance(d, a, blem, lemma, surface, 0, sc, threshold, flags | 1, rs) == false) {
					popback;
					return false;
				}
				popback;
			}
		}

		//we delete it
		if ((isaction(action_delete)) && sc <= threshold) {
			if (isaction(action_track))
				addtoaction(ipos, '~', rs, c, 0, 0);
			if (editdistance(d, a, e, lemma, surface, pos, sc, threshold, flags, rs) == false) {
				popback;
				return false;
			}
			popback;
		}

		for (it = arcs.begin(); it != arcs.end(); it++) {
			if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
				continue;

			//if it is a trasducer, ul and uc are part of the it->first...
			if ((it->second.status&kifarcdouble) == kifarcdouble) {
				uc = it->first & 0xFFFF;
				ul = it->first >> 32;
			}
			else {
				uc = it->first;
				ul = uc;
			}

			blem = lemma + s_unicodetoutf8(ul);
			bsurf = surface + s_unicodetoutf8(uc);

			if (c == uc) {//if it is the same...
				if (it->second.editdistance(d, a, e, blem, bsurf, pos, score, threshold, flags, rs) == false)
					return false;
			}
			else {
				//we change it
				if (ipos || isaction(action_first)) {
					if (sc <= threshold) {
						//no case
						if (isaction(action_nocase) && c_tolower(c) == c_tolower(uc)) {
							if (isaction(action_track)) {
								if (isaction(action_first))
									addtoaction(ipos, 'N', rs, c, uc, ul);
								else
									addtoaction(ipos, 'n', rs, c, uc, ul);
							}

							if (a.usepaths && a.paths.find(c) != a.paths.end() && a.paths[c].find(uc) != a.paths[c].end())
								localsc = score + a.paths[c][uc];
							else
								localsc = score + 0.1;
							if (it->second.editdistance(d, a, e, blem, bsurf, pos, localsc, threshold, flags, rs) == false) {
								popback;
								return false;
							}
							popback;
						}
						else {
							if (a.usepaths && a.paths.find(c) != a.paths.end() && a.paths[c].find(uc) != a.paths[c].end())
								localsc = score + a.paths[c][uc];
							else
								localsc = sc;
							if (isaction(action_change)) {
								if (isaction(action_track)) {
									if (isaction(action_first))
										addtoaction(ipos, 'C', rs, c, uc, ul);
									else
										addtoaction(ipos, 'c', rs, c, uc, ul);
								}
								if (it->second.editdistance(d, a, e, blem, bsurf, pos, localsc, threshold, flags, rs) == false) {
									popback;
									return false;
								}
								popback;
							}
						}
					}
				}
			}
			//we insert it
			if (ipos || isaction(action_first)) {
				if ((isaction(action_insert)) && sc <= threshold) {
					if (isaction(action_track)) {
						if (isaction(action_first))
							addtoaction(ipos, 'I', rs, c, uc, ul);
						else
							addtoaction(ipos, 'i', rs, c, uc, ul);
					}

					if (it->second.editdistance(d, a, e, blem, bsurf, ipos, sc, threshold, flags, rs) == false) {
						popback;
						return false;
					}
					popback;
				}
			}
		}
	}
	else {
		//we might need to insert a character at the end
		if ((isaction(action_insert)) && score<threshold) {
			for (it = arcs.begin(); it != arcs.end(); it++) {
				if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
					continue;
				//if it is a trasducer, ul and uc are part of the it->first...
				if ((it->second.status&kifarcdouble) == kifarcdouble) {
					uc = it->first & 0xFFFF;
					ul = it->first >> 32;
				}
				else {
					ul = it->first;
					uc = ul;
				}

				bsurf = surface + s_unicodetoutf8(uc);
				blem = lemma + s_unicodetoutf8(ul);
				//we insert it
				if (isaction(action_track)) {
					int j = pos;
					addtoaction(pos, 'a', rs, e.nextcode(j), uc, ul);
				}

				if (it->second.editdistance(d, a, e, blem, bsurf, pos, score + 1, threshold, flags, rs) == false) {
					popback;
					return false;
				}
				popback;
			}
		}

		if ((status&kifarcend) == kifarcend) {
			if (lemma.size()) {
				if (isnotaction(action_surface) && (status&kifarcfeature) == kifarcfeature)
					getfeature(a, lemma, surface, rs, score, flags);
				else {
					if (isaction(action_full)) {
						bsurf = surface;
						if (a.codefeat)
							bsurf += s_unicodetoutf8(a.codefeat);
						else
							bsurf += "\t";
						bsurf += lemma;
					}
					else
					if (isaction(action_surface))
						bsurf = surface;
					else
						bsurf = lemma;

					if (rs.kvectres->values.find(bsurf) == rs.kvectres->values.end())
						rs.kvectres->values[bsurf] = score;
					else
					if (rs.kvectres->values[bsurf]>score)
						rs.kvectres->values[bsurf] = score;

					if (rs.kactions != NULL)
						rs.updateactions(bsurf);
				}

				if (isaction(action_split)) {
					KifVectorString* splits = new KifVectorString(d->kifcode, NULL);
					splits->values = rs.wrds;
					splits->values.push_back(surface + "!");
					rs.splitwords->Push(splits);
				}
				else {
					if (rs.splitwords->values.size() == 0 && rs.kactions == NULL) {
						if (rs.kvectres->values.size())
							rs.splitwords->values.push_back(rs.kvectres);
						rs.kvectres->Setreference(rs.splitwords->reference + 1);
					}
				}
			}
		}
	}

	return true;
}


void kifstate::getfeature(KifAutomaton& a, string& lemma, string& surface, kifstatres& rs, double score, short flags) {
	string b;
	automap<BULONG, kifstate>::iterator it;
	for (it = arcs.begin(); it != arcs.end(); it++) {
		if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue) {
			if ((it->second.status&kifarcdouble) == kifarcdouble)
				b = lemma + a.ifeatures[it->first];
			else
				b = lemma + s_unicodetoutf8(a.codefeat) + a.ifeatures[it->first];

			if (isaction(action_full)) {
				string bsurf = surface;
				if (a.codefeat)
					bsurf += s_unicodetoutf8(a.codefeat);
				else
					bsurf += "\t";
				bsurf += b;
				b = bsurf;
			}

			if (rs.kvectres->values.find(b) == rs.kvectres->values.end())
				rs.kvectres->values[b] = score;
			else
			if (rs.kvectres->values[b]>score)
				rs.kvectres->values[b] = score;
			if (rs.kactions != NULL)
				rs.updateactions(b);
		}
	}
}


KifElement* KifAutomaton::evaluate(KifElement* contextualpattern, KifDomain* dom, KifElement* e, KifElement* o) {
	KifContainerLock _lock(this);
	if (function == NULL)
		return kifcode->Returnerror("KIF(634): Missing function, cannot look up");
	KifFunction* kfunc = function->Functioncall(dom);
	KifCallFunctionLocal kcfunc(kfunc->kifcode, kfunc->Name(), kfunc);
	agnostring estr(e->String());
	agnostring base;
	KifElement* obj = object;
	if (o != NULL)
		obj = o;
	KifMap* kvectres = new KifMap(kifcode, NULL);
	threadowner = kifGlobals->GetThreadid();
	vector<hmap<string, string> > actions;
	start.evaluate(dom, *this, kcfunc, actions, obj, estr, base, 0, 0, kvectres);
	return kvectres;
}

bool kifstate::evaluate(KifDomain* d, KifAutomaton& a, KifCallFunctionLocal& kfunc,
	vector<hmap<string, string> >& actions, KifElement* object, agnostring& e,
	agnostring& base, int pos, double score, KifMap* kvectres) {

	//First we try gather the right information...
	//The list of all arc labels...
	if (Errorid(a.threadowner))
		return false;

	agnostring b;
	BULONG ul, uc;
	BLONG tc;
	bool end = false;

	if (pos >= e.size())
		end = true;

	int ipos = pos;


	KifElement* kmap;
	long ch = 0xFFFF;

	automap<BULONG, kifstate>::iterator it;
	if (kfunc.body->arguments.size() == 7) {
		string lc;
		KifVectorString kvectstring(NULL, NULL);
		kvectstring.kifcode = d->kifcode;
		kvectstring.reference = 1;
		hmap<string, string>::iterator itact;
		for (ul = 0; ul<actions.size(); ul++) {
			itact = actions[ul].begin();
			if (itact->first != itact->second)
				kvectstring.values.push_back(itact->second);
		}

		KifBaseFloat kscore(NULL, NULL, score);
		kscore.kifcode = d->kifcode;
		kscore.reference = 1;

		KifVectorString kvect(NULL, NULL);
		kvect.kifcode = d->kifcode;
		kvect.reference = 1;

		for (it = arcs.begin(); it != arcs.end(); it++) {
			if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
				continue;
			//if it is a trasducer, ul and uc are part of the it->first...
			if ((it->second.status&kifarcdouble) == kifarcdouble)
				uc = it->first & 0xFFFF;
			else
				uc = it->first;

			lc = s_unicodetoutf8(uc);
			if (a.spotfinalstate) {
				if ((it->second.status&kifarcend) == kifarcend)
					lc += '$';
			}
			kvect.values.push_back(lc);
		}

		if (!end)
			ch = e.bytetocharposition(pos);

		KifBaseInteger kpos(NULL, NULL, ch);
		kpos.kifcode = d->kifcode;
		kpos.reference = 1;
		KifBaseString kcurrent(NULL, NULL, base);
		kcurrent.kifcode = d->kifcode;
		kcurrent.reference = 1;

		if (!end)
			ch = e.nextcode(pos);

		KifBaseString bs(NULL, NULL, e);
		bs.kifcode = d->kifcode;
		bs.reference = 1;

		kfunc.parameters.clear();

		kfunc.parameters.push_back(&bs);
		kfunc.parameters.push_back(&kcurrent);
		kfunc.parameters.push_back(&kpos);
		kfunc.parameters.push_back(&kvect);
		kfunc.parameters.push_back(&kvectstring);
		kfunc.parameters.push_back(&kscore);
		kfunc.parameters.push_back(object);
		kmap = kfunc.Execute(kifNULL, d);
	}
	else {
		KifBaseFloat kscore(NULL, NULL, score);
		kscore.kifcode = d->kifcode;
		kscore.reference = 1;

		if (!pos)
			ch = e.nextcode(pos)*-1;
		else
		if (!end)
			ch = e.nextcode(pos);

		KifBaseInteger bs(NULL, NULL, ch);
		bs.kifcode = d->kifcode;
		bs.reference = 1;

		KifVectorInteger kvect(NULL, NULL);
		kvect.kifcode = d->kifcode;
		kvect.reference = 1;

		for (it = arcs.begin(); it != arcs.end(); it++) {
			if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
				continue;
			//if it is a trasducer, ul and uc are part of the it->first...
			if ((it->second.status&kifarcdouble) == kifarcdouble)
				tc = it->first & 0xFFFF;
			else
				tc = it->first;

			if (a.spotfinalstate) {
				if ((it->second.status&kifarcend) == kifarcend)
					tc *= -1;
			}

			kvect.values.push_back(tc);
		}

		kfunc.parameters.clear();
		kfunc.parameters.push_back(&bs);
		kfunc.parameters.push_back(&kvect);
		kfunc.parameters.push_back(&kscore);
		kmap = kfunc.Execute(kifNULL, d);
	}

	if (kmap->type != kifMapFloat && kmap->type != kifVector) {
		d->kifcode->Returnerror("KIF(621): expected value should be a 'fmap' or a 'vector of vectors'");
		kmap->Release();
		return false;
	}

	if (kmap->Size() == 0) {
		kmap->Release();
		return true;
	}

	KifEvaluate* keval = (KifEvaluate*)kmap;
	vector<string>  svalues;
	vector<double>  dvalues;

	BULONG c;
	{
		KifContainerLock _lock(keval);
		if (kmap->type == kifMapFloat) {
			hmap<string, double>& hvalues = ((KifMapFloat*)keval)->values;
			hmap<string, double>::iterator itm;
			for (itm = hvalues.begin(); itm != hvalues.end(); itm++) {
				svalues.push_back(itm->first);
				dvalues.push_back(itm->second);
			}
		}
		else {
			//it should be a vector of vector...
			vector<KifElement*>& vvalues = ((KifVector*)keval)->values;
			for (int i = 0; i<vvalues.size(); i++) {
				if (!vvalues[i]->aVector() || vvalues[i]->Size() != 2) {
					d->kifcode->Returnerror("KIF(621): expected value should be a 'fmap' or a 'vector of vectors'");
					kmap->Release();
					return false;
				}
				svalues.push_back(vvalues[i]->ValueOnIndex(0)->String());
				dvalues.push_back(vvalues[i]->ValueOnIndex(1)->Float());
			}
		}
	}

	kmap->Release();

	if (end) {
		if ((status&kifarcend) == kifarcend) {
			KifVector* kvect = new KifVector(d->kifcode, NULL);
			for (ul = 0; ul<actions.size(); ul++) {
				KifMapString* kmapstr = new KifMapString(d->kifcode, NULL);
				kmapstr->values = actions[ul];
				kvect->Push(kmapstr);
			}

			kvect->Push(d->kifcode->Providefloat(score));

			if (base.size()) {
				if ((status&kifarcfeature) == kifarcfeature)
					getfeature(a, base, kvectres, kvect);
				else  {
					if (kvectres->values.find(base) == kvectres->values.end()) {
						KifVector* kvectsup = new KifVector(d->kifcode, NULL);
						kvectsup->Push(kvect);
						kvectres->Push(base, kvectsup);
					}
					else
						kvectres->values[base]->Push(kvect);
				}
			}
			kvect->Release();
		}
	}

	char sch[20];
	if (ch == 0xFFFF) {
		sch[0] = '$';
		sch[1] = 0;
	}
	else {
		if (ch<0)
			ch *= -1;
		s_unicodetoutf8(ch, sch);
	}

	//the map should contain all the necessary arcs, with the right scores...
	//we need then to traverse the whole automaton on the basis of the codes, which should be found in the automaton...
	//If the code is 0, then it is an epsilon and it means that the current character should be eliminated
	//If the code is negative, then it is an inserted character.
	for (int itm = 0; itm<dvalues.size(); itm++) {
		if (svalues[itm].size() == 0)
			continue;
		//the epsilons
		bool end = false;
		hmap<string, string> action;
		action[sch] = svalues[itm];
		actions.push_back(action);
		if (svalues[itm][0] == '~') {
			if (svalues[itm] == "~") {//we delete the current value
				//then we try again with the current state but with the next character												
				if (evaluate(d, a, kfunc, actions, object, e, base, pos, dvalues[itm], kvectres) == false)
					return false;
			}
			else {//the we insert a character into our string, this character is the next one after the ~
				string r = svalues[itm].substr(1, svalues[itm].size() - 1);
				c_utf8tounicode(USTR(r), c);
				if ((status&kifarcdouble) != kifarcdouble) {
					b = base + r;
					if (arcs[c].evaluate(d, a, kfunc, actions, object, e, b, ipos, dvalues[itm], kvectres) == false)
						return false;
				}
				else {
					for (it = arcs.begin(); it != arcs.end(); it++) {
						if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
							continue;
						//if it is a trasducer, ul and uc are part of the it->first...
						if ((it->second.status&kifarcdouble) == kifarcdouble) {
							uc = it->first & 0xFFFF;
							ul = it->first >> 32;
							if (uc != c)
								continue;
							b = base + s_unicodetoutf8(ul);
						}
						if (it->second.evaluate(d, a, kfunc, actions, object, e, b, ipos, dvalues[itm], kvectres) == false)
							return false;
					}
				}
			}
		}
		else {//regular character
			c_utf8tounicode(USTR(svalues[itm]), c);
			if ((status&kifarcdouble) != kifarcdouble) {
				b = base + svalues[itm];
				if (arcs[c].evaluate(d, a, kfunc, actions, object, e, b, pos, dvalues[itm], kvectres) == false)
					return false;
			}
			else {
				for (it = arcs.begin(); it != arcs.end(); it++) {
					if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
						continue;
					//if it is a trasducer, ul and uc are part of the it->first...
					if ((it->second.status&kifarcdouble) == kifarcdouble) {
						uc = it->first & 0xFFFF;
						ul = it->first >> 32;
						if (uc != c)
							continue;
						b = base + s_unicodetoutf8(ul);
					}
					if (it->second.evaluate(d, a, kfunc, actions, object, e, b, pos, dvalues[itm], kvectres) == false)
						return false;
				}
			}
		}
		actions.pop_back();
	}
	return true;
}


void kifstate::getfeature(KifAutomaton& a, string base, KifMap* kvectres, KifVector* score) {
	string b;
	automap<BULONG, kifstate>::iterator it;
	for (it = arcs.begin(); it != arcs.end(); it++) {
		if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue) {
			if ((it->second.status&kifarcdouble) == kifarcdouble)
				b = base + a.ifeatures[it->first];
			else
				b = base + s_unicodetoutf8(a.codefeat) + a.ifeatures[it->first];

			if (kvectres->values.find(b) == kvectres->values.end()) {
				KifVector* kvectsup = new KifVector(score->kifcode, NULL);
				kvectsup->Push(score);
				kvectres->Push(b, kvectsup);
			}
			else
				kvectres->values[b]->Push(score);
		}
	}
}

KifElement* KifAutomaton::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx) {
	KifContainerLock _lock(this);

	if (b->type = kifAutomaton) {
		int pos = 0;
		agnostring s(a->String());
		if (pattern->aContainer() || pattern->type == kifString) {
			int nb = -1;
			if (pattern->type == kifString)
				nb = 1;
			agnostring base;
			vector<string> v;
			start.search(s, base, pos, *this, v, nb);
			if (pattern->aContainer()) {
				KifElement* vs = Selectvectorstring(this, pattern);
				if (vs->type == kifVectorString) {
					KifContainerLock _lock((KifEvaluate*)vs);
					((KifVectorString*)vs)->values = v;
				}
				else {
					for (int i = 0; i<v.size(); i++)
						vs->Push(kifcode->Providestring(v[i]));
				}
				return vs;
			}
			if (v.size())
				return kifcode->Providestring(v[0]);
			return kifNULL;
		}
		else {
			if (start.search(s, pos) == true)
				return kifTRUE;
			if (idx == true)
				return kifMINUSONE;
		}
		return kifFALSE;
	}

	if (b != this)
		return b->in(a, b, kfunc, pattern, true);
	if (idx == true)
		return kifMINUSONE;
	return kifFALSE;
}

KifElement* KifAutomaton::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifAutomatonFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifAutomaton::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifAutomatonFunctions.begin(); it != kifGlobals->kifAutomatonFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

KifElement* KifAutomaton::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationsAutomaton.find(s) == kifGlobals->kifInformationsAutomaton.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsAutomaton[s]);
}

bool kifstate::search(agnostring& e, int pos) {
	//First we try gather the right information...
	//The list of all arc labels...
	if (pos<e.size()) {
		BULONG c = e.nextcode(pos);
		if ((status&kifarcdouble) == kifarcdouble) {
			automap<BULONG, kifstate>::iterator it;
			for (it = arcs.begin(); it != arcs.end(); it++) {
				if ((it->first & 0xFFFF) == c) {
					if (it->second.search(e, pos))
						return true;
				}
			}
			return false;
		}
		if (arcs.find(c) == arcs.end())
			return false;
		return arcs[c].search(e, pos);
	}
	else
	if ((status&kifarcend) == kifarcend)
		return true;
	return false;
}

void kifstate::search(agnostring& e, string base, int pos, KifAutomaton& a, vector<string>& v, int nb) {
	if (nb != -1 && v.size() >= nb)
		return;
	//First we try gather the right information...
	//The list of all arc labels...
	if (pos<e.size()) {
		BULONG c = e.nextcode(pos);
		if ((status&kifarcdouble) == kifarcdouble) {
			automap<BULONG, kifstate>::iterator it;
			for (it = arcs.begin(); it != arcs.end(); it++) {
				if ((it->first & 0xFFFF) == c)
					it->second.search(e, base + s_unicodetoutf8(it->first >> 32), pos, a, v, nb);
			}
			return;
		}
		if (arcs.find(c) == arcs.end())
			return;
		arcs[c].search(e, base + s_unicodetoutf8(c), pos, a, v, nb);
	}
	else {
		if ((status&kifarcend) == kifarcend) {
			if ((status&kifarcfeature) == kifarcfeature) {
				string b;
				automap<BULONG, kifstate>::iterator it;
				for (it = arcs.begin(); it != arcs.end(); it++) {
					if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue) {
						if ((it->second.status&kifarcdouble) == kifarcdouble)
							b = base + a.ifeatures[it->first];
						else
							b = base + s_unicodetoutf8(a.codefeat) + a.ifeatures[it->first];
						v.push_back(b);
					}
				}
			}
			else
				v.push_back(base);
		}
	}
}

struct valarc {
	unsigned char c;
	unsigned short v;
};

union onearc {
	valarc val;
	unsigned char ustr[sizeof(valarc)];
	onearc() {
		memset(ustr, 0, sizeof(valarc));
	}

	void clear() {
		memset(ustr, 0, 4);
	}

	unsigned char* str() {
		//check the padding
		if (val.v == 0)
			ustr[0] |= 32;
		else {
			char buff[5];
			memcpy(buff, ustr, 4);
			ustr[1] = ustr[2];
			ustr[2] = ustr[3];
			if (!ustr[1]) {
				ustr[0] |= 64;
				ustr[1] = ustr[2];
				ustr[2] = 0;
				ustr[3] = 0;
			}
			else
			if (!ustr[3])
				ustr[0] |= 128;
			else
				ustr[3] = 0;
		}
		return ustr;
	}
};

//------------------------------------------------------------------------------
void kifstate::store(ofstream& dumping, map<BULONG, short>& alphabet) {
	onearc arc;
	arc.val.c = kifbeginarc;
	dumping << arc.ustr;
	if ((status&kifarcend) == kifarcend) {
		if ((status&kifarcfeature) == kifarcfeature) {
			automap<BULONG, kifstate>::iterator it;
			for (it = arcs.begin(); it != arcs.end(); it++) {
				if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue) {
					arc.clear();
					arc.val.c = it->second.status;
					arc.val.v = it->first;
					dumping << arc.str();
				}
			}
		}
	}

	automap<BULONG, kifstate>::iterator it;
	for (it = arcs.begin(); it != arcs.end(); it++) {
		if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
			continue;
		arc.clear();
		arc.val.c = it->second.status;
		arc.val.v = alphabet[it->first];
		dumping << arc.str();
		it->second.store(dumping, alphabet);
	}
	arc.clear();
	arc.val.c = kifendarc;
	dumping << arc.ustr;
}

void kifstate::getalphabet(map<BULONG, short>& alphabet) {
	automap<BULONG, kifstate>::iterator it;
	short sz;
	for (it = arcs.begin(); it != arcs.end(); it++) {
		if ((it->second.status&kifarcfeaturevalue) == kifarcfeaturevalue)
			continue;

		if (alphabet.find(it->first) == alphabet.end()) {
			sz = alphabet.size() + 1;
			alphabet[it->first] = sz;
		}
		it->second.getalphabet(alphabet);
	}
}


Exported KifElement* KifAutomaton::Store(KifElement* kpath, KifDomain* dom) {
	if (kpath == NULL)
		return kifcode->Returnerror("KIF(635): Missing file name");

	KifContainerLock _lock(this);
	string path = kpath->String();
	ofstream dumping(STR(path), ios::binary);
	if (dumping.fail())
		return kifcode->Returnerror("KIF(637): Cannot create file");
	map<BULONG, short> alphabet;
	start.getalphabet(alphabet);
	if (alphabet.size() >= 4095)
		return kifcode->Returnerror("KIF(636): Cannot store in compact format, number of symbols too large");
	if (codefeat != 0)
		dumping << "codefeat" << "\n" << codefeat << "\n";
	dumping << "alphabet" << "\n";
	map<BULONG, short>::iterator it;
	for (it = alphabet.begin(); it != alphabet.end(); it++)
		dumping << it->second << "\t" << it->first << "\n";
	dumping << "features" << "\n";
	hmap<int, string>::iterator itr;
	for (itr = ifeatures.begin(); itr != ifeatures.end(); itr++)
		dumping << itr->first << "\t" << itr->second << "\n";
	dumping << "arcs" << "\n";
	onearc a;
	a.val.c = start.status;
	dumping << a.str();
	start.store(dumping, alphabet);
	a.clear();
	a.val.c = kifendfile;
	dumping << a.ustr;
	return kifTRUE;
}

bool readonearc(FILE* f, uchar& st, short& v) {
	if (feof(f))
		return false;
	unsigned char buff[sizeof(valarc)+1];
	memset(buff, 0, sizeof(valarc)+1);
	int c = fgetc(f);
	buff[0] = (unsigned char)c;
	if (buff[0] == kifbeginarc || buff[0] == kifendarc || buff[0] == kifendfile) {
		st = buff[0];
		v = 0;
		return true;
	}
	onearc a;

	switch (buff[0] & 224) {
	case 32:
		buff[0] &= ~32;
		break;
	case 64:
		c = fgetc(f);
		buff[3] = (unsigned char)c;
		buff[0] &= ~64;
		break;
	case 128:
		c = fgetc(f);
		buff[2] = (unsigned char)c;
		//we remove the indicative bit
		buff[0] &= ~128;
		break;
	default:
		c = fgetc(f);
		buff[2] = (unsigned char)c;
		c = fgetc(f);
		buff[3] = (unsigned char)c;
	}


	memcpy(a.ustr, buff, 4);
	st = a.val.c;
	v = a.val.v;
	return true;
}

bool kifstate::loadcompact(FILE* f, map<short, BULONG>& alphabet, short& v, unsigned char& st) {
	if (!readonearc(f, st, v))
		return false;
	if (st == kifendfile)
		return false;

	BULONG arc;
	if (st == kifbeginarc) {
		short vv = v;
		unsigned char stt = st;
		if (!readonearc(f, stt, vv))
			return false;
		while (stt != kifendarc && stt != kifendfile) {
			if ((stt&kifarcfeaturevalue) == kifarcfeaturevalue)
				arc = vv;
			else
				arc = alphabet[vv];
			arcs[arc].status = stt;
			if (!arcs[arc].loadcompact(f, alphabet, vv, stt))
				return false;
		}
		if (!readonearc(f, st, v))
			return false;
	}
	return true;
}

void trimming(char* buff) {
	int sz = strlen(buff) - 1;
	while (buff[sz] == 13 || buff[sz] == 10) sz--;
	buff[sz + 1] = 0;
}

KifElement* KifAutomaton::loadcompact(string path, string& featchar) {
	KifContainerLock _lock(this);
	codefeat = 0;

	if (featchar.size() == 1)
		c_utf8tounicode(USTR(featchar), codefeat);

	start.clear();
	nbwords = 0;
	ifeatures.clear();
	features.clear();
	paths.clear();
	usepaths = false;
	char buff[2048];
	FILE* f = fopen(STR(path), "rb");
	if (f == NULL)
		return kifcode->Returnerror("KIF(620): Cannot open file");

	map<short, BULONG> alphabet;
	//first we load the stuff...
	fgets(buff, 2048, f);
	trimming(buff);
	if (!strcmp(buff, "codefeat")) {
		fgets(buff, 2048, f);
		codefeat = atol(buff);
		fgets(buff, 2048, f);
		trimming(buff);
	}
	char* pt;
	BULONG l;
	short i;
	if (!strcmp(buff, "alphabet")) {
		fgets(buff, 2048, f);
		trimming(buff);
		while (!feof(f) && strcmp(buff, "features")) {
			pt = strchr(buff, '\t');
			if (pt != NULL) {
				*pt = 0;
				i = atoi(buff);
				l = s_XLConvert(pt + 1);
				alphabet[i] = l;
			}
			fgets(buff, 2048, f);
			trimming(buff);
		}
		if (!strcmp(buff, "features")) {
			fgets(buff, 2048, f);
			trimming(buff);
			while (!feof(f) && strcmp(buff, "arcs")) {
				pt = strchr(buff, '\t');
				if (pt != NULL) {
					*pt = 0;
					i = atoi(buff);
					ifeatures[i] = pt + 1;
				}
				fgets(buff, 2048, f);
				trimming(buff);
			}
			if (!strcmp(buff, "arcs")) {
				short v;
				uchar st;
				readonearc(f, st, v);
				start.status |= st;
				start.loadcompact(f, alphabet, v, st);
				return kifTRUE;
			}
		}
	}
	return kifcode->Returnerror("KIF(637): Cannot load compact format");
}

