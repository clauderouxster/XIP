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
filename   : kiftemplate.cxx
Date       : 09/09/2010
Purpose    : A KiF library template
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
//#define KIF_DLL
#include "kif.h"
#include "kifversion.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "kifcontainer.h"
#include "kifmaps.h"
#include "fractalhmap.h"
#include "kiftreemaps.h"
#include "kifpmaps.h"
#include "kifxml.h"


//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated


//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifxml::kifexportedmethods;
map<short, xmlMethod>  Kifxml::linkedmethods;
map<string, string> Kifxml::infomethods;
hmap<short, bool> Kifxml::methodassigned;

//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifxmldoc::kifexportedmethods;
map<short, xmldocMethod>  Kifxmldoc::linkedmethods;
map<string, string> Kifxmldoc::infomethods;
hmap<short, bool> Kifxmldoc::methodassigned;

extern "C" {
	int xmlXPathRegisterNs(xmlXPathContextPtr ctxt, const xmlChar *prefix, const xmlChar *ns_uri);
}


void xmlnodeclear(xmlNodePtr n) {
	n->doc = NULL;
	n->ns = NULL;
	if (n->children != NULL)
		xmlnodeclear(n->children);
	if (n->next != NULL)
		xmlnodeclear(n->next);
}

void KifXmlCleaningNode(xmlNodePtr x) {
	ThreadLock _lockg(kifGarbaging);
	ThreadLock _lock(kifGarbage);

	for (int i = 0; i < kifGlobals->garbage.size(); i++) {
		if (kifGlobals->garbage[i] != NULL && kifGlobals->garbage[i]->type == kifXML) {
			Kifxml* kxml = (Kifxml*)kifGlobals->garbage[i];
			if (kxml->node == NULL)
				continue;

			if (kxml->node == x)
				kxml->node = NULL;
		}
	}
}

void KifXmlCleaningDoc(xmlDocPtr dx) {
	ThreadLock _lockg(kifGarbaging);
	ThreadLock _lock(kifGarbage);
	for (int i = 0; i < kifGlobals->garbage.size(); i++) {
		if (kifGlobals->garbage[i] != NULL && kifGlobals->garbage[i]->type == kifXML) {
			Kifxml* kxml = (Kifxml*)kifGlobals->garbage[i];
			if (kxml->node == NULL)
				continue;

			if (kxml->node->doc == dx)
				kxml->node = NULL;
		}
	}
}

static bool kifxmlStop = false;

void startElementNs(void *ctx,
	const xmlChar *localname,
	const xmlChar *prefix,
	const xmlChar *URI,
	int nb_namespaces,
	const xmlChar **namespaces,
	int nb_attributes,
	int nb_defaulted,
	const xmlChar **attributes) {

	if (kifxmlStop)
		return;
	Kifxml* knode;
	KifFunction* func;
	Kifxmldoc* xmldoc;
	{
		ThreadLock _lock(kifXPathExpression);
		xmlSAX2StartElementNs(ctx, localname, prefix, URI, nb_namespaces, namespaces, nb_attributes, nb_defaulted, attributes);
		xmlParserCtxtPtr x = (xmlParserCtxtPtr)ctx;
		xmldoc = (Kifxmldoc*)x->_private;

		if (xmldoc->stop || Error()) {
			xmldoc->stop = true;
			return;
		}
		func = xmldoc->function->Functioncall(xmldoc->dom);
		knode = new Kifxml(xmldoc->kifcode, NULL);
		knode->node = x->node;
		if (x->node != NULL && x->node->_private == NULL)
			x->node->_private = (void*)xmldoc->idnode++;
	}

	KifCallFunction& kcf = xmldoc->kcf;
	knode->Setreference();
	kcf.declarations[func->Argument(0)->Name()] = knode;
	kcf.declarations[func->Argument(1)->Name()] = xmldoc->object;
	{
		d_pop p(&kcf, xmldoc->dom);
		func->Exec(xmldoc->context, xmldoc->dom, kifNULL, p.idstack, &kcf);
	}
	knode->Resetreference();
}

