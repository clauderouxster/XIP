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

#include "kif.h"
#include "kifcontainer.h"
#include <algorithm>

double localrandom(long mx);

static bool v_comma_split_string(string& thestr, vector<string>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	string value;
	uchar c, nxt;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '"':
		case '\'':
			nxt = c;
			break;
		case '@':
			nxt = '"';
			if (thestr[++pos] != '"') {
				v.clear();
				return false;
			}
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			comma = true;
			continue;
		default:
			v.clear();
			return false;

		}
		if (!comma) {
			v.clear();
			return false;
		}
		comma = false;
		value = "";
		pos++;
		while (pos < sz && thestr[pos] != nxt)
			value += thestr[pos++];
		if (pos == sz) {
			v.clear();
			return false;
		}
		if (c == '@') {
			if (thestr[++pos] != '@') {
				v.clear();
				return false;
			}
		}
		v.push_back(value);
	}

	if (comma) {
		v.clear();
		return false;
	}
	return true;
}

static bool v_comma_split_string(wstring& thestr, vector<wstring>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	wstring value;
	wchar_t c, nxt;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '"':
		case '\'':
			nxt = c;
			break;
		case '@':
			nxt = '"';
			if (thestr[++pos] != '"') {
				v.clear();
				return false;
			}
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			comma = true;
			continue;
		default:
			v.clear();
			return false;

		}
		if (!comma) {
			v.clear();
			return false;
		}
		comma = false;
		value = L"";
		pos++;
		while (pos < sz && thestr[pos] != nxt)
			value += thestr[pos++];
		if (pos == sz) {
			v.clear();
			return false;
		}
		if (c == '@') {
			if (thestr[++pos] != '@') {
				v.clear();
				return false;
			}
		}
		v.push_back(value);
	}

	if (comma) {
		v.clear();
		return false;
	}
	return true;
}

static bool v_comma_split_float(string& thestr, vector<double>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	string value;
	uchar c;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
			comma = false;
			value += c;
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			v.push_back(convertfloat(STR(value)));
			value = "";
			comma = true;
			break;
		default:
			v.clear();
			return false;
		}
	}
	if (comma) {
		v.clear();
		return false;
	}
	v.push_back(convertfloat(STR(value)));
	return true;
}

static bool v_comma_split_int(string& thestr, vector<long>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	bool point = false;
	long value = 0;
	uchar c;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (point)
				continue;
			value = value * 10 + c - 48;
			comma = false;
			break;
		case '.':
			comma = false;
			point = true;
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			v.push_back(value);
			value = 0;
			comma = true;
			point = false;
			break;
		default:
			v.clear();
			return false;
		}
	}
	if (comma) {
		v.clear();
		return false;
	}
	v.push_back(value);
	return true;
}

static bool v_comma_split_byte(string& thestr, vector<uchar>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	bool point = false;
	long value = 0;
	uchar c;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (point)
				continue;
			value = value * 10 + c - 48;
			comma = false;
			break;
		case '.':
			comma = false;
			point = true;
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			v.push_back(value);
			value = 0;
			comma = true;
			point = false;
			break;
		default:
			v.clear();
			return false;
		}
	}
	if (comma) {
		v.clear();
		return false;
	}
	v.push_back(value);
	return true;
}
//---------------------------------------------------------------------------------------------------------
bool KifEvaluate::Boolean() {
	KifContainerLock _lock(this);
	if (Size() == 0)
		return false;
	return true;
}

Exported KifElement* KifEvaluate::less(KifElement* a) {
	if (!a->aContainer())
		return kifFALSE;

	int sz = Size();
	int sza = a->Size();

	if (sz < sza)
		return kifTRUE;

	if (sz>sza || !sz)
		return kifFALSE;

	KifContainerLock _lock(this);
	KifContainerLock _locka((KifEvaluate*)a);

	KifIteration* it = (KifIteration*)Newiterator(false);
	KifIteration* itr = (KifIteration*)a->Newiterator(false);
	KifElement* k;
	KifElement* ka;
	KifElement* res = kifNULL;
	for (it->Begin(), itr->Begin(); it->End() == kifFALSE; it->Next(), itr->Next()) {
		k = it->IteratorValue();
		ka = itr->IteratorValue();
		if (k->same(ka) == kifFALSE)
			res = k->less(ka);
		else
			res = kifNULL;
		k->Release();
		ka->Release();
		if (res != kifNULL)
			break;
	}
	it->Release();
	itr->Release();
	if (res == kifNULL)
		return kifFALSE;
	return res;
}

Exported KifElement* KifEvaluate::more(KifElement* a) {
	if (!a->aContainer())
		return kifFALSE;

	int sz = Size();
	int sza = a->Size();

	if (sz > sza)
		return kifTRUE;

	if (sz < sza || !sz)
		return kifFALSE;

	KifContainerLock _lock(this);
	KifContainerLock _locka((KifEvaluate*)a);

	KifIteration* it = (KifIteration*)Newiterator(false);
	KifIteration* itr = (KifIteration*)a->Newiterator(false);
	KifElement* k;
	KifElement* ka;
	KifElement* res = kifNULL;
	for (it->Begin(), itr->Begin(); it->End() == kifFALSE; it->Next(), itr->Next()) {
		k = it->IteratorValue();
		ka = itr->IteratorValue();
		if (k->same(ka) == kifFALSE)
			res = k->more(ka);
		else
			res = kifNULL;
		k->Release();
		ka->Release();
		if (res != kifNULL)
			break;
	}
	it->Release();
	itr->Release();
	if (res == kifNULL)
		return kifFALSE;
	return res;
}

Exported KifElement* KifEvaluate::lessequal(KifElement* a) {
	if (!a->aContainer())
		return kifFALSE;

	int sz = Size();
	int sza = a->Size();

	if (sz < sza || !sz)
		return kifTRUE;

	if (sz>sza)
		return kifFALSE;

	KifContainerLock _lock(this);
	KifContainerLock _locka((KifEvaluate*)a);

	KifIteration* it = (KifIteration*)Newiterator(false);
	KifIteration* itr = (KifIteration*)a->Newiterator(false);
	KifElement* k;
	KifElement* ka;
	KifElement* res = kifNULL;
	for (it->Begin(), itr->Begin(); it->End() == kifFALSE; it->Next(), itr->Next()) {
		k = it->IteratorValue();
		ka = itr->IteratorValue();
		if (k->same(ka) == kifFALSE)
			res = k->less(ka);
		else
			res = kifNULL;
		k->Release();
		ka->Release();
		if (res != kifNULL)
			break;
	}
	it->Release();
	itr->Release();
	if (res == kifNULL)
		return kifTRUE;
	return res;
}

Exported KifElement* KifEvaluate::moreequal(KifElement* a) {
	if (!a->aContainer())
		return kifFALSE;

	int sz = Size();
	int sza = a->Size();

	if (sz > sza || !sz)
		return kifTRUE;

	if (sz < sza)
		return kifFALSE;

	KifContainerLock _lock(this);
	KifContainerLock _locka((KifEvaluate*)a);

	KifIteration* it = (KifIteration*)Newiterator(false);
	KifIteration* itr = (KifIteration*)a->Newiterator(false);
	KifElement* k;
	KifElement* ka;
	KifElement* res = kifNULL;
	for (it->Begin(), itr->Begin(); it->End() == kifFALSE; it->Next(), itr->Next()) {
		k = it->IteratorValue();
		ka = itr->IteratorValue();
		if (k->same(ka) == kifFALSE)
			res = k->more(ka);
		else
			res = kifNULL;
		k->Release();
		ka->Release();
		if (res != kifNULL)
			break;
	}
	it->Release();
	itr->Release();
	if (res == kifNULL)
		return kifTRUE;
	return res;
}

KifElement* KifEvaluate::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifElement* ke;
	KifElement* kref;
	bool found;

	KifContainerLock _lock(this);
	KifIteration* it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ref = a->Newinstance();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			kref = itr->IteratorValue();
			it = (KifIteration*)a->Newiterator(false);
			for (it->Begin(); it->End() == kifFALSE; it->Next()) {
				ke = it->IteratorValue();
				if (kref->same(ke) == kifTRUE) {
					found = true;
					ke->Release();
					break;
				}
				ke->Release();
			}
			it->Release();
			if (found == false)
				ref->Push(kref);
			kref->Release();
		}

		it = (KifIteration*)a->Newiterator(false);
		for (it->Begin(); it->End() == kifFALSE; it->Next()) {
			kref = it->IteratorValue();
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
				ke = itr->IteratorValue();
				if (kref->same(ke) == kifTRUE) {
					ke->Release();
					found = true;
					break;
				}
				ke->Release();
			}

			if (found == false)
				ref->Push(kref);
			kref->Release();
		}

		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifEvaluate::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);


	KifElement* ke;
	KifElement* kref;
	bool found;
	KifContainerLock _lock(this);

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ref = a->Newinstance();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			kref = itr->IteratorValue();
			KifIteration* it = (KifIteration*)a->Newiterator(false);
			for (it->Begin(); it->End() == kifFALSE; it->Next()) {
				ke = it->IteratorValue();
				if (kref->same(ke) == kifTRUE) {
					found = true;
					ke->Release();
					break;
				}
				ke->Release();
			}
			it->Release();
			if (found == false)
				ref->Push(kref);
			kref->Release();
		}
		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifEvaluate::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);


	KifElement* ke;
	KifElement* kref;
	bool found;
	KifContainerLock _lock(this);

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ref = a->Newinstance();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			kref = itr->IteratorValue();
			KifIteration* it = (KifIteration*)a->Newiterator(false);
			for (it->Begin(); it->End() == kifFALSE; it->Next()) {
				ke = it->IteratorValue();
				if (kref->same(ke) == kifTRUE) {
					found = true;
					ke->Release();
					break;
				}
				ke->Release();
			}
			it->Release();
			if (found == true)
				ref->Push(kref);
			kref->Release();
		}
		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifEvaluate::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifElement* ke;
	KifElement* kref;
	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE || itr->End() == kifFALSE; it->Next(), itr->Next()) {
			ke = it->IteratorValue();
			kref = itr->IteratorValue();
			ref->Push(ke->plus(ke, kref, context, false));
			ke->Release();
			kref->Release();
		}

		it->Release();
		itr->Release();
		return ref;
	}

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		ke = it->IteratorValue();
		ref->Push(ke->plus(ke, b, context, false));
		ke->Release();
	}
	it->Release();
	return ref;
}

KifElement* KifEvaluate::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->minus(a, b, context, autoself);

	KifElement* ke;
	KifElement* kref;
	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			ke = it->IteratorValue();
			kref = itr->IteratorValue();
			ref->Push(ke->minus(ke, kref, context, false));
			ke->Release();
			kref->Release();
		}

		it->Release();
		itr->Release();
		return ref;
	}

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		ke = it->IteratorValue();
		ref->Push(ke->minus(ke, b, context, false));
		ke->Release();
	}
	it->Release();
	return ref;
}

KifElement* KifEvaluate::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->multiply(a, b, context, autoself);

	KifElement* ke;
	KifElement* kref;
	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			ke = it->IteratorValue();
			kref = itr->IteratorValue();
			ref->Push(ke->multiply(ke, kref, context, false));
			ke->Release();
			kref->Release();
		}

		it->Release();
		itr->Release();
		return ref;
	}

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		ke = it->IteratorValue();
		ref->Push(ke->multiply(ke, b, context, false));
		ke->Release();
	}
	it->Release();
	return ref;
}

KifElement* KifEvaluate::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->divide(a, b, context, autoself);

	KifElement* ke;
	KifElement* kref;
	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();
	int idthread = kifGlobals->GetThreadid();

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			ke = it->IteratorValue();
			kref = itr->IteratorValue();
			ref->Push(ke->divide(ke, kref, context, false));
			ke->Release();
			kref->Release();
			if (Errorid(idthread)) {
				it->Release();
				itr->Release();
				ref->Release();
				return kifRAISEERROR;
			}
		}

		it->Release();
		itr->Release();
		return ref;
	}

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		ke = it->IteratorValue();
		ref->Push(ke->divide(ke, b, context, false));
		ke->Release();
		if (Errorid(idthread)) {
			it->Release();
			ref->Release();
			return kifRAISEERROR;
		}
	}
	it->Release();
	return ref;
}

KifElement* KifEvaluate::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->mod(a, b, context, autoself);

	KifElement* ke;
	KifElement* kref;
	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();
	int idthread = kifGlobals->GetThreadid();

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			ke = it->IteratorValue();
			kref = itr->IteratorValue();
			ref->Push(ke->mod(ke, kref, context, false));
			ke->Release();
			kref->Release();
			if (Errorid(idthread)) {
				it->Release();
				itr->Release();
				ref->Release();
				return kifRAISEERROR;
			}
		}

		it->Release();
		itr->Release();
		return ref;
	}

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		ke = it->IteratorValue();
		ref->Push(ke->mod(ke, b, context, false));
		ke->Release();
		if (Errorid(idthread)) {
			it->Release();
			ref->Release();
			return kifRAISEERROR;
		}
	}
	it->Release();
	return ref;
}

KifElement* KifEvaluate::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->shiftright(a, b, context, autoself);

	KifElement* ke;
	KifElement* kref;
	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			ke = it->IteratorValue();
			kref = itr->IteratorValue();
			ref->Push(ke->shiftright(ke, kref, context, false));
			ke->Release();
			kref->Release();
		}

		it->Release();
		itr->Release();
		return ref;
	}

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		ke = it->IteratorValue();
		ref->Push(ke->shiftright(ke, b, context, false));
		ke->Release();
	}
	it->Release();
	return ref;
}

KifElement* KifEvaluate::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->shiftleft(a, b, context, autoself);

	KifElement* ke;
	KifElement* kref;
	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			ke = it->IteratorValue();
			kref = itr->IteratorValue();
			ref->Push(ke->shiftleft(ke, kref, context, false));
			ke->Release();
			kref->Release();
		}

		it->Release();
		itr->Release();
		return ref;
	}

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		ke = it->IteratorValue();
		ref->Push(ke->shiftleft(ke, b, context, false));
		ke->Release();
	}
	it->Release();
	return ref;

}

KifElement* KifEvaluate::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->power(a, b, context, autoself);

	KifElement* ke;
	KifElement* kref;
	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			ke = it->IteratorValue();
			kref = itr->IteratorValue();
			ref->Push(ke->power(ke, kref, context, false));
			ke->Release();
			kref->Release();
		}

		it->Release();
		itr->Release();
		return ref;
	}

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		ke = it->IteratorValue();
		ref->Push(ke->power(ke, b, context, false));
		ke->Release();
	}
	it->Release();
	return ref;
}

KifElement* Mapcompare(KifElement*a, KifElement*b) {
	if (!b->aMapContainer() || a->Size() != b->Size())
		return kifFALSE;

	if (!a->Size())
		return kifTRUE;

	KifElement* key;
	KifElement* value;
	KifElement* vb;
	KifElement* res;
	KifContainerLock _lock((KifEvaluate*)a);

	KifIteration* itr = (KifIteration*)a->Newiterator(false);
	for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
		value = itr->IteratorValue();
		key = itr->IteratorKey();
		res = kifFALSE;
		switch (key->type) {
		case kifLong:
		case kifInteger:
			vb = b->ValueOnIntegerKey(key->Integer());
			if (vb != kifNOELEMENT)
				res = vb->same(value);
			break;
		case kifFloat:
			vb = b->ValueOnFloatKey(key->Float());
			if (vb != kifNOELEMENT)
				res = vb->same(value);
			break;
		default:
			vb = b->ValueOnKey(key->String());
			if (vb != kifNOELEMENT)
				res = vb->same(value);
		}
		vb->Release();
		key->Release();
		value->Release();
		if (res == kifFALSE) {
			itr->Release();
			return kifFALSE;
		}
	}
	itr->Release();
	return kifTRUE;
}
//---------------------------------------------------
KifElement* Selectacontainer(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->Assigning(kifAny)) {
		context->Clear();
		return context;
	}

	return context->Newinstance();
}

//We only create a new vector or a new map if we are not in a context where the current map or vector cannot be used
KifVector* Selectvector(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->Assigning(kifVector)) {
		context->Clear();
		return (KifVector*)context;
	}

	return new KifVector(kref->kifcode, NULL);
}

KifElement* Selectavector(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->Assigning(kifAny)) {
		context->Clear();
		return context;
	}

	return new KifVector(kref->kifcode, NULL);
}

KifElement* Selectvectorstring(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->Assigning(kifVectorString)) {
		context->Clear();
		return context;
	}

	return new KifVectorString(kref->kifcode, NULL);
}

KifElement* Selectvectorustring(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->Assigning(kifVectorString)) {
		context->Clear();
		return context;
	}

	return new KifVectorUString(kref->kifcode, NULL);
}

KifElement* Selectvectorbyte(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->Assigning(kifVectorByte)) {
		context->Clear();
		return context;
	}

	return new KifVectorByte(kref->kifcode, NULL);
}

KifElement* Selectvectorinteger(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->Assigning(kifVectorInteger)) {
		context->Clear();
		return context;
	}

	return new KifVectorInteger(kref->kifcode, NULL);
}

KifElement* Selectvectorfloat(KifElement* kref, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != kref && context->Assigning(kifVectorFloat)) {
		context->Clear();
		return context;
	}

	return new KifVectorFloat(kref->kifcode, NULL);
}


extern const char* KIFLIBSTRINGS[];
bool TestkifFunctions(short n, hmap<short, bool>& kiffunc);

Exported void KifElement::Push(KifElement*) {
	kifcode->Returnerror(KIFLIBSTRINGS[243]);
}
Exported void KifElement::PushFirst(KifElement*) {
	kifcode->Returnerror(KIFLIBSTRINGS[243]);
}
Exported void KifElement::PushLast(KifElement*) {
	kifcode->Returnerror(KIFLIBSTRINGS[243]);
}
Exported void KifElement::Pop(KifElement* ke) {
	kifcode->Returnerror(KIFLIBSTRINGS[243]);
}
Exported KifElement* KifElement::PopFirst(KifElement* ke) {
	return kifcode->Returnerror(KIFLIBSTRINGS[243]);
}
Exported KifElement* KifElement::PopLast(KifElement* ke) {
	return kifcode->Returnerror(KIFLIBSTRINGS[243]);
}
Exported KifElement* KifElement::Last() {
	return kifcode->Returnerror(KIFLIBSTRINGS[243]);
}
Exported KifElement* KifElement::First() {
	return kifcode->Returnerror(KIFLIBSTRINGS[243]);
}
//------------------------------------------------------------------------------------------
Exported KifElement* KifCycleVector::Newiterator(bool rev) {
	if (repeat)
		return new KifIteratorCycleElement(kifcode, NULL, this);

	return new KifIteratorCycleVector(kifcode, NULL, this);
}

Exported KifElement* KifCycleVector::Newinstance() {
	if (!repeat)
		return value->Newinstance();

	//Depends on the internal type of value...
	char code = value->inCode();
	switch (code) {
	case ckifInteger:
		return new KifVectorInteger(kifcode, NULL);
	case ckifFloat:
	case ckifIntFloat:
		return new KifVectorFloat(kifcode, NULL);
	case ckifString:
		return new KifVectorString(kifcode, NULL);
	case ckifUString:
		return new KifVectorUString(kifcode, NULL);
	}
	return new KifVector(kifcode, NULL);
}

Exported KifElement* KifCycleVector::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	value = base->Exec(contextualpattern, dom, idx, idthread, callfunc);
	return this;
}

Exported KifElement* KifReplicateVector::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	value = base->Exec(contextualpattern, dom, idx, idthread, callfunc);
	nb = nbbase->Exec(contextualpattern, dom, idx, idthread, callfunc);
	return this;
}

Exported KifElement* KifReplicateVector::Newiterator(bool rev) {
	KifIteratorReplicateElement* iter = new KifIteratorReplicateElement(kifcode, NULL, this);
	iter->mx = nb->Integer();
	return iter;
}

Exported KifElement* KifReplicateVector::Newinstance() {
	char code = value->inCode();
	switch (code) {
	case ckifInteger:
		return new KifVectorInteger(kifcode, NULL);
	case ckifFloat:
	case ckifIntFloat:
		return new KifVectorFloat(kifcode, NULL);
	case ckifString:
		return new KifVectorString(kifcode, NULL);
	case ckifUString:
		return new KifVectorUString(kifcode, NULL);
	}
	return new KifVector(kifcode, NULL);
}

KifElement* KifIteratorCycleVector::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	return iter->Exec(contextualpattern, dom, val, idthread, callfunc);
}

KifElement* KifIteratorCycleElement::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	return cycle->value;
}

KifElement* KifIteratorReplicateElement::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	return replicate->value;
}

//------------------------------------------------------------------------------------------

