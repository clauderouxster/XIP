#include "kifinit.h"
#include "kif.h"

class KifVecta : public KifElement {
public:
	VECTA<KifElement*> values;
	bool evaluate;

	KifVecta(KifCode* klc,KifElement* kp) : KifElement(klc,kp,kifVecta) {evaluate=false;}

	Exported KifElement* Newinstance() {
		KifElement* ke=new KifVecta(kifcode,NULL);
		return ke;
	}

	Exported KifElement* Newiterator(bool);
	Exported void Removecrossreference(KifElement* key,int);
	Exported KifElement* Copy(KifDomain* kp,KifElement* dom=kifNULL);
	Exported KifElement* Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,bool rcouple=false);
	Exported bool Setvalue(KifElement* ke,KifElement* idx,KifElement* dom=NULL);

	bool Basic() {
		return true;
	}

	void Toassign(bool v) {
		evaluate=v;
	}

	bool Assigning(KifType t) {
		if (t!=type)
			return false;
		return evaluate;
	}

	Exported void Push(KifElement* ke);

	Exported KifElement* Pop(KifElement* ke);
	Exported KifElement* Last();
	KifElement* ValueOnIndex(size_t i) {
		if (i>=values.size())
			return kifNULL;
		return values[i];
	}

	Exported void Add(KifElement* e) {
		values.push_back(e);
	}

	Exported KifElement* Declaration(string& n);
	Exported void List(KifVector* fcts);

	//Clear cleans the vector and free some of the inner instances
	//There is slight difference with the next function. In this case,
	//we do not want to interfere with an instance that would be a map or
	//a vector and free its content, while it should still be alive...
	//Clean is called recursively to clean all instances, then itself
	Exported void Setreference() {
		reference++;
		for (int itx=0;itx<values.size();itx++)
			values[itx]->Setreference();
	}

	Exported void Clean() {
		reference--;
		for (int itx=0;itx<values.size();itx++)
			values[itx]->Clean();
		values.raz();
	}

	Exported void Resetreference();

	Exported void Clear();


	Exported KifElement* in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* contextualpattern,bool idx);
	Exported long Integer() {
		return values.size();
	}
	Exported double Float() {
		return values.size();
	}

	Exported string String();

	Exported long Size() {
		return values.size();
	}

	Exported bool Boolean() {
		if (values.size()==0)
			return false;
		return true;
	}

	Exported KifElement* plus(KifElement* a,KifElement* b,bool autoself);
	Exported KifElement* minus(KifElement* a,KifElement* b,bool autoself);
	Exported KifElement* multiply(KifElement* a,KifElement* b,bool autoself);
	Exported KifElement* divide(KifElement* a,KifElement* b,bool autoself);
	Exported KifElement* mod(KifElement* a,KifElement* b,bool autoself);
	Exported KifElement* shiftright(KifElement* a,KifElement* b,bool autoself);
	Exported KifElement* shiftleft(KifElement* a,KifElement* b,bool autoself);
	Exported KifElement* power(KifElement* a,KifElement* b,bool autoself);
	Exported KifElement* orset(KifElement* a,KifElement* b,bool autoself);
	Exported KifElement* andset(KifElement* a,KifElement* b,bool autoself);

};

class KifIteratorVecta : public KifIteration {
public:
	VECTA<KifElement*>* values;
	int it;

	KifIteratorVecta(KifCode* klc,KifElement* kp) : KifIteration(klc,kp) {it=0;}

	Exported KifElement* IteratorKey() {
		KifElement* ke=new KifInteger(kifcode,NULL,it);		
		return ke;
	}

	Exported KifElement* IteratorValue() {
		if ((*values)[it]==NULL)
			return kifNULL;
		return (*values)[it];
	}

	Exported KifElement* Next() {
		if (reverse==false)
			it++;
		else
			it--;
		if ((*values)[it]==NULL)
			return kifNULL;
		return (*values)[it];	
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
			return kifNULL;
		return (*values)[it];		
	}
};


