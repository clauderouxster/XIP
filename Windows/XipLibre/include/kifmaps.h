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

class KifIntegerMap : public KifEvaluateMap {
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

	Exported KifIntegerMap(KifCode* klc,KifElement* kp) : KifEvaluateMap(klc,kp,kifIntegerMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	void storeInLoop(KifElement** alls);

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
		long n=conversionintegerhexa(STR(k));
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
		return new KifIntegerMap(kifcode, NULL);
	}
	Exported string String();
	Exported string JSonString();
    Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string k) {
        KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
		hmap<long,KifElement*>::iterator iter=values.find(n);
		if (iter==values.end())
			return kifNOELEMENT;
		return iter->second;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);
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

class KifFloatMap : public KifEvaluateMap {
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

	Exported KifFloatMap(KifCode* klc,KifElement* kp) : KifEvaluateMap(klc,kp,kifFloatMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	void storeInLoop(KifElement** alls);

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
		return new KifFloatMap(kifcode, NULL);
	}
	Exported string String();
	Exported string JSonString();
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

	Exported KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);
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

class KifIteratorIntegerMap : public KifIterationContainer {
public:
	hmap<long,KifElement*>* values;
	hmap<long,KifElement*>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<long,KifElement*>::iterator itr;
#else
	hmap<long,KifElement*>::reverse_iterator itr;
#endif


	KifIteratorIntegerMap(KifCode* klc,KifElement* kp,KifIntegerMap* k) : KifIterationContainer(klc,kp,k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
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
		return it->second->String();
	}

