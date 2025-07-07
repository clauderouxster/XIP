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
   filename   : xipkif.h
   Date       : 09/09/2010
   Purpose    : Specific implementation of the KiF language to account from XIP specific objects 
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef i_xipkif
#define i_xipkif



class KifStringPtr : public KifElement {
public:
	string* valueptr;
	KifStringPtr(KifCode* klc,KifElement* kp,string* v) : KifElement(klc,kp,kifStringPtr) {
		valueptr=v;
	}
	Exported KifElement* Atom() {
		return kifcode->Providestring(*valueptr);
	}
	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		return kifcode->Providestring(*valueptr);
	}
	
	Exported void Clean() {
		reference--;
		*valueptr="";
		if (reference<=0) {
			ThreadLock _lock(type);
			if (idgarbage!=-1 && idgarbage<kifGlobals->cursorstring)
				kifGlobals->cursorstring=idgarbage;
		}
	}

	Exported void Clear() {
		*valueptr="";
	}

    char inCode() {
        return ckifString;
    }

	bool Basic() {
		return true;
	}

	bool Simple() {
		return true;
	}	

	Exported KifElement* Newiterator(bool);
	
	Exported void Resetreference() {}
	Exported void Setreference() {}

	Exported KifElement* Newinstance() {
		return this;
	}

	bool isString() {
		return true;
	}

	void Pop(KifElement* ke) {
		ThreadLock _lock(type);
		if (valueptr->size()==0)
			return;
		int i=ke->Integer();
		if (i==-1)
			i=valueptr->size()-1;
		else
			if (i>=valueptr->size() || i<0)
				return;

		c_char_index_remove(*valueptr,i);
	}


	KifElement* Last() {		
		if (valueptr->size()==0)
			return kifNULL;
		int i=valueptr->size()-1;
		string res=c_char_index(*valueptr,i);
		return kifcode->Providestring(res);
	}


	Exported bool Setvalue(KifElement* ke,KifElement* idx=NULL,KifElement* dom=NULL);

	Exported long Size();

	bool Predefinedtype() {return true;}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc);	
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke) {
		threadblocinit;
		return Exec(contextualpattern, dom, ke, idthread, callfunc);
	}

	Exported string String() {
		return *valueptr;
	}
	Exported long Integer();
	Exported double Float();
	Exported bool Boolean() {
		if (*valueptr=="" || *valueptr=="false")
			return false;
		return true;
	}


	Exported KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* contextualpattern,bool idx);

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);


	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		if (autoself) {
			*valueptr+=b->String();
			return this;
		}
		string s=a->String()+b->String();
		KifElement* kv= kifcode->Providestring(s);

		return kv;
	}

	Exported KifElement* less(KifElement* a) {
		if (*valueptr<a->String())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* more(KifElement* a) {
		if (*valueptr>a->String())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* same(KifElement* a) {
		if (*valueptr==a->String())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* different(KifElement* a) {
		if (*valueptr!=a->String())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* lessequal(KifElement* a) {
		if (*valueptr<=a->String())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* moreequal(KifElement* a) {
		if (*valueptr>=a->String())
			return kifTRUE;
		return kifFALSE;
	}
	
	Exported KifElement* Vector(KifElement*,KifVector* v=NULL);
	Exported KifElement* Map(KifElement*,KifMap* m=NULL);
};

class KifFloatPtr : public KifElement {
public:
	double* valueptr;
	char* updated;
	KifFloatPtr(KifCode* klc,KifElement* kp,double* v,char* b) : KifElement(klc,kp,kifFloat) {
		valueptr=v;
		updated=b;
	}
	
	double theValue() {
		return *valueptr;
	}

	void inValue(double v) {
		*valueptr=v;
	}

	Exported KifElement* Atom() {
		return kifcode->Providefloat(*valueptr);
	}
	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		return kifcode->Providefloat(*valueptr);
	}
	
    char inCode() {
        return ckifFloat;
    }

	Exported KifFraction* Fraction();

	Exported void Resetreference() {}
	Exported void Setreference() {}

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	bool Simple() {
		return true;
	}	
	bool Basic() {
		return true;
	}

	Exported KifElement* Newinstance() {
		return this;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke) {
		return this;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc) {
		return this;
	}


	bool Predefinedtype() {return true;}

	Exported long Size() {
		return *valueptr;
	}
	Exported bool Setvalue(KifElement* ke,KifElement* index=NULL,KifElement* dom=NULL);
	Exported string String();
	Exported long Integer() {
		return *valueptr;
	}
	Exported double Float() {
		return *valueptr;
	}
	Exported bool Boolean() {
		if (*valueptr==0)
			return false;
		return true;
	}


	Exported KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* contextualpattern,bool idx) {
		if (kfunc==NULL) {
			if (b->type==kifFloat)
				return b->same(a);
			return b->in(a,b,NULL,contextualpattern,true);
		}
		else
			return applyfunc(a,b,kfunc);
	}


	Exported KifElement* andset(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		if (autoself) {
			*valueptr=(long)*valueptr & b->Integer();
			return this;
		}
		long i=a->Integer() & b->Integer();
		KifElement* kv= kifcode->Provideinteger(i);
		return kv;

	}

	Exported KifElement* orset(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		if (autoself) {
			*valueptr=(long)*valueptr | b->Integer();
			return this;
		}
		long i=a->Integer() | b->Integer();
		KifElement* kv= kifcode->Provideinteger(i);
		return kv;

	}

	Exported KifElement* xorset(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		if (autoself) {
			*valueptr=(long)*valueptr^b->Integer();
			return this;
		}
		long i=a->Integer()^b->Integer();
		KifElement* kv= kifcode->Provideinteger(i);
		return kv;

	}

	Exported KifElement* plusplus() {
		ThreadLock _lock(type);
		++*valueptr;
		return this;
	}

	Exported KifElement* minusminus() {
		ThreadLock _lock(type);
		--*valueptr;
		return this;
	}

	Exported KifElement* plus(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		if (autoself) {
			*valueptr+=b->Float();
			return this;
		}
		double i=a->Float()+b->Float();
		KifElement* kv= kifcode->Providefloat(i);
		return kv;

	}
	Exported KifElement* minus(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		if (autoself) {
			*valueptr-=b->Float();
			return this;
		}
		double i=a->Float()-b->Float();
		KifElement* kv= kifcode->Providefloat(i);

		return kv;
	}
	Exported KifElement* multiply(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		if (autoself) {
			*valueptr*=b->Float();
			return this;
		}
		double i=a->Float()*b->Float();
		KifElement* kv= kifcode->Providefloat(i);

		return kv;
	}
	Exported KifElement* divide(KifElement* a,KifElement* b,KifElement* context,bool autoself);

	Exported KifElement* mod(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		long ib=b->Integer();
		if (ib==0)
			return kifNULL;

		if (autoself) {
			*valueptr=(long)*valueptr % ib;
			return this;
		}
		double i=a->Integer()%ib;
		KifElement* kv= kifcode->Providefloat(i);
		return kv;
	}

	Exported KifElement* shiftleft(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		if (autoself) {
			*valueptr=(long)*valueptr << b->Integer();
			return this;
		}
		double i=a->Integer()<<b->Integer();
		KifElement* kv= kifcode->Providefloat(i);

		return kv;
	}
	Exported KifElement* shiftright(KifElement* a,KifElement* b,KifElement* context,bool autoself) {
		if (autoself) {
			*valueptr=(long)*valueptr >> b->Integer();
			return this;
		}
		double i=a->Integer()>>b->Integer();
		KifElement* kv= kifcode->Providefloat(i);

		return kv;
	}
	Exported KifElement* power(KifElement* a,KifElement* b,KifElement* context,bool autoself);

	Exported KifElement* less(KifElement* a) {
		if (*valueptr<a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* more(KifElement* a) {
		if (*valueptr>a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* same(KifElement* a) {
		if (*valueptr==a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* different(KifElement* a) {
		if (*valueptr!=a->Float())
			return kifTRUE;
		return kifFALSE;
	}

	Exported KifElement* lessequal(KifElement* a) {
		if (*valueptr<=a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	Exported KifElement* moreequal(KifElement* a) {
		if (*valueptr>=a->Float())
			return kifTRUE;
		return kifFALSE;
	}
};

class KifVectorNumberPtr : public KifElement {
public:
	VECTA<double>* values;
	int it;

	KifVectorNumberPtr(KifCode* klc,KifElement* kp,VECTA<double>* v) : KifElement(klc,kp,kifFloatVectorPtr) {
		values=v;
		it=0;
	}
	bool Variable() {
		return true;
	}


	KifElement* Index(KifElement* s) {
		if (values==NULL)
			return kifFALSE;

		long i=s->Integer();
		if (i<0 || i>=values->size())
			return kifFALSE;
		return kifTRUE;
	}


	Exported KifType theType() {
		return kifVector;
	}

	bool aContainer() {
		return true;
	}

	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		return this;
	}

	KifElement* Newinstance() {
		return this;
	}

	KifElement* Newiterator(bool);

	KifElement* ValueOnIndex(size_t i) {
		if (values->vecteur==NULL || i<0 || i>=values->dernier)
			return kifNOELEMENT;

		double v=(*values)[i];
		return kifcode->Providefloat(v);
	}

	void Push(KifElement* ke) {
		if (values!=NULL)
			values->ajoute(ke->Float());
	}

	void Pop(KifElement* ke);
	KifElement* Last();

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);

	string String();

	bool Boolean();

	KifElement* Exec(KifElement* pattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction*);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke) {
		threadblocinit;
		return Exec(contextualpattern, dom, ke, idthread, callfunc);
	}

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);
	//Clear cleans the vector and free some of the inner instances
	//There is slight difference with the next function. In this case,
	//we do not want to interfere with an instance that would be a map or
	//a vector and free its content, while it should still be alive...

	void Clear() {
		values->raz();
	}


	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);
	long Integer() {
		if (values==NULL)
			return 0;
		return values->size();
	}
	double Float() {
		if (values==NULL)
			return 0;

		return values->size();
	}
	long Size() {
		if (values==NULL)
			return 0;
		return values->size();
	}

	bool aVectorContainer() {
		return true;
	}

	void Merge(KifElement* ke);
};

class KifVectorStringPtr : public KifElement {
public:
	VECTA<string*>* values;

	KifVectorStringPtr(KifCode* klc,KifElement* kp,VECTA<string*>* v) : KifElement(klc,kp,kifVectorPtr) {
		values=v;
	}
	bool Variable() {
		return true;
	}

	bool aContainer() {
		return true;
	}

	KifElement* Index(KifElement* s) {
		if (values==NULL)
			return kifFALSE;

		long i=s->Integer();
		if (i<0 || i>=values->size())
			return kifFALSE;
		return kifTRUE;
	}

	Exported KifType theType() {
		return kifVector;
	}

	KifElement* Newinstance() {
		return this;
	}
	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		return this;
	}

	string String();
	KifElement* Newiterator(bool);

	KifElement* ValueOnIndex(size_t i) {
		if ((*values)[i]==NULL)
			return kifNOELEMENT;

		return kifcode->Providestringraw(*(*values)[i]);
	}

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);

	KifElement* Exec(KifElement* pattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction*);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke) {
		threadblocinit;
		return Exec(contextualpattern, dom, ke, idthread, callfunc);
	}

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);
	//Clear cleans the vector and free some of the inner instances
	//There is slight difference with the next function. In this case,
	//we do not want to interfere with an instance that would be a map or
	//a vector and free its content, while it should still be alive...

	void Clear() {
		for (int i=0;i<values->size();i++) {
			delete values->vecteur[i];
		}
		values->raz();
	}

	bool Boolean();

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);
	void Push(KifElement* ke) {
		if (values!=NULL)
			values->ajoute(new string(ke->String()));
	}


	void Pop(KifElement* ke);
	KifElement* Last();

	long Integer() {
		if (values==NULL)
			return 0;
		return values->size();
	}
	double Float() {
		if (values==NULL)
			return 0;
		return values->size();
	}
	long Size() {
		if (values==NULL)
			return 0;
		return values->size();
	}

	bool aVectorContainer() {
		return true;
	}

	void Merge(KifElement* ke);

};