string KifVecta::String() {
	string res;
	vector<KifElement*>::iterator it;
	res="[";
	bool beg=true;
	for (int it=0;it<values.size();it++) {
		if (beg==false)
			res+=",";
		beg=false;
		if (values[it]->type!=kifString)
			res+=values[it]->String();
		else {
			string sx=values[it]->String();
			if (sx.find("'")!=string::npos) {
				res+="\"";
				res+=sx;
				res+="\"";
			}
			else {
				res+="'";
				res+=sx;
				res+="'";
			}
		}
		if (res.size()>512) {
			res+="...";
			break;
		}
	}
	res+="]";
	return res;
}

KifElement* KifVecta::Newiterator(bool rev) {
#ifdef PROFILING
	profilingcall("Vector::Newiterator");
#endif
	KifIteratorVecta* iter=new KifIteratorVecta(kifcode,NULL);
	iter->reverse=rev;
	iter->values=&values;
	return iter;
}

KifElement* KifVecta::Pop(KifElement* ke) {
#ifdef PROFILING
	profilingcall("Vector::Pop");
#endif
	if (values.size()==0)
		return kifNULL;
	int i=ke->Integer();
	if (i==-1)
		i=values.size()-1;
	else
		if (i>=values.size())
			return kifNULL;

	KifElement* kres=values.retireElement(i);
	if (kres!=NULL) {
		kres->Resetreference();
		return kres;
	}
	return kifNULL;
}

KifElement* KifVecta::Last() {
	if (values.size()==0)
		return kifNULL;
	return values.fin();	
}
Exported void KifVecta::Push(KifElement* ke) {
	if (ke==NULL)
		return;
	int nb=reference;
	while (nb>=0) {
		ke->Setreference();
		nb--;
	}
	values.push_back(ke);
}

bool KifVecta::Setvalue(int idthread,KifCallFunction* callfunc,int idthread,KifCallFunction* callfunc,KifElement* ke,KifElement* idx,KifElement* dom) {
#ifdef PROFILING
	profilingcall("Vector::Setvalue");
#endif
	if (ke==this)
		return true;

	int nb;

	if (idx==NULL || idx==kifNULL){
		if (ke->type==kifVector) {
			KifVector* kvect=(KifVector*)ke;
			//We copy all values from ke to this
			Clear();
			vector<KifElement*>::iterator it;
			for (it=kvect->values.begin();it!=kvect->values.end();it++) {
				KifElement* x=*it;
				x->Setreference();
				values.push_back(x);
			}
		}
		else {
			//We gather all the keys from the MAP
			if (ke->type==kifMap) {
				hmap<string,KifElement*>::iterator itmap;
				KifMap* kmap=(KifMap*)ke;
				Clear();
				for (itmap=kmap->values.begin();itmap!=kmap->values.end();itmap++)
					Push(new KifString(kifcode,NULL,itmap->first));
			}
			else {
				//We clean all elements and store the new value instead
				KifElement* knew;
				if (values.size()!=0) {					
					for (int it=0;it<values.size();it++) {
						knew=ke->Copy((KifDomain*)dom);
						knew->reference=values[it]->reference;
						nb=reference;
						while (nb>=0) {
							values[it]->Resetreference();
							nb--;
						}
						values.affecte(it,knew);
					}
				}
				else
					//In this case, we simply "push" it
					Push(ke);
			}
		}
		return true;
	}

	int ikey=idx->Integer();

	if (ikey>=values.size())
		Push(ke);
	else {
		nb=reference;
		if (values[ikey]!=NULL) {			
			while (nb>=0) {
				values[ikey]->Resetreference();
				ke->Setreference();
				nb--;
			}
		}
		else {
			while (nb>=0) {
				ke->Setreference();
				nb--;
			}
		}
		values.affecte(ikey,ke);
	}

	return true;
}

extern map<string,bool> kifVectorFunctions;
bool TestkifFunctions(string& n,map<string,bool>& kiffunc);
KifElement* KifVecta::Declaration(string& n) {
#ifdef PROFILING
	profilingcall("Vector::Declaration");
#endif
	if (TestkifFunctions(n,kifVectorFunctions)==true)
		return kifcode->kifBaseFunctions[n];
	return NULL;
}

void KifVecta::List(KifVector* fcts) {
#ifdef PROFILING
	profilingcall("Vector::List");
#endif
	map<string,bool>::iterator it;
	for (it=kifVectorFunctions.begin();it!=kifVectorFunctions.end();it++)
		fcts->Push(new KifString(kifcode,NULL,it->first));
}

