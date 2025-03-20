#include "kif.h"
#include "kifinit.h"
#include "kifversion.h"
#include "kifn.h"

const char KIF_VERSION[]=KIFVERSION;
#define STR(x) (char*)x.c_str()
extern const char* kifErrorStrings[];
extern "C" {
void NormalisationNomFichier(char* nomFichier,char* tampon);
}

#ifdef WIN32
char KIFOS[]=KIFLIBSSTRINGS[0];
#else
#ifdef APPLE
char KIFOS[]=KIFLIBSSTRINGS[1];
#else
#ifdef ARCH64
char KIFOS[]=KIFLIBSSTRINGS[2];
#else
char KIFOS[]=KIFLIBSSTRINGS[3];
#endif
#endif
#endif

#ifdef WIN32
#define sprintf_ss(a,b,c,d,e) sprintf_s(a,b,c,d,e)
#else
#define sprintf_s(a,b,c,d) sprintf(a,c,d)
#define sprintf_ss(a,b,c,d,e) sprintf(a,c,d,e)
#define strcpy_s(a,b,c) strcpy(a,c)
#endif


typedef KifElement* (KifCode::*KifParseElement)(x_node*,KifElement*);

extern int kif_declencheur;

void KifFinalClean();

KifDebugInfo* DebugPtr();
KIFLONG XLConvert(char* number);
bool TestkifFunctions(short n,hmap<short,bool>& kiffunc);
string KifMetaCharacters(string& m);
string& Trim(string& chaine);
char KifCheckParameters(KifFunction* kfunction,KifCallFunction* kcf,bool add,KifFunction** body);
int KifLoadin(KifCode* kifcode,string path);
void KiFSetdebug(bool d);

#ifndef WIN32
#include <dlfcn.h>
#include <signal.h>
#endif

//===================================================================
//List of all KifCode in memory

bool TestKifFunctions(string& n,bool func) {
	if (n==KIFLIBSSTRINGS[4] || n==KIFLIBSSTRINGS[5])
		return true;
	if (func==false) {
		if (n==KIFLIBSSTRINGS[6] || n==KIFLIBSSTRINGS[7])
			return true;
	}
	return false;
}

Exported bool KifRecordLibraries(string name,KifExternalModule module) {
	if (kifGlobals->kifLibraries.find(name)!=kifGlobals->kifLibraries.end())
		return false;
	
	if (module==NULL)
		return true;

	kifGlobals->kifLibraries[name]=module;
	return true;
}

Exported bool KifRecordTerminateModule(string name,KifExternalTerminateModule module) {
	if (kifGlobals->kifTerminateLibraries.find(name)!=kifGlobals->kifTerminateLibraries.end())
		return false;
	
	if (module==NULL)
		return true;

	kifGlobals->kifTerminateLibraries[name]=module;
	return true;
}
//---------------------------------------------------------------------

Exported string Whoami_kif() {
	string s;
	s=KIFLIBSSTRINGS[8];
	s+=KIF_VERSION;
	s+=Endl;
	s+=KIFLIBSSTRINGS[9];
	s+=Endl;
	s+=KIFLIBSSTRINGS[10];
	s+=Endl;
	s+=KIFLIBSSTRINGS[11];
	s+=Endl;
	return s;
}


#ifdef WIN32
bool KifCode::KifLoadModule(x_node* xn,string n) {
#ifdef PROFILING
	profilingcall("Code::KifLoadModule");
#endif
	if (n=="")
		return false;

	string name=Getfilename(n);
	HINSTANCE LoadMe;
	long line=GetCurrentLine(0,xn);
	string lname=name;
	int pos=lname.find_last_of(SEP);
	string subname=lname;
	if (pos!=-1) {
		subname=lname.substr(pos+1,lname.size()-pos-1);
		pos=subname.find(".");
		if (pos!=-1)
			subname=subname.substr(0,pos);
	}
	
	string initname=KIFLIBSSTRINGS[12];
	initname+=subname;
	string terminatename=subname;
	terminatename+=KIFLIBSSTRINGS[13];
	subname+=KIFLIBSSTRINGS[14];
	KifExternalModule LibInitEntryPoint;
	//If it has already been loaded, we return...
	if (KifRecordLibraries(initname,NULL)==false) {
		LibInitEntryPoint=kifGlobals->kifLibraries[initname];
		(*LibInitEntryPoint)(this,KIF_VERSION);
		return true;
	}

	LoadMe = LoadLibrary(STR(lname));
    if (LoadMe == 0) {
        string kiflib;
        if (getenv(KIFLIBSSTRINGS[15])!=NULL)
            kiflib=getenv(KIFLIBSSTRINGS[15]);
        else {
            lname=KIFLIBSSTRINGS[17];
            throw new KifRaiseError(lname,current->name,line,xn->end);	
        }
        kiflib+="\\";
        kiflib+=subname;
        kiflib+=KIFLIBSSTRINGS[18];
		kiflib=Getfilename(kiflib);
        LoadMe = LoadLibrary(STR(kiflib));
    }
	// Check to see if the library was loaded successfully 

	if (LoadMe == 0) {
		DWORD err=GetLastError();
		lname=kifErrorStrings[189];
		lname+=name;
		throw new KifRaiseError(lname,current->name,line,xn->end);
	}

	KifExternalModule LibMainEntryPoint;            
	KifExternalTerminateModule LibTerminateEntryPoint;

	LibMainEntryPoint = (KifExternalModule)GetProcAddress(LoadMe,STR(subname));
	LibInitEntryPoint = (KifExternalModule)GetProcAddress(LoadMe,STR(initname));
	LibTerminateEntryPoint = (KifExternalTerminateModule)GetProcAddress(LoadMe,STR(terminatename));

	if (LibMainEntryPoint==NULL || LibInitEntryPoint==NULL) {
		lname=kifErrorStrings[190];
		lname+=name;
		throw new KifRaiseError(lname,current->name,line,xn->end);	
	}

	KifRecordLibraries(initname,LibInitEntryPoint);
	kifGlobals->kifTerminateLibraries[terminatename]=LibTerminateEntryPoint;

	if ((*LibMainEntryPoint)(this,KIF_VERSION)==false) {
		FreeLibrary(LoadMe);
		return false;
	}
	return true;
}
#else
bool KifCode::KifLoadModule(x_node* xn,string n) {
#ifdef PROFILING
	profilingcall("Code::KifLoadModule");
#endif
	string name=Getfilename(n);
	void* LoadMe;
	long line=GetCurrentLine(0,xn);
	char lname[1024];
	strcpy(lname,STR(name));
	char* error;
	char* pt=strrchr(lname,'/');
	char buff[1024];
	bool addso=true;
    string basename;
	if (strstr(lname+strlen(lname)-3,KIFLIBSSTRINGS[19]))
		addso=false;
	if (pt!=NULL) {
        if (memcmp(pt+1,KIFLIBSSTRINGS[20],3)) {
            if (addso)
                sprintf(buff,"lib%s.so",pt+1);
            else
                sprintf(buff,"lib%s",pt+1);
        }
        else {
            if (addso)
                sprintf(buff,"%s.so",pt+1);
            else
                strcpy(buff,pt+1);
        }
        basename=buff;
		strcpy(pt+1,buff);
	}
	else {
        if (memcmp(lname,KIFLIBSSTRINGS[20],3)) {
            if (addso)
                sprintf(buff,"lib%s.so",lname);
            else
                sprintf(buff,"lib%s",lname);
        }
        else {
            if (addso)
                sprintf(buff,"%s.so",lname);
            else
                strcpy(buff,lname);
        }
        basename=buff;
		strcpy(lname,buff);
	}

	NormalisationNomFichier(buff,lname);
	strcpy(lname,buff);
	string subname=basename.substr(3,basename.size()-6);
	string initname=KIFLIBSSTRINGS[12];
	initname+=subname;
	string terminatename=subname;
	terminatename+=KIFLIBSSTRINGS[13];
	subname+=KIFLIBSSTRINGS[14];
	KifExternalModule LibInitEntryPoint;
	//If it has already been loaded, we return...
	if (KifRecordLibraries(initname,NULL)==false) {
		LibInitEntryPoint=kifGlobals->kifLibraries[initname];
		(*LibInitEntryPoint)(this,KIF_VERSION);
		return true;
	}

	LoadMe = dlopen(lname,RTLD_LAZY);
	if (LoadMe == NULL) {
        string kiflib;
        if (getenv(KIFLIBSSTRINGS[15])!=NULL)
            kiflib=getenv(KIFLIBSSTRINGS[15]);
        else {
            basename=KIFLIBSSTRINGS[17];
            throw new KifRaiseError(basename,current->name,line,xn->end);	
        }
        kiflib+="/";
        kiflib+=basename;
		kiflib=Getfilename(kiflib);
        LoadMe = dlopen(STR(kiflib),RTLD_LAZY);
    }

	// Check to see if the library was loaded successfully 

	if (LoadMe == NULL) {
		error=dlerror();
		string message=error;
		message+=": ";
		message+=lname;
		cerr<<STR(message)<<Endl;
		throw new KifRaiseError(message,current->name,line,xn->end);
	}


	KifExternalModule LibMainEntryPoint;
	KifExternalTerminateModule LibTerminateEntryPoint;

	LibMainEntryPoint = (KifExternalModule)dlsym(LoadMe,STR(subname));
	LibInitEntryPoint = (KifExternalModule)dlsym(LoadMe,STR(initname));
	LibTerminateEntryPoint = (KifExternalTerminateModule)dlsym(LoadMe,STR(terminatename));

	if ((error = dlerror()) != NULL) {
		string message=error;
		message+=": ";
		message+=name;
		cerr<<STR(message)<<Endl;
		throw new KifRaiseError(message,current->name,line,xn->end);
	}

	KifRecordLibraries(initname,LibInitEntryPoint);
	kifGlobals->kifTerminateLibraries[terminatename]=LibTerminateEntryPoint;

	if ((*LibMainEntryPoint)(this,KIF_VERSION)==false) {
		dlclose(LoadMe);
		return false;
	}
	return true;
}
#endif

KifElement* KifCode::KifTerminateVariable(x_node* xn,KifElement* kv,KifElement* var,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifTerminateVariable");
#endif
	short name=KifNewId(xn->nodes[1]->value);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(name,var,NULL);
	return var;
}

KifCode* KifElement::KifCurrent() {
	return kifcode;
}

bool KifGlobalThread::KifCreatePredefined(KifCode* kcode,string name,unsigned int arity,Predefined func,bool tree) {
#ifdef PROFILING
	profilingcall("Code::KifCreatePredefined");
#endif
	int idname=KifNewId(name);
	if (tree==false) {
		if (kifBaseFunctions.find(idname)!=kifBaseFunctions.end())
			return false;

		kifBaseFunctions[idname]=new KifPredefined(NULL,arity,func,idname);
		kifBaseFunctions[idname]->kifcode=kcode;
	}
	else {
		if (kifBaseTreeFunctions.find(idname)!=kifBaseTreeFunctions.end())
			return false;

		kifBaseTreeFunctions[idname]=new KifPredefined(NULL,arity,func,idname);
		kifBaseTreeFunctions[idname]->kifcode=kcode;
	}

	return true;
}

void KifGlobalThread::RecordExternalFunction(short idname,unsigned int arity) {		
	if (kifExternalFunctions.find(idname)==kifExternalFunctions.end()) {			
		KifCallMethod* kcm=new KifCallMethod(idname,arity);
		kifExternalFunctions[idname]=kcm;
	}
	else
		kifExternalFunctions[idname]->arity|=arity;
}

bool KifGlobalThread::TestExternalArity(int id,int nbparams) {
	if (kifExternalFunctions.find(id)==kifExternalFunctions.end())
		return false;
    
	unsigned int arity=1<<nbparams;
	unsigned int expected=kifExternalFunctions[id]->arity;
	if ((expected&arity)!=arity)
		return false;
	return true;
}

KifElement* KifGlobalThread::KifBaseFunctionTest(short name) {
	if (kifBaseFunctions.find(name)!=kifBaseFunctions.end())
		return kifBaseFunctions[name];
    if (kifExternalFunctions.find(name)!=kifExternalFunctions.end())
        return kifExternalFunctions[name];
    if (kifBaseTreeFunctions.find(name)!=kifBaseTreeFunctions.end())
		return kifBaseTreeFunctions[name];
	return NULL;
}

bool KifCode::KifDeclaredFunctionTest(short name) {
	if (kifDeclaredFunction.find(name)!=kifDeclaredFunction.end())
		return true;
	return false;
}

KifElement* KifCode::KifParseNegation(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseNegation");
#endif
	kf->Setnegation(true);
	return kf;
}


KifElement* KifCode::KifParseBoolean(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseBoolean");
#endif
	KifInstruction* kbloc;
	if (xn->nodes.size()==1) {
		//It is a unique test
		kbloc=new KifInstruction(this,kf,kifBLOCBOOLEAN);
		kbloc->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
		KifBrowse(xn->nodes[0],kbloc);
		if (kbloc->instructions.size()==2) {
			kbloc->instructions[0]->BuildNumericalExpression(0);
			kbloc->instructions[1]->BuildNumericalExpression(0);
		}
		return kbloc;
	}
	//Else, we have two expressions and an operator
	//If our operator is new to our test
	KifElement* ke=kf;
	KifOperator op=kifGlobals->kifOperators[xn->nodes[1]->value];
	if (op!=kf->Action()) {
		if (kf->Action()==kifBLOCBOOLEAN)
			ke->Setaction(op);
		else {
			ke=new KifInstruction(this,kf,op);
			ke->Setline(linereference+GetCurrentLine(0,xn),currentfile);
		}
	}

	kbloc=new KifInstruction(this,ke,kifBLOCBOOLEAN); 
	kbloc->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
	KifBrowse(xn->nodes[0],kbloc);	
	if (kbloc->instructions.size()==2) {
		kbloc->instructions[0]->BuildNumericalExpression(0);
		kbloc->instructions[1]->BuildNumericalExpression(0);
	}
	if (xn->nodes.size()==3)
		KifBrowse(xn->nodes[2],ke);
	else
		KifBrowse(xn->nodes[1],kbloc);
	return ke;
}

KifElement* KifCode::KifParseTestSwitch(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseTestSwitch");
#endif
	if (xn->nodes[0]->token==KIFLIBSSTRINGS[28])
		kf->Add(kifDEFAULT);
	else
		KifBrowse(xn->nodes[0],kf);
	KifDomain* ktrue=new KifInstructionSEQUENCE(this,kf,idsequence);
	ktrue->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
	kifGlobals->PushStack(ktrue);
	KifBrowse(xn->nodes[1],ktrue);
	kifGlobals->PopStack();
	return ktrue;
}