Exported KifElement* KifInfinitiveVector::Newiterator(bool rev) {
	KifElement* s = seed;
	KifElement* st = step;

	int idthread = -1;
	KifDomain* dom = NULL;
	KifCallFunction* callfunc = NULL;
	if (seed->type != kifInteger && seed->type != kifFloat && seed->type != kifLong) {
		idthread = kifGlobals->GetThreadid();
		dom = kifGlobals->Beforetopid(idthread);
		callfunc = (KifCallFunction*)kifGlobals->Topid(idthread);
		s = seed->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	if (step->type != kifInteger && step->type != kifFloat && step->type != kifLong) {
		if (idthread == -1) {
			idthread = kifGlobals->GetThreadid();
			dom = kifGlobals->Beforetopid(idthread);
			callfunc = (KifCallFunction*)kifGlobals->Topid(idthread);
		}
		st = step->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}

	if (compute) {//In this case, the step is a difference between the two values step-seed
		if (s->type == kifFloat || st->type == kifFloat) {
			double d = st->Float() - s->Float();
			KifIteratorInfiniteveFloatVector* iter = new KifIteratorInfiniteveFloatVector(kifcode, NULL, this);
			iter->itx = s->Float();
			iter->inc = d;
			if (s != seed)
				s->Release();
			if (st != step)
				st->Release();
			return iter;
		}

		long d = st->Integer() - s->Integer();
		KifIteratorInfiniteveVector* iter = new KifIteratorInfiniteveVector(kifcode, NULL, this);
		iter->itx = s->Integer();
		iter->inc = d;
		if (s != seed)
			s->Release();
		if (st != step)
			st->Release();
		return iter;
	}

	if (s->type == kifFloat || st->type == kifFloat) {
		KifIteratorInfiniteveFloatVector* iter = new KifIteratorInfiniteveFloatVector(kifcode, NULL, this);
		iter->itx = s->Float();
		iter->inc = st->Float();
		if (s != seed)
			s->Release();
		if (st != step)
			st->Release();
		return iter;
	}

	KifIteratorInfiniteveVector* iter = new KifIteratorInfiniteveVector(kifcode, NULL, this);
	iter->itx = s->Integer();
	iter->inc = st->Integer();
	if (s != seed)
		s->Release();
	if (st != step)
		st->Release();
	return iter;
}

Exported string KifInfinitiveVector::String() {
	stringstream res;
	if (direction)
		res << "[.." << seed->String() << "]";
	else
		res << "[" << seed->String() << "..]";
	return res.str();
}

//------------------------------------------------------------------------------------------
KifElement* KifEvaluateMap::Split(KifBaseMap* basemap, KifCallFunction* current) {
	int sz = basemap->values.size();
	if (!sz) {
		if (Size() != 0)
			return kifRAISEERROR;
		return kifTRUE;
	}

	if (basemap->merge == 1 && Size() != sz)
		return kifRAISEERROR;

	KifElement* bkey = kifTRUE;
	KifElement* bval = kifTRUE;
	KifElement* val;
	KifElement* key;
	int iv = 0;
	int name;
	KifIteration* itr = (KifIteration*)Newiterator(false);
	itr->Begin();

	for (int i = 0; i < sz; i++) {
		bkey = basemap->keys[i];
		bval = basemap->values[i];

		if (bkey->Name() == idpipe) {
			basemap->merge = 2;
			KifElement* vals = Newinstance();
			//the variable in which we will store the rest of our list...
			while (itr->End() != kifTRUE) {
				key = itr->IteratorKey();
				val = itr->IteratorValue();
				vals->Push(key, val);
				key->Release();
				val->Release();
				itr->Next();
			}

			name = bval->Name();
			current->keep(name, vals);
			return kifTRUE;
		}

		if (itr->End() == kifTRUE)
			return kifRAISEERROR;



		key = itr->IteratorKey();
		name = bkey->Name();
		if (name != -1) {
			current->keep(name, key);
			key->Release();
		}
		else {
			//In this case, we need to compare the values...
			if (key->same(bkey) == kifFALSE) {
				itr->Release();
				key->Release();
				return kifRAISEERROR;
			}
		}

		val = itr->IteratorValue();
		name = bval->Name();
		if (name != -1) {
			current->keep(name, val);
			val->Release();
		}
		else {
			if (bval->type == kifBaseVector) {
				if (val->Split((KifBaseVector*)bval, current) == kifRAISEERROR) {
					itr->Release();
					val->Release();
					return kifRAISEERROR;
				}
			}
			else {
				if (bval->type == kifBaseMap) {
					if (val->Split((KifBaseMap*)bval, current) == kifRAISEERROR) {
						itr->Release();
						val->Release();
						return kifRAISEERROR;
					}
				}
				else {
					//In this case, we need to compare the values...
					if (val->same(bval) == kifFALSE) {
						itr->Release();
						val->Release();
						return kifRAISEERROR;
					}
				}
			}
		}

		itr->Next();
		iv++;
	}

	itr->Release();
	basemap->merge = 1;
	if (iv != Size())
		return kifRAISEERROR;

	return kifTRUE;
}
//---------------------------------------------------------------------------------------------------------
KifVector::KifVector(KifCode* klc, KifElement* kp) : KifEvaluate(klc, kp, kifVector) {}

//We match a BaseVector (an argument of a function) with a vector in a Haskell expression of the type [a,b|c]...
KifElement* KifVector::Split(KifBaseVector* basevector, KifCallFunction* current) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifRAISEERROR;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifRAISEERROR;

	KifElement* ret = kifTRUE;
	KifElement* val;
	int iv = 0;
	int szv = values.size();
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVector* vals = new KifVector(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->Push(values[iv]);

			if (parentval.Split((KifBaseVector*)ret, current) == kifRAISEERROR) {
				vals->Resetreference();
				return kifRAISEERROR;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifRAISEERROR;

		int name = ret->Name();
		val = values[iv];
		if (name != -1)
			current->keep(name, val);
		else {
			if (ret->type == kifBaseVector) {
				if (val->Split((KifBaseVector*)ret, current) == kifRAISEERROR)
					return kifRAISEERROR;
			}
			else {
				if (ret->type == kifBaseMap) {
					if (val->Split((KifBaseMap*)ret, current) == kifRAISEERROR)
						return kifRAISEERROR;
				}
				else {
					//In this case, we need to compare the values...
					if (val->same(ret) == kifFALSE)
						return kifRAISEERROR;
				}
			}
		}
		iv++;
	}
	basevector->merge = 1;
	if (iv != values.size())
		return kifRAISEERROR;

	return kifTRUE;
}

KifElement* KifVectorString::Split(KifBaseVector* basevector, KifCallFunction* current) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifRAISEERROR;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifRAISEERROR;

	KifElement* ret = kifTRUE;
	KifElement* val;
	int iv = 0;
	int szv = values.size();
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVectorString* vals = new KifVectorString(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->values.push_back(values[iv]);
			if (parentval.Split((KifBaseVector*)ret, current) == kifRAISEERROR) {
				vals->Resetreference();
				return kifRAISEERROR;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifRAISEERROR;

		int name = ret->Name();
		val = kifcode->Providestring(values[iv]);
		if (name != -1)
			current->keep(name, val);
		else {
			//In this case, we need to compare the values...
			if (val->same(ret) == kifFALSE) {
				val->Resetreference();
				return kifRAISEERROR;
			}
		}
		iv++;
	}

	basevector->merge = 1;

	if (iv != values.size())
		return kifRAISEERROR;

	return kifTRUE;
}

KifElement* KifVectorInteger::Split(KifBaseVector* basevector, KifCallFunction* current) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifRAISEERROR;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifRAISEERROR;

	int iv = 0;
	int szv = values.size();
	KifElement* ret;
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVectorInteger* vals = new KifVectorInteger(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->values.push_back(values[iv]);
			if (parentval.Split((KifBaseVector*)ret, current) == kifRAISEERROR) {
				vals->Resetreference();
				return kifRAISEERROR;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifRAISEERROR;

		int name = ret->Name();
		KifElement* val = kifcode->Provideinteger(values[iv]);
		if (name != -1)
			current->keep(name, val);
		else {
			//In this case, we need to compare the values...
			if (val->same(ret) == kifFALSE) {
				val->Resetreference();
				return kifRAISEERROR;
			}
		}
		iv++;
	}

	basevector->merge = 1;
	if (iv != values.size())
		return kifRAISEERROR;

	return kifTRUE;
}

KifElement* KifVectorFloat::Split(KifBaseVector* basevector, KifCallFunction* current) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifRAISEERROR;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifRAISEERROR;

	KifElement* ret = kifTRUE;
	KifElement* val;
	int iv = 0;
	int szv = values.size();
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVectorFloat* vals = new KifVectorFloat(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->values.push_back(values[iv]);

			if (parentval.Split((KifBaseVector*)ret, current) == kifRAISEERROR) {
				vals->Resetreference();
				return kifRAISEERROR;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifRAISEERROR;

		int name = ret->Name();
		val = kifcode->Providefloat(values[iv]);
		if (name != -1)
			current->keep(name, val);
		else {
			//In this case, we need to compare the values...
			if (val->same(ret) == kifFALSE) {
				val->Resetreference();
				return kifRAISEERROR;
			}
		}
		iv++;
	}

	basevector->merge = 1;
	if (iv != values.size())
		return kifRAISEERROR;

	return kifTRUE;
}


KifElement* KifElement::MatchPattern(KifElement* param, KifDomain* dom, int, KifCallFunction*) {
	return same(param);
}


KifElement* KifElement::Match(KifBaseVector* params, KifDomain* dom, int, KifCallFunction*) {
	return kifcode->Returnerror(KIFLIBSTRINGS[269]);
}

KifElement* KifVector::MatchPattern(KifElement* param, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (param->type == kifBaseVector) {
		if (Match((KifBaseVector*)param, dom, idthread, callfunc) == kifNOELEMENT)
			return kifFALSE;
		return kifTRUE;
	}
	return same(param);
}

KifElement* KifEvaluateMap::MatchPattern(KifElement* param, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (param->type == kifBaseMap) {
		if (Match((KifBaseMap*)param, dom, idthread, callfunc) == kifNOELEMENT)
			return kifFALSE;
		return kifTRUE;
	}
	return same(param);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------
KifElement* KifEvaluateMap::Match(KifBaseMap* basemap, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	int sz = basemap->values.size();
	if (!sz) {
		if (Size() != 0)
			return kifNOELEMENT;
		return kifTRUE;
	}

	if (basemap->merge == 1 && Size() != sz)
		return kifNOELEMENT;

	KifElement* bkey = kifTRUE;
	KifElement* bval = kifTRUE;
	KifElement* val;
	KifElement* key;
	int iv = 0;
	KifIteration* itr = (KifIteration*)Newiterator(false);
	itr->Begin();

	for (int i = 0; i < sz; i++) {
		bkey = basemap->keys[i];
		bval = basemap->values[i];

		if (bkey->Name() == idpipe) {
			basemap->merge = 2;
			KifElement* vals = Newinstance();
			//the variable in which we will store the rest of our list...
			while (itr->End() != kifTRUE) {
				key = itr->IteratorKey();
				val = itr->IteratorValue();
				vals->Push(key, val);
				key->Release();
				val->Release();
				itr->Next();
			}
			bval = bval->Thevariable(dom, idthread, callfunc);
			bval->Setvalue(vals);
			return kifTRUE;
		}

		if (itr->End() == kifTRUE)
			return kifNOELEMENT;

		key = itr->IteratorKey();
		if (bkey->Name() != -1) {
			bkey = bkey->Thevariable(dom, idthread, callfunc);
			bkey->Setvalue(key);
			key->Release();
		}
		else {
			//In this case, we need to compare the values...
			if (key->same(bkey) == kifFALSE) {
				itr->Release();
				key->Release();
				return kifNOELEMENT;
			}
		}

		val = itr->IteratorValue();
		if (bval->Name() != -1) {
			bval = bval->Thevariable(dom, idthread, callfunc);
			bval->Setvalue(val);
			val->Release();
		}
		else {
			if (bval->type == kifBaseVector) {
				if (val->Match((KifBaseVector*)bval, dom, idthread, callfunc) == kifNOELEMENT) {
					itr->Release();
					val->Release();
					return kifNOELEMENT;
				}
			}
			else {
				if (bval->type == kifBaseMap) {
					if (val->Match((KifBaseMap*)bval, dom, idthread, callfunc) == kifNOELEMENT) {
						itr->Release();
						val->Release();
						return kifNOELEMENT;
					}
				}
				else {
					//In this case, we need to compare the values...
					if (val->same(bval) == kifFALSE) {
						itr->Release();
						val->Release();
						return kifNOELEMENT;
					}
				}
			}
		}

		itr->Next();
		iv++;
	}

	itr->Release();
	basemap->merge = 1;
	if (iv != Size())
		return kifNOELEMENT;
	return kifTRUE;
}

KifElement* KifVector::Match(KifBaseVector* basevector, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifNOELEMENT;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifNOELEMENT;

	KifElement* ret = kifTRUE;
	KifElement* val;
	int iv = 0;
	int szv = values.size();
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVector* vals = new KifVector(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->Push(values[iv]);

			if (parentval.Match((KifBaseVector*)ret, dom, idthread, callfunc) == kifNOELEMENT) {
				vals->Resetreference();
				return kifNOELEMENT;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifNOELEMENT;

		val = values[iv];
		if (ret->Name() != -1) {
			ret = ret->Thevariable(dom, idthread, callfunc);
			ret->Setvalue(val);
		}
		else {
			if (ret->type == kifBaseVector) {
				if (val->Match((KifBaseVector*)ret, dom, idthread, callfunc) == kifNOELEMENT)
					return kifNOELEMENT;
			}
			else {
				if (ret->type == kifBaseMap) {
					if (val->Match((KifBaseMap*)ret, dom, idthread, callfunc) == kifNOELEMENT)
						return kifNOELEMENT;
				}
				else {
					//In this case, we need to compare the values...
					if (val->same(ret) == kifFALSE)
						return kifNOELEMENT;
				}
			}
		}
		iv++;
	}
	basevector->merge = 1;
	if (iv != values.size())
		return kifNOELEMENT;
	return kifTRUE;
}

KifElement* KifVectorString::MatchPattern(KifElement* param, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (param->type == kifBaseVector) {
		if (Match((KifBaseVector*)param, dom, idthread, callfunc) == kifNOELEMENT)
			return kifFALSE;
		return kifTRUE;
	}
	return same(param);
}

KifElement* KifVectorString::Match(KifBaseVector* basevector, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifNOELEMENT;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifNOELEMENT;

	KifElement* ret = kifTRUE;
	KifElement* val;
	int iv = 0;
	int szv = values.size();
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVectorString* vals = new KifVectorString(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->values.push_back(values[iv]);

			if (parentval.Match((KifBaseVector*)ret, dom, idthread, callfunc) == kifNOELEMENT) {
				vals->Resetreference();
				return kifNOELEMENT;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifNOELEMENT;

		val = kifcode->Providestring(values[iv]);
		if (ret->Name() != -1) {
			ret = ret->Thevariable(dom, idthread, callfunc);
			ret->Setvalue(val);
		}
		else {
			//In this case, we need to compare the values...
			if (val->same(ret) == kifFALSE) {
				val->Release();
				return kifNOELEMENT;
			}
		}
		iv++;
	}
	basevector->merge = 1;
	if (iv != values.size())
		return kifNOELEMENT;
	return kifTRUE;
}

KifElement* KifVectorInteger::MatchPattern(KifElement* param, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (param->type == kifBaseVector) {
		if (Match((KifBaseVector*)param, dom, idthread, callfunc) == kifNOELEMENT)
			return kifFALSE;
		return kifTRUE;
	}
	return same(param);
}


KifElement* KifVectorInteger::Match(KifBaseVector* basevector, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifNOELEMENT;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifNOELEMENT;

	KifElement* ret = kifTRUE;
	KifElement* val;
	int iv = 0;
	int szv = values.size();
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVectorInteger* vals = new KifVectorInteger(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->values.push_back(values[iv]);

			if (parentval.Match((KifBaseVector*)ret, dom, idthread, callfunc) == kifNOELEMENT) {
				vals->Resetreference();
				return kifNOELEMENT;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifNOELEMENT;

		val = kifcode->Provideinteger(values[iv]);
		if (ret->Name() != -1) {
			ret = ret->Thevariable(dom, idthread, callfunc);
			ret->Setvalue(val);
		}
		else {
			//In this case, we need to compare the values...
			if (val->same(ret) == kifFALSE) {
				val->Release();
				return kifNOELEMENT;
			}
		}
		iv++;
	}
	basevector->merge = 1;
	if (iv != values.size())
		return kifNOELEMENT;
	return kifTRUE;
}

KifElement* KifVectorFloat::MatchPattern(KifElement* param, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (param->type == kifBaseVector) {
		if (Match((KifBaseVector*)param, dom, idthread, callfunc) == kifNOELEMENT)
			return kifFALSE;
		return kifTRUE;
	}
	return same(param);
}


KifElement* KifVectorFloat::Match(KifBaseVector* basevector, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifNOELEMENT;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifNOELEMENT;

	KifElement* ret = kifTRUE;
	KifElement* val;
	int iv = 0;
	int szv = values.size();
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVectorFloat* vals = new KifVectorFloat(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->values.push_back(values[iv]);

			if (parentval.Match((KifBaseVector*)ret, dom, idthread, callfunc) == kifNOELEMENT) {
				vals->Resetreference();
				return kifNOELEMENT;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifNOELEMENT;

		val = kifcode->Providefloat(values[iv]);
		if (ret->Name() != -1) {
			ret = ret->Thevariable(dom, idthread, callfunc);
			ret->Setvalue(val);
		}
		else {
			//In this case, we need to compare the values...
			if (val->same(ret) == kifFALSE) {
				val->Release();
				return kifNOELEMENT;
			}
		}
		iv++;
	}
	basevector->merge = 1;
	if (iv != values.size())
		return kifNOELEMENT;
	return kifTRUE;
}


Exported KifElement* KifBaseVector::Vector(KifElement* dom, KifVector* v) {
	threadblocinit;
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = new KifVector(kifcode, NULL);
	else
		kvect->Clear();

	kvect->Reserve(values.size());
	KifElement* kval;
	for (int i = 0; i < values.size(); i++) {
		kval = values[i]->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
		kvect->Push(kval);
	}
	return kvect;
}

Exported KifElement* KifBaseVector::DuplicateHardCodedValue(KifDomain* dom) {
	threadblocinit;
	KifVector* kvect = new KifVector(kifcode, NULL);
	KifElement* kval;
	for (int i = 0; i < values.size(); i++) {
		kval = values[i]->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		kvect->Push(kval);
	}
	return kvect;
}

string KifBaseVectorMerge::String() {
	KifContainerLock _lock(this);
	string res("|");
	int it;
	bool beg = true;
	string sx;
	KifElement* element;
	for (it = 0; it < values.size(); it++) {
		element = values[it];
		if (beg == false)
			res += ",";
		beg = false;
		sx = element->StringForDisplay();
		if (!element->aString())
			res += sx;
		else
			stringing(res, sx);
	}
	return res;
}

string KifVector::String() {
	KifContainerLock _lock(this);
	string res;
	int it;
	res = "[";
	bool beg = true;
	string sx;
	KifElement* element;
	for (it = 0; it < values.size(); it++) {
		element = values[it];
		sx = element->StringForDisplay();
		if (!element->aString()) {
			if (sx == "")
				sx = "''";
			if (beg == false) {
				if (sx[0] != '|')
					res += ",";
			}
			res += sx;
		}
		else {
			if (beg == false)
				res += ",";
			stringing(res, sx);
		}
		beg = false;

	}
	res += "]";
	return res;
}

string KifVector::JSonString() {
	KifContainerLock _lock(this);
	string res;
	int it;
	res = "[";
	bool beg = true;
	string sx;
	KifElement* element;
	for (it = 0; it < values.size(); it++) {
		element = values[it];
		sx = element->JSonString();
		if (!element->aString()) {
			if (beg == false) {
				if (sx[0] != '|')
					res += ",";
			}
		}
		else {
			if (beg == false)
				res += ",";
		}
		res += sx;
		beg = false;
	}
	res += "]";
	return res;
}

string KifList::String() {
	KifContainerLock _lock(this);
	string res;
	list<KifElement*>::iterator it;
	res = "[";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res += ",";
		beg = false;
		sx = (*it)->StringForDisplay();
		if (!(*it)->aString())
			res += sx;
		else
			stringing(res, sx);
	}
	res += "]";
	return res;
}

string KifList::JSonString() {
	KifContainerLock _lock(this);
	string res;
	list<KifElement*>::iterator it;
	res = "[";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res += ",";
		beg = false;
		sx = (*it)->JSonString();
		if (!(*it)->aString())
			res += sx;
		else
			jstringing(res, sx);
	}
	res += "]";
	return res;
}


string KifVector::BaseString() {
	KifContainerLock _lock(this);
	string res;
	int it;
	res = "[";
	bool beg = true;
	string sx;
	KifElement* element;
	for (it = 0; it < values.size(); it++) {
		element = values[it];
		if (beg == false)
			res += ",";
		beg = false;
		sx = element->BaseString();
		res += sx;
	}
	res += "]";
	return res;
}

string KifList::BaseString() {
	KifContainerLock _lock(this);
	string res;
	list<KifElement*>::iterator it;
	res = "[";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res += ",";
		beg = false;
		sx = (*it)->BaseString();
		res += sx;
	}
	res += "]";
	return res;
}

KifElement* KifVector::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::orset");
#endif

	if (!a->aContainer())
		return a->xorset(a, b, context, autoself);


	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		int it;
		KifElement* ke;
		KifElement* kref;
		bool found;
		KifVector* A = (KifVector*)a;
		KifVector* ref = new KifVector(kifcode, NULL);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
			kref = itr->IteratorValue();
			found = false;
			for (it = 0; it < A->values.size(); it++) {
				ke = A->ValueOnIndex(it);
				if (kref->same(ke) == kifTRUE) {
					found = true;
					break;
				}
			}

			if (found == false)
				ref->Push(kref);
			kref->Release();
		}

		for (it = 0; it < A->values.size(); it++) {
			kref = A->values[it];
			found = false;
			for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
				ke = itr->IteratorValue();
				if (kref->same(ke) == kifTRUE) {
					ke->Release();
					found = true;
					break;
				}
				ke->Release();
			}
			if (found == false)
				ref->Push(kref);
		}

		itr->Release();
		return ref;
	}


	return kifNULL;
}

KifElement* KifVector::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (!a->aContainer())
		return a->orset(a, b, context, autoself);

	KifVector* ref = this;
	if (autoself == false)
		ref = (KifVector*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVector*)a;

	KifVector* A = (KifVector*)a;

	int it;

	KifContainerLock _lock(this);

	KifElement* ke;
	KifElement* kref;
	bool found;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
			kref = itr->IteratorValue();
			found = false;
			for (it = 0; it < A->values.size(); it++) {
				ke = A->ValueOnIndex(it);
				if (kref->same(ke) == kifTRUE) {
					found = true;
					break;
				}
			}
			if (found == false)
				ref->Push(kref);
			kref->Release();
		}
		itr->Release();
		return ref;
	}


	return kifNULL;
}

KifElement* KifVector::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (!a->aContainer())
		return a->andset(a, b, context, autoself);

	KifVector* A = (KifVector*)a;
	int it;
	KifElement* ke;
	KifElement* kref;
	bool found;

	KifContainerLock _lock(this);

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifVector* ref = new KifVector(kifcode, NULL);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
			kref = itr->IteratorValue();
			found = false;
			for (it = 0; it < A->values.size(); it++) {
				ke = A->ValueOnIndex(it);
				if (ke != kifNOELEMENT && kref->same(ke) == kifTRUE) {
					found = true;
					break;
				}
			}

			if (found == true)
				ref->Push(kref);
			kref->Release();
		}
		itr->Release();
		return ref;
	}

	return kifNULL;
}

Exported KifElement* KifVector::same(KifElement* a) {
	if (a->type != type) {
		if (a->aVectorContainer()) {
			if (a->Size() != values.size())
				return kifFALSE;
			KifElement* v;
			for (int i = 0; i < values.size(); i++) {
				v = a->ValueOnIndex(i);
				if (!v->same(values[i])->Boolean()) {
					v->Release();
					return kifFALSE;
				}
				v->Release();
			}
			return kifTRUE;
		}
		return kifFALSE;
	}

	KifVector* v = (KifVector*)a;
	KifContainerLock _lock(this);
	if (values.size() != v->values.size())
		return kifFALSE;

	for (int i = 0; i < values.size(); i++) {
		if (values[i]->same(v->values[i]) == kifFALSE)
			return kifFALSE;
	}
	return kifTRUE;
}

Exported KifElement* KifList::same(KifElement* a) {

	if (a->type != kifList)
		return kifFALSE;
	KifList* v = (KifList*)a;
	KifContainerLock _lock(this);

	if (v->values.size() != values.size())
		return kifFALSE;

	list<KifElement*>::iterator it;
	list<KifElement*>::iterator itv;
	for (it = values.begin(), itv = v->values.begin(); it != values.end(); it++, itv++) {
		if ((*it)->same(*itv) == kifFALSE)
			return kifFALSE;
	}
	return kifTRUE;
}

KifElement* KifVector::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (!a->aContainer())
		return a->plus(a, b, context, autoself);

	KifVector* ref = this;
	if (autoself == false)
		ref = (KifVector*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVector*)a;

	int it;
	KifElement* ke;
	KifContainerLock _lock(this);

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->plus(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}


	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->plus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifVector::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::minus");
#endif

	if (!a->aContainer())
		return a->minus(a, b, context, autoself);

	KifVector* ref = this;
	if (autoself == false)
		ref = (KifVector*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVector*)a;

	int it;
	KifElement* ke;
	KifContainerLock _lock(this);

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->minus(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->minus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifVector::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::multiply");
#endif

	if (!a->aContainer())
		return a->multiply(a, b, context, autoself);

	KifVector* ref = this;
	if (autoself == false)
		ref = (KifVector*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVector*)a;

	int it;
	KifElement* ke;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->multiply(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->multiply(ke, b, context, true);
	}
	return ref;
}

KifElement* KifVector::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (!a->aContainer())
		return a->divide(a, b, context, autoself);

	KifVector* ref = this;
	if (autoself == false)
		ref = (KifVector*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVector*)a;


	int it;
	KifElement* ke;
	int idthread = kifGlobals->GetThreadid();

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->divide(ke, ki, context, true);
			ki->Release();
			if (Errorid(idthread)) {
				itr->Release();
				ref->Release();
				return kifRAISEERROR;
			}
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->divide(ke, b, context, true);
		if (Errorid(idthread)) {
			ref->Release();
			return kifRAISEERROR;
		}
	}
	return ref;
}

KifElement* KifVector::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::mod");
#endif
	if (!a->aContainer())
		return a->mod(a, b, context, autoself);

	KifVector* ref = this;
	if (autoself == false)
		ref = (KifVector*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVector*)a;

	int it;
	KifElement* ke;
	int idthread = kifGlobals->GetThreadid();
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->mod(ke, ki, context, true);
			ki->Release();
			if (Errorid(idthread)) {
				itr->Release();
				ref->Release();
				return kifRAISEERROR;
			}
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->mod(ke, b, context, true);
		if (Errorid(idthread)) {
			ref->Release();
			return kifRAISEERROR;
		}
	}
	return ref;
}

KifElement* KifVector::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::shiftright");
#endif

	if (!a->aContainer())
		return a->shiftright(a, b, context, autoself);

	KifVector* ref = this;
	if (autoself == false)
		ref = (KifVector*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVector*)a;

	int it;
	KifElement* ke;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->shiftright(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->shiftright(ke, b, context, true);
	}
	return ref;
}

KifElement* KifVector::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (!a->aContainer())
		return a->shiftleft(a, b, context, autoself);

	KifVector* ref = this;
	if (autoself == false)
		ref = (KifVector*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVector*)a;

	int it;
	KifElement* ke;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->shiftleft(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->shiftleft(ke, b, context, true);
	}
	return ref;
}

KifElement* KifVector::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::power");
#endif
	if (!a->aContainer())
		return a->power(a, b, context, autoself);

	KifVector* ref = this;
	if (autoself == false)
		ref = (KifVector*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVector*)a;

	int it;
	KifElement* ke;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->power(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->power(ke, b, context, true);
	}
	return ref;
}

KifElement* KifList::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::orset");
#endif

	if (a->type != type)
		return a->xorset(a, b, context, autoself);


	KifList* A = (KifList*)a;

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;
	bool found;

	KifContainerLock _lock(A);

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifList* ref = new KifList(kifcode, NULL);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			kref = itr->IteratorValue();
			for (it = A->values.begin(); it != A->values.end(); it++) {
				ke = *it;
				if (kref->same(ke) == kifTRUE) {
					found = true;
					break;
				}
			}
			if (found == false)
				ref->Push(kref);
			kref->Release();
		}

		for (it = A->values.begin(); it != A->values.end(); it++) {
			kref = *it;
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
				ke = itr->IteratorValue();
				if (kref->same(ke) == kifTRUE) {
					ke->Release();
					found = true;
					break;
				}
				ke->Release();
			}

			if (found == false)
				ref->Push(kref);
		}

		itr->Release();
		return ref;
	}


	return kifNULL;
}

KifElement* KifList::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::orset");
#endif

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifList* ref = this;
	KifList* A = (KifList*)a;
	if (autoself == false)
		ref = (KifList*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifList*)a;

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;
	bool found;
	KifContainerLock _lock(A);

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			kref = itr->IteratorValue();
			for (it = A->values.begin(); it != A->values.end(); it++) {
				ke = *it;
				if (kref->same(ke) == kifTRUE) {
					found = true;
					break;
				}
			}
			if (found == false)
				ref->Push(kref);
			kref->Release();
		}
		itr->Release();
		return ref;
	}


	return kifNULL;
}

KifElement* KifList::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::andset");
#endif

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;
	bool found;
	KifList* A = (KifList*)a;
	KifContainerLock _lock(A);

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifList* ref = new KifList(kifcode, NULL);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			kref = itr->IteratorValue();
			for (it = A->values.begin(); it != A->values.end(); it++) {
				ke = *it;
				if (kref->same(ke) == kifTRUE) {
					found = true;
					break;
				}
			}
			if (found == true)
				ref->Push(kref);
			kref->Release();
		}
		itr->Release();
		return ref;
	}

	return kifNULL;
}

KifElement* KifList::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifList* ref = this;
	if (autoself == false)
		ref = (KifList*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifList*)a;

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = itr->IteratorValue();
			ke = *it;
			ke->plus(ke, kref, context, true);
			kref->Release();
		}
		itr->Release();
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = *it;
		ke->plus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifList::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::minus");
#endif

	if (a->type != type)
		return a->minus(a, b, context, autoself);

	KifList* ref = this;
	if (autoself == false)
		ref = (KifList*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifList*)a;

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = itr->IteratorValue();
			ke = *it;
			ke->minus(ke, kref, context, true);
			kref->Release();
		}
		itr->Release();
		return ref;
	}


	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = *it;
		ke->minus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifList::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::multiply");
#endif

	if (a->type != type)
		return a->multiply(a, b, context, autoself);

	KifList* ref = this;
	if (autoself == false)
		ref = (KifList*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifList*)a;

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = itr->IteratorValue();
			ke = *it;
			ke->multiply(ke, kref, context, true);
			kref->Release();
		}
		itr->Release();
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = *it;
		ke->multiply(ke, b, context, true);
	}
	return ref;
}

KifElement* KifList::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->divide(a, b, context, autoself);

	KifList* ref = this;
	if (autoself == false)
		ref = (KifList*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifList*)a;

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;
	int idthread = kifGlobals->GetThreadid();

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = itr->IteratorValue();
			ke = *it;
			ke->divide(ke, kref, context, true);
			kref->Release();
			if (Errorid(idthread)) {
				itr->Release();
				ref->Release();
				return kifRAISEERROR;
			}
		}
		itr->Release();
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = *it;
		ke->divide(ke, b, context, true);
		if (Errorid(idthread)) {
			ref->Release();
			return kifRAISEERROR;
		}
	}
	return ref;
}

KifElement* KifList::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::mod");
#endif

	if (a->type != type)
		return a->mod(a, b, context, autoself);

	KifList* ref = this;
	if (autoself == false)
		ref = (KifList*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifList*)a;

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;
	int idthread = kifGlobals->GetThreadid();

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = itr->IteratorValue();
			ke = *it;
			ke->mod(ke, kref, context, true);
			kref->Release();
			if (Errorid(idthread)) {
				itr->Release();
				ref->Release();
				return kifRAISEERROR;
			}
		}
		itr->Release();
		return ref;
	}


	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = *it;
		ke->mod(ke, b, context, true);
		if (Errorid(idthread)) {
			ref->Release();
			return kifRAISEERROR;
		}
	}
	return ref;
}

KifElement* KifList::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::shiftright");
#endif

	if (a->type != type)
		return a->shiftright(a, b, context, autoself);

	KifList* ref = this;
	if (autoself == false)
		ref = (KifList*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifList*)a;

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = itr->IteratorValue();
			ke = *it;
			ke->shiftright(ke, kref, context, true);
			kref->Release();
		}
		itr->Release();
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = *it;
		ke->shiftright(ke, b, context, true);
	}
	return ref;
}

KifElement* KifList::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::shiftleft");
#endif

	if (a->type != type)
		return a->shiftleft(a, b, context, autoself);

	KifList* ref = this;
	if (autoself == false)
		ref = (KifList*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifList*)a;

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = itr->IteratorValue();
			ke = *it;
			ke->shiftleft(ke, kref, context, true);
			kref->Release();
		}
		itr->Release();
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = *it;
		ke->shiftleft(ke, b, context, true);
	}
	return ref;
}

KifElement* KifList::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::power");
#endif

	if (a->type != type)
		return a->power(a, b, context, autoself);

	KifList* ref = this;
	if (autoself == false)
		ref = (KifList*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifList*)a;

	list<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;

	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it = ref->values.begin(); it != ref->values.end(); it++) {
			kref = itr->IteratorValue();
			ke = *it;
			ke->power(ke, kref, context, true);
			kref->Release();
		}
		itr->Release();
		return ref;
	}

	for (it = ref->values.begin(); it != ref->values.end(); it++) {
		ke = *it;
		ke->power(ke, b, context, true);
	}
	return ref;
}

KifElement* KifList::Newiterator(bool rev) {
	KifIteratorList* iter = new KifIteratorList(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

KifElement* KifVector::Newiterator(bool rev) {
	KifIteratorVector* iter = new KifIteratorVector(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}


bool KifVector::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;


	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifList) {
			ke->Setpopped(1, true);
			KifList* kvect = (KifList*)ke;
			//We copy all values from ke to this
			Clear();
			Reserve(ke->Size());
			{
				KifContainerLock _lock(kvect);
				list<KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++)
					Push(*it);
			}
			if (ke->popped)
				ke->Setpopped(0, true);
		}
		else {
			if (ke->aVectorContainer()) {
				ke->Setpopped(1, true);
				//We copy all values from ke to this
				Clear();
				Reserve(ke->Size());
				for (int it = 0; it < ke->Size(); it++)
					Push(ke->ValueOnIndex(it));
				if (ke->popped)
					ke->Setpopped(0, true);
			}
			else {
				//We gather all the keys from the MAP
				if (ke->aMapContainer()) {
					ke->Setpopped(1, true);
					Clear();
					Reserve(ke->Size());
					KifIteration* itr = (KifIteration*)ke->Newiterator(false);
					for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
						Push(itr->IteratorKey());
					itr->Release();
					if (ke->popped)
						ke->Setpopped(0, true);
				}
				else {
					ke = ke->Vector(dom, this);
					if (ke == kifNULL) {
						kifcode->Returnerror(KIFLIBSTRINGS[206]);
						return true;
					}
					if (ke != this) {
						Clear();
						Reserve(ke->Size());
						KifVector* kvect = (KifVector*)ke;
						//We copy all values from ke to this
						for (int it = 0; it < kvect->values.size(); it++)
							Push(kvect->values[it]);
					}
				}
			}
		}
		return true;
	}

	//In this specific case, we try to replace a bloc of values with a new bloc
	if (idx->type == kifIndex && ((KifIndex*)idx)->interval) {
		threadblocinit;
		KifContainerLock _lock(this);

		//First we remove the values...
		KifElement* krkey = ((KifIndex*)idx)->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
		int lkey = krkey->Integer();
		krkey->Release();
		if (lkey < 0)
			lkey = values.size() + lkey;

		krkey = ((KifIndex*)idx)->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
		int rkey = krkey->Integer();
		krkey->Release();
		if (rkey < 0)
			rkey = values.size() + rkey;

		if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return true;
		}

		if (rkey != lkey)
			rkey--;

		while (rkey >= lkey) {
			krkey = values[rkey];
			values.erase(values.begin() + rkey);
			krkey->Resetreference(reference + 1);
			rkey--;
		}

		if (ke->aVectorContainer()) {
			for (int i = ke->Size() - 1; i >= 0; i--) {
				krkey = ke->ValueOnIndex(i);
				krkey = krkey->Atom();
				values.insert(values.begin() + lkey, krkey);
				krkey->Setreference(reference + 1);
			}
			return true;
		}

		if (ke->aContainer()) {
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
				krkey = itr->IteratorValue();
				if (krkey->reference)
					krkey = krkey->Atom();
				values.insert(values.begin() + lkey, krkey);
				krkey->Setreference(reference + 1);
			}
			itr->Release();
			return true;
		}


		krkey = ke->Atom();
		values.insert(values.begin() + lkey, krkey);
		krkey->Setreference(reference + 1);
		return true;
	}

	register int ikey = idx->Integer();
	int nb, mx;
	{
		KifContainerLock _lock(this);
		mx = values.size();
	}

	if (ikey >= mx) {
		{
			KifContainerLock _lock(this);
			for (nb = values.size(); nb < ikey; nb++)
				values.push_back(kifNULL);
		}
		Push(ke);
	}
	else {
		if (ikey < 0) {
			ikey = mx + ikey;
			if (ikey < 0)
				kifcode->Returnerror(KIFLIBSTRINGS[206]);
		}

		ke = ke->Atom();
		{
			KifContainerLock _lock(this);
			if (values[ikey] != NULL)
				values[ikey]->Resetreference(reference + 1);
		}
		values[ikey] = ke;
		ke->Setreference(reference + 1);
	}

	return true;
}

