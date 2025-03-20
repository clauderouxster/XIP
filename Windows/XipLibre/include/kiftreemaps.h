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
   filename   : kiftreemaps.h
   Date       : 09/09/2010
   Purpose    : Implementation of the KiF language
   Programmer : Claude ROUX
   Reviewer   : 
*/


#ifndef i_kiftreemaps
#define i_kiftreemaps

class KifIntegerTreeMap : public KifEvaluateMap {
public:

	map<long,KifElement*> values;	

	void Items(KifVector* kvect) {
		map<long,KifElement*>::iterator it;
		KifIntegerTreeMap* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifIntegerTreeMap(kifcode,NULL);
			kval->push(it->first,it->second);
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	Exported KifIntegerTreeMap(KifCode* klc,KifElement* kp) : KifEvaluateMap(klc,kp,kifIntegerTreeMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);
	Exported KifElement* Map(KifElement*,KifMap* itself=NULL) {
		return this;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

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
		return new KifIntegerTreeMap(kifcode, NULL);
	}
	Exported string String();
	Exported string JSonString();
    Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string k) {
        KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
		map<long,KifElement*>::iterator iter=values.find(n);
		if (iter==values.end())
			return kifNOELEMENT;
		return iter->second;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);
	Exported bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		map<long,KifElement*>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second->Sum();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		map<long,KifElement*>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v*=itx->second->Product();
		return v;
	}
	
	Exported void Setreference(int inc=1);

	Exported void Setpopped(int v=1,bool force=false) {
		if (reference<=0 || force) {
			Putpopped(v);
			map<long,KifElement*>::iterator itx;
			for (itx=values.begin();itx!=values.end();itx++) {
				if (itx->second!=NULL)
					itx->second->Setpopped(v,force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		map<long,KifElement*>::iterator itx;
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

class KifFloatTreeMap : public KifEvaluateMap {
public:

	map<double,KifElement*> values;	

	void Items(KifVector* kvect) {
		map<double,KifElement*>::iterator it;
		KifFloatTreeMap* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifFloatTreeMap(kifcode,NULL);
			kval->push(it->first,it->second);
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	Exported KifFloatTreeMap(KifCode* klc,KifElement* kp) : KifEvaluateMap(klc,kp,kifFloatTreeMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);
	Exported KifElement* Map(KifElement*,KifMap* itself=NULL) {
		return this;
	}
	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

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
		return new KifFloatTreeMap(kifcode, NULL);
	}
	Exported string String();
	Exported string JSonString();
    Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string k) {
        KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		map<double,KifElement*>::iterator iter=values.find(n);
		if (iter==values.end())
			return kifNOELEMENT;
		return iter->second;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);
	Exported bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		map<double,KifElement*>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second->Sum();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		map<double,KifElement*>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v*=itx->second->Product();
		return v;
	}
	
	Exported void Setreference(int inc=1);

	Exported void Setpopped(int v=1,bool force=false) {
		if (reference<=0 || force) {
			Putpopped(v);
			map<double,KifElement*>::iterator itx;
			for (itx=values.begin();itx!=values.end();itx++) {
				if (itx->second!=NULL)
					itx->second->Setpopped(v,force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		map<double,KifElement*>::iterator itx;
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

class KifIteratorIntegerTreeMap : public KifIterationContainer {
public:
	map<long,KifElement*>* values;
	map<long,KifElement*>::iterator it;
	map<long,KifElement*>::reverse_iterator itr;

	KifIteratorIntegerTreeMap(KifCode* klc,KifElement* kp,KifIntegerTreeMap* k) : KifIterationContainer(klc,kp,k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);
	Exported KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNOELEMENT;
			return kifcode->Provideinteger(it->first);		
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
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
		if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifIteratorFloatTreeMap : public KifIterationContainer {
public:
	map<double,KifElement*>* values;
	map<double,KifElement*>::iterator it;
	map<double,KifElement*>::reverse_iterator itr;

	KifIteratorFloatTreeMap(KifCode* klc,KifElement* kp,KifFloatTreeMap* k) : KifIterationContainer(klc,kp,k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);
	Exported KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNOELEMENT;
			return kifcode->Providefloat(it->first);		
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
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
		if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifTreeMapStringString : public KifEvaluateMapValue {
public:
	map<string,string> values;

	void storekey(char* k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void storekey(string k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		map<string,string>::iterator it;
		KifTreeMapStringString* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapStringString(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapStringString(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifTreeMapStringString) {}

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
		return kifTreeMapStringString;
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
		return new KifTreeMapStringString(kifcode,NULL);
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);

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

class KifIteratorTreeMapStringString : public KifIterationContainer {
public:

	map<string,string>* values;

	map<string,string>::iterator it;
	map<string,string>::reverse_iterator itr;
    

	KifIteratorTreeMapStringString(KifCode* klc,KifElement* kp,KifTreeMapStringString* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->String();
			return;
		}
		if (itr==values->rend())
			return;
		itr->second=ke->String();
	}

	KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->first);
		}

		if (itr==values->rend())
			return kifNULL;

		return kifcode->Providestringraw(itr->first);
	}

	long IteratorKeyInteger() {
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return conversionintegerhexa(STR(it->first));
		}

		if (itr==values->rend())
			return -1;

		return conversionintegerhexa(STR(itr->first));
	}

	double IteratorKeyFloat() {
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return convertfloat(STR(it->first));
		}

		if (itr==values->rend())
			return -1;

		return convertfloat(STR(itr->first));
	}

	string IteratorKeyString() {
		if (reverse==false) {
			if (it==values->end())
				return "";

			return it->first;
		}
		if (itr==values->rend())
			return "";

		return itr->first;
	}

	string IteratorValueString() {
		if (reverse==false) {
			if (it==values->end())
				return "";
			return it->second;
		}
		if (itr==values->rend())
			return "";

		return itr->second;
	}

	KifElement* IteratorValue() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->second);
		}
		if (itr==values->rend())
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
			if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;

		return kifTRUE;
	}

};

class KifTreeMapStringInteger : public KifEvaluateMapValue {
public:
	map<string,long> values;

	void storekey(char* k,long v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void storekey(string k,long v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		map<string,long>::iterator it;
		KifTreeMapStringInteger* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapStringInteger(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapStringInteger(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifTreeMapStringInteger) {}

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
		map<string,long>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		map<string,long>::iterator itx;
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
		return kifTreeMapStringInteger;
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
		return new KifTreeMapStringInteger(kifcode,NULL);
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);

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

class KifIteratorTreeMapStringInteger : public KifIterationContainer {
public:

	map<string,long>* values;

	map<string,long>::iterator it;
	map<string,long>::reverse_iterator itr;

	KifIteratorTreeMapStringInteger(KifCode* klc,KifElement* kp,KifTreeMapStringInteger* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Integer();
			return;
		}
		if (itr==values->rend())
			return;
		itr->second=ke->Integer();
	}

	KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->first);
		}
		if (itr==values->rend())
			return kifNULL;

		return kifcode->Providestringraw(itr->first);
	}

	long IteratorKeyInteger() {
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return conversionintegerhexa(STR(it->first));
		}

		if (itr==values->rend())
			return -1;

		return conversionintegerhexa(STR(itr->first));
	}

	double IteratorKeyFloat() {
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return convertfloat(STR(it->first));
		}

		if (itr==values->rend())
			return -1;

		return convertfloat(STR(itr->first));
	}

	string IteratorKeyString() {
		if (reverse==false) {
			if (it==values->end())
				return "";

			return it->first;
		}
		if (itr==values->rend())
			return "";

		return itr->first;
	}

	long IteratorValueInteger() {
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
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
			if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;
		return kifTRUE;
	}
};


