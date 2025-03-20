#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
using namespace std;

#include "conversion.h"
#include "kifbinmap.h"

class Xip_Automaton;

const char xfarcend = 1;
const char xfepsilonword = 2;
const char xfepsilonlem = 4;
const char xfshort = 8;
const char xffull = 16;
const char xflast = 32;

#define hashincrement 1

template <class Z> class arc_map;

template<class Z> class arc_iter : public std::iterator<std::forward_iterator_tag, Z> {
public:
	arc_map<Z>* element;
	Z second;
	unsigned long first;
	int idx;

public:


	arc_iter<Z>() {
		element = NULL;
	}

	arc_iter<Z>(arc_map<Z>* n) {
		element = n;
		idx = 0;
	}


	arc_iter<Z>* operator->()  {
		return this;
	}

	arc_iter<Z>& operator++(int) {
		if (element == NULL || idx >= element->nb)
			element = NULL;
		else {
			first = element->indexes[idx];
			second = element->table[idx];
			idx++;
		}
		return *this;
	}

	friend bool operator!=(arc_iter<Z> a, arc_iter<Z> b) {
		if (a.element == b.element)
			return false;
		return true;
	}

	friend bool operator==(arc_iter<Z> a, arc_iter<Z> b) {
		if (a.element == b.element)
			return true;
		return false;
	}
};

template <class Z> class arc_map {
public:

	Z* table;
	unsigned long* indexes;
	unsigned char sz, nb, last;	

	arc_map<Z>() {
		sz = hashincrement;
		table = new Z[sz];
		memset(table, NULL, sz*sizeof(Z));
		indexes = new unsigned long[sz];
		memset(indexes, 0, sz*sizeof(unsigned long));
		nb = 0;
		last = 255;
	}

	~arc_map<Z>() {
		delete[] table;
		delete[] indexes;
	}

	void redimension(unsigned char sze) {
		if (sz >= sze)
			return;
		delete[] table;
		delete[] indexes;
		sz = sze;
		table = new Z[sz];
		memset(table, NULL, sz*sizeof(Z));
		indexes = new unsigned long[sz];
		memset(indexes, 0, sz*sizeof(unsigned long));
	}

	void resize(unsigned char sze) {
		if (sz >= sze)
			return;

		Z* ntable = new Z[sze];
		memset(ntable, NULL, sze*sizeof(Z));
		unsigned long* nindexes = new unsigned long[sze];
		memset(nindexes, 0, sze*sizeof(unsigned long));

		memcpy(ntable, table, sz*sizeof(Z));
		memcpy(nindexes, indexes, sz*sizeof(unsigned long));
		delete[] table;
		delete[] indexes;
		table = ntable;
		indexes = nindexes;
		sz = sze;
	}

	typedef arc_iter<Z> iterator;
	iterator begin(){ return iterator(this); }
	iterator end() {
		return iterator();
	}

	bool find(unsigned long r) {
		for (int i = 0; i < nb; i++) {
			if (indexes[i] == r) {
				last = i;
				return true;
			}
		}
		last = 255;
		return false;
	}

	Z& found(int i) {
		return table[i];
	}

	unsigned long code(int i) {
		return (indexes[i] >> 16);
	}

	bool check(unsigned long r,int& l) {
		unsigned long v;
		int i = l + 1;
		for (; i < nb; i++) {
			v = indexes[i] & 0xFFFF;
			if (v == r) {
				l = i;
				return true;
			}
		}
		return false;
	}

	void add(unsigned long r, Z v) {
		if (nb < sz) {
			indexes[nb] = r;
			table[nb] = v;
			nb++;
			return;
		}

		Z* ntable = new Z[sz + hashincrement];
		unsigned long* nindexes = new unsigned long[sz + hashincrement];
		memset(ntable, NULL, (sz + hashincrement)*sizeof(Z));
		memset(nindexes, 0, (sz + hashincrement)*sizeof(unsigned long));
		memcpy(ntable, table, sz*sizeof(Z));
		memcpy(nindexes, indexes, sz*sizeof(unsigned long));
		sz += hashincrement;
		delete[] table;
		delete[] indexes;
		table = ntable;
		indexes = nindexes;
		indexes[nb] = r;
		table[nb] = v;
		nb++;
		return;
	}

	Z& operator [](unsigned long r) {
		if (last != 255 && indexes[last] == r)
			return table[last];

		for (int i = 0; i < nb; i++) {
			if (indexes[i] == r)
				return table[i];
		}

		if (nb < sz) {
			indexes[nb] = r;
			return table[nb++];
		}

		Z* ntable = new Z[sz + hashincrement];
		unsigned long* nindexes = new unsigned long[sz + hashincrement];
		memset(ntable, NULL, (sz + hashincrement)*sizeof(Z));
		memset(nindexes, 0, (sz + hashincrement)*sizeof(unsigned long));
		memcpy(ntable, table, sz*sizeof(Z));
		memcpy(nindexes, indexes, sz*sizeof(unsigned long));
		sz += hashincrement;
		delete[] table;
		delete[] indexes;
		table = ntable;
		indexes = nindexes;
		indexes[nb] = r;
		return table[nb++];
	}

	int size() {
		return nb;
	}
};


