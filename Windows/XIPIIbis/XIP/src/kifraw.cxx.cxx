/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2010 - 2011 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the KiF library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : KiF
   Version    : 1.00
   filename   : kifraw.cxx
   Date       : 09/09/2010
   Purpose    : Implementation of the KiF language: External method implementation.
   Programmer : Claude ROUX
   Reviewer   : 
*/

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//KIF RAW SECTION
//Below are the basic bodies of specific methods, whose bodies are different in the XIP version

#include "kifinit.h"
#include "automate.h"
#include "kifversion.h"

extern const char* kifErrorStrings[];
#ifdef _DEBUG
extern KifCode* currentkifcode;
#endif

#ifdef WIN32
int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

int GETCH(bool echoing);
KifDebugInfo* DebugPtr();
int KifLoadin(KifCode* kifcode,string path);
void KifInitFunctions(KifCode* kifcodeptr,KifFrame* kstart);
KifVector* Selectvector(KifElement* kref,KifElement* context);
KifMap* Selectmap(KifElement* kref,KifElement* context);
string replacestringwithautomaton(string &str, string& reg,string& rep);
extern KifElement* kifWAIT;
string KifIdString(short n);
string Whoami_kif();
extern KifGlobalThread* kifGlobals;

#ifndef WIN32
#define sprintf_s(a,b,c,d) sprintf(a,c,d)
#define strcpy_s(a,b,c) strcpy(a,c)
#endif

//===================================================================
class KifComparison {
public:

	KifDomain* domain;
	KifFunction* compare;

	KifComparison(KifDomain* dom,KifFunction* comp) {
		domain=dom;
		compare=comp;
	}

	bool operator() (KifElement* i,KifElement* j) { 
		return domain->applycomparatorfunction(i,j,compare)->Boolean();
	}
};
//===================================================================
//We remove this element everywhere in KifCode kifWaitstates
hmap<string,vector<KifWaitState*> > kifWaitstates;

KifWaitStates::~KifWaitStates() {
	for (int i=0;i<flags.size();i++) {
		bool found=false;
		string flag=flags[i];
		if (kifWaitstates.find(flag)!=kifWaitstates.end()) {
			vector<KifWaitState*>& kws=kifWaitstates[flag];
			vector<KifWaitState*>::iterator it;
			for (it=kws.begin();it!=kws.end();it++) {
				if (*it==this) {
					found=true;
					break;
				}
			}
			if (found) {
				kws.erase(it);
				if (kws.empty())
					kifWaitstates.erase(flag);
			}
		}
	}
}

vector<KifWaitState*>* Freestate(KifCode* kifcode,string& flag) {
	ThreadLock _lock(kifSignal);
	if (kifWaitstates.find(flag)==kifWaitstates.end()) {
		string message=KIFRAWSTRINGS[0];
		message+=flag;
		message+="'";
		kifcode->Returnerror(message);
		return NULL;
	}
	return &kifWaitstates[flag];
}

//--------------------------------------------------------

KifElement* PreRange(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	KifElement* ksecond=callfunc->Evaluate(1,dom);
	if (kfirst->inType()==kifInteger && ksecond->inType()==kifInteger) {
		long l=kfirst->Integer();
		long r=ksecond->Integer();
		long d=r-l+1;	
		long inc=1;
		if (callfunc->Size()==3) 
			inc=callfunc->Evaluate(2,dom)->Integer();
		if (d<0)
			d*=-1;
		if (d<=10000) {
			KifVector* kvect=Selectvector(callfunc,contextualpattern);
			kvect->values.reserve(d);
			for (long i=l;i<r;i+=inc) 
				kvect->values.push_back(callfunc->kifcode->Provideinteger(i));
			return kvect;
		}
	}
	else
		if (kfirst->inType()==kifFloat || ksecond->inType()==kifFloat) {
			double l=kfirst->Float();
			double r=ksecond->Float();
			double inc=1;
			if (callfunc->Size()==3) 
				inc=callfunc->Evaluate(2,dom)->Float();
			if (inc==0)
				return callfunc->kifcode->Returnerror(kifErrorStrings[225]);
			double d=(r-l)/inc;
			if (d<0)
				d*=-1;
			if (d<=10000) {
				KifVector* kvect=Selectvector(callfunc,contextualpattern);
				kvect->values.reserve(d);
				for (double i=l;i<r;i+=inc) 
					kvect->values.push_back(callfunc->kifcode->Providefloat(i));
				return kvect;
			}
		}
		else
			if (kfirst->inType()==kifString || kfirst->inType()==kifStringPtr) {
				string l=kfirst->String();
				string r=ksecond->String();
				if (l.size()!=1 || r.size()!=1)
					return callfunc->kifcode->Returnerror(kifErrorStrings[230]);
				int inc=1;
				if (callfunc->Size()==3) 
					inc=callfunc->Evaluate(2,dom)->Float();
				char cl=l[0];
				char cr=r[0];
				int d=cr-cl;
				if (d<0)
					d*=-1;
				KifVector* kvect=Selectvector(callfunc,contextualpattern);
				kvect->values.reserve(d);				
				for (uchar i=cl;i<cr;i+=inc) {
					l=(char)i;
					kvect->values.push_back(callfunc->kifcode->Providestring(l));
				}
				return kvect;
			}
	return callfunc->kifcode->Returnerror(kifErrorStrings[227]);
}

//
KifElement* PrePushFirst(KifElement* kf,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter should be a vector
	kf=callfunc->Evaluate(0,dom);
	if (kf->type==kifList) {
		//next parameters are anything
		for (size_t i=1;i<callfunc->Size();i++)
			((KifList*)kf)->PushFirst(callfunc->Evaluate(i,dom));
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[39]);
}

KifElement* PrePushLast(KifElement* kf,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter should be a vector
	kf=callfunc->Evaluate(0,dom);
	if (kf->type==kifList) {
		//next parameters are anything
		for (size_t i=1;i<callfunc->Size();i++)
			((KifList*)kf)->PushLast(callfunc->Evaluate(i,dom));
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[39]);
}

KifElement* PrePopFirst(KifElement* kf,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter should be a vector
	kf=callfunc->Evaluate(0,dom);
	if (kf->type==kifList) {
		((KifList*)kf)->PopFirst();
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[41]);
}

KifElement* PrePopLast(KifElement* kf,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter should be a vector
	kf=callfunc->Evaluate(0,dom);
	if (kf->type==kifList) {
		((KifList*)kf)->PopLast();
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[41]);
}

KifElement* PreFirst(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc) {
	return callfunc->Evaluate(0,dom)->First();
}


KifElement* PreGetc(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//Reading of one character
	int c=GETCH(false);
	if (contextualpattern->type==kifInteger || contextualpattern->type==kifFloat)
		return callfunc->kifcode->Provideinteger(c);
	string s;
	s=(uchar)c;
	return callfunc->kifcode->Providestring(s);
}


