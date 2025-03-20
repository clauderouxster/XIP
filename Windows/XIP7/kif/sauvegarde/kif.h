#ifndef i_kif
#define i_kif

#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;
class KifElement;
class KifFunction;
class KifCallFunction;
class x_node;
class x_reading;

typedef KifElement* (*Predefined)(KifElement*,KifElement*,KifElement*,KifCallFunction*);

class KifRaiseError {
public:

    string message;
    string filename;
    long left;
    long right;

    KifRaiseError(string mes,string file="",long l=0,long r=0) {
		message=mes;
		filename=file;
        left=l;
        right=r;
    }
};

typedef enum {kifStart,kifAny,kifString,kifInteger,kifFloat,kifBoolean,kifCouple,kifDomain,kifCallFunction,
			  kifFrameDeclaration,kifFunctionDeclaration,kifFile,kifInstance,kifIndex,kifPredefined,
			  kifVariableDeclaration,kifFrame,kifMap,kifVector,kifFunction,kifInstruction,kifVOID} KifType;

typedef enum  {
	kifNONE,
	kifAFFECTATION,
	kifINITIALISATION,
	kifEQUAL,
	kifSUPERIOR,
	kifINFERIOR,
	kifSUPEQU,
	kifINFEQU,
	kifDIFFERENT,
	kifPLUS,
	kifMINUS,
	kifTIME,
	kifDIVIDE,
	kifMOD,
	kifPOWER,
	kifCHAINING,
	kifBLOCBOOLEAN,
	kifPARAMETER,
	kifDISJOINEDBLOC,
	kifOR,
	kifAND,
	kifIF,
	kifWHILE,
	kifFOR,
	kifIN
} KifOperators;



//We define the basic KIF element, which is KifObject
class KifLoadCode;
class KifElement;
class KifBoolean;
extern KifElement* kifNULL;
extern KifElement* kifTRUE;
extern KifElement* kifFALSE;

class KifElement {
public:
	static vector<KifElement*> garbage;
	KifType type;
	int reference;
	bool block;
	
	KifElement* parent;
	KifElement(KifElement* kp,KifType k=kifAny) {
		block=false;
		garbage.push_back(this);
		type=k;
		reference=1;
		parent=kp;
		if (kp!=NULL)
			kp->Add(this);
	}
	KifElement* Searchtype(string& n);
	virtual KifElement* Searchtypefull(string& n);
	KifElement* Searchframe(string& n);
	virtual KifElement* Declaration(string& s) {
		return NULL;
	}
	
	bool Error() {
		return false;
	}

	void FreeReference(bool temp=true) {
		if (block)
			return;
		if (!temp)
			return;
		reference--;
	}

	void clean(bool force) {
		if (force==false)
			if (garbage.size()<10000)
				return;

		vector<int> deleted;
		int i=0;
		vector<KifElement*>::iterator it;
		for (it=garbage.begin();it!=garbage.end();it++) {
			KifElement* ke=*it;
			if (ke->reference<=0) {
				deleted.push_back(i);
				delete ke;
			}
			i++;
		}
		it=garbage.begin();
		for (vector<int>::reverse_iterator itb=deleted.rbegin();itb!=deleted.rend();++itb) {
			i=*itb;
			garbage.erase(it+i);
		}
	}