class KifTreeMapStringFloat : public KifEvaluateMapValue {
public:
	map<string,double> values;

	void storekey(char* k,double v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void storekey(string k,double v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		map<string,double>::iterator it;
		KifTreeMapStringFloat* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapStringFloat(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapStringFloat(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifTreeMapStringFloat) {}

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
		return kifTreeMapStringFloat;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		map<string,double>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		map<string,double>::iterator itx;
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
		return new KifTreeMapStringFloat(kifcode,NULL);
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);

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

class KifIteratorTreeMapStringFloat : public KifIterationContainer {
public:

	map<string,double>* values;

	map<string,double>::iterator it;
	map<string,double>::reverse_iterator itr;

	KifIteratorTreeMapStringFloat(KifCode* klc,KifElement* kp,KifTreeMapStringFloat* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Float();
			return;
		}
		if (itr==values->rend())
			return;
		itr->second=ke->Float();
	}


	KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->first);
		}

		if (itr==values->rend())
			return kifNULL;

		return kifcode->Providestringraw(itr->first);
	}

	long IteratorKeyInteger() {
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return conversionintegerhexa(STR(it->first));
		}

		if (itr==values->rend())
			return -1;

		return conversionintegerhexa(STR(itr->first));
	}

	double IteratorKeyFloat() {
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return convertfloat(STR(it->first));
		}

		if (itr==values->rend())
			return -1;

		return convertfloat(STR(itr->first));
	}

	string IteratorKeyString() {
		if (reverse==false) {
			if (it==values->end())
				return "";

			return it->first;
		}
		if (itr==values->rend())
			return "";

		return itr->first;
	}

	long IteratorValueInteger() {
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->rend())
			return 0;

		return itr->second;
	}


	double IteratorValueFloat() {
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
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
			if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;
		return kifTRUE;
	}

};