bool KifList::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;


	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			KifContainerLock _lock(this);
			Clear();
			return true;
		}

		if (ke->type == kifList) {
			ke->Setpopped(1, true);
			KifList* kvect = (KifList*)ke;
			//We copy all values from ke to this
			Clear();
			{
				KifContainerLock _lock(this);
				list<KifElement*>::iterator it;
				for (it = kvect->values.begin(); it != kvect->values.end(); it++)
					Push(*it);
			}
			if (ke->popped)
				ke->Setpopped(0, true);
		}
		else {
			if (ke->aVectorContainer()) {
				ke->Setpopped(1, true);
				//We copy all values from ke to this
				Clear();
				for (int it = 0; it < ke->Size(); it++)
					Push(ke->ValueOnIndex(it));
				if (ke->popped)
					ke->Setpopped(0, true);
			}
			else {
				//We gather all the keys from the MAP
				if (ke->aMapContainer()) {
					ke->Setpopped(1, true);
					Clear();
					KifIteration* itr = (KifIteration*)ke->Newiterator(false);
					for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
						Push(itr->IteratorKey());
					itr->Release();
					if (ke->popped)
						ke->Setpopped(0, true);
				}
				else {
					ke = ke->Vector(dom);
					if (ke == kifNULL) {
						kifcode->Returnerror(KIFLIBSTRINGS[220]);
						return true;
					}
					Clear();
					KifVector* kvect = (KifVector*)ke;
					//We copy all values from ke to this
					for (int it = 0; it < kvect->values.size(); it++)
						Push(kvect->values[it]);
				}
			}
		}
		return true;
	}

	if (idx->type == kifIndex && ((KifIndex*)idx)->interval) {
		threadblocinit;
		KifContainerLock _lock(this);

		//First we remove the values...
		KifElement* krkey = ((KifIndex*)idx)->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
		int lkey = krkey->Integer();
		krkey->Release();
		if (lkey < 0)
			lkey = values.size() + lkey;

		krkey = ((KifIndex*)idx)->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
		int rkey = krkey->Integer();
		krkey->Release();
		if (rkey < 0)
			rkey = values.size() + rkey;

		if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return true;
		}

		list<KifElement*>::iterator it;
		listValue(it, lkey);
		vector<list<KifElement*>::iterator > vit;
		while (rkey > lkey) {
			krkey = *it;
			vit.push_back(it);
			krkey->Resetreference(reference + 1);
			rkey--;
			it++;
		}

		for (int i = vit.size() - 1; i >= 0; i--)
			values.erase(vit[i]);

		listValue(it, lkey);
		if (ke->aVectorContainer()) {
			for (int i = 0; i < ke->Size(); i++) {
				krkey = ke->ValueOnIndex(i);
				krkey = krkey->Atom();
				values.insert(it, krkey);
				krkey->Setreference(reference + 1);
			}
			return true;
		}

		if (ke->type == kifList) {
			list<KifElement*>::iterator iti;
			for (iti = ((KifList*)ke)->values.begin(); iti != ((KifList*)ke)->values.end(); iti++) {
				krkey = *iti;
				krkey = krkey->Atom();
				values.insert(it, krkey);
				krkey->Setreference(reference + 1);
			}
			return true;
		}

		if (ke->aContainer()) {
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
				krkey = itr->IteratorValue();
				if (krkey->reference)
					krkey = krkey->Atom();
				values.insert(it, krkey);
				krkey->Setreference(reference + 1);
			}
			itr->Release();
			return true;
		}

		krkey = ke->Atom();
		values.insert(it, krkey);
		krkey->Setreference(reference + 1);
		return true;
	}

	register int ikey = idx->Integer();
	list<KifElement*>::iterator it;
	int mx;
	{
		KifContainerLock _lock(this);
		mx = values.size();
	}

	if (ikey == mx)
		Push(ke);
	else {
		if (ikey > mx) {
			{
				KifContainerLock _lock(this);
				for (; mx < ikey; mx++)
					values.push_back(kifNULL);
			}
			Push(ke);
		}
		else {
			if (ikey < 0) {
				ikey = mx + ikey;
				if (ikey < 0)
					kifcode->Returnerror(KIFLIBSTRINGS[206]);
			}
			ke = ke->Atom();
			listValue(it, ikey);
			if (*it != NULL)
				(*it)->Resetreference(reference + 1);
			*it = ke;
			ke->Setreference(reference + 1);
		}
	}

	return true;
}


KifElement* KifVector::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	char ch[20];
	for (int it = 0; it < values.size(); it++) {
		sprintf_s(ch, 20, "%d", it);
		kmap->Push(ch, values[it]);
	}
	return kmap;
}

KifElement* KifList::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	list<KifElement*>::iterator it;
	int nb = 0;
	char ch[20];
	for (it = values.begin(); it != values.end(); it++) {
		sprintf_s(ch, 20, "%d", nb);
		kmap->Push(ch, *it);
		nb++;
	}
	return kmap;
}

KifElement* KifList::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();

	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	list<KifElement*>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
		kvect->Push(*it);
	return kvect;
}


KifElement* KifList::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Vector::Declaration");
#endif
	if (kifGlobals->kifMathFunctions.find(n) != kifGlobals->kifMathFunctions.end() || TestkifFunctions(n, kifGlobals->kifListFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifVector::Declaration(short n) {
#ifdef PROFILING
	profilingcall("Vector::Declaration");
#endif
	if (kifGlobals->kifMathFunctions.find(n) != kifGlobals->kifMathFunctions.end() || TestkifFunctions(n, kifGlobals->kifVectorFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifVector::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifMathFunctions.find(KifNewId(s)) != kifGlobals->kifMathFunctions.end())
		return kifZERO->Information(kstr);

	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

Exported KifElement* KifVectorString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

Exported KifElement* KifVectorInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifMathFunctions.find(KifNewId(s)) != kifGlobals->kifMathFunctions.end())
		return kifZERO->Information(kstr);

	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

Exported KifElement* KifVectorFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifMathFunctions.find(KifNewId(s)) != kifGlobals->kifMathFunctions.end())
		return kifZERO->Information(kstr);

	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

Exported KifElement* KifList::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifMathFunctions.find(KifNewId(s)) != kifGlobals->kifMathFunctions.end())
		return kifZERO->Information(kstr);

	if (kifGlobals->kifInformationList.find(s) == kifGlobals->kifInformationList.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationList[s]);
}

void KifVector::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifVectorFunctions.begin(); it != kifGlobals->kifVectorFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);

	hmap<short, mathFunc>::iterator itm;
	for (itm = kifGlobals->kifMathFunctions.begin(); itm != kifGlobals->kifMathFunctions.end(); itm++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

void KifList::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifListFunctions.begin(); it != kifGlobals->kifListFunctions.end(); it++)
		fcts->Push(kifcode->Providestring(kifGlobals->kifIdString[it->first]));

	hmap<short, mathFunc>::iterator itm;
	for (itm = kifGlobals->kifMathFunctions.begin(); itm != kifGlobals->kifMathFunctions.end(); itm++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

KifElement* KifVector::Copy(KifDomain* kp, KifElement* dom) {
	KifVector* ke = new KifVector(kifcode, NULL);
	KifElement* kx;
	for (int itx = 0; itx < values.size(); itx++) {
		kx = values[itx]->Copy(kp, dom);
		ke->Push(kx);
	}
	return ke;
}

KifElement* KifList::Copy(KifDomain* kp, KifElement* dom) {
	KifElement* ke = new KifList(kifcode, NULL);
	list<KifElement*>::iterator itx;
	KifElement* kx;
	for (itx = values.begin(); itx != values.end(); itx++) {
		kx = (*itx)->Copy(kp, dom);
		ke->Push(kx);
	}

	return ke;
}

void KifVector::addstringto(string s, int i) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	KifElement* ke;
	if (i < 0) {
		ke = values.back();
	}
	else {
		if (i >= values.size())
			return;
		ke = values[i];
	}
	ke->storevalue(s);
}

void KifVector::addustringto(wstring s, int i) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	KifElement* ke;
	if (i < 0) {
		ke = values.back();
	}
	else {
		if (i >= values.size())
			return;
		ke = values[i];
	}
	ke->storevalue(s);
}

void KifVector::addstringto(wchar_t s, int i) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	KifElement* ke;
	if (i < 0) {
		ke = values.back();
	}
	else {
		if (i >= values.size())
			return;
		ke = values[i];
	}
	ke->storevalue(s);
}

void KifVectorString::addstringto(string s, int i) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	if (i < 0)
		values.back() += s;
	else {
		if (i >= values.size())
			return;
		values[i] += s;
	}
}

void KifVectorString::addustringto(wstring ws, int i) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	string s;
	s_unicode_to_utf8(s, ws);
	if (i < 0)
		values.back() += s;
	else {
		if (i >= values.size())
			return;
		values[i] += s;
	}
}

void KifVectorString::addstringto(wchar_t wc, int i) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	string s = c_unicode_to_utf8(wc);
	if (i < 0)
		values.back() += s;
	else {
		if (i >= values.size())
			return;
		values[i] += s;
	}
}

KifElement* KifList::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* res;

	if (b->type = kifList) {
		int itbi = 0;
		KifElement* found = kifFALSE;
		list<KifElement*>::iterator itb;

		if (b != this)
			Clear();

		KifList* bb = (KifList*)b;

		//A sub vector in a vector... Each element of a should be in b, in that order
		if (a->type = kifList) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}

			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		KifContainerLock _lock(bb);
		for (itb = bb->values.begin(); itb != bb->values.end(); itb++) {
			res = *itb;
			if (a->aContainer() && !res->aContainer() && res->type != kifDomain)
				continue;
			res = res->in(a, res, kfunc, this, idx);
			if (res->Succeed()) {
				res->Release();
				if (b != this) {
					KifBaseInteger kint(NULL, NULL, itbi);
					kint.kifcode = kifcode;
					Push(&kint);
					found = this;
				}
				else {
					if (idx == true)
						return kifcode->Provideinteger(itbi);
					return kifTRUE;
				}
			}
			itbi++;
		}
		if (b == this && idx == true)
			return kifMINUSONE;

		return found;
	}

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		Clear();
		KifElement* ke;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			if (res->Succeed()) {
				Push(itr->IteratorKey());
				res->Release();
			}
			ke->Release();
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);
	if (idx == true)
		return kifMINUSONE;
	return kifFALSE;
}

KifElement* KifVector::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	KifElement* res;
	KifElement* ke;

	if (b->aVectorContainer()) {
		KifElement* found = kifFALSE;
		int itb;

		if (b != this)
			Clear();



		//A sub vector in a vector... Each element of a should be in b, in that order
		if (a->aVectorContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		for (itb = 0; itb != b->Size(); itb++) {
			ke = b->ValueOnIndex(itb);
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				res->Release();
				if (b != this) {
					KifBaseInteger kint(NULL, NULL, itb);
					kint.kifcode = kifcode;
					Push(&kint);
					found = this;
				}
				else {
					if (idx == true)
						return kifcode->Provideinteger(itb);
					return kifTRUE;
				}
			}
		}

		if (b == this && idx == true)
			return kifMINUSONE;

		return found;
	}

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			if (res->Succeed()) {
				Push(itr->IteratorKey());
				res->Release();
			}
			ke->Release();
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);
	if (idx == true)
		return kifMINUSONE;
	return kifFALSE;
}

//--------------------------------------------------------------------------------------
Exported void KifList::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror(KIFCONTAINER[0]);
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	list<KifElement*>::iterator itx;
	vector<KifElement*> doubles;
	{
		KifContainerLock _lock(this);
		for (itx = values.begin(); itx != values.end(); itx++)
			doubles.push_back(*itx);
	}
	for (int itx = 0; itx < doubles.size(); itx++)
		doubles[itx]->Setreference(inc);
	idgarbage = id;
}

Exported void KifSet::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror(KIFCONTAINER[0]);
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	kifsetiterator itx;
	vector<KifElement*> doubles;
	{
		KifContainerLock _lock(this);
		for (itx = values.begin(); itx != values.end(); itx++)
			doubles.push_back(*itx);
	}
	for (int itx = 0; itx < doubles.size(); itx++)
		doubles[itx]->Setreference(inc);
	idgarbage = id;
}

Exported void KifVector::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror(KIFCONTAINER[0]);
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	vector<KifElement*> doubles;
	{
		KifContainerLock _lock(this);
		doubles = values;
	}
	for (int itx = 0; itx < doubles.size(); itx++)
		doubles[itx]->Setreference(inc);
	idgarbage = id;
}

static void resetVector(KifVector* kvect, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kvect);
	vector<KifElement*>& values = kvect->values;
	if (values.size() == 0)
		return;

	for (int itx = 0; itx<values.size(); itx++)
		values[itx]->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}

void KifVector::Resetreference(int inc) {
	reference -= inc;
	if (reference>0)
		resetVector(this, inc, popped, false);
	else {
		resetVector(this, inc + 1 + reference - popped, popped, true);
		if (popped == 1)
			popped = 0;
		else
			KifElement::Resetreference(0);
	}
}

static void resetList(KifList* klist, int inc, bool popped, bool clr) {
	KifContainerLock _lock(klist);
	list<KifElement*>& values = klist->values;
	if (values.size() == 0)
		return;

	list<KifElement*>::iterator itx;

	for (itx = values.begin(); itx != values.end(); itx++)
		(*itx)->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}

void KifList::Resetreference(int inc) {
	reference -= inc;
	if (reference > 0)
		resetList(this, inc, popped, false);
	else {
		resetList(this, inc + 1 + reference - popped, popped, true);
		if (popped == 1)
			popped = 0;
		else
			KifElement::Resetreference(0);
	}
}

static void resetSet(KifSet* kset, int inc, bool popped, bool clr) {
	KifContainerLock _lock(kset);
	kifset& values = kset->values;
	if (values.size() == 0)
		return;

	kifsetiterator itx;

	for (itx = values.begin(); itx != values.end(); itx++)
		(*itx)->Resetreference(inc);

	if (clr && !popped)
		values.clear();
}

void KifSet::Resetreference(int inc) {
	reference -= inc;
	if (reference > 0)
		resetSet(this, inc, popped, false);
	else {
		resetSet(this, inc + 1 + reference - popped, popped, true);
		if (popped == 1)
			popped = 0;
		else
			KifElement::Resetreference(0);
	}
}

void KifList::Clear() {
	KifContainerLock _lock(this);

	list<KifElement*>::iterator itx;
	for (itx = values.begin(); itx != values.end(); itx++)
		(*itx)->Resetreference(reference + 1);
	values.clear();
}

void KifVector::Clear() {
	KifContainerLock _lock(this);
	for (int itx = 0; itx < values.size(); itx++)
		values[itx]->Resetreference(reference + 1);
	values.clear();
}

void KifList::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	//Three cases:
	if (ke->type == kifList) {
		KifList* klist = (KifList*)ke;
		list<KifElement*>::iterator it;
		for (it = klist->values.begin(); it != klist->values.end(); it++)
			Push(*it);
		return;
	}

	if (!ke->aContainer()) {
		Push(ke);
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		Push(itr->IteratorValue());
	itr->Release();
}

void KifVector::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	//Three cases:
	if (ke->type == type) {
		KifVector* kvect = (KifVector*)ke;
		for (int i = 0; i < kvect->values.size(); i++)
			Push(kvect->values[i]);
		return;
	}

	if (!ke->aContainer()) {
		Push(ke);
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		Push(itr->IteratorValue());
	itr->Release();
}

void KifVectorString::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	//Three cases:
	if (ke->type == type) {
		KifVectorString* kvect = (KifVectorString*)ke;
		for (int i = 0; i < kvect->values.size(); i++)
			values.push_back(kvect->values[i]);
		return;
	}

	if (!ke->aContainer()) {
		values.push_back(ke->String());
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.push_back(itr->IteratorValueString());
	itr->Release();
}

void KifVectorInteger::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	//Three cases:
	if (ke->type == type) {
		KifVectorInteger* kvect = (KifVectorInteger*)ke;
		for (int i = 0; i < kvect->values.size(); i++)
			values.push_back(kvect->values[i]);
		return;
	}

	if (ke->type == kifVectorFloat) {
		KifVectorFloat* kvect = (KifVectorFloat*)ke;
		for (int i = 0; i < kvect->values.size(); i++)
			values.push_back((long)kvect->values[i]);
		return;
	}

	if (!ke->aContainer()) {
		values.push_back(ke->Integer());
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.push_back(itr->IteratorValueInteger());
	itr->Release();
}

void KifVectorFloat::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	//Three cases:
	if (ke->type == type) {
		KifVectorFloat* kvect = (KifVectorFloat*)ke;
		for (int i = 0; i < kvect->values.size(); i++)
			values.push_back(kvect->values[i]);
		return;
	}

	if (ke->type == kifVectorInteger) {
		KifVectorInteger* kvect = (KifVectorInteger*)ke;
		for (int i = 0; i < kvect->values.size(); i++)
			values.push_back((double)kvect->values[i]);
		return;
	}

	if (!ke->aContainer()) {
		values.push_back(ke->Float());
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.push_back(itr->IteratorValueFloat());
	itr->Release();
}

KifElement* KifList::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {

	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {

		KifElement* ke;
		//In this case, we copy the elements from the vector to the map, using the position as index
		if (contextualpattern->aMapContainer()) {
			KifMap* map = Selectmap(this, contextualpattern);
			size_t i = 0;
			list<KifElement*>::iterator it;
			char ch[20];
			for (it = values.begin(); it != values.end(); it++) {
				sprintf_s(ch, 20, "%d", i);
				ke = *it;
				map->Push(ch, ke);
				i++;
			}
			return map;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());

		//Then some values must be replaced with their evaluation
		if (evaluate == true) {
			KifList* kvect = new KifList(kifcode, NULL);
			list<KifElement*>::iterator it;
			for (it = values.begin(); it != values.end(); it++) {
				ke = (*it)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
				if (ke == kifRAISEERROR) {
					kvect->Release();
					return kifRAISEERROR;
				}
				kvect->Push(ke);
			}
			return kvect;
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

	register int ikey = key->Integer();

	int iright;

	key->Release();
	if (keyright != NULL) {
		iright = keyright->Integer();
		keyright->Release();
	}

	list<KifElement*>::iterator it;
	register int i;
	KifList* kvect;
	if (ikey < 0)
		ikey = values.size() + ikey;
	if (ikey < 0 || ikey >= values.size()) {
		if (ikey != values.size() || keyright == NULL) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	it = values.begin();
	for (i = 0; i < ikey; i++) it++;


	if (keyright == NULL)
		return *it;

	if (iright < 0 || keyright == kifNULL) {
		iright = values.size() + iright;
		if (iright<ikey) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	else {
		if (iright>values.size()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	//In this case, we must create a new list
	kvect = new KifList(kifcode, NULL);
	for (; i < iright; i++, it++)
		kvect->Push(*it);

	return kvect;
}

KifElement* KifVector::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {

		KifElement* ke;
		//In this case, we copy the elements from the vector to the map, using the position as index
		if (contextualpattern->aMapContainer()) {
			KifMap* map = Selectmap(this, contextualpattern);
			size_t i = 0;
			char ch[20];
			for (int it = 0; it < values.size(); it++) {
				sprintf_s(ch, 20, "%d", i);
				ke = values[it];
				map->Push(ch, ke);
				i++;
			}
			return map;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());
		//Then some values must be replaced with their evaluation
		KifVector* kvect;
		int sz;
		if (evaluate == true) {
			kvect = new KifVector(kifcode, NULL);
			sz = values.size();
			for (int it = 0; it < sz; it++) {
				ke = values[it]->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
				if (ke == kifRAISEERROR) {
					kvect->Release();
					return kifRAISEERROR;
				}
				kvect->Push(ke);
			}
			return kvect;
		}

		if (type == kifBaseVector) {
			kvect = new KifVector(kifcode, NULL);

			sz = values.size();
			for (int it = 0; it < sz; it++)
				kvect->Push(values[it]);
			return kvect;
		}

		return this;
	}

	KifElement* key;
	KifElement* keyright = NULL;
	bool sleft = false;
	bool sright = false;

	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;
		sleft = kind->signkey;
		sright = kind->signright;
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true)
			keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	register int ikey;
	bool stringkey = false;
	if (key->type == kifString) {
		string sf = key->String();
		stringkey = true;
		bool found = false;
		if (sleft) {
			for (ikey = values.size() - 1; ikey >= 0; ikey--) {
				if (sf == values[ikey]->String()) {
					found = true;
					break;
				}
			}

		}
		else {
			for (ikey = 0; ikey < values.size(); ikey++) {
				if (sf == values[ikey]->String()) {
					found = true;
					break;
				}
			}
		}
		if (!found) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	else
		ikey = key->Integer();

	key->Release();
	KifVector* kvect;
	int iright;
	if (keyright != NULL) {
		if (keyright->type == kifString) {
			string sf = keyright->String();
			bool found = false;
			if (sright) {
				for (iright = values.size() - 1; iright >= 0; iright--) {
					if (sf == values[iright]->String()) {
						found = true;
						iright++;
						break;
					}
				}
			}
			else {
				for (iright = 0; iright < values.size(); iright++) {
					if (sf == values[iright]->String()) {
						found = true;
						iright++;
						break;
					}
				}
			}
			if (!found) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(KIFLIBSTRINGS[223]);
				return kifNOELEMENT;
			}
		}
		else {
			if (keyright == kifNULL)
				iright = 0;
			else {
				iright = keyright->Integer();
				if (stringkey && iright >= 0)
					iright = ikey + iright + 1;
			}
		}
	}

	if (ikey < 0)
		ikey = values.size() + ikey;

	if (ikey < 0 || ikey >= values.size()) {
		if (ikey != values.size() || keyright == NULL) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}

	if (keyright == NULL)
		return values[ikey];

	keyright->Release();

	if (iright < 0 || keyright == kifNULL) {
		iright = values.size() + iright;
		if (iright<ikey) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	else {
		if (iright>values.size()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}

	//In this case, we must create a new vector
	kvect = new KifVector(kifcode, NULL);
	for (int i = ikey; i < iright; i++)
		kvect->Push(values[i]);

	return kvect;
}

bool compString(string s1, string s2) {
	if (s1<s2)
		return true;
	return false;
}

bool icompString(string s1, string s2) {
	if (s1>s2)
		return true;
	return false;
}

bool compUString(wstring s1, wstring s2) {
	if (s1<s2)
		return true;
	return false;
}

bool icompUString(wstring s1, wstring s2) {
	if (s1>s2)
		return true;
	return false;
}

Exported void KifVectorString::Sort(bool direction) {
	if (direction == false)
		sort(values.begin(), values.end(), compString);
	else
		sort(values.begin(), values.end(), icompString);
}

Exported void KifVectorString::Push(KifElement* ke) {
	if (ke == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values.push_back(ke->String());
}

string KifVectorString::String() {
	string res;
	res = "[";
	bool beg = true;
	string sx;
	for (int i = 0; i < values.size(); i++) {
		if (beg == false)
			res += ",";
		beg = false;
		sx = values[i];
		stringing(res, sx);
	}
	res += "]";
	return res;
}

string KifVectorString::JSonString() {
	string res;
	res = "[";
	bool beg = true;
	string sx;
	for (int i = 0; i < values.size(); i++) {
		if (beg == false)
			res += ",";
		beg = false;
		sx = values[i];
		jstringing(res, sx);
	}
	res += "]";
	return res;
}

KifElement* KifVectorString::Newiterator(bool rev) {
	KifIteratorVectorString* iter = new KifIteratorVectorString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifVectorString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return;
	}

	int i;
	if (ke->isString()) {
		//In that case, we need to get rid of string that matches it...
		string s = ke->String();
		for (i = values.size() - 1; i >= 0; i--) {
			if (values[i] == s)
				values.erase(values.begin() + i);
		}
		return;
	}

	i = ke->Integer();

	if (i == -1)
		values.pop_back();
	else {
		if (values.size() <= i) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return;
		}

		values.erase(values.begin() + i);
	}
}

bool KifVectorString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifVectorString) {
			KifVectorString* kvect = (KifVectorString*)ke;
			//We copy all values from ke to this
			values = kvect->values;
			return true;
		}
		if (ke->aMapContainer()) {
			Clear();
			values.reserve(ke->Size());
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values.push_back(itr->IteratorKeyString());
			itr->Release();
			return true;
		}
		if (ke->aContainer()) {
			Clear();
			values.reserve(ke->Size());
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values.push_back(itr->IteratorValueString());
			itr->Release();
			return true;
		}
		if (ke->isString()) {
			values.clear();
			string sv = ke->String();
			if (!v_comma_split_string(sv, values)) {
				kifcode->Returnerror(KIFLIBSTRINGS[206]);
				return true;
			}
			return true;
		}
		ke = ke->Vector(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(KIFLIBSTRINGS[206]);
			return true;
		}
		Clear();
		values.reserve(ke->Size());
		//We copy all values from ke to this
		for (int it = 0; it < ke->Size(); it++)
			values.push_back(ke->getstring(it));
		ke->Release();
		return true;
	}
	else {
		//In this specific case, we try to replace a bloc of values with a new bloc
		if (idx->type == kifIndex && ((KifIndex*)idx)->interval) {
			threadblocinit;
			KifContainerLock _lock(this);
			//First we remove the values...
			KifElement* krkey = ((KifIndex*)idx)->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			int lkey = krkey->Integer();
			krkey->Release();
			if (lkey < 0)
				lkey = values.size() + lkey;

			krkey = ((KifIndex*)idx)->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			int rkey = krkey->Integer();
			krkey->Release();
			if (rkey < 0)
				rkey = values.size() + rkey;

			if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
				if (kifGlobals->erroronkey)
					kifcode->Returnerror(KIFLIBSTRINGS[223]);
				return true;
			}

			if (rkey != lkey)
				rkey--;

			while (rkey >= lkey) {
				values.erase(values.begin() + rkey);
				rkey--;
			}

			if (ke->aVectorContainer()) {
				for (int i = ke->Size() - 1; i >= 0; i--) {
					values.insert(values.begin() + lkey, ke->getstring(i));
				}
				return true;
			}

			if (ke->aContainer()) {
				KifIteration* itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
					values.insert(values.begin() + lkey, itr->IteratorValueString());
				itr->Release();
				return true;
			}

			values.insert(values.begin() + lkey, ke->String());
		}
		else {
			register int ikey = idx->Integer();
			int sz = values.size();
			int nb;
			KifContainerLock _lock(this);
			string n = ke->String();
			if (ikey < 0) {
				ikey = sz + ikey;
				if (ikey < 0)
					kifcode->Returnerror(KIFLIBSTRINGS[206]);
			}

			if (ikey >= sz) {
				for (nb = values.size(); nb < ikey; nb++)
					values.push_back("");
				values.push_back(n);
			}
			else
				values[ikey] = n;
		}
	}
	return true;
}

KifElement* KifVectorString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifVectorFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifVectorString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifVectorFunctions.begin(); it != kifGlobals->kifVectorFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifVectorString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	//two cases: b is this or not...

	if (b == this) {
		if (a->aContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
			return kifFALSE;
		}

		//then we are looking for a in this...
		string v = a->String();
		for (int i = 0; i < values.size(); i++) {
			if (values[i] == v) {
				if (idx)
					return kifcode->Provideinteger(i);
				return kifTRUE;
			}
		}
		if (idx)
			return kifMINUSONE;
		return kifFALSE;
	}

	//Then we are storing the comparison of a in b into this...
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		values.clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		KifElement* ke;
		KifElement* res;
		KifElement* found = kifFALSE;
		string itb;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				itb = itr->IteratorKeyString();
				values.push_back(itb);
				found = this;
				res->Release();
			}
		}
		return found;
	}

	return b->in(a, b, kfunc, contextualpattern, true);
}

KifElement* KifVectorString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvectorstring(this, contextualpattern);
			if (kv->type == kifVectorString)
				((KifVectorString*)kv)->values = values;
			else
			for (int i = 0; i < values.size(); i++)
				kv->Push(kifcode->Providestringraw(values[i]));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());
		return this;
	}

	KifElement* key;
	KifElement* keyright = NULL;
	bool sleft = false;
	bool sright = false;

	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;
		sleft = kind->signkey;
		sright = kind->signright;
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true)
			keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	register int ikey;
	bool stringkey = false;
	if (key->type == kifString) {
		stringkey = true;
		string sf = key->String();
		bool found = false;
		if (sleft) {
			for (ikey = values.size() - 1; ikey >= 0; ikey--) {
				if (sf == values[ikey]) {
					found = true;
					break;
				}
			}
		}
		else {
			for (ikey = 0; ikey < values.size(); ikey++) {
				if (sf == values[ikey]) {
					found = true;
					break;
				}
			}
		}
		if (!found) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	else
		ikey = key->Integer();

	key->Release();
	int iright;
	if (keyright != NULL) {
		if (keyright->type == kifString) {
			string sf = keyright->String();
			bool found = false;
			if (sright) {
				for (iright = values.size() - 1; iright >= 0; iright--) {
					if (sf == values[iright]) {
						found = true;
						iright++;
						break;
					}
				}
			}
			else {
				for (iright = 0; iright < values.size(); iright++) {
					if (sf == values[iright]) {
						found = true;
						iright++;
						break;
					}
				}
			}
			if (!found) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(KIFLIBSTRINGS[223]);
				return kifNOELEMENT;
			}
		}
		else {
			if (keyright == kifNULL)
				iright = 0;
			else {
				iright = keyright->Integer();
				if (stringkey && iright >= 0)
					iright = ikey + iright + 1;
			}
		}
	}

	KifVectorString* kvect;
	if (ikey < 0)
		ikey = values.size() + ikey;

	if (ikey < 0 || ikey >= values.size()) {
		if (ikey != values.size() || keyright == NULL) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}

	if (keyright == NULL)
		return kifcode->Providestring(values[ikey]);

	keyright->Release();
	if (iright < 0 || keyright == kifNULL) {
		iright = values.size() + iright;
		if (iright<ikey) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	else  {
		if (iright>values.size()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}

	//In this case, we must create a new vector
	kvect = new KifVectorString(kifcode, NULL);
	for (int i = ikey; i < iright; i++)
		kvect->values.push_back(values[i]);
	return kvect;
}

KifElement* KifVectorString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	char buff[100];
	for (int it = 0; it < values.size(); it++) {
		sprintf_s(buff, 100, "%d", it);
		kmap->Push(buff, kifcode->Providestring(values[it]));
	}
	return kmap;
}

KifElement* KifVectorString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	for (int i = 0; i < values.size(); i++)
		kvect->Push(kifcode->Providestring(values[i]));
	return kvect;
}

