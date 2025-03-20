/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the KiF library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : KiF
Version    : 1.00
filename   : kifsys.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language: System methods
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
#ifdef WIN32
#include "Windows.h"
#include <io.h>
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"

#include "osdir.h"

#ifdef WIN32
#define Exporting __declspec(dllexport)
#define DUP _dup
#define DUP2 _dup2
#define FILENO _fileno
#else
#include <unistd.h>
#define FILENO fileno
#define DUP dup
#define DUP2 dup2
#endif

short KifNewId(string n);
string KifIdString(short n);

#define STR(x) (char*)x.c_str()
bool DirectoryCreation(char* noms, int pos);
//Replace Sys by your own definition

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType Sys_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class KifSys;
//This typedef defines a type "SysMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (KifSys::*SysMethod)(KifElement*, KifDomain*, KifCallFunction*, int);


//---------------------------------------------------------------------------------------------------------------------

class KifSys : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, SysMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;


	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	string lastcommand;
	//---------------------------------------------------------------------------------------------------------------------
	KifSys(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, Sys_type) {
		//Do not forget your variable initialisation
		lastcommand = "";
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return Sys_type;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* idx, KifElement* domain) {
		if (idx != NULL && !idx->isConst()) {
			lastcommand = idx->String();
			string val = kval->String();
#ifdef WIN32
			lastcommand += "=";
			lastcommand += val;
			_putenv(STR(lastcommand));
#else
			setenv(lastcommand.c_str(), val.c_str(), 1);
#endif

		}
		return true;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
		if (idx != NULL && !idx->isConst()) {
			lastcommand = idx->String();
			char* rep = getenv(lastcommand.c_str());
			if (rep == NULL)
				return kifcode->Providestringraw("");
			return kifcode->Providestringraw(rep);
		}
		return this;
	}

	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return this;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}


	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCommand(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		//0 is the first parameter and so on...
		KifElement* kcmd = callfunc->Evaluatethread(0, domain, idthread);
		string filename;
		if (callfunc->Size() == 2)
			filename = callfunc->Evaluatethread(1, domain, idthread)->String();
		lastcommand = kcmd->String();
		if (lastcommand != "") {
			if (filename != "") {
				int o = DUP(1);

				FILE* stream = fopen(STR(filename), "w");
				if (stream == NULL)
					return kifcode->Returnerror("SYS(001): Cannot open output file");
				DUP2(FILENO(stream), 1);

				system(lastcommand.c_str());
				fflush(stdout);
				fclose(stream);
				DUP2(o, 1);
				stream = fopen(STR(filename), "r");
				string content;
				char buffer[2049];
				size_t nb;
				while (!feof(stream)) {
					nb = fread(buffer, 1, 2048, stream);
					buffer[nb] = 0;
					content += buffer;
				}
				fclose(stream);
				return kifcode->Providestring(content);
			}
			else
				system(lastcommand.c_str());
		}
		//you may return any value of course...
		return kifTRUE;
	}

	KifElement* MethodisDirectory(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		string dirName_in = callfunc->Evaluatethread(0, domain, idthread)->String();
#ifdef WIN32
		DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return kifFALSE;  //something is wrong with your path!

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			return kifTRUE;   // this is a directory!

#else
		struct stat st;
		if (stat(STR(dirName_in), &st) == 0)
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			return kifTRUE;
#endif
		return kifFALSE;    // this is not a directory!
	}

	KifElement* MethodEnv(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		//0 is the first parameter and so on...
		KifElement* kname = callfunc->Evaluatethread(0, domain, idthread);
		lastcommand = kname->String();
		if (callfunc->Size() == 1) {
			if (lastcommand != "") {
				char* rep = getenv(lastcommand.c_str());
				if (rep == NULL)
					return kifcode->Providestringraw("");
				return kifcode->Providestringraw(rep);
			}
			//you may not return any value of course...
			return kifNULL;
		}
		else {
			KifElement* kvalue = callfunc->Evaluatethread(1, domain, idthread);
			if (lastcommand != "") {
				string val = kvalue->String();
#ifdef WIN32
				lastcommand += "=";
				lastcommand += val;
				_putenv(STR(lastcommand));
#else
				setenv(lastcommand.c_str(), val.c_str(), 1);
#endif 			
				return kifTRUE;
			}
		}
		//you may return any value of course...
		return kifFALSE;
	}

	KifElement* MethodCreateDirectory(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		KifElement* kpath = callfunc->Evaluatethread(0, domain, idthread);
		string path = kpath->String();
		if (DirectoryCreation(STR(path), 0) == false)
			return kifFALSE;
		return kifTRUE;
	}

	KifElement* MethodFileInfo(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		KifElement* kpath = callfunc->Evaluatethread(0, domain, idthread);
		string filename = kpath->String();
#ifdef WIN32
		struct _stat scible;
		int stcible = _stat(STR(filename), &scible);
#else
		struct stat scible;
		int stcible = stat(STR(filename), &scible);
#endif
		if (stcible >= 0) {
			KifElement* size = kifcode->Provideinteger(scible.st_size);
			KifElement* change = new KifDate(kifcode, NULL, scible.st_mtime);
			KifElement* adate = new KifDate(kifcode, NULL, scible.st_atime);
			KifElement* cdate = new KifDate(kifcode, NULL, scible.st_ctime);
			kpath = kifcode->Providestring(filename);
			KifMap* mp = Selectmap(callfunc, contextualpattern);

			mp->Push("size", size);
			mp->Push("date", change);
			mp->Push("access", adate);
			mp->Push("change", cdate);
			mp->Push("pathname", kpath);
			if ((scible.st_mode & S_IFMT) == S_IFDIR)
				mp->Push("directory", kifTRUE);
			else
				mp->Push("directory", kifFALSE);
			return mp;
		}
		return kifNULL;
	}

	KifElement* MethodListDirectory(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		KifElement* kpath = callfunc->Evaluatethread(0, domain, idthread);
		string path = kpath->String();
		oslink::directory dir(path);
		if (contextualpattern->type == kifFloat || contextualpattern->type == kifInteger || contextualpattern->type == kifLong) {
			long i = 0;
			while (dir) {
				i++;
				dir.next();
			}
			return kifcode->Provideinteger(i);
		}

		KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
		while (dir) {
			if (kvect->type == kifVectorString)
				((KifVectorString*)kvect)->values.push_back(dir.next());
			else
				kvect->Push(kifcode->Providestringraw(dir.next()));
		}
		return kvect;
	}

	KifElement* MethodRealPath(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		KifElement* kpath = callfunc->Evaluatethread(0, domain, idthread);
		char localpath[4096];
		string path = kpath->String();
#ifdef WIN32
		_fullpath(localpath, STR(path), 4096);
#else
		realpath(STR(path), localpath);
#endif
		return kifcode->Providestringraw(localpath);
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)
			return (this->*linkedmethods[name])(contextualpattern, domain, callfunc, idthread);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	string String() {
		return lastcommand;
	}

	KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		string sa = a->String();
		string sb = b->String();
		sa += sb;
		return kifcode->Providestring(sa);
	}

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> KifSys::kifexportedmethods;
map<short, SysMethod>  KifSys::linkedmethods;
map<string, string> KifSys::infomethods;
hmap<short, bool> KifSys::methodassigned;