class parkour_context {
public:
	int bbegin;
	int cbegin;
	int bend;
	int cend;

	char buff[100];

	parkour_context()  {
		bbegin = bend = 0;
		cbegin = cend = 0;
	}

	void init(agnostring& w)  {
		bbegin = bend = w.bytepos;
		cbegin = cend = w.charpos;
	}

	void eset(agnostring& w) {
		if (cend < w.charpos) {
			cend = w.charpos;
			bend = w.bytepos;
		}
	}

	void eset(int b, int c) {
		bend = b;
		cend = c;
	}

	string extract(agnostring& w) {		
		return w.extract(bbegin, bend - bbegin);
	}

	string offsets() {
		sprintf_s(buff, 100, "+%d+%d+%d+%d", bbegin, bend, cbegin, cend);
		return buff;
	}

};

class Xip_Fst {
public:
	unsigned char status;
	arc_map<Xip_Fst*> arcs;
	int id;

	Xip_Fst(Xip_Automaton& a);

	Xip_Fst(wstring& w, wstring& lf, int posw, int posl, Xip_Automaton& a);

	void add(wstring& w, wstring& lf, int posw, int posl, Xip_Automaton& a);
	void regulars(Xip_Automaton& a);
	bool parse(Xip_Automaton& a, vector<string>& vs, int& i, vector<unsigned long>& indexes);
	bool parse(Xip_Automaton& a, agnostring& e, vector<unsigned long>& indexes);
	bool loadtext(string name, Xip_Automaton& a);
	bool load(string name, Xip_Automaton& a);
	void loadarcs(ifstream& dump, Xip_Automaton& a, bool);
	bool compile(string name, Xip_Automaton& a);
	void traverse(ofstream& dump);

	bool check(agnostring& w, Xip_Automaton& a);
	bool up(agnostring& w, string& s, vector<string>& res, Xip_Automaton& a);
	bool tokenize(agnostring& w, string& s, parkour_context& k, vector<string>& res, Xip_Automaton& a);
};

class Xip_Automaton {
public:
	vector<Xip_Fst*> garbage;

	unordered_map<string, unsigned short> alphabet;
	unordered_map<unsigned short, string> ialphabet;

	unordered_map<unsigned long, Xip_Fst*> features;
	hash_bin<unsigned short, unsigned short> encoding;


	Xip_Fst start;

	Xip_Automaton() : start(*this) {
		ialphabet[0] = "";
	}

	~Xip_Automaton() {
		//we cannot delete the first element, which is defined here...
		for (int i = 1; i < garbage.size(); i++) {
			if (garbage[i] != NULL)
				delete garbage[i];
		}
	}

