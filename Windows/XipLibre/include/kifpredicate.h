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
   filename   : kif.h
   Date       : 09/09/2010
   Purpose    : Implementation of the KiF language
   Programmer : Claude ROUX
   Reviewer   :
   */


#ifndef i_kifpredicate
#define i_kifpredicate
#include "kifbase.h"


//A PredicateVariable such as: ?val
class KifPredicateVariable : public KifElement {
public:
	short name;
	string label;

	KifPredicateVariable(KifCode* klc, KifElement* kp, short n, string l, KifType t = kifPredicateVariable);

	bool aPredicateVariable() {
		return true;
	}


	Exported void Setname(short n) {
		name = n;
	}

	Exported short Name() { return name; }

	bool Variable() {
		return true;
	}

	virtual Exported string String();

	virtual bool unified(KifDomain* dom, KifElement* a) {
		return false;
	}

	virtual bool unifiedDeclaration(KifDomain* dom);

	virtual Exported KifElement* Copy(KifDomain* kp, KifElement* dom) {
		return new KifPredicateVariable(kifcode, kp, name, label);
	}

	virtual Exported KifElement* Newinstance() {
		return new KifPredicateVariable(kifcode, NULL, name, label);
	}

	virtual Exported KifElement* same(KifElement* a) {
		return kifTRUE;
	}

	virtual Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	virtual Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

	virtual Exported bool Setvalue(KifElement* ke, KifElement* index, KifElement* dom);
	virtual Exported KifElement* ExtractPredicateVariables(KifElement* context, KifDomain* dom, KifElement* val, KifElement* e, bool root);
	virtual Exported KifElement* EvaluePredicateVariables(KifElement* context, KifDomain* dom);
};

class KifPredicateVariableInstance : public KifPredicateVariable {
public:

	KifElement* value;
	bool merge;

	KifPredicateVariableInstance(KifCode* klc, KifElement* kp, short n, string l) : KifPredicateVariable(klc, kp, n, l, kifPredicateVariableInstance) {
		value = kifNOELEMENT;
		merge = false;
	}


	bool isToMerge() {
		return merge;
	}

	Exported string String();
	Exported string BaseString();

	bool unified(KifDomain* dom, KifElement* a);
	bool unifiedDeclaration(KifDomain* dom);

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom) {
		return this;
	}

	KifElement* VariableValue() {
		return value;
	}

	KifElement* Variable(KifDomain* dom);

	Exported KifElement* Newinstance() {
		return new KifPredicateVariableInstance(kifcode, NULL, name, label);
	}

	Exported void Setreference(int inc = 1) {
		if (value != kifNOELEMENT)
			value->Setreference(inc);
		reference += inc;
	}
	Exported void Resetreference(int inc = 1);
	void Resetreferencenopopped(int inc = 1);
	
	void Cleans(KifElement* v, bool localvalue) {
		if (localvalue) {
			value->Resetreference(reference);
			value = v;
		}
		else
			v->Resetreference(reference);
	}

	Exported void Setpopped() {
		popped = 1;
		value->Setpopped();
	}

	KifElement* Value(KifDomain* dom);

	void affiche();

	Exported KifElement* same(KifElement* a);

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);

	bool Insertvalue(KifDomain* dom, KifElement* v, bin_hash<KifElement*>&);
	Exported bool Setvalue(KifElement* ke, KifElement* index, KifElement* dom);
	Exported KifElement* ExtractPredicateVariables(KifElement* context, KifDomain* dom, KifElement* val, KifElement* e, bool root);
	Exported KifElement* EvaluePredicateVariables(KifElement* context, KifDomain* dom);
	KifElement* Getvalues(KifDomain* dom, bool duplicate);
};

class KifPredicateVariableInstanceForCleaning : public KifElement {
public:
	KifPredicateVariableInstance* variable;

	KifPredicateVariableInstanceForCleaning(KifCode* k, KifPredicateVariableInstance* v) : KifElement(k, NULL, kifPredicateVariableInstance) {
		variable = v;
	}

