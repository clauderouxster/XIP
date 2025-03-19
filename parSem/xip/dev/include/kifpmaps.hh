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

class KifPrimeMap : public KifEvaluate {
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

	Exported KifPrimeMap(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifPrimeMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);

	bool Setseeds(unsigned short* p) {
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
		return kifcode->Providemap();
	}
	Exported string String();
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

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* idx=kifNULL,bool rcouple=false);
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

	Exported bool Boolean() {
		KifContainerLock _lock(this);
		if (values.size()==0)
			return false;
		return true;
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

class KifIteratorPrimeMap : public KifIteration {
public:
	prime_hash<string,KifElement*>* values;
	prime_iterator<string,KifElement*> it;
	KifPrimeMap* kbase;

	KifIteratorPrimeMap(KifCode* klc,KifElement* kp,KifPrimeMap* k) : kbase(k),KifIteration(klc,kp) {}

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* val=kifNULL,bool rcouple=false);

	Exported KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
		if (it.end())
			return kifNOELEMENT;
		return kifcode->Providestringraw(it.key());		
	}

	long IteratorKeyInteger() {
		KifContainerLock _lock(kbase);
		if (it.end())
			return -1;
		return s_XLConvert(STR(it.key()));
	}

	double IteratorKeyFloat() {
		KifContainerLock _lock(kbase);
		if (it.end())
			return -1;
		return convertfloat(STR(it.key()));
	}

	string IteratorKeyString() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return "";
		return it.key();
	}

	Exported KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);
		if (it.end())
			return kifNOELEMENT;
		return it.value();
	}

	Exported void Next() {
		it++;
	}

	Exported KifElement* End() {
		KifContainerLock _lock(kbase);
		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	Exported KifElement* Begin() {	
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}	
};


class KifIntegerPrimeMap : public KifEvaluate {
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

	Exported KifIntegerPrimeMap(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifIntegerPrimeMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(unsigned short* p) {
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
		long n=s_XLConvert(STR(k));
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
		return kifcode->Providemap();
	}
	Exported string String();
    Exported string BaseString();

	Exported KifElement* Newiterator(bool);

	KifElement* Values(string k) {
        KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
		if (values.find(n))
			return values[n];
		return kifNOELEMENT;
	}

	void Merge(KifElement* ke);

	void Pop(KifElement* ke);

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* idx=kifNULL,bool rcouple=false);
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

class KifFloatPrimeMap : public KifEvaluate {
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

	Exported KifFloatPrimeMap(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifFloatPrimeMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);
	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(unsigned short* p) {
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

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

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
		return kifcode->Providemap();
	}
	Exported string String();
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

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* idx=kifNULL,bool rcouple=false);
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

class KifIteratorIntegerPrimeMap : public KifIteration {
public:
	prime_hash<long,KifElement*>* values;
	prime_iterator<long,KifElement*> it;

	KifIntegerPrimeMap* kbase;

	KifIteratorIntegerPrimeMap(KifCode* klc,KifElement* kp,KifIntegerPrimeMap* k) : kbase(k),KifIteration(klc,kp) {}

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* val=kifNULL,bool rcouple=false);
	Exported KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);

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
		KifContainerLock _lock(kbase);
		return it.value()->String();
	}

	Exported KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNOELEMENT;
		return it.value();
	}

	Exported void Next() {
		it++;
	}

	Exported KifElement* End() {
		KifContainerLock _lock(kbase);		
		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	Exported KifElement* Begin() {	
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;	
	}
};

class KifIteratorFloatPrimeMap : public KifIteration {
public:
	prime_hash<double,KifElement*>* values;
	prime_iterator<double,KifElement*> it;

	KifFloatPrimeMap* kbase;

	KifIteratorFloatPrimeMap(KifCode* klc,KifElement* kp,KifFloatPrimeMap* k) : kbase(k),KifIteration(klc,kp) {}

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* val=kifNULL,bool rcouple=false);

	Exported KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
		return it.value()->String();
	}

	Exported KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNOELEMENT;
		return it.value();
	}

	Exported void Next() {	
		it++;
	}

	Exported KifElement* End() {
		KifContainerLock _lock(kbase);
		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	Exported KifElement* Begin() {	
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapString : public KifEvaluate {
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
		KifPrimeMapString* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapString(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapString(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifPrimeMapString) {}

	bool Variable() {
		return true;
	}

	bool aValuePrimeMap() {
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
		return kifPrimeMapString;
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

	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(unsigned short* p) {
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
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifPrimeMapString(kifcode,NULL);
	}

	string String();
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

class KifIteratorPrimeMapString : public KifIteration {
public:

	prime_hash<string,string>* values;
	prime_iterator<string,string> it;

	KifPrimeMapString* kbase;
	KifIteratorPrimeMapString(KifCode* klc,KifElement* kp,KifPrimeMapString* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);

		if (it.end())
			return;
		it.value(ke->String());
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.key());
	}

	long IteratorKeyInteger() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return -1;
		return s_XLConvert(STR(it.key()));
	}

	double IteratorKeyFloat() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return -1;
		return convertfloat(STR(it.key()));
	}

	string IteratorKeyString() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return "";

		return it.key();
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return "";
		return it.value();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;		
		return kifTRUE;
	}

};