	void fillencoding(bool add) {
		unordered_map<string, unsigned short>::iterator it;
		for (it = alphabet.begin(); it != alphabet.end(); it++) {
			agnostring s(it->first);
			if (s.sizec() == 1) {
				ushort p = s.nextcode();
				encoding[p] = it->second;
			}
			else {
				if (add) {
					unsigned long p = it->second << 16;
					features[p] = new Xip_Fst(*this);
					features[p]->status |= xfarcend;
				}
			}
		}
	}


	Xip_Fst* addfeature(unsigned long p, Xip_Fst* a = NULL) {
		if (features.find(p) == features.end()) {
			if (a == NULL)
				features[p] = new Xip_Fst(*this);
			else
				features[p] = a;

			features[p]->status |= xfarcend;
		}
		return features[p];
	}

	void regulars() {
		start.regulars(*this);
	}

	int index(string& s) {
		if (alphabet.find(s) == alphabet.end()) {
			//the 300 is a horrible hack, but it works with European languages...
			unsigned long fpos = 1 + alphabet.size();
			alphabet[s] = fpos;
			ialphabet[fpos] = s;
			return fpos;
		}

		return alphabet[s];
	}

	int index(unsigned long c) {
		return index(s_unicodetoutf8(c));
	}

	int code(unsigned long c) {
		if (encoding.check(c))
			return encoding.search(c);
		return 0;
	}

	bool load(string name) {
		return start.load(name, *this);
	}

	bool loadtext(string name) {
		return start.loadtext(name, *this);
	}

	bool compile(string name) {
		if (loadtext(name) == false)
			return false;
		name += ".cpt";
		return start.compile(name, *this);		
	}

	bool check(agnostring w) {
		w.begin();
		return start.check(w, *this);
	}

	bool up(agnostring w, vector<string>& res) {
		w.begin();
		res.clear();
		string s;
		return start.up(w, s, res, *this);
	}

	void tokenize(agnostring& w, parkour_context& pkc, vector<string>& res) {
		res.clear();
		string s;				
		if (!start.tokenize(w, s, pkc, res, *this)) {
			w.setpos(pkc.bbegin, pkc.cbegin);
			pkc.eset(pkc.bbegin, pkc.cbegin);

			BULONG cr = w.nextcode();
			if (c_is_punctuation(cr)) {
				pkc.eset(w);
				s = w.extract(pkc.bbegin, pkc.bend - pkc.bbegin);
				s += "\t+Punct+Guessed";
			}
			else {
				while (!w.end() && !c_is_space(cr) && !c_is_punctuation(cr)) {
					pkc.eset(w);
					cr = w.nextcode();
				}
				s = w.extract(pkc.bbegin, pkc.bend - pkc.bbegin);
				s += "\t+Noun+Guessed";
			}
			res.push_back(s);
		}
		w.setpos(pkc.bend, pkc.cend);
	}

};

Xip_Fst::Xip_Fst(Xip_Automaton& a) {
	id = a.garbage.size();
	a.garbage.push_back(this);
	status = 0;
}

Xip_Fst::Xip_Fst(wstring& w, wstring& lf, int posw, int posl, Xip_Automaton& a) {
	id = a.garbage.size();
	a.garbage.push_back(this);
	status = 0;
	add(w, lf, posw, posl, a);
}

union octet4 {
	unsigned long v;
	char b[4];
	octet4() {
		v = 0;
	}
};

union octet2 {
	short v;
	char b[2];

	octet2() {
		v = 0;
	}
};

