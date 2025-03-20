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
filename   : kifpython.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language: Python methods
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
#include <sys/stat.h>
#include <stdlib.h>
#include <Python.h>

#include "kif.h"
#include "kifcontainer.h"
#include "kifversion.h"

#if PY_MINOR_VERSION==4
#define Py_ssize_t int
#define OLDPYTHON
#endif

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

//Replace Python by your own definition

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType Python_type;

static bool init_python = false;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class KifPython;
//This typedef defines a type "PythonMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (KifPython::*PythonMethod)(KifElement*, KifDomain*, KifCallFunction*, int idthread);

static KifElement* tokif(KifCode* kifcode, PyObject* po) {
	if (PyBool_Check(po) == 1) {
		if (po == Py_True)
			return new KifBoolean(kifcode, NULL, true);
		return new KifBoolean(kifcode, NULL, false);
	}

	if (PyInt_Check(po) == 1) {
		long l = PyLong_AsLong(po);
		return kifcode->Provideinteger(l);
	}

	if (PyLong_Check(po) == 1) {
		KIFLONG l = PyLong_AsLong(po);
		return new KifLong(kifcode, NULL, l);
	}

	if (PyFloat_Check(po) == 1) {
		double d = PyFloat_AsDouble(po);
		return kifcode->Providefloat(d);
	}

	if (PyString_Check(po) == 1) {
		char* s = PyString_AsString(po);
		return kifcode->Providestringraw(s);
	}

	PyObject* pelement;
	KifElement* e;
	if (PyList_Check(po) == 1) {
		KifVector* kvect = new KifVector(kifcode, NULL);
		Py_ssize_t tail = PyList_GET_SIZE(po);
		for (Py_ssize_t i = 0; i < tail; i++) {
			//Puis chacun des objets, kj est l'index de l'element
			pelement = PyList_GetItem(po, i);
			e = tokif(kifcode, pelement);
			if (e == kifRAISEERROR) {
				kvect->Release();
				return kifRAISEERROR;
			}
			kvect->Push(e);
		}
		return kvect;
	}

	if (PyDict_Check(po) == 1) {
		KifMap* kmap = new KifMap(kifcode, NULL);
		Py_ssize_t pos = 0;
		KifElement* k;
		PyObject* key;
		string kval;
		while (PyDict_Next(po, &pos, &key, &pelement)) {
			if (key != NULL && pelement != NULL) {
				k = tokif(kifcode, key);
				if (k == kifRAISEERROR) {
					kmap->Release();
					return kifRAISEERROR;
				}
				e = tokif(kifcode, pelement);
				if (e == kifRAISEERROR) {
					kmap->Release();
					return kifRAISEERROR;
				}
				kval = k->String();
				kmap->Push(kval, e);
				k->Release();
			}
		}
		return kmap;
	}

#ifdef HAVE_WCHAR_H
	if (PyUnicode_Check(po)) {
		Py_ssize_t sz = PyUnicode_GetSize(po);
		Py_UNICODE* ibuff = PyUnicode_AsUnicode(po);
		string s;
		for (int i = 0; i < sz; i++)
			s += s_unicode_to_utf8(ibuff[i]);
		return kifcode->Providestringraw(s);
	}
#else
	if (PyUnicode_Check(po)) {
		Py_ssize_t sz = PyUnicode_GET_DATA_SIZE(po);
		const char* sbuff = PyUnicode_AS_DATA(po);
		string s;
		for (Py_ssize_t i = 0; i < sz; i++) {
			if (sbuff[i])
				s += sbuff[i];
		}
		return kifcode->Providestring(s);
	}
#endif

	PyObject* perr = PyObject_Str(po);
	if (perr != NULL) {
		char* s = PyString_AsString(perr);
		e = kifcode->Providestringraw(s);
		Py_DECREF(perr);
		return e;
	}

	return kifRAISEERROR;
}

