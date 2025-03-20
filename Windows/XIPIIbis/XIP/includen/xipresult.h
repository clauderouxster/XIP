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
   filename   : xipresult.h
   Date       : 10/01/2000
   Purpose    : Description of XipResult (VECTA version)
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
#define DISPLAY_RULE_NUMBER 268435456
#define LANGUAGE_GUESSER 536870912

#define XML_NONE 0
#define XML_OUTPUT 1
#define XML_INSERT 2


#ifdef WIN32

#define Endl "\r\n"
#if (defined(JMAX_DLL) || defined(XIPSTLUSE) )
#include <ostream>
using namespace std;
#include <sstream>
#else
#include <strstrea.h>
#endif

#else //it's a UNIX world
#ifdef XIPSTLUSE
#include <sstream>
#else
#include <strstream.h>
#endif
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

#include "xipvecta.h"

typedef enum {XIPNODE,XIPLEAF} XIPRESTYPE;


class XIPError {
public:

    char message[1000];
    char filename[1000];
    char timestamp[100];
    int line;
    int position;

    XIPError(char* mes,char* file,char* t,int l,int p) {
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


class XipFeature;
class XipNode;
class XipUnit;
class XipDependency;

typedef VECTA<XipFeature* > VXipFeature;
typedef VECTA<XipNode* > VXipNode;
typedef VECTA<XipDependency* > VXipDependency;
typedef VECTA<XipUnit* > VXipUnit;

class XipFeature {
public:

    char* attribute;
    char* value;

    Exportation void print(ostream& os);

    Exportation XipFeature() {};
    Exportation XipFeature(const XipFeature& x) {
        attribute=x.attribute;
        value=x.value;
    }
    

    Exportation XipFeature(char* a,char* v);
    Exportation ~XipFeature();
};


class XipNode {
public:

    char* category;
    long left;
    long right;
    
    long index;
    
    VXipFeature features;
    VXipNode daughters;

    XipNode* mother;

    Exportation XipNode() {left=-1;right=-1;}
    Exportation XipNode(const XipNode& x);

    Exportation virtual void print(ostream& os,char feature=0);
    Exportation virtual void print_daughters(ostream& os,char feature=0);

    Exportation XipNode(char* s, long g,long d);
    Exportation virtual ~XipNode();
    Exportation virtual XIPRESTYPE Type();
};


class XipLeaf : public XipNode {
public:

    char* lemma;
    char* surface;

    Exportation XipLeaf() {left=-1;right=-1;}
    
    Exportation XipLeaf(const XipLeaf& x);
    Exportation void print(ostream& os,char feature=0);
    Exportation void print_daughters(ostream& os,char feature=0);

    Exportation XipLeaf(char* c, char* s,char* l, long g,long d);
    Exportation ~XipLeaf();

    Exportation XIPRESTYPE Type();
};


class XipDependency {
public:

    char* name;
    VXipFeature features;
    VXipNode parameters;
    Exportation void print(ostream& os,char feature=0);

    Exportation XipDependency() {};
    Exportation XipDependency(const XipDependency& x);

    Exportation XipDependency(char* n);
    Exportation ~XipDependency();
};


class XipUnit {
public:

    XipNode* root;
    VXipDependency dependencies;
    VXipNode leaves;
    char* language;

    Exportation void print(ostream& os,char feature=0);

    Exportation XipUnit(const XipUnit& x);
    Exportation XipUnit();
    Exportation ~XipUnit();
};

class XipResult {
public:

    Exportation XipResult() {};
    VXipUnit sentences;
    
    Exportation void print(ostream& os,char feature=0);
    Exportation void printbare(ostream& os,char feature=0);
    
    Exportation ~XipResult();
};


typedef void (*XipFunction)(int,XipResult*,void*);
//--------------------------------------------------------------------
//DLL exported functions
#ifdef XIPSTLUSE
Exportation int XipParseStringOS(string text,int ipar, ostringstream* os,char xmloutput=XML_NONE,char apply_grammar=1);
Exportation int XipParseFileOS(string filename,int ipar, ostringstream* os,char xmloutput=XML_NONE,char apply_grammar=1);
#else
Exportation int XipParseStringOS(unsigned char* text,int ipar, ostrstream* os,char xmloutput=XML_NONE,char apply_grammar=1);
Exportation int XipParseFileOS(char* filename,int ipar, ostrstream* os,char xmloutput=XML_NONE,char apply_grammar=1);
#endif
Exportation int XipParseString(unsigned char* text,int ipar, XipResult* xip,char apply_grammar=1);
Exportation int XipParseFile(char* filename,int ipar, XipResult* xip,char apply_grammar=1);
Exportation int XipParseXipResult(XipResult* xipbase,XipResult* xipres,int ipar);
Exportation int XipParseXipLeaves(XipUnit* xipnode,XipResult* xipres,int ipar);
Exportation int XipParseXipNode(XipNode* xipnode,XipResult* xipres,int ipar);


Exportation int XipGrmFile(char* grmname,int xml=0);

Exportation int XipLoadGrammar(char* grammaire, int xmlinput=0, char* ntmfile=NULL,char* hmmfile=NULL);
Exportation int XipGrammarLoading(char* grammaire, int xmlinput=0, char* ntmfile=NULL,char* hmmfile=NULL);
Exportation void XipFreeGrammar(unsigned int iParseur);
//This function reloads the grammar in memory.
Exportation int XipReloadGrammar(int ipar,char reloadparameter=1,char* grmfile=NULL);

//Create a XipResult object
Exportation XipResult* XipResultCreate();
Exportation void XipResultDelete(XipResult* xip);

Exportation void Whoami(char* question);
Exportation void XipNtmVersion(char* str);
Exportation int XipExistGrammar(int iParseur);
Exportation int XipParameterFile(int ipar,char* filename);

//Return the number of days before the end of the license
Exportation int XipLicense(int ipar);
Exportation int XipIndentFile(int ipar,char open);

Exportation char* XipIndentFilePathname(int ipar);

#ifdef XIPLIBXML
Exportation int XipLoadXMLDataBase(int ipar,char* filename,char* alias);
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
#ifdef XIPSTLUSE
Exportation int XipParseStringXMLOS(string text,int ipar, ostringstream* os,int depth,char xmloutput=XML_NONE,char apply_grammar=1);
Exportation int XipParseFileXMLOS(string filename,int ipar, ostringstream* os,int depth,char xmloutput=XML_NONE,char apply_grammar=1);
#else
Exportation int XipParseStringXMLOS(unsigned char* text,int ipar, ostrstream* os,int depth,char xmloutput=XML_NONE,char apply_grammar=1);
Exportation int XipParseFileXMLOS(char* filename,int ipar, ostrstream* os,int depth,char xmloutput=XML_NONE,char apply_grammar=1);
Exportation int XipParseStringTextToXMLOS(string text,int ipar,ostrstream* os,char* rootmarkup,char* encoding,char xmloutput=XML_NONE,char applique_grammaire=1);
Exportation int XipParseFileTextToXMLOS(string filename,int ipar,ostrstream* os,char* rootmarkup,char* encoding,char xmloutput=XML_NONE,char applique_grammaire=1);
#endif
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
Exportation char XipSetVariable(int ipar,unsigned char* variable,float val);
Exportation char XipSetStringVariable(int ipar,unsigned char* variable,unsigned char* value);
Exportation char XipAddStringVariable(int ipar,unsigned char* string_vector,unsigned char* value);
Exportation char XipSetIntVariable(int ipar,unsigned char* variable,float value);
Exportation char XipAddIntVariable(int ipar,unsigned char* string_vector,float value);
Exportation char XipClearIntVariable(int ipar,unsigned char* string_vector);
Exportation char XipClearVariable(int ipar,unsigned char* string_vector);

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


