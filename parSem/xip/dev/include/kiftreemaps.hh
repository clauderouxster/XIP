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

class KifIntegerTreeMap : public KifEvaluate {
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

	Exported KifIntegerTreeMap(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifIntegerTreeMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);
	Exported KifElement* Map(KifElement*,KifMap* itself=NULL) {
		return this;
	}

	void storeInLoop(vector<KifElement*>& alls);
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
		map<long,KifElement*>::iterator iter=values.find(n);
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

class KifFloatTreeMap : public KifEvaluate {
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

	Exported KifFloatTreeMap(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifFloatTreeMap) {}

	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	Exported KifElement* Vector(KifElement*,KifVector* itself=NULL);
	Exported KifElement* Map(KifElement*,KifMap* itself=NULL) {
		return this;
	}
	void storeInLoop(vector<KifElement*>& alls);
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
		return kifcode->Providemap();
	}
	Exported string String();
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

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* idx=kifNULL,bool rcouple=false);
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

class KifIteratorIntegerTreeMap : public KifIteration {
public:
	map<long,KifElement*>* values;
	map<long,KifElement*>::iterator it;
	map<long,KifElement*>::reverse_iterator itr;

	KifIntegerTreeMap* kbase;

	KifIteratorIntegerTreeMap(KifCode* klc,KifElement* kp,KifIntegerTreeMap* k) : kbase(k),KifIteration(klc,kp) {}

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* val=kifNULL,bool rcouple=false);
	Exported KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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

class KifIteratorFloatTreeMap : public KifIteration {
public:
	map<double,KifElement*>* values;
	map<double,KifElement*>::iterator it;
	map<double,KifElement*>::reverse_iterator itr;

	KifFloatTreeMap* kbase;