KifElement* PreComplement(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {

	KifElement* kv=callfunc->Evaluate(0,dom);
	if (kv->type==kifBit)
		return ((KifBit*)kv)->Complement();
	else
		if (kv->type==kifBits)
			return ((KifBits*)kv)->Complement();
		else
			if (kv->type==kifBoolean) {
				if (kv->Boolean()==false)
					return new KifBoolean(callfunc->kifcode,NULL,true);
				return new KifBoolean(callfunc->kifcode,NULL,false);
			}
	long v=kv->Integer();
	v=~v;
	return callfunc->kifcode->Provideinteger(v);
}


KifElement* PreXipReplaceRgx(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//Replace an element with another

	string str=callfunc->Evaluate(0,dom)->String();
	string reg=callfunc->Evaluate(1,dom)->String();
	string rep=callfunc->Evaluate(2,dom)->String();

	string res=replacestringwithautomaton(str,reg,rep);
	return callfunc->kifcode->Providestring(res);
}


KifElement* PreXipRegex(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {

	string str=callfunc->Evaluate(0,dom)->String();
	string reg=callfunc->Evaluate(1,dom)->String();
	automate a(USTR(reg),(void*)1);
	if (contextualpattern->type==kifVector) {
		KifBaseString localstring(NULL,NULL);
		localstring.kifcode=callfunc->kifcode;

		KifVector* vect=Selectvector(callfunc,contextualpattern);
		VECTA<string*> vectr;
		a.rechercheavecextractiondansvecteur(USTR(str),vectr);
		for (int iv=0;iv<vectr.dernier;iv++) {
			localstring.value=*vectr[iv];
			vect->Push(&localstring);
		}
		vectr.nettoie();
		return vect;
	}
	if (contextualpattern->type==kifInteger || contextualpattern->type==kifFloat || contextualpattern->type==kifString) {
		int res=a.rechercheavecextraction(USTR(str),reg);
		if (contextualpattern->type==kifString)
			return callfunc->kifcode->Providestring(reg);
		return callfunc->kifcode->Provideinteger(res);
	}

	etat* e=a.recherche(USTR(str)); 
	if (e==NULL) 
		return kifFALSE;
	return kifTRUE;	
}


//test(value);
KifElement* PreTest(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//check an index value for a container
	return callfunc->Evaluate(0,dom)->Index(callfunc->Evaluate(1,dom));
}


//find(s,i);
KifElement* PreFind(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {

	//we search for a substring starting at position idx
	string str=callfunc->Evaluate(0,dom)->String();
	if (str=="")
		return kifMINUSONE;
	int i=0;
	if (callfunc->Size()==3) 
		i=callfunc->Evaluate(2,dom)->Integer();
	string sub=callfunc->Evaluate(1,dom)->String();

	if (contextualpattern->type==kifVector) {
		KifVector* kvect=Selectvector(callfunc,contextualpattern);
		KifInteger ki(NULL,NULL,0);
		i=c_find(str,sub,i);
		while (i!=-1) {
			ki.value=i;
			kvect->Push(&ki);
			i++;
			i=c_find(str,sub,i);
		}
		return kvect;
	}

	i=c_find(str,sub,i);
	if (i==string::npos)
		return kifMINUSONE;

	return callfunc->kifcode->Provideinteger(i);
}

KifElement* PreRFind(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {

	//we search for a substring starting at position idx
	string str=callfunc->Evaluate(0,dom)->String();
	if (str=="")
		return kifMINUSONE;
	int i=str.size()-1;
	if (callfunc->Size()==3)
		i=callfunc->Evaluate(2,dom)->Integer();
	string sub=callfunc->Evaluate(1,dom)->String();

	if (contextualpattern->type==kifVector) {
		KifVector* kvect=Selectvector(callfunc,contextualpattern);
		KifInteger ki(NULL,NULL,0);
		ki.kifcode=callfunc->kifcode;
		i=c_find(str,sub,i);
		while (i!=-1) {
			ki.value=i;
			kvect->Push(&ki);
			i++;
			i=c_find(str,sub,i);
		}
		return kvect;
	}

	i=c_rfind(str,sub,i);
	if (i==string::npos)
		return kifMINUSONE;
	return callfunc->kifcode->Provideinteger(i);
}

//From a character to a byte position
KifElement* PreBytePosition(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {

	string s=callfunc->Evaluate(0,dom)->String();	
	long i=callfunc->Evaluate(1,dom)->Integer();
	i=c_chartobyteposition(USTR(s),i);
	return callfunc->kifcode->Provideinteger(i);
}

//From a byte position to a character
KifElement* PreCharacterPosition(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {

	string s=callfunc->Evaluate(0,dom)->String();	
	long i=callfunc->Evaluate(1,dom)->Integer();	
	i=c_bytetocharposition(USTR(s),i);
	return callfunc->kifcode->Provideinteger(i);
}

KifElement* PreSplit(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifBaseString localstring(NULL,NULL);
	localstring.kifcode=callfunc->kifcode;

	//Third parameter should be a vector
	KifVector* kvect;
	bool returnvalue=false;
	if (callfunc->Size()==3) {
		contextualpattern=callfunc->Evaluate(2,dom);
		if (contextualpattern==kifNULL || contextualpattern->type!=kifVector)
			return callfunc->kifcode->Returnerror(kifErrorStrings[1]);
		kvect=(KifVector*)contextualpattern;
		kvect->Clear();
		returnvalue=true;
	}
	else
		kvect=Selectvector(callfunc,contextualpattern);

	//First parameter is a string to split
	string thestr=callfunc->Evaluate(0,dom)->String();


	//Second parameter is the splitter string
	string thesplitter=callfunc->Evaluate(1,dom)->String();
	if (thesplitter=="") {		
		for (int i=0;i<thestr.size();i++) {
			localstring.value=c_char_get(USTR(thestr),i);
			kvect->Push(&localstring);
		}
		if (returnvalue)
			return kifTRUE;
		else
			return kvect;
	}

	size_t pos=0;
	size_t found=0;

	while (pos!=string::npos) {
		found=thestr.find(thesplitter,pos);
		if (found!=string::npos) {
			localstring.value=thestr.substr(pos,found-pos);
			if (localstring.value!="")
				kvect->Push(&localstring);
			pos=found+thesplitter.size();
		}
		else
			break;
	}
	localstring.value=thestr.substr(pos,thestr.size()-pos);
	if (localstring.value!="")
		kvect->Push(&localstring);
	
	if (returnvalue)
		return kifTRUE;
	else
		return kvect;	
}

static locale loc;                 // the "C" locale
static const collate<char>& coll = use_facet<collate<char> >(loc);

KifElement* PreHash(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* ke=callfunc->Evaluate(0,dom);
	string s=ke->String();
	if (s=="")
		return kifNULL;
	long myhash = coll.hash(s.data(),s.data()+s.length());
	return callfunc->kifcode->Provideinteger(myhash);
}


KifElement* PreConvertVector(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* ke=callfunc->Evaluate(0,dom);
	if (ke->isString())
		return ke->Vector(dom);
	return ke->Vector(contextualpattern);
}

KifElement* PreConvertMap(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* ke=callfunc->Evaluate(0,dom);
	if (ke->isString())
		return ke->Map(dom);
	return ke->Map(contextualpattern);
}

KifElement* PreConvertString(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* ke=callfunc->Evaluate(0,dom);
	return callfunc->kifcode->Providestringraw(ke->String());
}

KifElement* PreConvertFloat(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* ke=callfunc->Evaluate(0,dom);
	return callfunc->kifcode->Providefloat(ke->Float());
}

KifElement* PreConvertFraction(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* ke=callfunc->Evaluate(0,dom);
	return ke->Fraction();
}


KifElement* PreConvertInteger(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* ke=callfunc->Evaluate(0,dom);
	return callfunc->kifcode->Provideinteger(ke->Integer());
}

KifElement* PreConvertLong(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* ke=callfunc->Evaluate(0,dom);
	return new KifLong(callfunc->kifcode,NULL,ke->Long());
}

KifElement* PreConvertBoolean(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* ke=callfunc->Evaluate(0,dom);
	return new KifBoolean(callfunc->kifcode,NULL,ke->Boolean());
}


KifElement* PreMethods(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	
	KifElement* ke=callfunc->Evaluate(0,dom);
	KifVector* kvect=Selectvector(callfunc,contextualpattern);	
	ke->Methods(kvect);
	return kvect;
}

KifElement* PreInfos(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	
	KifElement* ke=callfunc->Evaluate(0,dom);
	KifElement* kstr=callfunc->Evaluate(1,dom);
	return ke->Information(kstr);
}

KifElement* PreUTF8ToLatin(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();

	string res=conversionUTF8VersLatin(USTR(thestr));
	return callfunc->kifcode->Providestring(res);
}

KifElement* PreLatinToUTF8(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreLatinToUTF8");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();

	string res=conversionLatinVersUTF8(USTR(thestr));
	return callfunc->kifcode->Providestring(res);
}


KifElement* PreLeft(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreLeft");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();

	//Second parameter is an integer
	KifElement* sz=callfunc->Evaluate(1,dom);
	int nbchar=sz->Integer();

	string res=c_left(thestr,nbchar);
	return callfunc->kifcode->Providestring(res);
}

KifElement* PreRight(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreRight");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();

	//Second parameter is an integer
	KifElement* sz=callfunc->Evaluate(1,dom);
	int nbchar=sz->Integer();

	string res=c_right(thestr,nbchar);
	return callfunc->kifcode->Providestring(res);
}

KifElement* PreMid(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreMid");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();

	//Second parameter is a position
	KifElement* sz=callfunc->Evaluate(1,dom);
	int pos=sz->Integer();

	//Third parameter is a number of characters
	sz=callfunc->Evaluate(2,dom);
	int nbchar=sz->Integer();

	string res=c_middle(thestr,pos,nbchar);
	return callfunc->kifcode->Providestring(res);
}

KifElement* PreIsLower(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreIsLower");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();
	return new KifBoolean(callfunc->kifcode,NULL,c_Islower(thestr));
}

KifElement* PreIsUpper(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreIsUpper");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();
	return new KifBoolean(callfunc->kifcode,NULL,c_Isupper(thestr));
}

KifElement* PreIsDigit(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreIsDigit");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();
	string::iterator it;
	static char tabs[]={'0','1','2','3','4','5','6','7','8','9'};
	static char punct[]={'.',',','-'};
	for (it=thestr.begin();it!=thestr.end();it++) 
		if (strchr(tabs,*it)==NULL) {
			if (strchr(punct,*it)==NULL || thestr.size()==1)
				return kifFALSE;		
		}
	return kifTRUE;
}


KifElement* PreIsAlpha(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreIsAlpha");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();
	return new KifBoolean(callfunc->kifcode,NULL,c_Isalpha(thestr));
}


KifElement* PreLower(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreLower");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();
	thestr=c_Tolower(thestr);
	return callfunc->kifcode->Providestring(thestr);
}

KifElement* PreUpper(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreUpper");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();
	thestr=c_Toupper(thestr);
	return callfunc->kifcode->Providestring(thestr);
}


KifElement* PreTrim(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreTrim");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();
	int start=0;
	for (;start<thestr.size();start++) {
		if (!strchr(KIFRAWSTRINGS[1],thestr[start]))
			break;
	}

	int end=thestr.size()-1;
	for (;end>=0;end--) {
		if (!strchr(KIFRAWSTRINGS[1],thestr[end]))
			break;
	}
	int keep=end-start+1;

	string res=thestr.substr(start,keep);
	return callfunc->kifcode->Providestring(res);	
}

KifElement* PreTrimright(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreTrim");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();
	int start=0;
	int end=thestr.size()-1;
	for (;end>=0;end--) {
		if (!strchr(KIFRAWSTRINGS[1],thestr[end]))
			break;
	}
	int keep=end-start+1;
	string res=thestr.substr(start,keep);
	return callfunc->kifcode->Providestring(res);	
}

KifElement* PreTrimleft(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreTrim");
#endif

	//First parameter is a string
	KifElement* mystr=callfunc->Evaluate(0,dom);
	string thestr=mystr->String();
	int start=0;
	for (;start<thestr.size();start++) {
		if (!strchr(KIFRAWSTRINGS[1],thestr[start]))
			break;
	}

	int end=thestr.size()-1;
	int keep=end-start+1;

	string res=thestr.substr(start,keep);
	return callfunc->kifcode->Providestring(res);	
}

KifElement* PreType(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreType");
#endif
	KifElement* res=callfunc->Evaluate(0,dom);
	if (contextualpattern->type==kifInteger || contextualpattern->type==kifFloat) {
		int r=contextualpattern->type;
		return callfunc->kifcode->Provideinteger(r);
	}
	return callfunc->kifcode->Providestringraw(res->Type());
}

KifElement* PreRandom(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	static bool init=false;
	//we initialise our rand structure...
	if (init==false) {
		srand(time(NULL));
		init=true;
	}
	float i=rand();
	double f=i/RAND_MAX;
	return callfunc->kifcode->Providefloat(f);
}

KifElement* PreResetTime(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter is a time
	KifElement* timeinfo=callfunc->Evaluate(0,dom);
	if (timeinfo->type!=kifTime)
		return callfunc->kifcode->Returnerror(kifErrorStrings[219]);
	KifTime* ktime=(KifTime*)timeinfo;
	gettimeofday(&ktime->value, NULL);
	return ktime;
}

KifElement* PreSetDate(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreSetDate");
#endif


	//First parameter is a time
	KifElement* timeinfo=callfunc->Evaluate(0,dom);
	if (timeinfo->type!=kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[14]);

	KifDate* ktime=(KifDate*)timeinfo;
	struct tm* temps;
	time_t x=0;
	temps=localtime(&x);

	int fulldate=0;
	long res;
	//Year
	if (callfunc->Size()>=2) {
		res=callfunc->Evaluate(1,dom)->Integer();
		if (res>0) {
			temps->tm_year=res-1900;
			fulldate=100;
		}
	}

	//Month
	if (callfunc->Size()>=3) {
		res=callfunc->Evaluate(2,dom)->Integer();
		if (res>0) {
			fulldate+=10;
			temps->tm_mon=res-1;
		}
	}

	//Day
	if (callfunc->Size()>=4) {
		res=callfunc->Evaluate(3,dom)->Integer();
		if (res>0) {
			fulldate+=1;
			temps->tm_mday=res;		
		}
	}
	//Hour
	if (callfunc->Size()>=5) {
		res=callfunc->Evaluate(4,dom)->Integer();
		if (res>=0 && res<24) {
			//reference is 1AM
			if (fulldate==0)
				res+=2;
			else
				if (fulldate==1)
					res+=1;
			temps->tm_hour=res;
		}
	}

	//Minute
	if (callfunc->Size()>=6) {
		res=callfunc->Evaluate(5,dom)->Integer();
		if (res>=0 && res<60)
			temps->tm_min=res;
	}

	//Second
	if (callfunc->Size()>=7) {
		res=callfunc->Evaluate(6,dom)->Integer();
		if (res>=0 && res<60)
			temps->tm_sec=res;
	}

		
	x=mktime(temps);
	if (x<=0)
		return kifFALSE;
	ktime->value=x;
	return kifTRUE;
}

KifElement* PreDate(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreDate");
#endif

	//First parameter is a string
	KifElement* timeinfo=callfunc->Evaluate(0,dom);
	if (timeinfo->type!=kifDate)
		callfunc->kifcode->Returnerror(kifErrorStrings[16]);
	string d=timeinfo->String();
	return callfunc->kifcode->Providestring(d);
}


KifElement* PreYear(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreYear");
#endif

	//First parameter is a string
	KifElement* timeinfo=callfunc->Evaluate(0,dom);
	if (timeinfo->type!=kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[18]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value); 
	long vl=temps->tm_year+1900;
	return callfunc->kifcode->Provideinteger(vl);
}

KifElement* PreDay(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreDay");
#endif

	//First parameter is a string
	KifElement* timeinfo=callfunc->Evaluate(0,dom);
	if (timeinfo->type!=kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[20]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value); 
	long vl=temps->tm_mday;
	return callfunc->kifcode->Provideinteger(vl);
}

KifElement* PreHour(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreHour");
#endif

	//First parameter is a string
	KifElement* timeinfo=callfunc->Evaluate(0,dom);
	if (timeinfo->type!=kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[22]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value); 
	return callfunc->kifcode->Provideinteger(temps->tm_hour);
}

KifElement* PreMinute(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreMinute");
#endif

	//First parameter is a string
	KifElement* timeinfo=callfunc->Evaluate(0,dom);
	if (timeinfo->type!=kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[22]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value); 
	return callfunc->kifcode->Provideinteger(temps->tm_min);
}

KifElement* PreSecond(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreSecond");
#endif

	//First parameter is a string
	KifElement* timeinfo=callfunc->Evaluate(0,dom);
	if (timeinfo->type!=kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[22]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value); 
	return callfunc->kifcode->Provideinteger(temps->tm_sec);
}

KifElement* PreMonth(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreMonth");
#endif

	//First parameter is a string
	KifElement* timeinfo=callfunc->Evaluate(0,dom);
	if (timeinfo->type!=kifDate)
		return callfunc->kifcode->Returnerror(kifErrorStrings[24]);
	struct tm* temps = localtime(&((KifDate*)timeinfo)->value); 
	long vl=temps->tm_mon+1;
	return callfunc->kifcode->Provideinteger(vl);
}


KifElement* PreCatch(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreCatch");
#endif
	int idthread=kifGlobals->GetThreadid();
	if (Errorid(idthread)==false)
		return kifFALSE;
	if (callfunc->Size()!=1) {
		kifGlobals->Cleanerrorid(idthread);
		return kifTRUE;
	}
	string resstr=Errorstring();
	kifGlobals->Cleanerrorid(idthread);
	KifElement* res=callfunc->Evaluate(0,dom);
	if (res->Returnvalue()->type==kifString)
		res->Setvalue(callfunc->kifcode->Providestring(resstr));
	else {
		//We take the first three characters
		//we look for the first digit
		string resex;
		for (int i=0;i<resstr.size();i++) {
			while (i<resstr.size() && resstr[i]>='0' && resstr[i]<='9') {
				resex+=resstr[i];
				i++;
			}
			if (resex!="")
				break;
		}
		//Which we transform into a number
		res->Setvalue(callfunc->kifcode->Provideinteger(atol(STR(resex))));
	}
	return kifTRUE;
}

KifElement* PreRaise(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreRaise");
#endif
	if (Error())
		return kifFALSE;
	
	KifElement* res=callfunc->Evaluate(0,dom);
	string resstr=res->String();
	callfunc->kifcode->Returnerror(resstr);
	return res;
}

KifElement* PreTypeIs(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreTypeIs");
#endif
	KifElement* res=callfunc->Evaluate(0,dom);
	KifElement* typ=callfunc->Evaluate(1,dom);
	string resstr=res->Type();
	string typstr=typ->String();
	if (resstr==typstr)
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreMath(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreMath");
#endif
	double v=callfunc->Evaluate(0,dom)->Float();
	v=kifGlobals->kifMathFunctions[callfunc->Name()](v);
	return callfunc->kifcode->Providefloat(v);
}

KifElement* PreFill(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreOrd");
#endif
	KifElement* kvstr=callfunc->Evaluate(0,dom);
	if (kvstr->type!=kifString)
		return callfunc->kifcode->Returnerror(kifErrorStrings[210]);
	KifString* kstr=(KifString*)kvstr;
	KifElement* res=callfunc->Evaluate(1,dom);
	KifElement* kval=callfunc->Evaluate(2,dom);
	int sz=res->Integer();
	if (sz>(unsigned short)-1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[209]);
	char* s=(char*)malloc(sz+1);
	char c=' ';
	string sval=kval->String();
	if (sval!="")
		c=sval[0];
	memset(s,c,sz);
	s[sz]=0;
	kstr->value=s;
	free(s);
	return kval;
}	

KifElement* PreOrd(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreOrd");
#endif
	KifElement* res=callfunc->Evaluate(0,dom);
	string s=res->String();
	if (s.size()>=1) {
		long x;
		if (contextualpattern->type==kifVector) {
			vector<int> vect;
			c_char_index_code_all(s,vect);
			KifVector* kvect=Selectvector(callfunc,contextualpattern);
			for (int i=0;i<vect.size();i++)
				kvect->Push(callfunc->kifcode->Provideinteger(vect[i]));
			return kvect;
		}
		x=c_char_index_code(s,0);
		return callfunc->kifcode->Provideinteger(x);
	}
	return kifNULL;
}

KifElement* PreBytes(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreOrd");
#endif
	KifElement* res=callfunc->Evaluate(0,dom);
	int i;
	long v;
	if (res->type==kifVector) {
		//We transform the vector into a string
		KifVector* kvect=(KifVector*)res;
		char* buff=new char[kvect->values.size()+1];		
		for (i=0;i<kvect->values.size();i++) {
			v=kvect->values[i]->Integer();
			if (v<0 || v>255)
				buff[i]=32;
			else
				buff[i]=v;
		}
		buff[i]=0;
		res=callfunc->kifcode->Providestringraw(buff);
		delete[] buff;
		return res;
	}

	string s=res->String();
	if (s.size()>=1) {		
		if (contextualpattern->type==kifVector) {						
			KifVector* kvect=Selectvector(callfunc,contextualpattern);
			for (i=0;i<s.size();i++)
				kvect->Push(callfunc->kifcode->Provideinteger((uchar)s[i]));
			return kvect;
		}
		v=c_char_index_code(s,0);
		return callfunc->kifcode->Provideinteger(v);
	}
	return kifNULL;
}

KifElement* PreChr(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreChr");
#endif
	KifElement* res=callfunc->Evaluate(0,dom);
	KIFLONG x=res->Long();
    blongchar s;
    c_unicodetoutf8(x,s.ustr);
	return callfunc->kifcode->Providestringraw(s.str);
}


KifElement* PreKill(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreKill");
#endif

	ThreadLock _lock(kifSignal);
	vector<KifWaitState*>* kws;
	vector<KifWaitState*>::iterator it;
	if (callfunc->Size()==0) {
		hmap<string,vector<KifWaitState*> >::iterator itm;
		for (itm=kifWaitstates.begin();itm!=kifWaitstates.end();itm++) {
			kws=&itm->second;
			for (it=kws->begin();it!=kws->end();it++) {
				if ((*it)->loquet!=NULL) {
					(*it)->killed=true;
					(*it)->loquet->Released();
				}
			}
			kws->clear();			
		}
		kifWaitstates.clear();
		return kifTRUE;
	}

	if (callfunc->Size()!=1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[197]);

	KifElement* kflag=callfunc->Evaluate(0,dom);
	string flag=kflag->String();
	kws=Freestate(callfunc->kifcode,flag);
	if (kws==NULL)
		return kifNULL;
	for (it=kws->begin();it!=kws->end();it++) {
		if ((*it)->loquet!=NULL) {
			(*it)->killed=true;
			(*it)->loquet->Released();
		}
	}
	kws->clear();
	kifWaitstates.erase(flag);
	return kifTRUE;
}


KifElement* PreCast(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreCast");
#endif
	vector<KifWaitState*>* kws;
	vector<KifWaitState*>::iterator it;

	if (callfunc->Size()==0) {
        ThreadLock _lock(kifSignal);
		hmap<string,vector<KifWaitState*> >::iterator itm;
		//First we duplicate it...
		for (itm=kifWaitstates.begin();itm!=kifWaitstates.end();itm++) {
			kws=&itm->second;
			for (it=kws->begin();it!=kws->end();it++) {
				if ((*it)->loquet!=NULL)
					(*it)->loquet->Released();
			}
			kws->clear();
		}

		kifWaitstates.clear();
		return kifTRUE;
	}

	if (callfunc->Size()!=1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[198]);

	KifElement* kflag=callfunc->Evaluate(0,dom);
	string flag=kflag->String();
    
	ThreadLock _lock(kifSignal);
	kws=Freestate(callfunc->kifcode,flag);
	if (kws==NULL)
		return kifNULL;
	
	//We need to duplicate the vector beforehand, to take into account any specific modification
	//while applying the rest of the functions.
	for (it=kws->begin();it!=kws->end();it++) {
		(*it)->lastcast=flag;
		if ((*it)->loquet!=NULL)
			(*it)->loquet->Released();
	}
	kws->clear();
	kifWaitstates.erase(flag);
	return kifTRUE;
}

KifElement* PreWaitOnJoin(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	int idthread=kifGlobals->GetThreadid();
	{
		ThreadLock _lock(kifCallFunctionThread);
		if (kifGlobals->joinedloquets.find(idthread)==kifGlobals->joinedloquets.end())
			return kifFALSE;	
	}
	int i;

	vector<THREADPTR>* v;

	{
		ThreadLock _lock(kifCallFunctionThread);
		v=&kifGlobals->joinedloquets[idthread];	
	}


#ifdef WIN32
	for (i=0;i<v->size();i++) {
		if ((*v)[i]!=NULL) {
			WaitForSingleObject((*v)[i],INFINITE);
			CloseHandle((*v)[i]);
		}
	}
#else
	for (i=0;i<v->size();i++) {
		if ((*v)[i]!=NULL)
			pthread_join((*v)[i],NULL);			
	}
#endif

	ThreadLock _lock(kifCallFunctionThread);
	kifGlobals->joinedloquets.erase(idthread);
	return kifTRUE;
}

KifElement* PreWaitOnFalse(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {

	LockedThread* l=NULL;
	//first we get our variable back
	KifElement* var=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,true);
	int id=var->idgarbage;
	var=var->Returnvalue();
	//If we wait on a frame object, then the modification is on one of the variables of that object
	if (var->type==kifDomain) {
		KifDomain* vdom=(KifDomain*)var;
		hmap<short,KifElement*>::iterator it;
		for (it=vdom->declarations.begin();it!=vdom->declarations.end();it++) {
			if (it->second->isFunction())
				continue;
			l=kifGlobals->GetLoquet(it->second->idgarbage);
			if (l!=NULL)
				break;
		}
	}
	else
		l=kifGlobals->GetLoquet(id);
	
	if (l==NULL)
		return kifFALSE;

	//we wait until our variable changes its status	
	l->Blocked();
	return kifTRUE;
}

KifElement* PreLock(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//we create our lock
	KifThreadLock* ktl;
    {
        string lock=callfunc->Evaluate(0,dom)->String();
        ThreadLock _lock(kifLock);
        if (kifGlobals->locktables.find(lock)==kifGlobals->locktables.end()) {
            ktl=new KifThreadLock(NULL,false);
            kifGlobals->locktables[lock]=ktl;
        }
        else
            ktl=kifGlobals->locktables[lock];
    }
	ktl->Locking();
	return kifTRUE;
}

KifElement* PreUnlock(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifThreadLock* ktl;
    {
        string lock=callfunc->Evaluate(0,dom)->String();
        ThreadLock _lock(kifLock);
        if (kifGlobals->locktables.find(lock)==kifGlobals->locktables.end()) {
            string message=kifErrorStrings[240];
            message+=lock;
            return callfunc->kifcode->Returnerror(message);
        }
        ktl=kifGlobals->locktables[lock];
    }
	ktl->Unlocking();
	return kifTRUE;
}



KifElement* PreWait(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreWait");
#endif
	KifElement* kflag=callfunc->Evaluate(0,dom);
	string flag=kflag->String();
    
	if (flag!="") {			
        vector<string> flags;
        flags.push_back(flag);
        ThreadLock* lock=new ThreadLock(kifSignal);	
        LockedThread loquet;
		KifWaitState kws;
		kws.loquet=&loquet;
		kifWaitstates[flag].push_back(&kws);
		//we keep a track of all labels...
		//But only one semaphore is set...
		for (int i=1;i<callfunc->Size();i++) {
			flag=callfunc->Evaluate(i,dom)->String();
			flags.push_back(flag);
			kifWaitstates[flag].push_back(&kws);
		}
		delete lock;
        
		loquet.Blocked();
		lock=new ThreadLock(kifSignal);	
		if (kws.killed)				
			callfunc->Setreturnvalue(kifTRUE);
		//cleaning
		for (int i=0;i<flags.size();i++) {
			if (kifWaitstates.find(flags[i])!=kifWaitstates.end()) {
				vector<KifWaitState*>& kwsloops=kifWaitstates[flags[i]];
				for (int j=kwsloops.size()-1;j>=0;j--) {
					if (kwsloops[j]==&kws)
						kwsloops.erase(kwsloops.begin()+j);
				}
				if (kwsloops.empty())
					kifWaitstates.erase(flags[i]);
			}
		}
		flag=kws.lastcast;
        delete lock;
    }
    return callfunc->kifcode->Providestring(flag);
}

KifElement* PreWaiting(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreWait");
#endif
	ThreadLock _lock(kifSignal);
	KifVector* kvect=Selectvector(callfunc,contextualpattern);
	vector<KifWaitState*>::iterator it;
	hmap<string,vector<KifWaitState*> >::iterator itm;
	//First we duplicate it...
	for (itm=kifWaitstates.begin();itm!=kifWaitstates.end();itm++)
		kvect->Push(callfunc->kifcode->Providestringraw(itm->first));
	return kvect;
}

KifElement* PreVersion(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	return callfunc->kifcode->Providestringraw(Whoami_kif());
}

KifElement* PreFractionSimplify(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	return callfunc->Evaluate(0,dom)->FractionSimplify(false);
}

KifElement* PreEuclidian(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	return callfunc->Evaluate(0,dom)->Euclidian();
}

KifElement* PreNumeratorDenominator(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfract=callfunc->Evaluate(0,dom);
	KifElement* knum=callfunc->Evaluate(1,dom);
	KifElement* kden=callfunc->Evaluate(2,dom);
	KIFLONG den=kden->Long();
	if (den==0)
		return callfunc->kifcode->Returnerror(KIFRAWSTRINGS[5]);	
	kfract->ND(knum->Long(),den);
	return kfract;
}

KifElement* PreFractionSwitch(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {	
	KifElement* kf=callfunc->Evaluate(0,dom);

	if (kf->type!=kifFraction)
		kf=kf->Fraction();

	KIFLONG val=((KifFraction*)kf)->denominator;
	((KifFraction*)kf)->denominator=((KifFraction*)kf)->numerator;
	((KifFraction*)kf)->numerator=val;

	return kf;
}

KifElement* PrePartOf(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {	
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	KifElement* ksecond=callfunc->Evaluate(1,dom);

	if (kfirst->type==kifBits && ksecond->type==kifBits) {
		KifBits* k1=(KifBits*)kfirst;
		KifBits* k2=(KifBits*)ksecond;

		if (k2->bitmap.size()>k1->bitmap.size())
			return kifFALSE;

		map<short,unsigned short>::iterator it;

		for (it=k2->bitmap.begin();it!=k2->bitmap.end();it++) {
			if (k1->bitmap.find(it->first)==k1->bitmap.end())
				return kifFALSE;
			if ((it->second&k1->bitmap[it->first])!=it->second)
				return kifFALSE;
		}

		return kifTRUE;
	}

	if (kfirst->type==kifBit && ksecond->type==kifBit) {
		KifBit* k1=(KifBit*)kfirst;
		KifBit* k2=(KifBit*)ksecond;

		if (k2->bitvector.size()>k1->bitvector.size())
			return kifFALSE;

		int i;

		for (i=0;i<k2->bitvector.size();i++) {
			if ((k2->bitvector[i]&k1->bitvector[i])!=k2->bitvector[i])
				return kifFALSE;
		}

		return kifTRUE;
	}

	return callfunc->kifcode->Returnerror(KIFRAWSTRINGS[6]);
}


KifElement* PreDenominator(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kf=callfunc->Evaluate(0,dom);
	if (callfunc->Size()==1)
		return new KifLong(callfunc->kifcode,NULL,kf->D());
	
	KifElement* kval=callfunc->Evaluate(1,dom);
	KIFLONG val=kval->Long();
	if (val==0)
		return callfunc->kifcode->Returnerror(KIFRAWSTRINGS[5]);

	((KifFraction*)kf)->denominator=val;
	((KifFraction*)kf)->Simplify();
	return kval;
}

KifElement* PreNumerator(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	if (callfunc->Size()==1) {
		KifElement* kf=callfunc->Evaluate(0,dom);
		return new KifLong(callfunc->kifcode,NULL,kf->N());
	}
	KifElement* kf=callfunc->Evaluate(0,dom);
	KifElement* kval=callfunc->Evaluate(1,dom);
	((KifFraction*)kf)->numerator=kval->Long();
	((KifFraction*)kf)->Simplify();
	return kval;
}

KifElement* PreGetSTD(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//Reading
    KifElement* kstr=callfunc->Evaluate(0,dom);
    bool catchoutput=callfunc->Evaluate(1,dom)->Boolean();
    if (catchoutput) {
        if (kifGlobals->kstd!=NULL)
            return callfunc->kifcode->Returnerror(KIFRAWSTRINGS[8]);
        kifGlobals->kstd=kstr;
        return kifTRUE;
    }
    else {
        if (kifGlobals->kstd==kstr)
            kifGlobals->kstd=NULL;
    }
    return kifTRUE;
}

KifElement* PreGetERR(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//Reading
    KifElement* kstr=callfunc->Evaluate(0,dom);
    bool catchoutput=callfunc->Evaluate(1,dom)->Boolean();
    if (catchoutput) {
        if (kifGlobals->kstd!=NULL)
            return callfunc->kifcode->Returnerror(KIFRAWSTRINGS[9]);
        kifGlobals->kerr=kstr;
        kstr->Setreference();
        return kifTRUE;
    }
    else {
        if (kifGlobals->kerr==kstr) {
            kifGlobals->kerr->Resetreference();
            kifGlobals->kerr=NULL;
        }
    }
    return kifTRUE;
}


KifElement* PrePrint(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* res=kifNULL;
	ostream& os=*callfunc->kifcode->os;
    KifString* kval=(KifString*)kifGlobals->kstd;
    int idthread=kifGlobals->GetThreadid();

	for (size_t i=0;i<callfunc->Size();i++) {
		res=callfunc->EvaluateString(i,dom);
		if (Errorid(idthread))
			return Errorptr();
        if (kval==NULL)
            os<<res->String();
        else
            kval->value+=res->String();		
	}
	return res;
}


KifElement* PrePrintLN(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {    
	KifElement* res=kifNULL;
	ostream& os=*callfunc->kifcode->os;
    KifString* kval=(KifString*)kifGlobals->kstd;
    int idthread=kifGlobals->GetThreadid();

	for (size_t i=0;i<callfunc->Size();i++) {
		if (i) {
            if (kval==NULL)
                os<<" ";
            else
                kval->value+=" ";
            
        }
		res=callfunc->EvaluateString(i,dom);
		if (Errorid(idthread))
			return Errorptr();
        if (kval==NULL)
            os<<res->String();
        else
            kval->value+=res->String();
	}
    if (kval==NULL)
        os<<Endl;    
    else
        kval->value+=Endl;
	return res;
}

KifElement* PrePrintFlush(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PrePrintLN");
#endif
	ostream* os=callfunc->kifcode->os;
	os->flush();
	return kifTRUE;
}

KifElement* PrePrinterr(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* res=kifNULL;
    KifString* kval=(KifString*)kifGlobals->kerr;
    int idthread=kifGlobals->GetThreadid();
	for (size_t i=0;i<callfunc->Size();i++) {
		res=callfunc->EvaluateString(i,dom);
		if (Errorid(idthread))
			return Errorptr();
        if (kval==NULL)
            cerr<<res->String();
        else
            kval->value+=res->String();		
	}
	return res;
}

KifElement* PrePrinterrLN(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* res=kifNULL;
    KifString* kval=(KifString*)kifGlobals->kerr;
    int idthread=kifGlobals->GetThreadid();

	for (size_t i=0;i<callfunc->Size();i++) {
		if (i) {
            if (kval==NULL)
                cerr<<" ";
            else
                kval->value+=" ";
            
        }
		res=callfunc->EvaluateString(i,dom);
		if (Errorid(idthread))
			return Errorptr();
        if (kval==NULL)
            cerr<<res->String();
        else
            kval->value+=res->String();		
	}
    if (kval==NULL)
        cerr<<Endl;
    else
        kval->value+=Endl;
	return res;
}

KifElement* PreSeek(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreSeek");
#endif

	KifElement* kf=callfunc->Evaluate(0,dom);
	KifElement* ki=callfunc->Evaluate(1,dom);
	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[162]);
	KifFile* kfile=(KifFile*)kf;
	return kfile->Seek(ki->Integer());
}

KifElement* PreTell(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreTell");
#endif

	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[164]);
	KifFile* kfile=(KifFile*)kf;
	return kfile->Tell();
}


KifElement* PreWrite(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreWrite");
#endif

	KifElement* kf=callfunc->Evaluate(0,dom);

	KifElement* res=kifNULL;
	if (kf->type==kifWeka) {
		//First parameter should be the filename
		res=callfunc->Evaluate(1,dom);
		string mname=res->String();
		string thetype;
		if (callfunc->Size()>=3) {
			res=callfunc->Evaluate(2,dom);
			thetype=res->String();
		}
		kf->Save(mname,thetype);
		return kifTRUE;
	}

	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[27]);

	KifFile* kfile=(KifFile*)kf;

	for (size_t i=1;i<callfunc->Size();i++) {
		res=callfunc->parameters[i]->ExecuteRaw(callfunc->kifcode->kifTypeSTRING,dom);
		kfile->Write(res->String());
	}
	return kifTRUE;
}

KifElement* PreWriteBin(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreWrite");
#endif

	KifElement* kf=callfunc->Evaluate(0,dom);

	KifFile* kfile=(KifFile*)kf;
	int res; 
	for (size_t i=1;i<callfunc->Size();i++) {
		res=callfunc->Evaluate(i,dom)->Integer();
		kfile->WriteBin(res);
	}
	return kifTRUE;
}

KifElement* PreRead(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreRead");
#endif

	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[28]);
	KifFile* kfile=(KifFile*)kf;
	int nb=-1;
	if (callfunc->Size()==2)
		nb=callfunc->Evaluate(1,dom)->Integer();
	KifElement* res=kfile->Read(nb);
	return res;
}

KifElement* PreReadOneLine(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreReadOneLine");
#endif

	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[29]);

	KifFile* kfile=(KifFile*)kf;

	KifElement* res=kfile->Readoneline();
	return res;
}

//close(file)
KifElement* PreClose(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreClose");
#endif
	//First parameter should be a file
	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[30]);

	KifFile* kfile=(KifFile*)kf;
	kfile->Close();
	return kifTRUE;
}

//openread(file,name)
KifElement* PreOpenReadfile(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreOpenReadfile");
#endif
	//First parameter should be a file
	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[31]);

	//second parameter is a string
	KifElement* name=callfunc->Evaluate(1,dom);
	KifFile* kfile=(KifFile*)kf;
	if (!kfile->Openread(name->String())) {
		string mess=kifErrorStrings[32]+name->String();
		return callfunc->kifcode->Returnerror(mess);
	}
	return kifTRUE;
}

//openread(file,name)
KifElement* PreEndOfFile(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreEndOfFile");
#endif
	//First parameter should be a file
	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[33]);
	KifFile* kfile=(KifFile*)kf;
	return kfile->EndOfFile();
}

//openappend(file,name)
KifElement* PreOpenAppendfile(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreOpenWritefile");
#endif
	//First parameter should be a file
	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[34]);
	//second parameter is a string
	KifElement* name=callfunc->Evaluate(1,dom);
	KifFile* kfile=(KifFile*)kf;
	if (!kfile->Openappend(name->String())) {
		string mess=kifErrorStrings[35]+name->String();
		return callfunc->kifcode->Returnerror(mess);
	}
	return kifTRUE;
}


//openwrite(file,name)
KifElement* PreOpenWritefile(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreOpenWritefile");
#endif
	//First parameter should be a file
	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[34]);
	//second parameter is a string
	KifElement* name=callfunc->Evaluate(1,dom);
	KifFile* kfile=(KifFile*)kf;
	if (!kfile->Openwrite(name->String())) {
		string mess=kifErrorStrings[35]+name->String();
		return callfunc->kifcode->Returnerror(mess);
	}
	return kifTRUE;
}