void Xip_Fst::loadarcs(ifstream& dump, Xip_Automaton& a, bool top) {
	char buff[2];
	dump.read(buff, 1);
	status = buff[0];

	if (status & xflast)
		return;

	dump.read(buff, 2);
	arcs.nb = buff[0];

	arcs.redimension(buff[1]);

	octet4 v4;
	octet2 v2;
	for (int i = 0; i < arcs.nb; i++) {
		if (top)
			cerr << ".";
		if (status&xfshort) {
			dump.read(v2.b, 2);
			arcs.indexes[i] = (v2.v << 16) | v2.v;
		}
		else {
			dump.read(v4.b, 4);
			arcs.indexes[i] = v4.v;
		}
		if (a.features.find(arcs.indexes[i]) != a.features.end())
			arcs.table[i] = a.features[arcs.indexes[i]];
		else
			arcs.table[i] = new Xip_Fst(a);
		arcs.table[i]->loadarcs(dump, a, false);
	}
}

bool Xip_Fst::load(string name, Xip_Automaton& a) {
	ifstream dump(name, ios::in | ios::binary);
	if (dump.fail())
		return false;
	octet2 v;
	//the size
	dump.read(v.b, 2);
	int sz = v.v;
	char buff[2048];
	for (int i = 1; i <= sz; i++) {
		dump.read(v.b, 2); //the string size
		dump.read(buff, v.v);
		buff[v.v] = 0;
		a.ialphabet[i] = buff;		
		a.alphabet[buff] = i;
	}
	a.ialphabet[0] = "";
	a.fillencoding(true);
	a.start.loadarcs(dump, a, true);
	cerr << endl;
	return true;
}

void Xip_Fst::traverse(ofstream& dump) {
	//we check first if all indexes are identities
	char buff[] = { 0, 0, 0 };
	unsigned long u;

	if (arcs.nb == 0) {
		buff[0] = status | xflast;
		dump.write(buff, 1);
		return;
	}

	//Evaluation first...
	int i;
	char eval = 0;
	for (i = 0; i < arcs.nb; i++) {
		u = arcs.indexes[i];
		//three cases:
		if ((u >> 16) == (u & 0xFFFF))
			eval = 1;
		else {
			eval = 0;
			break;
		}
	}


	if (!eval) {
		buff[0] = status | xffull; buff[1] = arcs.nb; buff[2] = arcs.sz;
		dump.write(buff, 3);
		octet4 v;
		for (i = 0; i < arcs.nb; i++) {
			v.v = arcs.indexes[i];
			dump.write(v.b, 4);
			arcs.table[i]->traverse(dump);
		}
		return;
	}

	buff[0] = status | xfshort; buff[1] = arcs.nb; buff[2] = arcs.sz;
	dump.write(buff, 3);
	octet2 v;
	for (i = 0; i < arcs.nb; i++) {
		u = arcs.indexes[i];
		u >>= 16;
		v.v = (unsigned short)u;
		dump.write(v.b, 2);
		arcs.table[i]->traverse(dump);
	}
}

//we suppose our automaton without any loops... A pure lexicon
bool Xip_Fst::compile(string name, Xip_Automaton& a) {	
	unordered_map<unsigned short, string>::iterator it;
	map<unsigned short, string> sorted;

	for (it = a.ialphabet.begin(); it != a.ialphabet.end(); it++) {
		if (it->first != 0)
			sorted[it->first] = it->second;
	}

	ofstream dump(name, ios::binary);
	map<unsigned short, string>::iterator its;	
	octet2 v;
	v.v = sorted.size();
	dump.write(v.b, 2);

	for (its = sorted.begin(); its != sorted.end(); its++) {
		v.v = its->second.size();
		dump.write(v.b, 2);
		dump.write(its->second.c_str(), its->second.size());
	}

	//We reindex to skip the NULL...
	int idx = 0;
	int i;
	for (i = 0; i < a.garbage.size(); i++) {
		if (a.garbage[i] != NULL)
			a.garbage[i]->id = idx++;
	}

	for (i = 0; i < a.garbage.size(); i++) {
		if (a.garbage[i] != NULL) {

		}
	}

	return true;
}