class KifMapPtr : public KifElement {
public:
	map<string,string>* values;

	KifMapPtr(KifCode* klc,KifElement* kp,map<string,string>* v) : KifElement(klc,kp,kifMapPtr) {
		values=v;
	}

	bool Variable() {
		return true;
	}

	Exported KifType theType() {
		return kifMap;
	}

	KifElement* ValueOnKey(string k) {
		ThreadLock _lock(type);
		if (values==NULL)
			return kifNOELEMENT;
		if (values->find(k)!=values->end())
			return kifcode->Providestring((*values)[k]);
		return kifNOELEMENT;
	}

	string ValueOnKeyString(string k,bool& a) {
		ThreadLock _lock(type);
		if (values==NULL) {
			a=false;
			return "";
		}

		if (values->find(k)!=values->end()) {
			a=true;
			return (*values)[k];
		}
		a=false;
		return "";
	}

	KifElement* Index(KifElement* s) {
		if (values==NULL)
			return kifFALSE;

		string i=s->String();
		if (values->find(i)==values->end())
			return kifFALSE;
		return kifTRUE;
	}

	bool aContainer() {
		return true;
	}

	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		return this;
	}

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);

	KifElement* Newinstance() {
		return this;
	}

	string String();
	Exported void Push(string& key,KifElement* val);
	Exported void Push(char* key,KifElement* val);


	KifElement* Newiterator(bool);


	void Pop(KifElement* ke);


	KifElement* Values(string n) {
		if (values==NULL || values->find(n)==values->end())
			return kifNOELEMENT;
		return kifTRUE;
	}


	long Size() {
		return values->size();
	}

	KifElement* Exec(KifElement* pattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction*);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke) {
		threadblocinit;
		return Exec(contextualpattern, dom, ke, idthread, callfunc);
	}

	bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);


	void Clear() {
		values->clear();
	}


	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);

	long Integer() {
		return values->size();
	}
	double Float() {
		return values->size();
	}

	bool aVectorContainer() {
		return true;
	}

	void Merge(KifElement* ke);
};

