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
filename   : mathemat.cxx
Date       : 10/01/1999
Purpose    : Implementation of the compiling and implementation of mathematic formulae.
Programmer : Claude ROUX
Reviewer   :
*/

#ifdef XIPPYTHON
#ifndef WIN32
#define Py_BUILD_CORE
#endif
#ifdef APPLE
#ifdef DYNAMICLIB
#include <crt_externs.h>
char **environ = *_NSGetEnviron();
#endif
#endif
#endif

#if defined(WIN32) & !defined(_WINDLL)
extern char AFFICHAGEDOS;
#endif

#ifdef NTMCONNECTION
extern "C" {
#include "fst/context.h"
#include "fst/cfsm.h"
#include "applyloc.h"
}


#include "fst/modify.h"
#include "fst/bin-in.h"
#include "fst/pars-tbl.h"
#include "lktypes.h"
#include "ems.h"
#include "lookup.h"

#include "networks.h"

#include "tok-aux.h"
#include "text-io.h"
#include "bin-out.h"
#endif

#include "parsbin.h"
#include "varlibxml.h"
#include "graphe.h"
#include "conversion.h"
#include "kif.h"
#ifdef NTMCONNECTION
#include "ntm.h"
#endif
#include "ntmhmm.h"
#include "transduct.h"
#include "xipkif.h"
#include "kifcontainer.h"

extern string choix_ambigue;

extern ostream* flot_erreur;
extern char VERSION_XIP_BUILD[];
#ifdef XIPLIBXML
char recupere_donnees_xml(GlobalParseur* parseur,xmlNodePtr node,TestFormule* formule);
void RecupereTextXmlNode(string& accumulateur,xmlNodePtr node);
#endif
baseregle* XipCurrentRule(unsigned int ipar);
string& Trim(string& chaine);
//extern char parseur->python_debug;
//extern int num_python_ligne;
//extern int num_fichier_python;
//
//int fullpythonlexical=NON;
extern char* champs_anglais[];

extern VecteurDeParseurs gVecteurParseurs;
extern GlobalParseur* parseur_courant;
char parentde(ResultatFonction* fille,ResultatFonction* parent);
ResultatFonction* suivantde(ResultatFonction* courant,ResultatFonction* suivant,int increment);
ResultatFonction* perede(ResultatFonction* courant,ResultatFonction* suivant);
ResultatFonction* filsde(ResultatFonction* courant,ResultatFonction* suivant,char dernier);
ResultatFonction* tokensuivant(GlobalParseur*,ResultatFonction* courant,ResultatFonction* suivant,int pos);
string replacestringwithautomaton(string &str, string& reg,string& rep);
#ifdef XIPLIBXML
string xmlEncoding(string& tag,string& txt);
#endif
KifElement* ChercheDansKif(KifCode* kifcode,string name);
KifElement* XipKifExecuteIndexObject(KifCode*, string name, vector<KifElement*>& parameters, bool& erreur);
extern GlobalParseur* BaseParseur;
#include "arbren.h"


class x_lirechaine : public x_readstring {
public:

	x_lirechaine(string s) : x_readstring(s) {}

	void Initable() {
        int i=0;
		unsigned char c;
		for (c=65;c<=90;c++)
			table[i++]=c;
		for (c=97;c<=122;c++)
			table[i++]=c;
		for (c=128;c<255;c++)
			table[i++]=c;
        table[i++]='_';
		table[i]=0;
	}
};


string ExtraireGroupe(GlobalParseur* parseur,istream& fichier) {
	string chaine;
	int compte=1;
	char c;
	while (compte!=0 && !fichier.eof()) {
		c=parseur->LectCrypte(fichier);
		if (c=='{')
			compte++;
		if (c=='}')
			compte--;
		chaine+=c;
	}
	return chaine;
}

map<string,typeComparateur> labelequivalences;

void initlabelequivalences() {
	static char deja=NON;

	if (deja==OUI)
		return;
	deja=OUI;
	labelequivalences["egauxontologie"]=EGAUX_ONTOLOGIE;
	labelequivalences["compatibleontologie"]=COMPATIBLES_ONTOLOGIE;
	labelequivalences["affectationontologie"]=AFFECTATION_ONTOLOGIE;
	labelequivalences["egauxlecture"]=EGAUX_LECTURE;
	labelequivalences["affectationlecture"]=AFFECTATION_LECTURE;
	labelequivalences["compatibletrait"]=COMPATIBLES;
	labelequivalences["egauxtrait"]=EGAUX;
	labelequivalences["affectationtrait"]=AFFECTATION;
	labelequivalences["egalite"]=EGALITE;
	labelequivalences["inferieur"]=INFERIEUR;
	labelequivalences["superieur"]=SUPERIEUR;
	labelequivalences["inftraits"]=INF_TRAITS;
	labelequivalences["suptraits"]=SUP_TRAITS;
	labelequivalences["infegaltraits"]=INF_EGAL_TRAITS;
	labelequivalences["supegaltraits"]=SUP_EGAL_TRAITS;
	labelequivalences["testsimple"]=TEST;
}

//------------------------------------------------------------------------------------

void decoupechaine(string& chaine,string& coupeur,VECTA<string*>& vectr) {
	size_t pos=0;
	size_t trouve=0;

	while (pos!=string::npos) {
		trouve=chaine.find(coupeur,pos);
		if (trouve!=string::npos) {
			string sx=chaine.substr(pos,trouve-pos);
			if (sx!="")
				vectr.ajoute(new string(sx));
			pos=trouve+coupeur.size();
		}
		else
			break;
	}
	string s=chaine.substr(pos,chaine.size()-pos);
	if (s!="")
		vectr.ajoute(new string(s));
}
//------------------------------------------------------------------------------------


typeComparateur Comparateur(string s) {
	initlabelequivalences();
	return labelequivalences[s];
}

static char* field_name[] = {"",
                      "sequence",
                      "idrules",
                      "tagging",
                      "extraction",
                      "dependencyrules",
                      "semantic",
                      "marker",
                      "terms",
                      "reshuffling",
                      "reshuffling",
                      "split",
                      "doublereduction",
                      "",
                      "fragment",
                      "vocabulary",
                      "lprules",
                      "functions",
					  "multiwords",
					  "multilemmas",
					  "tokenselection",
					  "lexicals",
					  "generation",
					  "order",
					  "sequenceoncontext"};

string FieldName(int i) {
	return field_name[i];
}

static const char* affichagemessageerreur[] ={
    "Instruction interrupted: Wrong number of parameters for a LEFT instruction", //0
    "Instruction interrupted: Wrong number of parameters for a MID instruction", //1
    "Instruction interrupted: Wrong number of parameters for a REGEXP instruction", //2
    "Instruction interrupted: Wrong number of parameters for a TRIM instruction", //3
    "Instruction interrupted: Wrong number of parameters for a REGEXPSPLIT instruction", //4
    "Instruction interrupted: The third parameter is not a VECTOR in a regexpsplit instruction",//5
    "Instruction interrupted: Wrong number of parameters for a SEARCH instruction", //6
    "Instruction interrupted: The first parameter is not a VECTOR in a SEARCH",//7
    "Instruction interrupted: A variable has not been associated with a syntactic node",//8
    "Instruction interrupted: A variable is not associated with a dependency",//9
    "Instruction interrupted: This instruction does not have any operator",//10
    "Instruction interrupted: Missing element in an arithmetical operation",//11
    "Instruction interrupted: Division /0",//12
    "Instruction interrupted: Operation error",//13
    "Instruction interrupted: Wrong number of parameters for a RIGHT instruction", //14
    "Instruction interrupted: Character out of range",//15
    "Instruction interrupted: Wrong number of parameters for an INSERT instruction", //16
    "Instruction interrupted: The first parameter is not a VECTOR in an INSERT",//17
    "Instruction interrupted: Wrong number of parameters for an ADDSTRING instruction", //18
    "Instruction interrupted: Wrong number of parameters for a REMOVESTRING instruction", //19
    "Instruction interrupted: The first parameter is not a VECTOR in an REMOVESTRING",//20
    "Instruction interrupted: The first parameter is not a VECTOR in an ADDSTRING",//21
    "Instruction interrupted: Missing parameter for the _goto clause",//22
    "Instruction interrupted: The third parameter is not a VECTOR in a SEARCH instruction",//23
    "Instruction interrupted: Wrong number of parameters for a OFFSET instruction",//24
    "Instruction interrupted: The first parameter should a node variable in an OFFSET instruction",//25
    "Instruction interrupted: Wrong number of parameters for a LEMMAS instruction",//26
    "Instruction interrupted: The second parameter should a node variable in a LEMMAS instruction",//27
    "Instruction interrupted: The first parameter is not a VECTOR in a LEMMAS instruction",//28
    "Instruction interrupted: A vector can only be initialized with another vector or with the empty string:\"\"",//29
    "Instruction interrupted: Wrong number of parameters for a NODE instruction", //30
    "Instruction interrupted: The first parameter should a node variable in a NODE instruction",//31
    "Instruction interrupted: Index out of bound in a NODE instruction",//32
    "Instruction interrupted: This node variable has already been instanciated with another node",//33
    "Instruction interrupted: Wrong number of arguments for a NEXT,PREVIOUS, DAUGHTER or MOTHER instruction",//34
    "Instruction interrupted: One of the parameters is not a node for a NEXT,PREVIOUS, DAUGHTER or MOTHER instruction",//35
    "Instruction interrupted: Value out of range in VINT variable",//36
    "Instruction interrupted: Value out of range in struct vector",//37
    "Instruction interrupted: Wrong number of parameters for a UPPERCASE or a LOWERCASE instruction", //38
    "Instruction interrupted: Mismatched variable type in call to procedure",//39
    "Instruction interrupted: Wrong number of argument for a REMOVE instruction",//40
    "Instruction interrupted: Wrong number of argument for a COPYFEATURE instruction",//41
    "Instruction interrupted: Wrong first argument in a COPYFEATURE instruction",//42
    "Instruction interrupted: Wrong second argument in a COPYFEATURE instruction",//43
    "Instruction interrupted: Missing or wrong argument in a CLEANLEMMA instruction",//44
    "Instruction interrupted: Missing parameter in a RETRIEVELEMMAS instruction",//45
    "Instruction interrupted: The first parameter should be lemma variable in a RETRIEVELEMMAS instruction",//46
    "Instruction interrupted: The second parameter should be VECTOR in a RETRIEVELEMMAS instruction",//47
    "Instruction interrupted: The third parameter should be VINT in a RETRIEVELEMMAS instruction",//48
    "Instruction interrupted: Missing or wrong argument in a GETLEMMA instruction",//49
    "Instruction interrupted: The first parameter should be lemma variable in a GETLEMMA instruction",//50
    "Instruction interrupted: Wrong number of parameters in a FOPEN",//51
    "Instruction interrupted: This alias already exists",//52
    "Instruction interrupted: Error while opening a file",//53
    "Instruction interrupted: Wrong number of parameters in a FCLOSE",//54
    "Instruction interrupted: This alias does not exists",//55
    "Instruction interrupted: Missing parameter in a READ instruction",//56
    "Instruction interrupted: The parameter is not a variable in a READ instruction",//57
    "Instruction interrupted: Missing variable as a parameter in call to procedure",//58
    "Instruction interrupted: Missing parameter in ASCII",//59
    "Execution of XIP has been stopped with an ERROR instruction",//60
    "Instruction interrupted: The first parameter is not a DICTIONARY in a backsearch",//61
    "Instruction interrupted: Wrong number of parameters for a backsearch instruction", //62
    "Instruction interrupted: Wrong number of parameters for the help instruction", //63
    "Instruction interrupted: Wrong number of parameters for an INSERTVALUE instruction", //64
    "Instruction interrupted: Expecting a dependency variable",//65
    "Instruction interrupted: Missing parameter in a DEPENDENCYNUMBER instruction",//66
    "Instruction interrupted: Expecting a dependency variable",//67
    "Instruction interrupted: Missing parameters in a DEPENDENCY instruction",//68
    "Instruction interrupted: Expecting an integer variable",//69
    "Instruction interrupted: Error in a Python script",//70
    "Instruction interrupted: Error in SETFEATURE instruction, wrong number of parameters",//71
    "Instruction interrupted: Wrong first argument in a SETFEATURE instruction, it should be a node",//72
    "Instruction interrupted: Unknown attribute:value in a SETFEATURE instruction",//73
    "Instruction interrupted: Wrong number of parameter in a PUSH",//74
    "Instruction interrupted: Wrong number of parameter in a POP",//75
    "Instruction interrupted: Wrong number of parameter in a CLEANSTACK",//76
    "Instruction interrupted: Wrong number of parameter in the procedure",//77
    "Instruction interrupted: Wrong type of parameter in the procedure",//78
    "Instruction interrupted: Wrong number of parameter in the PYTHON procedure",//79
    "Instruction interrupted: Wrong type of parameter in the PYTHON procedure",//80
    "Instruction interrupted: Wrong number of parameters in a KEYS, a VALUES or a KEY instruction",//81
    "Instruction interrupted: Wrong type for the first parameter of a KEYS, a VALUES or a KEY instruction. Dictionary expected",//82
    "Instruction interrupted: Wrong type for the second parameter of a KEYS or a VALUES instruction. String vector expected",//83
	"Instruction interrupted: Missing parameters in a XMLDATA instruction",//84
	"Instruction interrupted: First parameter should be a XML variable ",//85
	"Instruction interrupted: Second parameter should be a DICTIONARY variable ",//86
	"Instruction interrupted: Missing parameters in a XMLPARENT,XMLCHILD,XMLNEXT,XMLPREVIOUS instruction",//87
	"Instruction interrupted: Second parameter should be a XML variable ",//88
	"Instruction interrupted: Second parameter should be a STRING variable ",//89
	"Instruction interrupted: Missing parameters in a XMLSET instruction",//90
	"Instruction interrupted: Wrong parameter in a PYTHONFULLLEXICAL instruction",//91
	"Instruction interrupted: Unknown @reference in XMLSET",//92
	"Instruction interrupted: Missing parameter a BLOCK or a FREE",//93
	"Instruction interrupted: Missing parameter in a BLOCKED or a RULESPACE",//94
	"Instruction interrupted: Wrong parameter in a GETENV",//95
	"Instruction interrupted: Wrong parameter in a PUTENV",//96
	"Instruction interrupted: Wrong affectation in a GRAPH operation: the second variable should be a graph",//97
	"Instruction interrupted: Wrong affectation in a GRAPH operation",//98
	"Instruction interrupted: Wrong number of parameters in ADDGRAPH",//99
	"Instruction interrupted: First parameter in ADDGRAPH should be GRAPH VECTOR",//100
	"Instruction interrupted: Wrong number of parameters in PROJECT",//101
	"Instruction interrupted: Wrong 3rd parameter in PROJECT: It should be GRAPH VECTOR",//102
	"Instruction interrupted: The second parameter should be a unique graph",//103
	"Instruction interrupted: Wrong number of parameters in SETGRAPHNAME",//104
	"Instruction interrupted: Wrong second parameter in SETGRAPH: It should be a GRAPH reference",//105
	"Instruction interrupted: Wrong number of parameters in GETGRAPHNAME",//106
	"Instruction interrupted: the first parameter should be a STRING variable in GETGRAPHNAME",//107
	"Instruction interrupted: the second parameter should be a graph reference in GETGRAPHNAME",//108
	"Instruction interrupted: their should be 3 graph reference parameters in EXTRACTGRAPH",//109
	"Instruction interrupted: The 3rd parameter should be a GRAPH VECTOR in EXTRACTGRAPH",//110
	"Instruction interrupted: their should be 3 graph reference parameters in REPLACEGRAPH",//111
	"Instruction interrupted: their should be only one graph at a time for each parameter REPLACEGRAPH",//112
	"Instruction interrupted: their should be 2 graph reference parameters in REMOVESUBGRAPH",//113
	"Instruction interrupted: The first parameter should be a graph variable in REMOVESUBGRAPH",//114
	"Instruction interrupted: No graph pattern to remove in REMOVESUBGRAPH",//115
	"Instruction interrupted: Wrong number of parameters in REMOVEGRAPH",//116
	"Instruction interrupted: First parameter in REMOVEGRAPH should be GRAPH VECTOR",//117
	"Instruction interrupted: Wrong number of parameters in DUPLICATEGRAPH",//118
	"Instruction interrupted: The first parameter should be a graph variable in DUPLICATEGRAPH",//119
	"Instruction interrupted: The second parameter should be a unique graph in DUPLICATEGRAPH",//120
	"Instruction interrupted: New words can only be added within a XIPTRANS grammar",//121
	"Instruction interrupted: We expect four parameters in an ADDWORD instruction",//122
	"Instruction interrupted: the alias in the ADDWORD instruction is unknown. It must be declared in a DYNAMIC section in the lex.trs file.",//123
	"Instruction interrupted: XMLTOKEN requires two parameters: a node variable and an xml variable",//124
	"Instruction interrupted: XMLTOKEN error: the node variable has not been created from a token",//125
	"Instruction interrupted: Wrong number of parameters in SAVEXMLGRAPH",//126
	"Instruction interrupted: The seond parameter should be a graph or a vector of graph in SAVEXMLGRAPH",//127
	"Instruction interrupted: Wrong number of parameters in GETINITIALFEATURES",//128
	"Instruction interrupted: First parameter should be node variable (#1) GETINITIALFEATURES",//129
	"Instruction interrupted: Second parameter should be a string vector GETINITIALFEATURES",//130
    "Instruction interrupted: Error in SETSUBFEATURE instruction, wrong number of parameters",//131
    "Instruction interrupted: Wrong first argument in a SETSUBFEATURE instruction, it should be a node",//132
    "Instruction interrupted: Unknown attribute:value in a SETSUBFEATURE instruction",//133
    "Instruction interrupted: Unknown node in a SETSUBFEATURE instruction",//134
    "Instruction interrupted: Unknown node in a SETFEATURE instruction",//135
	"Instruction interrupted: Missing root in a GENERATION rule",//136
	"Instruction interrupted: Wrong number of arguments in a INITLOOKDOWN instruction",//137
	"Instruction interrupted: Wrong argument in a INITLOOKDOWN instruction",//138
	"Instruction interrupted: Wrong argument in a DEPTH instruction",//139
	"Instruction interrupted: Wrong argument in a RANK instruction",//140
	"Instruction interrupted: Missing '(' while reading a dependency string",//141
	"Instruction interrupted: Unknown NODE or FUNCTION, while reading a dependency stream=",//142
	"Instruction interrupted: Missing stream",//143
	"Instruction interrupted: Expecting a ',' or a ')'",//144
	"Instruction interrupted: Expecting a '#'",//145
	"Instruction interrupted: Missing features in a node declaration",//146
	"Instruction interrupted: Missing lemma in a node declaration",//147
	"Instruction interrupted: Missing '.' at the end of this dependency",//148
	"Instruction interrupted: Wrong Number of Arguments in a NODE function",//149
	"Instruction interrupted: Wrong Argument in a NODE function",//150
	"Instruction interrupted: Wrong number of arguments in a LOOKUP instruction",//151
	"Instruction interrupted: Wrong number of arguments in a LOODOWN instruction",//152
	"Instruction interrupted: Too many loops in a dependency rule:",//153
	"Instruction interrupted: Unknown CATEGORY, while reading a dependency stream",//154
	"Instruction interrupted: Wrong Number of Arguments in a ISTOKEN function",//155
	"Instruction interrupted: Wrong Type of Arguments in a ISTOKEN function: a NODE is expected",//156
	"Instruction interrupted: Wrong Type of Arguments in a SETWORD function: integer, string",//157
	"Instruction interrupted: Wrong Type of Arguments in a GETWORD function: integer, string",//158
	"Instruction interrupted: Wrong Type of Arguments in a CLEANWORD function: integer, string",//159
	"Instruction interrupted: Wrong number of arguments in a PROBABILITY instruction",//160
	"Instruction interrupted: Wrong type of arguments in a RULETYPE instruction",//161
	"Instruction interrupted: No rule with this ID",//162
	"Instruction interrupted: Wrong number of arguments in SAVEPROBABILITIES",//163
	"Instruction interrupted: Wrong number of arguments in LOADPROBABILITIES",//164
	"Instruction interrupted: Wrong type of arguments in a RULECOUNTER instruction",//165
	"Instruction interrupted: Missing parameters in a XMLATTRIBUTES instruction",//166
	"Instruction interrupted: No XML token attached to this node in a XMLATTRIBUTES instruction",//167
	"Instruction interrupted: Missing parameters in a XMLGET instruction",//168
	"Instruction interrupted: No XML token attached to this node in a XMLGET instruction",//169
	"Instruction interrupted: Not enough parameter in CREATEDEPENDENCY",//170
	"Instruction interrupted: Wrong type of parameter in CREATEDEPENDENCY",//171
	"Instruction interrupted: Unknown dependency name in CREATEDEPENDENCY",//172
	"Instruction interrupted: Wrong feature structure in CREATEDEPENDENCY",//173
	"Instruction interrupted: Missing parameter in EVAL",//174
    "Instruction interrupted: Wrong number of parameters for a SPLITALONG instruction", //175
    "Instruction interrupted: The third parameter is not a VECTOR in a SPLITALONG instruction",//176
	"Instruction interrupted: Error in a KIF program: ",//177
	"Instruction interrupted: Missing parameter in a call to a KIF program: ",//178
    "Instruction interrupted: Wrong type of parameter in a KIF procedure",//179
	"Instruction interrupted: Wrong number of parameters in XMLSTRING",//180
    ""
};

#ifdef XIPPYTHON
#if PY_MINOR_VERSION==4
#define Py_ssize_t int
#define OLDPYTHON
#endif

#ifndef DONOTEXPORTXIPS

#if PY_MAJOR_VERSION >= 3
static bool initialization = true;

#define PyCheck_String PyUnicode_Check
#define ConvertToPythonLong(i)	PyLong_FromLong(i)
#define PyCheck_Number	PyLong_Check
#define PyAsNumber	PyLong_AsLong

PyObject* ConvertToPythonString(string s) {
	string res;
	s_latin_to_utf8(res, USTR(s));
	const char* buff = res.c_str();
	return PyUnicode_FromStringAndSize(buff, res.size());
}

string PyAsString(PyObject* po) {
	string s;
	Py_ssize_t sz = PyUnicode_GetSize(po);
	Py_UNICODE* ibuff = PyUnicode_AsUnicode(po);
	for (int i = 0; i < sz; i++)
		s += s_unicode_to_utf8(ibuff[i]);
	return s;
}

#else
#define ConvertToPythonString(s)  PyString_FromString(s)
#define ConvertToPythonLong(i)	PyInt_FromLong(i)
#define PyCheck_Number	PyInt_Check
#define PyAsNumber PyInt_AsLong
#define PyAsString PyString_AsString
#define PyCheck_String PyString_Check
#endif

/*
	func+="XIP_DISPLAY_LEMMA=1<<0\n";
	func+="XIP_DISPLAY_SURFACE =1<<1\n";
	func+="XIP_DISPLAY_MARKUP=1<<2\n";
	func+="XIP_DISPLAY_ENTREE=1<<3\n";
	func+="XIP_DISPLAY_CATEGORY=1<<4\n";
	func+="XIP_DISPLAY_REDUCED=1<<5\n";
	func+="XIP_DISPLAY_FULL=1<<6\n";
	func+="XIP_DISPLAY_OFFSET =1<<7\n";
	func+="XIP_DISPLAY_WORDNUM =1<<8\n";
	func+="XIP_DISPLAY_SENTENCE =1<<9\n";
	func+="XIP_DISPLAY_NONE =1<<14\n";
	func+="XIP_DISPLAY_DEPENDENCY_BY_NAME=1<<15\n";
	func+="XIP_DISPLAY_DEPENDENCY_BY_NODE=1<<16\n";
	func+="XIP_DISPLAY_DEPENDENCY_BY_CREATION=1<<17\n";
	func+="XIP_DISPLAY_TREE=1<<18\n";
	func+="XIP_DISPLAY_TREE_PHRASE=1<<19\n";
	func+="XIP_DISPLAY_TREE_COLUMN=1<<20\n";
	func+="XIP_DISPLAY_MERGE_XML_SUBTREE=1<<21\n";
	func+="XIP_DISPLAY_CONVERSION_UTF8=1<<22\n";
	func+="XIP_DISPLAY_EXECUTION_ERROR=1<<23\n";
	func+="XIP_DISPLAY_MATHEMATICS=1<<24\n";
	func+="XIP_DISPLAY_DEPENDENCY_NUMBER=1<<25\n";
	func+="XIP_UTF8_INPUT=1<<26\n";
	func+="XIP_DISPLAY_EXECUTE_TOKEN=1<<27\n";
	func+="XIP_DISPLAY_SENTENCE_NUMBER=1<<28\n";
	func+="XIP_LANGUAGE_GUESSER=1<<29\n";
	func+="XIP_NOT_USED=1<<30\n";
	func+="XIP_DISPLAY_CHUNK_TREE=1<<31\n";
	func+="XIP_DISPLAY_DEPENDENCY_FEATURE_VALUE=1<<32\n";
	func+="XIP_NO_TAG_NORMALISATION=1<<33\n";
	func+="XIP_LOWER_INPUT=1<<34\n";
	func+="XIP_CHECK_INPUT_UTF8=1<<35\n";
	func+="XIP_DISPLAY_GENERATION_CATEGORY=1<<36\n";
	func+="XIP_DISPLAY_GENERATION=1<<37\n";
	func+="XIP_RANDOM_ANALYSIS=1<<38\n";

	func+="XIP_DISPLAY_PASSAGE_XML_FORMAT=1<<39\n";
*/

string insere_xip_class() {
	string func;

	func += "class XipNode:\n";
	func += " parseur=0\n";
	func += " def getnode(self,i):\n";
	func += "  if i==-1:\n";
	func += "   return 0\n";
	func += "  liste=node(i,self.grm)\n";
	func += "  self.index=i\n";
	func += "  if liste[0]!=0:\n";
	func += "      self.surface=liste[0][2]\n";
	func += "      self.parent=liste[0][5]\n";
	func += "      self.daughter=liste[0][6]\n";
	func += "      self.last=liste[0][7]\n";
	func += "      self.next=liste[0][8]\n";
	func += "      self.previous=liste[0][9]\n";
	func += "      self.left=liste[0][10]\n";
	func += "      self.right=liste[0][11]\n";
	func += "      self.leftoffset=liste[0][12]\n";
	func += "      self.rightoffset=liste[0][13]\n";
	func += "      self.leftoffsetchar=liste[0][14]\n";
	func += "      self.rightoffsetchar=liste[0][15]\n";
	func += "      self.initialfeatures=liste[0][16]\n";
	func += "      self.pos=[]\n";
	func += "      self.lemma=[]\n";
	func += "      self.features=[]\n";
	func += "      for ls in liste:\n";
	func += "          self.pos.append(ls[1])\n";
	func += "          self.lemma.append(ls[3])\n";
	func += "          self.features.append(ls[4])\n";
	func += "  else:\n";
	func += "      self.pos=liste[1]\n";
	func += "      self.surface=liste[2]\n";
	func += "      self.lemma=liste[3]\n";
	func += "      self.features=liste[4]\n";
	func += "      self.parent=liste[5]\n";
	func += "      self.daughter=liste[6]\n";
	func += "      self.last=liste[7]\n";
	func += "      self.next=liste[8]\n";
	func += "      self.previous=liste[9]\n";
	func += "      self.left=liste[10]\n";
	func += "      self.right=liste[11]\n";
	func += "      self.leftoffset=liste[12]\n";
	func += "      self.rightoffset=liste[13]\n";
	func += "      self.leftoffsetchar=liste[14]\n";
	func += "      self.rightoffsetchar=liste[15]\n";
	func += "      self.initialfeatures=liste[16]\n";
	func += " def Next(self):\n";
	func += "  return XipNode(self.next,self.grm)\n";
	func += " def Previous(self):\n";
	func += "  return XipNode(self.previous,self.grm)\n";
	func += " def Last(self):\n";
	func += "  return XipNode(self.last,self.grm)\n";
	func += " def Parent(self):\n";
	func += "  return XipNode(self.parent,self.grm)\n";
	func += " def Daughter(self):\n";
	func += "  return XipNode(self.daughter,self.grm)\n";
	func += " def __init__(self,i,g=-1):\n";
	func += "  if g==-1:\n";
	func += "     self.grm=XipNode.parseur\n";
	func += "  else:\n";
	func += "     self.grm=g\n";
	func += "  self.surface=\"\"\n";
	func += "  self.lemma=\"\"\n";
	func += "  self.pos=\"\"\n";
	func += "  self.features=\"\"\n";
	func += "  self.parent=-1\n";
	func += "  self.daughter=-1\n";
	func += "  self.last=-1\n";
	func += "  self.next=-1\n";
	func += "  self.previous=-1\n";
	func += "  self.index=-1\n";
	func += "  self.left=-1\n";
	func += "  self.right=-1\n";
	func += "  self.leftoffset=-1\n";
	func += "  self.rightoffset=-1\n";
	func += "  self.leftoffsetchar=-1\n";
	func += "  self.rightoffsetchar=-1\n";
	func += "  self.initialfeatures=\"\"\n";
	func += "  self.getnode(i)\n\n";
	func += "class XipDependency:\n";
	func += " parseur=0\n";
	func += " def getdependency(self,i):\n";
	func += "  if i==-1:\n";
	func += "   return 0\n";
	func += "  liste=dependency(i,self.grm)\n";
	func += "  self.index=i\n";
	func += "  self.name=liste[0]\n";
	func += "  self.features=liste[1]\n";
	func += "  for i in liste[2:]:\n";
	func += "   self.parameters.append(i)\n";
	func += " def __init__(self,i,g=-1):\n";
	func += "  if g==-1:\n";
	func += "     self.grm=XipNode.parseur\n";
	func += "  else:\n";
	func += "     self.grm=g\n";
	func += "  self.name=\"\"\n";
	func += "  self.features=\"\"\n";
	func += "  self.parameters=[]\n";
	func += "  self.index=-1\n";
	func += "  self.getdependency(i)\n";

	return func;
}

string insere_xip_objects(char glob) {
	string func = "";
	func += "XIP_DISPLAY_LEMMA=1\n";
	func += "XIP_DISPLAY_SURFACE=2\n";
	func += "XIP_DISPLAY_MARKUP=4\n";
	func += "XIP_DISPLAY_ENTREE=8\n";
	func += "XIP_DISPLAY_CATEGORY=16\n";
	func += "XIP_DISPLAY_REDUCED=32\n";
	func += "XIP_DISPLAY_FULL=64\n";
	func += "XIP_DISPLAY_OFFSET=128\n";
	func += "XIP_DISPLAY_WORDNUM=256\n";
	func += "XIP_DISPLAY_SENTENCE=512\n";
	func += "XIP_DISPLAY_NONE=16384\n";
	func += "XIP_DISPLAY_DEPENDENCY_BY_NAME=32768\n";
	func += "XIP_DISPLAY_DEPENDENCY_BY_NODE=65536\n";
	func += "XIP_DISPLAY_DEPENDENCY_BY_CREATION=131072\n";
	func += "XIP_DISPLAY_TREE=262144\n";
	func += "XIP_DISPLAY_TREE_PHRASE=524288\n";
	func += "XIP_DISPLAY_TREE_COLUMN=1048576\n";
	func += "XIP_DISPLAY_MERGE_XML_SUBTREE=2097152\n";
	func += "XIP_DISPLAY_CONVERSION_UTF8=4194304\n";
	func += "XIP_DISPLAY_EXECUTION_ERROR=8388608\n";
	func += "XIP_DISPLAY_MATHEMATICS=16777216\n";
	func += "XIP_DISPLAY_DEPENDENCY_NUMBER=33554432\n";
	func += "XIP_UTF8_INPUT=67108864\n";
	func += "XIP_DISPLAY_EXECUTE_TOKEN=134217728\n";
	func += "XIP_DISPLAY_SENTENCE_NUMBER=268435456\n";
	func += "XIP_LANGUAGE_GUESSER=536870912\n";
	func += "XIP_UTF8_LEXICON=1073741824\n";
	func += "XIP_DISPLAY_CHUNK_TREE=2147483648\n";
	func += "XIP_DISPLAY_DEPENDENCY_FEATURE_VALUE=4294967296\n";
	func += "XIP_LOWER_INPUT=17179869184\n";
	func += "XIP_TEST_UTF8=34359738368\n";
	func += "XIP_DISPLAY_GENERATION_CATEGORY=68719476736\n";
	func += "XIP_DISPLAY_GENERATION=137438953472\n";
	func += "XIP_RANDOM_ANALYSIS=274877906944\n";
	/** Start MCR added code for passage XML output **/
	func += "XIP_DISPLAY_PASSAGE_XML_FORMAT=549755813888\n";
	/** End MCR added code for passage XML output **/
	func += "XIP_DISPLAY_JSON=1099511627776\n";
	func += "XIP_DISPLAY_JSON_API=2199023255552\n";
	func += "XIP_DISPLAY_COMPUTE_DISTANCE=4398046511104\n";
	func += "XIP_DISPLAY_CLOSING_POS=8796093022208\n";
	func += "XIP_DISPLAY_TREEBANK=17592186044416\n\n";
#if PY_MAJOR_VERSION == 2
	func += insere_xip_class();
#endif

	return func;
}

void retourInitialParseur(int precedent) {
	parseur_courant->vpython_echange->echange=NULL;
	if (precedent!=index_parseur_courant) {
		parseur_courant->actif=NON;
		parseur_courant=gVecteurParseurs[precedent];
		index_parseur_courant=precedent;
		if (parseur_courant!=NULL)
			parseur_courant->actif=OUI;
	}
}

GlobalParseur* initNouveauParseur(int parsnum) {
	if (parsnum!=index_parseur_courant) {
		GlobalParseur* parseur=gVecteurParseurs[parsnum];
		if (parseur==NULL)
			return NULL;

		if (parseur_courant!=NULL)
			parseur_courant->actif=NON;

		parseur_courant=parseur;
		parseur->actif=OUI;
		index_parseur_courant=parsnum;
	}
	return parseur_courant;
}

 char extraitParseurCourant(PyObject* args) {
	int parsnum=index_parseur_courant;

	if (!PyArg_ParseTuple(args,"|i",&parsnum))
		return NON;

	GlobalParseur* parseur=initNouveauParseur(parsnum);
	if (parseur==NULL)
		return NON;

	return OUI;
}

 void Ajoutenoeud(PyObject* liste,XipNode* node,map<int,PyObject*>& noeuds,int i) {
	 PyObject* noeud=PyDict_New();
	 PyList_Insert(liste,i,noeud);
	 Py_DECREF(noeud);

	 PyObject* pos=ConvertToPythonString(node->category.c_str());
	 PyDict_SetItemString(noeud, "#pos", pos);
	 Py_DECREF(pos);

	 pos=ConvertToPythonLong(node->idnode);
	 PyDict_SetItemString(noeud, "#id", pos);
	 Py_DECREF(pos);
	 noeuds[node->idnode]=noeud;

	 int j;
	 for (j=0;j<node->features.size();j++) {
		 pos=ConvertToPythonString(node->features[j]->value.c_str());
		 PyDict_SetItemString(noeud, node->features[j]->attribute.c_str(), pos);
		 Py_DECREF(pos);
	 }

	 pos=ConvertToPythonLong(node->left);
	 PyDict_SetItemString(noeud, "#left", pos);
	 Py_DECREF(pos);
	 pos=ConvertToPythonLong(node->right);
	 PyDict_SetItemString(noeud, "#right", pos);
	 Py_DECREF(pos);
	 if (node->leftchar!=node->left) {
		 pos=ConvertToPythonLong(node->leftchar);
		 PyDict_SetItemString(noeud, "#leftchar", pos);
		 Py_DECREF(pos);
	 }
	 if (node->rightchar!=node->right) {
		 pos=ConvertToPythonLong(node->rightchar);
		 PyDict_SetItemString(noeud, "#rightchar", pos);
		 Py_DECREF(pos);
	 }

	 if (node->Type()==XIPLEAF) {
		 XipLeaf* feuille=(XipLeaf*)node;
		 pos=ConvertToPythonString(feuille->lemma.c_str());
		 PyDict_SetItemString(noeud, "#lemma", pos);
		 Py_DECREF(pos);
		 pos=ConvertToPythonString(feuille->surface.c_str());
		 PyDict_SetItemString(noeud, "#surface", pos);
		 Py_DECREF(pos);
		 return;
	 }

	 if (node->daughters.size()) {
		 PyObject* liste=PyList_New(0);
		 PyDict_SetItemString(noeud, "#children", liste);
		 Py_DECREF(liste);
		 for (j=0;j<node->daughters.size();j++)
			 Ajoutenoeud(liste,node->daughters[j],noeuds,j);
	 }
 }

 void Ajoutedependancepython(PyObject* liste,map<int,PyObject*>& noeuds,XipDependency* d,int i) {
	 PyObject* dep=PyDict_New();
	 PyList_Insert(liste,i,dep);
	 Py_DECREF(dep);
	 PyObject* pos=ConvertToPythonString(d->name.c_str());
	 PyDict_SetItemString(dep, "#name", pos);
	 Py_DECREF(pos);
	 int j;
	 bool stacks=false;
	 for (j=0;j<d->features.size();j++) {
		 if (d->features[j]->attribute=="$STACK") {
			 stacks=true;
			 continue;
		 }
		 pos=ConvertToPythonString(d->features[j]->value.c_str());
		 PyDict_SetItemString(dep, d->features[j]->attribute.c_str(), pos);
		 Py_DECREF(pos);
	 }

	 if (stacks) {
		 PyObject* thestack=PyList_New(0);
		 for (j=0;j<d->features.size();j++) {
			 if (d->features[j]->attribute=="$STACK") {
				 pos=ConvertToPythonString(d->features[j]->value.c_str());
				 PyList_Insert(thestack,j,pos);
			 }
		 }
		 PyDict_SetItemString(dep,"$STACK",thestack);
		 Py_DECREF(thestack);
	 }

	 PyObject* params=PyList_New(0);
	 PyDict_SetItemString(dep, "#parameters", params);
	 Py_DECREF(params);

	 for (j=0;j<d->parameters.size();j++) {
		 pos=noeuds[d->parameters[j]->idnode];
		 PyList_Insert(params,j,pos);
	 }
 }

 bool RajouteSurfaceLemme(XipNode* node, string& surface, string& lemme, bool& premier) {

	 if (node->Type() == XIPLEAF) {
		 XipLeaf* feuille = (XipLeaf*)node;
		 if (!premier) {
			 surface += " ";
			 lemme += " ";
		 }
		 else
			 premier = false;
		 surface += feuille->surface.c_str();
		 lemme += feuille->lemma.c_str();
		 return false;
	 }

	 for (int j = 0; j < node->daughters.size(); j++) {
		 if (!RajouteSurfaceLemme(node->daughters[j], surface, lemme, premier))
			 break;
	 }

	 return true;
 }

 void Ajouteunnoeud(PyObject* liste, XipNode* node, int i) {
	 PyObject* noeud = PyDict_New();
	 PyList_Insert(liste, i, noeud);
	 Py_DECREF(noeud);

	 PyObject* pos = ConvertToPythonString(node->category.c_str());
	 PyDict_SetItemString(noeud, "#pos", pos);
	 Py_DECREF(pos);

	 pos = ConvertToPythonLong(node->idnode);
	 PyDict_SetItemString(noeud, "#id", pos);
	 Py_DECREF(pos);

	 int j;
	 for (j = 0; j<node->features.size(); j++) {
		 pos = ConvertToPythonString(node->features[j]->value.c_str());
		 PyDict_SetItemString(noeud, node->features[j]->attribute.c_str(), pos);
		 Py_DECREF(pos);
	 }

	 pos = ConvertToPythonLong(node->left);
	 PyDict_SetItemString(noeud, "#left", pos);
	 Py_DECREF(pos);
	 pos = ConvertToPythonLong(node->right);
	 PyDict_SetItemString(noeud, "#right", pos);
	 Py_DECREF(pos);
	 if (node->leftchar != node->left) {
		 pos = ConvertToPythonLong(node->leftchar);
		 PyDict_SetItemString(noeud, "#leftchar", pos);
		 Py_DECREF(pos);
	 }
	 if (node->rightchar != node->right) {
		 pos = ConvertToPythonLong(node->rightchar);
		 PyDict_SetItemString(noeud, "#rightchar", pos);
		 Py_DECREF(pos);
	 }

	 string surface;
	 string lemme;
	 bool premier = true;
	 RajouteSurfaceLemme(node, surface, lemme, premier);
	 pos = ConvertToPythonString(lemme.c_str());
	 PyDict_SetItemString(noeud, "#lemma", pos);
	 Py_DECREF(pos);
	 pos = ConvertToPythonString(surface.c_str());
	 PyDict_SetItemString(noeud, "#surface", pos);
	 Py_DECREF(pos);
 }

 void Ajoutedependanceseulepython(PyObject* liste, XipDependency* d, int i) {
	 PyObject* dep = PyDict_New();
	 PyList_Insert(liste, i, dep);
	 Py_DECREF(dep);
	 PyObject* pos = ConvertToPythonString(d->name.c_str());
	 PyDict_SetItemString(dep, "#name", pos);
	 Py_DECREF(pos);
	 int j;
	 bool stacks = false;
	 for (j = 0; j<d->features.size(); j++) {
		 if (d->features[j]->attribute == "$STACK") {
			 stacks = true;
			 continue;
		 }
		 pos = ConvertToPythonString(d->features[j]->value.c_str());
		 PyDict_SetItemString(dep, d->features[j]->attribute.c_str(), pos);
		 Py_DECREF(pos);
	 }

	 if (stacks) {
		 PyObject* thestack = PyList_New(0);
		 for (j = 0; j<d->features.size(); j++) {
			 if (d->features[j]->attribute == "$STACK") {
				 pos = ConvertToPythonString(d->features[j]->value.c_str());
				 PyList_Insert(thestack, j, pos);
			 }
		 }
		 PyDict_SetItemString(dep, "$STACK", thestack);
		 Py_DECREF(thestack);
	 }

	 PyObject* params = PyList_New(0);
	 PyDict_SetItemString(dep, "#parameters", params);
	 Py_DECREF(params);

	 for (j = 0; j < d->parameters.size(); j++)
		 Ajouteunnoeud(params, d->parameters[j], j);
 }

 void Ajouteunitpython(GlobalParseur* parseur, PyObject* unit, XipUnit* xunit, int i) {
	 PyObject* liste=PyList_New(0);
	 PyList_Insert(unit,i,liste);
	 Py_DECREF(liste);
	 if (parseur_courant->TestOptions(AFFICHAGE_ARBRE_CHUNK)) {
		 map<int, PyObject*> noeuds;
		 Ajoutenoeud(liste, xunit->root, noeuds, 1);
		 for (int j = 0; j < xunit->dependencies.size(); j++)
			 Ajoutedependancepython(liste, noeuds, xunit->dependencies[j], j + 2);
	 }
	 else {
		 for (int j = 0; j < xunit->dependencies.size(); j++)
			 Ajoutedependanceseulepython(liste, xunit->dependencies[j], j);
	 }

	 if (parseur->TestOptions(AFFICHAGE_CALCUL_DISTANCE) == OUI && parseur->distances.size()>i) {
		 hmap<int, hmap<int, int> >::iterator it;
		 hmap<int, int>::iterator itsub;
		 PyObject* obj;
		 char buff[100];
		 PyObject* dep = PyDict_New();
		 PyList_Insert(liste, PyList_Size(liste), dep);
		 Py_DECREF(dep);
		 for (it = parseur->distances[i].begin(); it != parseur->distances[i].end(); it++) {
			 sprintf_s(buff, 100, "%d", it->first);
			 PyObject* depsub = PyDict_New();
			 PyDict_SetItemString(dep, buff, depsub);
			 Py_DECREF(depsub);
			 for (itsub = it->second.begin(); itsub != it->second.end(); itsub++) {
				 sprintf_s(buff, 100, "%d", itsub->first);
				 obj = ConvertToPythonLong(itsub->second);
				 PyDict_SetItemString(depsub, buff, obj);
				 Py_DECREF(obj);
			 }
		 }
		 dep = PyDict_New();
		 PyList_Insert(liste, PyList_Size(liste), dep);
		 Py_DECREF(dep);
		 for (it = parseur->distancesarbre[i].begin(); it != parseur->distancesarbre[i].end(); it++) {
			 sprintf_s(buff, 100, "%d", it->first);
			 PyObject* depsub = PyDict_New();
			 PyDict_SetItemString(dep, buff, depsub);
			 Py_DECREF(depsub);
			 for (itsub = it->second.begin(); itsub != it->second.end(); itsub++) {
				 sprintf_s(buff, 100, "%d", itsub->first);
				 obj = ConvertToPythonLong(itsub->second);
				 PyDict_SetItemString(depsub, buff, obj);
				 Py_DECREF(obj);
			 }
		 }
	 }
 }

 void Creeresultatpython(GlobalParseur* parseur, PyObject* resultat, XipResult* result) {
	 for (int i = 0; i < result->sentences.size(); i++)
		 Ajouteunitpython(parseur, resultat, result->sentences[i], i);
 }

//Une fonction de base de python pour pouvoir jouer avec les noeuds
extern "C" {


	static PyObject* xipsError;

    static PyObject* xips_loadgrm(PyObject* self, PyObject* args) {
        char* grmfile;

#if PY_MAJOR_VERSION >= 3
		if (initialization) {
			string func = insere_xip_class();
			PyRun_SimpleString(func.c_str());
			initialization = false;
		}
#endif

		if (!PyArg_ParseTuple(args,"s",&grmfile)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}

		ostringstream os;
		flot_erreur=&os;
        int ipar=-1;
        try {
           ipar=XipGrmFile(grmfile,1,0);
        }
        catch(XIPError* msg) {
			flot_erreur=&cerr;
			PyErr_SetString(PyExc_AttributeError, msg->message.c_str());
            return ConvertToPythonString(msg->message.c_str());
        }

		flot_erreur=&cerr;
        return ConvertToPythonLong(ipar);
    }

    static PyObject* xips_freegrm(PyObject* self, PyObject* args) {
        int parsnum=index_parseur_courant;

		if (!PyArg_ParseTuple(args,"i",&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}
		XipFreeGrammar(parsnum);
		return ConvertToPythonLong(0);
	}


    static PyObject* xips_current_rule(PyObject* self, PyObject* args) {
		int precedent=index_parseur_courant;

		if (extraitParseurCourant(args)==NON) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return Py_BuildValue("[iiisssll]",-1,-1,-1,"","","",-1,-1);
		}

		if (parseur_courant->reglecourante==NULL) {
			PyErr_SetString(PyExc_AttributeError, "No Current Rule");
			return Py_BuildValue("[iiisssll]",-1,-1,-1,"","","",-1,-1);
		}

		return Py_BuildValue("[iiisssll]",
			parseur_courant->reglecourante->identificateur,
			parseur_courant->reglecourante->numero,
			parseur_courant->reglecourante->couche,
			champs_anglais[parseur_courant->reglecourante->label->type],
			parseur_courant->reglecourante->label->etiquette,
			parseur_courant->reglecourante->label->dr->nom_fichier,
			parseur_courant->reglecourante->label->dr->position,
			parseur_courant->reglecourante->label->dr->ligne);
    }

    static PyObject* xips_current_rule_text(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int display;

		if (!PyArg_ParseTuple(args,"i|i",&display,&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonString("");
		}

		GlobalParseur* parseur=initNouveauParseur(parsnum);

		if (parseur==NULL || parseur_courant->reglecourante==NULL) {
			PyErr_SetString(PyExc_AttributeError, "No Current Rule");
			return ConvertToPythonString("");
		}

		ostringstream os;
		parseur_courant->FormatRuleText(os,parseur_courant->reglecourante->identificateur,display);
		return ConvertToPythonString(os.str().c_str());
    }


    static PyObject* xips_setexechange(PyObject* self, PyObject* args) {

        PyObject* objet;
        char* nomvariable;
		if (!PyArg_ParseTuple(args,"sO",&nomvariable,&objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}

        etat* e=parseur_courant->VariablesNoms.recherche(nomvariable);
		if (e==NULL || e->valeur==NULL) {
			PyErr_SetString(PyExc_AttributeError, "Unknown variable");
            return ConvertToPythonLong(-2);
		}

        Variable* v=(Variable*)e->valeur;
		if (v->sorte!=VPYTHONOBJET) {
			PyErr_SetString(PyExc_AttributeError, "This object is not a Python object");
            return ConvertToPythonLong(-3);
		}

        Py_INCREF(objet);
        VariablePython* vp=(VariablePython*)v;
        if (vp->echange!=NULL)
            Py_DECREF(vp->echange);
        vp->echange=objet;
        return ConvertToPythonLong(0);
    }

	static PyObject* xips_setdebug(PyObject* self, PyObject* args) {

		GlobalParseur* parseur=parseur_courant;
		if (parseur==NULL)
			return ConvertToPythonLong(-2);
		int numligne,nummodule;
		if (!PyArg_ParseTuple(args,"ii",&nummodule,&numligne)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		if (nummodule==-1 && numligne==-1)
			parseur->python_debug=NON;
		else
			parseur->python_debug=OUI;

		parseur->num_python_ligne=numligne;
		parseur->num_fichier_python=nummodule;

        return ConvertToPythonLong(0);
    }

    static PyObject* xips_reloadgrm(PyObject* self, PyObject* args) {
        int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

		if (!PyArg_ParseTuple(args,"i",&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonString("Please provide a grammar handle");
		}

        try {
           XipReloadGrammar(parsnum,1,NULL);
        }
        catch(XIPError* msg) {
			PyErr_SetString(PyExc_AttributeError, msg->message.c_str());
            return ConvertToPythonString(msg->message.c_str());
        }

        return ConvertToPythonLong(0);
    }

	bool PythonVersString(PyObject* po,string& s) {
		if (PyCheck_String(po)==1) {
			s=PyAsString(po);
			return true;
		}

#ifdef HAVE_WCHAR_H
		if (PyUnicode_Check(po)) {
			Py_ssize_t sz=PyUnicode_GetSize(po);
			Py_UNICODE* ibuff=PyUnicode_AsUnicode(po);
			for (int i=0;i<sz;i++)
				s+=s_unicode_to_utf8(ibuff[i]);
			return true;
		}
#else
		if (PyUnicode_Check(po)) {
			Py_ssize_t sz=PyUnicode_GET_DATA_SIZE(po);
			const char* sbuff=PyUnicode_AS_DATA(po);
			for (Py_ssize_t i=0;i<sz;i++) {
				if (sbuff[i])
					s+=sbuff[i];
			}
			return true;
		}
#endif
		return false;
	}

    static PyObject* xips_parsestring(PyObject* self, PyObject* args) {
		int precedent=index_parseur_courant;
        int parsnum=index_parseur_courant;

        int nbelement=PyTuple_Size(args);
		if (nbelement!=2 && nbelement!=3) {
			PyErr_SetString(PyExc_AttributeError, "Wrong number of parameters");
			return ConvertToPythonLong(-1);
		}

		PyObject* pelement=PyTuple_GetItem(args, 0);
		if (PyCheck_Number(pelement)!=1) {
			PyErr_SetString(PyExc_AttributeError, "First element should be a integer");
			return ConvertToPythonLong(-1);
		}

		parsnum=PyLong_AsLong(pelement);
        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(-1);


		pelement=PyTuple_GetItem(args, 1);
        string phrase;
		if (!PythonVersString(pelement,phrase)) {
			PyErr_SetString(PyExc_AttributeError, "Second element should be a string");
			return ConvertToPythonLong(-1);
		}


        PyObject* objet=NULL;
		if (nbelement==3)
			objet=PyTuple_GetItem(args, 2);

		XipResult* xip=new XipResult;
        parseur_courant->vpython_echange->echange=objet;
		XipParseString(STR(phrase),parsnum, xip,1);
		PyObject* resultat=PyList_New(0);
		Creeresultatpython(parseur, resultat, xip);
		delete xip;
        retourInitialParseur(precedent);
        return resultat;
    }

	static PyObject* xips_stringtagging(PyObject* self, PyObject* args) {
		int precedent=index_parseur_courant;
		int parsnum=index_parseur_courant;

		int nbelement=PyTuple_Size(args);
		if (nbelement!=2) {
			PyErr_SetString(PyExc_AttributeError, "Wrong number of parameters");
			return ConvertToPythonLong(-1);
		}

		PyObject* pelement=PyTuple_GetItem(args, 0);
		if (PyCheck_Number(pelement)!=1) {
			PyErr_SetString(PyExc_AttributeError, "First element should be a integer");
			return ConvertToPythonLong(-1);
		}

		parsnum=PyLong_AsLong(pelement);
		GlobalParseur* parseur=initNouveauParseur(parsnum);
		if (parseur==NULL)
			return ConvertToPythonLong(-1);


		pelement=PyTuple_GetItem(args, 1);
		string phrase;
		if (!PythonVersString(pelement,phrase)) {
			PyErr_SetString(PyExc_AttributeError, "Second element should be a string");
			return ConvertToPythonLong(-1);
		}

		XipAddFlagDisplayMode(parsnum,AFFICHAGE_ARBRE_CHUNK|AFFICHAGE_COMPLET | AFFICHAGE_ENTREE,80);
		char vtag = parseur_courant->DESAMBIGUISATION;
		char vtagd = parseur_courant->DESAMBIGUISATIONDefini;
		char vtg = parseur_courant->TAGGING;

		parseur_courant->Desambiguisation(OUI);
		parseur_courant->Tagging(OUI);


		XipResult* xip=new XipResult;
		parseur_courant->vpython_echange->echange=NULL;
		XipParseString(STR(phrase),parsnum, xip, 0);
		PyObject* resultat=PyList_New(0);

		delete xip;
		retourInitialParseur(precedent);
		parseur_courant->DESAMBIGUISATION = vtag;
		parseur_courant->DESAMBIGUISATIONDefini = vtagd;
		parseur_courant->TAGGING = vtg;

		return resultat;
	}

	static PyObject* xips_parsedependencystring(PyObject* self, PyObject* args) {
        char* phrase;
        int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

        PyObject* objet;
		if (!PyArg_ParseTuple(args,"isO",&parsnum,&phrase,&objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}


        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(-1);

        parseur_courant->vpython_echange->echange=objet;

		ostringstream os;
		int res=XipGenerateFromString(phrase,parsnum,&os);
        retourInitialParseur(precedent);
		if (res==0)
			return ConvertToPythonString(os.str().c_str());
		else {
			PyErr_SetString(PyExc_AttributeError, affichagemessageerreur[res]);
			return ConvertToPythonString(affichagemessageerreur[res]);
		}
    }

	static PyObject* xips_generated(PyObject* self, PyObject* args) {
		int precedent=index_parseur_courant;
        int parsnum=index_parseur_courant;

		if (extraitParseurCourant(args)==NON)  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		string buff;
		parseur_courant->parcoursgeneration(buff);

        return ConvertToPythonString(buff.c_str());
    }

#ifdef XIPLIBXML
    static PyObject* xips_parsexmlstring(PyObject* self, PyObject* args) {
        char* phrase;
        int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
        int depth;

        PyObject* objet;
		if (!PyArg_ParseTuple(args,"isiO",&parsnum,&phrase,&depth,&objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(-1);


        //Py_INCREF(objet);
        parseur_courant->vpython_echange->echange=objet;
        ostringstream os;
        XipParseStringXMLOS(phrase,parsnum, &os,depth,parseur->XML,OUI);
        retourInitialParseur(precedent);
        return ConvertToPythonString(os.str().c_str());
    }

	static PyObject* xips_parsexmlfile(PyObject* self, PyObject* args) {
		char* filename;
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int depth=1;

		PyObject* objet;
		if (!PyArg_ParseTuple(args,"isiO",&parsnum,&filename,&depth,&objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		GlobalParseur* parseur=initNouveauParseur(parsnum);
		if (parseur==NULL)
			return ConvertToPythonLong(-1);


		//Py_INCREF(objet);
		parseur_courant->vpython_echange->echange=objet;
		//ostringstream os;
		XipParseFileXMLOS(filename,parsnum, NULL,depth,parseur->XML);
		retourInitialParseur(precedent);
		return ConvertToPythonString("end");
	}
#endif

    static PyObject* xips_version(PyObject* self, PyObject* args) {
        char version[1000];

         sprintf_s(version,1000,"XIP: Xerox Incremental Parser %s (2000-2007) ",VERSION_XIP_BUILD);
        return ConvertToPythonString(version);
    }

    static PyObject* xips_parsefile(PyObject* self, PyObject* args) {
        char* filename;
        int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

        PyObject* objet;
		if (!PyArg_ParseTuple(args,"isO",&parsnum,&filename,&objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(-1);


        //Py_INCREF(objet);
        parseur_courant->vpython_echange->echange=objet;
        //ostringstream os;
        XipParseFileOS(filename,parsnum, NULL,parseur->XML);
        retourInitialParseur(precedent);
        return ConvertToPythonString("end");
    }

	static PyObject* xips_parsedependencyfile(PyObject* self, PyObject* args) {
        char* filename;
        int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

        PyObject* objet;
		if (!PyArg_ParseTuple(args,"isO",&parsnum,&filename,&objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(-1);


		parseur_courant->vpython_echange->echange=objet;

		int res=XipGenerateFromFile(filename,parsnum, NULL);
        retourInitialParseur(precedent);
		if (res==0)
			return ConvertToPythonString("end");
		else {
			PyErr_SetString(PyExc_AttributeError, affichagemessageerreur[res]);
			return ConvertToPythonString(affichagemessageerreur[res]);
		}
    }

    static PyObject* xips_xipnode(PyObject* self, PyObject* args) {
        int node;
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

        if (!PyArg_ParseTuple(args,"i|i",&node,&parsnum))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return Py_BuildValue("[issssiiiiiiilllls]",0,
				"NONE","NONE","NONE","[NONE:NONE]",-1,-1,-1,-1,-1,0,0,-1,-1,-1,-1,"");
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL) {
            return Py_BuildValue("[issssiiiiiiilllls]",0,
                "NONE","NONE","NONE","[NONE:NONE]",-1,-1,-1,-1,-1,0,0,-1,-1,-1,-1,"");
        }

        Noeud* n=NULL;
        //if ((*parseur->TasDeListes)[node]!=NULL)
        //    n=&parseur->TasDeListes->vecteur[node]->N;

        if (parseur->NoeudsTemporaires[node]!=NULL)
            n=&parseur->NoeudsTemporaires[node]->N;


        if (n==NULL) {
			PyErr_SetString(PyExc_IndexError, "This index does not match any syntactic node");
            return Py_BuildValue("[issssiiiiiiilllls]",0,
                "NONE","NONE","NONE","[NONE:NONE]",-1,-1,-1,-1,-1,0,0,-1,-1,-1,-1,"");
        }

        string rpos;
        string rsurface;
        string rlemme;
        string rtraits;
		string rinitialfeatures;
        int rpere=-1;
        int rfils=-1;
        int rdernier=-1;
        int rsuivant=-1;
        int rprecedent=-1;
		PyObject* listeinter;
		PyObject* laliste=NULL;
		//dans le cas standard, il s'agit d'une simple concatenation dans les chaines pour les noeuds lexicaux
		if (parseur->fullpythonlexical==NON || n->source->voc==NULL || n->source->voc->nbX==1) {
			n->ConstruitUnNoeud(parseur,
				rpos,
				rsurface,
				rlemme,
				rtraits,
				rinitialfeatures,
				rpere,
				rfils,
				rdernier,
				rsuivant,
				rprecedent);

			listeinter=Py_BuildValue("[issssiiiiiiilllls]",
				parseur->fullpythonlexical,
				(char*)rpos.c_str(),
				(char*)rsurface.c_str(),
				(char*)rlemme.c_str(),
				(char*)rtraits.c_str(),
				rpere,
				rfils,
				rdernier,
				rsuivant,
				rprecedent,
				n->num_gauche,
				n->num_droite,
				n->gauche,
				n->droite,
				n->gauchecar,
				n->droitecar,
				(char*)rinitialfeatures.c_str());

			if (parseur->fullpythonlexical==OUI) {
				laliste=PyList_New(0);
				PyList_Insert(laliste,0,listeinter);
				Py_DECREF(listeinter);
			}
			else
				laliste=listeinter;
		}
		else {//sinon, on cree une liste de noeuds lexicaux
			int choix=n->source->choisie;

			laliste=PyList_New(0);
			for (int i=0;i<n->source->voc->nbX;i++) {
				n->source->choisie=i;
				n->ConstruitUnNoeud(parseur,
					rpos,
					rsurface,
					rlemme,
					rtraits,
					rinitialfeatures,
					rpere,
					rfils,
					rdernier,
					rsuivant,
					rprecedent);

				listeinter=Py_BuildValue("[issssiiiiiiilllls]",
					1,
					(char*)rpos.c_str(),
					(char*)rsurface.c_str(),
					(char*)rlemme.c_str(),
					(char*)rtraits.c_str(),
					rpere,
					rfils,
					rdernier,
					rsuivant,
					rprecedent,
					n->num_gauche,
					n->num_droite,
					n->gauche,
					n->droite,
					n->gauchecar,
					n->droitecar,
					(char*)rinitialfeatures.c_str());
				PyList_Insert(laliste,i,listeinter);
				Py_DECREF(listeinter);
			}
			n->source->choisie=choix;
		}
		return laliste;
	}


    static PyObject* xips_xipnodes(PyObject* self, PyObject* args) {
        char* node;
        int i,parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

        PyObject* liste=PyList_New(0);
		if (!PyArg_ParseTuple(args,"s|i",&node,&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return liste;
		}

		GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return liste;

        Categorie* c=parseur->RetrouveCat(node);
		if (c==NULL) {
			PyErr_SetString(PyExc_IndexError, "Unknown POS");
            return liste;
		}

        for (i=0; i< parseur->IndexParCat[c->X.val].dernier;i++) {
            PyObject* pcourant=ConvertToPythonLong(parseur->IndexParCat[c->X.val][i]->N.num_noeud);
            PyList_Insert(liste,i,pcourant);
			Py_DECREF(pcourant);
        }

        return liste;
    }

    static PyObject* xips_xiplexicalnodes(PyObject* self, PyObject* args) {
        int i;
		int precedent=index_parseur_courant;

        PyObject* liste=PyList_New(0);
		if (extraitParseurCourant(args)==NON) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return liste;
		}

        for (i=0; i< parseur_courant->NoeudsLexicaux.dernier;i++) {
            PyObject* pcourant=ConvertToPythonLong(parseur_courant->NoeudsLexicaux[i]->num_noeud);
            PyList_Insert(liste,i,pcourant);
			Py_DECREF(pcourant);
        }

        return liste;
    }

    static PyObject* xips_xipfictivenodes(PyObject* self, PyObject* args) {
        int i;
        int precedent=index_parseur_courant;

        PyObject* liste=PyList_New(0);
		if (extraitParseurCourant(args)==NON) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return liste;
		}

        Noeud* n=NULL;

        for (i=0; i< parseur_courant->NoeudsDependance.dernier;i++) {
            PyObject* pcourant=ConvertToPythonLong(parseur_courant->NoeudsDependance[i]->N.num_noeud);
            PyList_Insert(liste,i,pcourant);
			Py_DECREF(pcourant);
        }

        return liste;
    }

    static PyObject* xips_xipinternalstring(PyObject* self, PyObject* args) {

		int precedent=index_parseur_courant;
		if (extraitParseurCourant(args)==NON) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonString("");
		}

		if (parseur_courant->donneesATraiter==NULL) {
			PyErr_SetString(PyExc_IndexError, "No string to process");
            return ConvertToPythonString("");
		}

        return ConvertToPythonString((char*)parseur_courant->donneesATraiter);
    }

    static PyObject* xips_dependency(PyObject* self, PyObject* args) {
        int dep;
        int i,parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

		if (!PyArg_ParseTuple(args,"i|i",&dep,&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return Py_BuildValue("[ssi]","NONE","[NONE:NONE]",-1);
		}

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL) {
            return Py_BuildValue("[ssi]",
                "NONE","[NONE:NONE]",-1);
        }

        if (parseur->INTERFONCTIONS[0]==NULL) {
			PyErr_SetString(PyExc_IndexError, "Unknown dependency");
            return Py_BuildValue("[ssi]",
                "NONE","[NONE:NONE]",-1);
        }

        ResultatFonction* rf=(*parseur->INTERFONCTIONS[0])[dep];
        if (rf==NULL) {
			PyErr_SetString(PyExc_IndexError, "Unknown dependency");
            return Py_BuildValue("[ssi]",
                "NONE","[NONE:NONE]",-1);
        }


		string traits;
		if (rf->traits!=NULL)
			rf->traits->TraitsEnChaineString(parseur,traits,parseur->FILTREFONCTION);

		if (parseur->TestOptions(AFFICHAGE_NUMERO_DEPENDANCE)) {
			char num[10];
			traits+="<";
			for (i=0;i<rf->numeros.dernier;i++) {
				sprintf_s(num,10,"%d",parseur->VlabelVecteurIndex[rf->numeros[i]]->numero);
				if (i)
					traits+=",";
				traits+=num;
			}
			traits+=">";
		}

		for (i=0;i<rf->stack.dernier;i++) {
			if (i)
				traits+=",";
			else {
				traits=traits.substr(0,traits.size()-1);
				if (traits!="[")
					traits+=",";
			}
			traits+="stack:'";
			traits+=rf->stack[i];
			traits+="'";
		}

		if (rf->stack.dernier)
			traits+="]";


        PyObject* pcourant;
        PyObject* liste=PyList_New(0);
        int params=0;

        pcourant=ConvertToPythonString(rf->Fx->nom);
        PyList_Insert(liste,params++,pcourant);
		Py_DECREF(pcourant);

        pcourant=ConvertToPythonString(traits.c_str());
        PyList_Insert(liste,params++,pcourant);
		Py_DECREF(pcourant);

        for (i=0; i< rf->arg.dernier;i++) {
            pcourant=ConvertToPythonLong(rf->arg[i]->num_noeud);
            PyList_Insert(liste,params++,pcourant);
			Py_DECREF(pcourant);
        }

        return liste;
    }

	static PyObject* xips_setfloatvariable(PyObject* self, PyObject* args) {
        float valeur;
		char* variable;
        int grm;


		if (!PyArg_ParseTuple(args,"isf",&grm,&variable,&valeur))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}

		string var=variable;
		int r=XipSetVariable(grm,var,valeur);
        return ConvertToPythonLong(r);
    }

	static PyObject* xips_setintvariable(PyObject* self, PyObject* args) {
        int valeur;
		char* variable;
        int grm;

		if (!PyArg_ParseTuple(args,"isi",&grm,&variable,&valeur)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}

		string var=variable;
		int r=XipSetVariable(grm,var,(float)valeur);
        return ConvertToPythonLong(r);
    }

	static PyObject* xips_setstringvariable(PyObject* self, PyObject* args) {
        char* valeur;
		char* variable;
        int grm;

		if (!PyArg_ParseTuple(args,"iss",&grm,&variable,&valeur)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}

		string var=variable;
		string value=valeur;
		int r=XipSetStringVariable(grm,var,value);
        return ConvertToPythonLong(r);
    }


    static PyObject* xips_setdisplay(PyObject* self, PyObject* args) {
        TVecteur mode;
        int grm;

		if (!PyArg_ParseTuple(args,"iL",&grm,&mode)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}

        XipSetDisplayMode(grm,mode,80);

        return ConvertToPythonLong(0);
    }

	static PyObject* xips_adddisplay(PyObject* self, PyObject* args) {
        TVecteur mode;
        int grm;

		if (!PyArg_ParseTuple(args,"iL",&grm,&mode)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}

		XipAddFlagDisplayMode(grm,mode,80);

        return ConvertToPythonLong(0);
    }

	static PyObject* xips_testdisplay(PyObject* self, PyObject* args) {
        TVecteur mode;
        int grm;

		if (!PyArg_ParseTuple(args,"iL",&grm,&mode)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}

		if (XipTestFlagDisplayMode(grm,mode)==1)
			return ConvertToPythonLong(1);

        return ConvertToPythonLong(0);
    }

	static PyObject* xips_currentgrammarindex(PyObject* self, PyObject* args) {
        if (parseur_courant==NULL)
            return ConvertToPythonLong(-1);

        return ConvertToPythonLong(parseur_courant->iParseurCourant);
    }


	static PyObject* xips_removedisplay(PyObject* self, PyObject* args) {
        TVecteur mode;
        int grm;

		if (!PyArg_ParseTuple(args,"iL",&grm,&mode)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}

		XipRemoveFlagDisplayMode(grm,mode);

        return ConvertToPythonLong(0);
    }


    static PyObject* xips_dependencies(PyObject* self, PyObject* args) {
        char* dep;
        int i,parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
        PyObject* liste=PyList_New(0);

		if (!PyArg_ParseTuple(args,"s|i",&dep,&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return liste;
		}

		GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return liste;


        //On recupere donc nos dependances;
        Categorie* cdep=parseur->RetrouveFonc(dep);
        if (cdep==NULL)
            return liste;

        PyObject* pcourant;


        int idep=cdep->X.val;
		ResultatFonction* rf;
        if (parseur->INTERFONCTIONS[idep]!=NULL) {
            for (i=0;i<parseur->INTERFONCTIONS[idep]->dernier;i++) {
				rf=(*parseur->INTERFONCTIONS[idep])[i];
                if (rf!=NULL && rf->Fx!=NULL) {
                    pcourant=ConvertToPythonLong(rf->index);
                    PyList_Insert(liste,i,pcourant);
					Py_DECREF(pcourant);
                }
            }
        }

        return liste;
    }

    static PyObject* xips_dependencyonfirstnode(PyObject* self, PyObject* args) {
        int node,parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
        PyObject* liste=PyList_New(0);

		if (!PyArg_ParseTuple(args,"i|i",&node,&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return liste;
		}

		GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return liste;

        if (parseur->NoeudsTemporaires[node]==NULL)
            return liste;

        PyObject* pcourant;

        if (parseur->PREMIERFONCTIONS[node]!=NULL) {
            for (int i=0;i<parseur->PREMIERFONCTIONS[node]->dernier;i++) {
                if ((*parseur->PREMIERFONCTIONS[node])[i]!=NULL) {
                    pcourant=ConvertToPythonLong((*parseur->PREMIERFONCTIONS[node])[i]->index);
                    PyList_Insert(liste,i,pcourant);
					Py_DECREF(pcourant);
                }
            }
        }

        return liste;
    }

    static PyObject* xips_dependencyonsecondnode(PyObject* self, PyObject* args) {
        int node,parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

        PyObject* liste=PyList_New(0);

		if (!PyArg_ParseTuple(args,"i|i",&node,&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return liste;
		}

		GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return liste;

        if (parseur->NoeudsTemporaires[node]==NULL)
            return liste;

        PyObject* pcourant;

		if (parseur->SECONDFONCTIONS[node]!=NULL) {
			for (int i=0;i<parseur->SECONDFONCTIONS[node]->dernier;i++) {
				if ((*parseur->SECONDFONCTIONS[node])[i]!=NULL) {
					pcourant=ConvertToPythonLong((*parseur->SECONDFONCTIONS[node])[i]->index);
					PyList_Insert(liste,i,pcourant);
					Py_DECREF(pcourant);
				}
			}
		}
        return liste;
    }

    static PyObject* xips_get_sourcecharset(PyObject* self, PyObject* args) {
        int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

		if (!PyArg_ParseTuple(args,"i",&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonString("");
		}

		GlobalParseur* parseur=initNouveauParseur(parsnum);
		if (parseur==NULL) {
			PyErr_SetString(PyExc_AttributeError, "Unknown Grammar");
            return ConvertToPythonString("");
		}
		encodingType ty=XipGetSourceCharset(parsnum);
		if (ty==LATIN_ENCODING)
			return ConvertToPythonString("latin-1");
		return ConvertToPythonString("utf-8");
	}

	static PyObject* xips_get_grammarcharset(PyObject* self, PyObject* args) {
        int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

		if (!PyArg_ParseTuple(args,"i",&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonString("");
		}

		GlobalParseur* parseur=initNouveauParseur(parsnum);
		if (parseur==NULL) {
			PyErr_SetString(PyExc_AttributeError, "Unknown Grammar");
            return ConvertToPythonString("");
		}
		encodingType ty=XipGetGrammarCharset(parsnum);
		if (ty==LATIN_ENCODING)
			return ConvertToPythonString("latin-1");
		return ConvertToPythonString("utf-8");
	}

	static PyObject* xips_get_fstcharset(PyObject* self, PyObject* args) {
        int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

		if (!PyArg_ParseTuple(args,"i",&parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonString("");
		}

		GlobalParseur* parseur=initNouveauParseur(parsnum);
		if (parseur==NULL) {
			PyErr_SetString(PyExc_AttributeError, "Unknown Grammar");
            return ConvertToPythonString("");
		}
		if (parseur->encodage_fst==LATIN_ENCODING)
			return ConvertToPythonString("latin-1");
		return ConvertToPythonString("utf-8");
	}

    static PyObject* xips_metpoids(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int id;
		float value;

        if (!PyArg_ParseTuple(args,"iif",&parsnum,&id,&value))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(0);
		return ConvertToPythonLong(parseur->proba_metpoids(id,value));
	}

	static PyObject* xips_metseuil(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int id;
		float value;

        if (!PyArg_ParseTuple(args,"iif",&parsnum,&id,&value))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(0);





		return ConvertToPythonLong(parseur->proba_metseuil(id,value));
	}

	static PyObject* xips_metvaleur(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int id;
		float value;

        if (!PyArg_ParseTuple(args,"iif",&parsnum,&id,&value))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(0);





		return ConvertToPythonLong(parseur->proba_metvaleur(id,value));
	}

	static PyObject* xips_poids(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int id;

        if (!PyArg_ParseTuple(args,"ii",&parsnum,&id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(0);





		double value=parseur->proba_poids(id);
		return PyFloat_FromDouble(value);
	}

	static PyObject* xips_seuil(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int id;

        if (!PyArg_ParseTuple(args,"ii",&parsnum,&id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(0);





		double value=parseur->proba_seuil(id);
		return PyFloat_FromDouble(value);
	}

	static PyObject* xips_valeur(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int id;

        if (!PyArg_ParseTuple(args,"ii",&parsnum,&id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(0);





		double value=parseur->proba_valeur(id);
		return PyFloat_FromDouble(value);
	}

	static PyObject* xips_typeregle(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int id;

        if (!PyArg_ParseTuple(args,"ii",&parsnum,&id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);

        if (parseur==NULL || parseur->VBases[id]==NULL)
            return ConvertToPythonLong(-1);





		int ty=parseur->VBases[id]->label->type;
		return ConvertToPythonLong(ty);
	}

	static PyObject* xips_coucheregle(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int id;

        if (!PyArg_ParseTuple(args,"ii",&parsnum,&id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL || parseur->VBases[id]==NULL)
            return ConvertToPythonLong(-1);





		int ty=parseur->VBases[id]->couche;
		return ConvertToPythonLong(ty);
	}

	static PyObject* xips_compteurregle(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int id;

        if (!PyArg_ParseTuple(args,"ii",&parsnum,&id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL || parseur->VBases[id]==NULL)
            return ConvertToPythonLong(-1);


		int ty=parseur->VBases[id]->compteur;
		return ConvertToPythonLong(ty);
	}


	static PyObject* xips_loadkif(PyObject* self, PyObject* args) {
		char* filename;
		char* pargs;
		int mapping=0;
		int nbelements=PyTuple_Size(args);
		if (nbelements==2) {
			if (!PyArg_ParseTuple(args,"ss",&filename,&pargs)) {
				PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
				return ConvertToPythonLong(-1);
			}
		}
		else {
			if (!PyArg_ParseTuple(args,"ssi",&filename,&pargs,&mapping)) {
				PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
				return ConvertToPythonLong(-1);
			}
		}

		int kifhandler = KifExecuteProgram(filename, pargs, mapping);
		if (kifhandler == -1)
			return ConvertToPythonLong(-1);

		if (mapping==1) {
			stringstream code;
			KifCode* kifcode;
			if (kifhandler==0)
				kifcode=BaseParseur->kifloader;
			else
				kifcode=kifGlobals->kifCodeVector[kifhandler];

			if (kifcode==NULL) {
				PyErr_SetString(PyExc_AttributeError, "Unknown KIF space");
				return ConvertToPythonLong(-1);
			}

			bin_hash<KifElement*>::iterator it;
			KifElement* kfunc;
			string name;
			char var;
			for (it=kifcode->kstart->declarations.begin();it!=kifcode->kstart->declarations.end();it++) {
				kfunc=it->second;
				if (kfunc!=NULL && kfunc->isFunction()) {
					name=kifGlobals->kifIdString[it->first];
					code<<"def "<<name<<"(";
					var=97;
					int i;
					for (i=0;i<kfunc->Size();i++) {
						if (i)
							code<<",";
						code<<var;
						var++;
					}
					code<<"):\n\treturn xips.kif("<<kifhandler<<",'"<<name<<"'";
					var=97;
					for (i=0;i<kfunc->Size();i++) {
						code<<","<<var;
						var++;
					}
					code<<")\n";
				}
			}

			if (code.str().size()!=0)
				PyRun_SimpleString(code.str().c_str());
		}

		return ConvertToPythonLong(kifhandler);
	}

	KifElement* pythonkifclone(KifCode* kifcode,PyObject* po) {
		if (PyBool_Check(po)==1) {
			if (po==Py_True)
				return new KifBoolean(kifcode,NULL,true);
			return new KifBoolean(kifcode,NULL,false);
		}

		if (PyCheck_Number(po)==1) {
			long l=PyLong_AsLong(po);
			return kifcode->Provideinteger(l);
		}

		if (PyLong_Check(po)==1) {
			KIFLONG l=PyLong_AsLong(po);
			return new KifLong(kifcode,NULL,l);
		}

		if (PyFloat_Check(po)==1) {
			double d=PyFloat_AsDouble(po);
			return kifcode->Providefloat(d);
		}

		if (PyCheck_String(po)==1) {
			string s=PyAsString(po);
			return kifcode->Providestringraw(s);
		}

		PyObject* pelement;
		KifElement* e;
		if (PyList_Check(po)==1) {
			KifVector* kvect=new KifVector(kifcode,NULL);
			int tail= PyList_GET_SIZE(po);
			for (int i=0;i<tail;i++) {
				//Puis chacun des objets, kj est l'index de l'element
				pelement=PyList_GetItem(po,i);
				e=pythonkifclone(kifcode,pelement);
				if (e==kifRAISEERROR) {
					kvect->Release();
					return kifRAISEERROR;
				}
				kvect->Push(e);
			}
			return kvect;
		}

		if (PyDict_Check(po)==1) {
			KifMap* kmap=new KifMap(kifcode,NULL);
			Py_ssize_t pos=0;
			PyObject* key;
			KifElement* k;
			string kval;
			while (PyDict_Next(po, &pos, &key, &pelement)) {
				if (key!=NULL && pelement!=NULL) {
					k=pythonkifclone(kifcode,key);
					if (k==kifRAISEERROR) {
						kmap->Release();
						return kifRAISEERROR;
					}
					e=pythonkifclone(kifcode,pelement);
					if (e==kifRAISEERROR) {
						kmap->Release();
						return kifRAISEERROR;
					}
					kval=k->String();
					kmap->Push(kval,e);
					k->Release();
				}
			}
			return kmap;
		}

#ifdef HAVE_WCHAR_H
		if (PyUnicode_Check(po)) {
			Py_ssize_t sz=PyUnicode_GetSize(po);
			Py_UNICODE* ibuff=PyUnicode_AsUnicode(po);
			string s;
			for (int i=0;i<sz;i++)
				s+=s_unicode_to_utf8(ibuff[i]);
			return kifcode->Providestringraw(s);
		}
#else
		if (PyUnicode_Check(po)) {
			Py_ssize_t sz=PyUnicode_GET_DATA_SIZE(po);
			const char* sbuff=PyUnicode_AS_DATA(po);
			string s;
			for (Py_ssize_t i=0;i<sz;i++) {
				if (sbuff[i])
					s+=sbuff[i];
			}
			return kifcode->Providestring(s);
		}
#endif
		PyObject* perr=PyObject_Str(po);
		if (perr!=NULL) {
			string s=PyAsString(perr);
			e=kifcode->Providestringraw(s);
			Py_DECREF(perr);
			return e;
		}

		return kifRAISEERROR;
	}

	PyObject* kifclone(KifElement* resultat,bool relache) {
		if (resultat->type==kifBoolean) {
			PyObject* res;
			if (resultat==kifTRUE)
				res=Py_True;
			else
				res=Py_False;
			if (relache)
				resultat->Release();
			return res;
		}

		if (resultat->type==kifInteger || resultat->type==kifLong) {
			long l=resultat->Integer();
			if (relache)
				resultat->Release();
			return ConvertToPythonLong(l);
		}

		if (resultat->type==kifFloat || resultat->type==kifFraction) {
			double d=resultat->Float();
			if (relache)
				resultat->Release();
			return PyFloat_FromDouble(d);
		}

		PyObject* pcourant;
		PyObject* liste;

		if (resultat->aVectorContainer() || resultat->type==kifList || resultat->type==kifTable) {
	 		int i=0;
			liste=PyList_New(0);
			KifIteration* itr=(KifIteration*)resultat->Newiterator(false);
			for (itr->Begin();itr->End()==kifFALSE;itr->Next()) {
				pcourant= kifclone(itr->IteratorValue(),false);
				PyList_Insert(liste,i,pcourant);
				Py_DECREF(pcourant);
				i++;
			}
			itr->Release();
			if (relache)
				resultat->Release();
			return liste;
		}

		agnostring value;
		if (resultat->aMapContainer()) {
			liste=PyDict_New();
			KifIteration* itr=(KifIteration*)resultat->Newiterator(false);
			for (itr->Begin();itr->End()==kifFALSE;itr->Next()) {
				pcourant= kifclone(itr->IteratorValue(),false);
				value=itr->IteratorKeyString();
				PyDict_SetItemString(liste,STR(value),pcourant);
				Py_DECREF(pcourant);
			}
			itr->Release();
			if (relache)
				resultat->Release();
			return liste;
		}

		value=resultat->String();
		if (relache)
			resultat->Release();
#ifdef OLDPYTHON
		if (value.isutf8())
			return PyUnicode_DecodeUTF8(value.c_str(),value.size(),NULL);
#else
		if (value.isutf8())
			return PyUnicode_FromString(value.c_str());
#endif
		return ConvertToPythonString(STR(value));
	}


	static PyObject* xips_kif(PyObject* self, PyObject* args) {
		int nbelements=PyTuple_Size(args);

		if (nbelements<2) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters, at least two parameters are required: kif handler and function name");
			return ConvertToPythonLong(-1);
		}

		PyObject* pelement;
		pelement = PyTuple_GetItem(args, 0);
		if (!PyCheck_Number(pelement)) {
			PyErr_SetString(PyExc_AttributeError, "Expecting a kif handler");
			return ConvertToPythonLong(-2);
		}

		int kifhandler = PyAsNumber(pelement);
		KifCode* kifcode = kifGlobals->kifCodeVector[kifhandler];
		if (kifcode == NULL) {
			PyErr_SetString(PyExc_AttributeError, "This kif handler does not match an existing KiF program");
			return ConvertToPythonLong(-3);
		}

		pelement=PyTuple_GetItem(args, 1);
		if (!PyCheck_String(pelement)) {
			PyErr_SetString(PyExc_AttributeError, "Expecting a string");
			return ConvertToPythonLong(-4);
		}

		string functionname=PyAsString(pelement);

		vector<KifElement*> params;
		int i;
		KifElement* ke;

		for (int i=2;i<nbelements;i++) {
			pelement=PyTuple_GetItem(args, i);
			ke=pythonkifclone(kifcode,pelement);
			if (ke==kifRAISEERROR) {
				for (i=0;i<params.size();i++)
					params[i]->Release();
				PyErr_SetString(PyExc_AttributeError, "No mapping for this python element");
				return ConvertToPythonLong(-5);
			}
			params.push_back(ke);
		}

		bool theerror=false;
		KifElement* resultat=XipKifExecuteIndexObject(kifcode,functionname,params,theerror);
		if (theerror==true) {
			string value=Errorstring();
			PyErr_SetString(PyExc_AttributeError, STR(value));
			return ConvertToPythonLong(-6);
		}

		return kifclone(resultat,true);
	}

	static PyObject* xips_typereglestring(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;
		int id;

        if (!PyArg_ParseTuple(args,"ii",&parsnum,&id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL || parseur->VBases[id]==NULL)
            return ConvertToPythonLong(-1);





		int ty=parseur->VBases[id]->label->type;
		char* tys=field_name[ty];
		return ConvertToPythonString(tys);
	}

	static PyObject* xips_nbregles(PyObject* self, PyObject* args) {
		int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

        if (!PyArg_ParseTuple(args,"i",&parsnum))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
        }

        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(-1);





		return ConvertToPythonLong(parseur->VBases.dernier);
	}

	static PyObject* xips_chargeprobas(PyObject* self, PyObject* args) {
		char* fichier;
        int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

		if (!PyArg_ParseTuple(args,"is",&parsnum,&fichier)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}
        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(-1);
		int id=parseur->chargeprobabilites(fichier);
		return ConvertToPythonLong(id);
    }

	static PyObject* xips_sauveprobas(PyObject* self, PyObject* args) {
		char* fichier;
        int parsnum=index_parseur_courant;
		int precedent=index_parseur_courant;

		if (!PyArg_ParseTuple(args,"is",&parsnum,&fichier)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
            return ConvertToPythonLong(-1);
		}
        GlobalParseur* parseur=initNouveauParseur(parsnum);
        if (parseur==NULL)
            return ConvertToPythonLong(-1);
		parseur->sauvegardeprobabilites(fichier);
		return ConvertToPythonLong(0);
    }

    static PyMethodDef NoeudMethode[] = {

        {"node",xips_xipnode,METH_VARARGS,"Return a xip node."},
        {"nodeset",xips_xipnodes,METH_VARARGS,"Return a set of xip nodes."},
        {"dependency",xips_dependency,METH_VARARGS,"Return a dependency."},
        {"dependencyset",xips_dependencies,METH_VARARGS,"Return a set of dependency."},
		{"parsestring",xips_parsestring,METH_VARARGS,"parse a string with grammar g: xips.parsestring(g,str,data). (data is any python object)"},
		{"tagging",xips_stringtagging,METH_VARARGS,"tag a string with grammar g: xips.stringtagging(g,str)."},
		{"parsefile",xips_parsefile,METH_VARARGS,"parse a file with grammar g: xips.parsefile(g,filename,data). (data is any python object)"},
#ifdef XIPLIBXML
        {"parsexmlstring",xips_parsexmlstring,METH_VARARGS,"parse an XML string with grammar g at depth: xips.parsexmlstring(g,str,depth,data). (data is any python object)"},
        {"parsexmlfile",xips_parsexmlfile,METH_VARARGS,"parse an XML file with grammar g at depth: xips.parsexmlfile(g,filename,depth,data). (data is any python object)"},
#endif
		{"generatefromfile",xips_parsedependencyfile,METH_VARARGS,"Read a dependency file and generate a sentence from it: xips.generatefromfile(g,filename,data). (data is any python object)"},
		{"generatefromstring",xips_parsedependencystring,METH_VARARGS,"Read a dependency string and generate a sentence from it:: xips.generatefromstring(g,filename,data). (data is any python object)"},
		{"generated",xips_generated,METH_VARARGS,"Return the generated sentence: xips.generated(g)."},
        {"loadgrm",xips_loadgrm,METH_VARARGS,"Load a grammar file: xips.loadgrm(filename)."},
        {"reloadgrm",xips_reloadgrm,METH_VARARGS,"Reload the grammar file with the index grm: xips.reloadgrm(grm)."},
        {"freegrm",xips_freegrm,METH_VARARGS,"Clean a grammar from memory: xips.freegrm(grm)."},
        {"setdisplay",xips_setdisplay,METH_VARARGS,"Set the mode display xips.setdisplay(grm,mode)."},
        {"adddisplay",xips_adddisplay,METH_VARARGS,"Add a mode display display xips.adddisplay(grm,mode)."},
        {"removedisplay",xips_removedisplay,METH_VARARGS,"Remove a mode display xips.removedisplay(grm,mode)."},
        {"testdisplay",xips_testdisplay,METH_VARARGS,"Test a mode display xips.testdisplay(grm,mode), return 1 or 0"},
        {"whoami",xips_version,METH_VARARGS,"return the XIP engine version."},
        {"dependencyonfirstnode",xips_dependencyonfirstnode,METH_VARARGS,"return the list of dependencies whose first parameter is the node n: xips.dependencyonfirstnode(g,n)."},
        {"dependencyonsecondnode",xips_dependencyonsecondnode,METH_VARARGS,"return the list of dependencies whose second parameter is the node n: xips.dependencyonsecondnode(g,n)."},
		{"fictivenodes",xips_xipfictivenodes,METH_VARARGS,"return the list of fictive nodes that have been created so far:fictivenodes(g)."},
        {"setexchange",xips_setexechange,METH_VARARGS,"Set the exchange variable: xips.setexchange(v,o). v is a XIP python variable: return -1 if wrong parameters. -2 if unknown XIP variable. -3 if wrong XIP variable type. 0 if OK."},
        {"internalstring",xips_xipinternalstring,METH_VARARGS,"return the internal string buffer. Applies only when parsing with a string or from an XML file."},
		{"lexicals",xips_xiplexicalnodes,METH_VARARGS,"return the list of the lexical node indexes:lexicals(g)."},
        {"setfloat",xips_setfloatvariable,METH_VARARGS,"setfloat(par,v,f) Sets the variable v (a string) with the value f (a float) for the grammar grm."},
        {"setint",xips_setintvariable,METH_VARARGS,"setint(par,v,i) Sets the variable v (a string) with the value i (an int) for the grammar grm."},
        {"setstring",xips_setstringvariable,METH_VARARGS,"setstring(par,v,s) Sets the variable v (a string) with the value i (a string) for the grammar grm."},
		{"setdebug",xips_setdebug,METH_VARARGS,"setdebug(module_number,module_line) Sets a break point in the module module_num at module_line."},
		{"currentgrammarindex",xips_currentgrammarindex,METH_VARARGS,"grammarindex() return the current grammar index."},
		{"currentrule",xips_current_rule,METH_VARARGS,"currentrule() return the current rule index."},
		{"currentruletext",xips_current_rule_text,METH_VARARGS,"currentruletext(displayheader) return the current rule text, displayheader is 0,1 or 2."},
		{"grammarcharset",xips_get_grammarcharset,METH_VARARGS,"grammarcharset(g) return the charset encoding of the grammar"},
		{"sourcecharset",xips_get_sourcecharset,METH_VARARGS,"sourcecharset(g) return the charset encoding of the input"},
		{"fstcharset",xips_get_fstcharset,METH_VARARGS,"fstcharset(g) return the charset encoding of the FST"},
		{"setruleweight",xips_metpoids,METH_VARARGS,"setruleweight(g,id,w) set the weight to rule id"},
		{"setrulethreshold",xips_metseuil,METH_VARARGS,"setrulethreshold(g,id,w) set the threshold to rule id"},
		{"setrulevalue",xips_metvaleur,METH_VARARGS,"setrulevalue(g,id,w) set the trigger value to rule id"},
		{"getruleweight",xips_poids,METH_VARARGS,"getruleweight(g,id) return weight of rule id"},
		{"getrulethreshold",xips_seuil,METH_VARARGS,"getrulethreshold(g,id) return  threshold of rule id"},
		{"getrulevalue",xips_valeur,METH_VARARGS,"getrulevalue(g,id) return trigger value of rule id"},
		{"rulelayer",xips_coucheregle,METH_VARARGS,"rulelayer(g,id) return the layer number of rule ID"},
		{"ruletype",xips_typeregle,METH_VARARGS,"ruletype(g,id) return the type of rule ID"},
		{"rulecounter",xips_compteurregle,METH_VARARGS,"rulecounter(g,id) return the number of occurrence of rule ID"},
		{"ruletypestr",xips_typereglestring,METH_VARARGS,"ruletypestr(g,id) return the type of rule ID as a string"},
		{"nbrules",xips_nbregles,METH_VARARGS,"nbrules(g) return the number of rules"},
		{"loadprobabilities",xips_chargeprobas,METH_VARARGS,"loadprobabilities(g,filename) load a probability model stored in filename"},
		{"saveprobabilities",xips_sauveprobas,METH_VARARGS,"saveprobabilities(g,filename) save a probability model stored in filename"},
		{"loadkif",xips_loadkif,METH_VARARGS,"loadkif(filename,arguments) load a KiF program and return a kif handler"},
		{"kif",xips_kif,METH_VARARGS,"kif(khandler,function_name,arguments) Execute a KiF function, the argument should be a vector of strings"},
        {NULL,NULL,0,NULL}
    };


#if PY_MAJOR_VERSION >= 3
	static struct PyModuleDef xipsmodule = {
		PyModuleDef_HEAD_INIT,
		"xips",   /* name of module */
		NULL, /* module documentation, may be NULL */
		-1,       /* size of per-interpreter state of the module,
				  or -1 if the module keeps state in global variables. */
		NoeudMethode
	};

	Exportation PyMODINIT_FUNC PyInit_xips(void) {
		PyObject *m;

		m = PyModule_Create(&xipsmodule);
		if (m == NULL)
			return NULL;

		PyObject* pDict = PyModule_GetDict(m);
		PyRun_String(insere_xip_objects(OUI).c_str(), Py_file_input, pDict, pDict);

		xipsError = PyErr_NewException("xips.error", NULL, NULL);
		Py_INCREF(xipsError);
		PyModule_AddObject(m, "error", xipsError);
		initialization = true;

		return m;
	}

	//Initialisation unique
	void GlobalParseur::initialisation_python() {
		if (Py_IsInitialized())
			return;
		Py_Initialize();
		PyInit_xips();
	}

#else
    Exportation void initxips() {
        PyObject* module=Py_InitModule("xips",NoeudMethode);
        PyObject* pDict = PyModule_GetDict(module);
		xipsError=PyErr_NewException("xips.error",NULL,NULL);
		PyDict_SetItemString(pDict,"error",xipsError);

		PyRun_String(insere_xip_objects(OUI).c_str(),Py_file_input,pDict,pDict);
    }
}

//Initialisation unique
void GlobalParseur::initialisation_python() {
    if (Py_IsInitialized())
		return;
    Py_Initialize();
    initxips();
}
#endif
#endif
#else
void initxips() {}
#endif

CREERAMASSE(TestFormuleElement);
CREERAMASSE(Variable);
CREERAMASSE(TestFormule);
CREERAMASSE(TestFonction);

void transformedescendants(GlobalParseur* parseur, Hierarchie&,ontologie* ont);
char minlettre(unsigned char c);
static time_t  initialtime,finaltime;

#ifdef WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#endif

TVecteur TrouveValeur(Vecteur* v, Vecteur* vf,int& rang);
void TrouveBorneValeur(Vecteur* v, Vecteur* vf,int& gauche,int& droite);

void Trim(char* chaine);
void strcatchr(char* tmp,char c);
uchar* minuscule(unsigned char* c);

static const char* TabOperateurs="+-*/^%";
static const char* SoloOperateurs=TabOperateurs+2;
static const char* TableOperations[] =
{"=",":","<",">","<=",">=","~:","+","*","-","/","%","^","log","ln","exp","sqrt",
 "tan","sin","cos","atan","@pi","@e","len","left","right","mid","regexp","trim","regexpsplit",
 "for","insertvalue","search","@lasterror","errormessage","insert","addstring",
 "regexpstart","regexpfull","removestring","goto","exit","addstringunique","return","@pathname",
 "offset","nodenumber","@sentencenumber","lemmas","node",
 "next","previous","mother","daughter","last","lowercase","uppercase",
 "addvalue","addvalueunique","removevalue",
 "searchvalue","pause","removefirst","removelast","replace","copyfeature",
 "cleanlemmas","cleanalllemmas","retrievelemmas","getlemma","stop","fopen","fclose","fwrite",
 "starttimer","endtimer","@elapsedtime","read","print","regexplast","@language","_print",
 "@version","@xip","printerr","ascii","error","descendant","backsearch","regexpback",
 "@sentencelanguage","help","@date","@year","@monthstring","@daystring",
 "@hour","@day","@month","dependencynumber","dependency","nop","setfeature",
 "push","pop","cleanstack","createdependency","keys","values","@sentence","@intermediate",
 "xmldata","xmlparent","xmlchild","xmlnext","xmlprevious","xmltext","xmlset","xmltag","pythonfulllexical",
 "@wordcount","@punctcount","tokennumber","offsetchar","block","free","blocked","rulespace","getenv",
 "putenv","freefull","addgraph","project","setgraphname","getgraphname","extractgraph","replacegraph",
 "removesubgraph","removegraph","duplicategraph","apply","freed","addword","xmltoken",
 "cleanall","savexmlgraph","getinitialfeatures","setsubfeature",
 "@generated","initlookdown","depth","rank", "nodedescendant","nodenext","nodeprevious",
 "nodemother","nodedaughter","lookup","lookdown","nodelast","nodenexttoken","nodeprevioustoken",
 "nodefirsttoken","nodelasttoken", "istoken","words","setword","getword","cleanword",
 "setruleweight","setrulethreshold","setrulevalue","getruleweight","getrulethreshold","getrulevalue",
 "@nbrules","ruletype","ruletypestring","saveprobabilities","loadprobabilities",
 "rulecounter","rulelayer","xmlattributes","xmlget","eval","splitalong","keyonindex","@utf8","xmlstring",""}; //188



static const char* TableOperationsParametres[] =
{
 "1. Operators",
 "x = y\t(assignation)",
 "x : y\t(equality)",
 "x < y\t(inferior)",
 "x > y\t(superior)",
 "x <= y\t(inferior and equal)",
 "x >= y\t(superior and equal)",
 "x ~:y\t(different)",
 "x + y\t(addition or string concatenation)",
 "x * y\t(multiplication)",
 "x - y\t(minus)",
 "x / y\t(divide)",
 "x % y\t(modulo)",
 "x ^ y\t(power)",
 "2. Mathematical Functions",
 "log(x)\t(logaritm 10)",
 "ln(x)\t(neparian logarithm)",
 "exp(x)\t(exponantial)",
 "sqrt(x)\t(square root)",
 "tan(x)\t(tangent)",
 "sin(x)\t(sinus)",
 "cos(x)\t(cosinus)",
 "atan(x)\t(arc tangent)",
 "3. Mathematical Constants",
 "@pi;\t(PI value)",
 "@e;\t(E value)",
 "4. String Functions",
 "len(str)\t(string length)",
 "left(str,n)\t(return the n left characters of str)",
 "right(str,n)\t(return the n right characters of str)",
 "mid(str,i,n)\t(return the n characters starting at i from str)",
 "trim(str)\t(remove white space from both ends of str)",
 "ascii(str)\t(return the ascii value of the first character of str)",
 "lowercase(str)\t(return a string where all characters in str are in lowercase)",
 "uppercase(str)\t(return a string where all characters in str are in uppercase)",
 "removefirst(str,n)\t(return a string with the first n characters removed from str)",
 "removelast(str,n)\t(return a string with the last n characters removed from str)",
 "5. String Vector Functions",
 "insert(vect,str,i)\t(inserts str in vect at i)",
 "addstring(vect,str)\t(adds str to the end of vect)",
 "addstringunique(vect,str)\t(adds str to the end of vect, if strhas not been added so far)",
 "search(vect,str)\t(return the position of str in vect, -1 if no occurence)",
 "6. Numerical Vector Functions",
 "addvalue(ivect,x)\t(adds the value x to the end of the integer vector ivect)",
 "addvalueunique(ivect,x)\t(adds the value x to the end of the integer vector ivect, if x has not been added to ivect before)",
 "insertvalue(vect,val,i)\t(inserts the value val at the position i in vect)",
 "removevalue(ivect,i)\t(remove the ith element in a vector)",
 "searchvalue(ivect,v)\t(return the position of v in ivect. -1 otherwise)",
 "7. Dictionary Functions",
 "backsearch(dict,str,vect)\t(This instruction retrieves the list of indexes that are attached to str)",
 "keys(dict,vect)\t(This instuction return the list of keys of the dictionary in vect)",
 "values(dict,vect)\t(This instruction return the values of the dictionary in vect)",
 "keyonindex(dict,i)\t(This instuction return the key corresponding to ith position in dict)",
 "8. Regular Expression Functions",
 "regexp(str,rgx)\t\t\t(return the substring corresponding to the regular  expression rgx)",
 "regexpstart(str,rgx)\t\t(return the substring corresponding to rgx starting at the beginning of str)",
 "regexpsplit(str,rgx,vect)\t(store in vect all the occurrences of rgx over str)",
 "regexpfull(str,rgx)\t\t(return 1, if rgx applies to whole str. 0 otherwise)",
 "removestring(vect,i)\t\t(remove the ith element from vect)",
 "regexplast(string,rgx)\t\t(return the first substring corresponding to rgx but from the end of str)",
 "regexpback(str,rgx)\t\t(return the substring corresponding to rgx back from the end of str)",
 "replace(str,rgx,rep)\t\t(return a string where each substring matching rgx is replaced by rep)",
 "splitalong(str,char,vect)\t(store in vect str split along char)",
"9. System Functions",
 "goto(str)\t(goto label str)",
 "exit;\t(stop the current analysis)",
 "return(x)\t(return a numerical value x from a procedure)",
 "stop;\t(see exit)",
 "error(str);\t(stop the XIP engine and generates an exception with the error message str)",
 "getenv(var,env);\t(get the string value of the environment variable env into var)",
 "putenv(env,var);\t(set the string value of the environment variable env from var)",
 "@lasterror;\t(return the error number of the last error)",
 "errormessage(i)\t(return the error string corresponding to i)",
 "eval(s,add)\t(evaluate s as a piece of grammar. add is a int: 0 means absolute layering, 1 means relative layering)",
 "10. System Variables",
 "@pathname;\t(return the pathname of the file currently analysed)",
 "@sentencenumber;\t(return the current sentence number)",
 "@version;\t(return the version of the grammar)",
 "@xip;\t(version of the xip engine)",
 "@language;\t(return the language of the grammar)",
 "@sentencelanguage;\t(return the language that was actually detected for a given sentence)",
 "@sentence;\t(return the internal sentence buffer)",
 "@wordcount;\t(return the number of words in the current sentence)",
 "@punctcount;\t(return the number of punctuation signs in the current sentence)",
 "@generated;\t(yield the sentence that has been built with generation rules)",
 "nop;\t(do nothing)",
 "11. Node Functions",
 "offset(#1,left,right)\t(return the left offset and the right offset of the node #1)",
 "offsetchar(#1,left,right)\t(return the left offset and the right offset of the node #1 in character length)",
 "tokennumber(#1,left,right)\t(return the left and right token numbers of the node #1)",
 "nodenumber(#1)\t(	return the node number of #1)",
 "node(#1,i)\t(assign to the variable #1 the node identified by the node number i)",
 "lemmas(vect,#1)\t(store in vect the lemmas associated to #1)",
 "GetInitialFeatures(#1,vect)\t(store in vect the initial features associated to #1)",
 "next(#1,#2)\t(#2 is the next node after #1)",
 "previous(#1,#2)\t(#2 is the previous node before #1)",
 "mother(#1,#2)\t(#2 is the mother node of #1)",
 "daughter(#1,#2)\t(#2 is the leftest node under #1)",
 "last(#1,#2)\t(#2 is the rightest node under #1)",
 "descendant(#1,#2)\t(Check if #1 is a descendant of #2)",
 "nodedescendant($#1,$#2)\t(Check if $#1 is a descendant of $#2)",
 "nodenext($#1,$#2)\t($#2 is the next node after $#1)",
 "nodeprevious($#1,$#2)\t($#2 is the previous node before $#1)",
 "nodemother($#1,$#2)\t($#2 is the mother node of $#1)",
 "nodedaughter($#1,$#2)\t($#2 is the leftest node under $#1)",
 "nodelast($#1,$#2)\t($#2 is the rightest node under $#1)",
 "nodenexttoken($#1,$#2)\t(returns the token next to $#1 in the generated sentence)",
 "nodeprevioustoken($#1,$#2)\t(returns the token previous to $#1 in the generated sentence)",
 "nodefirsttoken($#1,$#2)\t(returns the first in depth NODE of $#1)",
 "nodelasttoken($#1,$#2)\t(returns the last in depth NODE of $#1)",
 "istoken($#1)\t(returns 1 if the NODE is a leaf)",
 "words(str,str,str,...)\t{test a sequence of regular expression over the new generated sentence)",
 "setword(i,str)\t{replace the ith position corresponding to the function \"words\" input with str, multiple occurences possible)",
 "cleanword(i)\t{delete the word in the ith position corresponding to the function \"words\" input)",
 "getword(i,str[])\t(return a list of strings corresponding to the ith element in \"words\" input)",
 "copyfeature(#1[feat],#2)\t(copy the features feat from #1 to #2, also works with dependency variable)",
 "setfeature(#1,attribute,value)\t(add attribute:value feature to #1, attribute and value are strings)",
 "setsubfeature(#1,attribute,value)\t(add attribute:value feature to #1 and all its subnodes, attribute and value are strings)",
 "dependencynumber($1)\t(return the index of a dependency variable)",
 "dependency($1,number)\t(assign to $1, the dependency whose index is number)",
 "createdependency($1,name,features,#1,#2,..)\t(Create a new dependency, the parameter $1 is optional,features is also optional: [feat=+,..])",
 "push($1,str)\t(push a new string on the top of $1's stack)",
 "pop($1)\t(pos the string on the top of $1's stack)",
 "cleanstack($1)\t(empty $1's stack)",
 "initlookdown(#1,feat)\t(use the generatefeatures string to update the node's features, with a FST lookdown. feat is optional)",
 "lookup(#1,vect)\t(return a string from the FST with the initial feature string)",
 "lookdown(lemma,feat)\t(return the surface form from a string containing the lemma and the feature string)",
 "depth($1)\t(return the depth of a NODE in a generation tree)",
 "rank($1)\t(return the position of a NODE in a generation tree sequence)",
 "12. Lemma Variable Functions",
 "cleanlemmas(var##)\t(remove the lemmas associated to the lemma variable var##)",
 "cleanalllemmas;\t(delete all lemma variables)",
 "retrievelemmas(var##,vect,ivect)\t(return the values and strings of the lemma variable var##)",
 "getlemma(var##,str)\t(This function return the value of a lemma variable for a given lemma string)",
 "13. Interaction Functions",
 "pause(x)\t(Pause for x seconds)",
 "read(str)\t(read a string on the console)",
 "print(str)\t(print a string on the console)",
 "printerr(str)\t(display the str string on error console)",
 "14. File Functions",
 "fopen(alias,pathname)\t(open the file pathname and associates it to alias)",
 "fclose(alias)\t(close the file associated to alias)",
 "fwrite(alias,string)\t(write the string str in the file associated to alias)",
 "15. Time Functions",
 "starttimer\t(start the timer)",
 "endtimer\t(stop the timer)",
 "@elapsedtime;\t(return the elapsed time between a startimer and a endtimer)",
 "@intermediate;\t(return the intermediate elapsed time since the last startimer)",
 "@date;\t(return the date as a string)",
 "@monthstring;\t(return the month as a string)",
 "@daystring;\t(return the day as a string)",
 "@hour;\t(return the hour as a string)",
 "@year;\t(return the year as a number)",
 "@day;\t(return the day number in the month)",
 "@month;\t(return the month number)",
 "16. Field Names",
 "semantic,s�mantique\tSemantics rules: lemme: Cat[features] = if ()... dep1(...),dep2(...)",
 "terms,termes\tTerms rules	lemme: Cat[feature]= Cat1...Catn.",
 "reshuffling,reconstructeur\tReshuffling rules",
 "tagging,d�sambigu�sation\tDisambiguation rules",
 "idrules,dir�gles\tId rules",
 "lprules,plr�gles\t Linear Precedence Rules",
 "marker,marqueur\tMarking rules",
 "dependencyrules,d�duction\tDependency rules",
 "translation,traduction\ttranslation rules",
 "categorydefinition,d�finitioncat�gorie\tDefinition of a category for a reading, when none was provided in the translation file",
 "featuredefinition,d�finitiontraits\tEnrichment a feature structure with new features within the translation file",
 "extraction\tSame as dependency rules, old format",
 "sequence,s�quence\tsequence rules",
 "doublereduction,doubler�duction\tDouble reduction rules",
 "fragment,fragmenter\tSplit a word into different categories",
 "script\tScription language",
 "final\tFinal section",
 "loading,enchargeant\tWhile loading a file of rules",
 "initial\tInitial section",
 "split,d�coupes�quence\tSplit rules",
 "multiwords,multimots\tRules based on a regular expression matching a sequence of words.",
 "multilemmas,multilemmes\tRules based on a regular expression matching a sequence of lemmas.",
 "generation,g�n�ration\tGeneration rules.",
 "order,ordre\tGeneration ordering rules.",
 "zones,zones\tTo define zone within the grammar",
 "vocabulary,vocabulaires\tTo define a XIP lexicon",
 "17. Control Fields",
 "lemma,lemme\tTo redefine the lemma feature name (default: lemma)",
 "surface,surface\tTo redefine the surface feature name (default: surface)",
 "tag\tThe default part of speech when none is avalaible",
 "alluppercase,toutmajuscule\tTo define the feature in case of all uppercase in a word",
 "uppercase,majuscule\tTo define the name feature in case of a word starting with an uppercase",
 "pos,cat�gories\tTo define the Part of speech section",
 "boundaries,finphrase\tTo define the substring that should be found in a lexical entry for the end of sentence",
 "regularboundaries,expressionfinphrase\tTo define a regular expression that should match the lexical entry for the end of sentence",
 "18. Dependency Definitions",
 "functions,fonctions\tDefinition of dependencies",
 "hidden,cacher\tTo hide specific dependencies",
 "keep,garder\tTo keep dependencies",
 "19. Output Controls",
 "display,affichage\tWhich feature should be displayed or passed in a library",
 "ontologydisplay,affichageontologie\tWhich ontology feature can be displayed",
 "functiondisplay,affichagefonction\tWhich feature is a dependency feature",
 "nodedisplay,affichagenoeud\tWhich feature should be displayed in the current output",
 "20. Feature Rules",
 "postdfs,poststd\tDefault Feature Specification applied after the percolation of feature",
 "postfcr,postrct\tFeature Coocurrence Restriction applied after the percolation of feature",
 "predfs,pr�std\tDefault Feature Specification applied before the percolation of feature",
 "prefcr,pr�rct\tFeature Coocurrence Restriction applied before the percolation of feature",
 "21. XIP Grammar File fields",
 "license\tlicense name",
 "modules\teffective modules",
 "expiration\texpiration date",
 "code\tValidation Code ",
 "language\tField language (either English or French)",
 "grammarlanguage\tLanguage which the grammar deals with",
 "locale\tLocale string, defines the input/output for the ISO charset",
 "charset\tDefines the default charset for XML file",
 "hmm\t(YES/NO) is the hmm present in the system?",
 "reentrance\tA dependency that has just been created can be reused by the same dependency rule",
 "maxword,maxmots\tMaximum number of words in a sentence (-1 for no limit)",
 "wordsize,taillemot\tMaximum size of one word in bytes",
 "utf8lexiconencoding,encodageutf8lexique\tSet to YES if the internal lexicons are encoded in UTF8",
 "number,nombre\tMaximum number of terminal features (multiplies this number by 64 to have this limit)",
 "indentation,sortie\tPathname of the indentation file",
 "trace,trace\tPathname of the trace file",
 "features,traits\tFile Feature section ",
 "lexicons,lexique\tLexicon section",
 "rules,r�gles\tRule section",
 "22. XIP GRM File Fields",
 "path:\tDefines a pathname variable",
 "hmm:\tDefines the HMM pathname",
 "ntm:\tDefines the NTM pathname",
 "gram,grammar:\tDefines the grammar language pathname (GRAM is used in the case of a crypted grammar)",
 "variable:\tDefines a variable that is used in a #define statement",
 "parameter:\tDefines a parameter pathname",
 "23. XML instructions",
 "xmlattributes(#1,vidict):\treturn the attributes associated to the XML token",
 "xmlget(#1,vxml):\treturn the XML node associated to the current node (mode tokenize)",
 "xmldata(vxml,vdict):\treturn the data associated to a XML node variable into a dictionary",
 "xmlparent(vxml,vxmlparent):\tStores in vxmlparent, the parent of vxml",
 "xmlchild(vxml,vxmlchild):\tStores in vxmlchild, the vxml child",
 "xmlnext(vxml,vxmlnext):\tStores in vxmlnext, the next node of vxml",
 "xmlprevious(vxml,vxmlprevious):\tStores in vxmlprevious, the previous node of vxml",
 "xmltext(vxml,str):\tStores in str the text of the XML node vxml",
 "xmltag(vxml,str):\tStores in str the tag name of the XML node vxml",
 "xmlset(\"@reference\",vxml):\tStores in @reference, the value of vxml",
 "xmltoken(#1,vxml):\tget the XML node, which was used in a tokenize mode to create the #1 node",
 "xmlstring(tag,s):\tCreate an XML node tag with s as content",
 "24. Rule Space instructions",
 "apply(str1,str2,...):\tActivate only the rule spaces str1, str2... Deactive the other rule spaces",
 "block(str):\tDeactivate the str rule space. return 1, if it succeeds",
 "free(str):\tReactivate the str rule space. return 1, if it succeeds",
 "freefull(str):\tReactivate the str rule space, including the parent rule space. return 1, if it succeeds",
 "blocked(vect):\tReturn the vector of deactivated rule space strings. return the vector size",
 "freed(vect):\tReturn the vector of active rule space strings. return the vector size",
 "rulespace(vect):\tReturn the string vector of all declared rule space in a grammar. return the vector size",
 "25. Conceptual Graph instructions",
 "graph var:\tDeclaration of a graph variable",
 "graph var[]:\tDeclaration of a graph vector variable",
 "g > gg:\t Projection of g on gg, return 1 if it succeeds",
 "g : gg:\t Graph equality",
 "g = gg:\t Assignation",
 "addgraph(v,g):\t Adds the graph (or the list of graphs) g to the vector of graph v",
 "duplicategraph(g,gg):\tDuplicates the graph gg and stores it into g",
 "project(g,gg):\t Projection of gg (a graph or a vector of graph) on g",
 "project(g,gg,res):\t Projection of gg (a graph or a vector of graph) on g, return the list of graphs from gg, which have succeeded",
 "extractgraph(g,gg,res):\t Projection of gg (a graph or a vector of graph) on g, return the list of sub-graphs from g, which have been projected on",
 "removesubgraph(g,pattern):\t Remove from g, the sub-graphs corresponding to the pattern graph",
 "removegraph(v,i):\t Remove from the vector v, the graph of index i",
 "replacegraph(g,pattern,replace):\t Replace in g, the sub-graphs corresponding to the pattern with the replace graph",
 "savexmlgraph(name,g|gg):\t Save a graph or a vector of graph as an XML file with name as filename",
 "26. Lexicon instructions",
 "addword(n,s,l,f):\t Add a new lexicon entry in a XIPTRANS lexicon, whose name is n (must be declared in a 'dynamic' field in the lex.trs file, with s as the surface form, l as the lexicon form and f as features separated with '+'",
 "27. Probabilities",
 "setruleweight(id,w):\t set the weight to rule id",
 "setrulethreshold(id,w):\t set the threshold to rule id",
 "setrulevalue(id,w):\t set the trigger value to rule id",
 "getruleweight(id):\t return weight of rule id",
 "getrulethreshold(id):\t return  threshold of rule id",
 "getrulevalue(id):\t return trigger value of rule id",
 "@nbrules:\t return the number of rules in the grammar",
 "rulecounter(id):\t return the number of occurrence of rule id",
 "rulelayer(id):\t return the layer number of rule id (if id==-1, return the layer of the current rule)",
 "ruletype(id):\t return the type of rule id (if id==-1, return the type of the current rule)",
 "ruletypestring(id):\t return the type of rule id as a string (if id==-1, return the type of the current rule)",
 "loadprobabilities(f):\t load the probabilities from file f",
 "saveprobabilities(f):\t store the probabilities in a file f",
 "#"}; //90

const char* TableOperationChaine[] = {"left","right","mid","regexp",
"trim","regexpsplit","errormessage","regexpstart","@pathname","lowercase","uppercase",
    "removefirst","removelast","replace","read","print","regexplast","@language",
    "_print","@version","@xip","printerr","error","regexpback","@sentencelanguage",
    "@date","@monthstring","@daystring","@hour","keys","values","@sentence","keyonindex"
	,"xmltext","xmltag","getenv","putenv","getgraphname","pop","addword","getinitialfeatures",
	"@generated","lookup","lookdown","ruletypestring","saveprobabilities","loadprobabilities","splitalong","xmlstring",""};

//Toutes ces instructions ont en commun la presence d'une expression reguliere comme deuxieme parametre
const char* TableExpressionReguliere[] = {"regexpsplit","regexpstart","regexpfull",
    "regexp","regexplast","regexpback",""};

const char* TableGraphes[] = {"addgraph","project","setgraphname","extractgraph","replacegraph","removesubgraph",
"removegraph","duplicategraph","savexmlgraph",""};//131,132,133,135,136,137,138,139,145

static const char* TableSansParametre[] = {"exit","cleanalllemmas","stop",
    "starttimer","endtimer","nop","cleanall",""};
static const char* TableBoucle[] = {"for",""};
static const char* TableOperationsNoeuds[] = {"node",""};

#define NEPASCREERVARIABLE 2
#define RENVOIEVARIABLE 1
#define TESTERVECTEURVARIABLE 0

static TVecteur valeur_affichage[] = {
			  AFFICHAGE_LEMME,
              AFFICHAGE_SURFACE,
              AFFICHAGE_BALISE,
              AFFICHAGE_CATEGORIE,
              AFFICHAGE_REDUIT,
              AFFICHAGE_COMPLET,
              AFFICHAGE_OFFSET,
              AFFICHAGE_NUM_MOT,
              AFFICHAGE_PHRASE,
              AFFICHAGE_NUL,
              AFFICHAGE_STANDARD,
              AFFICHAGE_DEP_NOM,
              AFFICHAGE_DEP_NOEUD,
              AFFICHAGE_DEP_CREATION,
              AFFICHAGE_NUMPHRASE,
              AFFICHAGE_NUMERO_DEPENDANCE,
              AFFICHAGE_MATHEMATIQUE,
              AFFICHAGE_ERREUR_EXECUTION,
              AFFICHAGE_FICHIER_XML_ECRAN,
              ARBRE_AFFICHE,
              EXECUTE_NUL,
              EXECUTE_CHUNK,
              EXECUTE_DEPENDENCY,
              EXECUTE_TOKENIZE,
              TESTER_LANGUE,
              ENTREE_UTF8,
              AFFICHAGE_UTF8,
			  ENTREE_EN_MINUSCULE,
			  AFFICHAGE_CATEGORIE_GENERATION
};
static const char* affichagemotclef[] = {
    "lemma", //AFFICHAGE_LEMME
    "surface", //AFFICHAGE_SURFACE
    "markup", //AFFICHAGE_BALISE
    "category",//AFFICHAGE_CATEGORIE
    "reduced",//AFFICHAGE_REDUIT
    "full",//AFFICHAGE_COMPLET
    "offset",//AFFICHAGE_OFFSET
    "wordnumber",//AFFICHAGE_NUM_MOT
    "sentence",//AFFICHAGE_PHRASE
    "none",//AFFICHAGE_NUL
    "standard",//AFFICHAGE_STANDARD
    "depbyname",//AFFICHAGE_DEP_NOM
    "depbynode",//AFFICHAGE_DEP_NOEUD
    "depbycreation",//AFFICHAGE_DEP_CREATION
    "sentencenumber",//AFFICHAGE_NUMPHRASE
    "rulenumber",//AFFICHAGE_NUMERO_DEPENDANCE
    "math",//AFFICHAGE_MATHEMATIQUE
    "error",//AFFICHAGE_ERREUR_EXECUTION
    "screenxml",//AFFICHAGE_FICHIER_XML_ECRAN
    "tree",//ARBRE_AFFICHE
    "nop",//EXECUTE_NUL
    "chunk",//EXECUTE_CHUNK
    "dependency",//EXECUTE_DEPENDENCY
    "tokenize",//EXECUTE_TOKENIZE
    "languagetest",//TESTER_LANGUE
    "utf8input",//ENTREE_UTF8
    "utf8output",//AFFICHAGE_UTF8
	"lower",//ENTREE_EN_MINUSCULE
	"generatecategory",//SORTIE EN MODE GENERATION POUR LES DEPENDANCES
    "%"};

Exportation void ListOfOperations(vector<string>& lists) {
    for (int i=0;TableOperationsParametres[i][0]!='#';i++) {
        lists.push_back(TableOperationsParametres[i]);
    }
}

void AfficheAide(int h) {
    for (int i=0;TableOperationsParametres[i][0]!='#';i++) {
        if (h==-1) {
            cout<<TableOperationsParametres[i]<<endl;
            continue;
        }
        unsigned char c=TableOperationsParametres[i][0];
        if (c<'0' || c>'9')
            continue;

        int k=c-48;
        unsigned char d=TableOperationsParametres[i][1];
        if (d>='0' && d<='9') {
            k*=10;
            k+=d-48;
        }
        if (k==h) {
            c=0;
            cout<<TableOperationsParametres[i]<<Endl;
            i++;
			while ((c<'0' || c>'9') && c!='#') {
                cout<<TableOperationsParametres[i]<<Endl;
                c=TableOperationsParametres[++i][0];
            }
			if (c=='#')
				break;
        }
    }
}




fichiersscript::fichiersscript(char* a,char* nf) {
    alias=DupliqueChaine(a);
    nom_fichier=DupliqueChaine(nf);
    fichier=NULL;
}

char fichiersscript::ouvrir() {
    if (fichier!=NULL)
        return -1;
    fichier=new ofstream(nom_fichier);
    if (fichier==NULL)
        return -1;
    return 0;
}

fichiersscript::~fichiersscript() {
    if (fichier!=NULL) {
        fichier->close();
        delete fichier;
    }
    libere(nom_fichier);
    libere(alias);
}


void GlobalParseur::affiche_derniere_erreur(int numero) {
    if (erreur_survenue!=-1) {
        ostream* os=&cout;

        if (OS!=NULL)
            os=OS;
        if (numero!=-1 && VlabelVecteur[numero]!=NULL && VlabelVecteur[numero]->dr!=NULL) {
            *os<<"In file:"
                <<VlabelVecteur[numero]->dr->nom_fichier
                <<" line:"<<VlabelVecteur[numero]->dr->ligne
                <<Endl;
        }
        *os<<affichagemessageerreur[erreur_survenue]
           <<ajoute_info_erreur<<Endl;

		if (kifloader != NULL && erreur_survenue != 177) {
			if (kifdebug) {
				string mess = "XIP(001):";
				mess += affichagemessageerreur[erreur_survenue];
				mess += ajoute_info_erreur;
				kifloader->Returnerror(mess);
			}
			else
				initialise_erreur_instruction();
		}
    }
}

string retournemessageerreur(GlobalParseur* parseur) {
	return affichagemessageerreur[parseur->erreur_survenue];
}

void GlobalParseur::initialise_erreur_instruction() {
    derniere_erreur_survenue=erreur_survenue;
    erreur_survenue=-1;
    ajoute_info_erreur[0]=0;
}



char GlobalParseur::ajoutemot(string& nom,string& surface, string& lem, string& traits) {
     erreur_survenue=121;
	 return NON;
}


int rechercheNomFonction(char* nom,const char* table[]) {
    int i=0;

    if (nom[0]==0)
        return -1;

    while (table[i][0]!=0) {
        if (!strcmp(table[i],nom))
            return i;
        i++;
    }
    return -1;
}

char existeFonction(char* nom) {
    return rechercheNomFonction(nom,TableOperations);
}


inline string EnMinuscule(string& iterateur) {
    return s_to_lower(iterateur);
}

inline string EnMajuscule(string& iterateur) {
    return s_to_upper(iterateur);
}

#ifdef XIPLIBXML
TestFonctionXML::~TestFonctionXML() {
    delete clefxml;
}
#endif

int TestFormuleElementNoeud ::arg() {
    if (tf->Type()!=TESTFONCTIONNOEUD)
        return -1;

    return ((TestFonctionNoeud*)tf)->arggauche;
}

int TestFormuleElementNoeud ::argument(GlobalParseur* parseur,
                                       VECTA<Noeud*>& argNoeuds) {
    if (tf->Type()!=TESTFONCTIONNOEUD)
        return -1;


    TestFonctionNoeud* tfn=(TestFonctionNoeud*)tf;
    int argu=tfn->arggauche;
    if (argNoeuds[argu]==NULL ||
        tfn->traits[GAUCHE]==NULL ||
        tfn->TestNoeud(parseur,argNoeuds,NULL,NULL))
        return argu;
    return -1;
}

//----- Les variables sont declarees dans une classe


Variable::Variable(char* n,typeVariable ty,sorteVariable s,char init) {
	RAMASSE(Variable);
    type=ty;
    procedure=NON;
    nom=DupliqueChaine(n);
    sorte=s;
    position=-1;
    defaut=0;
    utilisee=NON;
    reinit=init;
}

Variable::~Variable() {
	JETTE(Variable);
    libere(nom);
}

VariableKif::~VariableKif() {
	kif=NULL;
}

VariableGraphe::~VariableGraphe() {}
void VariableGraphe::raz() {}

#ifdef XIPPYTHON
VariablePython::~VariablePython() {
    if (echange!=NULL)
        Py_DECREF(echange);
}
#endif

#ifdef XIPLIBXML
VariableXML::~VariableXML() {
}
#endif

VariableString::VariableString(char* n) : Variable(n,CHAINE,VCHAINE,NON) {
    chaine="";
}

VariableString::~VariableString() {
}

void VariableString::raz() {
    chaine="";
}

VariableVector::~VariableVector() {
    raz();
}


int VariableIntVector::recherchepremier(int val) {
    for (int i=0;i<valeurs.dernier;i++) {
        if (val==valeurs[i])
            return i;
    }
    return -1;
}

int VariableIntVector::recherchetous(int val,VariableIntVector* vvect) {
    int j=0;
    vvect->raz();
    for (int i=0;i<valeurs.dernier;i++) {
        if (val==valeurs[i])
            vvect->Affecte(j++,val);
    }
    return j;
}


void recuperechainerulespace(rulespace* r,VariableVector* varvector,int& ind,char type) {
	if (r==NULL)
		return;
	if (type==2 || r->actif==type) {
		string* s=new string;
		r->chaine(*s);
		varvector->AffecteValeurStringIndex(s,ind++);
	}
	for (int i=0;i<r->fils.dernier;i++)
		recuperechainerulespace(r->fils[i],varvector,ind,type);
}


int VariableVector::recherchepremier(string& ch) {
    for (int i=0;i<chaine.dernier;i++) {
        if (ch==*chaine[i])
            return i;
    }
    return -1;
}

int VariableVector::recherchetous(string& ch,VariableVector* vvect) {
    char chiff[100];
    int j=0;
    vvect->raz();
    for (int i=0;i<chaine.dernier;i++) {
        if (ch==*chaine[i]) {
            sprintf_s(chiff,100,"%d",i);
            vvect->AffecteValeurStringIndex(new string(ch),j++);
        }
    }
    return j;
}

int VariableVectorIndex::recherchepremier(string& ch) {
    etat* e=index.recherche((uchar*)ch.c_str());
    if (e==NULL)
        return -1;

    VECTA<int>* vic=(VECTA<int>*)e->valeur;
    return vic->cell(0);
}

int VariableVectorIndex::recherchetous(string& ch,VariableVector* vvect) {
    char chiff[100];
    etat* e=index.recherche((uchar*)ch.c_str());
    if (e==NULL)
        return 0;

    VECTA<int>* vic=(VECTA<int>*)e->valeur;
    vvect->raz();
    for (int iv=0;iv<vic->dernier;iv++) {
        sprintf_s(chiff,100,"%d",vic->cell(iv));
        vvect->AffecteValeurStringIndex(new string(chiff),iv);
    }

    return vic->dernier;
}


VariableIntVector::VariableIntVector(char* n,typeVariable ty) : Variable(n,ty,VVECTEURINT,NON) {}


VariableVector::VariableVector(char* n) : Variable(n,CHAINE,VVECTEURCHAINE,NON) {
}

void VariableIntVector::raz() {
    valeurs.raz();
}

void VariableVector::raz() {
    chaine.nettoie();
}

VariableVectorIndex::~VariableVectorIndex() {
    raz();
}

void VariableVectorIndex::raz() {
    VECTA<int>* vic;
	for (int i = 0; i<index.lesetats.dernier; i++) {
		if (index.lesetats[i] != NULL && index.lesetats[i]->valeur != NULL) {
			vic = (VECTA<int>*)index.lesetats[i]->valeur;
            delete vic;
			index.lesetats[i]->valeur = NULL;
        }
    }

    if (index.e.valeur!=NULL) {
        vic=(VECTA<int>*)index.e.valeur;
        delete vic;
        index.e.valeur=NULL;
    }

    index.nettoie();
    chaine.nettoie();
}

void VariableDictionnaire::raz() {
	dico.clear();
}

VariableDictionnaire::~VariableDictionnaire() {
    raz();
}

int VariableIntVector::inserevaleur(double s,int i) {
    valeurs.insertion(i,s);
    return OUI;
}


int VariableVector::inserechaine(string& s,int i) {
    chaine.insere(i,new string(s));
    return OUI;
}

int VariableVectorIndex::inserechaine(string& s,int ind) {
    VECTA<int>* vic;
    int k;
	for (int i = 0; i<index.lesetats.dernier; i++) {
		if (index.lesetats[i] != NULL && index.lesetats[i]->valeur != NULL) {
			vic = (VECTA<int>*)index.lesetats[i]->valeur;
            //on incremente toutes les valeurs d'index superieur ou egal a ind
            for (k=0;k<vic->dernier;k++) {
                if (vic->cell(k)>=ind)
                    vic->affecte(k,vic->cell(k)+1);
            }
        }
    }
    if (index.e.valeur!=NULL) {
        vic=(VECTA<int>*)index.e.valeur;
        //on decremente toutes les valeurs d'index superieur a ind
        for (k=0;k<vic->dernier;k++) {
            if (vic->cell(k)>=ind)
                vic->affecte(k,vic->cell(k)+1);
        }
    }

    chaine.insere(ind,new string(s));
    if (s.size()) {
        etat* e=index.recherche((uchar*)s.c_str());
        if (e!=NULL) {
            vic=(VECTA<int>*)e->valeur;
            for (k=0;k<vic->dernier;k++) {
                if (vic->cell(k)>=ind) {
                    vic->insertion(k,ind);
                    break;
                }
            }
            if (k==vic->dernier)
                vic->ajoute(ind);
            return OUI;
        }

        vic=new VECTA<int>;
        vic->ajoute(ind);
        index.parsebrute((uchar*)s.c_str(),(void*)vic);
    }
    return OUI;
}

void VariableDictionnaire::AffecteValeurStringIndex(string* s,int idi) {
    if (idi>=dico.size())
		return;
	map<string, string>::iterator iter=dico.begin();
	for (int i=0;i<idi;i++)
		iter++;
	(*iter).second=*s;
}


char VariableDictionnaire::cherche(string& cle,string& valeur) {
	map<string,string>::iterator it=dico.find(cle);
	if (it!=dico.end() && it->second==valeur)
		return OUI;
	return NON;
}

string VariableDictionnaire::valeur(string& clef) {
	map<string,string>::iterator it=dico.find(clef);
	if (it!=dico.end())
		return it->second;
	return "";
}

//Nous inserons une chaine dont l'index est aussi une chaine (ind)
void VariableDictionnaire::AffecteValeurStringIndexChaine(string* valeur,string cle) {
	dico[cle]=*valeur;
}



char VariableDictionnaire::rechercheindex(int idi,string& res) {
	if (idi>=dico.size())
		return NON;

	map<string, string>::iterator iter=dico.begin();
	for (int i=0;i<idi;i++)
		iter++;
	res=(*iter).first;
	return OUI;
}

int VariableDictionnaire::recherchechaineindex(string cle,string& res) {

	map<string, string>::iterator test=dico.find(cle);
	if (test==dico.end())
		return -1;

	int i=0;
	map<string, string>::iterator iter=dico.begin();
	for (;iter!=dico.end();iter++) {
		if (iter==test) {
			res=(*iter).second;
			break;
		}
		i++;
	}
	return i;
}

int VariableDictionnaire::recherchetoutechaineindex(string cle,VariableVector* vvect) {
    //On remplit le tableau avec toutes les infos que l'on peut
	map<string, string>::iterator test=dico.find(cle);
	if (test==dico.end())
		return -1;
	int i=0;
	map<string, string>::iterator iter=dico.begin();
	for (;iter!=dico.end();iter++) {
		if (iter==test) {
			int j=0;
			vvect->raz();
			vvect->AffecteValeurStringIndex(new string((*iter).second),j++);
			break;
		}
		i++;
	}
	return i;
}

int VariableDictionnaire::lesclefs(VariableVector* vvect) {
    //On remplit le tableau avec toutes les infos que l'on peut
	int i=0;
	map<string, string>::iterator iter=dico.begin();
	vvect->raz();
	for (;iter!=dico.end();iter++)
		vvect->AffecteValeurStringIndex(new string((*iter).first),i++);
	return i;
}

int VariableDictionnaire::lesvaleurs(VariableVector* vvect) {
    //On remplit le tableau avec toutes les infos que l'on peut
	int i=0;
	map<string, string>::iterator iter=dico.begin();
	vvect->raz();
	for (;iter!=dico.end();iter++)
		vvect->AffecteValeurStringIndex(new string((*iter).second),i++);
	return i;
}

int VariableIntVector::retirevaleur(int i) {
    if (i<valeurs.dernier) {
        valeurs.retireElement(i);
        return OUI;
    }
    return NON;
}


int VariableVector::retirechaine(int i) {
    if (i<chaine.dernier) {
        string* s=chaine.retireElement(i);
        delete s;
        return OUI;
    }
    return NON;
}

int VariableVectorIndex::retirechaine(int ind) {
    //Beaucoup plus complexe, on doit tenir compte des indexes enregistrees
    //Pour toutes les valeurs qui suivent, on doit decrementer leur indexes respectifs
    //s'ils sont superieurs a i
    if (ind>=chaine.dernier)
        return NON;

    VECTA<int>* vic;
    int k;
	for (int i = 0; i<index.lesetats.dernier; i++) {
		if (index.lesetats[i] != NULL && index.lesetats[i]->valeur != NULL) {
			vic = (VECTA<int>*)index.lesetats[i]->valeur;
            //on decremente toutes les valeurs d'index superieur a ind
            for (k=0;k<vic->dernier;k++) {
                if (vic->cell(k)>ind)
                    vic->affecte(k,vic->cell(k)-1);
            }
        }
    }
    if (index.e.valeur!=NULL) {
        vic=(VECTA<int>*)index.e.valeur;
        //on decremente toutes les valeurs d'index superieur a ind
        for (k=0;k<vic->dernier;k++) {
            if (vic->cell(k)>ind)
                vic->affecte(k,vic->cell(k)-1);
        }
    }

    if (chaine[ind]!=NULL && chaine[ind]->size()) {
        uchar* ch=(uchar*)chaine[ind]->c_str();
        etat* e=index.recherche(ch);
        vic=(VECTA<int>*)e->valeur;
        if (vic->dernier==1) {
            index.retirebrute(ch);
            delete vic;
        }
        else {
            for (k=0;k<vic->dernier;k++) {
                if (vic->cell(k)==ind) {
                    vic->retireElement(k);
                    break;
                }
            }
        }

        string* s=chaine.retireElement(ind);
        delete s;
    }
    return OUI;
}

void VariableIntVector::Affecte(int i,double v) {
    valeurs.affecte(i,v);
}

void VariableVector::AffecteValeurStringIndex(string* v,int i) {
    if (chaine[i]!=NULL && chaine[i]->size()) {
        if (*chaine[i]==*v)
            return;
        delete chaine[i];
    }

    chaine.affecte(i,v);
}

void VariableVectorIndex::AffecteValeurStringIndex(string* v,int i) {
    etat* e;
    VECTA<int>* vic;
    int j;

    if (chaine[i]!=NULL && chaine[i]->size()) {
        if (*chaine[i]==*v)
            return;
        uchar* ch=(uchar*)chaine[i]->c_str();
        e=index.recherche(ch);
        vic=(VECTA<int>*)e->valeur;
        if (vic->dernier==1) {
            index.retirebrute(ch);
            delete vic;
        }
        else {
            for (int j=0;j<vic->dernier;j++) {
                if (vic->cell(j)==i) {
                    vic->retireElement(j);
                    break;
                }
            }
        }

        delete chaine[i];
    }

    chaine.affecte(i,v);
    if (v->size()) {
        e=index.recherche((uchar*)v->c_str());
        if (e!=NULL) {
            vic=(VECTA<int>*)e->valeur;
            for (j=0;j<vic->dernier;j++) {
                if (vic->cell(j)>=i) {
                    vic->insertion(j,i);
                    break;
                }
            }
            if (j==vic->dernier)
                vic->ajoute(i);
            return;
        }

        vic=new VECTA<int>;
        vic->ajoute(i);
        index.parsebrute((uchar*)v->c_str(),(void*)vic);
    }
}

TestFormuleElementVectorGraphe::~TestFormuleElementVectorGraphe() {
}

TestFormuleElementString::~TestFormuleElementString() {
}

TestFormuleElementIntVector::~TestFormuleElementIntVector() {
}

TestFormuleElementVectorString::~TestFormuleElementVectorString() {
}


void VariableString::AffecteValeurString(unsigned char* v) {
    chaine=(char*)v;
}

double VariableIndexeeNoeud::Valeurs(Noeud* n) {
    for (int i=0;i<indexes.dernier;i++)
        if (n==indexes[i])
            return valeurs[i];
        return -1;
}

string VariableIndexeeDependance::valdep(ResultatFonction* rf) {
	for (int i=0;i<indexes.dernier;i++)
		if (rf==indexes[i]) {
			char ch[100];
			if (type==REEL)
				sprintf_s(ch,100,"%g",valeurs[i]);
			else
				sprintf_s(ch,100,"%d",valeurs[i]);

			return ch;
		}
		return "";
}

string VariableIndexeeChaineDependance::valdep(ResultatFonction* rf) {
	for (int i=0;i<indexes.dernier;i++) {
		if (rf==indexes[i]) {
			if (valeurs[i]!=NULL)
				return *valeurs[i];
		}
	}
	return "";
}

int VariableIndexeeNoeud::AjouteNoeud(Noeud* n,char ajout) {
	for (int i=0;i<indexes.dernier;i++)
		if (n==indexes[i])
			return i;
	if (ajout==OUI) {
		indexes.ajoute(n);
		return indexes.dernier-1;
	}
	return -1;
}

int VariableIndexeeChaine::AjouteNoeud(Noeud* n,char ajout) {
	for (int i=0;i<indexes.dernier;i++)
		if (n==indexes[i])
			return i;
	if (ajout==OUI) {
		indexes.ajoute(n);
		return indexes.dernier-1;
	}
	return -1;
}


int VariableIndexeeDependance::AjouteDependance(ResultatFonction* n,char ajout) {
	for (int i=0;i<indexes.dernier;i++) {
		if (n==indexes[i])
			return i;
	}
	if (ajout==OUI) {
		indexes.ajoute(n);
		return indexes.dernier-1;
	}
	return -1;
}

int VariableIndexeeChaineDependance::AjouteDependance(ResultatFonction* n,char ajout) {
	for (int i=0;i<indexes.dernier;i++) {
		if (n==indexes[i])
			return i;
	}
	if (ajout==OUI) {
		indexes.ajoute(n);
		return indexes.dernier-1;
	}
	return -1;
}

//-----------------------------------------------------------------------------
void majvariablelemme(VariableLemmeIndex* vlemme,Noeud* n,uchar* chaine,int pos,string value,double res) {
   string vs;

	//On construit une expression reguliere
    if (n!=NULL)
        n->Lemme(vs);
    else {
        //Sinon on relit une partie de la chaine pour trouver le lemme
        char* pt=strstr((char*)chaine,"##");
        int pos=0;
        //Une chaine est de la forme: var##POS#lemme
        if (pt!=NULL && pt[2]!=0) {
            char* pt2=strchr(pt+2,'#');
            if (pt2!=NULL) {
                pt2++;
                while (*pt2==' ') pt2++;
				//vs.push_back(pt2);
				vs=pt2;
            }
        }
    }
	if (vs!="") {
		if (vlemme->sorte==VLEMMECHAINEINDEX) {
			VariableString* varglob=vlemme->inserechaine(vs,chaine,pos);
			if (varglob!=NULL) {
				varglob->utilisee=OUI;
				varglob->chaine=value;
			}
		}
		else {
			VariableGlobale* varglob=vlemme->insere(vs,chaine,pos);
			if (varglob!=NULL) {
				varglob->utilisee=OUI;
				varglob->valeur=res;
			}
		}
	}
}

VariableGlobale* VariableLemmeIndex::recherche(unsigned char* mot,int ipos) {
	hmap<string,Variable*>::iterator test=lemmes.find((char*)mot);
	if (test==lemmes.end())
		return NULL;

	return (VariableGlobale*)((*test).second);
}

VariableString* VariableLemmeChaineIndex::recherchechaine(unsigned char* mot,int ipos) {
	hmap<string,Variable*>::iterator test=lemmes.find((char*)mot);
	if (test==lemmes.end())
		return NULL;

	return (VariableString*)((*test).second);
}


VariableGlobale* VariableLemmeIndex::insere(string& expression,uchar* chaine,int ipos) {
    int index=listes.dernier;

    if (expression.size()==0)
        return NULL;

    //On ajoute cette expression a notre automate
    VariableGlobale* vg=recherche(USTR(expression),0);
	if (vg==NULL) {
        vg=new VariableGlobale((char*)chaine,type,VGLOBALE,NON);
        lemmes[expression]=vg;
        listes.ajoute(vg);
    }
    return vg;
}

VariableString* VariableLemmeChaineIndex::inserechaine(string& expression,uchar* chaine,int ipos) {
    int index=listes.dernier;

    if (expression.size()==0)
        return NULL;

    //On ajoute cette expression a notre automate
    VariableString* vg=recherchechaine(USTR(expression),0);
	if (vg==NULL) {
        vg=new VariableString((char*)chaine);
        lemmes[expression]=vg;
        listes.ajoute(vg);
    }
    return vg;
}

void VariableLemmeIndex::affiche(ostream& os) {
    for (int i=0;i<listes.dernier;i++)
        listes[i]->affiche(os);
}


void VariableLemmeIndex::afficheXML(ostream& os) {
    for (int i=0;i<listes.dernier;i++)
        listes[i]->afficheXML(os);
}

void VariableLemmeIndex::afficheNoeudXML(ostream& os,Noeud* n) {
    afficheXML(os);
}
//-----------------------------------------------------------------------------

void VariableString::afficheXML(ostream& os) {
    if (chaine.size()>0) {
        os<<"<FEATURE attribute=\"#"<<nom<<"\" value=\"";
        os<<chaine;
        os<<"\"/>"<<Endl;
    }
}

void VariableString::afficheNoeudXML(ostream& os,Noeud* n) {
    afficheXML(os);
}

void VariableIntVector::afficheXML(ostream& os) {
    for (int i=0;i<valeurs.size();i++) {
        os<<"<FEATURE attribute=\"#"<<nom<<"("<<i<<")\" value= "<<valeurs[i]<<";"<<Endl;
        os<<"\"/>"<<Endl;
    }

}

void VariableIntVector::afficheNoeudXML(ostream& os,Noeud* n) {
    afficheXML(os);
}



void VariableVector::afficheXML(ostream& os) {
    for (int i=0;i<chaine.size();i++) {
        if (chaine[i]!=NULL) {
            os<<"<FEATURE attribute=\"#"<<nom<<"("<<i<<")\" value= "<<chaine[i]->c_str()<<";"<<Endl;
            os<<"\"/>"<<Endl;
        }
    }

}

void VariableVector::afficheNoeudXML(ostream& os,Noeud* n) {
    afficheXML(os);
}


void VariableGlobale::afficheXML(ostream& os) {
    os<<"<FEATURE attribute=\"#"<<nom<<"\" value=\"";
    if (type==REEL)
        os<<valeur;
    else
        os<<(int)valeur;
    os<<"\"/>"<<Endl;
}

void VariableGlobale::afficheNoeudXML(ostream& os,Noeud* n) {
    afficheXML(os);
}

void VariableIndexee::afficheXML(ostream& os) {}

void VariableIndexeeDependance::affichageDependanceXML(ostream& os,ResultatFonction* rf) {
    for (int id=0;id<indexes.dernier;id++)
        if (rf==indexes[id]) {
            os<<"<FEATURE attribute=\"#"<<nom<<"\" value=\"";
            if (type==REEL)
                os<<valeurs[id];
            else
                os<<(int)valeurs[id];
            os<<"\"/>"<<Endl;
            return;
        }
}

void VariableIndexeeChaineDependance::affichageDependanceXML(ostream& os,ResultatFonction* rf) {
    for (int id=0;id<indexes.dernier;id++)
        if (rf==indexes[id]) {
            os<<"<FEATURE attribute=\"#"<<nom<<"\" value=\"";
			os<<(char*)valeurs[id]->c_str();
            os<<"\"/>"<<Endl;
            return;
        }
}


void VariableIndexeeNoeud::afficheNoeudXML(ostream& os,Noeud* n) {
    for (int i=0;i<indexes.dernier;i++)
        if (n==indexes[i]) {
            os<<"<FEATURE attribute=\"#"<<nom<<"\" value=\"";
            if (type==REEL)
                os<<valeurs[i];
            else
                os<<(int)valeurs[i];
            os<<"\"/>"<<Endl;
            return;
        }
}

void VariableIndexeeChaine::raz() {
	valeurs.nettoie();
	indexes.raz();
	utilisee=NON;
}

void VariableIndexeeChaine::afficheNoeudXML(ostream& os,Noeud* n) {
    for (int i=0;i<indexes.dernier;i++)
        if (n==indexes[i]) {
            os<<"<FEATURE attribute=\"#"<<nom<<"\" value=\"";
                os<<(char*)valeurs[i]->c_str();
            os<<"\"/>"<<Endl;
            return;
        }
}

#ifdef XIPLIBXML
//Il s'agit d'une serie de methodes pour creer des noeuds xml pour sauvegarder les chiffres

void VariableLemmeIndex::CreationXMLNoeud(GlobalParseur* parseur,xmlNodePtr parent,Noeud* n) {
    CreationXML(parseur,parent);
}

void VariableString::CreationXMLNoeud(GlobalParseur* parseur,xmlNodePtr parent,Noeud* n) {
    CreationXML(parseur,parent);
}

void VariableString::CreationXML(GlobalParseur* parseur,xmlNodePtr parent) {
    char ch[500];
    if (chaine.size()>0) {
        xmlNodePtr feature=parseur->CreationNoeudXML(parent,"FEATURE",NULL);
        sprintf_s(ch,500,"#%s",nom);
        parseur->AjouteAttributValeur(feature,"attribute",ch);
        parseur->AjouteAttributValeur(feature,"value",(char*)chaine.c_str());
    }
}

void VariableIntVector::CreationXMLNoeud(GlobalParseur* parseur,xmlNodePtr parent,Noeud* n) {
    CreationXML(parseur,parent);
}

void VariableIntVector::CreationXML(GlobalParseur* parseur,xmlNodePtr parent) {
    char ch[500];
    for (int i=0;i<valeurs.size();i++) {
        xmlNodePtr feature=parseur->CreationNoeudXML(parent,"FEATURE",NULL);
        sprintf_s(ch,500,"#%s(%d)",nom,i);
        parseur->AjouteAttributValeur(feature,"attribute",ch);
        sprintf_s(ch,500,"%d",valeurs[i]);
        parseur->AjouteAttributValeur(feature,"value",ch);
    }
}


void VariableVector::CreationXMLNoeud(GlobalParseur* parseur,xmlNodePtr parent,Noeud* n) {
    CreationXML(parseur,parent);
}

void VariableVector::CreationXML(GlobalParseur* parseur,xmlNodePtr parent) {
    char ch[500];
    for (int i=0;i<chaine.size();i++) {
        if (chaine[i]!=NULL) {
            xmlNodePtr feature=parseur->CreationNoeudXML(parent,"FEATURE",NULL);
            sprintf_s(ch,500,"#%s(%d)",nom,i);
            parseur->AjouteAttributValeur(feature,"attribute",ch);
            parseur->AjouteAttributValeur(feature,"value",(char*)chaine[i]->c_str());
        }
    }
}


void VariableGlobale::CreationXMLNoeud(GlobalParseur* parseur,xmlNodePtr parent,Noeud* n) {
    CreationXML(parseur,parent);
}

void VariableGlobale::CreationXML(GlobalParseur* parseur,xmlNodePtr parent) {
    char ch[500];
    xmlNodePtr feature=parseur->CreationNoeudXML(parent,"FEATURE",NULL);
    sprintf_s(ch,500,"#%s",nom);
    parseur->AjouteAttributValeur(feature,"attribute",ch);
    if (type==REEL)
        sprintf_s(ch,500,"%g",valeur);
    else
        sprintf_s(ch,500,"%d",(int)valeur);
    parseur->AjouteAttributValeur(feature,"value",ch);
}


void VariableLemmeIndex::CreationXML(GlobalParseur* parseur,xmlNodePtr parent) {
    for (int i=0;i<listes.dernier;i++)
        listes[i]->CreationXML(parseur,parent);
}

void VariableIndexee::CreationXML(GlobalParseur* parseur,xmlNodePtr parent) {}


void VariableIndexeeDependance::CreationXMLDependance(GlobalParseur* parseur,
                                                      xmlNodePtr parent,
                                                      ResultatFonction* rf) {
    char ch[500];
    for (int id=0;id<indexes.dernier;id++)
        if (rf==indexes[id]) {
            xmlNodePtr feature=parseur->CreationNoeudXML(parent,"FEATURE",NULL);
            sprintf_s(ch,500,"#%s",nom);
            parseur->AjouteAttributValeur(feature,"attribute",ch);
            if (type==REEL)
                sprintf_s(ch,500,"%g",valeurs[id]);
            else
                sprintf_s(ch,500,"%d",valeurs[id]);
            parseur->AjouteAttributValeur(feature,"value",ch);
            return;
        }
}

void VariableIndexeeChaineDependance::CreationXMLDependance(GlobalParseur* parseur,
                                                      xmlNodePtr parent,
                                                      ResultatFonction* rf) {
	char ch[500];
    for (int id=0;id<indexes.dernier;id++)
        if (rf==indexes[id]) {
            xmlNodePtr feature=parseur->CreationNoeudXML(parent,"FEATURE",NULL);
            sprintf_s(ch,500,"#%s",nom);
            parseur->AjouteAttributValeur(feature,"attribute",ch);
			if (valeurs[id]!=NULL)
				parseur->AjouteAttributValeur(feature,"value",(char*)valeurs[id]->c_str());
			else
				parseur->AjouteAttributValeur(feature,"value",ch);
            return;
        }
}

void VariableIndexeeNoeud::CreationXMLNoeud(GlobalParseur* parseur,xmlNodePtr parent,Noeud* n) {
    char ch[500];
    for (int i=0;i<indexes.dernier;i++)
        if (n==indexes[i]) {
            xmlNodePtr feature=parseur->CreationNoeudXML(parent,"FEATURE",NULL);
            sprintf_s(ch,500,"#%s",nom);
            parseur->AjouteAttributValeur(feature,"attribute",ch);
            if (type==REEL)
                sprintf_s(ch,500,"%g",valeurs[i]);
            else
                sprintf_s(ch,500,"%d",valeurs[i]);
            parseur->AjouteAttributValeur(feature,"value",ch);
            return;
        }
}

void VariableIndexeeChaine::CreationXMLNoeud(GlobalParseur* parseur,xmlNodePtr parent,Noeud* n) {
    char ch[500];
    for (int i=0;i<indexes.dernier;i++)
        if (n==indexes[i]) {
            xmlNodePtr feature=parseur->CreationNoeudXML(parent,"FEATURE",NULL);
            sprintf_s(ch,500,"#%s",nom);
            parseur->AjouteAttributValeur(feature,"attribute",ch);
			sprintf_s(ch,500,(char*)valeurs[i]->c_str());
            parseur->AjouteAttributValeur(feature,"value",ch);
            return;
        }
}

#endif

//-------------------------------------------------------------------------------------
TestFormuleElementAutomate::TestFormuleElementAutomate(uchar* expression) {
    type=VAUTOMATE;
	a=new automate(expression,(void*)1);
}

TestFonctionKif::TestFonctionKif(Cat* X,KifElement* p) {
	Fx=X;
	proc_kif=(KifFunction*)p;
}

TestFonctionKif::~TestFonctionKif() {}


TestFonctionNoeud::TestFonctionNoeud() : arg(3,-2), traits(3) {
    arggauche=-1;
    argdroit=-1;
    comparateur=COMPATIBLES;
    selection=NULL;
    selectiontous=NULL;
	X=NULL;
}

TestFonctionDependance::TestFonctionDependance(Cat* X) : arg(3,-2), traits(3) {
    Fx=X;
    changement=-1;
    traitsFonction=NULL;
    clearStack=NON;
	final=NON;
    vardep=-1;
    leType=TESTFONCTIONDEPENDANCE;
    pointeur=NON;
	if (X->type==C_GENERATION)
		typecat=C_GENERATION;
	else
		typecat=C_FONCTION;
}

TestFonctionNoeud::~TestFonctionNoeud() {
    //if (selection!=NULL)
    //    delete selection;
    //if (selectiontous!=NULL)
    //    delete selectiontous;
}

TestFonctionDependance::~TestFonctionDependance() {
    for (int ist=0;ist<stack.dernier;ist++)
        libere(stack[ist]);
}

//-------------------------------------------------------------------------
inline typeVariable RechercheType(char* tampon) {
    if (!strcmp(tampon,"float"))
        return REEL;

    if (!strcmp(tampon,"int"))
        return ENTIER;

    return AUCUN;
}

inline sorteVariable RechercheSorte(char* tampon,char& index,typeVariable& ty) {

    if (!strcmp(tampon,"float")) {
        ty=REEL;
        return VGLOBALE;
    }

    if (!strcmp(tampon,"int")) {
        ty=ENTIER;
        return VGLOBALE;
    }

    if (!strcmp(tampon,"string")) {
        ty=CHAINE;
        return VCHAINE;
    }

    if (!strcmp(tampon,"vector")) {
        ty=CHAINE;
        return VVECTEURCHAINE;
    }

    if (!strcmp(tampon,"vint")) {
        ty=ENTIER;
        return VVECTEURINT;
    }

    if (!strcmp(tampon,"vfloat")) {
        ty=REEL;
        return VVECTEURINT;
    }

    if (!strcmp(tampon,"vindex")) {
        index=OUI;
        ty=CHAINE;
        return VVECTEURCHAINE;
    }

    if (!strcmp(tampon,"dictionary")) {
        ty=CHAINE;
        return VDICTIONNAIRE;
    }

    if (!strcmp(tampon,"graph")) {
        ty=GRAPHE;
        return VGRAPHE;
    }


#ifdef XIPPYTHON
    if (!strcmp(tampon,"python")) {
        ty=PYTHONOBJET;
        return VPYTHONOBJET;
    }
#endif
#ifdef XIPLIBXML
    if (!strcmp(tampon,"xml")) {
        ty=XMLOBJET;
        return VXMLOBJET;
    }
#endif
    ty=AUCUN;
    return VAUCUN;
}

Variable* GlobalParseur::CreeVariable(char* tampon,typeVariable ty,sorteVariable sorte,char index,char ajout,char proc) {
    Variable* v=NULL;
    string err;

    if (RetrouveFonc(tampon)!=NULL ||
        RetrouveCat(tampon)!=NULL ||
        rechercheNomFonction(tampon,TableOperations)!=-1) {
			err=CHAINES[324];
			err+=tampon;
			erreur(STR(err));
    }

    char reinitialisation=OUI;


    switch(sorte) {
    case VVECTEURINT:
        reinitialisation=NON;
        v=new VariableIntVector(tampon,ty);
        break;
	case VGRAPHE:
		reinitialisation=NON;
		v= new VariableGraphe(tampon,ty);
		break;
	case VVECTEURGRAPHE:
		reinitialisation=NON;
		v= new VariableVectorGraphe(tampon,ty);
		break;
	case VKIFOBJET:
        reinitialisation=NON;
        v=new VariableKif(tampon,ty);
        break;
#ifdef XIPPYTHON
    case VPYTHONOBJET:
        reinitialisation=NON;
        v=new VariablePython(tampon,PYTHONOBJET,VPYTHONOBJET,NULL);
        break;
#endif
#ifdef XIPLIBXML
    case VXMLOBJET:
        reinitialisation=NON;
        v=new VariableXML(tampon,XMLOBJET,VXMLOBJET,NULL);
        break;
#endif
    case VDICTIONNAIRE:
        reinitialisation=NON;
        v = new VariableDictionnaire(tampon);
        break;
    case VCHAINE:
        reinitialisation=NON;
		if (strchr(tampon,'#')) {
			if (strstr(tampon,"##")) {
				VariableLemmeChaineIndex* vlemme=new VariableLemmeChaineIndex(tampon,ty,VLEMMECHAINEINDEX);
				VariablesLemmes.ajoute(vlemme);
				v=vlemme;
				reinitialisation=NON;
			}
			else {
				reinitialisation=OUI;
				v = new VariableIndexeeChaine(tampon,ty,VCHAINENOEUD);
			}
		}
		else {
			if (strchr(tampon,'$')) {
				reinitialisation=OUI;
				v = new VariableIndexeeChaineDependance(tampon,ty,VCHAINEDEPENDANCE);
			}
			else
				v= new VariableString(tampon);
		}
        //Dans une procedure, une variable d'initialisation qui commence avec un '_'
        //est une variable passee par reference, sinon, c'est une variable locale
        if (proc==OUI && tampon[0]!='_')
            reinitialisation=OUI;
        break;
    case VVECTEURCHAINE:
        //Une variable de chaine s'ecrit: nom...
        reinitialisation=NON;
        if (index==OUI)
            v= new VariableVectorIndex(tampon);
        else
            v= new VariableVector(tampon);
        break;
    default:
        if (strchr(tampon,'#')) {
            if (strstr(tampon,"##")) {
                VariableLemmeIndex* vlemme=new VariableLemmeIndex(tampon,ty,VLEMMEINDEX);
                VariablesLemmes.ajoute(vlemme);
                v=vlemme;
                reinitialisation=NON;
            }
            else
                v = new VariableIndexeeNoeud(tampon,ty,VNOEUD);
        }
        else
            if (strchr(tampon,'$'))
                v = new VariableIndexeeDependance(tampon,ty,VDEPENDANCE);
            else
                if (tampon[0]=='_') {
                    v= new VariableGlobale(tampon,ty,VGLOBALE,NON);
                    reinitialisation=NON;
                }
				else {
                    v= new VariableGlobale(tampon,ty,VGLOBALEREINIT,OUI);
					if (proc==OUI)
						reinitialisation=OUI;
					else
						reinitialisation=NON;
				}

    }

    v->position=Variables.dernier;

    char* pt=strstr(tampon,"##");
    int pos=0;
    char* lelemme=NULL;
    if (pt!=NULL && pt[2]!=0) {
        lelemme=strchr(pt+2,'#');
        if (lelemme!=NULL) {
            pos=lelemme-tampon+1;
            lelemme++;
            while (*lelemme==32) lelemme++;
        }
        pt[2]=0;
    }

    v->reinit=reinitialisation;

    //On ne l'ajoute pas forcement dans notre ensemble de variable (pour les structs)
    if (ajout==NON)
        return v;

    etat* e=VariablesNoms.recherche(tampon);
    //Relecture de variables
    if (e!=NULL && e->valeur!=NULL) {
		err=CHAINES[309];
		err+=tampon;
        erreur(STR(err));
    }



    //s'il s'agit d'une nouvelle variable
    if (v!=NULL) {
        Variables.ajoute(v);
        VariablesNoms.parsebrute((uchar*)v->nom,(void*)v);
    }
    return v;
}


//On lit les declarations de variables, qui sont pour l'instant de deux sortes:
//int et double.
char GlobalParseur::LitVariables(istream& fichier,NomProcedure* nomproc) {
    char k=0,tampon[XIPBUFFERSIZE];
	char lect[XIPBUFFERSIZE];
    string errmessage;
    etat* e;
    typeVariable ty=AUCUN;
    sorteVariable sorte=VAUCUN;
    Variable* v;
    char index=NON;


    while (k!=':') {
        k=lit(fichier,tampon);

        //c'est une variable de noeud de type #num
        if (nomproc!=NULL && k=='#') {
            k=lit(fichier,tampon);
            //Une variable
            int num_arg=atoi(tampon);
            nomproc->arg.ajoute(num_arg);
            return k;
        }

        //c'est une variable de dependance de type $num
        if (nomproc!=NULL && tampon[0]=='$') {
            //Une variable
            int num_arg=atoi(tampon+1)+1;
            nomproc->arg.ajoute(num_arg*-1);
            return k;
        }

        index=NON;
        sorte=VAUCUN;
        ty=RechercheType(tampon);
        sorte=RechercheSorte(tampon,index,ty);

        if (ty==AUCUN) {
            if (k==':' && nomproc==NULL)
                return AppelCons(fichier,k,tampon);

            e=VariablesNoms.recherche(tampon);
            char err=OUI;
            if (e!=NULL && e->valeur!=NULL)
                v=(Variable*)e->valeur;
            if (err==OUI) {
				sprintf_s(lect, XIPBUFFERSIZE, "%s: %s", CHAINES[303], tampon);
                erreur(lect);
            }
        }

        RemetCaractere(k);

        //------------------------------------------------------------------------------

        //Lecture d'une variable non structure
        k=litFormule(fichier,lect);
        if (nomproc==NULL)
            strcpy(tampon,lect);
        else
			sprintf_s(tampon, XIPBUFFERSIZE,"%s::%s", lect, nomproc->Fx->nom);
        char vecteur=NON;
        //lecture d'une declaration de tableau
        TestFormule* val=NULL;
        VECTA<char> variables_utilisees;
        if (k=='[') {
            vecteur=OUI;
            if (ty==CHAINE)
                sorte=VVECTEURCHAINE;
            else {
                if (ty==REEL || ty==ENTIER)
                    sorte=VVECTEURINT;
                else
					if (ty==GRAPHE)
						sorte=VVECTEURGRAPHE;
            }

            k=lit(fichier,lect);
            if (k!=']')
                erreur(CHAINES[116]);
            k=lit(fichier,lect);
        }
        else {
            //Est-ce qu'il y a une valeur d'initialisation par defaut ?
            if (k=='=')
                val=LireUneFormule(fichier,k,variables_utilisees,OUI);
        }


        if (nomproc!=NULL) {
            v=CreeVariable(tampon,ty,sorte,index,OUI,OUI);
			//Cas particulier pour le traitement des variables XML
			//qui sont globales par definition, mais que l'on peut forcer ici a etre locale.
			if (v->sorte==VXMLOBJET && v->nom[0]!='_')
				v->reinit=OUI;
            v->defaut=val;
            v->procedure=OUI;
            nomproc->parametres.ajoute(v);
            return k;
        }

        v=CreeVariable(tampon,ty,sorte,index,OUI,NON);
        v->defaut=val;

        if (k!=';')
            erreur(CHAINES[132]);
    }

    return NON;
}
//---------------------------------------------------------------------------------------------------

int GlobalParseur::VariableGeneration(string v,bool cree) {
	if (variablesgenerations.find(v)==variablesgenerations.end()) {
		if (cree==true)
			variablesgenerations[v]=10+variablesgenerations.size();
		else {
			static string err="Unknown NODE variable: ";
			err+=v;
			erreur(STR(err));
		}
	}
	return variablesgenerations[v];
}


inline int decaler(char* suite, int nb) {
	int taille = strlen(suite);
	for (int i = taille - 1; i >= 0; i--)
		suite[i + nb] = suite[i];
	suite[taille + nb] = 0;
	return (taille + nb);
}

TestFonction* GlobalParseur::LectureArgumentTest(istream& fichier,
                                                 VECTA<char>& variables_utilisees,
                                                 char carfin,
                                                 char* tampon,
                                                 char& k,
                                                 char neg,
                                                 char lectxml) {


	//On recule de la longueur de la chaine deja extraits
	char suite[XIPBUFFERSIZE];

    Categorie* c;


    char cgt=0;
    int varnum1=-1;
    int varnum2=-1;
    int vardep=-1;
    suite[0]=0;
    uchar K;


	//Cas particulier, les variables $# dont le $ est dans tampon et le # dans k
	if (k=='#' && tampon[0]=='$') {
		//dans ce cas, on reconstruit, un tampon avec k
		strcatchr(tampon,k);
		k=lit(fichier,tampon+2);
	}
    //s'il s'agit de la lecture d'une dependance sous la forme d'une variable
    //if (vardep==1)
    //    erreur(CHAINES[219]);
    //Un '#' peut preceder une dependance et etre equivalent a #subj <=> ^subj

    if (k=='#' || k=='^') {
        cgt=OUI; //cette variable indique que nous lisons une dependance qui doit etre modifiee
        //k=lit(fichier,tampon);
        //S'il s'agit d'une variable, c'est a dire que nous avons plutot #1,#2 etc...
        //nous sommes dans le cas d'un test
        K=k;
        lireunevariable(fichier,K,tampon);
        k=(char)K;
        if (tampon[0] >= '0' && tampon[0] <= '9') {
            //Une variable
            varnum1=atoi(tampon);
			variables_utilisees.affecte(varnum1,OUI);
            cgt=NON;
        }
    }
    else {
        if (k==NEGATION) {
            k=lit(fichier,tampon);
            neg=OUI;
        }
    }


    //Lecture d'une variable sur une dependance de type $x
    char* pt=strchr(tampon,'$');
    char pointeur=NON;
	char NODE=NON;
    if (pt!=NULL) {
        //cas particulier d'une variable $$x qui pointe sur la dependance elle-meme
        if (pt[1]=='$') {
            strcpy(pt,pt+1);
            pointeur=OUI;
        }
		//Autre cas particulier, une dependance de NODE
		if (pt[1]=='#') {
            strcpy(pt+1,pt+2);
            NODE=OUI;
        }

		if (pt[1]>='0' && pt[1]<='9')
			vardep=atoi(pt+1)+1;
		else {
			NODE=OUI;
			if (tampon[1]==0) {
				if (k=='(') {
					NODE=NON;
					vardep=1;
				}
				else {
					tampon[1]=k;
					k=lit(fichier,tampon+2);
				}
			}
			if (NODE==OUI)
				vardep=VariableGeneration(pt+1,true);
		}
		*pt=0;
    }


    Traits* trgauche=NULL;
    Traits* trdroit=NULL;
    Traits* trlexical=NULL;
    Traits* trlexicaltous=NULL;
	Traits* modiftrait=NULL;
    VECTA<char> affectations;

    //Nous avons deux cas possibles:
    //a) Nous lisons une fonction syntaxique
    //b) Nous lisons un test sur les traits d'une variable

    //Lecture des traits associes a la fonction
    //ou a la premiere variable
    VECTA<char*> pile;
    char clrStk=NON;
    char surlecture=NON;
    /*
    Les structures en tete sont les suivantes:

  #1[..]            trlexical=NULL trlexicaltous=NULL trgauche=xxxx
  #1<..>[..]        trlexical=xxx trlexicaltous=NULL trgauche=xxxx
  #1<*..><..>[..]   trlexical=xxx trlexicaltous=xxx trgauche=xxxx
  */


    if (k=='<') { //trait local a une lecture d'un mot donne
        char interk=litBis(fichier,suite);

        if (interk=='*') {
            trlexicaltous=BatitTraits(fichier,'>',&pile,&clrStk);
			//trgauche=trlexicaltous;
            k=lit(fichier,suite);
            if (k=='<') {
                trlexical=BatitTraits(fichier,'>',&pile,&clrStk);
                k=lit(fichier,suite);
            }
            surlecture=OUI;
        }
        else {
            MetEnAttenteLecture(interk,suite);
            //sinon, il faut verifier la chose suivante
            //a) s'il s'agit d'une comparaison '<', alors apres on doit avoir forcement
            //les choses suivantes pour interk: &,|,),#
            //sinon, c'est une autre comparaison et interk ne peut valoir que > ou ":" ou "="
            if (!strchr("&|)#",interk)) {
                trlexical=BatitTraits(fichier,'>',&pile,&clrStk);
				//trgauche=trlexical;
                k=lit(fichier,suite);
                surlecture=OUI;
            }
        }
    }

    char etat=0;
    char etatsuivant=0;
    if (k=='[') {
        if (trgauche!=NULL)
            trlexical=trgauche;
        trgauche=BatitTraits(fichier,']',&pile,&clrStk);
        if (carfin==']') {
            k=LectureMin(fichier);
            etat=25;
            etatsuivant=40;
        }
        else
            k=lit(fichier,suite);
    }

    c=CatVide;
    typeComparateur exacte=TEST;
    TestFonction* tf=NULL;


    int nv,i;
    char pair;
	Traits* trvar;
	VECTA<Traits*> traits_variables;
    //--------------------------------------------------------------------------------------
    //Traitement des comparaisons de traits
    /*
    Tous les cas sont les suivants:

    EGAUX_ONTOLOGIE         #1[@ontologie] :: #2[@ontologie]
    COMPATIBLES_ONTOLOGIE   #1[@ontologie] : #2[@ontologie]
    AFFECTATION_ONTOLOGIE   #1[@ontologie] = {#2 | #3..}


    COMPATIBLES_LECTURE     #1<tr>:#2<tr>
    AFFECTATION_LECTURE     #1<tr>={...}
    EGAUX_LECTURE

    COMPATIBLES             #1[tr]:#2[tr]

    EGAUX                   #1[tr]::#2[tr]
    EGALITE                 #1:#2

    INFERIEUR               #1<#2
    SUPERIEUR               #1>#2

    INF_TRAITS              #1[tr]< ch | #2 | #2[tr]
    SUP_TRAITS              #1[tr]> ch | #2 | #2[tr]
    SUP_EGAL_TRAITS         #1[tr]<= ch | #2 | #2[tr]
    INF_EGAL_TRAITS         #1[tr]>= ch | #2 | #2[tr]

    AFFECTATION             #1[tr]={...}
    TEST                    #1[tr]
    */
    if (varnum1!=-1) {
        TestFonctionNoeud* tfnoeud=NULL;
        char attr[100];
        trait* vattribut;
        trait* vvaleur;


        //Il peut y avoir d'abord une negation
        if (k==NEGATION) {
            neg=OUI;
            k=lit(fichier,suite);
        }
#ifdef _DEBUG
		VECTA<int> lesetats;
#endif
        while (etat!=100) {
#ifdef _DEBUG
			lesetats.ajoute(etat);
#endif

            switch(etat) {//apres la lecture de la variable et de sa structure de traits (quand il y en a une)
            case 0: { //on peut avoir les cas suivants:
                switch (k) {
                case ':':
                    exacte=COMPATIBLES;
                    etat=2;
                    break;
                case '=':
                    exacte=AFFECTATION;
                    etat=3;
                    break;
                case '<':
                    if (trgauche==NULL)
                        exacte=INFERIEUR;
                    else
                        exacte=INF_TRAITS;
                    etat=1;
                    break;
                case '>':
                    if (trgauche==NULL)
                        exacte=SUPERIEUR;
                    else
                        exacte=SUP_TRAITS;
                    etat=1;
                    break;
                default: //ce n'est pas un signe de comparaison
                    etat=25;
                    etatsuivant=40;
                    continue;
                }
                k=lit(fichier,suite);
                break;
                    }
            case 1: //on vient de lire un '>' ou un '<'
                //On relit une nouvelle fois, ce ne peut etre qu'un autre ':'
                if (k=='=') {
                    if (exacte==SUP_TRAITS)
                        exacte=SUP_EGAL_TRAITS;
                    if (exacte==INF_TRAITS)
                        exacte=INF_EGAL_TRAITS;
                    k=lit(fichier,suite);
                    etat=4; //dans ce cas on ne peut avoir que #1[] <= #2[] ou #1[] <= ch
                }
                else {
                    if (trgauche==NULL)
                        etat=7; //dans ce cas on peut encore avoir #1<#2
                    else
                        etat=4; //dans ce cas c'est une comparaison #1[] < chaine ou #1[]<#2[]
                }
                break;
            case 2:      //c'est une comparaison de type "::"
                if (trgauche!=NULL && trgauche->onto!=NULL)
                    exacte=COMPATIBLES_ONTOLOGIE;
                if (k==':') {
                    exacte=EGAUX;
                    if (trgauche!=NULL && trgauche->onto!=NULL)
                        exacte=EGAUX_ONTOLOGIE;
                    k=lit(fichier,suite);
                }
                if (trgauche==NULL && trlexical==NULL)
					etat=7; //dans ce cas il s'agit d'une comparaison de type: #1:#2
                else
                    etat=6; //sinon ce qui suit est forcement de type #1[]::#2[]
                break;
            case 3: //C'est une affectation
                //On vient de lire le signe d'affectation
                if (trgauche!=NULL && trgauche->onto!=NULL)
                    exacte=AFFECTATION_ONTOLOGIE;

                if (k=='{') {
					k=lit(fichier,suite);
					etat=20;
				}
				else
					etat=22;
                break;
            case 4: //il s'agit de la lecture qui suit l'operateur <,>,<=,>=
                //c'est une variable, on la traite
                if (k=='#') {
                    etat=6;
                    break;
                }
                //On recupere le nom de l'attribut...
                trgauche->present->ChaineAttributs(this,attr);
                //Attention il ne peut y avoir qu'un seul trait
                if (strchr(attr,',') || attr[0]==0)
                    erreur(CHAINES[219]);
                vattribut=ArbTraits->recherche(attr);
                vvaleur=RetrouveValeurTrait(vattribut,suite,OUI);
				trgauche->inittrait(vattribut,vvaleur);
                //(trgauche->valeur)->ou(vvaleur->valeur);
                //(trgauche->filtre)->ou(vattribut->valeur);
                etat=25; //c'est la fin
                etatsuivant=40;
                break;
            case 6:
                if (k!='#')
                    erreur(CHAINES[129]);
                k=lit(fichier,suite);
                etat=9;
                break;
            case 7: //il ne peut y avoir de traits
                if (k!='#')
                    erreur(CHAINES[129]);
                k=lit(fichier,suite);
                etat=10;
                break;
            case 9://on determine le numero de la variable
                if (suite[0] >= '0' && suite[0] <= '9')
                    varnum2=atoi(suite);
                else {
                    sprintf_s(suite,1000,"%s?",CHAINES[184]);
                    erreur(suite);
                }
                if (trgauche==NULL)
					if (trlexical!=NULL)
						trgauche=trlexical;
                TestTraitVide(trgauche);
                etat=11;
                break;
            case 10: //lecture sans traits
                if (suite[0] >= '0' && suite[0] <= '9')
                    varnum2=atoi(suite);
                else {
                    sprintf_s(suite,1000,"%s?",CHAINES[184]);
                    erreur(suite);
                }
                etat=25;
                etatsuivant=50;
                break;
            case 11://on doit lire une structure de traits
                if (k=='[' || k=='<') {
                    //On ne peut comparer que des structures semblables
                    if (k=='<' && surlecture!=NON)
                        trdroit=BatitTraits(fichier,'>');
                    else
                        trdroit=BatitTraits(fichier);
                    TestTraitVide(trdroit);
                    k=lit(fichier,suite);
                }
                etat=25;
                etatsuivant=60; //les deux structures de traits existent
                break;
            case 20://on lit la variable
                if (k!='#')
                    erreur(CHAINES[129]);

                k=lit(fichier,suite);
                //on lit le numero de la variable
                nv=atoi(suite);
                affectations.ajoute(nv);
                etat=21;
                break;
            case 21: //on lit l'operateur
                switch(k) {
			    case '<':
					trvar=BatitTraits(fichier,'>');
					traits_variables.affecte(affectations.dernier-1,FactoriseTraits(trvar));
					k=lit(fichier,suite);
					etat=21;
					break;
                case '&':
                case '|':
                    affectations.ajoute(k);
                    etat=20;
                    k=lit(fichier,suite);
                    break;
                case '}': //fin de la lecture
                    etat=25;
                    etatsuivant=30;
                    k=lit(fichier,suite);
                    break;
                default:
					sprintf_s(suite,1000,"%s:%c",CHAINES[77],k);
                    erreur(suite);
                }
                break;
            case 22://on lit la variable
                if (k!='#')
                    erreur(CHAINES[129]);

                k=lit(fichier,suite);
                //on lit le numero de la variable
                nv=atoi(suite);
                affectations.ajoute(nv);
                etat=25;
				etatsuivant=30;
                break;
			case 25: //creation de l'objet tfnoeud avec tous les parametres
				tfnoeud=new TestFonctionNoeud;
				tfnoeud->negation=neg;
				tf=tfnoeud;

				if (surlecture!=NON) {
					switch(exacte) {
					case COMPATIBLES:
						exacte=COMPATIBLES_LECTURE;
						break;
					case EGAUX:
						exacte=EGAUX_LECTURE;
						break;
					case AFFECTATION:
						exacte=AFFECTATION_LECTURE;
					}
				}

				tfnoeud->arggauche=varnum1;
				tfnoeud->selection=FactoriseTraits(trlexical);
				tfnoeud->selectiontous=FactoriseTraits(trlexicaltous);
				tfnoeud->comparateur=exacte;
				StockeTestFonction(tf);
				etat=etatsuivant;
				break;
			case 30: //creation de l'affectation
				if (trgauche==NULL) {
					if (trlexical!=NULL)
						modiftrait=trlexical;
					else
						if (trlexicaltous!=NULL)
							modiftrait=trlexicaltous;
						else
							if (affectations.dernier==0)
								erreur(CHAINES[125]);
				}
				else
					modiftrait=trgauche;

				if (modiftrait!=NULL) {
					//On fait subir une transfo a modiftrait
					if (modiftrait->lpresent!=NULL) {
						modiftrait->valeur->ou(modiftrait->lpresent);
						modiftrait->filtre->ou(modiftrait->lpresent);
						delete modiftrait->lpresent;
						modiftrait->lpresent=NULL;
					}
					else {
						if (modiftrait->present!=NULL) {
							modiftrait->valeur->ou(modiftrait->present);
							modiftrait->filtre->ou(modiftrait->present);
							delete modiftrait->present;
							modiftrait->present=NULL;
						}
					}
				}

				pair=OUI;
				for (i=0;i<affectations.dernier;i++) {
					if (pair==NON) {
						if (affectations[i]=='|')
							tfnoeud->arg.ajoute(OP_OU);
						else
							tfnoeud->arg.ajoute(OP_ET);
						pair=OUI;
					}
					else {
						tfnoeud->arg.ajoute(affectations[i]);
						tfnoeud->arg_traits.affecte(i,traits_variables[i]);
						pair=NON;
					}
				}
			case 40://c'est un simple test sur les traits
				if (trgauche==NULL &&
					trlexical==NULL &&
					trlexicaltous==NULL &&
					lectxml==NON)
					erreur(CHAINES[126]);
				if (trgauche!=NULL)
					tfnoeud->traits.ajoute(FactoriseTraits(trgauche));
				etat=100;
				break;
			case 50://c'est une comparaison brute sans traits entre deux noeuds
				tfnoeud->argdroit=varnum2;
				if (exacte<=EGAUX)
					tfnoeud->comparateur=EGALITE;
				etat=100;
				break;
			case 60://c'est une comparaison qui portent sur les traits entre deux noeuds
				if (exacte==EGAUX_ONTOLOGIE) {
					transformedescendants(this, *hierarchies,trgauche->onto->cell(ONTOLOGIE_COMPARAISON));
					transformedescendants(this, *hierarchies,trdroit->onto->cell(ONTOLOGIE_COMPARAISON));
				}

				tfnoeud->traits.ajoute(FactoriseTraits(trgauche));
				if (trdroit!=NULL)
					tfnoeud->traits.ajoute(FactoriseTraits(trdroit));
				tfnoeud->argdroit=varnum2;
				etat=100;
				break;
			}
	  }

	  return tf;
	}

    //--------------------------------------------------------------------------------------
    //Traitement des dependances
    //On regarde son type. "?" correspond a une dependance quelconque ce qui correspond a
    //la position 0 dans la table des dependances.
    if (tampon[0]=='?' || (tampon[0]==0 && vardep!=-1)) {
        c=CatVide;
        //le ? correspond a la position 1 dans la table des variables...
        if (vardep==-1)
            vardep=1;
    }
	else {//On recherche la fonction
        c=ChercheFonc(tampon);
		if (NODE==OUI && c->X.type!=C_GENERATION)
			erreur(CHAINES[397]);
	}

	TestFonctionKif* tfkif=NULL;
	char kifmode=NON;

    TestFonctionDependance* tfdep=NULL;
    TestFonctionAffichage* taff=NULL;

#ifdef XIPPYTHON
    TestFonctionPython* tfpython=NULL;

    if (c->X.type==C_PROCEDUREPYTHON) {
        for (int yp=0;yp<PythonScript.dernier;yp++) {
            if (PythonScript[yp]->depend->Fx==&c->X) {
                tfpython=new TestFonctionPython(&c->X,PythonScript[yp]);
                tf=tfpython;
				break;
            }
        }
        if (tf==NULL)
            erreur(CHAINES[360]);
    }
    else
#else
        TestFonctionDependance* tfpython=NULL;
#endif

        if (c==Affichage) {
            taff=new TestFonctionAffichage;
            tf=taff;
        }
		else {
			if (c->X.type==C_KIF) {
				KifElement* kfunc=ChercheDansKif(kifloader,c->X.nom);
				tfkif=new TestFonctionKif(&c->X,kfunc);
				tf=tfkif;
				kifmode=OUI;
			}
			else {
				tfdep=new TestFonctionDependance(&c->X);
				//Cas particulier, s'il s'agit d'un appel de procedure
				if (c->X.type==C_PROCEDURE ||
					c->X.type==C_PROCEDUREDECLAREE)
					tfdep->leType=TESTFONCTIONPROCEDURE;

				tf=tfdep;
				tfdep->stack.affecte(pile);
				tfdep->negation=neg;
				tfdep->clearStack=clrStk;
				tfdep->pointeur=pointeur;
			}
	}

    StockeTestFonction(tf);

    if (tfdep!=NULL) {
        tfdep->traitsFonction=FactoriseTraits(trgauche);
        tfdep->changement=cgt;
        tfdep->vardep=vardep;
    }

    i=0;
    char boucle=OUI;

    //si varnum1==-1, c'est la lecture d'une fonction syntaxique, mais si k!=')'
    //cela signifie qu'il s'agit d'une dependance sans arguments...
    if (taff!=NULL) {
        TVecteur aff=0;
        //On lit les criteres d'affichage, separe par une virgule
        k=lit(fichier,suite);
        while (boucle) {
            int mc=0;
            while (affichagemotclef[mc][0]!='%' &&
                compchmin((uchar*)affichagemotclef[mc],(uchar*)suite))
                mc++;
            if (affichagemotclef[mc][0]=='%') {
                sprintf(tampon,"%s: %s",CHAINES[270],suite);
                erreur(tampon);
            }
            //sinon, mc pointe sur la bonne valeur
            aff|=valeur_affichage[mc];

            if (k!= ',' && k!=carfin) {
                delete tf;
                erreur(CHAINES[133]);
            }

            if (k==carfin)
                boucle=NON;
            else
                k=lit(fichier,suite);
        }

        k=lit(fichier,tampon);
        taff->affiche=aff;
    }
    else {
        if (varnum1==-1 && k=='(') {

            //lecture des arguments de la fonction
            k=lit(fichier,suite);
            while (boucle) {
                int typeVariable=-1;
                char liretraits=NON;
                //La lecture des arguments est de la forme: fonc(#1,?)
                char typeargetat=0;
                if (k=='#' && suite[0]==0)
                    typeargetat=1;
                else {
					if (k=='#' && suite[0]=='$') {
						typeargetat=4;
						k=lit(fichier,suite+1);
					}
					else
						if (suite[0]=='$')
							typeargetat=4;
						else
							if (suite[0]=='?')
								typeargetat=2;
							else
								if (tfdep==NULL || tfdep->leType==TESTFONCTIONPROCEDURE)
									typeargetat=3;
				}
                TestFormule* tformule;
                switch(typeargetat) {
                case 1://lecture d'une variable #num
                    k=lit(fichier,suite);
                    liretraits=OUI;
                    //Une variable
					typeVariable=atoi(suite);
					if (tfkif!=NULL) {
						tfkif->arg.affecte(i,typeVariable);
						if (neg==NON)
							variables_utilisees.affecte(tfkif->arg[i],OUI);
					}
					else {
						if (tfpython!=NULL) {
							tfpython->arg.affecte(i,typeVariable);
							if (neg==NON)
								variables_utilisees.affecte(tfpython->arg[i],OUI);
						}
						else {
							tfdep->arg.affecte(i,typeVariable);
							if (neg==NON)
								variables_utilisees.affecte(tfdep->arg[i],OUI);
						}
					}
                    break;
                case 2://lecture d'un parametre ?
					if (tfkif!=NULL)
						tfkif->arg.affecte(i,typeVariable);
					else {
						if (tfpython!=NULL)
							tfpython->arg.affecte(i,typeVariable);
						else
							tfdep->arg.affecte(i,typeVariable);
					}
                    liretraits=OUI;
                    break;
                case 3://lecture d'une formule mathematique
                    if (k!=')' || suite[0]!=0) {
						//cas particulier, il s'agit d'une chaine vide...
						if (k==',' && LuEntreCotes==OUI) {
							tformule=new TestFormuleString;
							tformule->element=new TestFormuleElementValString((uchar*)suite);
						}
						else {
							if (LuEntreCotes) {
								//On reinjecte alors les doubles quotes, pour eviter que certains traitements
								//ne modifie la chaine de caractere (en particulier sa mise en minuscule)
								nv = decaler(suite, 1);
								suite[nv] = '"';
								suite[nv + 1] = 0;
								suite[0] = '"';
							}
							MetEnAttenteLecture(k,suite);
							tformule=LireUneFormule(fichier,k,variables_utilisees,OUI,kifmode);
						}
						if (tfkif!=NULL)
							tfkif->formules.ajoute(tformule);
						else {
							if (tfpython!=NULL)
								tfpython->formules.ajoute(tformule);
							else
								tfdep->formules.ajoute(tformule);
						}
                        if (k==';')
                            k=',';
                    }
                    break;
                case 4://Lecture d'une variable de dependance: $num
                    typeVariable=(atoi(suite+1)+1)*-1;
					if (tfkif!=NULL) {
						tfkif->arg.affecte(i,typeVariable);
						if (neg==NON)
							variables_utilisees.affecte(tfkif->arg[i],OUI);
					}
					else {
						if (tfpython!=NULL) {
							tfpython->arg.affecte(i,typeVariable);
							if (neg==NON)
								variables_utilisees.affecte(tfpython->arg[i],OUI);
						}
						else
							tfdep->arg.affecte(i,typeVariable);
					}
                    break;
                }

                //Lecture des traits associes aux arguments...
                if (liretraits && k=='[' && tfdep!=NULL) {
                    trgauche=BatitTraits(fichier);
                    tfdep->traits.affecte(i,FactoriseTraits(trgauche));
                    k=lit(fichier,suite);
                }

                if (k!= ',' && k!=carfin) {
                    delete tf;
                    erreur(CHAINES[133]);
                }

                i++;

                if (k==carfin)
                    boucle=NON;
                else
                    k=lit(fichier,suite);
            }

			//ICI, dans le cas d'un NODE, on remplace la categorie XCatVide par une autre
			if (NODE==OUI && tfdep->Fx==XCatVide)
				tfdep->typecat=C_GENERATION;

            k=lit(fichier,tampon);
        }
    }
    return tf;
}


void GlobalParseur::StockeTestFonction(TestFonction* tf) {

    if (memoiresem.indicateur==NON) {
        tf->index=ListeDesTestFonctions.dernier;
        ListeDesTestFonctions.ajoute(tf);
    }
    else {
        tf->index=memoiresem.nouveauxTests.dernier;
        memoiresem.nouveauxTests.ajoute(tf);
    }
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//Lecture d'une structure enchassee dans un "where"
static int aj=0;
static string pourmessage;
void GlobalParseur::LectureTest(istream& fichier,
                                char& k,
                                char* tampon,
                                char negation,
                                VECTA<char>& variables_utilisees,
                                TestArbre** tarbre,
                                char carfin,
                                char profondeur) {

    TestFonction* tf=NULL;
    TestArbre* alocale=*tarbre;
    TestArbre* blocale;

	Categorie* tempcat=NULL;
    char deterministe=NON;
    char opcourrant=NON;
    char neg=negation;
    int u;
	int i;
	int iu;
	char nodedep;
    uchar K;
    Categorie* c;
    char oper[]={k,0,0};
    char* ptdol;
    int operation;
    etat* e=NULL;
    TestFormule* tform;
    TestFormule* tfo;
	TestFonctionNoeud* tfcreation;
	x_node* x_mynode;
    int state=0;
	bnf_arbre* bnfarbre=NULL;
	x_lirechaine* xr;

#ifdef XIPLIBXML
    clefXML* clexml;
    TestFonctionXML* tfxml;
#endif
#ifdef _DEBUG
	VECTA<int> states;
#endif

//Test

	//if (enAttente[0]==0) {
	//	long ps=fichier.tellg();

	//	k=LectCrypte(fichier);
	//	K=k;LectureBlancs(fichier,K,'/');k=K;
	//	long pss=fichier.tellg();
	//	pss-=1;
	//	fichier.seekg(pss,ios::beg);

	//	x_read=new x_readxipstream(&fichier,this,pss);
	//	x_mynode=x_elementif(x_read,PARTIAL);
	//	delete x_read;
	//	if (x_mynode!=NULL)
	//		delete x_mynode;
	//	else {
	//		fichier.seekg(ps,ios::beg);
	//		fichier.read(tampon,20);
	//		tampon[20]=0;
	//		cout<<"->"<<tampon<<endl;
	//	}
	//
	//	fichier.seekg(ps,ios::beg);
	//}



    while (state!=-1) {
#ifdef _DEBUG
		states.ajoute(state);
#endif
        switch (state) {
        case 0:
            k=LectCrypte(fichier);
            K=k;LectureBlancs(fichier,K,'/');k=K;

            switch(k) {
            case '~':
                neg=1-negation;
                break;
            case '^':
            case '#':
                state=5;
                break;
            case '$':
                state=2;
                break;
            case '>':
                state=14;
                k=LectCrypte(fichier);
                oper[0]='>';
                break;
            case '<':
                state=14;
                k=LectCrypte(fichier);
                oper[0]='<';
                break;
            case '=':
            case ':':
                oper[0]=k;
                oper[1]=0;
                state=15;
                break;
            case '@':
                state=25;
                break;
            case '(':
                state=30;
                break;
            case '|':
                state=35;
                break;
            case '&':
                state=40;
                break;
			case '{':
				//dans ce cas, il nous faut lire la chaine entiere
				xr=new x_lirechaine(ExtraireGroupe(this,fichier));
				xr->loadtoken();
				bnfarbre=new bnf_arbre;
				x_mynode=bnfarbre->x_parsing(xr,PARTIAL);
				delete bnfarbre;
				if (x_mynode==NULL || alocale->arbre.fin()==NULL) {
					pourmessage="Wrong generation NODE structure: ";
					pourmessage+=xr->parcours;
					delete xr;
					erreur(STR(pourmessage));
				}
				delete xr;
				tf=construitarbregenere(alocale->arbre.retire()->test,x_mynode,variables_utilisees,neg);
				delete x_mynode;
				state=100;
				break;
			case '"':
				state=70;
				tampon[0]=0;
				break;
            default:
                if (k==carfin) {
                    //s'il ne s'agit pas d'un appel recursif, on lit la suite
                    if (alocale->operateur!='@' &&
                        alocale->arbre.dernier==1)
                        erreur(CHAINES[325]);
                    if (profondeur==NON)
                        k=lit(fichier,tampon);
                    //La nouvelle racine de notre retour est dans alocale
                    *tarbre=alocale;
                    return;
                }
                state=45;
            }
            break;
        case 2://lecture d'une variable $num
            tampon[0]='$';
            u=1;
            k=LectCrypte(fichier);
            //Double $$
            if (k=='$') {
                tampon[u++]='$';
                k=LectCrypte(fichier);
				//$$#
				if (k=='#') {
					tampon[u++]='#';
					k=LectCrypte(fichier);
				}
			}
            while (isdigit(k)) {
                tampon[u++]=k;
                k=LectCrypte(fichier);
            }
            tampon[u]=0;
        case 5: //Lecture d'une variable commencant par #num ou $num
            tf=LectureArgumentTest(fichier,variables_utilisees,carfin,tampon,k,neg);
			if (tempcat!=NULL && tf->Type()==TESTFONCTIONNOEUD) {
				tfcreation=(TestFonctionNoeud*)tf;
				tfcreation->comparateur=CREATION_NOEUD;
				tfcreation->X=&tempcat->X;
			}
			tempcat=NULL;
            tampon[0]=0;
            MetEnAttenteLecture(k,tampon);
            state=100; //ajout d'un argument
            break;
        case 14: //Il faut que k=='=' (<=, >=)
            oper[1]=0;
            if (k=='=')
                oper[1]='=';
            else {
                tampon[0]=0;
                MetEnAttenteLecture(k,tampon);
            }
        case 15://Lecture d'un operateur mathematique, ce qui suit doit etre une structure mathematique
            operation= rechercheNomFonction(oper,TableOperations);
            if (operation==-1) {
                sprintf(tampon,"Unknown operator: %s",oper);
                erreur(tampon);
            }
            tform=LireUneFormule(fichier,k,variables_utilisees,OUI);
            //alocale pointe sur le dernier element ajoute
            tf=alocale->arbre.fin()->test;
            //Qui doit etre de type formule mathematique
            if (tf->Type()!=TESTFONCTIONMATH &&
                tf->Type()!=TESTFONCTIONMATHCHAINE &&
				tf->Type()!=TESTFONCTIONGRAPHE)
                erreur(CHAINES[325]);

            tfo=new TestFormule;
            tfo->element=new TestFormuleElementOperation(operation);
            tfo->formules.ajoute(((TestFonctionMath*)tf)->formule);
            tfo->formules.ajoute(tform);
            tf->Formule(tfo);

            tf->negation=neg;
            neg=negation;
            if (carfin==';')
                tf->negation=SAUTE;
            tampon[0]=0;
            MetEnAttenteLecture(k,tampon);
            state=0;
            break;
        case 25://Lecture d'une expression XML ou d'une variable
            //On lit le reste de la chaine
            tampon[0]=k;
            u=1;
            k=LectCrypte(fichier);
            while (Lettre(k)) {
                tampon[u++]=k;
                k=LectCrypte(fichier);
            }
            tampon[u]=0;
            minuscule((uchar*)tampon);
            //On verifie s'il s'agit d'une variable numerique
            state=70;
            if (rechercheNomFonction(tampon,TableOperations)==-1) {
                //Il s'agit probablement d'un appel XML
                //dans ce cas, on replace dans le courant de lecture
                //les chaines que l'on vient de lire
                state=27;
				for (iu=0;iu<u;iu++)
					tampon[iu]=tampon[iu+1];
                MetEnAttenteLecture(k,tampon);
                k='@';
                tampon[0]=0;
            }
            break;
#ifdef XIPLIBXML
        case 27://Lecture d'une structure XML
            clexml=ParseXPathExpression(fichier,k,tampon,neg,variables_utilisees);
            tfxml=new TestFonctionXML;
            tfxml->negation=neg;
            tfxml->clefxml=clexml;
            tf=tfxml;
            StockeTestFonction(tfxml);
            if (carfin==';')
                tf->negation=SAUTE;
            state=100;
            break;
#else
        case 27:
            erreur(CHAINES[326]);
            break;
#endif
        case 30://Appel recursif
            //On cree la branche locale de cet appel recursif
            blocale=new TestArbre;
            deterministe=NON;
            //On l'appelle
            LectureTest(fichier,k,tampon,neg,variables_utilisees,&blocale,')',OUI);
            neg=negation;
            //Si cet parenthese est en tete de structure, elle devient la structure de base
            if (alocale->operateur=='@') {
                delete alocale;
                alocale=blocale;
                *tarbre=blocale;
            }
            else {//si nous avons recupere un gros paquet d'element, on l'insere d'un coup
                if (blocale->arbre.dernier > 1)
                    alocale->arbre.ajoute(blocale);
                else {//dans le cas ou les parantheses ne contiennent qu'un seul element
                    alocale->arbre.ajoute(blocale->arbre.retire());
                    delete blocale;
                }
            }
            state=0;
            break;
        case 35://On verifie la dupliquation du '|'
            deterministe=NON;
            k=LectCrypte(fichier);
            if (k=='|')
                deterministe=OUI;
            else {
                tampon[0]=0;
                MetEnAttenteLecture(k,tampon);
            }
            opcourrant='|';
            state=50;
            break;
        case 40://On verifie la dupliquation du '&'
            deterministe=NON;
            k=LectCrypte(fichier);
            if (k=='&')
                deterministe=OUI;
            else {
                tampon[0]=0;
                MetEnAttenteLecture(k,tampon);
            }
            opcourrant='&';
            state=50;
            break;
        case 45: //lecture d'une chaine de caractere pouvant etre une formule mathematique
            tampon[0]=k;
			aj++;
            u=1;
			iu=-1;
            state=55;
            k=LectCrypte(fichier);
			nodedep=NON;
			tempcat=NULL;
            while (Lettre(k) || k=='#' || k=='$') {
                tampon[u++]=k;
				if (k=='$')
					nodedep=OUI;
				if (k=='#' && nodedep==NON) {
					iu=u-1;
                    state=70; //il s'agit d'une formule
				}
				k=LectCrypte(fichier);
            }
            tampon[u]=0;
			minuscule((uchar*)tampon);
			//Si on a trouve une categorie suivie d'un # et d'un chiffre
			if (state==70 && isdigit(tampon[u-1]) && k=='[') {
				tampon[iu]=0;
				tempcat=ChercheCat(tampon);
				if (tempcat!=NULL) {
					state=5;
					tampon[iu]='#';
					MetEnAttenteLecture(k,tampon+iu+1);
					k='#';
					tampon[0]=0;
					break;
				}
			}
            K=k;LectureBlancs(fichier,K,NON);k=K;
            break;
        case 50://test sur la negation de l'operateur
            if (negation==OUI) {
                if (opcourrant=='&')
                    opcourrant='|';
                else
                    opcourrant='&';
            }
            //On intrroduit alors un niveau supplementaire dans l'arbre, dans le cas d'un
            //changement d'operateur
            if (opcourrant!=alocale->operateur || deterministe!=alocale->deterministe) {
                //on cree un nouveau sommet, qui devient un sommet avec le meme operateur
                TestArbre* narbre=new TestArbre;
                //On lui donne cet operateur courrant
                narbre->operateur=opcourrant;
                //Puis on lui attache alocale comme premiere tete
                narbre->arbre.ajoute(alocale);
                //alocale devient notre nouvelle tete
                alocale=narbre;
				alocale->deterministe=deterministe;
            }
            state=0;
            break;
        case 55://Verification du type d'objet a lire
            state=70;
			if (k=='[') {
                e=VariablesNoms.recherche(tampon);
				if (e==NULL || e->valeur==NULL) {
					if (nom_procedure!=NULL) {
						i=strlen(tampon);
						strcat(tampon,"::");
						strcat(tampon,nom_procedure);
						e=VariablesNoms.recherche(tampon);
						tampon[i]=0;
					}
				}
			}
            //Il s'agit dans ce cas d'une affectation de traits a une dependance
			if (k=='[' && (e==NULL || e->valeur==NULL))
                state=60;
            else {
                ptdol=strchr(tampon,'$');
                if (ptdol!=NULL)
                    *ptdol=0;
                //C'est une dependance
                if (k=='(' && rechercheNomFonction(tampon,TableOperations)==-1) {
                    //est-ce une procedure
                    c=ChercheFonc(tampon);
                    if (c->X.type!=C_PROCEDURE &&
						c->X.type!=C_KIF &&
                        c->X.type!=C_PROCEDUREDECLAREE &&
                        c->X.type!=C_PROCEDUREPYTHON)
                        state=60;
                }
                if (ptdol!=NULL)
                    *ptdol='$';
            }
            break;
        case 60://Lecture standard
            //cas particulier, lorsqu'on lit une procedure
            if (carfin=='.' || carfin==';')
                tf=LectureArgumentTest(fichier,variables_utilisees,')',tampon,k,neg);
            else
                tf=LectureArgumentTest(fichier,variables_utilisees,carfin,tampon,k,neg);
            tf->negation=neg;
            if (carfin==';')
                tf->negation=SAUTE;
            MetEnAttenteLecture(k,tampon);
            state=100;
            break;
        case 70://Lecture d'une formule mathematique
            MetEnAttenteLecture(k,tampon);
            tampon[0]=0;
            tform=LireUneFormule(fichier,k,variables_utilisees,OUI);
			if (tform->TypeGraphe())
				tf=new TestFonctionGraphe;
			else {
				if (tform->TypeString())
					tf=new TestFonctionMathString;
				else {
					if (tform->TypeBoucle())
						tf=new TestFonctionMathBoucle;
					else
						tf=new TestFonctionMath;
				}
			}

            StockeTestFonction(tf);
            tf->Formule(tform);

            tf->negation=neg;

            if (carfin==';')
                tf->negation=SAUTE;
            MetEnAttenteLecture(k,tampon);
            state=100;
            break;
        case 100://cas global, on rajoute notre tf
            if (alocale->operateur=='@' &&
                alocale->arbre.dernier>=1)
                erreur(CHAINES[327]);
            alocale->arbre.ajoute(new TestArbre(tf));
            tf=NULL;
            neg=negation;
            deterministe=NON;
            opcourrant=0;
            state=0;
            break;
         }
    }
}



static char estChiffre(unsigned char* forme) {
    int i=0;
    if (forme==NULL)
        return NON;

    while (forme[i]) {
        if (forme[i]<'0' || forme[i]>'9')
            if (forme[i]!='.' && forme[i]!='-' && forme[i]!='+')
                return NON;
            i++;
    }

    //renvoie 2 s'il s'agit d'un nombre a virgule
    return OUI;
}

TestFormule::TestFormule() {
	RAMASSE(TestFormule);
    element=NULL;
}

TestFormule::~TestFormule() {
	JETTE(TestFormule);
    if (element!=NULL)
        delete element;
//    for (int i=0;i<formules.dernier;i++)
//        delete formules[i];
}


void TestFormule::print(char premier) {
    TestFormuleElementOperation* op;
    TestFormuleElementReel* re;
    TestFormuleElementGlobal* gl;
    TestFormuleElementLemme* ch;
    TestFormuleElementDependance* dep;
    TestFormuleElementVariable* no;
	TestFormuleElementChaineNoeud* noch;
    int i;

    switch (element->type) {
    case VOPERATEUR:
        op=(TestFormuleElementOperation*)element;
        cout<<" "<<TableOperations[op->operation]<<"(";
        for (i=0;i<formules.dernier;i++)
            formules[i]->print(NON);
        cout<<")";
        break;
    case VALREEL:
        re= (TestFormuleElementReel*)element;
        cout<<" "<<re->var;
        break;
    case VNOEUD:
        no=(TestFormuleElementVariable*)element;
        cout<<" "<<no->var->nom;
        break;
    case VCHAINENOEUD:
        noch=(TestFormuleElementChaineNoeud*)element;
        cout<<" "<<noch->var->nom;
        break;
    case VDEPENDANCE:
        dep=(TestFormuleElementDependance*)element;
        cout<<" "<<dep->var->nom;
        break;
    case VCHAINEDEPENDANCE:
        dep=(TestFormuleElementDependance*)element;
        cout<<" "<<dep->var->nom;
        break;
	case VLEMMECHAINEINDEX:
    case VLEMMEINDEX:
        ch=(TestFormuleElementLemme*)element;
        cout<<" "<<ch->var->nom;
        break;
    case VGLOBALE:
        gl=(TestFormuleElementGlobal*)element;
        cout<<" "<<gl->var->nom;
        break;
    }
    if (premier==OUI)
        cout<<endl;
}


TestFormuleElementNoeud::~TestFormuleElementNoeud() {
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
int compteici=0;
TestFormule* GlobalParseur::LireUneFormule(istream& fichier,
                                           char& k,
                                           VECTA<char>& variables_utilisees,
                                           char testerreur,
										   char kifmode) {

    int operation=-1;
    char liresuite;
    char err[100];
    int indice;
    etat* e;
    etat* varprocglob;
    Variable* variab;
    char ajoutdansVPG;
    graphe* ungraphe=NULL;
    //Lecture particuliere ou le point decimal est recevable dans la lecture d'un element
    TestFormule* principal=NULL;
    TestFormule* soustf=NULL;
    TestFormule* racine=NULL;

    TestFormule* nouveau;
    char etat=0;
    char etatsuivant=0;
    int pos;
	char tampon[XIPBUFFERSIZE];
    TestFormuleString* sousstring;
    TestFormule* indice_chaine=NULL;
    TestFormule* indice_vecteur=NULL;
    TestFormule* partiegauche;
    TestFormule* partiedroite;
    char ope[]={0,0,0};
    int fonctionargument;
    sorteVariable lasorte;
    char rxp=NON;
    char rxplire=NON;

    char boucle=OUI;
    int nbdd=0;
    int nompos,id;
    static char tabstopchar[] = ",()&|!;+-*/^%[]=<>:~";

    char oper[2];
    oper[1]=0;
    char dollar=NON;
    TestFonction* tf;
    VECTA<TestFormule*>* vtform;
    uchar K;

    //cas particulier, il s'agit d'une expression reguliere que l'on doit lire
    if (testerreur==2) {
        rxplire=OUI;
        testerreur=OUI;
    }
#ifdef _DEBUG
	VECTA<int> lesetatspasses;
#endif
	tampon[0]=0;
    while (boucle) {
#ifdef _DEBUG
		lesetatspasses.ajoute(etat);
#endif
		switch(etat) {
        case 0:
            //On absorbe les blancs
            k=LectureMin(fichier);
            K=k;LectureBlancs(fichier,K,NON);k=K;
            switch(k) {
            case '(':
                //On part en recursion
                soustf=LireUneFormule(fichier,k,variables_utilisees,OUI);
                if (k!=')')
                    erreur(CHAINES[286]);
                k=LectureMin(fichier);
                etat=5;
                break;
            //lecture d'une variable
            case '$':
                pos=0;
                tampon[pos++]='$';
                k=LectureMin(fichier);
				if (k=='#') {
					tampon[pos++]='#';
					k=LectureMin(fichier);
				}
                etat=19;
                break;
            case '#':
                etat=11;
                break;
            case '\"':
                pos=0;
                k=LectCrypte(fichier);
                etat=13;
                break;
            case '+':
            case '-':
                //c'est un signe devant un chiffre
                etat=1;
                pos=0;
                tampon[pos++]=k;
                k=LectureMin(fichier);
                break;
            case '@':
                //Lecture d'une valeur par defaut
                etat=6;
                pos=0;
                tampon[pos++]='@';
                break;
            default:
                pos=0;
                if (k>='0' && k<='9') {
                    etat=1;
                    break;
                }
                //c'est une chaine, peut etre une fonction
                etat=2;
                tampon[pos++]=k;
            }
            break;
            case 12:
                while ((k>='0' && k<='9') || k=='.') {
                    tampon[pos++]=k;
                    k=LectureMin(fichier);
                }
                tampon[pos]=0;
                etat=11;
                break;
            case 19:
                while (k>='0' && k<='9') {
                    tampon[pos++]=k;
                    k=LectureMin(fichier);
                }
                tampon[pos]=0;
                etat=11;
                break;
            case 1:
                //On doit lire un chiffre
                while ((k>='0' && k<='9') || k=='.') {
                    tampon[pos++]=k;
                    k=LectureMin(fichier);
                }
                tampon[pos]=0;
				//dans le cas ou c'est en fait un separateur de champ...
				if (tampon[pos-1]==',') {
					tampon[pos-1]=0;
					err[0]=0;
					MetEnAttenteLecture(k,err);
					k=',';
				}
                soustf=new TestFormule;
                soustf->element = new TestFormuleElementReel(atof(tampon));
                //On sauve cet element
                etat=5;
                break;
            case 13:
                //On doit lire une chaine entre cote
                while (k!='\"') {
                    //dans le cas d'une expression reguliere, on saute
                    //l'interpretation du caractere d'echappement
                    if (k=='\\' && rxplire==NON) {
                        k=LectCrypte(fichier);
                        switch(k) {
                        case 'n':
                            tampon[pos++]='\n';
                            break;
                        case 't':
                            tampon[pos++]='\t';
                            break;
                        case 'r':
                            tampon[pos++]='\r';
                            break;
                        default:
                            tampon[pos++]=k;
                        }
                    }
                    else
                        tampon[pos++]=k;
                    k=LectCrypte(fichier);
                }
                tampon[pos]=0;
                k=LectureMin(fichier);
                if (rxplire==OUI)
                    etat=20;
                else
                    etat=7;
                break;
            case 7://enregistrement d'un chaine brute
                soustf=new TestFormuleString;
                soustf->element = new TestFormuleElementValString((uchar*)tampon);
                //On sauve cet element
                etat=5;
                break;
            case 20://Il s'agit de la creation d'un automate
                soustf=new TestFormule;
                soustf->element = new TestFormuleElementAutomate((uchar*)tampon);
                //On sauve cet element
                etat=5;
                break;
            case 6:
            case 2:
                //On lit une chaine de caractere
                //ce peut etre une variable (on doit compter le nombre de ##)
                //Les caracteres d'arret sont dans stopchar
                nbdd=0;
                nompos=-1;
                K=LectureMin(fichier);
                dollar=NON;
				while (K>32 && !strchr(tabstopchar, K) && pos < XIPBUFFERMAX) {
                    if (K=='#') {
                        nompos=pos;
                        nbdd++;
                    }

                    if (K=='$') {
                        dollar=OUI;
                        nompos=pos;
                        nbdd++;
                    }


                    tampon[pos++]=tolower(K);
                    K=LectureMin(fichier);
                }

				if (pos >= XIPBUFFERMAX) {
					sprintf_s(err, 100, "Line too long");
					erreur(err);
				}
				k=K;
                tampon[pos]=0;
                K=k;LectureBlancs(fichier,K,NON);k=K;

                if (etat==6)
                    etat=8;
                else
                    if (k=='(')
                        etat=4;
                    else
                        etat=3;
                break;
            case 3:
                if (LuEntreCotes==OUI) {
                    etat=7;
                    break;
                }

                //est-ce un operateur sans parametre)
                if (rechercheNomFonction(tampon,TableSansParametre)!=-1 && k==';') {
                    operation= rechercheNomFonction(tampon,TableOperations);
                    soustf=new TestFormule();
                    soustf->element=new TestFormuleElementOperation(operation);
                    etat=5;
                    break;
                }

                //est-ce une variable?
                if (nompos!=-1) {
                    indice=atoi(tampon+nompos+1);
                    tampon[nompos+1]=0;
                }

                e=NULL;
                varprocglob=NULL;
                ajoutdansVPG=NON;
                err[0]=0;
                //Si nous sommes dans une procedure, les variables locales priment sur les
                //variables globales. Ces variables sont construites avec le nom de la procedure
                //a la fin de leur nom precede par "::": k::PROCEDURE
                if (nom_procedure!=NULL) {
                    strcpy_s(err,100,tampon);
                    strcat_s(err,100,"::");
                    strcat_s(err,100,nom_procedure);
                    e=VariablesNoms.recherche(err);
                    if (e!=NULL && e->valeur!=NULL) {
                        variab=(Variable*)e->valeur;
                        if (variab->reinit==NON) {
                            //On garde la trace du soustf commun a tous
                            //ceux qui utiliserons cette variable globale de procedure
                            ajoutdansVPG=OUI;
                        }
                    }
                }

                if (e==NULL)
                    e=VariablesNoms.recherche(tampon);

                soustf=NULL;
                indice_chaine=NULL;
                indice_vecteur=NULL;
                liresuite=NON;

                if (e==NULL || e->valeur==NULL) {
					if (kifmode==1) {
						variab=new VariableKif(tampon,KIFOBJET);
						lasorte=VKIFOBJET;
					}
					else {
						if (testerreur==OUI) {
							sprintf_s(err,100,"Unknown variable: %s",tampon);
							erreur(err);
						}
						else
							return NULL;
					}
                }
                else {
                    variab=(Variable*)e->valeur;
                    lasorte=variab->sorte;
                }


				ungraphe=NULL;
				if (lasorte==VGRAPHE || lasorte==VVECTEURGRAPHE) {
					if (k=='<') {
						k=LectCrypte(fichier);
						if (k=='<') {
							//Il s'agit d'une mise-a-jour d'un graphe sous
							//la forme d'une structure XML
							//On lit donc jusqu'au "</g>;"
							ungraphe=litgraphe(fichier);
							k=';';
						}
						else
							erreur("The instanciation operator for graph is: <<");
					}
				}

                switch(lasorte) {
                case VGLOBALEREINIT:
                    soustf=new TestFormule;
                    soustf->element = new TestFormuleElementGlobal((VariableGlobale*)variab);
                    break;
                case VGRAPHE:
                    soustf=new TestFormuleGraphe;
                    soustf->element = new TestFormuleElementGraphe((VariableGraphe*)variab,ungraphe);
                    break;
                case VKIFOBJET:
                    soustf=new TestFormule;
                    soustf->element = new TestFormuleElementKif((VariableKif*)variab);
                    break;
#ifdef XIPPYTHON
                case VPYTHONOBJET:
                    soustf=new TestFormule;
                    soustf->element = new TestFormuleElementPython((VariablePython*)variab);
                    break;
#endif
#ifdef XIPLIBXML
                case VXMLOBJET:
                    soustf=new TestFormule;
                    soustf->element = new TestFormuleElementXML((VariableXML*)variab);
                    break;
#endif
                case VNOEUD:
                    soustf=new TestFormule;
                    soustf->element = new TestFormuleElementVariable((VariableIndexeeNoeud*)variab,indice);
                    break;
				case VCHAINENOEUD:
                    soustf=new TestFormuleString;
                    soustf->element = new TestFormuleElementChaineNoeud((VariableIndexeeChaine*)variab,indice);
                    break;
                case VDEPENDANCE:
                    soustf=new TestFormule;
                    soustf->element = new TestFormuleElementDependance((VariableIndexeeDependance*)variab,indice+1);
                    break;
                case VCHAINEDEPENDANCE:
                    soustf=new TestFormuleString;
                    soustf->element = new TestFormuleElementChaineDependance((VariableIndexeeChaineDependance*)variab,indice+1);
                    break;
                case VGLOBALE:
                    soustf=new TestFormule;
                    soustf->element = new TestFormuleElementGlobal((VariableGlobale*)variab);
                    break;
                case VLEMMEINDEX:
                    soustf=new TestFormule;
                    soustf->element = new TestFormuleElementLemme((VariableLemmeIndex*)variab,indice);
                    break;
                case VLEMMECHAINEINDEX:
                    soustf=new TestFormuleString;
                    soustf->element = new TestFormuleElementLemme((VariableLemmeChaineIndex*)variab,indice);
					soustf->element->type = VLEMMECHAINEINDEX;
                    break;
				case VVECTEURGRAPHE:
                case VVECTEURINT:
					if (k=='[') {
						indice_vecteur=LireUneFormule(fichier,k,variables_utilisees,OUI);
						if (k!=']')
							erreur(CHAINES[298]);
						k=LectureMin(fichier);

						if (variab->sorte==VVECTEURGRAPHE && ungraphe==NULL) {
							//On lit les blancs et on verifie la presence d'une description de graphe
							K=k;LectureBlancs(fichier,K,NON);k=K;
							//cas d'une instanciation directe dans une case du vecteur de graphes
							if (k=='<') {
								k=LectCrypte(fichier);
								if (k=='<') {
									//Il s'agit d'une mise-a-jour d'un graphe sous
									//la forme d'une structure XML
									//On lit donc jusqu'au "</g>;"
									ungraphe=litgraphe(fichier);
									k=';';
								}
								else
									erreur("The instanciation operator for graph is: <<");
							}
						}
					}
					if (variab->sorte==VVECTEURGRAPHE ) {
						soustf=new TestFormuleGraphe;
						soustf->element = new TestFormuleElementVectorGraphe((VariableVectorGraphe*)variab,indice_vecteur,ungraphe);
					}
					else {
						soustf=new TestFormule;
						soustf->element = new TestFormuleElementIntVector((VariableIntVector*)variab,indice_vecteur);
					}
					break;
                case VCHAINE:
                case VDICTIONNAIRE:
                case VVECTEURCHAINE:
                    sousstring=new TestFormuleString;
                    soustf=sousstring;
                    if (k=='[') {
                        indice_chaine=LireUneFormule(fichier,k,variables_utilisees,OUI);
                        if (k!=']')
                            erreur(CHAINES[298]);
                        k=LectureMin(fichier);
                    }

                    if (variab->sorte==VCHAINE)
                        sousstring->element = new TestFormuleElementString((VariableString*)variab,indice_chaine);
                    else {
                        indice_vecteur=indice_chaine;
                        indice_chaine=NULL;
                        if (k=='[') {
                            indice_chaine=LireUneFormule(fichier,k,variables_utilisees,OUI);
                            if (k!=']')
                                erreur(CHAINES[298]);
                            k=LectureMin(fichier);
                        }
						sousstring->element = new TestFormuleElementVectorString((VariableVector*)variab,indice_vecteur,indice_chaine);
						sousstring->element->type=lasorte;
                    }
                    break;
                }

                //On garde la trace de toutes les utilisations de cette variable globale
                if (ajoutdansVPG==OUI) {
                    varprocglob=VariablesProceduresGlobales.recherche(err);
                    if (varprocglob!=NULL)
                        vtform=(VECTA<TestFormule*>*)varprocglob->valeur;
                    else {
                        vtform=new VECTA<TestFormule*>;
                        VariablesProceduresGlobales.parsebrute((uchar*)err,(void*)vtform);
                    }
                    vtform->ajoute(soustf);
                }

                //Puis on lit la suite
                etat=5;
                break;
           case 8:
           case 4:
             //lecture d'une fonction
               operation= rechercheNomFonction(tampon,TableOperations);
               if (operation==-1 && tampon[0]) {
                   etat=18;
                   continue;
               }

               //s'il s'agit d'un "for", on passe a une autre sorte d'analyse
               if (rechercheNomFonction(tampon,TableBoucle)!=-1) {
                    etat=14;
                    break;
               }

               fonctionargument=rechercheNomFonction(tampon,TableOperationsNoeuds);

               //S'il s'agit d'une fonction de chaine
			   if (rechercheNomFonction(tampon,TableGraphes)!=-1)
				   soustf=new TestFormuleGraphe;
			   else
				   if (rechercheNomFonction(tampon,TableOperationChaine)!=-1)
					   soustf=new TestFormuleString;
				   else
					   soustf=new TestFormule;
               soustf->element=new TestFormuleElementOperation(operation);
               if (tampon[0]=='@') {//il s'agit d'une valeur particuliere @pi,@pathname
                   etat=5;
                   break;
               }
               //Cas particulier, s'il s'agit d'une regexp, le deuxieme parametre
               //est toujours une expression reguliere
               rxp=-1;
               if (rechercheNomFonction(tampon,TableExpressionReguliere)!=-1)
                   rxp=1;

               //On lit alors les arguments de la fonction separee par une virgule
               while (etat==4) {
                   if (rxp==0)
                       nouveau=LireUneFormule(fichier,k,variables_utilisees,2);
                   else
                       nouveau=LireUneFormule(fichier,k,variables_utilisees,OUI);
                   //On rajoute ce nouvel element
                   soustf->formules.ajoute(nouveau);
                   if (fonctionargument!=-1) {
                       id=nouveau->element->arg();
                       if (id!=-1)
                           variables_utilisees.affecte(id,OUI);
                   }

                   //On compte les parametres
                   rxp--;
                   if (k==')') {
                       etat=5;
                       k=LectureMin(fichier);
                       break;
                   }
                   if (k!=';' && k!=',')
                       erreur(CHAINES[321]);
               }
               break;
           case 14://lecture d'un for
               //On lit d'abord l'initialisation
               soustf=new TestFormuleBoucle(operation);
               soustf->element=new TestFormuleElementOperation(operation);
               if (k!='(')
                   erreur(CHAINES[30]);
               k=';';
               if (operation==30) {//s'il s'agit du for
                   etat=15;
                   etatsuivant=15;
               }
               else { //sinon c'est une boucle avec un seul test
                   etat=17;
                   etatsuivant=16;
               }
               break;
           case 15:
               if (soustf->formules.dernier==3) {
                   if (k!=')')
                       erreur(CHAINES[286]);
                   racine=soustf;
                   k=';';
                   boucle=NON;
               }
               else {
                   if (k!=';')
                       erreur(CHAINES[92]);
                   etat=17;
               }
               break;
           case 16:
               if (k!=')')
                   erreur(CHAINES[286]);
               racine=soustf;
               k=';';
               boucle=NON;
               break;
           case 17: //lecture du test du for
               partiegauche=LireUneFormule(fichier,k,variables_utilisees,OUI);
               ope[0]=k;
               k=LectureMin(fichier);
               if (ope[0]==NEGATION || ( (ope[0]=='>' || ope[0]=='<') && k=='='))
                   ope[1]=k;
               else {//s'il s'agit d'un vector, il faut lire son indice
                   tampon[0]=0;
                   MetEnAttenteLecture(k,tampon);
               }

               operation= rechercheNomFonction(ope,TableOperations);
               if (operation==-1) {
                   sprintf_s(tampon,1000,"Unknown operator: %s",ope);
                   erreur(tampon);
               }
               ope[1]=0;
               //On lit alors la partie droite
               partiedroite=LireUneFormule(fichier,k,variables_utilisees,OUI);

			   if (partiegauche->TypeGraphe())
				   nouveau=new TestFormuleGraphe;
			   else
				   if (partiegauche->TypeString())
					   nouveau=new TestFormuleString;
				   else
					   nouveau=new TestFormule;

               nouveau->element=new TestFormuleElementOperation(operation);
               nouveau->formules.ajoute(partiegauche);
               nouveau->formules.ajoute(partiedroite);
               soustf->formules.ajoute(nouveau);
               etat=etatsuivant;
               break;
           case 5:
               K=k;LectureBlancs(fichier,K,NON);k=K;
               //si ce qui suit est un operateur alors on doit ajouter nos elements
               //avec un nouvel operateur
               if (strchr(TabOperateurs,k)) {
                   etat=10;
                   break;
               }

               //s'il s'agit d'une fin, on s'arrete, et on rajoute notre element directement
               if (strchr(tabstopchar,k)) {
                   if (racine==NULL)
                       racine=soustf;
                   else
                       principal->formules.ajoute(soustf);
                   boucle=NON;
                   break;
               }
               //sinon c'est une erreur
                sprintf_s(err,100,"%s: %c",CHAINES[36],k);
                erreur(err);
           case 10:
               //Nous avons un nouvel operateur
               oper[0]=k;
               operation= rechercheNomFonction(oper,TableOperations);
               //si notre racine courante est deja du meme type, on ne cree rien de nouveau
               if (principal!=NULL) {
                   //a+b+
                   if (operation == principal->element->valeur_operation()) {
                       principal->formules.ajoute(soustf);
                       etat=0;
                       break;
                   }

                   //a+b-c+
                   if (racine!=principal && (k=='+' || k=='-')) {
                       if (operation == racine->element->valeur_operation()) {
                           principal->formules.ajoute(soustf);
                           principal=racine;
                           etat=0;
                           break;
                       }
                   }
               }

               //sinon on cree un nouvel element, dont le type est celui de son argument a gauche
			   if (soustf->TypeGraphe())
				   nouveau=new TestFormuleGraphe;
			   else
				   if (soustf->TypeString())
					   nouveau=new TestFormuleString;
				   else
					   nouveau=new TestFormule;

               nouveau->element= new TestFormuleElementOperation(operation);
               if (racine==NULL) {
                   racine=nouveau;
                   nouveau->formules.ajoute(soustf);
               }
               else {//La plusieurs cas selon le type de l'operateur sur la racine
               //Si le nouvel operateur est + ou - il devient la nouvelle racine
                   if (k=='+' || k=='-') {
                       principal->formules.ajoute(soustf);
                       nouveau->formules.ajoute(racine);
                       //On lui rajoute soustf
                       racine=nouveau;
                   }
                   else {//sinon, on le rajoute localement
                       principal->formules.ajoute(nouveau);
                       nouveau->formules.ajoute(soustf);
                   }
               }
               //Puis on rajoute soustf
               principal=nouveau;
               //Ce nouvel element devient desormais la tete de notre nouveau sous-arbre
               etat=0;
               break;
           case 11:
               //appel d'une variable de noeuds
               if (testerreur==2)
                   erreur(CHAINES[311]);
               tf=LectureArgumentTest(fichier,variables_utilisees,']',tampon,k,0,OUI);
               soustf=new TestFormuleString;
               soustf->element = new TestFormuleElementNoeud(tf);
               etat=5;
               break;
           case 18: //appel d'une procedure
               if (testerreur!=OUI && RetrouveFonc(tampon)==NULL)
                   return NULL;
               strcpy_s(err,100,tampon);
               tf=LectureArgumentTest(fichier,variables_utilisees,')',tampon,k,0,OUI);
               if (tf!=NULL) {
                   if (tf->Type()!=TESTFONCTIONPROCEDURE &&
					   tf->Type()!=TESTFONCTIONKIF &&
                       tf->Type()!=TESTFONCTIONPYTHON) {
                       sprintf_s(tampon,1000,"%s%s",CHAINES[307],err);
                       erreur(tampon);
                   }
               }
               soustf=new TestFormule;
               soustf->element = new TestFormuleElementNoeud(tf);
               etat=5;
               break;
            }
        }
    return racine;
}


//------------------------------------------------------------------------------
//Cette methode permet de recuperer la chaine de caractere d'un trait specifique
char GlobalParseur::RecuperationTraitValeur(TestFonction* tf,
											string& valeur,
                                            VECTA<Noeud*>& vectNoeuds,
                                            VECTA<ResultatFonction*>* vectFonc) {

    Traits* tr=NULL;
    Noeud* n=NULL;
    TestFonctionNoeud* test;
    Traits* trbase=NULL;
    ResultatFonction* rf;


    if (tf->Type()==TESTFONCTIONNOEUD) {
        test=(TestFonctionNoeud*)tf;
        tr=test->traits[0];
        n= vectNoeuds[test->arggauche];
        if (n==NULL)
            return NON;
        trbase=n->source->traitsbase;
    }
    else {
        //Sinon, il s'agit d'une dependance
        int ind=tf->IndexDependance();
        if (ind==-1 || vectFonc==NULL || (*vectFonc)[ind]==NULL)
            return NON;
        //On recupere notre fonction
        rf=(*vectFonc)[ind];
        tr=tf->LesTraits();
        trbase=rf->traits;
    }



    if (tr!=NULL) {
        //Plusieurs cas, il peut s'agir du lemme
        if (tr->lemme!=NULL)
            n->Lemme(valeur);
        else {//ou bien de la forme de surface
            if (tr->surface!=NULL)
                n->Surface(valeur);
            else {
				if (tr->onto!=NULL) {
					if (ITR(this,tr,trbase,NON,NULL)) {
						trbase->onto->cell(ONTOLOGIE_COMPARAISON)->ConcateneValeurs(this,hierarchies->lahierarchie,TraitsFiltreOntologie,valeur,",");
						if (valeur[0]==',')
							valeur=valeur.substr(1,valeur.size()-1);
					}
				}
                else {//enfin il peut s'agir d'un trait et un seul
                    STVecteur val;
                    char trouve=NON;
                    Vecteur* v;
                    if (tr->present!=NULL)
                        v=tr->present;
                    else
                        v=tr->lpresent;

                    valeur="";
                    for (int igd=0;igd<iTailleVect;igd++) {
                        val=trbase->valeur->vecteur[igd] & v->vecteur[igd];
                        if (val==0)
                            continue;
                        //On itere a l'envers
                        for (register int j=gBitVect-1;j>=0;j--) {
                            //Plus de bit a examiner on s'arrete
                            if (val==0)
                                break;
                            //le nombre est negatif, le jieme bit etait a un
							if (val < 0) {
								trouve=OUI;
								//nous calculons sa position absolue
								int pos= (igd*gBitVect + j)*2;
								if (tr->lpresent!=NULL) {
									if (valeur.size()!=0)
										valeur+=";";
									valeur+=TableDesTraits[pos];
									valeur+="_";
									valeur+=TableDesTraits[pos+1];
								}
								else {
									valeur=TableDesTraits[pos+1];
									return OUI;
								}
							}
                            //nous decalons tous les bits d'un cran vers la gauche, pour les
                            //amener les uns derriere les autres en position de bit de signe.
                            val<<=1;
                        }
                    }
                    return trouve;
                }
            }
        }
    }
    else {//il s'agit d'une comparaison de la categorie
        if (n!=NULL)
            valeur+=n->X->nom;
        else
            if (rf!=NULL)
                valeur+=rf->Fx->nom;
    }
    return OUI;
}

//------------------------------------------------------------------------
//Cette methode permet de retrouver une variable pour lui affecter une valeur

Variable* TestFormule::RenvoieVariable() {

    switch(element->type) {
    case VNOEUD:
        return element->valeur_variable()->var;
	case VCHAINENOEUD:
		return element->valeur_variable_chaine()->var;
    case VDEPENDANCE:
        return element->valeur_dependance()->var;
    case VCHAINEDEPENDANCE:
        return element->valeur_dependance()->var;
    case VGLOBALE:
        //Il s'agit d'une variable globale
        return element->valeur_global();
    case VLEMMEINDEX:
        //c'est une variable indexee sur un lemme
        return element->valeur_lemme();
    case VLEMMECHAINEINDEX:
        //c'est une variable indexee sur un lemme
        return element->valeur_lemme();
    case VCHAINE:
        //Il s'agit d'une variable globale
        return element->valeur_string();
    case VVECTEURINT:
        return ((TestFormuleElementIntVector*)element)->var;
    case VDICTIONNAIRE:
    case VVECTEURCHAINE:
        return ((TestFormuleElementVectorString*)element)->var;
	case VKIFOBJET:
		return ((TestFormuleElementKif*)element)->vp;
#ifdef XIPLIBXML
    case VXMLOBJET:
        return ((TestFormuleElementXML*)element)->vxml;
#endif
    }
    return NULL;
}


char TestFormule::AffecteVariable(Variable* v) {

    if (v->sorte!=element->type)
        return NON;

    return element->assigne(v);
}

void AffecteNombre(GlobalParseur* parseur,
                   TestFormule* formule,
                   double val,
                   VECTA<Noeud*>& noeuds,
                   VECTA<ResultatFonction*>* focus=NULL) {

    //Pour mettre a jour la variable globale
    TestFonctionMath* tfm=new TestFonctionMath;
    TestFormule* tfmv=new TestFormule;
    TestFormuleElementReel* tfge=new TestFormuleElementReel(0);
    TestFormule* tfom=new TestFormule;

    //Il s'agit d'une seule et unique initialisation.

	tfom->element=new TestFormuleElementOperation(0);
	tfm->formule=tfom;
	tfmv->element=tfge;

    char err=NON;
    tfom->formules.affecte(0,formule);
    tfge->var=val;
    tfom->formules.affecte(1,tfmv);
    tfm->TraitementFormule(parseur,noeuds,err,focus);
    tfom->formules.raz();
	delete tfm;
	delete tfmv;
}

void AffecteChaine(GlobalParseur* parseur,
                   TestFormule* formule,
                   string& val,
                   VECTA<Noeud*>& noeuds,
                   VECTA<ResultatFonction*>* focus=NULL) {
    //Pour mettre a jour la variable globale
    TestFonctionMathString tfs;
    TestFormuleString tfsv;
    TestFormuleElementValString* tfse=new TestFormuleElementValString((uchar*)"");
    TestFormule* tfos=new TestFormule;
    char err=NON;

    //Il s'agit d'une seule et unique initialisation.
	tfos->element=new TestFormuleElementOperation(0);
	tfs.formule=tfos;
	tfsv.element=tfse;

    tfos->formules.affecte(0,formule);
    libere(tfse->var);
    tfse->var=(uchar*)DupliqueChaine(val.c_str());
    tfos->formules.affecte(1,&tfsv);
    tfs.TraitementFormule(parseur,noeuds,err,focus);
    tfos->formules.raz();
}

char TestFormule::AffecteVariable(GlobalParseur* parseur,
                                  unsigned char* val,
                                  VECTA<Noeud*>& noeuds,
                                  VECTA<ResultatFonction*>* focus,
                                  double valres) {
    int ind,pos,ki;
    Noeud* n=NULL;
    VariableGlobale* varglob;
    string* s;
    VariableLemmeIndex* vlemme;
    VariableString* vstr;
    char erreur=NON;

    TestFormuleElementVariable* v;
	TestFormuleElementChaineNoeud* vchn;
    TestFormuleElementLemme* vch;
    TestFormuleElementDependance* dep;
	TestFormuleElementChaineDependance* depch;
    TestFormuleElementVectorString* vvect;
    TestFormuleElementIntVector* ivect;

    ResultatFonction* rf=NULL;
    string lemme;
    string buff;
    //On recupere notre variable
    v=NULL;
    dep=NULL;
    rf=NULL;
    ind=-1;
    varglob=NULL;
    double res;

    if (element->type !=VCHAINE) {
        if (val!=NULL)
            res=atof((char*)val);
        else
            res=valres;
    }

    switch(element->type) {
    case VNOEUD:
        v=element->valeur_variable();
        //l'indice correspond au noeud qui nous interesse:
        n = noeuds[v->indice];
        if (n==NULL)
            return NON;
        ind=v->var->AjouteNoeud(n);
        //Nous rangeons la valeur au bon endroit, soit n->source->position
        v->var->valeurs.affecte(ind,res);
        v->var->utilisee=OUI;
        //Et nous enregistrons le fait que ce noeud est associe a cette variable
        n->source->variables.affecte(v->var->position,v->var);
        break;
    case VCHAINENOEUD:
        vchn=element->valeur_variable_chaine();
        //l'indice correspond au noeud qui nous interesse:
        n = noeuds[vchn->indice];
        if (n==NULL)
            return NON;
        ind=vchn->var->AjouteNoeud(n);
        //Nous rangeons la valeur au bon endroit, soit n->source->position
        vchn->var->Affecte(ind,(char*)val);
        vchn->var->utilisee=OUI;
        //Et nous enregistrons le fait que ce noeud est associe a cette variable
        n->source->variables.affecte(vchn->var->position,vchn->var);
        break;
    case VDEPENDANCE:
        dep=element->valeur_dependance();
        rf=focus->cell(dep->indice);
        if (rf==NULL)
            return NON;
        ind=dep->var->AjouteDependance(rf);
        dep->var->valeurs.affecte(ind,res);
        dep->var->utilisee=OUI;
        rf->vardep.affecte(dep->var->position,dep->var);
        break;
    case VCHAINEDEPENDANCE:
        depch=element->valeur_dependance_chaine();
        rf=focus->cell(depch->indice);
        if (rf==NULL)
            return NON;
        ind=depch->var->AjouteDependance(rf);
        depch->var->Affecte(ind,(char*)val);
        rf->vardep.affecte(depch->var->position,depch->var);
        break;
    case VGLOBALEREINIT:
    case VGLOBALE:
        //Il s'agit d'une variable globale
        varglob=element->valeur_global();
        if (varglob==NULL)
            return NON;
        varglob->utilisee=OUI;
        varglob->valeur=res;
        break;
	case VLEMMECHAINEINDEX:
    case VLEMMEINDEX:
        //c'est une variable de chaine
        vlemme=element->valeur_lemme();
        vch = (TestFormuleElementLemme*)element;
        n = noeuds[vch->indice];
        if (n==NULL)
            return NON;

        //Il s'agit d'une concatenation du nom de la variable avec le lemme
        //sprintf((char*)lemme,"%s%s#",vlemme->nom,n->X->nom);
        lemme=vlemme->nom;
        pos=lemme.size();
        //On concatene les lemmes dans cette chaine
        n->Lemme(lemme);
        ki=pos;
        while (ki<lemme.size() && lemme[ki]==' ') ki++;
		if (ki>=lemme.size())
			return NON;

        varglob =vlemme->recherche(USTR(lemme)+ki,pos);
        //dans le cas de la creation d'une nouvelle variable, le
        //lemme a deja ete identifiee
        vlemme->utilisee=OUI;
		majvariablelemme(vlemme,n,USTR(lemme),pos,(char*)val,res);
        break;
    case VCHAINE:
        //Il s'agit d'une variable globale
        vstr=element->valeur_string();
        if (vstr==NULL)
            return NON;
        vstr->utilisee=OUI;
        vstr->chaine=(char*)val;
        break;
    case VVECTEURINT:
        ivect=(TestFormuleElementIntVector*)element;
        if (ivect->indice_vecteur==NULL)
            return NON;
        //On va chercher l'indice
        ind=(int)ivect->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;
        //On affecte notre valeur
        ivect->var->Affecte(ind,res);
        break;
    case VDICTIONNAIRE:
        vvect=(TestFormuleElementVectorString*)element;
        if (vvect->indice_vecteur==NULL)
            return NON;
        //On va chercher l'indice
        res=vvect->indice_vecteur->TraitementFormuleString(parseur,noeuds,erreur,buff,focus);
        if (erreur==OUI)
            return NON;
        //On affecte notre valeur
        s= new string((char*)val);
        vvect->var->AffecteValeurStringIndexChaine(s,buff);
        break;
    case VVECTEURCHAINE:
        vvect=(TestFormuleElementVectorString*)element;
        if (vvect->indice_vecteur==NULL)
            return NON;
        //On va chercher l'indice
        res=vvect->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;
        //On affecte notre valeur
        s= new string((char*)val);
        vvect->var->AffecteValeurStringIndex(s,(int)res);
        break;
    }
    return OUI;
}
//------------------------------------------------------------------------------------
char AffectationVariable(Variable* v,double res,string val) {

    VariableGlobale* varglob;
    VariableString* vstr;
    VariableVector* vvect;
    VariableIntVector* ivect;
    string* s;
    int i;

    switch(v->sorte) {
    case VNOEUD:
	case VCHAINENOEUD:
	case VLEMMEINDEX:
	case VLEMMECHAINEINDEX:
	case VCHAINEDEPENDANCE:
	case VDEPENDANCE:
		return NON;
	case VGLOBALEREINIT:
	case VGLOBALE:
		//Il s'agit d'une variable globale
		varglob=(VariableGlobale*)v;
		varglob->utilisee=OUI;
		varglob->valeur=res;
		break;
	case VCHAINE:
		//Il s'agit d'une variable globale
		vstr=(VariableString*)v;
		vstr->utilisee=OUI;
		vstr->chaine=val;
		break;
	case VVECTEURINT:
		ivect=(VariableIntVector*)v;
		if (val=="c") {
			ivect->raz();
			return OUI;
		}

		if (val=="a")
			i=ivect->valeurs.dernier;
		else {
			ivect->raz();
			i=0;
		}
		ivect->Affecte(i,res);
		break;
	case VVECTEURCHAINE:
		vvect=(VariableVector*)v;

		if (res==-100) {
			vvect->raz();
			return OUI;
		}

		//On affecte notre valeur
		s= new string(val);
		if (res==-10)
			i=vvect->chaine.dernier;
		else {
			vvect->raz();
			i=0;
		}
		vvect->AffecteValeurStringIndex(s,i);
		break;
	}
	return OUI;
}
//------------------------------------------------------------------------------------
char RenvoieValeurVariable(Variable* v,string& val) {

    VariableGlobale* varglob;
    VariableString* vstr;
    char ch[100];

    switch(v->sorte) {
    case VNOEUD:
	case VCHAINENOEUD:
    case VLEMMEINDEX:
    case VLEMMECHAINEINDEX:
    case VDEPENDANCE:
	case VCHAINEDEPENDANCE:
    case VVECTEURINT:
    case VVECTEURCHAINE:
        return NON;
    case VGLOBALEREINIT:
    case VGLOBALE:
        //Il s'agit d'une variable globale
        varglob=(VariableGlobale*)v;
        varglob->utilisee=OUI;
        sprintf_s(ch,100,"%g",varglob->valeur);
        val=ch;
        break;
   case VCHAINE:
        //Il s'agit d'une variable globale
        vstr=(VariableString*)v;
        vstr->utilisee=OUI;
        val=vstr->chaine;
        break;
    }
    return OUI;
}

//Cette methode renvoie une variable de type vecteur
Variable* TestFormule::RenvoieVariableVecteur(GlobalParseur* parseur,
                                              VECTA<Noeud*>& noeuds,
                                              char& erreur,
                                              VECTA<ResultatFonction*>* focus,
                                              char renvoiebrute) {


    Variable* variab;
    int id,ki;
    string lemme;
    TestFormuleElementLemme* vch;
    Noeud* n;
    VariableGlobale* varglob;
    VariableLemmeIndex* vlemme;

    //Nous allons donc dans un premier tester les types de variable
    switch(element->type) {
    case VNOEUD:
        if (renvoiebrute==TESTERVECTEURVARIABLE)
            return NULL;
        return element->valeur_variable()->var;
    case VCHAINENOEUD:
        if (renvoiebrute==TESTERVECTEURVARIABLE)
            return NULL;
        return element->valeur_variable_chaine()->var;
    case VDEPENDANCE:
        if (renvoiebrute==TESTERVECTEURVARIABLE)
            return NULL;
        return element->valeur_dependance()->var;
    case VCHAINEDEPENDANCE:
        if (renvoiebrute==TESTERVECTEURVARIABLE)
            return NULL;
        return element->valeur_dependance_chaine()->var;
    case VGLOBALE:
        //Il s'agit d'une variable globale
        if (renvoiebrute==TESTERVECTEURVARIABLE)
            return NULL;
        return element->valeur_global();
	case VGRAPHE:
		return ((TestFormuleElementGraphe*)element)->vg;
	case VKIFOBJET:
		return ((TestFormuleElementKif*)element)->vp;
	case VVECTEURGRAPHE:
		return ((TestFormuleElementVectorGraphe*)element)->var;
	case VLEMMECHAINEINDEX:
    case VLEMMEINDEX:
        //c'est une variable de chaine
        if (renvoiebrute==TESTERVECTEURVARIABLE)
            return NULL;
        vlemme=element->valeur_lemme();
        vch = (TestFormuleElementLemme*)element;
        n = noeuds[vch->indice];
        if (n==NULL)
            return NON;

        //Il s'agit d'une concatenation du nom de la variable avec le lemme
        //sprintf((char*)lemme,"%s%s#",vlemme->nom,n->X->nom);
        lemme=vlemme->nom;
        id=lemme.size();
        //On concatene les lemmes dans cette chaine
        n->Lemme(lemme);
        ki=id;
        while (ki<lemme.size() && lemme[ki]==' ') ki++;
		if (ki>=lemme.size())
			return NON;

        varglob =vlemme->recherche(USTR(lemme)+ki,id);
        return varglob;
    case VCHAINE:
        //Il s'agit d'une variable globale
        if (renvoiebrute==TESTERVECTEURVARIABLE)
            return NULL;
        return element->valeur_string();
    case VVECTEURINT:
        return ((TestFormuleElementIntVector*)element)->var;
    case VDICTIONNAIRE:
    case VVECTEURCHAINE:
        return ((TestFormuleElementVectorString*)element)->var;
#ifdef XIPPYTHON
    case VPYTHONOBJET:
        return ((TestFormuleElementPython*)element)->vp;
#endif
#ifdef XIPLIBXML
    case VXMLOBJET:
        return ((TestFormuleElementXML*)element)->vxml;
#endif
    default:
        return NULL;
    }

    if (renvoiebrute!=TESTERVECTEURVARIABLE)
        return variab;

    switch(variab->sorte) {
    case VVECTEURCHAINE:
    case VVECTEURINT:
        return variab;
    }

    return NULL;
}

//On inverse buffbis et on range le resultat dans bufftris
void inversionchaine(string buffbis,string& bufftris) {
    bufftris="";
    for (int i=buffbis.size()-1;i>=0;i--) {
        bufftris+=buffbis[i];
    }
}

void inversionchaineinterne(string& buffbis) {
    string bufftris="";
    for (int i=buffbis.size()-1;i>=0;i--) {
        bufftris+=buffbis[i];
    }
    buffbis=bufftris;
}


graphe* TestFormuleElementGraphe::clone(VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* focus) {
	graphe* g=NULL;
	if (patron!=NULL) {
		g=patron->clone(noeuds,erreur,focus);
		patron->parseur->retiregraphe(vg->g);
		vg->g=g;
	}
	return g;
}

void VariableVectorGraphe::raz() {
	GlobalParseur* parseur;
	if (graphes[0]!=NULL)
		parseur=graphes[0]->parseur;
	for (int i=0;i<graphes.dernier;i++)
		parseur->retiregraphe(graphes[i]);
	graphes.raz();
}


#ifdef XIPLIBXML
void VariableVectorGraphe::CreationXML(GlobalParseur* parseur,xmlNodePtr parent) {
	for (int i=0;i<graphes.dernier;i++)
		graphes[i]->sortieXml(parent);
}

void VariableGraphe::CreationXML(GlobalParseur* parseur,xmlNodePtr parent) {
	if (g!=NULL)
		g->sortieXml(parent);
}
#endif

void TestFormuleElementGraphe::metPatron(graphe* g) {
	ancien=patron;
	patron=g;
	patron->instanciee=OUI;
}
void TestFormuleElementGraphe::remetPatron() {
	patron->instanciee=NON;
	patron=ancien;
}


graphe* TestFormuleElementVectorGraphe::clone(VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* focus) {
	if (patron!=NULL)
		return patron->clone(noeuds,erreur,focus);
	return NULL;
}

void TestFormuleElementVectorGraphe::metPatron(graphe* g) {
	ancien=patron;
	patron=g;
	patron->instanciee=OUI;
}
void TestFormuleElementVectorGraphe::remetPatron() {
	patron->instanciee=NON;
	patron=ancien;
}

double TestFormule::TraitementFormuleGraphe(GlobalParseur* parseur,
											VECTA<graphe*>& lesgraphes,
                                            VECTA<Noeud*>& noeuds,
                                            char& erreur,
											VECTA<ResultatFonction*>* focus,
											sorteVariable surtype) {

	TestFormuleElementGraphe* tfeg;
	TestFormuleElementVectorGraphe* vtfeg;
	TestFormuleElementVectorGraphe* rvtfeg;
	VariableGraphe* vfeg;
	string buff;
	graphe* g;
	int id,i;
#ifdef XIPLIBXML
	xmlDocPtr grapheXmlRacine;
#endif
	char gardeprojection;
	sorteVariable testtype=element->type;

	if (surtype!=ACTIONGRAPHEVIDE)
		testtype=surtype;

	switch(testtype) {
	case VGRAPHE://On renvoie le graphe associe
		tfeg=(TestFormuleElementGraphe*)element;
		vfeg=tfeg->vg;
		if (vfeg->g!=NULL) {
			lesgraphes.ajoute(vfeg->g);
			return OUI;
		}
		return NON;
	case VVECTEURGRAPHE://On renvoie le graphe ou la taille a la position en question
        vtfeg=(TestFormuleElementVectorGraphe*)element;
        //dans ce cas on recupere la taille de la chaine
        if (vtfeg->indice_vecteur==NULL) {
			//On renvoie tout le contenu du vecteur
			for (i=0;i<vtfeg->var->graphes.dernier;i++)
				lesgraphes.ajoute(vtfeg->var->graphes[i]);
			return vtfeg->var->graphes.dernier;
		}
        id=(int)vtfeg->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;
		g=vtfeg->var->graphes[id];
		if (g!=NULL) {
			lesgraphes.ajoute(g);
			return OUI;
		}
		return NON;
	case VGRAPHECREATION://Creation d'un graphe
		//Il s'agit d'effectuer une creation de graphe
		tfeg=(TestFormuleElementGraphe*)element;
		tfeg->vg->utilisee=OUI;
		//On clone et on range dans la bonne variable
		g=tfeg->clone(noeuds,erreur,focus);
		if (g!=NULL) {
			lesgraphes.ajoute(g);
			return OUI;
		}
		return NON;
	case VVECTEURGRAPHECREATION://Creation d'un graphe au sein d'un vecteur de graphes
		//Il s'agit d'effectuer une creation de graphe
		vtfeg=(TestFormuleElementVectorGraphe*)element;
		//On clone et on range dans la bonne variable
		g=vtfeg->clone(noeuds,erreur,focus);
		if (g==NULL)
			return NON;
		vtfeg->var->utilisee=OUI;
		//Si on ne precise rien, pas d'indice, cela revient a un addgraph
	    if (vtfeg->indice_vecteur==NULL) {
			//On ajoute le graphe a la variable vecteur
			vtfeg->var->graphes.ajoute(g);
			lesgraphes.ajoute(g);
			return OUI;
		}
		//Sinon on calcule la position exacte
        id=(int)vtfeg->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;

		parseur->retiregraphe(vtfeg->var->graphes[id]);
		vtfeg->var->graphes.affecte(id,g);
		lesgraphes.ajoute(g);
		return OUI;
	}

	if (element->type!=VOPERATEUR)
		return NON;

	int operateur=element->valeur_operation();
	switch (operateur) {

		case 0: //affectation
		case 1://egalite
		case 3://projection
			//On prend les deux derniers graphes
			//Formules[0] est forcement un VGRAPHE ou un VVECTEURGRAPHE
			switch(formules[0]->element->type) {
				case VGRAPHE:
					tfeg=(TestFormuleElementGraphe*)formules[0]->element;
					vfeg=tfeg->vg;
					g=vfeg->g;
					break;
				case VVECTEURGRAPHE:
					vtfeg=(TestFormuleElementVectorGraphe*)formules[0]->element;
					//dans ce cas on verifie s'il s'agit bien d'un RAZ ( vecteur="")
					if (vtfeg->indice_vecteur==NULL) {
						if (formules[1]->element->type!=VALCHAINE &&
							formules[1]->element->valeur_valstring()!="") {
							erreur=OUI;
							parseur->erreur_survenue=98;
							return NON;
						}
						//On remet la structure a zero
						vtfeg->var->raz();
						return OUI;
					}
					id=(int)vtfeg->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
					if (erreur==OUI)
						return NON;
					g=vtfeg->var->graphes[id];
			}

			//On recupere notre graphe a affecter
			if (formules[1]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus,ACTIONGRAPHEVIDE)==NON)
				return NON;
			if (lesgraphes.dernier!=1)
				return NON;
			switch(operateur) {
				case 0: //affectation
					switch(formules[0]->element->type) {
						case VGRAPHE:
							parseur->retiregraphe(vfeg->g);
							vfeg->utilisee=OUI;
							vfeg->g=lesgraphes[0];
							lesgraphes[0]->ref++;
							break;
						case VVECTEURGRAPHE:
							parseur->retiregraphe(vtfeg->var->graphes[id]);
							vtfeg->var->graphes.affecte(id,g);
							vtfeg->var->utilisee=OUI;
							break;
					}
					break;
				case 1:
					if (g==NULL)
						return NON;
					return g->compare(lesgraphes[0]);
				case 3:
					if (g==NULL)
						return NON;
					return lesgraphes[0]->projection(g);
			}
			break;
#ifdef XIPLIBXML
		case 145://savexmlgraph(nom,vecteur|graph)
			if (formules.dernier!=2) {
				erreur=OUI;
				parseur->erreur_survenue=125;
				return 0;
			}
			if (formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buff,focus)==NON)
				return NON;
			if (erreur==OUI)
				return NON;

			if (formules[1]->element->type!=VGRAPHE && formules[1]->element->type!=VVECTEURGRAPHE) {
				erreur=OUI;
				parseur->erreur_survenue=126;
				return 0;
			}
			if (formules[1]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus,ACTIONGRAPHEVIDE)==NON)
				return NON;

			grapheXmlRacine=creeXMLGraphe();
			//On recupere notre graphe a affecter
			for (i=0;i<lesgraphes.dernier;i++)
				lesgraphes[i]->sortieXml(grapheXmlRacine->children);

			xmlSaveFormatFileEnc(STR(buff),grapheXmlRacine,parseur->encodage,1);
			xmlFreeDoc(grapheXmlRacine);
			return OUI;
#endif
		case 131://addgraph
			//Deux parametres
		if (formules.dernier!=2) {
			erreur=OUI;
			parseur->erreur_survenue=99;
			return 0;
		}
		if (formules[0]->element->type!=VVECTEURGRAPHE) {
			erreur=OUI;
			parseur->erreur_survenue=100;
			return 0;
		}
		vtfeg=(TestFormuleElementVectorGraphe*)formules[0]->element;
		//On recupere notre graphe a affecter
		if (formules[1]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus,ACTIONGRAPHEVIDE)==NON)
			return NON;
		for (i=0;i<lesgraphes.dernier;i++) {
			lesgraphes[i]->ref++;
			vtfeg->var->graphes.ajoute(lesgraphes[i]);
			vtfeg->var->utilisee=OUI;
		}
		return OUI;
		case 132: //project
			//Deux versions selon le nombre de parametres
			rvtfeg=NULL;
			if (formules.dernier==3) {
				gardeprojection=OUI;
				if (formules[2]->element->type!=VVECTEURGRAPHE) {
					erreur=OUI;
					parseur->erreur_survenue=102;
					return 0;
				}
				rvtfeg=(TestFormuleElementVectorGraphe*)formules[2]->element;
				rvtfeg->var->raz();
			}
			else {
				if (formules.dernier==2)
					gardeprojection=NON;
				else {
					erreur=OUI;
					parseur->erreur_survenue=101;
					return 0;
				}
			}
			//On recupere notre graphe a comparer avec la liste des autres graphes
			if (formules[1]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus,ACTIONGRAPHEVIDE)==NON)
				return NON;
			if (lesgraphes.dernier!=1) {
				erreur=OUI;
				parseur->erreur_survenue=103;
				return 0;
			}
			g=lesgraphes[0];
			lesgraphes.raz();
			//On recupere notre liste de graphe a projeter sur g
			if (formules[0]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==NON)
				return NON;
			for (i=0;i<lesgraphes.dernier;i++) {
				if (g->projection(lesgraphes[i])) {
					if (gardeprojection==NON)
						return OUI;
					rvtfeg->var->graphes.ajoute(lesgraphes[i]);
					lesgraphes[i]->ref++;
				}
			}
		if (rvtfeg!=NULL)
			return rvtfeg->var->graphes.dernier;
		return NON;
		case 133://setgraphname
			//Deux parametres
			if (formules.dernier!=2) {
				erreur=OUI;
				parseur->erreur_survenue=104;
				return 0;
			}
			//Premier parametre est un graph
			if (formules[0]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==NON)
				return NON;
			if (lesgraphes.dernier!=1) {
				erreur=OUI;
				parseur->erreur_survenue=105;
				return 0;
			}
			//Deuxieme parametre est une chaine
			if (formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buff,focus)==NON)
				return NON;
			if (erreur==OUI)
				return NON;
			lesgraphes[0]->nom=buff;
			return OUI;
		case 135://extractgraph
			//trois parametres
			if (formules.dernier!=3) {
				erreur=OUI;
				parseur->erreur_survenue=109;
				return 0;
			}
			//Premier parametre est un graph
			if (formules[0]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==NON)
				return NON;
			if (lesgraphes.dernier!=1) {
				erreur=OUI;
				parseur->erreur_survenue=109;
				return 0;
			}
			//Deuxieme parametre est un graph
			if (formules[1]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==NON)
				return NON;
			if (lesgraphes.dernier==1) {
				erreur=OUI;
				parseur->erreur_survenue=109;
				return 0;
			}
			//Le troisieme parametre est un vecteur, il peut y avoir plusieurs extractions
			if (formules[2]->element->type!=VVECTEURGRAPHE) {
				erreur=OUI;
				parseur->erreur_survenue=110;
				return 0;
			}
			vtfeg=(TestFormuleElementVectorGraphe*)formules[2]->element;
			vtfeg->var->raz();
			for (i=1;i<lesgraphes.dernier;i++)
				lesgraphes[0]->extraire(lesgraphes[i],vtfeg->var->graphes);
			return vtfeg->var->graphes.dernier;
		case 136://replacegraph
			//Trois parametres
			if (formules.dernier!=3) {
				erreur=OUI;
				parseur->erreur_survenue=111;
				return 0;
			}
			//Premier parametre, notre graphe de base, a modifier
			//Premier parametre est un graph
			if (formules[0]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==NON)
				return NON;
			//Il doit etre unique
			if (lesgraphes.dernier!=1) {
				erreur=OUI;
				parseur->erreur_survenue=112;
				return 0;
			}
			//Deuxieme parametre notre graphe patron, la aussi unique
			if (formules[1]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==NON)
				return NON;
			//Il doit etre unique
			if (lesgraphes.dernier!=2) {
				erreur=OUI;
				parseur->erreur_survenue=112;
				return 0;
			}
			//Troisieme parametre le graphe de remplacement, unique encore
			if (formules[2]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==NON)
				return NON;
			//Il doit etre unique
			if (lesgraphes.dernier!=3) {
				erreur=OUI;
				parseur->erreur_survenue=112;
				return 0;
			}
			return lesgraphes[0]->remplacement(lesgraphes[1],lesgraphes[2]);
		case 137://removesubgraph
			//Deux parametres
			if (formules.dernier!=2) {
				erreur=OUI;
				parseur->erreur_survenue=113;
				return 0;
			}
			//Premier parametre, notre graphe de base, a modifier
			//Premier parametre est un graph
			if (formules[0]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==NON)
				return NON;
			//Il doit etre unique
			if (lesgraphes.dernier!=1) {
				erreur=OUI;
				parseur->erreur_survenue=114;
				return 0;
			}
			//Deuxieme parametre notre graphe patron, la aussi unique
			if (formules[1]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==NON)
				return NON;
			//Il doit etre unique
			if (lesgraphes.dernier<=1) {
				erreur=OUI;
				parseur->erreur_survenue=115;
				return 0;
			}
			gardeprojection=NON;
			for (i=0;i<lesgraphes.dernier;i++) {
				if (lesgraphes[0]->detruitgraphe(lesgraphes[i])==OUI)
					gardeprojection=OUI;
			}
			return gardeprojection;
		case 138://removegraph
			//Deux parametres
			if (formules.dernier!=2) {
				erreur=OUI;
				parseur->erreur_survenue=116;
				return NON;
			}
			//Premier parametre un vecteur de graphes
			//Le troisieme parametre est un vecteur, il peut y avoir plusieurs extractions
			if (formules[0]->element->type!=VVECTEURGRAPHE) {
				erreur=OUI;
				parseur->erreur_survenue=117;
				return 0;
			}
			vtfeg=(TestFormuleElementVectorGraphe*)formules[0]->element;
			//Deux parametres, un index
			id=(int)formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
			if (erreur==OUI)
				return NON;
			g=NULL;
			if (id<vtfeg->var->graphes.dernier)
				g=vtfeg->var->graphes.retireElement(id);
			if (g==NULL)
				return NON;
			parseur->retiregraphe(g);
			return OUI;
		case 139://duplicategraph
			//Deux parametres
			if (formules.dernier!=2) {
				erreur=OUI;
				parseur->erreur_survenue=118;
				return NON;
			}
			//Premier parametre un vecteur de graphes
			//Le troisieme parametre est un vecteur, il peut y avoir plusieurs extractions
			if (formules[0]->element->type!=VGRAPHE && formules[0]->element->type!=VVECTEURGRAPHE) {
				erreur=OUI;
				parseur->erreur_survenue=119;
				return 0;
			}
			else {
				if (formules[0]->element->type==VGRAPHE)
					testtype=VGRAPHECREATION;
				else
					testtype=VVECTEURGRAPHECREATION;
			}

			//Deux parametres, un index
			//Deuxieme parametre notre graphe patron, la aussi unique
			if (formules[1]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus,ACTIONGRAPHEVIDE)==NON)
				return NON;
			//Il doit etre unique
			if (lesgraphes.dernier!=1) {
				erreur=OUI;
				parseur->erreur_survenue=120;
				return NON;
			}
			//On initialise le patron et on relance le calcul
			formules[0]->element->metPatron(lesgraphes[0]);
			lesgraphes.raz();
			formules[0]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus,testtype);
			if (lesgraphes.dernier==1)
				lesgraphes[0]->nom+="_dup";
			formules[0]->element->remetPatron();
			return lesgraphes.dernier;
	}
	return NON;
}


double TestFormule::TraitementFormuleString(GlobalParseur* parseur,
                                            VECTA<Noeud*>& noeuds,
                                            char& erreur,
                                            string& buff,
                                            VECTA<ResultatFonction*>* focus) {

    VariableString* vstr;
    TestFormuleElementString* vfs;
    TestFormuleElementNoeud* vnod;
    string* s;
    TestFormuleElementVectorString* vvect;
    TestFormuleElementIntVector* ivect;
    TestFormuleElementVectorString* vvectbis;
	TestFormuleElementChaineDependance* depch;
	TestFormuleElementChaineNoeud* vchn;

	VariableLemmeIndex* vlemme=NULL;
	TestFormuleElementLemme* vch;

	char evalue=OUI;
    Variable* variab;
    VariableVector* varvector;
    VariableVector* varvectbis;
    VariableDictionnaire* vdict;
	TestFonction* tf;
    ostream* os=&cout;
    static time_t date_et_heure;
    static struct tm *temps;
#ifdef XIPLIBXML
	VariableXML* vxml;
#endif
	ResultatFonction* rf;


    char ch[256];
	char* chaine;
    char car;
    double res;
    int id,iv;
    string lemme="";
    int i;
    string buffbis="";
    string bufftris="";
    string reschaine="";
    automate* a;
    char construire=OUI;

    etat* e;
    VECTA<string*> vectr;
    char egal;
    Noeud* n;
    if (erreur==2) {
        construire=NON;
        erreur=0;
    }
    //cas standard, ca devient une chaine
    STVecteur vval;
    double xres;
	VECTA<graphe*> lesgraphes;

    switch (element->type) {
	case VGRAPHE:
	case VVECTEURGRAPHE:
		if (TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==OUI) {
			for (i=0;i<lesgraphes.dernier;i++) {
				lesgraphes[i]->chaine(buffbis);
				buff+=buffbis;
			}
			return OUI;
		}
		return NON;
    case VALCHAINE:
        if (element->valeur_valstring()!=NULL)
            buff+=(char*)element->valeur_valstring();
        return OUI;
    case VCHAINEDEPENDANCE:
        depch=element->valeur_dependance_chaine();
        rf=focus->cell(depch->indice);
        if (rf==NULL) {
            erreur=OUI;
            parseur->erreur_survenue=9;
            sprintf_s(parseur->ajoute_info_erreur,4096," dependency:%d",depch->indice-1);
            return NON;
        }
        id=depch->var->AjouteDependance(rf);
        i=depch->var->position;

        if (rf->vardep[i]==NULL) {
            rf->vardep.affecte(i,depch->var);
            depch->var->Affecte(id,"");
        }

		if (depch->var->valeurs[id]!=NULL)
			buff+=*depch->var->valeurs[id];
        return OUI;
    case VLEMMECHAINEINDEX:
        //c'est une variable de chaine
        vlemme=element->valeur_lemme();
        vch = (TestFormuleElementLemme*)element;
        n = noeuds[vch->indice];
        if (n==NULL) {
            erreur=OUI;
            parseur->erreur_survenue=8;
            sprintf_s(parseur->ajoute_info_erreur,4096," node:%d",vch->indice);
            return NON;
        }

        //Il s'agit d'une concatenation du nom de la variable avec le lemme
        //sprintf((char*)lemme,"%s%s#",vlemme->nom,n->X->nom);
        lemme=vlemme->nom;
        i=lemme.size();
        //On concatene les lemmes dans cette chaine
        n->Lemme(lemme);
        id=i;
        while (lemme[id]==' ') id++;

        vstr =vlemme->recherchechaine(USTR(lemme)+id,i);
        if (vstr==NULL || vstr->utilisee==NON) {
            if (vstr!=NULL && vstr->defaut!=NULL)
                return vstr->defaut->TraitementFormule(parseur,noeuds,erreur,focus);
            if (vlemme->defaut!=NULL)
                return vlemme->defaut->TraitementFormule(parseur,noeuds,erreur,focus);
            return 0;
        }
		buff+=vstr->chaine;
        return OUI;
    case VCHAINE:
        vstr=element->valeur_string();
        if (vstr==NULL) {
            erreur=OUI;
            parseur->erreur_survenue=15;
            return NON;
        }

        if (vstr->chaine.size()==0) {
            //valeur par defaut
            if (vstr->defaut!=NULL && vstr->utilisee==NON) {
                vstr->defaut->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
                vstr->AffecteValeurString((uchar*)buffbis.c_str());
                vstr->utilisee=OUI;
            }
        }

        vfs=(TestFormuleElementString*)element;
        if (vfs->indice_chaine!=NULL) {
            id=(int)vfs->indice_chaine->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            if (id>=vstr->chaine.size()) {
                erreur=OUI;
                parseur->erreur_survenue=15;
                sprintf_s(parseur->ajoute_info_erreur,4096," %s",vstr->nom);
                return NON;
            }
            buff+=c_char_index(vstr->chaine,id);
            return OUI;
        }
        else {
            buff+=vstr->chaine;
            return OUI;
        }

        return NON;
    case VCHAINENOEUD:
        //c'est une variable
        vchn=element->valeur_variable_chaine();
        n = noeuds[vchn->indice];
        if (n==NULL) {
            erreur=OUI;
            parseur->erreur_survenue=8;
            sprintf_s(parseur->ajoute_info_erreur,4096," node:%d",vchn->indice);
            return OUI;
        }

        id = vchn->var->AjouteNoeud(n);
        i=vchn->var->position;

        //Dans le cas ou la variable n'a jamais ete encore utilisee
        //On lui met sa valeur par defaut
        if (n->source->variables[i]==NULL) {
			if (vchn->var->defaut!=NULL) {
                res=vchn->var->defaut->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
				if (erreur==OUI)
					return NON;
				vchn->var->Affecte(id,buffbis);
				n->source->variables.affecte(i,vchn->var);
				vchn->var->utilisee=OUI;
			}
		}
		if (id<vchn->var->Taille())
			buff+=*vchn->var->valeurs[id];
        return NON;
	case VVECTEURINT:
        ivect=(TestFormuleElementIntVector*)element;
        //dans ce cas on recupere la taille de la chaine
        if (ivect->indice_vecteur==NULL) {
            res=ivect->var->valeurs.dernier;
            sprintf_s(ch,256,"%d",(int)res);
            buff+=ch;
            return OUI;
        }

        //On va chercher l'indice
        id=(int)ivect->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;
        //On affecte notre valeur
        if (id<ivect->var->valeurs.dernier) {
            res=ivect->var->valeurs[id];
            vval=(TVecteur)res;
            xres=(double)vval;
            if (xres!=res)
                sprintf_s(ch,256,"%g",res);
            else
                sprintf_s(ch,256,"%d",(int)res);
            buff+=ch;
        }
        return OUI;
    case VDICTIONNAIRE:
        vvect=(TestFormuleElementVectorString*)element;
        //dans ce cas on recupere la taille de la chaine
        //On va chercher notre index
        vdict=(VariableDictionnaire*)vvect->var;
        if (vvect->indice_vecteur==NULL) {
            res=vdict->dico.size();
            sprintf_s(ch,256,"%d",(int)res);
            buff+=ch;
            return OUI;
        }

        //On va chercher l'indice dans le dictionnaire
        if (vvect->indice_vecteur->TypeString()) {
			//On recupere la chaine correspondant au parametre
            res=vvect->indice_vecteur->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
            if (erreur==OUI)
                return NON;
        }
        else {
			//En fait il s'agit d'un index numerique
            res=vvect->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
			if (vdict->rechercheindex((int)res,buffbis)==NON)
				return NON;
        }

		//buffbis contient notre index (obtenu a partir d'une chaine ou d'un index)
		//On recupere la valeur correspondant a cet index
		buffbis=vdict->valeur(buffbis);
		if (vvect->indice_chaine!=NULL) {
			id=(int)vvect->indice_chaine->TraitementFormule(parseur,noeuds,erreur,focus);
			if (erreur==OUI)
				return NON;
			if (id>=buffbis.size()) {
				erreur=OUI;
				parseur->erreur_survenue=15;
				sprintf_s(parseur->ajoute_info_erreur,4096," %s",vdict->nom);
				return NON;
			}
			car=buffbis[id];
			buff+=car;
		}
		else
			buff+=buffbis;
        return OUI;
    case VVECTEURCHAINE:
        vvect=(TestFormuleElementVectorString*)element;

        //dans ce cas on recupere la taille de la chaine
        if (vvect->indice_vecteur==NULL) {
            res=vvect->var->chaine.dernier;
            sprintf_s(ch,256,"%d",(int)res);
            buff+=ch;
            return OUI;
        }

        //On va chercher l'indice
        res=vvect->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;
        //On affecte notre valeur
        if (vvect->var->chaine[(int)res]!=NULL) {
            if (vvect->indice_chaine!=NULL) {
                id=(int)vvect->indice_chaine->TraitementFormule(parseur,noeuds,erreur,focus);
                if (erreur==OUI)
                    return NON;
                if (id>=vvect->var->chaine[(int)res]->size()) {
                    erreur=OUI;
                    parseur->erreur_survenue=15;
                    sprintf_s(parseur->ajoute_info_erreur,4096," %s",vvect->var->nom);
                    return NON;
                }
                car=(*vvect->var->chaine[(int)res])[id];
                buff+=car;
            }
            else
                buff+=*vvect->var->chaine[(int)res];
        }
        return OUI;
    case VALNOEUD:
        vnod=(TestFormuleElementNoeud*)element;

        if (vnod->tf->Type()==TESTFONCTIONPROCEDURE ||
			vnod->tf->Type()==TESTFONCTIONKIF ||
            vnod->tf->Type()==TESTFONCTIONPYTHON) {
            res=vnod->tf->TraitementFormule(parseur,noeuds,erreur,focus);
            sprintf_s(ch,256,"%g",res);
            buff+=ch;
            return OUI;
        }

        lemme="";
        if (parseur->RecuperationTraitValeur(vnod->tf,lemme,noeuds,focus)==OUI)
            buff+=lemme;
        return OUI;
    }

    if (element->type!=VOPERATEUR) {
        res=TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;
        vval=(TVecteur)res;
        xres=(double)vval;
        if (xres!=res)
            sprintf_s(ch,256,"%g",res);
        else
            sprintf_s(ch,256,"%d",(int)res);
        buff+=ch;
        return OUI;
    }

    //Sinon c'est un  operateur
    int operateur=element->valeur_operation();
    switch (operateur) {
        //= (affectation)
    case 7: //+,*,- addition, multiplication, soustraction
        for (i=0;i<formules.dernier;i++) {
            formules[i]->TraitementFormuleString(parseur,noeuds,erreur,buff,focus);
            if (erreur==OUI)
                return NON;
        }
        return OUI;
    case 24: //left of a string
    case 25: //right of a string
        //Parametre 0, c'est la chaine elle-meme
        if (formules.dernier!=2) {
            if (operateur==24) {
                erreur=OUI;
                parseur->erreur_survenue=0;
            }
            else {
                erreur=OUI;
                parseur->erreur_survenue=14;
            }
            return 0;
        }

        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

        //Parametre 1, c'est la longueur de la chaine
        iv=(int)formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;

        //buffbis contient la chaine
        if (operateur==24)//c'est la partie gauche
            buff+=s_left(buffbis,iv);
        else
            buff+=s_right(buffbis,iv);
        break;
    case 26: //mid of a string
        if (formules.dernier!=3) {
            erreur=OUI;
            parseur->erreur_survenue=1;
            return -1;
        }
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

        //Parametre 1, c'est la longueur de la chaine
        id=(int)formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;

        iv=(int)formules[2]->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;

        buff+=s_middle(buffbis,id,iv);
        break;
    case 55: //lowercase
    case 56: //uppercase
        //Parametre 0, c'est la chaine elle-meme
        if (formules.dernier!=1) {
            erreur=OUI;
            parseur->erreur_survenue=38;
            return 0;
        }

        //On va chercher notre chaine
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

        //buffbis contient la chaine
        if (operateur==55)//c'est la partie gauche
			buff+=EnMinuscule(buffbis);
        else
			buff+=EnMajuscule(buffbis);
        break;
    case 62://removefirst
    case 63://removelast
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=40;
            return -1;
        }
        //le premier argument est la chaine elle-meme
        //On va chercher notre chaine
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
        //Le second argument, le nombre de caractere a retirer
        id=(int)formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;
        //On retire en tete
        iv=buffbis.length();
        if (iv>0 && id<=iv) {
            if (operateur==62)
                buff+=buffbis.c_str()+id;
            else {
                lemme=buffbis.substr(0,iv-id);
                buff+=lemme;
            }
            return OUI;
        }
        return NON;
    case 27: //regular expression regexp
    case 37: //regexpstart
    case 38: //regexpfull
    case 79: //regexplast
    case 89: //regexpback
        res=-1;
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=2;
            return -1;
        }

        //On recupere notre chaine sur laquelle appliquer l'expression reguliere
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return -1;


        if (formules[1]->element->type==VAUTOMATE)
            a=((TestFormuleElementAutomate*)formules[1]->element)->a;
        else {
            //Ou on construit l'automate a la volee
            formules[1]->TraitementFormuleString(parseur,noeuds,erreur,bufftris,focus);
            if (bufftris.size()>0)
				a=new automate((uchar*)bufftris.c_str(),(void*)1);
        }

        if (erreur==OUI)
            return -1;

        if (a!=NULL) {
            switch(operateur) {
            case 27:           //regexp
                res=a->rechercheavecextraction((uchar*)buffbis.c_str(),reschaine);
                break;
            case 37:
                e=a->recherchepartielle((uchar*)buffbis.c_str()); //regexpstart
                if (e==NULL)
                    break;
                reschaine=s_left(buffbis,a->maxpos);
                res=0;
                break;
            case 89:
                res=a->recherchejusqualafin((uchar*)buffbis.c_str(),reschaine); //regexpback
                break;
            case 38:
                e=a->recherche((uchar*)buffbis.c_str()); //regexpfull
                if (e==NULL) {
                    res=NON;
                    if (construire==OUI)
                        buff+="0";
                    break;
                }
                res=OUI;
                if (construire==OUI)
                    buff+="1";
                construire=NON;
                break;
            case 79:
                //we invert the input string
                res=a->rechercheavecextractionparlafin((uchar*)buffbis.c_str(),reschaine);
                break;
            }
            if (res!=-1 && construire==OUI)
                buff+=reschaine;

            if (bufftris.size()>0)
                delete a;
        }
        return res;
    case 28://Trimage de la chaine (on retire les caracteres blancs) trim
        if (formules.dernier!=1) {
            erreur=OUI;
            parseur->erreur_survenue=3;
            return NON;
        }

        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

        if (buffbis.size())
            buff+=Trim(buffbis);
        return OUI;
    case 64://replace(x,"yy","zz"); each occurence of yy is replaced by zz in x
        res=0;
        if (formules.dernier!=3) {
            erreur=OUI;
            parseur->erreur_survenue=4;
            return -1;
        }

        //Le premier parametre est notre chaine d'entree
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

        formules[1]->TraitementFormuleString(parseur,noeuds,erreur,bufftris,focus);
        if (erreur==OUI)
            return NON;

        //Le troisieme parametre est la chaine a remplacer
        formules[2]->TraitementFormuleString(parseur,noeuds,erreur,reschaine,focus);
        if (erreur==OUI)
            return NON;
        res=1;
		buff+=replacestringwithautomaton(buffbis,bufftris,reschaine);
        return res;
    case 29: //Construction avec un vecteur et une expression reguliere (3 parametres) regexpsplit
        res=-1;
        if (formules.dernier!=3) {
            erreur=OUI;
            parseur->erreur_survenue=4;
            return -1;
        }
        //le troisieme parametre doit etre une variable vecteur
        variab= formules[2]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
            erreur=OUI;
            parseur->erreur_survenue=5;
            return -1;
        }

        varvector=(VariableVector*)variab;

        //Le premier parametre est notre chaine d'entree
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

        if (formules[1]->element->type==VAUTOMATE)
            a=((TestFormuleElementAutomate*)formules[1]->element)->a;
        else {
            //Ou on construit l'automate a la volee
            formules[1]->TraitementFormuleString(parseur,noeuds,erreur,bufftris,focus);
            if (bufftris.size()>0)
                a=new automate((uchar*)bufftris.c_str(),(void*)1);
        }



        if (erreur==OUI)
            return NON;

        if (a!=NULL) {
            varvector->raz();
            vectr.raz();
            res=a->rechercheavecextractiondansvecteur((uchar*)buffbis.c_str(),vectr);
            for (iv=0;iv<vectr.dernier;iv++)
                varvector->AffecteValeurStringIndex(vectr[iv],iv);

            if (bufftris.size()>0)
                delete a;
        }
        return res;
    case 185: //Construction avec un vecteur autour d'un caractere (3 parametres) splitalong(chaine,caractere,vect)
        res=-1;
        if (formules.dernier!=3) {
            erreur=OUI;
            parseur->erreur_survenue=175;
            return -1;
        }
        //le troisieme parametre doit etre une variable vecteur
        variab= formules[2]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
            erreur=OUI;
            parseur->erreur_survenue=176;
            return -1;
        }

        varvector=(VariableVector*)variab;

        //Le premier parametre est notre chaine d'entree
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

		formules[1]->TraitementFormuleString(parseur,noeuds,erreur,bufftris,focus);
        if (erreur==OUI)
            return NON;

		varvector->raz();
		vectr.raz();
		decoupechaine(buffbis,bufftris,vectr);
		for (iv=0;iv<vectr.dernier;iv++)
			varvector->AffecteValeurStringIndex(vectr[iv],iv);
		res=vectr.dernier;
		return res;
#ifdef XIPLIBXML
	case 188: //xmlstring
		if (formules.dernier!=2) {
			erreur=OUI;
			parseur->erreur_survenue=180;
			return -1;
		}
		//premier parametre le nom de la balise
		formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
		formules[1]->TraitementFormuleString(parseur,noeuds,erreur,bufftris,focus);
		buff+=xmlEncoding(buffbis,bufftris);
		return OUI;
#endif
    case 48: //lemmas
        res=-1;
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=26;
            return -1;
        }

        n= noeuds[formules[1]->element->argument(parseur,noeuds)];
        if (n==NULL || n->source->voc==NULL) {
            erreur=OUI;
            parseur->erreur_survenue=27;
            return -1;
        }

        variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
            erreur=OUI;
            parseur->erreur_survenue=28;
            return -1;
        }

        varvector=(VariableVector*)variab;
        varvector->raz();
        //On range alors tous les lemmes dans vvect
        for (iv=0;iv<n->source->voc->lemme.dernier;iv++)
            varvector->AffecteValeurStringIndex(new string(n->source->voc->lemme[iv]),iv);
        return 1;
	case 146://GetInitialFeatures
        res=-1;
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=128;
            return -1;
        }

        n= noeuds[formules[0]->element->argument(parseur,noeuds)];
        if (n==NULL || n->source->voc==NULL) {
            erreur=OUI;
            parseur->erreur_survenue=129;
            return -1;
        }

		variab= formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
            erreur=OUI;
            parseur->erreur_survenue=130;
            return -1;
        }

        varvector=(VariableVector*)variab;
        varvector->raz();
        //On range alors tous les lemmes dans vvect
		for (id=0;id<n->source->voc->nbX;id++) {
			if (n->source->voc->tag[id]!=NULL)
				varvector->AffecteValeurStringIndex(new string(n->source->voc->tag[id]),id);
		}

		if (n->source->voc->tag[n->source->choisie]!=NULL)
			buff+=n->source->voc->tag[n->source->choisie];
        return 1;
    case 88://backsearch uniquement avec un dictionary
        if (formules.dernier<2 || formules.dernier>3) {
            erreur=OUI;
            parseur->erreur_survenue=62;
            return -1;
        }

        //Tout d'abord notre variable de type dictionary
        variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VDICTIONNAIRE)  {
            erreur=OUI;
            parseur->erreur_survenue=61;
            return -1;
        }

        vdict=(VariableDictionnaire*)variab;

        //Si le troisieme parametre est fourni, on garde une trace dans un vecteur
        varvectbis=NULL;
        if (formules[2]!=NULL) {
            variab=formules[2]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
            if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
                erreur=8;
                parseur->erreur_survenue=62;
                return -1;
            }
            varvectbis=(VariableVector*)variab;
        }

        //Le second parametre est notre chaine a rechercher
        formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

        res=-1;
        iv=-1;
        if (buffbis.size()>0) {

            //si l'on a un troisieme parametre, on y stocke toutes les valeurs
            if (varvectbis!=NULL)
                iv=vdict->recherchetoutechaineindex(buffbis,varvectbis);
            else {//buffbis est une valeur dont on recherche l'index
                iv=vdict->recherchechaineindex(buffbis,reschaine);
                if (construire==OUI)
                    buff+=reschaine;
            }
        }
        return iv;
    case 107://keys uniquement avec un dictionary
		//recuperation de toutes les clefs d'un dictionaire
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=81;
            return -1;
        }

        //Tout d'abord notre variable de type dictionary
        variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VDICTIONNAIRE)  {
            erreur=OUI;
            parseur->erreur_survenue=82;
            return -1;
        }

        vdict=(VariableDictionnaire*)variab;

        //Si le troisieme parametre est fourni, on garde une trace dans un vecteur
        variab=formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
            erreur=8;
            parseur->erreur_survenue=83;
            return -1;
        }
        varvectbis=(VariableVector*)variab;
		return vdict->lesclefs(varvectbis);
    case 186://key uniquement avec un dictionary: keyonindex(dictionary,i)
		//recuperation de la clef correspondant a l'index i
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=81;
            return -1;
        }

        //Tout d'abord notre variable de type dictionary
        variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VDICTIONNAIRE)  {
            erreur=OUI;
            parseur->erreur_survenue=82;
            return -1;
        }

        vdict=(VariableDictionnaire*)variab;

		id=formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
		res=vdict->rechercheindex(id,buffbis);
		buff+=buffbis;
		return res;
    case 108://values uniquement avec un dictionary
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=81;
            return -1;
        }

        //Tout d'abord notre variable de type dictionary
        variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VDICTIONNAIRE)  {
            erreur=OUI;
            parseur->erreur_survenue=82;
            return -1;
        }

        vdict=(VariableDictionnaire*)variab;

        //Si le troisieme parametre est fourni, on garde une trace dans un vecteur
        variab=formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
            erreur=8;
            parseur->erreur_survenue=83;
            return -1;
        }
        varvectbis=(VariableVector*)variab;
		return vdict->lesvaleurs(varvectbis);
    case 109://@sentence
        if (parseur->donneesATraiter!=NULL) {
            buff+=(char*)parseur->donneesATraiter;
            return OUI;
        }
        return NON;
	case 148://@generated
		car=OUI;
		parseur->parcourssousnoeud(&parseur->graphegeneration,buffbis,car);
		buff+=buffbis;
        return OUI;
     case 157://lookup
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=151;
            return 0;
        }
		//On recupere la chaine
		formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
		//le deuxieme parametre doit etre une variable vecteur
		variab= formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
		if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
			erreur=OUI;
			parseur->erreur_survenue=21;
			return -1;
		}

		varvector=(VariableVector*)variab;
		varvector->raz();

		if (parseur->lecturemontante(buffbis,varvector->chaine)==NON)
			return NON;
		return varvector->chaine.dernier;
     case 158://lookdown
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=152;
            return 0;
        }
		//On recupere la chaine
		formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
		//On recupere les traits
		formules[1]->TraitementFormuleString(parseur,noeuds,erreur,bufftris,focus);
        if (erreur==OUI)
            return NON;

		if (parseur->lecturedescendante(buffbis,bufftris,reschaine)==NON)
			return NON;
		buff+=reschaine;
		return OUI;
	  case 177://ruletypestring
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=161;
			  return 0;
		  }
		  id=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  if (id==-1) {
			  if (parseur->reglecourante!=NULL) {
				  buff=field_name[parseur->reglecourante->label->type];
				  return OUI;
			  }
			  else
				  return NON;
		  }
		  if (parseur->VBases[id]==NULL) {
			  erreur=OUI;
			  parseur->erreur_survenue=162;
			  return 0;
		  }
		  buff=field_name[parseur->VBases[id]->label->type];
		  return OUI;
	  case 178://saveprobabilities
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=163;
			  return 0;
		  }
		  formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buff,focus);
		  parseur->sauvegardeprobabilites(buff);
		  return(1);
	  case 179://loadprobabilities
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=164;
			  return 0;
		  }
		  formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buff,focus);
		  parseur->chargeprobabilites(buff);
		  return(1);
    case 104://pop($1)
		if (formules.dernier!=1) {
			erreur=OUI;
			parseur->erreur_survenue=75;
			return 0;
		}

		if (formules[0]->element->type!=VALNOEUD) {
			erreur=OUI;
			parseur->erreur_survenue=65;
			return 0;
		}

		tf=((TestFormuleElementNoeud*)formules[0]->element)->tf;
		if (tf->Type()!=TESTFONCTIONDEPENDANCE) {
			erreur=OUI;
			parseur->erreur_survenue=65;
			return 0;
		}
		//Sinon, il s'agit d'une dependance
		res=0;
		id=tf->IndexDependance();
		if (id!=-1 && focus!=NULL && (*focus)[id]!=NULL) {
			rf=(*focus)[id];
			res=1;
			chaine=rf->stack.retire();
			if (chaine!=NULL) {
				buff+=chaine;
				libere(chaine);
			}
		}
		return res;
	case 128://getenv
		if (formules.dernier!=2) {
			erreur=OUI;
			parseur->erreur_survenue=95;
			return 0;
		}

		//On recupere ensuite la chaine a introduire dans la pile
		formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
		if (erreur==OUI)
			return NON;
		variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,RENVOIEVARIABLE);
		if (variab==NULL || variab->sorte!=VCHAINE)  {
			erreur=OUI;
			parseur->erreur_survenue=95;
			return 0;
		}
		vstr=(VariableString*)variab;
		if (getenv(buffbis.c_str())!=NULL) {
			vstr->chaine=getenv(buffbis.c_str());
			buff+=vstr->chaine;
		}
		break;
	case 129://putenv
		if (formules.dernier!=2) {
			erreur=OUI;
			parseur->erreur_survenue=96;
			return 0;
		}

		//On recupere ensuite la chaine a introduire dans la pile
		formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
		if (erreur==OUI)
			return NON;
		formules[1]->TraitementFormuleString(parseur,noeuds,erreur,bufftris,focus);
		if (erreur==OUI)
			return NON;
		buffbis+="=";
		buffbis+=bufftris;
		buff+=bufftris;
#ifdef WIN32
			_putenv((char*)buffbis.c_str());
#else
			putenv((char*)buffbis.c_str());
#endif
		break;
    case 32: //search
        if (formules.dernier<2 || formules.dernier>3) {
            erreur=OUI;
            parseur->erreur_survenue=6;
            return -1;
        }

        variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
            erreur=OUI;
            parseur->erreur_survenue=7;
            return -1;
        }

        varvector=(VariableVector*)variab;

        //Si le troisieme parametre est fourni, on garde une trace dans un vecteur
        varvectbis=NULL;
        if (formules[2]!=NULL) {
            variab=formules[2]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
            if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
                erreur=8;
                parseur->erreur_survenue=23;
                return -1;
            }
            varvectbis=(VariableVector*)variab;
        }

        //Le second parametre est notre chaine a rechercher
        formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

        res=-1;
        iv=-1;
        if (buffbis.size()>0) {

            //si l'on a un troisieme parametre, on y stocke toutes les valeurs
            if (varvectbis!=NULL) {
                iv=varvector->recherchetous(buffbis,varvectbis);
                if (iv==0)
                    return -1;
            }
            else
                iv=varvector->recherchepremier(buffbis);

            if (construire==OUI)
                buff+=buffbis;
        }
        return iv;
    case 34://le message d'erreur
        if (formules.dernier!=1) {
            buff+="No Error message provided";
            return NON;
        }
        iv=(int)formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
        if (iv==-1)
            buff+="No error detected so far";
        else
            buff+=affichagemessageerreur[iv];
        return iv;
    case 44://The pathname: @pathname
        if (parseur->fichier_en_cours!=NULL)
            buff+=parseur->fichier_en_cours;
        break;
    case 77: //read a string
        if (formules.dernier!=1) {
            erreur=OUI;
            parseur->erreur_survenue=56;
            return 0;
        }
        fgets(ch,256,stdin);
        Trim(ch);
        formules[0]->AffecteVariable(parseur,(uchar*)ch,noeuds,focus);
        break;
    case 78://print
    case 81://_print
        if (parseur->OS!=NULL)
            os=parseur->OS;
		for (iv=0;iv<formules.dernier;iv++)
			formules[iv]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
#if defined(WIN32) && !defined(_WINDLL)
        if (AFFICHAGEDOS==1)
            *os<<ConversionChaine(parseur,(char*)buffbis.c_str());
        else
#endif
            *os<<buffbis;

        break;
    case 84://printerr
        if (parseur->OS!=NULL)
            os=parseur->OS;
		for (iv=0;iv<formules.dernier;iv++)
			formules[iv]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
#if defined(WIN32) && !defined(_WINDLL)
        if (AFFICHAGEDOS==1)
            *flot_erreur<<ConversionChaine(parseur,(char*)buffbis.c_str());
        else
#endif
            *flot_erreur<<buffbis;
        break;
    case 80://The grammar language @language
        if (parseur->langue_de_la_grammaire!=NULL) {
            buff+=parseur->langue_de_la_grammaire;
			buff+=choix_ambigue;
		}
        break;
    case 90://The grammar language @sentencelanguage
        buff+=parseur->langue_de_la_phrase;
        break;
    case 82://The grammar version: @version
        buff+=parseur->version_grammaire;
        break;
    case 83://The XIP engine version: @xip
        buff+=VERSION_XIP_BUILD;
        break;
    case 86://error Arret de XIP
        if (formules.dernier>=1)
            formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        else
            buffbis=affichagemessageerreur[60];
        parseur->erreur((char*)buffbis.c_str());
        break;
    case 92: //@date
        time(&date_et_heure);
        temps = localtime(&date_et_heure);
        buff+=asctime(temps);
        break;
    case 94: //@monthstring
        time(&date_et_heure);
        temps = localtime(&date_et_heure);
        strcpy_s(ch,256,parseur->CHAINES[338+temps->tm_mon]);
        buff+=ch;
        break;
    case 95://@daystring
        time(&date_et_heure);
        temps = localtime(&date_et_heure);
        strcpy_s(ch,256,parseur->CHAINES[350+temps->tm_wday]);
        buff+=ch;
        break;
    case 96://@hour
        time(&date_et_heure);
        temps = localtime(&date_et_heure);
        sprintf_s(ch,256,"%d:%d:%d",temps->tm_hour,temps->tm_min,temps->tm_sec);
        buff+=ch;
        break;
	case 133://getgraphname
		//Deux parametres
		if (formules.dernier!=2) {
			erreur=OUI;
			parseur->erreur_survenue=106;
			return 0;
		}

		//premier parametre est une variable de chaine
		variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,RENVOIEVARIABLE);
		if (variab==NULL || variab->sorte!=VCHAINE)  {
			erreur=OUI;
			parseur->erreur_survenue=107;
			return 0;
		}
		vstr=(VariableString*)variab;

		//Deuxieme parametre est un graphe
		if (formules[1]->TraitementFormuleGraphe(parseur,lesgraphes,noeuds,erreur,focus)==NON)
			return NON;
		if (lesgraphes.dernier!=1) {
			erreur=OUI;
			parseur->erreur_survenue=108;
			return 0;
		}
		vstr->chaine=lesgraphes[0]->nom;
		return OUI;
#ifdef XIPLIBXML
	case 116://xmltext
	case 118://xmltag
		if (formules.dernier!=2) {
			erreur=OUI;
			parseur->erreur_survenue=87;
			return 0;
		}
		variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
		if (variab==NULL || variab->sorte!=VXMLOBJET)  {
			erreur=OUI;
			parseur->erreur_survenue=85;
			return -1;
		}
		vxml=(VariableXML*)variab;
		variab= formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,RENVOIEVARIABLE);
		if (variab==NULL || variab->sorte!=VCHAINE)  {
			erreur=OUI;
			parseur->erreur_survenue=89;
			return 0;
		}
		vstr=(VariableString*)variab;
		if (vxml->noeud==NULL || vxml->noeud->type==XML_DOCUMENT_NODE || vxml->noeud->name==NULL)
			return NON;
		switch(operateur) {
		case 116:
			RecupereTextXmlNode(buffbis,vxml->noeud);
			break;
		case 118:
			if (vxml->noeud==NULL || vxml->noeud->type==XML_DOCUMENT_NODE || vxml->noeud->name==NULL)
				return NON;
			buffbis=(char*)vxml->noeud->name;
			break;
		}
		vstr->chaine=buffbis;
		buff=buff+buffbis;
		break;
#endif
	case 142://addword
		if (formules.dernier!=4) {
			erreur=OUI;
			parseur->erreur_survenue=122;
			return 0;
		}
		//On recupere les lemmes, surfaces et autres traits
		formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buff,focus);
		formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
		formules[2]->TraitementFormuleString(parseur,noeuds,erreur,lemme,focus);
		formules[3]->TraitementFormuleString(parseur,noeuds,erreur,bufftris,focus);
		id=parseur->ajoutemot(buff,buffbis,lemme,bufftris);
		if (id==NON)
			erreur=OUI;
		if (id==2) {
			parseur->erreur_survenue=123;
			erreur=OUI;
		}
		break;
    case 0:
    case 1:
    case 6://INITIALISATION DES VARIABLES DE CHAINE
        //On recupere notre variable avec initialisation au besoin
        id=-1;
        n=NULL;
		evalue=operateur;
        switch(formules[0]->element->type) {
        case VLEMMECHAINEINDEX:
            //c'est une variable de chaine
            vlemme=formules[0]->element->valeur_lemme();
            vch = (TestFormuleElementLemme*)formules[0]->element;
            n = noeuds[vch->indice];
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=8;
                sprintf_s(parseur->ajoute_info_erreur,4096," node:%d",vch->indice);
                return NON;
            }
            //Il s'agit d'une concatenation du nom de la variable avec le lemme
            //sprintf((char*)lemme,"%s%s#",vlemme->nom,n->X->nom);
            lemme=vlemme->nom;
            i=lemme.size();
            //On concatene les lemmes dans cette chaine
            n->Lemme(lemme);
            //Il peut y avoir des blancs en debut de lemme
            id=i;
            while (lemme[id]==' ') id++;
			if (operateur==0)
				operateur=17;
            break;
        case VCHAINEDEPENDANCE:
            depch=formules[0]->element->valeur_dependance_chaine();
            rf=focus->cell(depch->indice);
            if (rf==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=9;
                sprintf_s(parseur->ajoute_info_erreur,4096," dependency:%d",depch->indice-1);
                return NON;
            }
            id=depch->var->AjouteDependance(rf);
			if (operateur==0)
				operateur=18;
            break;
        case VDICTIONNAIRE:
        case VVECTEURCHAINE:
            //On affecte notre valeur
            vvect=(TestFormuleElementVectorString*)formules[0]->element;


            //remise-a-zero du tableau
            if (vvect->indice_vecteur==NULL) {
                if (formules[1]!=NULL) {
                    vvectbis=NULL;
                    if (formules[1]->element->type==VVECTEURCHAINE)
                        vvectbis=(TestFormuleElementVectorString*)formules[1]->element;
                    if (vvectbis!=NULL) {
                        vvect->var->raz();
                        for (iv=0;iv<vvectbis->var->chaine.dernier;iv++)
                            vvect->var->AffecteValeurStringIndex(new string(*vvectbis->var->chaine[iv]),iv);
                        return OUI;
                    }

                    //dans ce cas c'est une raz du vecteur: vect="";
                    if (operateur==0) {
                        formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buff,focus);
                        if (buff.size()==0) {
                            vvect->var->raz();
                            return OUI;
                        }
                    }
                    else {//sinon c'est une comparaison mathematique avec la taille du vecteur
                        return TraitementFormule(parseur,noeuds,erreur,focus);
                    }
                }

                //dans ce cas, il s'agit soit
                sprintf_s(parseur->ajoute_info_erreur,4096," %s",vvect->var->nom);
                parseur->erreur_survenue=29;
                erreur=OUI;
                return -1;
            }

            //En cas d'affectation, il nous faut toutes les infos possibles
            if (operateur==0) {
                //On va chercher l'indice
                iv=-1;
                if (vvect->var->sorte!=VDICTIONNAIRE) {
                    //dans le cas d'un dictionnaire nous trouvons notre indice
                    //differement. Car l'indice est une chaine
                    res=vvect->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
                    if (erreur==OUI)
                        return NON;
                    iv=(int)res;
                }
                else {
                    //l'index est une chaine
                    if (vvect->indice_vecteur->TypeString()) {
                        res=vvect->indice_vecteur->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
                        operateur=20;
                    }
                    else {
                        res=vvect->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
                        operateur=21;
                    }
                    if (erreur==OUI)
                        return NON;
                    vdict=(VariableDictionnaire*)vvect->var;
                }

                if (vvect->indice_chaine!=NULL) {
                    id=(int)vvect->indice_chaine->TraitementFormule(parseur,noeuds,erreur,focus);
                    if (erreur==OUI)
                        return NON;
                }
            }

            //On affecte notre valeur
            if (operateur==0)
                operateur=10;
            break;
		case VCHAINENOEUD:
            vchn=formules[0]->element->valeur_variable_chaine();
            //l'indice correspond au noeud qui nous interesse:
            n = noeuds[vchn->indice];
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=8;
                sprintf_s(parseur->ajoute_info_erreur,4096," node:%d",vchn->indice);
                return NON;
            }
            id=vchn->var->AjouteNoeud(n);
			if (operateur==0)
				operateur=9;
            break;
        case VCHAINE:
            //En cas d'affectation, il nous faut tous les indexes
            if (operateur==0) {
                vstr=formules[0]->element->valeur_string();
                vfs=(TestFormuleElementString*)formules[0]->element;
                if (vfs->indice_chaine!=NULL) {
                    id=(int)vfs->indice_chaine->TraitementFormule(parseur,noeuds,erreur,focus);
                    if (erreur==OUI)
                        return NON;
                }
            }
       }


	   if (evalue!=NON) {
		   formules[0]->TraitementFormuleString(parseur,noeuds,erreur,reschaine,focus);
		   if (erreur==OUI)
			   return NON;
	   }

        formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buff,focus);
        if (erreur==OUI)
            return NON;

        egal=NON;
        switch (operateur) {
        case 0:     //affectation
			if (id==-1)
				vstr->chaine=buff;
			else {
				if (id>=vstr->chaine.size())
					return NON;
				else
					c_char_index_assign(vstr->chaine, buff,id);
			}
			vstr->utilisee=OUI;
            return OUI;
        case 6: //difference
            egal=OUI;
        case 1: //egalite
            if (buff==reschaine)
                egal=1-egal;
            return egal;
		case 9: // avec vchn
			vchn->var->Affecte(id,buff);
			vchn->var->utilisee=OUI;
			return OUI;
        case 10: //avec vvect(affectation)
            if (id==-1) {
                s=new string(buff);
                vvect->var->AffecteValeurStringIndex(s,iv);
            }
            else {
                if (vvect->var->chaine[(int)res]==NULL ||
                    id>=vvect->var->chaine[(int)res]->size())
                    return NON;
                else
                    (*vvect->var->chaine[(int)res])[id]=buff[0];
            }
            return OUI;
		case 17:
			vlemme->utilisee=OUI;
			majvariablelemme(vlemme,n,USTR(lemme),i,buff,0);
			return OUI;
		case 18://VCHAINEDEPENDANCE
			depch->var->Affecte(id,buff);
			rf->vardep.affecte(depch->var->position,depch->var);
			break;
        case 20://Affectation dans un dictionnaire avec une chaine comme index
            vdict->AffecteValeurStringIndexChaine(new string(buff),buffbis);
            break;
        case 21://Affectation dans un dictionnaire avec un index numerique
            vdict->AffecteValeurStringIndex(new string(buff),res);
            break;
        }
   default://Il ne s'agit pas d'une operation sur les chaines
       //On donne la main alors au traitement des formules numeriques
       res=TraitementFormule(parseur,noeuds,erreur,focus);
       if (erreur==OUI)
           return NON;
       vval=(TVecteur)res;
       xres=(double)vval;
       if (xres!=res)
           sprintf_s(ch,256,"%g",res);
       else
           sprintf_s(ch,256,"%d",(int)res);
       buff+=ch;
       return OUI;
    }

    return OUI;
}


double TestFormule::TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& noeuds,
                                      char& erreur,VECTA<ResultatFonction*>* focus) {

    //"=",":","<",">","<=",">=","+","*","/","-","^","mod","log","ln","exp","sqrt";
    string lemme;
    double res=0,resinter=0;
    int i,ind,pos=0,ki,id;
    char car;
    Noeud* n=NULL;
    Noeud* n2;
    string* s=NULL;
    Label* l;
    TestFonction* tf;
    Traits* trbase;
    Traits* copie=NULL;
    Vecteur* vecteur=NULL;
    Traits* tr;
	ResultatFonction* rf1;
	ResultatFonction* rf2;

    VariableGlobale* varglob;
    VariableLemmeIndex* vlemme=NULL;
    VariableVector* varvector;
    VariableIntVector* varintvector;
    VariableIntVector* varintvectorbis;

    Variable* variab;

#ifdef XIPLIBXML
	VariableXML* vxml;
	VariableXML* vxmlbis;
#endif

	TestFormuleElementChaineDependance* depch;
    TestFormuleElementVariable* v;
    TestFormuleElementLemme* vch;
    TestFormuleElementDependance* dep;
    TestFormuleElementNoeud* vnod;
    TestFormuleElementIntVector* ivect;
    ResultatFonction* rf=NULL;
    string buffbis="";
    static time_t tempsinitial,tempsfinal;
    fichiersscript* fscript;
    static time_t date_et_heure;
    static struct tm *temps;
    char* chaine;
    static time_t  intermediate;
    TestFormuleElementVectorGraphe* vtfeg;
	VECTA<automate*> expressions;
	VECTA<Noeud*> lnoeuds;
	Categorie* cfoncx;

    switch(element->type) {
	case VGRAPHE:
		return 0;
	case VVECTEURGRAPHE:
        vtfeg=(TestFormuleElementVectorGraphe*)element;
        //dans ce cas on recupere la taille de la chaine
        if (vtfeg->indice_vecteur==NULL)
			return vtfeg->var->graphes.dernier;
		return 0;
    case VALREEL:
        //on renvoie un nombre:
        return element->valeur_reel();
    case VALNOEUD:
        vnod=(TestFormuleElementNoeud*)element;
        if (vnod->tf->Type()==TESTFONCTIONPROCEDURE ||
			vnod->tf->Type()==TESTFONCTIONKIF ||
            vnod->tf->Type()==TESTFONCTIONPYTHON)
            return vnod->tf->TraitementFormule(parseur,noeuds,erreur,focus);


        lemme="";
        if (parseur->RecuperationTraitValeur(vnod->tf,lemme,noeuds,focus)==OUI)
            return atof(STR(lemme));
        return 0;
        break;
    case VNOEUD:
        //c'est une variable
        v=element->valeur_variable();
        n = noeuds[v->indice];
        if (n==NULL) {
            erreur=OUI;
            parseur->erreur_survenue=8;
            sprintf_s(parseur->ajoute_info_erreur,4096," node:%d",v->indice);
            return NON;
        }

        ind = v->var->AjouteNoeud(n);
        i=v->var->position;

        //Dans le cas ou la variable n'a jamais ete encore utilisee
        //On lui met sa valeur par defaut
        if (n->source->variables[i]==NULL) {
            if (v->var->defaut!=NULL)
                res=v->var->defaut->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            v->var->valeurs.affecte(ind,res);
            n->source->variables.affecte(i,v->var);
        }
        v->var->utilisee=OUI;
        return v->var->valeurs[ind];
    case VLEMMEINDEX:
        //c'est une variable de chaine
        vlemme=element->valeur_lemme();
        vch = (TestFormuleElementLemme*)element;
        n = noeuds[vch->indice];
        if (n==NULL) {
            erreur=OUI;
            parseur->erreur_survenue=8;
            sprintf_s(parseur->ajoute_info_erreur,4096," node:%d",vch->indice);
            return NON;
        }

        //Il s'agit d'une concatenation du nom de la variable avec le lemme
        //sprintf((char*)lemme,"%s%s#",vlemme->nom,n->X->nom);
        lemme=vlemme->nom;
        pos=lemme.size();
        //On concatene les lemmes dans cette chaine
        n->Lemme(lemme);
        ki=pos;
        while (ki<lemme.size() && lemme[ki]==' ') ki++;
		if (ki>=lemme.size())
			return NON;

        varglob =vlemme->recherche(USTR(lemme)+ki,pos);
        if (varglob==NULL || varglob->utilisee==NON) {
            if (varglob!=NULL && varglob->defaut!=NULL)
                return varglob->defaut->TraitementFormule(parseur,noeuds,erreur,focus);
            if (vlemme->defaut!=NULL)
                return vlemme->defaut->TraitementFormule(parseur,noeuds,erreur,focus);
            return 0;
        }
        return varglob->valeur;

    case VDEPENDANCE:
        dep=element->valeur_dependance();
        rf=focus->cell(dep->indice);
        if (rf==NULL) {
            erreur=OUI;
            parseur->erreur_survenue=9;
            sprintf_s(parseur->ajoute_info_erreur,4096," dependency:%d",dep->indice-1);
            return NON;
        }
        ind=dep->var->AjouteDependance(rf);
        i=dep->var->position;

        if (rf->vardep[i]==NULL) {
            if (dep->var->defaut!=NULL)
                res=dep->var->defaut->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            rf->vardep.affecte(i,dep->var);
            dep->var->valeurs.affecte(ind,res);
        }
        dep->var->utilisee=OUI;
        return dep->var->valeurs[ind];
    case VGLOBALEREINIT:
    case VGLOBALE:
        varglob=element->valeur_global();
        //On instancie les variables...
        if (varglob->utilisee==NON) {
            if (varglob->defaut!=NULL)
                res=varglob->defaut->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            varglob->valeur=res;
        }
        varglob->utilisee=OUI;
        return varglob->valeur;
    case VVECTEURINT:
        ivect=(TestFormuleElementIntVector*)element;
        //dans ce cas on recupere la taille de la chaine
        if (ivect->indice_vecteur==NULL)
            return ivect->var->valeurs.dernier;

        //On va chercher l'indice
        id=(int)ivect->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;
        //On affecte notre valeur
        if (id<ivect->var->valeurs.dernier)
            return ivect->var->valeurs[id];
        return -1;
    case VCHAINEDEPENDANCE:
    case VALCHAINE:
    case VCHAINENOEUD:
    case VCHAINE:
    case VDICTIONNAIRE:
    case VVECTEURCHAINE:
		TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
		res=atof(buffbis.c_str());
        return res;
	case VPYTHONOBJET:
		return 1;
    }

    if (element->type!=VOPERATEUR) {
        erreur=OUI;
        parseur->erreur_survenue=10;
        return NON;
    }

    //Sinon c'est un  operateur
    int operateur=element->valeur_operation();
    switch (operateur) {
        //= (affectation)
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        //On recupere notre variable avec initialisation au besoin
        v=NULL;
        dep=NULL;
        rf=NULL;
        ind=-1;
        varglob=NULL;
        switch(formules[0]->element->type) {
        case VNOEUD:
            v=formules[0]->element->valeur_variable();
            //l'indice correspond au noeud qui nous interesse:
            n = noeuds[v->indice];
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=8;
                sprintf_s(parseur->ajoute_info_erreur,4096," node:%d",v->indice);
                return NON;
            }
            ind=v->var->AjouteNoeud(n);
            //resinter=v->var->valeurs[ind];
            resinter=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            break;
        case VDEPENDANCE:
            dep=formules[0]->element->valeur_dependance();
            rf=focus->cell(dep->indice);
            if (rf==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=9;
                sprintf_s(parseur->ajoute_info_erreur,4096," dependency:%d",dep->indice-1);
                return NON;
            }
            ind=dep->var->AjouteDependance(rf);
            resinter=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            //resinter=dep->var->valeurs[ind];
            break;
        case VCHAINEDEPENDANCE:
            depch=formules[0]->element->valeur_dependance_chaine();
            rf=focus->cell(depch->indice);
            if (rf==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=9;
                sprintf_s(parseur->ajoute_info_erreur,4096," dependency:%d",dep->indice-1);
                return NON;
            }
            ind=depch->var->AjouteDependance(rf);
            resinter=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            //resinter=dep->var->valeurs[ind];
            break;
#ifdef XIPLIBXML
		case VXMLOBJET:
			variab=formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,RENVOIEVARIABLE);
			vxml=(VariableXML*)variab;
			variab=formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,RENVOIEVARIABLE);
			if (variab==NULL || variab->sorte!=VXMLOBJET)
				return NON;
			vxmlbis=(VariableXML*)variab;
			switch(operateur) {
				case 0://affectation
					vxml->noeud=vxmlbis->noeud;
					return OUI;
				case 1://comparaison
					if (vxml->noeud==vxmlbis->noeud)
						return OUI;
					return NON;
			}
			return NON;
#endif
        case VGLOBALE:
            //Il s'agit d'une variable globale
            varglob=formules[0]->element->valeur_global();
            resinter=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            //resinter=varglob->valeur;
            break;
        case VLEMMEINDEX:
            //c'est une variable de chaine
            vlemme=formules[0]->element->valeur_lemme();
            vch = (TestFormuleElementLemme*)formules[0]->element;
            n = noeuds[vch->indice];
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=8;
                sprintf_s(parseur->ajoute_info_erreur,4096," node:%d",vch->indice);
                return NON;
            }
            //Il s'agit d'une concatenation du nom de la variable avec le lemme
            //sprintf((char*)lemme,"%s%s#",vlemme->nom,n->X->nom);
            lemme=vlemme->nom;
            pos=lemme.size();
            //On concatene les lemmes dans cette chaine
            n->Lemme(lemme);
            //Il peut y avoir des blancs en debut de lemme
            ki=pos;
            while (ki<lemme.size() && lemme[ki]==' ') ki++;
			if (ki>=lemme.size())
				return NON;

            varglob =vlemme->recherche(USTR(lemme)+ki,pos);
            //si cette variable n'existe pas, on utilise la valeur par defaut
            if (varglob==NULL || varglob->utilisee==NON) {
                resinter=0;
                if (varglob!=NULL && varglob->defaut!=NULL)
                    resinter=varglob->defaut->TraitementFormule(parseur,noeuds,erreur,focus);
                if (erreur==OUI)
                    return NON;

                if (vlemme->defaut!=NULL)
                    resinter=vlemme->defaut->TraitementFormule(parseur,noeuds,erreur,focus);
            }
            else
                resinter=varglob->valeur;
            break;
        case VVECTEURINT:
            ivect=(TestFormuleElementIntVector*)formules[0]->element;
            id=-1;
            //dans ce cas on recupere la taille de la chaine
            if (ivect->indice_vecteur==NULL)
                resinter=ivect->var->valeurs.dernier;
            else {
                //On va chercher l'indice
                id=(int)ivect->indice_vecteur->TraitementFormule(parseur,noeuds,erreur,focus);
                if (erreur==OUI)
                    return NON;
                //On affecte notre valeur
                resinter=-1;
                if (id<ivect->var->valeurs.dernier)
                    resinter=ivect->var->valeurs[id];
                else {
                    if (operateur!=0) {
                        erreur=OUI;
                        parseur->erreur_survenue=36;
                        sprintf_s(parseur->ajoute_info_erreur,4096," %s",ivect->var->nom);
                        return NON;
                    }
                }
            }
            break;
        default:
            resinter=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
        }

        if (erreur==OUI)
            return NON;

        //on execute alors la formule associee
        //resinter contient la valeur initiale de notre variable
        res=formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;
        switch(operateur) {
        case 0: {// affectation
            switch(formules[0]->element->type) {
            case VNOEUD:
                //Nous rangeons la valeur au bon endroit, soit n->source->position
                v->var->valeurs.affecte(ind,res);
                v->var->utilisee=OUI;
                //Et nous enregistrons le fait que ce noeud est associe a cette variable
                n->source->variables.affecte(v->var->position,v->var);
                break;
            case VDEPENDANCE:
                dep->var->valeurs.affecte(ind,res);
                dep->var->utilisee=OUI;
                rf->vardep.affecte(dep->var->position,dep->var);
                break;
            case VLEMMEINDEX:
                //dans le cas de la creation d'une nouvelle variable, le
                //lemme a deja ete identifiee
                vlemme->utilisee=OUI;
				majvariablelemme(vlemme,n,USTR(lemme),pos,"",res);
                break;
            case VVECTEURINT:
                if (id==-1)
                    //c'est une raz
                    ivect->var->raz();
                else
                    ivect->var->Affecte(id,res);
                break;
            default:
                if (varglob!=NULL) {
                    varglob->utilisee=OUI;
                    varglob->valeur=res;
                }
            }
            return OUI;
        }
        case 1: //: comparaison
            if (resinter == res)
                return OUI;
            else
                return NON;
        case 2: //< inferieur
            if (resinter < res)
                return OUI;
            else
                return NON;
        case 3: //> superieur
            if (resinter > res)
                return OUI;
            else
                return NON;
        case 4://<= inferieur ou egal
            if (resinter <= res)
                return OUI;
            else
                return NON;
        case 5://>= superieur ou egal
            if (resinter >= res)
                return OUI;
            else
                return NON;
        case 6: // <> different
            if (resinter != res)
                return OUI;
            else
                return NON;
        }
        case 7: //+,*,- addition, multiplication, soustraction
        case 8:
        case 9:
            res=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            for (i=1;i<formules.dernier;i++) {
                resinter=formules[i]->TraitementFormule(parseur,noeuds,erreur,focus);
                if (erreur==OUI)
                    return NON;
                switch (operateur) {
                case 7: //+
                    res+=resinter;
                    break;
                case 8: //*
                    res*=resinter;
                    break;
                case 9: //-
                    res-=resinter;
                }
            }
            return res;
        case 10:
        case 11:
        case 12: {// division, modulo, puissance
            res = formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;

            if (formules[1]==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=11;
                return NON;
            }

            resinter = formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;

            switch (operateur) {
            case 10: // / division
                if (resinter==0) {
                    erreur=OUI;
                    parseur->erreur_survenue=12;
                    return NON;
                }
                res = res / resinter;
                break;
            case 11: // modulo
                if (resinter==0) {
                    erreur=OUI;
                    parseur->erreur_survenue=12;
                    return NON;
                }
                res = (double)((int)res % (int)resinter);
                break;
            case 12: // ^ puissance
                res = pow(res,resinter);
            }
            return res;
                 }
        case 13: //log
            res = formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            return log10(res);
        case 14: //ln
            res = formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            return log(res);
        case 15: //exp
            res = formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            return exp(res);
        case 16: //sqrt
            res = formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            return exp(res);
        case 17:
            res = formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            return tan(res);
        case 18:
            res = formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            return sin(res);
        case 19:
            res = formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            return cos(res);
        case 20:
            res = formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            return atan(res);
        case 21://@pi
            return M_PI;
        case 22://@e
            return M_E;
        case 23: //length of a string
            formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
            if (erreur==OUI)
                return -1;
            return size_c(buffbis);
        case 24: //left of a string
        case 25: //right of a string
        case 26: //mid of a string
        case 55: //lowercase
        case 56: //uppercase
        case 28: //trim
        case 62: //removefirst
        case 63: //removelast
        case 64: //replace
        case 77: //read
            res=TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
            if (erreur==OUI)
                return NON;
            return atof(buffbis.c_str());
        case 27: //regexp
        case 29: //regexpsplit
        case 32: //search
        case 37: //regexpstart
        case 48: //lemmas
        case 79: //regexplast
        case 88: //backsearch
        case 89: //regexpback
            erreur=2;
            res=TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
            if (erreur==OUI)
                return -1;
            return res;
        case 38: //regexpfull
            res=TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
            if (erreur==OUI)
                return -1;
            return res;
	  case 124://block
	  case 125://free
	  case 130://freefull
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=93;
			  return NON;
		  }

		  //Le parametre est notre label de saut
		  formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
		  if (erreur==OUI)
			  return NON;
		  if (operateur==124)
			  return parseur->desactiveEspaceRegle(STR(buffbis));
		  if (operateur==125)
			  return parseur->reactiveEspaceRegle(STR(buffbis),NON);
		  return parseur->reactiveEspaceRegle(STR(buffbis),OUI);
	  case 126://blocked
	  case 127://rulespace
	  case 141://freed
            res=-1;
            if (formules.dernier!=1) {
                erreur=OUI;
                parseur->erreur_survenue=94;
                return -1;
            }

            //le premier parametre doit etre une variable vecteur
			variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
			if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
				erreur=OUI;
				parseur->erreur_survenue=94;
				return -1;
			}

			varvector=(VariableVector*)variab;
			varvector->raz();
			ind=0;
			if (operateur==126)
				//On range alors tous les lemmes dans vvect
				recuperechainerulespace(parseur->spacename,varvector,ind,NON);
			else
				if (operateur==127)
					recuperechainerulespace(parseur->spacename,varvector,ind,2);
				else
					recuperechainerulespace(parseur->spacename,varvector,ind,OUI);
            return ind;
		case 140://apply
			//On desactive l'ensemble des regles, a partir de la racine
			if (parseur->spacename==NULL)
				return NON;
			parseur->activation(parseur->spacename,NON);
			parseur->spacename->actif=OUI;
			//Puis on active pas a pas
			for (ind=0;ind<formules.dernier;ind++) {
				buffbis="";
				formules[ind]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
				if (erreur==OUI)
					return NON;
				parseur->reactiveEspaceRegle(STR(buffbis),NON);
			}
			return ind;
        case 33://last_error
            return parseur->derniere_erreur_survenue;
		case 187://Retourne 1, si on est en utf8
			return parseur->TestOptions(ENTREE_UTF8);
        case 35://insert
            res=-1;
            if (formules.dernier!=3) {
                erreur=OUI;
                parseur->erreur_survenue=16;
                return -1;
            }

            //le premier parametre doit etre une variable vecteur
            variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
            if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
                erreur=OUI;
                parseur->erreur_survenue=17;
                return -1;
            }

            varvector=(VariableVector*)variab;

            //Le second parametre est notre chaine d'entree
            formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
            if (erreur==OUI)
                return NON;

            //Le troisieme parametre est l'index
            ind=(int)formules[2]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            varvector->inserechaine(buffbis,ind);
            return OUI;
        case 42: //addstringunique
        case 36://addstring
            res=-1;
            if (formules.dernier!=2) {
                erreur=OUI;
                parseur->erreur_survenue=18;
                return -1;
            }

            //le premier parametre doit etre une variable vecteur
            variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
            if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
                erreur=OUI;
                parseur->erreur_survenue=21;
                return -1;
            }

            varvector=(VariableVector*)variab;
            //Le second parametre est notre chaine d'entree
            formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
            if (erreur==OUI)
                return NON;

            if (operateur==42) {//addstringunique
                //si la chaine existe deja, on renvoie sa position, mais on ne l'ajoute pas
                ind=varvector->recherchepremier(buffbis);
                if (ind!=-1)
                    return ind;
            }

            ind=varvector->chaine.dernier;
            varvector->AffecteValeurStringIndex(new string(buffbis),ind);
            return ind;
        case 31://insertvalue
            res=-1;
            if (formules.dernier!=3) {
                erreur=OUI;
                parseur->erreur_survenue=64;
                return -1;
            }

            //le premier parametre doit etre une variable vecteur
            variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
            if (variab==NULL || variab->sorte!=VVECTEURINT)  {
                erreur=OUI;
                parseur->erreur_survenue=21;
                return -1;
            }

            varintvector=(VariableIntVector*)variab;


            //Le second parametre est notre valeur d'entree
            res=formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;

            //Le troisieme parametre est notre index
            ind=formules[2]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            varintvector->valeurs.insertion(ind,res);
            return ind;
        case 57://addvalue
        case 58://addvalueunique
            res=-1;
            if (formules.dernier!=2) {
                erreur=OUI;
                parseur->erreur_survenue=18;
                return -1;
            }

            //le premier parametre doit etre une variable vecteur
            variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
            if (variab==NULL || variab->sorte!=VVECTEURINT)  {
                erreur=OUI;
                parseur->erreur_survenue=21;
                return -1;
            }

            varintvector=(VariableIntVector*)variab;


            //Le second parametre est notre valeur d'entree
            res=formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;

            if (operateur==58) {//addvalueunique
                //si la valeur existe deja, on renvoie sa position, mais on ne l'ajoute pas
                ind=varintvector->recherchepremier(res);
                if (ind!=-1)
                    return ind;
            }

            ind=varintvector->valeurs.dernier;
            varintvector->Affecte(ind,res);
            return ind;
        case 39://removestring
            if (formules.dernier!=2) {
                erreur=OUI;
                parseur->erreur_survenue=19;
                return -1;
            }

            //le premier parametre doit etre une variable vecteur
            variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
            if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
                erreur=OUI;
                parseur->erreur_survenue=20;
                return -1;
            }

            varvector=(VariableVector*)variab;

            //Le deuxieme parametre est l'index de l'element a retirer
            ind=(int)formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            return varvector->retirechaine(ind);
        case 59://removevalue
            if (formules.dernier!=2) {
                erreur=OUI;
                parseur->erreur_survenue=19;
                return -1;
            }

            //le premier parametre doit etre une variable vecteur
            variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
            if (variab==NULL || variab->sorte!=VVECTEURINT)  {
                erreur=OUI;
                parseur->erreur_survenue=20;
                return -1;
            }

            varintvector=(VariableIntVector*)variab;

            //Le deuxieme parametre est l'index de l'element a retirer
            ind=(int)formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;
            return varintvector->retirevaleur(ind);
        case 40://goto
            if (formules.dernier!=1) {
                erreur=OUI;
                parseur->erreur_survenue=22;
                return NON;
            }

            //Le parametre est notre label de saut
            formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
            if (erreur==OUI)
                return NON;
            //alors dans stack nous avons stocke le label vers lequel sauter
			l = parseur->rechercheLabel((char*)buffbis.c_str());
            //ce label ne correspond a rien, on le saute
            if (l==NULL) {
                if (parseur->TRACE)
                    trace<<"Unknown label:"<<buffbis<<Endl;
                erreur=OUI;
                return NON;
            }
            //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
            //commencer a reappliquer nos regles
            parseur->demarre_regle=l->indexregle;
            parseur->demarre_couche=l->couche;
            parseur->aller_label=l;
            return OUI;
        case 41://exit
            l=parseur->label_terminal;
            //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
            //commencer a reappliquer nos regles
            parseur->demarre_regle=l->indexregle;
            parseur->demarre_couche=l->couche;
            parseur->aller_label=l;
            return OUI;
        case 70://stop
            l=parseur->label_stop;
            //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
            //commencer a reappliquer nos regles
            parseur->demarre_regle=l->indexregle;
            parseur->demarre_couche=l->couche;
            parseur->aller_label=l;
            return OUI;
        case 43: //return
            if (formules.dernier!=1) {
                erreur=OUI;
                parseur->erreur_survenue=22;
                return NON;
            }

            //Le parametre est notre label de saut
            res=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (erreur==OUI)
                return NON;

            l=parseur->label_retour;
            //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
            //commencer a reappliquer nos regles
            parseur->valeur_retour=res;
            parseur->aller_label=l;
            return res;
        case 45://offset(noeud,gauche,droit)
		case 122://tokennumber(noeud,gauche,droit)
		case 123://offsetchar(noeud,gauche,droit)
            if (formules.dernier!=3) {
                erreur=OUI;
                parseur->erreur_survenue=24;
                return -1;
            }
            n= noeuds[formules[0]->element->argument(parseur,noeuds)];
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=25;
                return -1;
            }
			switch(operateur) {
			case 45://offset
				AffecteNombre(parseur,formules[1],n->gauche,noeuds);
				AffecteNombre(parseur,formules[2],n->droite,noeuds);
				break;
			case 122://tokennumber
				AffecteNombre(parseur,formules[1],n->num_gauche,noeuds);
				AffecteNombre(parseur,formules[2],n->num_droite,noeuds);
				break;
			case 123://offsetchar
				AffecteNombre(parseur,formules[1],n->gauchecar,noeuds);
				AffecteNombre(parseur,formules[2],n->droitecar,noeuds);
				break;
			}
            return 1;
        case 46://nodenumber
            if (formules.dernier!=1) {
                erreur=OUI;
                parseur->erreur_survenue=24;
                return -1;
            }
            n= noeuds[formules[0]->element->argument(parseur,noeuds)];
            if (n==NULL){
                erreur=OUI;
                parseur->erreur_survenue=25;
                return -1;
            }
            return n->num_noeud;
        case 47: //@sentencenumber
            return parseur->num_phrase;
        case 49: //node(#1,num)
            //this method assigns a node according to its number
            if (formules.dernier!=2) {
                erreur=OUI;
                parseur->erreur_survenue=30;
                return 0;
            }

            //La deuxieme variable est un index
            ind=(int)formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
            if (ind<0 || ind>= parseur->dernierNumeroNoeud) {
                erreur=OUI;
                parseur->erreur_survenue=32;
                return 0;
            }

            //On recupere notre argument
            id=formules[0]->element->arg();
            if (id==-1) {
                erreur=OUI;
                parseur->erreur_survenue=31;
                return 0;
            }
            n=NULL;
            if (parseur->NoeudsTemporaires[ind]!=NULL)
                n=&parseur->NoeudsTemporaires[ind]->N;
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=32;
                return 0;
            }
            if (noeuds[id]==NULL)
                noeuds.affecte(id,n);
            else
                if (noeuds[id]!=n) {
                    erreur=OUI;
                    parseur->erreur_survenue=33;
                    return 0;
                }

            return 1;
    case 60: //searchvalue
        if (formules.dernier<2 || formules.dernier>3) {
            erreur=OUI;
            parseur->erreur_survenue=6;
            return -1;
        }

        //le premier parametre doit etre une variable vecteur
        variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VVECTEURINT)  {
            erreur=OUI;
            parseur->erreur_survenue=7;
            return -1;
        }

        varintvector=(VariableIntVector*)variab;

        //Si le troisieme parametre est fourni, on garde une trace dans un vecteur
        varintvectorbis=NULL;
        if (formules[2]!=NULL) {
            variab= formules[2]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
            if (variab==NULL || variab->sorte!=VVECTEURINT)  {
                erreur=OUI;
                parseur->erreur_survenue=23;
                return -1;
            }

            varintvectorbis=(VariableIntVector*)variab;
        }

        //Le second parametre est notre valeur a rechercher
        res=formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;

        //si l'on a un troisieme parametre, on y stocke toutes les valeurs
        if (varintvectorbis!=NULL) {
            id=varintvector->recherchetous(res,varintvectorbis);
            if (id==0)
                return -1;
        }
        else
            id=varintvector->recherchepremier(res);
        return id;
    case 50://next
    case 51://previous
    case 52://mother
    case 53://daughter
    case 54://last
    case 87://descendant
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=34;
            return 0;
        }
        //On recupere notre argument
        id=formules[0]->element->argument(parseur,noeuds);
        if (id==-1) {
            erreur=OUI;
            parseur->erreur_survenue=35;
            return 0;
        }
        n=noeuds[id];
        if (n==NULL) {
            erreur=OUI;
            parseur->erreur_survenue=35;
            return 0;
        }
        ind=formules[1]->element->arg();
        if (ind==-1) {
            erreur=OUI;
            parseur->erreur_survenue=35;
            return 0;
        }
        n2=noeuds[ind];
        switch(operateur) {
        case 50:
            n=n->frere;
            break;
        case 51:
            n=n->precedent;
            break;
        case 52:
            n=n->pere;
            break;
        case 53:
            n=n->fils;
            break;
        case 54:
            if (n->fils!=NULL)
                n=n->fils->dernier;
            else
                n=NULL;
            break;
        case 87:
            //Il faut que les deux noeuds existent
            if (n2==NULL)
                return 0;
            //n est le noeud fille, n2 le noeud mere
            while (n!=NULL && n!=n2) n=n->pere;
        }
        //echec dans la recherche du noeud
        if (n==NULL)
            return 0;

        if (n2!=NULL) {
            if (n2==n) {
                //On teste alors les traits
                if (formules[1]->element->argument(parseur,noeuds)==-1)
                    return 0;
                return 1;
            }
            else
                return 0;
        }
		noeuds.affecte(ind,n);
		//On teste alors les traits
		if (formules[1]->element->argument(parseur,noeuds)==-1) {
			//on retire le noeud
			noeuds.affecte(ind,NULL);
			return 0;
		}
        return 1;
    case 152://nodedescendant($#1,$#2)
    case 153://nodenext($#1,$#2)
    case 154://nodeprevious($#1,$#2)
    case 155://nodemother($#1,$#2)
    case 156://nodedaughter($#1,$#2)
	case 159://nodelast($#1,$#2)
	case 160://nodenexttoken($#1,$#2)
	case 161://nodeprevioustoken($#1,$#2)
	case 162://nodefirsttoken($#1,$#2)
	case 163://nodelasttoken($#1,$#2)
		if (formules.dernier!=2) {
			erreur=OUI;
			parseur->erreur_survenue=149;
			return 0;
		}

		if (formules[0]->element->type!=VALNOEUD) {
			erreur=OUI;
			parseur->erreur_survenue=150;
			return 0;
		}

		tf=((TestFormuleElementNoeud*)formules[0]->element)->tf;
		if (tf->Type()!=TESTFONCTIONDEPENDANCE) {
			erreur=OUI;
			parseur->erreur_survenue=150;
			return 0;
		}
		//Sinon, il s'agit d'une dependance
		res=0;
		id=tf->IndexDependance();
		if (id!=-1 && focus!=NULL && (*focus)[id]!=NULL) {
			rf1=(*focus)[id];
			if (rf1->Fx->type!=C_GENERATION) {
				erreur=OUI;
				parseur->erreur_survenue=150;
				return 0;
			}
		}
		else
			return 0;

		if (formules[1]->element->type!=VALNOEUD) {
			erreur=OUI;
			parseur->erreur_survenue=150;
			return 0;
		}

		tf=((TestFormuleElementNoeud*)formules[1]->element)->tf;
		if (tf->Type()!=TESTFONCTIONDEPENDANCE) {
			erreur=OUI;
			parseur->erreur_survenue=150;
			return 0;
		}
		//Deuxieme argument
		rf2=NULL;
		id=tf->IndexDependance();
		if (id!=-1 && focus!=NULL && (*focus)[id]!=NULL) {
			rf2=(*focus)[id];
			if (rf2->Fx->type!=C_GENERATION) {
				erreur=OUI;
				parseur->erreur_survenue=150;
				return 0;
			}
		}

		//Nous verifions si rf1 est un descendant de rf2
		switch(operateur) {
		case 152://descendant
			return parentde(rf1,rf2);
		case 153://next
			rf2=suivantde(rf1,rf2,1);
			if (rf2==NULL)
				return NON;
			break;
		case 154://previous
			rf2=suivantde(rf1,rf2,-1);
			if (rf2==NULL)
				return NON;
			break;
		case 155://mother
			rf2=perede(rf1,rf2);
			if (rf2==NULL)
				return NON;
			break;
		case 156://daughter
			rf2=filsde(rf1,rf2,NON);
			if (rf2==NULL)
				return NON;
			break;
		case 159://last
			rf2=filsde(rf1,rf2,OUI);
			if (rf2==NULL)
				return NON;
			break;
		case 160://nexttoken
			rf2=tokensuivant(parseur,rf1,rf2,1);
			if (rf2==NULL)
				return NON;
			break;
		case 161://previoustoken
			rf2=tokensuivant(parseur,rf1,rf2,-1);
			if (rf2==NULL)
				return NON;
			break;
		case 162://firsttoken
			rf2=parseur->tokendufond(rf1,rf2,NON);
			if (rf2==NULL)
				return NON;
			break;
		case 163://lasttoken
			rf2=parseur->tokendufond(rf1,rf2,OUI);
			if (rf2==NULL)
				return NON;
			break;
		}
		focus->affecte(id,rf2);
		return OUI;
	 case 164://istoken
		if (formules.dernier!=1) {
			erreur=OUI;
			parseur->erreur_survenue=155;
			return 0;
		}

		if (formules[0]->element->type!=VALNOEUD) {
			erreur=OUI;
			parseur->erreur_survenue=156;
			return 0;
		}

		tf=((TestFormuleElementNoeud*)formules[0]->element)->tf;
		if (tf->Type()!=TESTFONCTIONDEPENDANCE) {
			erreur=OUI;
			parseur->erreur_survenue=156;
			return 0;
		}
		//Sinon, il s'agit d'une dependance
		res=0;
		id=tf->IndexDependance();
		if (id!=-1 && focus!=NULL && (*focus)[id]!=NULL) {
			rf1=(*focus)[id];
			if (rf1->Fx->type!=C_GENERATION || rf1->frere==NULL) {
				erreur=OUI;
				parseur->erreur_survenue=156;
				return 0;
			}
			if (rf1->frere->enfants.dernier==0)
				return 1;
		}

		return 0;
     case 61: //pause
         time(&tempsinitial);
         res=10;
         if (formules.dernier==1)
             //Le second parametre est notre valeur a rechercher
             res=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
         time(&tempsfinal);
         car=8;
         while (difftime(tempsfinal,tempsinitial)<res) {
             cout<<"/"<<car<<"/"<<car<<"/"<<car;
             cout<<"-"<<car<<"-"<<car<<"-"<<car;
             cout<<"\\"<<car<<"\\"<<car<<"\\"<<car;
             time(&tempsfinal);
         }

         return OUI;
     case 66: //cleanlemmas cette instruction detruit les lemmes associes a une variable
        if (formules.dernier!=1) {
            erreur=OUI;
            parseur->erreur_survenue=44;
            return 0;
        }

        if (formules[0]->element->type!=VLEMMEINDEX) {
            erreur=OUI;
            parseur->erreur_survenue=44;
            return 0;
        }
        vch = (TestFormuleElementLemme*)formules[0]->element;
        vch->var->listes.nettoie();
		vch->var->lemmes.clear();
        return OUI;
     case 67://cleanalllemmas
         for (i=0;i<parseur->VariablesLemmes.dernier;i++) {
             parseur->VariablesLemmes[i]->lemmes.clear();
             parseur->VariablesLemmes[i]->listes.nettoie();
         }
         return OUI;
	 case 144://cleanall
         for (i=0;i<parseur->Variables.dernier;i++)
             parseur->Variables[i]->raz();
         return OUI;
     case 68://retrievelemmas, on place dans deux tableaux lemmes et valeurs numeriques
        if (formules.dernier!=3) {
            erreur=OUI;
            parseur->erreur_survenue=45;
            return -1;
        }
        //On recupere notre variable de chaine
        if (formules[0]->element->type!=VLEMMEINDEX) {
            erreur=OUI;
            parseur->erreur_survenue=46;
            return -1;
        }
        vch = (TestFormuleElementLemme*)formules[0]->element;
        //le deuxieme parametre doit etre une variable vecteur de chaine
        variab= formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
            erreur=OUI;
            parseur->erreur_survenue=47;
            return -1;
        }
        varvector=(VariableVector*)variab;


        //Le troisieme parametre doit etre une variable vecteur de int
        variab= formules[2]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
        if (variab==NULL || variab->sorte!=VVECTEURINT)  {
            erreur=OUI;
            parseur->erreur_survenue=48;
            return -1;
        }
        varintvector=(VariableIntVector*)variab;

        varvector->raz();
        varintvector->raz();
        id=strlen(vch->var->nom);
        for (i=0;i<vch->var->listes.dernier;i++) {
            s=new string(vch->var->listes[i]->nom+id);
            varvector->chaine.ajoute(s);
            varintvector->valeurs.ajoute(((VariableGlobale*)vch->var->listes[i])->valeur);
        }
        return varintvector->valeurs.dernier;
     case 69: //GetLemma
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=49;
            return 0;
        }
        //On recupere notre variable de chaine
        if (formules[0]->element->type!=VLEMMEINDEX) {
            erreur=OUI;
            parseur->erreur_survenue=50;
            return -1;
        }
        vch = (TestFormuleElementLemme*)formules[0]->element;
        //On recupere le lemme
        formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
        varglob =vch->var->recherche((uchar*)buffbis.c_str(),0);
        if (varglob==NULL)
            return 0;
        return varglob->valeur;
     case 65://copyfeature, cette instruction permet d'ecrire des traits d'une structure sur une autre
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=41;
            return 0;
        }

        if (formules[0]->element->type!=VALNOEUD) {
            erreur=OUI;
            parseur->erreur_survenue=42;
            return 0;
        }
        if (formules[1]->element->type!=VALNOEUD) {
            erreur=OUI;
            parseur->erreur_survenue=43;
            return 0;
        }

        vnod=(TestFormuleElementNoeud*)formules[0]->element;
        tf=vnod->tf;
        //le premier parametre est forcement un #1 ou un $1
        //Le second parametre aussi
        if (tf->Type()==TESTFONCTIONNOEUD) {
            id=formules[0]->element->argument(parseur,noeuds);
            if (id==-1) {
                erreur=OUI;
                parseur->erreur_survenue=41;
                return 0;
            }
            n= noeuds[id];
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=41;
                return 0;
            }
            trbase=n->source->traitsbase;
        }
        else {
            //Sinon, il s'agit d'une dependance
            ind=tf->IndexDependance();
            if (ind==-1 || focus==NULL || (*focus)[ind]==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=41;
                return NON;
            }
            //On recupere notre fonction
            rf=(*focus)[ind];
            trbase=rf->traits;
        }
        vnod=(TestFormuleElementNoeud*)formules[1]->element;
        tf=vnod->tf;
		copie=new Traits(parseur);
        //le premier parametre est forcement un #1 ou un $1
        //Le second parametre aussi
        if (tf->Type()==TESTFONCTIONNOEUD) {
            id=formules[1]->element->argument(parseur,noeuds);
            if (id==-1) {
                erreur=OUI;
                parseur->erreur_survenue=42;
                return 0;
            }
            n= noeuds[id];
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=42;
                return 0;
            }
            copie->valeur->ou(n->source->traitsbase->valeur);
            copie->filtre->ou(n->source->traitsbase->filtre);
            if (copie->libre==NULL)
                copie->libre=new Vecteur(parseur->iTailleVect);
            else
                copie->libre->raz();

            tr=((TestFonctionNoeud*)tf)->traits[0];
            if (tr->present!=NULL)
                copie->libre->ou(tr->present);
            else
                copie->libre->ou(tr->lpresent);
        }
        else {
            //Sinon, il s'agit d'une dependance
            ind=tf->IndexDependance();
            if (ind==-1 || focus==NULL || (*focus)[ind]==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=42;
                return NON;
            }
            //On recupere notre fonction
            rf=(*focus)[ind];
            copie->valeur->ou(rf->traits->valeur);
            copie->filtre->ou(rf->traits->filtre);
            if (copie->libre==NULL)
                copie->libre=new Vecteur(parseur->iTailleVect);
            else
                copie->libre->raz();

            tr=tf->LesTraits();
            if (tr->present!=NULL)
                copie->libre->ou(tr->present);
            else
                copie->libre->ou(tr->lpresent);
        }
        //sinon, on recopie nos traits dans trbase
        if (trbase->libre!=NULL) {
			vecteur=new Vecteur(trbase->libre);
            car=parseur->PTT(*trbase,copie);
            trbase->libre->recopie(vecteur);
			delete vecteur;
        }
        else {
            car=parseur->PTT(*trbase,copie);
            if (trbase->libre!=NULL) {
                delete trbase->libre;
                trbase->libre=NULL;
            }
        }
		delete copie;
        return car;
     case 149://initlookdown met a jour les structures de traits a l'issue d'un lookdown
        if (formules.dernier!=1 && formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=137;
            return 0;
        }

        if (formules[0]->element->type!=VALNOEUD) {
            erreur=OUI;
            parseur->erreur_survenue=138;
            return 0;
        }

        vnod=(TestFormuleElementNoeud*)formules[0]->element;
        tf=vnod->tf;
        //le premier parametre est forcement un #1 ou un $1
        //Le second parametre aussi
        if (tf->Type()==TESTFONCTIONNOEUD) {
            id=formules[0]->element->argument(parseur,noeuds);
            if (id==-1) {
                erreur=OUI;
                parseur->erreur_survenue=137;
                return 0;
            }
            n= noeuds[id];
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=138;
                return 0;
            }
        }
        else {
			erreur=OUI;
			parseur->erreur_survenue=138;
			return 0;
        }
		buffbis="";
		if (formules.dernier==2) {
			//On recupere la chaine des traits
			//On recupere ensuite la chaine a introduire dans la pile
			formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
			if (erreur==OUI)
				return NON;
		}
		return parseur->initgenere(n->source,buffbis);
      case 71://fopen 2 parametre: 1) alias 2) nom de fichier
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=51;
            return 0;
        }
        //On recupere d'abord l'alias
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
        lemme=buffbis;
        //Puis le nom du fichier
        buffbis="";
        formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
        //On verifie s'il n'y pas cet alias deja en memoire
        for (i=0;i<parseur->lesfichiersscripts.dernier;i++) {
            if (lemme==parseur->lesfichiersscripts[i]->alias)
                return OUI;
        }
        fscript=new fichiersscript(STR(lemme),STR(buffbis));
        i=fscript->ouvrir();
        if (i==-1) {
            erreur=OUI;
            parseur->erreur_survenue=53;
            delete fscript;
            return 0;
        }

        parseur->lesfichiersscripts.ajoute(fscript);
        return OUI;
      case 72://fclose only one parameter
        if (formules.dernier!=1) {
            erreur=OUI;
            parseur->erreur_survenue=54;
            return 0;
        }
        //On recupere d'abord l'alias
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
        //On verifie s'il n'y pas cet alias deja en memoire
        for (i=0;i<parseur->lesfichiersscripts.dernier;i++) {
            if (buffbis==parseur->lesfichiersscripts[i]->alias)
                break;
        }
        if (i==parseur->lesfichiersscripts.dernier) {
            erreur=OUI;
            parseur->erreur_survenue=55;
            return 0;
        }
        delete parseur->lesfichiersscripts.retireElement(i);
        return OUI;
      case 73://fwrite 2 parametre: 1) alias 2) chaine a ecrire
        if (formules.dernier!=2) {
            erreur=OUI;
            parseur->erreur_survenue=51;
            return 0;
        }

        //On recupere d'abord l'alias
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
        lemme=buffbis;
        //Puis le nom du fichier
        buffbis="";
        formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
        //On verifie s'il n'y pas cet alias deja en memoire
        for (i=0;i<parseur->lesfichiersscripts.dernier;i++) {
            if (lemme==parseur->lesfichiersscripts[i]->alias)
                break;
        }
        if (i==parseur->lesfichiersscripts.dernier) {
            erreur=OUI;
            parseur->erreur_survenue=55;
            return 0;
        }
        fscript=parseur->lesfichiersscripts[i];
        *(fscript->fichier)<<(char*)buffbis.c_str();
		return OUI;
      case 74://starttimer
          time(&initialtime);
          return OUI;
      case 75://endtimer
          time(&finaltime);
          return OUI;
      case 76://elapsedtime
          res = difftime( finaltime,initialtime);
          return res;
      case 110:
          time(&intermediate);
          res = difftime(intermediate,initialtime);
          return res;
    case 91://L'aide en ligne
        if (formules.dernier!=1) {
            erreur=OUI;
            parseur->erreur_survenue=63;
            return 0;
        }
        res=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
        if (erreur==OUI)
            return NON;
        AfficheAide((int)res);
        break;
      case 85://ascii
        if (formules.dernier!=1) {
            erreur=OUI;
            parseur->erreur_survenue=59;
            return 0;
        }
        //On recupere d'abord l'alias
        formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
        car=buffbis[0];
        res=(uchar)car;
        return res;
      case 93: //@year
        time(&date_et_heure);
        temps = localtime(&date_et_heure);
        return temps->tm_year+1900;
      case 97://@day
          time(&date_et_heure);
          temps = localtime(&date_et_heure);
          return temps->tm_mday;
      case 98://@month
          time(&date_et_heure);
          temps = localtime(&date_et_heure);
          return temps->tm_mon+1;
      case 99://dependencynumber
          if (formules.dernier!=1) {
              erreur=OUI;
              parseur->erreur_survenue=66;
              return 0;
          }

          if (formules[0]->element->type!=VALNOEUD) {
              erreur=OUI;
              parseur->erreur_survenue=65;
              return 0;
          }

          tf=((TestFormuleElementNoeud*)formules[0]->element)->tf;
          if (tf->Type()!=TESTFONCTIONDEPENDANCE) {
              erreur=OUI;
              parseur->erreur_survenue=65;
              return 0;
          }
          //Sinon, il s'agit d'une dependance
          res=-1;
          ind=tf->IndexDependance();
          if (ind!=-1 && focus!=NULL && (*focus)[ind]!=NULL) {
              rf=(*focus)[ind];
              res=rf->index;
          }
          return res;
      case 100:
          if (formules.dernier!=2) {
              erreur=OUI;
              parseur->erreur_survenue=68;
              return 0;
          }
          if (formules[0]->element->type!=VALNOEUD) {
              erreur=OUI;
              parseur->erreur_survenue=67;
              return 0;
          }

          tf=((TestFormuleElementNoeud*)formules[0]->element)->tf;
          if (tf->Type()!=TESTFONCTIONDEPENDANCE) {
              erreur=OUI;
              parseur->erreur_survenue=67;
              return 0;
          }
          //Il nous faut ensuite recuperer l'index de la dependance
          ind=formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
          rf=(*parseur->INTERFONCTIONS[0])[ind];
          if (erreur==OUI)
              return NON;
          if (rf!=NULL && focus!=NULL) {
              focus->affecte(tf->IndexDependance(),rf);
              return 1;
          }
          return 0;
      case 101://nop
          return 1;
     case 102://setfeature, cette instruction permet d'ecrire un trait sous la forme de strings
        if (formules.dernier!=3) {
            erreur=OUI;
            parseur->erreur_survenue=71;
            return 0;
        }

        if (formules[0]->element->type!=VALNOEUD) {
            erreur=OUI;
            parseur->erreur_survenue=72;
            return 0;
        }
        //On recupere d'abord l'alias
        formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
        lemme=buffbis;
        //Puis le nom du fichier
        buffbis="";
        formules[2]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

        vnod=(TestFormuleElementNoeud*)formules[0]->element;
        tf=vnod->tf;
        //le premier parametre est forcement un #1 ou un $1
        if (tf->Type()==TESTFONCTIONNOEUD) {
            id=formules[0]->element->argument(parseur,noeuds);
            if (id==-1) {
                erreur=OUI;
                parseur->erreur_survenue=135;
                return 0;
            }
            n= noeuds[id];
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=135;
                return 0;
            }
            if (parseur->XipFeatureTraits(n->source->traitsbase,STR(lemme),STR(buffbis))==NON) {
              erreur=OUI;
              parseur->erreur_survenue=73;
              return 0;
            }
            parseur->XipFeatureTraits(n->source->traitsactif,STR(lemme),STR(buffbis));
        }
        else {
            //Sinon, il s'agit d'une dependance
            ind=tf->IndexDependance();
            if (ind==-1 || focus==NULL || (*focus)[ind]==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=135;
                return NON;
            }
            //On recupere notre fonction
            rf=(*focus)[ind];
            if (parseur->XipFeatureTraits(rf->traits,STR(lemme),STR(buffbis))==NON) {
              erreur=OUI;
              parseur->erreur_survenue=73;
              return 0;
            }
        }
        return 1;
     case 150://depth, cette instruction retourne la profondeur d'un NODE
        if (formules.dernier!=1) {
            erreur=OUI;
            parseur->erreur_survenue=139;
            return -1;
        }

        if (formules[0]->element->type!=VALNOEUD) {
            erreur=OUI;
            parseur->erreur_survenue=139;
            return -1;
        }

		vnod=(TestFormuleElementNoeud*)formules[0]->element;
        tf=vnod->tf;
        //le premier parametre est forcement un #1 ou un $1
        if (tf->Type()==TESTFONCTIONDEPENDANCE) {
            //il doit s'agir d'une dependance
            ind=tf->IndexDependance();
            if (ind==-1 || focus==NULL || (*focus)[ind]==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=139;
                return -1;
            }
            //On recupere notre fonction
            rf=(*focus)[ind];
			parseur->graphegeneration.profondeur(0,0);
			return rf->profondeur;
        }
		erreur=OUI;
		parseur->erreur_survenue=139;
		return -1;
	 case 157://lookup pour renvoyer la taille du tableau
		 return TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
     case 151://rank, cette instruction retourne le numero d'ordre d'un NODE
        if (formules.dernier!=1) {
            erreur=OUI;
            parseur->erreur_survenue=140;
            return -1;
        }

        if (formules[0]->element->type!=VALNOEUD) {
            erreur=OUI;
            parseur->erreur_survenue=140;
            return -1;
        }

		vnod=(TestFormuleElementNoeud*)formules[0]->element;
        tf=vnod->tf;
        //le premier parametre est forcement un #1 ou un $1
        if (tf->Type()==TESTFONCTIONDEPENDANCE) {
            //il doit s'agir d'une dependance
            ind=tf->IndexDependance();
            if (ind==-1 || focus==NULL || (*focus)[ind]==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=140;
                return -1;
            }
            //On recupere notre fonction
            rf=(*focus)[ind];
			parseur->graphegeneration.profondeur(0,0);
			return rf->numero_ordre;
        }
		erreur=OUI;
		parseur->erreur_survenue=140;
		return -1;
     case 147://setsubfeature, cette instruction permet d'ecrire un trait sous la forme de strings
        if (formules.dernier!=3) {
            erreur=OUI;
            parseur->erreur_survenue=131;
            return 0;
        }

        if (formules[0]->element->type!=VALNOEUD) {
            erreur=OUI;
            parseur->erreur_survenue=132;
            return 0;
        }
        //On recupere d'abord l'alias
        formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;
        lemme=buffbis;
        //Puis le nom du fichier
        buffbis="";
        formules[2]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
        if (erreur==OUI)
            return NON;

        vnod=(TestFormuleElementNoeud*)formules[0]->element;
        tf=vnod->tf;
        //le premier parametre est forcement un #1
        if (tf->Type()==TESTFONCTIONNOEUD) {
            id=formules[0]->element->argument(parseur,noeuds);
            if (id==-1) {
                erreur=OUI;
                parseur->erreur_survenue=134;
                return 0;
            }
            n= noeuds[id];
            if (n==NULL) {
                erreur=OUI;
                parseur->erreur_survenue=134;
                return 0;
            }

            if (parseur->XipFeatureSubTraits(n,STR(lemme),STR(buffbis))==NON) {
              erreur=OUI;
              parseur->erreur_survenue=133;
              return 0;
            }
        }
		else {
			parseur->erreur_survenue=134;
			erreur=OUI;
			return 0;
		}
        return 1;
     case 103://push($1,str)
         if (formules.dernier!=2) {
             erreur=OUI;
             parseur->erreur_survenue=74;
             return 0;
         }

         if (formules[0]->element->type!=VALNOEUD) {
             erreur=OUI;
             parseur->erreur_survenue=65;
             return 0;
         }

         tf=((TestFormuleElementNoeud*)formules[0]->element)->tf;
         if (tf->Type()!=TESTFONCTIONDEPENDANCE) {
             erreur=OUI;
             parseur->erreur_survenue=65;
             return 0;
         }

         //On recupere ensuite la chaine a introduire dans la pile
         formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
         if (erreur==OUI)
             return NON;
         //Sinon, il s'agit d'une dependance
         res=0;
         ind=tf->IndexDependance();
         if (ind!=-1 && focus!=NULL && (*focus)[ind]!=NULL) {
             rf=(*focus)[ind];
             res=1;
             rf->stack.ajoute(DupliqueChaine((char*)buffbis.c_str()));
         }
         return res;
      case 105://cleanstack($1)
          if (formules.dernier!=1) {
              erreur=OUI;
              parseur->erreur_survenue=76;
              return 0;
          }

          if (formules[0]->element->type!=VALNOEUD) {
              erreur=OUI;
              parseur->erreur_survenue=65;
              return 0;
          }

          tf=((TestFormuleElementNoeud*)formules[0]->element)->tf;
          if (tf->Type()!=TESTFONCTIONDEPENDANCE) {
              erreur=OUI;
              parseur->erreur_survenue=65;
              return 0;
          }
          //Sinon, il s'agit d'une dependance
          res=0;
          ind=tf->IndexDependance();
          if (ind!=-1 && focus!=NULL && (*focus)[ind]!=NULL) {
              rf=(*focus)[ind];
              res=1;
              for (ind=0;ind<rf->stack.dernier;ind++) {
                  chaine=rf->stack.retire();
                  if (chaine!=NULL)
                      libere(chaine);
              }
          }
          return res;
      case 106://createdependency
          if (formules.dernier<2) {
              erreur=OUI;
              parseur->erreur_survenue=170;
              return 0;
          }

		 ind=0;
		 id=-1;
		 tf=NULL;
		 if (formules[0]->element->type==VALNOEUD) {
			 ind++;
			 //C'est necessairement une dependance...
			 tf=((TestFormuleElementNoeud*)formules[0]->element)->tf;
			 if (tf->Type()!=TESTFONCTIONDEPENDANCE) {
				 erreur=OUI;
				 parseur->erreur_survenue=171;
				 return 0;
			 }
			 id=tf->IndexDependance();
		 }

		  //Premier parametre, une chaine de caractere correspondant a la dependance.
         formules[ind]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
		 ind++;
         if (erreur==OUI)
             return NON;
		 cfoncx=parseur->RetrouveFonc((char*)buffbis.c_str());
		 if (cfoncx==NULL) {
			 erreur=OUI;
			 parseur->erreur_survenue=172;
			 return NON;
		 }
		 trbase=NULL;
		 if (formules[ind]->element->type!=VALNOEUD) {
			 buffbis="";
	         formules[ind]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
			 ind++;
			 trbase=parseur->TraduitChaineTraits(buffbis);
			 if (trbase==NULL) {
				 erreur=OUI;
				 parseur->erreur_survenue=173;
				 return NON;
			 }
		 }

		  //Les autres parametres sont des noeuds
		 for (;ind<formules.dernier;ind++) {
			 if (formules[ind]->element->type!=VALNOEUD) {
				 erreur=OUI;
				 parseur->erreur_survenue=171;
				 return NON;
			 }
			 n= noeuds[formules[ind]->element->argument(parseur,noeuds)];
			 if (n==NULL) {
				 erreur=OUI;
				 parseur->erreur_survenue=171;
				 return NON;
			 }
			 lnoeuds.ajoute(n);
		  }
		  rf=parseur->CreeNouvelleDependance(cfoncx,trbase,lnoeuds);
		  if (id!=-1 && focus!=NULL)
			  focus->affecte(id,rf);
          return 1;
#ifdef XIPLIBXML
	  case 111://xmldata
          if (formules.dernier!=2) {
              erreur=OUI;
              parseur->erreur_survenue=84;
              return 0;
          }
		  variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
		  if (variab==NULL || variab->sorte!=VXMLOBJET)  {
			  erreur=OUI;
			  parseur->erreur_survenue=85;
			  return -1;
		  }
		  vxml=(VariableXML*)variab;
		  return recupere_donnees_xml(parseur,vxml->noeud,formules[1]);
	  case 182://xmlattributes(#1,vdict)
          if (formules.dernier!=2) {
              erreur=OUI;
              parseur->erreur_survenue=166;
              return 0;
          }
		  n= noeuds[formules[0]->element->argument(parseur,noeuds)];
		  if (n==NULL || n->source->voc==NULL || n->source->voc->xmlnoeud==NULL) {
			  erreur=OUI;
			  parseur->erreur_survenue=167;
			  return NON;
		  }
		  return recupere_donnees_xml(parseur,n->source->voc->xmlnoeud,formules[1]);
	  case 183://xmlget(#1,vxml)
          if (formules.dernier!=2) {
              erreur=OUI;
              parseur->erreur_survenue=168;
              return 0;
          }
		  n= noeuds[formules[0]->element->argument(parseur,noeuds)];
		  if (n==NULL || n->source->voc==NULL || n->source->voc->xmlnoeud==NULL) {
			  erreur=OUI;
			  parseur->erreur_survenue=169;
			  return NON;
		  }
		  variab= formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
		  if (variab==NULL || variab->sorte!=VXMLOBJET)  {
			  erreur=OUI;
			  parseur->erreur_survenue=88;
			  return 0;
		  }
		  vxml=(VariableXML*)variab;
		  vxml->noeud=n->source->voc->xmlnoeud;
		  return OUI;
	  case 112://xmlparent(vxml,vxmlbis)
	  case 113://xmlchild(vxml,vxmlbis)
	  case 114://xmlnext(vxml,vxmlbis)
	  case 115://xmlprevious(vxml,vxmlbis)
		  if (formules.dernier!=2) {
			  erreur=OUI;
			  parseur->erreur_survenue=87;
			  return 0;
		  }
		  variab= formules[0]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
		  if (variab==NULL || variab->sorte!=VXMLOBJET)  {
			  erreur=OUI;
			  parseur->erreur_survenue=85;
			  return 0;
		  }
		  vxml=(VariableXML*)variab;
		  variab= formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
		  if (variab==NULL || variab->sorte!=VXMLOBJET)  {
			  erreur=OUI;
			  parseur->erreur_survenue=88;
			  return 0;
		  }
		  vxmlbis=(VariableXML*)variab;
		  if (vxml->noeud==NULL || vxml->noeud->type==XML_DOCUMENT_NODE || vxml->noeud->name==NULL)
			  return NON;

		  switch(operateur) {
		  case 112:
			  vxmlbis->noeud=vxml->noeud->parent;
			  break;
		  case 113:
			  vxmlbis->noeud=vxml->noeud->children;
			  break;
		  case 114:
			  vxmlbis->noeud=vxml->noeud->next;
			  break;
		  case 115:
			  vxmlbis->noeud=vxml->noeud->prev;
			  break;
		  }
		  if (vxmlbis->noeud==NULL || vxmlbis->noeud->type==XML_DOCUMENT_NODE || vxmlbis->noeud->name==NULL)
			  return NON;
		  return OUI;
	  case 117://xmlset("@reference",variable)
		  if (formules.dernier!=2) {
			  erreur=OUI;
			  parseur->erreur_survenue=90;
			  return 0;
		  }
		  formules[0]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
		  variab= formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
		  if (variab==NULL || variab->sorte!=VXMLOBJET)  {
			  erreur=OUI;
			  parseur->erreur_survenue=85;
			  return 0;
		  }
		  vxml=(VariableXML*)variab;
		  if (buffbis=="@_current" || buffbis=="@_xtd") {
			  parseur->basexmlcourante->XmlReferenceNode=vxml->noeud;
			  return OUI;
		  }
		  else {
			  for (i=0;i<parseur->BaseDonneesXML.dernier;i++) {
				  if (!compchmin((uchar*)buffbis.c_str(),parseur->BaseDonneesXML[i]->pointentree)) {
					  parseur->BaseDonneesXML[i]->XmlReferenceNode=vxml->noeud;
					  return OUI;
				  }
			  }
		  }
		  erreur=OUI;
		  parseur->erreur_survenue=92;
		  return NON;
	  case 143://xmltoken(#1,variable)
		  if (formules.dernier!=2) {
			  erreur=OUI;
			  parseur->erreur_survenue=124;
			  return NON;
		  }
		  n= noeuds[formules[0]->element->argument(parseur,noeuds)];
		  if (n==NULL) {
			  erreur=OUI;
			  parseur->erreur_survenue=124;
			  return NON;
		  }
		  variab= formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
		  if (variab==NULL || variab->sorte!=VXMLOBJET)  {
			  erreur=OUI;
			  parseur->erreur_survenue=124;
			  return NON;
		  }
		  vxml=(VariableXML*)variab;
		  if (n->source->voc!=NULL && n->source->voc->xmlnoeud!=NULL)
			  vxml->noeud=n->source->voc->xmlnoeud;
		  else {
			  erreur=OUI;
			  parseur->erreur_survenue=125;
			  return NON;
		  }
		  return OUI;
#endif
	  case 184://eval(string,add)
		  if (formules.dernier != 2) {
			  erreur = OUI;
			  parseur->erreur_survenue = 174;
			  return 0;
		  }
		  formules[0]->TraitementFormuleString(parseur, noeuds, erreur, buffbis, focus);
		  id = formules[1]->TraitementFormule(parseur, noeuds, erreur, focus);
		  parseur->LireParametresEnChaine(buffbis, id);
		  return 1;
	  case 165://words(w1,w2,w3,w4...)
		  //Nous enregistrons l'ensemble des ER que nous devrons evaluer
		  for (id=0;id<formules.dernier;id++) {
			  buffbis="";
			  formules[id]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
			  expressions.ajoute(new automate((uchar*)buffbis.c_str(),(void*)1));
		  }
		  //Puis nous recherchons dans nos structures de graphe, celles qui correspondent
		  id=parseur->ListeTokens(expressions);
		  expressions.nettoie();
		  return id;
	  case 166://setword(i,w)
		  if (formules.dernier!=2) {
			  erreur=OUI;
			  parseur->erreur_survenue=157;
			  return 0;
		  }
		  //Premier parametre est un int
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  //Deuxieme parametre est une string
		  formules[1]->TraitementFormuleString(parseur,noeuds,erreur,buffbis,focus);
		  //Nous changeons alors la donne pour tous les elements de ListeTokens
		  for (id=0;id<parseur->listedetokens.dernier;id++) {
			  if (pos>=parseur->listedetokens[id]->dernier)
				  return 0;
			  //Sinon, on met a jour la structure de mot dans notre element
			  parseur->listedetokens[id]->vecteur[pos]->mot=buffbis;
		  }
		  return 1;
	  case 167://getword(i,str[])
		  if (formules.dernier!=2) {
			  erreur=OUI;
			  parseur->erreur_survenue=158;
			  return 0;
		  }
		  //Premier parametre est un int
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  //Deuxieme parametre est un vstring
		  variab= formules[1]->RenvoieVariableVecteur(parseur,noeuds,erreur,focus,TESTERVECTEURVARIABLE);
		  if (variab==NULL || variab->sorte!=VVECTEURCHAINE)  {
			  erreur=OUI;
			  parseur->erreur_survenue=158;
			  return -1;
		  }

		  varvector=(VariableVector*)variab;
		  varvector->raz();
		  //Nous changeons alors la donne pour tous les elements de ListeTokens
		  for (id=0;id<parseur->listedetokens.dernier;id++) {
			  if (pos>=parseur->listedetokens[id]->dernier)
				  return 0;
			  //Sinon, on met a jour la structure de mot dans notre element
			  varvector->AffecteValeurStringIndex(new string(parseur->listedetokens[id]->vecteur[pos]->mot),id);
		  }

		  return 1;
	  case 168://cleanword(i)
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=159;
			  return 0;
		  }
		  //Premier parametre est un int
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  //Nous changeons alors la donne pour tous les elements de ListeTokens
		  for (id=0;id<parseur->listedetokens.dernier;id++) {
			  if (pos>=parseur->listedetokens[id]->dernier)
				  return 0;
			  //Sinon, on met a jour la structure de mot dans notre element
			  parseur->listedetokens[id]->vecteur[pos]->mot="";
		  }
		  return 1;
	  case 169://setruleweight
		  if (formules.dernier!=2) {
			  erreur=OUI;
			  parseur->erreur_survenue=160;
			  return 0;
		  }
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  resinter=formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
		  return parseur->proba_metpoids(pos,resinter);
	  case 170://setrulethreshold
		  if (formules.dernier!=2) {
			  erreur=OUI;
			  parseur->erreur_survenue=160;
			  return 0;
		  }
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  resinter=formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
		  return parseur->proba_metseuil(pos,resinter);
	  case 171://setrulevalue
		  if (formules.dernier!=2) {
			  erreur=OUI;
			  parseur->erreur_survenue=160;
			  return 0;
		  }
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  resinter=formules[1]->TraitementFormule(parseur,noeuds,erreur,focus);
		  return parseur->proba_metvaleur(pos,resinter);
	  case 172://getruleweight
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=160;
			  return 0;
		  }
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  return parseur->proba_poids(pos);
	  case 173://getrulethreshold
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=160;
			  return 0;
		  }
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  return parseur->proba_seuil(pos);
	  case 174://getrulevalue
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=160;
			  return 0;
		  }
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  return parseur->proba_valeur(pos);
	  case 175://@nbrules
		  return parseur->VBases.dernier;
	  case 176://ruletype
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=161;
			  return 0;
		  }
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  if (pos==-1) {
			  if (parseur->reglecourante!=NULL)
				  return parseur->reglecourante->label->type;
			  else
				  return NON;
		  }

		  if (parseur->VBases[pos]==NULL) {
			  erreur=OUI;
			  parseur->erreur_survenue=162;
			  return 0;
		  }
		  return parseur->VBases[pos]->label->type;
	  case 180://rulecounter
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=165;
			  return 0;
		  }
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  if (parseur->VBases[pos]==NULL) {
			  erreur=OUI;
			  parseur->erreur_survenue=162;
			  return 0;
		  }
		  return parseur->VBases[pos]->compteur;
	  case 181://rulelayer
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=165;
			  return -1;
		  }
		  pos=formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  if (pos==-1) {
			  if (parseur->reglecourante!=NULL)
				return parseur->reglecourante->couche;
			  return -1;
		  }
		  if (parseur->VBases[pos]==NULL) {
			  erreur=OUI;
			  parseur->erreur_survenue=162;
			  return -1;
		  }
		  return parseur->VBases[pos]->couche;
	  case 119://pythonfulllexical
		  if (formules.dernier!=1) {
			  erreur=OUI;
			  parseur->erreur_survenue=91;
			  return 0;
		  }
		  parseur->fullpythonlexical=(int)formules[0]->TraitementFormule(parseur,noeuds,erreur,focus);
		  return OUI;
	  case 120://wordcount
		  return parseur->phrase.dernier;
	  case 121://punctcount
		  return parseur->punctuations;
    }

    erreur=OUI;
    parseur->erreur_survenue=13;
    return NON;
}

//Remise-a-zero des variables globales
void TestFonctionDependance::raz(GlobalParseur* parseur) {
    VectTFInstruction* proced = parseur->ListeDesProcedures[parseur->IndexProcedures[Fx->index]];
    if (proced==NULL)
        return;

    //Tout d'abord on verifie que la dependance comprend le meme nombre d'argument, sinon
    //c'est dans tous les cas une erreur
    Dependance* depproc=proced->dependances[0];
    NomProcedure* nomproc=NULL;
    if (depproc->Type()==NON)
        return;

    nomproc=(NomProcedure*)depproc;

    //Tout d'abord on s'occupe des parametres mathematiques de la fonction
    for (int iar=0;nomproc!=NULL && iar<nomproc->parametres.dernier;iar++) {
        //Si c'est une variable globale
        if (nomproc->parametres[iar]->reinit==NON &&
            formules[iar]==NULL) {
            //Alors on retrouve notre pointeur
            nomproc->parametres[iar]->raz();
        }
    }
}


inline void RemiseAJourVariable(GlobalParseur* parseur,
                         NomProcedure* nomproc,
                         VECTA<float>* valeursglobales,
                         VECTA<char*>* valeurschaines,
						 VECTA<xmlNodePtr>* valeursxml) {
    etat* e;
    VariableString* vs;
    VariableGlobale* vg;
	VariableXML* vxml;

    //On redonne les valeurs des variables parametres a vocation locale
    for (int iar=0;nomproc!=NULL && iar<nomproc->parametres.dernier;iar++) {
        vs=NULL;
        vg=NULL;

        if (nomproc->parametres[iar]->reinit==NON) {
            e=parseur->VariablesProceduresGlobales.recherche(nomproc->parametres[iar]->nom);
            if (e!=NULL) {
                VECTA<TestFormule*>* vtform=(VECTA<TestFormule*>*)e->valeur;
                for (int jar=0;jar<vtform->dernier;jar++)
                    (*vtform)[jar]->AffecteVariable(nomproc->parametres[iar]);
            }
            continue;
        }

        if (nomproc->parametres[iar]->sorte==VCHAINE)
            vs=(VariableString*)nomproc->parametres[iar];
        else
            if (nomproc->parametres[iar]->sorte==VGLOBALE)
                vg=(VariableGlobale*)nomproc->parametres[iar];
            else
				if (nomproc->parametres[iar]->sorte==VXMLOBJET)
					vxml=(VariableXML*)nomproc->parametres[iar];

            //si c'est une variable globale, il faut remettre a jour sa valeur
            //si reinit vaut OUI, c'est une variable locale
            //En cas d'erreur, on redonne aussi leur valeurs aux variables globales
            if (vs!=NULL)
                vs->chaine=(*valeurschaines)[iar];
            else
                if (vg!=NULL)
                    vg->valeur=(*valeursglobales)[iar];
#ifdef XIPLIBXML
				else
					if (vxml!=NULL)
						vxml->noeud=(*valeursxml)[iar];
#endif
    }

	if (valeursglobales!=NULL)
        delete valeursglobales;
    if (valeursxml!=NULL)
		delete valeursxml;
    if (valeurschaines!=NULL) {
        valeurschaines->nettoie();
        delete valeurschaines;
    }

}


//Traitement d'une procedure
double TestFonctionDependance::TraitementFormule(GlobalParseur* parseur,
                                                 VECTA<Noeud*>& noeuds,
                                                 char& err,
                                                 VECTA<ResultatFonction*>* dep) {


    VectTFInstruction* proced = parseur->ListeDesProcedures[parseur->IndexProcedures[Fx->index]];
    if (proced==NULL)
        return negation;

    VECTA<Noeud*> vectechangenoeuds(noeuds.dernier);
    VECTA<ResultatFonction*> vectechangedep;

    VECTA<int>* retourvariables=NULL;
    VECTA<float>* valeursglobales=NULL;
    VECTA<char*>* valeurschaines=NULL;
	VECTA<xmlNodePtr>* valeursxml=NULL;



    int iar;
    int idep;
    char erreur=NON;
    Noeud* n;
    double res=OUI;
    char ch[100];


    //Tout d'abord on verifie que la dependance comprend le meme nombre d'argument, sinon
    //c'est dans tous les cas une erreur
    Dependance* depproc=proced->dependances[0];
    NomProcedure* nomproc=NULL;
    if (depproc->Type()==OUI)
        nomproc=(NomProcedure*)depproc;

	if (depproc->arg.dernier!=arg.dernier) {
		err=OUI;
		parseur->erreur_survenue=77;
		sprintf_s(parseur->ajoute_info_erreur,4096," %s",Fx->nom);
		return -1;
	}

    VariableString* vs;
    VariableGlobale* vg;
	VariableXML* vxml;
    etat* e;
    int jar;

    //On enregistre les valeurs des variables parametres
    for (iar=0;nomproc!=NULL && iar<nomproc->parametres.dernier;iar++) {
        vs=NULL;
        vg=NULL;
		vxml=NULL;

        //Si c'est une variable globale
        if (nomproc->parametres[iar]->reinit==NON)
            continue;

        //L'origine des valeurs est double:
        //a) Ca peut etre une autre variable: proc(var1,var2)
        //b) Ca peut etre une valeur donnee telle quelle: proc("ab",1)

        //vs correspond a une variable de chaine qui est un parametre declaree dans notre
        //procedure.
        //vg est la meme chose mais c'est dans ce cas une variable numerique
        if (nomproc->parametres[iar]->sorte==VCHAINE)
            vs=(VariableString*)nomproc->parametres[iar];
        else
            if (nomproc->parametres[iar]->sorte==VGLOBALE)
                vg=(VariableGlobale*)nomproc->parametres[iar];
			else
				if (nomproc->parametres[iar]->sorte==VXMLOBJET)
					vxml=(VariableXML*)nomproc->parametres[iar];

        //si cette variable avait deja une valeur, on la garde
        if (vs!=NULL) {
            if (valeurschaines==NULL)
                valeurschaines=new VECTA<char*>;
            valeurschaines->affecte(iar,DupliqueChaine(vs->chaine.c_str()));
        }
        else {
            if (vg!=NULL) {
                if (valeursglobales==NULL)
                    valeursglobales=new VECTA<float>;
                valeursglobales->affecte(iar,vg->valeur);
            }
#ifdef XIPLIBXML
			else {
				if (vxml!=NULL) {
					if (valeursxml==NULL)
						valeursxml=new VECTA<xmlNodePtr>;
					valeursxml->affecte(iar,vxml->noeud);
				}
			}
#endif
        }
    }

    //Tout d'abord on s'occupe des parametres mathematiques de la fonction
    for (iar=0;nomproc!=NULL && iar<nomproc->parametres.dernier;iar++) {
        vs=NULL;
        vg=NULL;

        //Si c'est une variable globale
        if (nomproc->parametres[iar]->reinit==NON) {
            //Alors on retrouve notre pointeur
            e=parseur->VariablesProceduresGlobales.recherche(nomproc->parametres[iar]->nom);
            if (e!=NULL) {
                //Il s'agit d'un mecanisme assez complexe de passage de parametre.
                //Notre variable est une variable globale qui est initialisee avec une autre
                //variable fourni en parametre
                VECTA<TestFormule*>* vtform=(VECTA<TestFormule*>*)e->valeur;
                if (vtform==NULL || formules[iar]==NULL) {
                    err=OUI;
                    parseur->erreur_survenue=58;
                    sprintf_s(parseur->ajoute_info_erreur,4096," %s",nomproc->Fx->nom);
                }
                else {
                    //variab est la variable fourni en appel dans la procedure
                    Variable* variab=formules[iar]->RenvoieVariableVecteur(parseur,noeuds,erreur,dep,RENVOIEVARIABLE);
                    //si nous n'avons pas donne de parametre en entree pour cet appel,
                    //on repart sans rien faire. Si de plus, le type de la variable est different de
                    //celui attendu, c'est aussi une erreur.
                    char typeerr=NON;
                    //cas d'erreur
                    if (variab==NULL)
                        typeerr=OUI;
                    else {
                        if (variab->type==ENTIER) {
                            if (variab->type != nomproc->parametres[iar]->type)
                                typeerr=OUI;
                            if (variab->sorte==VVECTEURINT) {
                                if (variab->sorte != nomproc->parametres[iar]->sorte)
                                    typeerr=OUI;
                            }
                            else
                                if (variab->sorte!=VGLOBALEREINIT && variab->sorte!=VGLOBALE)
                                    typeerr=OUI;
                        }
                        else {
                            if (variab->sorte != nomproc->parametres[iar]->sorte)
                                typeerr=OUI;
                        }
                    }

                    if (typeerr==OUI){
                        err=OUI;
                        parseur->erreur_survenue=39;
                        sprintf_s(parseur->ajoute_info_erreur,4096," %s",nomproc->Fx->nom);
                    }
                    else {
                        //sinon, partout ou cette variable est mentionnee dans la procedure
                        //nous la remplacons par la variable en parametre.
                        //vtform contient la liste de toutes les mentions de cette variable
                        //au sein de notre procedure.
                        for (jar=0;jar<vtform->dernier;jar++)
                            (*vtform)[jar]->AffecteVariable(variab);
                        continue;
                    }
                }
            }
            else
                err=OUI;

            //en cas d'erreur on repart en nettoyant nos vecteurs de sauvegarde
            //on retourne evidemment un message d'erreur
            if (err==OUI) {
                RemiseAJourVariable(parseur,nomproc,valeursglobales,valeurschaines,valeursxml);
                sprintf_s(parseur->ajoute_info_erreur,4096," %s",nomproc->Fx->nom);
                return NON;
            }
        }

        if (nomproc->parametres[iar]->sorte==VCHAINE)
            vs=(VariableString*)nomproc->parametres[iar];
        else
            vg=(VariableGlobale*)nomproc->parametres[iar];

        //dans le cas d'un appel, il faut initialiser les variables avec les parametres
        //fourni en appel. Ces parametres sont stockes dans formules...
        if (formules[iar]!=NULL) {
            if (formules[iar]->TypeString()) {
                string buff;
                formules[iar]->TraitementFormuleString(parseur,noeuds,err,buff,dep);
                if (nomproc->parametres[iar]==NULL)
                    return NON;
                if (vs!=NULL)
                    vs->chaine=buff;
                else
                    vg->valeur=atof((char*)buff.c_str());
            }
            else {
                res=formules[iar]->TraitementFormule(parseur,noeuds,err,dep);
                if (vs!=NULL) {
                    sprintf_s(ch,100,"%g",res);
                    vs->chaine=ch;
                }
                else {
                    vg->utilisee=OUI;
                    vg->valeur=res;
                }
            }
        }
        else {
            if (vs!=NULL && vs->reinit==OUI)
                vs->chaine="";
            else
                if (vg!=NULL && vg->reinit==OUI)
                    vg->valeur=0;
        }
    }

    res=OUI;

    //Puis on range les noeuds au bon endroit
    for (iar=0;iar<arg.dernier;iar++) {
        //On recupere l'argument
        if (arg[iar]>=0) {//c'est un noeud
            if ((int)(depproc->arg[iar])<0) {
                err=OUI;
                parseur->erreur_survenue=78;
                sprintf_s(parseur->ajoute_info_erreur,4096," %s",nomproc->Fx->nom);
                return -1;
            }
            n=noeuds[arg[iar]];
            //On le place dans notre tableau selon le numero de la variable presente
            //dans la procedure
            //Si la variable est nulle, il s'agit peut-etre d'une variable de retour
            if (n==NULL) {
                if (retourvariables==NULL)
                    retourvariables=new VECTA<int>;
                retourvariables->ajoute(iar);
            }

            vectechangenoeuds.affecte(depproc->arg[iar],n);
        }
        else {//c'est une dependance
            //on retrouve le numero initial
            if (dep!=NULL) {
                if (int(depproc->arg[iar])>=0) {
                    err=OUI;
                    parseur->erreur_survenue=78;
                    sprintf_s(parseur->ajoute_info_erreur,4096," %s",nomproc->Fx->nom);
                    return -1;
                }
                idep=arg[iar]*-1;
                if ((*dep)[idep]!=NULL)
                    vectechangedep.affecte(idep,(*dep)[idep]);
            }
        }
    }

    //On enregistre le retour precedent
    Label* retour_courant=parseur->aller_label;
    //Puis on teste
    res=parseur->DeduitFonction(0,DEDUCTIONPROCEDURE,OUI,parseur->IndexProcedures[Fx->index],&vectechangenoeuds,&vectechangedep);

    parseur->aller_label=retour_courant;

    if (retourvariables!=NULL) {
        for (iar=0;iar<retourvariables->dernier;iar++) {
            //on met a jour dans la table originale ci.noeuds (a partir de vectechangenoeuds)
            //C'est l'operation inverse de la precedente
            //On recupere le noeud
            int jar=depproc->arg[(*retourvariables)[iar]];
            n=vectechangenoeuds[jar];
            if (n==NULL)
                continue;

            jar=arg[(*retourvariables)[iar]];
            //Si cette variable a deja une valeur, on la compare
            if (noeuds[jar]!=NULL) {
                if (noeuds[jar]!=n)
                    return NON;
            }
            else
                //puis on l'affecte dans la table
                noeuds.affecte(jar,n);
        }
        delete retourvariables;
    }

    //On redonne les valeurs des variables parametres a vocation locale
    RemiseAJourVariable(parseur,nomproc,valeursglobales,valeurschaines,valeursxml);
    return res;
}

double TestFonctionKif::TraitementFormule(GlobalParseur* parseur,
										  VECTA<Noeud*>& noeuds,
										  char& err,
										  VECTA<ResultatFonction*>* dep) {

    int iar;
    char erreur=NON;
    double res=OUI;
    //VariableVector* vecteurchaine;
    //VariableIntVector* vecteurint;
    //VariableDictionnaire* vdict;
	//VariableXML* vxml;
	//VariableGraphe* vgraph;

    ostream* os=&cout;
    if (parseur->OS!=NULL)
        os=parseur->OS;


    //Tout d'abord on verifie que la dependance comprend le meme nombre d'argument, sinon
    //c'est dans tous les cas une erreur


	//dans le cas ou la fonction a ete declaree dans un programme KiF appelant,
	//il nous faut recuperer ce kifcode...
	KifCode* ancienkifcode=parseur->kifloader;
	if (parseur->kifloader!=proc_kif->kifcode)
		parseur->kifloader=proc_kif->kifcode;

    for (iar=0;iar<arg.dernier;iar++) {
        int valarg=(int)arg[iar];
        if (valarg>=0) {
            if (arg[iar]<0) {
                err=OUI;
                parseur->erreur_survenue=179;
                sprintf_s(parseur->ajoute_info_erreur,4096," %s",Fx->nom);
                return -1;
            }
        }
        else {
            if (arg[iar]>=0) {
                err=OUI;
                parseur->erreur_survenue=179;
                sprintf_s(parseur->ajoute_info_erreur,4096," %s",Fx->nom);
                return -1;
            }
        }
    }

    //on cree un tuple d'appel python dans lequel on va ranger nos objets


	//On conserve l'index du parseur courant sous la forme d'une variable dont le nom
	//est: current_parseur et dont la valeur est l'index du parseur courant


    double valeur;
	params.clear();
	nettoyage.clear();
	KifElement* ke=NULL;

    //On range les noeuds au bon endroit, les parametres noeuds sont les premiers appeles
    for (iar=0;iar<arg.dernier;iar++) {
		ke=NULL;
        //On recupere l'argument
        if (arg[iar]>=0) {//c'est un noeud
            Noeud* n=noeuds[arg[iar]];
            //On le place dans notre tableau selon le numero de la variable presente
            //dans la procedure
            //Si la variable est nulle, il s'agit peut-etre d'une variable de retour
            if (n==NULL) {
                err=OUI;
                sprintf_s(parseur->ajoute_info_erreur,4096," %s: Missing node variable",Fx->nom);
                return NON;
            }
            //On cree un objet de type KifXipNode
			ke=new KifXipNode(parseur->kifloader,NULL,n);
        }
        else {//c'est une dependance
            int idep=arg[iar]*-1;
			if (dep!=NULL && (*dep)[idep]!=NULL) {
				if ((*dep)[idep]->Fx->type==C_GENERATION)
					ke=new KifXipGeneration(parseur->kifloader,NULL,(*dep)[idep]);
				else
					ke=new KifXipDependency(parseur->kifloader,NULL,(*dep)[idep]);
			}
        }

		if (ke!=NULL) {
			ke->Setreference();
			nettoyage.push_back(ke);
			params.push_back(ke);
		}
    }

    int ikifobjet=0;
    //On initialise les valeurs de nos variables kif
	for (iar=0;iar<formules.dernier;iar++) {
		//dans le cas d'un appel, il faut initialiser les variables avec les parametres
		//fourni en appel. Ces parametres sont stockes dans formules..        .
		if (formules[iar]==NULL) {
			err=OUI;
			parseur->erreur_survenue=58;
			sprintf_s(parseur->ajoute_info_erreur,4096," %s",Fx->nom);
			return -1;
		}

		ke=NULL;
		//Variable* variab=formules[iar]->RenvoieVariableVecteur(parseur,noeuds,erreur,dep,RENVOIEVARIABLE);
		Variable* variab=formules[iar]->RenvoieVariable();
		//variab est la variable fourni en appel dans la procedure
		//Cas particulier du traitement des listes et dictionnaires
		bool xiplocalvariable=false;

		if (variab!=NULL) {
			switch(variab->sorte) {
			case VGLOBALEREINIT:
			case VGLOBALE:
			case VCHAINE:
			case VXMLOBJET:
			case VGRAPHE:
				ke=parseur->kifloader->kstart->Getstring(variab->nom);
				xiplocalvariable=true;
				break;
			case VDICTIONNAIRE:
			case VVECTEURCHAINE:
				if (((TestFormuleElementVectorString*)formules[iar]->element)->indice_vecteur==NULL) {
					ke=parseur->kifloader->kstart->Getstring(variab->nom);
					xiplocalvariable=true;
				}
				break;
			case VVECTEURINT:
				if (((TestFormuleElementIntVector*)formules[iar]->element)->indice_vecteur==NULL) {
					ke=parseur->kifloader->kstart->Getstring(variab->nom);
					xiplocalvariable=true;
				}
				break;
			case VKIFOBJET:
				xiplocalvariable=true;
				if (variab==parseur->vkif_echange)
					ke=parseur->vkif_echange->kif;
				else {
					if (parseur->kifparametres==NULL || ikifobjet>=parseur->kifparametres->instructions.size()) {
						err=OUI;
						parseur->erreur_survenue=178;
						sprintf_s(parseur->ajoute_info_erreur,4096," %s",Fx->nom);
						return -1;
					}
					ke=parseur->kifparametres->instructions[ikifobjet++];
				}
				break;
			}
		}

		if (ke==NULL) {
			xiplocalvariable=false;
			//sinon il nous faut tout calculer
			if (formules[iar]->TypeString()) {
				string buff;
				formules[iar]->TraitementFormuleString(parseur,noeuds,err,buff,dep);
				ke=parseur->kifloader->Providestring(buff);
			}
			else {
				res=formules[iar]->TraitementFormule(parseur,noeuds,err,dep);
				ke=parseur->kifloader->Providefloat(res);
			}
		}

		if (ke!=NULL) {
			//dans le cas ou il ne s'agit pas d'une variable pre-declaree
			params.push_back(ke);
			//Dans le cas ou il s'agit d'une variable XIP, on evite de les nettoyer...
			if (!xiplocalvariable) {
				ke->Setreference();
				nettoyage.push_back(ke);
			}
		}
	}

	void* ancienparseur=kifGlobals->data;
	ostream* ancienos=kifGlobals->os;
	kifGlobals->data=parseur;
	kifGlobals->os=os;
	//Il est possible que l'on execute une fonction dans un autre kifcode
	//dans ce cas on garde son ancienne valeur
	//on le fait pointer sur notre parseur courant
	KifElement* resultat=kifNULL;
	if (params.size() == proc_kif->arguments.size())
		resultat = parseur->kifloader->Execute(proc_kif, params);
	else {
		err = OUI;
		parseur->erreur_survenue = 179;
		sprintf_s(parseur->ajoute_info_erreur,4096, " %s", Fx->nom);
	}

	if (Error() || err) {
		err=OUI;
		parseur->erreur_survenue=177;
		string sres=Errorstring();
		if (sres.size()>=4096)
			sres=sres.substr(0,4095);
		sprintf_s(parseur->ajoute_info_erreur,4096," %s",STR(sres));

		if (parseur->TestOptions(AFFICHAGE_ERREUR_EXECUTION))
			parseur->affiche_derniere_erreur(-1);

		//On arrete de debugger dans le code kif
		kifGlobals->debuginfocurrent->kifdebugging=false;
		Cleanerror();
		valeur=0;
	}
	else {
		valeur=resultat->Float();
		resultat->Release();
	}
	for (iar=0;iar<nettoyage.size();iar++)
		nettoyage[iar]->Resetreference();
	kifGlobals->data=ancienparseur;
	kifGlobals->os=ancienos;
	parseur->kifloader=ancienkifcode;
	return valeur;
}



#ifdef XIPPYTHON
TestFonctionPython::~TestFonctionPython() {}

#ifndef DONOTEXPORTXIPS
double TestFonctionPython::TraitementFormule(GlobalParseur* parseur,
                                             VECTA<Noeud*>& noeuds,
                                             char& err,
                                             VECTA<ResultatFonction*>* dep) {

    int iar;
    int i;
    char erreur=NON;
    Noeud* n;
    double res=OUI;
    VariableVector* vecteurchaine;
    VariableIntVector* vecteurint;
    VariableDictionnaire* vdict;
	VariableGraphe* vgraph;
	VariableVectorGraphe* vecteurgraph;


    //Tout d'abord on verifie que la dependance comprend le meme nombre d'argument, sinon
    //c'est dans tous les cas une erreur
    Dependance* depproc=proc_python->depend;
    NomProcedure* nomproc=NULL;
    if (depproc->Type()==OUI)
        nomproc=(NomProcedure*)depproc;

    if (depproc->arg.dernier!=arg.dernier) {
        err=OUI;
        parseur->erreur_survenue=79;
        sprintf_s(parseur->ajoute_info_erreur,4096," %s",nomproc->Fx->nom);
        return -1;
    }

    for (iar=0;iar<arg.dernier;iar++) {
        int valarg=(int)depproc->arg[iar];
        if (valarg>=0) {
            if (arg[iar]<0) {
                err=OUI;
                parseur->erreur_survenue=80;
                sprintf_s(parseur->ajoute_info_erreur,4096," %s",nomproc->Fx->nom);
                return -1;
            }
        }
        else {
            if (arg[iar]>=0) {
                err=OUI;
                parseur->erreur_survenue=80;
                sprintf_s(parseur->ajoute_info_erreur,4096," %s",nomproc->Fx->nom);
                return -1;
            }
        }
    }

    int dans_tuple=0;
    //on cree un tuple d'appel python dans lequel on va ranger nos objets
    PyObject* pcourant;
    PyObject* pelement;
    PyObject* liste;
    PyObject* pentree;

	map<string, string>::iterator iter;

	//On conserve l'index du parseur courant sous la forme d'une variable dont le nom
	//est: current_parseur et dont la valeur est l'index du parseur courant

	PyObject* pDict_principal = PyModule_GetDict(parseur->module_principal);
	pcourant=ConvertToPythonLong(parseur->iParseurCourant);

	char chrun[100];
	sprintf_s(chrun,100,"XipNode.parseur=%d",parseur->iParseurCourant);
	PyRun_String(chrun,Py_file_input,pDict_principal,proc_python->pDico);
	sprintf_s(chrun,100,"XipDependency.parseur=%d",parseur->iParseurCourant);
	PyRun_String(chrun,Py_file_input,pDict_principal,proc_python->pDico);
	PyDict_SetItemString(proc_python->pDico,"current_parseur",pcourant);

    double valeur;
    VECTA<PyObject*> py_listes;
    VECTA<PyObject*> py_args;

    //On range les noeuds au bon endroit, les parametres noeuds sont les premiers appeles
    for (iar=0;iar<arg.dernier;iar++) {
        //On recupere l'argument
        if (arg[iar]>=0) {//c'est un noeud
            n=noeuds[arg[iar]];
            //On le place dans notre tableau selon le numero de la variable presente
            //dans la procedure
            //Si la variable est nulle, il s'agit peut-etre d'une variable de retour
            if (n==NULL) {
                err=OUI;
                sprintf_s(parseur->ajoute_info_erreur,4096," %s: Missing node variable",nomproc->Fx->nom);
                return NON;
            }
            //On donne le numero de noeud comme parametre
            pcourant=ConvertToPythonLong(n->num_noeud);
        }
        else {//c'est une dependance
            int idep=arg[iar]*-1;
            if (dep!=NULL && (*dep)[idep]!=NULL)
                pcourant=ConvertToPythonLong((*dep)[idep]->index);
            else
                pcourant=ConvertToPythonLong(-1);
        }
        //On le range alors dans le tuple des appels python
        py_args.ajoute(pcourant);
    }


    //On initialise les valeurs de nos variables python
    for (iar=0;nomproc!=NULL && iar<nomproc->parametres.dernier;iar++) {

        pentree=NULL;
        //dans le cas d'un appel, il faut initialiser les variables avec les parametres
        //fourni en appel. Ces parametres sont stockes dans formules..        .
        if (nomproc->parametres[iar]->reinit==NON) {
            if (formules[iar]==NULL) {
                err=OUI;
                parseur->erreur_survenue=58;
                sprintf_s(parseur->ajoute_info_erreur,4096," %s",nomproc->Fx->nom);
                return -1;
            }

            Variable* variab=formules[iar]->RenvoieVariableVecteur(parseur,noeuds,erreur,dep,RENVOIEVARIABLE);
            //variab est la variable fourni en appel dans la procedure
            //Cas particulier du traitement des listes et dictionnaires
            char type_utilise=NON;

            char typeerr=NON;
            //cas d'erreur
            if (variab->type==ENTIER) {
                if (variab->type != nomproc->parametres[iar]->type)
                    typeerr=OUI;
                if (variab->sorte==VVECTEURINT) {
                    if (variab->sorte != nomproc->parametres[iar]->sorte)
                        typeerr=OUI;
                }
                else
                    if (variab->sorte!=VGLOBALEREINIT && variab->sorte!=VGLOBALE)
                        typeerr=OUI;
            }
            else {
                if (variab->sorte != nomproc->parametres[iar]->sorte)
                    typeerr=OUI;
            }

            if (typeerr==OUI){
                err=OUI;
                parseur->erreur_survenue=39;
                sprintf_s(parseur->ajoute_info_erreur,4096," %s",nomproc->Fx->nom);
                return -1;
            }

            switch(variab->sorte) {
			case VGRAPHE:
				vgraph=(VariableGraphe*)variab;
				liste=pythonclone(vgraph->g);
                py_args.ajoute(liste);
                type_utilise=OUI;
				break;
			case VVECTEURGRAPHE:
				vecteurgraph=(VariableVectorGraphe*)variab;
				liste=PyList_New(0);
				for (i=0;i<vecteurgraph->graphes.dernier;i++) {
					pcourant=pythonclone(vecteurgraph->graphes[i]);
					PyList_Insert(liste,i,pcourant);
					Py_DECREF(pcourant);
				}
                py_args.ajoute(liste);
                type_utilise=OUI;
                break;
            case VPYTHONOBJET:
                if (((VariablePython*)variab)->echange!=NULL) {
                    pentree=((VariablePython*)variab)->echange;
                    py_args.ajoute(pentree);
                    Py_INCREF(pentree);
                    pentree=NULL;
                }
                else
                    py_args.ajoute(ConvertToPythonLong(0));
				type_utilise=OUI;
                break;
            case VVECTEURCHAINE:
                //Dans le cas d'une liste, on boucle sur les valeurs
                vecteurchaine=(VariableVector*)variab;
                liste=PyList_New(0);
                py_listes.affecte(iar,liste);
                for (i=0;i<vecteurchaine->chaine.dernier;i++) {
                    pcourant=ConvertToPythonString(vecteurchaine->chaine[i]->c_str());
                    PyList_Insert(liste,i,pcourant);
					Py_DECREF(pcourant);
                }
                py_args.ajoute(liste);
                type_utilise=OUI;
                break;
            case VVECTEURINT:
                //Dans le cas d'une liste, on boucle sur les valeurs
                vecteurint=(VariableIntVector*)variab;
                liste=PyList_New(0);
                py_listes.affecte(iar,liste);
                for (i=0;i<vecteurint->valeurs.dernier;i++) {
                    pcourant=PyFloat_FromDouble(vecteurint->valeurs[i]);
                    PyList_Insert(liste,i,pcourant);
					Py_DECREF(pcourant);
                }
                py_args.ajoute(liste);
                type_utilise=OUI;
                break;
            case VDICTIONNAIRE:
                vdict=(VariableDictionnaire*)variab;
                liste=PyDict_New();
                py_listes.affecte(iar,liste);
                for (iter=vdict->dico.begin();iter!=vdict->dico.end();iter++) {
					pelement=ConvertToPythonString((*iter).second.c_str());
					PyDict_SetItemString(liste, (*iter).first.c_str(), pelement);
                    Py_DECREF(pelement);
                }
                py_args.ajoute(liste);
                type_utilise=OUI;
                break;
            }

            if (type_utilise==OUI)
                continue;

        }

        if (formules[iar]!=NULL) {
            if (formules[iar]->TypeString()) {
                string buff;
                formules[iar]->TraitementFormuleString(parseur,noeuds,err,buff,dep);
                if (nomproc->parametres[iar]==NULL)
                    return NON;
                pcourant=ConvertToPythonString((char*)buff.c_str());
            }
            else {
                res=formules[iar]->TraitementFormule(parseur,noeuds,err,dep);
                if (nomproc->parametres[iar]->type==ENTIER)
                    pcourant=ConvertToPythonLong((long)res);
                else
                    pcourant=PyFloat_FromDouble(res);
            }
        }
        else {
            if (nomproc->parametres[iar]->sorte==VCHAINE)
                pcourant=ConvertToPythonString("");
            else
                pcourant=ConvertToPythonLong(0);
        }

        if (nomproc->parametres[iar]->reinit==NON) {
            string dom=nomproc->parametres[iar]->nom;
			int idom=dom.find("::");
			if (idom!=string::npos)
				dom=dom.substr(0,idom);
            pentree=ConvertToPythonString(STR(dom));
            PyDict_SetItem(proc_python->pDico,pentree,pcourant);
            py_listes.affecte(iar,pentree);
        }
        else
            py_args.ajoute(pcourant);

    }

	int tail;
    PyObject* procArgs = PyTuple_New(py_args.dernier);

    //On range les arguments dans un tuple
    for (iar=0;iar<py_args.dernier;iar++)
        PyTuple_SetItem(procArgs, iar, py_args[iar]);

    PyObject* presultat= PyObject_CallObject(proc_python->pFunction, procArgs);
    PyObject* perreur=PyErr_Occurred();
    if (perreur!=NULL) {
        err=OUI;
        parseur->erreur_survenue=70;
        if (parseur->TestOptions(AFFICHAGE_ERREUR_EXECUTION)) {
            ostream* os=&cout;
            if (parseur->OS==NULL) {
                PyObject *ptype;
                PyObject *pvalue;
                PyObject *ptraceback;
                PyErr_Fetch(&ptype,&pvalue,&ptraceback);
				PyErr_Clear();
                //os=parseur->OS;
                *os<<"Error in file:"<<proc_python->nomfichier<<Endl;

                if (pvalue!=NULL && PyCheck_String(pvalue)) {
					PyObject* perr=PyObject_Str(pvalue);
                    *os<<Endl<<"In python procedure \""<<proc_python->nom<<"\": "<<PyAsString(perr)<<Endl;
					Py_DECREF(perr);
				}

				if (ptraceback!=NULL) {
					//On passe dans ce cas par le module traceback importe par XIP...
					PyObject* espace_trace=PyDict_GetItemString(pDict_principal, "traceback");
					if (espace_trace!=NULL) {
						PyObject* pDict_traceback = PyModule_GetDict(espace_trace);
						//On peut alors faire appel a la fonction: format_list
						PyObject* formatlist=PyDict_GetItemString(pDict_traceback,"format_tb");

						PyObject* formatlistArgs = PyTuple_New(1);
						//On y range notre objet
						PyTuple_SetItem(formatlistArgs, 0, ptraceback);
						//On appelle notre fonction
						PyObject* plistetraceback= PyObject_CallObject(formatlist, formatlistArgs);
						//Le resultat est dans plistetraceback, une liste de chaine
						if (plistetraceback!=NULL) {
							if (PyList_Check(plistetraceback)==1) {
								//On recupere la taille de cette liste
								tail= PyList_GET_SIZE(plistetraceback);
								for (i=0;i<tail;i++) {
									//Puis chacun des objets, kj est l'index de l'element
									pelement=PyList_GetItem(plistetraceback,i);
									//On le range dans notre vecteur
									if (pelement!=NULL && PyCheck_String(pelement))
										*os<<PyAsString(pelement);
								}
							}
							Py_DECREF(plistetraceback);
						}
						Py_DECREF(formatlistArgs);
					}
				}

                if (ptype!=NULL)
                    Py_DECREF(ptype);
                if (pvalue!=NULL)
                    Py_DECREF(pvalue);
                if (ptraceback!=NULL)
                    Py_DECREF(ptraceback);
                *os<<Endl;
            }
            else {
                cout<<"Error in file:"<<proc_python->nomfichier<<Endl;
                PyErr_Print();
                cout<<"In python procedure \""<<proc_python->nom<<Endl<<Endl;
				PyErr_Clear();
            }
        }
        for (iar=0;nomproc!=NULL && iar<nomproc->parametres.dernier;iar++) {
            //Si c'est une variable globale
            if (nomproc->parametres[iar]->reinit==NON) {
                pcourant=py_listes[iar];
                if (pcourant!=NULL) {
                    //variab est la variable fourni en appel dans la procedure
                    Variable* variab=formules[iar]->RenvoieVariableVecteur(parseur,noeuds,erreur,dep,RENVOIEVARIABLE);
                    //si nous n'avons pas donne de parametre en entree pour cet appel,
                    //on repart sans rien faire. Si de plus, le type de la variable est different de
                    //celui attendu, c'est aussi une erreur.
                    switch(variab->sorte) {
                    case VGLOBALEREINIT:
                    case VGLOBALE:
                        Py_DECREF(pcourant);
                        PyDict_DelItem(proc_python->pDico, pcourant);
                        break;
                    case VCHAINE:
                        Py_DECREF(pcourant);
                        PyDict_DelItem(proc_python->pDico, pcourant);
                        break;
                    }
                }
            }
        }
        if (presultat!=NULL)
            Py_DECREF(presultat);
        Py_DECREF(procArgs);
        return -1;
    }

    for (iar=0;nomproc!=NULL && iar<nomproc->parametres.dernier;iar++) {
        //Si c'est une variable globale
        if (nomproc->parametres[iar]->reinit==NON) {
            string reschaine;
            string index;
            PyObject* key;
            PyObject* value;
#if (defined(APPLE) | defined(ARCH64))
			Py_ssize_t pos;
#else
            int pos;
#endif
            pcourant=py_listes[iar];
            if (pcourant!=NULL) {
                //variab est la variable fourni en appel dans la procedure
                Variable* variab=formules[iar]->RenvoieVariableVecteur(parseur,noeuds,erreur,dep,RENVOIEVARIABLE);
                //si nous n'avons pas donne de parametre en entree pour cet appel,
                //on repart sans rien faire. Si de plus, le type de la variable est different de
                //celui attendu, c'est aussi une erreur.
                switch(variab->sorte) {
                case VGLOBALEREINIT:
                case VGLOBALE:
                    //Il nous faut recuperer notre variable
                    variab->utilisee=OUI;
                    valeur=0;
                    pelement=PyDict_GetItem(proc_python->pDico,pcourant);
                    if (PyFloat_Check(pelement))
                        valeur=PyFloat_AsDouble(pelement);
                    else
                        if (PyCheck_Number(pelement))
                            valeur=PyAsNumber(pelement);
                    ((VariableGlobale*)variab)->valeur=valeur;
                    Py_DECREF(pcourant);
                    PyDict_DelItem(proc_python->pDico, pcourant);
					break;
					//On remet a jour les valeurs
				case VVECTEURCHAINE:
                    vecteurchaine=(VariableVector*)variab;
                    vecteurchaine->raz();
                    //Puis on rajoute tous les elements de notre liste
                    if (PyList_Check(pcourant)==1) {
                        //On recupere la taille de cette liste
                        tail= PyList_GET_SIZE(pcourant);
                        for (i=0;i<tail;i++) {
                            //Puis chacun des objets, kj est l'index de l'element
                            pelement=PyList_GetItem(pcourant,i);
                            //On le range dans notre vecteur
                            if (pelement!=NULL && PyCheck_String(pelement))
                                reschaine=PyAsString(pelement);
                            else
                                reschaine="";
                            vecteurchaine->AffecteValeurStringIndex(new string(reschaine),i);
                        }
                    }
                    break;
                case VVECTEURINT:
                    vecteurint=(VariableIntVector*)variab;
                    vecteurint->raz();
                    //Puis on rajoute tous les elements de notre liste
                    if (PyList_Check(pcourant)==1) {
                        //On recupere la taille de cette liste
                        tail= PyList_GET_SIZE(pcourant);
                        for (i=0;i<tail;i++) {
                            //Puis chacun des objets, kj est l'index de l'element
                            pelement=PyList_GetItem(pcourant,i);
                            //On le range dans notre vecteur
                            valeur=0;
                            if (pelement!=NULL) {
                                if (PyFloat_Check(pelement))
                                    valeur=PyFloat_AsDouble(pelement);
                                else
                                    if (PyCheck_Number(pelement))
                                        valeur=PyAsNumber(pelement);
                            }
                            vecteurint->valeurs.affecte(i,valeur);
                        }
                    }
                    break;
                case VDICTIONNAIRE:
                    vdict=(VariableDictionnaire*)variab;
                    vdict->raz();
                    pos=0;
                    while (PyDict_Next(pcourant, &pos, &key, &value)) {
                        if (key!=NULL && value!=NULL && PyCheck_String(key) && PyCheck_String(value)) {
                            index=PyAsString(key);
                            reschaine=PyAsString(value);
                            vdict->AffecteValeurStringIndexChaine(new string(reschaine),index);
                        }
                    }
                    break;
                case VCHAINE:
                    pelement=PyDict_GetItem(proc_python->pDico,pcourant);
                    if (PyCheck_String(pelement))
                        ((VariableString*)variab)->chaine=PyAsString(pelement);
                    Py_DECREF(pcourant);
                    PyDict_DelItem(proc_python->pDico, pcourant);
                    break;
                }
            }
        }
    }

    //On peut enfin liberer les arguments
    Py_DECREF(procArgs);
    res=0;
	if (presultat!=NULL) {
		if (PyFloat_Check(presultat))
			res=PyFloat_AsDouble(presultat);
		else {
			if (PyCheck_Number(presultat))
				res=PyAsNumber(presultat);
			else {
				if (PyLong_Check(presultat))
					res=PyLong_AsLong(presultat);
			}
		}
		Py_DECREF(presultat);
	}

    return res;
}
#endif

ProcedurePython::~ProcedurePython() {
    Py_DECREF(pDico);
}



#endif