KifElement* KifVecta::Copy(KifDomain* kp,KifElement* dom) {
#ifdef PROFILING
	profilingcall("Vector::Copy");
#endif
	KifElement* ke=new KifVecta(kifcode,kp);
	for (int itx=0;itx<values.size();itx++) {
		KifElement* kx=values[itx]->Copy(kp,dom);
		ke->Push(kx);
	}

	return ke;
}

KifElement* KifVecta::in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* contextualpattern,bool idx) {
#ifdef PROFILING
	profilingcall("Vector::in");
#endif
	KifElement* ke;
	KifElement* k;
	KifElement* res;
	KifElement* found=kifFALSE;


	//If it's a while, then we loop among all elements
	if (contextualpattern->Loop(a))
		return contextualpattern->Nextin(this,a,true);

	if (b->type==kifMap) {
		Clear();
		KifMap* bb=(KifMap*)b;
		hmap<string,KifElement*>::iterator itx;
		for (itx=bb->values.begin();itx!=bb->values.end();itx++) {
			k=itx->second;
			res=k->in(a,k,kfunc,this,idx);
			if (res!=kifFALSE && res!=kifNULL)
				Push(new KifString(kifcode,NULL,itx->first));
		}
		return this;
	}

	if (b->type==kifVector) {
		if (b!=this)
			Clear();
		KifVector* bb=(KifVector*)b;
		vector<KifElement*>::iterator it;
		int ix=0;
		for (it=bb->values.begin();it!=bb->values.end();it++) {
			k=*it;
			res=k->in(a,k,kfunc,this,idx);
			if (res!=kifFALSE && res!=kifNULL) {
				if (b!=this) {
					Push(new KifInteger(kifcode,NULL,ix));
					found=this;
				}
				else {
					if (idx==true) {
						ke=new KifInteger(kifcode,NULL,ix);						
						return ke;
					}
					return kifTRUE;
				}
			}
			ix++;
		}
		
		if (b==this && idx==true)
			return kifMINUSONE;

		return found;
	}

	if (b!=this)
		return b->in(a,b,kfunc,contextualpattern,true);
	if (idx==true)
		return kifMINUSONE;
	return kifFALSE;
}

void KifVecta::Removecrossreference(KifElement* key,int nb) {
#ifdef PROFILING
	profilingcall("Vector::Removecrossreference");
#endif
	int idx=key->Integer();
	if (idx<values.size() && values[idx]!=NULL && values[idx]!=kifNULL) {
		while (nb>=0) {
			values[idx]->Resetreference();
			nb--;
		}
	}
}

void KifVecta::Resetreference() {
	for (int itx=0;itx<values.size();itx++)
		values[itx]->Resetreference();

	reference--;
	if (reference<=0)
		values.raz();
}

void KifVecta::Clear() {
	for (int itx=0;itx<values.size();itx++) {
		int nb=reference;
		while (nb>=0) {
			values[itx]->Resetreference();
			nb--;
		}
	}
	values.raz();
}

