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
   Purpose    : Prolog implementation
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "kif.h"
#include "kifpredicate.h"
#include "kifcontainer.h"
#include <memory>

void PreLocalPrint(string s);

//-----------------------------------------------
//We implement a local reshuffling...
double localrandom(long mx);

static void shufflepredicates(vector<KifPredicate*>& values) {
	size_t sz = values.size();
	size_t i, f;
	int mx = sz;
	KifPredicate* v;
	for (i = 0; i<sz; i++) {
		f = localrandom(mx--);
		if (mx != f) {
			v = values[mx];
			values[mx] = values[f];
			values[f] = v;
		}
	}
}

static void shufflerules(vector<KifPredicateRule*>& values) {
	size_t sz = values.size();
	size_t i, f;
	int mx = sz;
	KifPredicateRule* v;
	for (i = 0; i<sz; i++) {
		f = localrandom(mx--);
		if (mx != f) {
			v = values[mx];
			values[mx] = values[f];
			values[f] = v;
		}
	}
}


//----------------------------------------------

void KifGlobalThread::UpdateFail(KifCode* klc) {
	if (gfail==NULL) {
		gfail=new KifPredicateFunction(NULL,NULL,NULL,idfail);
		gcut=new KifPredicateFunction(NULL,NULL,NULL,idthecut);
		kifFAIL->kifcode=klc;
		kifCUT->kifcode=klc;
	}
	klc->predicates[idfail]=kifFAIL;
	klc->predicates[idthecut]=kifCUT;
}

extern const char* kifErrorStrings[];
bool TestkifFunctions(short n,hmap<short,bool>& kiffunc);


KifElement* PrePredicateWeight(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	double d = 0;
	KifElement* kpred = callfunc->Evaluatethread(0, dom, idthread);
	if (callfunc->Size() == 1) {
		if (kpred->type == kifPredicate)
			d = ((KifPredicate*)kpred)->weight->Float();
		return callfunc->kifcode->Providefloat(d);
	}

	if (kpred->type == kifPredicate) {
		d = callfunc->Evaluatethread(1, dom, idthread)->Float();
		((KifPredicate*)kpred)->weight = callfunc->kifcode->Providefloat(d);
		((KifPredicate*)kpred)->weight->Setreference();
		return kifTRUE;
	}
	return kifFALSE;
}

KifElement* PreTrace(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc,int idthread) {
    KifElement* pred=callfunc->Evaluatethread(0,dom,idthread);
    if (pred->type==kifPredicate)
        callfunc->kifcode->predicates[pred->Name()]->trace=callfunc->Evaluatethread(1,dom,idthread)->Boolean();
    return kifTRUE;
}

KifElement* PreMode(KifElement* contextualpattern, KifDomain* dom, KifCallFunction* callfunc, int idthread) {
	int act = callfunc->Evaluatethread(0, dom, idthread)->Integer();
	callfunc->kifcode->checkprobabilities = act;

	if ((callfunc->kifcode->checkprobabilities & PRED_WEIGHTSTACK) == PRED_WEIGHTSTACK) {
		if (callfunc->Size() > 1) {
			KifElement* pred = callfunc->Evaluatethread(1, dom, idthread);
			if (pred->isFunction())
				callfunc->kifcode->weightfinalfunction = pred;
			else
			if (pred != kifNULL)
				return callfunc->kifcode->Returnerror("PRE(010): Expecting a function");
			if (callfunc->Size() == 3) {
				pred = callfunc->Evaluatethread(2, dom, idthread);
				if (pred->isFunction())
					callfunc->kifcode->weightaiguillagefunction = pred;
				else
				if (pred != kifNULL)
					return callfunc->kifcode->Returnerror("PRE(010): Expecting a function");
			}
		}
	}

	return kifTRUE;
}

KifPredicateContainer* KifDomain::Predicatecontainer() {
	if (declarations.find(KIFPREDICATEENTREE)==declarations.end())
		return NULL;
	return (KifPredicateContainer*)declarations[KIFPREDICATEENTREE];
}

KifElement* PrePredicateDump(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc,int idthread) {
	//We display all predicates or one...
	string label;
	ThreadLock _lock(kifPredicate);
	hmap<short,vector<KifPredicate*> >& knowledge=kifGlobals->knowledgebase;

	KifVector* kvect=new KifVector(callfunc->kifcode,NULL);
	if (callfunc->Size()==0) {
		hmap<short,vector<KifPredicate*> >::iterator it; 
		for (it=knowledge.begin();it!=knowledge.end();it++) {
			vector<KifPredicate*>& vect=it->second;
			for (int i=0;i<vect.size();i++)
				kvect->Push(vect[i]);
		}
		return kvect;
	}

	label=callfunc->Evaluatethread(0,dom,idthread)->String();
	short name=KifStringId(label);
	if (knowledge.find(name)==knowledge.end())
		return kvect;

	vector<KifPredicate*>& vect=knowledge[name];
	for (int i=0;i<vect.size();i++)
		kvect->Push(vect[i]);
	return kvect;
}

KifElement* PreRetractAll(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc,int idthread) {
	//We display all predicates or one...	
	ThreadLock _lock(kifPredicate);

	hmap<short,vector<KifPredicate*> >& knowledge=kifGlobals->knowledgebase;
	if (callfunc->Size()==0) {
		hmap<short,vector<KifPredicate*> >::iterator it; 
		for (it=knowledge.begin();it!=knowledge.end();it++) {
			vector<KifPredicate*>& vect=it->second;
			for (int i=0;i<vect.size();i++)
				vect[i]->Resetreference();
			vect.clear();
		}
		knowledge.clear();
		kifGlobals->knowledgebasekeys.clear();
		return kifTRUE;
	}

	string label=callfunc->Evaluatethread(0,dom,idthread)->String();
	short name=KifStringId(label);
	if (knowledge.find(name)==knowledge.end())
		return kifFALSE;

	vector<KifPredicate*>& vect=knowledge[name];
	for (int i=0;i<vect.size();i++)
		vect[i]->Resetreference();
	vect.clear();
	kifGlobals->knowledgebasekeys.erase(label);
	return kifTRUE;
}

bool KifCode::predicateTrace(short n) {
    return predicates[n]->trace;
}

int KifPredicate::nb=100;

void KifPredicateVariableInstance::affiche() {
    if (value!=kifNOELEMENT)
        cerr<<"="<<value->String();
    cerr<<"/"<<reference;
}

bool compshort(short n,short nn) {
    if (n<nn)
        return true;
    return false;
}

void AffichageDom(KifDomain* dom,int depth) {
    for (int i=0;i<depth;i++)
        cerr<<" ";
    hmap<short,KifElement*>::iterator it;
    vector<short> names;
    for (it=dom->declarations.begin();it!=dom->declarations.end();it++)
        names.push_back(it->first);
    
    sort(names.begin(),names.end(),compshort);
    bool first=true;
    for (int i=0;i<names.size();i++) {
        if (!first)
            cerr<<",";
        first=false;
        KifElement* v=dom->declarations[names[i]];
        if (names[i]!=v->Name())
            cerr<<names[i]<<":"<<v->Name();
        else {
            cerr<<names[i];
            ((KifPredicateVariableInstance*)v)->affiche();
        }
    }
    cerr<<endl;

}

