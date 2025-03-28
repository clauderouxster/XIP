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
filename   : python.cxx
Date       : 10/01/1999
Purpose    : Implementation of the compiling and implementation of mathematic formulae.
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef WIN32
#define Py_BUILD_CORE
#endif
#ifdef APPLE
#ifdef DYNAMICLIB
#include <crt_externs.h>
char **environ = *_NSGetEnviron();
#endif
#endif

#if defined(WIN32) & !defined(_WINDLL)
extern char AFFICHAGEDOS;
#endif

#define RENVOIEVARIABLE 1

#include "parsbin.h"

#include "ntm.h"

#include "xipkif.h"
#include "kifcontainer.h"
#include "graphe.h"

extern string choix_ambigue;

#define ConvertToPythonLong(i)	PyLong_FromLong(i)
#define PyCheck_Number	PyLong_Check
#define PyAsNumber	PyLong_AsLong

string PyAsString(PyObject* po) {
	string s;
	Py_ssize_t sz = PyUnicode_GetSize(po);
	Py_UNICODE* ibuff = PyUnicode_AsUnicode(po);
	for (int i = 0; i < sz; i++)
		s += s_unicode_to_utf8(ibuff[i]);
	return s;
}

bool PythonVersString(PyObject* po, string& s) {
	if (PyUnicode_Check(po)) {
		Py_ssize_t sz = PyUnicode_GetSize(po);
		Py_UNICODE* ibuff = PyUnicode_AsUnicode(po);
		for (int i = 0; i < sz; i++)
			s += s_unicode_to_utf8(ibuff[i]);
		return true;
	}
	return false;
}

extern ostream* flot_erreur;
extern char VERSION_XIP_BUILD[];
#ifdef XIPLIBXML
char recupere_donnees_xml(GlobalParseur* parseur, xmlNodePtr node, TestFormule* formule);
void RecupereTextXmlNode(string& accumulateur, xmlNodePtr node);
#endif
baseregle* XipCurrentRule(unsigned int ipar);
string& Trim(string& chaine);

extern char importation_python; 
extern char* champs_anglais[];

extern VecteurDeParseurs gVecteurParseurs;
static GlobalParseur* current_parser = NULL;
static int index_current_parser = -1;
char parentde(ResultatFonction* fille, ResultatFonction* parent);
ResultatFonction* suivantde(ResultatFonction* courant, ResultatFonction* suivant, int increment);
ResultatFonction* perede(ResultatFonction* courant, ResultatFonction* suivant);
ResultatFonction* filsde(ResultatFonction* courant, ResultatFonction* suivant, char dernier);
ResultatFonction* tokensuivant(GlobalParseur*, ResultatFonction* courant, ResultatFonction* suivant, int pos);
string replacestringwithautomaton(string &str, string& reg, string& rep);
#ifdef XIPLIBXML
string xmlEncoding(string& tag, string& txt);
#endif
KifElement* ChercheDansKif(KifCode* kifcode, string name);
KifElement* XipKifExecuteIndexObject(KifCode*, string name, vector<KifElement*>& parameters, bool& erreur);
extern GlobalParseur* BaseParseur;

static char* field_name[] = { "",
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
"sequenceoncontext" };

static const char* affichagemessageerreur[] = {
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

static bool initialization = true;
static PyObject* main_module = NULL;

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
	
	return func;
}

//Lecture d'un programme Python
//Attention, la premiere partie correspond a la lecture de l'interface
//Le reste est le code proprement dit
char TestBlanc(string& ligne) {
	for (int i = 0; i<ligne.size(); i++)
	if (ligne[i]>32)
		return NON;
	return OUI;
}

char TestFinLigne(string& ligne) {
	int i = ligne.size() - 1;
	for (; i>0; i--)
	if (ligne[i]>32)
		break;
	if (!i || ligne[i] != '\\')
		return NON;

	char* ch = (char*)ligne.c_str();
	ch[i] = ' ';
	return OUI;
}

void GlobalParseur::LitProcedurePython(istream& fichier) {
	char k, tampon[XIPBUFFERSIZE];
	char espace_module[XIPBUFFERSIZE];
	char nom[XIPBUFFERSIZE];
	int total = 0;
	vector<string> noms;
	int bd;

	TVecteur zoneLocale = MoinsUn;

	ostream* os = &cout;
	if (OS != NULL)
		os = OS;

	k = lit(fichier, nom);
	if (LectureParametres == OUI) {
		controle_max_regle++;
		if (controle_max_regle>max_controle_regle)
			erreur(CHAINES[190]);
	}

	//Il ne peut y avoir que du python dans ce fichier
	if (k == ':')
		return;

	Dependance* depend;
	VECTA<Dependance*> depends;
	while (!fichier.eof() && k != '.') {
		depend = ConstruitNomProcedure(fichier, k, nom);
		depends.ajoute(depend);
		noms.push_back(nom);
		depend->Fx->type = C_PROCEDUREPYTHON;
		k = lit(fichier, tampon);

		//S'il s'agit d'une simple declaration, on ne va pas plus loin
		if (k != '.' && k != ',')
			erreur(CHAINES[358]);
		if (k == ',')
			k = lit(fichier, nom);
	}

	if (fichier.eof())
		erreur(CHAINES[358]);
	initialisation_python();
	char initialise_premiere_fois = NON;
	if (module_principal == NULL) {
		initialise_premiere_fois = OUI;
		module_principal = PyImport_ImportModule("__main__");
	}

	PyObject* pDict_principal = PyModule_GetDict(module_principal);
	PyObject* pFunc = NULL;
	PyObject* pDict = NULL;
	int j;


	if (importation_python == NON) {
		int num_ligne = LIGNESERREUR;

		*flot_erreur << " module number:" << num_module_python << " (stored in __main__)" << Endl;
		//A partir de la on lit le code python
		unsigned char c = LectCrypte(fichier);
		//on cree le domaine des variables globales
		string che = "from xips import *\n";
		if (python_debug == OUI && num_module_python == num_fichier_python) {
			che += "step_by_step=0\n";
			che += "debug_line_number=";
			sprintf_s(tampon, 1000, "%d\n", num_python_ligne);
			che += tampon;

			che += "def test_mode_debug(num):\n";
			che += "   global step_by_step\n";
			che += "   global debug_line_number\n";
			che += "   if (step_by_step==1 or debug_line_number==num):\n";
			che += "       return 1\n";
			che += "   return 0\n\n";

			che += "def input_mode_debug():\n";
			che += "   global step_by_step\n";
			che += "   global debug_line_number\n";
			che += "   debugstring=' '\n";
			che += "   debugstring=raw_input(\"Debug(i(nside),n[ext],e[nd],g[o] line): \")\n";
			che += "   if debugstring=='e':\n";
			che += "      step_by_step=0\n";
			che += "      debug_line_number=-1\n";
			che += "      return 0\n";
			che += "   if debugstring=='i' or debugstring=='':\n";
			che += "      step_by_step=1\n";
			che += "      return 0\n";
			che += "   if debugstring=='n':\n";
			che += "      step_by_step=0\n";
			che += "      debug_line_number=debug_line_number+1\n";
			che += "      return 0\n";
			che += "   if debugstring[0]=='g':\n";
			che += "      step_by_step=0\n";
			che += "      try:\n";
			che += "        debug_line_number=int(debugstring[2:])\n";
			che += "        return 0\n";
			che += "      except:\n";
			che += "        return 0\n";
			che += "   return debugstring\n";
		}

		//on rajoute quelques retours chariot pour que python
		//s'y retrouve dans le numero de lignes dans le fichier
		for (int i = 0; i<LIGNES - 1; i++)
			che += 10;
		string ligne = "";
		string fonction_chaine;
		char ajout_global = NON;
		int num_def_ligne;
		while (!fichier.eof()) {
			if (c<32 && c != 9 && c != 10) {
				c = LectCrypte(fichier);
				continue;
			}

			//les tabulations sont virees...
			if (c == 9)
				c = 32;

			//On vient de rencontrer une ligne
			if (python_debug == OUI && num_module_python == num_fichier_python) {
				if (c == 10) {
					if (TestFinLigne(ligne) == OUI) {
						num_ligne++;
						c = LectCrypte(fichier);
						continue;
					}
					if (TestBlanc(ligne) == NON && ligne[0] == ' ' &&
						ligne.find("class ", 0) == -1 &&
						ligne.find("import ", 0) == -1 &&
						ligne.find("def ", 0) == -1 &&
						ligne.find("except", 0) == -1) {
						//ensuite on recupere tous les blancs en tete
						j = 0;
						string blc = "";
						while (ligne[j] == ' ' || ligne[j] == 9) {
							blc += ligne[j];
							j++;
						}
						//Les commentaires sont evidemment sautes
						if (ligne[j] == '#') {
							while (!fichier.eof() && c != 10)
								c = LectCrypte(fichier);
							ligne = "";
							if (c == 10)
								c = LectCrypte(fichier);
							num_ligne++;
							continue;
						}

						string sub;
						if (ajout_global == OUI) {
							ajout_global = NON;
							che += blc;
							che += "print '";
							sprintf_s(tampon, 1000, "%d:", num_def_ligne);
							che += tampon;
							sub = fonction_chaine;
							char* csub = (char*)sub.c_str();
							for (j = 0; j<sub.size(); j++) {
								if (csub[j] == '\'' || csub[j] == '\"')
									csub[j] = ' ';
							}
							che += sub;
							che += "'\n";
						}
						sub = ligne;
						char* csub = (char*)sub.c_str();
						for (j = 0; j<sub.size(); j++) {
							if (csub[j] == '\'' || csub[j] == '\"')
								csub[j] = ' ';
						}
						//On rajoute un print dans le tas
						che += blc;
						sprintf_s(tampon, 1000, "if test_mode_debug(%d)==1:\n", num_ligne);
						che += tampon;
						blc += " ";
						che += blc;
						che += "print '";
						sprintf_s(tampon, 1000, "%d:", num_ligne);
						che += tampon;
						che += sub;
						che += "'\n";
						che += blc;
						che += "debugstring=input_mode_debug()\n";
						che += blc;
						che += "while debugstring!=0:\n";
						che += blc;
						che += "  debugstring='print '+debugstring\n";
						che += blc;
						che += "  try:\n";
						che += blc;
						che += "    exec(debugstring)\n";
						che += blc;
						che += "  except:\n";
						che += blc;
						che += "    print 'Error...'\n";
						che += blc;
						che += "  debugstring=input_mode_debug()\n";
					}
					che += ligne;
					che += c;
					if (ligne.find("def ", 0) != -1) {
						ajout_global = OUI;
						fonction_chaine = ligne;
						num_def_ligne = num_ligne;
					}
					ligne = "";
					num_ligne++;
				}
				else
					ligne += c;
			}
			else
				che += c;

			c = LectCrypte(fichier);
		}

		if (initialise_premiere_fois == NON) {
			for (bd = 0; bd<noms.size(); bd++) {
				pFunc = PyDict_GetItemString(pDict_principal, noms[bd].c_str());
				if (pFunc != NULL) {
					*os << CHAINES[363] << Endl;
					return;
				}
			}
		}
		PyRun_SimpleString(che.c_str());
		pDict = pDict_principal;
	}
	else {
		//On importe le code python... sous la forme d'une importation
		//NOM contient le nom du module a importer
		try {
			((ifstream&)fichier).close();
		}
		catch (...) {
		}
		//L'importation se fait avec PyImport_Import
		//Le nom du module est le nom de notre fichier
		//sans son extension
		char* pt = strrchr(NOM, SEP);
		string chemin = "import sys\nimport traceback\n";

		if (pt != NULL) {
			strcpy_s(espace_module, XIPBUFFERSIZE, pt + 1);
			*pt = 0;
			string ch;
			if (SEP == '\\') {
				char* str = NOM;
				char* ptt = strchr(NOM, SEP);
				while (ptt != NULL) {
					*ptt = 0;
					ch += str;
					ch += "\\\\";
					*ptt = SEP;
					str = ptt + 1;
					ptt = strchr(str, SEP);
				}
				ch += str;
			}
			else
				ch = NOM;
			chemin += "sys.path.append(\"";
			chemin += ch;
			chemin += "\")\n";
			*pt = SEP;
		}
		else
			strcpy_s(espace_module, XIPBUFFERSIZE, NOM);

		//On retire le point
		pt = strchr(espace_module, '.');
		if (pt != NULL)
			*pt = 0;
		*flot_erreur << " module number:" << num_module_python << " (stored as " << espace_module << ")" << Endl;
		chemin += "import ";
		chemin += espace_module;
		chemin += "\n";
		PyRun_SimpleString(chemin.c_str());
		espace_local = PyDict_GetItemString(pDict_principal, espace_module);
		if (espace_local != NULL) {
			pDict = PyModule_GetDict(espace_local);
			chemin = "from xips import *\n";
			PyRun_String(chemin.c_str(), Py_file_input, pDict_principal, pDict);
		}
	}

	num_module_python++;
	string func = insere_xip_class();
	PyRun_String(func.c_str(), Py_file_input, pDict, pDict);

	for (bd = 0; bd<depends.dernier; bd++) {
		if (importation_python == NON)
			pFunc = PyDict_GetItemString(pDict_principal, noms[bd].c_str());
		else
			pFunc = PyDict_GetItemString(pDict, noms[bd].c_str());
		if (pFunc == NULL || !PyCallable_Check(pFunc))
			erreur(CHAINES[359]);

		ProcedurePython* tfp = new ProcedurePython(depends[bd], pFunc, pDict, (char*)noms[bd].c_str());
		tfp->nomfichier = NOM;
		PythonScript.ajoute(tfp);
	}
}