void endElementNs(void *ctx,
	const xmlChar * name,
	const xmlChar * prefix,
	const xmlChar * URI) {


	if (kifxmlStop)
		return;

	Kifxml* knode;
	KifFunction* func;
	Kifxmldoc* xmldoc;

	{
		ThreadLock _lock(kifXPathExpression);
		xmlParserCtxtPtr x = (xmlParserCtxtPtr)ctx;
		xmlSAX2EndElementNs(ctx, name, prefix, URI);
		xmldoc = (Kifxmldoc*)x->_private;

		if (xmldoc->stop || Error()) {
			xmldoc->stop = true;
			return;
		}

		func = xmldoc->functionend->Functioncall(xmldoc->dom);
		knode = new Kifxml(xmldoc->kifcode, NULL);
		knode->node = x->node;
		if (x->node != NULL && x->node->_private == NULL)
			x->node->_private = (void*)xmldoc->idnode++;
	}

	KifCallFunction& kcf = xmldoc->kcf;
	knode->Setreference();
	kcf.declarations[func->Argument(0)->Name()] = knode;
	kcf.declarations[func->Argument(1)->Name()] = xmldoc->objectend;
	{
		d_pop p(&kcf, xmldoc->dom);
		func->Exec(xmldoc->context, xmldoc->dom, kifNULL, p.idstack, &kcf);
	}
	knode->Resetreference();
}

xmlNodePtr CreateXMLNode(xmlDocPtr doc, xmlNodePtr pere, char* balise, char* texte) {
	xmlNodePtr node;
	xmlChar* n = xmlCharStrdup((const char*)balise);
	node = xmlNewNodeEatName(NULL, n);
	if (texte != NULL) {
		xmlNodePtr nouveau = xmlNewDocText(doc, (xmlChar*)texte);
		xmlAddChild(node, nouveau);
	}

	if (pere != NULL)
		xmlAddChild(pere, node);
	return node;
}

void AddAttributeValue(xmlNodePtr node, char* attribut, char* valeur) {

	if (valeur == NULL || attribut == NULL || node == NULL)
		return;
	xmlSetProp(node, (uchar*)attribut, (uchar*)valeur);
}

void finElementNs(void *ctx,
	const xmlChar * name,
	const xmlChar * prefix,
	const xmlChar * URI) {}


KifElement* Kifxmldoc::MethodLoadXML(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
	if (doc != NULL)
		return kifcode->Returnerror(KIFXMLSTRINGS[0]);

	stop = false;
	kifxmlStop = false;

	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	KifElement* kname = callfunc->Evaluatethread(0, domain, idthread);

	filename = kname->String();
	strcpy_s(fname, 1024, STR(filename));

	if (function != NULL || functionend != NULL) {
		sax = (xmlSAXHandler *)xmlMalloc(sizeof(xmlSAXHandler));
		sax->initialized = 0;
		//initxmlDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);       
		xmlSAX2InitDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);
		sax->initialized = XML_SAX2_MAGIC;
		//you may return any value of course...
		if (function != NULL) {
			sax->startElementNs = startElementNs;
			dom = domain;
			context = contextualpattern;
		}

		if (functionend != NULL) {
			sax->endElementNs = endElementNs;
			dom = domain;
			context = contextualpattern;
		}
		if (kifxmlStop)
			return kifNULL;

		//On conserve un pointeur sur l'objet GlobalParseur
		//Puis on analyse le document

		doc = xmlSAXParseFileWithData(sax, fname, 1, this);
	}
	else {
		sax = NULL;
		if (kifxmlStop)
			return kifNULL;
		doc = xmlParseFile(fname);
	}

	if (doc == NULL)
		return kifcode->Returnerror(KIFXMLSTRINGS[1]);

	return kifTRUE;
}

xmlNodePtr Kifxmldoc::buildnode(xmlNodePtr r, KifElement* e) {
	xmlNodePtr sub;
	xmlChar* name;
	if (e->aVectorContainer() || e->type == kifTable || e->type == kifList) {
		name = xmlCharStrdup(KIFXMLSTRINGS[2]);
		sub = xmlNewNodeEatName(NULL, name);
		xmlAddChild(r, sub);
		KifIteration* itr = (KifIteration*)e->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
			buildnode(sub, itr->IteratorValue());
		itr->Release();
		return sub;
	}
	if (e->aMapContainer()) {
		xmlNodePtr n;
		name = xmlCharStrdup(KIFXMLSTRINGS[3]);
		sub = xmlNewNodeEatName(NULL, name);
		xmlAddChild(r, sub);
		KifIteration* itr = (KifIteration*)e->Newiterator(false);
		string k;
		xmlChar ky[] = { 'k', 'e', 'y', 0 };
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			k = itr->IteratorKeyString();
			n = buildnode(sub, itr->IteratorValue());
			xmlSetProp(n, ky, (xmlChar*)k.c_str());
		}
		itr->Release();
		return sub;
	}

	string stype = kifGlobals->kifStringType[e->type];
	string v = e->String();
	name = xmlCharStrdup(stype.c_str());
	sub = xmlNewNodeEatName(NULL, name);
	xmlAddChild(r, sub);
	xmlNodePtr txt = xmlNewDocText(doc, (xmlChar*)v.c_str());
	xmlAddChild(sub, txt);
	return sub;
}