bool KifPredicate::Key(string& key,KifDomain* dom) {
	key="";
	if (!parameters.size()) {
		if (name!=-1) {
			key=KifIdString(name);
			return true;
		}
		return false;
	}

	if (parameters[0]!=kifUNIVERSAL && parameters[0]->unifiedDeclaration(dom)) {
		key=KifIdString(name);
		key+="_";
		key+=parameters[0]->BaseString();
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------
bool KifGlobalThread::testPredicate(KifDomain* dom,KifPredicate* p) {
	ThreadLock _lock(kifPredicate);
	if (knowledgebase.find(p->name)==knowledgebase.end())
		return false;

	string key;
	if (p->Key(key,dom)) {
		vector<KifPredicate*>& v=knowledgebasekeys[key];
		for (int i=0;i<v.size();i++)
			if (p->unified(dom,v[i]))
				return true;			
	}
	else {
		vector<KifPredicate*>& v=knowledgebase[p->name];
		for (int i = 0; i < v.size(); i++) {
			if (p->unified(dom, v[i]))
				return true;
		}
	}
	return false;
}

char KifGlobalThread::isaValidPredicate(KifDomain* dom,KifPredicate* p,hmap<short,vector<KifPredicateRule*> >& rulebase) {
	ThreadLock _lock(kifPredicate);
	if (knowledgebase.find(p->name)==knowledgebase.end())
		return 1;

	string key;	
	bool bkey=false;
	if (p->Key(key,dom)) {
		bkey=true;
		vector<KifPredicate*>& vx=knowledgebasekeys[key];
		for (int i=0;i<vx.size();i++) {
			if (p->unified(dom,vx[i]))
				return 2;			
		}
		if (dom->kifcode->predicatedatabase.find(p->name)==dom->kifcode->predicatedatabase.end()) {
			if (rulebase.find(p->name)!=rulebase.end())
				return 1;
			return 0;
		}
	}

	vector<KifPredicate*>& v=knowledgebase[p->name];
	int exist=-1;
	int i;
	for (i=0;i<v.size();i++) {
		if (v[i]->type!=kifPredicateDatabase) {
			if (!bkey && p->unified(dom,v[i]))
				return 2;
		}
		else {
			if (exist==-1)
				exist=i;
		}
	}

	if (exist!=-1) {
		KifInstructionEvaluate kvi(dom->kifcode,NULL);
		kvi.headrule=p;
		kvi.count=true;
		for (i=exist;i<v.size();i++) {
			if (v[i]->type==kifPredicateDatabase && v[i]->Name()==p->name) {
				kvi.head=v[i];
				if (v[i]->EvaluePredicateVariables(&kvi,dom)==kifTRUE) {
					kvi.value->Release();
					return 2;
				}
				kvi.value->Release();
			}
		}
	}
	//we keep a chance of finding a rule to match with
	if (rulebase.find(p->name)!=rulebase.end())
		return 1;
	return 0;
}

bool KifGlobalThread::GetPredicates(KifDomain* dom,KifPredicate* p,vector<KifPredicate*>& res,bool cut) {
	ThreadLock _lock(kifPredicate);
	if (knowledgebase.find(p->name)==knowledgebase.end())
		return false;

	bool ret=false;
	string key;
	int exist=-1;
	int i;
	bool bkey=false;

	if (p->Key(key, dom)) {
		bkey=true;
		vector<KifPredicate*>& v=knowledgebasekeys[key];
		for (i=0;i<v.size();i++) {
			if (p->unified(dom,v[i])) {
				res.push_back(v[i]);
				ret=true;
			}
		}
		if (dom->kifcode->predicatedatabase.find(p->name)==dom->kifcode->predicatedatabase.end())
			return ret;
	}
		
	vector<KifPredicate*>& v=knowledgebase[p->name];
	for (i=0;i<v.size();i++) {
		if (v[i]->type==kifPredicateDatabase) {
			if (exist==-1)
				exist=i;
		}
		else {
			if (!bkey && p->unified(dom, v[i])) {
				res.push_back(v[i]);
				if (cut)
					return true;
				ret = true;
			}
		}
	}

	if (exist!=-1) {
		KifInstructionEvaluate kvi(dom->kifcode,NULL);
		kvi.headrule=p;
		kvi.value=dom;
		for (i=exist;i<v.size();i++) {
			if (v[i]->type==kifPredicateDatabase && v[i]->Name()==p->name) {
				kvi.head=v[i];
				if (v[i]->EvaluePredicateVariables(&kvi,dom)==kifTRUE)
					ret=true;
			}
		}
		for (int j=0;j<kvi.results.size();j++) {
			if (storePredicate(dom,kvi.results[j],true)) {
				res.push_back(kvi.results[j]->Duplicate(NULL,NULL));
				kvi.results[j]->db=true;
			}
			else
				res.push_back(kvi.results[j]);
		}
	}
	return ret;
}

bool KifGlobalThread::storePredicate(KifDomain* dom,KifPredicate* pv,bool last) {
	if (testPredicate(dom,pv))
		return false;
	
	ThreadLock _lock(kifPredicate);
	string key;
	if (pv->Key(key,dom)) {
		if (last)
			knowledgebasekeys[key].push_back(pv);
		else
			knowledgebasekeys[key].insert(knowledgebasekeys[key].begin(),pv);
	}

	if (last)
		knowledgebase[pv->name].push_back(pv);
	else
		knowledgebase[pv->name].insert(knowledgebase[pv->name].begin(),pv);

	pv->Setreference();
	return true;
}

bool KifGlobalThread::RemovePredicates(KifDomain* dom,KifPredicate* p) {
	ThreadLock _lock(kifPredicate);
	if (knowledgebase.find(p->name)==knowledgebase.end())
		return false;

	string key;
	vector<KifPredicate*>& v=knowledgebase[p->name];
	bool res=false;
	for (int i=v.size()-1;i>=0;i--) {
		if (p->same(v[i])->Boolean()) {
			if (v[i]->Key(key,dom)) {
				vector<KifPredicate*>& vk=knowledgebasekeys[key];
				for (int j=vk.size()-1;j>=0;j--) {
					if (vk[j]==v[i]) {
						vk.erase(vk.begin()+j);
						break;
					}
				}
			}

			v[i]->Resetreference();
			v.erase(v.begin()+i);
			res=true;
		}
	}
	return res;
}

bool KifGlobalThread::RemoveThePredicate(KifDomain* dom, KifPredicate* p) {
	ThreadLock _lock(kifPredicate);

	string key;
	vector<KifPredicate*>& v = knowledgebase[p->name];
	bool res = false;
	for (int i = 0; i<v.size();i++) {
		if (p == v[i]) {
			if (v[i]->Key(key, dom)) {
				vector<KifPredicate*>& vk = knowledgebasekeys[key];
				for (int j = vk.size() - 1; j >= 0; j--) {
					if (p==vk[j]) {
						vk.erase(vk.begin() + j);
						break;
					}
				}
			}

			v[i]->Resetreference();
			v.erase(v.begin() + i);
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------------------------------

KifPredicateVariable::KifPredicateVariable(KifCode* klc,KifElement* kp,short n,string l,KifType t) : label(l),KifElement(klc,kp,t) {
	name=n;
}
//----------------------------------------------------------------------------------------
Exported KifElement* KifPredicateVariableInstance::same(KifElement* a) {
	KifDomain* dom=kifGlobals->Top();
	if (dom==NULL)
		return kifFALSE;

	KifElement* v=Value(dom);
	
	if (v==kifNOELEMENT)
		return kifFALSE;
	return v->same(a);
}

Exported KifElement* KifPredicate::same(KifElement* a) {
	if (a->type!=type || parameters.size()!=a->Size())
		return kifFALSE;

	for (int i=0;i<parameters.size();i++) {
		if (parameters[i]->same(a->Parameter(i)) == kifFALSE)
			return kifFALSE;
	}
	
	return kifTRUE;
}

Exported KifElement* KifPredicateTerm::same(KifElement* a) {
	if (a->type!=type || parameters.size()!=a->Size() || a->Name()!=name)
		return kifFALSE;
	for (int i=0;i<parameters.size();i++) {
		if (parameters[i]->same(a->Parameter(i))==kifFALSE)
			return kifFALSE;
	}
	return kifTRUE;
}
//----------------------------------------------------------------------------------------
// This method is used to detect if a variable or a set of variable (as in a vector) in a predicate has been unified.
// This is especially usefull to detect if a variable can be used as an index.
// By design, every object that is not one the following (which inherit the KifElement version) returns true for this method
bool KifPredicateVariable::unifiedDeclaration(KifDomain* dom) {
	return false;
}

bool KifInstructionPARAMETER::unifiedDeclaration(KifDomain* dom) {
	return instructions[0]->unifiedDeclaration(dom);
}

bool KifVector::unifiedDeclaration(KifDomain* dom) {
    for (int i=0;i<values.size();i++) {
        if (values[i]->unifiedDeclaration(dom)==false)
            return false;
    }
    return true;
}

bool KifBaseMap::unifiedDeclaration(KifDomain* dom) {
    for (int i=0;i<values.size();i++) {
        if (keys[i]->unifiedDeclaration(dom)==false)
            return false;
        if (values[i]->unifiedDeclaration(dom)==false)
            return false;
    }
    return true;
}

bool KifPredicateTerm::unifiedDeclaration(KifDomain* dom) {
    for (int i=0;i<parameters.size();i++) {
        if (parameters[i]->unifiedDeclaration(dom)==false)
            return false;
    }
    return true;
}

bool KifPredicateVariableInstance::unifiedDeclaration(KifDomain* dom) {
	KifElement* v=value;
	if (dom!=NULL) 
		v=Value(dom);
	if (v==kifNOELEMENT)
		return false;
	
	if (v==kifUNIVERSAL)
		return true;

	return v->unifiedDeclaration(dom);
}

bool KifPredicate::unifiedDeclaration(KifDomain* dom) {
    for (int i=0;i<parameters.size();i++) {
        if (parameters[i]->unifiedDeclaration(dom)==false)
            return false;
    }
    return true;
}


//--------------------------------------------------------------
bool KifElement::unified(KifDomain* dom,KifElement* a) {
	if (this == kifUNIVERSAL || a == kifUNIVERSAL)
		return true;

	if (same(a)->Boolean() == true)
		return true;
	return false;
}

bool KifPredicateVariableInstance::unified(KifDomain* dom,KifElement* a) {
	KifElement* v=Value(dom);

	if (v == kifNOELEMENT)
		return true;
	
	if (v == kifUNIVERSAL)
		return true;

	return v->unified(dom,a);
}

bool KifPredicateTerm::unified(KifDomain* dom,KifElement* a) {
	if (a->type!=type || a->Name()!=name || a->Size()!=parameters.size())
		return false;
	KifPredicateTerm* term=(KifPredicateTerm*)a;
    for (int i=0;i<parameters.size();i++) {
		if (parameters[i]->unified(dom, term->parameters[i]) == false)
			return false;
    }
    return true;
}

bool KifVector::unified(KifDomain* dom,KifElement* a) {
	if (a->type!=type || a->Size()!=values.size())
		return false;

	KifVector* vect=(KifVector*)a;
    for (int i=0;i<values.size();i++) {
        if (values[i]->unified(dom,vect->values[i])==false)
            return false;
    }
    return true;
}

bool KifBaseMap::unified(KifDomain* dom,KifElement* a) {
	if (a->type!=type || a->Size()!=values.size())
		return false;

	KifBaseMap* kmap=(KifBaseMap*)a;
    for (int i=0;i<values.size();i++) {
		if (keys[i]->unified(dom, kmap->keys[i]) == false)
			return false;
		if (values[i]->unified(dom, kmap->values[i]) == false)
			return false;
    }
    return true;
}

bool KifPredicate::unified(KifDomain* dom,KifElement* a) {
	if (a->type != type || a->Name() != name || a->Size() != parameters.size())
		return false;

	KifPredicate* pred=(KifPredicate*)a;
	for (int i=0;i<parameters.size();i++) {
		if (parameters[i]->unified(dom,pred->parameters[i])==false)
			return false;
	}
	return true;
}

//--------------------------------------------------------------------
bool KifPredicateVariableInstance::Setvalue(KifElement* ke,KifElement* prev,KifElement* dom,int idthread,KifCallFunction* callfunc) {
	KifPredicateVariableInstance* v=(KifPredicateVariableInstance*)Variable((KifDomain*)dom);

	KifElement* val=v->value;
	v->value=ke->Atom(true);
	val->Resetreference(reference);
	v->value->Setreference(reference);
	return true;
}

bool KifPredicateTerm::Setvalue(KifElement* ke,KifElement* prev,KifElement* dom,int idthread,KifCallFunction* callfunc) {
    if (ke->type!=type || ke->Size()!=parameters.size() || ke->Name()!=name) {
        dom->Setfail(true);
        return true;
    }
    KifPredicateTerm* kpf=(KifPredicateTerm*)ke;
    
    for (int i=0;i<parameters.size();i++)
        parameters[i]->Setvalue(kpf->parameters[i],prev,dom);
	return true;
}


Exported KifElement* KifPredicate::Declaration(short n) {
	if (TestkifFunctions(n,kifGlobals->kifPredefinedFunctions)==true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifPredicate::Information(KifElement* kstr) {
	return kifNULL;
}

Exported void KifPredicate::Methods(KifVectorString* fcts) {}


Exported KifElement* KifPredicateVar::Information(KifElement* kstr) {
	string s=kstr->String();
	if (kifGlobals->kifInformationsPredicate.find(s)==kifGlobals->kifInformationsPredicate.end())
		return kifNULL;
	return kifcode->Providestring(kifGlobals->kifInformationsPredicate[s]);
}

KifElement* KifPredicateVar::Declaration(short n) {
	if (TestkifFunctions(n,kifGlobals->kifPredicateFunctions)==true || 
		TestkifFunctions(n,kifGlobals->kifPredefinedFunctions)==true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

void KifPredicateVar::Methods(KifVectorString* fcts) {
	hmap<short,bool>::iterator it;
	for (it=kifGlobals->kifPredicateFunctions.begin();it!=kifGlobals->kifPredicateFunctions.end();it++)
		fcts->values.push_back(kifGlobals->kifIdString[it->first]);	
}

Exported KifElement* KifPredicateTerm::Declaration(short n) {
	if (TestkifFunctions(n,kifGlobals->kifPredefinedFunctions)==true)
		return kifGlobals->kifBaseFunctions[n];
	return NULL;
}

Exported KifElement* KifPredicateTerm::Information(KifElement* kstr) {
	return kifNULL;
}

Exported void KifPredicateTerm::Methods(KifVectorString* fcts) {}

Exported bool KifPredicate::Setvalue(KifElement* val,KifElement* index,KifElement* dom,int idthread,KifCallFunction* callfunc) {
	kifcode->Returnerror("PRE(009): You cannot instanciate a predicate this way");
	return true;
}

Exported bool KifPredicateVar::Setvalue(KifElement* val,KifElement* index,KifElement* dom,int idthread,KifCallFunction* callfunc) {
	if (val->type!=type) {
		kifcode->Returnerror("PRE(001): Cannot instantiate a predicate with this value");
		return true;
	}

	//val cannot contain any PredicateVariable
	int i;
	KifElement* e;
	vector<KifElement*> v;
	for (i=0;i<val->Size();i++) {
		e=val->Parameter(i)->Execute(kifNULL,(KifDomain*)dom);
		if (!e->unifiedDeclaration((KifDomain*)dom)) {
			string message("PRE(001): Cannot instantiate a predicate with this value");
			kifcode->Returnerror(message);
			return true;
		}
		v.push_back(e->Atom(true));
	}

	for (i=0;i<parameters.size();i++)
		parameters[i]->Resetreference(reference);

	parameters.clear();
	
	name=val->Name();
	for (i=0;i<v.size();i++) {
		v[i]->Setreference(reference);
		parameters.push_back(v[i]);
	}

	return true;
}

Exported void KifPredicateTerm::Setreference(int inc) {
	reference+=inc;
	popped=0;
	for (int i=0;i<parameters.size();i++)
		if (parameters[i]->type!=kifInstructionPARAMETER)
			parameters[i]->Setreference(inc);
}

Exported void KifPredicateTerm::Setpopped() {
	popped=1;
	for (int i=0;i<parameters.size();i++)
		if (parameters[i]->type!=kifInstructionPARAMETER)
			parameters[i]->Setpopped();
}

Exported void KifPredicateTerm::Resetreference(int inc) {
	for (int i=0;i<parameters.size();i++)
		if (parameters[i]->type!=kifInstructionPARAMETER)
			parameters[i]->Resetreference(inc);
	KifElement::Resetreference(inc);
}


Exported void KifPredicate::Setreference(int inc) {
	reference+=inc;
	popped=0;
	weight->Setreference(inc);
	for (int i=0;i<parameters.size();i++)
		if (parameters[i]->type!=kifInstructionPARAMETER)
			parameters[i]->Setreference(inc);
}

Exported void KifPredicate::Setpopped() {
	popped=1;
	for (int i=0;i<parameters.size();i++)
		if (parameters[i]->type!=kifInstructionPARAMETER)
			parameters[i]->Setpopped();
}

Exported void KifPredicate::Resetreference(int inc) {
	for (int i=0;i<parameters.size();i++) {
		if (parameters[i]->type!=kifInstructionPARAMETER) {
			if (popped)
				parameters[i]->Putpopped(1);
			parameters[i]->Resetreference(inc);
		}
    }
	weight->Resetreference();
	KifElement::Resetreference(inc);
}

Exported void KifPredicateFunction::Resetreference(int inc) {
	KifPredicate::Resetreference(inc);
}


string KifPredicateKnowledgeBaseFunction::String() {
	string s;
	switch(name) {
	case idponder:
		s = "ponder";
		break;
	case idasserta:
		s="asserta";
		break;
	case idassertz:
		s="assertz";
		break;
	case idassertdb:
		s="assertdb";
		break;
	case idretract:
		s="retract";
		break;
	case idretractdb:
		s="retractdb";
		break;
	default:
		return("");
	}

	s+="(";
	if (parameters.size()) {
		for (int i = 0; i < parameters.size(); i++) {
			if (i)
				s += ",";
			if (parameters[i]->aString())
				s += parameters[i]->BaseString();
			else
				s += parameters[i]->String();
		}
	}
	s += ")";
	return(s);
}

Exported string KifPredicateVariable::String() {
	KifDomain* dom=kifGlobals->Top();
    if (dom==NULL)
        return label;
    KifElement* d=dom->Declared(PREDICATEDICO);
    if (d==NULL)
        return label;
	hmap<short,KifPredicateVariableInstance*>* dico=d->Dico();
	if (dico->find(name)==dico->end())
		return label;

    stringstream v;
	KifPredicateVariableInstance* vx=(KifPredicateVariableInstance*)(*dico)[name]->Variable(dom);
    v<<label<<vx->name;
    if (vx->value!=kifUNIVERSAL && vx->value!=kifNOELEMENT)
        v<<":"<<vx->value->String();
    return v.str();
}

Exported string KifPredicateVariableInstance::String() {
	KifDomain* dom=kifGlobals->Top();
	if (dom==NULL)
		return label;
	KifPredicateVariableInstance* vx=(KifPredicateVariableInstance*)Variable(dom);

    stringstream s;
	if (merge) {
		s<<'|';
		if (vx->value->type==kifVector && vx->value->Size()==1) {
			KifVector* kvect=(KifVector*)vx->value;
			s<<kvect->values[0]->String();
		}
		else
			s<<vx->value->String();
	}
	else {
		if (vx->value==kifUNIVERSAL)
			s<<"_";
		else {
			if (vx->value!=kifNOELEMENT) {
				if (vx->value->aString())
					s<<vx->value->BaseString();
				else
					s<<vx->value->String();
			}
			else
				if (vx->label!="")
					s<<vx->label<<vx->name;
		}
	}
	return s.str().c_str();
}

Exported string KifPredicateTerm::String() {
	string v;
	if (name!=iduniversal)
		v=label;
	v+="(";
	for (int i=0;i<parameters.size();i++) {
		if (i)
			v+=",";
		v+=parameters[i]->String();
	}
	v+=")";
	return v;
}


Exported string KifPredicate::String() {
	string v=KifIdString(name);
	int sz=parameters.size();
	if (weight->Float() != 1) {
		v += "<";
		v += weight->String();
		v += ">";
	}

	if (sz) {
		v+="(";
		for (int i=0;i<sz;i++) {
			if (i)
				v+=",";
			if (parameters[i]->aString())
				v+=parameters[i]->BaseString();
			else
				v+=parameters[i]->String();
		}
		v+=")";
	}
	return v;
}

//----------------------------------------------------------------------------------
Exported string KifPredicateVariableInstance::BaseString() {
	KifDomain* dom=kifGlobals->Top();
	if (dom==NULL)
		return label;

	KifPredicateVariableInstance* vx=(KifPredicateVariableInstance*)Variable(dom);

	stringstream s;
	if (vx->value!=kifNOELEMENT) {
		if (vx->value==kifUNIVERSAL)
			s<<"_";
		else
			s<<vx->value->BaseString();
	}
	else
		if (vx->label!="")
			s<<vx->label;
	return s.str();
}

Exported string KifPredicateTerm::BaseString() {
	string v;
	if (name!=iduniversal)
		v=label;
	v+="(";
	for (int i=0;i<parameters.size();i++) {
		if (i)
			v+=",";
		v+=parameters[i]->BaseString();
	}
	v+=")";
	return v;
}


Exported string KifPredicate::BaseString() {
	stringstream v;
	v<<KifIdString(name);
	if (weight->Float() != 1)
		v << "<" << weight->String() << ">";
	int sz=parameters.size();
	if (sz) {
		v<<"(";
		string svalue;
		for (int i=0;i<parameters.size();i++) {
			if (i)
				v<<",";
			v<<parameters[i]->BaseString();
		}
		v<<")";
	}
	return v.str();
}
//----------------------------------------------------------------------------------------------
Exported KifElement* KifPredicate::Vector(KifElement* dom,KifVector* v) {
	KifVector* kvect=v;
	if (kvect==NULL)
		kvect=kifcode->Providevector();
	else
		kvect->Clear();
	KifBaseString local(NULL,NULL);
	local.kifcode=kifcode;
	local.value=KifIdString(name);
	kvect->Push(&local);
	KifElement* e;
	for (int i=0;i<parameters.size();i++) {
		e=parameters[i];
        if (e->type==kifPredicateTerm) {
            KifElement* vloc=e->Vector(dom,NULL);
            kvect->Push(vloc);
        }
        else
            if (e->type==kifPredicateVariable) {
                e=dom->Declared(e->Name());
                if (e==NULL || e==kifNOELEMENT) {
                    local.value=KifIdString(parameters[i]->Name());
                    kvect->Push(&local);
                }
                else
                    kvect->Push(e);
            }
            else
                kvect->Push(e);
	}
	return kvect;
}

Exported KifElement* KifPredicate::Map(KifElement* dom,KifMap* m) {
	KifMap* kmap=m;
	if (kmap==NULL)
		kmap=kifcode->Providemap();
	else
		kmap->Clear();
	KifBaseString local(NULL,NULL);
	local.kifcode=kifcode;
	local.value=KifIdString(name);
	kmap->Push("name",&local);
	char buff[]={'0',0,0};
    int pos=0;
	KifElement* e;
	for (int i=0;i<parameters.size();i++) {
		e=parameters[i];
		if (i && (i%10)==0) {
			buff[1]='0';
            if (!pos) {
                buff[0]='1';
                pos=1;
            }
            else
                buff[0]++;
		}
        if (e->type==kifPredicateTerm) {
            KifElement* locmap=e->Map(dom,NULL);
            kmap->Push(buff,locmap);
        }
        else
            if (e->type==kifPredicateVariable) {
                e=dom->Declared(e->Name());
                if (e==NULL || e==kifNOELEMENT) {
                    local.value=KifIdString(parameters[i]->Name());
                    kmap->Push(buff,&local);
                }
                else
                    kmap->Push(buff,e);
            }
            else
                kmap->Push(buff,e);
		buff[pos]++;
	}
	return kmap;
}


Exported KifElement* KifPredicateTerm::Vector(KifElement* dom,KifVector* v) {
	KifVector* kvect=v;
	if (kvect==NULL)
		kvect=kifcode->Providevector();
	else
		kvect->Clear();
	KifBaseString local(NULL,NULL);
	local.kifcode=kifcode;
	local.value=KifIdString(name);
	kvect->Push(&local);
	KifElement* e;
	for (int i=0;i<parameters.size();i++) {
		e=parameters[i];
        for (int i=0;i<parameters.size();i++) {
            e=parameters[i];
            if (e->type==kifPredicateTerm) {
                KifElement* vloc=e->Vector(dom,NULL);
                kvect->Push(vloc);
            }
            else
                if (e->type==kifPredicateVariable) {
                    e=dom->Declared(e->Name());
                    if (e==NULL || e==kifNOELEMENT) {
                        local.value=KifIdString(parameters[i]->Name());
                        kvect->Push(&local);
                    }
                    else
                        kvect->Push(e);
                }
                else
                    kvect->Push(e);
        }
	}
	return kvect;
}

Exported KifElement* KifPredicateTerm::Map(KifElement* dom,KifMap* m) {
	KifMap* kmap=m;
	if (kmap==NULL)
		kmap=kifcode->Providemap();
	else
		kmap->Clear();
	KifBaseString local(NULL,NULL);
	local.kifcode=kifcode;
	local.value=KifIdString(name);
	kmap->Push("name",&local);
	char buff[]={'0',0,0};
    int pos=0;
	KifElement* e;
	for (int i=0;i<parameters.size();i++) {
		e=parameters[i];
		if (i && (i%10)==0) {
			buff[1]='0';
            if (!pos) {
                buff[0]='1';
                pos=1;
            }
            else
                buff[0]++;
		}
        if (e->type==kifPredicateTerm) {
            KifElement* locmap=e->Map(dom,NULL);
            kmap->Push(buff,locmap);
        }
        else
            if (e->type==kifPredicateVariable) {
                e=dom->Declared(e->Name());
                if (e==NULL || e==kifNOELEMENT) {
                    local.value=KifIdString(parameters[i]->Name());
                    kmap->Push(buff,&local);
                }
                else
                    kmap->Push(buff,e);
            }
            else
                kmap->Push(buff,e);
		buff[pos]++;
	}
	return kmap;
}

bool KifPredicate::Boolean() {
	return true;
}

KifElement* KifElement::Predicate(KifDomain* dom) {
	string name;
	KifPredicate* pred=NULL;
	KifElement* e;
	int idname=-1;
	if (aVectorContainer()) {
		//It is a vector container
		KifIteration* itr=(KifIteration*)Newiterator(false);		
		bool first=true;
		//the first element should be the head name
		for (itr->Begin();itr->End()!=kifTRUE;itr->Next()) {		
			if (first==true) {
				name=itr->IteratorValueString();
				idname=KifNewId(name);
				if (kifcode->predicates.find(idname)==kifcode->predicates.end())
					kifcode->predicates[idname]=new KifPredicateFunction(kifcode,NULL,NULL,idname);
				pred=new KifPredicate(kifcode,NULL,idname);
				first=false;
				continue;				
			}
			//Then we proceed with the parameters...
			e=itr->IteratorValue();
			pred->parameters.push_back(e->Atom(true));
			e->Release();
		}
		itr->Release();
		pred->Putpopped(1);
		return pred;
	}
	
	if (aMapContainer()) {
		e=ValueOnKey("name");
		if (e==kifNOELEMENT)
			return kifcode->Returnerror("PRE(003): Unknown predicate name");
		name=e->String();
		if (kifGlobals->kifStringId.find(name)==kifGlobals->kifStringId.end())
			return kifcode->Returnerror("PRE(003): Unknown predicate name");
		idname=KifStringId(name);
		if (kifcode->predicates.find(idname)==kifcode->predicates.end())
			return kifcode->Returnerror("PRE(003): Unknown predicate name");

		pred=new KifPredicate(kifcode,NULL,idname);

		int sz=Size()-1;
		char buff[]={'0',0,0};
        int pos=0;
		for (int i=0;i<sz;i++) {
			if (i && (i%10)==0) {
				buff[1]='0';
                if (!pos) {
                    buff[0]='1';
                    pos=1;
                }
                else
                    buff[0]++;
			}
			e=ValueOnKey(buff);
			if (e==kifNOELEMENT)
				return kifcode->Returnerror("PRE(004): Missing index parameter in the map");
			pred->parameters.push_back(e->Atom(true));
			buff[pos]++;
		}

		pred->Putpopped(1);
		return pred;
	}

	return kifcode->Returnerror("PRE(005): Cannot build a predicate out of that object");
}


KifPredicate::KifPredicate(KifCode* klc,KifElement* kp,short n,KifType t) : name(n),KifPosition(klc,kp,t) {
	disjunction=false;
	negation=false;
	db=false;
	weight = kifONE;

#ifdef _DEBUG
	nom=KifIdString(name);
#endif
}

Exported void KifPredicateRuleElement::Addtail(KifPredicateRule* pred,int& disj) {
	if (instructions.size()==0)
		return;
	if (instructions[0]->type!=kifPredicateRuleElement) {
		if (disjunction && disj==-1)
			disj=pred->instructions.size();
		KifPredicateRuleItem* i=new KifPredicateRuleItem(kifcode,instructions[0],negation,disjunction,pred->instructions.size());
		pred->instructions.push_back(i);
	}

	if (instructions.back()->type==kifPredicateRuleElement)
		((KifPredicateRuleElement*)instructions.back())->Addtail(pred,disj);
	Release();
}

void KifPredicateRule::addfinal(KifPredicateContainer* kpc) {
	kpc->rules[name].push_back(this);
}


//-------------------------------------------------------------------------------------

bool KifPredicateVariableInstance::Insertvalue(KifDomain* dom,KifElement* val,hmap<short,KifElement*>& kept) {
	KifPredicateVariableInstance* v=(KifPredicateVariableInstance*)Variable((KifDomain*)dom);

    if (v->value!=kifNOELEMENT && v->value!=kifUNIVERSAL) {
        if (v->value->Insertvalue(dom,val,kept))
            return true;
    }
    
    kept[name]=v->value;
    v->value=val;
	val->Setreference(reference);
    return true;
}

bool KifPredicateTerm::Insertvalue(KifDomain* dom,KifElement* val,hmap<short,KifElement*>& kept) {
    KifPredicateTerm* kpt=(KifPredicateTerm*)val;
    for (int i=0;i<parameters.size();i++)
        parameters[i]->Insertvalue(dom,kpt->parameters[i],kept);
    return true;
}

bool KifVector::Insertvalue(KifDomain* dom,KifElement* val,hmap<short,KifElement*>& kept) {
    KifVector* kpt=(KifVector*)val;
    for (int i=0;i<values.size();i++)
        values[i]->Insertvalue(dom,kpt->values[i],kept);
    return true;
}

bool KifBaseMap::Insertvalue(KifDomain* dom,KifElement* val,hmap<short,KifElement*>& kept) {
    KifBaseMap* kpt=(KifBaseMap*)val;
    for (int i=0;i<values.size();i++) {
        keys[i]->Insertvalue(dom,kpt->keys[i],kept);
        values[i]->Insertvalue(dom,kpt->values[i],kept);
	}
    return true;
}

//---------------------------------------------------------------------
//Unification of the rules with the predicate variables...
KifElement* KifPredicateVariableInstance::Value(KifDomain* dom) {
	KifElement* v=this;
	while (dom->declarations.find(v->Name())!=dom->declarations.end() && v!=dom->declarations[v->Name()])
		v=dom->declarations[v->Name()];
	return ((KifPredicateVariableInstance*)v)->value;
}

KifElement* KifPredicateVariableInstance::Variable(KifDomain* dom) {
	KifElement* v=this;
	while (dom->declarations.find(v->Name())!=dom->declarations.end() && v!=dom->declarations[v->Name()])
		v=dom->declarations[v->Name()];
	return v;
}

//---------------------------------------------------------------------

Exported KifElement* KifInstructionPARAMETER::ExtractPredicateVariables(KifElement* contextualpattern,KifDomain* dom,KifElement* val,KifElement* e,bool root) {
	return instructions[0]->ExtractPredicateVariables(contextualpattern,dom,val,NULL,root);
}


Exported KifElement* KifPredicateVariable::ExtractPredicateVariables(KifElement* context,KifDomain* dom,KifElement* fromstack,KifElement* e,bool root) {
	//In this case, we are dealing with an element from the new rule to apply...
	//The fromstack is the element which has been extracted from the stack...
	hmap<short,KifPredicateVariableInstance*>* dico=context->Dico();
	KifPredicateVariableInstance* kpvi;
	if (dico->find(name)!=dico->end())
		kpvi=(*dico)[name];
	else {
		kpvi=new KifPredicateVariableInstance(kifcode,NULL,KifPredicate::nb++,label);
		(*dico)[name] = kpvi;
		kpvi->Setreference();
		dom->declarations[kpvi->Name()] = kpvi;
	}

	if (fromstack==NULL)
		return kpvi;

	return kpvi->ExtractPredicateVariables(context,dom,fromstack,NULL,root);
}

Exported KifElement* KifPredicateVariableInstance::ExtractPredicateVariables(KifElement* context,KifDomain* dom,KifElement* fromstack,KifElement* E,bool root) {
	if (fromstack->type == kifPredicateVariableInstance) {
		fromstack=fromstack->Variable(dom);
		KifElement* val=fromstack->VariableValue();
		if (val!=kifNOELEMENT) {
			if (value==kifNOELEMENT) {
				value=val->Atom(true);
				value->Setreference(reference);
			}
			else {
				if (!value->unified(dom,val))
					return NULL;
			}
		}

		if (dom->Declared(fromstack->Name())!=this) {
			dom->declarations[fromstack->Name()]=this;
			Setreference();
		}
		return this;
	}

	if (fromstack!=kifNOELEMENT) {
		if (value==kifNOELEMENT) {
			value=fromstack->Atom(true);
			value->Setreference(reference);
		}
		else {
			if (!value->unified(dom,fromstack))
				return NULL;
		}
	}

	return this;
}

Exported KifElement* KifElement::ExtractPredicateVariables(KifElement* contextualpattern,KifDomain* dom,KifElement* c,KifElement* e,bool root) {	
	if (e==NULL) {
		dom->Setfail(false);
		e=Execute(contextualpattern,dom,kifNULL);
		if (dom->Failed())
			return this;
	}
	if (c!=NULL) {
		if (c->type==kifPredicateVariableInstance) {
			KifPredicateVariableInstance* C=(KifPredicateVariableInstance*)c->Variable(dom);
			if (C->value==kifUNIVERSAL)
				return c;
			if (C->value == kifNOELEMENT) {
				//In this case, we need to duplicate this variable, in order to clean it later on...
				//Otherwise, it will keep its value for the rest of the evaluation
				short nm = C->name;
				//we create a new predicate instance, with the same label but a new name...
				C = new KifPredicateVariableInstance(kifcode, NULL, KifPredicate::nb++, C->label);
				C->Setreference();
				//we then introduce our indirection... Variables are linked in a chain.. The current name points to the new instance...
				dom->declarations[nm] = C;
				if (!e->isConst()) {
					//then we store our value...
					C->value = e->Atom(true);
					C->value->Setreference(C->reference);
				}
				else
					C->value = e;
				return e;
			}
			c=C->value;
		}

		if (e!=kifUNIVERSAL && c!=NULL && c!=kifNOELEMENT && c!=kifNULL)
			if (e->same(c)->Boolean()==false) 
				return NULL;
	}
	return e;
}

Exported KifElement* KifBaseVectorMerge::ExtractPredicateVariables(KifElement* context,KifDomain* dom,KifElement* C,KifElement* E,bool root) {
	KifElement* c=C;
	if (C!=NULL) {
		if (C->type!=kifVector) {
			if (C->type!=kifPredicateVariableInstance)
				return NULL;
			c=C->Variable(dom);
			if (c->VariableValue()!=kifNOELEMENT)
				c=c->VariableValue();
		}
	}

	int szc=-1;
	KifElement* e;
	KifElement* csend=c;
	int i;
	KifVector* localvect=NULL;
	if (c!=NULL && c->type==kifVector) {
		szc=c->Size();				
		if (szc) {
			KifElement* lst=c->Last();
			if (lst->isToMerge()) {
				localvect=new KifVector(NULL,NULL);
				for (i=0;i<szc-1;i++)
					localvect->values.push_back(c->ValueOnIndex(i));
				lst=lst->VariableValue()->ValueOnIndex(0)->VariableValue();
				for (i=0;i<lst->Size();i++)
					localvect->values.push_back(lst->ValueOnIndex(i));
				c=localvect;
				szc=c->Size();
			}
			else
				localvect=NULL;
		}
	}

	KifVector* vect=new KifVector(kifcode,NULL);
	for (i=0;i<values.size();i++) {
		e=values[i];
		if (szc!=-1) {
			//then, if it is the last element, then we associate our element with the rest of the vector
			if (i==values.size()-1) {
				if (values.size()==1)
					e=e->ExtractPredicateVariables(context,dom,c,NULL,false);
				else {
					//Else we need to extract a subset of that vector
					KifVector subvect(NULL,NULL);
					subvect.kifcode=kifcode;
					for (int j=i;j<c->Size();j++)
						subvect.values.push_back(((KifVector*)c)->values[j]);
					e=e->ExtractPredicateVariables(context,dom,&subvect,NULL,false);
				}
			}
			else {
				if (szc<=i) {
					vect->Release();
					return NULL;
				}
				e=e->ExtractPredicateVariables(context,dom,((KifVector*)c)->values[i],NULL,false);
			}
		}
		else
			e=e->ExtractPredicateVariables(context,dom,NULL,NULL,false);
		
		if (e==NULL) {
			vect->Release();
			return NULL;
		}
		vect->Push(e);
	}
	
	if (localvect!=NULL)
		delete localvect;

	KifPredicateVariableInstance* kpvi=new KifPredicateVariableInstance(kifcode,NULL,KifPredicate::nb++,"%M");

	if (type==kifBaseVectorMerge)
		kpvi->merge=true;

	kpvi->value=vect;
	kpvi->Setreference();
	dom->declarations[kpvi->name]=kpvi;

	if (csend!=NULL && csend->type==kifPredicateVariableInstance) {
        hmap<short,KifPredicateVariableInstance*>* dico=context->Dico();
		(*dico)[csend->Name()]=kpvi;
		dom->declarations[csend->Name()]=kpvi;
		kpvi->Setreference();
	}
	return kpvi;
}

Exported KifElement* KifVector::ExtractPredicateVariables(KifElement* context,KifDomain* dom,KifElement* C,KifElement* E,bool root) {
	KifElement* c=C;
	if (C!=NULL) {
		if (C->type!=kifVector) {
			if (C->type!=kifPredicateVariableInstance)
				return NULL;
			c=C->Variable(dom);
			if (c->VariableValue()!=kifNOELEMENT)
				c=c->VariableValue();
		}
	}


	int i;
	KifElement* e;
	KifElement* csend=c;
	int sz=values.size();
	int szc=-1;
	KifVector* localvect=NULL;
	if (c!=NULL && c->type==kifVector) {
		szc=c->Size();
		if (szc) {
			KifElement* lst=c->Last();
			if (lst->isToMerge()) {
				localvect=new KifVector(NULL,NULL);
				for (i=0;i<szc-1;i++)
					localvect->values.push_back(c->ValueOnIndex(i));
				lst=lst->VariableValue()->ValueOnIndex(0)->VariableValue();
				for (i=0;i<lst->Size();i++)
					localvect->values.push_back(lst->ValueOnIndex(i));
				c=localvect;
				szc=c->Size();
			}
			else
				localvect=NULL;
		}
		if (!sz || values.back()->type!=kifBaseVectorMerge)
			if (szc!=sz)
				return NULL;
	}

	KifVector* vect=new KifVector(kifcode,NULL);
	bool merge=false;
	
	for (i=0;i<sz;i++) {
		e=values[i];
		if (szc!=-1) {
			//In the case of merge, we need to extract from c the rest of the vector...
			if (e->type==kifBaseVectorMerge) {
				merge=true;
				KifVector subvect(NULL,NULL);
				subvect.kifcode=kifcode;
				for (int j=i;j<c->Size();j++)
					subvect.values.push_back(((KifVector*)c)->values[j]);
				e=e->ExtractPredicateVariables(context,dom,&subvect,NULL,false);
			}
			else {
				if (szc<=i) {
					vect->Release();
					return NULL;
				}
				e=e->ExtractPredicateVariables(context,dom,((KifVector*)c)->values[i],NULL,false);
			}
		}
		else
			e=e->ExtractPredicateVariables(context,dom,NULL,NULL,false);

		if (e==NULL) {
			vect->Release();
			return NULL;
		}
		vect->Push(e);
	}
    
	if (localvect!=NULL)
		delete localvect;

    if (root || merge) {
		KifPredicateVariableInstance* kpvi=new KifPredicateVariableInstance(kifcode,NULL,KifPredicate::nb++,"%V");
        kpvi->value=vect;
        dom->declarations[kpvi->name]=kpvi;
		kpvi->Setreference();
        if (csend!=NULL && csend->type==kifPredicateVariableInstance) {
            hmap<short,KifPredicateVariableInstance*>* dico=context->Dico();
            (*dico)[csend->Name()]=kpvi;
            dom->declarations[csend->Name()]=kpvi;
			kpvi->Setreference();
        }
        return kpvi;
    }
	return vect;
}

Exported KifElement* KifBaseMap::ExtractPredicateVariables(KifElement* context,KifDomain* dom,KifElement* C,KifElement* E,bool root) {
	KifElement* c=C;
	if (C!=NULL) {
		if (C->type!=kifBaseMap) {
			if (C->type!=kifPredicateVariableInstance)
				return NULL;
			c=C->Variable(dom);
			if (c->VariableValue()!=kifNOELEMENT)
				c=c->VariableValue();
		}
	}


	int sz=values.size();
	int szc=-1;
	if (c!=NULL && c->type==kifBaseMap) {
		szc=c->Size();
		if (!sz || values.back()->type!=kifBaseMap)
			if (szc!=sz)
				return NULL;
	}

	KifBaseMap* kmap=new KifBaseMap(kifcode,NULL,true);
	KifElement* ek;
	KifElement* ev;
	bool merge=false;
	int i;
	for (i=0;i<sz;i++) {
		ev=values[i];
		ek=keys[i];
		if (szc!=-1) {
			//In the case of merge, we need to extract from c the rest of the vector...
			if (szc<=i) {
				kmap->Release();
				return NULL;
			}
			ev=ev->ExtractPredicateVariables(context,dom,((KifBaseMap*)c)->values[i],NULL,false);
			ek=ek->ExtractPredicateVariables(context,dom,((KifBaseMap*)c)->keys[i],NULL,false);
		}
		else {
			ek=ek->ExtractPredicateVariables(context,dom,NULL,NULL,false);
			ev=ev->ExtractPredicateVariables(context,dom,NULL,NULL,false);
		}

		if (ek==NULL || ev==NULL) {
			kmap->Release();
			return NULL;
		}

		kmap->keys.push_back(ek);
		kmap->values.push_back(ev);
	}
    
    if (root || merge) {
		KifPredicateVariableInstance* kpvi=new KifPredicateVariableInstance(kifcode,NULL,KifPredicate::nb++,"%M");
        kpvi->value=kmap;
        dom->declarations[kpvi->name]=kpvi;
		kpvi->Setreference();
        if (c!=NULL && c->type==kifPredicateVariableInstance) {
            hmap<short,KifPredicateVariableInstance*>* dico=context->Dico();
            (*dico)[c->Name()]=kpvi;
            dom->declarations[c->Name()]=kpvi;
			kpvi->Setreference();
        }
        return kpvi;
    }
	return kmap;
}

Exported KifElement* KifPredicateTerm::ExtractPredicateVariables(KifElement* context,KifDomain* dom,KifElement* C,KifElement* E,bool root) {
	KifElement* c=C;
	int i;
	bool param = false;
	KifPredicateTerm* term;

	if (C != NULL) {
		if (C->type == kifPredicateTerm) {
			if (C->Name() != name || C->Size() != parameters.size()) {
				return NULL;
			}
			param = true;
		}
		else {
			if (C->type != kifPredicateVariableInstance)
				return NULL;
			c = C->Variable(dom);
			if (c->VariableValue() != kifNOELEMENT) {
				c = c->VariableValue();
				if (c->type != type || c->Name() != name || c->Size() != parameters.size())
					return NULL;
			}

			if (c->type == kifPredicateTerm)
				param = true;
		}
	}
    
	KifElement* e;
	term=new KifPredicateTerm(kifcode,NULL,name,label);
	for (i=0;i<parameters.size();i++) {
		e=parameters[i]->Frominstruction(0);
		if (param)
			e = e->ExtractPredicateVariables(context, dom, c->Parameter(i), NULL, false);
		else
			e=e->ExtractPredicateVariables(context,dom,NULL,NULL,false);
		
		if (e==NULL) {
			term->Release();
			return NULL;
		}
		term->parameters.push_back(e);
	}

    if (root) {
		KifPredicateVariableInstance* kpvi=new KifPredicateVariableInstance(kifcode,NULL,KifPredicate::nb++,"%T");
        kpvi->value=term;
		kpvi->Setreference();
        dom->declarations[kpvi->name]=kpvi;

		if (c!=NULL && c->type==kifPredicateVariableInstance) {
            hmap<short,KifPredicateVariableInstance*>* dico=context->Dico();
            (*dico)[c->Name()]=kpvi;
            dom->declarations[c->Name()]=kpvi;
			kpvi->Setreference();
        }		
        return kpvi;
    }

	return term;
}
//---------------------------------------------------------------------

//Recursive instanciation of a KifPredicateTerm based on its model
KifElement* KifPredicateDatabase::EvaluePredicateVariables(KifElement* contextualpattern,KifDomain* dom) {
	//This is a call to a database...
	//we read predicates into temporary
	short idvalue;
	KifInstructionEvaluate* context=(KifInstructionEvaluate*)contextualpattern;
	if (context->count)
		idvalue=kifGlobals->KifStringId("%countpredicate");
	else
		idvalue=kifGlobals->KifStringId("%readpredicate");
	string key=KifIdString(context->headrule->name);
	key+="(";
	for (int i=0;i<context->headrule->parameters.size();i++) {
		if (i)
			key+=",";
		if (context->headrule->parameters[i]->unifiedDeclaration(dom))
			key+=context->headrule->parameters[i]->BaseString();
		else
			key+="_";
	}
	key+=")";
	if (dejavue.find(key)!=dejavue.end()) {
		//we have already evaluated this predicate...
		if (dejavue[key]==_PREDNONE)
			return kifFALSE;
		if (context->count || dejavue[key]==_PREDLOADED)
			return kifTRUE;
	}

	KifCallFunction call(NULL,NULL,idvalue);
	call.kifcode=kifcode;
	database->ExecMethod(idvalue,contextualpattern,dom,context->threadowner,&call);
	if (context->count) {
		if (context->value==NULL || context->value->Integer()==0) {
			dejavue[key]=_PREDNONE;
			return kifFALSE;
		}
		dejavue[key]=true;
		return kifTRUE;
	}
	if (context->results.size()==0) {
		dejavue[key]=_PREDNONE;
		return kifFALSE;
	}

	dejavue[key]=_PREDLOADED;
	return kifTRUE;
}
//---------------------------------------------------------------------

KifPredicate* KifPredicateKnowledgeBaseFunction::Duplicate(KifElement* context, KifDomain* dom) {
	KifElement * e;
    
	KifPredicateKnowledgeBaseFunction* p;
    
	if (context==NULL && dom==NULL) {
		p = new KifPredicateKnowledgeBaseFunction(kifcode, NULL, action);
		p->name = name;
		p->weight = kifcode->Providefloat(weight->Float());
		for (int i=0;i<parameters.size();i++) {
			e=parameters[i]->Atom(true);
			p->parameters.push_back(e);
		}
		p->Setreference();
		return p;
	}
    
	p = new KifPredicateKnowledgeBaseFunction(NULL, NULL, action);
	p->name = name;
	p->weight = kifcode->Providefloat(weight->Float());

	p->kifcode=kifcode;
	int i,j;
	for (i=0;i<parameters.size();i++) {
		e=parameters[i]->Frominstruction(0)->ExtractPredicateVariables(context,dom,NULL,NULL,true);
		if (e==NULL) {
			for (j=0;j<i;j++) {
				//These elements will be cleared in KifPredicateRule::Evalue
				if (p->parameters[j]->type==kifPredicateVariableInstance)
					p->parameters[j]=kifNULL;
			}
			p->Release();
			return NULL;
		}
        
		p->parameters.push_back(e);
	}
    
	p->Setreference();
	return p;
}

Exported KifPredicate* KifPredicate::Duplicate(KifElement* context, KifDomain* dom) {
	KifElement * e;

	KifPredicate* p;

	if (context == NULL && dom == NULL) {
		p = new KifPredicate(kifcode, NULL, name);
		p->weight = kifcode->Providefloat(weight->Float());
		for (int i = 0; i<parameters.size(); i++) {
			e = parameters[i]->Atom(true);
			p->parameters.push_back(e);
		}
		p->Setreference();
		return p;
	}

	p = new KifPredicate(NULL, NULL, name);
	p->weight = kifcode->Providefloat(weight->Float());

	p->kifcode = kifcode;
	int i, j;
	for (i = 0; i<parameters.size(); i++) {
		e = parameters[i]->Frominstruction(0)->ExtractPredicateVariables(context, dom, NULL, NULL, true);
		if (e == NULL) {
			for (j = 0; j<i; j++) {
				//These elements will be cleared in KifPredicateRule::Evalue
				if (p->parameters[j]->type == kifPredicateVariableInstance)
					p->parameters[j] = kifNULL;
			}
			p->Release();
			return NULL;
		}

		p->parameters.push_back(e);
	}

	p->Setreference();
	return p;
}

Exported KifPredicate* KifPredicate::Copyfrom(KifElement* context,KifDomain* dom,KifPredicate* C) {
	KifElement * e;
    
	KifPredicate* p=new KifPredicate(NULL,NULL,name);
	p->kifcode=kifcode;
	p->weight = kifcode->Providefloat(weight->Float());

    int i;
	for (i=0;i<parameters.size();i++) {
		e = parameters[i]->Frominstruction(0)->ExtractPredicateVariables(context, dom, C->parameters[i], NULL, true);
		if (e == NULL) {
			delete p;
			return NULL;
		}

		e->Setreference();
		p->parameters.push_back(e);
	}	
    
	p->weight->reference++;
	p->reference++;
	return p;
}

//---------------------------------------------------------------------


Exported KifElement* KifInstructionPARAMETER::EvaluePredicateVariables(KifElement* context,KifDomain* dom) {
	return instructions[0]->EvaluePredicateVariables(context,dom);
}

Exported KifElement* KifElement::EvaluePredicateVariables(KifElement* context,KifDomain* dom) {
	KifElement* e=Execute(context,dom);
	return e->Atom(true);
}

Exported KifElement* KifPredicateVariable::EvaluePredicateVariables(KifElement* context,KifDomain* dom) {
	KifPredicateVariableInstance* kpi=new KifPredicateVariableInstance(kifcode,NULL,KifPredicate::nb,label);
	dom->declarations[KifPredicate::nb]=kpi;
	KifPredicate::nb++;
	return kpi;
}

Exported KifElement* KifPredicateVariableInstance::EvaluePredicateVariables(KifElement* context,KifDomain* dom) {
	KifElement* v=Variable(dom);

	if (v->VariableValue()!=kifNOELEMENT)
		return v->VariableValue();
	return v;
}

Exported KifElement* KifVector::EvaluePredicateVariables(KifElement* context,KifDomain* dom) {
	KifElement* e;

	KifVector* vect=new KifVector(kifcode,NULL);
	for (int i=0;i<values.size();i++) {
		e=values[i]->EvaluePredicateVariables(context,dom);
		if (e==NULL)
			vect->Push(values[i]);
		else
			vect->Push(e);
	}
	return vect;
}

Exported KifElement* KifBaseMap::EvaluePredicateVariables(KifElement* context,KifDomain* dom) {
	KifElement* e;

	KifBaseMap* kmap=new KifBaseMap(kifcode,NULL,true);
	for (int i=0;i<values.size();i++) {
		e=keys[i]->EvaluePredicateVariables(context,dom);
		if (e==NULL)
			kmap->keys.push_back(values[i]);
		else
			kmap->keys.push_back(e);

		e=values[i]->EvaluePredicateVariables(context,dom);
		if (e==NULL)
			kmap->values.push_back(values[i]);
		else
			kmap->values.push_back(e);
	}
	return kmap;
}

KifElement* KifPredicateTerm::EvaluePredicateVariables(KifElement* context,KifDomain* dom) {
	KifElement * e;
	KifPredicateTerm* model=new KifPredicateTerm(kifcode,NULL,name,label);

	for (int i=0;i<parameters.size();i++) {
		e=parameters[i]->EvaluePredicateVariables(context,dom);
		if (e==NULL)
			model->parameters.push_back(parameters[i]);
		else
			model->parameters.push_back(e);
	}
	return model;
}
//---------------------------------------------------------------------

KifPredicate* KifInstructionEvaluate::PredicateUnification(VECTA<KifElement*>& stack,int& posreplace,int& from) {
	posreplace=-1;
	from=0;
	KifPredicate* headpredicate=NULL;
	KifElement* e;
	KifElement* res;
	bool keep=false;
	bool test;
	
	for (int i=0;i<stack.size();i++) {
		e=stack[i];
		if (e->type==kifPredicate) {
			if (e==kifFAIL)
				return kifFAIL;

			if (e==kifCUT)
				test=false;
			else //unifiedDeclaration tests whether the current predicate is fully "unifiable": all variables have been unified...
				test=((KifPredicate*)e)->unifiedDeclaration(dom);

			if (test) {
				//If it is an action, such as assert then we return NULL
				short eval = ((KifPredicate*)e)->kbaction();
				if (eval && (eval == idasserta || eval == idassertdb || eval == idassertz)) {
					headpredicate = (KifPredicate*)e;
					from = i + 1;
					return headpredicate;
				}

				//We then evaluate it... This function returns 3 values:
				// 0 is fail
				// 1 is not enough data
				// 2 is sucess
				eval=kifGlobals->isaValidPredicate(dom,((KifPredicate*)e),rules);
				if (e->Negation()) {
					//if we have a match in the knowledge base, then it is an error
					if (eval!=0)
						return kifFAIL;
				}
				else {
					if (eval==0)
						return kifFAIL;
					else
						if (eval==1)
							test=false;
				}
			}

			if (!test) { //still some variables to go
				if (headpredicate==NULL) {
					headpredicate=(KifPredicate*)e;
					posreplace=i;
					keep=true;
				}
			}
			else {
				if (!keep) //If we have not found anything yet as a head, we will start the evaluation from the following element...
					from = i + 1;
			}
		}
		else
			if (headpredicate==NULL) {
				res=e->Exec(this,dom,kifNULL,threadowner,callfunc);
				if (res->Boolean()==false) {
					if (!e->Negation() && !dom->Failed())
						return kifFAIL;
					keep=true;
				}
				else {
					if (e->Negation())
						return kifFAIL;
					if (!keep)
						from=i+1;
				}
			}
			else
				e->setSucessfull(false);
	}
	return headpredicate;
}

//-------------------------------------------------------------------
KifElement* KifElement::Getvalues(KifDomain* dom,bool duplicate) {
	if (duplicate)
		return Atom(true);
	return this;
}

KifElement* KifPredicateVariableInstance::Getvalues(KifDomain* dom,bool duplicate) {
	KifElement* v=Value(dom);
    v=v->Getvalues(dom,duplicate);
	if (merge && v->type==kifVector)
		v->type=kifBaseVectorMerge;
	return v;
}

KifElement* KifPredicateTerm::Getvalues(KifDomain* dom,bool duplicate) {
    KifPredicateTerm* term=new KifPredicateTerm(kifcode,NULL,name,label);
    KifElement* e;
	for (int i=0;i<parameters.size();i++) {
        e=parameters[i]->Getvalues(dom,duplicate);
		if (e==kifNOELEMENT) {
			term->Release();
			return kifNOELEMENT;
		}
        term->parameters.push_back(e);
    }
    return term;
}

KifElement* KifVector::Getvalues(KifDomain* dom,bool duplicate) {
    KifVector* kvect=new KifVector(kifcode,NULL);
    KifElement* e;
	for (int i=0;i<values.size();i++) {
        e=values[i]->Getvalues(dom,duplicate);
		if (e==kifNOELEMENT) {
			kvect->Release();
			return kifNOELEMENT;
		}
		if (e==NULL || e==kifUNIVERSAL)
			continue;
		if (e->type==kifBaseVectorMerge) {
			e=e->ValueOnIndex(0);
			if (e->type==kifVector) {
				KifVector* vect=(KifVector*)e;
				for (int v=0;v<vect->values.size();v++)
					kvect->Push(vect->values[v]);
				e->Resetreference();
			}
		}
		else
			kvect->Push(e);
    }
    return kvect;
}

KifElement* KifBaseMap::Getvalues(KifDomain* dom,bool duplicate) {
    KifMap* kmap=new KifMap(kifcode,NULL);
    KifElement* ev;
	KifElement* ek;
	for (int i=0;i<values.size();i++) {
        ev=values[i]->Getvalues(dom,duplicate);
		if (ev==kifNOELEMENT) {
			kmap->Release();
			return kifNOELEMENT;
		}
		if (ev==NULL || ev==kifUNIVERSAL)
			continue;

        ek=keys[i]->Getvalues(dom,duplicate);
		if (ek==kifNOELEMENT) {
			ev->Release();
			kmap->Release();
			return kifNOELEMENT;
		}
		if (ek==NULL || ek==kifUNIVERSAL)
			continue;

		kmap->Push(STR(ek->String()),ev);
		ek->Release();
    }
    return kmap;
}

KifElement* KifInstructionEvaluate::PredicateCreate(KifPredicate* headrule,int depth) {
    KifPredicate* pv=new KifPredicate(kifcode,NULL,headrule->name);
    KifElement* e;
    int i;
	for (i=0;i<headrule->parameters.size();i++) {
        e=headrule->parameters[i]->Getvalues(dom,true);
		if (e==kifNOELEMENT) {
			pv->Release();
			return kifFALSE;
		}
        pv->parameters.push_back(e);
    }

	if ((kifcode->checkprobabilities & PRED_WEIGHTSTACK ) == PRED_WEIGHTSTACK && weightstacks.size()) {
		if (kifcode->weightfinalfunction != NULL) {
			KifVectorFloat* kvect = new KifVectorFloat(kifcode, NULL);
			kvect->Setreference();
			kvect->values = weightstacks;
			KifFunction* thefunction = (KifFunction*)kifcode->weightfinalfunction;
			KifCallFunctionGeneral kcall(NULL, NULL, thefunction->name, thefunction);
			kcall.Setline(thefunction->Line(), thefunction->Fileindex());
			kcall.kifcode = kifcode;
			kcall.parameters.push_back(kvect);
			pv->weight = kcall.Exec(this, dom, kifNULL, threadowner, callfunc);
			//We then copy the values from kvect (the function parameter) back
			//This is a way to modify the values within the function, in order to
			//change the weights back in the stack...
			if (weightstacks.size() == kvect->values.size())
				weightstacks = kvect->values;
			kvect->Resetreference();
		}
		else {
			//the default value...
			float v = 1;
			for (i = 0; i < weightstacks.size(); i++)
				v *= weightstacks[i];
			v /= weightstacks.size();
			pv->weight = kifcode->Providefloat(v);
		}
	}

	if (trace) {
		stringstream s;
		for (i=0;i<depth;i++)
			s<<" ";
		if (pv->weight->Float()!=1)
			s << "success:" << depth << "(w=" << pv->weight->Float() << ")=" << pv->String() << endl;
		else
			s << "success:" << depth << "=" << pv->String() << endl;
		PreLocalPrint(s.str());
	}

    pv->Setreference();

    if (headrule->Returnfunction()!=NULL) {
		KifPredicateFunction* hfunc=(KifPredicateFunction*)headrule;
        KifFunction* func=hfunc->function->Functioncall(dom);
        KifCallFunctionLocal kfunc(func->kifcode,func->Name(),func);
		hfunc->object->Setreference();
        kfunc.parameters.push_back(pv);
        kfunc.parameters.push_back(hfunc->object);
        e=kfunc.Execute(this,dom,kifNULL);
		hfunc->object->Resetreference();
        if (!e->Boolean()) {
            pv->Resetreference();
            return kifBREAK;
        }
    }
    
    results.push_back(pv);
	return kifTRUE;
}

//-----------------------------------------------------
//-----------------------------------------------------
void Displaypredicatestack(char ty, KifDomain* dom, KifPredicate* kpr, VECTA<KifElement*>& stack, int depth, KifInstructionEvaluate* ctx) {
    //AffichageDom(dom,depth);
    stringstream s;
    bool displayed=false;
	bool first=true;
	KifElement* se;
    int i,j,sz=stack.size();
	char sep=',';
	for (i=-1;i<sz;i++) {
		if (i==-1)
			se=kpr;
		else
			se=stack[i];
        if (se==NULL)
            continue;
		if (se->type==kifPredicate) {
			if (!displayed) {
				for (j=0;j<depth;j++)
					s<<" ";
				if ((ctx->kifcode->checkprobabilities & PRED_WEIGHTSTACK) == PRED_WEIGHTSTACK && ctx->weightstacks.size())
					s << ty << ":" << depth << "(w=" << ctx->weightstacks.back() << ")=";
				else
					s << ty << ":" << depth << "=";
			}
			
			if (first==false)
				s<<sep;
			if (i!=-1)
				first=false;
			displayed=true;
			KifPredicate* kpi=(KifPredicate*)se;
			string n=KifIdString(kpi->name);
			if (kpi->negation)
				s<<"~";
			s<<n;
			if (kpi==kifCUT || kpi==kifFAIL)
				continue;
			s<<"(";
			for (j=0;j<kpi->parameters.size();j++) {
				n=kpi->parameters[j]->String();
				if (j)
					s<<",";
				s<<n;
			}
			s<<")";
		}
		else {
			if (!displayed) {
				for (j=0;j<depth;j++)
					s<<" ";
				s<<ty<<":"<<depth<<"=";
			}
			if (first==false)
				s<<sep;
			if (i!=-1)
				first=false;
			displayed=true;
            if (se->Name()!=-1 || (se->Action()>=kifAFFECTATION && se->Action()<=kifMINUSMINUS))
                s<<se->String();
            else
                s<<"[i]";
		}
		if (i==-1) {
			if (sz)
				s<<" --> ";
			else
				s<<".";
		}
		sep=',';
		if (se->Disjunction())
			sep=';';
	}

	if (displayed)
		PreLocalPrint(s.str());
}
//---------------------------------------------------------------------

class predictvalue {
public:
	int idthread;
    VECTA<KifElement*> localstack;
    VECTA<KifElement*> basestack;
	vector<KifPredicate*> kbase;
    hmap<short,KifElement*> kept;
	predictvalue(int id) {
		idthread = id;
		kifGlobals->THR[idthread]->prologstack++;
	}

	~predictvalue() {
		kifGlobals->THR[idthread]->prologstack--;
	}
};

#ifndef C11
class o_predictvalue {
public:
    predictvalue* r;
    

    o_predictvalue(int id) {
        r=new predictvalue(id);
    }
    
    ~o_predictvalue() {
		delete r;
    }
};
#endif


bool KifCompareWeight(KifPredicate* p1, KifPredicate* p2) {
	if (p1->weight->Float() > p2->weight->Float())
		return true;
	return false;
}

bool KifCompareHeadWeight(KifPredicateRule* p1, KifPredicateRule* p2) {
	if (p1->head->weight->Float() > p2->head->weight->Float())
		return true;
	return false;
}

KifElement* KifInstructionEvaluate::PredicateEvalue(VECTA<KifElement*>& stack, KifPredicate* currenthead, bool cut, int depth) {
	int i;
    size_t sz=stack.size();		
    
	if (Errorid(threadowner))
        return kifFALSE;
    
#ifdef C11
	std::unique_ptr<predictvalue> Oo(new predictvalue(threadowner));
#else
	o_predictvalue o(threadowner);
	predictvalue* Oo = o.r;
#endif

	KifElement* res;
	if ((kifcode->checkprobabilities & PRED_WEIGHTSTACK ) == PRED_WEIGHTSTACK && weightstacks.size()) {
		if (kifcode->weightaiguillagefunction != NULL) {
			KifVectorFloat* kvect = new KifVectorFloat(kifcode, NULL);
			kvect->Setreference();
			kvect->values = weightstacks;
			KifFunction* thefunction = (KifFunction*)kifcode->weightaiguillagefunction;
			KifCallFunctionGeneral kcall(NULL, NULL, thefunction->name, thefunction);
			kcall.Setline(thefunction->Line(), thefunction->Fileindex());
			kcall.kifcode = kifcode;
			kcall.parameters.push_back(kvect);
			res = kcall.Exec(this, dom, kifNULL, threadowner, callfunc);
			if (res->Boolean() == false) {
				res->Release();
				return kifFALSE;
			}
			res->Release();
		}
	}

	res=kifFALSE;
	//If the top element in the stack is a disjunction, we then evaluate the next one...
	//The current one will be evaluate after... The disjunction is evaluated backward...
	if (sz && stack[0]->Disjunction()) {
		Oo->localstack.ajuste(sz);
		for (i=1;i<sz;i++)
			Oo->localstack.vecteur[i-1]=stack.vecteur[i];
		Oo->localstack.dernier=sz-1;
		res=PredicateEvalue(Oo->localstack,currenthead,cut,depth);
		if (res==kifBREAK)
			return kifBREAK;
		if (cut && res==kifTRUE)
			return kifTRUE;
	}


	int j,posreplace;
	int from;

	KifPredicate* headpredicate=PredicateUnification(stack,posreplace,from);
	//If the unification went wrong...
	if (headpredicate==kifFAIL) {
		if (trace) {
			stringstream scerr;
            scerr<<" --> Fail"<<endl;
			PreLocalPrint(scerr.str());
		}
		return res;
    }
    
	//It is already all unified...
	if (headpredicate==NULL) {
		if (trace)
			PreLocalPrint(Endl);
		for (j=from;j<sz;j++) {
			if (!stack[j]->hasbeenSuccesfull())
				return res;
		}
		res=PredicateCreate(headrule,depth);
		return res;
	}
        
	short kbaction = headpredicate->kbaction();
	if (kbaction == idasserta || kbaction == idassertz || kbaction == idassertdb) {
		KifPredicate* v;
		if (kbaction == idassertdb) {
			v = new KifPredicateVar(kifcode, NULL, headpredicate->name);
			for (i = 0; i < headpredicate->parameters.size(); i++) {
				res = headpredicate->parameters[i]->Exec(this, dom, kifNULL, threadowner, callfunc);
				v->parameters.push_back(res->Atom(true));
			}

			res = ((KifPredicateKnowledgeBaseFunction*)headpredicate)->dbase->Exec(this, dom, kifNULL, threadowner, callfunc);
			v->Store(res, dom, threadowner);
			v->Release();
		}
		else {
			v = new KifPredicate(kifcode, NULL, headpredicate->name);
			v->weight = headpredicate->weight->Exec(kifNULL, dom, kifNULL, threadowner, callfunc);

			for (i = 0; i < headpredicate->parameters.size(); i++) {
				res = headpredicate->parameters[i]->Exec(this, dom, kifNULL, threadowner, callfunc);
				v->parameters.push_back(res->Atom(true));
			}

			if (kbaction == idasserta)
				kifGlobals->storePredicate(dom, v, false);
			else
				kifGlobals->storePredicate(dom, v, true);
		}
		for (j = from; j<sz; j++) {
			if (j == posreplace || stack[j]->hasbeenSuccesfull())
				continue;
			Oo->localstack.push_back(stack[j]);
		}
		return PredicateEvalue(Oo->localstack, currenthead, cut, depth);
	}

	if (headpredicate==kifCUT) {
		for (j=from;j<sz;j++) {
			if (j==posreplace || stack[j]->hasbeenSuccesfull())
				continue;
			Oo->localstack.push_back(stack[j]);
		}
		res=PredicateEvalue(Oo->localstack,currenthead,true,depth);
		if (res==kifTRUE)
			return kifBREAK;
		return res;
	}
    
	if (trace)
		PreLocalPrint(Endl);
    
	//Some variables still need to be evaluated
	
	//We will try to solve headpredicate...
	//first it could be part of the knowledge base
    
    //We only keep the elements that we need
	bool skip=false;
    for (j=from;j<sz;j++) {
        if (stack[j]->hasbeenSuccesfull())
            continue;
		if (skip) {
			if (!stack[j]->Disjunction())
				skip=false;
			continue;
		}
        if (j==posreplace)
            posreplace=Oo->basestack.size();

		if (stack[j]->Disjunction())
			skip=true;
        Oo->basestack.push_back(stack[j]);
    }
    
	KifElement* localres;	
	bool check = false;

	if (kifGlobals->GetPredicates(dom,headpredicate,Oo->kbase,false)) {
		//we set the res value to kifTRUE, which will be considered as an error
		if (headpredicate->Negation()) {
			res=kifTRUE;
			Oo->kbase.clear();
		}
        Oo->basestack.erase(posreplace);
		Oo->localstack.resize(Oo->basestack.size());
		Oo->localstack.dernier=Oo->basestack.size();
        sz=Oo->kbase.size();
		int ix;
		//this normalization is done for all arcs stemming from here...
		if ((kifcode->checkprobabilities & PRED_NORMALIZE_WEIGHT) == PRED_NORMALIZE_WEIGHT) {
			double summing = 0;			
			for (ix = 0; ix < Oo->kbase.size(); ix++)
				summing += Oo->kbase[ix]->weight->Float();

			if (summing != 0 && summing!=1) {
				//in this case, we normalized each value from the base...
				double v;
				for (ix = 0; ix < Oo->kbase.size(); ix++) {
					v = Oo->kbase[ix]->weight->Float()/summing;
					Oo->kbase[ix]->weight->Resetreference(Oo->kbase[ix]->reference);
					Oo->kbase[ix]->weight = kifcode->Providefloat(v);
				}
			}
		}

		//we sort the values according to some random algorithm
		if ((kifcode->checkprobabilities & PRED_RANDOMIZE) == PRED_RANDOMIZE)
			shufflepredicates(Oo->kbase);

		//If we have an instruction such as ponderate or retract, we do not want the weights to interfere.
		//they should apply to all elements in the knowledge base...
		if (!kbaction && sz && (kifcode->checkprobabilities & PRED_WEIGHT ) == PRED_WEIGHT) {
			if ((kifcode->checkprobabilities & PRED_RANDOMIZE) == PRED_RANDOMIZE)
				check = true;
			else {
				for (ix = 0; ix < Oo->kbase.size(); ix++) {
					if (Oo->kbase[ix]->weight->Float() != 1) {
						check = true;
						sort(Oo->kbase.begin(), Oo->kbase.end(), KifCompareWeight);
						break;
					}
				}
			}
		}
		double wval;
		//we remove our element at the position posreplace in stack
		for (i=0;i<sz;i++) {
            
			//we then unify our values in dom with respect to the element in the stack (stored in headpredicate) and the extracted elements
			for (j=0;j<headpredicate->parameters.size();j++)
				headpredicate->parameters[j]->Insertvalue(dom,Oo->kbase[i]->parameters[j],Oo->kept);

            //It is a copy
            //Oo->kbase[i]->Release();
            
			for (j = 0; j < Oo->basestack.size(); j++)
				Oo->localstack.vecteur[j]=Oo->basestack.vecteur[j];
            
            if (trace)
                Displaypredicatestack('k',dom,headpredicate,Oo->localstack,depth,this);
			
			if ((kifcode->checkprobabilities & PRED_WEIGHTSTACK) == PRED_WEIGHTSTACK) {
				wval = Oo->kbase[i]->Float();
				weightstacks.push_back(wval);
			}

			nbpredicates++;
			localres=PredicateEvalue(Oo->localstack,currenthead,cut,depth+1);
			nbpredicates--;
			if ((kifcode->checkprobabilities & PRED_WEIGHTSTACK) == PRED_WEIGHTSTACK) {				
				//if the weight has been changed in the final weight stack function, then we modify 
				//the knowledgebase element weight...
				if (wval != weightstacks.back()) {
					if (Oo->kbase[i]->weight->type == kifFloat)
						((KifFloat*)Oo->kbase[i]->weight)->value = weightstacks.back();
					else {
						Oo->kbase[i]->weight->Resetreference(Oo->kbase[i]->reference);
						Oo->kbase[i]->weight = kifcode->Providefloat(weightstacks.back());
						Oo->kbase[i]->weight->Setreference(Oo->kbase[i]->reference);
					}
				}
				weightstacks.pop_back();
			}

			if (localres==kifTRUE) {
				if (kbaction) {
					KifElement* e;
					//we have an action to process: asssert etc...
					switch (kbaction) {
					case idponder:
						e = headpredicate->weight->Exec(kifNULL, dom, kifNULL, threadowner, callfunc);
						Oo->kbase[i]->weight->Resetreference(Oo->kbase[i]->reference);
						Oo->kbase[i]->weight = e;
						e->Setreference(Oo->kbase[i]->reference);
						break;
					case idretract:
						kifGlobals->RemoveThePredicate(dom, Oo->kbase[i]);
						break;
					case idretractdb:
						e = ((KifPredicateKnowledgeBaseFunction*)headpredicate)->dbase->Exec(this, dom, kifNULL, threadowner, callfunc);
						Oo->kbase[i]->Remove(e, dom, threadowner);
					}
				}
				if (cut || check) {					
                    for (it=Oo->kept.begin();it!=Oo->kept.end();it++)
                        ((KifPredicateVariableInstance*)dom->declarations[it->first])->Cleans(it->second,false);
					return localres;
				}
				else
					res=localres;
			}

			//We clean them, then...
			for (it=Oo->kept.begin();it!=Oo->kept.end();it++)
				((KifPredicateVariableInstance*)dom->declarations[it->first])->Cleans(it->second,true);

			Oo->kept.clear();
            
            if (localres==kifBREAK)
                return localres;
            
			if (Errorid(threadowner))
				return kifFALSE;
		}
        Oo->basestack.insert(posreplace,headpredicate);
	}
    
   
	//Else there might a rule somewhere to do the trick
	if (rules.find(headpredicate->name)==rules.end()) {
		if (headpredicate->Negation()) {
			if (res==kifFALSE) {
				//we then continue to analyse the rest of the stack
				from++;
				Oo->localstack.raz();
                sz=stack.size();
				for (j=from;j<sz;j++) {
					if (stack[j]->hasbeenSuccesfull())
						continue;
					Oo->localstack.push_back(stack[j]);
				}
				return PredicateEvalue(Oo->localstack,currenthead,cut,depth+1);
			}
			return kifFALSE;
		}
		return res; //no rule available
	}
    
	//we remove the predicate to analyze
	vector<KifPredicateRule*>& rulebase=rules[headpredicate->name];

	//If there is a negation on the headpredicate, then we test it individually
	if (headpredicate->Negation()) {
        
		//if the value kifTRUE was found before, then it is an error...
		if (res==kifTRUE)
			return kifFALSE;
        
        sz=rulebase.size();
        Oo->localstack.raz();
		for (i=0;i<sz;i++) {
			if (Errorid(threadowner))
				return kifFALSE;
            
			Oo->localstack.dernier=1;
			Oo->localstack.vecteur[0]=headpredicate;
            
			//We set cut to TRUE, since we need only one true answer to stop...
			localres=rulebase[i]->Evalue(this,currenthead,Oo->localstack,posreplace-from,depth,true);
			//if we have one succes, then it is an error
			if (localres==kifTRUE)
				return kifFALSE;
		}
		//then we clear it from our stack, and we launch it all over again on the remaining elements
		from++;
		Oo->localstack.raz();
        sz=stack.size();
		for (j=from;j<sz;j++) {
			if (stack[j]->hasbeenSuccesfull())
				continue;
			Oo->localstack.push_back(stack[j]);
		}
		return PredicateEvalue(Oo->localstack,currenthead,cut,depth+1);
	}
    
    
    sz=rulebase.size();
	check = sorted[headpredicate->name];

	//this normalization is done for all arcs stemming from here...
	if ((kifcode->checkprobabilities & PRED_NORMALIZE_WEIGHT )== PRED_NORMALIZE_WEIGHT) {
		double summing = 0;
		for (i = 0; i < sz; i++)
			summing += rulebase[i]->head->weight->Float();

		if (summing != 0 && summing != 1) {
			//in this case, we normalized each value from the base...
			double v;
			for (i = 0; i < sz; i++) {
				v = rulebase[i]->head->weight->Float() / summing;
				rulebase[i]->head->weight->Resetreference(rulebase[i]->head->reference);
				rulebase[i]->head->weight = kifcode->Providefloat(v);
			}
		}
	}

	//we sort the values according to some random algorithm
	if ((kifcode->checkprobabilities & PRED_RANDOMIZE) == PRED_RANDOMIZE)
		shufflerules(rulebase);


	for (i=0;i<sz;i++) {
		
		Oo->localstack=Oo->basestack;
		localres=rulebase[i]->Evalue(this,currenthead,Oo->localstack,posreplace,depth,cut);
        
        if (localres==kifBREAK)
            return localres;
		
		if (localres==kifTRUE) {
			if (cut || check)
				return localres;
			res=localres;
		}
        if (Errorid(threadowner))
            return kifFALSE;
	}
    
	return res;
}


class rulevalue {
public:
    hmap<short,KifPredicateVariableInstance*> currentdico;
    VECTA<KifElement*> localstack;
	hmap<short,KifElement*> localdom;
    rulevalue(hmap<short,KifElement*>& d) {
        localdom=d;
    }
};

#ifndef C11
class o_rulevalue {
public:
    rulevalue* r;
    
    o_rulevalue(hmap<short,KifElement*>& d) {
        r=new rulevalue(d);
    }
    
    ~o_rulevalue() {
        delete r;
    }
};
#endif

Exported KifElement* KifPredicateRule::Evalue(KifInstructionEvaluate* contextualpattern,KifPredicate* headhead,VECTA<KifElement*>& stack,int pos,int depth,bool cut) {
	
    KifPredicate* currenthead=(KifPredicate*)stack[pos];
    if (head->parameters.size()!=currenthead->parameters.size())
        return kifFALSE;

	KifDomain* dom = contextualpattern->dom;

#ifdef C11
	std::unique_ptr<rulevalue> Oo(new rulevalue(dom->declarations));
#else
	o_rulevalue oo(dom->declarations);
	rulevalue* Oo = oo.r;
#endif
    
	//Variable management
	contextualpattern->dico=&Oo->currentdico;
    
	if (contextualpattern->trace)
		Displaypredicatestack('r', dom, head, stack, depth, contextualpattern);
    stack.erase(pos);
    
	hmap<short,KifElement*>::iterator& it=contextualpattern->it;
	//newhead is not stored in the garbage collector, no need for that. Hence, we only need to destroy it to get rid of it...
	KifPredicate* newhead=head->Copyfrom(contextualpattern,dom,currenthead);
	if (newhead==NULL) {        
		for (it=dom->declarations.begin();it!=dom->declarations.end();it++) {
			if (Oo->localdom.find(it->first)==Oo->localdom.end() || Oo->localdom[it->first]!=it->second) {
				it->second->Putpopped(0);
				it->second->Resetreference();
			}
		}
		dom->declarations=Oo->localdom;
		if (contextualpattern->trace) {
			stringstream scerr;
			scerr<<" --> Fail"<<endl;
			PreLocalPrint(scerr.str());
		}
		return kifFALSE;
	}
    
	if (contextualpattern->trace)
		PreLocalPrint(Endl);
    
    //Oo->toclean.push_back(newhead);
    
    KifElement* res=kifTRUE;
    
    if (instructions.size()!=0) {
        KifPredicate* kpred;
		KifPredicateLocalInstruction* kpredi;
        KifPredicateRuleItem* ee;
        int i;
        for (i=0;i<instructions.size();i++) {
            ee=(KifPredicateRuleItem*)instructions[i];
            if (ee->item->type==kifPredicate) {
				//In that case, it is worth noting that the object IS NOT STORED in the garbage collector
				//The same applies to kpredi below... We will then be able to destroy them in one single pass
                kpred=((KifPredicate*)ee->item)->Duplicate(contextualpattern,dom);
                if (kpred==NULL)  {
                    res=kifFALSE;
                    break;
                }
				if (kpred->name==idthecut)
					Oo->localstack.push_back(kifCUT);
				else
					if (kpred->name==idfail)
						Oo->localstack.push_back(kifFAIL);
					else {
						//Oo->toclean.push_back(kpred);
						kpred->set(ee->negation,ee->disjunction);
						Oo->localstack.push_back(kpred);
					}
            }
            else {
				kpredi=new KifPredicateLocalInstruction(kifcode,&Oo->currentdico,ee->item,ee->negation,ee->disjunction);
                //Oo->toclean.push_back(kpredi);
				Oo->localstack.push_back(kpredi);
            }
        }
        
		if (res==kifTRUE) {
			for (i=Oo->localstack.size()-1;i>=0;i--)
				stack.insert(pos,Oo->localstack[i]);
		}
    }
    
	if (res==kifTRUE) {
        if (contextualpattern->trace)
			Displaypredicatestack('e', dom, newhead, stack, depth, contextualpattern);

		double wval;
		if ((kifcode->checkprobabilities & PRED_WEIGHTSTACK) == PRED_WEIGHTSTACK) {
			wval = newhead->weight->Float();
			contextualpattern->weightstacks.push_back(wval);
		}


		res=contextualpattern->PredicateEvalue(stack,newhead,cut,depth+1);
		if ((kifcode->checkprobabilities & PRED_WEIGHTSTACK) == PRED_WEIGHTSTACK) {
			//if the weight has been changed in the final weight stack function, then we modify 
			//the rule weight...
			if (wval != contextualpattern->weightstacks.back()) {
				if (head->weight->type == kifFloat)
					((KifFloat*)head->weight)->value = contextualpattern->weightstacks.back();
				else {
					head->weight->Resetreference(newhead->reference);
					head->weight = kifcode->Providefloat(contextualpattern->weightstacks.back());
					head->weight->Setreference(newhead->reference);
				}
			}
			contextualpattern->weightstacks.pop_back();
		}
    }
    
    newhead->Clean();
	delete newhead;
	for (int i=0;i<Oo->localstack.size();i++) {
		if (Oo->localstack[i]==kifCUT || Oo->localstack[i]==kifFAIL)
			continue;
        Oo->localstack[i]->Clean();
		delete Oo->localstack[i];
	}
    
    for (it=dom->declarations.begin();it!=dom->declarations.end();it++) {
        if (Oo->localdom.find(it->first)==Oo->localdom.end() || Oo->localdom[it->first]!=it->second) {
			it->second->Putpopped(0);
            it->second->Resetreference();
		}
    }
    
    dom->declarations=Oo->localdom;
	return res;
}
//---------------------------------------------------------------------------------------------------------------------

KifPredicateRuleItem::KifPredicateRuleItem(KifCode* klc,KifElement* kp,bool n,bool d,int i) : KifElement(klc,NULL,kifPredicateRuleElement) {
	negation=n;
	disjunction=d;
	position=i;
	item=kp;
}

Exported bool KifPredicateVariable::Setvalue(KifElement* ke,KifElement* index,KifElement* dom,int idthread,KifCallFunction* callfunc) {
	if (dom->Failed())
		return true;

    KifElement* d=dom->Declared(PREDICATEDICO);
    if (d==NULL)
        return true;
	hmap<short,KifPredicateVariableInstance*>* dico=d->Dico();
	if (dico->find(name)==dico->end()) {
		dom->Setfail(true);
		return true;
	}
	return (*dico)[name]->Setvalue(ke,index,dom);
}

Exported KifElement* KifPredicateVariable::Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* val) {
    KifElement* d=dom->Declared(PREDICATEDICO);
    if (d==NULL)
        return this;
	hmap<short,KifPredicateVariableInstance*>* dico=d->Dico();
	if (dico->find(name)==dico->end()) {
		dom->Setfail(true);
		return kifNOELEMENT;
	}
	return (*dico)[name]->Execute(contextualpattern,dom,val);
}

Exported KifElement* KifPredicateVariable::Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* val,int idthread,KifCallFunction* callfunc) {
    KifElement* d=dom->Declared(PREDICATEDICO);
    if (d==NULL)
        return this;
	hmap<short,KifPredicateVariableInstance*>* dico=d->Dico();
	if (dico->find(name)==dico->end()) {
		dom->Setfail(true);
		return kifNOELEMENT;
	}
	return (*dico)[name]->Exec(contextualpattern,dom,val,idthread,callfunc);
}

Exported KifElement* KifPredicateVariableInstance::Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* val) {
	KifElement* v=Value(dom);
    if (v==kifNOELEMENT)
        dom->Setfail(true);
	return v->Getvalues(dom,false);
}

Exported KifElement* KifPredicateVariableInstance::Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* val,int idthread,KifCallFunction* callfunc) {
	KifElement* v=Value(dom);
    if (v==kifNOELEMENT)
        dom->Setfail(true);
	return v->Getvalues(dom,false);
}

Exported KifElement* KifPredicateTerm::Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* ke) {
	if (ke==NULL || ke->isConst())
		return this;
	char ty=1;
	int left,right;
	if (ke->type==kifIndex) {
		ty=ke->Indexes(parameters.size(),dom,left,right);
		if (ty==0) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(kifErrorStrings[223]);
			return kifNOELEMENT;
		}
	}
	else {
		ke=ke->Execute(contextualpattern,dom);
        left=ke->Integer();
        ke->Release();
        
		if (left<0 || left>=parameters.size()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(kifErrorStrings[223]);
			return kifNOELEMENT;
		}
	}
    
	if (ty==1) {
		if (parameters[left]->type==kifPredicateVariable)
			return dom->Declared(parameters[left]->Name());
		return parameters[left];
	}
    
	//In this case, we must create a new vector
	KifVector* kvect=new KifVector(kifcode,NULL);
	for (int i=left;i<right;i++) {
        if (parameters[i]->type==kifPredicateVariable)
            kvect->Push(dom->Declared(parameters[i]->Name()));
        else
            kvect->values.push_back(parameters[i]);
	}
	return kvect;
}

Exported KifElement* KifPredicateTerm::Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* ke,int idthread,KifCallFunction* callfunc) {
	if (ke==NULL || ke->isConst())
		return this;
	char ty=1;
	int left,right;
	if (ke->type==kifIndex) {
		ty=ke->Indexes(parameters.size(),dom,left,right);
		if (ty==0) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(kifErrorStrings[223]);
			return kifNOELEMENT;
		}
	}
	else {
		ke=ke->Exec(contextualpattern,dom,kifNULL,idthread,callfunc);
        left=ke->Integer();
        ke->Release();
		if (left<0 || left>=parameters.size()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(kifErrorStrings[223]);
			return kifNOELEMENT;
		}
	}
    
	if (ty==1) {
		if (parameters[left]->type==kifPredicateVariable)
			return dom->Declared(parameters[left]->Name());
		return parameters[left];
	}
    
	//In this case, we must create a new vector
	KifVector* kvect=new KifVector(kifcode,NULL);
	for (int i=left;i<right;i++) {
        if (parameters[i]->type==kifPredicateVariable)
            kvect->Push(dom->Declared(parameters[i]->Name()));
        else
            kvect->values.push_back(parameters[i]);
	}
	return kvect;
}


Exported KifElement* KifPredicateKnowledgeBase::Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* ke,int idthread,KifCallFunction* callfunc) {
	//we add it...
	if (add) {
		//In this case, we need to evaluate the parameters
		KifPredicate* pv=new KifPredicate(kifcode,NULL,name);

		KifElement* e;
		for (int i=0;i<parameters.size();i++) {
			e=parameters[i]->EvaluePredicateVariables(contextualpattern,dom);
			pv->parameters.push_back(e);
		}

		if (!kifGlobals->storePredicate(dom,pv,true))
			pv->Release();
		pv->weight = kifcode->Providefloat(weight->Float());
		return kifTRUE;
	}

	//or we remove it...
	KifPredicate pv(NULL,NULL,name);
	KifElement* e;
	for (int i=0;i<parameters.size();i++) {
		e=parameters[i]->Exec(contextualpattern,dom,kifNULL,idthread,callfunc);
		e->Setpopped();
		pv.parameters.push_back(e);
	}

	if (!kifGlobals->RemovePredicates(dom,&pv))
		return kifFALSE;
	return kifTRUE;
}

