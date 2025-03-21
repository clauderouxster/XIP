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
   filename   : kifmaps.h
   Date       : 09/09/2010
   Purpose    : Implementation of the KiF language
   Programmer : Claude ROUX
   Reviewer   : 
*/


#ifndef i_kifmaps
#define i_kifmaps

class KifIntegerMap : public KifEvaluate {
public:

	hmap<long,KifElement*> values;	

	void Items(KifVector* kvect) {
		hmap<long,KifElement*>::iterator it;
		KifIntegerMap* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifIntegerMap(kifcode,NULL);
			kval->push(it->first,it->second);
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	Exported KifIntegerMap(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifIntegerMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	void storeInLoop(vector<KifElement*>& alls);

	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);
	Exported KifElement* Map(KifElement*,KifMap* itself=NULL) {
		return this;
	}

	bool aMapContainer() {
		return true;
	}

	int Garbager() {
		return kifMap;
	}

	void push(long l,KifElement*);

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
		if (values.find(n)!=values.end())
			return values[n];
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* ke) {
		KifContainerLock _lock(this);
		long s=ke->Integer();
		if (values.find(s)==values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newinstance() {
		return kifcode->Providemap();
	}
	Exported string String();
    Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string k) {
        KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
		hmap<long,KifElement*>::iterator iter=values.find(n);
		if (iter==values.end())
			return kifNOELEMENT;
		return iter->second;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* idx=kifNULL,bool rcouple=false);
	Exported bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		hmap<long,KifElement*>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second->Sum();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		hmap<long,KifElement*>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v*=itx->second->Product();
		return v;
	}
	
	Exported void Setreference(int inc=1);

	Exported void Setpopped(int v=1,bool force=false) {
		if (reference<=0 || force) {
			Putpopped(v);
			hmap<long,KifElement*>::iterator itx;
			for (itx=values.begin();itx!=values.end();itx++) {
				if (itx->second!=NULL)
					itx->second->Setpopped(v,force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		hmap<long,KifElement*>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++) {
			if (itx->second!=NULL)
				itx->second->Clean();
		}
		values.clear();
		if (reference<=0)
            KifElement::Resetreference();
	}

	Exported void Resetreference(int inc=1);
	Exported void Clear();


	Exported long Size() {
		KifContainerLock _lock(this);
		return values.size();
	}

	Exported KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* contextualpattern,bool idx);

	Exported long Integer() {
		KifContainerLock _lock(this);
		return values.size();
	}
	Exported double Float() {
		KifContainerLock _lock(this);
		return values.size();
	}

	Exported bool Boolean() {
		KifContainerLock _lock(this);
		if (values.size()==0)
			return false;
		return true;
	}

	//To insert a value into a map through the API 
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* minus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* multiply(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* divide(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* mod(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* power(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);

};

class KifFloatMap : public KifEvaluate {
public:

	hmap<double,KifElement*> values;	

	void Items(KifVector* kvect) {
		hmap<double,KifElement*>::iterator it;
		KifFloatMap* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifFloatMap(kifcode,NULL);
			kval->push(it->first,it->second);
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}				
	}

	Exported KifFloatMap(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifFloatMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	void storeInLoop(vector<KifElement*>& alls);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);
	Exported KifElement* Map(KifElement*,KifMap* itself=NULL) {
		return this;
	}

	bool aMapContainer() {
		return true;
	}

	int Garbager() {
		return kifMap;
	}

	void push(double l,KifElement*);

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)!=values.end())
			return values[n];
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* ke) {
		KifContainerLock _lock(this);
		double s=ke->Float();
		if (values.find(s)==values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newinstance() {
		return kifcode->Providemap();
	}
	Exported string String();
    Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string k) {
        KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		hmap<double,KifElement*>::iterator iter=values.find(n);
		if (iter==values.end())
			return kifNOELEMENT;
		return iter->second;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* idx=kifNULL,bool rcouple=false);
	Exported bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		hmap<double,KifElement*>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second->Sum();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		hmap<double,KifElement*>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v*=itx->second->Product();
		return v;
	}
	
	Exported void Setreference(int inc=1);