	void Resetreference(int inc = 1) {
		variable->value->Resetreference(variable->reference);
		variable->value = kifNOELEMENT;
		KifElement::Resetreference(inc);
	}
};

class KifPredicateTerm : public KifPredicateVariable {
public:

	VECTA<KifElement*> parameters;

	KifPredicateTerm(KifCode* klc, KifElement* kp, short n, string l) : KifPredicateVariable(klc, kp, n, l, kifPredicateTerm) {}
	Exported string BaseString();

	bool Insertvalue(KifDomain* dom, KifElement* v, bin_hash<KifElement*>&);

	virtual Exported KifElement* Newinstance() {
		return new KifPredicateTerm(kifcode, NULL, name, label);
	}


	KifElement* Value() {
		return this;
	}

	Exported KifElement* ExtractPredicateVariables(KifElement* context, KifDomain* dom, KifElement* val, KifElement* e, bool root);
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	Exported KifElement* EvaluePredicateVariables(KifElement* context, KifDomain* dom);

	Exported bool Setvalue(KifElement* ke, KifElement* index, KifElement* dom);

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifPredicateTerm* v = new KifPredicateTerm(kifcode, NULL, name, label);
		KifElement* e;
		for (int i = 0; i < parameters.size(); i++) {
			e = parameters[i]->Atom(true);
			e->Setreference();
			v->parameters.push_back(e);
		}
		return v;
	}

	bool unified(KifDomain* dom, KifElement* a);
	bool unifiedDeclaration(KifDomain* dom);


	Exported KifElement* Parameter(int i) {
		return parameters[i];
	}

	virtual void Release() {
		if (reference == 0) {
			popped = false;
			for (int i = 0; i < parameters.size(); i++)
				parameters[i]->Release();
			KifElement::Resetreference();
		}
	}

	Exported void Setreference(int inc = 1);
	Exported void Resetreference(int inc = 1);
	Exported void Setpopped();

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);

	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);

	KifElement* Getvalues(KifDomain* dom, bool duplicate);

	Exported void Add(KifElement* e) {
		parameters.push_back(e);
	}

	Exported string String();

	Exported KIFLONG Long() {
		return parameters.size();
	}

	Exported long Integer() {
		return (long)parameters.size();
	}
	Exported double Float() {
		return (double)parameters.size();
	}

	Exported KifElement* same(KifElement* a);

	Exported long Size() {
		return parameters.size();
	}

};

//The instance. It can contain predicate variables in its parameters: ?val
class KifPredicate : public KifPosition {
public:
	static int nb;
	VECTE<KifElement*> parameters;
	KifElement* weight;

	short name;

	bool negation;
	bool disjunction;
	bool db;

#ifdef _DEBUG
	string nom;
#endif


	KifPredicate(KifCode* klc, KifElement* kp, short n, KifType t = kifPredicate);
	Exported short Name() { return name; }

	virtual short kbaction() {
		return 0;
	}

	bool Key(string&, KifDomain*);
	Exported KifElement* Atom() {
		return this;
	}

	void Setdisjunction(bool v) {
		disjunction = v;
	}

	bool testfunction(KifElement* func) {
		if (func == NULL)
			return true;

		if (func->type == kifFunction) {
			KifFunction* kfunction = (KifFunction*)func;
			if (kfunction->arguments.size() != 2)
				return false;
			if (kfunction->arguments[0]->VariableValue()->inType() != type)
				return false;
			return true;
		}
		return false;
	}


	void set(bool n, bool d) {
		negation = n;
		disjunction = d;
	}

	bool Negation() {
		return negation;
	}

	bool Disjunction() {
		return disjunction;
	}

	bool unified(KifDomain* dom, KifElement* a);
	bool unifiedDeclaration(KifDomain* dom);
	Exported string BaseString();


	Exported virtual KifPredicate* Duplicate(KifElement* context, KifDomain* d);
	Exported KifPredicate* Copyfrom(KifElement* context, KifDomain* d, KifPredicate* h);