void retourInitialParseur(int precedent) {
	current_parser->vpython_echange->echange = NULL;

	if (precedent != index_current_parser) {
		current_parser->actif = NON;
		current_parser = gVecteurParseurs[precedent];
		index_current_parser = precedent;
		if (current_parser != NULL)
			current_parser->actif = OUI;
	}
}

GlobalParseur* initNouveauParseur(int parsnum) {
	if (parsnum != index_current_parser) {
		GlobalParseur* parseur = gVecteurParseurs[parsnum];
		if (parseur == NULL)
			return NULL;

		if (current_parser != NULL)
			current_parser->actif = NON;

		current_parser = parseur;
		parseur->actif = OUI;
		index_current_parser = parsnum;
	}
	return current_parser;
}

char extraitParseurCourant(PyObject* args) {
	int parsnum = index_current_parser;

	if (!PyArg_ParseTuple(args, "|i", &parsnum))
		return NON;

	GlobalParseur* parseur = initNouveauParseur(parsnum);
	if (parseur == NULL)
		return NON;

	return OUI;
}

PyObject* ConvertToPythonString(string s) {
	string res;
	s_latin_to_utf8(res, USTR(s));
	const char* buff = res.c_str();
	return PyUnicode_FromStringAndSize(buff, res.size());
}