	Exported void Setpopped(int v=1,bool force=false) {
		if (reference<=0 || force) {
			Putpopped(v);
			hmap<double,KifElement*>::iterator itx;
			for (itx=values.begin();itx!=values.end();itx++) {
				if (itx->second!=NULL)
					itx->second->Setpopped(v,force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		hmap<double,KifElement*>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++) {
			if (itx->second!=NULL)
				itx->second->Clean();
		}
		values.clear();
		if (reference<=0)
            KifElement::Resetreference();
	}

	Exported void Resetreference(int inc=1);
	Exported void Clear();


	Exported long Size() {
		KifContainerLock _lock(this);
		return values.size();
	}

	Exported KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* contextualpattern,bool idx);

	Exported long Integer() {
		KifContainerLock _lock(this);
		return values.size();
	}
	Exported double Float() {
		KifContainerLock _lock(this);
		return values.size();
	}

	Exported bool Boolean() {
		KifContainerLock _lock(this);
		if (values.size()==0)
			return false;
		return true;
	}

	//To insert a value into a map through the API 
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* minus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* multiply(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* divide(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* mod(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* power(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);

};

class KifIteratorIntegerMap : public KifIteration {
public:
	hmap<long,KifElement*>* values;
	hmap<long,KifElement*>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<long,KifElement*>::iterator itr;
#else
	hmap<long,KifElement*>::reverse_iterator itr;
#endif

	KifIntegerMap* kbase;

	KifIteratorIntegerMap(KifCode* klc,KifElement* kp,KifIntegerMap* k) : kbase(k),KifIteration(klc,kp) {}

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* val=kifNULL,bool rcouple=false);
	Exported KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNOELEMENT;
			return kifcode->Provideinteger(it->first);		
		}
		if (itr==values->Rend())
			return kifNOELEMENT;
		return kifcode->Provideinteger(itr->first);
	}

	long IteratorKeyInteger() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	double IteratorKeyFloat() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	string IteratorKeyString();

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		return it->second->String();
	}

	Exported KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNOELEMENT;
			return it->second;
		}
		if (itr==values->Rend())
			return kifNOELEMENT;
		return itr->second;
	}

	Exported void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	Exported KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
			return kifFALSE;
		}
		if (itr==values->Rend())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {	
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifIteratorFloatMap : public KifIteration {
public:
	hmap<double,KifElement*>* values;
	hmap<double,KifElement*>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<double,KifElement*>::iterator itr;
#else
	hmap<double,KifElement*>::reverse_iterator itr;
#endif

	KifFloatMap* kbase;

	KifIteratorFloatMap(KifCode* klc,KifElement* kp,KifFloatMap* k) : kbase(k),KifIteration(klc,kp) {}

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* val=kifNULL,bool rcouple=false);
	Exported KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNOELEMENT;
			return kifcode->Providefloat(it->first);		
		}
		if (itr==values->Rend())
			return kifNOELEMENT;
		return kifcode->Providefloat(itr->first);
	}

	long IteratorKeyInteger() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	double IteratorKeyFloat() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	string IteratorKeyString();
	
	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		return it->second->String();
	}

	Exported KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNOELEMENT;
			return it->second;
		}
		if (itr==values->Rend())
			return kifNOELEMENT;
		return itr->second;
	}

	Exported void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	Exported KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
			return kifFALSE;
		}
		if (itr==values->Rend())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {	
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifMapString : public KifEvaluate {
public:
	hmap<string,string> values;

	void storekey(char* k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void storekey(string k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		hmap<string,string>::iterator it;
		KifMapString* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapString(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapString(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifMapString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void Merge(KifElement* ke);	
	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}

	Exported KifElement* Atom(bool forced=false) {
		if (reference==0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	Exported KifType theType() {
		return kifMapString;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end())
			return kifcode->Providestring(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end())
			return kifcode->Providestring(values[buffer]);
		return kifNOELEMENT;
	}
	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	string ValueOnKeyString(string k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return "";
	}


	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		string i=s->String();
		if ( values.find(i)== values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifMapString(kifcode,NULL);
	}

	string String();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		if ( values.find(n)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple=false);

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);


	void Clear() {
		KifContainerLock _lock(this);
		 values.clear();
	}

	Exported KifElement* Map(KifElement*,KifMap* itself=NULL);	
	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);

};

class KifIteratorMapString : public KifIteration {
public:

	hmap<string,string>* values;

	hmap<string,string>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<string,string>::iterator itr;
#else
	hmap<string,string>::reverse_iterator itr;
#endif
    
	KifMapString* kbase;
	KifIteratorMapString(KifCode* klc,KifElement* kp,KifMapString* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->String();
			return;
		}
		if (itr==values->Rend())
			return;
		itr->second=ke->String();
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->first);
		}

		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providestringraw(itr->first);
	}

	long IteratorKeyInteger() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return s_XLConvert(STR(it->first));
		}

		if (itr==values->Rend())
			return -1;

		return s_XLConvert(STR(itr->first));
	}

	double IteratorKeyFloat() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return convertfloat(STR(it->first));
		}

		if (itr==values->Rend())
			return -1;

		return convertfloat(STR(itr->first));
	}

	string IteratorKeyString() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return "";

			return it->first;
		}
		if (itr==values->Rend())
			return "";

		return itr->first;
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return "";
			return it->second;
		}
		if (itr==values->Rend())
			return "";

		return itr->second;
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->second);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providestringraw(itr->second);
	}

	void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
		}
		else{
			if (itr==values->Rend())
				return kifTRUE;		
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;		
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;

		return kifTRUE;
	}

};

