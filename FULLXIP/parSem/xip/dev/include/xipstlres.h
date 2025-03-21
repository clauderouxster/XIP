/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.60
   filename   : xipstlres.h
   Date       : 10/01/2000
   Purpose    : Description of XipResult (STL version)
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef result_h
#define result_h

#ifdef WIN32
#define XIPLONG unsigned __int64
#else
#define XIPLONG unsigned long long
#endif

const XIPLONG One = 1;

class GlobalParseur;

//The different DISPLAY modes


const XIPLONG DISPLAY_LEMMA = One<<0;
const XIPLONG DISPLAY_SURFACE =	One<<1;
const XIPLONG DISPLAY_MARKUP = One<<2;
const XIPLONG DISPLAY_ENTREE = One<<3;
const XIPLONG DISPLAY_CATEGORY = One<<4;
const XIPLONG DISPLAY_REDUCED = One<<5;
const XIPLONG DISPLAY_FULL = One<<6;
const XIPLONG DISPLAY_OFFSET = One<<7;
const XIPLONG DISPLAY_WORDNUM = One<<8;
const XIPLONG DISPLAY_SENTENCE = One<<9;
const XIPLONG DISPLAY_NONE = One<<14;
const XIPLONG DISPLAY_DEPENDENCY_BY_NAME = One<<15;
const XIPLONG DISPLAY_DEPENDENCY_BY_NODE = One<<16;
const XIPLONG DISPLAY_DEPENDENCY_BY_CREATION = One<<17;
const XIPLONG DISPLAY_TREE = One<<18;
const XIPLONG DISPLAY_TREE_PHRASE = One<<19;
const XIPLONG DISPLAY_TREE_COLUMN = One<<20;
const XIPLONG DISPLAY_MERGE_XML_SUBTREE = One<<21;
const XIPLONG DISPLAY_CONVERSION_UTF8 = One<<22;
const XIPLONG DISPLAY_EXECUTION_ERROR = One<<23;
const XIPLONG DISPLAY_MATHEMATICS = One<<24;
const XIPLONG DISPLAY_DEPENDENCY_NUMBER = One<<25;
const XIPLONG UTF8_INPUT = One<<26;
const XIPLONG DISPLAY_EXECUTE_TOKEN = One<<27;
const XIPLONG DISPLAY_SENTENCE_NUMBER = One<<28;
const XIPLONG LANGUAGE_GUESSER = One<<29;
const XIPLONG NOT_USED = One<<30;
const XIPLONG DISPLAY_CHUNK_TREE = One<<31;
const XIPLONG DISPLAY_DEPENDENCY_FEATURE_VALUE = One<<32;
const XIPLONG NO_TAG_NORMALISATION = One<<33;
const XIPLONG LOWER_INPUT = One<<34;
const XIPLONG CHECK_INPUT_UTF8 = One<<35;
const XIPLONG DISPLAY_GENERATION_CATEGORY = One<<36;
const XIPLONG DISPLAY_GENERATION = One<<37;
const XIPLONG RANDOM_ANALYSIS = One<<38;

/** Start MCR added code for passage XML output **/
const XIPLONG DISPLAY_PASSAGE_XML_FORMAT = One<<39;
/** End MCR added code for passage XML output **/

const XIPLONG DISPLAY_JSON = One << 40;
const XIPLONG DISPLAY_JSON_API = One << 41;
const XIPLONG DISPLAY_COMPUTE_DISTANCE = One << 42;
const XIPLONG DISPLAY_CLOSING_POS = One << 43;
const XIPLONG DISPLAY_TREEBANK = One << 44;