Exported KifElement* KifVectorString::same(KifElement* a) {

	if (a->type != type) {
		if (a->aVector()) {
			if (a->Size() != values.size())
				return kifFALSE;
			for (int i = 0; i < values.size(); i++) {
				if (values[i] != ((KifVector*)a)->values[i]->String())
					return kifFALSE;
			}
			return kifTRUE;
		}
		return kifFALSE;
	}

	KifContainerLock _lock(this);
	KifVectorString* m = (KifVectorString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	for (int i = 0; i < values.size(); i++) {
		if (m->values[i] != values[i])
			return kifFALSE;
	}
	return kifTRUE;
}

KifElement* KifVectorString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);

	if (b->aVectorContainer()) {
		KifContainerLock _lock((KifEvaluate*)b);
		bool found = false;
		KifVectorString* A = (KifVectorString*)a;
		KifVectorString* ref = (KifVectorString*)a->Copy(NULL);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
			found = false;
			for (int it = 0; it < A->values.size(); it++) {
				if (A->values[it] == itr->IteratorValueString()) {
					found = true;
					break;
				}
			}
			if (found == false)
				ref->values.push_back(itr->IteratorValueString());
		}
		itr->Release();
		if (autoself && a == this) {
			values = ref->values;
			ref->Release();
			return this;
		}

		return ref;
	}
	return kifNULL;
}

KifElement* KifVectorString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);
	KifContainerLock _lock(this);

	if (b->aVectorContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifVectorString* A = (KifVectorString*)a;
		KifVectorString* ref = new KifVectorString(kifcode, NULL);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);

		for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
			for (int it = 0; it < A->values.size(); it++) {
				if (A->values[it] == itr->IteratorValueString()) {
					ref->values.push_back(itr->IteratorValueString());
					break;
				}
			}
		}
		itr->Release();

		if (autoself && a == this) {
			values = ref->values;
			ref->Release();
			return this;
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifVectorString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifContainerLock _lock(this);

	if (b->aVectorContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		bool found = false;
		vector<string>& val = ((KifVectorString*)a)->values;
		KifVectorString* ref;
		hmap<string, bool> kept;
		int itx;
		if (b->type == kifVectorString) {
			ref = (KifVectorString*)b;
			for (itx = 0; itx < ref->values.size(); itx++)
				kept[ref->values[itx]] = true;
		}
		else {
			KifIteration* itr = (KifIteration*)b->Newiterator(false);
			for (itr->Begin(); itr->End() != kifTRUE; itr->Next())
				kept[itr->IteratorValueString()] = true;
			itr->Release();
		}

		for (itx = 0; itx < val.size(); itx++) {
			if (kept.find(val[itx]) == kept.end())
				kept[val[itx]] = true;
			else
				kept.erase(val[itx]);
		}

		ref = new KifVectorString(kifcode, NULL);
		hmap<string, bool>::iterator it;
		for (it = kept.begin(); it != kept.end(); it++)
			ref->values.push_back(it->first);

		if (autoself && a == this) {
			values = ref->values;
			ref->Release();
			return this;
		}
		return ref;
	}

	return kifNULL;
}

KifElement* KifVectorString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorString* ref = this;
	if (autoself == false)
		ref = (KifVectorString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorString*)a;

	KifContainerLock _lock(this);
	int it;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] += itr->IteratorValueString();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	string v = b->String();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] += v;
	return ref;
}

KifElement* KifVectorString::Copy(KifDomain* kp, KifElement* dom) {
	KifVectorString* ke = new KifVectorString(kifcode, NULL);
	ke->values = values;
	return ke;
}

Exported void KifVectorInteger::Push(KifElement* ke) {
	if (ke == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values.push_back(ke->Integer());
}

bool compInteger(long s1, long s2) {
	if (s1<s2)
		return true;
	return false;
}

bool icompInteger(long s1, long s2) {
	if (s1>s2)
		return true;
	return false;
}

Exported void KifVectorInteger::Sort(bool direction) {
	if (direction == false)
		sort(values.begin(), values.end(), compInteger);
	else
		sort(values.begin(), values.end(), icompInteger);
}

string KifVectorInteger::String() {
	stringstream res;
	res << "[";
	bool beg = true;
	for (int i = 0; i < values.size(); i++) {
		if (beg == false)
			res << ",";
		else
			beg = false;
		res << values[i];
	}
	res << "]";
	return res.str();
}

KifElement* KifVectorInteger::Newiterator(bool rev) {
	KifIteratorVectorInteger* iter = new KifIteratorVectorInteger(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifVectorInteger::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return;
	}

	int i = ke->Integer();
	if (i == -1)
		values.pop_back();
	else {
		if (values.size() <= i) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return;
		}
		values.erase(values.begin() + i);
	}
}

bool KifVectorInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifVectorInteger) {
			KifVectorInteger* kvect = (KifVectorInteger*)ke;
			//We copy all values from ke to this
			values = kvect->values;
			return true;
		}

		if (ke->aMapContainer()) {
			Clear();
			values.reserve(ke->Size());
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values.push_back(itr->IteratorKeyInteger());
			itr->Release();
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			values.reserve(ke->Size());
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values.push_back(itr->IteratorValueInteger());
			itr->Release();
			return true;
		}
		if (ke->isString()) {
			values.clear();
			string sv = ke->String();
			if (!v_comma_split_int(sv, values)) {
				kifcode->Returnerror(KIFLIBSTRINGS[206]);
				return true;
			}
			return true;
		}
		ke = ke->Vector(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(KIFLIBSTRINGS[206]);
			return true;
		}
		Clear();
		values.reserve(ke->Size());
		//We copy all values from ke to this
		for (int it = 0; it < ke->Size(); it++)
			values.push_back(ke->getinteger(it));
		ke->Release();
		return true;
	}
	else {
		//In this specific case, we try to replace a bloc of values with a new bloc
		if (idx->type == kifIndex && ((KifIndex*)idx)->interval) {
			threadblocinit;
			KifContainerLock _lock(this);
			//First we remove the values...
			KifElement* krkey = ((KifIndex*)idx)->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			int lkey = krkey->Integer();
			krkey->Release();
			if (lkey < 0)
				lkey = values.size() + lkey;

			krkey = ((KifIndex*)idx)->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			int rkey = krkey->Integer();
			krkey->Release();
			if (rkey < 0)
				rkey = values.size() + rkey;

			if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
				if (kifGlobals->erroronkey)
					kifcode->Returnerror(KIFLIBSTRINGS[223]);
				return true;
			}

			if (rkey != lkey)
				rkey--;

			while (rkey >= lkey) {
				values.erase(values.begin() + rkey);
				rkey--;
			}
			if (ke->aVectorContainer()) {
				for (int i = ke->Size() - 1; i >= 0; i--) {
					values.insert(values.begin() + lkey, ke->getinteger(i));
				}
				return true;
			}

			if (ke->aContainer()) {
				KifIteration* itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
					values.insert(values.begin() + lkey, itr->IteratorValueInteger());
				itr->Release();
				return true;
			}

			values.insert(values.begin() + lkey, ke->Integer());
		}
		else {
			register int ikey = idx->Integer();
			int sz = values.size();
			int nb;
			KifContainerLock _lock(this);
			long n = ke->Integer();
			if (ikey < 0) {
				ikey = sz + ikey;
				if (ikey < 0)
					kifcode->Returnerror(KIFLIBSTRINGS[206]);
			}

			if (ikey >= sz) {
				for (nb = values.size(); nb < ikey; nb++)
					values.push_back(0);
				values.push_back(n);
			}
			else
				values[ikey] = n;
		}
	}
	return true;
}

KifElement* KifVectorInteger::Declaration(short n) {
	if (kifGlobals->kifMathFunctions.find(n) != kifGlobals->kifMathFunctions.end() || TestkifFunctions(n, kifGlobals->kifVectorFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifVectorInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifVectorFunctions.begin(); it != kifGlobals->kifVectorFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));

	hmap<short, mathFunc>::iterator itm;
	for (itm = kifGlobals->kifMathFunctions.begin(); itm != kifGlobals->kifMathFunctions.end(); itm++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

KifElement* KifVectorInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	//two cases: b is this or not...

	if (b == this) {
		if (a->aContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
			return kifFALSE;
		}

		//then we are looking for a in this...
		long v = a->Integer();
		for (int i = 0; i < values.size(); i++) {
			if (values[i] == v) {
				if (idx)
					return kifcode->Provideinteger(i);
				return kifTRUE;
			}
		}
		if (idx)
			return kifMINUSONE;
		return kifFALSE;
	}

	//Then we are storing the comparison of a in b into this...
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		values.clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		KifElement* ke;
		KifElement* res;
		KifElement* found = kifFALSE;
		long itb;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				itb = itr->IteratorKeyInteger();
				values.push_back(itb);
				found = this;
				res->Release();
			}
		}
		return found;
	}

	return b->in(a, b, kfunc, contextualpattern, true);
}

KifElement* KifVectorInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvectorinteger(this, contextualpattern);
			if (kv->type == kifVectorInteger)
				((KifVectorInteger*)kv)->values = values;
			else
			for (int i = 0; i < values.size(); i++)
				kv->Push(kifcode->Provideinteger(values[i]));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());
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

	register int ikey;
	ikey = key->Integer();

	key->Release();
	int iright;
	if (keyright != NULL)
		iright = keyright->Integer();

	KifVectorInteger* kvect;
	if (ikey < 0)
		ikey = values.size() + ikey;

	if (ikey < 0 || ikey >= values.size()) {
		if (ikey != values.size() || keyright == NULL) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}

	if (keyright == NULL)
		return kifcode->Provideinteger(values[ikey]);

	keyright->Release();
	if (iright < 0 || keyright == kifNULL) {
		iright = values.size() + iright;
		if (iright<ikey) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	else {
		if (iright>values.size()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	//In this case, we must create a new vector
	kvect = new KifVectorInteger(kifcode, NULL);
	for (int i = ikey; i < iright; i++)
		kvect->values.push_back(values[i]);
	return kvect;
}

KifElement* KifVectorInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	char buff[100];
	for (int it = 0; it < values.size(); it++) {
		sprintf_s(buff, 100, "%d", it);
		kmap->Push(buff, kifcode->Provideinteger(values[it]));
	}
	return kmap;
}

KifElement* KifVectorInteger::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	for (int i = 0; i < values.size(); i++)
		kvect->Push(kifcode->Provideinteger(values[i]));
	return kvect;
}

Exported KifElement* KifVectorInteger::same(KifElement* a) {

	if (a->type != type) {
		if (a->aVector()) {
			if (a->Size() != values.size())
				return kifFALSE;
			for (int i = 0; i < values.size(); i++) {
				if (values[i] != ((KifVector*)a)->values[i]->Integer())
					return kifFALSE;
			}
			return kifTRUE;
		}
		if (a->type == kifVectorFloat) {
			if (a->Size() != values.size())
				return kifFALSE;
			for (int i = 0; i < values.size(); i++) {
				if (((KifVectorFloat*)a)->values[i] != values[i])
					return kifFALSE;
			}
			return kifTRUE;
		}
		return kifFALSE;
	}

	KifContainerLock _lock(this);
	KifVectorInteger* m = (KifVectorInteger*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	for (int i = 0; i < values.size(); i++) {
		if (m->values[i] != values[i])
			return kifFALSE;
	}
	return kifTRUE;
}

KifElement* KifVectorInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] += itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	long v = b->Integer();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] += v;
	return ref;
}

KifElement* KifVectorInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] -= itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	long v = b->Integer();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] -= v;
	return ref;
}

KifElement* KifVectorInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] *= itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	long v = b->Integer();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] *= v;
	return ref;
}

KifElement* KifVectorInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;
	long v;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = itr->IteratorValueInteger();
			if (v == 0) {
				itr->Release();
				ref->Release();
				return kifcode->Returnerror(KIFLIBSTRINGS[225]);
			}
			ref->values[it] /= v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] /= v;
	return ref;
}

KifElement* KifVectorInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;
	long v;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = itr->IteratorValueInteger();
			if (v == 0) {
				itr->Release();
				ref->Release();
				return kifcode->Returnerror(KIFLIBSTRINGS[225]);
			}
			ref->values[it] %= v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);

	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] %= v;

	return ref;
}

KifElement* KifVectorInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] >>= itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	long v = b->Integer();

	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] >>= v;
	return ref;
}

KifElement* KifVectorInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] <<= itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	long v = b->Integer();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] <<= v;

	return ref;
}

KifElement* KifVectorInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] = pow((double)ref->values[it], itr->IteratorValueFloat());
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	double v = b->Float();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] = pow((double)ref->values[it], v);
	return ref;
}

KifElement* KifVectorInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] |= itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	long v = b->Integer();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] |= v;
	return ref;
}

KifElement* KifVectorInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] &= itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	long v = b->Integer();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] &= v;
	return ref;
}

KifElement* KifVectorInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifVectorInteger* ref = this;
	if (autoself == false)
		ref = (KifVectorInteger*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorInteger*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] ^= itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	long v = b->Integer();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] ^= v;
	return ref;
}

KifElement* KifVectorInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifVectorInteger* ke = new KifVectorInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}

Exported void KifVectorFloat::Push(KifElement* ke) {
	if (ke == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values.push_back(ke->Float());

}

bool compFloat(double s1, double s2) {
	if (s1<s2)
		return true;
	return false;
}

bool icompFloat(double s1, double s2) {
	if (s1>s2)
		return true;
	return false;
}

Exported void KifVectorFloat::Sort(bool direction) {
	if (direction == false)
		sort(values.begin(), values.end(), compFloat);
	else
		sort(values.begin(), values.end(), icompFloat);
}

string KifVectorFloat::String() {
	stringstream res;
	res << "[";
	bool beg = true;
	for (int i = 0; i < values.size(); i++) {
		if (beg == false)
			res << ",";
		else
			beg = false;
		res << values[i];
	}
	res << "]";
	return res.str();
}

KifElement* KifVectorFloat::Newiterator(bool rev) {
	KifIteratorVectorFloat* iter = new KifIteratorVectorFloat(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifVectorFloat::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return;
	}

	int i = ke->Integer();
	if (i == -1)
		values.pop_back();
	else {
		if (values.size() <= i) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return;
		}
		values.erase(values.begin() + i);
	}
}

bool KifVectorFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifVectorFloat) {
			KifVectorFloat* kvect = (KifVectorFloat*)ke;
			//We copy all values from ke to this
			values = kvect->values;
			return true;
		}

		if (ke->aMapContainer()) {
			Clear();
			values.reserve(ke->Size());
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values.push_back(itr->IteratorKeyFloat());
			itr->Release();
			return true;
		}
		if (ke->aContainer()) {
			Clear();
			values.reserve(ke->Size());
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values.push_back(itr->IteratorValueFloat());
			itr->Release();
			return true;
		}
		if (ke->isString()) {
			values.clear();
			string sv = ke->String();
			if (!v_comma_split_float(sv, values)) {
				kifcode->Returnerror(KIFLIBSTRINGS[206]);
				return true;
			}
			return true;
		}
		ke = ke->Vector(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(KIFLIBSTRINGS[206]);
			return true;
		}
		Clear();
		values.reserve(ke->Size());
		//We copy all values from ke to this
		for (int it = 0; it < ke->Size(); it++)
			values.push_back(ke->getfloat(it));
		ke->Release();
		return true;
	}
	else {
		//In this specific case, we try to replace a bloc of values with a new bloc
		if (idx->type == kifIndex && ((KifIndex*)idx)->interval) {
			threadblocinit;
			KifContainerLock _lock(this);

			//First we remove the values...
			KifElement* krkey = ((KifIndex*)idx)->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			int lkey = krkey->Integer();
			krkey->Release();
			if (lkey < 0)
				lkey = values.size() + lkey;

			krkey = ((KifIndex*)idx)->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			int rkey = krkey->Integer();
			krkey->Release();
			if (rkey < 0)
				rkey = values.size() + rkey;

			if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
				if (kifGlobals->erroronkey)
					kifcode->Returnerror(KIFLIBSTRINGS[223]);
				return true;
			}

			if (rkey != lkey)
				rkey--;

			while (rkey >= lkey) {
				values.erase(values.begin() + rkey);
				rkey--;
			}
			if (ke->aVectorContainer()) {
				for (int i = ke->Size() - 1; i >= 0; i--) {
					values.insert(values.begin() + lkey, ke->getfloat(i));
				}
				return true;
			}

			if (ke->aContainer()) {
				KifIteration* itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
					values.insert(values.begin() + lkey, itr->IteratorValueFloat());
				itr->Release();
				return true;
			}

			values.insert(values.begin() + lkey, ke->Float());
		}
		else {
			register int ikey = idx->Integer();
			int sz = values.size();
			int nb;
			KifContainerLock _lock(this);
			double n = ke->Float();
			if (ikey < 0) {
				ikey = sz + ikey;
				if (ikey < 0)
					kifcode->Returnerror(KIFLIBSTRINGS[206]);
			}

			if (ikey >= sz) {
				for (nb = values.size(); nb < ikey; nb++)
					values.push_back(0);
				values.push_back(n);
			}
			else
				values[ikey] = n;
		}
	}
	return true;
}

KifElement* KifVectorFloat::Declaration(short n) {
	if (kifGlobals->kifMathFunctions.find(n) != kifGlobals->kifMathFunctions.end() || TestkifFunctions(n, kifGlobals->kifVectorFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifVectorFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifVectorFunctions.begin(); it != kifGlobals->kifVectorFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));

	hmap<short, mathFunc>::iterator itm;
	for (itm = kifGlobals->kifMathFunctions.begin(); itm != kifGlobals->kifMathFunctions.end(); itm++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}


KifElement* KifVectorFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	//two cases: b is this or not...

	if (b == this) {
		//then we are looking for a in this...
		if (a->aContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
			return kifFALSE;
		}
		double v = a->Float();
		for (int i = 0; i < values.size(); i++) {
			if (values[i] == v) {
				if (idx)
					return kifcode->Provideinteger(i);
				return kifTRUE;
			}
		}
		if (idx)
			return kifMINUSONE;
		return kifFALSE;
	}

	//Then we are storing the comparison of a in b into this...
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		values.clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		KifElement* ke;
		KifElement* res;
		KifElement* found = kifFALSE;
		long itb;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				itb = itr->IteratorKeyInteger();
				values.push_back(itb);
				found = this;
				res->Release();
			}
		}
		return found;
	}

	return b->in(a, b, kfunc, contextualpattern, true);
}

KifElement* KifVectorFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvectorfloat(this, contextualpattern);
			if (kv->type == kifVectorFloat)
				((KifVectorFloat*)kv)->values = values;
			else
			for (int i = 0; i < values.size(); i++)
				kv->Push(kifcode->Providefloat(values[i]));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());
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

	register int ikey;
	ikey = key->Integer();

	key->Release();
	int iright;
	if (keyright != NULL)
		iright = keyright->Integer();

	KifVectorFloat* kvect;
	if (ikey < 0)
		ikey = values.size() + ikey;

	if (ikey < 0 || ikey >= values.size()) {
		if (ikey != values.size() || keyright == NULL) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}

	if (keyright == NULL)
		return kifcode->Providefloat(values[ikey]);

	keyright->Release();
	if (iright < 0 || keyright == kifNULL) {
		iright = values.size() + iright;
		if (iright<ikey) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	else {
		if (iright>values.size()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	//In this case, we must create a new vector
	kvect = new KifVectorFloat(kifcode, NULL);
	for (int i = ikey; i < iright; i++)
		kvect->values.push_back(values[i]);
	return kvect;
}

KifElement* KifVectorFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	char buff[100];
	for (int it = 0; it < values.size(); it++) {
		sprintf_s(buff, 100, "%d", it);
		kmap->Push(buff, kifcode->Providefloat(values[it]));
	}
	return kmap;
}

KifElement* KifVectorFloat::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	for (int i = 0; i < values.size(); i++)
		kvect->Push(kifcode->Providefloat(values[i]));
	return kvect;
}

Exported KifElement* KifVectorFloat::same(KifElement* a) {

	if (a->type != type) {
		if (a->aVector()) {
			if (a->Size() != values.size())
				return kifFALSE;
			for (int i = 0; i < values.size(); i++) {
				if (values[i] != ((KifVector*)a)->values[i]->Float())
					return kifFALSE;
			}
			return kifTRUE;
		}
		if (a->type == kifVectorInteger) {
			if (a->Size() != values.size())
				return kifFALSE;
			for (int i = 0; i < values.size(); i++) {
				if (((KifVectorInteger*)a)->values[i] != values[i])
					return kifFALSE;
			}
			return kifTRUE;
		}
		return kifFALSE;
	}

	KifContainerLock _lock(this);
	KifVectorFloat* m = (KifVectorFloat*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	for (int i = 0; i < values.size(); i++) {
		if (m->values[i] != values[i])
			return kifFALSE;
	}
	return kifTRUE;
}

KifElement* KifVectorFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] += itr->IteratorValueFloat();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	double v = b->Float();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] += v;
	return ref;
}

KifElement* KifVectorFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	int it;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] -= itr->IteratorValueFloat();
			itr->Next();
		}
		itr->Release();
		return ref;
	}


	double v = b->Float();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] -= v;
	return ref;
}

KifElement* KifVectorFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	int it;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] *= itr->IteratorValueFloat();
			itr->Next();
		}
		itr->Release();
		return ref;
	}


	double v = b->Float();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] *= v;
	return ref;
}

KifElement* KifVectorFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	int it;
	double v;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = itr->IteratorValueFloat();
			if (v == 0) {
				itr->Release();
				ref->Release();
				return kifcode->Returnerror(KIFLIBSTRINGS[225]);
			}
			ref->values[it] /= v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}


	v = b->Float();
	if (v == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] /= v;
	return ref;
}

KifElement* KifVectorFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	int it;
	long v;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = itr->IteratorValueInteger();
			if (v == 0) {
				ref->Release();
				itr->Release();
				return kifcode->Returnerror(KIFLIBSTRINGS[225]);
			}

			ref->values[it] = (long)ref->values[it] % v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Integer();
	if (v == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	long vv;
	for (it = 0; it < ref->values.size(); it++) {
		vv = ((long)ref->values[it]) % v;
		ref->values[it] = (double)vv;
	}
	return ref;
}

KifElement* KifVectorFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	int it;
	long v;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = itr->IteratorValueInteger();
			ref->values[it] = (long)ref->values[it] >> v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Integer();
	long vv;
	for (it = 0; it < ref->values.size(); it++) {
		vv = ((long)ref->values[it]) >> v;
		ref->values[it] = (double)vv;
	}
	return ref;
}

KifElement* KifVectorFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	int it;
	long v;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = itr->IteratorValueInteger();
			ref->values[it] = (long)ref->values[it] << v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Integer();
	long vv;
	for (it = 0; it < ref->values.size(); it++) {
		vv = ((long)ref->values[it]) << v;
		ref->values[it] = (double)vv;
	}
	return ref;
}

KifElement* KifVectorFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	int it;
	double v;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = itr->IteratorValueFloat();
			ref->values[it] = pow(ref->values[it], v);
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Float();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] = pow(ref->values[it], v);
	return ref;
}

KifElement* KifVectorFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	int it;
	long v;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = itr->IteratorValueInteger();
			ref->values[it] = (long)ref->values[it] | v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Integer();
	long vv;
	for (it = 0; it < ref->values.size(); it++) {
		vv = ((long)ref->values[it]) | v;
		ref->values[it] = (double)vv;
	}
	return ref;
}

KifElement* KifVectorFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	int it;
	long v;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = itr->IteratorValueInteger();
			ref->values[it] = (long)ref->values[it] & v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Integer();
	long vv;
	for (it = 0; it < ref->values.size(); it++) {
		vv = ((long)ref->values[it])&v;
		ref->values[it] = (double)vv;
	}
	return ref;
}

KifElement* KifVectorFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifVectorFloat* ref = this;
	if (autoself == false)
		ref = (KifVectorFloat*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorFloat*)a;

	int it;
	long v;
	KifContainerLock _lock(this);
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = itr->IteratorValueInteger();
			ref->values[it] = (long)ref->values[it] ^ v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Integer();
	long vv;
	for (it = 0; it < ref->values.size(); it++) {
		vv = ((long)ref->values[it]) ^ v;
		ref->values[it] = (double)vv;
	}
	return ref;
}


KifElement* KifVectorFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifVectorFloat* ke = new KifVectorFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//---------------------------------------KifTree---------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

Exported void KifTree::Flatten(KifVector* kvect, KifTree* t) {
	if (t == NULL)
		return;
	kvect->Push(t);
	Flatten(kvect, t->child);
	Flatten(kvect, t->next);
}


long KifTree::Computesize(KifTree* t) {
	if (t == NULL)
		return 0;
	long sz = 1;
	sz += Computesize(t->child);
	sz += Computesize(t->next);
	return(sz);
}

long KifTree::Size() {
	return Computesize(this);
}

Exported KifElement* KifTree::Vector(KifElement* contextualpattern, KifVector* v) {
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(Size());
	Flatten(kvect, this);
	return kvect;
}

Exported void KifTree::Mapping(KifMap* kmap, KifTree* t) {
	if (t == NULL)
		return;
	char ch[20];
	sprintf_s(ch, 20, "%d", t->hight);
	KifVector* kvect;

	if (kmap->values.find(ch) == kmap->values.end()) {
		kvect = new KifVector(kifcode, NULL);
		kmap->Push(ch, kvect);
	}
	else
		kvect = (KifVector*)kmap->values[ch];

	kvect->Push(t);
	Mapping(kmap, t->child);
	Mapping(kmap, t->next);
}

Exported KifElement* KifTree::Map(KifElement*, KifMap* m) {
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = new KifMap(kifcode, NULL);
	else
		kmap->Clear();
	Mapping(kmap, this);
	return kmap;
}

//-------------------------------------------------------------------------------------------------
void KifTree::Setreference(int inc) {
	reference += inc;
	value->Setreference(inc);
	if (next != NULL)
		next->Setreference(inc);
	if (child != NULL)
		child->Setreference(inc);
}

void KifTree::Resetreference(int inc) {
	value->Resetreference(inc);
	if (next != NULL)
		next->Resetreference(inc);
	if (child != NULL)
		child->Resetreference(inc);
	KifElement::Resetreference(inc);
}

Exported void KifTree::Setpopped(int v, bool force) {
	if (reference <= 0 || force) {
		Putpopped(v);
		value->Setpopped(v, force);
		if (next != NULL)
			next->Setpopped(v, force);
		if (child != NULL)
			child->Setpopped(v, force);
	}
}

//-------------------------------------------------------------------------------------------------
Exported  KifElement* KifTree::TreeAddchild(KifTree* t) {
	if (t == NULL)
		return kifFALSE;
	Setpopped(1, true);
	t->TreeExtract(this);
	t->SetHight(hight + 1);
	t->parent = this;
	if (last == NULL) {
		child = t;
		last = t;
	}
	else {
		t->previous = last;
		last->next = t;
		last = t;
	}
	t->Setreference(reference);
	return kifTRUE;
}

Exported KifElement* KifTree::TreeAddnext(KifTree* t) {
	if (t == NULL)
		return kifFALSE;
	Setpopped(1, true);
	t->TreeExtract(this);
	t->SetHight(hight);
	t->previous = this;
	t->next = next;
	if (next != NULL)
		next->previous = t;
	next = t;
	t->parent = parent;
	if (parent != NULL && parent->last == this)
		parent->last = t;
	t->Setreference(reference);
	return kifTRUE;
}

Exported KifElement* KifTree::TreeAddprevious(KifTree* t) {
	if (t == NULL)
		return kifFALSE;
	Setpopped(1, true);
	t->TreeExtract(this);
	t->SetHight(hight);
	t->next = this;
	t->parent = parent;
	t->previous = previous;
	if (previous != NULL)
		previous->next = t;
	previous = t;
	if (parent != NULL && parent->child == this)
		parent->child = t;
	t->Setreference(reference);
	return kifTRUE;
}

Exported void KifTree::SetHight(int seed) {
	hight = seed;
	if (child != NULL)
		child->SetHight(seed + 1);
	if (next != NULL)
		next->SetHight(seed);
}

Exported KifElement* KifTree::TreeExtract(KifTree* node) {
	if (node == this)
		return kifcode->Returnerror(KIFLIBSTRINGS[205]);
	if (parent != NULL) {
		if (node != NULL) {
			KifTree* p = node->parent;
			//We put a constraint, which is that is the current node is under the node,
			//which it should play with, then we return an error
			while (p != NULL) {
				if (p == this)
					return kifcode->Returnerror(KIFLIBSTRINGS[204]);
				p = p->parent;
			}
		}
		if (parent->child == this)
			parent->child = next;
		if (parent->last == this)
			parent->last = previous;
	}

	if (next != NULL) {
		if (next->previous != NULL)
			next->previous = previous;
	}

	if (previous != NULL) {
		if (previous->next != NULL)
			previous->next = next;
	}

	next = NULL;
	previous = NULL;
	parent = NULL;
	if (node == NULL)
		SetHight(0);
	return this;
}

Exported KifElement* KifTree::TreeRemove() {
	if (child != NULL) {
		while (child != NULL) {
			child->TreeRemove();
			child = child->next;
		}
	}
	if (next != NULL) {
		if (next->previous != NULL)
			next->previous = previous;
	}
	if (previous != NULL) {
		if (previous->next != NULL)
			previous->next = next;
	}
	if (parent != NULL) {
		if (parent->child == this)
			parent->child = next;
		if (parent->last == this)
			parent->last = previous;
	}

	next = NULL;
	previous = NULL;
	parent = NULL;
	child = NULL;
	Setpopped(1, true);
	Resetreference(reference);
	return kifTRUE;
}

void KifTree::TreeFromContainer(KifElement* k) {
	KifTree* newtree;
	KifIteration* itr = (KifIteration*)k->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		newtree = new KifTree(kifcode, NULL, NULL);
		newtree->value = itr->IteratorKey()->Atom();
		newtree->SetHight(hight + 1);
		newtree->Setreference();
		if (child != NULL) {
			last->next = newtree;
			newtree->previous = last;
			last = newtree;
		}
		else {
			child = newtree;
			last = newtree;
		}

		if (itr->IteratorValue()->aContainer())
			newtree->TreeFromContainer(itr->IteratorValue());
	}
	itr->Release();
}