	virtual Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifPredicate* v = new KifPredicate(kifcode, NULL, name);
		KifElement* e;
		for (int i = 0; i < parameters.size(); i++) {
			e = parameters[i]->Atom(true);
			e->Setreference();
			v->parameters.push_back(e);
		}
		return v;
	}

	Exported KifElement* Parameter(int i) {
		return parameters[i];
	}

	Exported KifElement* Vector(KifElement*, KifVector* itself = NULL);
	Exported KifElement* Map(KifElement*, KifMap* itself = NULL);

	virtual Exported KifElement* Declaration(short n);
	virtual Exported void Methods(KifVectorString* fcts);
	virtual Exported KifElement* Information(KifElement*);

	Exported void Setreference(int inc = 1);
	Exported void Resetreference(int inc = 1);
	Exported void Setpopped();

	void Clean() {
		for (int i = 0; i < parameters.size(); i++) {
			if (parameters[i]->type != kifInstructionPARAMETER) {
				parameters[i]->Putpopped(0);
				parameters[i]->Resetreference();
			}
		}
		parameters.clear();
		weight->Putpopped(0);
		weight->Resetreference();
	}

	virtual Exported void Add(KifElement* e) {
		parameters.push_back(e);
	}

	bool Basic() {
		return true;
	}

	bool Simple() {
		return true;
	}

	bool Statique() {
		return true;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int idthread, KifCallFunction* callfunc);

	bool Predefinedtype() { return true; }
	virtual Exported bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL);

	virtual Exported KifElement* Newinstance() {
		return new KifPredicate(kifcode, NULL, name);
	}

	Exported string String();

	Exported KIFLONG Long() {
		return weight->Long();
	}

	Exported long Integer() {
		return weight->Integer();
	}

	Exported double Float() {
		return weight->Float();
	}

	Exported bool Boolean();

	Exported KifElement* same(KifElement* a);

	Exported long Size() {
		return parameters.size();
	}
};


class KifPredicateFunction : public KifPredicate {
public:
	KifElement* function;
	KifElement* object;
	bool trace;


	KifPredicateFunction(KifCode* klc, KifElement* kp, KifElement* f, short n) : KifPredicate(klc, kp, n, kifPredicate) {
		function = f;
		object = kifNULL;
		trace = false;
	}

	KifElement* Returnfunction() {
		return function;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifPredicateFunction* v = new KifPredicateFunction(kifcode, NULL, function, name);
		v->trace = trace;
		KifElement* e;
		for (int i = 0; i < parameters.size(); i++) {
			e = parameters[i]->Atom(true);
			e->Setreference();
			v->parameters.push_back(e);
		}
		return v;
	}

	Exported KifElement* Newinstance() {
		return new KifPredicateFunction(kifcode, NULL, function, name);
	}

	Exported bool initialconstructor() {
		return true;
	}
	Exported void Resetreference(int inc = 1);
	KifElement* Returnobject() {
		return object;
	}

	KifElement* constructor(KifDomain* domloc, KifElement* params, int, KifCallFunction*);

};

class KifPredicateVar : public KifPredicate {
public:

	KifPredicateVar(KifCode* klc, KifElement* kp, short n) : KifPredicate(klc, kp, n, kifPredicate) {}

	Exported bool Setvalue(KifElement* ke, KifElement* index = NULL, KifElement* dom = NULL);
	Exported KifElement* Newinstance() {
		return new KifPredicateVar(kifcode, NULL, name);
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifPredicateVar* v = new KifPredicateVar(kifcode, NULL, name);
		KifElement* e;
		for (int i = 0; i < parameters.size(); i++) {
			e = parameters[i]->Atom(true);
			e->Setreference();
			v->parameters.push_back(e);
		}
		return v;
	}

	Exported KifElement* Declaration(short n);
	Exported void Methods(KifVectorString* fcts);
	Exported KifElement* Information(KifElement*);
	Exported KifElement* Store(KifElement* db, KifDomain*, int, KifCallFunction*);
	Exported KifElement* Remove(KifElement* db, KifDomain*, int, KifCallFunction*);
};

class KifPredicateKnowledgeBase : public KifPredicate {
public:
	bool add;