static KifSys* local;

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifSysElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	local = new KifSys(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, SysMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	KifSys::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	KifSys::linkedmethods[idname] = func;
	KifSys::methodassigned[idname] = true;
	KifSys::infomethods[name] = infos;
}

static bool Createstatickifsys(KifCode* kifcode) {
	string varname("kifsys");
	short idname = KifNewId(varname);
	if (!kifcode->gGlobals->kconstants->declarations.check(idname)) {
		//First we create a variable
		KifVariableDeclaration* var = kifcode->Createstaticvariable(varname);
		//then we create its value
		local = new KifSys(kifcode, var);
		varname = "_sys";
		idname = KifNewId(varname);
		if (!kifcode->gGlobals->kconstants->declarations.check(idname)) {
			var = kifcode->Createstaticvariable(varname);
			local = new KifSys(kifcode, var);
		}
		return true;
	}
	return false;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifsys(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}

		kifcode->KifInitLib();
		///The new type will be: Sys. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifSysElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: KifSys)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifSysElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("sys", CreatekifSysElement);
		if (!Createstatickifsys(kifcode))
			return false;

		Sys_type = res;

		return true;
	}

	Exporting bool kifsys_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifsys(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("command", &KifSys::MethodCommand, P_ONE | P_TWO, "command(string com,string outputfile): execute the command 'com'. outputfile is optional and can be used to redirect output to a file.");
		MethodInitialization("isdirectory", &KifSys::MethodisDirectory, P_ONE, "isdirectory(string path): return 'true' if path is a directory");
		MethodInitialization("createdirectory", &KifSys::MethodCreateDirectory, P_ONE, "createdirectory(string path): create the directory of path 'path'");
		MethodInitialization("mkdir", &KifSys::MethodCreateDirectory, P_ONE, "mkdir(string path): create the directory of path 'path'");
		MethodInitialization("listdirectory", &KifSys::MethodListDirectory, P_ONE, "listdirectory(string path): return the content of the directory 'path'");
		MethodInitialization("ls", &KifSys::MethodListDirectory, P_ONE, "ls(string path): return the content of the directory 'path'");
		MethodInitialization("realpath", &KifSys::MethodRealPath, P_ONE, "realpath(path relpath): return the actual path matching 'realpath'");
		MethodInitialization("fileinfo", &KifSys::MethodFileInfo, P_ONE, "fileinfo(string pathname): returns the info of the file 'pathname'");
		MethodInitialization("env", &KifSys::MethodEnv, P_ONE | P_TWO, "env(string var)|env(string var,string val): return or set the content of the environment variable 'var'");
		Createstatickifsys(kifcode);

		kifGlobals->kifAllFunctions[Sys_type] = KifSys::methodassigned;
		kifGlobals->kifAllInformation[Sys_type] = KifSys::infomethods;

		return true;
	}

	Exporting bool kifsys_Terminate(KifCode* kifcode) {
		KifSys::kifexportedmethods.clear();
		KifSys::linkedmethods.clear();
		KifSys::methodassigned.clear();
		KifSys::infomethods.clear();
		return true;
	}
}