KifElement* KifPredicateDatabase::Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* val,int idthread,KifCallFunction* callfunc) {
	if (database->type!=kifGlobals->kifTypeString["sqlite"]) {
		database=database->Execute(kifNULL,dom);
		if (database->type!=kifGlobals->kifTypeString["sqlite"])
			return kifcode->Returnerror("PRE(006): Expecting a database object");
	}

	ThreadLock _lock(kifPredicate);
	kifcode->predicatedatabase[name]=true;
	if (store==_PREDGET)
		kifGlobals->knowledgebase[name].push_back(this);
	else {
		if (store==_PREDSTORE)	{//we store this value in the database
			short idvalue=kifGlobals->KifStringId("%storepredicate");
			KifCallFunction call(NULL,NULL,idvalue);
			call.kifcode=kifcode;
			database->ExecMethod(idvalue,this,dom,idthread,&call);
		}
		else {
			short idvalue=kifGlobals->KifStringId("%removepredicate");
			KifCallFunction call(NULL,NULL,idvalue);
			call.kifcode=kifcode;
			database->ExecMethod(idvalue,this,dom,idthread,&call);
		}
		
		//Then we need to reset the keys of the calling predicates...
		if (kifGlobals->knowledgebase.find(name)!=kifGlobals->knowledgebase.end()) {
			vector<KifPredicate*>& v=kifGlobals->knowledgebase[name];
			string key;
			for (int i=v.size()-1;i>=0;i--) {
				if (v[i]->type==kifPredicateDatabase)
					((KifPredicateDatabase*)v[i])->dejavue.clear();
				else {
					if (v[i]->db)  {
						if (v[i]->Key(key,dom)) {
							vector<KifPredicate*>& vk=kifGlobals->knowledgebasekeys[key];
							for (int j=vk.size()-1;j>=0;j--) {
								if (vk[j]==v[i]) {
									vk.erase(vk.begin()+j);
									break;
								}
							}
						}
							
						v[i]->Resetreference();
						v.erase(v.begin()+i);
					}
				}
			}
		}
	}
	return kifTRUE;
}