class KifIteratorVectorStringPtr : public KifIteration {
public:
	VECTA<string*>* values;
	int it;

	KifIteratorVectorStringPtr(KifCode* klc,KifElement* kp) : KifIteration(klc,kp) {it=0;}

	Exported KifElement* IteratorKey() {
		KifElement* ke=kifcode->Provideinteger(it);		
		return ke;
	}

	Exported KifElement* IteratorValue() {
		if ((*values)[it]==NULL)
			return kifNOELEMENT;
		return kifcode->Providestringraw(*(*values)[it]);
	}

	Exported void Next() {
		if (reverse==false)
			it++;
		else
			it--;
	}

	Exported KifElement* End() {
		if (reverse==false) {
			if (it>=values->size())
				return kifTRUE;		
		}
		else {
			if (it<0)
				return kifTRUE;
		}
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		if (reverse==false)
			it=0;
		else
			it=values->size()-1;

		if ((*values)[it]==NULL)
			return kifFALSE;
		return kifTRUE;
	}
};


class KifIteratorVectorNumber : public KifIteration {
public:
	VECTA<double>* values;
	int it;

	KifIteratorVectorNumber(KifCode* klc,KifElement* kp) : KifIteration(klc,kp) {it=0;}

	Exported KifElement* IteratorKey() {
		if (it>=values->size() || it<0)
			return kifNOELEMENT;
		return kifcode->Provideinteger(it);		
	}