KifElement* PreFlushFile(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreFlushFile");
#endif
	//First parameter should be a file
	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->type!=kifFile)
		return callfunc->kifcode->Returnerror(kifErrorStrings[34]);
	KifFile* kfile=(KifFile*)kf;
	if (kfile->mode==2) {
		kfile->thefile->flush();
		return kifTRUE;
	}
	return kifFALSE;
}

KifElement* PreInsert(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	size_t idx;
	//First parameter should be a vector
	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->type==kifVector) {
		//second parameter is anything
		KifElement* kval=callfunc->Evaluate(2,dom);
		idx=callfunc->Evaluate(1,dom)->Integer();
		if (idx>=((KifVector*)kf)->values.size())
			((KifVector*)kf)->Push(kval);
		else
			((KifVector*)kf)->Insert(idx,kval);
		return kifTRUE;
	}

	if (kf->type==kifList) {
		//second parameter is anything
		KifElement* kval=callfunc->Evaluate(2,dom);
		idx=callfunc->Evaluate(1,dom)->Integer();
		if (idx>=((KifList*)kf)->values.size())
			((KifList*)kf)->Push(kval);
		else
			((KifList*)kf)->Insert(idx,kval);
		return kifTRUE;
	}

	if (kf->isString()) {
		//second parameter is anything
		idx=callfunc->Evaluate(1,dom)->Integer();
		string val=callfunc->Evaluate(2,dom)->String();
		string ret=kf->String();
		if (c_char_index_insert(ret,val,idx)==false)
			return kifNULL;
		return callfunc->kifcode->Providestring(ret);
	}
		
	return callfunc->kifcode->Returnerror(kifErrorStrings[37]);
}