KifElement* KifPredicateFunction::constructor(KifDomain* domloc, KifElement* params, int idthread, KifCallFunction* callfunc) {
	//If you do not have any parameters, then your function might return whatever you want... Here itself
	KifElement* e=kifcode->predicates[name];
	if (params->InstructionSize()==0 ||  e->Returnfunction()==NULL)
		return this;

	KifElement* o;
	//Your first parameter is at position 0 etc...		
	o=params->Instruction(0); //would match i as in the example above: mynewtype toto(i,j);
	if (o->type==kifInstance)
		o=((KifInstance*)o)->Thevariable(domloc,idthread,NULL);
	else
		o=object->Exec(kifNULL,domloc,kifNULL,idthread,callfunc);
	o->Setreference();
	if (e->Returnfunction()!=NULL)
		((KifPredicateFunction*)e)->object=o;
	return this;
}


Exported KifElement* KifPredicateVar::Store(KifElement* database,KifDomain* dom,int idthread) {	
	if (database==NULL) {
		//In this case, we add it to the internal knowledge base
		KifPredicate* pv=new KifPredicate(kifcode,NULL,name);		

		KifElement* e;
		for (int i=0;i<parameters.size();i++) {
			if (parameters[i]->unifiedDeclaration(dom)==false) {
				pv->Release();
				return kifFALSE;
			}
			e=parameters[i]->Execute(kifNULL,dom);
			pv->parameters.push_back(e->Atom(true));
		}

		if (!kifGlobals->storePredicate(dom,pv,true)) {
			pv->Release();
			return kifFALSE;
		}

		pv->weight = kifcode->Providefloat(weight->Float());
		return kifTRUE;
	}

	if (database->type!=kifGlobals->kifTypeString["sqlite"])
		return kifcode->Returnerror("PRE(006): Expecting a database object");

	short idvalue=kifGlobals->KifStringId("%storepredicate");
	KifCallFunction call(NULL,NULL,idvalue);
	call.kifcode=kifcode;
	return database->ExecMethod(idvalue,this,dom,idthread,&call);
}