	KifPredicateKnowledgeBase(KifCode* klc, KifElement* kp, short n) : KifPredicate(klc, kp, n, kifPredicateKnowledgeBase) {
		add = false;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifPredicateKnowledgeBase* v = new KifPredicateKnowledgeBase(kifcode, NULL, name);
		KifElement* e;
		for (int i = 0; i < parameters.size(); i++) {
			e = parameters[i]->Atom(true);
			e->Setreference();
			v->parameters.push_back(e);
		}
		return v;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int, KifCallFunction*);

	Exported KifElement* Newinstance() {
		return new KifPredicateKnowledgeBase(kifcode, NULL, name);
	}
};

class KifPredicateKnowledgeBaseFunction : public KifPredicate {
public:
	short action;
	KifElement* dbase;

	KifPredicateKnowledgeBaseFunction(KifCode* klc, KifElement* kp, short n) : KifPredicate(klc, kp, n) {
		action = n;
		dbase = kifNULL;
	}

	short kbaction() {
		return action;
	}
	string String();
	void Add(KifElement* e) {
		if (e->type == kifPredicate) {
			KifPredicate* p = (KifPredicate*)e;
			name = p->name;
			parameters = p->parameters;
			weight = p->weight;

			p->weight = kifONE;
			p->parameters.clear();
		}
		else
			dbase = e;
	}
	KifPredicate* Duplicate(KifElement* context, KifDomain* d);
};


//this class is used to record an entry within the knowledge base such as: toto(x,y) :- true; 
//with toto(x,y):- false; the element is removed.
class KifInstructionPredicate : public KifDomainInstruction {
public:
	KifInstructionPredicate(KifCode* klc, KifElement* kp, short n) : KifDomainInstruction(klc, kp, n, kifInstructionPredicate) {}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);
};

//This is a most important class, as it contains the element which is used as a trigger to force the analyse of rules...
class KifInstructionEvaluate : public KifElement {
public:
	KifPredicate* head;
	KifPredicate* headrule;
	KifDomain* localdomain;
	KifDomain* dom;
	VECTA<KifPredicate*> results;
	vector<double> weightstacks;
	KifElement* value;
	bool count;
	bool trace;
	hmap<short, vector<KifPredicateRule*> > rules;
	bin_hash<bool> sorted;
	bin_hash<KifPredicateVariableInstance*>* dico;
	bin_hash<KifElement*>::iterator it;
	int threadowner;
	int nbpredicates;
	KifCallFunction* callfunc;

	KifInstructionEvaluate(KifCode* klc, KifPredicate* h) : results(10), KifElement(NULL, NULL, kifInstructionPredicate) {
		nbpredicates = 0;
		callfunc = NULL;
		localdomain = NULL;
		value = kifNULL;
		trace = false;
		count = false;
		kifcode = klc;
		headrule = NULL;
		head = h;
		dom = NULL;
	}


	bin_hash<KifPredicateVariableInstance*>* Dico() {
		return dico;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction* callfunc);
	KifElement* PredicateEvalue(VECTA<KifElement*>& stack, KifPredicate* currenthead, int depth);
	KifPredicate* PredicateUnification(VECTA<KifElement*>& stack, int& posreplace, int& from);
	KifElement* PredicateCreate(KifPredicate* headrule, int depth);
	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx);

};

class KifPredicateDatabase : public KifPredicate {
public:
	char store;
	bool adddb;
	KifElement* database;
	hmap<string, char> dejavue;

	KifPredicateDatabase(KifCode* klc, KifElement* kp, KifElement* db, short n) : KifPredicate(klc, kp, n, kifPredicateDatabase) {
		store = _PREDGET;
		adddb = false;
		database = db;
	}