	Exported KifElement* IteratorValue() {
		if (it>=values->size() || it<0)
			return kifNOELEMENT;
		return kifcode->Providefloat((*values)[it]);
	}


	Exported void Next() {
		if (it>=values->size() || it<0)
			return;

		if (reverse==false)
			it++;
		else
			it--;
	}

	Exported KifElement* End() {
		if (reverse==false) {
			if (it>=values->size())
				return kifTRUE;		
		}
		else {
			if (it<0)
				return kifTRUE;
		}
		return kifFALSE;
	}

	Exported KifElement* Begin() {
		if (reverse==false)
			it=0;
		else
			it=values->size()-1;
		if (it>=values->size())
			return kifFALSE;
		return kifTRUE;
	}
};

class KifIteratorMapPtr : public KifIteration {
public:

	map<string,string>* values;

	map<string,string>::iterator it;
	map<string,string>::reverse_iterator itr;
	KifIteratorMapPtr(KifCode* klc,KifElement* kp) : KifIteration(klc,kp) {}


	KifElement* IteratorKey() {
		if (reverse==false) {
			if (it==values->end())
				return kifNOELEMENT;

			return kifcode->Providestringraw(it->second);
		}
		if (itr==values->rend())
			return kifNOELEMENT;

		return kifcode->Providestringraw(itr->second);	
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
		}
		else
			if (itr==values->rend())
				return "";