Exported bool KifTree::Setvalue(KifElement* k, KifElement* index, KifElement* dom) {
	if (value != kifNULL)
		value->Resetreference(reference);

	if (k->aContainer()) {
		if (child != NULL) {
			child->TreeRemove();
			child = NULL;
			last = NULL;
		}
		if (k->Size() != 1) {
			kifcode->Returnerror(KIFCONTAINER[1]);
			return true;
		}
		KifIteration* itr = (KifIteration*)k->Newiterator(false);
		itr->Begin();
		value = itr->IteratorKey()->Atom();
		value->Setreference(reference);
		TreeFromContainer(itr->IteratorValue());
		itr->Release();
		return true;
	}
	ThreadLock _lock(kifTree);
	if (k->type == kifTree)
		value = ((KifTree*)k)->value->Atom();
	else
		value = k->Atom();

	value->Setreference(reference);
	return true;
}

KifElement* KifTree::Declaration(short n) {
	ThreadLock _lock(kifTree);
	if (TestkifFunctions(n, kifGlobals->kifTreeFunctions) == true) {
		if (kifGlobals->kifBaseTreeFunctions.find(n) != kifGlobals->kifBaseTreeFunctions.end())
			return kifGlobals->kifBaseTreeFunctions[n];
		return kifGlobals->kifBaseFunctions[n];
	}
	return NULL;
}

Exported KifElement* KifTree::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationTree.find(s) == kifGlobals->kifInformationTree.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationTree[s]);
}

void KifTree::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifTreeFunctions.begin(); it != kifGlobals->kifTreeFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

Exported KifElement* KifTree::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifElement* res;
	if (b->type != kifTree)
		return b->in(a, b, kfunc, contextualpattern, true);

	KifIteratorTree itree(NULL, NULL, (KifTree*)b);


	KifElement* ktree = itree.Begin();
	KifElement* kvect = NULL;
	KifMap* kmap = NULL;
	while (itree.End() == kifFALSE) {
		KifElement* kval = ktree->TreeValue();
		res = kval->in(a, kval, kfunc, this, idx);
		if (res->Succeed()) {
			res->Release();
			if (idx == false)
				return kifTRUE;

			if (contextualpattern->aVector()) {
				if (kvect == NULL)
					kvect = Selectvector(this, contextualpattern);
				kvect->Push(ktree);
			}
			else {
				if (contextualpattern->type == kifMap) {
					if (kmap == NULL)
						kmap = Selectmap(this, contextualpattern);
					char ch[20];
					sprintf_s(ch, 20, "%d", itree.idx);
					kmap->Push(ch, ktree);
				}
				else
					return kifcode->Provideinteger(itree.idx);
			}
		}
		itree.Next();
		ktree->IteratorValue();
	}
	if (kvect != NULL)
		return kvect;
	if (kmap != NULL)
		return kmap;
	return kifFALSE;
}

//-------------------------------------------------------------------------------------------------
Exported KifElement* KifTree::Newiterator(bool rev) {
	KifIteratorTree* iter = new KifIteratorTree(kifcode, NULL);
	iter->reverse = rev;
	iter->tree = this;
	return iter;
}

Exported void KifIteratorTree::Flatten(KifTree* t) {
	if (t == NULL)
		return;
	flat.push_back(t);
	Flatten(t->child);
	Flatten(t->next);
}

Exported KifElement* KifIteratorTree::IteratorKey() {
	if (idx >= flat.size() || idx < 0)
		return kifNOELEMENT;
	return kifcode->Provideinteger(idx);
}

long KifIteratorTree::IteratorKeyInteger() {
	if (idx >= flat.size() || idx < 0)
		return 0;
	return idx;
}

double KifIteratorTree::IteratorKeyFloat() {
	if (idx >= flat.size() || idx < 0)
		return 0;
	return idx;
}

Exported KifElement* KifIteratorTree::IteratorValue() {
	if (idx >= flat.size() || idx < 0)
		return kifNOELEMENT;
	return flat[idx];
}

Exported void KifIteratorTree::Next() {
	if (reverse == false)
		idx++;
	else
		idx--;
}

Exported KifElement* KifIteratorTree::End() {
	if (reverse == false) {
		if (idx >= flat.size())
			return kifTRUE;
	}
	else {
		if (idx < 0)
			return kifTRUE;
	}

	return kifFALSE;
}

Exported KifElement* KifIteratorTree::Begin() {
	if (tree == NULL)
		return kifFALSE;
	Flatten(tree);
	if (reverse == true)
		idx = flat.size() - 1;
	else
		idx = 0;
	return kifTRUE;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
Exported void KifList::PushLast(KifElement* ke) {
	if (ke == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ke = ke->Atom();
	values.push_back(ke);
	ke->Setreference(reference + 1);
}

Exported void KifList::Push(KifElement* ke) {
	if (ke == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ke = ke->Atom();
	values.push_back(ke);
	ke->Setreference(reference + 1);
}

Exported void KifList::PushFirst(KifElement* ke) {
	if (ke == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	ke = ke->Atom();
	values.push_front(ke);
	ke->Setreference(reference + 1);
}


void KifList::PopLast() {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;
	KifElement* kres = values.back();
	kres->Resetreference(reference + 1);
	values.pop_back();
}

void KifList::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return;
	}


	if (ke->isString()) {
		//In that case, we need to get rid of string that matches it...
		string s = ke->String();
		list<KifElement*>::iterator it;
		vector<KifElement*> removed;
		for (it = values.begin(); it != values.end(); it++) {
			if ((*it)->String() == s)
				removed.push_back(*it);
		}

		for (int i = removed.size() - 1; i >= 0; i--) {
			values.remove(removed[i]);
			removed[i]->Resetreference(reference + 1);
		}
		return;
	}

	int nb = ke->Integer();

	if (nb == -1) {
		values.back()->Resetreference(reference + 1);
		values.pop_back();
		return;
	}

	if (nb >= values.size() || nb<0) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return;
	}

	list<KifElement*>::iterator it = values.begin();
	while (nb>0) {
		it++;
		nb--;
	}

	(*it)->Resetreference(reference + 1);
	values.erase(it);
}

void KifList::PopFirst() {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;
	KifElement* kres = values.front();
	kres->Resetreference(reference + 1);
	values.pop_front();
}

Exported void KifList::Insert(int idx, KifElement* ke) {
	ke = ke->Atom();
	KifContainerLock _lock(this);
	list<KifElement*>::iterator itl = values.begin();
	for (int nb = 0; nb < idx; nb++)
		itl++;
	values.insert(itl, ke);
	ke->Setreference(reference + 1);
}


Exported void KifVector::Push(KifElement* ke) {
	if (ke == kifRAISEERROR)
		return;
	ke = ke->Atom();
	KifContainerLock _lock(this);
	values.push_back(ke);
	ke->Setreference(reference + 1);
}

Exported void KifVector::Insert(int idx, KifElement* ke) {
	ke = ke->Atom();
	KifContainerLock _lock(this);
	values.insert(values.begin() + idx, ke);
	ke->Setreference(reference + 1);
}

void KifVector::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return;
	}

	int nb;
	if (ke->isString()) {
		//In that case, we need to get rid of string that matches it...
		string s = ke->String();
		for (nb = values.size() - 1; nb >= 0; nb--) {
			if (values[nb]->String() == s) {
				values[nb]->Resetreference(reference + 1);
				values.erase(values.begin() + nb);
			}
		}
		return;
	}

	nb = ke->Integer();
	if (nb == -1) {
		values.back()->Resetreference(reference + 1);
		values.pop_back();
	}
	else {
		if (nb >= values.size() || nb < 0) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return;
		}
		values[nb]->Resetreference(reference + 1);
		values.erase(values.begin() + nb);
	}
}

bool KifList::Permute() {
	return next_permutation(values.begin(), values.end());
}

void KifList::Shuffle() {
	list<KifElement*>::iterator it;
	vector<KifElement*> vb;
	for (it = values.begin(); it != values.end(); it++)
		vb.push_back(*it);

	size_t sz = vb.size();
	size_t i, f;
	int mx = sz;
	KifElement* v;
	for (i = 0; i < sz; i++) {
		f = localrandom(mx--);
		if (mx != f) {
			v = vb[mx];
			vb[mx] = vb[f];
			vb[f] = v;
		}
	}

	values.clear();
	for (i = 0; i < sz; i++)
		values.push_back(vb[i]);
}

KifElement* KifList::Unique() {
	KifContainerLock _lock(this);
	KifList* klist = new KifList(kifcode, NULL);
	list<KifElement*>::iterator it;
	map<string, KifElement*> inter;
	string k;
	for (it = values.begin(); it != values.end(); it++) {
		k = (*it)->String();
		if (inter.find(k) != inter.end()) {
			if (inter[k]->same(*it)->Boolean() == false)
				klist->Push(*it);
		}
		else {
			inter[k] = *it;
			klist->Push(*it);
		}
	}
	return klist;
}

bool KifVector::Permute() {
	return next_permutation(values.begin(), values.end());
}

void KifVector::Shuffle() {
	size_t sz = values.size();
	size_t i, f;
	int mx = sz;
	KifElement* v;
	for (i = 0; i < sz; i++) {
		f = localrandom(mx--);
		if (mx != f) {
			v = values[mx];
			values[mx] = values[f];
			values[f] = v;
		}
	}
}

KifElement* KifVector::Unique() {
	KifContainerLock _lock(this);
	KifVector* kvect = new KifVector(kifcode, NULL);
	map<string, KifElement*> inter;
	string k;
	for (int i = 0; i < values.size(); i++) {
		k = values[i]->String();
		if (inter.find(k) != inter.end()) {
			if (inter[k]->same(values[i])->Boolean() == false)
				kvect->Push(values[i]);
		}
		else {
			inter[k] = values[i];
			kvect->Push(values[i]);
		}
	}
	return kvect;
}

bool KifVectorString::Permute() {
	return next_permutation(values.begin(), values.end());
}

void KifVectorString::Shuffle() {
	size_t sz = values.size();
	size_t i, f;
	int mx = sz;
	string v;
	for (i = 0; i < sz; i++) {
		f = localrandom(mx--);
		if (mx != f) {
			v = values[mx];
			values[mx] = values[f];
			values[f] = v;
		}
	}
}

KifElement* KifVectorString::Unique() {
	KifContainerLock _lock(this);
	KifVectorString* kvect = new KifVectorString(kifcode, NULL);
	map<string, bool> inter;
	for (int i = 0; i < values.size(); i++) {
		if (inter.find(values[i]) == inter.end()) {
			inter[values[i]] = true;
			kvect->values.push_back(values[i]);
		}
	}
	return kvect;
}

bool KifVectorInteger::Permute() {
	return next_permutation(values.begin(), values.end());
}

void KifVectorInteger::Shuffle() {
	size_t sz = values.size();
	size_t i, f;
	int mx = sz;
	long v;
	for (i = 0; i < sz; i++) {
		f = localrandom(mx--);
		if (mx != f) {
			v = values[mx];
			values[mx] = values[f];
			values[f] = v;
		}
	}
}

KifElement* KifVectorInteger::Unique() {
	KifContainerLock _lock(this);
	KifVectorInteger* kvect = new KifVectorInteger(kifcode, NULL);
	map<long, bool> inter;
	for (int i = 0; i < values.size(); i++) {
		if (inter.find(values[i]) == inter.end()) {
			inter[values[i]] = true;
			kvect->values.push_back(values[i]);
		}
	}
	return kvect;
}

bool KifVectorFloat::Permute() {
	return next_permutation(values.begin(), values.end());
}

void KifVectorFloat::Shuffle() {
	size_t sz = values.size();
	size_t i, f;
	int mx = sz;
	double v;
	for (i = 0; i < sz; i++) {
		f = localrandom(mx--);
		if (mx != f) {
			v = values[mx];
			values[mx] = values[f];
			values[f] = v;
		}
	}
}

KifElement* KifVectorFloat::Unique() {
	KifContainerLock _lock(this);
	KifVectorFloat* kvect = new KifVectorFloat(kifcode, NULL);
	map<double, bool> inter;
	for (int i = 0; i < values.size(); i++) {
		if (inter.find(values[i]) == inter.end()) {
			inter[values[i]] = true;
			kvect->values.push_back(values[i]);
		}
	}
	return kvect;
}

Exported KifElement* KifTable::constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
	if (params->InstructionSize() == 0)
		return this;

	if (params->InstructionSize() != 1)
		return kifcode->Returnerror(KIFLIBSTRINGS[266]);


	params = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	long sz = params->Integer();
	params->Release();

	build(sz);
	return this;
}

void KifTable::build(int sz) {
	if (sz <= 0)
		return;


	if (values != NULL)
		delete[] values;

	values = new KifElement*[sz];
	size = sz;
	for (int i = 0; i < sz; i++)
		values[i] = kifNULL;
}

void KifTable::Insert(int idx, KifElement* ke) {
	if (idx >= size - 1) {
		kifcode->Returnerror(KIFLIBSTRINGS[243]);
		return;
	}

	//first we push all elements one step to the right from idx
	for (int i = size - 1; i >= idx; i--)
		values[i] = values[i - 1];
	ke = ke->Atom();
	values[idx] = ke;
	ke->Setreference(reference + 1);
}

void KifTable::Merge(KifElement* ke) {
	int i;

	for (i = size - 1; i >= 0; i--)
	if (values[i] == kifNULL)
		break;

	if (i == -1) {
		kifcode->Returnerror(KIFLIBSTRINGS[267]);
		return;
	}

	if (!ke->aContainer()) {
		values[i] = ke->Atom();
		values[i]->Setreference(reference + 1);
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		ke = itr->IteratorValue();
		values[i] = ke->Atom();
		values[i++]->Setreference(reference + 1);
		ke->Release();
		if (i >= size) {
			kifcode->Returnerror(KIFLIBSTRINGS[267]);
			break;
		}
	}

	itr->Release();
}

KifElement* KifTable::Unique() {
	KifTable* kvect = new KifTable(kifcode, NULL);
	kvect->build(size);
	map<string, KifElement*> inter;
	string k;
	KifElement* ke;
	int j = 0;

	for (int i = 0; i < size; i++) {
		if (values[i] == kifNULL)
			continue;
		k = values[i]->String();
		if (inter.find(k) != inter.end()) {
			if (inter[k]->same(values[i])->Boolean() == false) {
				ke = values[i]->Atom();
				ke->Setreference(1);
				kvect->values[j++] = ke;
			}
		}
		else {
			inter[k] = values[i];
			ke = values[i]->Atom();
			ke->Setreference(1);
			kvect->values[j++] = ke;
		}
	}
	return kvect;
}


KifElement* KifTable::Copy(KifDomain* kp, KifElement* dom) {
	KifTable* ke = new KifTable(kifcode, NULL);
	ke->build(size);
	KifElement* kx;
	int j = 0;

	for (int itx = 0; itx < size; itx++) {
		kx = values[itx]->Copy(kp, dom);
		kx->Setreference(1);
		ke->values[j++] = kx;
	}
	return ke;
}

string KifTable::String() {

	string res;
	int it;
	res = "[";
	bool beg = true;
	string sx;
	KifElement* element;
	for (it = 0; it < size; it++) {
		element = values[it];
		if (beg == false)
			res += ",";
		beg = false;
		sx = element->StringForDisplay();
		if (!element->aString())
			res += sx;
		else
			stringing(res, sx);
	}
	res += "]";
	return res;
}

string KifTable::JSonString() {

	string res;
	int it;
	res = "[";
	bool beg = true;
	string sx;
	KifElement* element;
	for (it = 0; it < size; it++) {
		element = values[it];
		if (beg == false)
			res += ",";
		beg = false;
		sx = element->JSonString();
		if (!element->aString())
			res += sx;
		else
			jstringing(res, sx);
	}
	res += "]";
	return res;
}

void KifTable::Push(KifElement* ke) {
	if (ke == kifRAISEERROR)
		return;

	for (int i = 0; i < size; i++) {
		if (values[i] == kifNULL) {
			ke = ke->Atom();
			values[i] = ke;
			ke->Setreference(reference + 1);
			return;
		}
	}
	kifcode->Returnerror(KIFLIBSTRINGS[267]);
}

void KifTable::Pop(KifElement* ke) {

	if (size == 0) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return;
	}

	int nb;

	if (ke->isString()) {
		//In that case, we need to get rid of string that matches it...
		string s = ke->String();
		for (nb = 0; nb < size; nb++) {
			if (values[nb]->String() == s) {
				values[nb]->Resetreference(reference + 1);
				values[nb] = kifNULL;
			}
		}
		return;
	}

	nb = ke->Integer();
	if (nb == -1) {
		for (int i = size - 1; i >= 0; i--) {
			if (values[i] != kifNULL) {
				values[i]->Resetreference(reference + 1);
				values[i] = kifNULL;
				return;
			}
		}
		return;
	}

	if (nb >= size || nb < 0) {
		kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return;
	}

	if (values[nb] != kifNULL) {
		values[nb]->Resetreference(reference + 1);
		values[nb] = kifNULL;
	}
}


KifElement* KifTable::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {


	if (idx == NULL || idx->isConst()) {

		KifElement* ke;
		//In this case, we copy the elements from the vector to the map, using the position as index
		if (contextualpattern->aMapContainer()) {
			KifMap* map = Selectmap(this, contextualpattern);
			size_t i = 0;
			char ch[20];

			for (int it = 0; it < size; it++) {
				sprintf_s(ch, 20, "%d", i);
				ke = values[it];
				map->Push(ch, ke);
				i++;
			}
			return map;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat) {

			return kifcode->Provideinteger(size);
		}

		//Then some values must be replaced with their evaluation
		KifTable* kvect;
		if (evaluate == true) {
			kvect = new KifTable(kifcode, NULL);
			kvect->build(size);

			for (int it = 0; it < size; it++) {
				ke = values[it]->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
				if (ke == kifRAISEERROR) {
					kvect->Release();
					return kifRAISEERROR;
				}
				ke->Setreference();
				kvect->values[it] = ke;
			}
			return kvect;
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

	register int ikey;
	ikey = key->Integer();

	key->Release();
	KifTable* kvect;
	int iright;
	if (keyright != NULL)
		iright = keyright->Integer();

	{

		if (ikey < 0)
			ikey = size + ikey;

		if (ikey < 0 || ikey >= size)
			return kifcode->Returnerror(KIFLIBSTRINGS[223]);

		if (keyright == NULL)
			return values[ikey];

		keyright->Release();
		if (iright < 0 || keyright == kifNULL) {
			iright = size + iright;
			if (iright<ikey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
		}
		else {
			if (iright>size)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
		}

		//In this case, we must create a new table
		kvect = new KifTable(kifcode, NULL);
		kvect->build(iright - ikey);
		for (int i = ikey; i < iright; i++) {
			key = values[i]->Atom();
			key->Setreference(1);
			kvect->values[i - ikey] = key;
		}
	}
	return kvect;
}

bool KifTuple::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	if (idx != NULL && !idx->isConst())
		return kifcode->Returnerror(KIFCONTAINER[2]);

	if (size)
		Clear();

	size = ke->Size();
	if (!size)
		return kifcode->Returnerror(KIFCONTAINER[3]);

	int j = 0;
	build(size);
	KifElement* k;

	if (!ke->aContainer())
		return kifcode->Returnerror(KIFCONTAINER[4]);

	if (ke->aVectorContainer()) {
		threadblocinit;
		for (j = 0; j < size; j++) {
			idx = ke->ValueOnIndex(j);
			if (idx->isInstruction()) {
				k = idx->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
				values[j] = k;
				k->Setreference(reference + 1);
			}
			else {
				values[j] = idx;
				idx->Setreference(reference + 1);
			}
		}
		return true;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		idx = itr->IteratorKey();
		values[j++] = idx;
		idx->Setreference(reference + 1);
	}
	itr->Release();

	return true;
}


bool KifTable::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		int j = 0;
		if (ke->type == kifList) {
			ke->Setpopped(1, true);
			KifList* kvect = (KifList*)ke;
			//We copy all values from ke to this
			Clear();
			if (ke->Size() > size)
				build(ke->Size());
			list<KifElement*>::iterator it;
			for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
				ke = (*it)->Atom();
				values[j++] = ke;
				ke->Setreference(reference + 1);
			}
			if (ke->popped)
				ke->Setpopped(0, true);
		}
		else {
			if (ke->aVectorContainer()) {
				ke->Setpopped(1, true);
				//We copy all values from ke to this
				Clear();
				if (ke->Size() > size)
					build(ke->Size());
				for (int it = 0; it < ke->Size(); it++) {
					idx = ke->ValueOnIndex(it);
					idx = idx->Atom();
					values[j++] = idx;
					idx->Setreference(reference + 1);
				}
				if (ke->popped)
					ke->Setpopped(0, true);
			}
			else {
				//We gather all the keys from the MAP
				if (ke->aMapContainer()) {
					ke->Setpopped(1, true);
					Clear();
					if (ke->Size() > size)
						build(ke->Size());

					KifIteration* itr = (KifIteration*)ke->Newiterator(false);
					for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
						idx = itr->IteratorKey();
						idx = idx->Atom();
						values[j++] = idx;
						idx->Setreference(reference + 1);
					}
					itr->Release();
					if (ke->popped)
						ke->Setpopped(0, true);
				}
			}
		}
		return true;
	}

	if (idx->type == kifIndex && ((KifIndex*)idx)->interval) {
		threadblocinit;

		//First we remove the values...
		KifElement* krkey = ((KifIndex*)idx)->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
		int lkey = krkey->Integer();
		krkey->Release();
		if (lkey < 0)
			lkey = size + lkey;

		krkey = ((KifIndex*)idx)->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
		int rkey = krkey->Integer();
		krkey->Release();
		if (rkey < 0)
			rkey = size + rkey;

		if (rkey < lkey || rkey >= size || lkey >= size) {
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return true;
		}

		if (rkey != lkey)
			rkey--;

		while (rkey >= lkey) {
			krkey = values[rkey];
			krkey->Resetreference(reference + 1);
			values[rkey] = kifNULL;
			rkey--;
		}

		if (ke->aVectorContainer()) {
			if (ke->Size() >= size - lkey) {
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
				return true;
			}

			for (int i = 0; i < ke->Size(); i++) {
				krkey = ke->ValueOnIndex(i);
				krkey = krkey->Atom();
				values[lkey++] = krkey;
				krkey->Setreference(reference + 1);
			}
			return true;
		}

		if (ke->aContainer()) {
			if (ke->Size() >= size - lkey) {
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
				return true;
			}

			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
				krkey = itr->IteratorValue();
				if (krkey->reference)
					krkey = krkey->Atom();
				values[lkey++] = krkey;
				krkey->Setreference(reference + 1);
			}

			itr->Release();
			return true;
		}

		if (lkey >= size) {
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return true;
		}

		krkey = ke->Atom();
		values[lkey] = krkey;
		krkey->Setreference(reference + 1);
		return true;
	}

	register int ikey = idx->Integer();

	if (ikey >= size)
		kifcode->Returnerror(KIFLIBSTRINGS[223]);
	else {
		if (ikey < 0) {
			ikey = size + ikey;
			if (ikey < 0)
				kifcode->Returnerror(KIFLIBSTRINGS[206]);
		}

		ke = ke->Atom();
		if (values[ikey] != kifNULL)
			values[ikey]->Resetreference(reference + 1);
		values[ikey] = ke;
		ke->Setreference(reference + 1);
	}

	return true;
}

KifElement* KifTuple::Copy(KifDomain* kp, KifElement* dom) {
	KifTuple* ke = new KifTuple(kifcode, NULL);
	ke->build(size);
	KifElement* kx;
	int j = 0;

	for (int itx = 0; itx < size; itx++) {
		kx = values[itx]->Copy(kp, dom);
		kx->Setreference(1);
		ke->values[j++] = kx;
	}
	return ke;
}

string KifTuple::String() {

	string res;
	int it;
	res = "(";
	bool beg = true;
	string sx;
	KifElement* element;
	for (it = 0; it < size; it++) {
		element = values[it];
		if (beg == false)
			res += ",";
		sx = element->StringForDisplay();
		if (!element->aString())
			res += sx;
		else
			stringing(res, sx);
		beg = false;
	}
	if (size == 1)
		res += ",";
	res += ")";
	return res;
}

string KifTuple::JSonString() {

	string res;
	int it;
	res = "(";
	bool beg = true;
	string sx;
	KifElement* element;
	for (it = 0; it < size; it++) {
		element = values[it];
		if (beg == false)
			res += ",";
		beg = false;
		sx = element->JSonString();
		if (!element->aString())
			res += sx;
		else
			jstringing(res, sx);
	}
	res += ")";
	return res;
}

Exported KifElement* KifTable::same(KifElement* a) {
	if (a->type != type)
		return kifFALSE;

	KifTable* v = (KifTable*)a;

	if (size != v->size)
		return kifFALSE;

	for (int i = 0; i < size; i++) {
		if (values[i]->same(v->values[i]) == kifFALSE)
			return kifFALSE;
	}
	return kifTRUE;
}

KifElement* KifTable::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself && type == kifTuple)
		return kifcode->Returnerror(KIFCONTAINER[2]);

	if (!a->aContainer())
		return a->plus(a, b, context, autoself);

	KifTable* ref = this;
	if (autoself == false)
		ref = (KifTable*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTable*)a;

	int it;
	KifElement* ke;


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->plus(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}


	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->plus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifTable::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself && type == kifTuple)
		return kifcode->Returnerror(KIFCONTAINER[2]);

	if (!a->aContainer())
		return a->minus(a, b, context, autoself);

	KifTable* ref = this;
	if (autoself == false)
		ref = (KifTable*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTable*)a;

	int it;
	KifElement* ke;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->minus(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->minus(ke, b, context, true);
	}
	return ref;
}

KifElement* KifTable::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself && type == kifTuple)
		return kifcode->Returnerror(KIFCONTAINER[2]);

	if (!a->aContainer())
		return a->multiply(a, b, context, autoself);

	KifTable* ref = this;
	if (autoself == false)
		ref = (KifTable*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTable*)a;

	int it;
	KifElement* ke;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->multiply(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->multiply(ke, b, context, true);
	}
	return ref;
}

KifElement* KifTable::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself && type == kifTuple)
		return kifcode->Returnerror(KIFCONTAINER[2]);

	if (!a->aContainer())
		return a->divide(a, b, context, autoself);

	KifTable* ref = this;
	if (autoself == false)
		ref = (KifTable*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTable*)a;

	int it;
	KifElement* ke;
	int idthread = kifGlobals->GetThreadid();


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->divide(ke, ki, context, true);
			ki->Release();
			if (Errorid(idthread)) {
				itr->Release();
				ref->Release();
				return kifRAISEERROR;
			}
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->divide(ke, b, context, true);
		if (Errorid(idthread)) {
			ref->Release();
			return kifRAISEERROR;
		}
	}
	return ref;
}

KifElement* KifTable::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself && type == kifTuple)
		return kifcode->Returnerror(KIFCONTAINER[2]);

	if (!a->aContainer())
		return a->mod(a, b, context, autoself);

	KifTable* ref = this;
	if (autoself == false)
		ref = (KifTable*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTable*)a;

	int it;
	KifElement* ke;
	int idthread = kifGlobals->GetThreadid();


	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->mod(ke, ki, context, true);
			ki->Release();
			if (Errorid(idthread)) {
				itr->Release();
				ref->Release();
				return kifRAISEERROR;
			}
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->mod(ke, b, context, true);
		if (Errorid(idthread)) {
			ref->Release();
			return kifRAISEERROR;
		}
	}
	return ref;
}

KifElement* KifTable::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself && type == kifTuple)
		return kifcode->Returnerror(KIFCONTAINER[2]);

	if (!a->aContainer())
		return a->shiftright(a, b, context, autoself);

	KifTable* ref = this;
	if (autoself == false)
		ref = (KifTable*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTable*)a;

	int it;
	KifElement* ke;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->shiftright(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->shiftright(ke, b, context, true);
	}
	return ref;
}

KifElement* KifTable::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself && type == kifTuple)
		return kifcode->Returnerror(KIFCONTAINER[2]);

	if (!a->aContainer())
		return a->shiftleft(a, b, context, autoself);

	KifTable* ref = this;
	if (autoself == false)
		ref = (KifTable*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTable*)a;

	int it;
	KifElement* ke;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->shiftleft(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->shiftleft(ke, b, context, true);
	}
	return ref;
}

KifElement* KifTable::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (autoself && type == kifTuple)
		return kifcode->Returnerror(KIFCONTAINER[2]);

	if (!a->aContainer())
		return a->power(a, b, context, autoself);

	KifTable* ref = this;
	if (autoself == false)
		ref = (KifTable*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifTable*)a;

	int it;
	KifElement* ke;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifElement* ki;
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->Size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ki = itr->IteratorValue();
			ke = ref->ValueOnIndex(it);
			ke->power(ke, ki, context, true);
			ki->Release();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	for (it = 0; it < ref->Size(); it++) {
		ke = ref->ValueOnIndex(it);
		ke->power(ke, b, context, true);
	}
	return ref;
}

KifElement* KifTable::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Vector::Newiterator");
#endif
	KifIteratorTable* iter = new KifIteratorTable(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = values;
	iter->size = size;
	return iter;
}

KifElement* KifTable::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifVectorFunctions) == true || n == idwaiton || n == idtrigger || n == KifStringId(KIFCONTAINER[5]) || n == KifStringId(KIFCONTAINER[6]) || n == KifStringId(KIFCONTAINER[7]))
		return kifGlobals->kifBaseFunctions[n];

	return NULL;
}

void KifTable::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifVectorFunctions.begin(); it != kifGlobals->kifVectorFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
	fcts->values.push_back(kifGlobals->kifIdString[KifStringId(KIFCONTAINER[6])]);
	fcts->values.push_back(kifGlobals->kifIdString[KifStringId(KIFCONTAINER[7])]);
}

Exported KifElement* KifTable::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}


KifElement* KifTable::Map(KifElement* contextualpattern, KifMap* m) {

	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();

	char ch[20];
	for (int nb = 0; nb < size; nb++) {
		sprintf_s(ch, 20, "%d", nb);
		kmap->Push(ch, values[nb]);
	}
	return kmap;
}

KifElement* KifTable::Vector(KifElement* contextualpattern, KifVector* v) {

	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();

	//We copy all values from ke to this
	kvect->values.reserve(size);
	for (int nb = 0; nb < size; nb++)
		kvect->Push(values[nb]);
	return kvect;
}