/*
const XIPLONG  DISPLAY_LEMMA=1;
const XIPLONG  DISPLAY_SURFACE =2;
const XIPLONG  DISPLAY_MARKUP= 4;
const XIPLONG  DISPLAY_ENTREE= 8;
const XIPLONG  DISPLAY_CATEGORY= 16;
const XIPLONG  DISPLAY_REDUCED= 32;
const XIPLONG  DISPLAY_FULL= 64;
const XIPLONG  DISPLAY_OFFSET =128;
const XIPLONG  DISPLAY_WORDNUM =256;
const XIPLONG  DISPLAY_SENTENCE =512;
const XIPLONG  DISPLAY_NONE =16384;
const XIPLONG  DISPLAY_DEPENDENCY_BY_NAME=32768;
const XIPLONG  DISPLAY_DEPENDENCY_BY_NODE=65536;
const XIPLONG  DISPLAY_DEPENDENCY_BY_CREATION=131072;
const XIPLONG  DISPLAY_TREE=262144;
const XIPLONG  DISPLAY_TREE_PHRASE=524288;
const XIPLONG  DISPLAY_TREE_COLUMN=1048576;
const XIPLONG  DISPLAY_MERGE_XML_SUBTREE=2097152;
const XIPLONG  DISPLAY_CONVERSION_UTF8=4194304;
const XIPLONG  DISPLAY_EXECUTION_ERROR=8388608;
const XIPLONG  DISPLAY_MATHEMATICS=16777216;
const XIPLONG  DISPLAY_DEPENDENCY_NUMBER=33554432;
const XIPLONG  UTF8_INPUT=67108864;
const XIPLONG  DISPLAY_EXECUTE_TOKEN=134217728;
const XIPLONG  DISPLAY_SENTENCE_NUMBER=268435456;
const XIPLONG  LANGUAGE_GUESSER=536870912;
const XIPLONG  NOT_USED=1073741824;
#ifdef WIN32
const XIPLONG  DISPLAY_CHUNK_TREE=2147483648;
const XIPLONG  DISPLAY_DEPENDENCY_FEATURE_VALUE=4294967296;
const XIPLONG  NO_TAG_NORMALISATION=8589934592;
const XIPLONG  LOWER_INPUT=17179869184;
const XIPLONG  CHECK_INPUT_UTF8=34359738368;
const XIPLONG  DISPLAY_GENERATION_CATEGORY=68719476736;
const XIPLONG  DISPLAY_GENERATION=137438953472;
const XIPLONG  RANDOM_ANALYSIS=274877906944;
#else
const XIPLONG  DISPLAY_CHUNK_TREE=2147483648LL;
const XIPLONG  DISPLAY_DEPENDENCY_FEATURE_VALUE=4294967296LL;
const XIPLONG  NO_TAG_NORMALISATION=8589934592LL;
const XIPLONG  LOWER_INPUT=17179869184LL;
const XIPLONG  CHECK_INPUT_UTF8=34359738368LL;
const XIPLONG  DISPLAY_GENERATION_CATEGORY=68719476736LL;
const XIPLONG  DISPLAY_GENERATION=137438953472LL;
const XIPLONG  RANDOM_ANALYSIS=274877906944LL;
#endif
**/

#define DEPENDENCY_FUNCTION_DISPLAY 1
#define DEPENDENCY_SENTENCE_NUMBER_DISPLAY 4
#define DEPENDENCY_INVERTED_DISPLAY 8
#define DEPENDENCY_FEATURE_DISPLAY 16
#define DEPENDENCY_INCOMPATIBLE_DISPLAY 32


#define XML_NONE 0
#define XML_OUTPUT 1
#define XML_INSERT 2

typedef enum {NO_STRATEGY,DEPTH,BREADTH} lookup_strategy;

typedef enum {LATIN_ENCODING,UTF8_ENCODING} encodingType;


#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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

    string message;
    string filename;
    string timestamp;
    long line;
    long position;

    XIPError(char* mes,char* file,char* t,long l,long p) {
		message="";
		filename="";
		timestamp="";
        if (mes!=NULL)
            message=mes;
        if (file!=NULL)
            filename=file;
        if (t!=NULL)
            timestamp=t;
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
	string infos;
    long left;
    long right;
    long leftchar;
    long rightchar;
	int idnode;
    
    long index;

    VXipFeature features;
    VXipNode daughters;

    XipNode* mother;

    Exportation XipNode() {left=-1;right=-1;leftchar=-1;rightchar=-1;idnode=-1;}
    Exportation XipNode(const XipNode& x);


    Exportation virtual void print(char feature=0);
    Exportation virtual void print_daughters(char feature=0);

    Exportation XipNode(string s,long g,long d, long gc,long dc);
    Exportation virtual ~XipNode();
    Exportation virtual XIPRESTYPE Type();
};