		return itr->second;
	}

	KifElement* IteratorValue() {		
		if (reverse==false) {
			if (it==values->end())
				return kifNOELEMENT;

			return kifcode->Providestringraw(it->second);
		}
		if (itr==values->rend())
			return kifNOELEMENT;

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


class KifGraph : public KifElement {
public:
	graphe* graph;
	bool local;
	KifGraph(KifCode* klc,KifElement* kp,graphe* g,bool l=false) : KifElement(klc,kp,kifGraph) {
		graph=g;
		local=l;
	}

	bool Boolean() {
		if (graph!=NULL)
			return true;
		return false;
	}


	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);
	KifElement* Declaration(short name);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement* kstr);
	string String();
	void Clean();
	void Clear();
	KifElement* same(KifElement* a);
	KifElement* different(KifElement* a);
	bool Setvalue(KifElement* a,KifElement* index=NULL,KifElement* dom=NULL);
	KifElement* Project(KifElement* g);
	KifElement* Extract(KifElement* g);
	KifElement* Replace(KifElement* pat,KifElement* replacement);
	bool Basic() {
		return true;
	}
	bool Variable() {
		return true;
	}

	KifElement* Newinstance() {
		KifElement* ke=new KifGraph(kifcode,NULL,NULL);
		return ke;
	}
	
	KifElement* Exec(KifElement* pattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction*);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke) {
		threadblocinit;
		return Exec(contextualpattern, dom, ke, idthread, callfunc);
	}

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx);
	KifElement* Remove(KifElement* a);
};

class KifXipElement : public KifElement {
public:

	KifXipElement(KifCode* klc,KifElement* kp,KifType t) : KifElement(klc,kp,t) {}

	virtual GlobalParseur* Leparseur() {
		return NULL;
	}
};

class KifXipNode : public KifXipElement {
public:
	static vector<KifXipNode*> nodes;
	Noeud* value;
	int position;

	KifXipNode(KifCode* klc,KifElement* kp,Noeud* n) : KifXipElement(klc,kp,kifXipNode) {
		value=n;
		position=KifXipNode::nodes.size();
		KifXipNode::nodes.push_back(this);
	}

	~KifXipNode() {
		if (position<KifXipNode::nodes.size())
			KifXipNode::nodes[position]=NULL;
	}

	GlobalParseur* Leparseur();

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	string String();
	string JSonString();
	long Integer();
	double Float();
	bool Variable() {
		return true;
	}

	void Constructionarbre(Noeud* n,KifVector* v);
	KifElement* Vector(KifElement*,KifVector* itself=NULL);
	KifElement* Map(KifElement*,KifMap* itself=NULL);

	KifElement* Newinstance() {
		KifElement* ke=new KifXipNode(kifcode,NULL,NULL);
		return ke;
	}

	void CleanPointer() {
		value=NULL;
	}

	bool Basic() {
		return true;
	}

	bool Boolean() {
		if (value!=NULL)
			return true;
		return false;
	}

	bool Setvalue(KifElement* a,KifElement* index=NULL,KifElement* dom=NULL);

	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		KifElement* ke=new KifXipNode(kifcode,kp,value);
		return ke;
	}
	
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke) {
		threadblocinit;
		return Exec(contextualpattern, dom, ke, idthread, callfunc);
	}

	KifElement* same(KifElement* a) {
		if (a==kifNULL) {
			if (value==NULL)
				return kifTRUE;
			return kifFALSE;
		}
		if (a->type!=kifXipNode)
			return kifFALSE;
		KifXipNode* kx=(KifXipNode*)a;
		if (kx->value==value)
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* different(KifElement* a) {
		if (same(a)->Boolean()==true)
			return kifFALSE;
		return kifTRUE;
	}

	KifElement* less(KifElement* a);
	KifElement* more(KifElement* a);
	KifElement* lessequal(KifElement* a);
	KifElement* moreequal(KifElement* a);

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
		if (kfunc==NULL)
			return a->same(b);
		return applyfunc(a,b,kfunc);
	}

};