class KifMapInteger : public KifEvaluate {
public:
	hmap<string,long> values;

	void storekey(char* k,long v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void storekey(string k,long v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		hmap<string,long>::iterator it;
		KifMapInteger* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapInteger(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapInteger(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifMapInteger) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void Merge(KifElement* ke);

	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}

	Exported KifElement* Atom(bool forced=false) {
		if (reference==0 && popped && !forced)
			return this;

		return Copy(NULL);
	}
	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		hmap<string,long>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		hmap<string,long>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end())
			return kifcode->Provideinteger(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end())
			return kifcode->Provideinteger(values[buffer]);
		return kifNOELEMENT;
	}


	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyIInteger(long k,bool& a) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end()) {
			a=true;
			return values[buffer];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyIFloat(long k,bool& a) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end()) {
			a=true;
			return values[buffer];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyFInteger(double k,bool& a) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end()) {
			a=true;
			return values[buffer];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFFloat(double k,bool& a) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end()) {
			a=true;
			return values[buffer];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifMapInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		string i=s->String();
		if ( values.find(i)== values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifMapInteger(kifcode,NULL);
	}

	string String();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		if ( values.find(n)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple=false);

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);


	void Clear() {
		KifContainerLock _lock(this);
		 values.clear();
	}

	Exported KifElement* Map(KifElement*,KifMap* itself=NULL);	
	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* minus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* multiply(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* divide(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* mod(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* power(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
};

class KifIteratorMapInteger : public KifIteration {
public:

	hmap<string,long>* values;

	hmap<string,long>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<string,long>::iterator itr;
#else
	hmap<string,long>::reverse_iterator itr;
#endif
    KifMapInteger* kbase;
	KifIteratorMapInteger(KifCode* klc,KifElement* kp,KifMapInteger* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Integer();
			return;
		}
		if (itr==values->Rend())
			return;
		itr->second=ke->Integer();
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->first);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providestringraw(itr->first);
	}

	long IteratorKeyInteger() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return s_XLConvert(STR(it->first));
		}

		if (itr==values->Rend())
			return -1;

		return s_XLConvert(STR(itr->first));
	}

	double IteratorKeyFloat() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return convertfloat(STR(it->first));
		}

		if (itr==values->Rend())
			return -1;

		return convertfloat(STR(itr->first));
	}

	string IteratorKeyString() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return "";

			return it->first;
		}
		if (itr==values->Rend())
			return "";

		return itr->first;
	}

	long IteratorValueInteger() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->second);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Provideinteger(itr->second);
	}

	void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
		}
		else{
			if (itr==values->Rend())
				return kifTRUE;		
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifMapFloat : public KifEvaluate {
public:
	hmap<string,double> values;

	void storekey(char* k,double v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void storekey(string k,double v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		hmap<string,double>::iterator it;
		KifMapFloat* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapFloat(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapFloat(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifMapFloat) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void Merge(KifElement* ke);

	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}
	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end())
			return kifcode->Providefloat(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end())
			return kifcode->Providefloat(values[buffer]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	Exported KifElement* Atom(bool forced=false) {
		if (reference==0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyIInteger(long k,bool& a) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end()) {
			a=true;
			return values[buffer];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyIFloat(long k,bool& a) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end()) {
			a=true;
			return values[buffer];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyFInteger(double k,bool& a) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end()) {
			a=true;
			return values[buffer];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFFloat(double k,bool& a) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer)!=values.end()) {
			a=true;
			return values[buffer];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifMapFloat;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		hmap<string,double>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		hmap<string,double>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		string i=s->String();
		if ( values.find(i)== values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifMapFloat(kifcode,NULL);
	}

	string String();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		if ( values.find(n)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple=false);

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);


	void Clear() {
		KifContainerLock _lock(this);
		 values.clear();
	}

	Exported KifElement* Map(KifElement*,KifMap* itself=NULL);	
	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* minus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* multiply(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* divide(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* mod(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* power(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
};

class KifIteratorMapFloat : public KifIteration {
public:

	hmap<string,double>* values;

	hmap<string,double>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<string,double>::iterator itr;
#else
	hmap<string,double>::reverse_iterator itr;
#endif
	KifMapFloat* kbase;
	KifIteratorMapFloat(KifCode* klc,KifElement* kp,KifMapFloat* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Float();
			return;
		}
		if (itr==values->Rend())
			return;
		itr->second=ke->Float();
	}


	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->first);
		}

		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providestringraw(itr->first);
	}

	string IteratorKeyString() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return "";

			return it->first;
		}
		if (itr==values->Rend())
			return "";

		return itr->first;
	}

	long IteratorKeyInteger() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return s_XLConvert(STR(it->first));
		}

		if (itr==values->Rend())
			return -1;

		return s_XLConvert(STR(itr->first));
	}

	double IteratorKeyFloat() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return convertfloat(STR(it->first));
		}

		if (itr==values->Rend())
			return -1;

		return convertfloat(STR(itr->first));
	}

	long IteratorValueInteger() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}


	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providefloat(it->second);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providefloat(itr->second);
	}

	void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
		}
		else{
			if (itr==values->Rend())
				return kifTRUE;		
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;
		return kifTRUE;
	}

};