static PyObject* topython(KifElement* resultat) {
	if (resultat->type == kifBoolean) {
		if (resultat == kifTRUE)
			return Py_True;
		return Py_False;
	}

	if (resultat->type == kifInteger || resultat->type == kifLong) {
		long l = resultat->Integer();
		return PyInt_FromLong(l);
	}

	if (resultat->type == kifFloat || resultat->type == kifFraction) {
		double d = resultat->Float();
		return PyFloat_FromDouble(d);
	}

	PyObject* pcourant;
	PyObject* liste;

	if (resultat->aVectorContainer() || resultat->type == kifList || resultat->type == kifTable) {
		int i = 0;
		liste = PyList_New(0);
		KifIteration* itr = (KifIteration*)resultat->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			pcourant = topython(itr->IteratorValue());
			PyList_Insert(liste, i, pcourant);
			Py_DECREF(pcourant);
			i++;
		}
		itr->Release();
		return liste;
	}

	agnostring value;
	if (resultat->aMapContainer()) {
		liste = PyDict_New();
		KifIteration* itr = (KifIteration*)resultat->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			pcourant = topython(itr->IteratorValue());
			value = itr->IteratorKeyString();
			PyDict_SetItemString(liste, STR(value), pcourant);
			Py_DECREF(pcourant);
		}
		itr->Release();
		return liste;
	}

	value = resultat->String();
#ifdef OLDPYTHON
	if (value.isutf8())
		return PyUnicode_DecodeUTF8(value.c_str(), value.size(), NULL);
#else
	if (value.isutf8())
		return PyUnicode_FromString(value.c_str());
#endif
	return PyString_FromString(value.c_str());
}

static string python_error_string() {
	string err;
	PyObject *ptype = NULL;
	PyObject *pvalue = NULL;
	PyObject *ptraceback = NULL;
	PyErr_Fetch(&ptype, &pvalue, &ptraceback);
	if (pvalue != NULL) {
		PyObject* perr = PyObject_Str(pvalue);
		err = PyString_AsString(perr);
		Py_DECREF(perr);
		Py_XDECREF(pvalue);
	}
	if (ptype != NULL)
		Py_XDECREF(ptype);
	if (ptraceback != NULL)
		Py_XDECREF(ptraceback);

	return err;
}