bool Xip_Fst::loadtext(string name, Xip_Automaton& a) {
	ifstream f(name.c_str(), ios::in | ios::binary);
	if (f.fail())
		return false;

	agnostring w;
	agnostring l;
	wstring wstr;
	wstring lstr;

	string feats;
	int compte = 0;
	int fpos = 0;

	while (!f.eof()) {
		getline(f, w);
		getline(f, l);
		w = w.trim();
		l = l.trim();
		if (w.size() && l.size()) {
			fpos = l.rfind("\t", l.size());
			if (fpos != string::npos) {
				feats = l.substr(fpos, l.size() - fpos);
				l = l.substr(0, fpos);
				fpos = a.index(feats);
				a.addfeature(fpos << 16);

				wstr = w.latintounicode();
				lstr = l.latintounicode();
				lstr += fpos;
				add(wstr, lstr, 0, 0, a);
				compte++;
				if ((compte % 10000) == 0)
					cerr << ".";
			}
		}
	}

	return true;
}

bool Xip_Fst::parse(Xip_Automaton& a, vector<string>& vs, int& i, vector<unsigned long>& indexes) {
	if (i == vs.size())
		return true;

	if (vs[i] == ")") {
		i++;
		return true;
	}

	int j;
	int subpos = -1;
	unsigned long cw;

	Xip_Fst* sub = NULL;
	if (vs[i] == "(") {
		status |= xfarcend;
		subpos = a.garbage.size();
		sub = new Xip_Fst(a);
		if (!sub->parse(a, vs, ++i, indexes))
			return false;

		for (j = 0; j < sub->arcs.nb; j++) {
			cw = sub->arcs.indexes[j];
			arcs[cw] = sub->arcs.table[j];
		}

		if (i == vs.size()) {
			delete sub;
			a.garbage[subpos] = NULL;
			return true;
		}
	}

	Xip_Fst* loop;
	Xip_Fst* element;

	if (vs[i] == "!") {
		i++;
		if (i == vs.size())
			return false;
		j = vs[i][0] - 49;
		cw = indexes[j];
		cw <<= 16;
		loop = new Xip_Fst(a);
		loop->status |= xfarcend;
		status |= xfepsilonword | xfarcend;
		arcs[cw] = loop;
		return parse(a, vs, ++i, indexes);
	}

	if (vs[i][0] == '$') {
		j = 1;
		if (vs[i].size() == 1) {
			i++;
			if (i == vs.size())
				return false;
			j = 0;
		}

		loop = this;
		Xip_Fst* first = NULL;
		unsigned long cwfirst = 0;
		for (; j < vs[i].size(); j++) {
			cw = a.index(vs[i][j]);
			cw |= cw << 16;
			element = new Xip_Fst(a);
			if (first == NULL) {
				first = element;
				cwfirst = cw;
			}
			loop->arcs[cw] = element;
			loop = element;
		}


		if (sub != NULL && first != NULL) {
			for (int e = 0; e < sub->arcs.nb; e++) {
				element = arcs[sub->arcs.indexes[e]];
				element->arcs[cwfirst] = first;
			}
			delete sub;
			a.garbage[subpos] = NULL;
		}

		return loop->parse(a, vs, ++i, indexes);
	}

	if (vs[i][0] == '%') {
		//the character should be next...
		if (vs[i].size() == 1) {
			i++;
			if (i == vs.size())
				return false;

			cw = a.index(vs[i]);
		}
		else
			cw = a.index(vs[i][1]);

		cw |= cw << 16;
		loop = new Xip_Fst(a);
		if (sub != NULL) {
			for (j = 0; j < sub->arcs.nb; j++) {
				element = arcs[sub->arcs.indexes[j]];
				element->arcs[cw] = loop;
			}
			delete sub;
			a.garbage[subpos] = NULL;
		}

		arcs[cw] = loop;
		return loop->parse(a, vs, ++i, indexes);		
	}

	if (vs[i] == "{") {
		i++;
		if (i == vs.size())
			return false;
		vector<Xip_Fst*> vl;
		vector<int> gloop;
		while (vs[i] != "}") {
			j = 0;
			vector<string> vsub;
			vsub.push_back("$" + vs[i]);
			gloop.push_back(a.garbage.size());
			loop = new Xip_Fst(a);
			loop->parse(a, vsub, j, indexes);
			vl.push_back(loop);
			i++;
			if (i == vs.size())
				return false;
		}

		if (vl.size()) {
			loop = vl[0];
			while (loop->arcs.size() != 0)
				loop = loop->arcs.table[0];
			loop->parse(a, vs, ++i, indexes);
			if (loop->arcs.nb) {
				cw = loop->arcs.indexes[0];
				for (j = 1; j < vl.size(); j++) {
					element = vl[j];
					while (element->arcs.size() != 0)
						element = element->arcs.table[0];
					element->arcs[cw] = loop->arcs[cw];
				}
			}
			for (j = 0; j < vl.size(); j++) {
				loop = vl[j];
				if (loop->arcs.nb) {
					cw = loop->arcs.indexes[0];
					arcs[cw] = loop->arcs.table[0];
					if (sub != NULL) {
						for (int e = 0; e < sub->arcs.nb; e++) {
							element = arcs[sub->arcs.indexes[e]];
							element->arcs[cw] = loop;
						}
					}
				}
				delete loop;
				a.garbage[gloop[j]] = NULL;
			}
		}

		if (sub != NULL) {
			delete sub;
			a.garbage[subpos] = NULL;
		}
		return true;
	}

	if (vs[i] == "[") {
		i++;
		if (i == vs.size())
			return false;
		vector<unsigned long> vl;
		while (vs[i] != "]") {
			cw = a.index(vs[i]);
			cw |= cw << 16;
			vl.push_back(cw);
			i++;
			if (i == vs.size())
				return false;
		}

		loop = new Xip_Fst(a);

		if ((i + 1) < vs.size() && vs[i + 1] == "+") {
			i++;
			for (j = 0; j < vl.size(); j++)
				loop->arcs[vl[j]] = loop;
		}

		for (j = 0; j < vl.size(); j++) {
			cw = vl[j];
			arcs[cw] = loop;

			if (sub != NULL) {
				for (int e = 0; e < sub->arcs.nb; e++) {
					element = arcs[sub->arcs.indexes[e]];
					element->arcs[cw] = loop;
				}
			}
		}

		if (sub != NULL) {
			delete sub;
			a.garbage[subpos] = NULL;
		}

		return loop->parse(a, vs, ++i, indexes);
	}
	return true;
}