class XipLeaf : public XipNode {
public:

    string lemma;
    string surface;
	string initialfeatures;

    Exportation XipLeaf() {left=-1;right=-1;leftchar=-1;rightchar=-1;}
    Exportation XipLeaf(const XipLeaf& x);

    Exportation void print(char feature=0);
    Exportation void print_daughters(char feature=0);

    Exportation XipLeaf(string c, string s,string l, string initf,long g,long d, long gc,long dc);
    Exportation ~XipLeaf();

    Exportation XIPRESTYPE Type();
};


class XipDependency {
public:

    string name;
    VXipFeature features;
    VXipNode parameters;
	VXipDependency subdependencies;
	void* free;
	char flag;
    Exportation void print(char feature=0);

    Exportation XipDependency() {free=NULL;flag=0;};
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
	Exportation string JSon();

    Exportation XipUnit();
    Exportation ~XipUnit();
};


class XipConcept;

class XipRelation {
public:

	string relation;
	vector<XipConcept*> concepts;
	XipRelation(string r) {
		relation=r;
	}
	Exportation ~XipRelation();
};

class XipConcept {
public:

	string concept;
	string instance;
	vector<XipRelation*> relations;

	XipConcept(string c,string i) {
		concept=c;
		instance=i;
	}
	Exportation ~XipConcept();
};

class XipGraph {
public:
	string name;
	XipConcept* root;

	XipGraph(string n) : name(n) {
		root=NULL;
	}

	Exportation ~XipGraph();
};

class XipResult {
public:

    Exportation XipResult() {};
    VXipUnit sentences;
    Exportation void print(char feature=0);
    Exportation void printbare(char feature=0);
    