void Ajoutenoeud(PyObject* liste, XipNode* node, map<int, PyObject*>& noeuds, int i) {
	PyObject* noeud = PyDict_New();
	PyList_Insert(liste, i, noeud);
	Py_DECREF(noeud);

	PyObject* pos = ConvertToPythonString(node->category.c_str());
	PyDict_SetItemString(noeud, "#pos", pos);
	Py_DECREF(pos);

	pos = ConvertToPythonLong(node->idnode);
	PyDict_SetItemString(noeud, "#id", pos);
	Py_DECREF(pos);
	noeuds[node->idnode] = noeud;

	int j;
	for (j = 0; j < node->features.size(); j++) {
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

	if (node->Type() == XIPLEAF) {
		XipLeaf* feuille = (XipLeaf*)node;
		pos = ConvertToPythonString(feuille->lemma.c_str());
		PyDict_SetItemString(noeud, "#lemma", pos);
		Py_DECREF(pos);
		pos = ConvertToPythonString(feuille->surface.c_str());
		PyDict_SetItemString(noeud, "#surface", pos);
		Py_DECREF(pos);
		return;
	}

	if (node->daughters.size()) {
		PyObject* liste = PyList_New(0);
		PyDict_SetItemString(noeud, "#children", liste);
		Py_DECREF(liste);
		for (j = 0; j < node->daughters.size(); j++)
			Ajoutenoeud(liste, node->daughters[j], noeuds, j);
	}
}

void Ajoutedependancepython(PyObject* liste, map<int, PyObject*>& noeuds, XipDependency* d, int i) {
	PyObject* dep = PyDict_New();
	PyList_Insert(liste, i, dep);
	Py_DECREF(dep);
	PyObject* pos = ConvertToPythonString(d->name.c_str());
	PyDict_SetItemString(dep, "#name", pos);
	Py_DECREF(pos);
	int j;
	bool stacks = false;
	for (j = 0; j < d->features.size(); j++) {
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
		for (j = 0; j < d->features.size(); j++) {
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

	for (j = 0; j < d->parameters.size(); j++) {
		pos = noeuds[d->parameters[j]->idnode];
		PyList_Insert(params, j, pos);
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
	for (j = 0; j < node->features.size(); j++) {
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
	for (j = 0; j < d->features.size(); j++) {
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
		for (j = 0; j < d->features.size(); j++) {
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
	PyObject* liste = PyList_New(0);
	PyList_Insert(unit, i, liste);
	Py_DECREF(liste);
	if (current_parser->TestOptions(AFFICHAGE_ARBRE_CHUNK)) {
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

PyObject* pythonclone(graphe* g) {
	PyObject* grap = PyDict_New();
	if (g == NULL || g->racine == NULL)
		return grap;

	PyObject* dico;

	//On range le nom du dictionaire
	PyObject* pelement = ConvertToPythonString(g->nom.c_str());
	PyDict_SetItemString(grap, "name", pelement);
	Py_DECREF(pelement);
	//Ensuite on construit un element pour chaque relation
	int i, j;
	VECTA<PyObject*> rels;
	VECTA<PyObject*> concs;
	PyObject* liste;
	for (i = 0; i < g->lesrelations.dernier; i++) {
		dico = PyDict_New();
		pelement = ConvertToPythonString("r");
		PyDict_SetItemString(dico, "type", pelement);
		Py_DECREF(pelement);
		pelement = ConvertToPythonString((char*)g->lesrelations[i]->hrel->semme);
		PyDict_SetItemString(dico, "id", pelement);
		Py_DECREF(pelement);
		rels.ajoute(dico);
	}

	for (i = 0; i < g->lesconcepts.dernier; i++) {
		dico = PyDict_New();
		pelement = ConvertToPythonString("c");
		PyDict_SetItemString(dico, "type", pelement);
		Py_DECREF(pelement);
		if (g->lesconcepts[i]->hconc != NULL)
			pelement = ConvertToPythonString((char*)g->lesconcepts[i]->hconc->semme);
		else
			pelement = ConvertToPythonString("#");
		PyDict_SetItemString(dico, "id", pelement);
		Py_DECREF(pelement);
		pelement = ConvertToPythonString(g->lesconcepts[i]->instance.c_str());
		PyDict_SetItemString(dico, "instance", pelement);
		Py_DECREF(pelement);
		liste = PyList_New(0);
		for (j = 0; j < g->lesconcepts[i]->relations.dernier; j++) {
			pelement = rels[g->lesconcepts[i]->relations[j]->index];
			PyList_Insert(liste, j, pelement);
			Py_DECREF(pelement);
		}
		PyDict_SetItemString(dico, "relations", liste);
		Py_DECREF(liste);
		concs.ajoute(dico);
	}

	for (i = 0; i < g->lesrelations.dernier; i++) {
		liste = PyList_New(0);
		for (j = 0; j < g->lesrelations[i]->concepts.dernier; j++) {
			pelement = concs[g->lesrelations[i]->concepts[j]->index];
			PyList_Insert(liste, j, pelement);
			Py_DECREF(pelement);
		}
		PyDict_SetItemString(rels[i], "concepts", liste);
		Py_DECREF(liste);
	}

	PyDict_SetItemString(grap, "root", concs[g->racine->index]);
	Py_DECREF(concs[g->racine->index]);
	return grap;
}

double TestFonctionPython::TraitementFormule(GlobalParseur* parseur,
	VECTA<Noeud*>& noeuds,
	char& err,
	VECTA<ResultatFonction*>* dep) {

	int iar;
	int i;
	char erreur = NON;
	Noeud* n;
	double res = OUI;
	VariableVector* vecteurchaine;
	VariableIntVector* vecteurint;
	VariableDictionnaire* vdict;
	VariableGraphe* vgraph;
	VariableVectorGraphe* vecteurgraph;


	//Tout d'abord on verifie que la dependance comprend le meme nombre d'argument, sinon
	//c'est dans tous les cas une erreur
	Dependance* depproc = proc_python->depend;
	NomProcedure* nomproc = NULL;
	if (depproc->Type() == OUI)
		nomproc = (NomProcedure*)depproc;

	if (depproc->arg.dernier != arg.dernier) {
		err = OUI;
		parseur->erreur_survenue = 79;
		sprintf_s(parseur->ajoute_info_erreur, 4096, " %s", nomproc->Fx->nom);
		return -1;
	}

	for (iar = 0; iar < arg.dernier; iar++) {
		int valarg = (int)depproc->arg[iar];
		if (valarg >= 0) {
			if (arg[iar] < 0) {
				err = OUI;
				parseur->erreur_survenue = 80;
				sprintf_s(parseur->ajoute_info_erreur, 4096, " %s", nomproc->Fx->nom);
				return -1;
			}
		}
		else {
			if (arg[iar] >= 0) {
				err = OUI;
				parseur->erreur_survenue = 80;
				sprintf_s(parseur->ajoute_info_erreur, 4096, " %s", nomproc->Fx->nom);
				return -1;
			}
		}
	}

	int dans_tuple = 0;
	//on cree un tuple d'appel python dans lequel on va ranger nos objets    
	PyObject* pcourant;
	PyObject* pelement;
	PyObject* liste;
	PyObject* pentree;

	map<string, string>::iterator iter;

	//On conserve l'index du parseur courant sous la forme d'une variable dont le nom
	//est: current_parseur et dont la valeur est l'index du parseur courant

	PyObject* pDict_principal = PyModule_GetDict(parseur->module_principal);
	pcourant = ConvertToPythonLong(parseur->iParseurCourant);

	char chrun[100];
	sprintf_s(chrun, 100, "XipNode.parseur=%d", parseur->iParseurCourant);
	PyRun_String(chrun, Py_file_input, pDict_principal, proc_python->pDico);
	sprintf_s(chrun, 100, "XipDependency.parseur=%d", parseur->iParseurCourant);
	PyRun_String(chrun, Py_file_input, pDict_principal, proc_python->pDico);
	PyDict_SetItemString(proc_python->pDico, "current_parseur", pcourant);

	double valeur;
	VECTA<PyObject*> py_listes;
	VECTA<PyObject*> py_args;

	//On range les noeuds au bon endroit, les parametres noeuds sont les premiers appeles
	for (iar = 0; iar < arg.dernier; iar++) {
		//On recupere l'argument
		if (arg[iar] >= 0) {//c'est un noeud
			n = noeuds[arg[iar]];
			//On le place dans notre tableau selon le numero de la variable presente
			//dans la procedure
			//Si la variable est nulle, il s'agit peut-etre d'une variable de retour
			if (n == NULL) {
				err = OUI;
				sprintf_s(parseur->ajoute_info_erreur, 4096, " %s: Missing node variable", nomproc->Fx->nom);
				return NON;
			}
			//On donne le numero de noeud comme parametre
			pcourant = ConvertToPythonLong(n->num_noeud);
		}
		else {//c'est une dependance
			int idep = arg[iar] * -1;
			if (dep != NULL && (*dep)[idep] != NULL)
				pcourant = ConvertToPythonLong((*dep)[idep]->index);
			else
				pcourant = ConvertToPythonLong(-1);
		}
		//On le range alors dans le tuple des appels python
		py_args.ajoute(pcourant);
	}


	//On initialise les valeurs de nos variables python
	for (iar = 0; nomproc != NULL && iar < nomproc->parametres.dernier; iar++) {

		pentree = NULL;
		//dans le cas d'un appel, il faut initialiser les variables avec les parametres
		//fourni en appel. Ces parametres sont stockes dans formules..        .
		if (nomproc->parametres[iar]->reinit == NON) {
			if (formules[iar] == NULL) {
				err = OUI;
				parseur->erreur_survenue = 58;
				sprintf_s(parseur->ajoute_info_erreur, 4096, " %s", nomproc->Fx->nom);
				return -1;
			}

			Variable* variab = formules[iar]->RenvoieVariableVecteur(parseur, noeuds, erreur, dep, RENVOIEVARIABLE);
			//variab est la variable fourni en appel dans la procedure
			//Cas particulier du traitement des listes et dictionnaires
			char type_utilise = NON;

			char typeerr = NON;
			//cas d'erreur
			if (variab->type == ENTIER) {
				if (variab->type != nomproc->parametres[iar]->type)
					typeerr = OUI;
				if (variab->sorte == VVECTEURINT) {
					if (variab->sorte != nomproc->parametres[iar]->sorte)
						typeerr = OUI;
				}
				else
				if (variab->sorte != VGLOBALEREINIT && variab->sorte != VGLOBALE)
					typeerr = OUI;
			}
			else {
				if (variab->sorte != nomproc->parametres[iar]->sorte)
					typeerr = OUI;
			}

			if (typeerr == OUI){
				err = OUI;
				parseur->erreur_survenue = 39;
				sprintf_s(parseur->ajoute_info_erreur, 4096, " %s", nomproc->Fx->nom);
				return -1;
			}

			switch (variab->sorte) {
			case VGRAPHE:
				vgraph = (VariableGraphe*)variab;
				liste = pythonclone(vgraph->g);
				py_args.ajoute(liste);
				type_utilise = OUI;
				break;
			case VVECTEURGRAPHE:
				vecteurgraph = (VariableVectorGraphe*)variab;
				liste = PyList_New(0);
				for (i = 0; i < vecteurgraph->graphes.dernier; i++) {
					pcourant = pythonclone(vecteurgraph->graphes[i]);
					PyList_Insert(liste, i, pcourant);
					Py_DECREF(pcourant);
				}
				py_args.ajoute(liste);
				type_utilise = OUI;
				break;
			case VPYTHONOBJET:
				if (((VariablePython*)variab)->echange != NULL) {
					pentree = ((VariablePython*)variab)->echange;
					py_args.ajoute(pentree);
					Py_INCREF(pentree);
					pentree = NULL;
				}
				else
					py_args.ajoute(ConvertToPythonLong(0));
				type_utilise = OUI;
				break;
			case VVECTEURCHAINE:
				//Dans le cas d'une liste, on boucle sur les valeurs
				vecteurchaine = (VariableVector*)variab;
				liste = PyList_New(0);
				py_listes.affecte(iar, liste);
				for (i = 0; i < vecteurchaine->chaine.dernier; i++) {
					pcourant = ConvertToPythonString(vecteurchaine->chaine[i]->c_str());
					PyList_Insert(liste, i, pcourant);
					Py_DECREF(pcourant);
				}
				py_args.ajoute(liste);
				type_utilise = OUI;
				break;
			case VVECTEURINT:
				//Dans le cas d'une liste, on boucle sur les valeurs
				vecteurint = (VariableIntVector*)variab;
				liste = PyList_New(0);
				py_listes.affecte(iar, liste);
				for (i = 0; i < vecteurint->valeurs.dernier; i++) {
					pcourant = PyFloat_FromDouble(vecteurint->valeurs[i]);
					PyList_Insert(liste, i, pcourant);
					Py_DECREF(pcourant);
				}
				py_args.ajoute(liste);
				type_utilise = OUI;
				break;
			case VDICTIONNAIRE:
				vdict = (VariableDictionnaire*)variab;
				liste = PyDict_New();
				py_listes.affecte(iar, liste);
				for (iter = vdict->dico.begin(); iter != vdict->dico.end(); iter++) {
					pelement = ConvertToPythonString((*iter).second.c_str());
					PyDict_SetItemString(liste, (*iter).first.c_str(), pelement);
					Py_DECREF(pelement);
				}
				py_args.ajoute(liste);
				type_utilise = OUI;
				break;
			}

			if (type_utilise == OUI)
				continue;

		}

		if (formules[iar] != NULL) {
			if (formules[iar]->TypeString()) {
				string buff;
				formules[iar]->TraitementFormuleString(parseur, noeuds, err, buff, dep);
				if (nomproc->parametres[iar] == NULL)
					return NON;
				pcourant = ConvertToPythonString((char*)buff.c_str());
			}
			else {
				res = formules[iar]->TraitementFormule(parseur, noeuds, err, dep);
				if (nomproc->parametres[iar]->type == ENTIER)
					pcourant = ConvertToPythonLong((long)res);
				else
					pcourant = PyFloat_FromDouble(res);
			}
		}
		else {
			if (nomproc->parametres[iar]->sorte == VCHAINE)
				pcourant = ConvertToPythonString("");
			else
				pcourant = ConvertToPythonLong(0);
		}

		if (nomproc->parametres[iar]->reinit == NON) {
			string dom = nomproc->parametres[iar]->nom;
			int idom = dom.find("::");
			if (idom != string::npos)
				dom = dom.substr(0, idom);
			pentree = ConvertToPythonString(STR(dom));
			PyDict_SetItem(proc_python->pDico, pentree, pcourant);
			py_listes.affecte(iar, pentree);
		}
		else
			py_args.ajoute(pcourant);

	}

	int tail;
	PyObject* procArgs = PyTuple_New(py_args.dernier);

	//On range les arguments dans un tuple
	for (iar = 0; iar < py_args.dernier; iar++)
		PyTuple_SetItem(procArgs, iar, py_args[iar]);

	PyObject* presultat = PyObject_CallObject(proc_python->pFunction, procArgs);
	PyObject* perreur = PyErr_Occurred();
	if (perreur != NULL) {
		err = OUI;
		parseur->erreur_survenue = 70;
		if (parseur->TestOptions(AFFICHAGE_ERREUR_EXECUTION)) {
			ostream* os = &cout;
			if (parseur->OS == NULL) {
				PyObject *ptype;
				PyObject *pvalue;
				PyObject *ptraceback;
				PyErr_Fetch(&ptype, &pvalue, &ptraceback);
				PyErr_Clear();
				//os=parseur->OS;      
				*os << "Error in file:" << proc_python->nomfichier << Endl;

				if (pvalue != NULL && PyUnicode_Check(pvalue)) {
					PyObject* perr = PyObject_Str(pvalue);
					*os << Endl << "In python procedure \"" << proc_python->nom << "\": " << PyAsString(perr) << Endl;
					Py_DECREF(perr);
				}

				if (ptraceback != NULL) {
					//On passe dans ce cas par le module traceback importe par XIP...					
					PyObject* espace_trace = PyDict_GetItemString(pDict_principal, "traceback");
					if (espace_trace != NULL) {
						PyObject* pDict_traceback = PyModule_GetDict(espace_trace);
						//On peut alors faire appel a la fonction: format_list
						PyObject* formatlist = PyDict_GetItemString(pDict_traceback, "format_tb");

						PyObject* formatlistArgs = PyTuple_New(1);
						//On y range notre objet
						PyTuple_SetItem(formatlistArgs, 0, ptraceback);
						//On appelle notre fonction
						PyObject* plistetraceback = PyObject_CallObject(formatlist, formatlistArgs);
						//Le resultat est dans plistetraceback, une liste de chaine
						if (plistetraceback != NULL) {
							if (PyList_Check(plistetraceback) == 1) {
								//On recupere la taille de cette liste
								tail = PyList_GET_SIZE(plistetraceback);
								for (i = 0; i < tail; i++) {
									//Puis chacun des objets, kj est l'index de l'element
									pelement = PyList_GetItem(plistetraceback, i);
									//On le range dans notre vecteur
									if (pelement != NULL && PyUnicode_Check(pelement))
										*os << PyAsString(pelement);
								}
							}
							Py_DECREF(plistetraceback);
						}
						Py_DECREF(formatlistArgs);
					}
				}

				if (ptype != NULL)
					Py_DECREF(ptype);
				if (pvalue != NULL)
					Py_DECREF(pvalue);
				if (ptraceback != NULL)
					Py_DECREF(ptraceback);
				*os << Endl;
			}
			else {
				cout << "Error in file:" << proc_python->nomfichier << Endl;
				PyErr_Print();
				cout << "In python procedure \"" << proc_python->nom << Endl << Endl;
				PyErr_Clear();
			}
		}
		for (iar = 0; nomproc != NULL && iar < nomproc->parametres.dernier; iar++) {
			//Si c'est une variable globale
			if (nomproc->parametres[iar]->reinit == NON) {
				pcourant = py_listes[iar];
				if (pcourant != NULL) {
					//variab est la variable fourni en appel dans la procedure
					Variable* variab = formules[iar]->RenvoieVariableVecteur(parseur, noeuds, erreur, dep, RENVOIEVARIABLE);
					//si nous n'avons pas donne de parametre en entree pour cet appel, 
					//on repart sans rien faire. Si de plus, le type de la variable est different de
					//celui attendu, c'est aussi une erreur.
					switch (variab->sorte) {
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
		if (presultat != NULL)
			Py_DECREF(presultat);
		Py_DECREF(procArgs);
		return -1;
	}

	for (iar = 0; nomproc != NULL && iar < nomproc->parametres.dernier; iar++) {
		//Si c'est une variable globale
		if (nomproc->parametres[iar]->reinit == NON) {
			string reschaine;
			string index;
			PyObject* key;
			PyObject* value;
#if (defined(APPLE) | defined(ARCH64)) 
			Py_ssize_t pos;
#else
			int pos;
#endif            
			pcourant = py_listes[iar];
			if (pcourant != NULL) {
				//variab est la variable fourni en appel dans la procedure
				Variable* variab = formules[iar]->RenvoieVariableVecteur(parseur, noeuds, erreur, dep, RENVOIEVARIABLE);
				//si nous n'avons pas donne de parametre en entree pour cet appel, 
				//on repart sans rien faire. Si de plus, le type de la variable est different de
				//celui attendu, c'est aussi une erreur.
				switch (variab->sorte) {
				case VGLOBALEREINIT:
				case VGLOBALE:
					//Il nous faut recuperer notre variable
					variab->utilisee = OUI;
					valeur = 0;
					pelement = PyDict_GetItem(proc_python->pDico, pcourant);
					if (PyFloat_Check(pelement))
						valeur = PyFloat_AsDouble(pelement);
					else
					if (PyCheck_Number(pelement))
						valeur = PyAsNumber(pelement);
					((VariableGlobale*)variab)->valeur = valeur;
					Py_DECREF(pcourant);
					PyDict_DelItem(proc_python->pDico, pcourant);
					break;
					//On remet a jour les valeurs
				case VVECTEURCHAINE:
					vecteurchaine = (VariableVector*)variab;
					vecteurchaine->raz();
					//Puis on rajoute tous les elements de notre liste
					if (PyList_Check(pcourant) == 1) {
						//On recupere la taille de cette liste
						tail = PyList_GET_SIZE(pcourant);
						for (i = 0; i < tail; i++) {
							//Puis chacun des objets, kj est l'index de l'element
							pelement = PyList_GetItem(pcourant, i);
							//On le range dans notre vecteur
							if (pelement != NULL && PyUnicode_Check(pelement))
								reschaine = PyAsString(pelement);
							else
								reschaine = "";
							vecteurchaine->AffecteValeurStringIndex(new string(reschaine), i);
						}
					}
					break;
				case VVECTEURINT:
					vecteurint = (VariableIntVector*)variab;
					vecteurint->raz();
					//Puis on rajoute tous les elements de notre liste
					if (PyList_Check(pcourant) == 1) {
						//On recupere la taille de cette liste
						tail = PyList_GET_SIZE(pcourant);
						for (i = 0; i < tail; i++) {
							//Puis chacun des objets, kj est l'index de l'element
							pelement = PyList_GetItem(pcourant, i);
							//On le range dans notre vecteur
							valeur = 0;
							if (pelement != NULL) {
								if (PyFloat_Check(pelement))
									valeur = PyFloat_AsDouble(pelement);
								else
								if (PyCheck_Number(pelement))
									valeur = PyAsNumber(pelement);
							}
							vecteurint->valeurs.affecte(i, valeur);
						}
					}
					break;
				case VDICTIONNAIRE:
					vdict = (VariableDictionnaire*)variab;
					vdict->raz();
					pos = 0;
					while (PyDict_Next(pcourant, &pos, &key, &value)) {
						if (key != NULL && value != NULL && PyUnicode_Check(key) && PyUnicode_Check(value)) {
							index = PyAsString(key);
							reschaine = PyAsString(value);
							vdict->AffecteValeurStringIndexChaine(new string(reschaine), index);
						}
					}
					break;
				case VCHAINE:
					pelement = PyDict_GetItem(proc_python->pDico, pcourant);
					if (PyUnicode_Check(pelement))
						((VariableString*)variab)->chaine = PyAsString(pelement);
					Py_DECREF(pcourant);
					PyDict_DelItem(proc_python->pDico, pcourant);
					break;
				}
			}
		}
	}

	//On peut enfin liberer les arguments
	Py_DECREF(procArgs);
	res = 0;
	if (presultat != NULL) {
		if (PyFloat_Check(presultat))
			res = PyFloat_AsDouble(presultat);
		else {
			if (PyCheck_Number(presultat))
				res = PyAsNumber(presultat);
			else {
				if (PyLong_Check(presultat))
					res = PyLong_AsLong(presultat);
			}
		}
		Py_DECREF(presultat);
	}

	return res;
}

//Une fonction de base de python pour pouvoir jouer avec les noeuds
extern "C" {


	static PyObject* xipsError;

	static PyObject* xips_loadgrm(PyObject* self, PyObject* args) {
		char* grmfile;

		if (!PyArg_ParseTuple(args, "s", &grmfile)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		ostringstream os;
		flot_erreur = &os;
		int ipar = -1;
		try {
			ipar = XipGrmFile(grmfile, 1, 0);
		}
		catch (XIPError* msg) {
			flot_erreur = &cerr;
			PyErr_SetString(PyExc_AttributeError, msg->message.c_str());
			return ConvertToPythonString(msg->message.c_str());
		}

		flot_erreur = &cerr;
		return ConvertToPythonLong(ipar);
	}

	static PyObject* xips_freegrm(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;

		if (!PyArg_ParseTuple(args, "i", &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}
		XipFreeGrammar(parsnum);
		return ConvertToPythonLong(0);
	}


	static PyObject* xips_current_rule(PyObject* self, PyObject* args) {
		int precedent = index_current_parser;

		if (extraitParseurCourant(args) == NON) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return Py_BuildValue("[iiisssll]", -1, -1, -1, "", "", "", -1, -1);
		}

		if (current_parser->reglecourante == NULL) {
			PyErr_SetString(PyExc_AttributeError, "No Current Rule");
			return Py_BuildValue("[iiisssll]", -1, -1, -1, "", "", "", -1, -1);
		}

		return Py_BuildValue("[iiisssll]",
			current_parser->reglecourante->identificateur,
			current_parser->reglecourante->numero,
			current_parser->reglecourante->couche,
			champs_anglais[current_parser->reglecourante->label->type],
			current_parser->reglecourante->label->etiquette,
			current_parser->reglecourante->label->dr->nom_fichier,
			current_parser->reglecourante->label->dr->position,
			current_parser->reglecourante->label->dr->ligne);
	}

	static PyObject* xips_current_rule_text(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int display;

		if (!PyArg_ParseTuple(args, "i|i", &display, &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonString("");
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);

		if (parseur == NULL || current_parser->reglecourante == NULL) {
			PyErr_SetString(PyExc_AttributeError, "No Current Rule");
			return ConvertToPythonString("");
		}

		ostringstream os;
		current_parser->FormatRuleText(os, current_parser->reglecourante->identificateur, display);
		return ConvertToPythonString(os.str().c_str());
	}


	static PyObject* xips_setexechange(PyObject* self, PyObject* args) {

		PyObject* objet;
		char* nomvariable;
		if (!PyArg_ParseTuple(args, "sO", &nomvariable, &objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		etat* e = current_parser->VariablesNoms.recherche(nomvariable);
		if (e == NULL || e->valeur == NULL) {
			PyErr_SetString(PyExc_AttributeError, "Unknown variable");
			return ConvertToPythonLong(-2);
		}

		Variable* v = (Variable*)e->valeur;
		if (v->sorte != VPYTHONOBJET) {
			PyErr_SetString(PyExc_AttributeError, "This object is not a Python object");
			return ConvertToPythonLong(-3);
		}

		Py_INCREF(objet);
		VariablePython* vp = (VariablePython*)v;
		if (vp->echange != NULL)
			Py_DECREF(vp->echange);
		vp->echange = objet;
		return ConvertToPythonLong(0);
	}

	static PyObject* xips_setdebug(PyObject* self, PyObject* args) {

		GlobalParseur* parseur = current_parser;
		if (parseur == NULL)
			return ConvertToPythonLong(-2);
		int numligne, nummodule;
		if (!PyArg_ParseTuple(args, "ii", &nummodule, &numligne)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		if (nummodule == -1 && numligne == -1)
			parseur->python_debug = NON;
		else
			parseur->python_debug = OUI;

		parseur->num_python_ligne = numligne;
		parseur->num_fichier_python = nummodule;

		return ConvertToPythonLong(0);
	}

	static PyObject* xips_reloadgrm(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		if (!PyArg_ParseTuple(args, "i", &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonString("Please provide a grammar handle");
		}

		try {
			XipReloadGrammar(parsnum, 1, NULL);
		}
		catch (XIPError* msg) {
			PyErr_SetString(PyExc_AttributeError, msg->message.c_str());
			return ConvertToPythonString(msg->message.c_str());
		}

		return ConvertToPythonLong(0);
	}

	static PyObject* xips_parsestring(PyObject* self, PyObject* args) {
		int precedent = index_current_parser;
		int parsnum = index_current_parser;

		int nbelement = PyTuple_Size(args);
		if (nbelement != 2 && nbelement != 3) {
			PyErr_SetString(PyExc_AttributeError, "Wrong number of parameters");
			return ConvertToPythonLong(-1);
		}

		PyObject* pelement = PyTuple_GetItem(args, 0);
		if (PyLong_Check(pelement) != 1) {
			PyErr_SetString(PyExc_AttributeError, "First element should be a integer");
			return ConvertToPythonLong(-1);
		}

		parsnum = PyLong_AsLong(pelement);
		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(-1);

		pelement = PyTuple_GetItem(args, 1);
		string phrase;
		if (!PythonVersString(pelement, phrase)) {
			PyErr_SetString(PyExc_AttributeError, "Second element should be a string");
			return ConvertToPythonLong(-1);
		}

		PyObject* objet = NULL;
		if (nbelement == 3)
			objet = PyTuple_GetItem(args, 2);

		XipResult* xip = new XipResult;
		current_parser->vpython_echange->echange = objet;
		XipParseString(STR(phrase), parsnum, xip, 1);
		PyObject* resultat = PyList_New(0);
		Creeresultatpython(parseur, resultat, xip);
		delete xip;
		retourInitialParseur(precedent);
		return resultat;
	}

	static PyObject* xips_stringtagging(PyObject* self, PyObject* args) {
		int precedent = index_parseur_courant;
		int parsnum = index_parseur_courant;

		int nbelement = PyTuple_Size(args);
		if (nbelement != 2) {
			PyErr_SetString(PyExc_AttributeError, "Wrong number of parameters");
			return ConvertToPythonLong(-1);
		}

		PyObject* pelement = PyTuple_GetItem(args, 0);
		if (PyCheck_Number(pelement) != 1) {
			PyErr_SetString(PyExc_AttributeError, "First element should be a integer");
			return ConvertToPythonLong(-1);
		}

		parsnum = PyLong_AsLong(pelement);
		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(-1);


		pelement = PyTuple_GetItem(args, 1);
		string phrase;
		if (!PythonVersString(pelement, phrase)) {
			PyErr_SetString(PyExc_AttributeError, "Second element should be a string");
			return ConvertToPythonLong(-1);
		}

		XipAddFlagDisplayMode(parsnum, AFFICHAGE_ARBRE_CHUNK | AFFICHAGE_COMPLET | AFFICHAGE_ENTREE, 80);

		char vtag = parseur->DESAMBIGUISATION;
		char vtagd = parseur->DESAMBIGUISATIONDefini;
		char vtg = parseur->TAGGING;

		parseur->Desambiguisation(OUI);
		parseur->Tagging(OUI);


		XipResult* xip = new XipResult;
		parseur->vpython_echange->echange = NULL;
		XipParseString(STR(phrase), parsnum, xip, OUI);
		PyObject* resultat = PyList_New(0);
		for (size_t st = 0; st < xip->sentences.size(); st++) {
			PyObject* alist = PyList_New(0);
			PyList_Insert(resultat, st, alist);
			Py_DECREF(alist);
			for (int i = 0; i < xip->sentences[st]->leaves.size(); i++)
				Ajouteunnoeud(alist, xip->sentences[st]->leaves[i], i);
		}

		delete xip;

		parseur->DESAMBIGUISATION = vtag;
		parseur->DESAMBIGUISATIONDefini = vtagd;
		parseur->TAGGING = vtg;

		retourInitialParseur(precedent);
		return resultat;
	}

	static PyObject* xips_parsedependencystring(PyObject* self, PyObject* args) {
		char* phrase;
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		PyObject* objet;
		if (!PyArg_ParseTuple(args, "isO", &parsnum, &phrase, &objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}


		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(-1);

		current_parser->vpython_echange->echange = objet;

		ostringstream os;
		int res = XipGenerateFromString(phrase, parsnum, &os);
		retourInitialParseur(precedent);
		if (res == 0)
			return ConvertToPythonString(os.str().c_str());
		else {
			PyErr_SetString(PyExc_AttributeError, affichagemessageerreur[res]);
			return ConvertToPythonString(affichagemessageerreur[res]);
		}
	}

	static PyObject* xips_generated(PyObject* self, PyObject* args) {
		int precedent = index_current_parser;
		int parsnum = index_current_parser;

		if (extraitParseurCourant(args) == NON)  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		string buff;
		current_parser->parcoursgeneration(buff);

		return ConvertToPythonString(buff.c_str());
	}

#ifdef XIPLIBXML
	static PyObject* xips_parsexmlstring(PyObject* self, PyObject* args) {
		char* phrase;
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int depth;

		PyObject* objet;
		if (!PyArg_ParseTuple(args, "isiO", &parsnum, &phrase, &depth, &objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(-1);


		//Py_INCREF(objet);
		current_parser->vpython_echange->echange = objet;
		ostringstream os;
		XipParseStringXMLOS(phrase, parsnum, &os, depth, parseur->XML, OUI);
		retourInitialParseur(precedent);
		return ConvertToPythonString(os.str().c_str());
	}

	static PyObject* xips_parsexmlfile(PyObject* self, PyObject* args) {
		char* filename;
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int depth = 1;

		PyObject* objet;
		if (!PyArg_ParseTuple(args, "isiO", &parsnum, &filename, &depth, &objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(-1);


		//Py_INCREF(objet);
		current_parser->vpython_echange->echange = objet;
		//ostringstream os;
		XipParseFileXMLOS(filename, parsnum, NULL, depth, parseur->XML);
		retourInitialParseur(precedent);
		return ConvertToPythonString("end");
	}
#endif

	static PyObject* xips_version(PyObject* self, PyObject* args) {
		char version[1000];

		sprintf_s(version, 1000, "XIP: Xerox Incremental Parser %s (2000-2007) ", VERSION_XIP_BUILD);
		return ConvertToPythonString(version);
	}

	static PyObject* xips_parsefile(PyObject* self, PyObject* args) {
		char* filename;
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		PyObject* objet;
		if (!PyArg_ParseTuple(args, "isO", &parsnum, &filename, &objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(-1);


		//Py_INCREF(objet);
		current_parser->vpython_echange->echange = objet;
		//ostringstream os;
		XipParseFileOS(filename, parsnum, NULL, parseur->XML);
		retourInitialParseur(precedent);
		return ConvertToPythonString("end");
	}

	static PyObject* xips_parsedependencyfile(PyObject* self, PyObject* args) {
		char* filename;
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		PyObject* objet;
		if (!PyArg_ParseTuple(args, "isO", &parsnum, &filename, &objet)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(-1);


		current_parser->vpython_echange->echange = objet;

		int res = XipGenerateFromFile(filename, parsnum, NULL);
		retourInitialParseur(precedent);
		if (res == 0)
			return ConvertToPythonString("end");
		else {
			PyErr_SetString(PyExc_AttributeError, affichagemessageerreur[res]);
			return ConvertToPythonString(affichagemessageerreur[res]);
		}
	}

	static PyObject* xips_xipnode(PyObject* self, PyObject* args) {
		int node;
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		if (!PyArg_ParseTuple(args, "i|i", &node, &parsnum))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return Py_BuildValue("[issssiiiiiiilllls]", 0,
				"NONE", "NONE", "NONE", "[NONE:NONE]", -1, -1, -1, -1, -1, 0, 0, -1, -1, -1, -1, "");
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL) {
			return Py_BuildValue("[issssiiiiiiilllls]", 0,
				"NONE", "NONE", "NONE", "[NONE:NONE]", -1, -1, -1, -1, -1, 0, 0, -1, -1, -1, -1, "");
		}

		Noeud* n = NULL;
		//if ((*parseur->TasDeListes)[node]!=NULL)
		//    n=&parseur->TasDeListes->vecteur[node]->N;

		if (parseur->NoeudsTemporaires[node] != NULL)
			n = &parseur->NoeudsTemporaires[node]->N;


		if (n == NULL) {
			PyErr_SetString(PyExc_IndexError, "This index does not match any syntactic node");
			return Py_BuildValue("[issssiiiiiiilllls]", 0,
				"NONE", "NONE", "NONE", "[NONE:NONE]", -1, -1, -1, -1, -1, 0, 0, -1, -1, -1, -1, "");
		}

		string rpos;
		string rsurface;
		string rlemme;
		string rtraits;
		string rinitialfeatures;
		int rpere = -1;
		int rfils = -1;
		int rdernier = -1;
		int rsuivant = -1;
		int rprecedent = -1;
		PyObject* listeinter;
		PyObject* laliste = NULL;
		//dans le cas standard, il s'agit d'une simple concatenation dans les chaines pour les noeuds lexicaux
		if (parseur->fullpythonlexical == NON || n->source->voc == NULL || n->source->voc->nbX == 1) {
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

			listeinter = Py_BuildValue("[issssiiiiiiilllls]",
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

			if (parseur->fullpythonlexical == OUI) {
				laliste = PyList_New(0);
				PyList_Insert(laliste, 0, listeinter);
				Py_DECREF(listeinter);
			}
			else
				laliste = listeinter;
		}
		else {//sinon, on cree une liste de noeuds lexicaux
			int choix = n->source->choisie;

			laliste = PyList_New(0);
			for (int i = 0; i < n->source->voc->nbX; i++) {
				n->source->choisie = i;
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

				listeinter = Py_BuildValue("[issssiiiiiiilllls]",
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
				PyList_Insert(laliste, i, listeinter);
				Py_DECREF(listeinter);
			}
			n->source->choisie = choix;
		}
		return laliste;
	}


	static PyObject* xips_xipnodes(PyObject* self, PyObject* args) {
		char* node;
		int i, parsnum = index_current_parser;
		int precedent = index_current_parser;

		PyObject* liste = PyList_New(0);
		if (!PyArg_ParseTuple(args, "s|i", &node, &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return liste;
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return liste;

		Categorie* c = parseur->RetrouveCat(node);
		if (c == NULL) {
			PyErr_SetString(PyExc_IndexError, "Unknown POS");
			return liste;
		}

		for (i = 0; i < parseur->IndexParCat[c->X.val].dernier; i++) {
			PyObject* pcourant = ConvertToPythonLong(parseur->IndexParCat[c->X.val][i]->N.num_noeud);
			PyList_Insert(liste, i, pcourant);
			Py_DECREF(pcourant);
		}

		return liste;
	}

	static PyObject* xips_xiplexicalnodes(PyObject* self, PyObject* args) {
		int i;
		int precedent = index_current_parser;

		PyObject* liste = PyList_New(0);
		if (extraitParseurCourant(args) == NON) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return liste;
		}

		for (i = 0; i < current_parser->NoeudsLexicaux.dernier; i++) {
			PyObject* pcourant = ConvertToPythonLong(current_parser->NoeudsLexicaux[i]->num_noeud);
			PyList_Insert(liste, i, pcourant);
			Py_DECREF(pcourant);
		}

		return liste;
	}

	static PyObject* xips_xipfictivenodes(PyObject* self, PyObject* args) {
		int i;
		int precedent = index_current_parser;

		PyObject* liste = PyList_New(0);
		if (extraitParseurCourant(args) == NON) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return liste;
		}

		Noeud* n = NULL;

		for (i = 0; i < current_parser->NoeudsDependance.dernier; i++) {
			PyObject* pcourant = ConvertToPythonLong(current_parser->NoeudsDependance[i]->N.num_noeud);
			PyList_Insert(liste, i, pcourant);
			Py_DECREF(pcourant);
		}

		return liste;
	}

	static PyObject* xips_xipinternalstring(PyObject* self, PyObject* args) {

		int precedent = index_current_parser;
		if (extraitParseurCourant(args) == NON) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonString("");
		}

		if (current_parser->donneesATraiter == NULL) {
			PyErr_SetString(PyExc_IndexError, "No string to process");
			return ConvertToPythonString("");
		}

		return ConvertToPythonString((char*)current_parser->donneesATraiter);
	}

	static PyObject* xips_dependency(PyObject* self, PyObject* args) {
		int dep;
		int i, parsnum = index_current_parser;
		int precedent = index_current_parser;

		if (!PyArg_ParseTuple(args, "i|i", &dep, &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return Py_BuildValue("[ssi]", "NONE", "[NONE:NONE]", -1);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL) {
			return Py_BuildValue("[ssi]",
				"NONE", "[NONE:NONE]", -1);
		}

		if (parseur->INTERFONCTIONS[0] == NULL) {
			PyErr_SetString(PyExc_IndexError, "Unknown dependency");
			return Py_BuildValue("[ssi]",
				"NONE", "[NONE:NONE]", -1);
		}

		ResultatFonction* rf = (*parseur->INTERFONCTIONS[0])[dep];
		if (rf == NULL) {
			PyErr_SetString(PyExc_IndexError, "Unknown dependency");
			return Py_BuildValue("[ssi]",
				"NONE", "[NONE:NONE]", -1);
		}


		string traits;
		if (rf->traits != NULL)
			rf->traits->TraitsEnChaineString(parseur, traits, parseur->FILTREFONCTION);

		if (parseur->TestOptions(AFFICHAGE_NUMERO_DEPENDANCE)) {
			char num[10];
			traits += "<";
			for (i = 0; i < rf->numeros.dernier; i++) {
				sprintf_s(num, 10, "%d", parseur->VlabelVecteurIndex[rf->numeros[i]]->numero);
				if (i)
					traits += ",";
				traits += num;
			}
			traits += ">";
		}

		for (i = 0; i < rf->stack.dernier; i++) {
			if (i)
				traits += ",";
			else {
				traits = traits.substr(0, traits.size() - 1);
				if (traits != "[")
					traits += ",";
			}
			traits += "stack:'";
			traits += rf->stack[i];
			traits += "'";
		}

		if (rf->stack.dernier)
			traits += "]";


		PyObject* pcourant;
		PyObject* liste = PyList_New(0);
		int params = 0;

		pcourant = ConvertToPythonString(rf->Fx->nom);
		PyList_Insert(liste, params++, pcourant);
		Py_DECREF(pcourant);

		pcourant = ConvertToPythonString(traits.c_str());
		PyList_Insert(liste, params++, pcourant);
		Py_DECREF(pcourant);

		for (i = 0; i < rf->arg.dernier; i++) {
			pcourant = ConvertToPythonLong(rf->arg[i]->num_noeud);
			PyList_Insert(liste, params++, pcourant);
			Py_DECREF(pcourant);
		}

		return liste;
	}

	static PyObject* xips_setfloatvariable(PyObject* self, PyObject* args) {
		float valeur;
		char* variable;
		int grm;


		if (!PyArg_ParseTuple(args, "isf", &grm, &variable, &valeur))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		string var = variable;
		int r = XipSetVariable(grm, var, valeur);
		return ConvertToPythonLong(r);
	}

	static PyObject* xips_setintvariable(PyObject* self, PyObject* args) {
		int valeur;
		char* variable;
		int grm;

		if (!PyArg_ParseTuple(args, "isi", &grm, &variable, &valeur)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		string var = variable;
		int r = XipSetVariable(grm, var, (float)valeur);
		return ConvertToPythonLong(r);
	}

	static PyObject* xips_setstringvariable(PyObject* self, PyObject* args) {
		char* valeur;
		char* variable;
		int grm;

		if (!PyArg_ParseTuple(args, "iss", &grm, &variable, &valeur)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		string var = variable;
		string value = valeur;
		int r = XipSetStringVariable(grm, var, value);
		return ConvertToPythonLong(r);
	}


	static PyObject* xips_setdisplay(PyObject* self, PyObject* args) {
		TVecteur mode;
		int grm;

		if (!PyArg_ParseTuple(args, "iL", &grm, &mode)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		XipSetDisplayMode(grm, mode, 80);

		return ConvertToPythonLong(0);
	}

	static PyObject* xips_adddisplay(PyObject* self, PyObject* args) {
		TVecteur mode;
		int grm;

		if (!PyArg_ParseTuple(args, "iL", &grm, &mode)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		XipAddFlagDisplayMode(grm, mode, 80);

		return ConvertToPythonLong(0);
	}

	static PyObject* xips_testdisplay(PyObject* self, PyObject* args) {
		TVecteur mode;
		int grm;

		if (!PyArg_ParseTuple(args, "iL", &grm, &mode)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		if (XipTestFlagDisplayMode(grm, mode) == 1)
			return ConvertToPythonLong(1);

		return ConvertToPythonLong(0);
	}

	static PyObject* xips_currentgrammarindex(PyObject* self, PyObject* args) {
		if (current_parser == NULL)
			return ConvertToPythonLong(-1);

		return ConvertToPythonLong(current_parser->iParseurCourant);
	}


	static PyObject* xips_removedisplay(PyObject* self, PyObject* args) {
		TVecteur mode;
		int grm;

		if (!PyArg_ParseTuple(args, "iL", &grm, &mode)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		XipRemoveFlagDisplayMode(grm, mode);

		return ConvertToPythonLong(0);
	}


	static PyObject* xips_dependencies(PyObject* self, PyObject* args) {
		char* dep;
		int i, parsnum = index_current_parser;
		int precedent = index_current_parser;
		PyObject* liste = PyList_New(0);

		if (!PyArg_ParseTuple(args, "s|i", &dep, &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return liste;
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return liste;


		//On recupere donc nos dependances;
		Categorie* cdep = parseur->RetrouveFonc(dep);
		if (cdep == NULL)
			return liste;

		PyObject* pcourant;


		int idep = cdep->X.val;
		ResultatFonction* rf;
		if (parseur->INTERFONCTIONS[idep] != NULL) {
			for (i = 0; i < parseur->INTERFONCTIONS[idep]->dernier; i++) {
				rf = (*parseur->INTERFONCTIONS[idep])[i];
				if (rf != NULL && rf->Fx != NULL) {
					pcourant = ConvertToPythonLong(rf->index);
					PyList_Insert(liste, i, pcourant);
					Py_DECREF(pcourant);
				}
			}
		}

		return liste;
	}

	static PyObject* xips_dependencyonfirstnode(PyObject* self, PyObject* args) {
		int node, parsnum = index_current_parser;
		int precedent = index_current_parser;
		PyObject* liste = PyList_New(0);

		if (!PyArg_ParseTuple(args, "i|i", &node, &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return liste;
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return liste;

		if (parseur->NoeudsTemporaires[node] == NULL)
			return liste;

		PyObject* pcourant;

		if (parseur->PREMIERFONCTIONS[node] != NULL) {
			for (int i = 0; i < parseur->PREMIERFONCTIONS[node]->dernier; i++) {
				if ((*parseur->PREMIERFONCTIONS[node])[i] != NULL) {
					pcourant = ConvertToPythonLong((*parseur->PREMIERFONCTIONS[node])[i]->index);
					PyList_Insert(liste, i, pcourant);
					Py_DECREF(pcourant);
				}
			}
		}

		return liste;
	}

	static PyObject* xips_dependencyonsecondnode(PyObject* self, PyObject* args) {
		int node, parsnum = index_current_parser;
		int precedent = index_current_parser;

		PyObject* liste = PyList_New(0);

		if (!PyArg_ParseTuple(args, "i|i", &node, &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return liste;
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return liste;

		if (parseur->NoeudsTemporaires[node] == NULL)
			return liste;

		PyObject* pcourant;

		if (parseur->SECONDFONCTIONS[node] != NULL) {
			for (int i = 0; i < parseur->SECONDFONCTIONS[node]->dernier; i++) {
				if ((*parseur->SECONDFONCTIONS[node])[i] != NULL) {
					pcourant = ConvertToPythonLong((*parseur->SECONDFONCTIONS[node])[i]->index);
					PyList_Insert(liste, i, pcourant);
					Py_DECREF(pcourant);
				}
			}
		}
		return liste;
	}

	static PyObject* xips_get_sourcecharset(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		if (!PyArg_ParseTuple(args, "i", &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonString("");
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL) {
			PyErr_SetString(PyExc_AttributeError, "Unknown Grammar");
			return ConvertToPythonString("");
		}
		encodingType ty = XipGetSourceCharset(parsnum);
		if (ty == LATIN_ENCODING)
			return ConvertToPythonString("latin-1");
		return ConvertToPythonString("utf-8");
	}

	static PyObject* xips_get_grammarcharset(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		if (!PyArg_ParseTuple(args, "i", &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonString("");
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL) {
			PyErr_SetString(PyExc_AttributeError, "Unknown Grammar");
			return ConvertToPythonString("");
		}
		encodingType ty = XipGetGrammarCharset(parsnum);
		if (ty == LATIN_ENCODING)
			return ConvertToPythonString("latin-1");
		return ConvertToPythonString("utf-8");
	}

	static PyObject* xips_get_fstcharset(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		if (!PyArg_ParseTuple(args, "i", &parsnum)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonString("");
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL) {
			PyErr_SetString(PyExc_AttributeError, "Unknown Grammar");
			return ConvertToPythonString("");
		}
		if (parseur->encodage_fst == LATIN_ENCODING)
			return ConvertToPythonString("latin-1");
		return ConvertToPythonString("utf-8");
	}

	static PyObject* xips_metpoids(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int id;
		float value;

		if (!PyArg_ParseTuple(args, "iif", &parsnum, &id, &value))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(0);
		return ConvertToPythonLong(parseur->proba_metpoids(id, value));
	}

	static PyObject* xips_metseuil(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int id;
		float value;

		if (!PyArg_ParseTuple(args, "iif", &parsnum, &id, &value))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(0);





		return ConvertToPythonLong(parseur->proba_metseuil(id, value));
	}

	static PyObject* xips_metvaleur(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int id;
		float value;

		if (!PyArg_ParseTuple(args, "iif", &parsnum, &id, &value))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(0);





		return ConvertToPythonLong(parseur->proba_metvaleur(id, value));
	}

	static PyObject* xips_poids(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int id;

		if (!PyArg_ParseTuple(args, "ii", &parsnum, &id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(0);





		double value = parseur->proba_poids(id);
		return PyFloat_FromDouble(value);
	}

	static PyObject* xips_seuil(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int id;

		if (!PyArg_ParseTuple(args, "ii", &parsnum, &id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(0);





		double value = parseur->proba_seuil(id);
		return PyFloat_FromDouble(value);
	}

	static PyObject* xips_valeur(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int id;

		if (!PyArg_ParseTuple(args, "ii", &parsnum, &id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(0);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(0);





		double value = parseur->proba_valeur(id);
		return PyFloat_FromDouble(value);
	}

	static PyObject* xips_typeregle(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int id;

		if (!PyArg_ParseTuple(args, "ii", &parsnum, &id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);

		if (parseur == NULL || parseur->VBases[id] == NULL)
			return ConvertToPythonLong(-1);





		int ty = parseur->VBases[id]->label->type;
		return ConvertToPythonLong(ty);
	}

	static PyObject* xips_coucheregle(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int id;

		if (!PyArg_ParseTuple(args, "ii", &parsnum, &id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL || parseur->VBases[id] == NULL)
			return ConvertToPythonLong(-1);





		int ty = parseur->VBases[id]->couche;
		return ConvertToPythonLong(ty);
	}

	static PyObject* xips_compteurregle(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int id;

		if (!PyArg_ParseTuple(args, "ii", &parsnum, &id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL || parseur->VBases[id] == NULL)
			return ConvertToPythonLong(-1);


		int ty = parseur->VBases[id]->compteur;
		return ConvertToPythonLong(ty);
	}


	static PyObject* xips_loadkif(PyObject* self, PyObject* args) {
		char* filename;
		char* pargs;
		int mapping = 0;
		int nbelements = PyTuple_Size(args);
		if (nbelements == 2) {
			if (!PyArg_ParseTuple(args, "ss", &filename, &pargs)) {
				PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
				return ConvertToPythonLong(-1);
			}
		}
		else {
			if (!PyArg_ParseTuple(args, "ssi", &filename, &pargs, &mapping)) {
				PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
				return ConvertToPythonLong(-1);
			}
		}

		int kifhandler = KifExecuteProgram(filename, pargs, mapping);
		if (kifhandler == -1)
			return ConvertToPythonLong(-1);

		if (mapping == 1) {
			stringstream code;
			KifCode* kifcode;
			if (kifhandler == 0)
				kifcode = BaseParseur->kifloader;
			else
				kifcode = kifGlobals->kifCodeVector[kifhandler];

			if (kifcode == NULL) {
				PyErr_SetString(PyExc_AttributeError, "Unknown KIF space");
				return ConvertToPythonLong(-1);
			}

			bin_hash<KifElement*>::iterator it;
			KifElement* kfunc;
			string name;
			char var;
			for (it = kifcode->kstart->declarations.begin(); it != kifcode->kstart->declarations.end(); it++) {
				kfunc = it->second;
				if (kfunc != NULL && kfunc->isFunction()) {
					name = kifGlobals->kifIdString[it->first];
					code << "def " << name << "(";
					var = 97;
					int i;
					for (i = 0; i < kfunc->Size(); i++) {
						if (i)
							code << ",";
						code << var;
						var++;
					}
					code << "):\n\treturn xips.kif(" << kifhandler << ",'" << name << "'";
					var = 97;
					for (i = 0; i < kfunc->Size(); i++) {
						code << "," << var;
						var++;
					}
					code << ")\n";
				}
			}

			if (code.str().size() != 0)
				PyRun_SimpleString(code.str().c_str());
		}

		return ConvertToPythonLong(kifhandler);
	}

	KifElement* pythonkifclone(KifCode* kifcode, PyObject* po) {
		if (PyBool_Check(po) == 1) {
			if (po == Py_True)
				return new KifBoolean(kifcode, NULL, true);
			return new KifBoolean(kifcode, NULL, false);
		}

		if (PyLong_Check(po) == 1) {
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

		if (PyUnicode_Check(po)) {
			string s;
			PythonVersString(po, s);
			return kifcode->Providestringraw(s);
		}

		PyObject* pelement;
		KifElement* e;
		if (PyList_Check(po) == 1) {
			KifVector* kvect = new KifVector(kifcode, NULL);
			int tail = PyList_GET_SIZE(po);
			for (int i = 0; i < tail; i++) {
				//Puis chacun des objets, kj est l'index de l'element
				pelement = PyList_GetItem(po, i);
				e = pythonkifclone(kifcode, pelement);
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
			PyObject* key;
			KifElement* k;
			string kval;
			while (PyDict_Next(po, &pos, &key, &pelement)) {
				if (key != NULL && pelement != NULL) {
					k = pythonkifclone(kifcode, key);
					if (k == kifRAISEERROR) {
						kmap->Release();
						return kifRAISEERROR;
					}
					e = pythonkifclone(kifcode, pelement);
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
			string s;
			PythonVersString(perr, s);
			e = kifcode->Providestringraw(s);
			Py_DECREF(perr);
			return e;
		}

		return kifRAISEERROR;
	}

	PyObject* kifclone(KifElement* resultat, bool relache) {
		if (resultat->type == kifBoolean) {
			PyObject* res;
			if (resultat == kifTRUE)
				res = Py_True;
			else
				res = Py_False;
			if (relache)
				resultat->Release();
			return res;
		}

		if (resultat->type == kifInteger || resultat->type == kifLong) {
			long l = resultat->Integer();
			if (relache)
				resultat->Release();
			return ConvertToPythonLong(l);
		}

		if (resultat->type == kifFloat || resultat->type == kifFraction) {
			double d = resultat->Float();
			if (relache)
				resultat->Release();
			return PyFloat_FromDouble(d);
		}

		PyObject* pcourant;
		PyObject* liste;

		if (resultat->aVectorContainer() || resultat->type == kifList || resultat->type == kifTable) {
			int i = 0;
			liste = PyList_New(0);
			KifIteration* itr = (KifIteration*)resultat->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
				pcourant = kifclone(itr->IteratorValue(), false);
				PyList_Insert(liste, i, pcourant);
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
			liste = PyDict_New();
			KifIteration* itr = (KifIteration*)resultat->Newiterator(false);
			for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
				pcourant = kifclone(itr->IteratorValue(), false);
				value = itr->IteratorKeyString();
				PyDict_SetItemString(liste, STR(value), pcourant);
				Py_DECREF(pcourant);
			}
			itr->Release();
			if (relache)
				resultat->Release();
			return liste;
		}

		value = resultat->String();
		if (relache)
			resultat->Release();

		if (value.isutf8())
			return PyUnicode_FromString(value.c_str());

		return ConvertToPythonString(STR(value));
	}


	static PyObject* xips_kif(PyObject* self, PyObject* args) {
		int nbelements = PyTuple_Size(args);

		if (nbelements < 2) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters, at least two parameters are required: kif handler and function name");
			return ConvertToPythonLong(-1);
		}

		PyObject* pelement;
		pelement = PyTuple_GetItem(args, 0);
		if (!PyLong_Check(pelement)) {
			PyErr_SetString(PyExc_AttributeError, "Expecting a kif handler");
			return ConvertToPythonLong(-2);
		}

		int kifhandler = PyLong_AsLong(pelement);
		KifCode* kifcode = kifGlobals->kifCodeVector[kifhandler];
		if (kifcode == NULL) {
			PyErr_SetString(PyExc_AttributeError, "This kif handler does not match an existing KiF program");
			return ConvertToPythonLong(-3);
		}

		pelement = PyTuple_GetItem(args, 1);
		if (!PyUnicode_Check(pelement)) {
			PyErr_SetString(PyExc_AttributeError, "Expecting a string");
			return ConvertToPythonLong(-4);
		}

		string functionname;
		PythonVersString(pelement, functionname);

		vector<KifElement*> params;
		KifElement* ke;

		for (int i = 2; i < nbelements; i++) {
			pelement = PyTuple_GetItem(args, i);
			ke = pythonkifclone(kifcode, pelement);
			if (ke == kifRAISEERROR) {
				for (i = 0; i < params.size(); i++)
					params[i]->Release();
				PyErr_SetString(PyExc_AttributeError, "No mapping for this python element");
				return ConvertToPythonLong(-5);
			}
			params.push_back(ke);
		}

		bool theerror = false;
		KifElement* resultat = XipKifExecuteIndexObject(kifcode, functionname, params, theerror);
		if (theerror == true) {
			string value = Errorstring();
			PyErr_SetString(PyExc_AttributeError, STR(value));
			return ConvertToPythonLong(-6);
		}

		return kifclone(resultat, true);
	}

	static PyObject* xips_typereglestring(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;
		int id;

		if (!PyArg_ParseTuple(args, "ii", &parsnum, &id))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL || parseur->VBases[id] == NULL)
			return ConvertToPythonLong(-1);





		int ty = parseur->VBases[id]->label->type;
		char* tys = field_name[ty];
		return ConvertToPythonString(tys);
	}

	static PyObject* xips_nbregles(PyObject* self, PyObject* args) {
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		if (!PyArg_ParseTuple(args, "i", &parsnum))  {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}

		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(-1);





		return ConvertToPythonLong(parseur->VBases.dernier);
	}

	static PyObject* xips_chargeprobas(PyObject* self, PyObject* args) {
		char* fichier;
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		if (!PyArg_ParseTuple(args, "is", &parsnum, &fichier)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}
		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(-1);
		int id = parseur->chargeprobabilites(fichier);
		return ConvertToPythonLong(id);
	}

	static PyObject* xips_sauveprobas(PyObject* self, PyObject* args) {
		char* fichier;
		int parsnum = index_current_parser;
		int precedent = index_current_parser;

		if (!PyArg_ParseTuple(args, "is", &parsnum, &fichier)) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
			return ConvertToPythonLong(-1);
		}
		GlobalParseur* parseur = initNouveauParseur(parsnum);
		if (parseur == NULL)
			return ConvertToPythonLong(-1);
		parseur->sauvegardeprobabilites(fichier);
		return ConvertToPythonLong(0);
	}

	static PyMethodDef XipsMethode[] = {

		{ "node", xips_xipnode, METH_VARARGS, "Return a xip node." },
		{ "nodeset", xips_xipnodes, METH_VARARGS, "Return a set of xip nodes." },
		{ "dependency", xips_dependency, METH_VARARGS, "Return a dependency." },
		{ "dependencyset", xips_dependencies, METH_VARARGS, "Return a set of dependency." },
		{ "parsestring", xips_parsestring, METH_VARARGS, "parse a string with grammar g: xips.parsestring(g,str,data). (data is any python object)" },
		{ "tagging", xips_stringtagging, METH_VARARGS, "tag a string with grammar g: xips.stringtagging(g,str)." },
		{ "parsefile", xips_parsefile, METH_VARARGS, "parse a file with grammar g: xips.parsefile(g,filename,data). (data is any python object)" },
#ifdef XIPLIBXML
		{ "parsexmlstring", xips_parsexmlstring, METH_VARARGS, "parse an XML string with grammar g at depth: xips.parsexmlstring(g,str,depth,data). (data is any python object)" },
		{ "parsexmlfile", xips_parsexmlfile, METH_VARARGS, "parse an XML file with grammar g at depth: xips.parsexmlfile(g,filename,depth,data). (data is any python object)" },
#endif
		{ "generatefromfile", xips_parsedependencyfile, METH_VARARGS, "Read a dependency file and generate a sentence from it: xips.generatefromfile(g,filename,data). (data is any python object)" },
		{ "generatefromstring", xips_parsedependencystring, METH_VARARGS, "Read a dependency string and generate a sentence from it:: xips.generatefromstring(g,filename,data). (data is any python object)" },
		{ "generated", xips_generated, METH_VARARGS, "Return the generated sentence: xips.generated(g)." },
		{ "loadgrm", xips_loadgrm, METH_VARARGS, "Load a grammar file: xips.loadgrm(filename)." },
		{ "reloadgrm", xips_reloadgrm, METH_VARARGS, "Reload the grammar file with the index grm: xips.reloadgrm(grm)." },
		{ "freegrm", xips_freegrm, METH_VARARGS, "Clean a grammar from memory: xips.freegrm(grm)." },
		{ "setdisplay", xips_setdisplay, METH_VARARGS, "Set the mode display xips.setdisplay(grm,mode)." },
		{ "adddisplay", xips_adddisplay, METH_VARARGS, "Add a mode display display xips.adddisplay(grm,mode)." },
		{ "removedisplay", xips_removedisplay, METH_VARARGS, "Remove a mode display xips.removedisplay(grm,mode)." },
		{ "testdisplay", xips_testdisplay, METH_VARARGS, "Test a mode display xips.testdisplay(grm,mode), return 1 or 0" },
		{ "whoami", xips_version, METH_VARARGS, "return the XIP engine version." },
		{ "dependencyonfirstnode", xips_dependencyonfirstnode, METH_VARARGS, "return the list of dependencies whose first parameter is the node n: xips.dependencyonfirstnode(g,n)." },
		{ "dependencyonsecondnode", xips_dependencyonsecondnode, METH_VARARGS, "return the list of dependencies whose second parameter is the node n: xips.dependencyonsecondnode(g,n)." },
		{ "fictivenodes", xips_xipfictivenodes, METH_VARARGS, "return the list of fictive nodes that have been created so far:fictivenodes(g)." },
		{ "setexchange", xips_setexechange, METH_VARARGS, "Set the exchange variable: xips.setexchange(v,o). v is a XIP python variable: return -1 if wrong parameters. -2 if unknown XIP variable. -3 if wrong XIP variable type. 0 if OK." },
		{ "internalstring", xips_xipinternalstring, METH_VARARGS, "return the internal string buffer. Applies only when parsing with a string or from an XML file." },
		{ "lexicals", xips_xiplexicalnodes, METH_VARARGS, "return the list of the lexical node indexes:lexicals(g)." },
		{ "setfloat", xips_setfloatvariable, METH_VARARGS, "setfloat(par,v,f) Sets the variable v (a string) with the value f (a float) for the grammar grm." },
		{ "setint", xips_setintvariable, METH_VARARGS, "setint(par,v,i) Sets the variable v (a string) with the value i (an int) for the grammar grm." },
		{ "setstring", xips_setstringvariable, METH_VARARGS, "setstring(par,v,s) Sets the variable v (a string) with the value i (a string) for the grammar grm." },
		{ "setdebug", xips_setdebug, METH_VARARGS, "setdebug(module_number,module_line) Sets a break point in the module module_num at module_line." },
		{ "currentgrammarindex", xips_currentgrammarindex, METH_VARARGS, "grammarindex() return the current grammar index." },
		{ "currentrule", xips_current_rule, METH_VARARGS, "currentrule() return the current rule index." },
		{ "currentruletext", xips_current_rule_text, METH_VARARGS, "currentruletext(displayheader) return the current rule text, displayheader is 0,1 or 2." },
		{ "grammarcharset", xips_get_grammarcharset, METH_VARARGS, "grammarcharset(g) return the charset encoding of the grammar" },
		{ "sourcecharset", xips_get_sourcecharset, METH_VARARGS, "sourcecharset(g) return the charset encoding of the input" },
		{ "fstcharset", xips_get_fstcharset, METH_VARARGS, "fstcharset(g) return the charset encoding of the FST" },
		{ "setruleweight", xips_metpoids, METH_VARARGS, "setruleweight(g,id,w) set the weight to rule id" },
		{ "setrulethreshold", xips_metseuil, METH_VARARGS, "setrulethreshold(g,id,w) set the threshold to rule id" },
		{ "setrulevalue", xips_metvaleur, METH_VARARGS, "setrulevalue(g,id,w) set the trigger value to rule id" },
		{ "getruleweight", xips_poids, METH_VARARGS, "getruleweight(g,id) return weight of rule id" },
		{ "getrulethreshold", xips_seuil, METH_VARARGS, "getrulethreshold(g,id) return  threshold of rule id" },
		{ "getrulevalue", xips_valeur, METH_VARARGS, "getrulevalue(g,id) return trigger value of rule id" },
		{ "rulelayer", xips_coucheregle, METH_VARARGS, "rulelayer(g,id) return the layer number of rule ID" },
		{ "ruletype", xips_typeregle, METH_VARARGS, "ruletype(g,id) return the type of rule ID" },
		{ "rulecounter", xips_compteurregle, METH_VARARGS, "rulecounter(g,id) return the number of occurrence of rule ID" },
		{ "ruletypestr", xips_typereglestring, METH_VARARGS, "ruletypestr(g,id) return the type of rule ID as a string" },
		{ "nbrules", xips_nbregles, METH_VARARGS, "nbrules(g) return the number of rules" },
		{ "loadprobabilities", xips_chargeprobas, METH_VARARGS, "loadprobabilities(g,filename) load a probability model stored in filename" },
		{ "saveprobabilities", xips_sauveprobas, METH_VARARGS, "saveprobabilities(g,filename) save a probability model stored in filename" },
		{ "loadkif", xips_loadkif, METH_VARARGS, "loadkif(filename,arguments) load a KiF program and return a kif handler" },
		{ "kif", xips_kif, METH_VARARGS, "kif(khandler,function_name,arguments) Execute a KiF function, the argument should be a vector of strings" },
		{ NULL, NULL, 0, NULL }
	};

	static struct PyModuleDef xipsmodule = {
		PyModuleDef_HEAD_INIT,
		"xips",   /* name of module */
		NULL, /* module documentation, may be NULL */
		-1,       /* size of per-interpreter state of the module,
				  or -1 if the module keeps state in global variables. */
				  XipsMethode
	};

	Exportation PyMODINIT_FUNC PyInit_xips(void) {
		main_module = PyModule_Create(&xipsmodule);
		if (main_module == NULL)
			return NULL;

		PyObject* pDict = PyModule_GetDict(main_module);
		PyRun_String(insere_xip_objects(OUI).c_str(), Py_file_input, pDict, pDict);

		xipsError = PyErr_NewException("xips.error", NULL, NULL);
		Py_INCREF(xipsError);
		PyModule_AddObject(main_module, "error", xipsError);

		initialization = true;

		return main_module;
	}
}

//Initialisation unique
void GlobalParseur::initialisation_python() {
	if (Py_IsInitialized())
		return;
	Py_Initialize();
	PyInit_xips();
}