class KifPython : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone	
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, PythonMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;


	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	string pythonfilename;
	PyObject* pModule;
	//---------------------------------------------------------------------------------------------------------------------
	KifPython(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, Python_type) {
		//Do not forget your variable initialisation
		pythonfilename = "";
		pModule = NULL;
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return Python_type;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		return true;
	}

	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifPython* kperso = new KifPython(kifcode, kp);
		return kperso;
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
	KifElement* MethodSetpath(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		if (!init_python) {
			Py_Initialize();
			init_python = true;
		}

		string path;
		stringstream code;
		code << "import sys\n";
#ifdef WIN32
		string local("\\");
		string with("\\\\");
#endif
		for (int i = 0; i < callfunc->Size(); i++) {
			//0 is the first parameter and so on...
			path = callfunc->Evaluate(i, domain)->String();
#ifdef WIN32
			path = s_replacestring(path, local, with);
			code << "sys.path.append('" << path << "')\n";
#else
			code << "sys.path.append('" << path << "')\n";
#endif
		}
		PyRun_SimpleString(code.str().c_str());

		if (PyErr_Occurred()) {
			string err = "PYT(996):";
			err += python_error_string();
			return kifcode->Returnerror(err);
		}
		//you may return any value of course...
		return kifTRUE;
	}

	KifElement* MethodClose(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (init_python == false)
			return kifFALSE;
		Py_Finalize();
		init_python = false;
		pModule = NULL;
		return kifTRUE;
	}


	KifElement* MethodRun(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		if (!init_python) {
			Py_Initialize();
			init_python = true;
		}

		//0 is the first parameter and so on...
		KifElement* kcmd = callfunc->Evaluate(0, domain);
		string code = kcmd->String();
		if (code != "") {
			PyRun_SimpleString(code.c_str());

			if (PyErr_Occurred()) {
				string err = "PYT(997):";
				err += python_error_string();
				return kifcode->Returnerror(err);
			}
		}
		//you may return any value of course...
		return kifTRUE;
	}

	KifElement* MethodImport(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (pModule != NULL)
			return kifcode->Returnerror("PYT(020): Module already imported");

		if (!init_python) {
			Py_Initialize();
			init_python = true;
		}

		//0 is the first parameter and so on...
		KifElement* kcmd = callfunc->Evaluate(0, domain);
		pythonfilename = kcmd->String();
		if (pythonfilename != "") {
			PyObject* pName = PyString_FromString(pythonfilename.c_str());
			pModule = PyImport_Import(pName);
			Py_DECREF(pName);
		}

		if (PyErr_Occurred()) {
			string err = "PYT(998):";
			err += python_error_string();
			return kifcode->Returnerror(err);
		}

		//you may return any value of course...
		return kifTRUE;
	}

	KifElement* MethodExecute(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (pModule == NULL)
			return kifcode->Returnerror("PYT(002): No Python file in memory");
		int nbelements = callfunc->Size();
		if (nbelements == 0)
			return kifcode->Returnerror("PYT(003): Missing parameters");
		string funcname = callfunc->Evaluate(0, domain)->String();
		PyObject* pFunc = PyObject_GetAttrString(pModule, STR(funcname));
		if (pFunc == NULL || !PyCallable_Check(pFunc))
			return kifcode->Returnerror("PYT(004): Unknown Python function");
		PyObject* pArgs = PyTuple_New(nbelements - 1);
		PyObject* pe;
		if (nbelements > 1) {
			for (int i = 1; i < nbelements; i++) {
				pe = topython(callfunc->Evaluate(i, domain));
				PyTuple_SetItem(pArgs, i - 1, pe);
			}
		}
		pe = PyObject_CallObject(pFunc, pArgs);
		Py_DECREF(pArgs);
		if (PyErr_Occurred()) {
			if (pe != NULL)
				Py_DECREF(pe);
			string err = "PYT(999):";
			err += python_error_string();
			return kifcode->Returnerror(err);
		}
		if (pe != NULL) {
			KifElement* o = tokif(kifcode, pe);
			Py_DECREF(pe);
			if (o == kifRAISEERROR)
				return kifNULL;
			return o;
		}
		return kifTRUE;
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
		return pythonfilename;
	}

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> KifPython::kifexportedmethods;
map<short, PythonMethod>  KifPython::linkedmethods;
map<string, string> KifPython::infomethods;
hmap<short, bool> KifPython::methodassigned;

static KifPython* local;

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifPythonElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	local = new KifPython(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, PythonMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	KifPython::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	KifPython::linkedmethods[idname] = func;
	KifPython::methodassigned[idname] = true;
	KifPython::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifpython(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}

		if (init_python) {
			Py_Finalize();
			init_python = false;
		}

		kifcode->KifInitLib();
		///The new type will be: Python. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifPythonElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: KifPython)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifPythonElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("python", CreatekifPythonElement);
		if (res == kifVOID)
			return false;

		Python_type = res;
		
		return true;
	}

	Exporting bool kifpython_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifpython(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("setpath", &KifPython::MethodSetpath, P_ATLEASTONE, "setpath(string path1,string path2 etc...): Add system paths to python");
		MethodInitialization("run", &KifPython::MethodRun, P_ONE, "run(string code): Execute python code");
		MethodInitialization("import", &KifPython::MethodImport, P_ONE, "import(string python): import a python file");
		MethodInitialization("execute", &KifPython::MethodExecute, P_ATLEASTONE, "execute(string funcname,p1,p2...): execute a python function with p1,p2 as parameters");
		MethodInitialization("close", &KifPython::MethodClose, P_NONE, "close(): close the current session");

		kifGlobals->kifAllFunctions[Python_type] = KifPython::methodassigned;
		kifGlobals->kifAllInformation[Python_type] = KifPython::infomethods;
		return true;
	}

	Exporting bool kifpython_Terminate(KifCode* kifcode) {
		KifPython::kifexportedmethods.clear();
		KifPython::linkedmethods.clear();
		KifPython::methodassigned.clear();
		KifPython::infomethods.clear();
		return true;
	}
}