class KifXipDependency : public KifXipElement {
public:

	static vector<KifXipDependency*> dependencies;
	int position;
	ResultatFonction* value;
	KifXipDependency(KifCode* klc,KifElement* kp,ResultatFonction* d,KifType kt=kifXipDependency) : KifXipElement(klc,kp,kt) {
		position=dependencies.size();
		dependencies.push_back(this);
		value=d;
	}
	
	string String();
	string JSonString();

	long Integer();
	double Float();
	bool Variable() {
		return true;
	}

	~KifXipDependency() {
		if (position<KifXipDependency::dependencies.size())
			KifXipDependency::dependencies[position]=NULL;
	}

	KifElement* Vector(KifElement*, KifVector* itself = NULL);
	KifElement* Map(KifElement*, KifMap* itself = NULL);

	GlobalParseur* Leparseur();

	bool Boolean() {
		if (value!=NULL)
			return true;
		return false;
	}

	KifElement* Newinstance() {
		KifElement* ke=new KifXipDependency(kifcode,NULL,NULL);
		return ke;
	}

	bool Basic() {
		return true;
	}

	void CleanPointer() {
		value=NULL;
	}

	virtual KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		KifElement* ke=new KifXipDependency(kifcode,kp,value);
		return ke;
	}

	virtual bool Setvalue(KifElement* a,KifElement* index=NULL,KifElement* dom=NULL);
	KifElement* Exec(KifElement* pattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction*);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke) {
		threadblocinit;
		return Exec(contextualpattern, dom, ke, idthread, callfunc);
	}

	virtual KifElement* Declaration(short n);
	virtual void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);


	virtual KifElement* same(KifElement* a) {
		if (a==kifNULL) {
			if (value==NULL)
				return kifTRUE;
			return kifFALSE;
		}
		if (a->type!=kifXipDependency)
			return kifFALSE;
		KifXipDependency* kx=(KifXipDependency*)a;
		if (kx->value==value)
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* different(KifElement* a) {
		if (same(a)->Boolean()==true)
			return kifFALSE;
		return kifTRUE;
	}

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
		if (kfunc==NULL)
			return a->same(b);
		return applyfunc(a,b,kfunc);
	}

};

class KifXipGeneration : public KifXipDependency {
public:

	KifXipGeneration(KifCode* klc,KifElement* kp,ResultatFonction* d) : KifXipDependency(klc,kp,d,kifXipGeneration) {}
	KifElement* same(KifElement* a) {
		if (a==kifNULL) {
			if (value==NULL)
				return kifTRUE;
			return kifFALSE;
		}
		if (a->type!=kifXipGeneration)
			return kifFALSE;
		KifXipGeneration* kx=(KifXipGeneration*)a;
		if (kx->value==value)
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	bool Setvalue(KifElement* a,KifElement* index=NULL,KifElement* dom=NULL);
	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		KifElement* ke=new KifXipGeneration(kifcode,kp,value);
		return ke;
	}
};

#ifdef XIPLIBXML
class KifXMLNodePtr : public KifElement {
public:

	VariableXML* value;
	KifXMLNodePtr(KifCode* klc,KifElement* kp,VariableXML* n) : KifElement(klc,kp,kifXML) {
		value=n;
	}

	void CleanXML() {
		value=NULL;
	}

	KifElement* Returnvalue();
	KifElement* Newinstance();

	KifElement* Declaration(short n);
	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	string String();
	bool Variable() {
		return true;
	}

	void* Node();

	bool Setvalue(KifElement* a,KifElement* index=NULL,KifElement* dom=NULL);
	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		return new KifXMLNodePtr(kifcode,kp,value);
	}
	KifElement* Exec(KifElement* pattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction*);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke) {
		threadblocinit;
		return Exec(contextualpattern, dom, ke, idthread, callfunc);
	}

	bool Boolean() {
		if (value!=NULL)
			return true;
		return false;
	}

	KifElement* same(KifElement* a);
	KifElement* different(KifElement* a) {
		if (same(a)->Boolean()==true)
			return kifFALSE;
		return kifTRUE;
	}

	KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
		if (kfunc==NULL)
			return a->same(b);
		return applyfunc(a,b,kfunc);
	}
};
#endif