    Exportation ~XipResult();
	Exportation string JSon();
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

Exportation int XipParseStringStructure(string text,int ipar, ostringstream* os,char xmloutput=XML_NONE,char apply_grammar=1);

Exportation int XipGrmFile(string grmname,char loadall=1,int xml=0);

Exportation int XipEncryptGrammar(const char* grmname, const char* destdir, int loadall, int xml);
Exportation int XipLoadGrammar(string grammaire, string generefile,int xmlinput=0, string ntmfile=NULL,string hmmfile=NULL,string token=NULL,string lookup=NULL,string lookup_flags=NULL,lookup_strategy strategy=NO_STRATEGY);
Exportation int XipGrammarLoading(string grammaire, string generefile,int xmlinput=0, string ntmfile=NULL,string hmmfile=NULL,string token=NULL,string lookup=NULL,string lookup_flags=NULL,lookup_strategy strategy=NO_STRATEGY);
Exportation void XipFreeGrammar(unsigned int iParseur);
Exportation void XipFreeAllGrammars();
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
//Open or close the identation file
Exportation int XipIndentFile(int ipar,char open);
//Set the indent file pathname
Exportation int XipSetIndentFilePathname(int ipar,string pathname);
//Get the indent file pathname
Exportation char* XipIndentFilePathname(int ipar);
Exportation int XipSetWorkingFilesPath(int ipar,string path);

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

Exportation void XipSetDisplayMode(int ipar, XIPLONG mode,int column=80);
Exportation void XipAddFlagDisplayMode(int ipar, XIPLONG mode,int column=80);
Exportation void XipRemoveFlagDisplayMode(int ipar,XIPLONG mode);
Exportation char XipTestFlagDisplayMode(int ipar,XIPLONG mode);
Exportation char XipSetVariable(int ipar,string variable,float value);
Exportation char XipSetStringVariable(int ipar,string variable,string value);
Exportation char XipAddStringVariable(int ipar,string string_vector,string value);
Exportation char XipSetIntVariable(int ipar,string variable,float value);
Exportation char XipAddIntVariable(int ipar,string string_vector,float value);
Exportation char XipClearVariable(int ipar,string string_vector);
Exportation char XipClearIntVariable(int ipar,string string_vector);
Exportation char XipGetDictionaryVariable(int ipar,string variable,vector<string>& dict);
Exportation char XipSetDictionaryVariable(int ipar,string variable,vector<string>& dict);

Exportation char XipGetLabels(int ipar, string& surface, string& lemme);

//This method is used to create a handle on a pre-existing GlobalParseur object
//to be used, in the case of GlobalParseur derivation
//The handle returned can then be used to call all previous functions
Exportation int XipAddExternalGlobalParseur(GlobalParseur*);
//Returns the grammar charset
Exportation encodingType XipGetGrammarCharset(int ipar);
Exportation encodingType XipGetSourceCharset(int ipar);
Exportation void XipSetGrammarCharset(int ipar,encodingType);
Exportation void XipSetSourceCharset(int ipar,encodingType);
Exportation encodingType XipGetFSTCharset(int ipar);
Exportation void XipFSTCharset(int ipar,encodingType);

Exportation void XipGetGraphs(int ipar,vector<XipGraph*>&);

#ifdef JMAX_DLL
//If jmaxonly=1 then only JMAX result is sent back
//By default jmaxonly=0
Exportation int XipParseFileJMAX(char* filename,int ipar, XipResult* xip,char jmaxonly=0);
Exportation int XipParseStringJMAX(const char* text,int ipar, XipResult* xip,char jmaxonly=0);
Exportation int XipLoadGrammarJMAX(const char* grammaire, const char* taf4, const char* lsc, const char* tot);
Exportation int XipGrammarLoadingJMAX(const char* grammaire, const char* taf4, const char* lsc, const char* tot);
#endif

//This function is needed to reset the grammar pathname loading
Exportation void ResetEnvironmentVariables(string grmname);
Exportation void GrmTrim(ifstream& grm,unsigned char* buf,int max,char norm);
Exportation void ReadPathValue(ifstream& grm);
Exportation void send_error(string message);
Exportation void CutPathFromName(string ptnm,string& path, string& name);
Exportation GlobalParseur* XipSelectGrammar(unsigned int iParseur);


Exportation int XipGenerateFromString(string texte,int ipar, ostringstream* os);
Exportation int XipGenerateFromStringXipResult(string texte,XipResult* xip,int ipar, ostringstream* os);
Exportation int XipGenerateFromFile(string file,int ipar, ostringstream* os);
Exportation string XipLastGeneratedString(int ipar);

Exportation char XipSetWeight(int ipar,int id,float v);
Exportation char XipSetThreshold(int ipar,int id,float v);
Exportation char XipSetValue(int ipar,int id,float v);
Exportation float XipGetWeight(int ipar,int id);
Exportation float XipGetThreshold(int ipar,int id);	
Exportation float XipGetValue(int ipar,int id);
Exportation int XipRuleCounter(int ipar,int id);
Exportation int XipRuleType(int ipar,int id);
Exportation string XipRuleTypeString(int ipar,int id);
Exportation int XipNbRules(int ipar);
Exportation void StoreProbabilisticModel(int ipar,string filename);
Exportation char LoadProbabilisticModel(int ipar,string filename);
Exportation int XipRuleLayer(int ipar,int id);
Exportation void KifSetSize(int threshold);
Exportation int XipKifLoad(vector<string>& paths,vector<string>& arguments,bool debugkif);

Exportation string XipKifExecute(string name,vector<string>& parameters,bool&);
Exportation string XipKifExecuteIndex(int kifhandler,string name, vector<string>& parameters, bool&);
Exportation int KifExecuteProgram(string filename, string arguments, int map = 0);
Exportation int KifExecuteCodeProgram(string code, string arguments);

Exportation string XipKifExecuteParseur(int ipar,string name,vector<string>& parameters,bool&);
Exportation int KifExecuteCodeProgramParseur(int ipar, string code, string arguments);
Exportation int KifExecuteProgramParseur(int ipar, string filename, string arguments);

Exportation int KifLoad(vector<string>& paths,vector<string>& arguments,bool debugkif);
Exportation string KifExecute(int ikif,string name,vector<string>& parameters,ostringstream* os,bool debugkif);
Exportation void KifDelete(int ipar);
#endif