	KifElement* Searchtop();
	KifElement* Searchtopframe();
	virtual void Clean() {}
	virtual void Run() {}
	virtual void Add(KifElement*) {}
	virtual KifElement* Current() {return NULL;}
	virtual KifElement* Framedefined(string,KifElement* ke) {return kifNULL;}
	virtual bool Predefinedtype() {return false;}
	virtual void Setstructures(string&,KifElement* ke) {}
	virtual KifElement* Loopexecute(KifElement* pattern,KifElement* dom,KifElement* ke,KifCallFunction* callfunc) {return kifNULL;}
	virtual KifElement* Forexecute(KifElement* pattern,KifElement* dom,KifElement* ke,KifCallFunction* callfunc) {return kifNULL;}
	virtual KifElement* Disjoinedexecute(KifElement* pattern,KifElement* dom,KifElement* ke,KifCallFunction* callfunc) {return kifNULL;}
	virtual void Setcurrent(KifElement*) {}
	virtual KifElement* Instruction(int i) {return NULL;}
	virtual bool Variable() {return false;}
	virtual KifElement* Subname() {return NULL;}
	virtual KifElement* Structures(string& name) {return NULL;}
	virtual KifElement* Instance() {return NULL;}
	virtual KifElement* Returnvalue() {return kifNULL;}
	virtual KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* val=kifNULL,KifCallFunction* callfunc=NULL) {return kifNULL;}
	virtual KifElement* Returninstance(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* val=kifNULL,KifCallFunction* callfunc=NULL) {
		return Execute(pattern,dom,val,callfunc);
	}
	virtual void Set(string& s,KifElement* k,KifLoadCode* klc) {}
	virtual void Setinitialisation(KifElement* ke) {};
	virtual KifElement* Value() {return NULL;}
	virtual KifElement* Copy(KifElement*,KifElement* ke=NULL,bool temp=true) {return kifNULL;}
	virtual KifElement* Domain() {return kifNULL;}
	virtual KifElement* Variabledeclaration() {return kifNULL;}
	virtual string Name() {return "";}
	virtual void Initialisation(KifElement* ke=NULL) {}
	virtual KifType  Typevariable() {return type;}
	virtual KifType  Frame() {return type;}
	virtual void print() {};
	virtual void Disableadding() {}
	virtual string String() {return "";}
	virtual int Integer() {return 0;}
	virtual float Float() {return 0;}
	virtual bool Boolean() {return false;}
	virtual void Setvalue(KifElement* k,KifElement* index=NULL) {}
	virtual void Initvalue(KifElement* k) {}
	virtual void Setreturnvalue(KifElement* k) {}
	virtual bool Multiple() {return false;}
	virtual void CopyInstances(KifElement* dom) {}
	virtual void SetInstances(KifElement* dom) {}
	virtual KifElement* in(KifElement* a,KifElement* b,bool idx) {
		return kifNULL;
	}
	virtual KifElement* plus(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	virtual KifElement* minus(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	virtual KifElement* multiply(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	virtual KifElement* divide(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	virtual KifElement* mod(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	virtual KifElement* power(KifElement* a,KifElement* b) {
		return kifNULL;
	}
	virtual KifElement* less(KifElement* a) {
		return kifFALSE;
	}
	virtual KifElement* more(KifElement* a) {
		return kifFALSE;
	}
	virtual KifElement* same(KifElement* a) {
		if (a==this)
			return kifTRUE;
		return kifFALSE;
	}
	virtual KifElement* different(KifElement* a) {
		if (a!=this)
			return kifTRUE;
		return kifFALSE;
	}
	virtual KifElement* lessequal(KifElement* a) {
		return kifFALSE;
	}
	virtual KifElement* moreequal(KifElement* a) {
		return kifFALSE;
	}
	
	virtual KifOperators Action() {
		return kifNONE;
	}
};


class KifObject : public KifElement {
public:

	vector<KifElement*> instructions;
	bool adding;
	KifObject(KifElement* kp,KifType k=kifAny) : KifElement(kp,k) {
		type=k;
		adding=true;
	}
	virtual void Add(KifElement* k) {
		if (adding)
			instructions.push_back(k);
	}
	
	void Disableadding() {
		adding=false;
	}
	
	KifElement* Instruction(int i) {
		if (i<instructions.size())
			return instructions[i];
		return NULL;
	}	
};


class KifInstruction : public KifObject {
public:
	KifOperators action;
	KifInstruction(KifElement* kp,KifOperators kop=kifNONE) : KifObject(kp,kifInstruction) {
		action=kop;
	}
	void Set(string& s,KifElement* k,KifLoadCode* klc);
	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* val=kifNULL,KifCallFunction* callfunc=NULL);
	KifElement* Loopexecute(KifElement* pattern,KifElement* dom,KifElement* ke,KifCallFunction* callfunc);
	KifElement* Forexecute(KifElement* pattern,KifElement* dom,KifElement* ke,KifCallFunction* callfunc);
	KifElement* Disjoinedexecute(KifElement* pattern,KifElement* dom,KifElement* ke,KifCallFunction* callfunc);
	KifOperators Action() {
		return action;
	}
};

class KifInteger : public KifElement {
public:
	long value;
	KifInteger(KifElement* kp,long v=0) : KifElement(kp,kifInteger) {value=v;}
	KifElement* Copy(KifElement* kp,KifElement* dom=kifNULL,bool temp=true) {
		KifElement* ke=new KifInteger(kp,value);
		ke->FreeReference(temp);
		return ke;
	}
	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* ke=NULL,KifCallFunction* callfunc=NULL) {
		return this;
	}
	bool Predefinedtype() {return true;}	
	void Setvalue(KifElement* ke,KifElement* index=NULL) {
		value=ke->Integer();
	}
	KifElement* Value() {
		return this;
	}
	string String() {
		char str[100];
		sprintf(str,"%ld",value);
		return str;		
	}
	int Integer() {
		return value;
	}
	float Float() {
		return value;
	}
	bool Boolean() {
		if (value==0)
			return false;
		return true;
	}

	KifElement* in(KifElement* a,KifElement* b,bool idx) {
		if (b->type==kifInteger)
			return kifNULL;
		return b->in(a,b,true);
	}

	KifElement* plus(KifElement* a,KifElement* b) {
		int i=a->Integer()+b->Integer();
		KifElement* kv= new KifInteger(NULL,i);
		kv->FreeReference();
		return kv;
		
	}
	KifElement* minus(KifElement* a,KifElement* b) {
		int i=a->Integer()-b->Integer();
		KifElement* kv= new KifInteger(NULL,i);
		kv->FreeReference();
		return kv;
	}
	KifElement* multiply(KifElement* a,KifElement* b) {
		int i=a->Integer()*b->Integer();
		KifElement* kv= new KifInteger(NULL,i);
		kv->FreeReference();
		return kv;
	}
	KifElement* divide(KifElement* a,KifElement* b) {
		if (b->Integer()==0)
			return kifNULL;
		int i=a->Integer()/b->Integer();
		KifElement* kv= new KifInteger(NULL,i);
		kv->FreeReference();
		return kv;
	}
	KifElement* mod(KifElement* a,KifElement* b) {
		int i=a->Integer()%b->Integer();
		KifElement* kv= new KifInteger(NULL,i);
		kv->FreeReference();
		return kv;
	}
	KifElement* power(KifElement* a,KifElement* b) {
		int i=a->Integer()^b->Integer();
		KifElement* kv= new KifInteger(NULL,i);
		kv->FreeReference();
		return kv;
	}

	KifElement* less(KifElement* a) {
		if (value<a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* more(KifElement* a) {
		if (value>a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* same(KifElement* a) {
		if (value==a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* different(KifElement* a) {
		if (value!=a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	
	KifElement* lessequal(KifElement* a) {
		if (value<=a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* moreequal(KifElement* a) {
		if (value>=a->Integer())
			return kifTRUE;
		return kifFALSE;
	}
	
};

//Then KifString
class KifString : public KifElement {
public:
	string value;
	KifString(KifElement* kp,string v="") : KifElement(kp,kifString) {value=v;}
	KifElement* Copy(KifElement* kp,KifElement* dom=kifNULL,bool temp=true) {
		KifElement* ke=new KifString(kp,value);
		ke->FreeReference(temp);
		return ke;
	}
	void Setvalue(KifElement* ke,KifElement* index=NULL) {
		value=ke->String();
	}
	KifElement* Value() {
		return this;
	}
	bool Predefinedtype() {return true;}
	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* ke=NULL,KifCallFunction* callfunc=NULL) {
		return this;
	}
	string String() {
		return value;
	}
	int Integer() {
		int i=atoi((char*)value.c_str());
		return i;
	}
	float Float() {
		float i=atof((char*)value.c_str());
		return i;
	}
	bool Boolean() {
		if (value=="")
			return false;
		return true;
	}

	KifElement* in(KifElement* a,KifElement* b,bool idx) {
		string s1=a->String();
		string s2=b->String();
		KifElement* kv=kifNULL;
		int rs=s2.find(s1);
		if (rs!=string::npos) {
			if (idx==false) 
				kv=new KifString(NULL,s2);
			else
				kv=new KifInteger(NULL,rs);
			kv->FreeReference();
			return kv;
		}
		return kv;
	}

	KifElement* plus(KifElement* a,KifElement* b) {
		string s=a->String()+b->String();
		KifElement* kv= new KifString(NULL,s);
		kv->FreeReference();
		return kv;
	}

	KifElement* minus(KifElement* a,KifElement* b) {
		KifElement* kv= new KifString(NULL,"");
		kv->FreeReference();
		return kv;
	}
	KifElement* multiply(KifElement* a,KifElement* b) {
		KifElement* kv= new KifString(NULL,"");
		kv->FreeReference();
		return kv;
	}
	KifElement* divide(KifElement* a,KifElement* b) {
		KifElement* kv= new KifString(NULL,"");
		kv->FreeReference();
		return kv;
	}
	KifElement* mod(KifElement* a,KifElement* b) {
		KifElement* kv= new KifString(NULL,"");
		kv->FreeReference();
		return kv;
	}
	KifElement* power(KifElement* a,KifElement* b) {
		KifElement* kv= new KifString(NULL,"");
		kv->FreeReference();
		return kv;
	}
	KifElement* less(KifElement* a) {
		if (value<a->String())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* more(KifElement* a) {
		if (value>a->String())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* same(KifElement* a) {
		if (value==a->String())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* different(KifElement* a) {
		if (value!=a->String())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* lessequal(KifElement* a) {
		if (value<=a->String())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* moreequal(KifElement* a) {
		if (value>=a->String())
			return kifTRUE;
		return kifFALSE;
	}
	
};



class KifFloat : public KifElement {
public:
	double value;
	KifFloat(KifElement* kp,double v=0) : KifElement(kp,kifFloat) {value=v;}
	KifElement* Copy(KifElement* kp,KifElement* dom=kifNULL,bool temp=true) {
		KifElement* ke=new KifFloat(kp,value);
		ke->FreeReference(temp);
		return ke;
	}
	
	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* ke=NULL,KifCallFunction* callfunc=NULL) {
		return this;
	}
	bool Predefinedtype() {return true;}

	void Setvalue(KifElement* ke,KifElement* index=NULL) {
		value=ke->Float();
	}
	KifElement* Value() {
		return this;
	}
	string String() {
		char str[100];
		sprintf(str,"%g",value);
		return str;		
	}
	int Integer() {
		return value;
	}
	float Float() {
		return value;
	}
	bool Boolean() {
		if (value==0)
			return false;
		return true;
	}

	KifElement* in(KifElement* a,KifElement* b,bool idx) {
		if (b->type==kifFloat)
			return kifNULL;
		return b->in(a,b,true);
	}

	KifElement* plus(KifElement* a,KifElement* b) {
		float i=a->Float()+b->Float();
		KifElement* kv= new KifFloat(NULL,i);
		kv->FreeReference();
		return kv;
		
	}
	KifElement* minus(KifElement* a,KifElement* b) {
		float i=a->Float()-b->Float();
		KifElement* kv= new KifFloat(NULL,i);
		kv->FreeReference();
		return kv;
	}
	KifElement* multiply(KifElement* a,KifElement* b) {
		float i=a->Float()*b->Float();
		KifElement* kv= new KifFloat(NULL,i);
		kv->FreeReference();
		return kv;
	}
	KifElement* divide(KifElement* a,KifElement* b) {
		if (b->Integer()==0)
			return kifNULL;
		float i=a->Float()/b->Float();
		KifElement* kv= new KifFloat(NULL,i);
		kv->FreeReference();
		return kv;
	}
	KifElement* mod(KifElement* a,KifElement* b) {
		float i=a->Integer()%b->Integer();
		KifElement* kv= new KifFloat(NULL,i);
		kv->FreeReference();
		return kv;
	}
	KifElement* power(KifElement* a,KifElement* b) {
		float i=a->Integer()^b->Integer();
		KifElement* kv= new KifFloat(NULL,i);
		kv->FreeReference();
		return kv;
	}
	KifElement* less(KifElement* a) {
		if (value<a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* more(KifElement* a) {
		if (value>a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* same(KifElement* a) {
		if (value==a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* different(KifElement* a) {
		if (value!=a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	
	KifElement* lessequal(KifElement* a) {
		if (value<=a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* moreequal(KifElement* a) {
		if (value>=a->Float())
			return kifTRUE;
		return kifFALSE;
	}
	
};

class KifBoolean : public KifElement {
public:
	bool value;
	KifBoolean(KifElement* kp,bool v=false) : KifElement(kp,kifBoolean) {value=v;}
	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* ke=NULL,KifCallFunction* callfunc=NULL) {
		return this;
	}

	void Setvalue(KifElement* ke,KifElement* index=NULL) {
		value=ke->Boolean();
	}
	KifElement* Value() {
		return this;
	}
	bool Predefinedtype() {return true;}

	string String() {
		if (value)
			return "True";
		return "false";
	}
	int Integer() {
		if (value)
			return 1;
		return 0;
	}
	float Float() {
		if (value)
			return 1;
		return 0;
	}
	bool Boolean() {
		return value;
	}

	KifElement* in(KifElement* a,KifElement* b,bool idx) {
		if (b->in(a,b,false)!=kifNULL)
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* plus(KifElement* a,KifElement* b) {
		bool v=true;
		int i=a->Integer()+b->Integer();
		if (i==0)
			v=false;
		KifElement* kv= new KifBoolean(NULL,v);
		kv->FreeReference();
		return kv;
		
	}
	KifElement* minus(KifElement* a,KifElement* b) {
		bool v=true;
		int i=a->Integer()-b->Integer();
		if (i==0)
			v=false;
		KifElement* kv= new KifBoolean(NULL,v);
		kv->FreeReference();
		return kv;
	}
	KifElement* multiply(KifElement* a,KifElement* b) {
		bool v=true;
		int i=a->Integer()*b->Integer();
		if (i==0)
			v=false;
		KifElement* kv= new KifBoolean(NULL,v);
		kv->FreeReference();
		return kv;
	}
	KifElement* divide(KifElement* a,KifElement* b) {
		if (b->Integer()==0)
			return kifNULL;
		bool v=true;
		int i=a->Integer()+b->Integer();
		if (i==0)
			v=false;
		KifElement* kv= new KifBoolean(NULL,v);
		kv->FreeReference();
		return kv;
	}
	KifElement* mod(KifElement* a,KifElement* b) {
		bool v=true;
		int i=a->Integer()%b->Integer();
		if (i==0)
			v=false;
		KifElement* kv= new KifBoolean(NULL,v);
		kv->FreeReference();
		return kv;
	}
	KifElement* power(KifElement* a,KifElement* b) {
		bool v=true;
		int i=a->Integer()^b->Integer();
		if (i==0)
			v=false;
		KifElement* kv= new KifBoolean(NULL,v);
		kv->FreeReference();
		return kv;
	}
	KifElement* less(KifElement* a) {
		if (value<a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* more(KifElement* a) {
		if (value>a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* same(KifElement* a) {
		if (value==a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* different(KifElement* a) {
		if (value!=a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}	
	KifElement* lessequal(KifElement* a) {
		if (value<=a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}
	KifElement* moreequal(KifElement* a) {
		if (value>=a->Boolean())
			return kifTRUE;
		return kifFALSE;
	}
	
};


class KifVector : public KifElement {
public:
	vector<KifElement*> values;
	KifVector(KifElement* kp) : KifElement(kp,kifVector) {}
	KifElement* Copy(KifElement* kp,KifElement* dom=kifNULL,bool temp=true) {
		KifElement* ke=new KifVector(kp);
		ke->FreeReference(temp);
		return ke;
	}
	KifElement* Execute(KifElement* pattern,KifElement* dom,KifElement* ke,KifCallFunction* callfunc=NULL) {
		if (ke==NULL || ke==kifNULL) {
			KifElement* kv=new KifInteger(NULL,values.size());
			kv->FreeReference();
			return kv;
		}
		int key=ke->Integer();
		if (key>=values.size())
			return kifNULL;
		return values[key];
	}
	bool Multiple() {return true;}
	void Setvalue(KifElement* ke,KifElement* index) {
		if (ke==this)
			return;
		vector<KifElement*>::iterator it=values.begin();
		if (index==NULL) {
			if (ke->type!=kifVector)
				return;
			KifVector* kvect=(KifVector*)ke;
			//We copy all values from ke to this
			Clean();
			for (it=kvect->values.begin();it!=kvect->values.end();it++) {
				KifElement* x=*it;
				values.push_back(x);
			}
			return;
		}
		
		int key=index->Integer();
		ke->reference++;
		if (key>=values.size())
			values.push_back(ke);
		else {
			if (values[key]!=NULL)
				values[key]->FreeReference();
			values[key]=ke;
		}
	}
	void Clean() {
		vector<KifElement*>::iterator it;
		for (it=values.begin();it!=values.end();it++) {
			(*it)->FreeReference();
		}
		values.clear();
	}

	KifElement* in(KifElement* a,KifElement* b,bool idx) {
		if (b!=this)
			Clean();
		KifVector* bb=(KifVector*)b;
		vector<KifElement*>::iterator it;
		int ix=0;
		for (it=bb->values.begin();it!=bb->values.end();it++) {
			if ((*it)->same(a)==kifTRUE) {
				if (b!=this) {
					values.push_back(*it);
					(*it)->reference++;
				}
				else {
					if (idx==true) {
						KifElement* ke=new KifInteger(NULL,ix);
						ke->FreeReference();
						return ke;
					}
					return kifTRUE;
				}
			}
			ix++;
		}
		if (b!=this && values.size()!=0)
			return this;
		return kifFALSE;
	}
	int Integer() {
		return values.size();
	}
	float Float() {
		return values.size();
	}
};

class KifMap : public KifElement {
public:
	map<string,KifElement*> values;
	KifMap(KifElement* kp) : KifElement(kp,kifMap) {}
	KifElement* Copy(KifElement* kp,KifElement* dom=kifNULL,bool temp=true) {
		KifElement* ke=new KifMap(kp);
		ke->FreeReference(temp);
		return ke;
	}
	bool Multiple() {return true;}

	KifElement* Execute(KifElement* pattern,KifElement* dom,KifElement* ke,KifCallFunction* callfunc=NULL) {
		if (ke==NULL || ke==kifNULL) {
			KifElement* kv=new KifInteger(NULL,values.size());
			kv->FreeReference();
			return kv;
		}
		string key=ke->String();
		KifElement* kval=values[key];
		if (kval==NULL)
			return kifNULL;
		return kval;
	}
	void Setvalue(KifElement* ke,KifElement* index) {
		if (ke==this)
			return;
		if (index==NULL) {
			if (ke->type!=kifMap)
				return;
			map<string,KifElement*>::iterator it;
			KifMap* kvect=(KifMap*)ke;
			//We copy all values from ke to this
			Clean();
			for (it=kvect->values.begin();it!=kvect->values.end();it++) {
				KifElement* x=it->second;
				string n=it->first;
				values[n]=x;
			}
		}
		else {
			string key=index->String();
			ke->reference++;
			if (values[key]!=NULL)
				values[key]->FreeReference();
			values[key]=ke;
		}
	}
	void Clean() {
		map<string,KifElement*>::iterator itm;
		for (itm=values.begin();itm!=values.end();itm++) {
			(*itm).second->FreeReference();
		}
		values.clear();
	}

	KifElement* in(KifElement* a,KifElement* b,bool idx) {
		if (idx==true)
			return kifNULL;
		if (b->type!=kifMap)
			return b->in(a,b,idx);
		KifMap* bb=(KifMap*)b;
		string s=a->String();
		if (bb->values[s]!=NULL)
			return kifTRUE;
		return kifFALSE;
	}

	int Integer() {
		return values.size();
	}
	float Float() {
		return values.size();
	}

};

class KifFile : public KifElement {
public:

	string filename;
	ifstream* readfile;
	ofstream* writefile;
	char buffer[1024];

	KifFile(KifElement* kp) : KifElement(kp,kifFile) {
		readfile=NULL;
		writefile=NULL;
	}

	bool Openread(string n) {
		if (readfile!=NULL)
			return kifFALSE;
		readfile=new ifstream((char*)n.c_str(),ios::in|ios::binary);
		if (readfile->fail()) {
			readfile=NULL;
			return false;
		}
		return true;
	}

	bool Openwrite(string n) {
		if (writefile!=NULL)
			return kifFALSE;
		writefile=new ofstream((char*)n.c_str());
		if (writefile->fail()) {
			writefile=NULL;
			return false;
		}
		return true;
	}

	void Close() {
		if (writefile!=NULL)
			writefile->close();
		if (readfile!=NULL)
			readfile->close();
		writefile=NULL;
		readfile=NULL;
	}
	bool Write(string n) {
		if (writefile==NULL)
			return false;
		*writefile<<(char*)n.c_str();
		return true;
	}

	KifElement* Read(size_t sz=1024) {
		if (readfile==NULL)
			return kifNULL;
		if (sz>1024)
			sz=1024;
		readfile->read(buffer,sz);
		KifElement* ke=new KifString(this,buffer);
		ke->FreeReference();
		return ke;
	}

	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* val=kifNULL,KifCallFunction* callfunc=NULL) {
		return this;
	}
	~KifFile() {
		Close();
	}
};


class KifIndex : public KifElement {
public:
	KifElement* key;
	KifIndex(KifElement* kp) : KifElement(kp,kifIndex) {
		key=NULL;
	}
	void Add(KifElement* ke) {
		key=ke;
	}
	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* ke=NULL,KifCallFunction* callfunc=NULL) {
		return key->Execute(pattern,dom,ke,callfunc);
	}
};


class KifCouple : public KifElement {
public:	
	KifElement* index;
	KifElement* recipient;

	KifCouple(KifElement* rec,KifElement* i) : KifElement(NULL,kifCouple) {
		index=i;
		recipient=rec;
		reference=0;
	}
	void Setvalue(KifElement* k,KifElement* index=NULL);
	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* ke=NULL,KifCallFunction* callfunc=NULL) {
		return recipient->Execute(pattern,dom,index,callfunc);
	}
	string String() {
		KifElement* ke=Execute(this,this,NULL,NULL);
		return ke->String();
	}
	int Integer() {
		KifElement* ke=Execute(this,this,NULL,NULL);
		return ke->Integer();
	}
	float Float() {
		KifElement* ke=Execute(this,this,NULL,NULL);
		return ke->Float();
	}
	bool Boolean() {
		KifElement* ke=Execute(this,this,NULL,NULL);
		return ke->Boolean();
	}	
	KifElement* plus(KifElement* a,KifElement* b) {
		if (a->type==kifCouple)
			a=a->Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->plus(a,b);
	}
	KifElement* minus(KifElement* a,KifElement* b) {
		if (a->type==kifCouple)
			a=a->Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->minus(a,b);
	}
	KifElement* multiply(KifElement* a,KifElement* b) {
		if (a->type==kifCouple)
			a=a->Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->multiply(a,b);
	}
	KifElement* divide(KifElement* a,KifElement* b) {
		if (a->type==kifCouple)
			a=a->Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->divide(a,b);
	}
	KifElement* mod(KifElement* a,KifElement* b) {
		if (a->type==kifCouple)
			a=a->Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->mod(a,b);
	}
	KifElement* power(KifElement* a,KifElement* b) {
		if (a->type==kifCouple)
			a=a->Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->power(a,b);
	}
	KifElement* less(KifElement* b) {
		KifElement* a=Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->less(b);
	}
	KifElement* more(KifElement* b) {
		KifElement* a=Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->more(b);
	}
	KifElement* same(KifElement* b) {
		KifElement* a=Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->same(b);
	}
	KifElement* different(KifElement* b) {
		KifElement* a=Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->different(b);
	}
	
	KifElement* lessequal(KifElement* b) {
		KifElement* a=Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->lessequal(b);
	}
	KifElement* moreequal(KifElement* b) {
		KifElement* a=Execute();
		if (b->type==kifCouple)
			b=b->Execute();
		return a->moreequal(b);
	}	
};

//A basic variable declaration is of type: KifType
class KifVariableDeclaration : public KifElement {
public:
	string name; //can be a frame as well
	KifElement* typevariable;
	KifElement* initialisation;
	KifVariableDeclaration(KifElement* kp,string n,KifType kt=kifVariableDeclaration) : name(n),KifElement(kp,kt) {
		typevariable=NULL;		
		initialisation=NULL;
	}
	
	void Setinitialisation(KifElement* ke) {
		initialisation=ke;
	}

	void Add(KifElement* ke) {
		typevariable=ke;
	}
	string Name() {
		return name;
	}
	
	KifType Typevariable() {
		return typevariable->type;
	}

	KifElement* Variabledeclaration() {
		return typevariable;
	}

	bool Variable() {
		return true;
	}
	virtual KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* val=kifNULL,KifCallFunction* callfunc=NULL);
	
	void print();
	bool Multiple() {
		if (typevariable!=NULL && typevariable!=this)
			return typevariable->Multiple();
		return false;
	}
	virtual KifElement* Copy(KifElement* kp,KifElement* dom,bool temp) {
		KifElement* ke=typevariable->Copy(kp,dom,temp);
		ke->FreeReference(temp);
		kp->Set(name,ke,NULL);
		return ke;
	}
};


class KifConst : public KifVariableDeclaration {
public:

	KifConst(KifElement* kp,string n) : KifVariableDeclaration(kp,n,kifVOID) {
		typevariable=this;
	}
	KifElement* Value() {return this;}
	KifElement* Copy(KifElement* kp,KifElement* dom,bool temp) {
		return kifNULL;
	}
};

class KifFrameDeclaration : public KifVariableDeclaration {
public:
	KifElement* frame;
	KifFrameDeclaration(KifElement* kp,string n,KifElement* d) : KifVariableDeclaration(kp,n,kifFrameDeclaration) {
		frame=d;
	}
	
	KifElement* Domain() {
		return frame;
	}
	
};

class KifFunctionDeclaration : public KifVariableDeclaration {
public:
	KifElement* function;
	KifFunctionDeclaration(KifElement* kp,string n,KifElement* d) : KifVariableDeclaration(kp,n,kifFunctionDeclaration) {
		function=d;
	}
	KifElement* Domain() {
		return function;
	}
};


//This class is used when a variable is in an expression
class KifInstance : public KifElement {
public:
	//The domain is its type either in a frame or in a function
	string name;
	KifVariableDeclaration* declaration;
	KifElement* value;
	KifIndex* index;
	bool returninstance;
	
	KifElement* Returninstance(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* val=kifNULL,KifCallFunction* callfunc=NULL) {
		returninstance=true;
		KifElement* ke=Execute(pattern, dom,val,callfunc);
		returninstance=false;
		return ke;
	}
	
	KifInstance(KifElement* kp,string n,KifVariableDeclaration* d) : KifElement(kp,kifInstance) {
		declaration=d;
		name=n;
		value=d->typevariable;
		index=NULL;
		returninstance=false;
	}

	KifElement* Execute(KifElement* pattern,KifElement* dom,KifElement* val,KifCallFunction* callfunc);
	
	KifElement* Searchtypefull(string& name) {
		return declaration->typevariable->Searchtypefull(name);
	}
	
	void Initvalue(KifElement* ke) {
		if (ke->type==kifIndex)
			index=(KifIndex*)ke;
		else
			value=ke;
	}
};

class KifDomain : public KifObject {
public:

	map<string,KifElement*> declarations;
	string name;
	KifElement* frame;
	KifDomain(KifElement* kp,string n,KifType t) : KifObject(kp,t) {
		name=n;
		frame=kifNULL;
	}
	KifElement* Declaration(string& s) {
		return declarations[s];
	}
	virtual bool Multiple() {return true;}

	string Name() {return name;}

	virtual KifElement* Copy(KifElement* dom,KifElement* decl,bool temp) {
		map<string,KifElement*>::iterator it;
		KifDomain* kdom=new KifDomain(dom,decl->Name(),kifDomain);
		kdom->frame=decl->Variabledeclaration();
		kdom->Disableadding();
		for (it=declarations.begin();it!=declarations.end();it++) {
			string n=it->first;
			KifElement* xe=Searchtype(n);
			if (xe!=NULL) {
				KifElement* ke=xe->Copy(kdom,xe,temp);
				kdom->declarations[n]=ke;
			}
		}
		return kdom;
	}	

	void SetInstances(KifElement* dom) {
		map<string,KifElement*>::iterator it;
		dom->Disableadding();
		for (it=declarations.begin();it!=declarations.end();it++) {
			string n=it->first;
			KifElement* xe=Searchtype(n);
			if (xe!=NULL)
				((KifDomain*)dom)->declarations[n]=xe;
		}
	}	

	void CopyInstances(KifElement* dom);

	virtual void Setvalue(KifElement* ke,KifElement* idx=NULL) {
		if (frame!=kifNULL && frame->type==ke->Frame())
			ke->CopyInstances(this);
	}
	
	KifType  Frame() {
		return frame->type;
	}
	KifElement* Framedefined(string met,KifElement* dom=kifNULL);
	
	string String() {
		KifElement* ke=Framedefined("String",this);
		return ke->String();
	}

	int Integer() {
		KifElement* ke=Framedefined("Integer",this);
		return ke->Integer();
	}
	float Float() {
		KifElement* ke=Framedefined("Float",this);
		return ke->Float();
	}
	bool Boolean() {
		KifElement* ke=Framedefined("Boolean",this);
		return ke->Boolean();
	}

	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* val=kifNULL,KifCallFunction* callfunc=NULL);
	virtual void Set(string& s,KifElement* k,KifLoadCode*);


	void print() {
		cout<<"Frame:"<<name<<endl;
		map<string,KifElement*>::iterator it;
		for (it=declarations.begin();it!=declarations.end();it++) {
			cout<<it->first<<endl;
		}
	}
};

//Next is the basic Kif instruction: KifFunction
class KifFunction : public KifDomain {
public:
	vector<KifElement*> arguments;
	char choice;
	
	bool Multiple() {return false;}
	KifFunction(KifElement* kp,string n) : KifDomain(kp,n,kifFunction) {
		choice=0;
		declarations[name]=this;
	}
	void Add(KifElement* ke) {
		if (choice==0)
			arguments.push_back(ke);
		else
			KifObject::Add(ke);
	}
	void Setvalue(KifElement* ke,KifElement* idx=NULL) {
		return;
	}
	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* val=kifNULL,KifCallFunction* callfunc=NULL);
	KifElement* Copy(KifElement* dom,KifElement* decl,bool temp) {
		return this;
	}
};

class KifCallFunction : public KifDomain {
public:
	KifFunction* body;
	vector<KifElement*> parameters;
	KifElement* returnvalue;
	
	KifCallFunction(KifElement* kp,string& n,KifFunction* b) : KifDomain(kp,n,kifCallFunction) {
		returnvalue=kifNULL;
		body=b;
	}

	bool Multiple() {return false;}
	KifElement* Execute(KifElement* pattern=kifNULL,KifElement* dom=kifNULL,KifElement* val=kifNULL,KifCallFunction* callfunc=NULL);
	void Initialisation(KifElement* ke=NULL) {
		returnvalue=kifNULL;
	}
	void Setreturnvalue(KifElement* k) {
		returnvalue=k;
	}
	void Add(KifElement* ke) {
		parameters.push_back(ke);
	}
	KifElement* Returnvalue() {
		return returnvalue;
	}
	string Name() {return name;}
};

class KifPredefined : public KifElement {
public:
	string name;
	Predefined func;
	
	KifPredefined(Predefined f,string n) : KifElement(NULL,kifPredefined) {
		func=f;
		name=n;
	}
	bool Multiple() {return false;}
	KifElement* Execute(KifElement* a,KifElement* b,KifElement* c,KifCallFunction* d) {
		return (*func)(a,b,c,d);
	}
};

//A User Based Variable: frame
class KifFrame : public KifDomain {
public:
	KifFrame(KifElement* kp,string n) : KifDomain(kp,n,kifFrame) {}
	void Set(string& s,KifElement* k,KifLoadCode*);
	void Run();
	
};


class KifLoadCode {
public:	
	map<string,KifElement*> kifInstructions;
	x_reading* current;
	KifElement* kstart;
	vector<KifElement*> stack;

	KifElement* KifParseVarUserDefined(x_node* xn ,KifElement* kf);
	KifElement* KifParseAString(x_node* xn ,KifElement* kf);
	KifElement* KifParseANumber(x_node* xn ,KifElement* kf);
	KifElement* KifParseOperation(x_node* xn ,KifElement* kf);
	KifElement* KifParseOperator(x_node* xn ,KifElement* kf);
	KifElement* KifParseVarInt(x_node* xn ,KifElement* kf);
	KifElement* KifParseVarString(x_node* xn ,KifElement* kf);
	KifElement* KifParseVarFloat(x_node* xn ,KifElement* kf);
	KifElement* KifParseVarVector(x_node* xn ,KifElement* kf);
	KifElement* KifParseVarMap(x_node* xn ,KifElement* kf);
	KifElement* KifParseVarBoolean(x_node* xn ,KifElement* kf);
	KifElement* KifParseVarFile(x_node* xn ,KifElement* kf);
	KifElement* KifParseVariable(x_node* xn ,KifElement* kf);
	KifElement* KifParseDeclaration(x_node* xn,KifElement* kf);
	KifElement* KifParseIndexes(x_node* xn,KifElement* kf);
	KifElement* KifParseCall(x_node* xn,KifElement* kf);
	KifElement* KifParseUniqueCall(x_node* xn,KifElement* kf);
	KifElement* KifParseBloc(x_node* xn,KifElement* kf);
	KifElement* KifParseTest(x_node* xn,KifElement* kf);
	KifElement* KifParseLoop(x_node* xn,KifElement* kf);
	KifElement* KifParseFor(x_node* xn,KifElement* kf);
	KifElement* KifParseBoolean(x_node* xn,KifElement* kf);
	KifElement* KifParseABoolean(x_node* xn,KifElement* kf);
	void ComputeParameters(x_node* xn,KifCallFunction* kcf);
	//Function creation
	KifElement* KifParseFunction(x_node* xn,KifElement* kf);
	KifElement* KifParseAffectation(x_node* xn,KifElement* kf);
	KifElement* KifParseFrame(x_node* frame,KifElement* kf);
	KifElement* CreateVariable(x_node* xn,string& name,KifElement* kf);
	void KifBrowse(x_node* xn,KifElement* kf);
	KifLoadCode(x_reading* xr);
	//We keep a map of each function, which is then called from within KifBrowse
	//It reduces the complexity of a huge switch case on strings.
};

#endif