KifElement* Kifxmldoc::MethodSerialize(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
	//first we clean everything, to replace it with the new structure...
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

	string buf;
	KifElement* kifobject = callfunc->parameters[0]; //would match i as in the example above: mynewtype toto(i,j);
	if (kifobject->type == kifInstance) {
		kifobject = kifobject->Thevariable(domain, kifGlobals->GetThreadid(), NULL);
		string name = kifGlobals->KifIdString(kifobject->Name());
		buf = "<kif name=\"" + name + "\"/>";
	}
	else {
		kifobject = callfunc->Evaluatethread(0, domain, idthread);
		buf = "<kif/>";
	}

	stop = false;
	doc = xmlSAXParseMemory(NULL, STR(buf), buf.size(), 0);
	buildnode(doc->children, kifobject->Returnvalue());
	return kifTRUE;
}

KifElement* Kifxmldoc::MethodSerializeString(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
	//first we clean everything, to replace it with the new structure...
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

	KifElement* kifobject = callfunc->parameters[0]; //would match i as in the example above: mynewtype toto(i,j);
	if (kifobject->type != kifInstance)
		return kifcode->Returnerror(KIFXMLSTRINGS[4]);

	kifobject = kifobject->Thevariable(domain, kifGlobals->GetThreadid(), NULL);
	string name = kifGlobals->KifIdString(kifobject->Name());
	string buf;
	buf = "<kif name=\"" + name + "\"/>";
	stop = false;
	doc = xmlSAXParseMemory(NULL, STR(buf), buf.size(), 0);
	buildnode(doc->children, kifobject->Returnvalue());

	xmlChar* buff = NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
	kifobject = kifcode->Providestringraw((char*)buff);
	xmlFree(buff);

	if (doc != NULL) {
		ThreadLock _lock(kifXPathExpression);
		KifXmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	sax = NULL;
	return kifobject;
}

string Kifxmldoc::String() {
	if (doc == NULL)
		return "";
	xmlChar* buff = NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
	string s((char*)buff);
	xmlFree(buff);
	return s;
}

KifElement* Kifxmldoc::MethodClose(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (doc != NULL) {
		ThreadLock _lock(kifXPathExpression);
		KifXmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	object->Resetreference();
	sax = NULL;
	filename = "";
	return kifTRUE;
}

KifElement* Kifxmldoc::MethodLoadXMLBuffer(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
	if (doc != NULL)
		return kifcode->Returnerror(KIFXMLSTRINGS[0]);
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	stop = false;
	kifxmlStop = false;
	KifElement* kbuffer = callfunc->Evaluatethread(0, domain, idthread);

	string buff = kbuffer->String();

	if (function != NULL || functionend != NULL) {
		sax = (xmlSAXHandler *)xmlMalloc(sizeof(xmlSAXHandler));
		sax->initialized = 0;
		//initxmlDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);       
		xmlSAX2InitDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);

		//you may return any value of course...
		if (function != NULL) {
			sax->startElementNs = startElementNs;
			dom = domain;
			context = contextualpattern;
		}

		if (functionend != NULL) {
			sax->endElementNs = endElementNs;
			dom = domain;
			context = contextualpattern;
		}
		if (kifxmlStop)
			return kifNULL;

		//On conserve un pointeur sur l'objet GlobalParseur
		//Puis on analyse le document
		doc = xmlSAXParseMemoryWithData(sax, STR(buff), buff.size(), 1, this);
	}
	else {
		sax = NULL;
		if (kifxmlStop)
			return kifNULL;
		doc = xmlSAXParseMemory(NULL, STR(buff), buff.size(), 1);
	}

	if (doc == NULL)
		return kifcode->Returnerror(KIFXMLSTRINGS[5]);

	return kifTRUE;
}

KifElement* Selectprimestringmap(KifElement* callfunc, KifElement* context) {
	if (context != NULL && context->reference <= 1 && context != callfunc && context->aMapContainer() && context->Evaluate()) {
		context->Clear();
		return context;
	}

	return new KifPrimeMapStringString(callfunc->kifcode, NULL);
}


bool Kifxml::Setvalue(KifElement* kval, KifElement* idx, KifElement* domain) {
	if (kval == kifNULL || kval == kifNOELEMENT) {
		node = NULL;
		return true;
	}

	if (kval->type != kifXML) {
		if (node != NULL && idx != NULL && !idx->isConst()) {
			string att = idx->String();
			string val = kval->String();
			xmlSetProp(node, USTR(att), USTR(val));
		}
		return true;
	}

	Kifxml* xml = (Kifxml*)kval;
	node = xml->node;
	return true;
}

KifElement* Kifxml::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* func) {
	if (node == NULL)
		return kifNOELEMENT;
	if (idx != NULL && !idx->isConst()) {
		string att = idx->String();
		xmlAttrPtr propriete;
		if (node->properties == NULL)
			return kifNOELEMENT;
		propriete = node->properties;
		string p;
		while (propriete != NULL) {
			p = (char*)propriete->name;
			if (att == p)
				return kifcode->Providestringraw((char*)propriete->children->content);
			propriete = propriete->next;
		}
		return kifNOELEMENT;
	}
	return this;
}