Exported KifElement* KifPredicateVar::Remove(KifElement* database,KifDomain* dom,int idthread) {
	if (database==NULL) {
		if (!kifGlobals->RemovePredicates(dom,this))
			return kifFALSE;
		return kifTRUE;
	}

	if (database->type!=kifGlobals->kifTypeString["sqlite"])
		return kifcode->Returnerror("PRE(006): Expecting a database object");

	short idvalue=kifGlobals->KifStringId("%removepredicate");
	KifCallFunction call(NULL,NULL,idvalue);
	call.kifcode=kifcode;
	return database->ExecMethod(idvalue,this,dom,idthread,&call);
}


Exported KifElement* KifInstructionPredicate::Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* val,int idthread,KifCallFunction* callfunc) {
	return instructions[0]->Exec(contextualpattern,dom,val,idthread,callfunc);
}


//We test our value here...
Exported KifElement* KifPredicate::Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* ke,int idthread,KifCallFunction* callfunc) {
	//In this case, we have our variables in contextualpattern
	if (ke==NULL || ke->isConst())
		return this;
	char ty=1;
	int left,right;
	if (ke->type==kifIndex) {
		ty=ke->Indexes(parameters.size(),dom,left,right);
		if (ty==0) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(kifErrorStrings[223]);
			return kifNOELEMENT;
		}
	}
	else {
		ke=ke->Exec(contextualpattern,dom,kifNULL,idthread,callfunc);
        left=ke->Integer();
        ke->Release();
		if (left<0 || left>=parameters.size()) {
			if (kifGlobals->erroronkey)
				return kifcode->Returnerror(kifErrorStrings[223]);
			return kifNOELEMENT;
		}
	}

	if (ty==1) {
		if (parameters[left]->type==kifPredicateVariable)
			return dom->Declared(parameters[left]->Name());
		return parameters[left];
	}

	//In this case, we must create a new vector
	KifVector* kvect=new KifVector(kifcode,NULL);
	for (int i=left;i<right;i++) {
        if (parameters[i]->type==kifPredicateVariable)
            kvect->Push(dom->Declared(parameters[i]->Name()));
        else
            kvect->values.push_back(parameters[i]);
	}
	return kvect;
}