class KifPrimeMapInteger : public KifEvaluate {
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
		KifPrimeMapInteger* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapInteger(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapInteger(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifPrimeMapInteger) {}

	bool Variable() {
		return true;
	}

	bool aValuePrimeMap() {
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
	bool Setseeds(unsigned short* p) {
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
			v+=itx.value();
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
		return kifPrimeMapInteger;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		string i=s->String();
		if (values.find(i))
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifPrimeMapInteger(kifcode,NULL);
	}

	string String();
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

class KifIteratorPrimeMapInteger : public KifIteration {
public:

	prime_hash<string,long>* values;

	prime_iterator<string,long> it;

	KifPrimeMapInteger* kbase;
	KifIteratorPrimeMapInteger(KifCode* klc,KifElement* kp,KifPrimeMapInteger* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);

		if (it.end())
			return;
		it.value(ke->Integer());
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.key());
	}

	long IteratorKeyInteger() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return -1;
		return s_XLConvert(STR(it.key()));
	}

	double IteratorKeyFloat() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return -1;
		return convertfloat(STR(it.key()));
	}

	string IteratorKeyString() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return "";

		return it.key();
	}

	long IteratorValueInteger() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.value());
	}
	void Next() {		
		it++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);
		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapFloat : public KifEvaluate {
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
		KifPrimeMapFloat* kval;
		for (itx=values;!itx.end();itx++) {
			kval=new KifPrimeMapFloat(kifcode,NULL);
			kval->values[itx.key()]=itx.value();
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifPrimeMapFloat(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifPrimeMapFloat) {}

	bool Variable() {
		return true;
	}

	bool aValuePrimeMap() {
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
	bool Setseeds(unsigned short* p) {
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
		return kifPrimeMapFloat;
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
			v+=itx.value();
		return v;
	}

	KifElement* Index(KifElement* s) {
		KifContainerLock _lock(this);
		string i=s->String();
		if (values.find(i))
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifPrimeMapFloat(kifcode,NULL);
	}

	string String();
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

class KifIteratorPrimeMapFloat : public KifIteration {
public:

	prime_hash<string,double>* values;

	prime_iterator<string,double> it;

	KifPrimeMapFloat* kbase;
	KifIteratorPrimeMapFloat(KifCode* klc,KifElement* kp,KifPrimeMapFloat* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);

		if (it.end())
			return;
		it.value(ke->Float());
	}


	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.key());
	}

	string IteratorKeyString() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return "";

		return it.key();
	}

	long IteratorKeyInteger() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return -1;
		return s_XLConvert(STR(it.key()));
	}

	double IteratorKeyFloat() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return -1;
		return convertfloat(STR(it.key()));
	}

	long IteratorValueInteger() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}


	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}

};

class KifPrimeMapIntegerInteger : public KifEvaluate {
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

	KifPrimeMapIntegerInteger(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifPrimeMapIntegerInteger) {}

	bool Variable() {
		return true;
	}

	bool aValuePrimeMap() {
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
	bool Setseeds(unsigned short* p) {
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
			v+=itx.value();
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
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
		long n=s_XLConvert(STR(k));
		if (values.find(n)) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
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
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

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
		long k=s_XLConvert(STR(n));
		if (values.find(k)==false)
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

class KifIteratorPrimeMapIntegerInteger : public KifIteration {
public:

	prime_hash<long,long>* values;

	prime_iterator<long,long> it;

	KifPrimeMapIntegerInteger* kbase;
	KifIteratorPrimeMapIntegerInteger(KifCode* klc,KifElement* kp,KifPrimeMapIntegerInteger* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);

		if (it.end())
			return;
		it.value(ke->Integer());
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);

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
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.value());
	}