KifElement* KifVecta::Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* idx,bool rcouple) {
#ifdef PROFILING
	profilingcall("Vector::Execute");
#endif
	if (idx==NULL || idx==kifNULL) {

		KifElement* ke;
		int it;
		//In this case, we copy the elements from the vector to the map, using the position as index
		if (contextualpattern->type==kifMap) {
			KifMap* map=new KifMap(kifcode,this);
			size_t i=0;
			
			for (it=0;it<values.size();it++) {
				sprintf_s(kifcode->buffer,100,"%d",i);
				ke=values[it];
				ke->Setreference();
				map->values[kifcode->buffer]=ke;
				i++;
			}			
			return map;
		}

		if (contextualpattern->type==kifInteger || contextualpattern->type==kifFloat) {
			KifElement* kv=new KifInteger(kifcode,NULL,values.size());
			return kv;
		}
		//Then some value must be replaced with their evaluation
		if (evaluate==true) {
			KifVector* kvect;
			if (contextualpattern->Assigning(kifVector)) {
				kvect=(KifVector*)contextualpattern;
				kvect->Clear();
			}
			else
				kvect=new KifVector(kifcode,NULL);			
			for (it=0;it<values.size();it++) {
				ke=values[it];
				KifElement* kv=ke->Execute(kifNULL,(KifDomain*)dom,kifNULL,false);
				//if (kv->reference==1)
				//	kv=kv->Copy(dom);
				kvect->Push(kv);
			}
			return kvect;
		}

		return this;
	}
	
	KifElement* key;
	KifElement* keyright=NULL;
	if (idx->type==kifIndex) {
		KifIndex* kind=(KifIndex*)idx;		
		key=kind->key->Execute(kifNULL,dom,kifNULL,false);
		if (kind->interval==true)
			keyright=kind->right->Execute(kifNULL,dom,kifNULL,false);
	}
	else
		key=idx->Execute(contextualpattern,dom,kifNULL,false);

	if (key==kifNULL)
		return kifNULL;

	int ikey=key->Integer();
	if (ikey<0)
		ikey=values.size()+ikey;
	
	if (ikey<0 || ikey>=values.size())
		return kifNULL;

	if (keyright==NULL) 
		return values[ikey];

	int iright=keyright->Integer();
	if (iright<0 || keyright==kifNULL)
		iright=values.size()+iright;
	else 
		if (iright>=values.size())
			iright=values.size();
	//In this case, we must create a new vector
	KifVector* kvect=new KifVector(kifcode,NULL);
	for (int i=ikey;i<iright;i++)
		kvect->Push(values[i]);
	return kvect;
}