class KifMapIntegerInteger : public KifEvaluate {
public:
	hmap<long,long> values;

	void storekey(long k,double v) {
		KifContainerLock _lock(this);
		values[k]=(long)v;
	}

	void storekey(double k,long v) {
		KifContainerLock _lock(this);
		values[(long)k]=v;
	}

	void storekey(double k,double v) {
		KifContainerLock _lock(this);
		values[(long)k]=(long)v;
	}

	void storekey(long k,long v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		hmap<long,long>::iterator it;
		KifMapIntegerInteger* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapIntegerInteger(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapIntegerInteger(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifMapIntegerInteger) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void Merge(KifElement* ke);

	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}
	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	Exported KifElement* Atom(bool forced=false) {
		if (reference==0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		hmap<long,long>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		hmap<long,long>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
		if (values.find(n)!=values.end())
			return kifcode->Provideinteger(values[n]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}


	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
		if (values.find(n)!=values.end()) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
		if (values.find(n)!=values.end()) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyIInteger(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyIFloat(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyFInteger(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFFloat(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifMapIntegerInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i=s->Integer();
		if ( values.find(i)== values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifMapIntegerInteger(kifcode,NULL);
	}

	string String();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		long k=s_XLConvert(STR(n));
		if ( values.find(k)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple=false);

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);


	void Clear() {
		KifContainerLock _lock(this);
		 values.clear();
	}

	Exported KifElement* Map(KifElement*,KifMap* itself=NULL);	
	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* minus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* multiply(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* divide(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* mod(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* power(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
};

class KifIteratorMapIntegerInteger : public KifIteration {
public:

	hmap<long,long>* values;

	hmap<long,long>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<long,long>::iterator itr;
#else
	hmap<long,long>::reverse_iterator itr;
#endif
    KifMapIntegerInteger* kbase;
	KifIteratorMapIntegerInteger(KifCode* klc,KifElement* kp,KifMapIntegerInteger* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Integer();
			return;
		}
		if (itr==values->Rend())
			return;
		itr->second=ke->Integer();
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->first);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Provideinteger(itr->first);
	}

	long IteratorKeyInteger() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	double IteratorKeyFloat() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	string IteratorKeyString();

	long IteratorValueInteger() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->second);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Provideinteger(itr->second);
	}

	void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
		}
		else{
			if (itr==values->Rend())
				return kifTRUE;		
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifMapIntegerFloat : public KifEvaluate {
public:
	hmap<long,double> values;

	void storekey(long k,long v) {
		KifContainerLock _lock(this);
		values[k]=(double)v;
	}

	void storekey(double k,long v) {
		KifContainerLock _lock(this);
		values[(long)k]=(double)v;
	}

	void storekey(double k,double v) {
		KifContainerLock _lock(this);
		values[(long)k]=v;
	}

	void storekey(long k,double v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		hmap<long,double>::iterator it;
		KifMapIntegerFloat* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapIntegerFloat(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapIntegerFloat(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifMapIntegerFloat) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	void Merge(KifElement* ke);

	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}

	Exported KifElement* Atom(bool forced=false) {
		if (reference==0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		hmap<long,double>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		hmap<long,double>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
		if (values.find(n)!=values.end())
			return kifcode->Providefloat(values[n]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
		if (values.find(n)!=values.end()) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
		if (values.find(n)!=values.end()) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyIInteger(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyIFloat(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyFInteger(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFFloat(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifMapIntegerFloat;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i=s->Integer();
		if ( values.find(i)== values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifMapIntegerFloat(kifcode,NULL);
	}

	string String();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		long k=s_XLConvert(STR(n));
		if ( values.find(k)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple=false);

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);


	void Clear() {
		KifContainerLock _lock(this);
		 values.clear();
	}

	Exported KifElement* Map(KifElement*,KifMap* itself=NULL);	
	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* minus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* multiply(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* divide(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* mod(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* power(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
};

class KifIteratorMapIntegerFloat : public KifIteration {
public:

	hmap<long,double>* values;

	hmap<long,double>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<long,double>::iterator itr;
#else
	hmap<long,double>::reverse_iterator itr;
#endif
    KifMapIntegerFloat* kbase;
	KifIteratorMapIntegerFloat(KifCode* klc,KifElement* kp,KifMapIntegerFloat* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Float();
			return;
		}
		if (itr==values->Rend())
			return;
		itr->second=ke->Float();
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->first);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Provideinteger(itr->first);
	}

	long IteratorKeyInteger() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	double IteratorKeyFloat() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	string IteratorKeyString();

	long IteratorValueInteger() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providefloat(it->second);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providefloat(itr->second);
	}

	void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
		}
		else{
			if (itr==values->Rend())
				return kifTRUE;		
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifMapIntegerString : public KifEvaluate {
public:
	hmap<long,string> values;

	void storekey(long k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		hmap<long,string>::iterator it;
		KifMapIntegerString* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapIntegerString(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapIntegerString(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifMapIntegerString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	void Merge(KifElement* ke);

	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}

	Exported KifElement* Atom(bool forced=false) {
		if (reference==0 && popped && !forced)
			return this;
		return Copy(NULL);
	}

	Exported KifType theType() {
		return kifMapIntegerString;
	}

	KifElement* ValueOnKey(string n) {
		KifContainerLock _lock(this);
		long k=s_XLConvert(STR(n));
		if (values.find(k)!=values.end())
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	string ValueOnKeyString(string n,bool& a) {
		KifContainerLock _lock(this);
		long k=s_XLConvert(STR(n));
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return "";
	}


	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i=s->Integer();
		if ( values.find(i)== values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifMapIntegerString(kifcode,NULL);
	}

	string String();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		long k=s_XLConvert(STR(n));
		if ( values.find(k)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple=false);

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);


	void Clear() {
		KifContainerLock _lock(this);
		 values.clear();
	}

	Exported KifElement* Map(KifElement*,KifMap* itself=NULL);	
	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);

};

class KifIteratorMapIntegerString : public KifIteration {
public:

	hmap<long,string>* values;

	hmap<long,string>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<long,string>::iterator itr;
#else
	hmap<long,string>::reverse_iterator itr;
#endif
    
	KifMapIntegerString* kbase;
	KifIteratorMapIntegerString(KifCode* klc,KifElement* kp,KifMapIntegerString* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->String();
			return;
		}
		if (itr==values->Rend())
			return;
		itr->second=ke->String();
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->first);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Provideinteger(itr->first);
	}

	long IteratorKeyInteger() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	double IteratorKeyFloat() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	string IteratorKeyString();

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return "";
			return it->second;
		}
		if (itr==values->Rend())
			return "";

		return itr->second;
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->second);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providestringraw(itr->second);
	}

	void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
		}
		else{
			if (itr==values->Rend())
				return kifTRUE;		
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;		
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;

		return kifTRUE;
	}

};

class KifMapFloatInteger : public KifEvaluate {
public:
	hmap<double,long> values;

	void storekey(long k,long v) {
		KifContainerLock _lock(this);
		values[(double)k]=v;
	}

	void storekey(long k,double v) {
		KifContainerLock _lock(this);
		values[(double)k]=(long)v;
	}

	void storekey(double k,double v) {
		KifContainerLock _lock(this);
		values[k]=(long)v;
	}

	void storekey(double k,long v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		hmap<double,long>::iterator it;
		KifMapFloatInteger* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapFloatInteger(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapFloatInteger(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifMapFloatInteger) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	void Merge(KifElement* ke);

	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}

	Exported KifElement* Atom(bool forced=false) {
		if (reference==0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		hmap<double,long>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		hmap<double,long>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)!=values.end())
			return kifcode->Provideinteger(values[n]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)!=values.end()) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)!=values.end()) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyIInteger(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyIFloat(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyFInteger(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFFloat(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifMapFloatInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		double i=s->Float();
		if ( values.find(i)== values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifMapFloatInteger(kifcode,NULL);
	}

	string String();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		double k=convertfloat(STR(n));
		if ( values.find(k)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple=false);

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);


	void Clear() {
		KifContainerLock _lock(this);
		 values.clear();
	}

	Exported KifElement* Map(KifElement*,KifMap* itself=NULL);	
	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* minus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* multiply(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* divide(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* mod(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* power(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
};

class KifIteratorMapFloatInteger : public KifIteration {
public:

	hmap<double,long>* values;

	hmap<double,long>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<double,long>::iterator itr;
#else
	hmap<double,long>::reverse_iterator itr;
#endif
    KifMapFloatInteger* kbase;
	KifIteratorMapFloatInteger(KifCode* klc,KifElement* kp,KifMapFloatInteger* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Integer();
			return;
		}
		if (itr==values->Rend())
			return;
		itr->second=ke->Integer();
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providefloat(it->first);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providefloat(itr->first);
	}

	long IteratorKeyInteger() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	double IteratorKeyFloat() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	string IteratorKeyString();

	long IteratorValueInteger() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->second);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Provideinteger(itr->second);
	}

	void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
		}
		else{
			if (itr==values->Rend())
				return kifTRUE;		
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifMapFloatFloat : public KifEvaluate {
public:
	hmap<double,double> values;

	void storekey(long k,long v) {
		KifContainerLock _lock(this);
		values[(double)k]=(double)v;
	}

	void storekey(long k,double v) {
		KifContainerLock _lock(this);
		values[(double)k]=v;
	}

	void storekey(double k,long v) {
		KifContainerLock _lock(this);
		values[k]=(double)v;
	}

	void storekey(double k,double v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		hmap<double,double>::iterator it;
		KifMapFloatFloat* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapFloatFloat(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapFloatFloat(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifMapFloatFloat) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	void Merge(KifElement* ke);

	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}

	Exported KifElement* Atom(bool forced=false) {
		if (reference==0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		hmap<double,double>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		hmap<double,double>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)!=values.end())
			return kifcode->Providefloat(values[n]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)!=values.end()) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)!=values.end()) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyIInteger(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyIFloat(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyFInteger(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFFloat(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifMapFloatFloat;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		double i=s->Float();
		if ( values.find(i)== values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifMapFloatFloat(kifcode,NULL);
	}

	string String();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		double k=convertfloat(STR(n));
		if ( values.find(k)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple=false);

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);


	void Clear() {
		KifContainerLock _lock(this);
		 values.clear();
	}

	Exported KifElement* Map(KifElement*,KifMap* itself=NULL);	
	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* minus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* multiply(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* divide(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* mod(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* power(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
};

class KifIteratorMapFloatFloat : public KifIteration {
public:

	hmap<double,double>* values;

	hmap<double,double>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<double,double>::iterator itr;
#else
	hmap<double,double>::reverse_iterator itr;
#endif
    KifMapFloatFloat* kbase;
	KifIteratorMapFloatFloat(KifCode* klc,KifElement* kp,KifMapFloatFloat* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Float();
			return;
		}
		if (itr==values->Rend())
			return;
		itr->second=ke->Float();
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providefloat(it->first);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providefloat(itr->first);
	}

	long IteratorKeyInteger() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	double IteratorKeyFloat() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	string IteratorKeyString();

	long IteratorValueInteger() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->Rend())
			return 0;

		return itr->second;
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providefloat(it->second);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providefloat(itr->second);
	}

	void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
		}
		else{
			if (itr==values->Rend())
				return kifTRUE;		
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifMapFloatString : public KifEvaluate {
public:
	hmap<double,string> values;

	void storekey(double k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		hmap<double,string>::iterator it;
		KifMapFloatString* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapFloatString(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapFloatString(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifMapFloatString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	void Merge(KifElement* ke);

	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}

	Exported KifElement* Atom(bool forced=false) {
		if (reference==0 && popped && !forced)
			return this;
		return Copy(NULL);
	}

	Exported KifType theType() {
		return kifMapFloatString;
	}

	KifElement* ValueOnKey(string n) {
		KifContainerLock _lock(this);
		double k=convertfloat(STR(n));
		if (values.find(k)!=values.end())
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k)!=values.end())
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	string ValueOnKeyString(string n,bool& a) {
		KifContainerLock _lock(this);
		double k=convertfloat(STR(n));
		if (values.find(k)!=values.end()) {
			a=true;
			return values[k];
		}
		a=false;
		return "";
	}


	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		double i=s->Float();
		if ( values.find(i)== values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifMapFloatString(kifcode,NULL);
	}

	string String();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		double k=convertfloat(STR(n));
		if ( values.find(k)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple=false);

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);


	void Clear() {
		KifContainerLock _lock(this);
		 values.clear();
	}

	Exported KifElement* Map(KifElement*,KifMap* itself=NULL);	
	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself);

};

class KifIteratorMapFloatString : public KifIteration {
public:

	hmap<double,string>* values;

	hmap<double,string>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<double,string>::iterator itr;
#else
	hmap<double,string>::reverse_iterator itr;
#endif
    
	KifMapFloatString* kbase;
	KifIteratorMapFloatString(KifCode* klc,KifElement* kp,KifMapFloatString* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->String();
			return;
		}
		if (itr==values->Rend())
			return;
		itr->second=ke->String();
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providefloat(it->first);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providefloat(itr->first);
	}

	long IteratorKeyInteger() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	double IteratorKeyFloat() {
		if (it==values->end())
			return -1;
		return it->first;
	}

	string IteratorKeyString();

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return "";
			return it->second;
		}
		if (itr==values->Rend())
			return "";

		return itr->second;
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->second);
		}
		if (itr==values->Rend())
			return kifNULL;

		return kifcode->Providestringraw(itr->second);
	}

	void Next() {
		if (reverse==false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return kifTRUE;		
		}
		else{
			if (itr==values->Rend())
				return kifTRUE;		
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			it=values->begin();
			if (it==values->end())
				return kifFALSE;		
			return kifTRUE;
		}
		itr=values->Rbegin();
		if (itr==values->Rend())
			return kifFALSE;

		return kifTRUE;
	}

};

#endif