	Exported KifElement* IteratorValue() {
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

class KifIteratorFloatMap : public KifIterationContainer {
public:
	hmap<double,KifElement*>* values;
	hmap<double,KifElement*>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<double,KifElement*>::iterator itr;
#else
	hmap<double,KifElement*>::reverse_iterator itr;
#endif


	KifIteratorFloatMap(KifCode* klc,KifElement* kp,KifFloatMap* k) : KifIterationContainer(klc,kp,k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
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
		return it->second->String();
	}

	Exported KifElement* IteratorValue() {
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

class KifMapStringString : public KifEvaluateMapValue {
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
		KifMapStringString* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapStringString(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapStringString(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifMapStringString) {}

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
		return kifMapStringString;
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
	void storeInLoop(KifElement** alls);
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

	KifElement* Newinstance() {
		return new KifMapStringString(kifcode,NULL);
	}

	string String();
	string JSonString();
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);

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

class KifIteratorMapStringString : public KifIterationContainer {
public:

	hmap<string,string>* values;

	hmap<string,string>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<string,string>::iterator itr;
#else
	hmap<string,string>::reverse_iterator itr;
#endif
    
	KifIteratorMapStringString(KifCode* klc,KifElement* kp,KifMapStringString* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
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
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return conversionintegerhexa(STR(it->first));
		}

		if (itr==values->Rend())
			return -1;

		return conversionintegerhexa(STR(itr->first));
	}

	double IteratorKeyFloat() {
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

class KifMapStringInteger : public KifEvaluateMapValue {
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
		KifMapStringInteger* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapStringInteger(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapStringInteger(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifMapStringInteger) {}

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
	void storeInLoop(KifElement** alls);
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
			v*=itx->second;
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
		return kifMapStringInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		string i=s->String();
		if ( values.find(i)== values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Newinstance() {
		return new KifMapStringInteger(kifcode,NULL);
	}

	string String();
	string JSonString();
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);

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

class KifIteratorMapStringInteger : public KifIterationContainer {
public:

	hmap<string,long>* values;

	hmap<string,long>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<string,long>::iterator itr;
#else
	hmap<string,long>::reverse_iterator itr;
#endif

	KifIteratorMapStringInteger(KifCode* klc,KifElement* kp,KifMapStringInteger* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
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
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return conversionintegerhexa(STR(it->first));
		}

		if (itr==values->Rend())
			return -1;

		return conversionintegerhexa(STR(itr->first));
	}

	double IteratorKeyFloat() {
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
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
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

class KifMapStringFloat : public KifEvaluateMapValue {
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
		KifMapStringFloat* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifMapStringFloat(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifMapStringFloat(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifMapStringFloat) {}

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
	void storeInLoop(KifElement** alls);
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
		return kifMapStringFloat;
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
			v*=itx->second;
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

	KifElement* Newinstance() {
		return new KifMapStringFloat(kifcode,NULL);
	}

	string String();
	string JSonString();
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);

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

class KifIteratorMapStringFloat : public KifIterationContainer {
public:

	hmap<string,double>* values;

	hmap<string,double>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<string,double>::iterator itr;
#else
	hmap<string,double>::reverse_iterator itr;
#endif

	KifIteratorMapStringFloat(KifCode* klc,KifElement* kp,KifMapStringFloat* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
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
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return conversionintegerhexa(STR(it->first));
		}

		if (itr==values->Rend())
			return -1;

		return conversionintegerhexa(STR(itr->first));
	}

	double IteratorKeyFloat() {
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
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
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

class KifMapIntegerInteger : public KifEvaluateMapValue {
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

	KifMapIntegerInteger(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifMapIntegerInteger) {}

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
	void storeInLoop(KifElement** alls);
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
			v*=itx->second;
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
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
		long n=conversionintegerhexa(STR(k));
		if (values.find(n)!=values.end()) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
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
		long k=conversionintegerhexa(STR(n));
		if ( values.find(k)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);

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

class KifIteratorMapIntegerInteger : public KifIterationContainer {
public:

	hmap<long,long>* values;

	hmap<long,long>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<long,long>::iterator itr;
#else
	hmap<long,long>::reverse_iterator itr;
#endif

	KifIteratorMapIntegerInteger(KifCode* klc,KifElement* kp,KifMapIntegerInteger* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
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
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
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

class KifMapIntegerFloat : public KifEvaluateMapValue {
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

	KifMapIntegerFloat(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifMapIntegerFloat) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(KifElement** alls);
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
			v*=itx->second;
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
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
		long n=conversionintegerhexa(STR(k));
		if (values.find(n)!=values.end()) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
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
		long k=conversionintegerhexa(STR(n));
		if ( values.find(k)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);

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

class KifIteratorMapIntegerFloat : public KifIterationContainer {
public:

	hmap<long,double>* values;

	hmap<long,double>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<long,double>::iterator itr;
#else
	hmap<long,double>::reverse_iterator itr;
#endif

	KifIteratorMapIntegerFloat(KifCode* klc,KifElement* kp,KifMapIntegerFloat* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
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
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
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

class KifMapIntegerString : public KifEvaluateMapValue {
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

	KifMapIntegerString(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifMapIntegerString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(KifElement** alls);
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
		long k=conversionintegerhexa(STR(n));
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
		long k=conversionintegerhexa(STR(n));
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

	KifElement* Newinstance() {
		return new KifMapIntegerString(kifcode,NULL);
	}

	string String();
	string JSonString();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);
	Exported void Push(KifElement* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		long k=conversionintegerhexa(STR(n));
		if ( values.find(k)== values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);

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

class KifIteratorMapIntegerString : public KifIterationContainer {
public:

	hmap<long,string>* values;

	hmap<long,string>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<long,string>::iterator itr;
#else
	hmap<long,string>::reverse_iterator itr;
#endif
    

	KifIteratorMapIntegerString(KifCode* klc,KifElement* kp,KifMapIntegerString* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
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

class KifMapFloatInteger : public KifEvaluateMapValue {
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

	KifMapFloatInteger(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifMapFloatInteger) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(KifElement** alls);
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
			v*=itx->second;
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);

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

class KifIteratorMapFloatInteger : public KifIterationContainer {
public:

	hmap<double,long>* values;

	hmap<double,long>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<double,long>::iterator itr;
#else
	hmap<double,long>::reverse_iterator itr;
#endif

	KifIteratorMapFloatInteger(KifCode* klc,KifElement* kp,KifMapFloatInteger* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
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
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
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

class KifMapFloatFloat : public KifEvaluateMapValue {
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

	KifMapFloatFloat(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifMapFloatFloat) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(KifElement** alls);
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
			v*=itx->second;
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);

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

class KifIteratorMapFloatFloat : public KifIterationContainer {
public:

	hmap<double,double>* values;

	hmap<double,double>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<double,double>::iterator itr;
#else
	hmap<double,double>::reverse_iterator itr;
#endif

	KifIteratorMapFloatFloat(KifCode* klc,KifElement* kp,KifMapFloatFloat* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
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
		stringstream os;
		os<<it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
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

class KifMapFloatString : public KifEvaluateMapValue {
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

	KifMapFloatString(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifMapFloatString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(KifElement** alls);
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

	KifElement* Newinstance() {
		return new KifMapFloatString(kifcode,NULL);
	}

	string String();
	string JSonString();
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int,KifCallFunction*);

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

class KifIteratorMapFloatString : public KifIterationContainer {
public:

	hmap<double,string>* values;

	hmap<double,string>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<double,string>::iterator itr;
#else
	hmap<double,string>::reverse_iterator itr;
#endif
    

	KifIteratorMapFloatString(KifCode* klc,KifElement* kp,KifMapFloatString* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
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
//------------------------------------------------------------------
class KifUStringMap : public KifEvaluateMap {
public:

	hmap<wstring, KifElement*> values;

	void Items(KifVector* kvect) {
		hmap<wstring, KifElement*>::iterator it;
		KifUStringMap* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifUStringMap(kifcode, NULL);
			kval->push(it->first, it->second);
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	Exported KifUStringMap(KifCode* klc, KifElement* kp) : KifEvaluateMap(klc, kp, kifUStringMap) {}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);

	void storeInLoop(KifElement** alls);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL) {
		return this;
	}

	bool aMapContainer() {
		return true;
	}

	int Garbager() {
		return kifUStringMap;
	}


	KifElement* ValueOnKey(wstring k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return values[buffer];
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return values[buffer];
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* ke) {
		KifContainerLock _lock(this);
		wstring s = ke->UString();
		if (values.find(s) == values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newinstance() {
		return new KifUStringMap(kifcode, NULL);
	}

	Exported string String();
	Exported wstring UString();
	Exported string JSonString();
	Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(wstring n) {
		KifContainerLock _lock(this);
		hmap<wstring, KifElement*>::iterator iter = values.find(n);
		if (iter == values.end())
			return kifNOELEMENT;
		return iter->second;
	}

	KifElement* Values(string n) {
		wstring wn;
		s_utf8_to_unicode(wn, STR(n));
		return Values(wn);
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);
	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		hmap<wstring, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second->Sum();
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		hmap<wstring, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v *= itx->second->Product();
		return v;
	}

	Exported void Setreference(int inc = 1);

	Exported void Setpopped(int v = 1, bool force = false) {
		if (reference <= 0 || force) {
			Putpopped(v);
			hmap<wstring, KifElement*>::iterator itx;
			for (itx = values.begin(); itx != values.end(); itx++) {
				if (itx->second != NULL)
					itx->second->Setpopped(v, force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		hmap<wstring, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++) {
			if (itx->second != NULL)
				itx->second->Clean();
		}
		values.clear();
		if (reference <= 0)
			KifElement::Resetreference();
	}

	Exported void Resetreference(int inc = 1);
	Exported void Clear();


	Exported long Size() {
		KifContainerLock _lock(this);
		return values.size();
	}

	Exported KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);

	Exported long Integer() {
		KifContainerLock _lock(this);
		return values.size();
	}
	Exported double Float() {
		KifContainerLock _lock(this);
		return values.size();
	}

	//To insert a value into a map through the API 


	Exported void push(wstring key, KifElement* val);
	Exported void Push(wstring& key, KifElement* val);

	Exported void push(string key, KifElement* val);
	Exported void Push(string& key, KifElement* val);
	Exported void Push(char* key, KifElement* val);

	Exported void Push(KifElement* key, KifElement* val);


	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);

};

class KifIteratorUStringMap : public KifIterationContainer {
public:
	hmap<wstring, KifElement*>* values;
	hmap<wstring, KifElement*>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<wstring, KifElement*>::iterator itr;
#else
	hmap<wstring, KifElement*>::reverse_iterator itr;
#endif

	KifIteratorUStringMap(KifCode* klc, KifElement* kp, KifUStringMap* k) : KifIterationContainer(klc, kp, k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return kifcode->Provideustring(it->first);
		}
		if (itr == values->Rend())
			return kifNOELEMENT;
		return kifcode->Provideustring(it->first);
	}

	long IteratorKeyInteger() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->first);
			sc_unicode_to_utf8(x, wx);
			return conversionintegerhexa(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->first);
		sc_unicode_to_utf8(x, wx);
		return conversionintegerhexa(STR(x));
	}

	double IteratorKeyFloat() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->first);
			sc_unicode_to_utf8(x, wx);
			return convertfloat(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->first);
		sc_unicode_to_utf8(x, wx);
		return convertfloat(STR(x));
	}

	wstring IteratorKeyUString() {
		if (reverse == false) {
			if (it == values->end())
				return L"";
			return it->first;
		}
		if (itr == values->Rend())
			return L"";
		return itr->first;
	}

	string IteratorKeyString() {
		string s;
		if (reverse == false) {
			if (it == values->end())
				return "";
			wstring ws(it->first);
			s_unicode_to_utf8(s, ws);
			return s;
		}
		if (itr == values->Rend())
			return "";
		wstring ws(it->first);
		s_unicode_to_utf8(s, ws);
		return s;
	}

	Exported KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNOELEMENT;
			return it->second;
		}
		if (itr == values->Rend())
			return kifNOELEMENT;
		return itr->second;
	}

	Exported void Next() {
		if (reverse == false)
			it++;
		else
			itr++;
	}

	Exported KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
			return kifFALSE;
		}
		if (itr == values->Rend())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		if (reverse == false) {
			it = values->begin();
			if (it == values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr = values->Rbegin();
		if (itr == values->Rend())
			return kifFALSE;
		return kifTRUE;
	}
};

//--------------------------------------------------------------------------------------------
class KifMapUStringUString : public KifEvaluateMapValue {
public:
	hmap<wstring, wstring> values;

	void storekey(wstring k, wstring v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void storekey(string sk, wstring v) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));

		KifContainerLock _lock(this);
		values[k] = v;
	}

	void storekey(char* sk, wstring v) {
		wstring k;
		s_utf8_to_unicode(k, sk);

		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		hmap<wstring, wstring>::iterator it;
		KifMapUStringUString* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifMapUStringUString(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifMapUStringUString(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifMapUStringUString) {}

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

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	Exported KifType theType() {
		return kifMapUStringUString;
	}

	KifElement* ValueOnKey(wstring k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnKey(string sk) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return kifcode->Provideustring(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return kifcode->Provideustring(values[buffer]);
		return kifNOELEMENT;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);

	
	wstring ValueOnKeyUString(wstring k, bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return L"";
	}

	string ValueOnKeyString(string sk, bool& a) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));

		KifContainerLock _lock(this);
		if (values.find(k) != values.end()) {
			a = true;
			sc_unicode_to_utf8(sk, values[k]);
			return sk;
		}
		a = false;
		return "";
	}


	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		wstring i = s->UString();
		if (values.find(i) == values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifMapUStringUString(kifcode, NULL);
	}

	wstring UString();
	string String();
	string JSonString();

	Exported void Push(wstring& key, KifElement* val);
	Exported void Push(string& key, KifElement* val);
	Exported void Push(char* key, KifElement* val);

	Exported void Push(KifElement* key, KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string sn) {
		wstring n;
		s_utf8_to_unicode(n, STR(sn));

		KifContainerLock _lock(this);
		if (values.find(n) == values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}

	KifElement* Values(wstring n) {
		KifContainerLock _lock(this);
		if (values.find(n) == values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);

};

class KifIteratorMapUStringUString : public KifIterationContainer {
public:

	hmap<wstring, wstring>* values;

	hmap<wstring, wstring>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<wstring, wstring>::iterator itr;
#else
	hmap<wstring, wstring>::reverse_iterator itr;
#endif

	KifIteratorMapUStringUString(KifCode* klc, KifElement* kp, KifMapUStringUString* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (it == values->end())
				return;
			it->second = ke->UString();
			return;
		}
		if (itr == values->Rend())
			return;
		itr->second = ke->UString();
	}

	KifElement* IteratorKey() {
		if (reverse == false) {
			if (it == values->end())
				return kifNULL;

			return kifcode->Provideustring(it->first);
		}

		if (itr == values->Rend())
			return kifNULL;

		return kifcode->Provideustring(itr->first);
	}

	long IteratorKeyInteger() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->first);
			sc_unicode_to_utf8(x, wx);
			return conversionintegerhexa(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->first);
		sc_unicode_to_utf8(x, wx);
		return conversionintegerhexa(STR(x));
	}

	double IteratorKeyFloat() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->first);
			sc_unicode_to_utf8(x, wx);
			return convertfloat(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->first);
		sc_unicode_to_utf8(x, wx);
		return convertfloat(STR(x));
	}


	wstring IteratorKeyUString() {
		if (reverse == false) {
			if (it == values->end())
				return L"";

			return it->first;
		}
		if (itr == values->Rend())
			return L"";

		return itr->first;
	}

	string IteratorKeyString() {
		wstring ws;
		string s;
		if (reverse == false) {
			if (it == values->end())
				return "";
			ws = it->first;
			s_unicode_to_utf8(s, ws);
			return s;
		}
		if (itr == values->Rend())
			return "";

		ws = it->first;
		s_unicode_to_utf8(s, ws);
		return s;
	}

	long IteratorValueInteger() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->second);
			sc_unicode_to_utf8(x, wx);
			return conversionintegerhexa(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->second);
		sc_unicode_to_utf8(x, wx);
		return conversionintegerhexa(STR(x));
	}

	double IteratorValueFloat() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->second);
			sc_unicode_to_utf8(x, wx);
			return convertfloat(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->second);
		sc_unicode_to_utf8(x, wx);
		return convertfloat(STR(x));
	}

	string IteratorValueString() {
		wstring ws;
		string s;
		if (reverse == false) {
			if (it == values->end())
				return "";
			ws = it->second;
			s_unicode_to_utf8(s, ws);
			return s;
		}
		if (itr == values->Rend())
			return "";

		ws = it->second;
		s_unicode_to_utf8(s, ws);
		return s;
	}

	wstring IteratorValueUString() {
		if (reverse == false) {
			if (it == values->end())
				return L"";
			return it->second;
		}
		if (itr == values->Rend())
			return L"";

		return itr->second;
	}
	
	KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNULL;

			return kifcode->Provideustring(it->second);
		}
		if (itr == values->Rend())
			return kifNULL;

		return kifcode->Provideustring(itr->second);
	}

	void Next() {
		if (reverse == false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
		}
		else{
			if (itr == values->Rend())
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		if (reverse == false) {
			it = values->begin();
			if (it == values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr = values->Rbegin();
		if (itr == values->Rend())
			return kifFALSE;

		return kifTRUE;
	}

};
//--------------------------------------------------------------------------------------------
class KifMapUStringInteger : public KifEvaluateMapValue {
public:
	hmap<wstring, long> values;

	void storekey(wstring k, long v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void storekey(string sk, long v) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));

		KifContainerLock _lock(this);
		values[k] = v;
	}

	void storekey(char* sk, long v) {
		wstring k;
		s_utf8_to_unicode(k, sk);

		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		hmap<wstring, long>::iterator it;
		KifMapUStringInteger* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifMapUStringInteger(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifMapUStringInteger(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifMapUStringInteger) {}

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

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}
	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		hmap<wstring, long>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second;
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		hmap<wstring, long>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v *= itx->second;
		return v;
	}

	KifElement* ValueOnKey(wstring k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnKey(string sk) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return kifcode->Provideinteger(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return kifcode->Provideinteger(values[buffer]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(wstring k, bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFloat(wstring k, bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyInteger(string sk, bool& a) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));

		KifContainerLock _lock(this);
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFloat(string sk, bool& a) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));

		KifContainerLock _lock(this);
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyIInteger(long k, bool& a) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end()) {
			a = true;
			return values[buffer];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyIFloat(long k, bool& a) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end()) {
			a = true;
			return values[buffer];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyFInteger(double k, bool& a) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end()) {
			a = true;
			return values[buffer];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFFloat(double k, bool& a) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end()) {
			a = true;
			return values[buffer];
		}
		a = false;
		return 0;
	}

	Exported KifType theType() {
		return kifMapUStringInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		wstring i = s->UString();
		if (values.find(i) == values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifMapUStringInteger(kifcode, NULL);
	}

	wstring UString();
	string String();
	string JSonString();
	
	Exported void Push(wstring& key, KifElement* val);
	Exported void Push(string& key, KifElement* val);
	Exported void Push(char* key, KifElement* val);
	Exported void Push(KifElement* key, KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);

	KifElement* Values(wstring n) {
		KifContainerLock _lock(this);
		if (values.find(n) == values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}

	KifElement* Values(string sn) {
		wstring n;
		s_utf8_to_unicode(n, STR(sn));
		KifContainerLock _lock(this);
		if (values.find(n) == values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
};

class KifIteratorMapUStringInteger : public KifIterationContainer {
public:

	hmap<wstring, long>* values;

	hmap<wstring, long>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<wstring, long>::iterator itr;
#else
	hmap<wstring, long>::reverse_iterator itr;
#endif

	KifIteratorMapUStringInteger(KifCode* klc, KifElement* kp, KifMapUStringInteger* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (it == values->end())
				return;
			it->second = ke->Integer();
			return;
		}
		if (itr == values->Rend())
			return;
		itr->second = ke->Integer();
	}

	KifElement* IteratorKey() {
		if (reverse == false) {
			if (it == values->end())
				return kifNULL;

			return kifcode->Provideustring(it->first);
		}
		if (itr == values->Rend())
			return kifNULL;

		return kifcode->Provideustring(itr->first);
	}

	long IteratorKeyInteger() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->first);
			sc_unicode_to_utf8(x, wx);
			return conversionintegerhexa(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->first);
		sc_unicode_to_utf8(x, wx);
		return conversionintegerhexa(STR(x));
	}

	double IteratorKeyFloat() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->first);
			sc_unicode_to_utf8(x, wx);
			return convertfloat(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->first);
		sc_unicode_to_utf8(x, wx);
		return convertfloat(STR(x));
	}

	wstring IteratorKeyUString() {
		if (reverse == false) {
			if (it == values->end())
				return L"";

			return it->first;
		}
		if (itr == values->Rend())
			return L"";

		return itr->first;
	}

	string IteratorKeyString() {
		wstring ws;
		string s;
		if (reverse == false) {
			if (it == values->end())
				return "";
			ws = it->first;
			s_unicode_to_utf8(s, ws);
			return s;
		}
		if (itr == values->Rend())
			return "";

		ws = it->first;
		s_unicode_to_utf8(s, ws);
		return s;
	}

	long IteratorValueInteger() {
		if (reverse == false) {
			if (it == values->end())
				return 0;
			return it->second;
		}
		if (itr == values->Rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		if (reverse == false) {
			if (it == values->end())
				return 0;
			return it->second;
		}
		if (itr == values->Rend())
			return 0;

		return itr->second;
	}

	wstring IteratorValueUString() {
		std::wstringstream os;
		os << it->second;
		return os.str();
	}

	string IteratorValueString() {
		stringstream os;
		os << it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->second);
		}
		if (itr == values->Rend())
			return kifNULL;

		return kifcode->Provideinteger(itr->second);
	}

	void Next() {
		if (reverse == false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
		}
		else{
			if (itr == values->Rend())
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		if (reverse == false) {
			it = values->begin();
			if (it == values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr = values->Rbegin();
		if (itr == values->Rend())
			return kifFALSE;
		return kifTRUE;
	}
};
//-----------------------------------------------------------------------------------
class KifMapUStringFloat : public KifEvaluateMapValue {
public:
	hmap<wstring, double> values;

	void storekey(wstring k, double v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void storekey(string sk, double v) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));

		KifContainerLock _lock(this);
		values[k] = v;
	}

	void storekey(char* sk, double v) {
		wstring k;
		s_utf8_to_unicode(k, sk);

		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		hmap<wstring, double>::iterator it;
		KifMapUStringFloat* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifMapUStringFloat(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifMapUStringFloat(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifMapUStringFloat) {}

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
	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);

	KifElement* ValueOnKey(wstring k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnKey(string sk) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}
	
	KifElement* ValueOnIntegerKey(long k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return kifcode->Providefloat(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end())
			return kifcode->Providefloat(values[buffer]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(wstring k, bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFloat(wstring k, bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyInteger(string sk, bool& a) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));

		KifContainerLock _lock(this);
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFloat(string sk, bool& a) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));

		KifContainerLock _lock(this);
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	long ValueOnKeyIInteger(long k, bool& a) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end()) {
			a = true;
			return values[buffer];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyIFloat(long k, bool& a) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end()) {
			a = true;
			return values[buffer];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyFInteger(double k, bool& a) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end()) {
			a = true;
			return values[buffer];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFFloat(double k, bool& a) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer) != values.end()) {
			a = true;
			return values[buffer];
		}
		a = false;
		return 0;
	}

	Exported KifType theType() {
		return kifMapUStringFloat;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		hmap<wstring, double>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second;
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		hmap<wstring, double>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v *= itx->second;
		return v;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		wstring i = s->UString();
		if (values.find(i) == values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifMapUStringFloat(kifcode, NULL);
	}

	wstring UString();
	string String();
	string JSonString();
	Exported void Push(string& key, KifElement* val);
	Exported void Push(char* key, KifElement* val);
	Exported void Push(KifElement* key, KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(wstring n) {
		KifContainerLock _lock(this);
		if (values.find(n) == values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}

	KifElement* Values(string sn) {
		wstring n;
		s_utf8_to_unicode(n, STR(sn));
		KifContainerLock _lock(this);
		if (values.find(n) == values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* divide(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* mod(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftright(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* shiftleft(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* power(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
};

class KifIteratorMapUStringFloat : public KifIterationContainer {
public:

	hmap<wstring, double>* values;

	hmap<wstring, double>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<wstring, double>::iterator itr;
#else
	hmap<wstring, double>::reverse_iterator itr;
#endif

	KifIteratorMapUStringFloat(KifCode* klc, KifElement* kp, KifMapUStringFloat* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (it == values->end())
				return;
			it->second = ke->Float();
			return;
		}
		if (itr == values->Rend())
			return;
		itr->second = ke->Float();
	}


	KifElement* IteratorKey() {
		if (reverse == false) {
			if (it == values->end())
				return kifNULL;

			return kifcode->Provideustring(it->first);
		}
		if (itr == values->Rend())
			return kifNULL;

		return kifcode->Provideustring(itr->first);
	}

	long IteratorKeyInteger() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->first);
			sc_unicode_to_utf8(x, wx);
			return conversionintegerhexa(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->first);
		sc_unicode_to_utf8(x, wx);
		return conversionintegerhexa(STR(x));
	}

	double IteratorKeyFloat() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->first);
			sc_unicode_to_utf8(x, wx);
			return convertfloat(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->first);
		sc_unicode_to_utf8(x, wx);
		return convertfloat(STR(x));
	}

	wstring IteratorKeyUString() {
		if (reverse == false) {
			if (it == values->end())
				return L"";

			return it->first;
		}
		if (itr == values->Rend())
			return L"";

		return itr->first;
	}

	string IteratorKeyString() {
		wstring ws;
		string s;
		if (reverse == false) {
			if (it == values->end())
				return "";
			ws = it->first;
			s_unicode_to_utf8(s, ws);
			return s;
		}
		if (itr == values->Rend())
			return "";

		ws = it->first;
		s_unicode_to_utf8(s, ws);
		return s;
	}

	long IteratorValueInteger() {
		if (reverse == false) {
			if (it == values->end())
				return 0;
			return it->second;
		}
		if (itr == values->Rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		if (reverse == false) {
			if (it == values->end())
				return 0;
			return it->second;
		}
		if (itr == values->Rend())
			return 0;

		return itr->second;
	}

	wstring IteratorValueUString() {
		std::wstringstream os;
		os << it->second;
		return os.str();
	}


	string IteratorValueString() {
		stringstream os;
		os << it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNULL;

			return kifcode->Providefloat(it->second);
		}
		if (itr == values->Rend())
			return kifNULL;

		return kifcode->Providefloat(itr->second);
	}

	void Next() {
		if (reverse == false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
		}
		else{
			if (itr == values->Rend())
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		if (reverse == false) {
			it = values->begin();
			if (it == values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr = values->Rbegin();
		if (itr == values->Rend())
			return kifFALSE;
		return kifTRUE;
	}

};
//-----------------------------------------------------------------------------------
class KifMapIntegerUString : public KifEvaluateMapValue {
public:
	hmap<long, wstring> values;

	void storekey(long k, wstring v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		hmap<long, wstring>::iterator it;
		KifMapIntegerUString* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifMapIntegerUString(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifMapIntegerUString(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifMapIntegerUString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);

	void Merge(KifElement* ke);

	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;
		return Copy(NULL);
	}

	Exported KifType theType() {
		return kifMapIntegerUString;
	}
	
	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnKey(string n) {
		KifContainerLock _lock(this);
		long k = conversionintegerhexa(STR(n));
		if (values.find(k) != values.end())
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	wstring ValueOnKeyUString(wstring ik, bool& a) {
		KifContainerLock _lock(this);
		string n;
		s_unicode_to_utf8(n, ik);
		long k = conversionintegerhexa(STR(n));
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return L"";
	}

	string ValueOnKeyString(string n, bool& a) {
		KifContainerLock _lock(this);
		long k = conversionintegerhexa(STR(n));
		if (values.find(k) != values.end()) {
			a = true;
			sc_unicode_to_utf8(n, values[k]);
			return n;
		}
		a = false;
		return "";
	}


	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i = s->Integer();
		if (values.find(i) == values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifMapIntegerUString(kifcode, NULL);
	}

	wstring UString();
	string String();
	string JSonString();
	Exported void Push(string& key, KifElement* val);
	Exported void Push(char* key, KifElement* val);
	Exported void Push(KifElement* key, KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		long k = conversionintegerhexa(STR(n));
		if (values.find(k) == values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}

	KifElement* Values(wstring sn) {
		string n;
		s_unicode_to_utf8(n, sn);
		long k = conversionintegerhexa(STR(n));
		KifContainerLock _lock(this);
		if (values.find(k) == values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}

	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);

};

class KifIteratorMapIntegerUString : public KifIterationContainer {
public:

	hmap<long, wstring>* values;

	hmap<long, wstring>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<long, wstring>::iterator itr;
#else
	hmap<long, wstring>::reverse_iterator itr;
#endif


	KifIteratorMapIntegerUString(KifCode* klc, KifElement* kp, KifMapIntegerUString* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (it == values->end())
				return;
			it->second = ke->UString();
			return;
		}
		if (itr == values->Rend())
			return;
		itr->second = ke->UString();
	}

	KifElement* IteratorKey() {
		if (reverse == false) {
			if (it == values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->first);
		}
		if (itr == values->Rend())
			return kifNULL;

		return kifcode->Provideinteger(itr->first);
	}

	long IteratorKeyInteger() {
		if (it == values->end())
			return -1;
		return it->first;
	}

	double IteratorKeyFloat() {
		if (it == values->end())
			return -1;
		return it->first;
	}

	string IteratorKeyString();

	long IteratorValueInteger() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->second);
			sc_unicode_to_utf8(x, wx);
			return conversionintegerhexa(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->second);
		sc_unicode_to_utf8(x, wx);
		return conversionintegerhexa(STR(x));
	}

	double IteratorValueFloat() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->second);
			sc_unicode_to_utf8(x, wx);
			return convertfloat(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->second);
		sc_unicode_to_utf8(x, wx);
		return convertfloat(STR(x));
	}

	string IteratorValueString() {
		wstring ws;
		string s;
		if (reverse == false) {
			if (it == values->end())
				return "";
			ws = it->second;
			s_unicode_to_utf8(s, ws);
			return s;
		}
		if (itr == values->Rend())
			return "";

		ws = it->second;
		s_unicode_to_utf8(s, ws);
		return s;
	}

	wstring IteratorValueUString() {
		if (reverse == false) {
			if (it == values->end())
				return L"";
			return it->second;
		}
		if (itr == values->Rend())
			return L"";

		return itr->second;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNULL;

			return kifcode->Provideustring(it->second);
		}
		if (itr == values->Rend())
			return kifNULL;

		return kifcode->Provideustring(itr->second);
	}

	void Next() {
		if (reverse == false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
		}
		else{
			if (itr == values->Rend())
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		if (reverse == false) {
			it = values->begin();
			if (it == values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr = values->Rbegin();
		if (itr == values->Rend())
			return kifFALSE;

		return kifTRUE;
	}

};
//-----------------------------------------------------------------------------------
class KifMapFloatUString : public KifEvaluateMapValue {
public:
	hmap<double, wstring> values;

	void storekey(double k, wstring v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		hmap<double, wstring>::iterator it;
		KifMapFloatUString* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifMapFloatUString(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifMapFloatUString(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifMapFloatUString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}
	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);

	void Merge(KifElement* ke);

	bool aValueContainer() {
		return true;
	}

	bool aMapContainer() {
		return true;
	}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;
		return Copy(NULL);
	}

	Exported KifType theType() {
		return kifMapFloatUString;
	}


	wstring ValueOnKeyUString(wstring ik, bool& a) {
		KifContainerLock _lock(this);
		string n;
		s_unicode_to_utf8(n, ik);
		double k = convertfloat(STR(n));
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
		}
		a = false;
		return L"";
	}

	string ValueOnKeyString(string n, bool& a) {
		KifContainerLock _lock(this);
		double k = convertfloat(STR(n));
		if (values.find(k) != values.end()) {
			a = true;
			sc_unicode_to_utf8(n, values[k]);
			return n;
		}
		a = false;
		return "";
	}

	KifElement* ValueOnKey(string n) {
		KifContainerLock _lock(this);
		double k = convertfloat(STR(n));
		if (values.find(k) != values.end())
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		double i = s->Float();
		if (values.find(i) == values.end())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifMapFloatUString(kifcode, NULL);
	}

	wstring UString();
	string String();
	string JSonString();
	Exported void Push(string& key, KifElement* val);
	Exported void Push(char* key, KifElement* val);
	Exported void Push(KifElement* key, KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		double k = convertfloat(STR(n));
		if (values.find(k) == values.end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);


	void Clear() {
		KifContainerLock _lock(this);
		values.clear();
	}

	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);
	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);

	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* pattern, bool idx);

	long Integer() {
		KifContainerLock _lock(this);
		return  values.size();
	}
	double Float() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	Exported KifElement* same(KifElement* a);
	Exported KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* xorset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	Exported KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);

};

class KifIteratorMapFloatUString : public KifIterationContainer {
public:

	hmap<double, wstring>* values;

	hmap<double, wstring>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	hmap<double, wstring>::iterator itr;
#else
	hmap<double, wstring>::reverse_iterator itr;
#endif


	KifIteratorMapFloatUString(KifCode* klc, KifElement* kp, KifMapFloatUString* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse == false) {
			if (it == values->end())
				return;
			it->second = ke->UString();
			return;
		}
		if (itr == values->Rend())
			return;
		itr->second = ke->UString();
	}

	KifElement* IteratorKey() {
		if (reverse == false) {
			if (it == values->end())
				return kifNULL;

			return kifcode->Providefloat(it->first);
		}
		if (itr == values->Rend())
			return kifNULL;

		return kifcode->Providefloat(itr->first);
	}

	long IteratorKeyInteger() {
		if (it == values->end())
			return -1;
		return it->first;
	}

	double IteratorKeyFloat() {
		if (it == values->end())
			return -1;
		return it->first;
	}

	string IteratorKeyString();

	long IteratorValueInteger() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->second);
			sc_unicode_to_utf8(x, wx);
			return conversionintegerhexa(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->second);
		sc_unicode_to_utf8(x, wx);
		return conversionintegerhexa(STR(x));
	}

	double IteratorValueFloat() {
		string x;
		if (reverse == false) {
			if (it == values->end())
				return -1;
			wstring wx(it->second);
			sc_unicode_to_utf8(x, wx);
			return convertfloat(STR(x));
		}

		if (itr == values->Rend())
			return -1;

		wstring wx(it->second);
		sc_unicode_to_utf8(x, wx);
		return convertfloat(STR(x));
	}

	string IteratorValueString() {
		wstring ws;
		string s;
		if (reverse == false) {
			if (it == values->end())
				return "";
			ws = it->second;
			s_unicode_to_utf8(s, ws);
			return s;
		}
		if (itr == values->Rend())
			return "";

		ws = it->second;
		s_unicode_to_utf8(s, ws);
		return s;
	}

	wstring IteratorValueUString() {
		if (reverse == false) {
			if (it == values->end())
				return L"";
			return it->second;
		}
		if (itr == values->Rend())
			return L"";

		return itr->second;
	}

	KifElement* IteratorValue() {
		if (reverse == false) {
			if (it == values->end())
				return kifNULL;

			return kifcode->Provideustring(it->second);
		}
		if (itr == values->Rend())
			return kifNULL;

		return kifcode->Provideustring(itr->second);
	}

	void Next() {
		if (reverse == false)
			it++;
		else
			itr++;
	}

	KifElement* End() {
		if (reverse == false) {
			if (it == values->end())
				return kifTRUE;
		}
		else{
			if (itr == values->Rend())
				return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* Begin() {
		if (reverse == false) {
			it = values->begin();
			if (it == values->end())
				return kifFALSE;
			return kifTRUE;
		}
		itr = values->Rbegin();
		if (itr == values->Rend())
			return kifFALSE;

		return kifTRUE;
	}

};

//---------------------------------------------------------------------------------------------------------

#endif