bool Xip_Fst::parse(Xip_Automaton& a, agnostring& expression, vector<unsigned long>& indexes) {
	vector<string> vs;
	expression.tokenize(vs, false, false, true);
	int i = 0;
	int ig = a.garbage.size();
	Xip_Fst* xf = new Xip_Fst(a);
	if (!xf->parse(a, vs, i, indexes))
		return false;

	arcs.resize(arcs.sz + xf->arcs.nb + 1);
	status |= xf->status;

	for (int i = 0; i < xf->arcs.nb; i++)
		arcs.add(xf->arcs.indexes[i], xf->arcs.table[i]);
	return true;
}

void Xip_Fst::regulars(Xip_Automaton& a) {
	//Our features
	vector<unsigned long> indexes;

	string e = "\t+Dig+Card";
	indexes.push_back(a.index(e));
	e = "\t+Dig+Dec";
	indexes.push_back(a.index(e));
	e = "\t+Exp+Dec";
	indexes.push_back(a.index(e));
	e = "\t+Dig+Ord";
	indexes.push_back(a.index(e));
	e = "\t+Dig+Rom";
	indexes.push_back(a.index(e));

	agnostring expression = "([- +])[0 1 2 3 4 5 6 7 8 9]+!1(%.[0 1 2 3 4 5 6 7 8 9]+!2([e E]([- +])[0 1 2 3 4 5 6 7 8 9]+!3))";
	parse(a, expression, indexes);

	expression = "[D M C L X V I]+!5";
	parse(a, expression, indexes);

	expression = "{1st 2nd 3rd}!4";
	parse(a, expression, indexes);

	expression = "[0 1 2 3 4 5 6 7 8 9]([0 1 2 3 4 5 6 7 8 9]+)$th!4";
	parse(a, expression, indexes);
}