KifElement* PreJoin(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreJoin");
#endif

	KifElement* kf=callfunc->Evaluate(0,dom);
	if (kf->aVector()) {
		if (callfunc->Size()!=2)
			return callfunc->kifcode->Returnerror(kifErrorStrings[165]);
		//The separator
		KifElement* kstrsep=callfunc->Evaluate(1,dom);
		string sep=kstrsep->String();
		string res;
		int it;
		bool beg=true;
		for (it=0;it<kf->Size();it++) {
			if (beg==false)
				res+=sep;
			beg=false;
			res+=kf->ValueOnIndex(it)->String();
		}
		return callfunc->kifcode->Providestring(res);
	}

	if (kf->type==kifList) {
		if (callfunc->Size()!=2)
			return callfunc->kifcode->Returnerror(kifErrorStrings[165]);
		KifList* kvect=(KifList*)kf;
		//The separator
		KifElement* kstrsep=callfunc->Evaluate(1,dom);
		string sep=kstrsep->String();
		string res;
		list<KifElement*>::iterator it;
		bool beg=true;
		for (it=kvect->values.begin();it!=kvect->values.end();it++) {
			if (beg==false)
				res+=sep;
			beg=false;
			res+=(*it)->String();			
		}
		return callfunc->kifcode->Providestring(res);
	}
	if (kf->type==kifMap) {
		if (callfunc->Size()!=3)
			return callfunc->kifcode->Returnerror(kifErrorStrings[165]);
		KifMap* kvect=(KifMap*)kf;
		//The separator between keys
		string keysep=callfunc->Evaluate(1,dom)->String();
		//The separator between values
		string sep=callfunc->Evaluate(2,dom)->String();
		string res;
		hmap<string,KifElement*>::iterator it;
		bool beg=true;
		for (it=kvect->values.begin();it!=kvect->values.end();it++) {
			if (beg==false)
				res+=sep;
			beg=false;
			res+=it->first;
			res+=keysep;
			res+=it->second->String();
		}
		return callfunc->kifcode->Providestring(res);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[166]);
}

