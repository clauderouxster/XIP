/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003, Xerox Corporation, XRCE-Grenoble.
* This file can only be used with the XIP library, 
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : XIP
Version    : 3.16
filename   : xipstlres.h
Date       : 10/01/2000
Purpose    : Description of XipResult (STL version)
Programmer : Claude ROUX
Reviewer   : 
*/

#ifndef result_h
#define result_h

//The different DISPLAY modes
#define DISPLAY_LEMMA 1
#define DISPLAY_SURFACE 2
#define DISPLAY_MARKUP 4
#define DISPLAY_ENTREE 8
#define DISPLAY_CATEGORY 16
#define DISPLAY_REDUCED 32
#define DISPLAY_FULL 64
#define DISPLAY_OFFSET 128
#define DISPLAY_WORDNUM 256
#define DISPLAY_SENTENCE 512
#define DISPLAY_NONE 16384
#define DISPLAY_DEPENDENCY_BY_NAME 32768
#define DISPLAY_DEPENDENCY_BY_NODE 65536
#define DISPLAY_DEPENDENCY_BY_CREATION 131072
#define DISPLAY_TREE 262144
#define DISPLAY_TREE_PHRASE 524288
#define DISPLAY_TREE_COLUMN 1048576
#define DISPLAY_MERGE_XML_SUBTREE 2097152
#define DISPLAY_CONVERSION_UTF8 4194304
#define DISPLAY_EXECUTION_ERROR 8388608
#define DISPLAY_MATHEMATICS 16777216
#define DISPLAY_DEPENDENCY_NUMBER 33554432
#define UTF8_INPUT 67108864
#define DISPLAY_EXECUTE_TOKEN 134217728
#define DISPLAY_SENTENCE_NUMBER 268435456
#define LANGUAGE_GUESSER 536870912
#define UTF8_LEXICON 1073741824
#define DISPLAY_CHUNK_TREE 2147483648
#define DISPLAY_DEPENDENCY_FEATURE_VALUE 4294967296
#define XML_NONE 0
#define XML_OUTPUT 1
#define XML_INSERT 2

typedef enum {NO_STRATEGY,DEPTH,BREADTH} lookup_strategy;

#include <string>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

#ifdef WIN32
#define Endl "\r\n"
#else
#define Endl "\n"
#endif


#ifdef XIPDLL_EXPORT
#define Exportation __declspec(dllexport)
#else
#define Exportation
#endif

#ifdef USEXIPLIBXML
#include <libxml/xmlmemory.h>
#endif


class XipFeature;
class XipNode;
class XipUnit;
class XipDependency;
class XipLeaf;

typedef vector<XipFeature* > VXipFeature;
typedef vector<XipNode* > VXipNode;
typedef vector<XipDependency* > VXipDependency;
typedef vector<XipUnit* > VXipUnit;
typedef vector<XipLeaf* > VXipLeaf;

typedef enum {XIPNODE,XIPLEAF} XIPRESTYPE;

class XIPError {
	public:
	char message[1000];
	char filename[1000];
	char timestamp[100];
	long line;
	long position;
	XIPError(char* mes,char* file,char* t,long l,long p) {
		message[0]=0;
		filename[0]=0;
		timestamp[0]=0;
		if (mes!=NULL)
			strcpy(message,mes);
		if (file!=NULL)
			strcpy(filename,file);
		if (t!=NULL)
			strcpy(timestamp,t);
		line=l;
		position=p;
	}
};

class XipFeature {
	public:
	string attribute;
	string value;
	Exportation void print();
	Exportation XipFeature() {};
	Exportation XipFeature(const XipFeature& x) {
		attribute=x.attribute;
		value=x.value;
	}
	Exportation XipFeature(string a,string v);
	Exportation ~XipFeature();
};


class XipNode {
	public:
	string category;    
	long left;
	long right;
	long index;
	VXipFeature features;
	VXipNode daughters;
	XipNode* mother;
	Exportation XipNode() {left=-1;right=-1;}
	Exportation XipNode(const XipNode& x);
	Exportation virtual void print(char feature=0);
	Exportation virtual void print_daughters(char feature=0);
	Exportation XipNode(string s,long g,long d);
	Exportation virtual ~XipNode();
	Exportation virtual XIPRESTYPE Type();
};