	Exported KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifPredicateDatabase* v = new KifPredicateDatabase(kifcode, NULL, database, name);
		KifElement* e;
		for (int i = 0; i < parameters.size(); i++) {
			e = parameters[i]->Atom(true);
			e->Setreference();
			v->parameters.push_back(e);
		}
		return v;
	}

	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* ke = kifNULL) {
		int idthread = kifGlobals->GetThreadid();
		return Exec(contextualpattern, dom, ke, idthread, (KifCallFunction*)kifGlobals->Topid(idthread));
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* ke, int, KifCallFunction*);

	Exported KifElement* Newinstance() {
		return new KifPredicateDatabase(kifcode, NULL, database, name);
	}
	KifElement* EvaluePredicateVariables(KifElement* contextualpattern, KifDomain* dom);
	virtual Exported void Add(KifElement* e) {
		if (adddb)
			database = e;
		else
			parameters.push_back(e);
	}
};


class KifInstructionLaunch : public KifElement {
public:
	KifPredicate* head;

	KifInstructionLaunch(KifCode* klc, KifElement* kp) : KifElement(klc, kp, kifInstructionPredicate) {}
	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);
	void Add(KifElement* e) {
		head = (KifPredicate*)e;
	}
};

//definition of a rule element, which can be anything from another predicate to a function call...
//contextualpattern is ALWAYS a KifInstructionEvaluate
class KifPredicateRuleElement : public KifElement {
public:
	vector<KifElement*> instructions;
	bool negation;
	bool disjunction;

	KifPredicateRuleElement(KifCode* klc, KifElement* kp) : disjunction(false), negation(false), KifElement(klc, kp, kifPredicateRuleElement) {}

	KifOperator Action() {
		return kifPREDICATEELEMENT;
	}

	Exported void Add(KifElement* ke) {
		instructions.push_back(ke);
	}

	void Setdisjunction(bool v) {
		disjunction = v;
	}

	Exported void Addtail(KifPredicateRule*, int& disj);
};


//Definition of a predicate rule
class KifPredicateRule : public KifElement {
public:
	vector<KifElement*> instructions;
	KifPredicate* head;
	int disjunction;
	short name;

	short Name() {
		return name;
	}

	KifPredicateRule(KifCode* klc, KifElement* kp, short n) : KifElement(klc, kp, kifPredicateRule) {
		name = n;
		disjunction = 0;
	}

	void Reorderdisjunction();
	Exported void Addtail(KifPredicateContainer* kcont, KifPredicateRuleElement* elements) {
		if (elements != NULL) {
			disjunction = -1;
			kcont->rules[name].push_back(this);
			elements->Addtail(this, disjunction);
			if (disjunction != -1)
				Reorderdisjunction();
		}
	}
	Exported KifElement* Execute(KifElement* contextualpattern, KifDomain* dom, KifElement* val = kifNULL);	
	void addfinal(KifPredicateContainer* kpc);
};

class KifPredicateRuleItem : public KifElement {
public:
	KifElement* item;
	int position;
	bool negation;
	bool disjunction;

	KifPredicateRuleItem(KifCode* klc, KifElement* kp, bool n, bool d, int i);

	void Setdisjunction(bool v) {
		item->Setdisjunction(v);
		disjunction = v;
	}

	bool Negation() {
		return negation;
	}

	bool Disjunction() {
		return disjunction;
	}

};

class KifPredicateLocalInstruction : public KifElement {
public:

	bin_hash<KifPredicateVariableInstance*>* dico;
	KifElement* instruction;
	bool negation;
	bool success;
	bool disjunction;


	KifPredicateLocalInstruction(KifCode* klc, bin_hash<KifPredicateVariableInstance*>* context, KifElement* e, bool n, bool d) : KifElement(NULL, NULL, kifPredicateLocalInstruction) {
		dico = context;
		negation = n;
		disjunction = d;
		instruction = e;
		kifcode = klc;
		success = false;
	}

	bin_hash<KifPredicateVariableInstance*>* Dico() {
		return dico;
	}

	void Clean() {}

	void Setdisjunction(bool v) {
		disjunction = v;
	}


	bool hasbeenSuccesfull() {
		return success;
	}

	void setSucessfull(bool v) {
		success = v;
	}

	Exported KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int, KifCallFunction*);
	KifOperator Action() {
		return instruction->Action();
	}

	short Name() {
		return instruction->Name();
	}

	string String();
};


#endif