bool KifCompareInteger(KifElement* e1,KifElement* e2) {
	if (e1->Integer()<e2->Integer())
		return true;
	return false;
}

bool KifCompareLong(KifElement* e1,KifElement* e2) {
	if (e1->Long()<e2->Long())
		return true;
	return false;
}

bool KifCompareFloat(KifElement* e1,KifElement* e2) {
	if (e1->Float()<e2->Float())
		return true;
	return false;
}

bool KifCompareString(KifElement* e1,KifElement* e2) {
	if (e1->String()<e2->String())
		return true;
	return false;
}


bool KifInvCompareInteger(KifElement* e1,KifElement* e2) {
	if (e1->Integer()>e2->Integer())
		return true;
	return false;
}

bool KifInvCompareLong(KifElement* e1,KifElement* e2) {
	if (e1->Long()>e2->Long())
		return true;
	return false;
}

bool KifInvCompareFloat(KifElement* e1,KifElement* e2) {
	if (e1->Float()>e2->Float())
		return true;
	return false;
}

bool KifInvCompareString(KifElement* e1,KifElement* e2) {
	if (e1->String()>e2->String())
		return true;
	return false;
}


KifElement* PreSortInt(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is an integer (the number of pop)
	if (kf->type!=kifVector)
		kf=kf->Vector(dom);

	KifVector* kvect=(KifVector*)kf;
	if (kvect->values.size()<=1)
		return kvect;
	KifElement* order=callfunc->Evaluate(1,dom);
	if (order->Boolean()==true)
		sort(kvect->values.begin(),kvect->values.end(),KifCompareInteger);
	else
		sort(kvect->values.begin(),kvect->values.end(),KifInvCompareInteger);
	return kvect;
}

KifElement* PreSortFloat(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is an integer (the number of pop)
	if (kf->type!=kifVector)
		kf=kf->Vector(dom);

	KifVector* kvect=(KifVector*)kf;
	if (kvect->values.size()<=1)
		return kvect;
	KifElement* order=callfunc->Evaluate(1,dom);
	if (order->Boolean()==true)
		sort(kvect->values.begin(),kvect->values.end(),KifCompareFloat);
	else
		sort(kvect->values.begin(),kvect->values.end(),KifInvCompareFloat);
	return kvect;
}

KifElement* PreSortLong(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is an integer (the number of pop)
	if (kf->type!=kifVector)
		kf=kf->Vector(dom);

	KifVector* kvect=(KifVector*)kf;
	if (kvect->values.size()<=1)
		return kvect;
	KifElement* order=callfunc->Evaluate(1,dom);
	if (order->Boolean()==true)
		sort(kvect->values.begin(),kvect->values.end(),KifCompareLong);
	else
		sort(kvect->values.begin(),kvect->values.end(),KifInvCompareLong);
	return kvect;
}

KifElement* PreSortString(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is an integer (the number of pop)
	if (kf->type!=kifVector)
		kf=kf->Vector(dom);

	KifVector* kvect=(KifVector*)kf;
	if (kvect->values.size()<=1)
		return kvect;
	KifElement* order=callfunc->Evaluate(1,dom);
	if (order->Boolean()==true)
		sort(kvect->values.begin(),kvect->values.end(),KifCompareString);
	else
		sort(kvect->values.begin(),kvect->values.end(),KifInvCompareString);
	return kvect;
}

KifElement* PreSort(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreSort");
#endif

	//First parameter should be a vector
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is an integer (the number of pop)
	if (kf->type!=kifVector)
		kf=kf->Vector(dom);

	KifVector* kvect=(KifVector*)kf;
	if (kvect->values.size()<=1)
		return kvect;
	//Second parameter should be function
	if (callfunc->Size()==2) {
		kf=callfunc->Evaluate(1,dom)->Functioncall(dom);
		if (kf!=NULL && kf->isFunction()) {
			KifComparison kfcomp(dom,(KifFunction*)kf);
			sort(kvect->values.begin(),kvect->values.end(),kfcomp);
			return kvect;
		}
	}
	else {
		//we use the first element as a trigger
		KifElement* kfirst=kvect->values[0];
		switch (kfirst->type) {
		case kifInteger:
			sort(kvect->values.begin(),kvect->values.end(),KifCompareInteger);
			break;
		case kifLong:
			sort(kvect->values.begin(),kvect->values.end(),KifCompareLong);
			break;
		case kifFraction:
		case kifFloat:
			sort(kvect->values.begin(),kvect->values.end(),KifCompareFloat);
			break;
		default: //else we only use strings for other types
			sort(kvect->values.begin(),kvect->values.end(),KifCompareString);
		}
		return kvect;
	}
	return kvect;
}

