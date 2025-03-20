#ifndef KIFXML_H
#define KIFXML_H

#include "kifinit.h"

short KifNewId(string n);
string KifIdString(short n);

class Kifxmldoc;
class Kifxml;
//This typedef defines a type "xmldocMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifxmldoc::*xmldocMethod)(KifElement*, KifDomain*, KifCallFunction*, int);
typedef KifElement* (Kifxml::*xmlMethod)(KifElement*, KifDomain*, KifCallFunction*, int);

void KifXmlCleaningDoc(xmlDocPtr d);
void KifXmlCleaningNode(xmlNodePtr n);
void xmlnodeclear(xmlNodePtr n);

class Kifxml : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, xmlMethod> linkedmethods;
	static map<string, string> infomethods;
	static hmap<short, bool> methodassigned;

	xmlNodePtr node;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	//---------------------------------------------------------------------------------------------------------------------
	Kifxml(KifCode* kifcode, KifElement* base, xmlNodePtr n = NULL) : KifBasic(kifcode, base, kifXML) {
		//Do not forget your variable initialisation 
		node = n;
	}

	Exported KifElement* Newinstance() {
		return new Kifxml(kifcode, NULL, NULL);
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain);

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return kifXMLDOC;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		return new Kifxml(kifcode, NULL);
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		return kifNULL;
	}

	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* val, int idthread, KifCallFunction*);
	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	Exported bool initialconstructor() {
		return true;
	}

	Exported KifElement* constructor(KifDomain* dom, KifElement* params, int, KifCallFunction*);

	//-----------------------------------------------------------------------------------------------------------------------
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
		return kifcode->Providestringraw(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.	

	KifElement* MethodRoot(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);

		Kifxml* xml = new Kifxml(kifcode, NULL);
		xml->node = node->doc->children;
		return xml;
	}

	KifElement* MethodDocument(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread);

	KifElement* MethodNext(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);

		if (callfunc->Size() == 0) {
			if (node->next == NULL)
				return kifNULL;
			return new Kifxml(kifcode, NULL, node->next);
		}

		KifElement* knode = callfunc->Evaluatethread(0, domain, idthread);
		if (knode->type != kifXML)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[1]);
		Kifxml* xml = (Kifxml*)knode;
		if (xml->node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);
		xmlUnlinkNode(xml->node);
		xmlnodeclear(xml->node);
		xmlAddNextSibling(node, xml->node);
		return kifTRUE;
	}

	KifElement* MethodPrevious(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);

		if (callfunc->Size() == 0) {
			if (node->prev == NULL)
				return kifNULL;
			return new Kifxml(kifcode, NULL, node->prev);
		}

		KifElement* knode = callfunc->Evaluatethread(0, domain, idthread);
		if (knode->type != kifXML)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[1]);
		Kifxml* xml = (Kifxml*)knode;
		if (xml->node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);
		xmlUnlinkNode(xml->node);
		xmlnodeclear(xml->node);
		xmlAddPrevSibling(node, xml->node);
		return kifTRUE;
	}

	KifElement* MethodParent(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);

		if (node->parent == NULL || node->parent->type == XML_DOCUMENT_NODE || node->parent->name == NULL)
			return kifNULL;
		return new Kifxml(kifcode, NULL, node->parent);
	}

	KifElement* MethodChild(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);

		if (callfunc->Size() == 0) {
			if (node->children == NULL)
				return kifNULL;
			return new Kifxml(kifcode, NULL, node->children);
		}

		KifElement* knode = callfunc->Evaluatethread(0, domain, idthread);
		if (knode->type != kifXML)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[1]);
		Kifxml* xml = (Kifxml*)knode;
		if (xml->node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);
		xmlUnlinkNode(xml->node);
		xmlnodeclear(xml->node);
		xmlAddChild(node, xml->node);
		return kifTRUE;
	}

	KifElement* MethodUnlink(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);

		xmlUnlinkNode(node);
		xmlnodeclear(node);
		return kifTRUE;
	}

	KifElement* MethodDelete(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);

		xmlUnlinkNode(node);
		xmlFreeNode(node);
		node = NULL;
		return kifTRUE;
	}


	KifElement* MethodNew(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		string name = callfunc->Evaluatethread(0, domain, idthread)->String();
		xmlChar* n = xmlCharStrdup((const char*)name.c_str());
		node = xmlNewNodeEatName(NULL, n);
		return this;
	}

	KifElement* MethodxmlType(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);
		return kifcode->Provideinteger(node->type);
	}

	KifElement* MethodLine(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);
		return kifcode->Provideinteger(node->line);
	}

	KifElement* MethodId(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);
		return kifcode->Provideinteger((long)node->_private);
	}

	KifElement* MethodName(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);
		if (callfunc->Size() == 0) {
			if (node->name == NULL)
				return kifNULL;
			return kifcode->Providestringraw((char*)node->name);
		}
		string name = callfunc->Evaluatethread(0, domain, idthread)->String();
		xmlFree((xmlChar*)node->name);
		node->name = xmlCharStrdup((const char*)USTR(name));
		return kifTRUE;
	}

	void TraverseXML(xmlNodePtr n, string& contenu, bool first) {
		if (n == NULL)
			return;
		if (n->content != NULL)
			contenu += (char*)n->content;
		TraverseXML(n->children, contenu, false);
		if (!first)
			TraverseXML(n->next, contenu, false);
	}

	KifElement* MethodContent(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (node == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[0]);

		string content;
		if (callfunc->Size() == 0) {
			TraverseXML(node, content, true);
			return kifcode->Providestringraw(content);
		}

		content = callfunc->Evaluatethread(0, domain, idthread)->String();
		if (node->content == NULL) {
			xmlNodePtr nouveau = xmlNewDocText(node->doc, (xmlChar*)content.c_str());
			xmlAddChild(node, nouveau);
			return kifTRUE;
		}
		xmlFree((xmlChar*)node->content);
		node->content = xmlStrdup((xmlChar*)content.c_str());
		return kifTRUE;
	}

	KifElement* MethodProperties(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread);

	KifElement* Methodxmlstring(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread);

	KifElement* MethodNamespace(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifElement* kvect = Selectvectorstring(callfunc, contextualpattern);
		if (node != NULL && node->nsDef != NULL) {
			xmlNsPtr cur = node->nsDef;
			while (cur != NULL) {
				if (cur->href != NULL)
					kvect->storevalue((char*)cur->href);
				else
					kvect->storevalue("");
				kvect->storevalue((char*)cur->prefix);
				cur = cur->next;
			}
		}
		return kvect;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFXMLSTRINGSINCLUDE[2]
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

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFXMLSTRINGSINCLUDE[3] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() {
		if (node == NULL || node->name == NULL)
			return "";
		return (char*)node->name;
	}

	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

	void* Node() {
		return node;
	}

	//equality: this==a
	KifElement* same(KifElement* a) {
		if (a == kifNULL || a == kifNOELEMENT) {
			if (node == NULL)
				return kifTRUE;
			return kifFALSE;
		}
		if (a->type != kifXML)
			return kifFALSE;
		if (((Kifxml*)a)->node == node)
			return kifTRUE;
		return kifFALSE;
	}

	KifElement* different(KifElement* a) {
		if (a == kifNULL || a == kifNOELEMENT) {
			if (node == NULL)
				return kifFALSE;
			return kifTRUE;
		}

		if (a->type != kifXML)
			return kifTRUE;
		if (((Kifxml*)a)->node != node)
			return kifTRUE;
		return kifFALSE;
	}

};