Exported KifElement* KifPredicateLocalInstruction::Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* val,int idthread,KifCallFunction* callfunc) {
	//we need to get the right evaluation for the possible variables hidden in the instruction
	//we have been through a variable renaming, which is stored in dico[localname]=declaration name

	success=false;
	dom->Setfail(false);

    KifInstructionEvaluate* keval=(KifInstructionEvaluate*)contextualpattern;
	KifDomain* localdom=keval->localdomain;
	hmap<short,KifElement*>::iterator& it=keval->it;
	if (localdom!=NULL) {		
		for (it=localdom->declarations.begin();it!=localdom->declarations.end();it++)
			dom->declarations[it->first]=it->second;
	}
	
    dom->declarations[PREDICATEDICO]=this;
	dom->reference++;
	KifElement* res=instruction->Exec(contextualpattern,dom,kifNULL,idthread,callfunc);
    dom->declarations.erase(PREDICATEDICO);
	dom->reference--;

	if (localdom!=NULL) {
		for (it=localdom->declarations.begin();it!=localdom->declarations.end();it++)
			dom->declarations.erase(it->first);
	}

	if (dom->Failed() || Errorid(keval->threadowner))
		return kifFALSE;

	if (negation) {
		if (!res->Boolean()) {
			success=true;
			return kifTRUE;
		}
		return kifFALSE;
	}


	if (res->Boolean())
		success=true;

	return res;
}