KifElement* PreSum(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreSum");
#endif

	//First parameter should be a vector
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is an integer (the number of pop)
	double v=kf->Sum();
	return callfunc->kifcode->Providefloat(v);
}

KifElement* PreValues(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter should be a map
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is an integer (the number of pop)
	if (kf->type==kifMap) {
		KifMap* kmap=(KifMap*)kf;
		KifVector* kvect=Selectvector(callfunc,contextualpattern);
		hmap<string,KifElement*>::iterator it;
		for (it=kmap->values.begin();it!=kmap->values.end();it++)
			kvect->Push(it->second);
		return kvect;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[187]);
}


KifElement* PreKeys(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter should be a map
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is an integer (the number of pop)
	KifBaseString localstring(NULL,NULL);
	localstring.kifcode=callfunc->kifcode;
	if (kf->type==kifMap) {
		KifMap* kmap=(KifMap*)kf;
		KifVector* kvect=Selectvector(callfunc,contextualpattern);
		hmap<string,KifElement*>::iterator it;
		for (it=kmap->values.begin();it!=kmap->values.end();it++) {
			localstring.value=it->first;
			kvect->Push(&localstring);
		}
		return kvect;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[193]);
}

KifElement* PreProduct(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreProduct");
#endif

	//First parameter should be a vector
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is an integer (the number of pop)
	double v=kf->Product();
	return callfunc->kifcode->Providefloat(v);
}



KifElement* PreApply(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter should be a XIP rule
	KifElement* kfuncelement=callfunc->Evaluate(0,dom);

	if (kfuncelement->type==kifCall || kfuncelement->isFunction())
		return kfuncelement->Execute(contextualpattern,callfunc,kfuncelement);

	KifElement* kf;
	if (kfuncelement->type==kifIterator) {
		kf=kfuncelement->IteratorValue();
		return kf->Execute(contextualpattern,callfunc,kf);
	}

	KifElement* localfunction=NULL;
	KifElement* localparameter=callfunc->parameters[1];
	if (callfunc->Size()>=2) {
		localfunction=callfunc->Evaluate(1,dom);
		//then it is a simple parameter
		if (localfunction->type!=kifCall && !localfunction->isFunction())
			localfunction=NULL;
	}
	
	if (kfuncelement->aVector()) {
		KifVector* kvect=(KifVector*)kfuncelement;
		for (int itx=0;itx<kvect->Size();itx++) {
			kf=kvect->values[itx];
			if ((kf->type==kifCall || kf->isFunction()) || localfunction==NULL)
				kf->Execute(contextualpattern,callfunc,kf);
			else {
				//We expect a function with one single argument
				callfunc->parameters[1]=kf;
				localfunction->Execute(contextualpattern,callfunc,localfunction);
			}
			
			if (Error())
				return Errorptr();
		}
		callfunc->parameters[1]=localparameter;
		return kifTRUE;
	}


	if (kfuncelement->type==kifList) {
		KifList* kvect=(KifList*)kfuncelement;
		list<KifElement*>::iterator itx;
		for (itx=kvect->values.begin();itx!=kvect->values.end();itx++) {
			kf=*itx;
			if ((kf->type==kifCall || kf->isFunction()) || localfunction==NULL)
				kf->Execute(contextualpattern,callfunc,kf);
			else {
				//We expect a function with one single argument
				callfunc->parameters[1]=kf;
				localfunction->Execute(contextualpattern,callfunc,localfunction);
			}
			
			if (Error())
				return Errorptr();
		}
		callfunc->parameters[1]=localparameter;
		return kifTRUE;
	}

	if (kfuncelement->type==kifMap) {
		KifMap* kvect=(KifMap*)kfuncelement;
		hmap<string,KifElement*>::iterator itx;
		for (itx=kvect->values.begin();itx!=kvect->values.end();itx++) {
			kf=itx->second;
			if ((kf->type==kifCall || kf->isFunction()) || localfunction==NULL)
				kf->Execute(contextualpattern,callfunc,kf);
			else {
				//We expect a function with one single argument
				callfunc->parameters[1]=kf;
				localfunction->Execute(contextualpattern,callfunc,localfunction);
			}
			
			if (Error())
				return Errorptr();
		}
		callfunc->parameters[1]=localparameter;
		return kifTRUE;
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[49]);
}

KifElement* PreClear(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreClear");
#endif

	//First parameter should be a vector
	KifElement* kf=callfunc->Evaluate(0,dom);
	kf->Clear();
	return kifTRUE;
}

KifElement* PreTreeRemove(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	return kfirst->TreeRemove();
}

KifElement* PreTreeIsolate(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
		return kfirst->TreeExtract(NULL);
}

KifElement* PreExtract(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	KifElement* pos=callfunc->Evaluate(1,dom);
	KifElement* from=callfunc->Evaluate(2,dom);
	vector<string> v;
	int i;
	for (i=3;i<callfunc->Size();i++)
		v.push_back(callfunc->Evaluate(i,dom)->String());
	string init=kfirst->String();
	i=pos->Integer();
	string sfrom=from->String();
	i=init.find(sfrom,i);
	int lgfrom=sfrom.size();
	KifVector* vect=Selectvector(callfunc,contextualpattern);
	while (i!=-1) {
		int npos=-1;
		for (int j=0;j<v.size();j++) {
			int spos=init.find(v[j],i+lgfrom);
			if (spos!=-1 && (npos==-1 || spos<npos))
				npos=spos;
		}
		if (npos==-1)
			break;
		int nb=npos-i-lgfrom;
		string sub=init.substr(i+lgfrom,nb);
		vect->Push(callfunc->kifcode->Providestring(sub));
		i=init.find(sfrom,npos+1);
	}
	return vect;
}
KifElement* PreTreeNext(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	KifElement* kval=kfirst->TreeNext();
	if (callfunc->Size()==1)
		return kval;
	else {
		KifElement* kelement=callfunc->Evaluate(1,dom);
		if (kval==kelement)
			return kifTRUE;
	}
	return kifFALSE;
}


KifElement* PreTreePrevious(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	KifElement* kval=kfirst->TreePrevious();
	if (callfunc->Size()==1)
		return kval;
	else {
		KifElement* kelement=callfunc->Evaluate(1,dom);
		if (kval==kelement)
			return kifTRUE;
	}
	return kifFALSE;
}
KifElement* PreTreeParent(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	KifElement* kval=kfirst->TreeParent();
	if (callfunc->Size()==1)
		return kval;
	else {
		KifElement* kelement=callfunc->Evaluate(1,dom);
		if (kval==kelement)
			return kifTRUE;
	}
	return kifFALSE;
}
KifElement* PreTreeChild(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	if (callfunc->Size()==1)
		return kfirst->TreeChild();
	else {
		KifElement* kelement=callfunc->Evaluate(1,dom);
		if (kfirst==kelement->TreeParent())
			return kifTRUE;
	}
	return kifFALSE;
}

KifElement* PreTreeLast(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	KifElement* kval=kfirst->TreeLast();
	if (callfunc->Size()==1)
		return kval;
	else {
		KifElement* kelement=callfunc->Evaluate(1,dom);
		if (kval==kelement)
			return kifTRUE;
	}
	return kifFALSE;
}

KifElement* PreTreeDepth(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kroot=callfunc->Evaluate(0,dom);
	return callfunc->kifcode->Provideinteger( kroot->Depth());
}

KifElement* PreTreeAddChild(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kroot=callfunc->Evaluate(0,dom);
	KifElement* kelement=callfunc->Evaluate(1,dom);
	if (kelement->type!=kifTree)
		return callfunc->kifcode->Returnerror(kifErrorStrings[201]);
	return kroot->TreeAddchild((KifTree*)kelement);
}

KifElement* PreTreeAddNext(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kroot=callfunc->Evaluate(0,dom);
	KifElement* kelement=callfunc->Evaluate(1,dom);
	if (kelement->type!=kifTree)
		return callfunc->kifcode->Returnerror(kifErrorStrings[201]);
	return kroot->TreeAddnext((KifTree*)kelement);
}

KifElement* PreTreeAddPrevious(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kroot=callfunc->Evaluate(0,dom);
	KifElement* kelement=callfunc->Evaluate(1,dom);
	if (kelement->type!=kifTree)
		return callfunc->kifcode->Returnerror(kifErrorStrings[201]);
	return kroot->TreeAddprevious((KifTree*)kelement);
}

KifElement* PreSize(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreSize");
#endif
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	long sz=kfirst->Size();
	KifElement* ksz=callfunc->kifcode->Provideinteger(sz);
	return ksz;
}

KifElement* PreReverse(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreReverse");
#endif
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	if (kfirst->aVector()) {
		KifVector* kvectres=callfunc->kifcode->Providevector();
		for (int it=kfirst->Size()-1;it>=0;it--) {
			kvectres->Push(kfirst->ValueOnIndex(it));
		}
		return kvectres;
	}

	if (kfirst->type==kifList) {
		KifList* kvect=(KifList*)kfirst;
		list<KifElement*>::reverse_iterator it;
		KifList* kvectres=new KifList(callfunc->kifcode,NULL);
		for (it=kvect->values.rbegin();it!=kvect->values.rend();it++) {
			kvectres->Push(*it);
		}
		return kvectres;
	}
	string s=kfirst->String();
	int i;
	s=c_char_reverse(s,i);
	return callfunc->kifcode->Providestring(s);
}


KifElement* PreRemovefirst(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreRemovefirst");
#endif

	KifElement* kstr=callfunc->Evaluate(0,dom);
	KifElement* knb=callfunc->Evaluate(1,dom);
	string s=kstr->String();
	int id=knb->Integer();
	//On retire en tete
	int iv=s.length();

	if (iv>0 && id<=iv) {
		s=s.c_str()+id;
		return callfunc->kifcode->Providestring(s);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[85]);
}

KifElement* PreRemovelast(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreRemovelast");
#endif

	KifElement* kstr=callfunc->Evaluate(0,dom);
	KifElement* knb=callfunc->Evaluate(1,dom);
	string s=kstr->String();
	int id=knb->Integer();
	//On retire en tete
	int iv=s.length();

	if (iv>0 && id<=iv) {
		s=s.substr(0,iv-id);
		return callfunc->kifcode->Providestring(s);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[126]);
}

KifElement* PreNbInstances(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreNbInstances");
#endif
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	if (kfirst->type==kifWeka) {
		long i=((KifWeka*)kfirst)->values.size();
		return callfunc->kifcode->Provideinteger(i);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[87]);
}


