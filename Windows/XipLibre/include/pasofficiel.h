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
   filename   : pasofficiel.h
   Date       : 10/02/2005
   Purpose    : API functions for internal use only.
   Programmer : Claude ROUX
   Reviewer   : 
*/
#ifndef pasofficielxip_h
#define pasofficielxip_h

#include <vector>
using namespace std;
#include <string>
#include "xipstlres.h"
#include "xipvecta.h"


#ifdef pasofficielxip_variable
const char* typeVariableString[]={"UNKNOWN", "REAL", "INTEGER", "STRING", "STRUCTURE","PYTHONOBJECT","XMLOBJECT"};
#else
extern const char* typeVariableString[];
#endif

#define DISPLAY_INFORMATION 1
#define DISPLAY_COMMENTS 2
#define DISPLAY_RULESPACE 4

class Label;

class rulespace {
public:
	string nom;
	rulespace* parent;
	VECTA<rulespace*> children;
	VECTA<Label*> rules;
	char actif;
};

class arbretrait {
public:

    string nom;
    VECTA<arbretrait*> fils;
    arbretrait* pere;
    virtual char type();

};

class featureroot : public arbretrait {
public:
    string filename;
    char type();
};

typedef enum {VIDE,
              REGLES_SEQUENCE,
              REGLES_DI,
              REGLES_SIMPLIFICATION,
              REGLES_EXTRACTION,
              REGLES_DEDUCTION, 
              REGLES_SEMANTIQUE,
              REGLES_MARQUAGE,
              REGLES_TERME,
              REGLES_RECONSTRUCTION,
              REGLES_RECONSTRUCTION_MODELE,
              REGLES_DECOUPESEQUENCE,
              REGLES_CARTESIEN,
              REGLES_BOUCLE,
              REGLES_FRAGMENTE,
              REGLES_VOCABULAIRE,
              REGLES_PL,
              REGLES_MULTIMOTS,
			  REGLES_MULTILEMMES,
			  REGLES_TOKENSELECTION,
			  REGLES_LEXICALES,
			  REGLES_GENERATION,
			  REGLES_ORDRE,
			  REGLES_SEQUENCE_CONTEXTE} typeRegle;


class DecompteRegle {
public:

    typeRegle regle;
    char* nom_fichier;
    unsigned long position;
    unsigned int ligne;
    int couche;
    int couche_relative;
    void* user;
	unsigned long endoffset;
};


class Label {
public:

    char* etiquette;
	string comment;
    typeRegle type;
    int index;
	int numero;
    int couche;
    DecompteRegle* dr;  
	int ruleindex;
	rulespace* space;
};


class rulebasis {
public:

	int identifier;
	int number;
	int layer;
	Label* label;	
	float weight;
	float threshold;
	float value;
	int counter;
};

typedef enum {VT_NONE, VT_REAL, VT_INTEGER, VT_STRING, VT_STRUCTURE} typeVariable;


Exportation rulebasis* XipCurrentRule(unsigned int ipar);
Exportation int XipTheCurrentRule(unsigned int ipar);
Exportation VECTA<Label*>* XipReturnInfoRuleIndex(int ipar);
Exportation VECTA<Label*>* XipReturnInfoRuleNumero(int ipar);
Exportation char XipIsGrammarCrypted(unsigned int ipar);
Exportation void XipSetRuleLimit(int ipar,int num_rule);
Exportation char XipSetLabelRuleLimit(int ipar,string num_rule);
Exportation void XipGetListCategories(int,vector<string>&);
Exportation void XipGetListDependencies(int,vector<string>&);
Exportation void XipGetListVariables(int,vector<string>&,vector<typeVariable>&);
Exportation char XipMessageLanguage(unsigned int ipar);
Exportation char XipGetVariableValue(int ipar,string variable,string& val);
Exportation arbretrait* XipGetFeatureTree(int ipar);
Exportation void XipGrammarPathnames(int ipar,
                                    string& grammar,
                                    vector<string>& features,
                                    long& debutf,
                                    vector<string>& lexicons,
                                    long& debutl,
                                    vector<string>& rules,
                                    long& debutr,
                                    vector<string>& parameters,
									vector<string>& scripts,
									vector<string>& divers);

Exportation void XipGrammarPathnamesFull(int ipar,
	string& grmname,
	string& grammar,
	vector<string>& features,
	long& debutf,
	vector<string>& lexicons,
	long& debutl,
	vector<string>& rules,
	long& debutr,
	vector<string>& parameters,
	vector<string>& scripts,
	vector<string>& divers);

Exportation void XipReturnRules(int ipar,vector<int>& rulenumbers);
Exportation int XipNbRules(int ipar);
Exportation void XipDisplayActivatedRules(int ipar,ostringstream* os,int index, char display);
Exportation void XipGrammarInformation(int ipar,
                                   vector<string>& license,
                                   string& clef,
                                   string& version,
                                   string& language,
                                   string& grammarlanguage,
                                   string& thelocale,
                                   string& charset,
                                   char& hmm,
                                   char& reentrance,
                                   int& max_words,                                   
                                   int& max_size_word,
                                   char& lexicon_encoding,
                                   int& featurenumber,
                                   string& tracepathname,
                                   string& indentpathname);

Exportation int XipParseStringAndOS(string texte,
                                    int ipar, 
                                    XipResult* xip,
                                    ostringstream* os,
                                    char xml,
                                    char applique_grammaire);
Exportation void ListOfOperations(vector<string>& lists);
unsigned char* XipGetCurrentInternalStringBuffer(unsigned int ipar);
Exportation void XipGetUnknownVariables(VECTA<char*>& unknowns);
Exportation void XipSetErrorStream(ostringstream* os);
Exportation XIPError* XipGetLastError();
Exportation void XipCleanLastError();
Exportation int XipSetPythonDebug(int ipar,char reloadparameter,char* grammar,char python,int module,int ligne);
Exportation void FindRuleBoundaries(unsigned char* buffer,long& firstsel,long& lastsel,DecompteRegle* dr,DecompteRegle* drnext);
Exportation char XipSetTaggerMode(unsigned int ipar,char tagger);
Exportation char XipGetTaggerMode(unsigned int ipar);
Exportation void XipLayerRenumbering(int ipar,int idrule);
Exportation rulespace* XipReturnRuleSpace(int ipar,string name);
rulespace* XipReturnRuleSpaceRoot(int ipar);
Exportation char XipAddBlockingRuleSpace(int ipar,string name);
Exportation char XipRemoveBlockingRuleSpace(int ipar,string name,char full);
Exportation char XipGetCurrentXMLNodeData(int ipar,string& tagname,vector<string>& properties);
Exportation void XipSetRuleRankNumberLimit(int ipar,int order);
Exportation int XipGetMaxSentence(int ipar);
Exportation void XipSetMaxSentence(int ipar, int value);
void compileautomaton(string nom,string sauve,int nbarcs);
Exportation int XipGetNbParsers();
Exportation void XipBaseParseur(XipFunction f,bool fonc,void* data);
Exportation XipResult* XipCleanCurrentFullXipResult(int ipar);
Exportation string XipCurrentSentence(int i);
#endif