string KifPredicateLocalInstruction::String() {
    KifDomain* dom=kifGlobals->Top();
    if (dom==NULL)
        return "";

    dom->declarations[PREDICATEDICO]=this;
    string v=instruction->String();
    dom->declarations.erase(PREDICATEDICO);
    return v;
}

//-------------------------------------------------------------------------------------------------------
void KifPredicateRule::Reorderdisjunction() {
	int i;
	vector<KifElement*> ins;
	for (i=disjunction;i<instructions.size();i++) {
		if (instructions[i]->Disjunction()) {
			if (disjunction==-1)
				disjunction=i;
			ins.push_back(instructions[i]);
		}
		else {
			if (disjunction!=-1) {
				ins.push_back(instructions[i]);
				int j;
				for (j=disjunction;j<=i;j++)
					instructions.erase(instructions.begin()+disjunction);
				for (j=0;j<ins.size();j++) {
					ins[j]->Setdisjunction(j);
					instructions.insert(instructions.begin()+disjunction,ins[j]);
				}
				ins.clear();
			}
			disjunction=-1;
		}			
	}	
}

Exported KifElement* KifPredicateRule::Execute(KifElement* contextualpattern,KifDomain* dom,KifElement* val) {
	return kifTRUE;
}

//This is the entry point of a predicate analysis...
Exported KifElement* KifInstructionEvaluate::Execute(KifElement* contextualpattern,KifDomain* domcall,KifElement* dombase) {
	//head is our definition
	//First we create an actionable KifPredicate 
	//we gather all the rules and elements, we might be interested in
	//First the elements from the knowledge base
	//Second the rules from rulebase
    
    //if we have a vector as a recipient, then we want to explore the whole tree, otherwise, one solution is enough
    bool cut=true;
    if (contextualpattern->aVector() || head->Returnfunction()!=NULL)
        cut=false;

    	//we use our current element as a way to transfer our stuff...
	KifCallFunction domain(NULL,NULL,head->name);
    domain.type=kifPredicateDomain;
	domain.kifcode=kifcode;
	domain.declarations=domcall->declarations;
	VECTA<KifElement*> stack;
	stack.push_back(head);
	headrule=head;
    callfunc=&domain;

	weightstacks.clear();
	//In the case of probabilities attached to rules, we sort the rules according to their probabilities.
	//We put the top probas at the beginning... 
	//this normalization is done for all arcs stemming from here...
	if ((kifcode->checkprobabilities & PRED_NORMALIZE_WEIGHT) == PRED_NORMALIZE_WEIGHT) {		
		hmap<short, vector<KifPredicateRule*> >::iterator itrule;
		for (itrule = rules.begin(); itrule != rules.end(); itrule++) {
			vector<KifPredicateRule*>& rulebase = itrule->second;
			double summing = 0;
			int i;
			for (i = 0; i < rulebase.size(); i++)
				summing += rulebase[i]->head->weight->Float();

			if (summing != 0 && summing != 1) {
				//in this case, we normalized each value from the base...
				double v;
				for (i = 0; i < rulebase.size(); i++) {
					v = rulebase[i]->head->weight->Float() / summing;
					rulebase[i]->head->weight->Resetreference(rulebase[i]->head->reference);
					rulebase[i]->head->weight = kifcode->Providefloat(v);
				}
				if ((kifcode->checkprobabilities & PRED_RANDOMIZE) == PRED_RANDOMIZE)
					shufflerules(rulebase);
			}
		}
	}

	//we sort the values according to some random algorithm
	if ((kifcode->checkprobabilities & PRED_WEIGHT) == PRED_WEIGHT && sorted.size() == 0) {
		if ((kifcode->checkprobabilities & PRED_RANDOMIZE) != PRED_RANDOMIZE) {
			hmap<short, vector<KifPredicateRule*> >::iterator itrule;
			for (itrule = rules.begin(); itrule != rules.end(); itrule++) {
				vector<KifPredicateRule*>& rulebase = itrule->second;
				bool check = false;
				for (int i = 0; i < rulebase.size(); i++) {
					if (rulebase[i]->head->weight->Float() != 1) {
						check = true;
						break;
					}
				}
				sorted[itrule->first] = check;
				if (check)
					sort(rulebase.begin(), rulebase.end(), KifCompareHeadWeight);
			}
		}
	}

	int idstack = kifGlobals->PushFunctionStackid(threadowner, &domain, callfunc);

	dom=&domain;
	PredicateEvalue(stack,NULL,cut,0);
	
	kifGlobals->PopFunctionStack(idstack);
	
	for (it=domain.declarations.begin();it!=domain.declarations.end();it++) {
		if (domcall->declarations.find(it->first)==domcall->declarations.end()) {
			it->second->Putpopped(0);
			it->second->Resetreference();
		}
	}

	int i;
	if (contextualpattern->aVectorContainer()) {
		KifElement* kvect=Selectavector(this,contextualpattern);
		for (i=0;i<results.size();i++) {
			kvect->Push(results[i]);
			results[i]->Resetreference();
		}
		return kvect;
	}

	if (results.size()==0)
		return kifFALSE;

	if (contextualpattern->type==kifBoolean) {
		for (i=0;i<results.size();i++)
			results[i]->Resetreference();
		return kifTRUE;
	}

	if (contextualpattern->type==kifString && contextualpattern!=kifTypeSTRING) {
		string v=results[0]->String();
		results[0]->Resetreference();
		return kifcode->Providestring(v);
	}

	if (contextualpattern->type==kifInteger || contextualpattern->type==kifFloat) {
		for (i=0;i<results.size();i++)
			results[i]->Resetreference();
		return kifcode->Provideinteger(results.size());
	}

	if (contextualpattern->type==kifPredicate) {
		results[0]->Putpopped(1);
		results[0]->Resetreference();
		return results[0];
	}

	for (i=0;i<results.size();i++)
		results[i]->Resetreference();

	return kifTRUE;
}