static void resetTable(KifTable* ktable, int inc, char popped, bool clr, int sz) {
	if (sz == 0)
		return;

	KifContainerLock _lock(ktable);
	for (int itx = 0; itx<sz; itx++) {
		ktable->values[itx]->Resetreference(inc);
		if (clr && !popped)
			ktable->values[itx] = kifNULL;
	}
}

void KifTable::Resetreference(int inc) {
	reference -= inc;
	if (reference>0)
		resetTable(this, inc, popped, false, size);
	else {
		resetTable(this, inc + 1 + reference - popped, popped, true, size);
		if (popped == 1)
			popped = 0;
		else
			KifElement::Resetreference(0);
	}
}

Exported void KifTable::Setreference(int inc) {
	if (idgarbage == KIFCIRCULARDETECTION) {
		kifcode->Returnerror(KIFCONTAINER[0]);
		kifGlobals->executionbreak = 1;
		return;
	}
	int id = idgarbage;
	idgarbage = KIFCIRCULARDETECTION;
	reference += inc;
	Putpopped(0);
	for (int itx = 0; itx < size; itx++)
		values[itx]->Setreference(inc);
	idgarbage = id;
}


KifElement* KifTable::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifElement* res;
	KifElement* ke;



	if (b->aVectorContainer()) {

		KifElement* found = kifFALSE;
		int itb;

		if (b != this)
			Clear();

		//A sub vector in a vector... Each element of a should be in b, in that order
		if (a->aVectorContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
		}

		for (itb = 0; itb != b->Size(); itb++) {
			ke = b->ValueOnIndex(itb);
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				res->Release();
				if (b != this) {
					KifBaseInteger kint(NULL, NULL, itb);
					kint.kifcode = kifcode;
					Push(&kint);
					found = this;
				}
				else {
					if (idx == true)
						return kifcode->Provideinteger(itb);
					return kifTRUE;
				}
			}
		}

		if (b == this && idx == true)
			return kifMINUSONE;

		return found;
	}

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		Clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			if (res->Succeed()) {
				res->Release();
				Push(itr->IteratorKey());
			}
			ke->Release();
		}
		itr->Release();
		return this;
	}

	if (b != this)
		return b->in(a, b, kfunc, contextualpattern, true);
	if (idx == true)
		return kifMINUSONE;
	return kifFALSE;
}


KifElement* KifIteratorList::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (reverse == false) {
		if (itx == values->size())
			return kifNULL;
	}
	else {
		if (itx < 0)
			return kifNULL;
	}

	KifElement* ke = listValue();
	return ke->Exec(contextualpattern, dom, val, idthread, callfunc);
}

KifElement* KifIteratorVector::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (reverse == false) {
		if (itx == values->size())
			return kifNULL;
	}
	else {
		if (itx < 0)
			return kifNULL;
	}
	return (*values)[itx]->Exec(contextualpattern, dom, val, idthread, callfunc);
}

KifElement* KifIteratorTable::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc) {
	if (reverse == false) {
		if (itx == size)
			return kifNULL;
	}
	else {
		if (itx < 0)
			return kifNULL;
	}

	return values[itx]->Exec(contextualpattern, dom, val, idthread, callfunc);
}

string KifElement::ValueOnKeyString(string k, bool& a) {
	KifElement* ke = ValueOnKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return "";
	}
	a = true;
	string n = ke->String();
	ke->Release();
	return n;
}

long KifElement::ValueOnKeyInteger(string k, bool& a) {
	KifElement* ke = ValueOnKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return 0;
	}
	a = true;
	long n = ke->Integer();
	ke->Release();
	return n;
}

double KifElement::ValueOnKeyFloat(string k, bool& a) {
	KifElement* ke = ValueOnKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return 0;
	}
	a = true;
	double n = ke->Float();
	ke->Release();
	return n;
}

wstring KifElement::ValueOnKeyUString(wstring k, bool& a) {
	KifElement* ke = ValueOnKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return L"";
	}
	a = true;
	wstring n = ke->UString();
	ke->Release();
	return n;
}

long KifElement::ValueOnKeyInteger(wstring k, bool& a) {
	KifElement* ke = ValueOnKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return 0;
	}
	a = true;
	long n = ke->Integer();
	ke->Release();
	return n;
}

double KifElement::ValueOnKeyFloat(wstring k, bool& a) {
	KifElement* ke = ValueOnKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return 0;
	}
	a = true;
	double n = ke->Float();
	ke->Release();
	return n;
}



string KifElement::ValueOnKeyIString(long k, bool& a) {
	KifElement* ke = ValueOnIntegerKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return "";
	}
	a = true;
	string n = ke->String();
	ke->Release();
	return n;
}

long KifElement::ValueOnKeyIInteger(long k, bool& a) {
	KifElement* ke = ValueOnIntegerKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return 0;
	}
	a = true;
	long n = ke->Integer();
	ke->Release();
	return n;
}

double KifElement::ValueOnKeyIFloat(long k, bool& a) {
	KifElement* ke = ValueOnIntegerKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return 0;
	}
	a = true;
	double n = ke->Float();
	ke->Release();
	return n;
}

string KifElement::ValueOnKeyFString(double k, bool& a) {
	KifElement* ke = ValueOnFloatKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return "";
	}
	a = true;
	string n = ke->String();
	ke->Release();
	return n;
}

long KifElement::ValueOnKeyFInteger(double k, bool& a) {
	KifElement* ke = ValueOnFloatKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return 0;
	}
	a = true;
	long n = ke->Integer();
	ke->Release();
	return n;
}

double KifElement::ValueOnKeyFFloat(double k, bool& a) {
	KifElement* ke = ValueOnFloatKey(k);
	if (ke == kifNOELEMENT) {
		a = false;
		return 0;
	}
	a = true;
	double n = ke->Float();
	ke->Release();
	return n;
}

void KifElement::storekey(long k, long v) {
	char buff[20]; sprintf_s(buff, 20, "%ld", k);
	KifBaseInteger ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(buff, &ke);
}

void KifElement::storekey(long k, double v) {
	char buff[20]; sprintf_s(buff, 20, "%ld", k);
	KifBaseFloat ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(buff, &ke);
}

void KifElement::storekey(long k, string v) {
	char buff[20]; sprintf_s(buff, 20, "%ld", k);
	KifBaseString ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(buff, &ke);
}

void KifElement::storekey(long k, wstring v) {
	wchar_t buff[20];
	swprintf_s(buff, 20, L"%ld", k);
	wstring ws(buff);
	Push(ws, kifcode->Provideustring(buff));
}

void KifElement::storekey(double k, wstring v) {
	wchar_t buff[20];
	swprintf_s(buff, 20, L"%g", k);
	wstring ws(buff);
	Push(ws, kifcode->Provideustring(buff));
}

void KifElement::storekey(double k, long v) {
	char buff[20]; sprintf_s(buff, 20, "%g", k);
	KifBaseInteger ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(buff, &ke);
}

void KifElement::storekey(double k, double v) {
	char buff[20]; sprintf_s(buff, 20, "%g", k);
	KifBaseFloat ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(buff, &ke);
}

void KifElement::storekey(double k, string v) {
	char buff[20]; sprintf_s(buff, 20, "%g", k);
	KifBaseString ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(buff, &ke);
}

void KifElement::storekey(string k, long v) {
	KifBaseInteger ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(k, &ke);
}

void KifElement::storekey(string k, double v) {
	KifBaseFloat ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(k, &ke);
}

void KifElement::storekey(string k, string v) {
	KifBaseString ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(k, &ke);
}

void KifElement::storekey(wstring k, long v) {
	KifBaseInteger ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(k, &ke);
}

void KifElement::storekey(wstring k, double v) {
	KifBaseFloat ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(k, &ke);
}

void KifElement::storekey(wstring k, wstring v) {
	KifUString* ke = kifcode->Provideustring(v);
	Push(k, ke);
}

void KifElement::storekey(char* k, long v) {
	KifBaseInteger ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(k, &ke);
}

void KifElement::storekey(char* k, double v) {
	KifBaseFloat ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(k, &ke);
}

void KifElement::storekey(char* k, string v) {
	KifBaseString ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(k, &ke);
}

void KifElement::storevalue(long v) {
	KifBaseInteger ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(&ke);
}

void KifElement::storevalue(double v) {
	KifBaseFloat ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(&ke);
}

void KifElement::storevalue(string v) {
	KifBaseString ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(&ke);
}

void KifElement::storevalue(char* v) {
	KifBaseString ke(NULL, NULL, v); ke.kifcode = kifcode;
	Push(&ke);
}

void KifElement::storevalue(wstring v) {
	KifUString* ke = kifcode->Provideustring(v);
	Push(ke);
}

void KifElement::storevalue(wchar_t v) {
	KifUString* ke = kifcode->Provideustring(v);
	Push(ke);
}


//------------------------------------------------------------------------------------------------

KifElement* KifEvaluate::apply(KifCallFunctionGeneral* kfunclocal, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc) {
	KifElement* kcont = kifNULL;

	if (contextualpattern->aContainer() && contextualpattern != this) {
		kcont = contextualpattern;
		kcont->Clear();
	}
	int idthread = kifGlobals->GetThreadid();
	KifElement* result = kifNULL;
	KifElement* kf;
	KifIteration* itr = (KifIteration*)Newiterator(false);
	for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
		kf = itr->IteratorValue();
		kfunclocal->parameters[0] = kf;
		kf->Setreference();
		result = kfunclocal->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (Errorid(idthread)) {
			kf->Resetreference();
			itr->Release();
			return Errorptr();
		}

		if (result != kifNULL) {
			if (kcont == kifNULL)
				kcont = Newinstance();
			if (kcont->aMapContainer()) {
				KifElement* key = itr->IteratorKey();
				kcont->Push(key, result);
				key->Release();
			}
			else
				kcont->Push(result);
		}
		kf->Resetreference();
	}
	itr->Release();
	return kcont;
}

KifElement* KifVectorString::apply(KifCallFunctionGeneral* kfunclocal, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc) {

	KifElement* kcont = kifNULL;

	if (contextualpattern->aContainer() && contextualpattern != this) {
		kcont = contextualpattern;
		kcont->Clear();
	}
	KifElement* result = kifNULL;
	KifBaseString kf(NULL, NULL);
	kf.kifcode = dom->kifcode;
	KifBaseInteger key(NULL, NULL);
	key.kifcode = dom->kifcode;
	kf.reference = 1;
	key.reference = 1;
	int idthread = kifGlobals->GetThreadid();

	for (int itx = 0; itx < values.size(); itx++) {
		kf.value = values[itx];
		//values in this case are atomic... They cannot be a function as above, hence this local case
		//We expect a function with one single argument
		kfunclocal->parameters[0] = &kf;
		result = kfunclocal->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (Errorid(idthread))
			return Errorptr();
		if (result != kifNULL) {
			if (kcont == kifNULL)
				kcont = Newinstance();
			if (kcont->aMapContainer()) {
				key.value = itx;
				kcont->Push(&key, result);
			}
			else
				kcont->Push(result);
		}
		values[itx] = kf.value;
	}
	return kcont;
}

KifElement* KifVectorInteger::apply(KifCallFunctionGeneral* kfunclocal, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc) {

	KifElement* kcont = kifNULL;

	if (contextualpattern->aContainer() && contextualpattern != this) {
		kcont = contextualpattern;
		kcont->Clear();
	}
	KifElement* result = kifNULL;
	KifBaseInteger kf(NULL, NULL);
	kf.kifcode = dom->kifcode;
	KifBaseInteger key(NULL, NULL);
	key.kifcode = dom->kifcode;
	kf.reference = 1;
	key.reference = 1;
	int idthread = kifGlobals->GetThreadid();

	for (int itx = 0; itx < values.size(); itx++) {
		kf.value = values[itx];
		//values in this case are atomic... They cannot be a function as above, hence this local case
		//We expect a function with one single argument
		kfunclocal->parameters[0] = &kf;
		result = kfunclocal->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (Errorid(idthread))
			return Errorptr();
		if (result != kifNULL) {
			if (kcont == kifNULL)
				kcont = Newinstance();
			if (kcont->aMapContainer()) {
				key.value = itx;
				kcont->Push(&key, result);
			}
			else
				kcont->Push(result);
		}
		values[itx] = kf.value;
	}
	return kcont;
}

KifElement* KifVectorFloat::apply(KifCallFunctionGeneral* kfunclocal, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc) {

	KifElement* kcont = kifNULL;

	if (contextualpattern->aContainer() && contextualpattern != this) {
		kcont = contextualpattern;
		kcont->Clear();
	}
	KifElement* result = kifNULL;
	KifBaseFloat kf(NULL, NULL);
	kf.kifcode = dom->kifcode;
	KifBaseInteger key(NULL, NULL);
	key.kifcode = dom->kifcode;
	kf.reference = 1;
	key.reference = 1;
	int idthread = kifGlobals->GetThreadid();

	for (int itx = 0; itx < values.size(); itx++) {
		kf.value = values[itx];
		//values in this case are atomic... They cannot be a function as above, hence this local case
		//We expect a function with one single argument
		kfunclocal->parameters[0] = &kf;
		result = kfunclocal->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (Errorid(idthread))
			return Errorptr();
		if (result != kifNULL) {
			if (kcont == kifNULL)
				kcont = Newinstance();
			if (kcont->aMapContainer()) {
				key.value = itx;
				kcont->Push(&key, result);
			}
			else
				kcont->Push(result);
		}
		values[itx] = kf.value;
	}
	return kcont;
}

KifElement* KifVector::apply(KifCallFunctionGeneral* kfunclocal, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc) {
	KifElement* kcont = kifNULL;

	if (contextualpattern->aContainer() && contextualpattern != this) {
		kcont = contextualpattern;
		kcont->Clear();
	}
	KifElement* result = kifNULL;
	KifElement* kf;
	KifBaseInteger key(NULL, NULL);
	key.kifcode = dom->kifcode;
	key.reference = 1;
	int idthread = kifGlobals->GetThreadid();

	for (int itx = 0; itx < values.size(); itx++) {
		kf = values[itx];
		//We expect a function with one single argument
		kfunclocal->parameters[0] = kf;
		result = kfunclocal->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (Errorid(idthread))
			return Errorptr();
		if (result != kifNULL) {
			if (kcont == kifNULL)
				kcont = Newinstance();
			if (kcont->aMapContainer()) {
				key.value = itx;
				kcont->Push(&key, result);
			}
			else
				kcont->Push(result);
		}
	}
	return kcont;
}


KifElement* KifEvaluateMap::apply(KifCallFunctionGeneral* kfunclocal, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc) {
	KifElement* kcont = kifNULL;

	if (contextualpattern->aContainer() && contextualpattern != this) {
		kcont = contextualpattern;
		kcont->Clear();
	}
	KifElement* result = kifNULL;
	KifElement* kf;
	KifElement* kkey;
	KifIteration* itr = (KifIteration*)Newiterator(false);
	int idthread = kifGlobals->GetThreadid();

	for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
		kf = itr->IteratorValue();
		kkey = itr->IteratorKey();
		kfunclocal->parameters[0] = kkey;
		kfunclocal->parameters[1] = kf;

		kkey->Setreference();
		kf->Setreference();
		result = kfunclocal->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (Errorid(idthread)) {
			kkey->Resetreference();
			kf->Resetreference();
			itr->Release();
			return Errorptr();
		}

		if (result != kifNULL) {
			if (kcont == kifNULL)
				kcont = Newinstance();
			if (kcont->aMapContainer()) {
				KifElement* key = itr->IteratorKey();
				kcont->Push(key, result);
				key->Release();
			}
			else
				kcont->Push(result);
		}

		kkey->Resetreference();
		kf->Resetreference();
	}
	itr->Release();
	return kcont;
}

KifElement* KifEvaluateMapValue::apply(KifCallFunctionGeneral* kfunclocal, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc) {
	KifElement* kcont = kifNULL;

	if (contextualpattern->aContainer() && contextualpattern != this) {
		kcont = contextualpattern;
		kcont->Clear();
	}
	int idthread = kifGlobals->GetThreadid();
	KifElement* result = kifNULL;
	KifElement* kf;
	KifElement* kkey;
	KifIteration* itr = (KifIteration*)Newiterator(false);
	for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
		kf = itr->IteratorValue();
		kf->Setreference();
		kkey = itr->IteratorKey();
		kkey->Setreference();

		kfunclocal->parameters[0] = kkey;
		kfunclocal->parameters[1] = kf;
		result = kfunclocal->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

		if (Errorid(idthread)) {
			kkey->Resetreference();
			kf->Resetreference();
			itr->Release();
			return Errorptr();
		}

		if (result != kifNULL) {
			if (kcont == kifNULL)
				kcont = Newinstance();
			if (kcont->aMapContainer()) {
				KifElement* key = itr->IteratorKey();
				kcont->Push(key, result);
				key->Release();
			}
			else
				kcont->Push(result);
		}

		itr->IteratorSetvalue(kf);

		kkey->Resetreference();
		kf->Resetreference();
	}
	itr->Release();
	return kcont;
}
//-----------------------Edit Distance Implementation----------------------------------------
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

unsigned int KifElement::EditDistance(KifElement* e) {
	if (e->type != type || !same(e)->Boolean())
		return 1;

	return 0;
}

unsigned int StringEditDistance(wstring& value, wstring& s2) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = value.size();
	s2len = s2.size();
	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (value[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	s2len = column[s1len];
	delete[] column;
	return s2len;
}


unsigned int StringEditDistance(string& value, string& s2) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = value.size();
	s2len = s2.size();
	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (value[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	s2len = column[s1len];
	delete[] column;
	return s2len;
}

unsigned int KifRawString::EditDistance(KifElement* e) {
	string s2 = e->String();

	if (value == NULL)
		return s2.size();

	string s1((char*)value);
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = s1.size();
	s2len = s2.size();
	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (value[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	s2len = column[s1len];
	delete[] column;
	return s2len;
}

unsigned int KifString::EditDistance(KifElement* e) {
	string s2 = e->String();
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = value.size();
	s2len = s2.size();
	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (value[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	s2len = column[s1len];
	delete[] column;
	return s2len;
}

unsigned int KifUString::EditDistance(KifElement* e) {
	wstring s2 = e->UString();
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = value.size();
	s2len = s2.size();
	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (value[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	s2len = column[s1len];
	delete[] column;
	return s2len;
}

//This is for trees only
static void flattening(KifVector* v, KifTree* t) {
	if (t == NULL)
		return;
	if (t->value == NULL)
		v->Push(kifNULL);
	else
		v->Push(t->value);
	flattening(v, t->child);
	flattening(v, t->next);
}

unsigned int KifTree::EditDistance(KifElement* e) {
	if (e->type != kifTree)
		return Size();
	KifTree* t = (KifTree*)e;
	KifVector v1(NULL, NULL);
	KifVector v2(NULL, NULL);
	v1.kifcode = kifcode;
	v2.kifcode = kifcode;
	flattening(&v1, this);
	flattening(&v2, t);
	return v1.EditDistance(&v2);
}

//This is for vectors only
unsigned int KifEvaluate::EditDistance(KifElement* e) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = Size();
	s2len = e->Size();

	KifElement** v1 = new KifElement*[s1len + 1];
	KifElement** v2 = new KifElement*[s2len + 1];

	y = max(s1len, s2len);
	KifIteration* itr = (KifIteration*)Newiterator(false);
	KifIteration* ite = (KifIteration*)e->Newiterator(false);
	itr->Begin();
	ite->Begin();
	for (x = 0; x < y; x++) {
		if (x < s1len) {
			v1[x] = itr->IteratorValue();
			itr->Next();
		}

		if (x < s2len) {
			v2[x] = ite->IteratorValue();
			ite->Next();
		}
	}
	itr->Release();
	ite->Release();

	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;

	unsigned int ed;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			ed = v1[y - 1]->EditDistance(v2[x - 1]);
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
			lastdiag = olddiag;
		}
	}

	y = max(s1len, s2len);
	for (x = 0; x < y; x++) {
		if (x < s1len)
			v1[x]->Release();
		if (x < s2len)
			v2[x]->Release();
	}

	delete[] v1;
	delete[] v2;

	s2len = column[s1len];
	delete[] column;
	return s2len;

}

unsigned int KifVector::EditDistance(KifElement* e) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = Size();
	s2len = e->Size();

	KifElement** v1 = new KifElement*[s1len + 1];
	KifElement** v2 = new KifElement*[s2len + 1];

	y = max(s1len, s2len);
	for (x = 0; x < y; x++) {
		if (x < s1len)
			v1[x] = ValueOnIndex(x);
		if (x < s2len)
			v2[x] = e->ValueOnIndex(x);
	}

	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;

	unsigned int ed;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			ed = v1[y - 1]->EditDistance(v2[x - 1]);
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
			lastdiag = olddiag;
		}
	}

	y = max(s1len, s2len);
	for (x = 0; x < y; x++) {
		if (x < s1len)
			v1[x]->Release();
		if (x < s2len)
			v2[x]->Release();
	}

	delete[] v1;
	delete[] v2;

	s2len = column[s1len];
	delete[] column;
	return s2len;

}

unsigned int KifVectorString::EditDistance(KifElement* e) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = Size();
	s2len = e->Size();

	string* v1 = new string[s1len + 1];
	string* v2 = new string[s2len + 1];

	y = max(s1len, s2len);
	for (x = 0; x < y; x++) {
		if (x < s1len)
			v1[x] = values[x];
		if (x < s2len)
			v2[x] = e->getstring(x);
	}

	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;

	unsigned int ed;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			ed = StringEditDistance(v1[y - 1], v2[x - 1]);
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
			lastdiag = olddiag;
		}
	}

	delete[] v1;
	delete[] v2;

	s2len = column[s1len];
	delete[] column;
	return s2len;

}

unsigned int KifVectorInteger::EditDistance(KifElement* e) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = Size();
	s2len = e->Size();

	long* v1 = new long[s1len + 1];
	long* v2 = new long[s2len + 1];

	y = max(s1len, s2len);
	for (x = 0; x < y; x++) {
		if (x < s1len)
			v1[x] = values[x];
		if (x < s2len)
			v2[x] = e->getinteger(x);
	}

	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;

	unsigned int ed;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			if (v1[y - 1] == v2[x - 1])
				ed = 0;
			else
				ed = 1;
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
			lastdiag = olddiag;
		}
	}


	delete[] v1;
	delete[] v2;

	s2len = column[s1len];
	delete[] column;
	return s2len;

}

unsigned int KifVectorFloat::EditDistance(KifElement* e) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = Size();
	s2len = e->Size();

	double* v1 = new double[s1len + 1];
	double* v2 = new double[s2len + 1];

	y = max(s1len, s2len);
	for (x = 0; x < y; x++) {
		if (x < s1len)
			v1[x] = values[x];
		if (x < s2len)
			v2[x] = e->getfloat(x);
	}

	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;

	unsigned int ed;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			if (v1[y - 1] == v2[x - 1])
				ed = 0;
			else
				ed = 1;
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
			lastdiag = olddiag;
		}
	}

	delete[] v1;
	delete[] v2;

	s2len = column[s1len];
	delete[] column;
	return s2len;
}


unsigned int KifEvaluateMap::EditDistance(KifElement* e) {
	vector<KifElement*> vkeys;
	vector<KifElement*> v1;
	vector<KifElement*> v2;

	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	//First of all, we need to identify the common keys...
	int nbfailure = 0;
	KifElement* key;
	KifIteration* itr = (KifIteration*)Newiterator(false);
	for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
		key = itr->IteratorKey();
		if (!e->Index(key)->Boolean()) {
			nbfailure++;
			key->Release();
		}
		else {
			vkeys.push_back(key);
			v1.push_back(itr->IteratorValue());
			switch (key->type) {
			case kifString:
				v2.push_back(e->ValueOnKey(key->String()));
				break;
			case kifInteger:
				v2.push_back(e->ValueOnIntegerKey(key->Integer()));
				break;
			default:
				v2.push_back(e->ValueOnFloatKey(key->Float()));
			}
		}
	}
	itr->Release();

	s1len = vkeys.size();

	if (s1len == 0)
		return nbfailure;

	s2len = s1len;

	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;

	unsigned int ed;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			ed = v1[y - 1]->EditDistance(v2[x - 1]);
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
			lastdiag = olddiag;
		}
	}

	s2len = column[s1len];
	delete[] column;

	for (x = 0; x < s1len; x++) {
		vkeys[x]->Release();
		v1[x]->Release();
		v2[x]->Release();
	}

	s2len += nbfailure;
	return s2len;

}
//--------------------------------------------------------------------------------
void KifSetString::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	if (!ke->aContainer()) {
		values.insert(ke->String());
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.insert(itr->IteratorValueString());
	itr->Release();
}

KifElement* KifSetString::Copy(KifDomain* kp, KifElement* dom) {
	KifSetString* ke = new KifSetString(kifcode, NULL);
	ke->values = values;
	return ke;
}


KifElement* KifSetString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifSetFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifSetString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifSetFunctions.begin(); it != kifGlobals->kifSetFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

Exported KifElement* KifSetString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

string KifSetString::String() {
	KifContainerLock _lock(this);
	string res;
	res = "[";
	bool beg = true;
	string sx;
	std::set<string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res += ",";
		beg = false;
		sx = *it;
		stringing(res, sx);
	}
	res += "]";
	return res;
}

string KifSetString::JSonString() {
	KifContainerLock _lock(this);
	string res;
	res = "[";
	bool beg = true;
	string sx;
	std::set<string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res += ",";
		beg = false;
		sx = *it;
		jstringing(res, sx);
	}
	res += "]";
	return res;
}


KifElement* KifSetString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();

	KifElement* ke;

	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	std::set<string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = kifcode->Providestringraw(*it);
		kvect->Push(ke);
	}
	return kvect;
}

KifElement* KifSetString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();

	char ch[20];
	int i = 0;
	KifElement* ke;
	std::set<string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = kifcode->Providestringraw(*it);
		sprintf_s(ch, 20, "%d", i++);
		kmap->Push(ch, ke);
	}
	return kmap;
}


KifElement* KifSetString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst())
		return this;

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

	std::set<string>::iterator it = values.find(key->String());

	if (it == values.end()) {
		key->Release();
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return kifNOELEMENT;
	}

	if (keyright == NULL)
		return key;

	if (keyright != kifNULL) {
		std::set<string>::iterator itr = values.find(keyright->String());
		if (itr == values.end()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
		KifSetString* kret = new KifSetString(kifcode, NULL);
		while (it != values.end() && it != itr) {
			kret->values.insert(*it);
			it++;
		}

		if (it == values.end()) {
			kret->Release();
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
		kret->values.insert(*itr);
		return kret;
	}

	KifSetString* kret = new KifSetString(kifcode, NULL);
	while (it != values.end()) {
		kret->values.insert(*it);
		it++;
	}

	return kret;
}


bool KifSetString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	if (ke->type == type) {
		values = ((KifSetString*)ke)->values;
		return true;
	}

	values.clear();
	if (!ke->aContainer()) {
		values.insert(ke->String());
		return true;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.insert(itr->IteratorValueString());
	itr->Release();

	return true;
}

KifElement* KifSetString::Newiterator(bool r) {
	KifIteratorSetString* ke = new KifIteratorSetString(kifcode, NULL, this);
	ke->reverse = r;
	return ke;
}


KifElement* KifSetString::same(KifElement* a) {
	if (!a->aContainer() || a->Size() != values.size())
		return kifFALSE;

	KifBaseString bs(NULL, NULL);
	bs.kifcode = kifcode;
	std::set<string>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		bs.value = *it;
		if (!a->Index(&bs)->Boolean())
			return kifFALSE;
	}
	return kifTRUE;
}

KifElement* KifSetString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	//two cases: b is this or not...

	if (b == this) {
		//then we are looking for a in this...
		if (a->aContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
			return kifFALSE;
		}
		string v = a->String();
		if (values.find(v) == values.end()) {
			if (idx)
				return kifMINUSONE;
			return kifFALSE;
		}
		return kifTRUE;
	}

	//The we are storing the comparison of a in b into this...

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		values.clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		KifElement* ke;
		KifElement* res;
		KifElement* found = kifFALSE;
		string itb;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				itb = itr->IteratorKeyString();
				values.insert(itb);
				found = this;
				res->Release();
			}
		}
		return found;
	}

	return b->in(a, b, kfunc, contextualpattern, true);
}

//--------------------------------------------------------------------------------
void KifSetInteger::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	if (!ke->aContainer()) {
		values.insert(ke->Integer());
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.insert(itr->IteratorValueInteger());
	itr->Release();
}

KifElement* KifSetInteger::Copy(KifDomain* kp, KifElement* dom) {
	KifSetInteger* ke = new KifSetInteger(kifcode, NULL);
	ke->values = values;
	return ke;
}


KifElement* KifSetInteger::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifSetFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifSetInteger::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifSetFunctions.begin(); it != kifGlobals->kifSetFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

Exported KifElement* KifSetInteger::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

string KifSetInteger::String() {
	KifContainerLock _lock(this);
	stringstream res;
	res << "[";
	bool beg = true;
	string sx;
	char ch[100];
	std::set<long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sprintf_s(ch, 100, "%ld", *it);
		res << ch;
	}
	res << "]";
	return res.str();
}

string KifSetInteger::JSonString() {
	stringstream res;
	res << "[";
	bool beg = true;
	string sx;
	char ch[100];
	std::set<long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sprintf_s(ch, 100, "%ld", *it);
		res << ch;
	}
	res << "]";
	return res.str();
}


KifElement* KifSetInteger::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();

	KifElement* ke;

	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	std::set<long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = kifcode->Provideinteger(*it);
		kvect->Push(ke);
	}
	return kvect;
}

KifElement* KifSetInteger::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();

	char ch[20];
	int i = 0;
	KifElement* ke;
	std::set<long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = kifcode->Provideinteger(*it);
		sprintf_s(ch, 20, "%d", i++);
		kmap->Push(ch, ke);
	}
	return kmap;
}