bool Xip_Fst::tokenize(agnostring& w, string& s, parkour_context& pkc, vector<string>& res, Xip_Automaton& a) {
	BULONG cr;
	unsigned long cw;
	bool endtoken = false;
	bool found = false;
	int bpos, cpos;
	int i;

	if (w.end())
		endtoken = true;
	else {
		int pbpos = w.bytepos;
		int pcpos = w.charpos;
		cr = w.nextcode();
		bpos = w.bytepos;
		cpos = w.charpos;

		cw = a.code(cr);
		if (c_is_space(cr) || c_is_punctuation(cr)) {
			//we need then to check if we can carry on with arcs...
			string v;			
			i = -1;
			while (arcs.check(cw, i)) {
				if (!(status&xfepsilonlem)) {
					v = s + a.ialphabet[arcs.code(i)];
					if (arcs.found(i)->tokenize(w, v, pkc, res, a))
						found = true;
				}
				else {
					if (arcs.found(i)->tokenize(w, s, pkc, res, a))
						found = true;
				}
				w.setpos(bpos, cpos);
			}

			if (found)
				return true;

			//In this case, the character does not belong to the string
			//We try to analyze it...
			if (s != "") {
				w.setpos(pbpos, pcpos);
				endtoken = true;
			}
			else
			if (c_is_space(cr)) {
				pkc.eset(bpos, cpos);
				return true;
			}
		}
	}

	if (endtoken) {
		if (status&xfarcend) {
			if (status&xfepsilonword) {
				agnostring empty;
				up(empty, s, res, a);
				pkc.eset(w);
			}
			else {
				pkc.eset(w);
				res.push_back(s);
			}
			return true;
		}
		return false;
	}


	string v;	
	i = -1;
	while (arcs.check(cw, i)) {
		if (!(status&xfepsilonlem)) {
			v = s + a.ialphabet[arcs.code(i)];
			if (arcs.found(i)->tokenize(w, v, pkc, res, a))
				found = true;
		}
		else {
			if (arcs.found(i)->tokenize(w, s, pkc, res, a))
				found = true;
		}
		w.setpos(bpos, cpos);
	}

	if (c_is_upper(cr)) {
		cw = a.index(c_to_lower(cr));	
		i = -1;
		while (arcs.check(cw,i)) {
			if (!(status&xfepsilonlem)) {
				v = s + a.ialphabet[arcs.code(i)];
				if (arcs.found(i)->tokenize(w, v, pkc, res, a))
					found = true;
			}
			else {
				if (arcs.found(i)->tokenize(w, s, pkc, res, a))
					found = true;
			}
			w.setpos(bpos, cpos);
		}

		cw = a.index(cr);
	}

	return found;
}

//We only check one word within the automaton and return all its readings.
bool Xip_Fst::up(agnostring& w, string& s, vector<string>& res, Xip_Automaton& a) {
	int cpos, bpos;
	if (w.end()) {
		if (status&xfarcend) {
			if (status&xfepsilonword) {
				string v;
				for (int i = 0; i < arcs.nb; i++) {
					//It should not be a transducer arc....
					if (arcs.indexes[i] & 0xFFFF)
						continue;
					v = s + a.ialphabet[arcs.code(i)];
					arcs.found(i)->up(w, v, res, a);
				}
			}
			else
				res.push_back(s);
			return true;
		}
		return false;
	}

	unsigned long cw = a.index(w.nextcode());
	cpos = w.charpos;
	bpos = w.bytepos;

	string v;
	bool found = false;
	int i = -1;
	while (arcs.check(cw, i)) {
		if (!(status&xfepsilonlem)) {
			v = s + a.ialphabet[arcs.code(i)];
			if (arcs.found(i)->up(w, v, res, a))
				found = true;
		}
		else {
			if (arcs.found(i)->up(w, s, res, a))
				found = true;
		}
		w.setpos(cpos, bpos);
	}
	return found;
}