Exported KifElement* KifInstructionLaunch::Exec(KifElement* contextualpattern,KifDomain* dom,KifElement* ke,int idthread,KifCallFunction* callfunc) {
	//head is our definition
	//First we create an actionable KifPredicate 
	int i;


	KifPredicateFunction pv(NULL,NULL,kifcode->predicates[head->name]->Returnfunction(),head->name);
	if (pv.function!=NULL)
		pv.object=kifcode->predicates[head->name]->Returnobject();
	pv.kifcode=kifcode;
	KifInstructionEvaluate kl(kifcode,&pv);
    kl.threadowner=idthread;
	short lastnb=KifPredicate::nb;
    KifDomain domain(NULL,NULL,head->name,kifPredicateDomain);
    domain.kifcode=kifcode;
    hmap<short,KifPredicateVariableInstance*> localdico;
    kl.dico=&localdico;
	KifPredicateContainer* kpcont=kifcode->kstart->Predicatecontainer();

	if (kpcont!=NULL)
		kl.rules=kpcont->rules;

	if (dom!=dom->kifcode->kstart) {
		kpcont=dom->Predicatecontainer();

		if (kpcont!=NULL) {
			if (kl.rules.size()==0)
				kl.rules=kpcont->rules;
			else {
				hmap<short,vector<KifPredicateRule*> >::iterator it;
				for (it=kpcont->rules.begin();it!=kpcont->rules.end();it++) {
					if (kl.rules.find(it->first)==kl.rules.end())
						kl.rules[it->first]=it->second;
					else {
						for (i=0;i<it->second.size();i++)
							kl.rules[it->first].push_back(it->second[i]);
					}
				}
			}
			//we are in a frame...
			//We try to get the maximum ID within that domain...
			int mx=0; 
			hmap<short,KifElement*>::iterator itr;
			for (itr=dom->declarations.begin();itr!=dom->declarations.end();itr++)
				if (mx<itr->first)
					mx=itr->first;
			//In order to distinguish when necessary between the embedding domain keys and the local domain...
			KifPredicate::nb=mx+1;
			kl.localdomain=dom;
		}
	}

    if (kifcode->predicateTrace(head->name))
        kl.trace=true;

    KifElement* e;
    for (i=0;i<head->parameters.size();i++) {
        e=head->parameters[i]->Frominstruction(0)->ExtractPredicateVariables(&kl,&domain,NULL,NULL,true);
        e->Setreference();
        pv.parameters.push_back(e);
    }
    
	KifElement* res=kl.Execute(contextualpattern,&domain,dom);

	for (i=0;i<pv.Size();i++)
		pv.parameters[i]->Resetreference();

    hmap<short,KifElement*>::iterator it;
    for (it=domain.declarations.begin();it!=domain.declarations.end();it++) {
		it->second->Putpopped(0);
        it->second->Resetreference();
	}
	KifPredicate::nb=lastnb;	

	return res;
}


KifElement* PreQuery(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc,int idthread) {
	KifCode* kifcode=callfunc->kifcode;
	string nm=callfunc->Evaluatethread(1,dom,idthread)->String();

	short name=KifStringId(nm);
	if (name==-1)
		return kifNULL;

	int i;


	KifPredicateFunction pv(NULL,NULL,kifcode->predicates[name]->Returnfunction(),name);
	if (pv.function!=NULL)
		pv.object=kifcode->predicates[name]->Returnobject();
	pv.kifcode=kifcode;
	KifInstructionEvaluate kl(kifcode,&pv);
    kl.threadowner=kifGlobals->GetThreadid();
	short lastnb=KifPredicate::nb;
    KifDomain domain(NULL,NULL,name,kifPredicateDomain);
    domain.kifcode=kifcode;
    hmap<short,KifPredicateVariableInstance*> localdico;
    kl.dico=&localdico;
	KifPredicateContainer* kpcont=kifcode->kstart->Predicatecontainer();

	if (kpcont!=NULL)
		kl.rules=kpcont->rules;

	if (dom!=dom->kifcode->kstart) {
		kpcont=dom->Predicatecontainer();

		if (kpcont!=NULL) {
			if (kl.rules.size()==0)
				kl.rules=kpcont->rules;
			else {
				hmap<short,vector<KifPredicateRule*> >::iterator it;
				for (it=kpcont->rules.begin();it!=kpcont->rules.end();it++) {
					if (kl.rules.find(it->first)==kl.rules.end())
						kl.rules[it->first]=it->second;
					else {
						for (i=0;i<it->second.size();i++)
							kl.rules[it->first].push_back(it->second[i]);
					}
				}
			}
			//we are in a frame...
			//We try to get the maximum ID within that domain...
			int mx=0; 
			hmap<short,KifElement*>::iterator itr;
			for (itr=dom->declarations.begin();itr!=dom->declarations.end();itr++)
				if (mx<itr->first)
					mx=itr->first;
			//In order to distinguish when necessary between the embedding domain keys and the local domain...
			KifPredicate::nb=mx+1;
			kl.localdomain=dom;
		}
	}

    if (kifcode->predicateTrace(name))
        kl.trace=true;

    KifElement* e;
    for (i=2;i<callfunc->Size();i++) {
		e=callfunc->parameters[i]->ExtractPredicateVariables(&kl,&domain,NULL,NULL,true);
        e->Setreference();
        pv.parameters.push_back(e);
    }
    
	KifElement* res=kl.Execute(contextualpattern,&domain,dom);

	for (i=0;i<pv.Size();i++)
		pv.parameters[i]->Resetreference();

    hmap<short,KifElement*>::iterator it;
    for (it=domain.declarations.begin();it!=domain.declarations.end();it++) {
		it->second->Putpopped(0);
        it->second->Resetreference();
	}
	KifPredicate::nb=lastnb;	

	return res;
}