class KifTreeMapIntegerInteger : public KifEvaluateMapValue {
public:
	map<long,long> values;

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
		map<long,long>::iterator it;
		KifTreeMapIntegerInteger* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapIntegerInteger(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapIntegerInteger(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifTreeMapIntegerInteger) {}

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
		map<long,long>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		map<long,long>::iterator itx;
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
		return kifTreeMapIntegerInteger;
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
		return new KifTreeMapIntegerInteger(kifcode,NULL);
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);

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

class KifIteratorTreeMapIntegerInteger : public KifIterationContainer {
public:

	map<long,long>* values;

	map<long,long>::iterator it;
	map<long,long>::reverse_iterator itr;

	KifIteratorTreeMapIntegerInteger(KifCode* klc,KifElement* kp,KifTreeMapIntegerInteger* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Integer();
			return;
		}
		if (itr==values->rend())
			return;
		itr->second=ke->Integer();
	}

	KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->first);
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
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
			if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifTreeMapIntegerFloat : public KifEvaluateMapValue {
public:
	map<long,double> values;

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
		map<long,double>::iterator it;
		KifTreeMapIntegerFloat* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapIntegerFloat(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapIntegerFloat(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifTreeMapIntegerFloat) {}

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
		map<long,double>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		map<long,double>::iterator itx;
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
		return kifTreeMapIntegerFloat;
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
		return new KifTreeMapIntegerFloat(kifcode,NULL);
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);

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

class KifIteratorTreeMapIntegerFloat : public KifIterationContainer {
public:

	map<long,double>* values;

	map<long,double>::iterator it;
	map<long,double>::reverse_iterator itr;

	KifIteratorTreeMapIntegerFloat(KifCode* klc,KifElement* kp,KifTreeMapIntegerFloat* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Float();
			return;
		}
		if (itr==values->rend())
			return;
		itr->second=ke->Float();
	}

	KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->first);
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
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
			if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifTreeMapIntegerString : public KifEvaluateMapValue {
public:
	map<long,string> values;

	void storekey(long k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		map<long,string>::iterator it;
		KifTreeMapIntegerString* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapIntegerString(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapIntegerString(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifTreeMapIntegerString) {}

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
		return kifTreeMapIntegerString;
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
		return new KifTreeMapIntegerString(kifcode,NULL);
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);

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

class KifIteratorTreeMapIntegerString : public KifIterationContainer {
public:

	map<long,string>* values;

	map<long,string>::iterator it;
	map<long,string>::reverse_iterator itr;
    
	KifIteratorTreeMapIntegerString(KifCode* klc,KifElement* kp,KifTreeMapIntegerString* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->String();
			return;
		}
		if (itr==values->rend())
			return;
		itr->second=ke->String();
	}

	KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->first);
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
			return "";

		return itr->second;
	}

	KifElement* IteratorValue() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->second);
		}
		if (itr==values->rend())
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
			if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;

		return kifTRUE;
	}

};

class KifTreeMapFloatInteger : public KifEvaluateMapValue {
public:
	map<double,long> values;

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
		map<double,long>::iterator it;
		KifTreeMapFloatInteger* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapFloatInteger(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapFloatInteger(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifTreeMapFloatInteger) {}

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
		map<double,long>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		map<double,long>::iterator itx;
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
		return kifTreeMapFloatInteger;
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
		return new KifTreeMapFloatInteger(kifcode,NULL);
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);

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

class KifIteratorTreeMapFloatInteger : public KifIterationContainer {
public:

	map<double,long>* values;

	map<double,long>::iterator it;
	map<double,long>::reverse_iterator itr;

	KifIteratorTreeMapFloatInteger(KifCode* klc,KifElement* kp,KifTreeMapFloatInteger* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Integer();
			return;
		}
		if (itr==values->rend())
			return;
		itr->second=ke->Integer();
	}

	KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providefloat(it->first);
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
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
			if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifTreeMapFloatFloat : public KifEvaluateMapValue {
public:
	map<double,double> values;

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
		map<double,double>::iterator it;
		KifTreeMapFloatFloat* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapFloatFloat(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapFloatFloat(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifTreeMapFloatFloat) {}

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
		map<double,double>::iterator itx;
		for (itx=values.begin();itx!=values.end();itx++)
			v+=itx->second;
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		map<double,double>::iterator itx;
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
		return kifTreeMapFloatFloat;
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
		return new KifTreeMapFloatFloat(kifcode,NULL);
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);

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

class KifIteratorTreeMapFloatFloat : public KifIterationContainer {
public:

	map<double,double>* values;

	map<double,double>::iterator it;
	map<double,double>::reverse_iterator itr;

	KifIteratorTreeMapFloatFloat(KifCode* klc,KifElement* kp,KifTreeMapFloatFloat* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->Float();
			return;
		}
		if (itr==values->rend())
			return;
		itr->second=ke->Float();
	}

	KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providefloat(it->first);
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
			return 0;

		return itr->second;
	}

	double IteratorValueFloat() {
		if (reverse==false) {
			if (it==values->end())
				return 0;
			return it->second;
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
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
			if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifTreeMapFloatString : public KifEvaluateMapValue {
public:
	map<double,string> values;

	void storekey(double k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		map<double,string>::iterator it;
		KifTreeMapFloatString* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapFloatString(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapFloatString(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifTreeMapFloatString) {}

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
		return kifTreeMapFloatString;
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
		return new KifTreeMapFloatString(kifcode,NULL);
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

	KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);

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

class KifIteratorTreeMapFloatString : public KifIterationContainer {
public:

	map<double,string>* values;

	map<double,string>::iterator it;
	map<double,string>::reverse_iterator itr;

    
	KifIteratorTreeMapFloatString(KifCode* klc,KifElement* kp,KifTreeMapFloatString* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (reverse==false) {
			if (it==values->end())
				return;
			it->second=ke->String();
			return;
		}
		if (itr==values->rend())
			return;
		itr->second=ke->String();
	}

	KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providefloat(it->first);
		}
		if (itr==values->rend())
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
		if (itr==values->rend())
			return "";

		return itr->second;
	}

	KifElement* IteratorValue() {
		if (reverse==false) {
			if (it==values->end())
				return kifNULL;

			return kifcode->Providestringraw(it->second);
		}
		if (itr==values->rend())
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
			if (itr==values->rend())
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
		itr=values->rbegin();
		if (itr==values->rend())
			return kifFALSE;

		return kifTRUE;
	}

};
//------------------------------------------------------------------
class KifUStringTreeMap : public KifEvaluateMap {
public:

	map<wstring, KifElement*> values;

	void Items(KifVector* kvect) {
		map<wstring, KifElement*>::iterator it;
		KifUStringTreeMap* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifUStringTreeMap(kifcode, NULL);
			kval->push(it->first, it->second);
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	Exported KifUStringTreeMap(KifCode* klc, KifElement* kp) : KifEvaluateMap(klc, kp, kifUStringTreeMap) {}

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
		return kifUStringTreeMap;
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
		return new KifUStringTreeMap(kifcode, NULL);
	}

	Exported string String();
	Exported wstring UString();
	Exported string JSonString();
	Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(wstring n) {
		KifContainerLock _lock(this);
		map<wstring, KifElement*>::iterator iter = values.find(n);
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
		map<wstring, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second->Sum();
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		map<wstring, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v *= itx->second->Product();
		return v;
	}

	Exported void Setreference(int inc = 1);

	Exported void Setpopped(int v = 1, bool force = false) {
		if (reference <= 0 || force) {
			Putpopped(v);
			map<wstring, KifElement*>::iterator itx;
			for (itx = values.begin(); itx != values.end(); itx++) {
				if (itx->second != NULL)
					itx->second->Setpopped(v, force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		map<wstring, KifElement*>::iterator itx;
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

class KifIteratorUStringTreeMap : public KifIterationContainer {
public:
	map<wstring, KifElement*>* values;
	map<wstring, KifElement*>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	map<wstring, KifElement*>::iterator itr;
#else
	map<wstring, KifElement*>::reverse_iterator itr;
#endif

	KifIteratorUStringTreeMap(KifCode* klc, KifElement* kp, KifUStringTreeMap* k) : KifIterationContainer(klc, kp, k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

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
class KifTreeMapUStringUString : public KifEvaluateMapValue {
public:
	map<wstring, wstring> values;

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
		map<wstring, wstring>::iterator it;
		KifTreeMapUStringUString* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifTreeMapUStringUString(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifTreeMapUStringUString(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifTreeMapUStringUString) {}

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
		return kifTreeMapUStringUString;
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
		return new KifTreeMapUStringUString(kifcode, NULL);
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

class KifIteratorTreeMapUStringUString : public KifIterationContainer {
public:

	map<wstring, wstring>* values;

	map<wstring, wstring>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	map<wstring, wstring>::iterator itr;
#else
	map<wstring, wstring>::reverse_iterator itr;
#endif

	KifIteratorTreeMapUStringUString(KifCode* klc, KifElement* kp, KifTreeMapUStringUString* k) : KifIterationContainer(klc, kp, k) {}

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
class KifTreeMapUStringInteger : public KifEvaluateMapValue {
public:
	map<wstring,long> values;

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
		map<wstring,long>::iterator it;
		KifTreeMapUStringInteger* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifTreeMapUStringInteger(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifTreeMapUStringInteger(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifTreeMapUStringInteger) {}

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
		map<wstring,long>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second;
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		map<wstring,long>::iterator itx;
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
		return kifTreeMapUStringInteger;
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
		return new KifTreeMapUStringInteger(kifcode, NULL);
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

class KifIteratorTreeMapUStringInteger : public KifIterationContainer {
public:

	map<wstring,long>* values;

	map<wstring,long>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	map<wstring,long>::iterator itr;
#else
	map<wstring,long>::reverse_iterator itr;
#endif

	KifIteratorTreeMapUStringInteger(KifCode* klc, KifElement* kp, KifTreeMapUStringInteger* k) : KifIterationContainer(klc, kp, k) {}

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
class KifTreeMapUStringFloat : public KifEvaluateMapValue {
public:
	map<wstring, double> values;

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
		map<wstring, double>::iterator it;
		KifTreeMapUStringFloat* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifTreeMapUStringFloat(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifTreeMapUStringFloat(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifTreeMapUStringFloat) {}

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
		return kifTreeMapUStringFloat;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		map<wstring, double>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second;
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		map<wstring, double>::iterator itx;
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
		return new KifTreeMapUStringFloat(kifcode, NULL);
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

class KifIteratorTreeMapUStringFloat : public KifIterationContainer {
public:

	map<wstring, double>* values;

	map<wstring, double>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	map<wstring, double>::iterator itr;
#else
	map<wstring, double>::reverse_iterator itr;
#endif

	KifIteratorTreeMapUStringFloat(KifCode* klc, KifElement* kp, KifTreeMapUStringFloat* k) : KifIterationContainer(klc, kp, k) {}

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
//----------------------------------------------------------------------------------------
class KifTreeMapIntegerUString : public KifEvaluateMapValue {
public:
	map<long, wstring> values;

	void storekey(long k, wstring v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		map<long, wstring>::iterator it;
		KifTreeMapIntegerUString* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifTreeMapIntegerUString(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifTreeMapIntegerUString(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifTreeMapIntegerUString) {}

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
		return kifTreeMapIntegerUString;
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
		return new KifTreeMapIntegerUString(kifcode, NULL);
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

class KifIteratorTreeMapIntegerUString : public KifIterationContainer {
public:

	map<long, wstring>* values;

	map<long, wstring>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	map<long, wstring>::iterator itr;
#else
	map<long, wstring>::reverse_iterator itr;
#endif


	KifIteratorTreeMapIntegerUString(KifCode* klc, KifElement* kp, KifTreeMapIntegerUString* k) : KifIterationContainer(klc, kp, k) {}

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
class KifTreeMapFloatUString : public KifEvaluateMapValue {
public:
	map<double, wstring> values;

	void storekey(double k, wstring v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		map<double, wstring>::iterator it;
		KifTreeMapFloatUString* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifTreeMapFloatUString(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifTreeMapFloatUString(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifTreeMapFloatUString) {}

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
		return kifTreeMapFloatUString;
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
		return new KifTreeMapFloatUString(kifcode, NULL);
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

class KifIteratorTreeMapFloatUString : public KifIterationContainer {
public:

	map<double, wstring>* values;

	map<double, wstring>::iterator it;
#if defined(KIF_HASH_MAP) && !defined(WIN32)
	map<double, wstring>::iterator itr;
#else
	map<double, wstring>::reverse_iterator itr;
#endif


	KifIteratorTreeMapFloatUString(KifCode* klc, KifElement* kp, KifTreeMapFloatUString* k) : KifIterationContainer(klc, kp, k) {}

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

#endif