KifElement* KifCode::KifParseSwitch(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseSwitch");
#endif
	//We create a IF section
	KifElement* kswitch=new KifInstructionSWITCH(this,kf);
	kswitch->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
	KifBrowse(xn->nodes[0],kswitch);
	int i=1;
	if (xn->nodes[1]->token==KIFLIBSSTRINGS[29]) {
		i++;
		long line=GetCurrentLine(0,xn->nodes[1]);
		string funcname=xn->nodes[1]->nodes[0]->value;
		short idf=KifNewId(funcname);
		KifElement* kfunc=NULL;
		kfunc=Getbasic(idf,kf);
		//We have a WITH description
		if (kfunc==NULL) {
			string message=kifErrorStrings[178]+funcname;				
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
		if (kfunc->isFunction())
			kswitch->Setfunction(kfunc);
		else {
			if (!kfunc->isCall()) {
				string message=KIFLIBSSTRINGS[30]+kifGlobals->kifIdString[kfunc->Name()]+"'";
				throw new KifRaiseError(message,current->name,current_start,current_end);
			}
			kswitch->Setfunction(new KifInstance(this,NULL,kfunc->Name(),(KifVariableDeclaration*)kfunc));
		}
	}

	for (;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],kswitch);
	return kswitch;
}

KifElement* KifCode::KifParseTryCatch(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseTryCatch");
#endif
	KifElement* ke=new KifInstructionTRY(this,kf);
	ke->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	string name;
	KifElement* declaration;
	KifInstance* ki;

	long line=GetCurrentLine(0,xn);
	short id;
	if (xn->nodes.size()!=1 && xn->nodes[1]->token==KIFLIBSSTRINGS[31]) {
		name=xn->nodes[1]->value;
		id=KifNewId(name);
		declaration=Getbasic(id,kf);

		if (declaration==NULL || declaration->XIP()) {
			string message=KIFLIBSSTRINGS[32]+name+"'";
			throw new KifRaiseError(message,current->name,line,xn->end);
		}

		if (!declaration->Variable()) {
			if (xn->nodes.size()==1 && (kifAllTypes[id]==true || declaration->isFunction())) {
				KifElement* ke=new KifSelf(this,kf,declaration);
				ke->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
				return ke;
			}
			string message=KIFLIBSSTRINGS[33]+name+"'";
			throw new KifRaiseError(message,current->name,line,xn->end);
		}

		KifInstruction* kaff=new KifInstruction(this,ke,kifAFFECTATION);
		kaff->Setline(linereference+GetCurrentLine(0,xn),currentfile);
		ki=new KifInstance(this,kaff,id,(KifVariableDeclaration*)declaration);
		if (declaration->type==kifFunctionDeclaration)
			((KifInstance*)ki)->Setinstancetype(kinFunction);
		else
			if (declaration->type==kifFrameDeclaration) {
				KifElement* currentdeclaration=FindFrameForSequence(declaration,xn,-1);
				if (currentdeclaration==kstart)
					((KifInstance*)ki)->Setinstancetype(kinGlobal);
				else
					((KifInstance*)ki)->Setinstancetype(kinFrame);
			}
		ki->Setstringvalue(name);
		kaff->Add(kifNULL);
	}

	KifBrowse(xn->nodes[0],ke);

	
	KifElement* kfunc=kifGlobals->kifBaseFunctions[idcatch];
	KifCallFunction* kcf=new KifCallFunctionPredefined(this,ke,idcatch,(KifFunction*)kfunc);
	kcf->Setline(linereference+GetCurrentLine(0,xn),currentfile);

	if (xn->nodes.size()!=1) {
		if (xn->nodes[1]->token==KIFLIBSSTRINGS[31]) {
			ki=new KifInstance(this,kcf,id,(KifVariableDeclaration*)declaration);
			if (declaration->type==kifFunctionDeclaration)
				((KifInstance*)ki)->Setinstancetype(kinFunction);
			else
				if (declaration->type==kifFrameDeclaration) {
					KifElement* currentdeclaration=FindFrameForSequence(declaration,xn,-1);
					if (currentdeclaration==kstart)
						((KifInstance*)ki)->Setinstancetype(kinGlobal);
					else
						((KifInstance*)ki)->Setinstancetype(kinFrame);
				}

			ki->Setstringvalue(name);
			ki->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
			if (xn->nodes.size()==3) {
				if (xn->nodes[2]->token==KIFLIBSSTRINGS[35])  {
					KifElement* kbloc=new KifInstructionCATCH(this,ke);
					kbloc->Setline(linereference+GetCurrentLine(0,xn->nodes[2]),currentfile);
					//Instruction
					KifBrowse(xn->nodes[2],kbloc);
				}
			}
		}
		else {
			if (xn->nodes[1]->token==KIFLIBSSTRINGS[35])  {
				KifElement* kbloc=new KifInstructionCATCH(this,ke);
				kbloc->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
				//Instruction
				KifBrowse(xn->nodes[1],kbloc);
			}
		}
	}
	kcf->add=0;
	return ke;
}


KifElement* KifCode::KifParseTest(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseTest");
#endif
	KifElement* ke=new KifInstructionIF(this,kf);
	ke->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
	KifBrowse(xn->nodes[0],ke);
	KifElement* kobj=ke;
	bool neg=ke->Instruction(0)->Negation();
	while (kobj!=NULL && kobj->InstructionSize()==1) {
		if (kobj->Negation())
			neg=true;
		kobj=(KifObject*)kobj->Instruction(0);
	}
	if (kobj!=ke->Instruction(0))
		((KifObjectInstruction*)ke)->instructions[0]=kobj;
	ke->Setnegation(neg);	
	KifDomain* ktrue=new KifInstructionSEQUENCE(this,ke,idsequence);
	kifGlobals->PushStack(ktrue);
	ktrue->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	KifBrowse(xn->nodes[1],ktrue);
	kifGlobals->PopStack();
	
	if (xn->nodes.size()==3) {
		KifDomain* kfalse=new KifInstructionSEQUENCE(this,ke,idsequence);
		kfalse->Setline(linereference+GetCurrentLine(0,xn->nodes[2]),currentfile);
		kifGlobals->PushStack(kfalse);
		KifBrowse(xn->nodes[2],kfalse);
		kifGlobals->PopStack();
	}

	return ke;
}

KifElement* KifCode::KifParseWhile(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseWhile");
#endif
	KifInstructionWHILE* kwhile=new KifInstructionWHILE(this,kf);
	kwhile->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
	KifBrowse(xn->nodes[0],kwhile);
	KifDomain* kseq=new KifInstructionSEQUENCE(this,kwhile,idsequence);
	kseq->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kifGlobals->PushStack(kseq);
	KifBrowse(xn->nodes[1],kseq);
	kifGlobals->PopStack();

	KifInstruction* ki=(KifInstruction*)(kwhile->instructions[0]);
	switch (ki->action) {
	case kifEQUAL:
	case kifSUPERIOR:
	case kifINFERIOR:
	case kifSUPEQU:
	case kifINFEQU:
	case kifDIFFERENT:
		kwhile->neg=ki->negation;
		kwhile->compare=true;
	}

	KifElement* kinst=kwhile->instructions[1];
	if (kinst->type==kifInstruction)
		kwhile->evalue=true;
	else //a sequence of one instruction...
		if (kinst->type==kifInstructionSEQUENCE && 
			kinst->InstructionSize()==1) { 
				kwhile->instructions[1]=kinst->Instruction(0);
				if (kinst->Instruction(0)->type==kifInstruction)				
					kwhile->evalue=true;		
		}

	return kwhile;
}

KifElement* KifCode::KifParseForIn(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseWhile");
#endif
	
	KifObjectInstruction* kwhile;
	KifInstruction ktemp(NULL,NULL,kifBLOCLOOPIN); 
	KifBrowse(xn->nodes[1],&ktemp);
	KifElement* kcontainer=ktemp.instructions[0];
	if (kcontainer->type==kifInstance && ((KifInstance*)kcontainer)->value->type==kifFile)
		kwhile=new KifInstructionFILEIN(this,kf);
	else
		kwhile=new KifInstructionFORIN(this,kf);

	kwhile->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);	
	KifElement* kin=new KifInstruction(this,kwhile,kifBLOCLOOPIN); 
	KifBrowse(xn->nodes[0],kin);	
	kin->Add(kcontainer);
	
	//We then compile the instruction bloc
	KifDomain* ktrue=new KifInstructionSEQUENCE(this,kwhile,idsequence);
	ktrue->Setline(linereference+GetCurrentLine(0,xn->nodes[2]),currentfile);
	kifGlobals->PushStack(ktrue);
	KifBrowse(xn->nodes[2],ktrue);
	kifGlobals->PopStack();	

	KifElement* kinst=kwhile->instructions[1];
	if (kinst->type==kifInstruction)
		kwhile->Setevalue();
	else //a sequence of one instruction...
		if (kinst->type==kifInstructionSEQUENCE && 
			kinst->InstructionSize()==1) {
				kwhile->instructions[1]=kinst->Instruction(0);
				if (kinst->Instruction(0)->type==kifInstruction)				
					kwhile->Setevalue();
		}

	return kwhile;
}

KifElement* KifCode::KifParseBlocFor(x_node* xn,KifElement* kf) {
	KifDomain* kbloc=new KifInstructionSEQUENCE(this,kf,idsequence);
	for (int i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],kbloc);
	return kbloc;
}

KifElement* KifCode::KifParseFor(x_node* xn,KifElement* kf) {
	KifInstructionFOR* kfor=new KifInstructionFOR(this,kf);
	kfor->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
	//Initialisation
	KifBrowse(xn->nodes[0],kfor);
	//Test
	KifBrowse(xn->nodes[1],kfor);
	//Increment
	KifBrowse(xn->nodes[2],kfor);
	KifDomain* kbloc=new KifInstructionSEQUENCE(this,kfor,idsequence);
	kbloc->Setline(linereference+GetCurrentLine(0,xn->nodes[3]),currentfile);
	//Instruction
	kifGlobals->PushStack(kbloc);
	KifBrowse(xn->nodes[3],kbloc);
	kifGlobals->PopStack();

	if (kfor->instructions[1]->Action()>=kifEQUAL && kfor->instructions[1]->Action()<=kifDIFFERENT) {
		kfor->neg=((KifInstruction*)kfor->instructions[1])->negation;
		kfor->compare=true;
	}

	return kfor;
}

//The parameter list is a two nodes tree, where the second node is always a parameter
//hence a recursive analysis
void KifCode::ComputeParameters(x_node* xn,KifCallFunction* kcf) {
#ifdef PROFILING
	profilingcall("Code::ComputeParameters");
#endif
	KifElement* kbloc=new KifInstructionPARAMETER(this,kcf);
	kbloc->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
	KifBrowse(xn->nodes[0],kbloc);
	kbloc->BuildNumericalExpression(0);
	if (xn->nodes.size()==2)
		ComputeParameters(xn->nodes[1],kcf);
}

KifElement* KifCode::KifParseUniqueCall(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseUniqueCall");
#endif
	string& name=xn->value;
	//Looking if it is known as function
	KifElement* kcf;	

	if (name==KIFLIBSSTRINGS[37])
		kcf=new KifCallFunctionBreakOuContinue(this,kf,kifBREAK);
	else {
		if (name==KIFLIBSSTRINGS[38])
			kcf=new KifCallFunctionBreakOuContinue(this,kf,kifCONTINUE);
		else
			if (name==KIFLIBSSTRINGS[39])
				kcf=new KifReturn(this,kf);
			else
				if (name==KIFLIBSSTRINGS[40])
					kcf=new KifCallFunctionBreakpoint(this,kf);
				else {				
					KifElement* kfunc=kifGlobals->kifBaseFunctions[idkifmain];		
					kcf=new KifCallFunctionPredefined(this,kf,idkifmain,(KifFunction*)kfunc);
					((KifCallFunctionPredefined*)kcf)->add=0;
					kcf->Setline(linereference+GetCurrentLine(0,xn),currentfile);
					kf->Initvalue(kcf);		

				}
	}
	return kcf;
}

KifElement* KifCode::KifNonLimited(x_node* xn,KifElement* kf) {
	kf->Setlimited(true);
	return kf;
}