KifElement* PreUnGet(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreUnGet");
#endif
	
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	int nb=1;
	if (callfunc->Size()==2) {
		KifElement* knb=callfunc->Evaluate(1,dom);
		nb=knb->Integer();
	}


	if (kfirst->type==kifFile) {
		KifFile* kfile=(KifFile*)kfirst;
		if (kfile->mode==1) {
			while (nb>0) {
				kfile->thefile->unget();
				nb--;
			}
			kfile->thefile->clear();
			return kifTRUE;
		}
		return kifFALSE;
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[174]);
}

#ifdef WIN32
static bool errorsprintf=false;
void wrongSprintf(const wchar_t* expression,
   const wchar_t* function, 
   const wchar_t* file, 
   unsigned int line, 
   uintptr_t pReserved) {
	   errorsprintf=true;
}
#endif

KifElement* PreFormat(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreFormat");
#endif

	KifElement* knum=callfunc->Evaluate(0,dom);
	KifElement* kformat=callfunc->Evaluate(1,dom);
	string sformat=kformat->String();
	if (knum->type==kifDate) {
		KifDate* ktime=(KifDate*)knum;
		struct tm* temps = localtime(&ktime->value); 		
		strftime(kifGlobals->buffer,100,STR(sformat),temps);
		return callfunc->kifcode->Providestringraw(kifGlobals->buffer);
	}

#ifdef WIN32
   _set_invalid_parameter_handler(wrongSprintf);

	errorsprintf=false;
	if (knum->type==kifInteger)
		sprintf_s(kifGlobals->buffer,1024,STR(sformat),knum->Integer());
	else 
		sprintf_s(kifGlobals->buffer,1024,STR(sformat),knum->Float());
	if (errorsprintf==true)
		return callfunc->kifcode->Returnerror(KIFRAWSTRINGS[10]);
#else
	if (knum->type==kifInteger) {
		if (sprintf_s(kifGlobals->buffer,1024,STR(sformat),knum->Integer())<0)
			return callfunc->kifcode->Returnerror(KIFRAWSTRINGS[10]);
	}
	else 
		if (sprintf_s(kifGlobals->buffer,1024,STR(sformat),knum->Float())<0)
			return callfunc->kifcode->Returnerror(KIFRAWSTRINGS[10]);
#endif	
	return callfunc->kifcode->Providestringraw(kifGlobals->buffer);
}


KifElement* PreEnvironment(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreEnvironment");
#endif
	KifCallFunction* calltop=(KifCallFunction*)kifGlobals->Top();
	if (calltop==NULL)
		return kifNULL;
	//This is a case, where we are looking for the type of the contextualpattern
	if (contextualpattern->type==kifInteger || contextualpattern->type==kifFloat) {
		int r=contextualpattern->type;
		return callfunc->kifcode->Provideinteger(r);
	}

	string resstr=calltop->environment->Type();
	return callfunc->kifcode->Providestring(resstr);
}

KifElement* PostEval(KifCode* kifcode,string& code,KifElement* contextualpattern,KifDomain* dom) {
	
	bool pushed=kifGlobals->Pushlocal(kifcode->kstart);
	KifInstructionSEQUENCE ki(NULL,NULL,idsequence);
	ki.kifcode=kifcode;
	//We deactivate the garbage temporary (to avoid problems)
	
	kifGlobals->nogarbage=true;
	kifcode->Evaluate(&ki,code,false);
	if (pushed)
		kifGlobals->PopStack();
	KifElement* ret=ki.Execute(contextualpattern,dom);
	//we reactivate it...
	kifGlobals->nogarbage=false;
	return ret;
}

KifElement* PreEval(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	string code=kfirst->String();
	return PostEval(callfunc->kifcode,code,contextualpattern,dom);
}

KifElement* PreOpenSession(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifCode* kifcode=new KifCode(kifGlobals->trigger,callfunc->kifcode->data,callfunc->kifcode->os);
    KifElement* kkif=callfunc->Evaluate(0,dom);
	if ( ((KifKiF*)kkif)->id!=-1)
		return callfunc->kifcode->Returnerror(kifErrorStrings[221]);
    ((KifKiF*)kkif)->id=kifcode->idkifmain;
	kifcode->session=true;
#ifdef FLTKCODE	
	KifExternalModule LibInitEntryPoint;
	LibInitEntryPoint=kifGlobals->kifLibraries[KIFRAWSTRINGS[13]];
	(*LibInitEntryPoint)(kifcode,KIFVERSION);
#endif
    return kifTRUE;
}

KifElement* PreCloseSession(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
    KifElement* kkif=callfunc->Evaluate(0,dom);
    int i=((KifKiF*)kkif)->id;
    
	KifCode* kifcode=kifGlobals->kifCodeVector[i];
	if (kifcode==NULL || kifcode==callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);
		
	((KifKiF*)kkif)->id=-1;
	delete kifcode;

	return kifTRUE;
}

KifElement* PreCompile(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	
    KifElement* kkif=callfunc->Evaluate(0,dom);
    int i=((KifKiF*)kkif)->id;

	KifCode* kifcode=kifGlobals->kifCodeVector[i];
	if (kifcode==NULL || kifcode==callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);
	
	int firstinstruction=kifcode->kstart->instructions.size();
	int igarbage=kifGlobals->garbage.size();
	int ilocal=kifcode->locals.size();
	int cursor=kifGlobals->cursor;
	KifElement* kfirst=callfunc->Evaluate(1,dom);
	string code=kfirst->String();
    KifElement* kname=kifNULL;
    if (callfunc->Size()==3)
        kname=callfunc->Evaluate(2,dom);
        
	hmap<short,KifElement*> stack;
	hmap<short,KifElement*>::iterator it;
	for (it=kifcode->kstart->declarations.begin();it!=kifcode->kstart->declarations.end();it++)
		stack[it->first]=it->second;

    vector<KifDomain*> keepstack;
	int id=kifGlobals->GetThreadid();
	if (id==-1) {
        for (i=0;i<kifGlobals->stacking.size();i++)
            keepstack.push_back(kifGlobals->stacking[i]);
        kifGlobals->stacking.clear();
    }
	else  {
        for (i=0;i<kifGlobals->threadstacks[id].size();i++)
            keepstack.push_back(kifGlobals->threadstacks[id][i]);
        kifGlobals->threadstacks[id].clear();
    }

 	kifcode->kstart->adding=true;
	kifcode->addtolocal=true;
	kifGlobals->PushStack(kifcode->kstart);
	//We evaluate in kstart...
	kifcode->Evaluate(kifcode->kstart,code,false);
    kifGlobals->PopStack();
	kifcode->addtolocal=false;
	if (id==-1) {
        for (i=0;i<keepstack.size();i++)
            kifGlobals->stacking.push_back(keepstack[i]);
    }
	else  {
        for (i=0;i<keepstack.size();i++)
            kifGlobals->threadstacks[id].push_back(keepstack[i]);
    }
    int j;
	//then from igarbage up to the end, we reference our elements
	if (Errorid(id)==true) {
		string error=Errorstring();
		kifGlobals->Cleanerrorid(id,false);
		for (i=firstinstruction;i<kifcode->kstart->instructions.size();i++)
			kifcode->kstart->instructions.pop_back();
		kifcode->kstart->declarations.clear();
		for (it=stack.begin();it!=stack.end();it++)
			kifcode->kstart->declarations[it->first]=it->second;
		
		for (i=kifcode->locals.size()-1;i>=ilocal;i--) {
			j=kifcode->locals[i];
			kifcode->locals.pop_back();
			if (kifGlobals->garbage[j]!=NULL) {
				delete kifGlobals->garbage[j];
				kifGlobals->garbage[j]=NULL;
			}
		}

		kifGlobals->garbage.dernier=igarbage;
		kifGlobals->cursor=cursor;
		return callfunc->kifcode->Returnerror(error);
	}

	for (i=0;i<kifcode->locals.size();i++) {		
		j=kifcode->locals[i];
		if (kifGlobals->garbage[j]!=NULL) {
			kifGlobals->garbage[j]->Setpopped();
			kifGlobals->garbage[j]->Setreference();
		}
	}

    if (kname!=kifNULL) {
        code=kname->String();
        DebugPtr()->AddFilename(code,kifcode);
    }

	return callfunc->kifcode->Provideinteger(firstinstruction);
}