class XipLeaf : public XipNode {
	public:
	string lemma;
	string surface;
	Exportation XipLeaf() {left=-1;right=-1;}
	Exportation XipLeaf(const XipLeaf& x);
	Exportation void print(char feature=0);
	Exportation void print_daughters(char feature=0);
	Exportation XipLeaf(string c, string s,string l, long g,long d);
	Exportation ~XipLeaf();
	Exportation XIPRESTYPE Type();
};


class XipDependency {
	public:
	string name;
	VXipFeature features;
	VXipNode parameters;
	Exportation void print(char feature=0);
	Exportation XipDependency() {};
	Exportation XipDependency(const XipDependency& x);
	Exportation XipDependency(string n);
	Exportation ~XipDependency();
};


class XipUnit {
	public:
	XipNode* root;
	VXipDependency dependencies;
	VXipNode leaves;
	string language;
	Exportation void print(char feature=0);
	Exportation XipUnit(const XipUnit& x);
	Exportation XipUnit();
	Exportation ~XipUnit();
};

class XipResult {
	public:
	Exportation XipResult() {};
	VXipUnit sentences;
	Exportation void print(char feature=0);
	Exportation void printbare(char feature=0);
	Exportation ~XipResult();
};


typedef void (*XipFunction)(int,XipResult*,void*) ;
//--------------------------------------------------------------------
//DLL exported functions
Exportation int XipParseStringOS(string text,int ipar, ostringstream* os,char xmloutput=XML_NONE,char apply_grammar=1);
Exportation int XipParseFileOS(string filename,int ipar, ostringstream* os,char xmloutput=XML_NONE,char apply_grammar=1);
Exportation int XipParseFileToFile(string filename,int ipar, ofstream* os,char xmloutput=XML_NONE,char apply_grammar=1);

Exportation int XipParseString(string text,int ipar, XipResult* xip,char apply_grammar=1);
Exportation int XipParseFile(string filename,int ipar, XipResult* xip,char apply_grammar=1);
Exportation int XipParseXipResult(XipResult* xipbase,XipResult* xipres,int ipar);
Exportation int XipParseXipLeaves(XipUnit* xipunit,XipResult* xipres,int ipar);
Exportation int XipParseXipNode(XipNode* xipnode,XipResult* xipres,int ipar);


Exportation int XipGrmFile(string grmname,int xml=0);

Exportation int XipLoadGrammar(string grammaire, int xmlinput=0, string ntmfile=NULL,string hmmfile=NULL,string token=NULL,string lookup=NULL,string lookup_flags=NULL,lookup_strategy strategy=NO_STRATEGY);
Exportation int XipGrammarLoading(string grammaire, int xmlinput=0, string ntmfile=NULL,string hmmfile=NULL,string token=NULL,string lookup=NULL,string lookup_flags=NULL,lookup_strategy strategy=NO_STRATEGY);
Exportation void XipFreeGrammar(unsigned int iParseur);
//This function reloads the grammar in memory.
//if grmfile is not NULL, then the GRM file is also reloaded before reloading the grammar
Exportation int XipReloadGrammar(int ipar,char reloadparameter=1,char* grmfile=NULL);

//Create a XipResult object
Exportation XipResult* XipResultCreate();
Exportation void XipResultDelete(XipResult* xip);

Exportation void Whoami(string& question);
Exportation void XipNtmVersion(char* str);
Exportation int XipExistGrammar(int iParseur);
Exportation int XipParameterFile(int ipar,string filename);

//Return the number of days before the end of the license
Exportation int XipLicense(int ipar);
Exportation int XipIndentFile(int ipar,char open);

Exportation char* XipIndentFilePathname(int ipar);