KifElement* KifVecta::orset(KifElement* a,KifElement* b,bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::orset");
#endif
	if (a->type!=kifVecta)
		return kifNULL;
	
	KifVecta* ref=this;
	KifVecta* A=(KifVecta*)a;
	
	int it;
	KifElement* ke;
	KifElement* kref;
	bool found;
	if (b->type==kifVecta) {
		if (autoself==false)
			ref=(KifVecta*)a->Copy(NULL);

		KifVecta* bref=(KifVecta*)b;		
		int itref;
		for (itref=0;itref<bref->values.size();itref++) {
			kref=bref->values[itref];
			found=false;
			for (it=0;it<A->values.size();it++) {
				ke=A->values[it];
				if (kref->same(ke)==kifTRUE) {
					found=true;
					break;
				}
			}	
			if (found==false)
				ref->Push(kref);
		}
		return ref;
	}

	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		hmap<string,KifElement*>::iterator itmap=bref->values.begin();
		if (autoself==false)
			ref=(KifVecta*)a->Copy(NULL);
		for (;itmap!=bref->values.end();itmap++) {
			kref=itmap->second;
			for (it=0;it<A->values.size();it++) {
				ke=A->values[it];
				if (kref->same(ke)==kifTRUE) {
					found=true;
					break;
				}
			}
			if (found==false)
				ref->Push(kref);
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifVecta::andset(KifElement* a,KifElement* b,bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::andset");
#endif
	if (a->type!=kifVecta)
		return kifNULL;
	
	KifVecta* A=(KifVecta*)a;
	KifVecta* ref;
	int it;
	KifElement* ke;
	KifElement* kref;
	bool found;
	if (b->type==kifVecta) {
		ref=new KifVecta(kifcode,NULL);
		KifVecta* bref=(KifVecta*)b;		
		int itref;
		for (itref=0;itref<bref->values.size();itref++) {
			kref=bref->values[itref];
			found=false;
			for (it=0;it<A->values.size();it++) {
				ke=A->values[it];
				if (kref->same(ke)==kifTRUE) {
					found=true;
					break;
				}
			}	
			if (found==true)
				ref->Push(kref);
		}
		return ref;
	}

	if (b->type==kifMap) {
		ref=new KifVecta(kifcode,NULL);
		KifMap* bref=(KifMap*)b;		
		hmap<string,KifElement*>::iterator itmap=bref->values.begin();
		for (;itmap!=bref->values.end();itmap++) {
			kref=itmap->second;
			found=false;
			for (it=0;it<A->values.size();it++) {
				ke=A->values[it];
				if (kref->same(ke)==kifTRUE) {
					found=true;
					break;
				}
			}
			if (found==true)
				ref->Push(kref);
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifVecta::plus(KifElement* a,KifElement* b,int idthread,KifCallFunction * callfunc,int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::plus");
#endif
	if (a->type!=kifVecta)
		return kifNULL;
	
	KifVecta* ref=this;
	if (autoself==false)
		ref=(KifVecta*)a->Copy(NULL);

	int it;
	KifElement* ke;
	if (b->type==kifVecta) {
		KifVecta* bref=(KifVecta*)b;		
		int j=0;
		for (it=0;it!=ref->values.size();it++) {
			if (j>=bref->values.size())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->plus(ke,bref->values[j++],true));
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		hmap<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=0;it!=ref->values.size();it++) {
			if (itmap==bref->values.end())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->plus(ke,itmap->second,true));
			itmap++;
		}
		return ref;
	}
	for (it=0;it!=ref->values.size();it++) {
		ke=values[it];
		ke->Resetreference();
		values.affecte(it,ke->plus(ke,b,true));
	}
	return ref;
}

KifElement* KifVecta::minus(KifElement* a,KifElement* b,int idthread,KifCallFunction * callfunc,int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::minus");
#endif
	if (a->type!=kifVecta)
		return kifNULL;
	
	KifVecta* ref=this;
	if (autoself==false)
		ref=(KifVecta*)a->Copy(NULL);

	int it;
	KifElement* ke;
	if (b->type==kifVecta) {
		KifVecta* bref=(KifVecta*)b;		
		int j=0;
		for (it=0;it!=ref->values.size();it++) {
			if (j>=bref->values.size())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->minus(ke,bref->values[j++],true));
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		hmap<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=0;it!=ref->values.size();it++) {
			if (itmap==bref->values.end())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->minus(ke,itmap->second,true));
			itmap++;
		}
		return ref;
	}

	for (it=0;it!=ref->values.size();it++) {
		ke=values[it];
		ke->Resetreference();
		values.affecte(it,ke->minus(ke,b,true));
	}
	return ref;
}

KifElement* KifVecta::multiply(KifElement* a,KifElement* b,int idthread,KifCallFunction * callfunc,int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::multiply");
#endif
	if (a->type!=kifVecta)
		return kifNULL;
	
	KifVecta* ref=this;
	if (autoself==false)
		ref=(KifVecta*)a->Copy(NULL);

	int it;
	KifElement* ke;
	if (b->type==kifVecta) {
		KifVecta* bref=(KifVecta*)b;		
		int j=0;
		for (it=0;it!=ref->values.size();it++) {
			if (j>=bref->values.size())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->multiply(ke,bref->values[j++],true));
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		hmap<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=0;it!=ref->values.size();it++) {
			if (itmap==bref->values.end())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->multiply(ke,itmap->second,true));
			itmap++;
		}
		return ref;
	}

	for (it=0;it!=ref->values.size();it++) {
		ke=values[it];
		ke->Resetreference();
		values.affecte(it,ke->multiply(ke,b,true));
	}
	return ref;
}
KifElement* KifVecta::divide(KifElement* a,KifElement* b,int idthread,KifCallFunction * callfunc,int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::divide");
#endif
	if (a->type!=kifVecta)
		return kifNULL;
	
	KifVecta* ref=this;
	if (autoself==false)
		ref=(KifVecta*)a->Copy(NULL);

	int it;
	KifElement* ke;
	if (b->type==kifVecta) {
		KifVecta* bref=(KifVecta*)b;		
		int j=0;
		for (it=0;it!=ref->values.size();it++) {
			if (j>=bref->values.size())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->divide(ke,bref->values[j++],true));
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		hmap<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=0;it!=ref->values.size();it++) {
			if (itmap==bref->values.end())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->divide(ke,itmap->second,true));
			itmap++;
		}
		return ref;
	}

	for (it=0;it!=ref->values.size();it++) {
		ke=values[it];
		ke->Resetreference();
		values.affecte(it,ke->divide(ke,b,true));
	}
	return ref;
}
KifElement* KifVecta::mod(KifElement* a,KifElement* b,int idthread,KifCallFunction * callfunc,int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::mod");
#endif
	if (a->type!=kifVecta)
		return kifNULL;
	
	KifVecta* ref=this;
	if (autoself==false)
		ref=(KifVecta*)a->Copy(NULL);

	int it;
	KifElement* ke;
	if (b->type==kifVecta) {
		KifVecta* bref=(KifVecta*)b;		
		int j=0;
		for (it=0;it!=ref->values.size();it++) {
			if (j>=bref->values.size())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->mod(ke,bref->values[j++],true));
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		hmap<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=0;it!=ref->values.size();it++) {
			if (itmap==bref->values.end())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->mod(ke,itmap->second,true));
			itmap++;
		}
		return ref;
	}

	for (it=0;it!=ref->values.size();it++) {
		ke=values[it];
		ke->Resetreference();
		values.affecte(it,ke->mod(ke,b,true));
	}
	return ref;
}
KifElement* KifVecta::shiftright(KifElement* a,KifElement* b,int idthread,KifCallFunction * callfunc,int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::shiftright");
#endif
	if (a->type!=kifVecta)
		return kifNULL;
	
	KifVecta* ref=this;
	if (autoself==false)
		ref=(KifVecta*)a->Copy(NULL);

	int it;
	KifElement* ke;
	if (b->type==kifVecta) {
		KifVecta* bref=(KifVecta*)b;		
		int j=0;
		for (it=0;it!=ref->values.size();it++) {
			if (j>=bref->values.size())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->shiftright(ke,bref->values[j++],true));
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		hmap<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=0;it!=ref->values.size();it++) {
			if (itmap==bref->values.end())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->shiftright(ke,itmap->second,true));
			itmap++;
		}
		return ref;
	}

	for (it=0;it!=ref->values.size();it++) {
		ke=values[it];
		ke->Resetreference();
		values.affecte(it,ke->shiftright(ke,b,true));
	}
	return ref;
}
KifElement* KifVecta::shiftleft(KifElement* a,KifElement* b,int idthread,KifCallFunction * callfunc,int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::shiftleft");
#endif
	if (a->type!=kifVecta)
		return kifNULL;
	
	KifVecta* ref=this;
	if (autoself==false)
		ref=(KifVecta*)a->Copy(NULL);

	int it;
	KifElement* ke;
	if (b->type==kifVecta) {
		KifVecta* bref=(KifVecta*)b;		
		int j=0;
		for (it=0;it!=ref->values.size();it++) {
			if (j>=bref->values.size())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->shiftleft(ke,bref->values[j++],true));
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		hmap<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=0;it!=ref->values.size();it++) {
			if (itmap==bref->values.end())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->shiftleft(ke,itmap->second,true));
			itmap++;
		}
		return ref;
	}

	for (it=0;it!=ref->values.size();it++) {
		ke=values[it];
		ke->Resetreference();
		values.affecte(it,ke->shiftleft(ke,b,true));
	}
	return ref;
}
KifElement* KifVecta::power(KifElement* a,KifElement* b,int idthread,KifCallFunction * callfunc,int idthread,KifCallFunction * callfunc,bool autoself) {
#ifdef PROFILING
	profilingcall("Vector::power");
#endif
	if (a->type!=kifVecta)
		return kifNULL;
	
	KifVecta* ref=this;
	if (autoself==false)
		ref=(KifVecta*)a->Copy(NULL);

	int it;
	KifElement* ke;
	if (b->type==kifVecta) {
		KifVecta* bref=(KifVecta*)b;		
		int j=0;
		for (it=0;it!=ref->values.size();it++) {
			if (j>=bref->values.size())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->power(ke,bref->values[j++],true));
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		hmap<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=0;it!=ref->values.size();it++) {
			if (itmap==bref->values.end())
				break;
			ke=values[it];
			ke->Resetreference();
			values.affecte(it,ke->power(ke,itmap->second,true));
			itmap++;
		}
		return ref;
	}
	for (it=0;it!=ref->values.size();it++) {
		ke=values[it];
		ke->Resetreference();
		values.affecte(it,ke->power(ke,b,true));
	}
	return ref;
}

//------------------------------------------------------------------------------





KifElement* KifCode::KifParseVarVecta(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarVector");
#endif
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifVecta* kv=new KifVecta(this,var);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}