bool Xip_Fst::check(agnostring& w, Xip_Automaton& a) {
	if (w.end()) {
		if (status&xfarcend)
			return true;
		return false;
	}
	unsigned long cw = a.index(w.nextcode());
	int i = -1;
	while (arcs.check(cw,i)) {
		if (arcs.found(i)->check(w, a))
			return true;
	}
	return false;
}

//We add a pair of characters to the automaton...
//We conflate the character code of the lower and upper sides on one single 32 bits value...
void Xip_Fst::add(wstring& w, wstring& lf, int posw, int posl, Xip_Automaton& a) {
	unsigned long cw;

	if (posw < w.size()) {
		cw = w[posw++];
		cw = a.index(cw);
		//we merge the bits of the two characters as a key
		if (posl < lf.size()) {
			unsigned long  clf = lf[posl++];
			//the last character is the feature structure, which we do not want to reinterpret
			if (posl < lf.size())
				clf = a.index(clf);
			//the upper side is stored on the upper bits
			cw |= clf << 16;
		}
		else
			status |= xfepsilonlem;

		if (!arcs.find(cw))
			arcs[cw] = new Xip_Fst(w, lf, posw, posl, a);
		else
			arcs[cw]->add(w, lf, posw, posl, a);
		return;
	}

	
	status |= xfarcend;
	if (posl < lf.size()) {
		cw = lf[posl++];
		//the last character is the feature structure, which we do not want to reinterpret
		if (posl < lf.size())
			cw = a.index(cw);
		cw <<= 16;
		if (!arcs.find(cw)) {
			if (a.features.find(cw) != a.features.end())
				arcs[cw] = a.features[cw];
			else
				arcs[cw] = new Xip_Fst(w, lf, posw, posl, a);			
		}
		else
			arcs[cw]->add(w, lf, posw, posl, a);
		status |= xfepsilonword;
	}
}

void recharge(string chemin) {
	Xip_Automaton a;
	chemin += ".cpt";
	a.load(chemin);
}

void lance(string chemin) {	

	Xip_Automaton a;
	a.compile(chemin);	

	return;

	chemin += ".cpt";
	if (a.load(chemin) == false) {
		cerr << "Erreur" << endl;
		return;
	}

	//a.loadtext(chemin);
	a.regulars();

	a.fillencoding(false);

	arc_map<Xip_Fst*>::iterator it;
	for (it = a.start.arcs.begin(); it != a.start.arcs.end(); it++) {
		cerr << it->first << endl;
	}

	vector<string> v;
	a.up("test", v);
	for (int i = 0; i < v.size(); i++)
		cerr << v[i] << endl;

	agnostring s("234th and MMLXVII Mich. 4th The ladyyy, with a 10 nice 211.234 bag, is happy in MMLXVII with a 1ST and a 121th and 4th and 6th strike. she has it that it's quite rude.");
	parkour_context pkc;
	s.begin();
	while (!s.end()) {
		pkc.init(s);
		a.tokenize(s, pkc, v);
		cerr << pkc.extract(s) << endl;
		//cerr << pkc.offsets() << endl;
		for (int i = 0; i < v.size(); i++)
			cerr << v[i] << endl;
	}
	cerr << endl;
	cerr << a.check("Chuadsad") << endl;
	cerr << a.ialphabet.size() << endl;
	cerr << endl;
}

void main(int argc, char *argv[]) {
	inittableutf8();
	lance("C:\\XIP\\Test\\kifcxx\\phonologie\\lexiqueanglais.txt");
	cerr << "FIN" << endl;
}