KifElement* KifCode::KifParseCall(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseCall");
#endif
	string name=xn->nodes[0]->value;
	short idframe=-1;
	short fullid=-1;
	if (xn->nodes[0]->nodes.size()!=1) {
		string framename=xn->nodes[0]->nodes[0]->nodes[0]->value;
		name=xn->nodes[0]->nodes[1]->value;
		idframe=KifNewId(framename);
		framename+="::";
		framename+=name;
		fullid=KifNewId(framename);
	}

	short id=KifNewId(name);
	bool basictype=false;
	bool getbasefunction=false;
	long line=GetCurrentLine(0,xn);

	if (name==KIFLIBSSTRINGS[41]) {
		//very specific case, to set the garbage size from within
		if (xn->nodes.size()==2) {
			KifElement kxf(NULL,NULL);
			KifElement* res=KifBrowse(xn->nodes[1],&kxf);
			res=res->Execute(kifNULL,kstart);
			long sz=res->Integer();
			if (sz>kif_declencheur) {
				kifGlobals->trigger=sz;
				kifGlobals->garbage.ajuste(sz);
			}
		}
		else {
			string message=KIFLIBSSTRINGS[42];
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
		return kf;
	}

	if (name==KIFLIBSSTRINGS[43]) {
		if (xn->nodes.size()==2) {
			KifElement* kfunc=NULL;
			name=xn->nodes[1]->nodes[0]->nodes[0]->value;
			id=KifNewId(name);
			kfunc=Getbasic(id,kf);
			//We have a WITH description
			if (kfunc==NULL || kfunc->type!=kifFunction) {
				string message=kifErrorStrings[178]+name;
				long line=GetCurrentLine(0,xn);
				throw new KifRaiseError(message,current->name,line,xn->end);
			}
			kifGlobals->garbagefunction=(KifFunction*)kfunc;
		}
		else {
			string message=KIFLIBSSTRINGS[44];
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
		return kf;
	}

	if (name==KIFLIBSSTRINGS[45]) {
		//very specific case, where we load our lib on the fly
		if (xn->nodes.size()==2) {
			KifElement kxf(NULL,NULL);
			KifElement* pathlib;
			KifElement* platform;
			bool load=true;
			if (xn->nodes[1]->nodes.size()==2) {
				if (xn->nodes[1]->nodes[1]->nodes.size()==2) {
					KifElement* arg=KifBrowse(xn->nodes[1]->nodes[0],&kxf);
					arg=arg->Execute(kifNULL,kstart);
					platform=KifBrowse(xn->nodes[1]->nodes[1]->nodes[0],&kxf);				
					platform=platform->Execute(kifNULL,kstart);
					if (arg->String()==platform->String())
						pathlib=KifBrowse(xn->nodes[1]->nodes[1]->nodes[1],&kxf);
					else
						load=false;
				}
				else {
					platform=KifBrowse(xn->nodes[1]->nodes[0],&kxf);				
					platform=platform->Execute(kifNULL,kstart);
					if (platform->String()!=KIFOS)
						load=false;
					else
						pathlib=KifBrowse(xn->nodes[1]->nodes[1],&kxf);
				}
			}
			else
				pathlib=KifBrowse(xn->nodes[1],&kxf);
			if (load) {
				pathlib=pathlib->Execute(kifNULL,kstart);
				KifLoadModule(xn,pathlib->String());
			}
		}
		else {
			string message=KIFLIBSSTRINGS[46];
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
		return kf;
	}

	if (name==KIFLIBSSTRINGS[47]) {
		//very specific case, where we load a KiF program on the fly
		if (xn->nodes.size()==2) {
			KifElement kxf(NULL,NULL);
			KifElement* res=KifBrowse(xn->nodes[1],&kxf);
			res=res->Execute(kifNULL,kstart);
			KifLoadin(this,res->String());
		}
		else {
			string message=KIFLIBSSTRINGS[48];
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
		return kf;
	}

	//Particular case, where the function is a predefined function linked to an instance
	if (kifGlobals->KifBaseFunctionTest(id)!=NULL) {
		if(kf->type==kifInstanceFunction)
			basictype=true;
		else
			if (kf->type==kifIndex || CALLFUNCTION(kf)) {
				basictype=true;
				getbasefunction=true;
			}
			else
				getbasefunction=true;
	}
	
	//Particular case, where a different frame was provided through a frame::function call
	KifElement* kframe=kf;
	KifElement* kfunc=NULL;
	if (idframe!=-1 && kf->Frame()!=NULL) {
		KifElement* localframe=kf->Frame();
		kfunc=Getbasic(fullid,localframe);
		if (kfunc==NULL) {
			kframe=Getbasic(idframe,kf);
			kfunc=Getbasic(id,kframe);			
			localframe->Set(fullid,kfunc,NULL);
		}
	}
	else
		//Looking if it is known as a function
		kfunc=Getbasic(id,kframe);

	if (kifAllTypes[id]==true && kfunc!=NULL && kfunc->type==kifFrame) {
		//This is a very specific case, where we create a new instance
		//for a frame. kfunc is the frame definition, which will be used
		//to create a new instance in Setvalue.
		KifSelf* ke=new KifSelf(this,kframe,kfunc);
		ke->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
		KifInstruction* ki=new KifInstruction(this,ke,kifINITIALISATION);
		ki->Setline(ke->Line(),currentfile);
		//if we have some parameters
		if (xn->nodes.size()==2)
			KifBrowse(xn->nodes[1],ki);
		for (int ii=0;ii<ki->instructions.size();ii++)
			ki->instructions[ii]->BuildNumericalExpression(0);
		ke->parameter=ki;
		return ke;
	}

	if (kfunc==NULL && getbasefunction==true) {
		if (basictype==false && kifGlobals->kifMethods.find(id)!=kifGlobals->kifMethods.end()) {
			string message=KIFLIBSSTRINGS[30]+name+"'";
			throw new KifRaiseError(message,current->name,line,xn->end);
		}

		kfunc=kifGlobals->KifBaseFunctionTest(id);
	}
	
	if (name==KIFLIBSSTRINGS[39])
		kfunc=new KifCallReturn(this,kf,id);

	if (kfunc==NULL) {
		string message=KIFLIBSSTRINGS[30]+name+"'";
		throw new KifRaiseError(message,current->name,line,xn->end);
	}

	if (kfunc==kifTRUE)
		kfunc=NULL;
	
	if (idframe!=-1)
		id=fullid;

	KifCallFunction* kcf=NULL;
	if (kfunc!=NULL) {
        if (kfunc->type==kifVariable) {
            kfunc=kfunc->Returnvalue();
            if (kfunc!=NULL && kfunc->type==kifCall)
				kcf=new KifCallFunctionCall(this,kf,id,kfunc);
            else {
                string message=KIFLIBSSTRINGS[52];
                message+=name;
                throw new KifRaiseError(message,current->name,line,xn->end);
            }
        }
        else {
            if (name==KIFLIBSSTRINGS[39])
                kcf=(KifCallFunction*)kfunc;
            else
                if (kfunc->Linkedvalue()!=NULL && kfunc->Linkedvalue()->type==kifCall)
                    kcf=new KifCallFunctionCall(this,kf,id,kfunc);
                else {
                    kfunc->Setused();
                    if (kfunc->type==kifPredefined)
                        kcf=new KifCallFunctionPredefined(this,kf,id,(KifFunction*)kfunc);
                    else
                        if (kfunc->type==kifCallMethod)
                            kcf=new KifCallFunctionExternal(this,kf,id,(KifFunction*)kfunc);
                        else
                            if (kfunc->type==kifThread)
                                kcf=new KifCallFunctionThread(this,kf,id,(KifFunction*)kfunc);
                }
        }
	}

	if (kcf==NULL)
		kcf=new KifCallFunctionGeneral(this,kf,id,(KifFunction*)kfunc);

	kcf->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
	
	int minusarity=0;
	//We check again to see if a specific function name was not overloaded
	if (basictype==true && kfunc!=NULL && kfunc->type==kifPredefined) {
		kcf->Setselfinsert(true);
		kcf->parameters.push_back(kifNULL);
		minusarity=1;
	}
	else
		if (kfunc!=NULL && kfunc->type==kifCallMethod)
			kcf->Setselfinsert(true);

	kf->Initvalue(kcf);	
	if (xn->nodes.size()>=2) {
		int last=1;
		if (xn->nodes[last]->token==KIFLIBSSTRINGS[54]) {
			ComputeParameters(xn->nodes[last],kcf);
			last++;
		}
		if (xn->nodes.size()>last) {
			if (TestKifFunctions(xn->nodes[last]->token,false)) {
				kcf->add=2;
				KifBrowse(xn->nodes[last],kcf);
			}
		}
	}

	if (kfunc!=NULL && kfunc->isFunction()) {
		//if we have a call:  var.FRAME::Function, then we record this FRAME::Function as a new function
		//within the frame...

		char ret=KifCheckParameters((KifFunction*)kfunc,kcf,true,NULL);
		if (ret==1) {
			string message=kifErrorStrings[158];
			message+=name;
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
		else
			if (ret==2) {
				string message=KIFLIBSSTRINGS[55];
				message+=name;
				throw new KifRaiseError(message,current->name,line,xn->end);
			}
	
	}

	if (kfunc!=NULL) {
		int nbparams=kcf->parameters.size();
		unsigned int arity=1<<nbparams;
		if (kfunc->type==kifPredefined) {
			unsigned int expected=((KifPredefined*)kfunc)->arity;			
			if ((expected&arity)!=arity && kifGlobals->TestExternalArity(id,nbparams-minusarity)==false) {
				string message=kifErrorStrings[257];
				message+=name;
				throw new KifRaiseError(message,current->name,line,xn->end);                
			}
		}
		else {
			if (kfunc->type==kifCallMethod) {
				unsigned int expected=((KifCallMethod*)kfunc)->arity;	
				if ((expected&arity)!=arity) {
					string message=kifErrorStrings[257];
					message+=name;
					throw new KifRaiseError(message,current->name,line,xn->end);
				}
			}
		}
	}

	kcf->add=0;
	return kcf;
}

KifElement* KifCode::KifParseIndexes(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseIndexes");
#endif
	KifElement* ki=new KifIndex(this,kf,false);
	ki->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	kf->Initvalue(ki);
	//KifInstruction* kidx=new KifInstruction(this,ki,kifBLOC);
	KifInstruction kidx(NULL,NULL,kifBLOC);
	kidx.kifcode=this;
	//kidx->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
	KifBrowse(xn->nodes[0],&kidx);
	kidx.instructions[0]->BuildNumericalExpression(0);
	ki->Add(kidx.instructions[0]);
	if (xn->nodes.size()==2)
		KifBrowse(xn->nodes[1],ki);
	return ki;
}

KifElement* KifCode::KifParseInterval(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseInterval");
#endif
	string ty=kf->Type();
	long line=GetCurrentLine(0,xn);
	//if (ty!=KIFLIBSSTRINGS[56] && ty!=KIFLIBSSTRINGS[57]) {
	//	string message=KIFLIBSSTRINGS[58]+kf->Name()+KIFLIBSSTRINGS[59]+ty;
	//	throw new KifRaiseError(message,current->name,line,xn->end);
	//}

	int xsz=xn->nodes.size();

	if (xsz==1) {
		string message=KIFLIBSSTRINGS[60];
		throw new KifRaiseError(message,current->name,line,xn->end);
	}


	KifIndex* ki=new KifIndex(this,kf,true);
	ki->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	kf->Initvalue(ki);

	if (TestKifFunctions(xn->nodes[xsz-1]->token,false))
		xsz--;
	
	KifInstruction kidx(NULL,NULL,kifBLOC);
	kidx.kifcode=this;
	//Three cases
	if (xsz==3) {
		//We have the first value, then the separator then the other value
		KifBrowse(xn->nodes[0],&kidx);
		kidx.instructions[0]->BuildNumericalExpression(0);
		ki->Add(kidx.instructions[0]);
		KifInstruction kidxx(NULL,NULL,kifBLOC);
		kidxx.kifcode=this;
		KifBrowse(xn->nodes[2],&kidxx);
		kidxx.instructions[0]->BuildNumericalExpression(0);
		ki->Add(kidxx.instructions[0]);
	}
	else
		if (xn->nodes[0]->token==KIFLIBSSTRINGS[61]) {
			//then the first parameter is not known
			ki->key=kifNULL;
			KifBrowse(xn->nodes[1],&kidx);
			kidx.instructions[0]->BuildNumericalExpression(0);
			ki->Add(kidx.instructions[0]);
		}
		else {//The last parameter is not known
			KifBrowse(xn->nodes[0],&kidx);
			ki->Add(kidx.instructions[0]);
			kidx.instructions[0]->BuildNumericalExpression(0);
			ki->right=kifNULL;
		}
	
	//If we have a call
	if (xsz!=xn->nodes.size()) {
		ki->interval=false;
		KifBrowse(xn->nodes[xsz],ki);
		ki->interval=true;
	}

	return ki;
}

KifElement* KifCode::KifParseOperator(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseOperator");
#endif
	short op= kifGlobals->kifOperators[xn->value];
	kf->Set(op,kf,NULL);
	return kf;
}

KifElement* KifCode::KifParseBloc(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseBloc");
#endif
	KifInstruction* ki=(KifInstruction*)kf;
	if (kf->Action()!=kifBLOCBOOLEAN) {
		ki=new KifInstruction(this,kf,kifBLOC);
		ki->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	}
	for (size_t i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],ki);
	return ki;	
}

KifElement* KifCode::KifParseComparison(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseComparison");
#endif
	//The first parameter is the operator
	KifElement* ki=kf;
	KifOperator op=kifGlobals->kifOperators[xn->nodes[0]->value];
	if (kf->Action()==kifBLOCBOOLEAN || kf->Action()==kifBLOC)
		ki->Setaction(op);
	else
		if (kf->Action()>=kifPLUS && kf->Action()<=kifINTERSECTION) {
			KifElement* kparent=kf->Parent();
			if (kparent->Action()==kifBLOCBOOLEAN || kparent->Action()==kifBLOC) {
				kparent->Setaction(op);
				ki=kparent;
			}
			else
				ki=new KifInstruction(this,kparent,op);		
		}
		else
			ki=new KifInstruction(this,kf,op);		
	//ki->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	for (size_t i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],ki);
	return ki;
}

KifElement* KifCode::KifParsePlusPlus(x_node* xn,KifElement* kf) {
	KifOperator op=kifGlobals->kifOperators[xn->value];
	kf->Setaction(op);
	return kf;
}


KifElement* KifCode::KifParseOperation(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseOperation");
#endif
		
	//The first parameter is the operator	
	KifOperator op=kifGlobals->kifOperators[xn->nodes[0]->value];
	//The second parameter is the rest of the operation
	//kf is the TOP instruction
	KifInstruction* ki;
	if (kf->Action()==kifBLOC) {
		//We are in a new bloc, which is our current element
		//In this case, we create a new level
		ki=new KifInstruction(this,NULL);
		ki->Setline(linereference+GetCurrentLine(0,xn),currentfile);
		//It becomes the new element
		ki->Setaction(op);
		ki->Add(kf->Instruction(0));
		kf->Addinstruction(0,ki);
		ki->Addparent(kf);
		KifBrowse(xn->nodes[1],ki);
		return ki;
	}

	if (kf->Action()==op) {
		KifBrowse(xn->nodes[1],kf);
		return kf;
	}

	//In this case, the operator is not the same
	//we still have two cases: if it is
	//if it is a PLUS or a MINUS, we reset the top node with the new information
	ki=new KifInstruction(this,NULL);
	ki->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	//It has to be in the middle of an operation
	if (kifGlobals->kifOperatorMath[kf->Action()]==true) {
		if (op==kifPLUS || op==kifMINUS) {
			ki->Setaction(kf->Action());
			for (int i=0;i<kf->InstructionSize();i++)
				ki->instructions.push_back(kf->Instruction(i));
			kf->Setaction(op);
			kf->InstructionClear();
			kf->Addinstruction(0,ki);
			ki->Addparent(kf);
			KifBrowse(xn->nodes[1],kf);
			return kf;
		}
	}

	//we create a new level
	ki->Setaction(op);
	ki->Add(kf->Lastinstruction());
	kf->Addinstruction(kf->InstructionSize()-1,ki);
	ki->Addparent(kf);
	KifBrowse(xn->nodes[1],ki);
	return kf;
}

KifElement* KifCode::KifParseOperationIn(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseOperationIn");
#endif
	//The first parameter is the operator
	KifInstruction* ki;
	if (kf->Action()==kifBLOCBOOLEAN)
		ki=(KifInstruction*)kf;
	else {
		KifElement* last;
		//In this case, we replace the previous last element in kf with this one
		if (kf->Action()==kifAFFECTATION || kf->Action()==kifINITIALISATION) {
			last=kf->InstructionRemoveLast();
			ki=new KifInstruction(this,kf);
			ki->instructions.push_back(last);
			ki->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
		}
		else {
			if (kf->Parent()!=NULL) {
				//In this case, we need to keep the full instruction			
				last=kf->Parent()->InstructionRemoveLast();
				ki=new KifInstruction(this,kf->Parent());
				ki->instructions.push_back(kf);
				kf->Addparent(ki);
				ki->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
			}
			else {
				last=kf->InstructionRemoveLast();
				ki=new KifInstruction(this,kf);
				if (last!=NULL)
					ki->instructions.push_back(last);
				ki->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
			}
		}
	}
	ki->action=kifGlobals->kifOperators[xn->nodes[0]->value];
	if (ki->action==kifNOTIN) {
		ki->action=kifIN;
		ki->negation=true;
	}

	KifBrowse(xn->nodes[1],ki);
	if (xn->nodes.size()==3) {
		KifElement* kfunc=NULL;
		string name=xn->nodes[2]->value;
		short id=KifNewId(name);
		kfunc=Getbasic(id,kf);
		//We have a WITH description
		if (kfunc==NULL) {
			string message=kifErrorStrings[178]+name;
			long line=GetCurrentLine(0,xn);
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
		if (kfunc->isFunction())
			ki->Add(kfunc);
		else {
			if (!kfunc->isCall()) {
				string message=KIFLIBSSTRINGS[30]+kifGlobals->kifIdString[kfunc->Name()]+"'";
				throw new KifRaiseError(message,current->name,current_start,current_end);
			}
			ki->Add(new KifInstance(this,NULL,kfunc->Name(),(KifVariableDeclaration*)kfunc));
		}
	}
	return ki;
}

KifElement* KifCode::KifParseSubFunc(x_node* xn,KifElement* kf) {
	string& name=xn->nodes[0]->nodes[0]->value;
	if (xn->nodes[0]->nodes[0]->nodes.size()!=1)
		name=xn->nodes[0]->nodes[0]->nodes[1]->value;
	short id=KifNewId(name);
	
	KifElement* declaration=kf->Declaration(id);	
	if (declaration==NULL)
		if CALLFUNCTION(kf)
			declaration=kifGlobals->KifBaseFunctionTest(id);

	if (declaration==NULL || declaration->Private()) {
		string message=KIFLIBSSTRINGS[30]+name+"'";
		throw new KifRaiseError(message,current->name,current_start,current_end);
	}

	return KifBrowse(xn->nodes[0],kf);
}

KifElement* KifCode::KifParseSubVar(x_node* xn,KifElement* kf) {
	string& name=xn->nodes[0]->nodes[0]->value;
	short id=KifNewId(name);
	KifElement* declaration=kf->Declaration(id);
	if (declaration==NULL || declaration->Private()) {
		string message=KIFLIBSSTRINGS[33]+name+"'";
		throw new KifRaiseError(message,current->name,current_start,current_end);
	}

	return KifBrowse(xn->nodes[0],kf);
}

//A variable is a complex structure involving: expression, crochets
KifElement* KifCode::KifParseVariable(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVariable");
#endif
	string& name=xn->nodes[0]->value;
	short id=KifNewId(name);
	KifElement* declaration=NULL;
	long line=GetCurrentLine(0,xn);
    if (kf->type==kifEnum) {        
        KifVariableDeclaration* var=Createstaticvariable(name,kf,xn);
        //then we create its value
        return new KifBaseInteger(this,var,((KifEnum*)kf)->values.size()-1);        
    }
    
	//specific case, this is itself...
	if (name==KIFLIBSSTRINGS[65]) {
		if (xn->nodes.size()!=1) {
			string message=KIFLIBSSTRINGS[66];
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
		kf->Add(new KifThis(this,NULL));
		return kf;
	}

	declaration=Getbasic(id,kf);
	
	if (declaration==NULL) {
		string lname=c_Tolower(name);
		short idlname=KifNewId(lname);
		declaration=Getbasic(idlname,kf);
		if (declaration==NULL || declaration->XIP()==false) {
			string message=KIFLIBSSTRINGS[33]+name+"'";
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
	}

	if (!declaration->Variable()) {
		if (xn->nodes.size()==1 && declaration->isFunction()) {
			KifElement* ke=new KifSelf(this,kf,declaration);
			ke->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
			return ke;
		}
		if (declaration->type!=kifVariable) {
			string message=KIFLIBSSTRINGS[33]+name+"'";
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
	}
	KifInstanceCommon* ki;
	if (xn->nodes.size()>1 && TestKifFunctions(xn->nodes[1]->token,true)) 
		ki=new KifInstanceFunction(this,kf,id,(KifVariableDeclaration*)declaration);
	else 
		ki=new KifInstance(this,kf,id,(KifVariableDeclaration*)declaration);

	if (declaration->type==kifFunctionDeclaration)
		((KifInstance*)ki)->Setinstancetype(kinFunction);
	else {
		if (declaration->type==kifFrameDeclaration) {
			KifElement* currentdeclaration=FindFrameForSequence(declaration,xn,-1);
			if (currentdeclaration==kstart)
				((KifInstance*)ki)->Setinstancetype(kinGlobal);
			else
				((KifInstance*)ki)->Setinstancetype(kinFrame);
		}
	}
	

	ki->Setstringvalue(xn->value);
	ki->Setline(linereference+GetCurrentLine(0,xn->nodes[0]),currentfile);
	kf->Initvalue(ki);
	KifElement* ret=NULL;
	for (size_t i=1;i<xn->nodes.size();i++) {
		KifElement* retloc=KifBrowse(xn->nodes[i],ki);
		if (retloc!=NULL)
			ret=retloc;
	}
	if (ret==NULL)
		ki->Setself();
	return ki;
}

KifElement* KifCode::KifParseIncrement(x_node* xn,KifElement* kf) {
	KifInstruction* ki=new KifInstruction(this,kf,kifNONE);
	ki->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	for (size_t i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],ki);	
	return ki;	
}

KifElement* KifCode::KifParseAffectation(x_node* xn,KifElement* kf) {
	KifInstruction* ki=new KifInstruction(this,kf,kifAFFECTATION);
	ki->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	for (size_t i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],ki);
	ki->BuildNumericalExpression(0);
	return ki;
}


//Hexa-decimal value
KifElement* KifCode::KifParseAXNumber(x_node* xn ,KifElement* kf) {
	string name=xn->value;
	KifElement* kv;
	kv=new KifInteger(this,kf,XLConvert(STR(name)));	
	kv->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	return kv;
}

KifElement* KifCode::KifParseANumber(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseANumber");
#endif
	string name=xn->value;
	KifElement* kv;	
	if (name.find(".")==-1)
		kv=new KifBaseInteger(this,kf,XLConvert(STR(name)));	
	else
		kv=new KifBaseFloat(this,kf,atof(STR(name)));
	kv->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	return kv;
}

KifElement* KifCode::KifParseABoolean(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseABoolean");
#endif
	if (xn->value==KIFLIBSSTRINGS[69]) {
		kf->Add(kifTRUE);
		return kifTRUE;
	}
	kf->Add(kifFALSE);
	return kifFALSE;
}

KifElement* KifCode::KifParseAString(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseAString");
#endif
	string name="";
	size_t i;
	//if (xn->token==KIFLIBSSTRINGS[70]) {
	//	for (i=0;i<xn->nodes.size();i++) {
	//		if (xn->nodes[i]->value[0]=='\\')
	//			name+=KifMetaCharacters(xn->nodes[i]->value);
	//		else
	//			name+=xn->nodes[i]->value;
	//	}
	//}
	//else {
	//	for (i=0;i<xn->nodes.size();i++)
	//			name+=xn->nodes[i]->value;
	//}
	string s;
	if (xn->token==KIFLIBSSTRINGS[70]) {
		for (i=1;i<xn->value.size()-1;i++) {
			if (xn->value[i]=='\\' && i<xn->value.size()-2) {
				s=xn->value[i];
				char c=xn->value[i+1];								
				if (c>='0' && c<='9') {
					s=c;
					i++;
					if (i<xn->value.size()-2) {
						c=xn->value[i+1];
						if (c>='0' && c<='9') {
							s+=c;					
							i++;
							if (i<xn->value.size()-2) {
								c=xn->value[i+1];
								if (c>='0' && c<='9') {								
									s+=c;
									i++;
									char nbchar=atoi(STR(s));
									name+=nbchar;
									s="";
								}
							}
						}
					}
					i--;
					if (s!="")
						name+=s;
				}
				else {
					s+=c;
					name+=KifMetaCharacters(s);
				}
				i++;

			}
			else
				name+=xn->value[i];
		}
	}
	else {
		for (i=1;i<xn->value.size()-1;i++)
			name+=xn->value[i];
	}
	
	KifString* kv=new KifBaseString(this,kf,name);
	kv->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	return kv;
}

KifElement* KifCode::FindFrameForSequence(KifElement* ktop,x_node* xn,short id) {
	KifType currenttype=ktop->type;
	KifElement* kbefore=ktop;
	if (currenttype==kifFrameDeclaration) {
		kbefore=ktop->Frame();
		currenttype=kbefore->type;
	}
	if (currenttype==kifInstructionSEQUENCE) {
		//we check if a variable of the same name was not already declared at a previous level...		
		int i=kifGlobals->stacking.size()-2;
		//We look for the first non SEQUENCE bloc, which might be embedded one into the others... 
		while (i>=0 && kifGlobals->stacking[i]->type==kifInstructionSEQUENCE) 
			i--;
		if (i>=0)
			kbefore=kifGlobals->stacking[i];
		if (kbefore!=NULL)  {
			//This level defines the type of this variable as a function or a domain related variable...
			if (id!=-1 && kbefore->Declaration(id)!=NULL) {
				long line=GetCurrentLine(0,xn);
				string message=KIFLIBSSTRINGS[72];
				message+=kifGlobals->kifIdString[id];
				message+=KIFLIBSSTRINGS[73];
				throw new KifRaiseError(message,current->name,line,xn->end);
			}
		}
	}
	return kbefore;
}

KifElement* KifCode::CreateVariable(x_node* xn,short id,KifElement* kf,KifInstructionType instype) {
#ifdef PROFILING
	profilingcall("Code::CreateVariable");
#endif
	KifElement* ktop=kifGlobals->Top();
	KifElement* var;
	long line=GetCurrentLine(0,xn);
	//If it is a global variable, instance is unique
	if (kf->Declaration(id)!=NULL) {
		string message=KIFLIBSSTRINGS[72];
		message+=kifGlobals->kifIdString[id];
		message+=KIFLIBSSTRINGS[73];
		throw new KifRaiseError(message,current->name,line,xn->end);
	}

	KifType currenttype=kifVOID;
	if (ktop!=NULL) {
		KifElement* kbefore=FindFrameForSequence(ktop,xn,id);
		currenttype=kbefore->type;
	}
	
	if (common==true && currenttype!=kifFrame) {
		string message=KIFLIBSSTRINGS[76];
		message+=kifGlobals->kifIdString[id];
		throw new KifRaiseError(message,current->name,line,xn->end);
	}
	
	if (constant)
		var=new KifStaticVariable(this,kf,id);
	else
		if (currenttype==kifVOID)
			var=new KifVariableDeclaration(this,kf,id);
		else
			//Otherwise, we keep a track of that specific frame
			if (currenttype==kifFrame) {
				if (common)
					var=new KifFrameDeclarationCommon(this,kf,id,ktop);
				else
					var=new KifFrameDeclaration(this,kf,id,ktop);
			}
			else
				var=new KifFunctionDeclaration(this,kf,id,ktop);
	
	var->Setprivate(prive);
	
	var->Setline(linereference+GetCurrentLine(0,xn),currentfile);
	if (xn->nodes.size()>=3) {
		for (int nxt=2;nxt<xn->nodes.size();nxt++) {
			if (xn->nodes[nxt]->token==KIFLIBSSTRINGS[29]) {
				string funcname=xn->nodes[nxt]->nodes[0]->value;
				short idf=KifNewId(funcname);
				KifElement* kfunc=NULL;
				kfunc=Getbasic(idf,kf);
				//We have a WITH description
				if (kfunc==NULL) {
					string message=kifErrorStrings[178]+funcname;				
					throw new KifRaiseError(message,current->name,line,xn->end);
				}
				if (kfunc->isFunction())
					var->Setfunction(kfunc);
				else {
					if (!kfunc->isCall()) {
						string message=KIFLIBSSTRINGS[30]+kifGlobals->kifIdString[kfunc->Name()]+"'";
						throw new KifRaiseError(message,current->name,current_start,current_end);
					}
					var->Setfunction(new KifInstance(this,NULL,kfunc->Name(),(KifVariableDeclaration*)kfunc));
				}
			}
			else {
				if (xn->nodes[nxt]->token!=KIFLIBSSTRINGS[79]) {
                    if (instype==kifENUM) {
                        KifInstruction ki(NULL,var,kifINITIALISATION);
                        ki.instructiontype=instype;
                        KifBrowse(xn->nodes[nxt],&ki);		                    
                    }
                    else {
                        KifInstruction* ki=new KifInstruction(this,var,kifINITIALISATION);
                        ki->Setline(linereference+GetCurrentLine(0,xn->nodes[nxt]),currentfile);
                        var->Setinitialisation(ki);
                        ki->instructiontype=instype;
                        KifBrowse(xn->nodes[nxt],ki);		
                        ki->BuildNumericalExpression(0);
                        //we cannot keep it as a value...
                        ki->instructiontype=kifNOTYPE;
                    }
				}
			}
		}
	}

	return var;
}

KifElement* KifCode::KifParseVarFrame(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarFrame");
#endif
	string& type=xn->nodes[0]->value;
	short id=KifNewId(type);
	string& name=xn->nodes[1]->value;
	short idname=KifNewId(name);
	KifElement* ke;
	ke=Getframe(id,kf);
	if (ke==NULL) {
		string message=KIFLIBSSTRINGS[80]+type+"'";
		long line=GetCurrentLine(0,xn);
		throw new KifRaiseError(message,current->name,line,xn->end);
	}
	KifElement* var=CreateVariable(xn,idname,kf,kifNOTYPE);
	((KifVariableDeclaration*)var)->value=ke;
	//In the case of a frame element defined within a frame, we add the postpone flag
	//which delays if necessary, the call to the _initial frame function for that variable frame...
	if (kf!=kstart && kf->type==kifFrame)
		var->Setpostpone(true);
	kf->Set(idname,var,NULL);

	return var;
}

KifElement* KifCode::KifParseVarSelf(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarSelf");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifSelf* kv=new KifSelf(this,var,kifNULL);	
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarCall(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarCall");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifCall* kv=new KifCall(this,var,NULL);	
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarDate(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarTime");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifDate* kv=new KifDate(this,var);	
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarTime(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarTime");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifTime* kv=new KifTime(this,var);	
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}


KifElement* KifCode::KifParseVarBit(x_node* xn ,KifElement* kf) {
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifBit* kv=new KifBit(this,var);	
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarBits(x_node* xn ,KifElement* kf) {
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifBits* kv=new KifBits(this,var);	
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}


KifElement* KifCode::KifParseVarFraction(x_node* xn ,KifElement* kf) {
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifFraction* kv=new KifFraction(this,var,0,1,true);	
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarKifKiF(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarKifKiF");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifKiF* kv=new KifKiF(this,var);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarLong(x_node* xn ,KifElement* kf) {
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifFULLINTEGER);
	KifLong* kv=new KifLong(this,var,0);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}


KifElement* KifCode::KifParseVarInt(x_node* xn ,KifElement* kf) {
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifFULLINTEGER);
	KifInteger* kv;
	kv=new KifBaseInteger(this,var);	
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarString(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarString");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifFULLSTRING);
	KifString* kv;
	kv=new KifBaseString(this,var);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarFloat(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarFloat");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifFULLFLOAT);
	KifFloat* kv;
	kv=new KifBaseFloat(this,var);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarBoolean(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarBoolean");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifBoolean* kv=new KifBoolean(this,var);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}
KifElement* KifCode::KifParseVarFile(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarFile");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifFile* kv=new KifFile(this,var);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarVector(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarVector");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifVector* kv;
	if (evaluation) {
		kv=Providevector();
		var->Add(kv);
	}
	else
		kv=new KifVector(this,var);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarEnum(x_node* xn ,KifElement* kf) {
    short id=KifNewId(xn->nodes[1]->value);
    KifElement* var=CreateVariable(xn,id,kf,kifENUM);
    kf->Set(id,var,NULL);
    return var;
}

KifElement* KifCode::KifParseVarList(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarVector");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifList* kv;
	if (evaluation) {
		kv=Providelist();
		var->Add(kv);
	}
	else
		kv=new KifList(this,var);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarMap(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarMap");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifMap* kv;
	if (evaluation) {
		kv=Providemap();
		var->Add(kv);
	}
	else
		kv=new KifMap(this,var);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarWeka(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarWeka");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifWeka* kv=new KifWeka(this,var,id);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarTree(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarIterator");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifTree* kv=new KifTree(this,var,kifNULL);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}


KifElement* KifCode::KifParseVarIterator(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarIterator");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifIterator* kv=new KifIterator(this,var,false);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarRIterator(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseVarRIterator");
#endif
	short id=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,id,kf,kifNOTYPE);
	KifIterator* kv=new KifIterator(this,var,true);
	kv->Setline(linereference+GetCurrentLine(0,xn->nodes[1]),currentfile);
	kf->Set(id,var,NULL);
	return var;
}

KifElement* KifCode::KifParseMultiDeclaration(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseDeclaration");
#endif
	string& type=xn->nodes[0]->value;
	bool oldprive=prive;
	bool oldcommon=common;
	bool oldconstant=constant;
	if (xn->nodes[0]->nodes.size() && xn->nodes[0]->nodes[0]->token==KIFLIBSSTRINGS[81]) {
		string& s=xn->nodes[0]->nodes[0]->nodes[0]->token;
		if (s==KIFLIBSSTRINGS[82]) {
			prive=true;
			if (xn->nodes[0]->nodes[0]->nodes.size()!=1)
				common=true;
		}
		else
			if (s==KIFLIBSSTRINGS[83])
				common=true;
		else
			if (s==KIFLIBSSTRINGS[84])
				constant=true;
		type=xn->nodes[0]->nodes[1]->value;
	}
	
	KifElement* element=NULL;
	int last=xn->nodes.size()-1;
	bool recall=false;
	
	if (xn->nodes[last]->token==KIFLIBSSTRINGS[79]) {
		x_node* xnew=new x_node();
		xnew->token=xn->nodes[0]->token;
		xnew->value=type;
		xn->nodes[last]->nodes.insert(xn->nodes[last]->nodes.begin(),xnew);
		recall=true;
	}

	if (kifGlobals->kifTypes.find(type)!=kifGlobals->kifTypes.end()) {
		KifParseElement kpe=kifGlobals->kifTypes[type];
		element=(this->*kpe)(xn,kf);
	}
	else
		if (kifGlobals->kifExternalTypes.find(type)!=kifGlobals->kifExternalTypes.end()) {
			KifExternalParse kext=kifGlobals->kifExternalTypes[type];
			element=(*kext)(this,xn,kf);
		}
		else
			KifParseVarFrame(xn,kf);
	
	if (recall)
		KifParseMultiDeclaration(xn->nodes[last],kf);
	prive=oldprive;
	common=oldcommon;
	constant=oldconstant;
	return element;
}


KifElement* KifCode::KifParseDeclaration(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseDeclaration");
#endif
	string& type=xn->nodes[0]->value;
	bool oldprive=prive;
	if (xn->nodes[0]->nodes[0]->token==KIFLIBSSTRINGS[82]) {
		prive=true;
		type=xn->nodes[0]->nodes[1]->value;
	}
	KifElement* element=NULL;
	if (kifGlobals->kifTypes.find(type)!=kifGlobals->kifTypes.end()) {
		KifParseElement kpe=kifGlobals->kifTypes[type];
		element=(this->*kpe)(xn,kf);
	}
	else
		if (kifGlobals->kifExternalTypes.find(type)!=kifGlobals->kifExternalTypes.end()) {
			KifExternalParse kext=kifGlobals->kifExternalTypes[type];
			element=(*kext)(this,xn,kf);
		}
		else
			KifParseVarFrame(xn,kf);
	prive=oldprive;
	return element;
}

KifElement* KifCode::KifParseList(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseList");
#endif
	//KifInstruction* kbloc=new KifInstruction(this,kf,kifBLOC);
    if (kf->type==kifEnum) {
        //In this case, we create for each value, a variable, whose value depends on its position
        //in the enumeration
        KifBrowse(xn->nodes[0],kf);
        if (xn->nodes.size()==2)
            KifBrowse(xn->nodes[1],kf);
        return kf;
    }
    
	KifInstruction kbloc(NULL,NULL,kifBLOC);
	kbloc.kifcode=this;
	KifBrowse(xn->nodes[0],&kbloc);
	KifElement* ke=kbloc.instructions[0];
	ke->BuildNumericalExpression(0);
	kf->Push(ke);
	if (xn->nodes.size()==2)
		KifBrowse(xn->nodes[1],kf);
	return ke;
}


KifElement* KifCode::KifParseValVector(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseValVector");
#endif
	int i;

	if (kf->Instructiontype()==kifENUM) {
        //In this case, each value will be declared as a variable
        KifEnum* ken=new KifEnum(this,kf->Parent());
        ken->Setline(linereference+GetCurrentLine(0,xn),currentfile);
        for (i=0;i<xn->nodes.size();i++)
            KifBrowse(xn->nodes[i],ken);
        return ken;
    }

	KifBaseVector* kvect=new KifBaseVector(this,kf);
	for (i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],kvect);
	kvect->evaluate=false;
	for (i=0;i<kvect->values.size();i++) {
		if (kvect->values[i]->type<kifString || kvect->values[i]->type>kifBoolean) {
			kvect->evaluate=true;
			break;
		}
	}

	return kvect;
}

KifElement* KifCode::KifParseDico(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseDico");
#endif
	KifInstruction kbloc(NULL,NULL,kifBLOC);
	kbloc.kifcode=this;

	//First the key
	KifBrowse(xn->nodes[0],&kbloc);
	KifElement* key=kbloc.instructions[0];
	key->BuildNumericalExpression(0);
	kbloc.instructions.clear();

	//then the value
	KifBrowse(xn->nodes[1],&kbloc);
	KifElement* val=kbloc.instructions[0];
	val->BuildNumericalExpression(0);
	
	KifBaseMap* kmap=(KifBaseMap*)kf;
	kmap->keys.push_back(key);
	kmap->values.push_back(val);
	if (xn->nodes.size()==3)
		KifBrowse(xn->nodes[2],kf);
	return kf;
}


KifElement* KifCode::KifParseValMap(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseValMap");
#endif
	
	KifBaseMap* kmap=new KifBaseMap(this,kf);
	for (int i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],kmap);
	return kmap;	
}

//label detection
KifElement* KifCode::KifParseLabel(x_node* xn,KifElement* kf) {
	string name=xn->nodes[0]->value;
	KifLabel lab;	
	lab.line=bnfkif->x_linenumber(xn->start+1);
	lab.fileindex=currentfile;
    lab.label=name;
	if (kifGlobals->kifLabels.find(name)!=kifGlobals->kifLabels.end()) {
		string message=KIFLIBSSTRINGS[87]+name+"'";
		throw new KifRaiseError(message,current->name,lab.line,xn->end);
	}
	kifGlobals->kifLabels[name]=lab;
	kifGlobals->kifLabelIdx[lab.line]=lab;
	return kifTRUE;
}

//Function creation
KifElement* KifCode::KifParseFunction(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseFunction");
#endif
	string name=xn->nodes[1]->value;
	short idname=KifNewId(name);
	long line=GetCurrentLine(0,xn);
	if (kifGlobals->KifBaseFunctionTest(idname)!=NULL && kifGlobals->KifConversion(name)==false) {		
		if (kifGlobals->kifMethods.find(idname)==kifGlobals->kifMethods.end()) {
			string message=KIFLIBSSTRINGS[88]+name+"'";
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
	}

	if (kf!=kstart) 
		kifDeclaredFunction[idname]=true;
	KifFunction* kfunc=NULL;
	KifElement* kprevious=kf->Declaration(idname);
	bool autorun=false;
	bool privatefunction=false;
	bool strictfunction=false;
	bool joinfunction=false;
	int last=xn->nodes.size()-1;

	//Two cases:
	//If this is the first implementation of that function OR NO predeclaration had been issued OR it is not a function, then we create a new function
	if (kprevious==NULL || kprevious->Predeclared()==false || kprevious->isFunction()==false) {
		if (kprevious!=NULL && kprevious->Alreadydeclared() && xn->nodes[last]->token==KIFLIBSSTRINGS[89])
			return kprevious;

		string typefunction;
		int protection=0;
		vector<x_node*>& xsub=xn->nodes[0]->nodes;
		int si=0;
		if (xsub[si]->value==KIFLIBSSTRINGS[90]) {
			joinfunction=true;
			si++;
		}

		if (xsub[si]->value==KIFLIBSSTRINGS[91]) {
			protection=1;
			si++;
		}
		
		if (xsub[si]->value==KIFLIBSSTRINGS[92]) {
			protection=2;
			si++;
		}
		
		if (xsub[si]->value==KIFLIBSSTRINGS[82]) {
			privatefunction=true;
			si++;
		}
		if (xsub[si]->value==KIFLIBSSTRINGS[94]) {
			strictfunction=true;
			si++;
		}
		if (xsub[si]->token==KIFLIBSSTRINGS[95])
			typefunction=xsub[si]->value;

		if (typefunction==KIFLIBSSTRINGS[96])
			kfunc=new KifPolynomial(this,kf,idname);			
		else
			if (typefunction==KIFLIBSSTRINGS[97])
				kfunc=new KifThread(this,kf,idname,joinfunction,protection);			
			else {
				kfunc=new KifFunction(this,kf,idname);
				if (typefunction==KIFLIBSSTRINGS[98])
					autorun=true;
			}
	}

	//If we already have an implementation for that function, either it is a predeclaration, then we simply use it
	//Or this predeclaration is NOT a function
	if (kprevious!=NULL) {		
		if (kprevious->isFunction()) {
			if (kprevious->Predeclared()) {
				kfunc=(KifFunction*)kprevious; //we use it
				kprevious=NULL;
				//We clear our argument list and declaration list
				kfunc->declarations.clear();
				kfunc->arguments.clear();
			}//else, we will add this new function to a previous declaration...
			else
				if (kprevious->isUsed()==true)
					kprevious=NULL;
				else //if it has been implemented in the mother frame already, then it should not be attached to that set of functions
					if (kf!=kprevious->Frame())
						kprevious=NULL;
		}
		else
			kprevious=NULL;
	}

	if (kfunc==NULL) {
		string message=KIFLIBSSTRINGS[99]+name+"'";
		throw new KifRaiseError(message,current->name,line,xn->end);
	}

	kfunc->privatefunction=privatefunction;
	kfunc->strict=strictfunction;
	kifGlobals->PushStack(kfunc);
	kfunc->Setline(linereference+GetCurrentLine(1,xn),currentfile);
	
	if (xn->nodes[last]->token==KIFLIBSSTRINGS[89]) {
		kfunc->choice=0;
		//we process the arguments
		KifBrowse(xn->nodes[2],kfunc);
		kfunc->choice=2;
	}
	else {
		if (xn->nodes[2]->token==KIFLIBSSTRINGS[101]) {
			kfunc->choice=1;
			KifBrowse(xn->nodes[2],kfunc);
		}
		else {
			kfunc->choice=0;
			KifBrowse(xn->nodes[2],kfunc);
			kfunc->choice=1;
			KifBrowse(xn->nodes[3],kfunc);
		}
	}
	
	if (kprevious!=NULL)
		kprevious->Addfunction(kfunc);
	else
		kf->Set(idname,kfunc,NULL);

	kifGlobals->PopStack();
	if (autorun) {
		if (kfunc->arguments.size()!=0) {
			string message=KIFLIBSSTRINGS[102]+name+"'";
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
		if (kf==kstart || kf->type!=kifFrame)
			new KifCallFunctionGeneral(this,kf,idname,kfunc);
		kfunc->autorun=true;
	}
	return kfunc;
}


KifElement* KifCode::KifParseFrame(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifParseFrame");
#endif
	//We create a frame
	//The name is the next parameter
	string name=xn->nodes[0]->value;
	short idname=KifNewId(name);
	KifFrame* kframe=NULL;
	KifElement* ke;
	ke=kf->Declaration(idname);

	if (ke==NULL)
		kframe=new KifFrame(this,kf,idname);
	else {
		if (ke->type==kifFrame) {
			kframe=(KifFrame*)ke;
			//if (kframe->declared==false)
			//	kframe=NULL;
		}
	}
	if (kframe==NULL) {
		long line=GetCurrentLine(0,xn);
		string message=KIFLIBSSTRINGS[103]+name;
		throw new KifRaiseError(message,current->name,line,xn->end);
	}

	if (xn->nodes[1]->token==KIFLIBSSTRINGS[89]) {
		kframe->declared=true;
		kf->Set(kframe->name,kframe,NULL);
		return kframe;
	}


	kifGlobals->PushStack(kframe);
	kframe->Setline(linereference+GetCurrentLine(0,xn),currentfile);

	//If it is a sub-frame definition
	if (kf->type==kifFrame && kstart!=kf)
		//We copy all our declarations in it
		//These declarations, will be replaced by local ones if necessary
		kf->Sharedeclaration(kframe);
	//We then record this new Frame in our instructions list
	kifAllTypes[kframe->name]=true;
	//We also store it at the TOP level, so that others can have access to it...
	kstart->Set(kframe->name,kframe,NULL);
	if (xn->nodes[1]->token==KIFLIBSSTRINGS[29]) {
		string funcname=xn->nodes[1]->nodes[0]->value;			
		short idf=KifNewId(funcname);
		KifElement* kfunc=NULL;
		kfunc=Getbasic(idf,kf);
		//We have a WITH description
		if (kfunc==NULL) {
			long line=GetCurrentLine(0,xn);
			string message=kifErrorStrings[178]+funcname;				
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
		if (kfunc->isFunction())
			kframe->Setfunction(kfunc);
		else {
			if (!kfunc->isCall()) {
				string message=KIFLIBSSTRINGS[30]+kifGlobals->kifIdString[kfunc->Name()]+"'";
				throw new KifRaiseError(message,current->name,current_start,current_end);
			}
			kframe->Setfunction(new KifInstance(this,NULL,kfunc->Name(),(KifVariableDeclaration*)kfunc));
		}

		KifBrowse(xn->nodes[2],kframe);
	}
	else
		KifBrowse(xn->nodes[1],kframe);
	kifGlobals->PopStack();
	return kframe;
}


KifElement* KifCode::KifBrowse(x_node* xn,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifBrowse");
#endif
    if (xn==NULL)
        return NULL;

	KifElement* ke=NULL;

	if (kifGlobals->kifFunctions.find(xn->token)!=kifGlobals->kifFunctions.end())
		ke=(this->*kifGlobals->kifFunctions[xn->token])(xn,kf);
	else {
		for (size_t i=0;i<xn->nodes.size();i++) {
			KifElement* s=KifBrowse(xn->nodes[i],kf);
			if (s!=NULL)
				ke=s;
		}
	}
	return ke;
}

void KifCode::Loadfile(string n,KifCode* loader) {
#ifdef PROFILING
	profilingcall("Code::Loadfile");
#endif

	string filename=Getfilename(n);
	x_readfile xf(STR(filename));
	int previous=currentfile;
	if (DebugPtr()->kifdebugging==true) {
		ifstream fkif(STR(filename),modeOuverture);
		string s;
		int lref=1;
		while (!fkif.eof()) {
			getline(fkif,s);
			if (s!="") {
				string strim=s;
				Trim(strim);
				if (strim!="")
					DebugPtr()->kiftabledebug[currentfile][lref]=s;
			}
			lref++;
		}		
	}
    DebugPtr()->AddFilename(n,this);
	Load(&xf);
	currentfile=previous;
	Setloader(loader);
}

void KifCode::Loadfile(ifstream* f,string n,long pos,KifCode* loader) {
#ifdef PROFILING
	profilingcall("Code::Loadfile");
#endif
	
	string filename=Getfilename(n);
	if (f->fail() ) {
		delete f;
		string compiled=filename+KIFLIBSSTRINGS[107];
		f=new ifstream(STR(compiled),modeOuverture);		
		if (f->fail()) {
			delete f;
			string message=KIFLIBSSTRINGS[108];
			message+=filename;		
			throw new KifRaiseError(message,filename,-1,-1);
		}
		delete f;
		f=NULL;
	}


	string s;
    DebugPtr()->AddFilename(filename,this);
	long l=1;
	delete f;
	if (DebugPtr()->kifdebugging==true) {
		f=new ifstream(STR(filename),modeOuverture);
		f->seekg(pos);
		while (!f->eof()) {
			getline(*f,s);
			if (s!="") {
				string strim=s;
				Trim(strim);
				if (strim!="")
					DebugPtr()->kiftabledebug[currentfile][linereference+l]=s;
			}
			l+=1;
		}		
		delete f;
	}

	Setloader(loader);
	f=new ifstream(STR(filename),modeOuverture);
	x_readfile xf(f,STR(filename),pos);
	f->seekg(pos);
	Load(&xf);
	delete f;
	
	int i,j;
	kifGlobals->basegarbage=kifGlobals->garbage.size();
	kifGlobals->trigger+=kifGlobals->basegarbage;
	for (j=0;j<locals.size();j++) {
        i=locals[j];
		if (kifGlobals->garbage[i]!=NULL) {			
			if (kifGlobals->garbage[i]->Basevalue()) {
				kifGlobals->garbage[i]->reference=30000;
				kifGlobals->garbage[i]->popped=staticval;
			}
			else
				if (kifGlobals->garbage[i]->reference==0)
					kifGlobals->garbage[i]->reference=1;
		}
	}
}

KifElement* KifCode::EvaluateVector(KifElement* dom,string s) {
	
	x_readstring xr(s);
	current=&xr;	
	
	xr.loadtoken();
    bnfkif->initialize(&xr);	
    
	string lret;
    x_node* xn=new x_node;
    char res=bnfkif->m_valvector(lret,&xn);
	string message;
	if (res!=1)
		return Returnerror(kifErrorStrings[244]);	
	
	KifElement* kret=kifNULL;
	if (dom==NULL)
		dom=kstart;
	evaluation=true;
	try {
		kret=KifBrowse(xn,dom);
	}
	catch(KifRaiseError* m) {		
		delete xn;
		string message=m->message;
		delete m;
		evaluation=false;
		return Returnerror(message);
	}
	evaluation=false;	
	return kret;
}

KifElement* KifCode::EvaluateMap(KifElement* dom,string s) {
	
	x_readstring xr(s);
	current=&xr;	
	
	xr.loadtoken();
    bnfkif->initialize(&xr);	
    
	string lret;
    x_node* xn=new x_node;	
    char res=bnfkif->m_valmap(lret,&xn);
	string message;
	if (res!=1)		
		return Returnerror(kifErrorStrings[244]);
	
	KifElement* kret=kifNULL;
	if (dom==NULL)
		dom=kstart;
	evaluation=true;
	try {
		kret=KifBrowse(xn,dom);
	}
	catch(KifRaiseError* m) {		
		delete xn;
		string message=m->message;
		delete m;
		evaluation=false;
		return Returnerror(message);
	}
	evaluation=false;
	KifElement* kmap=kret->Map(dom);
	kret->Resetpopped();
	kret->Resetreference();	
	return kmap;
}

KifElement* KifCode::Evaluate(KifElement* kf,string sbase,bool commandline) {
#ifdef PROFILING
	profilingcall("Code::Evaluate");
#endif
	string s;
	if (commandline==true) {
		kifGlobals->PushStack(kstart);
		s=KIFLIBSSTRINGS[109];
	}
	s+=sbase;

	x_readstring xr(s);
	current=&xr;	
	xr.loadtoken();
	bnfkif->baseline=1;
	x_node* xn=bnfkif->x_parsing(&xr,FULL,false);
	string message;
	if (xn==NULL) {
		if (bnfkif->errornumber!=-1) {
			string msg(KIFLIBSSTRINGS[110]);
			msg+=bnfkif->x_errormsg(bnfkif->errornumber);
			return Returnerror(msg);	
		}
		return Returnerror(kifErrorStrings[244]);	
	}

	KifElement* kret=kifNULL;
	try {
		kret=KifBrowse(xn,kf);
	}
	catch(KifRaiseError* m) {		
		if (commandline==true)
			kifGlobals->PopStack();
		delete xn;
		string message=m->message;
		delete m;
		return Returnerror(message);
	}	

	if (commandline==true) {
		kifGlobals->PopStack();
        for (int i=0;i<kifGlobals->garbage.dernier;i++) {
            if (kifGlobals->garbage[i]!=NULL) {                
                if (kifGlobals->garbage[i]->Basevalue()) {
					kifGlobals->garbage[i]->reference=30000;
                    kifGlobals->garbage[i]->popped=staticval;
				}
				else
					if (kifGlobals->garbage[i]->reference==0)
						kifGlobals->garbage[i]->reference=1;
            }
        }
    }
	return kret;
}

Exported void KifCode::Setbaseline(int i) {
	bnfkif->baseline=i;
}

void KifCode::Load(x_reading* xr) {
#ifdef PROFILING
	profilingcall("Code::Load");
#endif 
	current=xr;
	KifConst* var=(KifConst*)kstart->Declaration(idpath);
	KifVector* vargs=(KifVector*)var->value;
	vargs->Push(new KifBaseString(this,NULL,xr->name));
	xr->loadtoken();
	x_reading* previous=bnfkif->fx;
	x_node* xn=bnfkif->x_parsing(xr,FULL);
	if (xn==NULL) {
		long line=bnfkif->lineerror;
		string message=KIFLIBSSTRINGS[111];
		if (xr->error()) {
			message=KIFLIBSSTRINGS[112];
			message+=xr->name;
		}
		else
			if (bnfkif->errornumber!=-1)
				message+=bnfkif->x_errormsg(bnfkif->errornumber);
			else
				message+=bnfkif->labelerror;		
		throw new KifRaiseError(message,current->name,line,bnfkif->lineerror);
	}	
#ifndef _DEBUG
	try {
#endif
		bool addto=addtolocal;
		addtolocal=true;
		kifGlobals->PushStack(kstart);
		KifBrowse(xn,kstart);
		kifGlobals->PopStack();
		addtolocal=addto;
#ifndef _DEBUG
	}
	catch(KifRaiseError* m) {		
		delete xn;
		bnfkif->fx=previous;
		throw m;
	}
#endif
	bnfkif->fx=previous;
	delete xn;
}


KifElement* KifCode::Find(string& name) {
#ifdef PROFILING
	profilingcall("Code::Search");
#endif
	hmap<short,KifElement*>::iterator iter=kstart->declarations.begin();
	for (;iter!=kstart->declarations.end();iter++) {
		string val=kifGlobals->kifIdString[iter->first];
		val=c_Tolower(val);
		if (val==name)
			return iter->second;
	}
	return NULL;
}

KifElement* KifCode::Findraw(string& name) {
#ifdef PROFILING
	profilingcall("Code::Searchraw");
#endif
	hmap<short,KifElement*>::iterator iter=kstart->declarations.begin();
	for (;iter!=kstart->declarations.end();iter++) {
		string val=kifGlobals->kifIdString[iter->first];
		if (val==name)
			return iter->second;
	}
	return NULL;
}

KifElement* KifSearch(string name) {
	if (kifGlobals==NULL)
		return NULL;
	KifElement* ret=NULL;
	for (int i=0;i<kifGlobals->kifCodeVector.size();i++) {
		if (kifGlobals->kifCodeVector[i]!=NULL) {
			ret=kifGlobals->kifCodeVector[i]->Find(name);
			if (ret!=NULL)
				return ret;
		}
	}
	return NULL;
}


KifElement* KifSearchraw(string name) {
	if (kifGlobals==NULL)
		return NULL;
	KifElement* ret=NULL;
	for (int i=0;i<kifGlobals->kifCodeVector.size();i++) {
		if (kifGlobals->kifCodeVector[i]!=NULL) {
			ret=kifGlobals->kifCodeVector[i]->Findraw(name);
			if (ret!=NULL)
				return ret;
		}
	}
	return NULL;
}


KifElement* KifCode::Execute(KifFunction* func,vector<KifElement*>& parameters) {
#ifdef PROFILING
	profilingcall("Code::Execute");
#endif
	string met="__"+func->name;
	short idmet=KifNewId(met);
	KifCallFunctionLocal kfunc(this,idmet,(KifFunction*)func);
	hmap<short,KifElement*> stacks;
	kfunc.SetParameters(parameters,&stacks);
	kifGlobals->PushStack(kstart);
	kifGlobals->PushStack(&kfunc);
	if (DebugPtr()->kifdebugging==true)
		DebugPtr()->kifinfunction=true;
	KifElement* ret=kfunc.Execute(kifNULL,kstart);
	kifGlobals->PopStack();
	kifGlobals->PopStack();
	kfunc.Resetdeclaration(stacks);
	return ret;
}

void KifGlobalThread::Clean() {
	garbage_collector(basegarbage);
}


void KifCode::FinalGarbage() {
	hmap<short,KifElement*>::iterator it;
	KifElement* ke;
	for (it=kstart->declarations.begin();it!=kstart->declarations.end();it++) {
		ke=it->second;
		ke->popped=false;
		ke->threadowner=kifNoOwner;
	}

	kstart->popped=false;
	kstart->Resetreference();
	for (int i=0;i<locals.size();i++) {
		int j=locals[i];
		if (kifGlobals->garbage[j]!=NULL) {
			if (kifGlobals->garbage[j]!=NULL) {
				delete kifGlobals->garbage[j];
				kifGlobals->garbage[j]=NULL;
			}
		}
	}
	//kifGlobals->garbage_collector(0);
}


void KifGlobalThread::Garbage() {
	if (Trigger()==true)
		garbage_collector(basegarbage);
}


KifCode::~KifCode() {
	if (session==false) {
		kifGlobals->WaitForTheEnd();
		ThreadLock::Clean();
	}
    
    ThreadLock _lock(kifLoading);
	kifGlobals->kifCodeVector[idkifmain]=NULL;
    if (function!=NULL) {
        function->instructions.clear();
        delete function;
    }

	hmap<string,int>::iterator it;
	for (it=kifGlobals->kifCodePaths.begin();it!=kifGlobals->kifCodePaths.end();it++) {
		if (it->second==idkifmain)
			it->second=-1;
	}
	delete bnfkif;
	if (idkifmain==0) {
		//If it is the first module, then we clean also our libraries...
		hmap<string,KifExternalTerminateModule>::iterator term;
		for (term=kifGlobals->kifTerminateLibraries.begin();term!=kifGlobals->kifTerminateLibraries.end();term++)
			(*term->second)(this);
		hmap<short,KifPredefined*>::iterator ifunc;
		for (ifunc=kifGlobals->kifBaseFunctions.begin();ifunc!=kifGlobals->kifBaseFunctions.end();ifunc++)
			delete ifunc->second;

		for (ifunc=kifGlobals->kifBaseTreeFunctions.begin();ifunc!=kifGlobals->kifBaseTreeFunctions.end();ifunc++)
			delete ifunc->second;
		KifFinalClean();
	}
	else
		FinalGarbage();
}




Exported bool Garbaging() {
    return kifGlobals->garbaging;
}

//Generic GARBAGE: clean all instances
void KifGlobalThread::garbage_collector(int from) {
    
	int idthread=kifGlobals->GetThreadid();
	//Only the main thread can do garbaging
	if (idthread!=-1)
		return;
	
    if (garbagefunction!=NULL)
		garbagefunction->Execute(kifNULL,NULL);

    garbaging=true;
	KifElement* ke;
	size_t it;

	ThreadLock _lock(kifGarbaging);

	//We only free elements from that specific thread, to avoid weird issues
	for (it=from;it<garbage.size();it++) {
		ke=garbage[it];
		if (ke==NULL || ke->popped)
			continue;
		if (ke->reference==0 && (ke->threadowner==kifNoOwner || ke->threadowner==idthread))
			ke->Clean();
	}

	cursormax=from;
	cursor=-1;
	cursorvector=-1;
	//We need first to avoid problems with vectors
	for (it=0;it<vectorlist.size();it++) {
		//we clear all unused vectors of each possible value that might be stored in
		ke=vectorlist[it];
		if (ke->reference==0 && (ke->threadowner==kifNoOwner || ke->threadowner==idthread)) {
			if (ke->popped)
				continue;
			if (cursorvector==-1)
				cursorvector=it;
			vectorlist[it]->Clear();
		}
	}

	cursormap=-1;
	for (it=0;it<maplist.size();it++) {
		ke=maplist[it];
		//we clear all unused maps of each possible value that might be stored in
		if (ke->reference==0 && (ke->threadowner==kifNoOwner || ke->threadowner==idthread)) {
			if (ke->popped)
				continue;
			if (cursormap==-1)
				cursormap=it;
			ke->Clear();
		}
	}

	cursorlist=-1;
	//We need first to avoid problems with lists
	for (it=0;it<listlist.size();it++) {
		//we clear all unused lists of each possible value that might be stored in
		ke=listlist[it];
		if (ke->reference==0  && (ke->threadowner==kifNoOwner || ke->threadowner==idthread)) {
			if (ke->popped)
				continue;
			if (cursorlist==-1)
				cursorlist=it;
			ke->Clear();
		}
	}

	int nbfree=garbage.taille-garbage.dernier-1;
	for (it=from;it<garbage.size();it++) {
		ke=garbage[it];
		if (ke==NULL) {
			nbfree++;
			if (cursor==-1)
				cursor=it;
			cursormax=it;
			continue;
		}
		
		if (ke->reference<=0 && (ke->threadowner==kifNoOwner || ke->threadowner==idthread)) {
			if (ke->popped)
				continue;
			nbfree++;
			delete ke;
			if (cursor==-1)
				cursor=it;
			cursormax=it;
			garbage[it]=NULL;
		}
	}
	//if nbfree is less than 12 percent of the garbage, we need to increase it
	nbfree<<=3;
	if (nbfree<garbage.taille) {
		garbage.ajuste(garbage.taille*2);
		cursormax=garbage.taille-10;
	}
    garbaging=false;
}



int KifGlobalThread::Add(KifElement* ke) {
#ifdef PROFILING
	profilingcall("Code::Add");
#endif	
	ke->threadowner=kifGlobals->GetThreadid();
	Used(ke->threadowner,kifGarbageOwner);
	int current;
	ThreadLock _lock(kifGarbage);
	if (cursor<=cursormax) {		
		garbage[cursor]=ke;
		current=cursor;
		cursor++;
		while (cursor<=cursormax && garbage[cursor]!=NULL) cursor++;
	}
	else {
		cursor=garbage.size();
		current=cursor;
		//if we are going to have an extension of the garbage
		//then we can delay the next garbage collecting
		if (current>=garbage.taille)
			cursormax=(garbage.taille*2)-1;		
		garbage.push_back(ke,garbage.taille);
	}
	return current;		
}

bool KifGlobalThread::Trigger() {
	if (!nogarbage && cursor>=cursormax)
		return true;
	return false;
}
	

//Specific GARBAGE: we clean all KifElement elements pointing to a XIP temporary information
void KifGlobalThread::garbage_xip() {
	ThreadLock _lock(kifGarbage);
	for (size_t it=0;it<garbage.size();it++) {
		KifElement* ke=garbage[it];
		if (ke!=NULL)
			ke->CleanXIP();
	}
}

void KifGlobalThread::garbage_xml() {
	for (size_t it=0;it<garbage.size();it++) {
		KifElement* ke=garbage[it];
		if (ke!=NULL)
			ke->CleanXML();
	}
}


void KifCode::AddArguments(vector<string>& args) {
	KifConst* var=(KifConst*)kstart->Declaration(idargs);
	KifVector* vargs=(KifVector*)var->value;
	for (int i=0;i<args.size();i++)
		vargs->Push(new KifBaseString(this,NULL,args[i]));
}


void KifCode::SetArguments() {
	KifConst* var=(KifConst*)kstart->Declaration(idargs);
	KifVector* vargs=(KifVector*)var->value;
	for (int i=0;i<kifGlobals->arguments.size();i++)
		vargs->Push(new KifBaseString(this,NULL,kifGlobals->arguments[i]));
}

void KifCode::InitArguments(vector<string>& args) {
    kifGlobals->arguments.clear();
	for (int i=0;i<args.size();i++) 
		kifGlobals->arguments.push_back(args[i]);
	KifConst* var=(KifConst*)kstart->Declaration(idargs);
	KifVector* vargs=(KifVector*)var->value;
    vargs->Clear();
	for (int i=0;i<kifGlobals->arguments.size();i++)
		vargs->Push(new KifBaseString(this,NULL,kifGlobals->arguments[i]));
}


bool KifGlobalThread::KifConversion(string n) {
#ifdef PROFILING
	profilingcall("Code::KifConversion");
#endif
	if (kifConversionNames.find(n)!=kifConversionNames.end())
		return true;
	return false;
}


KifElement* KifCode::KifInitVariable(x_node* xn ,KifElement* kf) {
#ifdef PROFILING
	profilingcall("Code::KifInitVariable");
#endif
	short name=KifNewId(xn->nodes[1]->value);
	KifElement* var=CreateVariable(xn,name,kf,kifNOTYPE);
	return var;
}


KifVariableDeclaration* KifCode::Createvariable(string name,KifElement* kf,x_node* xn) {
	short idname=KifNewId(name);
    if (kstart->declarations.find(idname)!=kstart->declarations.end()) {
        string message=KIFLIBSSTRINGS[72];
        message+=name;
        message+=KIFLIBSSTRINGS[114];
        if (xn==NULL)
            throw new KifRaiseError(message,current->name,-1,-1);
        else
            throw new KifRaiseError(message,current->name,GetCurrentLine(0,xn),xn->end);
    }
	KifVariableDeclaration* var=new KifVariableDeclaration(this,NULL,idname);
	kstart->Set(idname,var,NULL);
	return var;
}

KifStaticVariable* KifCode::Createstaticvariable(string name,KifElement* kf,x_node* xn) {
	short idname=KifNewId(name);
    if (kstart->declarations.find(idname)!=kstart->declarations.end()) {
        string message=KIFLIBSSTRINGS[72];
        message+=name;
        message+=KIFLIBSSTRINGS[114];
        if (xn==NULL)
            throw new KifRaiseError(message,current->name,-1,-1);
        else
            throw new KifRaiseError(message,current->name,GetCurrentLine(0,xn),xn->end);
    }
	KifStaticVariable* var=new KifStaticVariable(this,kf,idname);
        
	kstart->Set(idname,var,NULL);
	return var;
}

Exported void KifCode::Setloader(KifCode* k) {
	if (k==NULL)
		return;
	if (loader==NULL)
		loader=k;
	KifElement* kifvar=kstart->Declaration(idloader);
	KifKiF* kifkif=(KifKiF*)kifvar->Returnvalue();
	kifkif->id=k->idkifmain;
}


Exported long KifCode::GetCurrentLine(int i,x_node* xn) {
	if (bnfkif->fx==NULL)
		return -1;

	current_start=bnfkif->x_linenumber(xn->start+i);
	current_end=xn->end;
	return current_start;
}



KifCode::KifCode(int trig,void* d,ostream* o) {	
	if (kifGlobals==NULL) {
		kifGlobals=new KifGlobalThread;
		kifGlobals->Initialisation(trig,this);
	}
    gGlobals=kifGlobals;
	addtolocal=false;
    debugfunction=NULL;
    endofexecution=false;
    function=NULL;
	loader=NULL;
	ObjectInitialisation(trig);
    kstart->declarations[idpop]=new KifPredefined(this,P_ONE|P_TWO|P_THREE,&PrePop,idpop);
    
	data=d;		
	if (o==NULL)
		os=&cout;
	else
		os=o;
}

Exported KifString* KifCode::Providestring(string& z) {
	KifString* ke=kifGlobals->Providestring(z);
	ke->kifcode=this;
	return ke;
}

Exported KifString* KifCode::Providestringraw(string z) {
	KifString* ke=kifGlobals->Providestring(z);
	ke->kifcode=this;
	return ke;
}

Exported KifInteger* KifCode::Provideinteger(long val) {
	KifInteger* ke=kifGlobals->Provideinteger(val);
	ke->kifcode=this;
	return ke;
}
Exported KifFloat* KifCode::Providefloat(double val) {
	KifFloat* ke=kifGlobals->Providefloat(val);
	ke->kifcode=this;
	return ke;
}

Exported KifVector* KifCode::Providevector() {
	KifVector* ke=kifGlobals->Providevector();
	ke->kifcode=this;
	return ke;
}

Exported KifMap* KifCode::Providemap() {
	KifMap* ke=kifGlobals->Providemap();
	ke->kifcode=this;
	return ke;
}
Exported KifList* KifCode::Providelist() {
	KifList* ke=kifGlobals->Providelist();
	ke->kifcode=this;
	return ke;
}

KifString* KifGlobalThread::Providestring(string& str) {
	int id=kifGlobals->GetThreadid();
	Used(id,kifStringOwner);
	int i=cursorstring;
	KifString* ke;
	ThreadLock _lock(kifPString);	
	for (;cursorstring<stringlist.size();cursorstring++) {
		ke=stringlist[cursorstring];
		if (ke->reference<=0 && (ke->threadowner==id || ke->threadowner==kifNoOwner)) {
			if (ke->popped)
				continue;
			ke->reference=0;
			ke->value=str;
			ke->Putpopped(1);
			ke->threadowner=id;
			return stringlist[cursorstring++];
		}
	}
	
	for (cursorstring=0;cursorstring<i;cursorstring++) {
		ke=stringlist[cursorstring];
		if (ke->reference<=0 &&  (ke->threadowner==id || ke->threadowner==kifNoOwner)) {
			if (ke->popped)
				continue;
			ke->reference=0;
			ke->value=str;
			ke->Putpopped(1);
			ke->threadowner=id;
			return stringlist[cursorstring++];
		}
	}		
	
	sizestring<<=1;
	stringlist.resize(sizestring);
	for (i=cursorstring;i<sizestring;i++) {
		ke=new KifString(NULL,NULL,"");
		//ke->kifcode=this;
		ke->Putpopped(0);
		ke->idgarbage=i;
		stringlist[i]=ke;
	}
	ke=stringlist[cursorstring];
	ke->value=str;
	ke->threadowner=id;
	ke->Putpopped(1);
	cursorstring++;
	return ke;
}

KifString* KifGlobalThread::Providestringraw(string str) {
	int id=kifGlobals->GetThreadid();
	Used(id,kifStringOwner);
	int i=cursorstring;
	KifString* ke;
	ThreadLock _lock(kifPString);
	for (;cursorstring<stringlist.size();cursorstring++) {
		ke=stringlist[cursorstring];
		if (ke->reference<=0 && (ke->threadowner==id || ke->threadowner==kifNoOwner)) {
			if (ke->popped)
				continue;
			ke->Putpopped(1);
			ke->reference=0;
			ke->value=str;
			ke->threadowner=id;
			return stringlist[cursorstring++];
		}
	}
	
	for (cursorstring=0;cursorstring<i;cursorstring++) {
		ke=stringlist[cursorstring];
		if (ke->reference<=0 &&  (ke->threadowner==id || ke->threadowner==kifNoOwner)) {
			if (ke->popped)
				continue;
			ke->Putpopped(1);
			ke->reference=0;
			ke->value=str;
			ke->threadowner=id;
			return stringlist[cursorstring++];
		}
	}		
	
	sizestring<<=1;
	stringlist.resize(sizestring);
	for (i=cursorstring;i<sizestring;i++) {
		ke=new KifString(NULL,NULL,"");
		//ke->kifcode=this;
		ke->Putpopped(0);
		ke->idgarbage=i;
		stringlist[i]=ke;
	}
	ke=stringlist[cursorstring];
	ke->value=str;
	ke->threadowner=id;
	ke->Putpopped(1);
	cursorstring++;
	return ke;
}

KifInteger* KifGlobalThread::Provideinteger(long val) {
	int id=kifGlobals->GetThreadid();
	Used(id,kifIntegerOwner);
	int i=cursorinteger;
	KifInteger* ke;
	ThreadLock _lock(kifPInteger);
	for (;cursorinteger<integerlist.size();cursorinteger++) {
		ke=integerlist[cursorinteger];			
		if (ke->reference<=0  && (ke->threadowner==id || ke->threadowner==kifNoOwner)) {
			if (ke->popped)
				continue;
			ke->Putpopped(1);
			ke->reference=0;
			ke->value=val;
			ke->threadowner=id;
			return integerlist[cursorinteger++];
		}
	}
	
	for (cursorinteger=0;cursorinteger<i;cursorinteger++) {
		ke=integerlist[cursorinteger];
		if (ke->reference<=0 &&  (ke->threadowner==id || ke->threadowner==kifNoOwner)) {
			if (ke->popped)
				continue;
			ke->Putpopped(1);
			ke->reference=0;
			ke->value=val;
			ke->threadowner=id;
			return integerlist[cursorinteger++];
		}
	}		
	sizeinteger<<=1;
	integerlist.resize(sizeinteger);
	for (i=cursorinteger;i<sizeinteger;i++) {
		ke=new KifInteger(NULL,NULL,0);
		//ke->kifcode=this;
		ke->Putpopped(0);
		ke->idgarbage=i;
		integerlist[i]=ke;
	}
	ke=integerlist[cursorinteger];
	ke->value=val;
	ke->threadowner=id;
	ke->Putpopped(1);
	cursorinteger++;
	return ke;
}

KifFloat* KifGlobalThread::Providefloat(double val) {
	int id=kifGlobals->GetThreadid();
	Used(id,kifFloatOwner);
	int i=cursorfloat;
	KifFloat* kfloat;
	ThreadLock _lock(kifPFloat);
	for (;cursorfloat<floatlist.size();cursorfloat++) {
		kfloat=floatlist[cursorfloat];
		if (kfloat->reference<=0 && (kfloat->threadowner==id || kfloat->threadowner==kifNoOwner)) {
			if (kfloat->popped)
				continue;
			kfloat->Putpopped(1);
			kfloat->reference=0;
			kfloat->value=val;
			kfloat->threadowner=id;
			return floatlist[cursorfloat++];
		}
	}
	for (cursorfloat=0;cursorfloat<i;cursorfloat++) {
		kfloat=floatlist[cursorfloat];
		if (kfloat->reference<=0 && (kfloat->threadowner==id || kfloat->threadowner==kifNoOwner)) {
			if (kfloat->popped)
				continue;
			kfloat->Putpopped(1);
			kfloat->reference=0;
			kfloat->value=val;
			kfloat->threadowner=id;
			return floatlist[cursorfloat++];
		}
	}		
	sizefloat<<=1;
	floatlist.resize(sizefloat);
	for (i=cursorfloat;i<sizefloat;i++) {
		kfloat=new KifFloat(NULL,NULL,0);
		//kfloat->kifcode=this;
		kfloat->idgarbage=i;
		kfloat->Putpopped(0);
		floatlist[i]=kfloat;
	}
	kfloat=floatlist[cursorfloat];
	kfloat->value=val;
	kfloat->threadowner=id;
	kfloat->Putpopped(1);
	cursorfloat++;
	return kfloat;
}

KifVector* KifGlobalThread::Providevector() {
	int id=kifGlobals->GetThreadid();
	Used(id,kifVectorOwner);
	int i=cursorvector;
	KifVector* kvect;
	ThreadLock _lock(kifPVector);
	
	for (;cursorvector<vectorlist.size();cursorvector++) {
		kvect=vectorlist[cursorvector];
		if (kvect->reference<=0 && (kvect->threadowner==id || kvect->threadowner==kifNoOwner)) {
			if (kvect->popped)
				continue;
			kvect->Putpopped(1);
			kvect->reference=0;
			kvect->threadowner=id;
			kvect->Clear();
			return vectorlist[cursorvector++];
		}
	}
	for (cursorvector=0;cursorvector<i;cursorvector++) {
		kvect=vectorlist[cursorvector];
		if (kvect->reference<=0 && (kvect->threadowner==id || kvect->threadowner==kifNoOwner)) {
			if (kvect->popped)
				continue;
			kvect->reference=0;
			kvect->Putpopped(1);
			kvect->threadowner=id;
			kvect->Clear();
			cursorvector++;
			return vectorlist[cursorvector-1];
		}
	}		
	sizevector<<=1;
	vectorlist.resize(sizevector);
	for (i=cursorvector;i<sizevector;i++) {
		kvect=new KifVector(NULL,NULL);
		//kvect->kifcode=this;
		kvect->Putpopped(0);
		kvect->idgarbage=i;
		vectorlist[i]=kvect;
	}
	kvect=vectorlist[cursorvector];
	kvect->threadowner=id;
	kvect->Putpopped(1);
	cursorvector++;
	return kvect;
}

KifList* KifGlobalThread::Providelist() {
	int id=kifGlobals->GetThreadid();
	Used(id,kifListOwner);
	int i=cursorlist;
	KifList* klist;
	
	ThreadLock _lock(kifPList);
	for (;cursorlist<listlist.size();cursorlist++) {
		klist=listlist[cursorlist];
		if (klist->reference<=0  && (klist->threadowner==id || klist->threadowner==kifNoOwner)) {
			if (klist->popped)
				continue;
			klist->Putpopped(1);
			klist->reference=0;
			klist->threadowner=id;
			klist->Clear();
			return listlist[cursorlist++];
		}
	}
	for (cursorlist=0;cursorlist<i;cursorlist++) {
		klist=listlist[cursorlist];
		if (klist->reference<=0 && (klist->threadowner==id || klist->threadowner==kifNoOwner)) {
			if (klist->popped)
				continue;
			klist->Putpopped(1);
			klist->reference=0;
			klist->threadowner=id;
			klist->Clear();
			cursorlist++;
			return listlist[cursorlist-1];
		}
	}		
	sizelist<<=1;
	listlist.resize(sizelist);
	for (i=cursorlist;i<sizelist;i++) {
		klist=new KifList(NULL,NULL);
		//klist->kifcode=this;
		klist->idgarbage=i;
		klist->Putpopped(0);
		listlist[i]=klist;
	}
	klist=listlist[cursorlist];
	klist->threadowner=id;
	klist->Putpopped(1);
	cursorlist++;
	return klist;
}

KifMap* KifGlobalThread::Providemap() {
	int id=kifGlobals->GetThreadid();
	Used(id,kifMapOwner);
	int i=cursormap;
	KifMap* kmap;
	
	ThreadLock _lock(kifPMap);
	for (;cursormap<maplist.size();cursormap++) {
		kmap=maplist[cursormap];
		if (kmap->reference<=0 && !kmap->popped && (kmap->threadowner==id || kmap->threadowner==kifNoOwner)) {
			if (kmap->popped)
				continue;
			kmap->Putpopped(1);
			kmap->reference=0;
			kmap->threadowner=id;
			kmap->Clear();
			return maplist[cursormap++];
		}
	}
	for (cursormap=0;cursormap<i;cursormap++) {
		kmap=maplist[cursormap];
		if (kmap->reference<=0 && !kmap->popped && (kmap->threadowner==id || kmap->threadowner==kifNoOwner)) {
			if (kmap->popped)
				continue;
			kmap->Putpopped(1);
			kmap->reference=0;
			kmap->threadowner=id;
			kmap->Clear();
			return maplist[cursormap++];
		}
	}		
	sizemap<<=1;
	maplist.resize(sizemap);
	for (i=cursormap;i<sizemap;i++) {
		kmap=new KifMap(NULL,NULL);
		//kmap->kifcode=this;
		kmap->idgarbage=i;
		kmap->Putpopped(0);
		maplist[i]=kmap;
	}
	kmap=maplist[cursormap];
	kmap->threadowner=id;
	kmap->Clear();
	kmap->Putpopped(1);
	cursormap++;
	return kmap;
}
	
Exported short KifNewId(string n) {
    return kifGlobals->KifNewId(n);
}

Exported short KifStringId(string n) {
    return kifGlobals->KifStringId(n);
}
    
Exported string KifIdString(short n) {
    return kifGlobals->KifIdString(n);
}



void KifFinalClean() {
	if (kifGlobals!=NULL) {
		delete kifGlobals;
		kifGlobals=NULL;
	}
}


void KifCleaning() {
	kifGlobals->stacking.clear();
	kifGlobals->threadstacks.clear();
	kifGlobals->linestack.clear();
	kifGlobals->filestack.clear();
	kifGlobals->namestack.clear();
	kifGlobals->parentthreads.clear();
	kifGlobals->threadalloc=0;
	KifFinalClean();
}



int KifTestPath(string path) {
	if (kifGlobals->kifCodePaths.find(path)==kifGlobals->kifCodePaths.end() || kifGlobals->kifCodePaths[path]==-1)
		return -1;
	return kifGlobals->kifCodePaths[path];
}

void SetKifPath(string path,int id) {
	char localpath[1024];
#ifdef WIN32
	_fullpath(localpath,STR(path),1024);
#else
	realpath(STR(path),localpath);
#endif
	kifGlobals->kifCodePaths[localpath]=id;
}

int KifLoadin(KifCode* kifcode,string path) {
	int previous=kifcode->currentfile;
	ifstream* fkif;
	if (DebugPtr()->kifdebugging==true) {
		fkif=new ifstream(STR(path),modeOuverture);
		string s;
		int lref=1;
		while (!fkif->eof()) {
			getline(*fkif,s);
			if (s!="") {
				string strim=s;
				Trim(strim);
				if (strim!="")
					DebugPtr()->kiftabledebug[kifcode->currentfile][lref]=s;
			}
			lref++;
		}		
		delete fkif;
	}

    DebugPtr()->AddFilename(path,kifcode);

	x_reading* precurrent=kifcode->current;
	int from=kifcode->kstart->InstructionSize();
	fkif=new ifstream(STR(path),modeOuverture);
	kifcode->bnfkif->baseline=1;
	kifcode->Loadfile(fkif,path,0,NULL);
	SetKifPath(path,kifcode->idkifmain);
	for (int i=from;i<kifcode->kstart->instructions.size();i++) {
		if (kifcode->kstart->instructions[i]->Basevalue()) {
			kifcode->kstart->instructions[i]->reference=30000;
			kifcode->kstart->instructions[i]->popped=staticval;	
		}
		else
			if (kifcode->kstart->instructions[i]->reference==0)
				kifcode->kstart->instructions[i]->reference=1;				
	}
	if (kifGlobals->error.find(-1)!=kifGlobals->error.end())
		return -1;

	kifcode->current=precurrent;
	kifcode->currentfile=previous;
	return kifcode->idkifmain;
}

Exported KifCode* KifCode::Newinstance() {
	return new KifCode(kifGlobals->trigger,data,os);
}


Exported int KifCode::KifLoad(vector<string>& paths,bool debugkif) {
	//On initialise une valeur par defaut du declencheur 
	//de garbage, pour que celui-ci puisse s'effectuer
	//a chaque fin d'execution
	KifCode* kifcode=Newinstance();
    bool deb=DebugPtr()->kifdebugging;
    DebugPtr()->kifdebugging=false;
	kifcode->SetArguments();
	register int i;
	for (i=0;i<paths.size();i++) {
		string fname=Getfilename(paths[i]);
		ifstream* fkif=new ifstream(STR(fname),modeOuverture);
		try {
			kifcode->bnfkif->baseline=1;
			kifcode->Loadfile(fkif,fname,0,this);
			SetKifPath(fname,kifcode->idkifmain);
		}
		catch(KifRaiseError* m) {
			cerr<<STR(m->message)<<KIFLIBSSTRINGS[117]<<m->left<<Endl;
			delete m;
			return -1;
		}	
	}
	
	kifcode->Run();
    DebugPtr()->kifdebugging=deb;
	if (kifGlobals->error.find(-1)!=kifGlobals->error.end())
		return -1;
	return kifcode->idkifmain;
}

int KifCode::KifLoadPath(string& path,bool debugkif) {
	char localpath[1024];
#ifdef WIN32
	_fullpath(localpath,STR(path),1024);
#else
	realpath(STR(path),localpath);
#endif

    ThreadLock _lock(kifLoading);
	int ikif=KifTestPath(localpath);
	if (ikif==-1) {
		vector<string> paths;
		paths.push_back(localpath);
		ikif=KifLoad(paths,debugkif);
        if (ikif>=0)
            kifGlobals->kifCodeVector[ikif]->debugfunction=debugfunction;
	}
	return ikif;
}


Exported string KifExecute(int ikif,string name,vector<string>& parameters,ostringstream* os,bool debugkif) {
	KifCode* kifcode=kifGlobals->kifCodeVector[ikif];
	if (kifcode==NULL)
		return "";

	ostream* old_os=kifcode->os;
	if (os!=NULL)
		kifcode->os=os;
	KiFSetdebug(debugkif);

	vector<KifElement*> params;
	char buf[10];
	for (int i=0;i<parameters.size();i++) {
		KifElement* ke=kifGlobals->Providestring(parameters[i]);
		string varname=KIFLIBSSTRINGS[118];
		sprintf_s(buf,100,"%d",i);
		varname+=buf;
		short idvarname=KifNewId(varname);
		KifVariableDeclaration* kvar=new KifVariableDeclaration(kifcode,NULL,idvarname);
		kvar->value=ke;
		KifInstance* ki=new KifInstance(kifcode,NULL,idvarname,kvar);
		ki->Setstringvalue(varname);
		ki->value=ke;
		params.push_back(ki);
	}

	string valeur;

	KifElement* proc_ke=KifSearchraw(name);
	if (proc_ke==NULL || proc_ke->type!=kifFunction) {
		valeur=kifErrorStrings[178];
		valeur+=name;
		return valeur;
	}

	KifFunction* proc=(KifFunction*)proc_ke;
	KifElement* resultat=kifcode->Execute(proc,params);

	if (Error()) {
		valeur=resultat->String();
		return valeur;
	}

	valeur=resultat->String();
	kifGlobals->Clean();
	kifcode->os=old_os;
	return valeur;	
}

Exported void KifDelete(bool session,int ipar) {
    KifCode* kifcode;
    {
        ThreadLock _lock(kifLoading);
        kifcode=kifGlobals->kifCodeVector[ipar];
        if (kifcode==NULL)
            return;
        if (session==true)
            kifcode->session=true;
    }
    
    delete kifcode;
}

Exported void KifSetSize(int threshold) {
	kif_declencheur=threshold;
	kifGlobals->trigger=kif_declencheur;
}


Exported KifType KifAddNewType(string name,KifExternalParse func,bool error) {
    return kifGlobals->KifAddNewType(name,func,error);
}

Exported KifType KifNewType() {
	kifGlobals->kifLastType++;
	return (KifType)kifGlobals->kifLastType;
}

//void GetXNKeys(map<string,unsigned short>& keys, x_node* xn) {
//	if (keys.find(xn->token)==keys.end())
//		keys[xn->token]=keys.size();
//	
//	if (xn->value!="" && keys.find(xn->value)==keys.end())
//		keys[xn->value]=keys.size();
//	
//	for (int i=0;i<xn->nodes.size();i++)
//		GetXNKeys(keys,xn->nodes[i]);
//}				  
//
//void AfficheXN(x_node* xn) {
//		cout<<STR(xn->token)<<" --> "<<STR(xn->value)<<endl;
//	
//	for (int i=0;i<xn->nodes.size();i++)
//		AfficheXN(xn->nodes[i]);
//}				  
//
//void SaveXNKeys(ofstream& f,map<string,unsigned short>& keys, x_node* xn) {
//	if (xn->nodes.empty())
//		f<<"@"<<keys[xn->token]<<"$";
//	else
//		f<<"@"<<keys[xn->token]<<"#";
//	if (xn->value=="")
//		f<<0;
//	else
//		f<<keys[xn->value];
//	
//	if (xn->nodes.empty())
//		return;
//	
//	f<<"{";
//	for (int i=0;i<xn->nodes.size();i++)
//		SaveXNKeys(f,keys,xn->nodes[i]);
//	f<<"}";
//}		  
//	
//void SaveXN(string filename,x_node* xn) {
//	map<string,unsigned short> keys;
//	filename+=".kif";
//	ofstream f(STR(filename),ios::binary);
//	f<<"kif:\r";
//	f<<"@JekiffeKiFcommeunouf@\r";
//	keys[filename]=1;
//	GetXNKeys(keys,xn);
//	f<<keys.size()<<"\r";
//	map<string,unsigned short>::iterator it;
//	for (it=keys.begin();it!=keys.end();it++)
//		f<<it->second<<" "<<STR(it->first)<<"\r";
//	f<<"*\r";
//	SaveXNKeys(f,keys, xn);
//}
//
//x_node* ReadXN(ifstream& f,vector<string>& keys,bool error) {
//	if (f.eof()) {
//		error=true;
//		return NULL;
//	}
//	char c=f.get();
//	if (f.eof() || error==true || c=='}')
//		return NULL;
//	if (c!='@') {
//		error=true;
//		return NULL;
//	}
//	string k;
//	short key,val;
//	x_node* xn;
//
//	f>>key;
//	f>>c;
//	f>>val;
//	if (c=='$') {
//		xn=new x_node();
//		xn->set(keys[key],keys[val]);
//		return xn;
//	}
//	else
//		if (c!='#') {
//			error=true;
//			return NULL;
//		}
//	f>>c;	
//	if (f.eof() || c!='{') {
//		error=true;
//		return NULL;
//	}
//	
//	xn=new x_node();
//	xn->set(keys[key],keys[val]);
//	
//	x_node* xsub=xn;
//	while (xsub!=NULL) {
//		xsub=ReadXN(f,keys,error);
//		if (xsub!=NULL)
//			xn->nodes.push_back(xsub);
//	}
//	
//	if (error) {
//		delete xn;
//		return NULL;
//	}
//	
//	return xn;
//}
//	
//x_node* LoadXN(string filename) { 
//	vector<string> keys;
//	keys.push_back("");
//	filename+=".kif";
//	ifstream f(STR(filename),ios::binary);
//	char buff[1000];
//	f.getline(buff,1000,'\r');
//	if (!strcmp(buff,"kif:"))
//		f.getline(buff,1000,'\r');
//	if (strcmp(buff,"@JekiffeKiFcommeunouf@"))
//		return NULL;
//	
//	char* pos;
//	short key;
//	size_t sz;
//	f>>sz;
//	keys.reserve(sz+1);
//	while (sz!=0) {
//		keys.push_back("");
//		sz--;
//	}
//	f.getline(buff,1000,'\r');
//	while (!f.eof() && buff[0]!='*') {
//		pos=strchr(buff,' ');
//		if (pos!=NULL) {
//			*pos=0;
//			key=atoi(buff);
//			keys[key]=pos+1;
//		}
//		f.getline(buff,1000,'\r');
//	}
//
//	if (f.eof())
//		return NULL;
//	bool error=false; 
//	return ReadXN(f,keys,error);
//}
//	
//bool Compiled(string base) {
//	string compiled=base+".kif";
//	int stinit=-1;
//	int stcible=-1;
//	struct stat scible;
//	struct stat sinit;
//	FILE* init=fopen((char*)base.c_str(),"r");
//	FILE* cible=fopen((char*)compiled.c_str(),"r");
//	if (init!=NULL) {
//		if (cible!=NULL) {
//#if  defined(WIN32) | defined(APPLE) | defined(XIPFSTAT64)
//			stinit=fstat(init->_file,&sinit);
//			stcible=fstat(cible->_file,&scible);
//#else
//			stinit=fstat(init->_fileno,&sinit);
//			stcible=fstat(cible->_fileno,&scible);
//#endif
//			fclose(cible);
//		}
//		fclose(init);
//	}
//	else
//		if (cible!=NULL) {
//			fclose(cible);
//			return true;
//		}
//	if (stinit!=-1 && stcible!=-1 && scible.st_mtime > sinit.st_mtime)
//		return true;
//	return false;
//}
//	