KifElement* PreRun(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {

    KifElement* kkif=callfunc->Evaluate(0,dom);
    int i=((KifKiF*)kkif)->id;
	KifCode* kifcode=kifGlobals->kifCodeVector[i];
	if (kifcode==NULL || kifcode==callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);
	int firstinstruction=callfunc->Evaluate(1,dom)->Integer();
	kifcode->endofexecution=false;
    vector<KifDomain*> keepstack;
	int id=kifGlobals->GetThreadid();
	if (id==-1) {
        for (i=0;i<kifGlobals->stacking.size();i++)
            keepstack.push_back(kifGlobals->stacking[i]);
        kifGlobals->stacking.clear();
    }
	else  {
        for (i=0;i<kifGlobals->threadstacks[id].size();i++)
            keepstack.push_back(kifGlobals->threadstacks[id][i]);
        kifGlobals->threadstacks[id].clear();
    }
	
	kifcode->kstart->Run(firstinstruction);

	if (id==-1) {
        for (i=0;i<keepstack.size();i++)
            kifGlobals->stacking.push_back(keepstack[i]);
    }
	else  {
        for (i=0;i<keepstack.size();i++)
            kifGlobals->threadstacks[id].push_back(keepstack[i]);
    }

    kifcode->endofexecution=true;
	return kifTRUE;
}

KifElement* PreRunAsThread(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
    
    KifElement* kkif=callfunc->Evaluate(0,dom);
    int i=((KifKiF*)kkif)->id;
	KifCode* kifcode=kifGlobals->kifCodeVector[i];
	if (kifcode==NULL || kifcode==callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);
	int firstinstruction=callfunc->Evaluate(1,dom)->Integer();
    vector<KifDomain*> keepstack;
	int id=kifGlobals->GetThreadid();
	if (id==-1) {
        for (i=0;i<kifGlobals->stacking.size();i++)
            keepstack.push_back(kifGlobals->stacking[i]);
        kifGlobals->stacking.clear();
    }
	else  {
        for (i=0;i<kifGlobals->threadstacks[id].size();i++)
            keepstack.push_back(kifGlobals->threadstacks[id][i]);
        kifGlobals->threadstacks[id].clear();
    }

    kifcode->endofexecution=false;
	kifcode->kstart->RunThread(firstinstruction);

	if (id==-1) {
        for (i=0;i<keepstack.size();i++)
            kifGlobals->stacking.push_back(keepstack[i]);
    }
	else  {
        for (i=0;i<keepstack.size();i++)
            kifGlobals->threadstacks[id].push_back(keepstack[i]);
    }

	return kifTRUE;
}


KifElement* PreRunEnd(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
    
    KifElement* kkif=callfunc->Evaluate(0,dom);
    int i=((KifKiF*)kkif)->id;
	KifCode* kifcode=kifGlobals->kifCodeVector[i];
	if (kifcode==NULL || kifcode==callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);    
    if (kifcode->endofexecution)
        return kifTRUE;
	return kifFALSE;
}

KifElement* PreRunStart(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
    
    KifElement* kkif=callfunc->Evaluate(0,dom);
    int i=((KifKiF*)kkif)->id;
	KifCode* kifcode=kifGlobals->kifCodeVector[i];
	if (kifcode==NULL || kifcode==callfunc->kifcode)
		return callfunc->kifcode->Returnerror(kifErrorStrings[222]);    
    return kifcode->kstart->Map(contextualpattern);
}


KifElement* PreSleep(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PrePause");
#endif
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	int res=kfirst->Integer();
#ifdef WIN32
	Sleep(res);
#else
	sleep(res);
#endif
	return kifTRUE;
}



KifElement* PreBegin(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	if (kfirst->type==kifIterator) 
		return kfirst->Begin();
	return callfunc->kifcode->Returnerror(kifErrorStrings[132]);
}

KifElement* PreEnd(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	if (kfirst->type==kifIterator) 
		return kfirst->End();

	return callfunc->kifcode->Returnerror(kifErrorStrings[133]);
}

KifElement* PreNend(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreNend");
#endif
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	if (kfirst->type==kifIterator) {
		if (kfirst->End()->Boolean()==true)
			return kifFALSE;
		return kifTRUE;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[134]);
}

KifElement* PreIteratorKey(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreIteratorKey");
#endif
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	if (kfirst->type==kifIterator) {
		KifElement* ke=kfirst->IteratorKey();
		if (ke==NULL)
			return kifNULL;
		return ke;
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[135]);
}

KifElement* PreValue(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	if (kfirst->type==kifIterator)
		return kfirst->IteratorValue();
	
	return callfunc->kifcode->Returnerror(kifErrorStrings[136]);
}

KifElement* PreValueType(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreValueType");
#endif
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	if (kfirst->type==kifIterator) {
		KifElement* ke=kfirst->IteratorValue();
		string resstr=ke->Type();
		return callfunc->kifcode->Providestring(resstr);
	}
	else {
		string resstr=kfirst->Type();
		return callfunc->kifcode->Providestring(resstr);
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[137]);
}

KifElement* PreValueTypeIs(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
#ifdef PROFILING
	profilingcall("PreValueTypeIs");
#endif
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	KifElement* typ=callfunc->Evaluate(1,dom);
	string resstr;
	if (kfirst->type==kifIterator) {
		KifElement* ke=kfirst->IteratorValue();
		resstr=ke->Type();
	}
	else
		resstr=kfirst->Type();

	string typstr=typ->String();
	if (resstr==typstr)
		return kifTRUE;
	return kifFALSE;
}

KifElement* PreReplace(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	//Replace an element with another
	KifElement* kstr=callfunc->Evaluate(0,dom);
	KifElement* kreg=callfunc->Evaluate(1,dom);
	KifElement* krep=callfunc->Evaluate(2,dom);

	string str=kstr->String();
	string reg=kreg->String();
	string rep=krep->String();

	size_t lookHere = 0;
	size_t foundHere;
	while((foundHere = str.find(reg, lookHere)) != string::npos)    {
		str.replace(foundHere, reg.size(), rep);
		lookHere = foundHere + rep.size();
	}

	return callfunc->kifcode->Providestring(str);
}


KifElement* PreNext(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {

	//First parameter should be a node
	KifElement* kfirst=callfunc->Evaluate(0,dom);
	if (kfirst->type==kifIterator)
		return kfirst->Next();
	return callfunc->kifcode->Returnerror(kifErrorStrings[141]);
}

KifElement* PreLoad(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc) {

	KifElement* ke=callfunc->Evaluate(0,dom);
	KifElement* filename;

	if (ke->type==kifKiF || ke->type==kifLoader) {
		if (callfunc->Size()!=2)
			return callfunc->kifcode->Returnerror(kifErrorStrings[129]);
		KifKiF* kpars=(KifKiF*)ke;
		if (kpars->id!=-1)
			return callfunc->kifcode->Returnerror(kifErrorStrings[179]);
		filename=callfunc->Evaluate(1,dom);
		kpars->name=filename->String();		
		kpars->name=Getfilename(kpars->name);

		string error;
		kpars->id=callfunc->kifcode->KifLoadPath(kpars->name,false);
		if (kpars->id==-1) {
			error=kifErrorStrings[184];
			error+=kpars->name;
			return callfunc->kifcode->Returnerror(error);
		}

		return callfunc->kifcode->Provideinteger(kpars->id);
	}
	return callfunc->kifcode->Returnerror(kifErrorStrings[131]);
}

KifElement* PrePush(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter should be a vector
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is anything
	KifElement* res;
	for (size_t i=1;i<callfunc->Size();i++) {
		res=callfunc->Evaluate(i,dom);
		kf->Push(res);
	}
	return kifTRUE;
}

KifElement* PrePop(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter should be a vector
	KifElement* kf=callfunc->Evaluate(0,dom);

	//second parameter is an integer (the number of pop)
	KifElement* i=NULL;
	int nb=1;
	if (callfunc->Size()==3)
		nb=callfunc->Evaluate(2,dom)->Integer();
	if (callfunc->Size()>=2)
		i=callfunc->Evaluate(1,dom);
	else
		i=kifMINUSONE;

	while (nb>0) {
		kf->Pop(i);
		nb--;
	}

	return kifTRUE;		
}

KifElement* PreLast(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc) {
	//First parameter should be a vector
	KifElement* kf=callfunc->Evaluate(0,dom);
	//second parameter is an integer (the number of pop)

	if (kf->aContainer() || kf->isString())
		return kf->Last();
	return callfunc->kifcode->Returnerror(kifErrorStrings[45]);
}


KifElement* PreSet(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc) {

	KifElement* kfirst=callfunc->Evaluate(0,dom);

	if (kfirst->type==kifWeka) {
		if (callfunc->Size()!=5)
			return callfunc->kifcode->Returnerror(kifErrorStrings[88]);
		KifElement* kline=callfunc->Evaluate(1,dom);
		KifElement* kclass=callfunc->Evaluate(2,dom);
		KifElement* katt=callfunc->Evaluate(3,dom);
		KifElement* kval=callfunc->Evaluate(4,dom);
		KifWeka* kmat=(KifWeka*)kfirst;
		string mclass=kclass->String();
		if (mclass=="")
			return callfunc->kifcode->Returnerror(kifErrorStrings[90]);
		string matt=katt->String();
		if (matt=="")
			return callfunc->kifcode->Returnerror(kifErrorStrings[91]);
		long mline=kline->Integer();
		kmat->Addvalue(mclass,matt,kval,mline);
		return kifTRUE;
	}
	if (kfirst->type==kifIterator) {
		if (callfunc->Size()!=2)
			return callfunc->kifcode->Returnerror(kifErrorStrings[88]);
		KifElement* value=callfunc->Evaluate(1,dom);
		KifElement* var=kfirst->IteratorValue();
		var->Setvalue(value);
		return kifTRUE;
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[92]);
}

KifElement* PreGet(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc) {
	KifElement* kfirst=callfunc->Evaluate(0,dom);

	if (kfirst->isString() || kfirst->type==kifFloat || kfirst->type==kifInteger) { 
		callfunc->kifcode->Gets();
		KifElement* ke=callfunc->kifcode->Providestringraw(kifGlobals->buffer);
		kfirst->Setvalue(ke,NULL,dom);
		return ke;
	}
	if (kfirst->type==kifWeka) {
		if (callfunc->Size()!=4)
			return callfunc->kifcode->Returnerror(kifErrorStrings[93]);
		KifElement* kline=callfunc->Evaluate(1,dom);
		KifElement* kclass=callfunc->Evaluate(2,dom);
		KifElement* katt=callfunc->Evaluate(3,dom);
		KifWeka* kmat=(KifWeka*)kfirst;
		string mclass=kclass->String();
		if (mclass=="")
			return callfunc->kifcode->Returnerror(kifErrorStrings[94]);
		string matt=katt->String();
		if (matt=="")
			return callfunc->kifcode->Returnerror(kifErrorStrings[95]);
		long mline=kline->Integer();	
		if (mline<kmat->values.size()) {
			if (kmat->values[mline].find(mclass)!=kmat->values[mline].end())
				if (kmat->values[mline][mclass].find(matt)!=kmat->values[mline][mclass].end())
					return kmat->values[mline][mclass][matt];
		}
		return kifNULL;
	}

	if (kfirst->type==kifFile) {
		if (callfunc->Size()!=1)
			return callfunc->kifcode->Returnerror(kifErrorStrings[93]);
		KifFile* kfile=(KifFile*)kfirst;
		if (kfile->thefile!=NULL) {
			if (kfile->EndOfFile()==kifTRUE)
				return kifMINUSONE;
			int c=kfile->thefile->get();
			if (pattern->type==kifInteger || pattern->type==kifFloat)
				return callfunc->kifcode->Provideinteger(c);
			else {
				string s;
				s=(char)c;
				return callfunc->kifcode->Providestring(s);
			}
		}
	}

	return callfunc->kifcode->Returnerror(kifErrorStrings[96]);
}

KifElement* PreThreadId(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	int id=kifGlobals->GetThreadid();
	return callfunc->kifcode->Provideinteger(id);
}


KifElement* PreExit(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc) {
	if (callfunc->Size()==0)
		exit(-1);
	KifElement* res=callfunc->Evaluate(0,dom);
	exit(res->Integer());
	return res;
}



//===================================================================
//It is an XML document
#ifdef KIF_RAW

extern "C" {
void NormalisationNomFichier(char* nomFichier,char* tampon) {
    //Tout d'abord on ramene tous les chemins a de l'UNIX
    int l=strlen(tampon);
    //On normalise les chemins
    for (int i=0;i<l;i++) {
        if (tampon[i]==ANTISEP)
            tampon[i]=SEP;
    }

    //s'il y a une variable d'environnement
    char* vari=strchr(tampon,'$');
    nomFichier[0]=0;
	while (vari) {        
		char* reper=getenv(vari+1);
		char* pt=strchr(vari+1,SEP);

		if (pt!=NULL)
			*pt=0;

		//On recopie la partie qui precede la variable
		int lvar=vari-tampon;
		int lnom=strlen(nomFichier);
		memcpy(nomFichier+lnom,tampon,lvar);
		nomFichier[lvar+lnom]=0;                

		if (reper!=NULL)            
			strcat(nomFichier,reper);

		if (pt!=NULL) {
			*pt=SEP;
			static char inter[1000];
			strcpy(inter,pt);
			strcpy(tampon,inter);        
		}
		else
			tampon[0]=0;
		vari=strchr(tampon,'$');
	}

	strcat(nomFichier,tampon);

}
}

KifElement* KifCode::KifParseXipRule(x_node* xn,KifElement* kf) {
	string message=KIFRAWSTRINGS[14]+xn->value;			
	throw new KifRaiseError(message,current->name,current_start,current_end);
}

KifElement* KifCode::KifParseXPath(x_node* xn,KifElement* kf) {
	string message=KIFRAWSTRINGS[14]+xn->value;			
	throw new KifRaiseError(message,current->name,current_start,current_end);
}
#endif