class Kifxmldoc : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	//-----------------------------------------------------------
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, xmldocMethod> linkedmethods;
	static map<string, string> infomethods;
	static hmap<short, bool> methodassigned;
	//-----------------------------------------------------------

	string filename;
	char fname[1024];

	xmlSAXHandler* sax;
	xmlDocPtr doc;
	KifElement* function;
	KifElement* object;
	KifDomain* dom;
	KifElement* context;

	KifElement* functionend;
	KifElement* objectend;
	int idnode;
	bool stop;

	xmlXPathContextPtr xpathcontext;
	KifCallFunction kcf;
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	//---------------------------------------------------------------------------------------------------------------------
	Kifxmldoc(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, kifXMLDOC), kcf(NULL, NULL, -1) {
		//Do not forget your variable initialisation
		function = base->Robfunction(this);
		if (function != NULL)
			kcf.name = function->Name();
		kcf.kifcode = kifcode;
		doc = NULL;
		object = kifNULL;
		functionend = NULL;
		objectend = kifNULL;
		sax = NULL;
		idnode = 1;
		xpathcontext = NULL;
		stop = false;
	}

	//this function is called to test the kind of function we expect from the library with the KIFXMLSTRINGSINCLUDE[4] operator...
	bool testfunction(KifElement* func) {
		if (func->type == kifFunction) {
			KifFunction* kfunction = (KifFunction*)func;
			if (kfunction->arguments.size() != 2)
				return false;
			if (kfunction->arguments[0]->VariableValue()->inType() != kifXML)
				return false;
			return true;
		}
		return false;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		if (doc != NULL) {
			ThreadLock _lock(kifXPathExpression);
			KifXmlCleaningDoc(doc);
			xmlFreeDoc(doc);
			doc = NULL;
		}
		if (sax != NULL)
			xmlFree(sax);
		sax = NULL;
		filename = "";

		string topnode = kval->String();
		stop = false;

		if (topnode.size() > 4 && topnode[0] == '<' && topnode[topnode.size() - 1] == '>')
			doc = xmlSAXParseMemory(NULL, STR(topnode), topnode.size(), 0);

		return true;
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return kifXMLDOC;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifxmldoc* kperso = new Kifxmldoc(kifcode, kifNULL);
		kperso->function = function;
		return kperso;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	KifElement* Newiterator(bool rev) {
		return kifNULL;
	}
	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	Exported bool initialconstructor() {
		return true;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	Exported KifElement* constructor(KifDomain* domloc, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;

		//Your first parameter is at position 0 etc...		
		object = params->Instruction(0); //would match i as in the example above: mynewtype toto(i,j);
		if (object->type == kifInstance)
			object = object->Thevariable(domloc, idthread, NULL);
		else
			object = object->Exec(kifNULL, domloc, kifNULL, idthread, callfunc);
		object->Setreference();
		return this;
	}

	//-----------------------------------------------------------------------------------------------------------------------
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
		return kifcode->Providestringraw(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodLoadXML(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread);
	KifElement* MethodLoadXMLBuffer(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread);
	KifElement* MethodSerialize(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread);
	KifElement* MethodSerializeString(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread);
	xmlNodePtr buildnode(xmlNodePtr node, KifElement* e);

	KifElement* MethodOnClosing(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {

		functionend = callfunc->Evaluatethread(0, domain, idthread);
		objectend = callfunc->parameters[1];

		if (objectend->type == kifInstance)
			objectend = objectend->Thevariable(domain, kifGlobals->GetThreadid(), NULL);
		else {
			threadblocinit;
			objectend = objectend->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
		}
		return kifTRUE;
	}

	KifElement* MethodCreate(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (doc != NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[5]);

		string topnode = callfunc->Evaluatethread(0, domain, idthread)->String();
		stop = false;

		if (topnode.size() > 4 && topnode[0] == '<' && topnode[topnode.size() - 1] == '>')
			doc = xmlSAXParseMemory(NULL, STR(topnode), topnode.size(), 0);
		else {
			string buf = "<";
			buf += topnode;
			buf += "/>";
			doc = xmlSAXParseMemory(NULL, STR(buf), buf.size(), 0);
		}

		return kifTRUE;
	}

	KifElement* MethodSave(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (doc == NULL)
			return kifcode->Returnerror(KIFXMLSTRINGSINCLUDE[6]);
		string encoding("UTF-8");
		if (callfunc->Size() == 2)
			encoding = callfunc->Evaluatethread(1, domain, idthread)->String();
		string filename = callfunc->Evaluatethread(0, domain, idthread)->String();
		xmlKeepBlanksDefault(0);
		xmlSaveFormatFileEnc(STR(filename), doc, STR(encoding), 1);
		return kifTRUE;
	}

	KifElement* MethodNode(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (doc != NULL) {
			Kifxml* xml = new Kifxml(kifcode, NULL);
			xml->node = doc->children;
			return xml;
		}
		return kifNULL;
	}

	KifElement* MethodClose(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread);


	KifElement* MethodXPath(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread);

	KifElement* MethodxmlEncoding(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread);
	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFXMLSTRINGSINCLUDE[2]
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

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFXMLSTRINGSINCLUDE[3] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String();
	long Integer() {
		if (doc == NULL)
			return 0;
		else
			return 1;
	}
	double Float() {
		if (doc == NULL)
			return 0;
		else
			return 1;
	}
	bool Boolean() {
		if (doc == NULL)
			return false;
		else
			return true;
	}

	//equality: this==a
	KifElement* same(KifElement* a) {
		if (a == kifNULL) {
			if (doc == NULL)
				return kifTRUE;
			return kifFALSE;
		}
		if (a->type != kifXMLDOC)
			return kifFALSE;
		if (((Kifxmldoc*)a)->filename == filename)
			return kifTRUE;
		return kifFALSE;
	}

	~Kifxmldoc() {
		if (xpathcontext != NULL)
			xmlXPathFreeContext(xpathcontext);
		if (doc != NULL) {
			KifXmlCleaningDoc(doc);
			xmlFreeDoc(doc);
		}
		if (sax != NULL)
			xmlFree(sax);
	}
};

#endif