KifElement* KifSetInteger::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst())
		return this;

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

	std::set<long>::iterator it = values.find(key->Integer());

	if (it == values.end()) {
		key->Release();
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return kifNOELEMENT;
	}

	if (keyright == NULL)
		return key;

	if (keyright != kifNULL) {
		std::set<long>::iterator itr = values.find(keyright->Integer());
		if (itr == values.end()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
		KifSetInteger* kret = new KifSetInteger(kifcode, NULL);
		while (it != values.end() && it != itr) {
			kret->values.insert(*it);
			it++;
		}

		if (it == values.end()) {
			kret->Release();
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
		kret->values.insert(*itr);
		return kret;
	}

	KifSetInteger* kret = new KifSetInteger(kifcode, NULL);
	while (it != values.end()) {
		kret->values.insert(*it);
		it++;
	}

	return kret;
}


bool KifSetInteger::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	if (ke->type == type) {
		values = ((KifSetInteger*)ke)->values;
		return true;
	}

	values.clear();
	if (!ke->aContainer()) {
		values.insert(ke->Integer());
		return true;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.insert(itr->IteratorValueInteger());
	itr->Release();

	return true;
}

KifElement* KifSetInteger::Newiterator(bool r) {
	KifIteratorSetInteger* ke = new KifIteratorSetInteger(kifcode, NULL, this);
	ke->reverse = r;
	return ke;
}


KifElement* KifSetInteger::same(KifElement* a) {
	if (!a->aContainer() || a->Size() != values.size())
		return kifFALSE;

	KifBaseInteger bs(NULL, NULL);
	bs.kifcode = kifcode;
	std::set<long>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		bs.value = *it;
		if (!a->Index(&bs)->Boolean())
			return kifFALSE;
	}
	return kifTRUE;
}

KifElement* KifSetInteger::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	//two cases: b is this or not...

	if (b == this) {
		//then we are looking for a in this...
		if (a->aContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
			return kifFALSE;
		}
		long v = a->Integer();
		if (values.find(v) == values.end()) {
			if (idx)
				return kifMINUSONE;
			return kifFALSE;
		}
		return kifTRUE;
	}

	//The we are storing the comparison of a in b into this...

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		values.clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		KifElement* ke;
		KifElement* res;
		KifElement* found = kifFALSE;
		long itb;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				itb = itr->IteratorKeyInteger();
				values.insert(itb);
				found = this;
				res->Release();
			}
		}
		return found;
	}

	return b->in(a, b, kfunc, contextualpattern, true);
}
//--------------------------------------------------------------------------------
void KifSetFloat::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	if (!ke->aContainer()) {
		values.insert(ke->Float());
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.insert(itr->IteratorValueFloat());
	itr->Release();
}

KifElement* KifSetFloat::Copy(KifDomain* kp, KifElement* dom) {
	KifSetFloat* ke = new KifSetFloat(kifcode, NULL);
	ke->values = values;
	return ke;
}


KifElement* KifSetFloat::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifSetFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifSetFloat::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifSetFunctions.begin(); it != kifGlobals->kifSetFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

Exported KifElement* KifSetFloat::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

string KifSetFloat::String() {
	KifContainerLock _lock(this);
	stringstream res;
	res << "[";
	bool beg = true;
	string sx;
	char ch[100];
	std::set<double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sprintf_s(ch, 100, "%g", *it);
		res << ch;
	}
	res << "]";
	return res.str();
}

string KifSetFloat::JSonString() {
	KifContainerLock _lock(this);
	stringstream res;
	res << "[";
	bool beg = true;
	string sx;
	char ch[100];
	std::set<double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res << ",";
		beg = false;
		sprintf_s(ch, 100, "%g", *it);
		res << ch;
	}
	res << "]";
	return res.str();
}


KifElement* KifSetFloat::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();

	KifElement* ke;

	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	std::set<double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = kifcode->Providefloat(*it);
		kvect->Push(ke);
	}
	return kvect;
}

KifElement* KifSetFloat::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();

	char ch[20];
	int i = 0;
	KifElement* ke;
	std::set<double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = kifcode->Providefloat(*it);
		sprintf_s(ch, 20, "%l", i++);
		kmap->Push(ch, ke);
	}
	return kmap;
}


KifElement* KifSetFloat::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);
	if (idx == NULL || idx->isConst())
		return this;

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

	std::set<double>::iterator it = values.find(key->Float());

	if (it == values.end()) {
		key->Release();
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return kifNOELEMENT;
	}

	if (keyright == NULL)
		return key;

	if (keyright != kifNULL) {
		std::set<double>::iterator itr = values.find(keyright->Float());
		if (itr == values.end()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
		KifSetFloat* kret = new KifSetFloat(kifcode, NULL);
		while (it != values.end() && it != itr) {
			kret->values.insert(*it);
			it++;
		}

		if (it == values.end()) {
			kret->Release();
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
		kret->values.insert(*itr);
		return kret;
	}

	KifSetFloat* kret = new KifSetFloat(kifcode, NULL);
	while (it != values.end()) {
		kret->values.insert(*it);
		it++;
	}

	return kret;
}


bool KifSetFloat::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	if (ke->type == type) {
		values = ((KifSetFloat*)ke)->values;
		return true;
	}

	values.clear();
	if (!ke->aContainer()) {
		values.insert(ke->Float());
		return true;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.insert(itr->IteratorValueFloat());
	itr->Release();

	return true;
}

KifElement* KifSetFloat::Newiterator(bool r) {
	KifIteratorSetFloat* ke = new KifIteratorSetFloat(kifcode, NULL, this);
	ke->reverse = r;
	return ke;
}


KifElement* KifSetFloat::same(KifElement* a) {
	if (!a->aContainer() || a->Size() != values.size())
		return kifFALSE;

	KifBaseInteger bs(NULL, NULL);
	bs.kifcode = kifcode;
	std::set<double>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		bs.value = *it;
		if (!a->Index(&bs)->Boolean())
			return kifFALSE;
	}
	return kifTRUE;
}

KifElement* KifSetFloat::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	//two cases: b is this or not...

	if (b == this) {
		//then we are looking for a in this...
		if (a->aContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
			return kifFALSE;
		}
		double v = a->Float();
		if (values.find(v) == values.end()) {
			if (idx)
				return kifMINUSONE;
			return kifFALSE;
		}
		return kifTRUE;
	}

	//The we are storing the comparison of a in b into this...

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		values.clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		KifElement* ke;
		KifElement* res;
		KifElement* found = kifFALSE;
		double itb;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				itb = itr->IteratorKeyFloat();
				values.insert(itb);
				found = this;
				res->Release();
			}
		}
		return found;
	}

	return b->in(a, b, kfunc, contextualpattern, true);
}
//--------------------------------------------------------------------------------
void KifSet::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	if (!ke->aContainer()) {
		ke = ke->Atom();
		values.insert(ke);
		ke->Setreference(reference + 1);
		return;
	}

	KifElement* n;
	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		ke = itr->IteratorValue();
		n = ke->Atom();
		values.insert(n);
		n->Setreference(reference + 1);
		ke->Release();
	}
	itr->Release();
}

KifElement* KifSet::Copy(KifDomain* kp, KifElement* dom) {
	KifSet* ke = new KifSet(kifcode, NULL);
	KifElement* n;
	KifElement* k;
	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		k = itr->IteratorValue();
		n = k->Atom();
		n->Setreference();
		ke->values.insert(n);
		k->Release();
	}
	itr->Release();
	return ke;
}

KifElement* KifSet::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifSetFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifSet::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifSetFunctions.begin(); it != kifGlobals->kifSetFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

Exported KifElement* KifSet::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

string KifSet::String() {
	KifContainerLock _lock(this);
	string res;
	kifsetiterator it;
	res = "[";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res += ",";
		beg = false;
		sx = (*it)->StringForDisplay();
		if (!(*it)->aString())
			res += sx;
		else
			stringing(res, sx);
	}
	res += "]";
	return res;
}

string KifSet::JSonString() {
	KifContainerLock _lock(this);
	string res;
	kifsetiterator it;
	res = "[";
	bool beg = true;
	string sx;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res += ",";
		beg = false;
		sx = (*it)->JSonString();
		if (!(*it)->aString())
			res += sx;
		else
			jstringing(res, sx);
	}
	res += "]";
	return res;
}


KifElement* KifSet::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();

	KifElement* ke;

	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	kifsetiterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = (*it)->Atom();
		ke->Setreference();
		kvect->Push(ke);
	}
	return kvect;
}

KifElement* KifSet::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();

	char ch[20];
	int i = 0;
	KifElement* ke;
	kifsetiterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = (*it)->Atom();
		ke->Setreference();
		sprintf_s(ch, 20, "%d", i++);
		kmap->Push(ch, ke);
	}
	return kmap;
}

void KifSet::Clear() {
	KifContainerLock _lock(this);

	kifsetiterator it;
	for (it = values.begin(); it != values.end(); it++)
		(*it)->Resetreference(reference + 1);
	values.clear();
}

KifElement* KifSet::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst())
		return this;

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

	kifsetiterator it = values.find(key);

	if (it == values.end()) {
		key->Release();
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return kifNOELEMENT;
	}

	if (keyright == NULL)
		return key;

	KifElement* ke;
	if (keyright != kifNULL) {
		kifsetiterator itr = values.find(keyright);
		if (itr == values.end()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
		KifSet* kret = new KifSet(kifcode, NULL);
		while (it != values.end() && it != itr) {
			ke = (*it)->Atom();
			ke->Setreference();
			kret->values.insert(ke);
			it++;
		}

		if (it == values.end()) {
			kret->Release();
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
		kret->values.insert(*itr);
		return kret;
	}

	KifSet* kret = new KifSet(kifcode, NULL);
	while (it != values.end()) {
		ke = (*it)->Atom();
		ke->Setreference();
		kret->values.insert(ke);
		it++;
	}

	return kret;
}


bool KifSet::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	if (ke->type == type) {
		values = ((KifSet*)ke)->values;
		return true;
	}

	values.clear();
	if (!ke->aContainer()) {
		Push(ke);
		return true;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	KifElement* n;
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		ke = itr->IteratorValue();
		n = ke->Atom();
		values.insert(n);
		n->Setreference(reference + 1);
		ke->Release();
	}
	itr->Release();

	return true;
}

KifElement* KifSet::Newiterator(bool r) {
	KifIteratorSet* ke = new KifIteratorSet(kifcode, NULL, this);
	ke->reverse = r;
	return ke;
}

void KifSet::Push(KifElement* e) {
	KifContainerLock _lock(this);
	e = e->Atom();
	std::pair<kifsetiterator, bool> ret;
	ret = values.insert(e);
	e->Setreference(reference + 1);
	if (ret.second == false)
		e->Resetreference(reference - 1);
}


KifElement* KifSet::same(KifElement* a) {
	if (!a->aContainer() || a->Size() != values.size())
		return kifFALSE;

	KifIteration* itr = (KifIteration*)a->Newiterator(false);
	kifsetiterator it;
	KifElement* e;
	for (itr->Begin(), it = values.begin(); it != values.end(); itr->Next(), it++) {
		e = itr->IteratorValue();
		if (!(*it)->same(e)->Boolean()) {
			e->Release();
			return kifFALSE;
		}
		e->Release();
	}
	itr->Release();
	return kifTRUE;
}

KifElement* KifSet::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	//two cases: b is this or not...

	if (b == this) {
		//then we are looking for a in this...
		if (a->aContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
			return kifFALSE;
		}
		if (values.find(a) == values.end()) {
			if (idx)
				return kifMINUSONE;
			return kifFALSE;
		}
		return kifTRUE;
	}

	//The we are storing the comparison of a in b into this...

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		values.clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		KifElement* ke;
		KifElement* res;
		KifElement* found = kifFALSE;
		KifElement* itb;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				itb = itr->IteratorKey()->Atom();
				values.insert(itb);
				itb->Setreference(reference + 1);
				found = this;
				res->Release();
			}
		}
		return found;
	}

	return b->in(a, b, kfunc, contextualpattern, true);
}

//---------------------------------------------------------------------------------------------------

KifElement* KifSetString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	string sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueString() + itr->IteratorValueString();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	string sb = b->String();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueString() + sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

//-----------------------------------------------------------------------------------------------------
KifElement* KifSetInteger::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() + itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() + sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetInteger::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->minus(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() - itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() - sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;

}

KifElement* KifSetInteger::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->multiply(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() * itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() * sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;

}

KifElement* KifSetInteger::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->divide(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifSetFloat* ref = new KifSetFloat(kifcode, NULL);

	double sa, sb;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sb = itr->IteratorValueFloat();
			if (sb == 0) {
				it->Release();
				itr->Release();
				ref->Release();
				return kifcode->Returnerror(KIFLIBSTRINGS[225]);
			}
			sa = it->IteratorValueFloat();
			sa /= sb;
			ref->values.insert(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	sb = b->Float();
	if (sb == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger();
		sa /= sb;
		ref->values.insert(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetInteger::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->mod(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa, sb;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sb = itr->IteratorValueInteger();
			if (sb == 0) {
				it->Release();
				itr->Release();
				ref->Release();
				return kifcode->Returnerror(KIFLIBSTRINGS[225]);
			}
			sa = it->IteratorValueInteger() % sb;
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	sb = b->Integer();
	if (sb == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() % sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetInteger::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->shiftright(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() >> itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() >> sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetInteger::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->shiftleft(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() << itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() << sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetInteger::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->power(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifSetFloat* ref = new KifSetFloat(kifcode, NULL);

	double sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = pow(it->IteratorValueFloat(), itr->IteratorValueFloat());
			ref->values.insert(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	double sb = b->Float();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = pow(it->IteratorValueFloat(), sb);
		ref->values.insert(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetInteger::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() ^ itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() ^ sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetInteger::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() | itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() | sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetInteger::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() & itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() & sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

//-----------------------------------------------------------------------------------------------------
KifElement* KifSetFloat::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	double sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueFloat() + itr->IteratorValueFloat();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	double sb = b->Float();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueFloat() + sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetFloat::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->minus(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	double sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueFloat() - itr->IteratorValueFloat();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	double sb = b->Float();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueFloat() - sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;

}

KifElement* KifSetFloat::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->multiply(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	double sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueFloat() * itr->IteratorValueFloat();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	double sb = b->Float();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueFloat() * sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;

}

KifElement* KifSetFloat::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->divide(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	double sa, sb;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sb = itr->IteratorValueFloat();
			if (sb == 0) {
				it->Release();
				itr->Release();
				ref->Release();
				return kifcode->Returnerror(KIFLIBSTRINGS[225]);
			}
			sa = it->IteratorValueFloat() / sb;
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	sb = b->Float();
	if (sb == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueFloat() / sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetFloat::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->mod(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa, sb;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sb = itr->IteratorValueInteger();
			if (sb == 0) {
				it->Release();
				itr->Release();
				ref->Release();
				return kifcode->Returnerror(KIFLIBSTRINGS[225]);
			}
			sa = it->IteratorValueInteger() % sb;
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	sb = b->Integer();
	if (sb == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);

	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() % sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetFloat::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->shiftright(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() >> itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() >> sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetFloat::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->shiftleft(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() << itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() << sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetFloat::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->power(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	double sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = pow(it->IteratorValueFloat(), itr->IteratorValueFloat());
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	double sb = b->Float();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = pow(it->IteratorValueFloat(), sb);
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetFloat::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() ^ itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() ^ sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetFloat::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() | itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() | sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}

KifElement* KifSetFloat::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	long sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueInteger() & itr->IteratorValueInteger();
			ref->storevalue(sa);
		}

		it->Release();
		itr->Release();
		return ref;
	}

	long sb = b->Integer();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueInteger() & sb;
		ref->storevalue(sa);
	}
	it->Release();
	return ref;
}
//----------------------------------------------------------------------------

KifElement* KifVectorByte::Split(KifBaseVector* basevector, KifCallFunction* current) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifRAISEERROR;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifRAISEERROR;

	int iv = 0;
	int szv = values.size();
	KifElement* ret;
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVectorByte* vals = new KifVectorByte(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->values.push_back(values[iv]);
			if (parentval.Split((KifBaseVector*)ret, current) == kifRAISEERROR) {
				vals->Resetreference();
				return kifRAISEERROR;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifRAISEERROR;

		int name = ret->Name();
		KifElement* val = new KifByte(kifcode, NULL, values[iv]);
		if (name != -1)
			current->keep(name, val);
		else {
			//In this case, we need to compare the values...
			if (val->same(ret) == kifFALSE) {
				val->Resetreference();
				return kifRAISEERROR;
			}
		}
		iv++;
	}

	basevector->merge = 1;
	if (iv != values.size())
		return kifRAISEERROR;

	return kifTRUE;
}

KifElement* KifVectorByte::MatchPattern(KifElement* param, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (param->type == kifBaseVector) {
		if (Match((KifBaseVector*)param, dom, idthread, callfunc) == kifNOELEMENT)
			return kifFALSE;
		return kifTRUE;
	}
	return same(param);
}


KifElement* KifVectorByte::Match(KifBaseVector* basevector, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifNOELEMENT;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifNOELEMENT;

	KifElement* ret = kifTRUE;
	KifElement* val;
	int iv = 0;
	int szv = values.size();
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVectorByte* vals = new KifVectorByte(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->values.push_back(values[iv]);

			if (parentval.Match((KifBaseVector*)ret, dom, idthread, callfunc) == kifNOELEMENT) {
				vals->Resetreference();
				return kifNOELEMENT;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifNOELEMENT;

		val = new KifByte(kifcode, NULL, values[iv]);
		if (ret->Name() != -1) {
			ret = ret->Thevariable(dom, idthread, callfunc);
			ret->Setvalue(val);
		}
		else {
			//In this case, we need to compare the values...
			if (val->same(ret) == kifFALSE) {
				val->Release();
				return kifNOELEMENT;
			}
		}
		iv++;
	}
	basevector->merge = 1;
	if (iv != values.size())
		return kifNOELEMENT;
	return kifTRUE;
}

Exported KifElement* KifVectorByte::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

void KifVectorByte::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	//Three cases:
	if (ke->type == type) {
		KifVectorByte* kvect = (KifVectorByte*)ke;
		for (int i = 0; i < kvect->values.size(); i++)
			values.push_back(kvect->values[i]);
		return;
	}

	if (ke->type == kifVectorInteger) {
		KifVectorInteger* kvect = (KifVectorInteger*)ke;
		for (int i = 0; i < kvect->values.size(); i++)
			values.push_back((unsigned char)kvect->values[i]);
		return;
	}

	if (ke->type == kifVectorFloat) {
		KifVectorFloat* kvect = (KifVectorFloat*)ke;
		for (int i = 0; i < kvect->values.size(); i++)
			values.push_back((unsigned char)kvect->values[i]);
		return;
	}

	if (!ke->aContainer()) {
		values.push_back(ke->Integer());
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.push_back((unsigned char)itr->IteratorValueInteger());
	itr->Release();
}

Exported void KifVectorByte::Push(KifElement* ke) {
	if (ke == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values.push_back(ke->Byte());
}


Exported void KifVectorByte::Sort(bool direction) {
	if (direction == false)
		sort(values.begin(), values.end(), compInteger);
	else
		sort(values.begin(), values.end(), icompInteger);
}

string KifVectorByte::String() {
	stringstream res;
	res << "[";
	bool beg = true;
	for (int i = 0; i < values.size(); i++) {
		if (beg == false)
			res << ",";
		else
			beg = false;
		res << (int)values[i];
	}
	res << "]";
	return res.str();
}

KifElement* KifVectorByte::Newiterator(bool rev) {
	KifIteratorVectorByte* iter = new KifIteratorVectorByte(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifVectorByte::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return;
	}

	int i = ke->Integer();
	if (i == -1)
		values.pop_back();
	else {
		if (values.size() <= i) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return;
		}
		values.erase(values.begin() + i);
	}
}

bool KifVectorByte::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifVectorByte) {
			KifVectorByte* kvect = (KifVectorByte*)ke;
			//We copy all values from ke to this
			values = kvect->values;
			return true;
		}

		if (ke->aMapContainer()) {
			Clear();
			values.reserve(ke->Size());
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values.push_back((uchar)itr->IteratorKeyInteger());
			itr->Release();
			return true;
		}

		if (ke->aContainer()) {
			Clear();
			values.reserve(ke->Size());
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values.push_back((uchar)itr->IteratorValueInteger());
			itr->Release();
			return true;
		}
		if (ke->isString()) {
			values.clear();
			string sv = ke->String();
			if (!v_comma_split_byte(sv, values)) {
				kifcode->Returnerror(KIFLIBSTRINGS[206]);
				return true;
			}
			return true;
		}
		ke = ke->Vector(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(KIFLIBSTRINGS[206]);
			return true;
		}
		Clear();
		values.reserve(ke->Size());
		//We copy all values from ke to this
		for (int it = 0; it < ke->Size(); it++)
			values.push_back((uchar)ke->getinteger(it));
		ke->Release();
		return true;
	}
	else {
		//In this specific case, we try to replace a bloc of values with a new bloc
		if (idx->type == kifIndex && ((KifIndex*)idx)->interval) {
			threadblocinit;
			KifContainerLock _lock(this);
			//First we remove the values...
			KifElement* krkey = ((KifIndex*)idx)->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			int lkey = krkey->Integer();
			krkey->Release();
			if (lkey < 0)
				lkey = values.size() + lkey;

			krkey = ((KifIndex*)idx)->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			int rkey = krkey->Integer();
			krkey->Release();
			if (rkey < 0)
				rkey = values.size() + rkey;

			if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
				if (kifGlobals->erroronkey)
					kifcode->Returnerror(KIFLIBSTRINGS[223]);
				return true;
			}

			if (rkey != lkey)
				rkey--;

			while (rkey >= lkey) {
				values.erase(values.begin() + rkey);
				rkey--;
			}
			if (ke->aVectorContainer()) {
				for (int i = ke->Size() - 1; i >= 0; i--) {
					values.insert(values.begin() + lkey, (uchar)ke->getinteger(i));
				}
				return true;
			}

			if (ke->aContainer()) {
				KifIteration* itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
					values.insert(values.begin() + lkey, (uchar)itr->IteratorValueInteger());
				itr->Release();
				return true;
			}

			values.insert(values.begin() + lkey, ke->Byte());
		}
		else {
			register int ikey = idx->Integer();
			int sz = values.size();
			int nb;
			KifContainerLock _lock(this);
			uchar n = ke->Byte();
			if (ikey < 0) {
				ikey = sz + ikey;
				if (ikey < 0)
					kifcode->Returnerror(KIFLIBSTRINGS[206]);
			}

			if (ikey >= sz) {
				for (nb = values.size(); nb < ikey; nb++)
					values.push_back(0);
				values.push_back(n);
			}
			else
				values[ikey] = n;
		}
	}
	return true;
}

KifElement* KifVectorByte::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifVectorFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifVectorByte::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifVectorFunctions.begin(); it != kifGlobals->kifVectorFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifVectorByte::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	//two cases: b is this or not...

	if (b == this) {
		if (a->aContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
			return kifFALSE;
		}

		//then we are looking for a in this...
		uchar v = a->Byte();
		for (int i = 0; i < values.size(); i++) {
			if (values[i] == v) {
				if (idx)
					return new KifByte(kifcode, NULL, i);
				return kifTRUE;
			}
		}
		if (idx)
			return kifMINUSONE;
		return kifFALSE;
	}

	//Then we are storing the comparison of a in b into this...
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		values.clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		KifElement* ke;
		KifElement* res;
		KifElement* found = kifFALSE;
		uchar itb;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				itb = (uchar)itr->IteratorKeyInteger();
				values.push_back(itb);
				found = this;
				res->Release();
			}
		}
		return found;
	}

	return b->in(a, b, kfunc, contextualpattern, true);
}

KifElement* KifVectorByte::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvectorbyte(this, contextualpattern);
			if (kv->type == kifVectorByte)
				((KifVectorByte*)kv)->values = values;
			else
			for (int i = 0; i < values.size(); i++)
				kv->Push(new KifByte(kifcode, NULL, values[i]));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());
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

	register int ikey;
	ikey = key->Integer();

	key->Release();
	int iright;
	if (keyright != NULL)
		iright = keyright->Integer();

	KifVectorByte* kvect;
	if (ikey < 0)
		ikey = values.size() + ikey;

	if (ikey < 0 || ikey >= values.size()) {
		if (ikey != values.size() || keyright == NULL) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}

	if (keyright == NULL)
		return new KifByte(kifcode, NULL, values[ikey]);

	keyright->Release();
	if (iright < 0 || keyright == kifNULL) {
		iright = values.size() + iright;
		if (iright<ikey) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	else {
		if (iright>values.size()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	//In this case, we must create a new vector
	kvect = new KifVectorByte(kifcode, NULL);
	for (int i = ikey; i < iright; i++)
		kvect->values.push_back(values[i]);
	return kvect;
}

KifElement* KifVectorByte::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	char buff[100];
	for (int it = 0; it < values.size(); it++) {
		sprintf_s(buff, 100, "%d", it);
		kmap->Push(buff, new KifByte(kifcode, NULL, values[it]));
	}
	return kmap;
}

KifElement* KifVectorByte::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	for (int i = 0; i < values.size(); i++)
		kvect->Push(new KifByte(kifcode, NULL, values[i]));
	return kvect;
}

Exported KifElement* KifVectorByte::same(KifElement* a) {

	if (a->type != type) {
		if (a->aVector()) {
			if (a->Size() != values.size())
				return kifFALSE;
			for (int i = 0; i < values.size(); i++) {
				if (values[i] != ((KifVector*)a)->values[i]->Byte())
					return kifFALSE;
			}
			return kifTRUE;
		}
		if (a->type == kifVectorInteger) {
			if (a->Size() != values.size())
				return kifFALSE;
			for (int i = 0; i < values.size(); i++) {
				if (((KifVectorInteger*)a)->values[i] != values[i])
					return kifFALSE;
			}
			return kifTRUE;
		}
		if (a->type == kifVectorFloat) {
			if (a->Size() != values.size())
				return kifFALSE;
			for (int i = 0; i < values.size(); i++) {
				if (((KifVectorFloat*)a)->values[i] != values[i])
					return kifFALSE;
			}
			return kifTRUE;
		}
		return kifFALSE;
	}

	KifContainerLock _lock(this);
	KifVectorByte* m = (KifVectorByte*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	for (int i = 0; i < values.size(); i++) {
		if (m->values[i] != values[i])
			return kifFALSE;
	}
	return kifTRUE;
}

KifElement* KifVectorByte::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] += (uchar)itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	unsigned char v = b->Byte();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] += v;
	return ref;
}

KifElement* KifVectorByte::minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] -= (uchar)itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	unsigned char v = b->Byte();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] -= v;
	return ref;
}

KifElement* KifVectorByte::multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] *= (uchar)itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	unsigned char v = b->Byte();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] *= v;
	return ref;
}

KifElement* KifVectorByte::divide(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;
	uchar v;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = (uchar)itr->IteratorValueInteger();
			if (v == 0) {
				itr->Release();
				ref->Release();
				return kifcode->Returnerror(KIFLIBSTRINGS[225]);
			}
			ref->values[it] /= v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Byte();
	if (v == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] /= v;
	return ref;
}

KifElement* KifVectorByte::mod(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;
	uchar v;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			v = (uchar)itr->IteratorValueInteger();
			if (v == 0) {
				itr->Release();
				ref->Release();
				return kifcode->Returnerror(KIFLIBSTRINGS[225]);
			}
			ref->values[it] %= v;
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	v = b->Byte();
	if (v == 0)
		return kifcode->Returnerror(KIFLIBSTRINGS[225]);

	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] %= v;

	return ref;
}

KifElement* KifVectorByte::shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] >>= (uchar)itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	unsigned char v = b->Byte();

	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] >>= v;
	return ref;
}

KifElement* KifVectorByte::shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] <<= (uchar)itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	unsigned char v = b->Byte();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] <<= v;

	return ref;
}

KifElement* KifVectorByte::power(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] = pow((double)ref->values[it], itr->IteratorValueFloat());
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	double v = b->Float();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] = pow((double)ref->values[it], v);
	return ref;
}

KifElement* KifVectorByte::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] |= (uchar)itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	unsigned char v = b->Byte();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] |= v;
	return ref;
}

KifElement* KifVectorByte::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] &= (uchar)itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	unsigned char v = b->Byte();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] &= v;
	return ref;
}

KifElement* KifVectorByte::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifVectorByte* ref = this;
	if (autoself == false)
		ref = (KifVectorByte*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorByte*)a;

	KifContainerLock _lock(this);
	int it;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] ^= (uchar)itr->IteratorValueInteger();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	unsigned char v = b->Byte();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] ^= v;
	return ref;
}

KifElement* KifVectorByte::Copy(KifDomain* kp, KifElement* dom) {
	KifVectorByte* ke = new KifVectorByte(kifcode, NULL);
	ke->values = values;
	return ke;
}

bool KifVectorByte::Permute() {
	return next_permutation(values.begin(), values.end());
}

void KifVectorByte::Shuffle() {
	size_t sz = values.size();
	size_t i, f;
	int mx = sz;
	long v;
	for (i = 0; i < sz; i++) {
		f = localrandom(mx--);
		if (mx != f) {
			v = values[mx];
			values[mx] = values[f];
			values[f] = v;
		}
	}
}

KifElement* KifVectorByte::Unique() {
	KifContainerLock _lock(this);
	KifVectorByte* kvect = new KifVectorByte(kifcode, NULL);
	map<uchar, bool> inter;
	for (int i = 0; i < values.size(); i++) {
		if (inter.find(values[i]) == inter.end()) {
			inter[values[i]] = true;
			kvect->values.push_back(values[i]);
		}
	}
	return kvect;
}

KifElement* KifVectorByte::apply(KifCallFunctionGeneral* kfunclocal, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc) {

	KifElement* kcont = kifNULL;

	if (contextualpattern->aContainer() && contextualpattern != this) {
		kcont = contextualpattern;
		kcont->Clear();
	}
	KifElement* result = kifNULL;
	KifByte kf(NULL, NULL);
	kf.kifcode = dom->kifcode;
	KifBaseInteger key(NULL, NULL);
	key.kifcode = dom->kifcode;
	kf.reference = 1;
	key.reference = 1;
	int idthread = kifGlobals->GetThreadid();

	for (int itx = 0; itx < values.size(); itx++) {
		kf.value = values[itx];
		//values in this case are atomic... They cannot be a function as above, hence this local case
		//We expect a function with one single argument
		kfunclocal->parameters[0] = &kf;
		result = kfunclocal->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (Errorid(idthread))
			return Errorptr();
		if (result != kifNULL) {
			if (kcont == kifNULL)
				kcont = Newinstance();
			if (kcont->aMapContainer()) {
				key.value = itx;
				kcont->Push(&key, result);
			}
			else
				kcont->Push(result);
		}
		values[itx] = kf.value;
	}
	return kcont;
}