#ifdef NTMCONNECTION
class KifFST : public KifElement {
public:
	TokLOOK* tok;
	NETptr net;
	NVptr nets;
	STATEptr final_state;
	STATEptr *start_destinations;
	int result_count;
	int sd_len;
	short idnet;
	bool utf8mode;
	bool tocomplete;

	KifFST(KifCode* klc,KifElement* kp);
	~KifFST();

	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		KifFST* kfst=new KifFST(kifcode,NULL);
		kfst->tok=tok;
		return kfst;
	}


	bool Basic() {
		return true;
	}

	bool Setvalue(KifElement* ke, KifElement* idx, KifElement* dom = NULL);
	KifElement* Exec(KifElement* pattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction*) {
		return this;
	}

	KifElement* Execute(KifElement* pattern, KifDomain* dom, KifElement* idx) {
		return this;
	}

	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement* kstr); 
	KifElement* Tokup(KifElement* str,KifCallFunction* call,KifElement* contextualpattern);
	KifElement* Tokdown(KifElement* str,KifElement* feats,KifElement* pattern);
	KifElement* Load(KifElement* filename,KifElement* flags,KifElement* ty,KifElement* utf8);
	KifElement* Loadfst(KifElement* filename,KifElement* flags,KifElement* ty,KifElement* utf8);
	KifElement* Declaration(short name);
	string String();
	bool Boolean() {
		if (tok!=NULL)
			return true;
		return false;
	}
	
	Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);
	Exported bool initialconstructor() {
		return true;
	}

	KifElement* Compilenet(KifElement* kmap,bool utf8mode);
	KifElement* CompilenetWith(KifElement* kmap,KifElement* filename,bool utf8mode);
	KifElement* Compileline(string upper,string lower);
	KifElement* CompilelineInit(bool);
	KifElement* CompilelineSave(string filename,bool utf8);
	void Loadspacedtext(FILE* filename,bool utf8mode);
	void Loadregular(char* filename, bool utf8mode);
	void Compilescript(char* filename,bool utf8mode);

	NETptr getnet();
	KifElement* orset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	KifElement* andset(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself);
	KifElement* multiply(KifElement* a, KifElement* b, KifElement* context, bool autoself); //In this case, it will be used for composition

	void Clean();
	void Clear();
};

const int maxlong=2048;

class KifNTM : public KifElement {
public:

	bool loaded;
	bool utf8;
    string name;
	string data;
    NtmContext ntmContexte;

	KifNTM(KifCode* klc,KifElement* kp) : KifElement(klc,kp,kifNTM) {
		utf8=false;
		loaded=false;
	}

	~KifNTM();

	KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL) {
		return new KifNTM(kifcode,NULL);
	}

	bool Basic() {
		return true;
	}

	void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement* kstr);
	KifElement* Process(KifVector* kvect);
	void ntmProcessToken(NtmToken* token,KifVector*);
	void localNtmProcessToken(NtmToken* token,KifVector*);
	KifElement* Declaration(short name);
	string String();

	bool Boolean() {
		return loaded;
	}
	
	Exported KifElement* constructor(KifDomain*, KifElement*, int, KifCallFunction*);
	
	Exported bool initialconstructor() {
		return true;
	}

	void Clean();
	void Clear();
};

#endif


class KifParser : public KifKiF {
public:
	bool toinitialize;
	bool todelete;

	KifParser(KifCode* klc,KifElement* kp) : toinitialize(true),KifKiF(klc,kp,kifParser) {todelete=true;}
	Exported KifElement* Information(KifElement* kstr);
	KifElement* Declaration(short name);
	void Methods(KifVectorString*);
	Exported KifElement* constructor(KifDomain*,KifElement*, int idthread, KifCallFunction* callfunc);

	KifElement* Setgrammar(KifElement* k);

	~KifParser();

};


#endif