KifElement* Kifxml::MethodProperties(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
	if (node == NULL)
		return kifcode->Returnerror(KIFXMLSTRINGS[6]);


	if (callfunc->Size() == 0) {
		KifElement* kmap = Selectprimestringmap(callfunc, contextualpattern);
		xmlAttrPtr propriete;
		if (node->properties != NULL) {
			propriete = node->properties;
			while (propriete != NULL) {
				kmap->storekey((char*)propriete->name, (char*)propriete->children->content);
				propriete = propriete->next;
			}
		}
		return kmap;
	}

	KifElement* km = callfunc->Evaluatethread(0, domain, idthread);
	if (!km->aMapContainer())
		return kifcode->Returnerror(KIFXMLSTRINGS[7]);

	xmlAttrPtr propriete;
	xmlAttrPtr base;
	//Cleaning existing properties
	if (node->properties != NULL) {
		propriete = node->properties;
		while (propriete != NULL) {
			base = propriete;
			propriete = propriete->next;
			xmlFreeProp(base);
		}
		node->properties = NULL;
	}
	string v;
	string k;
	KifIteration* itr = (KifIteration*)km->Newiterator(false);
	for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
		k = itr->IteratorKeyString();
		v = itr->IteratorValueString();
		xmlSetProp(node, USTR(k), USTR(v));
	}
	itr->Release();
	return kifTRUE;
}


KifElement* Kifxml::Methodxmlstring(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
	if (node == NULL)
		return kifNULL;

	xmlDocPtr doc = node->doc;
	xmlNodePtr topnode = doc->children;
	doc->children = node;
	xmlNodePtr thenext = node->next;
	node->next = NULL;
	xmlChar* buff = NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
	KifElement* kstr = kifcode->Providestringraw((char*)buff);
	xmlFree(buff);
	doc->children = topnode;
	node->next = thenext;
	return kstr;
}


KifElement* Kifxmldoc::MethodxmlEncoding(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
	if (doc == NULL)
		return kifcode->Returnerror(KIFXMLSTRINGS[8]);

	xmlChar* buff = NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
	KifElement* kstr = kifcode->Providestringraw((char*)buff);
	xmlFree(buff);
	return kstr;
}

KifElement* Kifxmldoc::MethodXPath(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
	if (doc == NULL)
		return kifcode->Returnerror(KIFXMLSTRINGS[8]);

	xmlNodePtr nn;
	if (xpathcontext == NULL) {
		xpathcontext = xmlXPathNewContext(doc);

		nn = doc->children;
		xmlNsPtr cur = NULL;
		while (nn != NULL && nn->type != XML_ELEMENT_NODE)
			nn = nn->next;
		if (nn != NULL)
			cur = nn->nsDef;

		//pour le contexte xpath a partir du namespace du document
		while (cur != NULL) {
			xmlXPathRegisterNs(xpathcontext, cur->prefix, cur->href);
			cur = cur->next;
		}
	}

	xmlXPathObjectPtr nodeList;
	string theXPath = callfunc->Evaluatethread(0, domain, idthread)->String();
	nodeList = xmlXPathEval((xmlChar*)theXPath.c_str(), xpathcontext);
	KifVector* kvect = Selectvector(callfunc, contextualpattern);
	if (nodeList != NULL) {
		int nbelements = 0;
		if (nodeList->nodesetval != NULL)
			nbelements = nodeList->nodesetval->nodeNr;
		Kifxml* xml;
		//On jette un coup d'oeil sur la structure construite en memoire
		for (int j = 0; j < nbelements; j++) {
			nn = nodeList->nodesetval->nodeTab[j];
			xml = new Kifxml(kifcode, NULL, nn);
			kvect->Push(xml);
		}
		xmlXPathFreeObject(nodeList);
	}
	return kvect;
}