unsigned int KifVectorByte::EditDistance(KifElement* e) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = Size();
	s2len = e->Size();

	long* v1 = new long[s1len + 1];
	long* v2 = new long[s2len + 1];

	y = max(s1len, s2len);
	for (x = 0; x < y; x++) {
		if (x < s1len)
			v1[x] = values[x];
		if (x < s2len)
			v2[x] = e->getinteger(x);
	}

	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;

	unsigned int ed;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			if (v1[y - 1] == v2[x - 1])
				ed = 0;
			else
				ed = 1;
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
			lastdiag = olddiag;
		}
	}


	delete[] v1;
	delete[] v2;

	s2len = column[s1len];
	delete[] column;
	return s2len;

}

//--------------------------------------------------------------------------------------------------------------------------
KifElement* KifVectorUString::Split(KifBaseVector* basevector, KifCallFunction* current) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifRAISEERROR;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifRAISEERROR;

	KifElement* ret = kifTRUE;
	KifElement* val;
	int iv = 0;
	int szv = values.size();
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVectorUString* vals = new KifVectorUString(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->values.push_back(values[iv]);
			if (parentval.Split((KifBaseVector*)ret, current) == kifRAISEERROR) {
				vals->Resetreference();
				return kifRAISEERROR;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifRAISEERROR;

		int name = ret->Name();
		val = kifcode->Provideustring(values[iv]);
		if (name != -1)
			current->keep(name, val);
		else {
			//In this case, we need to compare the values...
			if (val->same(ret) == kifFALSE) {
				val->Resetreference();
				return kifRAISEERROR;
			}
		}
		iv++;
	}

	basevector->merge = 1;

	if (iv != values.size())
		return kifRAISEERROR;

	return kifTRUE;
}

KifElement* KifVectorUString::MatchPattern(KifElement* param, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	if (param->type == kifBaseVector) {
		if (Match((KifBaseVector*)param, dom, idthread, callfunc) == kifNOELEMENT)
			return kifFALSE;
		return kifTRUE;
	}
	return same(param);
}

KifElement* KifVectorUString::Match(KifBaseVector* basevector, KifDomain* dom, int idthread, KifCallFunction* callfunc) {
	int sz = basevector->values.size();
	if (!sz) {
		if (values.size() != 0)
			return kifNOELEMENT;
		return kifTRUE;
	}

	if (basevector->merge == 1 && values.size() != sz)
		return kifNOELEMENT;

	KifElement* ret = kifTRUE;
	KifElement* val;
	int iv = 0;
	int szv = values.size();
	for (int i = 0; i < sz; i++) {
		ret = basevector->values[i];
		if (ret->Name() == iduniversal) {
			iv++;
			continue;
		}

		if (ret->type == kifBaseVectorMerge) {
			basevector->merge = 2;
			KifVectorUString* vals = new KifVectorUString(kifcode, NULL);
			//A little hack, we want to match the inner elements
			KifVector parentval(NULL, NULL);
			parentval.kifcode = kifcode;
			parentval.values.push_back(vals);
			parentval.reference = 1;
			parentval.popped = 0;
			vals->values.reserve(szv);
			for (; iv < szv; iv++)
				vals->values.push_back(values[iv]);

			if (parentval.Match((KifBaseVector*)ret, dom, idthread, callfunc) == kifNOELEMENT) {
				vals->Resetreference();
				return kifNOELEMENT;
			}
			return kifTRUE;
		}

		if (iv == szv)
			return kifNOELEMENT;

		val = kifcode->Provideustring(values[iv]);
		if (ret->Name() != -1) {
			ret = ret->Thevariable(dom, idthread, callfunc);
			ret->Setvalue(val);
		}
		else {
			//In this case, we need to compare the values...
			if (val->same(ret) == kifFALSE) {
				val->Release();
				return kifNOELEMENT;
			}
		}
		iv++;
	}
	basevector->merge = 1;
	if (iv != values.size())
		return kifNOELEMENT;
	return kifTRUE;
}

Exported KifElement* KifVectorUString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

void KifVectorUString::addstringto(string ms, int i) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	wstring s;
	s_utf8_to_unicode(s, STR(ms));
	if (i < 0)
		values.back() += s;
	else {
		if (i >= values.size())
			return;
		values[i] += s;
	}
}

void KifVectorUString::addustringto(wstring s, int i) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	if (i < 0)
		values.back() += s;
	else {
		if (i >= values.size())
			return;
		values[i] += s;
	}
}

void KifVectorUString::addstringto(wchar_t s, int i) {
	KifContainerLock _lock(this);
	if (values.size() == 0)
		return;

	if (i < 0)
		values.back() += s;
	else {
		if (i >= values.size())
			return;
		values[i] += s;
	}
}

void KifVectorUString::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	//Three cases:
	if (ke->type == type) {
		KifVectorUString* kvect = (KifVectorUString*)ke;
		for (int i = 0; i < kvect->values.size(); i++)
			values.push_back(kvect->values[i]);
		return;
	}

	if (!ke->aContainer()) {
		values.push_back(ke->UString());
		return;
	}

	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.push_back(itr->IteratorValueUString());
	itr->Release();
}

Exported void KifVectorUString::Sort(bool direction) {
	if (direction == false)
		sort(values.begin(), values.end(), compUString);
	else
		sort(values.begin(), values.end(), icompUString);
}

Exported void KifVectorUString::Push(KifElement* ke) {
	if (ke == kifRAISEERROR)
		return;
	KifContainerLock _lock(this);
	values.push_back(ke->UString());
}

string KifVectorUString::String() {
	string res;
	res = "[";
	bool beg = true;
	string sx;
	for (int i = 0; i < values.size(); i++) {
		if (beg == false)
			res += ",";
		beg = false;
		sc_unicode_to_utf8(sx, values[i]);
		stringing(res, sx);
	}
	res += "]";
	return res;
}

wstring KifVectorUString::UString() {
	wstring res;
	res = L"[";
	bool beg = true;
	wstring sx;
	for (int i = 0; i < values.size(); i++) {
		if (beg == false)
			res += L",";
		beg = false;
		sx += values[i];
		ustringing(res, sx);
	}
	res += L"]";
	return res;
}

string KifVectorUString::JSonString() {
	string res;
	res = "[";
	bool beg = true;
	string sx;
	for (int i = 0; i < values.size(); i++) {
		if (beg == false)
			res += ",";
		beg = false;
		sc_unicode_to_utf8(sx, values[i]);
		jstringing(res, sx);
	}
	res += "]";
	return res;
}

KifElement* KifVectorUString::Newiterator(bool rev) {
	KifIteratorVectorUString* iter = new KifIteratorVectorUString(kifcode, NULL, this);
	iter->reverse = rev;
	iter->values = &values;
	return iter;
}

void KifVectorUString::Pop(KifElement* ke) {
	KifContainerLock _lock(this);
	if (values.size() == 0) {
		if (kifGlobals->erroronkey)
			kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return;
	}

	int i;
	if (ke->isString()) {
		//In that case, we need to get rid of string that matches it...
		wstring s = ke->UString();
		for (i = values.size() - 1; i >= 0; i--) {
			if (values[i] == s)
				values.erase(values.begin() + i);
		}
		return;
	}

	i = ke->Integer();

	if (i == -1)
		values.pop_back();
	else {
		if (values.size() <= i) {
			if (kifGlobals->erroronkey)
				kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return;
		}

		values.erase(values.begin() + i);
	}
}

bool KifVectorUString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	KifContainerLock _lock(this);
	if (ke == this)
		return true;

	if (idx == NULL || idx->isConst()) {
		if (ke == kifNULL) {
			Clear();
			return true;
		}

		if (ke->type == kifVectorUString) {
			KifVectorUString* kvect = (KifVectorUString*)ke;
			//We copy all values from ke to this
			values = kvect->values;
			return true;
		}
		if (ke->aMapContainer()) {
			Clear();
			values.reserve(ke->Size());
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values.push_back(itr->IteratorKeyUString());
			itr->Release();
			return true;
		}
		if (ke->aContainer()) {
			Clear();
			values.reserve(ke->Size());
			KifIteration* itr = (KifIteration*)ke->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
				values.push_back(itr->IteratorValueUString());
			itr->Release();
			return true;
		}
		if (ke->isString()) {
			values.clear();
			wstring sv = ke->UString();
			if (!v_comma_split_string(sv, values)) {
				kifcode->Returnerror(KIFLIBSTRINGS[206]);
				return true;
			}
			return true;
		}
		ke = ke->Vector(dom, NULL);
		if (ke == kifNULL) {
			kifcode->Returnerror(KIFLIBSTRINGS[206]);
			return true;
		}
		Clear();
		values.reserve(ke->Size());
		//We copy all values from ke to this
		for (int it = 0; it < ke->Size(); it++)
			values.push_back(ke->getustring(it));
		ke->Release();
		return true;
	}
	else {
		//In this specific case, we try to replace a bloc of values with a new bloc
		if (idx->type == kifIndex && ((KifIndex*)idx)->interval) {
			threadblocinit;
			KifContainerLock _lock(this);
			//First we remove the values...
			KifElement* krkey = ((KifIndex*)idx)->key->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			int lkey = krkey->Integer();
			krkey->Release();
			if (lkey < 0)
				lkey = values.size() + lkey;

			krkey = ((KifIndex*)idx)->right->Exec(kifNULL, (KifDomain*)dom, kifNULL, idthread, callfunc);
			int rkey = krkey->Integer();
			krkey->Release();
			if (rkey < 0)
				rkey = values.size() + rkey;

			if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
				if (kifGlobals->erroronkey)
					kifcode->Returnerror(KIFLIBSTRINGS[223]);
				return true;
			}

			if (rkey != lkey)
				rkey--;

			while (rkey >= lkey) {
				values.erase(values.begin() + rkey);
				rkey--;
			}

			if (ke->aVectorContainer()) {
				for (int i = ke->Size() - 1; i >= 0; i--) {
					values.insert(values.begin() + lkey, ke->getustring(i));
				}
				return true;
			}

			if (ke->aContainer()) {
				KifIteration* itr = (KifIteration*)ke->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
					values.insert(values.begin() + lkey, itr->IteratorValueUString());
				itr->Release();
				return true;
			}

			values.insert(values.begin() + lkey, ke->UString());
		}
		else {
			register int ikey = idx->Integer();
			int sz = values.size();
			int nb;
			KifContainerLock _lock(this);
			wstring n = ke->UString();
			if (ikey < 0) {
				ikey = sz + ikey;
				if (ikey < 0)
					kifcode->Returnerror(KIFLIBSTRINGS[206]);
			}

			if (ikey >= sz) {
				for (nb = values.size(); nb < ikey; nb++)
					values.push_back(L"");
				values.push_back(n);
			}
			else
				values[ikey] = n;
		}
	}
	return true;
}

KifElement* KifVectorUString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifVectorFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifVectorUString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifVectorFunctions.begin(); it != kifGlobals->kifVectorFunctions.end(); it++)
		fcts->values.push_back(KifIdString(it->first));
}

KifElement* KifVectorUString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	//two cases: b is this or not...

	if (b == this) {
		if (a->aContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}
			if (a->same(b) == kifTRUE)
				return kifTRUE;
			return kifFALSE;
		}

		//then we are looking for a in this...
		wstring v = a->UString();
		for (int i = 0; i < values.size(); i++) {
			if (values[i] == v) {
				if (idx)
					return kifcode->Provideinteger(i);
				return kifTRUE;
			}
		}
		if (idx)
			return kifMINUSONE;
		return kifFALSE;
	}

	//Then we are storing the comparison of a in b into this...
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		values.clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		KifElement* ke;
		KifElement* res;
		KifElement* found = kifFALSE;
		wstring itb;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				itb = itr->IteratorKeyUString();
				values.push_back(itb);
				found = this;
				res->Release();
			}
		}
		return found;
	}

	return b->in(a, b, kfunc, contextualpattern, true);
}

KifElement* KifVectorUString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst()) {
		KifElement* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->aVector()) {
			kv = Selectvectorustring(this, contextualpattern);
			if (kv->type == kifVectorUString)
				((KifVectorUString*)kv)->values = values;
			else
			for (int i = 0; i < values.size(); i++)
				kv->Push(kifcode->Provideustring(values[i]));
			return kv;
		}

		if (contextualpattern->type == kifInteger || contextualpattern->type == kifFloat)
			return kifcode->Provideinteger(values.size());
		return this;
	}

	KifElement* key;
	KifElement* keyright = NULL;
	bool sleft = false;
	bool sright = false;

	if (idx->type == kifIndex) {
		KifIndex* kind = (KifIndex*)idx;
		sleft = kind->signkey;
		sright = kind->signright;
		key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
		if (kind->interval == true)
			keyright = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
	}
	else
		key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

	register int ikey;
	bool stringkey = false;
	if (key->type == kifString) {
		stringkey = true;
		wstring sf = key->UString();
		bool found = false;
		if (sleft) {
			for (ikey = values.size() - 1; ikey >= 0; ikey--) {
				if (sf == values[ikey]) {
					found = true;
					break;
				}
			}
		}
		else {
			for (ikey = 0; ikey < values.size(); ikey++) {
				if (sf == values[ikey]) {
					found = true;
					break;
				}
			}
		}
		if (!found) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	else
		ikey = key->Integer();

	key->Release();
	int iright;
	if (keyright != NULL) {
		if (keyright->type == kifString) {
			wstring sf = keyright->UString();
			bool found = false;
			if (sright) {
				for (iright = values.size() - 1; iright >= 0; iright--) {
					if (sf == values[iright]) {
						found = true;
						iright++;
						break;
					}
				}
			}
			else {
				for (iright = 0; iright < values.size(); iright++) {
					if (sf == values[iright]) {
						found = true;
						iright++;
						break;
					}
				}
			}
			if (!found) {
				if (kifGlobals->erroronkey)
					return kifcode->Returnerror(KIFLIBSTRINGS[223]);
				return kifNOELEMENT;
			}
		}
		else {
			if (keyright == kifNULL)
				iright = 0;
			else {
				iright = keyright->Integer();
				if (stringkey && iright >= 0)
					iright = ikey + iright + 1;
			}
		}
	}

	KifVectorUString* kvect;
	if (ikey < 0)
		ikey = values.size() + ikey;

	if (ikey < 0 || ikey >= values.size()) {
		if (ikey != values.size() || keyright == NULL) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}

	if (keyright == NULL)
		return kifcode->Provideustring(values[ikey]);

	keyright->Release();
	if (iright < 0 || keyright == kifNULL) {
		iright = values.size() + iright;
		if (iright<ikey) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}
	else  {
		if (iright>values.size()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}
	}

	//In this case, we must create a new vector
	kvect = new KifVectorUString(kifcode, NULL);
	for (int i = ikey; i < iright; i++)
		kvect->values.push_back(values[i]);
	return kvect;
}

KifElement* KifVectorUString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();
	char buff[100];
	for (int it = 0; it < values.size(); it++) {
		sprintf_s(buff, 100, "%d", it);
		kmap->Push(buff, kifcode->Provideustring(values[it]));
	}
	return kmap;
}

KifElement* KifVectorUString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();
	kvect->values.reserve(values.size());
	for (int i = 0; i < values.size(); i++)
		kvect->Push(kifcode->Provideustring(values[i]));
	return kvect;
}

Exported KifElement* KifVectorUString::same(KifElement* a) {

	if (a->type != type) {
		if (a->aVector()) {
			if (a->Size() != values.size())
				return kifFALSE;
			for (int i = 0; i < values.size(); i++) {
				if (values[i] != ((KifVector*)a)->values[i]->UString())
					return kifFALSE;
			}
			return kifTRUE;
		}
		return kifFALSE;
	}

	KifContainerLock _lock(this);
	KifVectorUString* m = (KifVectorUString*)a;
	if (m->values.size() != values.size())
		return kifFALSE;

	for (int i = 0; i < values.size(); i++) {
		if (m->values[i] != values[i])
			return kifFALSE;
	}
	return kifTRUE;
}

KifElement* KifVectorUString::orset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->orset(a, b, context, autoself);

	KifContainerLock _lock(this);

	if (b->aVectorContainer()) {
		KifContainerLock _lock((KifEvaluate*)b);
		bool found = false;
		KifVectorUString* A = (KifVectorUString*)a;
		KifVectorUString* ref = (KifVectorUString*)a->Copy(NULL);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
			found = false;
			for (int it = 0; it < A->values.size(); it++) {
				if (A->values[it] == itr->IteratorValueUString()) {
					found = true;
					break;
				}
			}
			if (found == false)
				ref->values.push_back(itr->IteratorValueUString());
		}
		itr->Release();
		if (autoself && a == this) {
			values = ref->values;
			ref->Release();
			return this;
		}

		return ref;
	}
	return kifNULL;
}

KifElement* KifVectorUString::andset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->andset(a, b, context, autoself);
	KifContainerLock _lock(this);

	if (b->aVectorContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		KifVectorUString* A = (KifVectorUString*)a;
		KifVectorUString* ref = new KifVectorUString(kifcode, NULL);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);

		for (itr->Begin(); itr->End() != kifTRUE; itr->Next()) {
			for (int it = 0; it < A->values.size(); it++) {
				if (A->values[it] == itr->IteratorValueUString()) {
					ref->values.push_back(itr->IteratorValueUString());
					break;
				}
			}
		}
		itr->Release();

		if (autoself && a == this) {
			values = ref->values;
			ref->Release();
			return this;
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifVectorUString::xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->xorset(a, b, context, autoself);

	KifContainerLock _lock(this);

	if (b->aVectorContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);

		bool found = false;
		vector<wstring>& val = ((KifVectorUString*)a)->values;
		KifVectorUString* ref;
		hmap<wstring, bool> kept;
		int itx;
		if (b->type == kifVectorString) {
			ref = (KifVectorUString*)b;
			for (itx = 0; itx < ref->values.size(); itx++)
				kept[ref->values[itx]] = true;
		}
		else {
			KifIteration* itr = (KifIteration*)b->Newiterator(false);
			for (itr->Begin(); itr->End() != kifTRUE; itr->Next())
				kept[itr->IteratorValueUString()] = true;
			itr->Release();
		}

		for (itx = 0; itx < val.size(); itx++) {
			if (kept.find(val[itx]) == kept.end())
				kept[val[itx]] = true;
			else
				kept.erase(val[itx]);
		}

		ref = new KifVectorUString(kifcode, NULL);
		hmap<wstring, bool>::iterator it;
		for (it = kept.begin(); it != kept.end(); it++)
			ref->values.push_back(it->first);

		if (autoself && a == this) {
			values = ref->values;
			ref->Release();
			return this;
		}
		return ref;
	}

	return kifNULL;
}

KifElement* KifVectorUString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {

	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifVectorUString* ref = this;
	if (autoself == false)
		ref = (KifVectorUString*)a->Copy(NULL);
	else
	if (a != this)
		ref = (KifVectorUString*)a;

	KifContainerLock _lock(this);
	int it;
	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		itr->Begin();
		for (it = 0; it < ref->values.size(); it++) {
			if (itr->End() == kifTRUE)
				break;
			ref->values[it] += itr->IteratorValueUString();
			itr->Next();
		}
		itr->Release();
		return ref;
	}

	wstring v = b->UString();
	for (it = 0; it < ref->values.size(); it++)
		ref->values[it] += v;
	return ref;
}

KifElement* KifVectorUString::Copy(KifDomain* kp, KifElement* dom) {
	KifVectorUString* ke = new KifVectorUString(kifcode, NULL);
	ke->values = values;
	return ke;
}

bool KifVectorUString::Permute() {
	return next_permutation(values.begin(), values.end());
}

void KifVectorUString::Shuffle() {
	size_t sz = values.size();
	size_t i, f;
	int mx = sz;
	wstring v;
	for (i = 0; i < sz; i++) {
		f = localrandom(mx--);
		if (mx != f) {
			v = values[mx];
			values[mx] = values[f];
			values[f] = v;
		}
	}
}

KifElement* KifVectorUString::Unique() {
	KifContainerLock _lock(this);
	KifVectorUString* kvect = new KifVectorUString(kifcode, NULL);
	map<wstring, bool> inter;
	for (int i = 0; i < values.size(); i++) {
		if (inter.find(values[i]) == inter.end()) {
			inter[values[i]] = true;
			kvect->values.push_back(values[i]);
		}
	}
	return kvect;
}

KifElement* KifVectorUString::apply(KifCallFunctionGeneral* kfunclocal, KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc) {

	KifElement* kcont = kifNULL;

	if (contextualpattern->aContainer() && contextualpattern != this) {
		kcont = contextualpattern;
		kcont->Clear();
	}
	KifElement* result = kifNULL;
	KifBaseUString kf(NULL, NULL, 0);
	kf.kifcode = dom->kifcode;
	KifBaseInteger key(NULL, NULL);
	key.kifcode = dom->kifcode;
	kf.reference = 1;
	key.reference = 1;
	int idthread = kifGlobals->GetThreadid();

	for (int itx = 0; itx < values.size(); itx++) {
		kf.value = values[itx];
		//values in this case are atomic... They cannot be a function as above, hence this local case
		//We expect a function with one single argument
		kfunclocal->parameters[0] = &kf;
		result = kfunclocal->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);
		if (Errorid(idthread))
			return Errorptr();
		if (result != kifNULL) {
			if (kcont == kifNULL)
				kcont = Newinstance();
			if (kcont->aMapContainer()) {
				key.value = itx;
				kcont->Push(&key, result);
			}
			else
				kcont->Push(result);
		}
		values[itx] = kf.value;
	}
	return kcont;
}

unsigned int KifVectorUString::EditDistance(KifElement* e) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = Size();
	s2len = e->Size();

	wstring* v1 = new wstring[s1len + 1];
	wstring* v2 = new wstring[s2len + 1];

	y = max(s1len, s2len);
	for (x = 0; x < y; x++) {
		if (x < s1len)
			v1[x] = values[x];
		if (x < s2len)
			v2[x] = e->getustring(x);
	}

	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;

	unsigned int ed;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			ed = StringEditDistance(v1[y - 1], v2[x - 1]);
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
			lastdiag = olddiag;
		}
	}

	delete[] v1;
	delete[] v2;

	s2len = column[s1len];
	delete[] column;
	return s2len;

}
//---------------------------------------------------------------------------------
void KifSetUString::Merge(KifElement* ke) {
	KifContainerLock _lock(this);
	if (!ke->aContainer()) {
		values.insert(ke->UString());
		return;
	}


	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.insert(itr->IteratorValueUString());
	itr->Release();
}

KifElement* KifSetUString::Copy(KifDomain* kp, KifElement* dom) {
	KifSetUString* ke = new KifSetUString(kifcode, NULL);
	ke->values = values;
	return ke;
}

KifElement* KifSetUString::Declaration(short n) {
	if (TestkifFunctions(n, kifGlobals->kifSetFunctions) == true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifSetUString::Methods(KifVectorString* fcts) {
	hmap<short, bool>::iterator it;
	for (it = kifGlobals->kifSetFunctions.begin(); it != kifGlobals->kifSetFunctions.end(); it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);
}

Exported KifElement* KifSetUString::Information(KifElement* kstr) {
	string s = kstr->String();
	if (kifGlobals->kifInformationVector.find(s) == kifGlobals->kifInformationVector.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationVector[s]);
}

wstring KifSetUString::UString() {
	KifContainerLock _lock(this);
	wstring res;
	res = L"[";
	bool beg = true;
	wstring sx;
	std::set<wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res += L",";
		beg = false;
		sx = *it;
		ustringing(res, sx);
	}

	res += L"]";
	return res;
}


string KifSetUString::String() {
	KifContainerLock _lock(this);
	string res;
	res = "[";
	bool beg = true;
	string sx;
	wstring ws;
	std::set<wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res += ",";
		beg = false;
		ws = *it;
		sc_unicode_to_utf8(sx, ws);
		stringing(res, sx);
	}

	res += "]";
	return res;
}

string KifSetUString::JSonString() {
	KifContainerLock _lock(this);
	string res;
	res = "[";
	bool beg = true;
	string sx;
	wstring ws;
	std::set<wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		if (beg == false)
			res += ",";
		beg = false;
		ws = *it;
		sc_unicode_to_utf8(sx, ws);
		jstringing(res, sx);
	}

	res += "]";
	return res;
}

KifElement* KifSetUString::Vector(KifElement* contextualpattern, KifVector* v) {
	KifContainerLock _lock(this);
	KifVector* kvect = v;
	if (kvect == NULL)
		kvect = Selectvector(this, contextualpattern);
	else
		kvect->Clear();

	KifElement* ke;

	//We copy all values from ke to this
	kvect->values.reserve(values.size());
	std::set<wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = kifcode->Provideustring(*it);
		kvect->Push(ke);
	}

	return kvect;
}

KifElement* KifSetUString::Map(KifElement* contextualpattern, KifMap* m) {
	KifContainerLock _lock(this);
	KifMap* kmap = m;
	if (kmap == NULL)
		kmap = Selectmap(this, contextualpattern);
	else
		kmap->Clear();

	char ch[20];
	int i = 0;
	KifElement* ke;
	std::set<wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		ke = kifcode->Provideustring(*it);
		sprintf_s(ch, 20, "%d", i++);
		kmap->Push(ch, ke);
	}

	return kmap;
}

KifElement* KifSetUString::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	KifContainerLock _lock(this);

	if (idx == NULL || idx->isConst())
		return this;

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

	std::set<wstring>::iterator it = values.find(key->UString());

	if (it == values.end()) {
		key->Release();
		if (kifGlobals->erroronkey)
			return kifcode->Returnerror(KIFLIBSTRINGS[223]);
		return kifNOELEMENT;
	}


	if (keyright == NULL)
		return key;

	if (keyright != kifNULL) {
		std::set<wstring>::iterator itr = values.find(keyright->UString());
		if (itr == values.end()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}

		KifSetUString* kret = new KifSetUString(kifcode, NULL);
		while (it != values.end() && it != itr) {
			kret->values.insert(*it);
			it++;
		}


		if (it == values.end()) {
			kret->Release();
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(KIFLIBSTRINGS[223]);
			return kifNOELEMENT;
		}

		kret->values.insert(*itr);
		return kret;
	}


	KifSetUString* kret = new KifSetUString(kifcode, NULL);
	while (it != values.end()) {
		kret->values.insert(*it);
		it++;
	}


	return kret;
}

bool KifSetUString::Setvalue(KifElement* ke, KifElement* idx, KifElement* dom) {
	if (ke == this)
		return true;

	if (ke->type == type) {
		values = ((KifSetUString*)ke)->values;
		return true;
	}


	values.clear();
	if (!ke->aContainer()) {
		values.insert(ke->UString());
		return true;
	}


	KifIteration* itr = (KifIteration*)ke->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
		values.insert(itr->IteratorValueUString());
	itr->Release();

	return true;
}

KifElement* KifSetUString::Newiterator(bool r) {
	KifIteratorSetUString* ke = new KifIteratorSetUString(kifcode, NULL, this);
	ke->reverse = r;
	return ke;
}

KifElement* KifSetUString::same(KifElement* a) {
	if (!a->aContainer() || a->Size() != values.size())
		return kifFALSE;

	KifBaseUString bs(NULL, NULL, L"");
	bs.kifcode = kifcode;
	std::set<wstring>::iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		bs.value = *it;
		if (!a->Index(&bs)->Boolean())
			return kifFALSE;
	}

	return kifTRUE;
}

KifElement* KifSetUString::in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
	KifContainerLock _lock(this);

	//two cases: b is this or not...

	if (b == this) {
		//then we are looking for a in this...
		if (a->aContainer()) {
			if (a->Size() == 0) {
				if (idx == true)
					return kifNOELEMENT;
				return kifTRUE;
			}

			if (a->same(b) == kifTRUE)
				return kifTRUE;
			return kifFALSE;
		}

		wstring v = a->UString();
		if (values.find(v) == values.end()) {
			if (idx)
				return kifMINUSONE;
			return kifFALSE;
		}

		return kifTRUE;
	}


	//The we are storing the comparison of a in b into this...

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		values.clear();
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		KifElement* ke;
		KifElement* res;
		KifElement* found = kifFALSE;
		wstring itb;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			ke = itr->IteratorValue();
			if (a->aContainer() && !ke->aContainer() && ke->type != kifDomain)
				continue;
			if (kfunc == NULL)
				res = ke->in(a, ke, NULL, this, idx);
			else
				res = applyfunc(a, ke, kfunc);
			ke->Release();
			if (res->Succeed()) {
				itb = itr->IteratorKeyUString();
				values.insert(itb);
				found = this;
				res->Release();
			}

		}

		return found;
	}


	return b->in(a, b, kfunc, contextualpattern, true);
}

KifElement* KifSetUString::plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
	if (a->type != type)
		return a->plus(a, b, context, autoself);

	KifContainerLock _lock(this);
	KifElement* ref = a->Newinstance();

	wstring sa;

	if (b->aContainer()) {
		KifContainerLock _lockb((KifEvaluate*)b);
		KifIteration* it = (KifIteration*)a->Newiterator(false);
		KifIteration* itr = (KifIteration*)b->Newiterator(false);
		for (it->Begin(), itr->Begin(); it->End() == kifFALSE && itr->End() == kifFALSE; it->Next(), itr->Next()) {
			sa = it->IteratorValueUString() + itr->IteratorValueUString();
			ref->storevalue(sa);
		}


		it->Release();
		itr->Release();
		return ref;
	}


	wstring sb = b->UString();
	KifIteration* it = (KifIteration*)a->Newiterator(false);
	for (it->Begin(); it->End() == kifFALSE; it->Next()) {
		sa = it->IteratorValueUString() + sb;
		ref->storevalue(sa);
	}

	it->Release();
	return ref;
}