#ifdef XIPLIBXML
Exportation int XipLoadXMLDataBase(int ipar, string filename,string alias);
Exportation int XipParseXMLFile(int ipar,string xmlfilename,int depth,XipResult* xip);
Exportation int XipParseXMLString(int ipar,string text,int depth,XipResult* xip);
Exportation char XipSetResultDirectory(int ipar,char* directory);
Exportation void XipAliasList(int ipar,vector<string>& alias);
Exportation char XipCleanXMLAlias(int ipar,string alias);
//This function yields the current xmlNodePtr that is under treatment
#ifdef USEXIPLIBXML
Exportation xmlNodePtr XipXMLCurrentNode(int ipar);
#endif
Exportation char XipSetCallBackXML(int ipar,XipFunction f,void* data);
Exportation int XipParseStringXMLOS(string text,int ipar, ostringstream* os,int depth,char xmloutput=XML_NONE,char apply_grammar=1);
Exportation int XipParseFileXMLOS(string filename,int ipar, ostringstream* os,int depth,char xmloutput=XML_NONE,char apply_grammar=1);
Exportation int XipParseFileXMLToFile(string filename,int ipar, ofstream* os,int depth,char xmloutput=XML_NONE,char apply_grammar=1);
Exportation int XipParseStringTextToXMLOS(string text,int ipar,ostringstream* os,char* rootmarkup,char* encoding,char xmloutput=XML_NONE,char applique_grammaire=1);
Exportation int XipParseFileTextToXMLOS(string filename,int ipar,ostringstream* os,char* rootmarkup,char* encoding,char xmloutput=XML_NONE,char applique_grammaire=1);
#endif

//computeDependency==1, the dependencies are computed
//computeDependency=0, the dependencies are not computed
//the default is computeDependency=1, when XipLoadLibrary is called
Exportation void XipSetDependencyExtraction(int ipar,char computeDependency);

//Those two functions may be used to intercept a new result for one sentence
Exportation char XipSetCallBack(int ipar,XipFunction f,void* data);
//If the first XipResult is to be destroyed in the Call Back function, then 
//the initial XipResult should be create with XipSetCurrentXipResult
//and deleted with XipDeleteCurrentXipResult
Exportation XipResult* XipCreateCurrentXipResult(int ipar);
Exportation char XipDeleteCurrentXipResult(int ipar);

//This function cleans the current XipResult object and 
//reinitializes it.
Exportation XipResult* XipCleanCurrentXipResult(int ipar);

//This function retrieves the current XipResult object
Exportation XipResult* XipGetCurrentXipResult(int ipar);

Exportation void XipSetDisplayMode(int ipar, unsigned long mode,int column=80);
Exportation void XipAddFlagDisplayMode(int ipar,unsigned long mode,int column=80);
Exportation void XipRemoveFlagDisplayMode(int ipar,unsigned long mode);
Exportation char XipTestFlagDisplayMode(int ipar,unsigned long mode);
Exportation char XipSetVariable(int ipar,string variable,float value);
Exportation char XipSetStringVariable(int ipar,string variable,string value);
Exportation char XipAddStringVariable(int ipar,string string_vector,string value);
Exportation char XipSetIntVariable(int ipar,string variable,float value);
Exportation char XipAddIntVariable(int ipar,string string_vector,float value);
Exportation char XipClearVariable(int ipar,string string_vector);
Exportation char XipClearIntVariable(int ipar,string string_vector);

Exportation char XipGetLabels(int ipar, char* surface,char* lemme);

#ifdef JMAX_DLL
//If jmaxonly=1 then only JMAX result is sent back
//By default jmaxonly=0
Exportation int XipParseFileJMAX(char* filename,int ipar, XipResult* xip,char jmaxonly=0);
Exportation int XipParseStringJMAX(const char* text,int ipar, XipResult* xip,char jmaxonly=0);
Exportation int XipLoadGrammarJMAX(const char* grammaire, const char* taf4, const char* lsc, const char* tot);
Exportation int XipGrammarLoadingJMAX(const char* grammaire, const char* taf4, const char* lsc, const char* tot);
#endif

#endif