//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifxmldocElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifxmldoc* local = new Kifxmldoc(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitializationxmldoc(string name, xmldocMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifxmldoc::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifxmldoc::linkedmethods[idname] = func;
	Kifxmldoc::methodassigned[idname] = true;
	Kifxmldoc::infomethods[name] = infos;
}

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifxmlElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifxml* local = new Kifxml(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitializationxml(string name, xmlMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifxml::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifxml::linkedmethods[idname] = func;
	Kifxml::methodassigned[idname] = true;
	Kifxml::infomethods[name] = infos;
}


KifElement* Kifxml::MethodDocument(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
	if (node == NULL)
		return kifcode->Returnerror(KIFXMLSTRINGS[6]);

	if (node->doc == NULL)
		return kifcode->Returnerror(KIFXMLSTRINGS[9]);
	Kifxmldoc* xdoc = new Kifxmldoc(kifcode, NULL);
	xdoc->doc = node->doc;
	xdoc->filename = xdoc->doc->name;
	return xdoc;
}


//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {

	Exported KifType xmldoc_Type() {
		return kifXMLDOC;
	}

	Exported bool Init_xmldoc(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << KIFXMLSTRINGS[10] << endl;
			return false;
		}
		kifcode->KifInitLib();

		if (kifGlobals->TestExternalType(KIFXMLSTRINGS[11]) == true)
			return false;

		kifGlobals->kifStringType[kifXMLDOC] = KIFXMLSTRINGS[12];
		kifGlobals->kifTypeString[KIFXMLSTRINGS[12]] = kifXMLDOC;
		kifGlobals->kifExternalTypes[KIFXMLSTRINGS[12]] = CreatekifxmldocElement;

		kifGlobals->kifStringType[kifXML] = KIFXMLSTRINGS[11];
		kifGlobals->kifTypeString[KIFXMLSTRINGS[11]] = kifXML;
		kifGlobals->kifExternalTypes[KIFXMLSTRINGS[11]] = CreatekifxmlElement;

		kifGlobals->equivalences[kifXML] = kifXML;
		kifGlobals->equivalences[kifXMLDOC] = kifXMLDOC;

		return true;
	}

	Exported bool xmldoc_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_xmldoc(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitializationxmldoc(KIFXMLSTRINGS[13], &Kifxmldoc::MethodLoadXML, P_ONE, KIFXMLSTRINGS[14]);
		MethodInitializationxmldoc(KIFXMLSTRINGS[15], &Kifxmldoc::MethodOnClosing, P_TWO, KIFXMLSTRINGS[16]);
		MethodInitializationxmldoc(KIFXMLSTRINGS[17], &Kifxmldoc::MethodLoadXMLBuffer, P_ONE, KIFXMLSTRINGS[18]);
		MethodInitializationxmldoc(KIFXMLSTRINGS[19], &Kifxmldoc::MethodCreate, P_ONE, KIFXMLSTRINGS[20]);
		MethodInitializationxmldoc(KIFXMLSTRINGS[21], &Kifxmldoc::MethodXPath, P_ONE, KIFXMLSTRINGS[22]);
		MethodInitializationxmldoc(KIFXMLSTRINGS[23], &Kifxmldoc::MethodSave, P_ONE | P_TWO, KIFXMLSTRINGS[24]);
		MethodInitializationxmldoc(KIFXMLSTRINGS[25], &Kifxmldoc::MethodClose, P_NONE, KIFXMLSTRINGS[26]);
		MethodInitializationxmldoc(KIFXMLSTRINGS[27], &Kifxmldoc::MethodNode, P_NONE, KIFXMLSTRINGS[28]);
		MethodInitializationxmldoc(KIFXMLSTRINGS[29], &Kifxmldoc::MethodxmlEncoding, P_NONE, KIFXMLSTRINGS[30]);
		MethodInitializationxmldoc(KIFXMLSTRINGS[31], &Kifxmldoc::MethodSerialize, P_ONE, KIFXMLSTRINGS[32]);
		MethodInitializationxmldoc(KIFXMLSTRINGS[33], &Kifxmldoc::MethodSerializeString, P_ONE, KIFXMLSTRINGS[34]);

		MethodInitializationxml(KIFXMLSTRINGS[35], &Kifxml::MethodRoot, P_NONE, KIFXMLSTRINGS[36]);
		MethodInitializationxml(KIFXMLSTRINGS[37], &Kifxml::MethodDocument, P_NONE, KIFXMLSTRINGS[38]);
		MethodInitializationxml(KIFXMLSTRINGS[39], &Kifxml::MethodNext, P_NONE | P_ONE, KIFXMLSTRINGS[40]);
		MethodInitializationxml(KIFXMLSTRINGS[41], &Kifxml::MethodUnlink, P_NONE, KIFXMLSTRINGS[42]);
		MethodInitializationxml(KIFXMLSTRINGS[43], &Kifxml::MethodDelete, P_NONE, KIFXMLSTRINGS[44]);
		MethodInitializationxml(KIFXMLSTRINGS[45], &Kifxml::MethodPrevious, P_NONE | P_ONE, KIFXMLSTRINGS[46]);
		MethodInitializationxml(KIFXMLSTRINGS[47], &Kifxml::MethodParent, P_NONE, KIFXMLSTRINGS[48]);
		MethodInitializationxml(KIFXMLSTRINGS[49], &Kifxml::MethodChild, P_NONE | P_ONE, KIFXMLSTRINGS[50]);
		MethodInitializationxml(KIFXMLSTRINGS[51], &Kifxml::MethodName, P_NONE | P_ONE, KIFXMLSTRINGS[52]);
		MethodInitializationxml(KIFXMLSTRINGS[53], &Kifxml::MethodLine, P_NONE, KIFXMLSTRINGS[54]);
		MethodInitializationxml(KIFXMLSTRINGS[55], &Kifxml::MethodId, P_NONE, KIFXMLSTRINGS[56]);
		MethodInitializationxml(KIFXMLSTRINGS[57], &Kifxml::MethodxmlType, P_NONE, KIFXMLSTRINGS[58]);
		MethodInitializationxml(KIFXMLSTRINGS[59], &Kifxml::MethodProperties, P_NONE | P_ONE, KIFXMLSTRINGS[60]);
		MethodInitializationxml(KIFXMLSTRINGS[61], &Kifxml::MethodContent, P_NONE | P_ONE, KIFXMLSTRINGS[62]);
		MethodInitializationxml(KIFXMLSTRINGS[63], &Kifxml::MethodNamespace, P_NONE, KIFXMLSTRINGS[64]);
		MethodInitializationxml(KIFXMLSTRINGS[65], &Kifxml::MethodNew, P_ONE, KIFXMLSTRINGS[66]);
		MethodInitializationxml(KIFXMLSTRINGS[29], &Kifxml::Methodxmlstring, P_NONE, KIFXMLSTRINGS[67]);

		kifGlobals->kifAllFunctions[kifXMLDOC] = Kifxmldoc::methodassigned;
		kifGlobals->kifAllFunctions[kifXML] = Kifxml::methodassigned;

		kifGlobals->kifAllInformation[kifXMLDOC] = Kifxmldoc::infomethods;
		kifGlobals->kifAllInformation[kifXML] = Kifxml::infomethods;

		//If you want to create specific variables in the main frame		
		return true;
	}

	Exported bool xmldoc_Terminate(KifCode* kifcode) {
		Kifxmldoc::kifexportedmethods.clear();
		Kifxmldoc::linkedmethods.clear();
		Kifxmldoc::methodassigned.clear();
		Kifxmldoc::infomethods.clear();
		Kifxml::kifexportedmethods.clear();
		Kifxml::linkedmethods.clear();
		Kifxml::methodassigned.clear();
		Kifxml::infomethods.clear();

		kifxmlStop = true;
		return true;
	}
}

Exported KifElement* Kifxml::constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
	if (params->InstructionSize() == 0)
		return this;

	KifElement* ke = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc);

	if (ke == kifNULL)
		return this;

	if (ke->type == kifXML) {
		node = ((Kifxml*)ke)->node;
		return this;
	}

	string name = ke->String();
	ke->Release();
	xmlChar* n = xmlCharStrdup((const char*)name.c_str());
	node = xmlNewNodeEatName(NULL, n);
	return this;
}