	KifIteratorFloatTreeMap(KifCode* klc,KifElement* kp,KifFloatTreeMap* k) : kbase(k),KifIteration(klc,kp) {}

	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* val=kifNULL,bool rcouple=false);
	Exported KifElement* IteratorKey() {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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

class KifTreeMapString : public KifEvaluate {
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
		KifTreeMapString* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapString(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapString(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifTreeMapString) {}

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
		return kifTreeMapString;
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
		return new KifTreeMapString(kifcode,NULL);
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

class KifIteratorTreeMapString : public KifIteration {
public:

	map<string,string>* values;

	map<string,string>::iterator it;
	map<string,string>::reverse_iterator itr;
    
	KifTreeMapString* kbase;
	KifIteratorTreeMapString(KifCode* klc,KifElement* kp,KifTreeMapString* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return s_XLConvert(STR(it->first));
		}

		if (itr==values->rend())
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

		if (itr==values->rend())
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
		if (itr==values->rend())
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
		if (itr==values->rend())
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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

class KifTreeMapInteger : public KifEvaluate {
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
		KifTreeMapInteger* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapInteger(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapInteger(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifTreeMapInteger) {}

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
		return kifTreeMapInteger;
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
		return new KifTreeMapInteger(kifcode,NULL);
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

class KifIteratorTreeMapInteger : public KifIteration {
public:

	map<string,long>* values;

	map<string,long>::iterator it;
	map<string,long>::reverse_iterator itr;

	KifTreeMapInteger* kbase;
	KifIteratorTreeMapInteger(KifCode* klc,KifElement* kp,KifTreeMapInteger* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return s_XLConvert(STR(it->first));
		}

		if (itr==values->rend())
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

		if (itr==values->rend())
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
		if (itr==values->rend())
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
		if (itr==values->rend())
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
		if (itr==values->rend())
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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


class KifTreeMapFloat : public KifEvaluate {
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
		KifTreeMapFloat* kval;
		for (it=values.begin();it!=values.end();it++) {
			kval=new KifTreeMapFloat(kifcode,NULL);
			kval->values[it->first]=it->second;
			kval->Setreference(kvect->reference+1);
			kvect->values.push_back(kval);
		}		
	}

	KifTreeMapFloat(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifTreeMapFloat) {}

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
		return kifTreeMapFloat;
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
		return new KifTreeMapFloat(kifcode,NULL);
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

class KifIteratorTreeMapFloat : public KifIteration {
public:

	map<string,double>* values;

	map<string,double>::iterator it;
	map<string,double>::reverse_iterator itr;

	KifTreeMapFloat* kbase;
	KifIteratorTreeMapFloat(KifCode* klc,KifElement* kp,KifTreeMapFloat* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
		if (reverse==false) {
			if (it==values->end())
				return -1;
			return s_XLConvert(STR(it->first));
		}

		if (itr==values->rend())
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

		if (itr==values->rend())
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
		if (itr==values->rend())
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
		if (itr==values->rend())
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
		if (itr==values->rend())
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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


class KifTreeMapIntegerInteger : public KifEvaluate {
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

	KifTreeMapIntegerInteger(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifTreeMapIntegerInteger) {}

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

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

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

class KifIteratorTreeMapIntegerInteger : public KifIteration {
public:

	map<long,long>* values;

	map<long,long>::iterator it;
	map<long,long>::reverse_iterator itr;

	KifTreeMapIntegerInteger* kbase;
	KifIteratorTreeMapIntegerInteger(KifCode* klc,KifElement* kp,KifTreeMapIntegerInteger* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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

class KifTreeMapIntegerFloat : public KifEvaluate {
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

	KifTreeMapIntegerFloat(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifTreeMapIntegerFloat) {}

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

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

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

class KifIteratorTreeMapIntegerFloat : public KifIteration {
public:

	map<long,double>* values;

	map<long,double>::iterator it;
	map<long,double>::reverse_iterator itr;

	KifTreeMapIntegerFloat* kbase;
	KifIteratorTreeMapIntegerFloat(KifCode* klc,KifElement* kp,KifTreeMapIntegerFloat* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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

class KifTreeMapIntegerString : public KifEvaluate {
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

	KifTreeMapIntegerString(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifTreeMapIntegerString) {}

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
		return kifTreeMapIntegerString;
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
		return new KifTreeMapIntegerString(kifcode,NULL);
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

class KifIteratorTreeMapIntegerString : public KifIteration {
public:

	map<long,string>* values;

	map<long,string>::iterator it;
	map<long,string>::reverse_iterator itr;
    
	KifTreeMapIntegerString* kbase;
	KifIteratorTreeMapIntegerString(KifCode* klc,KifElement* kp,KifTreeMapIntegerString* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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


class KifTreeMapFloatInteger : public KifEvaluate {
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

	KifTreeMapFloatInteger(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifTreeMapFloatInteger) {}

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

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

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

class KifIteratorTreeMapFloatInteger : public KifIteration {
public:

	map<double,long>* values;

	map<double,long>::iterator it;
	map<double,long>::reverse_iterator itr;

	KifTreeMapFloatInteger* kbase;
	KifIteratorTreeMapFloatInteger(KifCode* klc,KifElement* kp,KifTreeMapFloatInteger* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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

class KifTreeMapFloatFloat : public KifEvaluate {
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

	KifTreeMapFloatFloat(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifTreeMapFloatFloat) {}

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

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

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

class KifIteratorTreeMapFloatFloat : public KifIteration {
public:

	map<double,double>* values;

	map<double,double>::iterator it;
	map<double,double>::reverse_iterator itr;

	KifTreeMapFloatFloat* kbase;
	KifIteratorTreeMapFloatFloat(KifCode* klc,KifElement* kp,KifTreeMapFloatFloat* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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

class KifTreeMapFloatString : public KifEvaluate {
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

	KifTreeMapFloatString(KifCode* klc,KifElement* kp) : KifEvaluate(klc,kp,kifTreeMapFloatString) {}

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

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	KifElement* Newinstance() {
		return new KifTreeMapFloatString(kifcode,NULL);
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

class KifIteratorTreeMapFloatString : public KifIteration {
public:

	map<double,string>* values;

	map<double,string>::iterator it;
	map<double,string>::reverse_iterator itr;

    
	KifTreeMapFloatString* kbase;
	KifIteratorTreeMapFloatString(KifCode* klc,KifElement* kp,KifTreeMapFloatString* k) : kbase(k),KifIteration(klc,kp) {}

	Exported void IteratorSetvalue(KifElement* ke) {
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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
		KifContainerLock _lock(kbase);
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

#endif