	void Next() {
		it++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapIntegerFloat : public KifEvaluate {
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


	KifPrimeMapIntegerFloat(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifPrimeMapIntegerFloat) {}

	bool Variable() {
		return true;
	}

	bool aValuePrimeMap() {
		return true;
	}

	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(unsigned short* p) {
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
			v+=itx.value();
		return v;
	}

	KifElement* ValueOnKey(string k) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
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
		long n=s_XLConvert(STR(k));
		if (values.find(n)) {
			a=true;
			return values[n];
		}
		a=false;
		return 0;
	}

	double ValueOnKeyFloat(string k,bool& a) {
		KifContainerLock _lock(this);
		long n=s_XLConvert(STR(k));
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
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

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
		long k=s_XLConvert(STR(n));
		if (values.find(k)==false)
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

class KifIteratorPrimeMapIntegerFloat : public KifIteration {
public:

	prime_hash<long,double>* values;

	prime_iterator<long,double> it;

	KifPrimeMapIntegerFloat* kbase;
	KifIteratorPrimeMapIntegerFloat(KifCode* klc,KifElement* kp,KifPrimeMapIntegerFloat* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);

		if (it.end())
			return;
		it.value(ke->Float());
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);

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
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapIntegerString : public KifEvaluate {
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

	KifPrimeMapIntegerString(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifPrimeMapIntegerString) {}

	bool Variable() {
		return true;
	}

	bool aValuePrimeMap() {
		return true;
	}

	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(unsigned short* p) {
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
		long k=s_XLConvert(STR(n));
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
		long k=s_XLConvert(STR(n));
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
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifPrimeMapIntegerString(kifcode,NULL);
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
		if (values.find(k)==false)
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

class KifIteratorPrimeMapIntegerString : public KifIteration {
public:

	prime_hash<long,string>* values;

	prime_iterator<long,string> it;


	KifPrimeMapIntegerString* kbase;
	KifIteratorPrimeMapIntegerString(KifCode* klc,KifElement* kp,KifPrimeMapIntegerString* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);

		if (it.end())
			return;
		it.value(ke->String());
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);

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
		KifContainerLock _lock(kbase);

		if (it.end())
			return "";
		return it.value();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.value());
	}

	void Next() {

		it++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;		
		return kifTRUE;
	}

};

class KifPrimeMapFloatInteger : public KifEvaluate {
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

	KifPrimeMapFloatInteger(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifPrimeMapFloatInteger) {}

	bool Variable() {
		return true;
	}

	bool aValuePrimeMap() {
		return true;
	}

	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(unsigned short* p) {
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
			v+=itx.value();
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
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

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

class KifIteratorPrimeMapFloatInteger : public KifIteration {
public:

	prime_hash<double,long>* values;

	prime_iterator<double,long> it;

	KifPrimeMapFloatInteger* kbase;
	KifIteratorPrimeMapFloatInteger(KifCode* klc,KifElement* kp,KifPrimeMapFloatInteger* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);

		if (it.end())
			return;
		it.value(ke->Integer());
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);

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
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Provideinteger(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapFloatFloat : public KifEvaluate {
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

	KifPrimeMapFloatFloat(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifPrimeMapFloatFloat) {}

	bool Variable() {
		return true;
	}

	bool aValuePrimeMap() {
		return true;
	}

	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(unsigned short* p) {
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
			v+=itx.value();
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
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

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

class KifIteratorPrimeMapFloatFloat : public KifIteration {
public:

	prime_hash<double,double>* values;

	prime_iterator<double,double> it;

	KifPrimeMapFloatFloat* kbase;
	KifIteratorPrimeMapFloatFloat(KifCode* klc,KifElement* kp,KifPrimeMapFloatFloat* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);

		if (it.end())
			return;
		it.value(ke->Float());
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);

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
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	double IteratorValueFloat() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return 0;
		return it.value();
	}

	string IteratorValueString() {
		KifContainerLock _lock(kbase);
		stringstream os;
		os<<it.value();
		return os.str();
	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Providefloat(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifPrimeMapFloatString : public KifEvaluate {
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

	KifPrimeMapFloatString(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifPrimeMapFloatString) {}

	bool Variable() {
		return true;
	}

	bool aValuePrimeMap() {
		return true;
	}

	void storeInLoop(vector<KifElement*>& alls);
	KifElement* Getkeys(KifCallFunction* callfunc,KifElement* contextualpattern);
	KifElement* Getvalues(KifCallFunction* callfunc,KifElement* contextualpattern);
	bool Setseeds(unsigned short* p) {
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
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifPrimeMapFloatString(kifcode,NULL);
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

class KifIteratorPrimeMapFloatString : public KifIteration {
public:

	prime_hash<double,string>* values;

	prime_iterator<double,string> it;

	KifPrimeMapFloatString* kbase;
	KifIteratorPrimeMapFloatString(KifCode* klc,KifElement* kp,KifPrimeMapFloatString* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);

		if (it.end())
			return;
		it.value(ke->String());
	}

	KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);

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
		KifContainerLock _lock(kbase);

		if (it.end())
			return "";
		return it.value();

	}

	KifElement* IteratorValue() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifNULL;

		return kifcode->Providestringraw(it.value());
	}

	void Next() {		
		it++;
	}

	KifElement* End() {
		KifContainerLock _lock(kbase);

		if (it.end())
			return kifTRUE;		
		return kifFALSE;
	}

	KifElement* Begin() {
		KifContainerLock _lock(kbase);

		it=*values;
		if (it.end())
			return kifFALSE;		
		return kifTRUE;
	}

};

#endif

