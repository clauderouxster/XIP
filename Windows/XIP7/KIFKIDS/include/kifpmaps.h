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


#ifndef i_kifpmaps
#define i_kifpmaps

class KifPrimeMap : public KifEvaluateMap {
public:

	prime_hash<string,KifElement*> values;	

	void Items(KifVector* kvect) {
		prime_iterator<string,KifElement*> itx;
		KifPrimeMap* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMap(kifcode,NULL);
			kval->push(itx.key(),itx.value());
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	Exported KifPrimeMap(KifCode* klc,KifElement* kp) : KifEvaluateMap(klc,kp,kifPrimeMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
    }

	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);
	Exported KifElement* PrimeMap(KifElement*,KifPrimeMap* itself=NULL) {
		return this;
	}

	bool aMapContainer() {
		return true;
	}

	int Garbager() {
		return kifPrimeMap;
	}


	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return values[buffer];
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return values[buffer];
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* ke) {
		KifContainerLock _lock(this);
		string s=ke->String();
		if (values.find(s)==false)
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newinstance() {
		return new KifPrimeMap(kifcode, NULL);
	}
	Exported string String();
	Exported string JSonString();
    Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string n) {
        KifContainerLock _lock(this);
		if (values.find(n))
			return values[n];
		return kifNOELEMENT;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);
	Exported bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		prime_iterator<string,KifElement*> itx;
		for (itx=values;!itx.end();itx++)
			v+=itx.value()->Sum();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		prime_iterator<string,KifElement*> itx;
		for (itx=values;!itx.end();itx++)
			v*=itx.value()->Product();
		return v;
	}
	
	Exported void Setreference(int inc=1);

	Exported void Setpopped(int v=1,bool force=false) {
		if (reference<=0 || force) {
			Putpopped(v);
			prime_iterator<string,KifElement*> itx;
			for (itx=values;!itx.end();itx++) {
				if (itx.value()!=NULL)
					itx.value()->Setpopped(v,force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		prime_iterator<string,KifElement*> itx;
		for (itx=values;!itx.end();itx++) {
			if (itx.value()!=NULL)
				itx.value()->Clean();
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
	Exported void push(string key,KifElement* val);
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

class KifIteratorPrimeMap : public KifIterationContainer {
public:
	prime_hash<string,KifElement*>* values;
	prime_iterator<string,KifElement*> it;

	KifIteratorPrimeMap(KifCode* klc,KifElement* kp,KifPrimeMap* k) : KifIterationContainer(klc,kp,k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		if (it.end())
			return kifNOELEMENT;
		return kifcode->Providestringraw(it.key());		
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return conversionintegerhexa(STR(it.key()));
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return convertfloat(STR(it.key()));
	}

	string IteratorKeyString() {

		if (it.end())
			return "";
		return it.key();
	}

	Exported KifElement* IteratorValue() {
		if (it.end())
			return kifNOELEMENT;
		return it.value();
	}

	Exported void Next() {
		it++;
	}

	Exported KifElement* End() {
		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	Exported KifElement* Begin() {	

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}	
};


class KifIntegerPrimeMap : public KifEvaluateMap {
public:

	prime_hash<long,KifElement*> values;	

	void Items(KifVector* kvect) {
		prime_iterator<long,KifElement*> itx;
		KifIntegerPrimeMap* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifIntegerPrimeMap(kifcode,NULL);
			kval->push(itx.key(),itx.value());
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	Exported KifIntegerPrimeMap(KifCode* klc,KifElement* kp) : KifEvaluateMap(klc,kp,kifIntegerPrimeMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
    }

	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);
	Exported KifElement* PrimeMap(KifElement*,KifPrimeMap* itself=NULL) {
		return this;
	}

	bool aMapContainer() {
		return true;
	}

	int Garbager() {
		return kifPrimeMap;
	}

	void push(long l,KifElement*);

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
		if (values.find(n))
			return values[n];
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* ke) {
		KifContainerLock _lock(this);
		long s=ke->Integer();
		if (values.find(s)==false)
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newinstance() {
		return new KifIntegerPrimeMap(kifcode, NULL);
	}
	Exported string String();
	Exported string JSonString();
    Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string k) {
        KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
		if (values.find(n))
			return values[n];
		return kifNOELEMENT;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);
	Exported bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		prime_iterator<long,KifElement*> itx;
		for (itx=values;!itx.end();itx++)
			v+=itx.value()->Sum();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		prime_iterator<long,KifElement*> itx;
		for (itx=values;!itx.end();itx++)
			v*=itx.value()->Product();
		return v;
	}
	
	Exported void Setreference(int inc=1);

	Exported void Setpopped(int v=1,bool force=false) {
		if (reference<=0 || force) {
			Putpopped(v);
			prime_iterator<long,KifElement*> itx;
			for (itx=values;!itx.end();itx++) {
				if (itx.value()!=NULL)
					itx.value()->Setpopped(v,force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		prime_iterator<long,KifElement*> itx;
		for (itx=values;!itx.end();itx++) {
			if (itx.value()!=NULL)
				itx.value()->Clean();
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

class KifFloatPrimeMap : public KifEvaluateMap {
public:

	prime_hash<double,KifElement*> values;	

	void Items(KifVector* kvect) {
		prime_iterator<double,KifElement*> itx;
		KifFloatPrimeMap* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifFloatPrimeMap(kifcode,NULL);
			kval->push(itx.key(),itx.value());
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	Exported KifFloatPrimeMap(KifCode* klc,KifElement* kp) : KifEvaluateMap(klc,kp,kifFloatPrimeMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);
	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
    }

	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);
	Exported KifElement* PrimeMap(KifElement*,KifPrimeMap* itself=NULL) {
		return this;
	}

	bool aMapContainer() {
		return true;
	}

	int Garbager() {
		return kifPrimeMap;
	}

	void push(double l,KifElement*);

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n))
			return values[n];
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* ke) {
		KifContainerLock _lock(this);
		double s=ke->Float();
		if (values.find(s)==false)
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newinstance() {
		return new KifFloatPrimeMap(kifcode, NULL);
	}
	Exported string String();
	Exported string JSonString();
    Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string k) {
        KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n))
			return values[n];
		return kifNOELEMENT;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,int idthread,KifCallFunction* callfunc);
	Exported bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		prime_iterator<double,KifElement*> itx;
		for (itx=values;!itx.end();itx++)
			v+=itx.value()->Sum();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		prime_iterator<double,KifElement*> itx;
		for (itx=values;!itx.end();itx++)
			v*=itx.value()->Product();
		return v;
	}
	
	Exported void Setreference(int inc=1);

	Exported void Setpopped(int v=1,bool force=false) {
		if (reference<=0  || force) {
			Putpopped(v);
			prime_iterator<double,KifElement*> itx;
			for (itx=values;!itx.end();itx++) {
				if (itx.value()!=NULL)
					itx.value()->Setpopped(v,force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		prime_iterator<double,KifElement*> itx;
		for (itx=values;!itx.end();itx++) {
			if (itx.value()!=NULL)
				itx.value()->Clean();
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

class KifIteratorIntegerPrimeMap : public KifIterationContainer {
public:
	prime_hash<long,KifElement*>* values;
	prime_iterator<long,KifElement*> it;

	KifIteratorIntegerPrimeMap(KifCode* klc,KifElement* kp,KifIntegerPrimeMap* k) : KifIterationContainer(klc,kp,k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);
	Exported KifElement* IteratorKey() {

		if (it.end())
			return kifNOELEMENT;
		return kifcode->Provideinteger(it.key());		
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return it.key();
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return it.key();
	}

	string IteratorKeyString();

	string IteratorValueString() {
		return it.value()->String();
	}

	Exported KifElement* IteratorValue() {

		if (it.end())
			return kifNOELEMENT;
		return it.value();
	}

	Exported void Next() {
		it++;
	}

	Exported KifElement* End() {
		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	Exported KifElement* Begin() {	

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;	
	}
};

class KifIteratorFloatPrimeMap : public KifIterationContainer {
public:
	prime_hash<double,KifElement*>* values;
	prime_iterator<double,KifElement*> it;


	KifIteratorFloatPrimeMap(KifCode* klc,KifElement* kp,KifFloatPrimeMap* k) : KifIterationContainer(klc,kp,k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		if (it.end())
			return kifNOELEMENT;
		return kifcode->Providefloat(it.key());		
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return it.key();
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return it.key();
	}

	string IteratorKeyString();

	string IteratorValueString() {
		return it.value()->String();
	}

	Exported KifElement* IteratorValue() {

		if (it.end())
			return kifNOELEMENT;
		return it.value();
	}

	Exported void Next() {	
		it++;
	}

	Exported KifElement* End() {
		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	Exported KifElement* Begin() {	

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapStringString : public KifEvaluateMapValue {
public:
	prime_hash<string,string> values;

	void storekey(char* k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void storekey(string k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		prime_iterator<string,string> itx;
		KifPrimeMapStringString* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapStringString(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapStringString(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifPrimeMapStringString) {}

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
		return kifPrimeMapStringString;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Providestring(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Providestring(values[buffer]);
		return kifNOELEMENT;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
    }

	string ValueOnKeyString(string k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return "";
	}


	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		if (values.find(s->String()))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapStringString(kifcode,NULL);
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
		if (values.find(n)==false)
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

class KifIteratorPrimeStringString : public KifIterationContainer {
public:

	prime_hash<string,string>* values;
	prime_iterator<string,string> it;

	KifIteratorPrimeStringString(KifCode* klc,KifElement* kp,KifPrimeMapStringString* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->String());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.key());
	}

	long IteratorKeyInteger() {

		if (it.end())
			return -1;
		return conversionintegerhexa(STR(it.key()));
	}

	double IteratorKeyFloat() {

		if (it.end())
			return -1;
		return convertfloat(STR(it.key()));
	}

	string IteratorKeyString() {

		if (it.end())
			return "";

		return it.key();
	}

	string IteratorValueString() {

		if (it.end())
			return "";
		return it.value();
	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {
		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {

		it=*values;
		if (it.end())
			return kifFALSE;		
		return kifTRUE;
	}

};

class KifPrimeMapStringInteger : public KifEvaluateMapValue {
public:
	prime_hash<string,long> values;

	void storekey(char* k,long v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void storekey(string k,long v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		prime_iterator<string,long> itx;
		KifPrimeMapStringInteger* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapStringInteger(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapStringInteger(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifPrimeMapStringInteger) {}

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
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
    }

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		prime_iterator<string,long> itx;
		for (itx=values;!itx.end();itx++)
			v+=itx.value();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		prime_iterator<string,long> itx;
		for (itx=values;!itx.end();itx++)
			v*=itx.value();
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Provideinteger(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Provideinteger(values[buffer]);
		return kifNOELEMENT;
	}


	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
			a=true;
			return values[buffer];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifPrimeMapStringInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		string i=s->String();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapStringInteger(kifcode,NULL);
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
		if ( values.find(n)==false)
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

class KifIteratorPrimeMapStringInteger : public KifIterationContainer {
public:

	prime_hash<string,long>* values;

	prime_iterator<string,long> it;

	KifIteratorPrimeMapStringInteger(KifCode* klc,KifElement* kp,KifPrimeMapStringInteger* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->Integer());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.key());
	}

	long IteratorKeyInteger() {

		if (it.end())
			return -1;
		return conversionintegerhexa(STR(it.key()));
	}

	double IteratorKeyFloat() {

		if (it.end())
			return -1;
		return convertfloat(STR(it.key()));
	}

	string IteratorKeyString() {

		if (it.end())
			return "";

		return it.key();
	}

	long IteratorValueInteger() {

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.value());
	}
	void Next() {		
		it++;
	}

	KifElement* End() {
		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapStringFloat : public KifEvaluateMapValue {
public:
	prime_hash<string,double> values;

	void storekey(char* k,double v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void storekey(string k,double v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		prime_iterator<string,double> itx;
		KifPrimeMapStringFloat* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapStringFloat(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapStringFloat(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifPrimeMapStringFloat) {}

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
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
    }

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		char buffer[20];
		sprintf_s(buffer,20,"%ld",k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Providefloat(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		char buffer[20];
		sprintf_s(buffer,20,"%g",k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Providefloat(values[buffer]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
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
		if (values.find(k)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
			a=true;
			return values[buffer];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifPrimeMapStringFloat;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		prime_iterator<string,double> itx;
		for (itx=values;!itx.end();itx++)
			v+=itx.value();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		prime_iterator<string,double> itx;
		for (itx=values;!itx.end();itx++)
			v*=itx.value();
		return v;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		string i=s->String();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapStringFloat(kifcode,NULL);
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
		if (values.find(n)==false)
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

class KifIteratorPrimeMapStringFloat : public KifIterationContainer {
public:

	prime_hash<string,double>* values;

	prime_iterator<string,double> it;

	KifIteratorPrimeMapStringFloat(KifCode* klc,KifElement* kp,KifPrimeMapStringFloat* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->Float());
	}


	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.key());
	}

	string IteratorKeyString() {

		if (it.end())
			return "";

		return it.key();
	}

	long IteratorKeyInteger() {

		if (it.end())
			return -1;
		return conversionintegerhexa(STR(it.key()));
	}

	double IteratorKeyFloat() {

		if (it.end())
			return -1;
		return convertfloat(STR(it.key()));
	}

	long IteratorValueInteger() {

		if (it.end())
			return 0;
		return it.value();
	}


	double IteratorValueFloat() {

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}

};

class KifPrimeMapIntegerInteger : public KifEvaluateMapValue {
public:
	prime_hash<long,long> values;

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
		prime_iterator<long,long> itx;
		KifPrimeMapIntegerInteger* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapIntegerInteger(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapIntegerInteger(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifPrimeMapIntegerInteger) {}

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
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
    }

	Exported KifElement* Atom(bool forced=false) {
		if (reference==0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		prime_iterator<long,long> itx;
		for (itx=values;!itx.end();itx++)
			v+=itx.value();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		prime_iterator<long,long> itx;
		for (itx=values;!itx.end();itx++)
			v*=itx.value();
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
		if (values.find(n))
			return kifcode->Provideinteger(values[n]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}


	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
		if (values.find(n)) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
		if (values.find(n)) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyIInteger(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyIFloat(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyFInteger(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFFloat(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifPrimeMapIntegerInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i=s->Integer();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapIntegerInteger(kifcode,NULL);
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
		if (values.find(k)==false)
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

class KifIteratorPrimeMapIntegerInteger : public KifIterationContainer {
public:

	prime_hash<long,long>* values;

	prime_iterator<long,long> it;

	KifIteratorPrimeMapIntegerInteger(KifCode* klc,KifElement* kp,KifPrimeMapIntegerInteger* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->Integer());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.key());
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return it.key();
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return it.key();
	}

	string IteratorKeyString();

	long IteratorValueInteger() {

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.value());
	}

	void Next() {
		it++;
	}

	KifElement* End() {

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapIntegerFloat : public KifEvaluateMapValue {
public:
	prime_hash<long,double> values;

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
		prime_iterator<long,double> itx;
		KifPrimeMapIntegerFloat* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapIntegerFloat(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}


	KifPrimeMapIntegerFloat(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifPrimeMapIntegerFloat) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;        
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

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		prime_iterator<long,double> itx;
		for (itx=values;!itx.end();itx++)
			v+=itx.value();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		prime_iterator<long,double> itx;
		for (itx=values;!itx.end();itx++)
			v*=itx.value();
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
		if (values.find(n))
			return kifcode->Providefloat(values[n]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
		if (values.find(n)) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=conversionintegerhexa(STR(k));
		if (values.find(n)) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyIInteger(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyIFloat(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyFInteger(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFFloat(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifPrimeMapIntegerFloat;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i=s->Integer();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapIntegerFloat(kifcode,NULL);
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
		if (values.find(k)==false)
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

class KifIteratorPrimeMapIntegerFloat : public KifIterationContainer {
public:

	prime_hash<long,double>* values;

	prime_iterator<long,double> it;

	KifIteratorPrimeMapIntegerFloat(KifCode* klc,KifElement* kp,KifPrimeMapIntegerFloat* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->Float());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.key());
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return it.key();
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return it.key();
	}

	string IteratorKeyString();

	long IteratorValueInteger() {

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapIntegerString : public KifEvaluateMapValue {
public:
	prime_hash<long,string> values;

	void storekey(long k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		prime_iterator<long,string> itx;
		KifPrimeMapIntegerString* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapIntegerString(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapIntegerString(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifPrimeMapIntegerString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
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
		return kifPrimeMapIntegerString;
	}

	KifElement* ValueOnKey(string n) {
		KifContainerLock _lock(this);
		long k=conversionintegerhexa(STR(n));
		if (values.find(k))
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	string ValueOnKeyString(string n,bool& a) {
		KifContainerLock _lock(this);
		long k=conversionintegerhexa(STR(n));
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return "";
	}


	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i=s->Integer();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapIntegerString(kifcode,NULL);
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
		if (values.find(k)==false)
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

class KifIteratorPrimeMapIntegerString : public KifIterationContainer {
public:

	prime_hash<long,string>* values;

	prime_iterator<long,string> it;


	KifIteratorPrimeMapIntegerString(KifCode* klc,KifElement* kp,KifPrimeMapIntegerString* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->String());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.key());
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return it.key();
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return it.key();
	}

	string IteratorKeyString();

	string IteratorValueString() {

		if (it.end())
			return "";
		return it.value();
	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.value());
	}

	void Next() {

		it++;
	}

	KifElement* End() {

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {

		it=*values;
		if (it.end())
			return kifFALSE;		
		return kifTRUE;
	}

};

class KifPrimeMapFloatInteger : public KifEvaluateMapValue {
public:
	prime_hash<double,long> values;

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
		prime_iterator<double,long> itx;
		KifPrimeMapFloatInteger* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapFloatInteger(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapFloatInteger(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifPrimeMapFloatInteger) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
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

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		prime_iterator<double,long> itx;
		for (itx=values;!itx.end();itx++)
			v+=itx.value();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		prime_iterator<double,long> itx;
		for (itx=values;!itx.end();itx++)
			v*=itx.value();
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n))
			return kifcode->Provideinteger(values[n]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyIInteger(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyIFloat(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyFInteger(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFFloat(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifPrimeMapFloatInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		double i=s->Float();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapFloatInteger(kifcode,NULL);
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
		if (values.find(k)==false)
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

class KifIteratorPrimeMapFloatInteger : public KifIterationContainer {
public:

	prime_hash<double,long>* values;

	prime_iterator<double,long> it;

	KifIteratorPrimeMapFloatInteger(KifCode* klc,KifElement* kp,KifPrimeMapFloatInteger* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->Integer());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.key());
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return it.key();
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return it.key();
	}

	string IteratorKeyString();

	long IteratorValueInteger() {

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapFloatFloat : public KifEvaluateMapValue {
public:
	prime_hash<double,double> values;

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
		prime_iterator<double,double> itx;
		KifPrimeMapFloatFloat* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapFloatFloat(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapFloatFloat(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifPrimeMapFloatFloat) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
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

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v=0;
		prime_iterator<double,double> itx;
		for (itx=values;!itx.end();itx++)
			v+=itx.value();
		return v;
	}
	
	Exported double Product() {
		KifContainerLock _lock(this);
		double v=1;
		prime_iterator<double,double> itx;
		for (itx=values;!itx.end();itx++)
			v*=itx.value();
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n))
			return kifcode->Providefloat(values[n]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(string k,bool& a) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		double n=convertfloat(STR(k));
		if (values.find(n)) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyIInteger(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyIFloat(long k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	long ValueOnKeyFInteger(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFFloat(double k,bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return 0;
	}

	Exported KifType theType() {
		return kifPrimeMapFloatFloat;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		double i=s->Float();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapFloatFloat(kifcode,NULL);
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
		if (values.find(k)==false)
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

class KifIteratorPrimeMapFloatFloat : public KifIterationContainer {
public:

	prime_hash<double,double>* values;

	prime_iterator<double,double> it;

	KifIteratorPrimeMapFloatFloat(KifCode* klc,KifElement* kp,KifPrimeMapFloatFloat* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->Float());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.key());
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return it.key();
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return it.key();
	}

	string IteratorKeyString();

	long IteratorValueInteger() {

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapFloatString : public KifEvaluateMapValue {
public:
	prime_hash<double,string> values;

	void storekey(double k,string v) {
		KifContainerLock _lock(this);
		values[k]=v;
	}

	void Items(KifVector* kvect) {
		prime_iterator<double,string> itx;
		KifPrimeMapFloatString* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapFloatString(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapFloatString(KifCode* klc,KifElement* kp) : KifEvaluateMapValue(klc,kp,kifPrimeMapFloatString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

    KifElement* EvaluationMap() {
        KifVectorInteger* kvect=new KifVectorInteger(kifcode,NULL);
        int t,c,n,m;
        values.occupation(t,c,n,m);
        kvect->values.push_back(t);
        kvect->values.push_back(c);
        kvect->values.push_back(n);
        kvect->values.push_back(m);
        return kvect;
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
		return kifPrimeMapFloatString;
	}

	KifElement* ValueOnKey(string n) {
		KifContainerLock _lock(this);
		double k=convertfloat(STR(n));
		if (values.find(k))
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	string ValueOnKeyString(string n,bool& a) {
		KifContainerLock _lock(this);
		double k=convertfloat(STR(n));
		if (values.find(k)) {
			a=true;
			return values[k];
		}
		a=false;
		return "";
	}


	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		double i=s->Float();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapFloatString(kifcode,NULL);
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
		if (values.find(k)==false)
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

class KifIteratorPrimeMapFloatString : public KifIterationContainer {
public:

	prime_hash<double,string>* values;

	prime_iterator<double,string> it;

	KifIteratorPrimeMapFloatString(KifCode* klc,KifElement* kp,KifPrimeMapFloatString* k) : KifIterationContainer(klc,kp,k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->String());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.key());
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return it.key();
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return it.key();
	}

	string IteratorKeyString();

	string IteratorValueString() {

		if (it.end())
			return "";
		return it.value();

	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {

		it=*values;
		if (it.end())
			return kifFALSE;		
		return kifTRUE;
	}

};

//------------------------------------------------------------------
class KifUStringPrimeMap : public KifEvaluateMap {
public:

	prime_hash<wstring, KifElement*> values;

	void Items(KifVector* kvect) {
		KifUStringPrimeMap* kval;
		prime_iterator<wstring, KifElement*> it;
		for (it = values; !it.end(); it++) {
			kval = new KifUStringPrimeMap(kifcode, NULL);
			kval->push(it.key(), it.value());
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	Exported KifUStringPrimeMap(KifCode* klc, KifElement* kp) : KifEvaluateMap(klc, kp, kifUStringPrimeMap) {}

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
		return kifUStringPrimeMap;
	}


	KifElement* ValueOnKey(wstring k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return values[buffer];
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return values[buffer];
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* ke) {
		KifContainerLock _lock(this);
		wstring s = ke->UString();
		if (values.find(s))
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newinstance() {
		return new KifUStringPrimeMap(kifcode, NULL);
	}

	Exported string String();
	Exported wstring UString();
	Exported string JSonString();
	Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(wstring n) {
		KifContainerLock _lock(this);
		if (values.find(n))
			return values[n];
		return kifNOELEMENT;
	}

	KifElement* Values(string n) {
		wstring wn;
		s_utf8_to_unicode(wn, STR(n));
		if (values.find(wn))
			return values[wn];
		return kifNOELEMENT;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);
	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		prime_iterator<wstring, KifElement*> it;
		for (it = values; !it.end(); it++)
			v += it.value()->Sum();
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		prime_iterator<wstring, KifElement*> it;
		for (it = values; !it.end(); it++)
			v *= it.value()->Product();
		return v;
	}

	Exported void Setreference(int inc = 1);

	Exported void Setpopped(int v = 1, bool force = false) {
		if (reference <= 0 || force) {
			Putpopped(v);
			prime_iterator<wstring, KifElement*> it;
			for (it = values; !it.end(); it++) {
				if (it.value() != NULL)
					it.value()->Setpopped(v, force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		prime_iterator<wstring, KifElement*> it;
		for (it = values; !it.end(); it++) {
			if (it.value() != NULL)
				it.value()->Clean();
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

class KifIteratorUStringPrimeMap : public KifIterationContainer {
public:
	prime_hash<wstring,KifElement*>* values;
	prime_iterator<wstring,KifElement*> it;

	KifIteratorUStringPrimeMap(KifCode* klc, KifElement* kp, KifUStringPrimeMap* k) : KifIterationContainer(klc, kp, k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		if (it.end())
			return kifNOELEMENT;
		return kifcode->Provideustring(it.key());
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		string x;
		wstring wx(it.key());
		sc_unicode_to_utf8(x, wx);

		return conversionintegerhexa(STR(x));
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		string x;
		wstring wx(it.key());
		sc_unicode_to_utf8(x, wx);
		return convertfloat(STR(x));
	}

	wstring IteratorKeyUString() {

		if (it.end())
			return L"";
		return it.key();
	}

	string IteratorKeyString() {

		if (it.end())
			return "";
		
		string x;
		wstring wx(it.key());
		sc_unicode_to_utf8(x, wx);
		return x;
	}

	Exported KifElement* IteratorValue() {
		if (it.end())
			return kifNOELEMENT;
		return it.value();
	}

	Exported void Next() {
		it++;
	}

	Exported KifElement* End() {
		if (it.end())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {

		it = *values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};
//------------------------------------------------------------------------------------
class KifPrimeMapUStringUString : public KifEvaluateMapValue {
public:
	prime_hash<wstring, wstring> values;

	void storekey(char* sk, string sv) {
		wstring k;
		s_utf8_to_unicode(k, sk);
		wstring v;
		s_utf8_to_unicode(v, STR(sv));
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void storekey(string sk, string sv) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));
		wstring v;
		s_utf8_to_unicode(v, STR(sv));
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void storekey(wstring k, wstring v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}


	void Items(KifVector* kvect) {
		prime_iterator<wstring, wstring> itx;
		KifPrimeMapUStringUString* kval;
		for (itx = values; !itx.end(); itx++) {
			kval = new KifPrimeMapUStringUString(kifcode, NULL);
			kval->values[itx.key()] = itx.value();
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifPrimeMapUStringUString(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifPrimeMapUStringUString) {}

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
		return kifPrimeMapUStringUString;
	}

	KifElement* ValueOnKey(wstring k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnKey(string sk) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Provideustring(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Provideustring(values[buffer]);
		return kifNOELEMENT;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

	KifElement* EvaluationMap() {
		KifVectorInteger* kvect = new KifVectorInteger(kifcode, NULL);
		int t, c, n, m;
		values.occupation(t, c, n, m);
		kvect->values.push_back(t);
		kvect->values.push_back(c);
		kvect->values.push_back(n);
		kvect->values.push_back(m);
		return kvect;
	}

	wstring ValueOnKeyString(wstring k, bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
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
		if (values.find(k)) {
			a = true;
			s_unicode_to_utf8(sk, values[k]);
			return sk;
		}
		a = false;
		return "";
	}


	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		if (values.find(s->UString()))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapUStringUString(kifcode, NULL);
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
		if (values.find(n) == false)
			return kifNOELEMENT;
		return kifTRUE;
	}

	KifElement* Values(wstring n) {
		KifContainerLock _lock(this);
		if (values.find(n) == false)
			return kifNOELEMENT;
		return kifTRUE;
	}

	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc);

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

class KifIteratorPrimeUStringUString : public KifIterationContainer {
public:

	prime_hash<wstring, wstring>* values;
	prime_iterator<wstring, wstring> it;

	KifIteratorPrimeUStringUString(KifCode* klc, KifElement* kp, KifPrimeMapUStringUString* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->UString());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideustring(it.key());
	}

	long IteratorKeyInteger() {

		if (it.end())
			return -1;
		return convertinteger(it.key());
	}

	double IteratorKeyFloat() {

		if (it.end())
			return -1;
		return convertfloat(it.key());
	}

	wstring IteratorKeyUString() {

		if (it.end())
			return L"";

		return it.key();
	}


	string IteratorKeyString() {

		if (it.end())
			return "";
		string s;
		wstring ws = it.key();
		s_unicode_to_utf8(s, ws);
		return s;
	}

	wstring IteratorValueUString() {

		if (it.end())
			return L"";
		return it.value();
	}


	string IteratorValueString() {

		if (it.end())
			return "";
		string s;
		wstring ws = it.value();
		s_unicode_to_utf8(s, ws);
		return s;
	}

	KifElement* IteratorValue() {
		if (it.end())
			return kifNULL;

		return kifcode->Provideustring(it.value());
	}

	void Next() {
		it++;
	}

	KifElement* End() {
		if (it.end())
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* Begin() {

		it = *values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}

};

//------------------------------------------------------------------------------------
class KifPrimeMapUStringInteger : public KifEvaluateMapValue {
public:
	prime_hash<wstring, long> values;

	void storekey(char* k, long v) {
		wstring sk;
		s_utf8_to_unicode(sk, k);
		KifContainerLock _lock(this);
		values[sk] = v;
	}

	void storekey(string k, long v) {
		wstring sk;
		s_utf8_to_unicode(sk, STR(k));
		KifContainerLock _lock(this);
		values[sk] = v;
	}

	void storekey(wstring k, long v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		prime_iterator<wstring, long> itx;
		KifPrimeMapUStringInteger* kval;
		for (itx = values; !itx.end(); itx++) {
			kval = new KifPrimeMapUStringInteger(kifcode, NULL);
			kval->values[itx.key()] = itx.value();
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifPrimeMapUStringInteger(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifPrimeMapUStringInteger) {}

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
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

	KifElement* EvaluationMap() {
		KifVectorInteger* kvect = new KifVectorInteger(kifcode, NULL);
		int t, c, n, m;
		values.occupation(t, c, n, m);
		kvect->values.push_back(t);
		kvect->values.push_back(c);
		kvect->values.push_back(n);
		kvect->values.push_back(m);
		return kvect;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		prime_iterator<wstring, long> itx;
		for (itx = values; !itx.end(); itx++)
			v += itx.value();
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		prime_iterator<wstring, long> itx;
		for (itx = values; !itx.end(); itx++)
			v *= itx.value();
		return v;
	}

	KifElement* ValueOnKey(wstring k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnKey(string sk) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Provideinteger(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Provideinteger(values[buffer]);
		return kifNOELEMENT;
	}


	long ValueOnKeyInteger(string sk, bool& a) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));
		KifContainerLock _lock(this);
		if (values.find(k)) {
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
		if (values.find(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyInteger(wstring k, bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFloat(wstring k, bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
			a = true;
			return values[buffer];
		}
		a = false;
		return 0;
	}

	Exported KifType theType() {
		return kifPrimeMapUStringInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		wstring i = s->UString();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapUStringInteger(kifcode, NULL);
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


	KifElement* Values(string sk) {
		wstring k;
		s_utf8_to_unicode(k, STR(sk));
		KifContainerLock _lock(this);
		if (values.find(k) == false)
			return kifNOELEMENT;
		return kifTRUE;
	}

	KifElement* Values(wstring k) {
		KifContainerLock _lock(this);
		if (values.find(k) == false)
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc);

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

class KifIteratorPrimeMapUStringInteger : public KifIterationContainer {
public:

	prime_hash<wstring, long>* values;

	prime_iterator<wstring, long> it;

	KifIteratorPrimeMapUStringInteger(KifCode* klc, KifElement* kp, KifPrimeMapUStringInteger* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->Integer());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideustring(it.key());
	}

	long IteratorKeyInteger() {

		if (it.end())
			return -1;
		return convertinteger(it.key());
	}

	double IteratorKeyFloat() {

		if (it.end())
			return -1;
		return convertfloat(it.key());
	}

	wstring IteratorKeyUString() {

		if (it.end())
			return L"";

		return it.key();
	}


	string IteratorKeyString() {

		if (it.end())
			return "";
		wstring v = it.key();
		string s;
		s_unicode_to_utf8(s, v);
		return s;
	}

	long IteratorValueInteger() {

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {

		if (it.end())
			return 0;
		return it.value();
	}

	wstring IteratorValueUString() {
		std::wstringstream os;
		os << it.value();
		return os.str();
	}


	string IteratorValueString() {
		stringstream os;
		os << it.value();
		return os.str();
	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.value());
	}
	void Next() {
		it++;
	}

	KifElement* End() {
		if (it.end())
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* Begin() {

		it = *values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};
//------------------------------------------------------------------------------------
class KifPrimeMapUStringFloat : public KifEvaluateMapValue {
public:
	prime_hash<wstring, double> values;

	void storekey(char* k, double v) {
		wstring sk;
		s_utf8_to_unicode(sk, k);
		KifContainerLock _lock(this);
		values[sk] = v;
	}

	void storekey(string k, double v) {
		wstring sk;
		s_utf8_to_unicode(sk, STR(k));
		KifContainerLock _lock(this);
		values[sk] = v;
	}

	void storekey(wstring k, double v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		prime_iterator<wstring, double> itx;
		KifPrimeMapUStringFloat* kval;
		for (itx = values; !itx.end(); itx++) {
			kval = new KifPrimeMapUStringFloat(kifcode, NULL);
			kval->values[itx.key()] = itx.value();
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifPrimeMapUStringFloat(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifPrimeMapUStringFloat) {}

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
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

	KifElement* EvaluationMap() {
		KifVectorInteger* kvect = new KifVectorInteger(kifcode, NULL);
		int t, c, n, m;
		values.occupation(t, c, n, m);
		kvect->values.push_back(t);
		kvect->values.push_back(c);
		kvect->values.push_back(n);
		kvect->values.push_back(m);
		return kvect;
	}

	KifElement* ValueOnKey(wstring k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnKey(string k) {
		wstring sk;
		s_utf8_to_unicode(sk, STR(k));
		KifContainerLock _lock(this);
		if (values.find(sk))
			return kifcode->Providefloat(values[sk]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Providefloat(values[buffer]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%g", k);
		KifContainerLock _lock(this);
		if (values.find(buffer))
			return kifcode->Providefloat(values[buffer]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(wstring k, bool& a) {
		KifContainerLock _lock(this);
		if (values.find(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFloat(wstring sk, bool& a) {
		KifContainerLock _lock(this);
		if (values.find(sk)) {
			a = true;
			return values[sk];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyInteger(string k, bool& a) {
		wstring sk;
		s_utf8_to_unicode(sk, STR(k));
		KifContainerLock _lock(this);
		if (values.find(sk)) {
			a = true;
			return values[sk];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFloat(string k, bool& a) {
		wstring sk;
		s_utf8_to_unicode(sk, STR(k));
		KifContainerLock _lock(this);
		if (values.find(sk)) {
			a = true;
			return values[sk];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyIInteger(long k, bool& a) {
		wchar_t buffer[20];
		swprintf_s(buffer, 20, L"%ld", k);
		KifContainerLock _lock(this);
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
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
		if (values.find(buffer)) {
			a = true;
			return values[buffer];
		}
		a = false;
		return 0;
	}

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	Exported KifType theType() {
		return kifPrimeMapUStringFloat;
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		prime_iterator<wstring, double> itx;
		for (itx = values; !itx.end(); itx++)
			v += itx.value();
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		prime_iterator<wstring, double> itx;
		for (itx = values; !itx.end(); itx++)
			v *= itx.value();
		return v;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		wstring i = s->UString();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapUStringFloat(kifcode, NULL);
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


	KifElement* Values(string n) {
		wstring sk;
		s_utf8_to_unicode(sk, STR(n));

		KifContainerLock _lock(this);
		if (values.find(sk) == false)
			return kifNOELEMENT;
		return kifTRUE;
	}

	KifElement* Values(wstring n) {
		KifContainerLock _lock(this);
		if (values.find(n) == false)
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc);

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

class KifIteratorPrimeMapUStringFloat : public KifIterationContainer {
public:

	prime_hash<wstring, double>* values;

	prime_iterator<wstring, double> it;

	KifIteratorPrimeMapUStringFloat(KifCode* klc, KifElement* kp, KifPrimeMapUStringFloat* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->Float());
	}


	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideustring(it.key());
	}

	wstring IteratorKeyUString() {

		if (it.end())
			return L"";

		return it.key();
	}


	string IteratorKeyString() {

		if (it.end())
			return "";

		string s;
		wstring ws = it.key();
		s_unicode_to_utf8(s, ws);
		return s;
	}

	long IteratorKeyInteger() {

		if (it.end())
			return -1;
		return convertinteger(it.key());
	}

	double IteratorKeyFloat() {

		if (it.end())
			return -1;
		return convertfloat(it.key());
	}

	long IteratorValueInteger() {

		if (it.end())
			return 0;
		return it.value();
	}


	double IteratorValueFloat() {

		if (it.end())
			return 0;
		return it.value();
	}

	wstring IteratorValueUString() {
		std::wstringstream os;
		os << it.value();
		return os.str();
	}


	string IteratorValueString() {
		stringstream os;
		os << it.value();
		return os.str();
	}

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.value());
	}

	void Next() {
		it++;
	}

	KifElement* End() {

		if (it.end())
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* Begin() {

		it = *values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}

};
//------------------------------------------------------------------------------------
class KifPrimeMapIntegerUString : public KifEvaluateMapValue {
public:
	prime_hash<long, wstring> values;

	void storekey(long k, string sv) {
		wstring v;
		s_utf8_to_unicode(v, STR(sv));
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void storekey(long k, wstring v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		prime_iterator<long, wstring> itx;
		KifPrimeMapIntegerUString* kval;
		for (itx = values; !itx.end(); itx++) {
			kval = new KifPrimeMapIntegerUString(kifcode, NULL);
			kval->values[itx.key()] = itx.value();
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifPrimeMapIntegerUString(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifPrimeMapIntegerUString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

	KifElement* EvaluationMap() {
		KifVectorInteger* kvect = new KifVectorInteger(kifcode, NULL);
		int t, c, n, m;
		values.occupation(t, c, n, m);
		kvect->values.push_back(t);
		kvect->values.push_back(c);
		kvect->values.push_back(n);
		kvect->values.push_back(m);
		return kvect;
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
		return kifPrimeMapIntegerUString;
	}

	KifElement* ValueOnKey(wstring n) {
		KifContainerLock _lock(this);
		long k = convertinteger(n);
		if (values.find(k))
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnKey(string n) {
		KifContainerLock _lock(this);
		long k = conversionintegerhexa(STR(n));
		if (values.find(k))
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	wstring ValueOnKeyString(wstring n, bool& a) {
		KifContainerLock _lock(this);
		long k = convertinteger(n);
		if (values.find(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return L"";
	}

	string ValueOnKeyString(string n, bool& a) {
		KifContainerLock _lock(this);
		long k = conversionintegerhexa(STR(n));
		if (values.find(k)) {
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
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapIntegerUString(kifcode, NULL);
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


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		long k = conversionintegerhexa(STR(n));
		if (values.find(k) == false)
			return kifNOELEMENT;
		return kifTRUE;
	}

	KifElement* Values(wstring n) {
		KifContainerLock _lock(this);
		long k = convertinteger(n);
		if (values.find(k) == false)
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc);

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

class KifIteratorPrimeMapIntegerUString : public KifIterationContainer {
public:

	prime_hash<long, wstring>* values;

	prime_iterator<long, wstring> it;


	KifIteratorPrimeMapIntegerUString(KifCode* klc, KifElement* kp, KifPrimeMapIntegerUString* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->UString());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.key());
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return it.key();
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return it.key();
	}

	wstring IteratorKeyUString();

	wstring IteratorValueUString() {

		if (it.end())
			return L"";
		return it.value();
	}


	string IteratorValueString() {

		if (it.end())
			return "";
		string s;
		wstring ws = it.value();
		s_unicode_to_utf8(s, ws);
		return s;
	}


	string IteratorKeyString();

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideustring(it.value());
	}

	void Next() {

		it++;
	}

	KifElement* End() {

		if (it.end())
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* Begin() {

		it = *values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}

};
//------------------------------------------------------------------------------------
class KifPrimeMapFloatUString : public KifEvaluateMapValue {
public:
	prime_hash<double, wstring> values;

	void storekey(double k, wstring v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void storekey(double k, string sv) {
		wstring v;
		s_utf8_to_unicode(v, STR(sv));
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		prime_iterator<double, wstring> itx;
		KifPrimeMapFloatUString* kval;
		for (itx = values; !itx.end(); itx++) {
			kval = new KifPrimeMapFloatUString(kifcode, NULL);
			kval->values[itx.key()] = itx.value();
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifPrimeMapFloatUString(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifPrimeMapFloatUString) {}

	bool Variable() {
		return true;
	}

	bool aValueMap() {
		return true;
	}

	void storeInLoop(KifElement** alls);
	KifElement* Getkeys(KifCallFunction* callfunc, KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc, KifElement* contextualpattern);
	bool Setseeds(ushort* p) {
		return values.setseeds(p);
	}

	KifElement* EvaluationMap() {
		KifVectorInteger* kvect = new KifVectorInteger(kifcode, NULL);
		int t, c, n, m;
		values.occupation(t, c, n, m);
		kvect->values.push_back(t);
		kvect->values.push_back(c);
		kvect->values.push_back(n);
		kvect->values.push_back(m);
		return kvect;
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
		return kifPrimeMapFloatUString;
	}

	KifElement* ValueOnKey(wstring n) {
		KifContainerLock _lock(this);
		double k = convertfloat(n);
		if (values.find(k))
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnKey(string n) {
		KifContainerLock _lock(this);
		double k = conversionintegerhexa(STR(n));
		if (values.find(k))
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k))
			return kifcode->Provideustring(values[k]);
		return kifNOELEMENT;
	}

	wstring ValueOnKeyString(wstring n, bool& a) {
		KifContainerLock _lock(this);
		double k = convertfloat(n);
		if (values.find(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return L"";
	}

	string ValueOnKeyString(string n, bool& a) {
		KifContainerLock _lock(this);
		double k = convertfloat(STR(n));
		if (values.find(k)) {
			a = true;
			s_unicode_to_utf8(n, values[k]);
			return n;
		}
		a = false;
		return "";
	}


	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		double i = s->Float();
		if (values.find(i))
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifPrimeMapFloatUString(kifcode, NULL);
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
		double k = convertfloat(n);
		if (values.find(k) == false)
			return kifNOELEMENT;
		return kifTRUE;
	}

	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		double k = convertfloat(STR(n));
		if (values.find(k) == false)
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc);

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

class KifIteratorPrimeMapFloatUString : public KifIterationContainer {
public:

	prime_hash<double, wstring>* values;

	prime_iterator<double, wstring> it;

	KifIteratorPrimeMapFloatUString(KifCode* klc, KifElement* kp, KifPrimeMapFloatUString* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {

		if (it.end())
			return;
		it.value(ke->UString());
	}

	KifElement* IteratorKey() {

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.key());
	}

	long IteratorKeyInteger() {
		if (it.end())
			return -1;
		return it.key();
	}

	double IteratorKeyFloat() {
		if (it.end())
			return -1;
		return it.key();
	}

	wstring IteratorKeyUString();

	wstring IteratorValueUString() {

		if (it.end())
			return L"";
		return it.value();

	}


	string IteratorValueString() {

		if (it.end())
			return "";
		string s;
		s_unicode_to_utf8(s, it.value());
		return s;

	}


	string IteratorKeyString();

	KifElement* IteratorValue() {

		if (it.end())
			return kifNULL;

		return kifcode->Provideustring(it.value());
	}

	void Next() {
		it++;
	}

	KifElement* End() {

		if (it.end())
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* Begin() {

		it = *values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}

};

//--------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

class KifIntegerBinMap : public KifEvaluateMap {
public:

	hash_bin<ushort, KifElement*> values;

	void Items(KifVector* kvect) {
		hash_bin<ushort, KifElement*>::iterator it;
		KifIntegerBinMap* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifIntegerBinMap(kifcode, NULL);
			kval->push(it->first, it->second);
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	Exported KifIntegerBinMap(KifCode* klc, KifElement* kp) : KifEvaluateMap(klc, kp, kifIntegerBinMap) {}

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
		return kifMap;
	}

	void push(long l, KifElement*);

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n = conversionintegerhexa(STR(k));
		if (values.check(n))
			return values[n];
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.check(k))
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.check(k))
			return values[k];
		return kifNOELEMENT;
	}

	KifElement* Index(KifElement* ke) {
		KifContainerLock _lock(this);
		long s = ke->Integer();
		if (!values.check(s))
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Newinstance() {
		return new KifIntegerBinMap(kifcode, NULL);
	}
	Exported string String();
	Exported string JSonString();
	Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string k) {
		KifContainerLock _lock(this);
		long n = conversionintegerhexa(STR(k));
		KifElement* e = values.search(n);
		if (e==NULL)
			return kifNOELEMENT;
		return e;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int, KifCallFunction*);
	Exported bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		hash_bin<ushort, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second->Sum();
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		hash_bin<ushort, KifElement*>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v *= itx->second->Product();
		return v;
	}

	Exported void Setreference(int inc = 1);

	Exported void Setpopped(int v = 1, bool force = false) {
		if (reference <= 0 || force) {
			Putpopped(v);
			hash_bin<ushort, KifElement*>::iterator itx;
			for (itx = values.begin(); itx != values.end(); itx++) {
				if (itx->second != NULL)
					itx->second->Setpopped(v, force);
			}
		}
	}

	Exported void Clean() {
		KifContainerLock _lock(this);
		reference--;
		hash_bin<ushort, KifElement*>::iterator itx;
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

class KifIteratorIntegerBinMap : public KifIterationContainer {
public:
	hash_bin<ushort, KifElement*>* values;
	hash_bin<ushort, KifElement*>::iterator it;

	KifIteratorIntegerBinMap(KifCode* klc, KifElement* kp, KifIntegerBinMap* k) : KifIterationContainer(klc, kp, k) {}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);

	Exported KifElement* IteratorKey() {
		if (it == values->end())
			return kifNOELEMENT;
		return kifcode->Provideinteger(it->first);
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

	string IteratorValueString() {
		return it->second->String();
	}

	Exported KifElement* IteratorValue() {
		if (it == values->end())
			return kifNOELEMENT;
		return it->second;
	}

	Exported void Next() {
		it++;
	}

	Exported KifElement* End() {
		if (it == values->end())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		it = values->begin();
		if (it == values->end())
			return kifFALSE;
		return kifTRUE;
	}
};
//---------------------------------------------------------------------------------------
class KifBinMapIntegerInteger : public KifEvaluateMapValue {
public:
	hash_bin<ushort, long> values;

	void storekey(long k, double v) {
		KifContainerLock _lock(this);
		values[k] = (long)v;
	}

	void storekey(double k, long v) {
		KifContainerLock _lock(this);
		values[(long)k] = v;
	}

	void storekey(double k, double v) {
		KifContainerLock _lock(this);
		values[(long)k] = (long)v;
	}

	void storekey(long k, long v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		hash_bin<ushort, long>::iterator it;
		KifBinMapIntegerInteger* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifBinMapIntegerInteger(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifBinMapIntegerInteger(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifBinMapIntegerInteger) {}

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

	Exported KifElement* Atom(bool forced = false) {
		if (reference == 0 && popped && !forced)
			return this;

		return Copy(NULL);
	}

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		hash_bin<ushort, long>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second;
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		hash_bin<ushort, long>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v *= itx->second;
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n = conversionintegerhexa(STR(k));
		if (values.check(n))
			return kifcode->Provideinteger(values[n]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.check(k))
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.check(k))
			return kifcode->Provideinteger(values[k]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(string k, bool& a) {
		KifContainerLock _lock(this);
		long n = conversionintegerhexa(STR(k));
		if (values.check(n)) {
			a = true;
			return values[n];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFloat(string k, bool& a) {
		KifContainerLock _lock(this);
		long n = conversionintegerhexa(STR(k));
		if (values.check(n)) {
			a = true;
			return values[n];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyIInteger(long k, bool& a) {
		KifContainerLock _lock(this);
		if (values.check(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyIFloat(long k, bool& a) {
		KifContainerLock _lock(this);
		if (values.check(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyFInteger(double k, bool& a) {
		KifContainerLock _lock(this);
		if (values.check(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFFloat(double k, bool& a) {
		KifContainerLock _lock(this);
		if (values.check(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	Exported KifType theType() {
		return kifBinMapIntegerInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i = s->Integer();
		if (!values.check(i))
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifBinMapIntegerInteger(kifcode, NULL);
	}

	string String();
	Exported void Push(string& key, KifElement* val);
	Exported void Push(char* key, KifElement* val);
	Exported void Push(KifElement* key, KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		long k = conversionintegerhexa(STR(n));
		if (!values.check(k))
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc);

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

class KifIteratorBinMapIntegerInteger : public KifIterationContainer {
public:

	hash_bin<ushort, long>* values;
	hash_bin<ushort, long>::iterator it;

	KifIteratorBinMapIntegerInteger(KifCode* klc, KifElement* kp, KifBinMapIntegerInteger* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (it == values->end())
			return;
		it->second = ke->Integer();
		return;
	}

	KifElement* IteratorKey() {
		if (it == values->end())
			return kifNULL;

		return kifcode->Provideinteger(it->first);
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
		if (it == values->end())
			return 0;
		return it->second;
	}

	double IteratorValueFloat() {
		if (it == values->end())
			return 0;
		return it->second;
	}

	string IteratorValueString() {
		stringstream os;
		os << it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
		if (it == values->end())
			return kifNULL;

		return kifcode->Provideinteger(it->second);
	}

	void Next() {
		it++;
	}

	KifElement* End() {
		if (it == values->end())
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* Begin() {
		it = values->begin();
		if (it == values->end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifBinMapIntegerFloat : public KifEvaluateMapValue {
public:
	hash_bin<ushort, double> values;

	void storekey(long k, long v) {
		KifContainerLock _lock(this);
		values[k] = (double)v;
	}

	void storekey(double k, long v) {
		KifContainerLock _lock(this);
		values[(long)k] = (double)v;
	}

	void storekey(double k, double v) {
		KifContainerLock _lock(this);
		values[(long)k] = v;
	}

	void storekey(long k, double v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		hash_bin<ushort, double>::iterator it;
		KifBinMapIntegerFloat* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifBinMapIntegerFloat(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifBinMapIntegerFloat(KifCode* klc, KifElement* kp) : KifEvaluateMapValue(klc, kp, kifBinMapIntegerFloat) {}

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

	Exported double Sum() {
		KifContainerLock _lock(this);
		double v = 0;
		hash_bin<ushort, double>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v += itx->second;
		return v;
	}

	Exported double Product() {
		KifContainerLock _lock(this);
		double v = 1;
		hash_bin<ushort, double>::iterator itx;
		for (itx = values.begin(); itx != values.end(); itx++)
			v *= itx->second;
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n = conversionintegerhexa(STR(k));
		if (values.check(n))
			return kifcode->Providefloat(values[n]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.check(k))
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.check(k))
			return kifcode->Providefloat(values[k]);
		return kifNOELEMENT;
	}

	long ValueOnKeyInteger(string k, bool& a) {
		KifContainerLock _lock(this);
		long n = conversionintegerhexa(STR(k));
		if (values.check(n)) {
			a = true;
			return values[n];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFloat(string k, bool& a) {
		KifContainerLock _lock(this);
		long n = conversionintegerhexa(STR(k));
		if (values.check(n)) {			
			a = true;
			return values[n];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyIInteger(long k, bool& a) {
		KifContainerLock _lock(this);
		if (values.check(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyIFloat(long k, bool& a) {
		KifContainerLock _lock(this);
		if (values.check(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	long ValueOnKeyFInteger(double k, bool& a) {
		KifContainerLock _lock(this);
		if (values.check(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	double ValueOnKeyFFloat(double k, bool& a) {
		KifContainerLock _lock(this);
		if (values.check(k)) {
			a = true;
			return values[k];
		}
		a = false;
		return 0;
	}

	Exported KifType theType() {
		return kifBinMapIntegerFloat;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		long i = s->Integer();
		if (!values.check(i))
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL);

	KifElement* Newinstance() {
		return new KifBinMapIntegerFloat(kifcode, NULL);
	}

	string String();
	Exported void Push(string& key, KifElement* val);
	Exported void Push(char* key, KifElement* val);
	Exported void Push(KifElement* key, KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		KifContainerLock _lock(this);
		long k = conversionintegerhexa(STR(n));
		if (!values.check(k))
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc);

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

class KifIteratorBinMapIntegerFloat : public KifIterationContainer {
public:

	hash_bin<ushort, double>* values;

	hash_bin<ushort, double>::iterator it;

	KifIteratorBinMapIntegerFloat(KifCode* klc, KifElement* kp, KifBinMapIntegerFloat* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (it == values->end())
			return;
		it->second = ke->Float();
		return;
	}

	KifElement* IteratorKey() {
		if (it == values->end())
			return kifNULL;

		return kifcode->Provideinteger(it->first);
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
		if (it == values->end())
			return 0;
		return it->second;
	}

	double IteratorValueFloat() {
		if (it == values->end())
			return 0;
		return it->second;
	}

	string IteratorValueString() {
		stringstream os;
		os << it->second;
		return os.str();
	}

	KifElement* IteratorValue() {
		if (it == values->end())
			return kifNULL;

		return kifcode->Providefloat(it->second);
	}

	void Next() {
		it++;
	}

	KifElement* End() {
		if (it == values->end())
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* Begin() {
		it = values->begin();
		if (it == values->end())
			return kifFALSE;
		return kifTRUE;
		return kifTRUE;
	}
};

class KifBinMapIntegerString : public KifEvaluateMapValue {
public:
	hash_bin<ushort, string> values;

	void storekey(long k, string v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		hash_bin<ushort, string>::iterator it;
		KifBinMapIntegerString* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifBinMapIntegerString(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifBinMapIntegerString(KifCode* klc, KifElement* kp) : values(false), KifEvaluateMapValue(klc, kp, kifBinMapIntegerString) {}

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
		return kifBinMapIntegerString;
	}

	KifElement* ValueOnKey(string n) {
		KifContainerLock _lock(this);
		long k = conversionintegerhexa(STR(n));
		if (values.find(k) != values.end())
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnIntegerKey(long k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	KifElement* ValueOnFloatKey(double k) {
		KifContainerLock _lock(this);
		if (values.find(k) != values.end())
			return kifcode->Providestring(values[k]);
		return kifNOELEMENT;
	}

	string ValueOnKeyString(string n, bool& a) {
		KifContainerLock _lock(this);
		long k = conversionintegerhexa(STR(n));
		if (values.find(k) != values.end()) {
			a = true;
			return values[k];
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
		return new KifBinMapIntegerString(kifcode, NULL);
	}

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


	long Size() {
		KifContainerLock _lock(this);
		return  values.size();
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc);

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

class KifIteratorBinMapIntegerString : public KifIterationContainer {
public:

	hash_bin<ushort, string>* values;
	hash_bin<ushort, string>::iterator it;

	KifIteratorBinMapIntegerString(KifCode* klc, KifElement* kp, KifBinMapIntegerString* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		if (it == values->end())
			return;
		it->second = ke->String();
		return;
	}

	KifElement* IteratorKey() {
		if (it == values->end())
			return kifNULL;

		return kifcode->Provideinteger(it->first);
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

	string IteratorValueString() {
		if (it == values->end())
			return "";
		return it->second;
	}

	KifElement* IteratorValue() {
		if (it == values->end())
			return kifNULL;

		return kifcode->Providestringraw(it->second);
	}

	void Next() {
		it++;
	}

	KifElement* End() {
		if (it == values->end())
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* Begin() {
		it = values->begin();
		if (it == values->end())
			return kifFALSE;
		return kifTRUE;
	}

};

//----------------------------------------------------------------------------------------
class KifBinMapIntegerUString : public KifEvaluateMapValue {
public:
	hash_bin<ushort, wstring> values;

	void storekey(long k, wstring v) {
		KifContainerLock _lock(this);
		values[k] = v;
	}

	void Items(KifVector* kvect) {
		hash_bin<ushort, wstring>::iterator it;
		KifBinMapIntegerUString* kval;
		for (it = values.begin(); it != values.end(); it++) {
			kval = new KifBinMapIntegerUString(kifcode, NULL);
			kval->values[it->first] = it->second;
			kval->Setreference(kvect->reference + 1);
			kvect->values.push_back(kval);
		}
	}

	KifBinMapIntegerUString(KifCode* klc, KifElement* kp) : values(false), KifEvaluateMapValue(klc, kp, kifBinMapIntegerUString) {}

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
		return kifBinMapIntegerUString;
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
		return new KifBinMapIntegerUString(kifcode, NULL);
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

class KifIteratorBinMapIntegerUString : public KifIterationContainer {
public:

	hash_bin<ushort, wstring>* values;

	hash_bin<ushort, wstring>::iterator it;
	hash_bin<ushort, wstring>::iterator itr;


	KifIteratorBinMapIntegerUString(KifCode* klc, KifElement* kp, KifBinMapIntegerUString* k) : KifIterationContainer(klc, kp, k) {}

	Exported void IteratorSetvalue(KifElement* ke) {
			if (it == values->end())
				return;
			it->second = ke->UString();
	}

	KifElement* IteratorKey() {
			if (it == values->end())
				return kifNULL;

			return kifcode->Provideinteger(it->first);
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
		if (it == values->end())
			return -1;
		wstring wx(it->second);
		sc_unicode_to_utf8(x, wx);
		return conversionintegerhexa(STR(x));
	}

	double IteratorValueFloat() {
		string x;
		if (it == values->end())
			return -1;
		wstring wx(it->second);
		sc_unicode_to_utf8(x, wx);
		return convertfloat(STR(x));
	}

	string IteratorValueString() {
		wstring ws;
		string s;
		if (it == values->end())
			return "";
		ws = it->second;
		s_unicode_to_utf8(s, ws);
		return s;
	}

	wstring IteratorValueUString() {
		if (it == values->end())
			return L"";
		return it->second;
	}

	KifElement* IteratorValue() {
		if (it == values->end())
			return kifNULL;

		return kifcode->Provideustring(it->second);
	}

	void Next() {
		it++;
	}

	KifElement* End() {
		if (it == values->end())
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* Begin() {
		it = values->begin();
		if (it == values->end())
			return kifFALSE;
		return kifTRUE;
	}

};
//-----------------------------------------------------------------------------------

#endif

