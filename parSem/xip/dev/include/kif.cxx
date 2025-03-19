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
#include "osdir.h"

#ifdef NTMCONNECTION
#include "ntmhmm.h"
#include "maintok.h"
#endif 

#ifdef WIN32
#include <conio.h>
#define GETCH  _getch
#else
#define GETCH  getchar_unlocked 
#endif 
#include "varlibxml.h"
#include "kifn.h"
#include "kif.h"
#include "conversion.h"
#include "graphe.h"


int kif_declencheur=1000;

Exportation int KifLoad(vector<string>& paths,vector<string>& arguments,bool debugkif);
Exportation string KifExecute(int ipar,string name,vector<string>& parameters,ostringstream* os,bool debugkif);
Exportation void KifDelete(int ipar);
Exportation int XipParseStringAndOS(string texte,int ipar, XipResult* xip,ostringstream* os,char xml,char applique_grammaire);
extern VECTA<GlobalParseur*> gVecteurParseurs;
void CreationRepertoire(char* noms,int pos);

static char* kifErrorStrings[]={"KIF(0): Wrong number of parameters in SPLIT",
"KIF(1): Wrong 3rd parameter in SPLIT",
"KIF(2): Wrong number of parameters in LIST",
"KIF(3): Wrong number of parameters in UTF8",
"KIF(4): Wrong number of parameters in LATIN",
"KIF(5): Wrong number of parameters in LEFT",
"KIF(6): Wrong number of parameters in RIGHT",
"KIF(7): Wrong number of parameters in MID",
"KIF(8): Wrong number of parameters in ISLOWER",
"KIF(9): Wrong number of parameters in ISUPPER",
"KIF(10): Wrong number of parameters in LOWER",
"KIF(11): Wrong number of parameters in UPPER",
"KIF(12): Wrong number of parameters in TRIM",
"KIF(13): Wrong number of parameters in SETDATE",
"KIF(14): Wrong time parameter in SETDATE",
"KIF(15): Wrong number of parameters in DATE",
"KIF(16): Wrong time parameter in DATE",
"KIF(17): Wrong number of parameters in YEAR",
"KIF(18): Wrong time parameter in YEAR",
"KIF(19): Wrong number of parameters in DAY",
"KIF(20): Wrong time parameter in DAY",
"KIF(21): Wrong number of parameters in HOUR",
"KIF(22): Wrong time parameter in HOUR",
"KIF(23): Wrong number of parameters in MONTH",
"KIF(24): Wrong time parameter in MONTH",
"KIF(25): Missing parameter in RAISE",
"KIF(26): Wrong number of parameters in WRITE",
"KIF(27): First parameter in WRITE should be a file",
"KIF(28): First parameter in READ should be a file",
"KIF(29): First parameter in READLN should be a file",
"KIF(30): First parameter in CLOSE should be a file",
"KIF(31): First parameter in OPENREAD should be a file",
"KIF(32): Error in OPENREAD while opening the file:",
"KIF(33): First parameter in EOF should be a file",
"KIF(34): First parameter in OPENWRITE should be a file",
"KIF(35): Error in OPENWRITE while opening the file:",
"KIF(36): Wrong number of parameters in INSERT",
"KIF(37): Wrong call to INSERT",
"KIF(38): Wrong number of parameters in PUSH",
"KIF(39): Wrong call to a PUSH",
"KIF(40): Wrong number of parameters in POP",
"KIF(41): Wrong call to a POP",
"KIF(42): Wrong number of parameters in LAST",
"KIF(43): Unknown NODE in LAST",
"KIF(44): Unknown GENERATION NODE in LAST",
"KIF(45): Wrong call to LAST",
"KIF(46): Wrong number of parameters in SORT",
"KIF(47): Wrong call to a SORT",
"KIF(48): Missing parameters in APPLY",
"KIF(49): Wrong call to APPLY",
"KIF(50): Wrong number of parameters in CREATEDEPENDENCY",
"KIF(51): Unknown dependency:",
"KIF(52): 2nd parameter should be a MAP in CREATEDEPENDENCY",
"KIF(53): 3rd parameter should be a VECTOR in CREATEDEPENDENCY",
"KIF(54): 3rd parameter should only contain NODE in CREATEDEPENDENCY",
"KIF(55): 3rd parameter should only contain NODE in CREATEDEPENDENCY",
"KIF(56): Unknown feature: ",
"KIF(57): Dependency not created",
"KIF(58): Wrong number of parameters in SETFEATURE",
"KIF(59): Wrong type of variable in SETFEATURE",
"KIF(60): Unknown feature: ",
"KIF(61): Wrong number of parameters in REMOVEFEATURE",
"KIF(62): Wrong type of variable in REMOVEFEATURE",
"KIF(63): Unknown attribute: ",
"KIF(64): Wrong number of parameters in FEATURE",
"KIF(65): Wrong type of variable in FEATURE",
"KIF(66): Unknown attribute: ",
"KIF(67): Unknown feature value: ",
"KIF(68): Parameter should be either a VECTOR or a MAP in CLEAR",
"KIF(69): Second parameter should be a MAP in DATA",
"KIF(70): Unknown NODE in DATA",
"KIF(71): Unknown DEPENDENCY in DATA",
"KIF(72): No DATA for this variable",
"KIF(73): Unknown NODE in NAME",
"KIF(74): Unknown DEPENDENCY in NAME",
"KIF(75): No NAME for this variable",
"KIF(76): Wrong number of parameters in STACK",
"KIF(77): 2nd parameter should be a VECTOR in STACK",
"KIF(78): Unknown dependency in STACK",
"KIF(79): No STACK for this variable",
"KIF(80): Wrong number of parameters in PARAMETERS",
"KIF(81): 2nd parameter should be a VECTOR in PARAMETERS",
"KIF(82): Unknown dependency in PARAMETERS",
"KIF(83): No PARAMETERS for this variable",
"KIF(84): Wrong number of parameters in REMOVEFIRST",
"KIF(85): No REMOVEFIRST for this variable",
"KIF(86): Wrong number of parameters in INSTANCES",
"KIF(87): No INSTANCES for this variable",
"KIF(88): Wrong number of parameters in SET",
"KIF(89): Wrong number of parameters in REGEX",
"KIF(90): Missing class in MATRIX SET",
"KIF(91): Missing attribute in MATRIX SET",
"KIF(92): No SET for this variable",
"KIF(93): Wrong number of parameters in GET",
"KIF(94): Missing class in MATRIX GET",
"KIF(95): Missing attribute in MATRIX GET",
"KIF(96): No GET for this variable",
"KIF(97): Wrong number of parameters in OFFSET",
"KIF(98): Unknown node in OFFSET",
"KIF(99): No OFFSET for this variable",
"KIF(100): Wrong number of parameters in OFFSETCHAR",
"KIF(101): Unknown node in OFFSETCHAR",
"KIF(102): No OFFSETCHAR for this variable",
"KIF(103): Wrong number of parameters in TOKENOFFSET",
"KIF(104): Unknown node in TOKENOFFSET",
"KIF(105): No TOKENOFFSET for this variable",
"KIF(106): Wrong number of parameters in LEFTOFFSET",
"KIF(107): Unknown node in LEFTOFFSET",
"KIF(108): No LEFTOFFSET for this variable",
"KIF(109): Wrong number of parameters in RIGHTOFFSET",
"KIF(110): Unknown node in RIGHTOFFSET",
"KIF(111): No RIGHTOFFSET for this variable",
"KIF(112): Wrong number of parameters in LEFTOFFSETCHAR",
"KIF(113): Unknown node in LEFTOFFSETCHAR",
"KIF(114): No LEFTOFFSETCHAR for this variable",
"KIF(115): Wrong number of parameters in RIGHTOFFSETCHAR",
"KIF(116): Unknown node in RIGHTOFFSETCHAR",
"KIF(117): No RIGHTOFFSETCHAR for this variable",
"KIF(118): Wrong number of parameters in LEFTTOKENOFFSET",
"KIF(119): Unknown node in LEFTTOKENOFFSET",
"KIF(120): No LEFTTOKENOFFSET for this variable",
"KIF(121): Wrong number of parameters in RIGHTTOKENOFFSET",
"KIF(122): Unknown node in RIGHTTOKENOFFSET",
"KIF(123): No RIGHTTOKENOFFSET for this variable",
"KIF(124): No LEMMA for this variable",
"KIF(125): Wrong number of parameters in REMOVELAST",
"KIF(126): No REMOVELAST for this variable",
"KIF(127): No SURFACE for this variable",
"KIF(128): Wrong number of parameters in REPLACE",
"KIF(129): Wrong number of parameters in LOAD",
"KIF(130): No XMLNODE for this NODE",
"KIF(131): Unknown parameter in LOAD",
"KIF(132): BEGIN failed for this ITERATOR",
"KIF(133): END failed for this ITERATOR",
"KIF(134): NEND failed for this ITERATOR",
"KIF(135): No KEY for this ITERATOR",
"KIF(136): No VALUE for this ITERATOR",
"KIF(137): No VALUETYPE for this ITERATOR",
"KIF(138): Unknown NODE in NEXT",
"KIF(139): Unknown GENERATION NODE in NEXT",
"KIF(140): Unknown XML in NEXT",
"KIF(141): No NEXT for this variable",
"KIF(142): Unknown NODE in PREVIOUS",
"KIF(143): Unknown GENERATION NODE in PREVIOUS",
"KIF(144): Unknown XML in PREVIOUS",
"KIF(145): No PREVIOUS for this variable",
"KIF(146): Unknown NODE in PARENT",
"KIF(147): Unknown GENERATION NODE in PARENT",
"KIF(148): Unknown XML in PARENT",
"KIF(149): No PARENT for this variable",
"KIF(150): Unknown NODE in CHILD",
"KIF(151): Unknown GENERATION NODE in CHILD",
"KIF(152): Unknown XML in CHILD",
"KIF(153): No CHILD for this variable",
"KIF(154): Wrong TYPE for a WRITE",
"KIF(155): Wrong frame affectation",
"KIF(156): Wrong CALL affectation",
"KIF(157): ", //used, do not modify
"KIF(158): Please check the number of parameters in: ",
"KIF(159): Please check the parameters in: ",
"KIF(160): Wrong frame initialisation",
"KIF(161): Wrong number of parameters in SEEK",
"KIF(162): First parameter shoud be a file in SEEK",
"KIF(163): Wrong number of parameters in TELL",
"KIF(164): First parameter shoud be a file in TELL",
"KIF(165): Wrong number of parameters in JOIN",
"KIF(166): Wrong parameter in JOIN",
"KIF(167): Wrong number of parameters in COMPILE",
"KIF(168): Wrong parameter in COMPILE",
"KIF(169): Wrong number of parameters in SUM",
"KIF(170): Wrong parameter in SUM",
"KIF(171): Wrong number of parameters in PRODUCT",
"KIF(172): Wrong parameter in PRODUCT",
"KIF(173): Wrong number of parameters in UNGET",
"KIF(174): No UNGET for this variable",
"KIF(175): Wrong number of parameters in REVERSE",
"KIF(176): Wrong number of parameters in EXTRACT",
"KIF(177): Wrong parameter in EXTRACT",
"KIF(178): Unknown function: ",
"KIF(179): This parser has already been loaded",
"KIF(180): Wrong number of parameters in PARSE",
"KIF(181): Empty grammar in PARSE",
"KIF(182): Wrong parameters in PARSE",
"KIF(183): Unknown instance: ",
"KIF(184): Loading error: ",
"KIF(185): Wrong number of parameters in REALPATH",
"KIF(186): Wrong number of parameters in CREATEDIRECTORY",
"KIF(187): Wrong number of parameters in LISTDIRECTORY"
};

//===================================================================
//List of all KifCode in memory
VECTA<KifCode*> kifCodeVector;
//===================================================================


char parentde(ResultatFonction* fille,ResultatFonction* parent);
ResultatFonction* suivantde(ResultatFonction* courant,ResultatFonction* suivant,int increment);
ResultatFonction* perede(ResultatFonction* courant,ResultatFonction* suivant);
ResultatFonction* filsde(ResultatFonction* courant,ResultatFonction* suivant,char dernier);
ResultatFonction* tokensuivant(GlobalParseur*,ResultatFonction* courant,ResultatFonction* suivant,int pos);
string replacestringwithautomaton(string &str, string& reg,string& rep);

string& Trim(string& chaine);

#ifndef WIN32
#define sprintf_s(a,b,c,d) sprintf(a,c,d)
#endif

#ifdef _DEBUG
KifCode* currentkifcode;
#endif

typedef KifElement* (KifCode::*KifParseElement)(x_node*,KifElement*);
typedef double (*mathFunc)(double);

static map<string,mathFunc> kifMathFunctions;

static map<string,bool> kifStringFunctions;
static map<string,bool> kifMatrixFunctions;
static map<string,bool> kifDependencyFunctions;
static map<string,bool> kifGenerationFunctions;
static map<string,bool> kifTimeFunctions;
static map<string,bool> kifVectorFunctions;
static map<string,bool> kifFSTFunctions;
static map<string,bool> kifParserFunctions;
static map<string,bool> kifKiFFunctions;
static map<string,bool> kifMapFunctions;
static map<string,bool> kifFileFunctions;
static map<string,bool> kifXipRuleFunctions;
static map<string,bool> kifXMLFunctions;
static map<string,bool> kifXipFunctions;
static map<string,bool> kifGraphFunctions;
static map<string,bool> kifIteratorFunctions;
static map<string,bool> kifGenericFunctions;

static bool TestkifFunctions(string& n,map<string,bool>& kiffunc) {
	map<string,bool>::iterator it=kiffunc.find(n);
	if (it==kiffunc.end())
		return false;
	return true;
}

static map<string,KifParseElement> kifFunctions;
static map<string,KifParseElement> kifTypes;
static map<string,KifOperators> kifOperators;
static map<KifType,string> kifStringType;
static map<string,string> kifMetaCharacters;
static map<KifType,bool> kifClean;
static map<KifOperators,bool> kifOperatorMath;

KifElement* kifNULL=NULL;
KifElement* kifTRUE=NULL;
KifElement* kifFALSE=NULL;
static KifElement* kifMINUSONE=NULL;
static KifElement* kifDEFAULT=NULL;
static KifElement* kifBREAK=NULL;
static KifElement* kifBREAKPOINT=NULL;
static KifElement* kifCONTINUE=NULL;
static KifElement* kifNEG=NULL;
//Types
KifElement* kifTypeSTRING=NULL;
KifElement* kifTypeVECTOR=NULL;
KifElement* kifTypeINTEGER=NULL;
KifElement* kifTypeFLOAT=NULL;
KifElement* kifTypeBOOLEAN=NULL;



void StaticInitialisation() {
	if (kifNULL==NULL) {
		//We only create these values ONCE
		kifStringType[kifFST]="fst";
		kifStringType[kifParser]="parser";
		kifStringType[kifString]="string";
		kifStringType[kifStringPtr]="string";
		kifStringType[kifInteger]="int";
		kifStringType[kifTime]="time";
		kifStringType[kifCall]="call";
		kifStringType[kifFloat]="float";
		kifStringType[kifBoolean]="boolean";
		kifStringType[kifIntegerPtr]="int";
		kifStringType[kifFloatPtr]="float";
		kifStringType[kifSelf]="self";
		kifStringType[kifMap]="map";
		kifStringType[kifMatrix]="matrix";
		kifStringType[kifVector]="vector";
		kifStringType[kifIterator]="iterator";
		kifStringType[kifXipRule]="rule";
		kifStringType[kifXipNode]="node";
		kifStringType[kifGraph]="graph";
		kifStringType[kifXipDependency]="dependency";
		kifStringType[kifXipGeneration]="generation";
		kifStringType[kifXMLNode]="xml";
		kifStringType[kifXMLNodePtr]="xml";
		kifStringType[kifFile]="file";

		kifStringType[kifStart]="kifStart";
		kifStringType[kifAny]="kifAny";
		kifStringType[kifCouple]="kifCouple";
		kifStringType[kifCallFunction]="kifCallFunction";
		kifStringType[kifFrameDeclaration]="kifFrameDeclaration";
		kifStringType[kifFunctionDeclaration]="kifFunctionDeclaration";
		kifStringType[kifInstance]="kifInstance";
		kifStringType[kifIndex]="kifIndex";
		kifStringType[kifPredefined]="kifPredefined";
		kifStringType[kifVariableDeclaration]="kifVariableDeclaration";
		kifStringType[kifFrame]="kifFrame";
		kifStringType[kifFunction]="kifFunction";
		kifStringType[kifInstruction]="kifInstruction";
		//===================================================================	
		kifClean[kifString]=true;
		kifClean[kifInteger]=true;
		kifClean[kifFloat]=true;
		kifClean[kifBoolean]=true;
		//===================================================================
#ifdef NTMCONNECTION
		kifTypes["fst"]=&KifCode::KifParseVarFST;
#endif
		kifTypes["parser"]=&KifCode::KifParseVarParser;
		kifTypes["kif"]=&KifCode::KifParseVarKifKiF;
		kifTypes["self"]=&KifCode::KifParseVarSelf;
		kifTypes["int"]=&KifCode::KifParseVarInt;
		kifTypes["time"]=&KifCode::KifParseVarTime;
		kifTypes["call"]=&KifCode::KifParseVarCall;
		kifTypes["string"]=&KifCode::KifParseVarString;
		kifTypes["float"]=&KifCode::KifParseVarFloat;
		kifTypes["vector"]=&KifCode::KifParseVarVector;
		kifTypes["map"]=&KifCode::KifParseVarMap;
		kifTypes["matrix"]=&KifCode::KifParseVarMatrix;
		kifTypes["dictionary"]=&KifCode::KifParseVarMap;
		kifTypes["boolean"]=&KifCode::KifParseVarBoolean;
		kifTypes["file"]=&KifCode::KifParseVarFile;
		kifTypes["node"]=&KifCode::KifParseVarNode;
		kifTypes["graph"]=&KifCode::KifParseVarGraph;
		kifTypes["dependency"]=&KifCode::KifParseVarDependency;
		kifTypes["generation"]=&KifCode::KifParseVarGeneration;
		kifTypes["xml"]=&KifCode::KifParseVarXML;
		kifTypes["iterator"]=&KifCode::KifParseVarIterator;
		kifTypes["riterator"]=&KifCode::KifParseVarRIterator;
		kifTypes["rule"]=&KifCode::KifParseVarRule;
		//===================================================================
		kifFunctions["xpathexpression"]=&KifCode::KifParseXPath;
		kifFunctions["bodyrule"]=&KifCode::KifParseXipRule;


		kifFunctions["frame"]=&KifCode::KifParseFrame;
		kifFunctions["valmap"]=&KifCode::KifParseValMap;
		kifFunctions["valvector"]=&KifCode::KifParseValVector;
		kifFunctions["list"]=&KifCode::KifParseList;
		kifFunctions["dico"]=&KifCode::KifParseDico;
		kifFunctions["declaration"]=&KifCode::KifParseDeclaration;
		kifFunctions["function"]=&KifCode::KifParseFunction;
		kifFunctions["affectation"]=&KifCode::KifParseAffectation;
		kifFunctions["operatoraffectation"]=&KifCode::KifParseOperator;
		kifFunctions["variable"]=&KifCode::KifParseVariable;
		kifFunctions["operator"]=&KifCode::KifParseOperator;
		kifFunctions["comparator"]=&KifCode::KifParseOperator;
		kifFunctions["orand"]=&KifCode::KifParseOperator;
		kifFunctions["operation"]=&KifCode::KifParseOperation;
		kifFunctions["operationin"]=&KifCode::KifParseOperationIn;
		kifFunctions["comparison"]=&KifCode::KifParseComparison;
		kifFunctions["abool"]=&KifCode::KifParseABoolean;
		kifFunctions["astringdouble"]=&KifCode::KifParseAString;
		kifFunctions["astringsimple"]=&KifCode::KifParseAString;
		kifFunctions["anumber"]=&KifCode::KifParseANumber;
		kifFunctions["indexes"]=&KifCode::KifParseIndexes;
		kifFunctions["interval"]=&KifCode::KifParseInterval;
		kifFunctions["regularcall"]=&KifCode::KifParseCall;
		kifFunctions["breakcall"]=&KifCode::KifParseUniqueCall;
		kifFunctions["returncall"]=&KifCode::KifParseUniqueCall;
		kifFunctions["breakpointcall"]=&KifCode::KifParseUniqueCall;
		kifFunctions["continuecall"]=&KifCode::KifParseUniqueCall;
		kifFunctions["optional"]=&KifCode::KifParseBloc;
		kifFunctions["test"]=&KifCode::KifParseTest;
		kifFunctions["switch"]=&KifCode::KifParseSwitch;
		kifFunctions["trycatch"]=&KifCode::KifParseTryCatch;
		kifFunctions["testswitch"]=&KifCode::KifParseTestSwitch;
		kifFunctions["testelif"]=&KifCode::KifParseTest;
		kifFunctions["loop"]=&KifCode::KifParseLoop;
		kifFunctions["for"]=&KifCode::KifParseFor;
		kifFunctions["negation"]=&KifCode::KifParseNegation;
		kifFunctions["booleanexpression"]=&KifCode::KifParseBoolean;
		kifFunctions["optionalboolean"]=&KifCode::KifParseBloc;

		//===================================================================
		kifOperators["=="]=kifEQUAL;
		kifOperators[":"]=kifEQUAL;
		kifOperators["<"]=kifINFERIOR;
		kifOperators[">"]=kifSUPERIOR;
		kifOperators[">="]=kifSUPEQU;
		kifOperators["<="]=kifINFEQU;
		kifOperators["="]=kifAFFECTATION;
		kifOperators["<>"]=kifDIFFERENT;
		kifOperators["~:"]=kifDIFFERENT;
		kifOperators["!="]=kifDIFFERENT;
		kifOperators["+"]=kifPLUS;
		kifOperators["-"]=kifMINUS;
		kifOperators["*"]=kifMULTIPLY;
		kifOperators["/"]=kifDIVIDE;
		kifOperators["%"]=kifMOD;
		kifOperators["^"]=kifPOWER;
		kifOperators["||"]=kifUNION;
		kifOperators["&&"]=kifINTERSECTION;
		kifOperators["<<"]=kifSHIFTLEFT;
		kifOperators[">>"]=kifSHIFTRIGHT;
		kifOperators["+="]=kifPLUSEQU;
		kifOperators["-="]=kifMINUSEQU;
		kifOperators["*="]=kifMULTIPLYEQU;
		kifOperators["/="]=kifDIVIDEEQU;
		kifOperators["%="]=kifMODEQU;
		kifOperators["^="]=kifPOWEREQU;
		kifOperators["<<="]=kifSHIFTLEFTEQU;
		kifOperators[">>="]=kifSHIFTRIGHTEQU;
		kifOperators["||="]=kifUNIONEQU;
		kifOperators["&&="]=kifINTERSECTIONEQU;
		kifOperators["|"]=kifOR;
		kifOperators["&"]=kifAND;
		kifOperators["in"]=kifIN;


		kifOperatorMath[kifPLUS]=true;
		kifOperatorMath[kifMINUS]=true;
		kifOperatorMath[kifMULTIPLY]=true;
		kifOperatorMath[kifDIVIDE]=true;
		kifOperatorMath[kifMOD]=true;
		kifOperatorMath[kifPOWER]=true;
		kifOperatorMath[kifUNION]=true;
		kifOperatorMath[kifINTERSECTION]=true;
		kifOperatorMath[kifSHIFTLEFT]=true;
		kifOperatorMath[kifSHIFTRIGHT]=true;

		//===================================================================
		kifStringFunctions["string"]=true;
		kifStringFunctions["int"]=true;
		kifStringFunctions["float"]=true;
		kifStringFunctions["boolean"]=true;

		kifStringFunctions["size"]=true;
		kifStringFunctions["get"]=true;
		kifStringFunctions["reverse"]=true;
		kifStringFunctions["ord"]=true;
		kifStringFunctions["regex"]=true;
		kifStringFunctions["replace"]=true;
		kifStringFunctions["split"]=true;
		kifStringFunctions["removefirst"]=true;
		kifStringFunctions["removelast"]=true;
		kifStringFunctions["utf8"]=true;
		kifStringFunctions["latin"]=true;
		kifStringFunctions["left"]=true;
		kifStringFunctions["right"]=true;
		kifStringFunctions["mid"]=true;
		kifStringFunctions["isalpha"]=true;
		kifStringFunctions["isupper"]=true;
		kifStringFunctions["islower"]=true;
		kifStringFunctions["upper"]=true;
		kifStringFunctions["lower"]=true;
		kifStringFunctions["trim"]=true;
		kifStringFunctions["pop"]=true;
		kifStringFunctions["last"]=true;
		kifStringFunctions["insert"]=true;
		kifStringFunctions["isa"]=true;
		kifStringFunctions["list"]=true;
		kifStringFunctions["type"]=true;
		kifStringFunctions["realpath"]=true;
		kifStringFunctions["createdirectory"]=true;
		kifStringFunctions["listdirectory"]=true;

		kifVectorFunctions["size"]=true;
		kifVectorFunctions["reverse"]=true;
		kifVectorFunctions["insert"]=true;
		kifVectorFunctions["push"]=true;
		kifVectorFunctions["join"]=true;
		kifVectorFunctions["pop"]=true;
		kifVectorFunctions["last"]=true;
		kifVectorFunctions["sort"]=true;
		kifVectorFunctions["sum"]=true;
		kifVectorFunctions["product"]=true;
		kifVectorFunctions["clear"]=true;
		kifVectorFunctions["apply"]=true;
		kifVectorFunctions["isa"]=true;
		kifVectorFunctions["list"]=true;
		kifVectorFunctions["type"]=true;
		kifVectorFunctions["string"]=true;
		kifVectorFunctions["int"]=true;
		kifVectorFunctions["float"]=true;
		kifVectorFunctions["boolean"]=true;

		kifFSTFunctions["load"]=true;
		kifFSTFunctions["compile"]=true;
		kifFSTFunctions["compilewith"]=true;
		kifFSTFunctions["up"]=true;
		kifFSTFunctions["down"]=true;	
		kifFSTFunctions["isa"]=true;
		kifFSTFunctions["list"]=true;
		kifFSTFunctions["type"]=true;
	
		kifKiFFunctions["load"]=true;
		kifKiFFunctions["name"]=true;

		kifParserFunctions["load"]=true;
		kifParserFunctions["parse"]=true;
		kifParserFunctions["name"]=true;

		kifMapFunctions["size"]=true;
		kifMapFunctions["clear"]=true;
		kifMapFunctions["sum"]=true;
		kifMapFunctions["product"]=true;
		kifMapFunctions["pop"]=true;
		kifMapFunctions["isa"]=true;
		kifMapFunctions["list"]=true;
		kifMapFunctions["type"]=true;
		kifMapFunctions["string"]=true;
		kifMapFunctions["int"]=true;
		kifMapFunctions["float"]=true;
		kifMapFunctions["boolean"]=true;

		kifMatrixFunctions["size"]=true;
		kifMatrixFunctions["clear"]=true;
		kifMatrixFunctions["write"]=true;
		kifMatrixFunctions["set"]=true;
		kifMatrixFunctions["instances"]=true;
		kifMatrixFunctions["get"]=true;
		kifMatrixFunctions["isa"]=true;
		kifMatrixFunctions["list"]=true;
		kifMatrixFunctions["type"]=true;

		kifFileFunctions["openwrite"]=true;
		kifFileFunctions["openread"]=true;
		kifFileFunctions["get"]=true;
		kifFileFunctions["unget"]=true;
		kifFileFunctions["close"]=true;
		kifFileFunctions["seek"]=true;
		kifFileFunctions["tell"]=true;
		kifFileFunctions["write"]=true;
		kifFileFunctions["read"]=true;
		kifFileFunctions["readln"]=true;
		kifFileFunctions["size"]=true;
		kifFileFunctions["isa"]=true;
		kifFileFunctions["list"]=true;
		kifFileFunctions["type"]=true;
		kifFileFunctions["eof"]=true;
		kifFileFunctions["string"]=true;
		kifFileFunctions["int"]=true;
		kifFileFunctions["float"]=true;
		kifFileFunctions["boolean"]=true;

		kifXipRuleFunctions["apply"]=true;
		kifXipRuleFunctions["set"]=true;
		kifXipRuleFunctions["get"]=true;
		kifXipRuleFunctions["isa"]=true;
		kifXipRuleFunctions["list"]=true;
		kifXipRuleFunctions["type"]=true;

		kifTimeFunctions["setdate"]=true;
		kifTimeFunctions["date"]=true;
		kifTimeFunctions["year"]=true;
		kifTimeFunctions["month"]=true;
		kifTimeFunctions["day"]=true;
		kifTimeFunctions["hour"]=true;
		kifTimeFunctions["isa"]=true;
		kifTimeFunctions["list"]=true;
		kifTimeFunctions["type"]=true;

		kifTimeFunctions["string"]=true;
		kifTimeFunctions["int"]=true;
		kifTimeFunctions["float"]=true;
		kifTimeFunctions["boolean"]=true;

		//"log","ln","exp","sqrt", "tan","sin","cos","atan","@pi","@e"
		//Mathematics Function
		kifMathFunctions["log"]=log10;
		kifMathFunctions["ln"]=log;
		kifMathFunctions["exp"]=exp;
		kifMathFunctions["sqrt"]=sqrt;
		kifMathFunctions["tan"]=tan;
		kifMathFunctions["sin"]=sin;
		kifMathFunctions["cos"]=cos;
		kifMathFunctions["atan"]=atan;

		kifGenericFunctions["isa"]=true;
		kifGenericFunctions["type"]=true;
		kifGenericFunctions["list"]=true;
		kifGenericFunctions["chr"]=true;
		
		kifGenericFunctions["string"]=true;
		kifGenericFunctions["int"]=true;
		kifGenericFunctions["float"]=true;
		kifGenericFunctions["boolean"]=true;

		kifGraphFunctions["isa"]=true;
		kifGraphFunctions["set"]=true;
		kifGraphFunctions["name"]=true;
		kifGraphFunctions["type"]=true;
		kifGraphFunctions["list"]=true;
		kifGraphFunctions["replace"]=true;
		kifGraphFunctions["project"]=true;
		kifGraphFunctions["pop"]=true;
		kifGraphFunctions["extract"]=true;

		kifIteratorFunctions["next"]=true;
		kifIteratorFunctions["end"]=true;
		kifIteratorFunctions["set"]=true;
		kifIteratorFunctions["nend"]=true;
		kifIteratorFunctions["begin"]=true;
		kifIteratorFunctions["key"]=true;
		kifIteratorFunctions["value"]=true;
		kifIteratorFunctions["apply"]=true;
		kifIteratorFunctions["valuetype"]=true;
		kifIteratorFunctions["isa"]=true;
		kifIteratorFunctions["list"]=true;
		kifIteratorFunctions["type"]=true;

		kifXipFunctions["next"]=true;
		kifXipFunctions["last"]=true;
		kifXipFunctions["previous"]=true;
		kifXipFunctions["parent"]=true;
		kifXipFunctions["data"]=true;
		kifXipFunctions["name"]=true;
		kifXipFunctions["pos"]=true;
		kifXipFunctions["child"]=true;
		kifXipFunctions["xmlnode"]=true;
		kifXipFunctions["surface"]=true;
		kifXipFunctions["lemma"]=true;
		kifXipFunctions["setfeature"]=true;
		kifXipFunctions["feature"]=true;
		kifXipFunctions["removefeature"]=true;
		kifXipFunctions["isa"]=true;
		kifXipFunctions["list"]=true;
		kifXipFunctions["type"]=true;
		kifXipFunctions["number"]=true;
		kifXipFunctions["offset"]=true;
		kifXipFunctions["offsetchar"]=true;
		kifXipFunctions["tokenoffset"]=true;
		kifXipFunctions["leftoffset"]=true;
		kifXipFunctions["leftoffsetchar"]=true;
		kifXipFunctions["lefttokenoffset"]=true;
		kifXipFunctions["rightoffset"]=true;
		kifXipFunctions["rightoffsetchar"]=true;
		kifXipFunctions["righttokenoffset"]=true;
		kifXipFunctions["string"]=true;
		kifXipFunctions["int"]=true;
		kifXipFunctions["float"]=true;
		kifXipFunctions["boolean"]=true;

		kifDependencyFunctions["string"]=true;
		kifDependencyFunctions["int"]=true;
		kifDependencyFunctions["float"]=true;
		kifDependencyFunctions["boolean"]=true;

		kifDependencyFunctions["data"]=true;
		kifDependencyFunctions["name"]=true;
		kifDependencyFunctions["setfeature"]=true;
		kifDependencyFunctions["feature"]=true;
		kifDependencyFunctions["removefeature"]=true;
		kifDependencyFunctions["isa"]=true;
		kifDependencyFunctions["list"]=true;
		kifDependencyFunctions["type"]=true;
		kifDependencyFunctions["stack"]=true;
		kifDependencyFunctions["push"]=true;
		kifDependencyFunctions["pop"]=true;
		kifDependencyFunctions["last"]=true;
		kifDependencyFunctions["parameters"]=true;

		kifGenerationFunctions["string"]=true;
		kifGenerationFunctions["int"]=true;
		kifGenerationFunctions["float"]=true;
		kifGenerationFunctions["boolean"]=true;

		kifGenerationFunctions["data"]=true;
		kifGenerationFunctions["name"]=true;
		kifGenerationFunctions["setfeature"]=true;
		kifGenerationFunctions["feature"]=true;
		kifGenerationFunctions["removefeature"]=true;
		kifGenerationFunctions["isa"]=true;
		kifGenerationFunctions["list"]=true;
		kifGenerationFunctions["type"]=true;
		kifGenerationFunctions["stack"]=true;
		kifGenerationFunctions["push"]=true;
		kifGenerationFunctions["pop"]=true;
		kifGenerationFunctions["last"]=true;
		kifGenerationFunctions["parameters"]=true;
		kifGenerationFunctions["next"]=true;
		kifGenerationFunctions["previous"]=true;
		kifGenerationFunctions["parent"]=true;
		kifGenerationFunctions["last"]=true;
		kifGenerationFunctions["child"]=true;

		kifXMLFunctions["string"]=true;
		kifXMLFunctions["boolean"]=true;
		kifXMLFunctions["next"]=true;
		kifXMLFunctions["previous"]=true;
		kifXMLFunctions["parent"]=true;
		kifXMLFunctions["child"]=true;
		kifXMLFunctions["data"]=true;
		kifXMLFunctions["name"]=true;
		kifXMLFunctions["isa"]=true;
		kifXMLFunctions["list"]=true;
		kifXMLFunctions["type"]=true;
#ifdef WIN32
		kifMetaCharacters["\\n"]="\r\n";
#else
		kifMetaCharacters["\\n"]=13;
#endif
		kifMetaCharacters["\\r"]=10;
		kifMetaCharacters["\\\""]=34;
		kifMetaCharacters["\\t"]=9;	
		//===================================================================
		
		string nullname="null";
		kifNULL=new KifConst(NULL,NULL,nullname);


		kifBREAK=new KifConst(NULL,NULL,"_break");

		kifBREAKPOINT=new KifConst(NULL,NULL,"_breakpoint");

		kifDEFAULT=new KifConst(NULL,NULL,"_default");

		kifCONTINUE=new KifConst(NULL,NULL,"_continue");

		kifNEG=new KifConst(NULL,NULL,"_negation");

		kifMINUSONE=new KifInteger(NULL,NULL,-1);
		kifMINUSONE->Setreference();

		kifTRUE=new KifBoolean(NULL,NULL,true);
		kifFALSE=new KifBoolean(NULL,NULL,false);

		kifTypeSTRING=new KifString(NULL,NULL);
		kifTypeINTEGER=new KifInteger(NULL,NULL);
		kifTypeFLOAT=new KifFloat(NULL,NULL);
		kifTypeBOOLEAN=new KifBoolean(NULL,NULL);
		kifTypeVECTOR=new KifVector(NULL,NULL);
	}
}

class KifComparison {
public:

	KifDomain* domain;
	KifFunction* compare;

	KifComparison(KifDomain* dom,KifFunction* comp) {
		domain=dom;
		compare=comp;
	}

	bool operator() (KifElement* i,KifElement* j) { 
		KifElement* ke=domain->applyfullfunc(i,j,compare);
		return  ke->Boolean();
	}
};


static void MapFeatureSet(GlobalParseur* parseur,Vecteur* v,KifMap* map) {
    for (register int i=v->TailleVect-1;i>=0;i--) {
        STVecteur val=(STVecteur)v->vecteur[i];
        if (val!=0) {
            //On itere a l'envers
            for (register int j=gBitVect-1;j>=0;j--) {
                //Plus de bit a examiner on s'arrete
                if (val==0)
                    break;
                //le nombre est negatif, le jieme bit etait a un
                if (val < 0) {
                    //nous calculons sa position absolue
                    int pos= (i*gBitVect + j)*2;       
                    string att=parseur->TableDesTraits[pos];
                    string val=parseur->TableDesTraits[pos+1];
					KifString katt(NULL,NULL,att);
					KifElement* kval=new KifString(parseur->kifcode,NULL,val);
					map->Setvalue(kval,&katt);
                }
                //nous decalons tous les bits d'un cran vers la gauche, pour les
                //amener les uns derriere les autres en position de bit de signe.
                val<<=1;
            }
        }
    }
}

static void MapFeatureDependency(GlobalParseur* parseur,ResultatFonction* f,KifMap* map) {
	MapFeatureSet(parseur,f->traits->valeur,map);	
}

static KifElement* KifSurface(GlobalParseur* parseur,Noeud* n) {
	string surface;
	n->Surface(surface);
	KifElement* ksurface=new KifString(parseur->kifcode,NULL,surface);
	return ksurface;
}

static void MapSurface(GlobalParseur* parseur,Noeud* n,KifMap* map) {
	string surfacename=parseur->SURFACE;
	KifElement* ksurface=KifSurface(parseur,n);
	KifElement* ksurfacename=new KifString(parseur->kifcode,NULL,surfacename);
	map->Setvalue(ksurface,ksurfacename);
}

static KifElement* KifLemma(GlobalParseur* parseur,Noeud* n) {
	string lemma;
	n->Lemme(lemma);
	KifElement* klemma=new KifString(parseur->kifcode,NULL,lemma);
	return klemma;
}

static void MapLemma(GlobalParseur* parseur,Noeud* n,KifMap* map) {
	string lemmaname=parseur->LEMME;
	KifElement* klemmaname=new KifString(parseur->kifcode,NULL,lemmaname);
	KifElement* klemma=KifLemma(parseur,n);
	map->Setvalue(klemma,klemmaname);
}

static void MapFeatureNode(GlobalParseur* parseur,Noeud* n,KifMap* map) {
	Vecteur* v=n->source->traitsactif->valeur;
	MapFeatureSet(parseur,v,map);
	MapSurface(parseur,n,map);
	MapLemma(parseur,n,map);
}


//===================================================================

KifElement* KifCode::Returnerror(string message) {
	if (error)
		return errorptr;

	message+=" at line: ";	
	long line=tabledebugline[currentline];
	map<long,string>::reverse_iterator it;
	string filename="";
	for (it=tabledebugfilename.rbegin();it!=tabledebugfilename.rend();it++) {
		if (it->first<=currentline) {
			filename=it->second;
			break;
		}
	}
	char ch[100];
	sprintf_s(ch,100,"%d",line);
	message+=ch;
	message+=" in "+filename;

	error=true;
	errorptr=new KifError(this,NULL,message);
	return errorptr;
}

static KifElement* PreReplace(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	//Replace an element with another
	if (callfunc->parameters.size()!=3)
		return kifcodeptr->Returnerror(kifErrorStrings[128]);
	KifElement* kstr=callfunc->parameters[0]->Execute(kifTypeVECTOR,dom,kifNULL,false);
	KifElement* kreg=callfunc->parameters[1]->Execute(kifTypeVECTOR,dom,kifNULL,false);
	KifElement* krep=callfunc->parameters[2]->Execute(kifTypeVECTOR,dom,kifNULL,false);
	
	if (kstr->type==kifGraph) {
		KifGraph* kg=(KifGraph*)kstr;
		return kg->Replace(kreg,krep);
	}

	string str=kstr->String();
	string reg=kreg->String();
	string rep=krep->String();
	string res=replacestringwithautomaton(str,reg,rep);
	return new KifString(kifcodeptr,NULL,res);
}


static KifElement* PreCreateDirectory(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[186]);

	KifElement* kpath=callfunc->parameters[0]->Execute(kifTypeVECTOR,dom,kifNULL,false);
	string path=kpath->String();
	CreationRepertoire(STR(path),0);
	return kifTRUE;
}


static KifElement* PreListDirectory(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc)
{   
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[187]);

	KifElement* kpath=callfunc->parameters[0]->Execute(kifTypeVECTOR,dom,kifNULL,false);
	string path=kpath->String();
	oslink::directory dir(path);
	KifVector* kvect=new KifVector(kifcodeptr,NULL);
	while (dir)
      kvect->Push(new KifString(kifcodeptr,NULL,dir.next()));
	return kvect;
}

static KifElement* PreRealPath(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[185]);

	KifElement* kpath=callfunc->parameters[0]->Execute(kifTypeVECTOR,dom,kifNULL,false);
	char localpath[1024];
	string path=kpath->String();
#ifdef WIN32
	_fullpath(localpath,STR(path),1024);
#else
	realpath(STR(path),localpath);
#endif
	return new KifString(kifcodeptr,NULL,localpath);
}

static KifElement* PreRegex(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[89]);

	KifElement* kstr=callfunc->parameters[0]->Execute(kifTypeVECTOR,dom,kifNULL,false);
	KifElement* kreg=callfunc->parameters[1]->Execute(kifTypeVECTOR,dom,kifNULL,false);
	string str=kstr->String();
	string reg=kreg->String();
	automate a(USTR(reg),(void*)1);
	if (pattern->type==kifVector) {
		KifVector* vect=new KifVector(kifcodeptr,NULL);
		VECTA<string*> vectr;
          a.rechercheavecextractiondansvecteur(USTR(str),vectr);
		  for (int iv=0;iv<vectr.dernier;iv++)
			  vect->Push(new KifString(kifcodeptr,NULL,*vectr[iv]));
		  vectr.nettoie();
		  return vect;
	}
	if (pattern->type==kifInteger || pattern->type==kifFloat || pattern->type==kifString) {
		int res=a.rechercheavecextraction(USTR(str),reg);
		if (pattern->type==kifString)
			return new KifString(kifcodeptr,NULL,reg);
		return new KifInteger(kifcodeptr,NULL,res);
	}
	
	etat* e=a.recherche(USTR(str)); 
	if (e==NULL) 
		return kifFALSE;
	return kifTRUE;	
}

static KifElement* PreSplit(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2 && callfunc->parameters.size()!=3)
		return kifcodeptr->Returnerror(kifErrorStrings[0]);

	//Third parameter should be a vector
	KifVector* kvect;
	bool returnvalue=false;
	if (callfunc->parameters.size()==3) {
		KifElement* kf=callfunc->parameters[2]->Execute(kifTypeVECTOR,dom,kifNULL,false);
		if (kf==kifNULL || kf->type!=kifVector)
			return kifcodeptr->Returnerror(kifErrorStrings[1]);
		kvect=(KifVector*)kf;
		kvect->Clear();
		returnvalue=true;
	}
	else
		kvect=new KifVector(kifcodeptr,NULL);

	//First parameter is a string to split
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();

	//Second parameter is the splitter string
	KifElement* splitter=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thesplitter=splitter->String();
	if (thesplitter=="") {		
		for (int i=0;i<thestr.size();i++)
			kvect->Push(new KifString(kifcodeptr,NULL,c_char_get(USTR(thestr),i)));
		if (returnvalue)
			return kifTRUE;
		else
			return kvect;
	}

	size_t pos=0;
	size_t found=0;

	while (pos!=string::npos) {
		found=thestr.find(thesplitter,pos);
		if (found!=string::npos) {
			string n=thestr.substr(pos,found-pos);
			if (n!="")
				kvect->Push(new KifString(kifcodeptr,NULL,n));
			pos=found+thesplitter.size();
		}
		else
			break;
	}
	string s=thestr.substr(pos,thestr.size()-pos);
	if (s!="")
		kvect->Push(new KifString(kifcodeptr,NULL,s));

	if (returnvalue)
		return kifTRUE;
	else
		return kvect;	
}

static KifElement* PreParse(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[180]);
	
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	if (ke->type==kifParser) {
		KifParser* kpars=(KifParser*)ke;
		if (kpars->ipar==-1)
			return kifcodeptr->Returnerror(kifErrorStrings[181]);
		KifElement* ks=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
		ostringstream os;
		//In the case of an external CALL within XIP as a library, we use a XipResult as well
		if (kifcodeptr->parseur!=NULL && kifcodeptr->parseur->AppelExterne!=NULL) {
			kifcodeptr->parseur->InitialisationAvecParseur(kpars->ipar);
			XipResult* xip=XipCreateCurrentXipResult(kpars->ipar);
			XipParseStringAndOS(ks->String(),kpars->ipar,xip,&os,XML_NONE,1);
		}
		else
			XipParseStringOS(ks->String(),kpars->ipar,&os);
		
		if (pattern!=kifNULL) {
			ks=new KifString(kifcodeptr,NULL,os.str());
			return ks;
		}
		else {
			*kifcodeptr->os<<STR(os.str());
			return kifTRUE;
		}
	}
	return kifcodeptr->Returnerror(kifErrorStrings[182]);
}

static KifElement* PreConvertString(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	return new KifString(kifcodeptr,NULL,ke->String());
}

static KifElement* PreConvertFloat(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	return new KifFloat(kifcodeptr,NULL,ke->Float());
}

static KifElement* PreConvertInteger(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	return new KifInteger(kifcodeptr,NULL,ke->Integer());
}

static KifElement* PreConvertBoolean(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	return new KifBoolean(kifcodeptr,NULL,ke->Boolean());
}

static KifElement* PreLoad(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	KifElement* filename;
	if (ke->type==kifFST) {
		if (callfunc->parameters.size()!=5)
			return kifcodeptr->Returnerror(kifErrorStrings[129]);

		filename=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
		KifElement* flags=callfunc->parameters[2]->Execute(kifTypeSTRING,dom,kifNULL,false);
		KifElement* ty=callfunc->parameters[3]->Execute(kifTypeSTRING,dom,kifNULL,false);
		KifElement* utf8=callfunc->parameters[4]->Execute(kifTypeSTRING,dom,kifNULL,false);
		return ((KifFST*)ke)->Load(filename,flags,ty,utf8);
	}

	if (ke->type==kifKiF) {
		if (callfunc->parameters.size()!=2)
			return kifcodeptr->Returnerror(kifErrorStrings[129]);
		KifKiF* kpars=(KifKiF*)ke;
		if (kpars->ipar!=-1)
			return kifcodeptr->Returnerror(kifErrorStrings[179]);
		filename=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
		string flname=filename->String();
		vector<string> paths;
		paths.push_back(flname);

		vector<string> arguments;
		for (int i=0;i<kifcodeptr->arguments.size();i++)
			arguments.push_back(kifcodeptr->arguments[i]);

		string error;

		try {            
			kpars->ipar=KifLoad(paths,arguments,false);
			if (kpars->ipar==-1) {
				error=kifErrorStrings[184];
				error+=flname;
				kifcodeptr->Returnerror(error);
				return kifFALSE;
			}

			kpars->name=flname;
			kifCodeVector[kpars->ipar]->parseur=kifcodeptr->parseur;
			kifCodeVector[kpars->ipar]->debug=kifcodeptr->debug;
			kifCodeVector[kpars->ipar]->debugging=kifcodeptr->debugging;
		}
		catch (XIPError* message) {
			error=message->message;
			error+=" in "+flname;
			error+=" line: ";
			char ch[100];
			sprintf_s(ch,10,"%d",message->line);
			error+=ch;					
			kifcodeptr->Returnerror(error);
			return kifFALSE;
		}
		return kifTRUE;
	}

	if (ke->type==kifParser) {
		if (callfunc->parameters.size()!=2)
			return kifcodeptr->Returnerror(kifErrorStrings[129]);
		KifParser* kpars=(KifParser*)ke;
		if (kpars->ipar!=-1)
			return kifcodeptr->Returnerror(kifErrorStrings[179]);
		filename=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
		string flname=filename->String();
		try {            
			kpars->ipar=XipGrmFile(flname);
			kpars->name=flname;
			if (kifcodeptr->parseur!=NULL)
				kifcodeptr->parseur->InitialisationAvecParseur(kpars->ipar);
		}
		catch (XIPError* message) {
			string error;
			error=message->message;
			error+=" in "+flname;
			error+=" line: ";
			char ch[100];
			sprintf_s(ch,10,"%d",message->line);
			error+=ch;					
			kifcodeptr->Returnerror(error);
		}
		return kifTRUE;
	}

	return kifcodeptr->Returnerror(kifErrorStrings[131]);
}


#ifdef NTMCONNECTION
static KifElement* PreCompilenet(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=3)
		return kifcodeptr->Returnerror(kifErrorStrings[167]);
	
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	if (ke->type!=kifFST)
		return kifcodeptr->Returnerror(kifErrorStrings[168]);
	KifElement* kfmap=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
	if (kfmap->type!=kifMap)
		return kifcodeptr->Returnerror(kifErrorStrings[168]);
	KifElement* kfile=callfunc->parameters[2]->Execute(kifTypeSTRING,dom,kifNULL,false);

	return ((KifFST*)ke)->Compilenet((KifMap*)kfmap,kfile,false);

}

static KifElement* PreCompilenetwith(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=3)
		return kifcodeptr->Returnerror(kifErrorStrings[167]);
	
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	if (ke->type!=kifFST)
		return kifcodeptr->Returnerror(kifErrorStrings[168]);
	KifElement* kfmap=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
	if (kfmap->type!=kifMap)
		return kifcodeptr->Returnerror(kifErrorStrings[168]);
	KifElement* kfile=callfunc->parameters[2]->Execute(kifTypeSTRING,dom,kifNULL,false);

	return ((KifFST*)ke)->Compilenet((KifMap*)kfmap,kfile,true);

}

static KifElement* PreTokup(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[129]);
	
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	if (ke->type!=kifFST)
		return kifcodeptr->Returnerror(kifErrorStrings[131]);
	KifVector* kvect=new KifVector(kifcodeptr,NULL);
	KifElement* str=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
	((KifFST*)ke)->Tokup(str,kvect);
	return kvect;
}

static KifElement* PreTokdown(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2 && callfunc->parameters.size()!=3)
		return kifcodeptr->Returnerror(kifErrorStrings[129]);
	
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	if (ke->type!=kifFST)
		return kifcodeptr->Returnerror(kifErrorStrings[131]);
	KifElement* str=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
	KifElement* feats=kifNULL;
	if (callfunc->parameters.size()==3)
		feats=callfunc->parameters[2]->Execute(kifTypeSTRING,dom,kifNULL,false);
	return ((KifFST*)ke)->Tokdown(str,feats,pattern);
}

#endif

static KifElement* PreList(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[2]);
	
	KifElement* ke=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	KifVector* kvect=new KifVector(kifcodeptr,NULL);	
	ke->List(kvect);
	return kvect;
}

static KifElement* PreUTF8ToLatin(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[3]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();

	string res=conversionUTF8VersLatin((unsigned char*)thestr.c_str());
	mystr=new KifString(kifcodeptr,NULL,res);
	return mystr;		
}

static KifElement* PreLatinToUTF8(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[4]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();

	string res=conversionLatinVersUTF8((unsigned char*)thestr.c_str());
	mystr=new KifString(kifcodeptr,NULL,res);
	return mystr;		
}


static KifElement* PreLeft(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[5]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();

	//Second parameter is an integer
	KifElement* sz=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
	int nbchar=sz->Integer();

	string res=c_left(thestr,nbchar);
	mystr=new KifString(kifcodeptr,NULL,res);
	return mystr;		
}

static KifElement* PreRight(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[6]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();

	//Second parameter is an integer
	KifElement* sz=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
	int nbchar=sz->Integer();

	string res=c_right(thestr,nbchar);
	mystr=new KifString(kifcodeptr,NULL,res);
	return mystr;		
}

static KifElement* PreMid(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=3)
		return kifcodeptr->Returnerror(kifErrorStrings[7]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();

	//Second parameter is a position
	KifElement* sz=callfunc->parameters[1]->Execute(kifTypeSTRING,dom,kifNULL,false);
	int pos=sz->Integer();

	//Third parameter is a number of characters
	sz=callfunc->parameters[2]->Execute(kifTypeSTRING,dom,kifNULL,false);
	int nbchar=sz->Integer();

	string res=c_middle(thestr,pos,nbchar);
	mystr=new KifString(kifcodeptr,NULL,res);
	return mystr;		
}

static KifElement* PreIsLower(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[8]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();
	return new KifBoolean(kifcodeptr,NULL,c_Islower(thestr));
}

static KifElement* PreIsUpper(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[9]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();
	return new KifBoolean(kifcodeptr,NULL,c_Isupper(thestr));
}

static KifElement* PreIsAlpha(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[9]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();
	return new KifBoolean(kifcodeptr,NULL,c_Isalpha(thestr));
}


static KifElement* PreLower(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[10]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();
	thestr=c_Tolower(thestr);
	return new KifString(kifcodeptr,NULL,thestr);
}

static KifElement* PreUpper(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[11]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();
	thestr=c_Toupper(thestr);
	return new KifString(kifcodeptr,NULL,thestr);
}


static KifElement* PreTrim(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[12]);

	//First parameter is a string
	KifElement* mystr=callfunc->parameters[0]->Execute(kifTypeSTRING,dom,kifNULL,false);
	string thestr=mystr->String();
	int start=0;
	for (;start<thestr.size();start++) {
		if (!strchr("\n\r\t ",thestr[start]))
			break;
	}

	int end=thestr.size()-1;
	for (;end>=0;end--) {
		if (!strchr("\n\r\t ",thestr[end]))
			break;
	}
	int keep=end-start+1;

	string res=thestr.substr(start,keep);
	mystr=new KifString(kifcodeptr,NULL,res);
	return mystr;		
}

static KifElement* PreEnvironment(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	KifCallFunction* calltop=(KifCallFunction*)kifcodeptr->Top();
	if (calltop==NULL)
		return kifNULL;
	if (callfunc->parameters.size()==0) {
		//This is a case, where we are looking for the type of the pattern
		if (pattern->type==kifInteger || pattern->type==kifFloat) {
			int r=pattern->type;
			return new KifInteger(kifcodeptr,NULL,r);
		}

		string resstr=calltop->environment->Type();
		return new KifString(kifcodeptr,NULL,resstr);
	}
	return kifNULL;
}


static KifElement* PreType(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* res=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (pattern->type==kifInteger || pattern->type==kifFloat) {
		int r=pattern->type;
		return new KifInteger(kifcodeptr,NULL,r);
	}
	string resstr=res->Type();
	return new KifString(kifcodeptr,NULL,resstr);
}

static KifElement* PreSetDate(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()<=2 || callfunc->parameters.size()>=8)
		return kifcodeptr->Returnerror(kifErrorStrings[13]);

	//First parameter is a time
	KifElement* timeinfo=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (timeinfo->type!=kifTime)
		kifcodeptr->Returnerror(kifErrorStrings[14]);

	KifTime* ktime=(KifTime*)timeinfo;
	struct tm* temps;
	time_t x=0;
	temps=localtime(&x);

	time_t v=0;
	int fulldate=0;

	//Year
	long res=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false)->Integer();
	if (res>0) {
		temps->tm_year=res-1900;
		fulldate=100;
	}

	//Month
	if (callfunc->parameters.size()>=3) {
		res=callfunc->parameters[2]->Execute(kifNULL,dom,kifNULL,false)->Integer();
		if (res>0) {
			fulldate+=10;
			temps->tm_mon=res-1;
		}
	}

	//Day
	if (callfunc->parameters.size()>=4) {
		res=callfunc->parameters[3]->Execute(kifNULL,dom,kifNULL,false)->Integer();
		if (res>0) {
			fulldate+=1;
			temps->tm_mday=res;		
		}
	}
	//Hour
	if (callfunc->parameters.size()>=5) {
		res=callfunc->parameters[4]->Execute(kifNULL,dom,kifNULL,false)->Integer();
		if (res>=0 && res<24) {
			//reference is 1AM
			if (fulldate==0)
				res+=2;
			else
				if (fulldate==1)
					res+=1;
			temps->tm_hour=res;
		}
	}

	//Minute
	if (callfunc->parameters.size()>=6) {
		res=callfunc->parameters[5]->Execute(kifNULL,dom,kifNULL,false)->Integer();
		if (res>=0 && res<60)
			temps->tm_min=res;
	}

	//Second
	if (callfunc->parameters.size()>=7) {
		res=callfunc->parameters[6]->Execute(kifNULL,dom,kifNULL,false)->Integer();
		if (res>=0 && res<60)
			temps->tm_sec=res;
	}

		
	x=mktime(temps);
	if (x<=0)
		return kifFALSE;
	ktime->value=x;
	return kifTRUE;
}

static KifElement* PreDate(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[15]);

	//First parameter is a string
	KifElement* time=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (time->type!=kifTime)
		kifcodeptr->Returnerror(kifErrorStrings[16]);
	string d=time->String();
	return new KifString(kifcodeptr,NULL,d);
}


static KifElement* PreYear(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[17]);

	//First parameter is a string
	KifElement* time=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (time->type!=kifTime)
		kifcodeptr->Returnerror(kifErrorStrings[18]);
	struct tm* temps = localtime(&((KifTime*)time)->value); 
	long vl=temps->tm_year+1900;
	return new KifInteger(kifcodeptr,NULL,vl);
}

static KifElement* PreDay(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[19]);

	//First parameter is a string
	KifElement* time=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (time->type!=kifTime)
		kifcodeptr->Returnerror(kifErrorStrings[20]);
	struct tm* temps = localtime(&((KifTime*)time)->value); 
	long vl=temps->tm_mday;
	return new KifInteger(kifcodeptr,NULL,vl);
}

static KifElement* PreHour(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[21]);

	//First parameter is a string
	KifElement* time=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (time->type!=kifTime)
		kifcodeptr->Returnerror(kifErrorStrings[22]);
	struct tm* temps = localtime(&((KifTime*)time)->value); 
	char ch[100];
	sprintf(ch,"%d:%d:%d",temps->tm_hour,temps->tm_min,temps->tm_sec);
	return new KifString(kifcodeptr,NULL,ch);
}

static KifElement* PreMonth(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[23]);

	//First parameter is a string
	KifElement* time=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (time->type!=kifTime)
		kifcodeptr->Returnerror(kifErrorStrings[24]);
	struct tm* temps = localtime(&((KifTime*)time)->value); 
	long vl=temps->tm_mon+1;
	return new KifInteger(kifcodeptr,NULL,vl);
}


static KifElement* PreCatch(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (kifcodeptr->error==false || kifcodeptr->errorptr==NULL)
		return kifFALSE;
	if (callfunc->parameters.size()!=1) {
		kifcodeptr->error=false;
		kifcodeptr->errorptr=NULL;
		return kifTRUE;
	}
	kifcodeptr->error=false;
	KifElement* res=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,true);
	string resstr=kifcodeptr->errorptr->String();
	if (res->Returnvalue()->type==kifString)
		res->Setvalue(new KifString(kifcodeptr,NULL,resstr));
	else {
		//We take the first three characters
		string resex=resstr.substr(0,3);
		//Which we transform into a number
		res->Setvalue(new KifInteger(kifcodeptr,NULL,atol(STR(resex))));
	}
	kifcodeptr->errorptr=NULL;
	return kifTRUE;
}

static KifElement* PreRaise(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (kifcodeptr->error==true)
		return kifFALSE;
	
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[25]);

	KifElement* res=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,true);
	string resstr=res->String();
	kifcodeptr->Returnerror(resstr);
	return res;
}

static KifElement* PreTypeIs(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifFALSE;
	KifElement* res=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	KifElement* typ=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false);
	string resstr=res->Type();
	string typstr=typ->String();
	if (resstr==typstr)
		return kifTRUE;
	return kifFALSE;
}

static KifElement* PreMath(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* res=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	double v=res->Float();	
	v=kifMathFunctions[myself->name](v);
	KifFloat* kres=new KifFloat(kifcodeptr,NULL,v);
	return kres;
}

static KifElement* PreOrd(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* res=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	string s=res->String();
	if (s.size()>=1) {
		long x;
		if (pattern->type==kifVector) {
			vector<int> vect;
			c_char_index_code_all(s,vect);
			KifVector* kvect=new KifVector(kifcodeptr,NULL);
			for (int i=0;i<vect.size();i++)
				kvect->Push(new KifInteger(kifcodeptr,NULL,vect[i]));
			return kvect;
		}
		x=c_char_index_code(s,0);
		return new KifInteger(kifcodeptr,NULL,x);
	}
	return kifNULL;
}

static KifElement* PreChr(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* res=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	long x=res->Integer();
	if (x>=0 && x<=255) {
		char c=(char)x;
		string s;
		s=c;
		return new KifString(kifcodeptr,NULL,s);
	}
	return kifNULL;
}


static KifElement* PreReturn(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1) {
		callfunc->Setreturnvalue(kifNULL);
		return kifTRUE;
	}
	KifElement* res=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	callfunc->Setreturnvalue(res);
	return res;
}

static KifElement* PreBreak(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	return kifBREAK;
}

static KifElement* PreBreakpoint(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	kifcodeptr->debug=true;
	kifcodeptr->debugging=true;
	return kifTRUE;
}

static KifElement* PreContinue(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	return kifCONTINUE;
}

static KifElement* PrePrint(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	KifElement* res=kifNULL;
	ostream* os=kifcodeptr->os;
	for (size_t i=0;i<callfunc->parameters.size();i++) {
		res=callfunc->parameters[i]->Execute(kifTypeSTRING,dom,kifNULL,false);
		*os<<res->String();
	}
	return res;
}

static KifElement* PrePrinterr(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	KifElement* res=kifNULL;
	ostream* os=kifcodeptr->os;
	for (size_t i=0;i<callfunc->parameters.size();i++) {
		res=callfunc->parameters[i]->Execute(kifTypeSTRING,dom,kifNULL,false);
		cerr<<res->String();
	}
	return res;
}

static KifElement* PreSeek(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[161]);

	KifElement* kf=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	KifElement* ki=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false);
	if (kf->type!=kifFile)
		return kifcodeptr->Returnerror(kifErrorStrings[162]);
	KifFile* kfile=(KifFile*)kf;
	return kfile->Seek(ki->Integer());
}

static KifElement* PreTell(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[163]);

	KifElement* kf=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kf->type!=kifFile)
		return kifcodeptr->Returnerror(kifErrorStrings[164]);
	KifFile* kfile=(KifFile*)kf;
	return kfile->Tell();
}


static KifElement* PreWrite(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()<2)
		return kifcodeptr->Returnerror(kifErrorStrings[26]);

	KifElement* kf=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);

	KifElement* res=kifNULL;
	if (kf->type==kifMatrix) {
		//First parameter should be the filename
		res=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false);
		string mname=res->String();
		string thetype;
		if (callfunc->parameters.size()>=3) {
			res=callfunc->parameters[2]->Execute(kifNULL,dom,kifNULL,false);
			thetype=res->String();
		}
		kf->Save(mname,thetype);
		return kifTRUE;
	}

	if (kf->type!=kifFile)
		return kifcodeptr->Returnerror(kifErrorStrings[27]);

	KifFile* kfile=(KifFile*)kf;

	for (size_t i=1;i<callfunc->parameters.size();i++) {
		res=callfunc->parameters[i]->Execute(kifNULL,dom,kifNULL,false);		
		kfile->Write(res->String());
	}
	return kifTRUE;
}

static KifElement* PreRead(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;

	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type!=kifFile)
		return kifcodeptr->Returnerror(kifErrorStrings[28]);
	KifFile* kfile=(KifFile*)kf;

	KifElement* res=kfile->Read();
	return res;
}

static KifElement* PreReadOneLine(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;

	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type!=kifFile)
		return kifcodeptr->Returnerror(kifErrorStrings[29]);

	KifFile* kfile=(KifFile*)kf;

	KifElement* res=kfile->Readoneline();
	return res;
}

//close(file)
static KifElement* PreClose(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifFALSE;
	//First parameter should be a file
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type!=kifFile)
		return kifcodeptr->Returnerror(kifErrorStrings[30]);

	KifFile* kfile=(KifFile*)kf;
	kfile->Close();
	return kifTRUE;
}

//openread(file,name)
static KifElement* PreOpenReadfile(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifFALSE;
	//First parameter should be a file
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type!=kifFile)
		return kifcodeptr->Returnerror(kifErrorStrings[31]);

	//second parameter is a string
	KifElement* name=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
	KifFile* kfile=(KifFile*)kf;
	if (!kfile->Openread(name->String())) {
		string mess=kifErrorStrings[32]+name->String();
		return kifcodeptr->Returnerror(mess);
	}
	return kifTRUE;
}

//openread(file,name)
static KifElement* PreEndOfFile(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifTRUE;
	//First parameter should be a file
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type!=kifFile)
		return kifcodeptr->Returnerror(kifErrorStrings[33]);
	KifFile* kfile=(KifFile*)kf;
	return kfile->EndOfFile();
}


//openwrite(file,name)
static KifElement* PreOpenWritefile(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifFALSE;
	//First parameter should be a file
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type!=kifFile)
		return kifcodeptr->Returnerror(kifErrorStrings[34]);
	//second parameter is a string
	KifElement* name=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
	KifFile* kfile=(KifFile*)kf;
	if (!kfile->Openwrite(name->String())) {
		string mess=kifErrorStrings[35]+name->String();
		return kifcodeptr->Returnerror(mess);
	}
	return kifTRUE;
}

static KifElement* PreInsert(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=3)
		return kifcodeptr->Returnerror(kifErrorStrings[36]);
	//First parameter should be a vector
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type==kifVector) {
		//second parameter is anything
		KifElement* kidx=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
		KifElement* kval=callfunc->parameters[2]->Execute(pattern,dom,kifNULL,false);
		KifVector* kvect=(KifVector*)kf;
		size_t idx=kidx->Integer();
		if (idx>=kvect->values.size())
			kvect->Push(val);
		else
			kvect->values.insert(kvect->values.begin()+idx,kval);
		return kifTRUE;
	}

	if (kf->type==kifString || kf->type==kifStringPtr) {
		string* refval=kf->ValuePtr();
		if (refval==NULL)
			return kifFALSE;
		//second parameter is anything
		KifElement* kidx=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
		KifElement* kval=callfunc->parameters[2]->Execute(pattern,dom,kifNULL,false);
		KifString* kvect=(KifString*)kf;
		size_t idx=kidx->Integer();
		string val=kval->String();
		if (c_char_index_insert(*refval, val,idx)==false)
			return kifFALSE;
		return kifTRUE;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[37]);
}

static KifElement* PreJoin(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[165]);
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type==kifVector) {
		KifVector* kvect=(KifVector*)kf;
		//The separator
		KifElement* kstrsep=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
		string sep=kstrsep->String();
		string res;
		vector<KifElement*>::iterator it;
		bool beg=true;
		for (it=kvect->values.begin();it!=kvect->values.end();it++) {
			if (beg==false)
				res+=sep;
			beg=false;
			res+=(*it)->String();			
		}
		return new KifString(kifcodeptr,NULL,res);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[166]);
}


static KifElement* PrePush(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()<2)
		return kifcodeptr->Returnerror(kifErrorStrings[38]);
	//First parameter should be a vector
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type==kifVector) {
		//second parameter is anything
		KifElement* res;
		for (size_t i=1;i<callfunc->parameters.size();i++) {
			res=callfunc->parameters[i]->Execute(pattern,dom,kifNULL,false);
			kf->Push(res);
		}
		return kifTRUE;
	}
	if (kf->type==kifXipDependency || kf->type==kifXipGeneration) {

		KifXipDependency* knode=(KifXipDependency*)kf;
		if (knode->value==NULL)
			return kifFALSE;
		KifElement* res;
		char* s;
		for (size_t i=1;i<callfunc->parameters.size();i++) {
			res=callfunc->parameters[i]->Execute(pattern,dom,kifNULL,false);
			string sres=res->String();
			s=DupliqueChaine(STR(sres));
			knode->value->stack.ajoute(s);
		}
		return kifTRUE;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[39]);
}

static KifElement* PreExtract(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[176]);
	//First parameter should be a vector
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	
	
	if (kf->type==kifGraph) {
		KifGraph* kg=(KifGraph*)kf;
		if (kg->graph==NULL)
			return kifFALSE;
		KifElement* ks=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
		return kg->Extract(ks);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[177]);
}


static KifElement* PrePop(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1 && callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[40]);
	//First parameter should be a vector
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	
	
	if (kf->type==kifGraph) {
		KifGraph* kg=(KifGraph*)kf;
		if (kg->graph==NULL)
			return kifFALSE;
		KifElement* ks=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
		return kg->Remove(ks);
	}

	//second parameter is an integer (the number of pop)
	KifElement* nb=NULL;
	if (callfunc->parameters.size()==2)
		nb=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
	else
		nb=kifMINUSONE;

	if (kf->type==kifVector || kf->type==kifMap || kf->type==kifString || kf->type==kifStringPtr)
		return kf->Pop(nb);


	int ix=nb->Integer();
	if (kf->type==kifXipDependency || kf->type==kifXipGeneration) {

		KifXipDependency* knode=(KifXipDependency*)kf;
		if (knode->value==NULL)
			return kifFALSE;
		
		char* s;
		if (ix==-1)
			s=knode->value->stack.retire();
		else
			s=knode->value->stack.retireElement(ix);
			
		if (s!=NULL) {
			string str=s;
			delete s;		
			return new KifString(kifcodeptr,NULL,str);
		}
	}

	return kifcodeptr->Returnerror(kifErrorStrings[41]);
}

static KifElement* PreLast(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[42]);
	//First parameter should be a vector
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	//second parameter is an integer (the number of pop)

	if (kf->type==kifVector || kf->type==kifMap || kf->type==kifString || kf->type==kifStringPtr)
		return kf->Last();

	if (kf->type==kifXipNode) {

		KifXipNode* knode=(KifXipNode*)kf;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[43]);

		Noeud* n=knode->value->fils;
		if (n==NULL || n->fils->dernier==NULL)
			return kifNULL;
		n=n->fils->dernier;

		knode=new KifXipNode(kifcodeptr,dom,n);
		return knode;
	}

	if (kf->type==kifXipGeneration) {
		KifXipGeneration* kgen=(KifXipGeneration*)kf;
		if (kgen->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[44]);
		ResultatFonction* rf=NULL;
		rf=filsde(kgen->value,rf,OUI);
		kgen=new KifXipGeneration(kifcodeptr,dom,rf);
		return kgen;
	}

	return kifcodeptr->Returnerror(kifErrorStrings[45]);
}

static KifElement* PreSort(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[46]);

	//First parameter should be a vector
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	//second parameter is an integer (the number of pop)

	if (kf->type==kifVector) {
		KifVector* kvect=(KifVector*)kf;
		//Second parameter should be function
		kf=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
		if (kf->type==kifFunction) {
			KifComparison kfcomp(dom,(KifFunction*)kf);
			sort(kvect->values.begin(),kvect->values.end(),kfcomp);
			return kifTRUE;
		}
	}

	return kifcodeptr->Returnerror(kifErrorStrings[47]);
}

static KifElement* PreSum(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[169]);

	//First parameter should be a vector
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	//second parameter is an integer (the number of pop)
	double v=0;
	if (kf->type==kifVector) {
		KifVector* kvect=(KifVector*)kf;
		vector<KifElement*>::iterator it;		
		for (it=kvect->values.begin();it!=kvect->values.end();it++)
			v+=(*it)->Float();
		return new KifFloat(kifcodeptr,NULL,v);
	}
	if (kf->type==kifMap) {
		KifMap* kvect=(KifMap*)kf;
		map<string,KifElement*>::iterator it;		
		for (it=kvect->values.begin();it!=kvect->values.end();it++)
			v+=it->second->Float();
		return new KifFloat(kifcodeptr,NULL,v);
	}

	return kifcodeptr->Returnerror(kifErrorStrings[170]);
}

static KifElement* PreProduct(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[171]);

	//First parameter should be a vector
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	//second parameter is an integer (the number of pop)
	double v=1;
	if (kf->type==kifVector) {
		KifVector* kvect=(KifVector*)kf;
		vector<KifElement*>::iterator it;		
		for (it=kvect->values.begin();it!=kvect->values.end();it++)
			v*=(*it)->Float();
		return new KifFloat(kifcodeptr,NULL,v);
	}
	if (kf->type==kifMap) {
		KifMap* kvect=(KifMap*)kf;
		map<string,KifElement*>::iterator it;		
		for (it=kvect->values.begin();it!=kvect->values.end();it++)
			v*=it->second->Float();
		return new KifFloat(kifcodeptr,NULL,v);
	}

	return kifcodeptr->Returnerror(kifErrorStrings[172]);
}



static KifElement* PreApply(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()<1)
		return kifcodeptr->Returnerror(kifErrorStrings[48]);
	//First parameter should be a XIP rule
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);

	if (kf->type==kifCall)
		return kf->Execute(pattern,callfunc,kifNULL,false);

	if (kf->type==kifXipRule)
		return kf->Execute(pattern,callfunc,kifNULL,false);
	if (kf->type==kifIterator) {
		KifIterator* iter=(KifIterator*)kf;
		KifElement* val=iter->IteratorValue();
		if (val->type==kifCall)
			return val->Execute(pattern,callfunc,kifNULL,false);
		return val->Execute(pattern,callfunc,kifNULL,false);
	}
	if (kf->type==kifVector) {
		KifVector* kvect=(KifVector*)kf;
		vector<KifElement*>::iterator itx;
		for (itx=kvect->values.begin();itx!=kvect->values.end();itx++)
			(*itx)->Execute(pattern,callfunc,kifNULL,false);
		return kifTRUE;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[49]);
}

static KifElement* PreCreateDependency(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* kval,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=3 || kifcodeptr->parseur==NULL)
		return kifcodeptr->Returnerror(kifErrorStrings[50]);
	//First parameter is the name of our dependency
	KifElement* kname=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	Categorie* cfoncx;
	string name=kname->String();
	cfoncx=kifcodeptr->parseur->RetrouveFonc(STR(name));
	if (cfoncx==NULL) {
		string mess=kifErrorStrings[51]+name;
		return kifcodeptr->Returnerror(mess);
	}

	//Second parameter is a feature map
	KifElement* kfeat=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
	if (kfeat->type!=kifMap)
		return kifcodeptr->Returnerror(kifErrorStrings[52]);
	KifMap* kfeatmap=(KifMap*)kfeat;
	//Third parameter is a vector of nodes
	KifElement* knodes=callfunc->parameters[2]->Execute(pattern,dom,kifNULL,false);
	if (knodes->type!=kifVector)
		return kifcodeptr->Returnerror(kifErrorStrings[53]);
	KifVector* knodevect=(KifVector*)knodes;
	vector<KifElement*>::iterator itx;
	VECTA<Noeud*> lnoeuds;
	for (itx=knodevect->values.begin();itx!=knodevect->values.end();itx++) {
		if ((*itx)->type!=kifXipNode)
			return kifcodeptr->Returnerror(kifErrorStrings[54]);
		KifXipNode* node=(KifXipNode*)(*itx);
		if (node->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[55]);
		lnoeuds.ajoute(node->value);
	}
	Traits* trbase=new Traits(kifcodeptr->parseur);
	map<string,KifElement*>::iterator itm;
	for(itm=kfeatmap->values.begin();itm!=kfeatmap->values.end();itm++) {
		string att=c_Tolower(itm->first);
		string val=c_Tolower(itm->second->String());
		if (kifcodeptr->parseur->XipFeatureTraits(trbase,STR(att),STR(val))==0) {
			delete trbase;
			string mess=kifErrorStrings[56]+att+":"+val+" in CREATEDEPENDENCY";
			return kifcodeptr->Returnerror(mess);
		}
	}
	ResultatFonction* rf=kifcodeptr->parseur->CreeNouvelleDependance(cfoncx,trbase,lnoeuds);	
	if (rf!=NULL) {
		KifXipDependency* kdep=new KifXipDependency(kifcodeptr,NULL,rf);
		return kdep;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[57]);
}

static KifElement* PreSetFeature(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=3  || kifcodeptr->parseur==NULL)
		return kifcodeptr->Returnerror(kifErrorStrings[58]);
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type!=kifXipNode && kf->type!=kifXipDependency && kf->type!=kifXipGeneration)
		return kifcodeptr->Returnerror(kifErrorStrings[59]);
	KifElement* katt=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
	KifElement* kval=callfunc->parameters[2]->Execute(pattern,dom,kifNULL,false);
	string att=c_Tolower(katt->String());
	string vval=c_Tolower(kval->String());
	Traits trbase(kifcodeptr->parseur);
	if (att==kifcodeptr->parseur->LEMME || att==kifcodeptr->parseur->SURFACE) {
		if (kf->type==kifXipNode) {
			char* inter;
			KifXipNode* knode=(KifXipNode*)kf;
			if (knode->value!=NULL) {
				if (att==kifcodeptr->parseur->LEMME) {
					int posmot=knode->value->source->choisie;
					inter=knode->value->source->voc->lemme[posmot];
					libere(inter);
					inter=(char*)DupliqueChaine(USTR(vval));
					knode->value->source->voc->lemme.affecte(posmot,inter);
					knode->value->fils->lemme=inter;
				}
				else {
					inter=knode->value->source->voc->surface;
					libere(inter);
					inter=(char*)DupliqueChaine(STR(vval));
					knode->value->source->voc->surface=inter;
					knode->value->fils->surface=inter;
				}
			}
		}
		return kifFALSE;
	}

	if (kifcodeptr->parseur->XipFeatureTraits(&trbase,STR(att),STR(vval))==0) {
		string mess=kifErrorStrings[60]+att+":"+vval+" in SETFEATURE";
		return kifcodeptr->Returnerror(mess);
	}
	if (kf->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kf;
		if (knode->value!=NULL) {
			knode->value->source->traitsactif->ou(trbase);
			knode->value->source->traitsbase->ou(trbase);
		}
	}
	else {
		KifXipDependency* kdep=(KifXipDependency*)kf;
		if (kdep->value!=NULL)
			kdep->value->traits->ou(trbase);
	}
	return  kifTRUE;
}


static KifElement* PreRemoveFeature(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2  || kifcodeptr->parseur==NULL)
		return kifcodeptr->Returnerror(kifErrorStrings[61]);
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type!=kifXipNode && kf->type!=kifXipDependency && kf->type!=kifXipGeneration)
		return kifcodeptr->Returnerror(kifErrorStrings[62]);

	KifElement* katt=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
	string att=c_Tolower(katt->String());
	if (att==kifcodeptr->parseur->LEMME || att==kifcodeptr->parseur->SURFACE)
		return kifFALSE;

	trait* vattribut=kifcodeptr->parseur->ArbTraits->recherche(STR(att));

	if (!vattribut) {
		string mess=kifErrorStrings[63]+att+" in REMOVEFEATURE";
		return kifcodeptr->Returnerror(mess);
	}

	if (kf->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kf;
		if (knode->value!=NULL) {
			knode->value->source->traitsactif->valeur->etnon(vattribut->valeur);
			knode->value->source->traitsbase->valeur->etnon(vattribut->valeur);
			knode->value->source->traitsactif->filtre->etnon(vattribut->valeur);
			knode->value->source->traitsbase->filtre->etnon(vattribut->valeur);
		}
	}
	else {
		KifXipDependency* kdep=(KifXipDependency*)kf;
		if (kdep->value!=NULL) {
			kdep->value->traits->valeur->etnon(vattribut->valeur);
			kdep->value->traits->filtre->etnon(vattribut->valeur);
		}
	}
	return  kifTRUE;
}


static KifElement* PreFeature(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()<2 || callfunc->parameters.size()>3  || kifcodeptr->parseur==NULL)
		return kifcodeptr->Returnerror(kifErrorStrings[64]);
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type!=kifXipNode && kf->type!=kifXipDependency && kf->type!=kifXipGeneration)
		return kifcodeptr->Returnerror(kifErrorStrings[65]);

	KifElement* katt=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
	string att=c_Tolower(katt->String());
	string vval;
	if (callfunc->parameters.size()==3) {
		KifElement* kval=callfunc->parameters[2]->Execute(pattern,dom,kifNULL,false);
		vval=c_Tolower(kval->String());
	}

	if (att==kifcodeptr->parseur->LEMME || att==kifcodeptr->parseur->SURFACE)
		return kifFALSE;

	trait* vattribut=kifcodeptr->parseur->ArbTraits->recherche(STR(att));
	string mess;
	if (!vattribut) {
		mess=kifErrorStrings[66]+att+" in FEATURE";
		return kifcodeptr->Returnerror(mess);
	}

	Traits* tr=NULL;
	if (kf->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kf;
		if (knode->value!=NULL)
			tr=knode->value->source->traitsactif;
	}
	else {
		KifXipDependency* kdep=(KifXipDependency*)kf;
		if (kdep->value!=NULL)
			tr=kdep->value->traits;
	}

	if (vval=="") {
		if (tr==NULL)
			return kifNULL;
		
		Vecteur v(kifcodeptr->parseur->iTailleVect);
		v.ou(vattribut->valeur);
		v.et(tr->valeur);
		char chaine[100];
		for (int i=0;i<v.TailleVect;i++) {
			if (v.vecteur[i]!=0) {
				v.Valeur(kifcodeptr->parseur,chaine,i);
				return new KifString(kifcodeptr,NULL,chaine);
			}
		}
		return kifNULL;
	}

	trait* vvaleur=kifcodeptr->parseur->RetrouveValeurTrait(vattribut,STR(vval),NON);
    if (vvaleur==NULL) {
		mess=kifErrorStrings[67]+att+":"+vval+" in FEATURE";
		return kifcodeptr->Returnerror(mess);
	}

	if (tr->valeur->contient(vvaleur->valeur)==1)
		return kifTRUE;

	return  kifFALSE;
}



static KifElement* PreClear(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifFALSE;

	//First parameter should be a vector
	KifElement* kf=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kf->type!=kifVector && kf->type!=kifMap)
		return kifcodeptr->Returnerror(kifErrorStrings[68]);
	kf->Clear();
	return kifTRUE;
}

static KifElement* PreSize(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	int sz=kfirst->Size();
	KifElement* ksz=new KifInteger(kifcodeptr,NULL,sz);
	return ksz;
}

static KifElement* PreReverse(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[175]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifVector) {
		KifVector* kvect=(KifVector*)kfirst;
		vector<KifElement*>::reverse_iterator it;
		KifVector* kvectres=new KifVector(kifcodeptr,NULL);
		for (it=kvect->values.rbegin();it!=kvect->values.rend();it++) {
			kvectres->Push(*it);
		}
		return kvectres;
	}
	string s=kfirst->String();
	s=c_char_reverse(s);
	return new KifString(kifcodeptr,NULL,s);
}

static KifElement* PreGetData(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2 && callfunc->parameters.size()!=1)
		return kifFALSE;
	KifMap* map;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (callfunc->parameters.size()==2) {
		KifElement* ksecond=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false);
		if (ksecond->type!=kifMap)
			return kifcodeptr->Returnerror(kifErrorStrings[69]);
		map=(KifMap*)ksecond;
	}
	else
		map=new KifMap(kifcodeptr,NULL);

	if (kfirst->type==kifXMLNode || kfirst->type==kifXMLNodePtr) {

		string index="#tag";
		xmlNodePtr ndp=kfirst->Node();
		if (ndp==NULL)
			return kifNULL;
		string valeur=(char*)ndp->name;
		KifElement* kval=new KifString(kifcodeptr,NULL,valeur);
		KifString kidx(NULL,NULL,index);
		map->Setvalue(kval,&kidx);
		xmlAttrPtr propriete;
		if (ndp->properties!=NULL) {
			propriete=ndp->properties;
			while (propriete!=NULL) {                
				index=(char*)propriete->name;
				valeur=(char*)propriete->children->content;
				kval=new KifString(kifcodeptr,NULL,valeur);
				KifString kidxp(NULL,NULL,index);
				map->Setvalue(kval,&kidxp);
				propriete=propriete->next;
			}           
		}
		return map;
	}
	
	if (kfirst->type==kifXipNode) {

		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL  || kifcodeptr->parseur==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[70]);
		MapFeatureNode(kifcodeptr->parseur,knode->value,map);
		return map;
	}

	if (kfirst->type==kifXipDependency || kfirst->type==kifXipGeneration) {

		KifXipDependency* knode=(KifXipDependency*)kfirst;
		if (knode->value==NULL  || kifcodeptr->parseur==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[71]);
		MapFeatureDependency(kifcodeptr->parseur,knode->value,map);
		return map;
	}

	return kifcodeptr->Returnerror(kifErrorStrings[72]);
}

static KifElement* PreGetName(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	string valeur;
	if (kfirst->type==kifXMLNode || kfirst->type==kifXMLNodePtr) {

		xmlNodePtr ndp=kfirst->Node();
		if (ndp==NULL)
			return kifNULL;
		valeur=(char*)ndp->name;
		return new KifString(kifcodeptr,NULL,valeur);
	}

	if (kfirst->type==kifGraph) {
		KifGraph* kg=(KifGraph*)kfirst;
		if (kg->graph==NULL)
			return kifNULL;
		return new KifString(kifcodeptr,NULL,kg->graph->nom);
	}
	
	if (kfirst->type==kifXipNode) {

		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[73]);
		
		if (pattern->type==kifVector) {
			ListeVoc* voc=knode->value->source->voc;
			KifVector* vect=new KifVector(kifcodeptr,NULL);
			if (voc!=NULL) {				
				for (int i=0;i<voc->X.dernier;i++) {
					valeur=voc->X[i]->nom;
					vect->Push(new KifString(kifcodeptr,NULL,valeur));
				}
			}
			else {
				valeur=knode->value->X->nom;
				vect->Push(new KifString(kifcodeptr,NULL,valeur));
			}
			return vect;			
		}
		valeur=knode->value->X->nom;
		return new KifString(kifcodeptr,NULL,valeur);
	}

	if (kfirst->type==kifXipDependency || kfirst->type==kifXipGeneration) {

		KifXipDependency* knode=(KifXipDependency*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[74]);
		valeur=knode->value->Fx->nom;
		return new KifString(kifcodeptr,NULL,valeur);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[75]);
}


static KifElement* PreDependencyStack(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[76]);
	
	KifElement* kvect=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
	if (kvect->type!=kifVector)
		return kifcodeptr->Returnerror(kifErrorStrings[77]);

	KifElement* kfirst=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kfirst->type==kifXipDependency || kfirst->type==kifXipGeneration) {

		KifXipDependency* kdep=(KifXipDependency*)kfirst;
		if (kdep->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[78]);
		kvect->Clear();
		for (int i=0;i<kdep->value->stack.dernier;i++)
			kvect->Push(new KifString(kifcodeptr,NULL,kdep->value->stack[i]));
		return kifTRUE;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[79]);
}

static KifElement* PreDependencyParameters(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[80]);
	
	KifElement* kvect=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
	if (kvect->type!=kifVector)
		return kifcodeptr->Returnerror(kifErrorStrings[81]);;

	KifElement* kfirst=callfunc->parameters[0]->Execute(pattern,dom,kifNULL,false);
	if (kfirst->type==kifXipDependency  || kfirst->type==kifXipGeneration) {

		KifXipDependency* kdep=(KifXipDependency*)kfirst;
		if (kdep->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[82]);
		kvect->Clear();
		for (int i=0;i<kdep->value->arg.dernier;i++) {
			KifElement* knode=new KifXipNode(kifcodeptr,NULL,kdep->value->arg[i]);
			kvect->Push(knode);
		}
		return kifTRUE;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[83]);
}


static KifElement* PreRemovefirst(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[84]);

	KifElement* kstr=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	KifElement* knb=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false);
	string s=kstr->String();
	int id=knb->Integer();
	//On retire en tete
	int iv=s.length();

	if (iv>0 && id<=iv) {
		s=s.c_str()+id;
		return new KifString(kifcodeptr,NULL,s);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[85]);
}

static KifElement* PreRemovelast(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[125]);

	KifElement* kstr=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	KifElement* knb=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false);
	string s=kstr->String();
	int id=knb->Integer();
	//On retire en tete
	int iv=s.length();

	if (iv>0 && id<=iv) {
		s=s.substr(0,iv-id);
		return new KifString(kifcodeptr,NULL,s);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[126]);
}

static KifElement* PreXipNumber(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifMINUSONE;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifMINUSONE;
		return new KifInteger(kifcodeptr,NULL,knode->value->num_noeud);
	}
	return kifMINUSONE;
}

static KifElement* PreNbInstances(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[86]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifMatrix) {
		long i=((KifMatrix*)kfirst)->values.size();
		return new KifInteger(kifcodeptr,NULL,i);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[87]);
}

static KifElement* PreSet(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()==0)
		return kifcodeptr->Returnerror(kifErrorStrings[88]);
	
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifGraph) {
		if (callfunc->parameters.size()!=2)
			return kifcodeptr->Returnerror(kifErrorStrings[88]);
		KifElement* kname=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false);
		KifGraph* kg=(KifGraph*)kfirst;
		if (kg->graph!=NULL)
			kg->graph->nom=kname->String();
		return kifTRUE;
	}

	if (kfirst->type==kifXipRule) {
		if (callfunc->parameters.size()!=4)
			return kifcodeptr->Returnerror(kifErrorStrings[88]);
		KifXipRule* krule=(KifXipRule*)kfirst;
		KifElement* kweight=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false);
		KifElement* kthreshold=callfunc->parameters[2]->Execute(kifNULL,dom,kifNULL,false);
		KifElement* kvalue=callfunc->parameters[3]->Execute(kifNULL,dom,kifNULL,false);
		vector<VectTestFonction*>::iterator it;
		for (it=krule->values.begin();it!=krule->values.end();it++) {
			VectTestFonction* vt=*it;
			vt->seuil=kthreshold->Float();
			vt->poids=kweight->Float();
			vt->valeur=kvalue->Float();
		}
		return kifTRUE;
	}

	if (kfirst->type==kifMatrix) {
		if (callfunc->parameters.size()!=5)
			return kifcodeptr->Returnerror(kifErrorStrings[88]);
		KifElement* kline=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
		KifElement* kclass=callfunc->parameters[2]->Execute(pattern,dom,kifNULL,false);
		KifElement* katt=callfunc->parameters[3]->Execute(pattern,dom,kifNULL,false);
		KifElement* kval=callfunc->parameters[4]->Execute(pattern,dom,kifNULL,false);
		KifMatrix* kmat=(KifMatrix*)kfirst;
		string mclass=kclass->String();
		if (mclass=="")
			return kifcodeptr->Returnerror(kifErrorStrings[90]);
		string matt=katt->String();
		if (matt=="")
			return kifcodeptr->Returnerror(kifErrorStrings[91]);
		long mline=kline->Integer();
		kmat->Addvalue(mclass,matt,kval,mline);
		return kifTRUE;
	}
	if (kfirst->type==kifIterator) {
		if (callfunc->parameters.size()!=2)
			return kifcodeptr->Returnerror(kifErrorStrings[88]);
		KifElement* value=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
		KifElement* var=kfirst->IteratorValue();
		var->Setvalue(value);
		return kifTRUE;
	}

	return kifcodeptr->Returnerror(kifErrorStrings[92]);
}

static KifElement* PreGet(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()==0)
		return kifcodeptr->Returnerror(kifErrorStrings[93]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);

	if (kfirst->type==kifString || kfirst->type==kifFloat || kfirst->type==kifInteger) { 
		char buffer[1024];
		gets(buffer);
		KifElement* ke=new KifString(kifcodeptr,NULL,buffer);
		kfirst->Setvalue(ke,NULL,dom);
		return ke;
	}

	if (kfirst->type==kifXipRule) {
		if (callfunc->parameters.size()!=4)
			return kifcodeptr->Returnerror(kifErrorStrings[93]);
		KifXipRule* krule=(KifXipRule*)kfirst;
		if (krule->values.size()==0)
			return kifFALSE;
		KifElement* kweight=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,true);
		KifElement* kthreshold=callfunc->parameters[2]->Execute(kifNULL,dom,kifNULL,true);
		KifElement* kvalue=callfunc->parameters[3]->Execute(kifNULL,dom,kifNULL,true);
		VectTestFonction* vt=krule->values[0];
		kthreshold->Setvalue(new KifFloat(kifcodeptr,NULL,vt->seuil));
		kweight->Setvalue(new KifFloat(kifcodeptr,NULL,vt->poids));
		kvalue->Setvalue(new KifFloat(kifcodeptr,NULL,vt->valeur));
		return kifTRUE;
	}

	if (kfirst->type==kifMatrix) {
		if (callfunc->parameters.size()!=4)
			return kifcodeptr->Returnerror(kifErrorStrings[93]);
		KifElement* kline=callfunc->parameters[1]->Execute(pattern,dom,kifNULL,false);
		KifElement* kclass=callfunc->parameters[2]->Execute(pattern,dom,kifNULL,false);
		KifElement* katt=callfunc->parameters[3]->Execute(pattern,dom,kifNULL,false);
		KifMatrix* kmat=(KifMatrix*)kfirst;
		string mclass=kclass->String();
		if (mclass=="")
			return kifcodeptr->Returnerror(kifErrorStrings[94]);
		string matt=katt->String();
		if (matt=="")
			return kifcodeptr->Returnerror(kifErrorStrings[95]);
		long mline=kline->Integer();	
		if (mline<kmat->values.size()) {
			if (kmat->values[mline].find(mclass)!=kmat->values[mline].end())
				if (kmat->values[mline][mclass].find(matt)!=kmat->values[mline][mclass].end())
					return kmat->values[mline][mclass][matt];
		}
		return kifNULL;
	}

	if (kfirst->type==kifFile) {
		if (callfunc->parameters.size()!=1)
			return kifcodeptr->Returnerror(kifErrorStrings[93]);
		KifFile* kfile=(KifFile*)kfirst;
		if (kfile->readfile!=NULL) {
			if (kfile->EndOfFile()==kifTRUE)
				return kifMINUSONE;
			int c=kfile->readfile->get();
			if (pattern->type==kifInteger || pattern->type==kifFloat)
				return new KifInteger(kifcodeptr,NULL,c);
			else {
				string s;
				s=(char)c;
				return new KifString(kifcodeptr,NULL,s);
			}
		}
	}

	return kifcodeptr->Returnerror(kifErrorStrings[96]);
}

static KifElement* PreUnGet(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1 && callfunc->parameters.size()!=2)
		return kifcodeptr->Returnerror(kifErrorStrings[173]);
	
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	int nb=1;
	if (callfunc->parameters.size()==2) {
		KifElement* knb=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false);
		nb=knb->Integer();
	}


	if (kfirst->type==kifFile) {
		KifFile* kfile=(KifFile*)kfirst;
		if (kfile->readfile!=NULL) {
			while (nb>0) {
				kfile->readfile->unget();
				nb--;
			}
			kfile->readfile->clear();
			return kifTRUE;
		}
		return kifFALSE;
	}

	return kifcodeptr->Returnerror(kifErrorStrings[174]);
}

static KifElement* PreOffset(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=3)
		return kifcodeptr->Returnerror(kifErrorStrings[97]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[98]);
		KifElement* kleft=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,true);
		KifElement* kright=callfunc->parameters[2]->Execute(kifNULL,dom,kifNULL,true);
		kleft->Setvalue(new KifInteger(kifcodeptr,NULL,knode->value->gauche));
		kright->Setvalue(new KifInteger(kifcodeptr,NULL,knode->value->droite));
		return kifTRUE;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[99]);
}

static KifElement* PreOffsetChar(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=3)
		return kifcodeptr->Returnerror(kifErrorStrings[100]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[101]);
		KifElement* kleft=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,true);
		KifElement* kright=callfunc->parameters[2]->Execute(kifNULL,dom,kifNULL,true);
		kleft->Setvalue(new KifInteger(kifcodeptr,NULL,knode->value->gauchecar));
		kright->Setvalue(new KifInteger(kifcodeptr,NULL,knode->value->droitecar));
		return kifTRUE;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[102]);
}

static KifElement* PreTokenOffset(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=3)
		return kifcodeptr->Returnerror(kifErrorStrings[103]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[104]);
		KifElement* kleft=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,true);
		KifElement* kright=callfunc->parameters[2]->Execute(kifNULL,dom,kifNULL,true);
		kleft->Setvalue(new KifInteger(kifcodeptr,NULL,knode->value->num_gauche));
		kright->Setvalue(new KifInteger(kifcodeptr,NULL,knode->value->num_droite));
		return kifTRUE;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[105]);
}

static KifElement* PreLeftOffset(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[106]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[107]);
		return new KifInteger(kifcodeptr,NULL,knode->value->gauche);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[108]);
}

static KifElement* PreRightOffset(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[109]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[110]);
		return new KifInteger(kifcodeptr,NULL,knode->value->droite);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[111]);
}

static KifElement* PreLeftOffsetChar(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[112]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[113]);
		return new KifInteger(kifcodeptr,NULL,knode->value->gauchecar);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[114]);
}

static KifElement* PreRightOffsetChar(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[115]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[116]);
		return new KifInteger(kifcodeptr,NULL,knode->value->droitecar);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[117]);
}

static KifElement* PreLeftTokenOffset(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[118]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[119]);
		return new KifInteger(kifcodeptr,NULL,knode->value->num_gauche);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[120]);
}

static KifElement* PreRightTokenOffset(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifcodeptr->Returnerror(kifErrorStrings[121]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[122]);
		return new KifInteger(kifcodeptr,NULL,knode->value->num_droite);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[123]);
}


static KifElement* PrePause(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifFALSE;
	static time_t tempsinitial,tempsfinal;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	int res=kfirst->Integer();
	time(&tempsinitial);
	time(&tempsfinal);         
	char car=8;
	while (difftime(tempsfinal,tempsinitial)<res) {
		cout<<"/"<<car<<"/"<<car<<"/"<<car;
		cout<<"-"<<car<<"-"<<car<<"-"<<car;
		cout<<"\\"<<car<<"\\"<<car<<"\\"<<car;
		time(&tempsfinal);
	}
	return kifTRUE;
}


static KifElement* PreLemma(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1  || kifcodeptr->parseur==NULL)
		return kifcodeptr->Returnerror(kifErrorStrings[124]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[124]);
		if (pattern->type==kifVector) {
			ListeVoc* voc=knode->value->source->voc;
			KifVector* vect=new KifVector(kifcodeptr,NULL);
			if (voc!=NULL) {
				string valeur;
				for (int i=0;i<voc->lemme.dernier;i++) {
					valeur=voc->lemme[i];
					vect->Push(new KifString(kifcodeptr,NULL,valeur));
				}
			}
			else
				vect->Push(KifLemma(kifcodeptr->parseur,knode->value));
			return vect;
		}
		return KifLemma(kifcodeptr->parseur,knode->value);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[124]);
}

static KifElement* PreSurface(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1  || kifcodeptr->parseur==NULL)
		return kifcodeptr->Returnerror(kifErrorStrings[127]);
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {
		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[127]);
		return KifSurface(kifcodeptr->parseur,knode->value);
	}
	return kifcodeptr->Returnerror(kifErrorStrings[127]);
}



static KifElement* PreXmlNode(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;

	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	
	if (kfirst->type==kifXipNode) {

		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL || knode->value->source==NULL || 
			knode->value->source->voc==NULL || knode->value->source->voc->xmlnoeud==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[130]);
		
		xmlNodePtr nd=knode->value->source->voc->xmlnoeud;
		KifXMLNode* kxml=new KifXMLNode(kifcodeptr,NULL,nd);
		return kxml;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[130]);
}


static KifElement* PreBegin(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifIterator) 
		return kfirst->Begin();
	return kifcodeptr->Returnerror(kifErrorStrings[132]);
}

static KifElement* PreEnd(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifIterator) 
		return kfirst->End();
	return kifcodeptr->Returnerror(kifErrorStrings[133]);
}

static KifElement* PreNend(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifIterator) {
		if (kfirst->End()->Boolean()==true)
			return kifFALSE;
		return kifTRUE;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[134]);
}

static KifElement* PreKey(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifIterator) {
		KifElement* ke=kfirst->Key();
		if (ke==NULL)
			return kifNULL;
		return ke;
	}
	return kifcodeptr->Returnerror(kifErrorStrings[135]);
}

static KifElement* PreValue(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifIterator)
		return kfirst->IteratorValue();

	return kifcodeptr->Returnerror(kifErrorStrings[136]);
}

static KifElement* PreValueType(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifIterator) {
		KifElement* ke=kfirst->IteratorValue();
		string resstr=ke->Type();
		return new KifString(kifcodeptr,NULL,resstr);
	}
	else {
		string resstr=kfirst->Type();
		return new KifString(kifcodeptr,NULL,resstr);
	}

	return kifcodeptr->Returnerror(kifErrorStrings[137]);
}

static KifElement* PreValueTypeIs(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=2)
		return kifNULL;
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	KifElement* typ=callfunc->parameters[1]->Execute(kifNULL,dom,kifNULL,false);
	string resstr;
	if (kfirst->type==kifIterator) {
		KifElement* ke=kfirst->IteratorValue();
		resstr=ke->Type();
	}
	else
		resstr=kfirst->Type();

	string typstr=typ->String();
	if (resstr==typstr)
		return kifTRUE;
	return kifFALSE;
}


static KifElement* PreNext(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;

	//First parameter should be a node
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {

		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[138]);

		Noeud* n=knode->value->frere;
		if (n==NULL)
			return kifNULL;

		knode=new KifXipNode(kifcodeptr,dom,n);
		return knode;
	}

	if (kfirst->type==kifXipGeneration) {
		KifXipGeneration* kgen=(KifXipGeneration*)kfirst;
		if (kgen->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[139]);
		ResultatFonction* rf=NULL;
		rf=suivantde(kgen->value,rf,1);
		kgen=new KifXipGeneration(kifcodeptr,dom,rf);
		return kgen;
	}

	if (kfirst->type==kifXMLNode || kfirst->type==kifXMLNodePtr) {

		xmlNodePtr n=kfirst->Node();
		if (n==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[140]);
		n=n->next;
		if (n==NULL)
			return kifNULL;
		return new KifXMLNode(kifcodeptr,dom,n);
	}
	if (kfirst->type==kifIterator)
		return kfirst->Next();
	return kifcodeptr->Returnerror(kifErrorStrings[141]);
}

static KifElement* PrePrevious(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;

	//First parameter should be a node
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {

		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[142]);

		Noeud* n=knode->value->precedent;
		if (n==NULL)
			return kifNULL;

		knode=new KifXipNode(kifcodeptr,dom,n);
		return knode;
	}

	if (kfirst->type==kifXipGeneration) {
		KifXipGeneration* kgen=(KifXipGeneration*)kfirst;
		if (kgen->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[143]);
		ResultatFonction* rf=NULL;
		rf=suivantde(kgen->value,rf,-1);
		kgen=new KifXipGeneration(kifcodeptr,dom,rf);
		return kgen;
	}

	if (kfirst->type==kifXMLNode || kfirst->type==kifXMLNodePtr) {

		xmlNodePtr n=kfirst->Node();
		if (n==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[144]);
		n=n->prev;
		if (n==NULL)
			return kifNULL;
		return new KifXMLNode(kifcodeptr,dom,n);
	}

	return kifcodeptr->Returnerror(kifErrorStrings[145]);
}

static KifElement* PreParent(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;

	//First parameter should be a node
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,kifNULL,false);
	if (kfirst->type==kifXipNode) {

		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[146]);

		Noeud* n=knode->value->pere;
		if (n==NULL)
			return kifNULL;

		knode=new KifXipNode(kifcodeptr,dom,n);
		return knode;
	}
	if (kfirst->type==kifXipGeneration) {
		KifXipGeneration* kgen=(KifXipGeneration*)kfirst;
		if (kgen->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[147]);
		ResultatFonction* rf=NULL;
		rf=perede(kgen->value,rf);
		kgen=new KifXipGeneration(kifcodeptr,dom,rf);
		return kgen;
	}

	if (kfirst->type==kifXMLNode || kfirst->type==kifXMLNodePtr) {

		xmlNodePtr n=kfirst->Node();
		if (n==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[148]);
		n=n->parent;
		if (n==NULL)
			return kifNULL;
		return new KifXMLNode(kifcodeptr,dom,n);
	}

	return kifcodeptr->Returnerror(kifErrorStrings[149]);
}

static KifElement* PreChild(KifCode* kifcodeptr,KifPredefined* myself,KifElement* pattern,KifDomain* dom,KifElement* val,KifCallFunction* callfunc) {
	if (callfunc->parameters.size()!=1)
		return kifNULL;

	//First parameter should be a node
	KifElement* kfirst=callfunc->parameters[0]->Execute(kifNULL,dom,val,false);
	if (kfirst->type==kifXipNode) {

		KifXipNode* knode=(KifXipNode*)kfirst;
		if (knode->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[150]);

		Noeud* n=knode->value->fils;
		if (n==NULL)
			return kifNULL;

		knode=new KifXipNode(kifcodeptr,dom,n);
		return knode;
	}

	if (kfirst->type==kifXipGeneration) {
		KifXipGeneration* kgen=(KifXipGeneration*)kfirst;
		if (kgen->value==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[151]);
		ResultatFonction* rf=NULL;
		rf=filsde(kgen->value,rf,NON);
		kgen=new KifXipGeneration(kifcodeptr,dom,rf);
		return kgen;
	}

	if (kfirst->type==kifXMLNode || kfirst->type==kifXMLNodePtr) {

		xmlNodePtr n=kfirst->Node();
		if (n==NULL)
			return kifcodeptr->Returnerror(kifErrorStrings[152]);
		n=n->children;
		if (n==NULL)
			return kifNULL;
		return new KifXMLNode(kifcodeptr,dom,n);
	}

	return kifcodeptr->Returnerror(kifErrorStrings[153]);
}

//===================================================================

void KifCode::KifCreatePredefined(string name,Predefined func) {
	kifBaseFunctions[name]=new KifPredefined(this,func,name);
}

KifElement* KifCode::KifBaseFunctionTest(string& name) {
	if (kifBaseFunctions.find(name)!=kifBaseFunctions.end())
		return kifBaseFunctions[name];
	return NULL;
}


bool KifCode::KifDeclaredFunctionTest(string& name)  {
	if (KifDeclaredFunction.find(name)!=KifDeclaredFunction.end())
		return true;
	return false;
}

void KifInitFunctions(KifCode* kifcodeptr,KifFrame* kstart) {
	StaticInitialisation();

	kstart->instructions.push_back(kifNULL);
	kstart->instructions.push_back(kifBREAK);
	kstart->instructions.push_back(kifBREAKPOINT);
	kstart->instructions.push_back(kifDEFAULT);
	kstart->instructions.push_back(kifCONTINUE);
	kstart->instructions.push_back(kifNEG);
	kstart->instructions.push_back(kifMINUSONE);
	kstart->instructions.push_back(kifTRUE);
	kstart->instructions.push_back(kifFALSE);
	kstart->instructions.push_back(kifTypeSTRING);
	kstart->instructions.push_back(kifTypeINTEGER);
	kstart->instructions.push_back(kifTypeFLOAT);
	kstart->instructions.push_back(kifTypeBOOLEAN);
	kstart->instructions.push_back(kifTypeVECTOR);
	
	string name="true";
	kstart->Set(name,kifTRUE,NULL);
	name="false";
	kstart->Set(name,kifTRUE,NULL);
	name="null";
	kstart->Set(name,kifNULL,NULL);
//===================================================================

//===================================================================
	kifcodeptr->KifCreatePredefined("string",&PreConvertString);
	kifcodeptr->KifCreatePredefined("float",&PreConvertFloat);
	kifcodeptr->KifCreatePredefined("int",&PreConvertInteger);
	kifcodeptr->KifCreatePredefined("boolean",&PreConvertBoolean);

	kifcodeptr->KifCreatePredefined("realpath",&PreRealPath);
	kifcodeptr->KifCreatePredefined("createdirectory",&PreCreateDirectory);
	kifcodeptr->KifCreatePredefined("listdirectory",&PreListDirectory);

	kifcodeptr->KifCreatePredefined("log",&PreMath);
	kifcodeptr->KifCreatePredefined("ln",&PreMath);
	kifcodeptr->KifCreatePredefined("exp",&PreMath);
	kifcodeptr->KifCreatePredefined("sqrt",&PreMath);
	kifcodeptr->KifCreatePredefined("tan",&PreMath);
	kifcodeptr->KifCreatePredefined("sin",&PreMath);
	kifcodeptr->KifCreatePredefined("cos",&PreMath);
	kifcodeptr->KifCreatePredefined("atan",&PreMath);

	kifcodeptr->KifCreatePredefined("pause",&PrePause);

	kifcodeptr->KifCreatePredefined("surface",&PreSurface);
	kifcodeptr->KifCreatePredefined("lemma",&PreLemma);

	kifcodeptr->KifCreatePredefined("list",&PreList);
	kifcodeptr->KifCreatePredefined("isa",&PreTypeIs);
	kifcodeptr->KifCreatePredefined("type",&PreType);
	kifcodeptr->KifCreatePredefined("environment",&PreEnvironment);
	kifcodeptr->KifCreatePredefined("catch",&PreCatch);
	kifcodeptr->KifCreatePredefined("raise",&PreRaise);
	kifcodeptr->KifCreatePredefined("load",&PreLoad);
	kifcodeptr->KifCreatePredefined("parse",&PreParse);

#ifdef NTMCONNECTION
	kifcodeptr->KifCreatePredefined("compile",&PreCompilenet);
	kifcodeptr->KifCreatePredefined("compilewith",&PreCompilenetwith);
	kifcodeptr->KifCreatePredefined("up",&PreTokup);
	kifcodeptr->KifCreatePredefined("down",&PreTokdown);
#endif
	kifcodeptr->KifCreatePredefined("reverse",&PreReverse);

	kifcodeptr->KifCreatePredefined("return",&PreReturn);
	kifcodeptr->KifCreatePredefined("break",&PreBreak);
	kifcodeptr->KifCreatePredefined("breakpoint",&PreBreakpoint);
	kifcodeptr->KifCreatePredefined("continue",&PreContinue);
	kifcodeptr->KifCreatePredefined("print",&PrePrint);
	kifcodeptr->KifCreatePredefined("printerr",&PrePrinterr);
	kifcodeptr->KifCreatePredefined("openread",&PreOpenReadfile);
	kifcodeptr->KifCreatePredefined("openwrite",&PreOpenWritefile);
	kifcodeptr->KifCreatePredefined("eof",&PreEndOfFile);
	kifcodeptr->KifCreatePredefined("read",&PreRead);
	kifcodeptr->KifCreatePredefined("readln",&PreReadOneLine);
	kifcodeptr->KifCreatePredefined("seek",&PreSeek);
	kifcodeptr->KifCreatePredefined("tell",&PreTell);
	kifcodeptr->KifCreatePredefined("write",&PreWrite);
	kifcodeptr->KifCreatePredefined("close",&PreClose);
	kifcodeptr->KifCreatePredefined("insert",&PreInsert);
	kifcodeptr->KifCreatePredefined("push",&PrePush);
	kifcodeptr->KifCreatePredefined("join",&PreJoin);
	kifcodeptr->KifCreatePredefined("pop",&PrePop);
	kifcodeptr->KifCreatePredefined("extract",&PreExtract);
	kifcodeptr->KifCreatePredefined("last",&PreLast);
	kifcodeptr->KifCreatePredefined("sort",&PreSort);
	kifcodeptr->KifCreatePredefined("sum",&PreSum);
	kifcodeptr->KifCreatePredefined("product",&PreProduct);
	kifcodeptr->KifCreatePredefined("clear",&PreClear);

	kifcodeptr->KifCreatePredefined("chr",&PreChr);
	kifcodeptr->KifCreatePredefined("ord",&PreOrd);

	kifcodeptr->KifCreatePredefined("instances",&PreNbInstances);

	//Rules	
	kifcodeptr->KifCreatePredefined("set",&PreSet);
	kifcodeptr->KifCreatePredefined("get",&PreGet);
	kifcodeptr->KifCreatePredefined("apply",&PreApply);

	kifcodeptr->KifCreatePredefined("unget",&PreUnGet);

	kifcodeptr->KifCreatePredefined("createdependency",&PreCreateDependency);
	kifcodeptr->KifCreatePredefined("setfeature",&PreSetFeature);
	kifcodeptr->KifCreatePredefined("feature",&PreFeature);
	kifcodeptr->KifCreatePredefined("removefeature",&PreRemoveFeature);
	
	//String functions
	
	kifcodeptr->KifCreatePredefined("regex",&PreRegex);
	kifcodeptr->KifCreatePredefined("replace",&PreReplace);
	kifcodeptr->KifCreatePredefined("split",&PreSplit);
	kifcodeptr->KifCreatePredefined("removefirst",&PreRemovefirst);
	kifcodeptr->KifCreatePredefined("removelast",&PreRemovelast);
	kifcodeptr->KifCreatePredefined("utf8",&PreLatinToUTF8);
	kifcodeptr->KifCreatePredefined("latin",&PreUTF8ToLatin);
	kifcodeptr->KifCreatePredefined("left",&PreLeft);
	kifcodeptr->KifCreatePredefined("right",&PreRight);
	kifcodeptr->KifCreatePredefined("mid",&PreMid);
	kifcodeptr->KifCreatePredefined("islower",&PreIsLower);
	kifcodeptr->KifCreatePredefined("isupper",&PreIsUpper);
	kifcodeptr->KifCreatePredefined("isalpha",&PreIsAlpha);
	kifcodeptr->KifCreatePredefined("lower",&PreLower);
	kifcodeptr->KifCreatePredefined("upper",&PreUpper);
	kifcodeptr->KifCreatePredefined("trim",&PreTrim);

	//Time
	kifcodeptr->KifCreatePredefined("setdate",&PreSetDate);
	kifcodeptr->KifCreatePredefined("date",&PreDate);
	kifcodeptr->KifCreatePredefined("month",&PreMonth);
	kifcodeptr->KifCreatePredefined("year",&PreYear);
	kifcodeptr->KifCreatePredefined("day",&PreDay);
	kifcodeptr->KifCreatePredefined("hour",&PreHour);

	//Iterator functions
	kifcodeptr->KifCreatePredefined("begin",&PreBegin);
	kifcodeptr->KifCreatePredefined("end",&PreEnd);
	kifcodeptr->KifCreatePredefined("nend",&PreNend);
	kifcodeptr->KifCreatePredefined("key",&PreKey);
	kifcodeptr->KifCreatePredefined("value",&PreValue);
	kifcodeptr->KifCreatePredefined("valuetype",&PreValueType);
	kifcodeptr->KifCreatePredefined("valueisa",&PreValueTypeIs);


	//Functions for: iterator,node,xml
	kifcodeptr->KifCreatePredefined("next",&PreNext);
	kifcodeptr->KifCreatePredefined("previous",&PrePrevious);
	
	//Functions for: node,xml
	kifcodeptr->KifCreatePredefined("parent",&PreParent);
	kifcodeptr->KifCreatePredefined("child",&PreChild);
	
	//Function for: node
	kifcodeptr->KifCreatePredefined("xmlnode",&PreXmlNode);
	kifcodeptr->KifCreatePredefined("number",&PreXipNumber);
	kifcodeptr->KifCreatePredefined("offset",&PreOffset);
	kifcodeptr->KifCreatePredefined("offsetchar",&PreOffsetChar);
	kifcodeptr->KifCreatePredefined("tokenoffset",&PreTokenOffset);
	kifcodeptr->KifCreatePredefined("leftoffset",&PreLeftOffset);
	kifcodeptr->KifCreatePredefined("leftoffsetchar",&PreLeftOffsetChar);
	kifcodeptr->KifCreatePredefined("lefttokenoffset",&PreLeftTokenOffset);
	kifcodeptr->KifCreatePredefined("rightoffset",&PreRightOffset);
	kifcodeptr->KifCreatePredefined("rightoffsetchar",&PreRightOffsetChar);
	kifcodeptr->KifCreatePredefined("righttokenoffset",&PreRightTokenOffset);

	//Function for: node,xml,dependency
	kifcodeptr->KifCreatePredefined("data",&PreGetData);
	kifcodeptr->KifCreatePredefined("name",&PreGetName);
	kifcodeptr->KifCreatePredefined("pos",&PreGetName);
	kifcodeptr->KifCreatePredefined("parameters",&PreDependencyParameters);
	//Function for: string,map,vector
	kifcodeptr->KifCreatePredefined("size",&PreSize);

	//Function for: dependencies
	kifcodeptr->KifCreatePredefined("stack",&PreDependencyStack);


}


string KifElement::Type() {
	return kifStringType[type];
}

string KifVariableDeclaration::Type() {
	if (value==this)
		return kifStringType[type];
	return value->Type();
}


string KifMetaCharacters(string& m) {
	if (m.size()==1)
		return "";

	if (kifMetaCharacters.find(m)!=kifMetaCharacters.end())
		return kifMetaCharacters[m];
	string s;
	s=m[1];
	return s;
}

//===================================================================
KifElement* KifGraph::Copy(KifDomain* kp,KifElement* dom) {
	KifElement* ke=new KifGraph(kifCodeVector[idkifcode],kp,graph);
	return ke;
}


string KifGraph::String() {
	if (graph==NULL)
		return "";
	return graph->nom;
}

void KifGraph::Clean() {
	reference--;
	if (local==true && graph!=NULL)
		delete graph;
	graph=NULL;
}

void KifGraph::Clear() {
	if (local==true && graph!=NULL)
		delete graph;
	graph=NULL;
}

KifElement* KifGraph::same(KifElement* a) {
	if (graph==NULL || a->type!=kifGraph)
		return kifFALSE;
	KifGraph* g=(KifGraph*)a;
	if (g->graph==NULL)
		return kifFALSE;

	if (graph->compare(g->graph)==1)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifGraph::different(KifElement* a) {
	if (same(a)==kifTRUE)
		return kifFALSE;
	return kifTRUE;
}


bool KifGraph::Setvalue(KifElement* a,KifElement* index,KifElement* dom) {
	if (a->type!=kifGraph)
		return true;
	KifGraph* g=(KifGraph*)a;
	graph=g->graph;
	return true;
}

KifElement* KifGraph::Project(KifElement* a) {
	if (graph==NULL || a->type!=kifGraph)
		return kifFALSE;
	KifGraph* g=(KifGraph*)a;
	if (g->graph==NULL)
		return kifFALSE;
	if (graph->projection(g->graph)==OUI)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifGraph::Extract(KifElement* a) {
	if (graph==NULL || a->type!=kifGraph)
		return kifFALSE;
	KifGraph* g=(KifGraph*)a;
	if (g->graph==NULL)
		return kifFALSE;
	VECTA<graphe*> gvect;
	graph->extraire(g->graph,gvect);
	KifVector* kvect=new KifVector(kifCodeVector[idkifcode],NULL);
	for (int i=0;i<gvect.dernier;i++) {
		g=new KifGraph(kifCodeVector[idkifcode],NULL,gvect[i],true);
		kvect->Push(g);
	}
	return kvect;
}

KifElement* KifGraph::Remove(KifElement* a) {
	if (graph==NULL || a->type!=kifGraph)
		return kifFALSE;
	KifGraph* g=(KifGraph*)a;
	if (g->graph==NULL)
		return kifFALSE;
	if (graph->detruitgraphe(g->graph)==OUI)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifGraph::Replace(KifElement* pat,KifElement* replacement) {
	if (graph==NULL || pat->type!=kifGraph || replacement->type!=kifGraph) 
		return kifFALSE;
	KifGraph* gpat=(KifGraph*)pat;
	if (gpat->graph==NULL)
		return kifFALSE;
	KifGraph* grep=(KifGraph*)replacement;
	if (grep->graph==NULL)
		return kifFALSE;

	if (graph->remplacement(gpat->graph,grep->graph)==OUI)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifGraph::in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
	if (a->type!=kifGraph || b->type!=kifGraph)
		return kifFALSE;
	KifGraph* ga=(KifGraph*)a;
	KifGraph* gb=(KifGraph*)b;
	if (ga->graph==NULL || gb->graph==NULL)
		return kifFALSE;
	if (gb->graph->projection(ga->graph)==OUI)
		return kifTRUE;
	return kifFALSE;
}

//===================================================================
string KifXMLNode::String() {
	if (value!=NULL)
		return (char*)value->name;
	return "";
}

string KifXMLNodePtr::String() {
	if (value!=NULL && value->noeud!=NULL)
		return (char*)value->noeud->name;
	return "";
}

string KifXipNode::String() {
	if (value!=NULL)
		return value->X->nom;
	return "";
}

long KifXipNode::Integer() {
	if (value!=NULL)
		return value->num_noeud;
	return -1;
}

double KifXipNode::Float() {
	if (value!=NULL)
		return value->num_noeud;
	return -1;
}

string KifXipDependency::String() {
	if (value!=NULL)
		return value->Fx->nom;
	return "";
}
long KifXipDependency::Integer() {
	if (value!=NULL)
		return value->index;
	return -1;
}

double KifXipDependency::Float() {
	if (value!=NULL)
		return value->index;
	return -1;
}

long KifString::Size() {	
	string s=Value();
	return c_size(s);
}

long KifString::Integer() {
	if (Value().size()==0)
		return 0;
	long i=atoi(STR(Value()));
	return i;
}
double KifString::Float() {
	if (Value().size()==0)
		return 0;
	double i=atof(STR(Value()));
	return i;
}

long KifFile::Size() {
	struct stat scible;
	int stcible=-1;
	long size=-1;
	FILE* cible=fopen(STR(filename),"rb");
	if (cible!=NULL) {

#if  defined(WIN32) | defined(APPLE) | defined(XIPFSTAT64)
		stcible=fstat(cible->_file,&scible);
#else
		stcible=fstat(cible->_fileno,&scible);
#endif
		if (stcible>=0)
			size=scible.st_size;
		fclose(cible);
	}
	return size;
}

long KifFile::Integer() {
	return Size();
}

double KifFile::Float() {
	return Size();
}


string KifMap::String() {
	string res;
	map<string,KifElement*>::iterator it;
	res="{";
	bool beg=true;
	for (it=values.begin();it!=values.end();it++) {
		if (beg==false)
			res+=",";
		beg=false;
		res+="'";
		res+=it->first;
		res+="'";
		res+=":";
		if (it->second->type==kifInteger || it->second->type==kifFloat || it->second->type==kifBoolean)
			res+=it->second->String();
		else {
			string sx=it->second->String();
			if (sx.find("'")!=string::npos) {
				res+="\"";
				res+=sx;
				res+="\"";
			}
			else {
				res+="'";
				res+=sx;
				res+="'";
			}
		}
		if (res.size()>512) {
			res+="...";
			break;
		}
	}
	res+="}";
	return res;
}

string KifMapPtr::String() {
	string res;
	if (values==NULL)
		return "";
	map<string,string>::iterator it;
	res="[";
	bool beg=true;
	for (it=values->begin();it!=values->end();it++) {
		if (beg==false)
			res+=",";
		beg=false;
		res+=it->first;
		res+=":";
		res+=it->second;
		if (res.size()>512) {
			res+="...";
			break;
		}
	}
	res+="]";
	return res;
}

bool KifVectorNumberPtr::Boolean() {
	if (values==NULL || values->size()==0)
		return false;
	return true;
}

bool KifVectorStringPtr::Boolean() {
	if (values==NULL || values->size()==0)
		return false;
	return true;
}


string KifVector::String() {
	string res;
	vector<KifElement*>::iterator it;
	res="[";
	bool beg=true;
	for (it=values.begin();it!=values.end();it++) {
		if (beg==false)
			res+=",";
		beg=false;
		if ((*it)->type==kifInteger || (*it)->type==kifFloat || (*it)->type==kifBoolean)
			res+=(*it)->String();
		else {
			string sx=(*it)->String();
			if (sx.find("'")!=string::npos) {
				res+="\"";
				res+=sx;
				res+="\"";
			}
			else {
				res+="'";
				res+=sx;
				res+="'";
			}
		}
		if (res.size()>512) {
			res+="...";
			break;
		}
	}
	res+="]";
	return res;
}

string KifVectorNumberPtr::String() {
	string res;
	if (values==NULL)
		return res;
	int i;
	res="[";
	char ch[20];
	bool beg=true;
	for (i=0;i<values->size();i++) {
		if (beg==false)
			res+=",";
		beg=false;
		double v=(*values)[i];
		sprintf_s(ch,20,"%g",v);
		res+=ch;
		if (res.size()>512) {
			res+="...";
			break;
		}
	}
	res+="]";
	return res;
}

string KifVectorStringPtr::String() {
	string res;
	if (values==NULL)
		return res;
	int i;
	res="[";
	bool beg=true;
	for (i=0;i<values->size();i++) {
		string* s=(*values)[i];
		if (beg==false)
			res+=",";
		beg=false;
		if (s==NULL || *s=="")
			continue;
		res+=*s;
		if (res.size()>512) {
			res+="...";
			break;
		}
	}
	res+="]";
	return res;
}

//===================================================================
KifElement* KifFile::Read() {
	if (EndOfFile()->Boolean()==true)
		return kifNULL;

	string s;
	getline(*readfile,s);
	KifElement* ke=new KifString(kifCodeVector[idkifcode],this,s);
	return ke;
}

KifElement* KifFile::Readoneline() {
	if (EndOfFile()->Boolean()==true)
		return kifNULL;
	string s;
	getline(*readfile,s,'\n');
	KifElement* ke=new KifString(kifCodeVector[idkifcode],this,s);
	return ke;
}

//===================================================================
//A variable is looked for, first in the function declarations, then in the domain, then among the global
//variables
KifElement* KifCode::Getbasic(string& n,KifElement* dom) {
	KifElement* v;
	if (dom!=NULL) {
		v=dom->Declaration(n);
		if (v!=NULL)
			return v;
	}
	vector<KifDomain*>::reverse_iterator it;
	
	for (it=stacking.rbegin();it!=stacking.rend();it++) {
		v=(*it)->Declaration(n);
		if (v!=NULL)
			return v;
	}
	return NULL;
}

KifElement* KifCode::Get(string& n,KifDomain* subdom) {
	KifDomain* func=Top();
	KifDomain* dom=Beforetop();
	
	KifElement* v=NULL;

	if (subdom!=dom && subdom!=NULL)
		v=subdom->Declaration(n);

	if (v==NULL && func!=NULL)
		v=func->Declaration(n);
	
	if (v==NULL && dom!=NULL)
		v=dom->Declaration(n);
	
	if (v==NULL && dom!=kstart)
		return kstart->Declaration(n);
	return v;
}

KifElement* KifCode::Getframe(string& n) {
	KifDomain* func=Top();
	KifDomain* dom=Beforetop();
	
	KifElement* v=NULL;

	if (func!=NULL && func->Declaration(n))
		return func;
	
	if (dom!=NULL && dom->Declaration(n))
		return dom;
	
	if (dom!=kstart && kstart->Declaration(n))
		return kstart;
	return kifNULL;
}
//===================================================================
#ifdef NTMCONNECTION
string insert_while_plus(char* s) {
	string res;
	int lg=strlen(s);
	bool beg=true;
	for (int i=0;i<lg;i++) {
		if (s[i]=='+') {
			if (beg==false)
				res+=" ";
			beg=false;
		}		
		res+=s[i];
	}
	return res;
}


KifElement* KifFST::Compilenet(KifMap* kmap,KifElement* filename,bool with) {
	if (tok!=NULL || kifCodeVector[idkifcode]->parseur==NULL)
		return kifFALSE;
	string file=filename->String();
	string fileinter=file+".txt";
	//key is a lemma+\t+features	
	ofstream inter;
	if (with==true)
		inter.open(STR(fileinter),ios::app);
	else
		inter.open(STR(fileinter));
	map<string,KifElement*>::iterator itmap;
	for (itmap=kmap->values.begin();itmap!=kmap->values.end();itmap++) {
		//first we create a file to store our values
		string surface=itmap->second->String();
		surface=c_insert_sep(surface," ");
		string lemma=itmap->first;
		char* ps=strchr(STR(lemma),'\t');
		if (ps==NULL) {
			cerr<<"Wrong input:"<<STR(lemma)<<endl;
			return kifFALSE;
		}
		else {
			*ps=0;
			string lem=STR(lemma);
			lem=c_insert_sep(lem," ");
			string feat=insert_while_plus(ps+1);
			*ps=8;
			lem+="\t";
			lem+=feat;
			inter<<STR(lem)<<"\n"<<STR(surface)<<"\n";
		}
	}
	inter.close();
	//The we compile the file as a network
	FILE* fichier=fopen(STR(fileinter),"r");
	initialisation_fst();
	NETptr net=network_from_text_file(fichier);
	fclose(fichier);
	write_net(net,STR(file));
	GlobalParseur* p=kifCodeVector[idkifcode]->parseur;
	tok=new TokLOOK(p,net,"",0,kifCodeVector[idkifcode]->utf8);
	return kifTRUE;
}

void KifFST::Clean() {
	reference--;
	if (tok!=NULL)
		delete tok;
	tok=NULL;
}
void KifFST::Clear() {
	if (tok!=NULL)
		delete tok;
	tok=NULL;
}

string KifFST::String() {
	if (tok==NULL)
		return "";
	return tok->script_file;
}

KifElement* KifFST::Tokup(KifElement* str,KifVector* kvect) {
	if (tok==NULL || kifCodeVector[idkifcode]->parseur==NULL)
		return kifFALSE;
	string wrd=str->String();
	tok->up(STR(wrd));
	if (tok->parseur->lectures.dernier) {
		int sz=wrd.size()+1;
		char* res=tok->parseur->lectures[0]->mot+sz;
		if (res[0]!='?') {
			for (int i=0;i<tok->parseur->lectures.dernier;i++)
				kvect->Push(new KifString(kifCodeVector[idkifcode],NULL,tok->parseur->lectures[i]->mot+sz));
			tok->parseur->nettoieLectures();
			return kifTRUE;
		}
	}
	tok->parseur->nettoieLectures();
	return kifFALSE;
}

KifElement* KifFST::Tokdown(KifElement* str,KifElement* feats,KifElement* pattern) {
	if (tok==NULL  || kifCodeVector[idkifcode]->parseur==NULL)
		return kifFALSE;
	string wrd=str->String();
	string w=wrd;
	w+=feats->String();
	tok->down(STR(w));
	if (tok->parseur->lectures.dernier) {		
		KifElement* reponse;
		char* res=tok->parseur->lectures[0]->mot+wrd.size()+1;
		if (res[0]!='?') {
			if (pattern->type!=kifVector) {				
				reponse=new KifString(kifCodeVector[idkifcode],NULL,res);
				tok->parseur->nettoieLectures();
				return reponse;
			}
			else {
				KifVector* kvect=new KifVector(kifCodeVector[idkifcode],NULL);
				for (int i=0;i<tok->parseur->lectures.dernier;i++) {
					res=tok->parseur->lectures[i]->mot+wrd.size()+1;
					reponse=new KifString(kifCodeVector[idkifcode],NULL,res);
					kvect->Push(reponse);
				}
				tok->parseur->nettoieLectures();
				return kvect;
			}
		}
	}
	tok->parseur->nettoieLectures();
	return kifNULL;
}

KifElement* KifFST::Load(KifElement* filename,KifElement* flags,KifElement* ty,KifElement* utf8) {
	if (tok!=NULL  || kifCodeVector[idkifcode]->parseur==NULL)
		return kifFALSE;
	string sflags=flags->String();
	char utf=0;
	if (utf8->Boolean()==true)
		utf=1;
	string sfilename=filename->String();
	int itype=ty->Integer();
	tok=new TokLOOK(kifCodeVector[idkifcode]->parseur,STR(sfilename),STR(sflags),itype,utf);
	return kifTRUE;
}
#endif


KifElement* KifVector::orset(KifElement* a,KifElement* b) {
	if (a->type!=kifVector)
		return kifNULL;
	
	KifVector* ref=this;
	KifVector* A=(KifVector*)a;
	
	vector<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;
	bool found;
	if (b->type==kifVector) {
		if (a!=this)
			ref=(KifVector*)a->Copy(NULL);

		KifVector* bref=(KifVector*)b;		
		vector<KifElement*>::iterator itref;
		for (itref=bref->values.begin();itref!=bref->values.end();itref++) {
			kref=*itref;
			found=false;
			for (it=A->values.begin();it!=A->values.end();it++) {
				ke=*it;
				if (kref->same(ke)==kifTRUE) {
					found=true;
					break;
				}
			}	
			if (found==false)
				ref->Push(kref);
		}
		return ref;
	}

	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		map<string,KifElement*>::iterator itmap=bref->values.begin();
		if (a!=this)
			ref=(KifVector*)a->Copy(NULL);
		for (;itmap!=bref->values.end();itmap++) {
			kref=itmap->second;
			for (it=A->values.begin();it!=A->values.end();it++) {
				ke=*it;
				if (kref->same(ke)==kifTRUE) {
					found=true;
					break;
				}
			}
			if (found==false)
				ref->Push(kref);
		}
		return ref;
	}
	return kifNULL;
}

KifElement* KifVector::andset(KifElement* a,KifElement* b) {
	if (a->type!=kifVector)
		return kifNULL;
	
	KifVector* A=(KifVector*)a;
	KifVector* ref;
	vector<KifElement*>::iterator it;
	KifElement* ke;
	KifElement* kref;
	bool found;
	if (b->type==kifVector) {
		ref=new KifVector(kifCodeVector[idkifcode],NULL);
		KifVector* bref=(KifVector*)b;		
		vector<KifElement*>::iterator itref;
		for (itref=bref->values.begin();itref!=bref->values.end();itref++) {
			kref=*itref;
			found=false;
			for (it=A->values.begin();it!=A->values.end();it++) {
				ke=*it;
				if (kref->same(ke)==kifTRUE) {
					found=true;
					break;
				}
			}	
			if (found==true)
				ref->Push(kref);
		}
		return ref;
	}

	if (b->type==kifMap) {
		ref=new KifVector(kifCodeVector[idkifcode],NULL);
		KifMap* bref=(KifMap*)b;		
		map<string,KifElement*>::iterator itmap=bref->values.begin();
		for (;itmap!=bref->values.end();itmap++) {
			kref=itmap->second;
			found=false;
			for (it=A->values.begin();it!=A->values.end();it++) {
				ke=*it;
				if (kref->same(ke)==kifTRUE) {
					found=true;
					break;
				}
			}
			if (found==true)
				ref->Push(kref);
		}
		return ref;
	}
	return kifNULL;
}


KifElement* KifVector::plus(KifElement* a,KifElement* b) {
	if (a->type!=kifVector)
		return kifNULL;
	
	KifVector* ref=this;
	if (a!=this)
		ref=(KifVector*)a->Copy(NULL);

	vector<KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->plus(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		map<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (itmap==bref->values.end())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->plus(ke,itmap->second);
			itmap++;
		}
		return ref;
	}
	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=*it;
		ke->Resetreference();
		*it=ke->plus(ke,b);
	}
	return ref;
}

KifElement* KifVector::minus(KifElement* a,KifElement* b) {
	if (a->type!=kifVector)
		return kifNULL;
	
	KifVector* ref=this;
	if (a!=this)
		ref=(KifVector*)a->Copy(NULL);

	vector<KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->minus(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		map<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (itmap==bref->values.end())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->minus(ke,itmap->second);
			itmap++;
		}
		return ref;
	}

	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=*it;
		ke->Resetreference();
		*it=ke->minus(ke,b);
	}
	return ref;
}

KifElement* KifVector::multiply(KifElement* a,KifElement* b) {
	if (a->type!=kifVector)
		return kifNULL;
	
	KifVector* ref=this;
	if (a!=this)
		ref=(KifVector*)a->Copy(NULL);

	vector<KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->multiply(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		map<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (itmap==bref->values.end())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->multiply(ke,itmap->second);
			itmap++;
		}
		return ref;
	}

	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=*it;
		ke->Resetreference();
		*it=ke->multiply(ke,b);
	}
	return ref;
}
KifElement* KifVector::divide(KifElement* a,KifElement* b) {
	if (a->type!=kifVector)
		return kifNULL;
	
	KifVector* ref=this;
	if (a!=this)
		ref=(KifVector*)a->Copy(NULL);

	vector<KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->divide(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		map<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (itmap==bref->values.end())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->divide(ke,itmap->second);
			itmap++;
		}
		return ref;
	}

	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=*it;
		ke->Resetreference();
		*it=ke->divide(ke,b);
	}
	return ref;
}
KifElement* KifVector::mod(KifElement* a,KifElement* b) {
	if (a->type!=kifVector)
		return kifNULL;
	
	KifVector* ref=this;
	if (a!=this)
		ref=(KifVector*)a->Copy(NULL);

	vector<KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->mod(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		map<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (itmap==bref->values.end())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->mod(ke,itmap->second);
			itmap++;
		}
		return ref;
	}

	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=*it;
		ke->Resetreference();
		*it=ke->mod(ke,b);
	}
	return ref;
}
KifElement* KifVector::shiftright(KifElement* a,KifElement* b) {
	if (a->type!=kifVector)
		return kifNULL;
	
	KifVector* ref=this;
	if (a!=this)
		ref=(KifVector*)a->Copy(NULL);

	vector<KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->shiftright(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		map<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (itmap==bref->values.end())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->shiftright(ke,itmap->second);
			itmap++;
		}
		return ref;
	}

	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=*it;
		ke->Resetreference();
		*it=ke->shiftright(ke,b);
	}
	return ref;
}
KifElement* KifVector::shiftleft(KifElement* a,KifElement* b) {
	if (a->type!=kifVector)
		return kifNULL;
	
	KifVector* ref=this;
	if (a!=this)
		ref=(KifVector*)a->Copy(NULL);

	vector<KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->shiftleft(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		map<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (itmap==bref->values.end())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->shiftleft(ke,itmap->second);
			itmap++;
		}
		return ref;
	}

	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=*it;
		ke->Resetreference();
		*it=ke->shiftleft(ke,b);
	}
	return ref;
}
KifElement* KifVector::power(KifElement* a,KifElement* b) {
	if (a->type!=kifVector)
		return kifNULL;
	
	KifVector* ref=this;
	if (a!=this)
		ref=(KifVector*)a->Copy(NULL);

	vector<KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->power(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;		
		map<string,KifElement*>::iterator itmap=bref->values.begin();
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (itmap==bref->values.end())
				break;
			ke=*it;
			ke->Resetreference();
			*it=ke->power(ke,itmap->second);
			itmap++;
		}
		return ref;
	}
	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=*it;
		ke->Resetreference();
		*it=ke->power(ke,b);
	}
	return ref;
}
//===================================================================

KifElement* KifMap::orset(KifElement* a,KifElement* b) {
	if (a->type!=kifMap || b->type!=kifMap)
		return kifNULL;

	KifMap* A=(KifMap*)a;
	map<string,KifElement*>::iterator it;

	KifMap* ref=this;
	if (a!=this)
		ref=(KifMap*)a->Copy(NULL);
	KifMap* bref=(KifMap*)b;		
	for (it=bref->values.begin();it!=bref->values.end();it++) {
		if (A->values.find(it->first)==A->values.end()) {
			ref->values[it->first]=it->second;
			it->second->Setreference();
		}
	}
	return ref;
}

KifElement* KifMap::andset(KifElement* a,KifElement* b) {
	if (a->type!=kifMap || b->type!=kifMap)
		return kifNULL;

	KifMap* A=(KifMap*)a;
	map<string,KifElement*>::iterator it;
	map<string,KifElement*>::iterator itres;

	KifMap* ref=new KifMap(kifCodeVector[idkifcode],NULL);
	KifMap* bref=(KifMap*)b;		
	for (it=bref->values.begin();it!=bref->values.end();it++) {
		itres=A->values.find(it->first);
		if (itres!=A->values.end()) {
			ref->values[it->first]=itres->second;
			itres->second->Setreference();
		}
	}
	return ref;
}

KifElement* KifMap::plus(KifElement* a,KifElement* b) {
	if (a->type!=kifMap)
		return kifNULL;
	
	KifMap* ref=this;
	if (a!=this)
		ref=(KifMap*)a->Copy(NULL);

	map<string,KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=it->second;
			ke->Resetreference();
			it->second=ke->plus(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;				
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (bref->values.find(it->first)!=bref->values.end()) {
				ke=it->second;
				ke->Resetreference();
				it->second=ke->plus(ke,bref->values[it->first]);
			}
		}
		return ref;
	}
	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=it->second;
		ke->Resetreference();
		it->second=ke->plus(ke,b);
	}
	return ref;
}

KifElement* KifMap::minus(KifElement* a,KifElement* b) {
	if (a->type!=kifMap)
		return kifNULL;
	
	KifMap* ref=this;
	if (a!=this)
		ref=(KifMap*)a->Copy(NULL);

	map<string,KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=it->second;
			ke->Resetreference();
			it->second=ke->minus(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;				
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (bref->values.find(it->first)!=bref->values.end()) {
				ke=it->second;
				ke->Resetreference();
				it->second=ke->minus(ke,bref->values[it->first]);
			}
		}
		return ref;
	}
	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=it->second;
		ke->Resetreference();
		it->second=ke->minus(ke,b);
	}
	return ref;
}


KifElement* KifMap::multiply(KifElement* a,KifElement* b) {
	if (a->type!=kifMap)
		return kifNULL;
	
	KifMap* ref=this;
	if (a!=this)
		ref=(KifMap*)a->Copy(NULL);

	map<string,KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=it->second;
			ke->Resetreference();
			it->second=ke->multiply(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;				
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (bref->values.find(it->first)!=bref->values.end()) {
				ke=it->second;
				ke->Resetreference();
				it->second=ke->multiply(ke,bref->values[it->first]);
			}
		}
		return ref;
	}
	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=it->second;
		ke->Resetreference();
		it->second=ke->multiply(ke,b);
	}
	return ref;
}

KifElement* KifMap::divide(KifElement* a,KifElement* b) {
	if (a->type!=kifMap)
		return kifNULL;
	
	KifMap* ref=this;
	if (a!=this)
		ref=(KifMap*)a->Copy(NULL);

	map<string,KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=it->second;
			ke->Resetreference();
			it->second=ke->divide(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;				
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (bref->values.find(it->first)!=bref->values.end()) {
				ke=it->second;
				ke->Resetreference();
				it->second=ke->divide(ke,bref->values[it->first]);
			}
		}
		return ref;
	}
	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=it->second;
		ke->Resetreference();
		it->second=ke->divide(ke,b);
	}
	return ref;
}

KifElement* KifMap::mod(KifElement* a,KifElement* b) {
	if (a->type!=kifMap)
		return kifNULL;
	
	KifMap* ref=this;
	if (a!=this)
		ref=(KifMap*)a->Copy(NULL);

	map<string,KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=it->second;
			ke->Resetreference();
			it->second=ke->mod(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;				
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (bref->values.find(it->first)!=bref->values.end()) {
				ke=it->second;
				ke->Resetreference();
				it->second=ke->mod(ke,bref->values[it->first]);
			}
		}
		return ref;
	}
	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=it->second;
		ke->Resetreference();
		it->second=ke->mod(ke,b);
	}
	return ref;
}

KifElement* KifMap::shiftright(KifElement* a,KifElement* b) {
	if (a->type!=kifMap)
		return kifNULL;
	
	KifMap* ref=this;
	if (a!=this)
		ref=(KifMap*)a->Copy(NULL);

	map<string,KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=it->second;
			ke->Resetreference();
			it->second=ke->shiftright(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;				
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (bref->values.find(it->first)!=bref->values.end()) {
				ke=it->second;
				ke->Resetreference();
				it->second=ke->shiftright(ke,bref->values[it->first]);
			}
		}
		return ref;
	}
	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=it->second;
		ke->Resetreference();
		it->second=ke->shiftright(ke,b);
	}
	return ref;
}

KifElement* KifMap::shiftleft(KifElement* a,KifElement* b) {
	if (a->type!=kifMap)
		return kifNULL;
	
	KifMap* ref=this;
	if (a!=this)
		ref=(KifMap*)a->Copy(NULL);

	map<string,KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=it->second;
			ke->Resetreference();
			it->second=ke->shiftleft(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;				
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (bref->values.find(it->first)!=bref->values.end()) {
				ke=it->second;
				ke->Resetreference();
				it->second=ke->shiftleft(ke,bref->values[it->first]);
			}
		}
		return ref;
	}
	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=it->second;
		ke->Resetreference();
		it->second=ke->shiftleft(ke,b);
	}
	return ref;
}

KifElement* KifMap::power(KifElement* a,KifElement* b) {
	if (a->type!=kifMap)
		return kifNULL;
	
	KifMap* ref=this;
	if (a!=this)
		ref=(KifMap*)a->Copy(NULL);

	map<string,KifElement*>::iterator it;
	KifElement* ke;
	if (b->type==kifVector) {
		KifVector* bref=(KifVector*)b;		
		int j=0;
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (j>=bref->values.size())
				break;
			ke=it->second;
			ke->Resetreference();
			it->second=ke->power(ke,bref->values[j++]);
		}
		return ref;
	}
	if (b->type==kifMap) {
		KifMap* bref=(KifMap*)b;				
		for (it=ref->values.begin();it!=ref->values.end();it++) {
			if (bref->values.find(it->first)!=bref->values.end()) {
				ke=it->second;
				ke->Resetreference();
				it->second=ke->power(ke,bref->values[it->first]);
			}
		}
		return ref;
	}
	for (it=ref->values.begin();it!=ref->values.end();it++) {
		ke=it->second;
		ke->Resetreference();
		it->second=ke->power(ke,b);
	}
	return ref;
}

//===================================================================
KifIteratorXipRule::KifIteratorXipRule(KifCode* klc,KifElement* kp) : KifIteration(klc,kp) {
	itx=0;
}

KifElement* KifIteratorXipRule::ApplyRule(VectTestFonction* v) {
	if (kifCodeVector[idkifcode]->parseur==NULL)
		return kifNULL;
	VECTA<VectTestFonction*> deductionkif;
	deductionkif.ajoute(v);
	kifCodeVector[idkifcode]->parseur->DeductionDeKif=&deductionkif;
	string name="apply";
	KifElement* kfunc=kifCodeVector[idkifcode]->kifBaseFunctions[name];
	KifCallFunction apply(NULL,NULL,name,(KifFunction*)kfunc);
	return krule->ApplyRules(kifNULL,&apply);
}


KifElement* KifIteratorXipRule::ExecuteRule(VectTestFonction* v,KifElement* pattern,KifDomain* dom) {
	if (kifCodeVector[idkifcode]->parseur==NULL)
		return kifNULL;
	VECTA<VectTestFonction*> deductionkif;
	deductionkif.ajoute(v);
	kifCodeVector[idkifcode]->parseur->DeductionDeKif=&deductionkif;
	return krule->ApplyRules(pattern,dom);
}

KifElement* KifIteratorXipRule::IteratorValue() {
	if (reverse==false) {
		if (it==values->end())
			return kifNULL;
		return ApplyRule(*it);
	}
	if (itr==values->rend())
		return kifNULL;
	return ApplyRule(*itr);
}

KifElement* KifIteratorXipRule::Next() {
	if (reverse==false) {
		it++;
		itx++;
		if (it==values->end())
			return kifFALSE;
		return kifTRUE;
	}

	itr++;
	itx--;
	if (itr==values->rend())
		return kifFALSE;
	return kifTRUE;
}

KifElement* KifIteratorXipRule::Begin() {
	if (reverse==false) {
		it=values->begin();
		if (it==values->end())
			return kifFALSE;
		itx=0;
		return kifTRUE;
	}
	itr=values->rbegin();
	if (itr==values->rend())
		return kifFALSE;
	itx=values->size()-1;
	return kifTRUE;
}
//===================================================================
KifElement* KifIteratorString::Key() {
	int i=itx;
	if (reverse==true)
		i=value.size()-itx-1;
	KifElement* ke=new KifInteger(kifCodeVector[idkifcode],NULL,i);		
	return ke;
}

KifElement* KifIteratorString::IteratorValue() {
	if (itx==value.size())
		return kifNULL;
	int i=itx;
	string s=c_char_get(USTR(value),i);
	KifString* ke=new KifString(kifCodeVector[idkifcode],NULL,s);		
	return ke;
}

KifElement* KifIteratorString::Next() {
	int nb=c_char_next(USTR(value),itx);
	itx++;
	if (itx==value.size())
		return kifNULL;
	int i=itx;
	string s=c_char_get(USTR(value),i);
	KifString* ke=new KifString(kifCodeVector[idkifcode],NULL,s);
	return ke;
}

KifElement* KifIteratorString::End() {
	if (itx==value.size())
		return kifTRUE;		
	return kifFALSE;
}

KifElement* KifIteratorString::Begin() {
	itx=0;
	if (itx==value.size())
		return kifNULL;

	if (reverse==true)
		value=c_char_reverse(value);
	
	int i=itx;
	string s=c_char_get(USTR(value),i);
	KifString* ke=new KifString(kifCodeVector[idkifcode],NULL,s);		
	return ke;
}

//===================================================================

KifElement* KifXipRule::Newiterator(bool rev) {
	KifIteratorXipRule* iter=new KifIteratorXipRule(kifCodeVector[idkifcode],NULL);
	iter->reverse=rev;
	iter->Setreference();
	iter->values=&values;
	iter->krule=this;
	return iter;
}


KifElement* KifString::Newiterator(bool rev) {
	KifIteratorString* iter=new KifIteratorString(kifCodeVector[idkifcode],NULL);
	iter->reverse=rev;
	iter->Setreference();
	iter->value=value;
	return iter;
}

KifElement* KifStringPtr::Newiterator(bool rev) {
	KifIteratorString* iter=new KifIteratorString(kifCodeVector[idkifcode],NULL);
	iter->reverse=rev;
	iter->Setreference();
	iter->value=*value;
	return iter;
}

KifElement* KifVectorStringPtr::Newiterator(bool rev) {
	KifIteratorVectorString* iter=new KifIteratorVectorString(kifCodeVector[idkifcode],NULL);
	iter->reverse=rev;
	iter->Setreference();
	iter->values=values;
	return iter;
}

KifElement* KifVectorNumberPtr::Newiterator(bool rev) {
	KifIteratorVectorNumber* iter=new KifIteratorVectorNumber(kifCodeVector[idkifcode],NULL);
	iter->reverse=rev;
	iter->Setreference();
	iter->values=values;
	return iter;
}

KifElement* KifVector::Newiterator(bool rev) {
	KifIteratorVector* iter=new KifIteratorVector(kifCodeVector[idkifcode],NULL);
	iter->reverse=rev;
	iter->Setreference();
	iter->values=&values;
	return iter;
}
KifElement* KifMap::Newiterator(bool rev) {
	KifIteratorMap* iter=new KifIteratorMap(kifCodeVector[idkifcode],NULL);
	iter->reverse=rev;
	iter->Setreference();
	iter->values=&values;
	return iter;
}
KifElement* KifMapPtr::Newiterator(bool rev) {
	KifIteratorMapPtr* iter=new KifIteratorMapPtr(kifCodeVector[idkifcode],NULL);
	iter->reverse=rev;
	iter->Setreference();
	iter->values=values;
	return iter;
}
//===================================================================
KifElement* KifString::Pop(KifElement* ke) {
	if (value.size()==0)
		return kifNULL;
	int i=ke->Integer();
	if (i==-1)
		i=value.size()-1;
	else
		if (i>=value.size())
			return kifNULL;

	string res=c_char_index_remove(value,i);
	return new KifString(kifCodeVector[idkifcode],NULL,res);
}

KifElement* KifStringPtr::Pop(KifElement* ke) {
	if (value==NULL || value->size()==0)
		return kifNULL;
	int i=ke->Integer();
	if (i==-1)
		i=value->size()-1;
	else
		if (i>=value->size())
			return kifNULL;

	string res=c_char_index_remove(*value,i);
	return new KifString(kifCodeVector[idkifcode],NULL,res);
}

KifElement* KifVector::Pop(KifElement* ke) {
	if (values.size()==0)
		return kifNULL;
	int i=ke->Integer();
	if (i==-1)
		i=values.size()-1;
	else
		if (i>=values.size())
			return kifNULL;

	KifElement* kres=values[i];
	values.erase(values.begin()+i);
	if (kres!=NULL) {
		kres->Resetreference();
		return kres;
	}
	return kifNULL;
}

KifElement* KifVectorNumberPtr::Pop(KifElement* ke) {
	if (values!=NULL) {
		int i=ke->Integer();
		if (i==-1)
			i=values->size()-1;
		if (i>=values->size())
			return kifNULL;

		double v=values->retireElement(i);
		return new KifFloat(kifCodeVector[idkifcode],NULL,v);
	}
	return kifNULL;
}

KifElement* KifVectorStringPtr::Pop(KifElement* ke) {
	if (values!=NULL) {
		int i=ke->Integer();
		if (i==-1)
			i=values->size()-1;
		if (i>=values->size())
			return kifNULL;
		string* s=values->retireElement(i);
		if (s!=NULL) {
			KifElement* kres=new KifString(kifCodeVector[idkifcode],NULL,*s);
			delete s;
			return kres;
		}
	}
	return kifNULL;
}

KifElement* KifMap::Pop(KifElement* ke) {
	if (values.size()==0)
		return kifNULL;

	string s=ke->String();
	map<string,KifElement*>::iterator itx=values.find(s);

	if (itx==values.end())
		return kifNULL;

	KifElement* kres=itx->second;
	values.erase(itx);

	if (kres!=NULL) {
		kres->Resetreference();
		return kres;
	}
	return kifNULL;
}

KifElement* KifMapPtr::Pop(KifElement* ke) {
	if (values==NULL || values->size()==0)
		return kifNULL;

	string s=ke->String();
	map<string,string>::iterator itx=values->find(s);

	if (itx==values->end())
		return kifNULL;

	string kres=itx->second;
	values->erase(itx);

	if (kres!="")
		return new KifString(kifCodeVector[idkifcode],NULL,kres);
	return kifNULL;
}

//===================================================================
KifElement* KifString::Last() {
	if (value.size()==0)
		return kifNULL;
	int i=value.size()-1;
	string res=c_char_index(value,i);
	return new KifString(kifCodeVector[idkifcode],NULL,res);
}

KifElement* KifStringPtr::Last() {
	if (value==NULL || value->size()==0)
		return kifNULL;
	int i=value->size()-1;
	string res=c_char_index(*value,i);
	return new KifString(kifCodeVector[idkifcode],NULL,res);
}

KifElement* KifVector::Last() {
	if (values.size()==0)
		return kifNULL;
	
	int i=values.size()-1;

	return values[i];
}

KifElement* KifVectorNumberPtr::Last() {
	if (values!=NULL && values->size()!=0) {
		int i=values->size()-1;
		double v=(*values)[i];
		return new KifFloat(kifCodeVector[idkifcode],NULL,v);
	}
	return kifNULL;
}

KifElement* KifVectorStringPtr::Last() {
	if (values!=NULL && values->size()!=0) {
		int i=values->size()-1;
		string* s=(*values)[i];
		if (s!=NULL)
			return new KifString(kifCodeVector[idkifcode],NULL,*s);
	}
	return kifNULL;
}

//===================================================================

void KifDomain::Resetdeclaration(map<string,KifElement*>& stacks) {
	map<string,KifElement*>::iterator it;
	for (it=stacks.begin();it!=stacks.end();it++) {
		string n=it->first;
		if (Declaration(n)!=NULL)
			declarations[n]->Resetreference();
		declarations[n]=it->second;
	}
}

void KifDomain::Setdeclaration(string& n,KifElement* ke,map<string,KifElement*>* stacks)  {
	if (Declaration(n)==ke)
		return;

	if (stacks!=NULL) 
		(*stacks)[n]=Declaration(n);
	
	if (ke->reference!=0)
		ke->Setreference();
	else
		ke->reference++;

	declarations[n]=ke;
}

void KifInstruction::Add(KifElement* k) {
	if (adding)
		instructions.push_back(k);
}

void KifCallFunction::Add(KifElement* ke) {
	if (add==1)
		parameters.push_back(ke);
	else
		if (add==2)
			function=(KifCallFunction*)ke;
}

void KifInstruction::Set(string& op,KifElement* kf,map<string,KifElement*>* stacks) {
	action=kifOperators[op];	
}

void KifDomain::Sharedeclaration(KifDomain* k) {
	map<string,KifElement*>::iterator it;
	for (it=declarations.begin();it!=declarations.end();it++) {
		string n=it->first;
		k->declarations[n]=it->second;
	}
}

void KifDomain::Set(string& s,KifElement* k,map<string,KifElement*>* stacks) {
	Setdeclaration(s,k,stacks);
}

void KifFrame::Set(string& s,KifElement* k,map<string,KifElement*>* stacks) {
	Setdeclaration(s,k,stacks);
}

void KifCallFunction::SetParameters(vector<KifElement*>& vect,map<string,KifElement*>* stacks) {
	vector<KifElement*>::iterator it;
	for (it=vect.begin();it!=vect.end();it++) {
		KifElement* ke=*it;
		string thename=ke->Name();
		if (ke->type==kifInstance)
			Setdeclaration(thename,((KifInstance*)ke)->value,stacks);
		else
			Setdeclaration(thename,ke,stacks);
		parameters.push_back(ke);
	}
}

//===================================================================
//It is an XML document
void KifMatrix::Save(string& filename,string& thetype) {
	//first we need to isolate the class instances
	vector<map<string,map<string,KifElement*> > >::iterator itins;
	map<string,map<string,KifElement*> >::iterator itclass;
	map<string, KifElement*>::iterator itatt;
	map<string,bool>::iterator itatts;
	KifElement* k;

	int i;
	string nm;
	map<string,bool>::iterator it;
	string add;

	if (thetype=="arff") {
		ofstream file(STR(filename));
		file<<"%KIF GENERATED"<<endl<<endl;
		file<<"@RELATION "<<STR(name)<<endl<<endl;
		for (it=attributes.begin();it!=attributes.end();it++) {
			nm="@ATTRIBUTE ";
			nm+=it->first.c_str();
			if (it->second==true)
				nm+=" STRING\n";
			else
				nm+=" NUMERIC\n";
			file<<STR(nm);
		}		
		nm="@ATTRIBUTE class {";
		i=0;
		for (it=classes.begin();it!=classes.end();it++) {
			if (i!=0)
				nm+=",";
			i=1;
			nm+=it->first;
		}
		nm+="}";
		file<<STR(nm)<<endl<<endl<<"@DATA"<<endl;
		for (itins=values.begin();itins!=values.end();itins++) {
			for (itclass=(*itins).begin();itclass!=(*itins).end();itclass++) {
				add="";
				for (itatts=attributes.begin();itatts!=attributes.end();itatts++) {
					k=kifNULL;
					if (itclass->second.find(itatts->first)!=itclass->second.end())
						k=itclass->second[itatts->first];
					if (add!="")
						add+=",";
					if (itatts->second==true)
						add+="\"";
					else
						if (k==kifNULL)
							add+="0";
					add+=k->String();
					if (itatts->second==true)
						add+="\"";
				}
				add+=",";
				add+=itclass->first;
				add=c_to_utf8(USTR(add));
				file<<STR(add)<<endl;
			}
		}
		return;
	}

	if (thetype=="xml") {
		if (kifCodeVector[idkifcode]->parseur==NULL)
			return;

		map<string,xmlNodePtr> classnodes;
		map<string,xmlNodePtr>::iterator itnodes;
		char buffer[100];
		sprintf_s(buffer,100,"<Matrix name=\"%s\"/>",STR(name));
		//On cree la racine de notre document
		xmlDocPtr doc=xmlSAXParseMemory(NULL,buffer,strlen(buffer),0);
		doc->encoding=xmlCharStrdup("UTF-8");
		//First we store ou class elements
		xmlNodePtr node;
		for (it=classes.begin();it!=classes.end();it++) {						
			node=kifCodeVector[idkifcode]->parseur->CreationNoeudXML(doc->children,"class","");
			nm=c_to_utf8(USTR(it->first));
			kifCodeVector[idkifcode]->parseur->AjouteAttributValeur(node,"name",STR(nm));				
		}
		for (it=attributes.begin();it!=attributes.end();it++) {
			node=kifCodeVector[idkifcode]->parseur->CreationNoeudXML(doc->children,"attribute","");
			nm=c_to_utf8(USTR(it->first));
			kifCodeVector[idkifcode]->parseur->AjouteAttributValeur(node,"name",STR(nm));
			if (it->second==true)
				kifCodeVector[idkifcode]->parseur->AjouteAttributValeur(node,"type","string");
			else
				kifCodeVector[idkifcode]->parseur->AjouteAttributValeur(node,"type","float");
		}

		xmlNodePtr parent=kifCodeVector[idkifcode]->parseur->CreationNoeudXML(doc->children,"values","");
		i=0;
		xmlNodePtr xmlclass;
		for (itins=values.begin();itins!=values.end();itins++) {
			for (itclass=(*itins).begin();itclass!=(*itins).end();itclass++) {
				itnodes=classnodes.find(itclass->first);
				if (itnodes==classnodes.end()) {
					xmlclass=kifCodeVector[idkifcode]->parseur->CreationNoeudXML(parent,"class","");
					nm=c_to_utf8(USTR(itclass->first));
					kifCodeVector[idkifcode]->parseur->AjouteAttributValeur(xmlclass,"name",STR(nm));
					classnodes[itclass->first]=xmlclass;
				}
				else
					xmlclass=itnodes->second;

				add="";
				for (itatts=attributes.begin();itatts!=attributes.end();itatts++) {
					k=kifNULL;
					if (itclass->second.find(itatts->first)!=itclass->second.end())
						k=itclass->second[itatts->first];
					if (add!="")
						add+=",";
					if (itatts->second==true)
						add+="\"";
					else
						if (k==kifNULL)
							add+="0";
					add+=k->String();
					if (itatts->second==true)
						add+="\"";
				}
				add=c_to_utf8(USTR(add));
				xmlNodePtr xmlatt=kifCodeVector[idkifcode]->parseur->CreationNoeudXML(xmlclass,"instance",STR(add));
				sprintf_s(buffer,100,"%d",i);
				kifCodeVector[idkifcode]->parseur->AjouteAttributValeur(xmlatt,"id",buffer);
			}
			i++;
		}
		xmlKeepBlanksDefault(0);    
		xmlSaveFormatFileEnc(STR(filename),doc,"UTF-8",1);
		return;
	}
	kifCodeVector[idkifcode]->Returnerror(kifErrorStrings[154]);
}

void KifMatrix::Addvalue(string& c,string& a,KifElement* v,long line) {
	int nb=values.size();
	if (nb<=line) {
		for (int i=nb;i<=line;i++) {
			map<string, map<string,KifElement*> > v;
			values.push_back(v);
			values[i][c][a]=kifNULL;
		}
	}
	
	if (attributes.find(a)==attributes.end()) {
		if (v->type==kifString)
			attributes[a]=true;
		else
			attributes[a]=false;
	}

	classes[c]=true;
	KifElement* ke=values[line][c][a];
	if (ke!=kifNULL && ke!=NULL)
		values[line][c][a]->Resetreference();
	else
		nbelements++;
	v->Setreference();
	values[line][c][a]=v;
}

bool KifMatrix::Setvalue(KifElement* ke,KifElement* index,KifElement* dom) {
	if (ke->type!=kifMatrix)
		return true;

	vector<map<string,map<string,KifElement*> > >::iterator itins;
	map<string,map<string,KifElement*> >::iterator itclass;
	map<string, KifElement*>::iterator itatt;
	long i=0;
	for (itins=values.begin();itins!=values.end();itins++) {
		for (itclass=(*itins).begin();itclass!=(*itins).end();itclass++) {
			string c=itclass->first;
			for (itatt=itclass->second.begin();itatt!=itclass->second.end();itatt++) {
				string a=itatt->first;
				Addvalue(c,a,itatt->second,i);
			}
		}
		i++;
	}
	return true;
}

bool KifSelf::Setvalue(KifElement* ke,KifElement* index,KifElement* dom) {
	value=ke;
	return true;
}

bool KifXPathExpression::Setvalue(KifElement* a,KifElement* index,KifElement* dom) {
	if (a==kifNULL) {
		txml=NULL;
		return true;
	}

	if (a->type!=kifXPathExpression)
		return true;
	KifXPathExpression* kx=(KifXPathExpression*)a;
	txml=kx->txml;
	return true;
}

bool KifXipRule::Setvalue(KifElement* a,KifElement* index,KifElement* dom) {
	if (a==kifNULL) {
		values.clear();
		return true;
	}

	if (a->type!=kifXipRule)
		return true;
	KifXipRule* kx=(KifXipRule*)a;
	vector<VectTestFonction*>::iterator it;
	for (it=kx->values.begin();it!=kx->values.end();it++)
		values.push_back(*it);
	script=kx->script;
	return true;
}

bool KifXipNode::Setvalue(KifElement* a,KifElement* index,KifElement* dom) {
	if (a==kifNULL) {
		value=NULL;
		return true;
	}

	if (a->type!=kifXipNode)
		return true;
	KifXipNode* kx=(KifXipNode*)a;
	value=kx->value;
	return true;
}

bool KifXipDependency::Setvalue(KifElement* a,KifElement* index,KifElement* dom) {
	if (a==kifNULL) {
		value=NULL;
		return true;
	}

	if (a->type!=kifXipDependency)
		return true;
	KifXipDependency* kx=(KifXipDependency*)a;
	value=kx->value;
	return true;
}

bool KifXipGeneration::Setvalue(KifElement* a,KifElement* index,KifElement* dom) {
	if (a==kifNULL) {
		value=NULL;
		return true;
	}

	if (a->type!=kifXipGeneration)
		return true;
	KifXipGeneration* kx=(KifXipGeneration*)a;
	value=kx->value;
	return true;
}

bool KifStringPtr::Setvalue(KifElement* ke,KifElement* idx,KifElement* dom) {
	if (idx==NULL || idx==kifNULL)
		*value=ke->String();
	else {
		int ikey=idx->Integer();
		string s=ke->String();
		if (value==NULL)
			return false;
		return c_char_index_assign(*value,s,ikey);
	}
	return true;
}

bool KifXMLNodePtr::Setvalue(KifElement* a,KifElement* index,KifElement* dom) {
	if (a==kifNULL) {
		if (value!=NULL)
			value->noeud=NULL;
		return true;
	}
	if (value==NULL)
		return true;
	value->noeud=a->Node();
	return true;
}

bool KifVariable::Setvalue(KifElement* k,KifElement* idx,KifElement* dom) {
	if (value->reference>1) {		
		
		//Simpler case, value and k have the same type
		value->Resetreference();
		if (k->type==value->type && value->type!=kifDomain) {
			if (value->type==kifVector || value->type==kifMap) {
				value=k->Copy(NULL);
				value->reference++;
				return true;
			}

			k->Setreference();
			value=k;
			return true;
		}

		if (value->type==kifDomain && k->type==kifFrame) {
			//Then we have either a new instance, if k is a kifFrame
			//we need to check if we can use it...
			KifDomain* vdom=(KifDomain*)value;
			if (vdom->frame==k) {
				value=k->Copy(NULL,k);
				value->Setframe(k);
				value->reference++;
			}
			return false;
		}
		//In the other cases, we create a new instance and we set our value with k
		value=value->Newinstance();
		value->Setreference();
		value->Setvalue(k,idx,dom);
	}
	else
		value->Setvalue(k,idx,dom);
	return true;
}

bool KifVariableDeclaration::Setvalue(KifElement* k,KifElement* idx,KifElement* dom) {
	if (value->reference>1) {		
		
		//Simpler case, value and k have the same type
		value->Resetreference();
		if (k->type==value->type && value->type!=kifDomain) {
			k->Setreference();
			value=k;
			return true;
		}

		if (value->type==kifDomain && k->type==kifFrame) {
			//Then we have either a new instance, if k is a kifFrame
			//we need to check if we can use it...
			KifDomain* vdom=(KifDomain*)value;
			if (vdom->frame==k) {
				value=k->Copy(NULL,k);
				value->Setframe(k);
				value->reference++;
			}
			return false;
		}
		//In the other cases, we create a new instance and we set our value with k
		value=value->Newinstance();
		value->Setreference();
		value->Setvalue(k,idx,dom);
	}
	else
		value->Setvalue(k,idx,dom);
	return true;
}


bool KifCouple::Setvalue(KifElement* k,KifElement* idx,KifElement* dom) {
	//We simply replace, the current element with k
	if (indexes->interval==true)
		return true;

	KifElement* kres=recipient;
	KifElement* key=indexes->key->Execute(kifNULL,domain,kifNULL,false);
	//Specific case, where the function is also an index reference...
	if (indexes->function!=NULL && indexes->function->type==kifIndex) {
		KifIndex* kidx=(KifIndex*)indexes->function;
		while (kidx!=NULL) {
			//In this case, what we try to do is first to get the element with this index
			kres=kres->Execute(kifNULL,domain,indexes,false);					
			if (kidx->function!=NULL && kidx->function->type==kifIndex) {
				indexes=kidx;
				kidx=(KifIndex*)kidx->function;
			}
			else
				break;
		}

		key=kidx->key->Execute(kifNULL,domain,kifNULL,false);
		kres->Setvalue(k,key,dom);
	}
	else
		recipient->Setvalue(k,key,dom);
	return true;
}


//In this case, we use the VALUE as the type
string KifCouple::Type() {
	if (indexes->interval==true)
		return "";

	KifElement* key=indexes->key->Execute(kifNULL,domain,kifNULL,false);
	KifElement* value=recipient->Execute(this,domain,key,false);
	return value->Type();
}

bool KifVector::Setvalue(KifElement* ke,KifElement* idx,KifElement* dom) {
	if (ke==this)
		return true;
	if (ke==kifNULL) {
		Clear();
		return true;
	}
	int nb;

	if (idx==NULL || idx==kifNULL){
		if (ke->type==kifVector) {
			KifVector* kvect=(KifVector*)ke;
			//We copy all values from ke to this
			Clear();
			vector<KifElement*>::iterator it;
			for (it=kvect->values.begin();it!=kvect->values.end();it++) {
				KifElement* x=*it;
				x->Setreference();
				values.push_back(x);
			}
		}
		else {
			//We gather all the keys from the MAP
			if (ke->type==kifMap) {
				map<string,KifElement*>::iterator itmap;
				KifMap* kmap=(KifMap*)ke;
				Clear();
				for (itmap=kmap->values.begin();itmap!=kmap->values.end();itmap++)
					Push(new KifString(kifCodeVector[idkifcode],NULL,itmap->first));
			}
			else {
				//We clean all elements and store the new value instead
				KifElement* knew;
				if (values.size()!=0) {
					vector<KifElement*>::iterator it;
					for (it=values.begin();it!=values.end();it++) {
						knew=ke->Copy((KifDomain*)dom);
						knew->reference=(*it)->reference;
						nb=reference;
						while (nb>0) {
							(*it)->Resetreference();
							nb--;
						}
						*it=knew;
					}
				}
				else
					//In this case, we simply "push" it
					Push(ke);
			}
		}
		return true;
	}

	int ikey=idx->Integer();
	ke->Setreference();
	
	if (ikey>=values.size())
		values.push_back(ke);
	else {
		if (values[ikey]!=NULL) {
			nb=reference;
			while (nb>0) {
				values[ikey]->Resetreference();
				nb--;
			}
		}			
		values[ikey]=ke;
	}
	return true;
}

bool KifVectorNumberPtr::Setvalue(KifElement* ke,KifElement* idx,KifElement* dom) {
	if (ke==this)
		return true;

	if (ke==kifNULL) {
		Clear();
		return true;
	}

	vector<KifElement*>::iterator it;
	if (idx==NULL || idx==kifNULL) {
		if (ke->type==kifVector) {
			KifVector* kvect=(KifVector*)ke;
			//We copy all values from ke to this
			Clear();
			for (it=kvect->values.begin();it!=kvect->values.end();it++) {
				KifElement* x=*it;				
				values->push_back(x->Float());
			}
		}
		else
			values->push_back(ke->Float());
		return true;
	}

	int ikey=idx->Integer();
	if (ikey>=values->size())
		values->push_back(ke->Float());
	else {
		values->affecte(ikey,ke->Float());
	}
	return true;
}

bool KifVectorStringPtr::Setvalue(KifElement* ke,KifElement* idx,KifElement* dom) {
	if (ke==this)
		return true;
	if (ke==kifNULL) {
		Clear();
		return true;
	}


	vector<KifElement*>::iterator it;
	if (idx==NULL || idx==kifNULL) {
		if (ke->type==kifVector) {
			KifVector* kvect=(KifVector*)ke;
			//We copy all values from ke to this
			Clear();
			for (it=kvect->values.begin();it!=kvect->values.end();it++) {
				KifElement* x=*it;
				values->push_back(new string(x->String()));
			}
		}
		else
			values->push_back(new string(ke->String()));
		return true;
	}

	int ikey=idx->Integer();
	if (ikey>=values->size())
		values->push_back(new string(ke->String()));
	else {
		if ((*values)[ikey]!=NULL)
			delete (*values)[ikey];
		values->affecte(ikey,new string(ke->String()));
	}
	return true;
}

bool KifString::Setvalue(KifElement* ke,KifElement* idx,KifElement* dom) {
	if (idx==NULL || idx==kifNULL)
		value=ke->String();
	else {
		int ikey=idx->Integer();
		string s=ke->String();
		return c_char_index_assign(value,s,ikey);
	}
	return true;
}

bool KifMap::Setvalue(KifElement* ke,KifElement* idx,KifElement* dom) {
	if (ke==this)
		return true;
	if (ke==kifNULL) {
		Clear();
		return true;
	}

	int nb;

	if (idx==NULL || idx==kifNULL) {
		if (ke->type==kifMap) {
			KifMap* kvect=(KifMap*)ke;
			//We copy all values from ke to this
			Clear();
			map<string,KifElement*>::iterator it;
			for (it=kvect->values.begin();it!=kvect->values.end();it++) {
				KifElement* x=it->second;
				string n=it->first;
				x->Setreference();
				values[n]=x;
			}
		}
		else
			if (ke->type==kifVector) {
				KifVector* kvect=(KifVector*)ke;
				vector<KifElement*>::iterator it;
				Clear();
				nb=0;
				char buff[20];
				for (it=kvect->values.begin();it!=kvect->values.end();it++) {
					sprintf_s(buff,20,"%d",nb);
					(*it)->Setreference();
					values[buff]=*it;
					nb++;
				}
			}
			else {
				map<string,KifElement*>::iterator it;
				KifElement* knew;
				for (it=values.begin();it!=values.end();it++) {
					knew=ke->Copy((KifDomain*)dom);
					nb=reference;
					while (nb>0) {
						it->second->Resetreference();
						knew->Setreference();
						nb--;
					}
					it->second=knew;
				}
			}
	}
	else {
		string key=idx->String();
		if (Values(key)!=NULL) {
			nb=reference;
			while (nb>0) {
				values[key]->Resetreference();
				nb--;
			}
		}
		ke->Setreference();
		values[key]=ke;
	}
	return true;
}


bool KifMapPtr::Setvalue(KifElement* ke,KifElement* idx,KifElement* dom) {
	if (ke==this)
		return true;
	if (ke==kifNULL) {
		Clear();
		return true;
	}


	if (idx==NULL || idx==kifNULL) {
		if (ke->type!=kifMap)
			return true;
		KifMap* kvect=(KifMap*)ke;
		map<string,KifElement*>::iterator it;
		//We copy all values from ke to this
		Clear();
		for (it=kvect->values.begin();it!=kvect->values.end();it++) {
			KifElement* x=it->second;
			string n=it->first;				
			(*values)[n]=x->String();
		}
	}
	else {
		string key=idx->String();
		(*values)[key]=ke->String();
	}
	return true;
}

//===================================================================
KifElement* KifSelf::Declaration(string& n) {
	if (kifCodeVector[idkifcode]->KifBaseFunctionTest(n))
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	if (kifCodeVector[idkifcode]->KifDeclaredFunctionTest(n)==true)
		return kifTRUE;
	return NULL;
}

KifElement* KifDomain::Declaration(string& s) {
	if (s=="this")
		return this;
	if (s=="list")
		return kifCodeVector[idkifcode]->kifBaseFunctions["list"];
	map<string,KifElement*>::iterator iter=declarations.find(s);
	if (iter==declarations.end())
		return NULL;
	return iter->second;
}

KifElement* KifGraph::Declaration(string& n) {
	if (TestkifFunctions(n,kifGraphFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifCall::Declaration(string& s) {
	if (s=="apply")
		return kifCodeVector[idkifcode]->kifBaseFunctions["apply"];
	return NULL;
}

KifElement* KifMatrix::Declaration(string& n) {
	if (TestkifFunctions(n,kifMatrixFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifCallFunction::Declaration(string& s) {
	map<string,KifElement*>::iterator iter=declarations.find(s);
	if (iter==declarations.end())
		return NULL;
	return iter->second;
}

KifElement* KifString::Declaration(string& n) {
	if (TestkifFunctions(n,kifStringFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifFloat::Declaration(string& n) {
	if (kifMathFunctions[n]!=NULL || TestkifFunctions(n,kifGenericFunctions)==true || n=="get")
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}


KifElement* KifInteger::Declaration(string& n) {
	if (kifMathFunctions[n]!=NULL || TestkifFunctions(n,kifGenericFunctions)==true  || n=="get")
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifTime::Declaration(string& n) {
	if (TestkifFunctions(n,kifTimeFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}
#ifdef NTMCONNECTION
KifElement* KifFST::Declaration(string& n) {
	if (TestkifFunctions(n,kifFSTFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}
#endif

KifElement* KifParser::Declaration(string& n) {
	if (TestkifFunctions(n,kifParserFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	if (ipar!=-1) {
		GlobalParseur* parseur=XipSelectGrammar(ipar);
		if (parseur->kifcode!=NULL)
			return parseur->kifcode->kstart->Declaration(n);
	}
	else
		return kifTRUE;
	return NULL;
}

KifElement* KifKiF::Declaration(string& n) {
	if (TestkifFunctions(n,kifKiFFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	if (ipar!=-1) {
		KifCode* kifcode=kifCodeVector[ipar];
		if (kifcode!=NULL)
			return kifcode->kstart->Declaration(n);
	}
	else
		return kifTRUE;
	return NULL;
}


KifElement* KifVector::Declaration(string& n) {
	if (TestkifFunctions(n,kifVectorFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifVectorNumberPtr::Declaration(string& n) {
	if (TestkifFunctions(n,kifVectorFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifVectorStringPtr::Declaration(string& n) {
	if (TestkifFunctions(n,kifVectorFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifMap::Declaration(string& n) {
	if (TestkifFunctions(n,kifMapFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifMapPtr::Declaration(string& n) {
	if (TestkifFunctions(n,kifMapFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifFile::Declaration(string& n) {	
	if (TestkifFunctions(n,kifFileFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifXipRule::Declaration(string& n) {
	if (TestkifFunctions(n,kifXipRuleFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}


KifElement* KifXipNode::Declaration(string& n) {
	if (TestkifFunctions(n,kifXipFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifIterator::Declaration(string& n) {
	if (TestkifFunctions(n,kifIteratorFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}


KifElement* KifXMLNode::Declaration(string& n) {
	if (TestkifFunctions(n,kifXMLFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifXMLNodePtr::Declaration(string& n) {
	if (TestkifFunctions(n,kifXMLFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}


KifElement* KifXipDependency::Declaration(string& n) {
	if (TestkifFunctions(n,kifDependencyFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

KifElement* KifXipGeneration::Declaration(string& n) {
	if (TestkifFunctions(n,kifGenerationFunctions)==true)
		return kifCodeVector[idkifcode]->kifBaseFunctions[n];
	return NULL;
}

//===================================================================
void KifCall::List(KifVector* fcts) {
	fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,"apply"));
}


void KifMatrix::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifMatrixFunctions.begin();it!=kifMatrixFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));		
}


void KifString::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifStringFunctions.begin();it!=kifStringFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifFloat::List(KifVector* fcts) {
	map<string,mathFunc>::iterator itx;
	for (itx=kifMathFunctions.begin();itx!=kifMathFunctions.end();itx++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,itx->first));
	map<string,bool>::iterator it;
	for (it=kifGenericFunctions.begin();it!=kifGenericFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}


void KifInteger::List(KifVector* fcts) {
	map<string,mathFunc>::iterator itx;
	for (itx=kifMathFunctions.begin();itx!=kifMathFunctions.end();itx++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,itx->first));
	map<string,bool>::iterator it;
	for (it=kifGenericFunctions.begin();it!=kifGenericFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifTime::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifTimeFunctions.begin();it!=kifTimeFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}


void KifVector::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifVectorFunctions.begin();it!=kifVectorFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifVectorNumberPtr::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifVectorFunctions.begin();it!=kifVectorFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifVectorStringPtr::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifVectorFunctions.begin();it!=kifVectorFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifMap::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifMapFunctions.begin();it!=kifMapFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifMapPtr::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifMapFunctions.begin();it!=kifMapFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifFile::List(KifVector* fcts) {	
	map<string,bool>::iterator it;
	for (it=kifFileFunctions.begin();it!=kifFileFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifXipRule::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifXipRuleFunctions.begin();it!=kifXipRuleFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}


void KifXipNode::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifXipFunctions.begin();it!=kifXipFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifIterator::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifIteratorFunctions.begin();it!=kifIteratorFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}


void KifXMLNode::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifXMLFunctions.begin();it!=kifXMLFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifXMLNodePtr::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifXMLFunctions.begin();it!=kifXMLFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}


void KifXipDependency::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifDependencyFunctions.begin();it!=kifDependencyFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifXipGeneration::List(KifVector* fcts) {
	map<string,bool>::iterator it;
	for (it=kifGenerationFunctions.begin();it!=kifGenerationFunctions.end();it++)
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,it->first));
}

void KifDomain::List(KifVector* fcts) {
	map<string,KifElement*>::iterator iter;
	for (iter=declarations.begin();iter!=declarations.end();iter++) {
		fcts->Push(new KifString(kifCodeVector[idkifcode],NULL,iter->first));
	}
}

//===================================================================
KifElement* KifCallFunction::applyfunction(KifElement* a) {
	if (function==NULL)
		return a;
	if (a->type==kifDomain)
		return function->Execute(kifNULL,(KifDomain*)a,a,false);
	return function->Execute(kifNULL,this,a,false);
}

KifElement* KifElement::applyfunc(KifElement* a,KifElement* b,KifDomain* func) {
	string name=func->Name();
	KifCallFunction kfunc(NULL,this,name,(KifFunction*)func);
	kfunc.idkifcode=idkifcode;
	kfunc.parameters.push_back(a);
	kfunc.parameters.push_back(b);
	return kfunc.Execute(kifNULL,func,kifNULL,false);
}

KifElement* KifElement::applyfullfunc(KifElement* a,KifElement* b,KifFunction* func) {
	string name=func->Name();
	KifCallFunction kfunc(NULL,this,name,func);
	kfunc.idkifcode=idkifcode;
	kfunc.parameters.push_back(a);
	kfunc.parameters.push_back(b);

	return kfunc.Execute(kifNULL,(KifDomain*)this,kifNULL,false);
}

KifElement* KifDomain::Framedefined(string methodname,KifElement* pattern,KifDomain* dom) {


	if (dom==NULL)
		dom=this;

	KifElement* func=kifCodeVector[idkifcode]->Getbasic(methodname,dom);
	if (func!=NULL) {
		KifCallFunction kfunc(NULL,this,methodname,(KifFunction*)func);
		kfunc.idkifcode=idkifcode;
		return kfunc.Execute(pattern,this,kifNULL,false);
	}
	return kifNULL;
}
//--------------------------------------------------------------------------------------
KifElement* KifVector::Copy(KifDomain* kp,KifElement* dom) {
	KifElement* ke=new KifVector(kifCodeVector[idkifcode],kp);
	vector<KifElement*>::iterator itx;
	for (itx=values.begin();itx!=values.end();itx++) {
		KifElement* kx=(*itx)->Copy(kp,dom);
		ke->Push(kx);
	}

	return ke;
}


KifElement* KifMap::Copy(KifDomain* kp,KifElement* dom) {
	KifMap* ke=new KifMap(kifCodeVector[idkifcode],kp);
	map<string,KifElement*>::iterator itx;
	for (itx=values.begin();itx!=values.end();itx++) {
		string n=itx->first;
		if (itx->second!=NULL) {
			ke->values[n]=itx->second->Copy(kp,dom);		
			ke->values[n]->Setreference();
		}
		else
			ke->values[n]=NULL;
	}
	return ke;
}


KifElement* KifDomain::Copy(KifDomain* dom,KifElement* decl) {
	map<string,KifElement*>::iterator it;
	KifDomain* kdom=new KifDomain(kifCodeVector[idkifcode],dom,decl->Name(),kifDomain);
	kdom->frame=decl->Typevariable();
	kdom->Disableadding();
	for (it=declarations.begin();it!=declarations.end();it++) {
		string n=it->first;
		KifElement* xe=kifCodeVector[idkifcode]->Getbasic(n,this);
		if (xe!=NULL) {
			KifElement* ke=xe->Copy(kdom,xe);
			kdom->Setdeclaration(n,ke,NULL);
		}
	}
	return kdom;
}	
//--------------------------------------------------------------------------------------
KifElement* KifDomain::applyinitial(KifDomain* dom,KifElement* params) {
	static string na="initial";
	KifElement* func=Declaration(na);
	//If such a function has been defined, we use it...
	if (func!=NULL) {
		KifCallFunction kfunc(NULL,this,na,(KifFunction*)func);
		if (params->type==kifInstruction) {
			KifInstruction* ki=(KifInstruction*)params;
			for (int i=0;i<ki->instructions.size();i++) {
				KifElement* ke=ki->instructions[i]->Execute(kifNULL,dom,kifNULL,false);
				kfunc.parameters.push_back(ke);
			}
		}
		kfunc.idkifcode=idkifcode;
		return kfunc.Execute(kifNULL,this,kifNULL,false);
	}	
	return kifFALSE;
}



KifElement* KifDomain::applyone(string na,KifElement* a) {
	KifElement* func=Declaration(na);
	//If such a function has been defined, we use it...
	if (func!=NULL) {
		KifCallFunction kfunc(NULL,this,na,(KifFunction*)func);
		kfunc.idkifcode=idkifcode;
		//We add our parameters
		kfunc.parameters.push_back(a);
		return kfunc.Execute(kifNULL,this,kifNULL,false);
	}	
	return kifFALSE;
}

KifElement* KifFile::in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
	if (b!=this)
		return kifFALSE;
	if (EndOfFile()==kifTRUE)
		return kifFALSE;
	if (a->type==kifInteger || a->type==kifFloat) {
		int c=(uchar)readfile->get();
		KifInteger v(NULL,NULL,c);
		a->Setvalue(&v);
	}
	else {
		
		if (a->type==kifVector) {
			KifElement* e=Readoneline();
			a->Push(e);
		}
		else {
			string s;
			getline(*readfile,s,'\n');
			KifString v(NULL,NULL,s);
			a->Setvalue(&v,kifNULL,kfunc);
		}
	}
	return kifTRUE;
}

KifElement* KifDomain::in(KifElement* a,KifElement* b,KifDomain* kexfunc,KifElement* pattern,bool idx) {
	static string na="in";
	if (kexfunc!=NULL)
		return applyfunc(a,b,kexfunc);

	
	KifElement* func=Declaration(na);
	//If such a function has been defined, we use it...
	if (func!=NULL) {
		KifCallFunction kfunc(NULL,this,na,(KifFunction*)func);
		kfunc.idkifcode=idkifcode;
		//We add our parameters
		kfunc.parameters.push_back(a);
		return kfunc.Execute(kifNULL,this,kifNULL,false);
	}	
	return kifNULL;
}


KifElement* KifMatrix::in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
	KifElement* k;
	KifElement* res;
	vector<map<string,map<string,KifElement*> > >::iterator itins;
	map<string,map<string,KifElement*> >::iterator itclass;
	map<string, KifElement*>::iterator itatt;
	for (itins=values.begin();itins!=values.end();itins++) {
		for (itclass=(*itins).begin();itclass!=(*itins).end();itclass++) {
			for (itatt=itclass->second.begin();itatt!=itclass->second.end();itatt++) {
				k=itatt->second;
				res=k->in(a,k,kfunc,this,false);
				if (res!=kifFALSE && res!=kifNULL)
					return kifTRUE;
			}
		}
	}
	return kifFALSE;
}

KifElement* KifString::in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
	if (kfunc==NULL) {
		if (b->type!=kifString)
			return b->in(a,b,kfunc,this,true);
		string s1=a->String();
		string s2=b->String();
		KifElement* kv;
		if (pattern->type==kifVector || pattern->type==kifMap) {
			if (s1==s2)
				return kifTRUE;
			return kifFALSE;
		}

		int rs=s2.find(s1);
		if (rs!=string::npos) {
			if (pattern->type==kifString)
				return new KifString(kifCodeVector[idkifcode],NULL,s1);

			if (idx==false)
				return kifTRUE;
			

			kv=new KifInteger(kifCodeVector[idkifcode],NULL,rs);
			return kv;
		}
		if (pattern->type==kifString)
			return kifNULL;

		if (idx==false)
			return kifFALSE;
		return kifMINUSONE;
	}
	else
		return applyfunc(a,b,kfunc);
}

KifElement* KifVector::in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
	KifElement* ke;
	KifElement* k;
	KifElement* res;
	KifElement* found=kifFALSE;

	if (b->type==kifMap) {
		Clear();
		KifMap* bb=(KifMap*)b;
		map<string,KifElement*>::iterator itx;
		for (itx=bb->values.begin();itx!=bb->values.end();itx++) {
			k=itx->second;
			res=k->in(a,k,kfunc,this,false);
			if (res!=kifFALSE && res!=kifNULL)
				Push(new KifString(kifCodeVector[idkifcode],NULL,itx->first));
		}
		return this;
	}

	if (b->type==kifVector) {
		if (b!=this)
			Clear();
		KifVector* bb=(KifVector*)b;
		vector<KifElement*>::iterator it;
		int ix=0;
		for (it=bb->values.begin();it!=bb->values.end();it++) {
			k=*it;
			res=k->in(a,k,kfunc,this,false);
			if (res!=kifFALSE && res!=kifNULL) {
				if (b!=this) {
					Push(new KifInteger(kifCodeVector[idkifcode],NULL,ix));
					found=this;
				}
				else {
					if (idx==true) {
						ke=new KifInteger(kifCodeVector[idkifcode],NULL,ix);						
						return ke;
					}
					return kifTRUE;
				}
			}
			ix++;
		}
		
		if (b==this && idx==true)
			return kifMINUSONE;

		return found;
	}

	if (b!=this)
		return b->in(a,b,kfunc,this,idx);
	if (idx==true)
		return kifMINUSONE;
	return kifFALSE;
}

KifElement* KifVectorNumberPtr::in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
	if (b!=this)
		return kifFALSE;

	int ix=0;
	KifElement* ke;
	double v=a->Float();
	for (ix=0;ix<values->size();ix++) {
		if (values->vecteur[ix]==NULL)
			continue;
		if (values->vecteur[ix]==v) {
			if (idx==true) {
				ke=new KifInteger(kifCodeVector[idkifcode],NULL,ix);						
				return ke;
			}
			return kifTRUE;
		}
	}
	if (idx==true)
		return kifMINUSONE;
	return kifFALSE;
}

KifElement* KifVectorStringPtr::in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
	if (b!=this)
		return kifFALSE;

	int ix=0;
	string v=a->String();
	for (ix=0;ix<values->size();ix++) {
		if (values->vecteur[ix]==NULL)
			continue;
		if (*values->vecteur[ix]==v) {
			if (idx==true) {
				KifElement* ke=new KifInteger(kifCodeVector[idkifcode],NULL,ix);						
				return ke;
			}
			return kifTRUE;
		}
	}
	if (idx==true)
		return kifMINUSONE;
	return kifFALSE;
}

KifElement* KifMap::in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
	KifElement* ke;
	KifElement* k;
	KifElement* res;
	KifElement* found=kifFALSE;


	if (b->type==kifMap) {
		if (b!=this)
			Clear();
		KifMap* bb=(KifMap*)b;
		map<string,KifElement*>::iterator itx;
		for (itx=bb->values.begin();itx!=bb->values.end();itx++) {
			k=itx->second;
			res=k->in(a,k,kfunc,this,false);
			if (res!=kifFALSE && res!=kifNULL) {				
				if (b!=this) {
					ke=new KifString(kifCodeVector[idkifcode],NULL,itx->first);
					Setvalue(res,ke);
					found=this;
				}
				else 
					if (idx==true) {
						ke=new KifString(kifCodeVector[idkifcode],NULL,itx->first);
						return ke;
					}
					else
						return kifTRUE;
			}
		}

		if (b==this && idx==true)
			return kifNULL;
		return found;
	}

	if (b->type==kifVector) {
		KifVector* bb=(KifVector*)b;
		vector<KifElement*>::iterator it;
		int ix=0;
		for (it=bb->values.begin();it!=bb->values.end();it++) {
			k=*it;
			res=k->in(a,k,kfunc,this,false);
			if (res!=kifFALSE && res!=kifNULL) {
				if (b!=this) {					
					ke=new KifInteger(kifCodeVector[idkifcode],NULL,ix);
					Setvalue(res,ke);
				}
			}
			ix++;
		}
		return this;
	}

	if (b!=this)
		return b->in(a,b,kfunc,this,idx);

	if (idx==true)
		return kifNULL;

	return kifFALSE;
}

KifElement* KifMapPtr::in(KifElement* a,KifElement* b,KifDomain* kfunc,KifElement* pattern,bool idx) {
	if (b!=this)
		return kifFALSE;


	map<string,string>::iterator itx;
	string v=a->String();
	for (itx=values->begin();itx!=values->end();itx++) {
		if (itx->second==v) {		
			if (idx==true) {
				KifElement* ke=new KifString(kifCodeVector[idkifcode],NULL,itx->first);
				return ke;
			}
			else
				return kifTRUE;
		}
	}
	if (idx==true)
		return kifNULL;
	return kifFALSE;
}

//--------------------------------------------------------------------------------------
void KifDomain::SetInstances(KifElement* dom,map<string,KifElement*>* stacks) {
	map<string,KifElement*>::iterator it;
	dom->Disableadding();
	for (it=declarations.begin();it!=declarations.end();it++) {
		string n=it->first;
		KifElement* xe=it->second;
		if (xe!=NULL)
			dom->Setdeclaration(n,xe,stacks);
	}
}	

void KifDomain::SetInstanceslocal(KifElement* dom,map<string,KifElement*>* stacks) {
	map<string,KifElement*>::iterator it;
	dom->Disableadding();
	for (it=declarations.begin();it!=declarations.end();it++) {
		string n=it->first;
		if (dom->Declaration(n)==NULL) {
			KifElement* xe=it->second;
			if (xe!=NULL) {
				dom->Setdeclaration(n,xe,stacks);
			}
		}
	}
}	


void KifDomain::Setreference() {
	reference++;
	map<string,KifElement*>::iterator it;
	for (it=declarations.begin();it!=declarations.end();it++) {
		if (it->second!=NULL)
			it->second->Setreference();
	}	
}

void KifVector::Removecrossreference(KifElement* key,int nb) {
	int idx=key->Integer();
	if (idx<values.size() && values[idx]!=NULL && values[idx]!=kifNULL) {
		while (nb>0) {
			values[idx]->Resetreference();
			nb--;
		}
	}
}

void KifMap::Removecrossreference(KifElement* key,int nb) {
	string idx=key->String();
	if (values.find(idx)!=values.end()) {
		if (values[idx]!=kifNULL) {
			while (nb>0) {
				values[idx]->Resetreference();
				nb--;
			}
		}
	}
}

void KifMatrix::Setreference() {
	reference++;
	vector<map<string,map<string,KifElement*> > >::iterator itins;
	map<string,map<string,KifElement*> >::iterator itclass;
	map<string, KifElement*>::iterator itatt;
	for (itins=values.begin();itins!=values.end();itins++) {
		for (itclass=(*itins).begin();itclass!=(*itins).end();itclass++) {
			for (itatt=itclass->second.begin();itatt!=itclass->second.end();itatt++) {
				if (itatt->second!=kifNULL)
					itatt->second->Setreference();
			}
		}
	}
}

void KifMatrix::Clean() {
	reference--;
	vector<map<string,map<string,KifElement*> > >::iterator itins;
	map<string,map<string,KifElement*> >::iterator itclass;
	map<string, KifElement*>::iterator itatt;
	for (itins=values.begin();itins!=values.end();itins++) {
		for (itclass=(*itins).begin();itclass!=(*itins).end();itclass++) {
			for (itatt=itclass->second.begin();itatt!=itclass->second.end();itatt++) {
				if (itatt->second!=kifNULL)
					itatt->second->Clean();
			}
		}
	}
	values.clear();
}


void KifMatrix::Resetreference() {
	reference--;
	vector<map<string,map<string,KifElement*> > >::iterator itins;
	map<string,map<string,KifElement*> >::iterator itclass;
	map<string, KifElement*>::iterator itatt;
	for (itins=values.begin();itins!=values.end();itins++) {
		for (itclass=(*itins).begin();itclass!=(*itins).end();itclass++) {
			for (itatt=itclass->second.begin();itatt!=itclass->second.end();itatt++) {
				if (itatt->second!=kifNULL)
					itatt->second->Resetreference();
			}
		}
	}
	if (reference<=0)
		values.clear();
}

void KifMatrix::Clear() {
	vector<map<string,map<string,KifElement*> > >::iterator itins;
	map<string,map<string,KifElement*> >::iterator itclass;
	map<string, KifElement*>::iterator itatt;
	for (itins=values.begin();itins!=values.end();itins++) {
		for (itclass=(*itins).begin();itclass!=(*itins).end();itclass++) {
			for (itatt=itclass->second.begin();itatt!=itclass->second.end();itatt++) {
				if (itatt->second!=kifNULL)
					itatt->second->Resetreference();
			}
		}
	}
	values.clear();
}


void KifVector::Resetreference() {
	vector<KifElement*>::iterator itx;
	for (itx=values.begin();itx!=values.end();itx++)		
		(*itx)->Resetreference();

	reference--;
	if (reference<=0)
		values.clear();
}


void KifVector::Clear() {
	vector<KifElement*>::iterator itx;	
	for (itx=values.begin();itx!=values.end();itx++) {
		int nb=reference;
		while (nb>0) {
			(*itx)->Resetreference();
			nb--;
		}
	}
	values.clear();
}

void KifMap::Resetreference() {
	
	map<string,KifElement*>::iterator itx;
	for (itx=values.begin();itx!=values.end();itx++) {
		if (itx->second!=NULL)
			itx->second->Resetreference();
	}
	
	reference--;
	if (reference<=0)
		values.clear();
}

void KifMap::Clear() {
	map<string,KifElement*>::iterator itx;
	for (itx=values.begin();itx!=values.end();itx++) {
		int nb=reference;
		while (nb>0) {
			itx->second->Resetreference();
			nb--;
		}
	}		
	values.clear();
}

void KifDomain::Clean() {
	reference--;
	map<string,KifElement*>::iterator it;
	for (it=declarations.begin();it!=declarations.end();it++) {
		if (it->second!=NULL)
			it->second->Clean();
	}	
	declarations.clear();
}

void KifDomain::Clear() {
	reference--;
	map<string,KifElement*>::iterator it;
	for (it=declarations.begin();it!=declarations.end();it++) {
		if (it->second!=NULL)
			it->second->Resetreference();
	}
	declarations.clear();
}

void KifDomain::Resetreference() {
	reference--;
	map<string,KifElement*>::iterator it;
	for (it=declarations.begin();it!=declarations.end();it++) {
		if (it->second!=NULL)
			it->second->Resetreference();
	}
	if (reference<=0)
		declarations.clear();
}

bool KifDerivation(KifElement* frame,KifElement* derive,bool typetest) {
	if (frame->type!=kifFrame || derive->type!=kifFrame)
		return typetest;

	KifFrame* derivation=(KifFrame*)derive;
	while (derivation!=NULL) {
		if (frame==derivation)
			return true;
		derivation=derivation->parent;
	}
	return false;
}


bool KifDomain::Setvalue(KifElement* ke,KifElement* idx,KifElement* dom) {

	if (frame==kifNULL || KifDerivation(frame,ke->Frame(),false)==false) {
		kifCodeVector[idkifcode]->Returnerror(kifErrorStrings[155]);
		return true;
	}
	Clear();
	ke->SetInstances(this,NULL);
	return true;
}

bool KifCall::Setvalue(KifElement* val,KifElement* idx,KifElement* dom) {
	if (val->type!=kifFunction) {
		kifCodeVector[idkifcode]->Returnerror(kifErrorStrings[156]);
		return true;
	}
	body=(KifFunction*)val;
	domain=(KifDomain*)dom;
	return true;
}

//===================================================================
string KifInteger::String() {
	ostringstream is;
	is<<Value();
	return is.str();
}

string KifFloat::String() {
	ostringstream is;
	is<<Value();
	return is.str();
}

string KifTime::String() {
	struct tm* temps = localtime(&value); 
	char strtime[100];
	strftime(strtime,100,"%Y/%m/%d %H:%M:%S",temps);
	return strtime;
}

//===================================================================
xmlNodePtr KifXMLNodePtr::Node() {
	if (value==NULL)
		return NULL;
	return value->noeud;
}

//===================================================================

KifElement* KifXMLNodePtr::same(KifElement* a) {
	if (a==kifNULL) {
		if (value==NULL || value->noeud==NULL)
			return kifTRUE;
		return kifFALSE;
	}
	if (a->type!=kifXMLNodePtr)
		return kifFALSE;
	KifXMLNodePtr* kx=(KifXMLNodePtr*)a;
	if (kx->value->noeud==value->noeud)
		return kifTRUE;
	return kifFALSE;
}



KifElement* KifXMLNode::same(KifElement* a) {
	if (a==kifNULL) {
		if (value==NULL)
			return kifTRUE;
		return kifFALSE;
	}
	if (a->type!=kifXMLNode)
		return kifFALSE;
	KifXMLNode* kx=(KifXMLNode*)a;
	if (kx->value==value)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifXipNode::less(KifElement* a) {
	if (a==kifNULL || value==NULL)
		return kifFALSE;
	if (a->type!=kifXipNode)
		return kifFALSE;
	KifXipNode* kx=(KifXipNode*)a;
	if (kx->value==NULL)
		return kifFALSE;
	if (value->num_droite < kx->value->num_gauche)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifXipNode::more(KifElement* a) {
	if (a==kifNULL || value==NULL)
		return kifFALSE;
	if (a->type!=kifXipNode)
		return kifFALSE;
	KifXipNode* kx=(KifXipNode*)a;
	if (kx->value==NULL)
		return kifFALSE;
	if (value->num_droite > kx->value->num_gauche)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifXipNode::lessequal(KifElement* a) {
	if (a==kifNULL || value==NULL)
		return kifFALSE;
	if (a->type!=kifXipNode)
		return kifFALSE;
	KifXipNode* kx=(KifXipNode*)a;
	if (kx->value==NULL)
		return kifFALSE;
	if (value->num_droite <= kx->value->num_gauche)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifXipNode::moreequal(KifElement* a) {
	if (a==kifNULL || value==NULL)
		return kifFALSE;
	if (a->type!=kifXipNode)
		return kifFALSE;
	KifXipNode* kx=(KifXipNode*)a;
	if (kx->value==NULL)
		return kifFALSE;
	if (value->num_droite >= kx->value->num_gauche)
		return kifTRUE;
	return kifFALSE;
}
//===================================================================
void KifFrame::Run() {
	size_t i;
	KifCode* kcode=kifCodeVector[idkifcode];
	kcode->basegarbage=kcode->garbage.size();
	kcode->trigger+=kcode->basegarbage;
	string n="_args";
	KifVariableDeclaration* var=(KifVariableDeclaration*)kcode->kstart->Declaration(n);
	KifVector* vargs=(KifVector*)var->value;
	for (i=0;i<kcode->arguments.size();i++)
		vargs->Push(new KifString(kcode,NULL,kcode->arguments[i]));

	//We look for the first instruction to execute
	for (i=6;i<instructions.size();i++) {
		KifType xt=instructions[i]->type;
		KifElement* ke=instructions[i];
		if (ke->Variable() || xt==kifInstruction || xt==kifCallFunction || xt==kifInstance || xt==kifXPathExpression) {
			kifCodeVector[idkifcode]->Debugging(this,ke,this,NULL);
			ke->Execute(kifNULL,this,kifNULL,false);
		}
	}
}
//===================================================================

KifElement* KifIteratorXipRule::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	if (reverse==false) {
		if (it==values->end())
			return kifNULL;
		return ExecuteRule(*it,pattern,dom);
	}
	if (itr==values->rend())
		return kifNULL;
	return ExecuteRule(*itr,pattern,dom);
}

KifElement* KifIteratorVector::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	if (reverse==false) {
		if (it==values->end())
			return kifNULL;
		return (*it)->Execute(pattern,dom,val,rcouple);
	}
	if (itr==values->rend())
		return kifNULL;
	return (*itr)->Execute(pattern,dom,val,rcouple);
}

KifElement* KifIteratorMap::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	if (reverse==false) {
		if (it==values->end())
			return kifNULL;
		return it->second->Execute(pattern,dom,val,rcouple);
	}
	if (itr==values->rend())
		return kifNULL;
	return itr->second->Execute(pattern,dom,val,rcouple);
}


KifElement* KifXipRule::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	if (kifCodeVector[idkifcode]->parseur==NULL)
		return kifNULL;

	if (pattern->type==kifXipRule)
		return this;
    //First we set our rule list to the current element
	if (values.size()==0)
		return kifFALSE;

	VECTA<VectTestFonction*> deductionkif;
	vector<VectTestFonction*>::iterator it;
	for (it=values.begin();it!=values.end();it++)
		deductionkif.ajoute(*it);

	kifCodeVector[idkifcode]->parseur->DeductionDeKif=&deductionkif;
	return ApplyRules(pattern,dom);
}

KifElement* KifXipRule::ApplyRules(KifElement* pattern,KifDomain* dom) {
	if (kifCodeVector[idkifcode]->parseur==NULL)
		return kifNULL;

	KifCallFunction* callfunc=(KifCallFunction*)dom;

	if (pattern->type==kifVector && kifCodeVector[idkifcode]->insidecall==0)
		kifCodeVector[idkifcode]->parseur->FONCTIONSKIF=new VECTA< ResultatFonction*>;

	KifObject kifobject(NULL,NULL); 
	for (int i=1;i<callfunc->parameters.size();i++)
		kifobject.Add(callfunc->parameters[i]->Execute(pattern,NULL,kifNULL,false));

	double res=0;
	kifCodeVector[idkifcode]->insidecall++;
	VECTA<VectTestFonction*>* anciendeduction=kifCodeVector[idkifcode]->parseur->DeductionDeKif;
	KifObject* ancien=kifCodeVector[idkifcode]->parseur->kifparametres;

	kifCodeVector[idkifcode]->parseur->kifparametres=&kifobject;

	if (script)
		res=kifCodeVector[idkifcode]->parseur->DeduitFonction(-1,DEDUCTIONKIFSCRIPT,NON,-1,NULL,NULL,-1);
	else
		res=kifCodeVector[idkifcode]->parseur->DeduitFonction(-1,DEDUCTIONKIF,NON,-1,NULL,NULL,-1);

	kifCodeVector[idkifcode]->parseur->kifparametres=ancien;
	kifCodeVector[idkifcode]->parseur->DeductionDeKif=anciendeduction;
	kifCodeVector[idkifcode]->insidecall--;
	
	if (pattern->type==kifVector && kifCodeVector[idkifcode]->insidecall==0) {
		for (int i=0;i<kifCodeVector[idkifcode]->parseur->FONCTIONSKIF->size();i++) {
			KifXipDependency* dep=new KifXipDependency(kifCodeVector[idkifcode],NULL,kifCodeVector[idkifcode]->parseur->FONCTIONSKIF->vecteur[i]);
			pattern->Push(dep);
		}

		delete kifCodeVector[idkifcode]->parseur->FONCTIONSKIF;
		kifCodeVector[idkifcode]->parseur->FONCTIONSKIF=NULL;
		return pattern;
	}

	if (res!=0)
		return kifTRUE;
	return kifFALSE;
}

KifElement* KifThis::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {

	return kifCodeVector[idkifcode]->Getframe(name);
}

KifElement* KifIndex::Execute(KifElement* pattern,KifDomain* dom,KifElement* ke,bool rcouple) {
	KifCouple couple(NULL,ke,this,dom);
	return couple.Execute(pattern,dom,this,false);
}

KifElement* KifCouple::Execute(KifElement* pattern,KifDomain* dom,KifElement* ke,bool rcouple) {
	KifElement* kres=recipient->Execute(pattern,domain,indexes,rcouple);
	if (indexes->function==NULL)
		return kres;
	KifIndex* kidx=indexes;
	if (kidx->function->type==kifIndex) {
		//Double indexes...
		kidx=(KifIndex*)indexes->function;
		while (kidx!=NULL) {
			//In this case, what we try to do is first to get the element with this index
			kres=kres->Execute(kifNULL,domain,kidx,false);		
			if (kidx->function!=NULL && kidx->function->type==kifIndex)
				kidx=(KifIndex*)kidx->function;
			else
				break;
		}
	}
	if (kidx->function!=NULL) {
		if (kres->type==kifDomain)
			kres=kidx->function->Execute(pattern,(KifDomain*)kres,kres,rcouple);
		else
			kres=kidx->function->Execute(pattern,domain,kres,rcouple);
	}
	return kres;
}

KifElement* KifXPathExpression::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	if (kifCodeVector[idkifcode]->parseur==NULL)
		return kifNULL;

	xmlresultat resxmlnode;
	static VECTA<Noeud*> vectNoeuds;
	static VECTA<ResultatFonction*> fonctions;
	VECTA<uchar*>* lesClefs;

	KifElement* ret=kifTRUE;
	resxmlnode.raz();
	if (kifCodeVector[idkifcode]->parseur->ExecuteXML(txml->clefxml,vectNoeuds,fonctions,&resxmlnode) == txml->negation)                
		ret=kifFALSE;            

	if (resxmlnode.dernier()>0) {
		lesClefs=new VECTA<uchar*>;
		for (int iar=0;iar<resxmlnode.dernier();iar++) {                    
			if (txml->clefxml->AnalyseClefNodeXML(kifCodeVector[idkifcode]->parseur,resxmlnode[iar],
				resxmlnode.index,resxmlnode.arboxml,*lesClefs,vectNoeuds,fonctions)) {
					break;
			}
		}
		delete lesClefs;
	}            

	return ret;
}

KifElement* KifString::Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple) {
	if (idx==NULL || idx==kifNULL || pattern==idx)
		return this;

	KifElement* ke;
	KifElement* left=idx;
	KifElement* right=NULL;
	if (idx->type==kifIndex) {
		KifIndex* kind=(KifIndex*)idx;		
		left=kind->key->Execute(kifNULL,dom,kifNULL,false);
		if (kind->interval==true)
			right=kind->right->Execute(kifNULL,dom,kifNULL,false);
	}

	string v=Value();
	string sc;		
	int i;

	vector<string> lw;
	conversionUTF8VersGras(v,lw);

	int ikey=left->Integer();
	if (ikey<0)
		ikey=Value().size()+ikey;
	
	if (ikey<0 || ikey>=Value().size())
		return kifNULL;

	if (right==NULL) {				
		if (ikey>=lw.size())
			return kifNULL;
		ke=new KifString(kifCodeVector[idkifcode],NULL,lw[ikey]);
		return ke;
	}

	int iright=right->Integer();
	if (iright<=0)
		iright=Value().size()+iright;
	else 
		if (iright>=lw.size())
			iright=lw.size();

	if ((iright-ikey)<=0)
		return kifNULL;
	
	string res;
	for (i=ikey;i<iright;i++)
		res+=lw[i];

	return new KifString(kifCodeVector[idkifcode],NULL,res);
}




KifElement* KifMap::Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple) {

	if (idx==NULL || idx==kifNULL) {
		KifElement* kv;
		//particular case, the pattern is a vector, which means that we expect a set of keys
		//as a result
		if (pattern->type==kifVector) {
			KifVector* vect=new KifVector(kifCodeVector[idkifcode],NULL);
			map<string,KifElement*>::iterator it;
			for (it=values.begin();it!=values.end();it++) {
				string n=it->first;
				KifElement* x=new KifString(kifCodeVector[idkifcode],vect,n);
				x->Setreference();
				vect->values.push_back(x);
			}			
			return vect;
		}

		if (pattern->type==kifInteger || pattern->type==kifFloat) {
			kv=new KifInteger(kifCodeVector[idkifcode],NULL,values.size());
			return kv;

		}
		//Then some value must be replaced with their evaluation
		if (evaluate==true) {
			KifMap* kmap=new KifMap(kifCodeVector[idkifcode],NULL);
			map<string,KifElement*>::iterator it;
			for (it=values.begin();it!=values.end();it++) {
				string n=it->first;
				KifElement* ke=it->second;
				kv=ke->Execute(kifNULL,(KifDomain*)dom,kifNULL,false);
				if (kv->reference==1)
					kv=kv->Copy(dom);
				kv->Setreference();
				kmap->values[n]=kv;
			}
			return kmap;
		}
		return this;
	}

	KifElement* key;
	if (idx->type==kifIndex) {
		KifIndex* kind=(KifIndex*)idx;		
		if (kind->interval==true)
			return kifNULL;
		key=kind->key->Execute(kifNULL,dom,kifNULL,false);
	}
	else
		key=idx->Execute(pattern,dom,kifNULL,false);

	if (key==kifNULL)
		return kifNULL;

	string skey=key->String();
	KifElement* kval=Values(skey);
	if (kval==NULL)
		return kifNULL;
	return kval;
}

KifElement* KifMapPtr::Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple) {

	if (idx==NULL || idx==kifNULL) {
		KifElement* kv;
		//particular case, the pattern is a vector, which means that we expect a set of keys
		//as a result
		if (pattern->type==kifVector) {
			KifVector* vect=new KifVector(kifCodeVector[idkifcode],NULL);
			map<string,string>::iterator it;
			for (it=values->begin();it!=values->end();it++) {
				string n=it->first;
				KifElement* x=new KifString(kifCodeVector[idkifcode],vect,n);
				x->Setreference();
				vect->values.push_back(x);
			}			
			return vect;
		}

		if (pattern->type==kifInteger || pattern->type==kifFloat) {
			kv=new KifInteger(kifCodeVector[idkifcode],NULL,values->size());
			return kv;
		}
		return this;
	}

	KifElement* key;
	if (idx->type==kifIndex) {
		KifIndex* kind=(KifIndex*)idx;		
		if (kind->interval==true)
			return kifNULL;
		key=kind->key->Execute(kifNULL,dom,kifNULL,false);
	}
	else
		key=idx->Execute(pattern,dom,kifNULL,false);

	if (key==kifNULL)
		return kifNULL;
	string skey=key->String();
	KifElement* kval=new KifString(kifCodeVector[idkifcode],this,(*values)[skey]);
	if (kval==NULL)
		return kifNULL;
	return kval;
}

KifElement* KifVector::Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple) {
	if (idx==NULL || idx==kifNULL) {

		KifElement* ke;
		//In this case, we copy the elements from the vector to the map, using the position as index
		if (pattern->type==kifMap) {
			KifMap* map=new KifMap(kifCodeVector[idkifcode],this);
			size_t i=0;
			char buff[20];
			
			vector<KifElement*>::iterator it;
			for (it=values.begin();it!=values.end();it++) {
				sprintf_s(buff,20,"%d",i);
				ke=*it;
				ke->Setreference();
				map->values[buff]=ke;
				i++;
			}			
			return map;
		}

		if (pattern->type==kifInteger || pattern->type==kifFloat) {
			KifElement* kv=new KifInteger(kifCodeVector[idkifcode],NULL,values.size());
			return kv;
		}
		//Then some value must be replaced with their evaluation
		if (evaluate==true) {
			KifVector* kvect=new KifVector(kifCodeVector[idkifcode],NULL);
			vector<KifElement*>::iterator it;
			for (it=values.begin();it!=values.end();it++) {
				ke=*it;
				KifElement* kv=ke->Execute(kifNULL,(KifDomain*)dom,kifNULL,false);
				if (kv->reference==1)
					kv=kv->Copy(dom);
				kvect->Push(kv);
			}
			return kvect;
		}

		return this;
	}
	
	KifElement* key;
	KifElement* keyright=NULL;
	if (idx->type==kifIndex) {
		KifIndex* kind=(KifIndex*)idx;		
		key=kind->key->Execute(kifNULL,dom,kifNULL,false);
		if (kind->interval==true)
			keyright=kind->right->Execute(kifNULL,dom,kifNULL,false);
	}
	else
		key=idx->Execute(pattern,dom,kifNULL,false);

	if (key==kifNULL)
		return kifNULL;

	int ikey=key->Integer();
	if (ikey<0)
		ikey=values.size()+ikey;
	
	if (ikey<0 || ikey>=values.size())
		return kifNULL;

	if (keyright==NULL) 
		return values[ikey];

	int iright=keyright->Integer();
	if (iright<=0)
		iright=values.size()+iright;
	else 
		if (iright>=values.size())
			iright=values.size();
	//In this case, we must create a new vector
	KifVector* kvect=new KifVector(kifCodeVector[idkifcode],NULL);
	for (int i=ikey;i<iright;i++)
		kvect->Push(values[i]);
	return kvect;
}

KifElement* KifVectorNumberPtr::Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple) {
	if (idx==NULL || idx==kifNULL) {

		//In this case, we copy the elements from the vector to the map, using the position as index
		if (pattern->type==kifMap) {
			KifMap* map=new KifMap(kifCodeVector[idkifcode],this);
			char buff[20];
			for (size_t i=0;i<values->size();i++) {
				sprintf_s(buff,20,"%d",i);
				KifElement* ke=new KifFloat(kifCodeVector[idkifcode],this,(*values)[i]);
				ke->Setreference();
				map->values[buff]=ke;
			}			
			return map;
		}
		
		if (pattern->type==kifInteger || pattern->type==kifFloat) {
			KifElement* kv=new KifInteger(kifCodeVector[idkifcode],NULL,values->size());
			return kv;
		}
		return this;
	}

	KifElement* key;
	KifElement* keyright=NULL;
	if (idx->type==kifIndex) {
		KifIndex* kind=(KifIndex*)idx;		
		key=kind->key->Execute(kifNULL,dom,kifNULL,false);
		if (kind->interval==true)
			keyright=kind->right->Execute(kifNULL,dom,kifNULL,false);
	}
	else
		key=idx->Execute(pattern,dom,kifNULL,false);

	if (key==kifNULL)
		return kifNULL;

	int ikey=key->Integer();
	if (ikey<0)
		ikey=values->size()+ikey;
	
	if (ikey<0 || ikey>=values->size())
		return kifNULL;

	if (keyright==NULL)
		return new KifFloat(kifCodeVector[idkifcode],NULL,(*values)[ikey]);

	int iright=keyright->Integer();
	if (iright<=0)
		iright=values->size()+iright;
	else 
		if (iright>=values->size())
			iright=values->size();
	//In this case, we must create a new vector
	KifVector* kvect=new KifVector(kifCodeVector[idkifcode],NULL);
	for (int i=ikey;i<iright;i++)
		kvect->Push(new KifFloat(kifCodeVector[idkifcode],NULL,(*values)[i]));
	return kvect;
}

KifElement* KifVectorStringPtr::Execute(KifElement* pattern,KifDomain* dom,KifElement* idx,bool rcouple) {
	if (idx==NULL || idx==kifNULL) {

		//In this case, we copy the elements from the vector to the map, using the position as index
		if (pattern->type==kifMap) {
			KifMap* map=new KifMap(kifCodeVector[idkifcode],this);
			char buff[20];
			for (size_t i=0;i<values->size();i++) {
				sprintf_s(buff,20,"%d",i);
				KifElement* ke=new KifString(kifCodeVector[idkifcode],this,*(*values)[i]);
				ke->Setreference();
				map->values[buff]=ke;
			}			
			return map;
		}

		if (pattern->type==kifInteger || pattern->type==kifFloat) {
			KifElement* kv=new KifInteger(kifCodeVector[idkifcode],NULL,values->size());
			return kv;
		}
		return this;
	}
	
	KifElement* key;
	KifElement* keyright=NULL;
	if (idx->type==kifIndex) {
		KifIndex* kind=(KifIndex*)idx;		
		key=kind->key->Execute(kifNULL,dom,kifNULL,false);
		if (kind->interval==true)
			keyright=kind->right->Execute(kifNULL,dom,kifNULL,false);
	}
	else
		key=idx->Execute(pattern,dom,kifNULL,false);

	if (key==kifNULL)
		return kifNULL;

	int ikey=key->Integer();
	if (ikey<0)
		ikey=values->size()+ikey;
	
	if (ikey<0 || ikey>=values->size())
		return kifNULL;

	if (keyright==NULL) {
		if ((*values)[ikey]==NULL)
			return kifNULL;
		return new KifString(kifCodeVector[idkifcode],NULL,*(*values)[ikey]);
	}

	int iright=keyright->Integer();
	if (iright<=0)
		iright=values->size()+iright;
	else 
		if (iright>=values->size())
			iright=values->size();
	//In this case, we must create a new vector
	KifVector* kvect=new KifVector(kifCodeVector[idkifcode],NULL);
	for (int i=ikey;i<iright;i++) {
		if ((*values)[ikey]!=NULL)
			kvect->Push(new KifString(kifCodeVector[idkifcode],NULL,*(*values)[i]));
	}
	return kvect;}


KifElement* KifDomain::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	if (pattern==NULL)
		return kifNULL;
	string n=kifStringType[pattern->type];
	if (n=="self")
		return this;
	if (n=="")
		return kifNULL;
	return Framedefined(n,pattern,this);
}

KifElement* KifVariable::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	return value;
}


KifElement* KifVariableDeclaration::Copy(KifDomain* dom,KifElement* vartype) {
	KifElement* ke=value->Copy(NULL,vartype);
	//We look for the initial function
	ke->applyinitial(dom,initialisation);

	if (ke->Basic()) {
		KifVariable* kvar=new KifVariable(kifCodeVector[idkifcode],NULL,ke,name);
		ke->reference++;
		ke=kvar;
	}

	dom->Set(name,ke,NULL);

	if (value->type!=kifFrame) {
		if (initialisation!=kifNULL)
			initialisation->Execute(value,dom,ke,false);
	}
	return ke;
}

KifElement* KifVariableDeclaration::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	dom->Disableadding();

	if (type==kifVariableDeclaration) {
		if (value->type!=kifFrame) {
			if (initialisation!=kifNULL)
				initialisation->Execute(value,dom,value,false);
			Setreference();
			return value;
		}
	}
	KifElement* kdom=Copy(dom,this);
	return kdom;
}


KifElement* KifFunction::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	
	KifElement* returnval=kifNULL;
	KifElement* res;
	size_t i;
	map<string,KifElement*> stacks;
	bool testdebug=kifCodeVector[idkifcode]->debugging;

	KifElement* ke;
	KifCallFunction* callfunc=(KifCallFunction*)kifCodeVector[idkifcode]->Top();
	kifCodeVector[idkifcode]->Debugging(this,this,dom,callfunc);
	for (i=0;i<instructions.size();i++) {
		ke=instructions[i];		
		kifCodeVector[idkifcode]->currentline=ke->Line();
		if (ke->Variable()) {
			string thename=ke->Name();
			//If this variable has already been declared
			if (stacks.find(thename)!=stacks.end()) {
				string message=kifErrorStrings[157]+thename+" has already been declared in "+name;
				kifCodeVector[idkifcode]->Returnerror(message);
				break;
			}
			stacks[thename]=callfunc->Declaration(thename);
			if (testdebug)
				testdebug=kifCodeVector[idkifcode]->Debugging(this,ke,dom,callfunc);
			res=ke->Execute(pattern,callfunc,val,false);
		}
		else {
			KifType xt=ke->type;

			if (xt==kifInstruction || xt==kifCallFunction || xt==kifInstance || xt==kifXPathExpression) {
				if (testdebug)
					testdebug=kifCodeVector[idkifcode]->Debugging(this,ke,dom,callfunc);
				ke->Execute(pattern,dom,val,false);			
				if (callfunc!=NULL && callfunc->Isreturned()) {
					returnval=callfunc->returnvalue;
					break;				
				}
			}
		}
	}
	callfunc->Resetdeclaration(stacks);
	return returnval;
}


KifElement* KifError::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	kifCodeVector[idkifcode]->currentline=Line();
	KifCallFunction* callfunc=(KifCallFunction*)kifCodeVector[idkifcode]->Top();
	if (callfunc!=NULL)
		callfunc->Setreturnvalue(this);
	return this;
}

KifElement* KifCall::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	kifCodeVector[idkifcode]->currentline=Line();
	string na="_call";
	KifCallFunction kfunc(NULL,this,na,body);
	kfunc.idkifcode=idkifcode;
	//We add our parameters
	KifCallFunction* callfunc=(KifCallFunction*)dom;
	for (int i=1;i<callfunc->parameters.size();i++)
		kfunc.parameters.push_back(callfunc->parameters[i]);
	return kfunc.Execute(kifNULL,domain,kifNULL,false);	
}


KifElement* KifCallFunction::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {

	KifElement* ke;	

	map<string,KifElement*> stacksparam;
	KifCallFunction* callfunc=(KifCallFunction*)kifCodeVector[idkifcode]->Top();
	KifElement* oldpattern=environment;
	environment=pattern;

	kifCodeVector[idkifcode]->currentline=Line();
	//For predefined functions, we use the existing bodies
	if (body!=NULL && body->type==kifPredefined) {
		Initialisation();		
		KifPredefined* kpred=(KifPredefined*)body;
		if (selfinsert==true) {
			KifSelf kbloc(NULL,NULL,val->Returnvalue());
			parameters[0]=&kbloc;
			ke=kpred->Execute(pattern,dom,val,false,this);
		}
		else
			ke=kpred->Execute(pattern,dom,val,false,this);
		ke=applyfunction(ke);
		Resetdeclaration(stacksparam);
		if (Isreturned())
			callfunc->Setreturnvalue(returnvalue);
		environment=oldpattern;
		return ke;
	}

	KifFunction* bd=body;
	string message;
	if (bd==NULL) {
		//Then we are into a vector or a map..., we need to find out which 
		//function to use
		ke=dom->Declaration(name);
		if (ke==NULL || ke->type!=kifFunction) {
			message=kifErrorStrings[178];
			message+="'";
			message+=name;			
			message+= "' in object of type '";			
			if (dom->Type()=="")
				message+=val->Type();
			else
				message+=dom->Type();
			message+="'";
			ke=kifCodeVector[idkifcode]->Returnerror(message);
			return ke;
		}
		bd=(KifFunction*)ke;
	}

	//We set the parameters
	if (bd->arguments.size()!=parameters.size()) {
		message=kifErrorStrings[158];
		message+=name;
		ke=kifCodeVector[idkifcode]->Returnerror(message);
		return kifNULL;
	}


	size_t i;
	bool err=false;
	KifElement* karg;
	KifElement* newarg;
	//The parameters are expressions in most encompassing frame, not the local one...
	KifDomain* anciendom=kifCodeVector[idkifcode]->Beforetop();
	
	for (i=0;i<bd->arguments.size();i++) {
		//We expect some arguments in the function declaration
		KifElement* arg=bd->arguments[i];
		//the name of this argument
		//We then compute our parameter
		string na=arg->Name();		
		KifElement* thetype=arg->Typevariable();
		karg=parameters[i]->Execute(thetype,anciendom,kifNULL,false);
		//self is always transparent
		if (thetype->type==kifSelf || karg==kifNULL)
			Set(na,karg,&stacksparam);
		else {//Simple is: Float, integer, string or boolean
			if (thetype->Simple()) {
				Set(na,kifNULL,&stacksparam);
				newarg=arg->Copy(this,arg);
				newarg->Setvalue(karg);
			}
			else {
				//In this case, the bridges from one structure to another are rather limited...
				if (thetype->type==kifFrame && karg->type==kifDomain) {
					//if a frame is a subframe of another frame
					if (KifDerivation(thetype,karg->Frame(),true)==false) {
						err=true;
						break;
					}
				}
				else {
					//otherwise, it is always an error...
					if (thetype->type!=karg->type) {
						err=true;
						break;
					}
				}

				Set(na,karg,&stacksparam);
			}
		}		
	}

	if (err==false) {
		kifCodeVector[bd->idkifcode]->Push(dom);
		kifCodeVector[bd->idkifcode]->Push(this);
		ke=bd->Execute(pattern,dom,val,false);
		ke=applyfunction(ke);
		kifCodeVector[bd->idkifcode]->Pop();
		kifCodeVector[bd->idkifcode]->Pop();
	}
	else {
		message=kifErrorStrings[159];
		message+=name;
		ke=kifCodeVector[idkifcode]->Returnerror(message);
	}

	environment=oldpattern;
	Resetdeclaration(stacksparam);
	Initialisation();
	if (pattern==kifNULL && kifCodeVector[idkifcode]->Trigger())
		kifCodeVector[idkifcode]->Clean();
	return ke;
}


KifElement* KifInstance::Execute(KifElement* pattern,KifDomain* dom,KifElement* val,bool rcouple) {
	KifElement* v=NULL;
	kifCodeVector[idkifcode]->currentline=Line();

	if (value->type==kifCallFunction) {
		v=kifCodeVector[idkifcode]->Get(name,dom);
		if (v==NULL)
			return kifNULL;

		v=v->Returnvalue();
		if (((KifCallFunction*)value)->selfinsert==true) {
			//In this case, we use the value as the value itself
			if (v->Typevariable()!=kifNULL)
				v=v->Typevariable();
			v=value->Execute(pattern,dom,v,false);
			return v;
		}

		
		//We detect the sub frame declarations
		KifCallFunction* bd=(KifCallFunction*)value;
		string nn=value->Name();
		KifElement* decl=v->Declaration(nn);
		if (decl==NULL) {
			string mess=kifErrorStrings[183]+nn;
			kifCodeVector[idkifcode]->Returnerror(mess);
			return kifNULL;
		}
			
		KifFunction* oldbody=bd->body;
		if (bd->body!=decl && decl->type==kifFunction)
			bd->body=(KifFunction*)decl;
		decl=bd->Execute(pattern,(KifDomain*)v,val,false);
		bd->body=oldbody;
		return decl;
	}
	if (value->type!=kifInstance) {
		//A Global variable, we simply return its value
		if (declaration->type==kifVariableDeclaration) {
			if (declaration->value->type!=kifFrame) {
				if (index!=NULL) {
					v=new KifCouple(kifCodeVector[idkifcode],declaration->value->Returnvalue(),index,dom);
					if (rcouple)
						return v;
					else
						return v->Execute(pattern,dom,index,false);
				}
				if (self==true || pattern==kifNULL || pattern->Type()==declaration->value->Type()) {
					if (rcouple==false)
						return declaration->value->Returnvalue();
					return declaration;
				}
				return declaration->value->Execute(pattern,dom,index,false);
			}
			else {
				//In this particular case, the domain is the variable itself, as it is a frame already
				v=kifCodeVector[idkifcode]->Get(name,dom);
				if (v==NULL || v->isDomain()==false) {
					string mess=kifErrorStrings[183]+name;
					kifCodeVector[idkifcode]->Returnerror(mess);
					return kifNULL;
				}
				dom=(KifDomain*)v;
			}
		}
		else
			//otherwise, we need to find the right value
			v=kifCodeVector[idkifcode]->Get(name,dom);

		if (v==NULL) {
			string mess=kifErrorStrings[183]+name;
			kifCodeVector[idkifcode]->Returnerror(mess);
			return kifNULL;
		}

		if (index!=NULL) {
			v=new KifCouple(kifCodeVector[idkifcode],v->Returnvalue(),index,dom);
			if (rcouple)
				return v;
			else
				return v->Execute(pattern,dom,index,false);
		}

		if (self==true || pattern==kifNULL  || pattern->Type()==v->Type()) {
			if (rcouple==false)
				return v->Returnvalue();
			return v;
		}

		return v->Execute(pattern,dom,index,false);
	}
	//Else, we need to find, the right occurence in our dom
	v=kifCodeVector[idkifcode]->Get(name,dom);
	v=v->Returnvalue();
	if (v==NULL || v->isDomain()==false) {
		string mess=kifErrorStrings[183]+name;
		kifCodeVector[idkifcode]->Returnerror(mess);
		return kifNULL;
	}
	KifDomain* newdom=(KifDomain*)v;
	return value->Execute(pattern,newdom,val,false);
}


KifElement* KifInstruction::LoopExecute(KifElement* pattern,KifDomain* dom,KifElement* ke) {
	kifCodeVector[idkifcode]->currentline=Line();
	KifElement* result=instructions[0]->Execute(kifTRUE,dom,kifNULL,false);
	if (result->Boolean()==false)
		return kifNULL;
	
	KifCallFunction* callfunc=(KifCallFunction*)kifCodeVector[idkifcode]->Top();
	bool continuation=false;
	KifElement* res=kifNULL;
	
	while (result->Boolean()==true) {
		res=instructions[1]->Execute(pattern,dom,kifNULL,false);		
		if (callfunc!=NULL && callfunc->Isreturned())
			return callfunc->returnvalue;

		if (res==kifBREAK)
			return kifTRUE;

		result=instructions[0]->Execute(kifTRUE,dom,kifNULL,false);
		if (kifCodeVector.vecteur[0]->stacking.size()==0 && kifCodeVector[idkifcode]->Trigger())
			kifCodeVector[idkifcode]->Clean();

	}
	
	return kifTRUE;
}

KifElement* KifInstruction::DisjoinedExecute(KifElement* pattern,KifDomain* dom,KifElement* ke) {
	kifCodeVector[idkifcode]->currentline=Line();
	KifElement* ret=kifTRUE;
	KifCallFunction* callfunc=(KifCallFunction*)kifCodeVector[idkifcode]->Top();
	for (size_t i=0;i<instructions.size();i++) {
		kifCodeVector[idkifcode]->Debugging(this,instructions[i],dom,callfunc);
		KifElement* res=instructions[i]->Execute(pattern,dom,kifNULL,false);
		if (callfunc!=NULL && callfunc->Isreturned())
			return callfunc->returnvalue;

		if (res==kifBREAK || res==kifCONTINUE)
			return res;
	}

	return kifTRUE;
}

KifElement* KifInstruction::TryCatch(KifElement* pattern,KifDomain* dom,KifElement* ke) {
	kifCodeVector[idkifcode]->currentline=Line();
	KifElement* ret=kifTRUE;
	KifCallFunction* callfunc=(KifCallFunction*)kifCodeVector[idkifcode]->Top();
	int last=instructions.size()-1;
	for (size_t i=0;i<last;i++) {
		kifCodeVector[idkifcode]->Debugging(this,instructions[i],dom,callfunc);
		KifElement* res=instructions[i]->Execute(pattern,dom,kifNULL,false);
		if (kifCodeVector[idkifcode]->error==true) {
			instructions[last]->Execute(pattern,dom,kifNULL,false);
			return kifFALSE;
		}
		if (callfunc!=NULL && callfunc->Isreturned())
			return callfunc->returnvalue;

		if (res==kifBREAK || res==kifCONTINUE)
			return res;
	}
	return kifTRUE;
}

KifElement* KifInstruction::ForExecute(KifElement* pattern,KifDomain* dom,KifElement* ke) {
	kifCodeVector[idkifcode]->currentline=Line();
	//Initialisation
	instructions[0]->Execute(kifTRUE,dom,kifNULL,false);
	//then test
	KifElement* test=instructions[1]->Execute(kifTRUE,dom,kifNULL,false);
	KifCallFunction* callfunc=(KifCallFunction*)kifCodeVector[idkifcode]->Top();

	while (test->Boolean()==true) {
		//then the body		
		KifElement* res=instructions[3]->Execute(pattern,dom,kifNULL,false);
		if (callfunc!=NULL && callfunc->Isreturned())
			return callfunc->returnvalue;

		if (res==kifBREAK)
			return kifTRUE;

		//then the increment
		instructions[2]->Execute(pattern,dom,kifNULL,false);
		//then the test again
		test=instructions[1]->Execute(kifTRUE,dom,kifNULL,false);
		if (kifCodeVector.vecteur[0]->stacking.size()==0 && kifCodeVector[idkifcode]->Trigger())
			kifCodeVector[idkifcode]->Clean();
	}

	return kifTRUE;
}


KifElement* KifInstruction::Execute(KifElement* pattern,KifDomain* dom,KifElement* ke,bool rcouple) {
	KifElement* var;
	KifElement* val;
	KifElement* result;
	KifSelf* self;
	size_t i;
	bool neg=false;	
	
	kifCodeVector[idkifcode]->currentline=Line();
	if (kifCodeVector[idkifcode]->error==true)
		return kifNULL;

	if (pattern==NULL)
		pattern=ke;
	
	if (instructions.size()==0) {
		if (action==kifNEGATION)
			return kifNEG;
		return kifNULL;
	}
	switch(action) {
		case kifINITIALISATION:
			//first, we get the target element where to store our result
			var=ke;
			val=var->Returnvalue();

			if (instructions[0]->type==kifSelf)
				kifCodeVector[idkifcode]->Returnerror(kifErrorStrings[160]);

			result=instructions[0]->Execute(val,dom,kifNULL,false);

			if (instructions.size()==2)
				result=instructions[1]->Execute(val,dom,result,false);
			
			var->Setvalue(result,NULL,dom);
			return var;
		case kifAFFECTATION:
		case kifPLUSEQU:
		case kifMINUSEQU:
		case kifMULTIPLYEQU:
		case kifDIVIDEEQU:
		case kifMODEQU:
		case kifPOWEREQU:
		case kifUNIONEQU:
		case kifINTERSECTIONEQU:
		case kifSHIFTLEFTEQU:
		case kifSHIFTRIGHTEQU:
			var=instructions[0]->Execute(kifNULL,dom,kifNULL,true);
			//If we do not have it, we stop
			if (var==kifNULL || var==NULL)
				return kifNULL;
			val=var->Returnvalue();
			result=instructions[1]->Execute(val,dom,kifNULL,false);
			if (instructions.size()==3)
				result=instructions[2]->Execute(val,dom,result,false);
			switch(action) {
				case kifPLUSEQU:
					val->plus(val,result);
					return var;
				case kifMINUSEQU:
					val->minus(val,result);
					return var;
				case kifMULTIPLYEQU:
					val->multiply(val,result);
					return var;
				case kifDIVIDEEQU:
					val->divide(val,result);
					return var;
				case kifMODEQU:
					val->mod(val,result);
					return var;
				case kifPOWEREQU:
					val->power(val,result);
					return var;
				case kifUNIONEQU:
					val->orset(val,result);
					return var;
				case kifINTERSECTIONEQU:
					val->andset(val,result);
					return var;
				case kifSHIFTLEFTEQU:
					val->shiftleft(val,result);
					return var;
				case kifSHIFTRIGHTEQU:
					val->shiftright(val,result);
					return var;
			}

			if (var->Setvalue(result,NULL,dom)==false) {
				//then we must call the initialisation function
				self=(KifSelf*)instructions[1];
				val->applyinitial(dom,self->parameters);
			}
			return var;
		case kifCHAINING://A bloc in parentheses
			result=instructions[0]->Execute(pattern,dom,ke,false);
			for (i=1;i<instructions.size();i++)
				result=instructions[i]->Execute(result,dom,result,false);
			return result;
		case kifBLOCBOOLEAN://A bloc in parentheses
			result=instructions[0]->Execute(pattern,dom,ke,false);
			if (result==kifNEG) {
				neg=true;
				result=kifNULL;
			}
			for (i=1;i<instructions.size();i++)
				result=instructions[i]->Execute(pattern,dom,result,false);
			if (neg) {
				if (result->Boolean()==false || result==kifNULL)
					return kifTRUE;
				return kifFALSE;
			}
			return result;
		case kifPARAMETER://A parameter
			result=instructions[0]->Execute(pattern,dom,ke,rcouple);
			for (i=1;i<instructions.size();i++)
				result=instructions[i]->Execute(result,dom,result,false);
			return result;
		case kifINTERSECTION:
			val=instructions[0]->Execute(pattern,dom,kifNULL,false);
			for (i=1;i<instructions.size();i++) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				val=pattern->andset(val,result);
			}
			return val;
		case kifUNION:
			val=instructions[0]->Execute(pattern,dom,kifNULL,false);
			for (i=1;i<instructions.size();i++) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				val=pattern->orset(val,result);
			}
			return val;
		case kifPLUS:
			val=instructions[0]->Execute(pattern,dom,kifNULL,false);
			for (i=1;i<instructions.size();i++) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				val=pattern->plus(val,result);
			}
			return val;
		case kifMINUS:
			val=instructions[0]->Execute(pattern,dom,kifNULL,false);
			for (i=1;i<instructions.size();i++) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				val=pattern->minus(val,result);
			}
			return val;
		case kifMULTIPLY:
			val=instructions[0]->Execute(pattern,dom,kifNULL,false);
			for (i=1;i<instructions.size();i++) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				val=pattern->multiply(val,result);
			}
			return val;
		case kifDIVIDE:
			val=instructions[0]->Execute(pattern,dom,kifNULL,false);
			for (i=1;i<instructions.size();i++) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				val=pattern->divide(val,result);
			}
			return val;
		case kifMOD:
			val=instructions[0]->Execute(pattern,dom,kifNULL,false);
			for (i=1;i<instructions.size();i++) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				val=pattern->mod(val,result);
			}
			return val;
		case kifIN:
			result=instructions[0]->Execute(kifNULL,dom,kifNULL);
			if (instructions.size()==2) {
				var=instructions[1];
				return pattern->in(ke,result,(KifDomain*)var,pattern,false);
			}
			return pattern->in(ke,result,NULL,pattern,false);
		case kifPOWER:
			val=instructions[0]->Execute(pattern,dom,kifNULL,false);
			for (i=1;i<instructions.size();i++) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				val=pattern->power(val,result);
			}
			return val;
		case kifSHIFTRIGHT:
			val=instructions[0]->Execute(pattern,dom,kifNULL,false);
			for (i=1;i<instructions.size();i++) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				val=pattern->shiftright(val,result);
			}
			return val;
		case kifSHIFTLEFT:
			val=instructions[0]->Execute(pattern,dom,kifNULL,false);
			for (i=1;i<instructions.size();i++) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				val=pattern->shiftleft(val,result);
			}
			return val;
		case kifEQUAL:
			if (ke==NULL)
				return kifFALSE;
			result=instructions[0]->Execute(pattern,dom,kifNULL,false);
			if (instructions.size()==2)
				result=instructions[1]->Execute(pattern,dom,result,false);
			return ke->same(result);			
		case kifSUPERIOR:
			if (ke==NULL)
				return kifFALSE;
			result=instructions[0]->Execute(pattern,dom,kifNULL,false);
			if (instructions.size()==2)
				result=instructions[1]->Execute(pattern,dom,result,false);
			return ke->more(result);			
		case kifINFERIOR:
			if (ke==NULL)
				return kifFALSE;
			result=instructions[0]->Execute(pattern,dom,kifNULL,false);
			if (instructions.size()==2)
				result=instructions[1]->Execute(pattern,dom,result,false);
			return ke->less(result);			
		case kifSUPEQU:
			if (ke==NULL)
				return kifFALSE;
			result=instructions[0]->Execute(pattern,dom,kifNULL,false);
			if (instructions.size()==2)
				result=instructions[1]->Execute(pattern,dom,result,false);
			return ke->moreequal(result);			
		case kifINFEQU:
			if (ke==NULL)
				return kifFALSE;
			result=instructions[0]->Execute(pattern,dom,kifNULL,false);
			if (instructions.size()==2)
				result=instructions[1]->Execute(pattern,dom,result,false);
			return ke->lessequal(result);			
		case kifDIFFERENT:
			if (ke==NULL)
				return kifFALSE;
			result=instructions[0]->Execute(pattern,dom,kifNULL,false);
			if (instructions.size()==2)
				result=instructions[1]->Execute(pattern,dom,result,false);
			return ke->different(result);						
		case kifOR:
			for (i=0;i<instructions.size();i+=1) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				if (result->Boolean()==true)
					return kifTRUE;
			}
			return kifFALSE;
		case kifAND:
			for (i=0;i<instructions.size();i+=1) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				if (result->Boolean()==false)
					return kifFALSE;
			}
			return kifTRUE;
		case kifSWITCH:
			//First our variable
			var=instructions[0]->Execute(kifTRUE,dom,kifNULL,false);
			//Then the instructions, the odd element is the value to compare with, the even element
			//the instruction to execute.
			for (i=1;i<instructions.size();i+=2) {
				result=instructions[i]->Execute(pattern,dom,kifNULL,false);
				if (result==kifDEFAULT || var->same(result)->Boolean()==true)
					return instructions[i+1]->Execute(pattern,dom,kifNULL,false);
			}
			return kifNULL;
		case kifTRY:
			return TryCatch(pattern,dom,ke);
		case kifIF:
			result=instructions[0]->Execute(kifTRUE,dom,kifNULL,false);
			if (result->Boolean()==true || result->Integer()!=0)
				return instructions[1]->Execute(pattern,dom,ke,false);
			else
				if (instructions.size()==3)
					return instructions[2]->Execute(pattern,dom,ke,false);
			return kifNULL;
		case kifWHILE:
			return LoopExecute(pattern,dom,ke);
		case kifFOR:
			return ForExecute(pattern,dom,ke);
		case kifDISJOINEDBLOC:
			return DisjoinedExecute(pattern,dom,ke);
	}
	return kifNULL;
}


//===================================================================
KifElement* KifCode::KifParseNegation(x_node* xn,KifElement* kf) {
	KifElement* ke=new KifInstruction(this,kf,kifNEGATION);	
	ke->Setline(linereference+bnfkif->x_linenumber(xn->start));
	return ke;
}


KifElement* KifCode::KifParseBoolean(x_node* xn,KifElement* kf) {
	KifElement* ke=kf;
	//In the case that we are parsing a long Boolean expression
	bool newelement=false;
	
	if (kf->Action()!=kifOR && kf->Action()!=kifAND)
		newelement=true;
	else if (xn->nodes.size()==3)		
		//Then the Boolean operator is on nodes[1]
		if (kifOperators[xn->nodes[1]->value]!=kf->Action())
			newelement=true;
	if (newelement==true) {
		ke=new KifInstruction(this,kf,kifBLOCBOOLEAN);
		ke->Setline(linereference+bnfkif->x_linenumber(xn->start));
	}
	KifElement* kbloc=new KifInstruction(this,ke,kifBLOCBOOLEAN); 
	kbloc->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	KifBrowse(xn->nodes[0],kbloc);
	for (size_t i=1;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],ke);
	return ke;
}

KifElement* KifCode::KifParseTestSwitch(x_node* xn,KifElement* kf) {
	if (xn->nodes[0]->token=="default")
		kf->Add(kifDEFAULT);
	else
		KifBrowse(xn->nodes[0],kf);
	KifElement* ktrue=new KifInstruction(this,kf,kifDISJOINEDBLOC);
	ktrue->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	KifBrowse(xn->nodes[1],ktrue);
	return ktrue;
}

KifElement* KifCode::KifParseSwitch(x_node* xn,KifElement* kf) {
	//We create a IF section
	KifElement* kswitch=new KifInstruction(this,kf,kifSWITCH);
	kswitch->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	KifBrowse(xn->nodes[0],kswitch);
	for (int i=1;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],kswitch);
	return kswitch;
}

KifElement* KifCode::KifParseTryCatch(x_node* xn,KifElement* kf) {
	KifElement* ke=new KifInstruction(this,kf,kifTRY);
	ke->Setline(linereference+bnfkif->x_linenumber(xn->start));
	string name;
	KifElement* declaration;
	KifInstance* ki;

	long line=bnfkif->x_linenumber(xn->start);

	if (xn->nodes.size()!=1) {
		name=xn->nodes[1]->value;
		declaration=Getbasic(name,kf);

		if (declaration==NULL || declaration->XIP()) {
			string message="Unknown variable or XIP declared variable: "+name;			
			throw new KifRaiseError(message,current->name,line,xn->end);
		}

		if (!declaration->Variable()) {
			if (xn->nodes.size()==1 && (kifAllTypes[name]==true || declaration->type==kifFunction)) {
				KifElement* ke=new KifSelf(this,kf,declaration);
				ke->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
				return ke;
			}
			string message="Unknown variable:"+name;
			throw new KifRaiseError(message,current->name,line,xn->end);
		}

		KifInstruction* kaff=new KifInstruction(this,ke,kifAFFECTATION);
		kaff->Setline(linereference+bnfkif->x_linenumber(xn->start));
		ki=new KifInstance(this,kaff,name,(KifVariableDeclaration*)declaration);
		kaff->Add(kifNULL);
	}

	KifBrowse(xn->nodes[0],ke);

	
	string na="catch";
	KifElement* kfunc=kifBaseFunctions["catch"];
	KifCallFunction* kcf=new KifCallFunction(this,ke,na,(KifFunction*)kfunc);
	kcf->Setline(linereference+bnfkif->x_linenumber(xn->start));

	if (xn->nodes.size()!=1) {
		ki=new KifInstance(this,kcf,name,(KifVariableDeclaration*)declaration);
		ki->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	}
	kcf->add=0;
	return ke;
}

KifElement* KifCode::KifParseTest(x_node* xn,KifElement* kf) {
	KifElement* ke=new KifInstruction(this,kf,kifIF);
	ke->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	KifBrowse(xn->nodes[0],ke);
	KifElement* ktrue=new KifInstruction(this,ke,kifDISJOINEDBLOC);
	ktrue->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	KifBrowse(xn->nodes[1],ktrue);
	KifElement* kfalse;
	
	if (xn->nodes.size()==3) {
		kfalse=new KifInstruction(this,ke,kifDISJOINEDBLOC);
		kfalse->Setline(linereference+bnfkif->x_linenumber(xn->nodes[2]->start));
		KifBrowse(xn->nodes[2],kfalse);
	}

	return ke;
}

KifElement* KifCode::KifParseLoop(x_node* xn,KifElement* kf) {
	KifElement* ke=new KifInstruction(this,kf,kifWHILE);
	ke->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	KifBrowse(xn->nodes[0],ke);
	KifElement* ktrue=new KifInstruction(this,ke,kifDISJOINEDBLOC);
	ktrue->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	KifBrowse(xn->nodes[1],ktrue);
	return ke;
}



KifElement* KifCode::KifParseFor(x_node* xn,KifElement* kf) {
	KifElement* ke=new KifInstruction(this,kf,kifFOR);
	ke->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	//Initialisation
	KifBrowse(xn->nodes[0],ke);
	//Test
	KifBrowse(xn->nodes[1],ke);
	//Increment
	KifBrowse(xn->nodes[2],ke);
	KifElement* kbloc=new KifInstruction(this,ke,kifDISJOINEDBLOC);
	kbloc->Setline(linereference+bnfkif->x_linenumber(xn->nodes[3]->start));
	//Instruction
	KifBrowse(xn->nodes[3],kbloc);
	return ke;
}

//The parameter list is a two nodes tree, where the second node is always a parameter
//hence a recursive analysis
void KifCode::ComputeParameters(x_node* xn,KifCallFunction* kcf) {
	KifElement* kbloc=new KifInstruction(this,kcf,kifPARAMETER);
	kbloc->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	KifBrowse(xn->nodes[0],kbloc);

	if (xn->nodes.size()==2)
		ComputeParameters(xn->nodes[1],kcf);
}

KifElement* KifCode::KifParseUniqueCall(x_node* xn,KifElement* kf) {
	string& name=xn->value;
	//Looking if it is known as function
	
	KifElement* kfunc=kifBaseFunctions[name];
		
	KifCallFunction* kcf=new KifCallFunction(this,kf,name,(KifFunction*)kfunc);
	kcf->Setline(linereference+bnfkif->x_linenumber(xn->start));
	kf->Initvalue(kcf);	
	kcf->add=0;
	return kcf;
}


KifElement* KifCode::KifParseCall(x_node* xn,KifElement* kf) {
	string& name=xn->nodes[0]->value;
	bool basictype=false;
	bool getbasefunction=false;

	//Particular case, where the function is a predefined function linked to an instance
	if (KifBaseFunctionTest(name)!=NULL) {
		if(kf->type==kifInstance)
			basictype=true;
		else
			if (kf->type==kifIndex || kf->type==kifCallFunction) {
				basictype=true;
				getbasefunction=true;
			}
			else
				getbasefunction=true;
	}
	

	//Looking if it is known as a function
	KifElement* kfunc=Getbasic(name,kf);
	if (kifAllTypes[name]==true && kfunc!=NULL && kfunc->type==kifFrame) {
		KifSelf* ke=new KifSelf(this,kf,kfunc);
		ke->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
		KifInstruction* ki=new KifInstruction(this,ke,kifINITIALISATION);
		ki->Setline(ke->Line());
		KifBrowse(xn->nodes[1],ki);
		ke->parameters=ki;
		return ke;
	}
	if (kfunc==NULL && getbasefunction==true)
		kfunc=KifBaseFunctionTest(name);
	
	if (kfunc==NULL) {
		//Then in this case, the only possible situation are: self, function or index
		if ((kf->type!=kifIndex && kf->type!=kifCallFunction) || KifDeclaredFunctionTest(name)==false) {
			long line=bnfkif->x_linenumber(xn->start);
			string message="Unknown function: '"+name+"' in '"+kf->Name();
			throw new KifRaiseError(message,current->name,line,xn->end);
		}

	}

	if (kfunc==kifTRUE)
		kfunc=NULL;
	
	//Particular case

	KifCallFunction* kcf=new KifCallFunction(this,kf,name,(KifFunction*)kfunc);
	kcf->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	
	//We check again to see if a specific function name was not overloaded
	if (basictype==true && kfunc!=NULL && kfunc->type==kifPredefined) {
		kcf->selfinsert=true;
		kcf->parameters.push_back(kifNULL);
	}

	kf->Initvalue(kcf);	
	if (xn->nodes.size()>=2) {
		int last=1;
		if (xn->nodes[last]->token=="parameters") {
			ComputeParameters(xn->nodes[last],kcf);
			last++;
		}
		if (xn->nodes.size()>last) {
			if (xn->nodes[last]->token=="indexes" || xn->nodes[last]->token=="regularcall") {
				kcf->add=2;
				KifBrowse(xn->nodes[last],kcf);
			}
		}
	}
	kcf->add=0;
	return kcf;
}

KifElement* KifCode::KifParseIndexes(x_node* xn,KifElement* kf) {
	KifElement* ki=new KifIndex(this,kf,false);
	ki->Setline(linereference+bnfkif->x_linenumber(xn->start));
	kf->Initvalue(ki);
	KifInstruction* kidx=new KifInstruction(this,ki,kifCHAINING);
	kidx->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	KifBrowse(xn->nodes[0],kidx);
	if (xn->nodes.size()==2)
		KifBrowse(xn->nodes[1],ki);
	return ki;
}

KifElement* KifCode::KifParseInterval(x_node* xn,KifElement* kf) {
	string ty=kf->Type();
	long line=bnfkif->x_linenumber(xn->start);
	//if (ty!="vector" && ty!="string") {
	//	string message="Wrong interval for this variable:"+kf->Name()+" type is="+ty;
	//	throw new KifRaiseError(message,current->name,line,xn->end);
	//}

	int xsz=xn->nodes.size();

	if (xsz==1) {
		string message="Wrong interval";
		throw new KifRaiseError(message,current->name,line,xn->end);
	}


	KifIndex* ki=new KifIndex(this,kf,true);
	ki->Setline(linereference+bnfkif->x_linenumber(xn->start));
	kf->Initvalue(ki);


	if (xn->nodes[xsz-1]->token=="regularcall" || xn->nodes[xsz-1]->token=="indexes")
		xsz--;
	
	KifInstruction* kidx;
	//Three cases
	if (xsz==3) {
		//We have the first value, then the separator then the other value
		kidx=new KifInstruction(this,ki,kifCHAINING);
		kidx->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
		KifBrowse(xn->nodes[0],kidx);
		kidx=new KifInstruction(this,ki,kifCHAINING);
		kidx->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
		KifBrowse(xn->nodes[2],kidx);
	}
	else
		if (xn->nodes[0]->token=="sep") {
			//then the first parameter is not known
			ki->key=kifNULL;
			kidx=new KifInstruction(this,ki,kifCHAINING);
			kidx->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
			KifBrowse(xn->nodes[1],kidx);
		}
		else {//The last parameter is not known
			kidx=new KifInstruction(this,ki,kifCHAINING);
			kidx->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
			KifBrowse(xn->nodes[0],kidx);
			ki->right=kifNULL;
		}
	
	//If we have a call
	if (xsz!=xn->nodes.size()) {
		ki->interval=false;
		KifBrowse(xn->nodes[xsz],ki);
		ki->interval=true;
	}

	return ki;
}

KifElement* KifCode::KifParseOperator(x_node* xn,KifElement* kf) {
	string op=xn->value;
	kf->Set(op,kf,NULL);
	return kf;
}

KifElement* KifCode::KifParseBloc(x_node* xn,KifElement* kf) {
	KifInstruction* ki=new KifInstruction(this,kf,kifCHAINING);
	ki->Setline(linereference+bnfkif->x_linenumber(xn->start));
	for (size_t i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],ki);
	return ki;	
}

KifElement* KifCode::KifParseComparison(x_node* xn,KifElement* kf) {
	//The first parameter is the operator
	KifElement* ki=kf;
	if (kf->Action()!=kifOperators[xn->nodes[0]->value])
		ki=new KifInstruction(this,kf);
	ki->Setline(linereference+bnfkif->x_linenumber(xn->start));
	for (size_t i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],ki);
	return ki;
}


KifElement* KifCode::KifParseOperation(x_node* xn,KifElement* kf) {
	//The first parameter is the operator
	KifElement* ki=kf;
	KifOperators op=kifOperators[xn->nodes[0]->value];
	if (kf->Action()!=op) {
		ki=NULL;
		KifInstruction* kki=new KifInstruction(this,NULL);
		int i;
		KifInstruction* kkf;			
		if (kifOperatorMath[kf->Action()]) {			
			//In this specific case, we create one level up to the top
			if (op==kifPLUS || op==kifMINUS) {				
				while (kf->Parent()!=NULL && kifOperatorMath[kf->Parent()->Action()])
					kf=kf->Parent();
				kkf=(KifInstruction*)kf->Parent();
				kki->parent=kkf;
				for (i=0;i<kkf->instructions.size();i++) {
					if (kkf->instructions[i]==kf)
						break;
				}

				kkf->instructions[i]=kki;
				kki->Add(kf);
				kkf=(KifInstruction*)kf;
				kkf->parent=kki;
				ki=kki;
			}
		}
		if (ki==NULL) {
			//in this case, we need to move the current value to where it belongs
			//whatever the new operation
			//We replace the last value of the current node, with new operation, moving this last element
			//as the first element of this new operation
			kki->parent=kf;
			kkf=(KifInstruction*)kf;
			i=kkf->instructions.size()-1;
			kki->Add(kkf->instructions[i]);
			kkf->instructions[i]=kki;
			ki=kki;
		}
	}
	ki->Setline(linereference+bnfkif->x_linenumber(xn->start));
	for (size_t i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],ki);
	return ki;
}

KifElement* KifCode::KifParseOperationIn(x_node* xn,KifElement* kf) {
	//The first parameter is the operator
	KifInstruction* ki=new KifInstruction(this,kf);
	ki->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	ki->action=kifIN;
	KifBrowse(xn->nodes[0],ki);
	if (xn->nodes.size()==2) {
		KifElement* kfunc=NULL;
		string name=xn->nodes[1]->value;
		if (KifDeclaredFunctionTest(name)==true)
			kfunc=Getbasic(name,kf);
		//We have a WITH description
		if (kfunc==NULL) {
			string message="Unknown function:"+name;
			long line=bnfkif->x_linenumber(xn->start);
			throw new KifRaiseError(message,current->name,line,xn->end);
		}

		ki->Add(kfunc);
	}
	return ki;
}


//A variable is a complex structure involving: expression, crochets
KifElement* KifCode::KifParseVariable(x_node* xn,KifElement* kf) {
	string& name=xn->nodes[0]->value;
	KifElement* declaration=NULL;
	
	//specific case, this is itself...
	if (name=="this") {
		kf->Add(new KifThis(this,kf));
		return kf;
	}

	declaration=Getbasic(name,kf);
	long line=bnfkif->x_linenumber(xn->start);
	
	if (declaration==NULL) {
		string lname=c_Tolower(name);
		declaration=Getbasic(lname,kf);
		if (declaration==NULL || declaration->XIP()==false) {
			string message="Unknown variable:"+name;
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
	}

	if (!declaration->Variable()) {
		if (xn->nodes.size()==1 && (kifAllTypes[name]==true || declaration->type==kifFunction)) {
			KifElement* ke=new KifSelf(this,kf,declaration);
			ke->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
			return ke;
		}
		if (declaration->type!=kifVariable) {
			string message="Unknown variable:"+name;
			throw new KifRaiseError(message,current->name,line,xn->end);
		}
	}

	KifInstance* ki=new KifInstance(this,kf,name,(KifVariableDeclaration*)declaration);
	ki->Setline(linereference+bnfkif->x_linenumber(xn->nodes[0]->start));
	kf->Initvalue(ki);
	KifElement* ret=NULL;
	for (size_t i=1;i<xn->nodes.size();i++) {
		KifElement* retloc=KifBrowse(xn->nodes[i],ki);
		if (retloc!=NULL)
			ret=retloc;
	}
	if (ret==NULL)
		ki->self=true;
	return ki;
}

KifElement* KifCode::KifParseAffectation(x_node* xn,KifElement* kf) {
	KifInstruction* ki=new KifInstruction(this,kf,kifAFFECTATION);
	ki->Setline(linereference+bnfkif->x_linenumber(xn->start));
	for (size_t i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],ki);
	return ki;
}



KifElement* KifCode::KifParseANumber(x_node* xn ,KifElement* kf) {
	string& name=xn->value;
	KifElement* kv;
	if (name.find(".")==string::npos)
		kv=new KifInteger(this,kf,atoi(STR(name)));	
	else
		kv=new KifFloat(this,kf,atof(STR(name)));
	kv->Setline(linereference+bnfkif->x_linenumber(xn->start));
	return kv;
}

KifElement* KifCode::KifParseABoolean(x_node* xn,KifElement* kf) {
	if (xn->value=="true") {
		kf->Add(kifTRUE);
		return kifTRUE;
	}
	kf->Add(kifFALSE);
	return kifFALSE;
}

KifElement* KifCode::KifParseAString(x_node* xn ,KifElement* kf) {
	string name="";
	size_t i;
	if (xn->token=="astringdouble") {
		for (i=0;i<xn->nodes.size();i++) {
			if (xn->nodes[i]->value[0]=='\\')
				name+=KifMetaCharacters(xn->nodes[i]->value);
			else
				name+=xn->nodes[i]->value;
		}
	}
	else {
		for (i=0;i<xn->nodes.size();i++)
				name+=xn->nodes[i]->value;
	}

	KifString* kv=new KifString(this,kf,name);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->start));
	return kv;
}

KifElement* KifCode::CreateVariable(x_node* xn,string& name,KifElement* kf) {
	KifElement* ktop=Top();
	KifElement* var;
	//If it is a global variable, instance is unique
	if (ktop==NULL)
		var=new KifVariableDeclaration(this,kf,name);
	else
		//Otherwise, we keep a track of that specific frame
		if (ktop->type==kifFrame)
			var=new KifFrameDeclaration(this,kf,name,ktop);
		else
			var=new KifFunctionDeclaration(this,kf,name,ktop);

	var->Setline(linereference+bnfkif->x_linenumber(xn->start));
	if (xn->nodes.size()==3) {
		KifInstruction* ki=new KifInstruction(this,var,kifINITIALISATION);
		ki->Setline(linereference+bnfkif->x_linenumber(xn->nodes[2]->start));
		var->Setinitialisation(ki);
		KifBrowse(xn->nodes[2],ki);		
	}

	return var;
}

KifElement* KifCode::KifParseVarUserDefined(x_node* xn ,KifElement* kf) {
	string& type=xn->nodes[0]->value;
	string& name=xn->nodes[1]->value;
	KifElement* ke=Getbasic(type,kf);
	if (ke==NULL) {
		string message="Unknown type:"+type;
		long line=bnfkif->x_linenumber(xn->start);
		throw new KifRaiseError(message,current->name,line,xn->end);
	}
	KifElement* var=CreateVariable(xn,name,kf);
	((KifVariableDeclaration*)var)->value=ke;
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarSelf(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifSelf* kv=new KifSelf(this,var,var);	
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarCall(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifCall* kv=new KifCall(this,var,NULL);	
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarTime(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifTime* kv=new KifTime(this,var);	
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}



KifElement* KifCode::KifParseVarInt(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifInteger* kv=new KifInteger(this,var);	
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}
#ifdef NTMCONNECTION
KifElement* KifCode::KifParseVarFST(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifFST* kv=new KifFST(this,var);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}
#endif

KifElement* KifCode::KifParseVarParser(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifParser* kv=new KifParser(this,var);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}
KifElement* KifCode::KifParseVarKifKiF(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifKiF* kv=new KifKiF(this,var);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarString(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifString* kv=new KifString(this,var);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarFloat(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifFloat* kv=new KifFloat(this,var);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}
KifElement* KifCode::KifParseVarBoolean(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifBoolean* kv=new KifBoolean(this,var);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}
KifElement* KifCode::KifParseVarFile(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifFile* kv=new KifFile(this,var);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}
KifElement* KifCode::KifParseVarVector(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifVector* kv=new KifVector(this,var);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarMap(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifMap* kv=new KifMap(this,var);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarMatrix(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifMatrix* kv=new KifMatrix(this,var,name);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarXML(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifXMLNode* kv=new KifXMLNode(this,var,NULL);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarIterator(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifIterator* kv=new KifIterator(this,var,false);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarRIterator(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifIterator* kv=new KifIterator(this,var,true);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseXipRule(x_node* xn ,KifElement* kf) {
	istringstream is(xn->value);
	bool script=false;
	if (xn->nodes[1]->token=="xipbloc") {
		parseur->LitLesDeductions(is,DEDUCTIONKIFSCRIPT);
		script=true;
	}
	else
		parseur->LitLesDeductions(is,DEDUCTIONKIF);
	KifXipRule* kv=new KifXipRule(this,kf,parseur->DeductionDansKif,script);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->start));
	parseur->DeductionDansKif.raz();
	return kv;
}

KifElement* KifCode::KifParseVarRule(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifXipRule* kv=new KifXipRule(this,var,parseur->DeductionDansKif,false);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarNode(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifXipNode* kv=new KifXipNode(this,var,NULL);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarGraph(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifGraph* kv=new KifGraph(this,var,NULL);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarDependency(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifXipDependency* kv=new KifXipDependency(this,var,NULL);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}

KifElement* KifCode::KifParseVarGeneration(x_node* xn ,KifElement* kf) {
	string& name=xn->nodes[1]->value;
	KifElement* var=CreateVariable(xn,name,kf);
	KifXipGeneration* kv=new KifXipGeneration(this,var,NULL);
	kv->Setline(linereference+bnfkif->x_linenumber(xn->nodes[1]->start));
	kf->Set(name,var,NULL);
	return var;
}


KifElement* KifCode::KifParseDeclaration(x_node* xn,KifElement* kf) {
	string& type=xn->nodes[0]->value;
	KifElement* element=NULL;
	KifParseElement kpe=kifTypes[type];
	if (kpe!=NULL)		
		element=(this->*kpe)(xn,kf);
	else
		KifParseVarUserDefined(xn,kf);
	return element;
}

KifElement* KifCode::KifParseList(x_node* xn,KifElement* kf) {
	KifInstruction* kbloc=new KifInstruction(this,kf,kifCHAINING);
	KifElement* ke=KifBrowse(xn->nodes[0],kbloc);
	if (xn->nodes.size()==2)
		KifBrowse(xn->nodes[1],kf);
	return kbloc;
}


KifElement* KifCode::KifParseValVector(x_node* xn,KifElement* kf) {
	KifVector* kvect=new KifVector(this,kf);
	kvect->evaluate=true;
	for (int i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],kvect);
	return kvect;
}

KifElement* KifCode::KifParseDico(x_node* xn,KifElement* kf) {
	KifElement* key=KifBrowse(xn->nodes[0],kf);
	KifInstruction* kbloc=new KifInstruction(this,kf,kifCHAINING);
	KifElement* val=KifBrowse(xn->nodes[1],kbloc);
	
	KifMap* kmap=(KifMap*)kf;
	kmap->values[key->String()]=val;
	val->reference++;
	if (xn->nodes.size()==3)
		KifBrowse(xn->nodes[2],kf);
	return kf;
}


KifElement* KifCode::KifParseValMap(x_node* xn,KifElement* kf) {
	KifMap* kmap=new KifMap(this,kf);
	kmap->evaluate=true;
	for (int i=0;i<xn->nodes.size();i++)
		KifBrowse(xn->nodes[i],kmap);
	return kmap;	
}


//Function creation
KifElement* KifCode::KifParseFunction(x_node* xn,KifElement* kf) {
	string& name=xn->nodes[0]->value;
	long line=bnfkif->x_linenumber(xn->start);
	if (KifBaseFunctionTest(name)!=NULL && KifConversion(name)==false) {		
		string message="Error: Predefined function name can not be declared into a frame, consider choosing another name:"+name;
		throw new KifRaiseError(message,current->name,line,xn->end);
	}
	KifDeclaredFunction[name]=true;
	KifFunction* kfunc=NULL;
	KifElement* ke;
	ke=kf->Declaration(name);
	if (ke==NULL)
		kfunc=new KifFunction(this,kf,name);
	else {
		if (ke->type==kifFunction) {
			kfunc=(KifFunction*)ke;
			if (kfunc->choice!=2)
				kfunc=NULL;
		}
	}

	if (kfunc==NULL) {
		string message="Error: This function has already been declared:"+name;
		throw new KifRaiseError(message,current->name,line,xn->end);
	}

	Push(kfunc);
	kfunc->Setline(linereference+bnfkif->x_linenumber(xn->start+1));
	if (xn->nodes[1]->token=="declarationending")
		kfunc->choice=2;
	else
		if (xn->nodes[1]->token=="instruction") {
			kfunc->choice=1;
			KifBrowse(xn->nodes[1],kfunc);
		}
		else {
			KifBrowse(xn->nodes[1],kfunc);
			kfunc->choice=1;
			KifBrowse(xn->nodes[2],kfunc);
		}
	kf->Set(name,kfunc,NULL);
	Pop();
	return kfunc;
}

KifElement* KifCode::KifParseXPath(x_node* xn,KifElement* kf) {
	char k='@';
	char tampon[1000];
	char neg=NON;
	static VECTA<char> variables_utilisees;
	variables_utilisees.raz();

	tampon[0]=0;
	string s=xn->value.substr(1,xn->value.length()-1);
	s+=";";
	istringstream is(s);
	clefXML* clexml=parseur->ParseXPathExpression(is,k,tampon,neg,variables_utilisees);            
	parseur->enAttente[0]=0;

	TestFonctionXML* tfxml=new TestFonctionXML;
	tfxml->negation=neg;
	tfxml->clefxml=clexml;
	parseur->StockeTestFonction(tfxml);
	KifXPathExpression* kxpath=new KifXPathExpression(this,kf,tfxml);
	kxpath->Setline(linereference+bnfkif->x_linenumber(xn->start));
	return kxpath;
}



KifElement* KifCode::KifParseFrame(x_node* xn,KifElement* kf) {
	//We create a frame
	//The name is the next parameter
	string name=xn->nodes[0]->value;
	KifFrame* kframe=NULL;
	KifElement* ke;
	ke=kf->Declaration(name);

	if (ke==NULL)
		kframe=new KifFrame(this,kf,name);
	else {
		if (ke->type==kifFrame) {
			kframe=(KifFrame*)ke;
			if (kframe->declared==false)
				kframe=NULL;
		}
	}
	if (kframe==NULL) {
		long line=bnfkif->x_linenumber(xn->start);
		string message="Error: This frame has already been declared:"+name;
		throw new KifRaiseError(message,current->name,line,xn->end);
	}

	if (xn->nodes[1]->token=="declarationending") {
		kframe->declared=true;
		kf->Set(kframe->name,kframe,NULL);
		return kframe;
	}


	Push(kframe);
	kframe->Setline(linereference+bnfkif->x_linenumber(xn->start));

	//If it is a sub-frame definition
	if (kf->type==kifFrame && kstart!=kf)
		//We copy all our declarations in it
		//These declarations, will be replaced by local ones if necessary
		kf->Sharedeclaration(kframe);
	//We then record this new Frame in our instructions list
	kifAllTypes[kframe->name]=true;
	//We also store it at the TOP level, so that others can have access to it...
	kstart->Set(kframe->name,kframe,NULL);
	KifBrowse(xn->nodes[1],kframe);
	Pop();
	return kframe;
}


KifElement* KifCode::KifBrowse(x_node* xn,KifElement* kf) {
    if (xn==NULL)
        return NULL;

	KifElement* ke=NULL;
    if (kifFunctions.find(xn->token)!=kifFunctions.end()) {
		return (this->*kifFunctions[xn->token])(xn,kf);
	}
	else {
		for (size_t i=0;i<xn->nodes.size();i++) {
			KifElement* s=KifBrowse(xn->nodes[i],kf);
			if (s!=NULL)
				ke=s;
		}
	}
	return ke;
}

KifCode::KifCode(GlobalParseur* p,int trig) {	
#ifdef _DEBUG
	currentkifcode=this;
#endif
	char localpath[1024];
#ifdef WIN32
	_fullpath(localpath,".",1024);
#else
	realpath(".",localpath);
#endif

	trigger=trig;
	garbage.ajuste(trigger);
	inittableutf8();
	id=kifCodeVector.size();
	kifCodeVector.ajoute(this);
	bnfkif=new bnf_kif;
	linereference=0;
	gotoline=-10;
	debug=false;	
	debugging=false;
	currentline=0;
	current=NULL;
	basegarbage=0;
	errorptr=NULL;
	error=false;
	cursor=0;
	parseur=p;
	os=&cout;
	insidecall=0;
	utf8=false;
	if (p!=NULL) {
		if (p->OS!=NULL)
			os=p->OS;
		utf8=p->UTF8();
	}
	kstart=new KifFrame(this,NULL,"%%START");
	string n="_args";
	KifVariableDeclaration* args=new KifVariableDeclaration(this,NULL,n);
	KifVector* vargs=new KifVector(this,args);
	kstart->Set(n,args,NULL);
	n="_paths";
	args=new KifVariableDeclaration(this,NULL,n);
	vargs=new KifVector(this,args);
	kstart->Set(n,args,NULL);
	vargs->Push(new KifString(this,NULL,localpath));
	n="_sep";
	args=new KifVariableDeclaration(this,NULL,n);
	n=SEP;
	KifString* sep=new KifString(this,args,n);
	n="_sep";
	kstart->Set(n,args,NULL);
	KifInitFunctions(this,kstart);
}

void KifCode::Loadfile(string filename) {
	x_readfile xf(STR(filename));
	Load(&xf);
}


void KifCode::Loadfile(ifstream& fichier,string filename,long pos) {
	x_readfile xf(&fichier,STR(filename),pos);
	Load(&xf);
	ifstream f(STR(filename));
	string s;
	tabledebugfilename[linereference]=filename;
	long l=1;
	while (!f.eof()) {
		getline(f,s);
		if (s!="") {
			s=Trim(s);
			if (s!="") {
				tabledebug[linereference+1]=s;
				tabledebugline[linereference+1]=l;
			}
		}
		linereference+=1;
		l+=1;
	}
}

KifElement* KifCode::Evaluate(KifElement* kf,string sbase) {
	string s="kif:\n";
	s+=sbase;

	x_readstring xr(s);
	current=&xr;	
	bnfkif->baseline=0;
	x_node* xn=bnfkif->x_parsing(&xr,PARTIAL);
	string message;
	if (xn==NULL) {
		message="Unknown expression";
		return new KifString(this,NULL,message);
	}
	KifElement* kret=kifNULL;
	try {
		kret=KifBrowse(xn,kf);
	}
	catch(KifRaiseError* m) {
		delete xn;
		message="Unknown expression";
		delete m;
		return new KifString(this,NULL,message);
	}		
	return kret;
}

void KifCode::Load(x_reading* xr) { 
	current=xr;
	if (parseur!=NULL)
		bnfkif->baseline=parseur->LIGNES;

	string n="_paths";
	KifVariableDeclaration* var=(KifVariableDeclaration*)kstart->Declaration(n);
	KifVector* vargs=(KifVector*)var->value;
	vargs->Push(new KifString(this,NULL,xr->name));
	static time_t tempsinitial,tempsfinal;
	time(&tempsinitial);
	x_node* xn=bnfkif->x_parsing(xr,PARTIAL);
	time(&tempsfinal);         
	double d=difftime(tempsfinal,tempsinitial);
	if (xn==NULL) {
		long line=bnfkif->x_linenumber(xr->lastposerror);
		string message="Error while reading KIF file: ";
		if (xr->errornumber!=-1)
			message+=bnfkif->x_errormsg(xr->errornumber);
		else
			message+=xr->labelerror;
		throw new KifRaiseError(message,current->name,line,xr->lastposerror);
	}

	try {
		Push(kstart);
		KifBrowse(xn,kstart);
		Pop();
	}
	catch(KifRaiseError* m) {
		delete xn;
		throw m;
	}		

	delete xn;
}

void KifCode::Run() {
	kstart->Run();
}

KifElement* KifCode::Search(string name) {
	map<string,KifElement*>::iterator iter=kstart->declarations.begin();
	for (;iter!=kstart->declarations.end();iter++) {
		string val=iter->first;
		val=c_Tolower(val);
		if (val==name)
			return iter->second;
	}
	return NULL;
}

KifElement* KifCode::Searchraw(string name) {
	map<string,KifElement*>::iterator iter=kstart->declarations.begin();
	for (;iter!=kstart->declarations.end();iter++) {
		string val=iter->first;
		if (val==name)
			return iter->second;
	}
	return NULL;
}


bool KifCode::Debugging(KifObject* func,KifElement* ke,KifDomain* dom,KifCallFunction* callfunc) {
	if (debugging==false)
		return false;

	if (gotoline==-10) {
		cout<<"$h for help"<<endl;
		gotoline=-1;
	}
	
	long cline=tabledebugline[ke->Line()];

	if (gotoline==-100) {
		if  (func->Name()!="") {
			gotoline=-1;
			debug=true;
		}
	}
	else
		if (gotoline==cline) {
			debug=true;
			gotoline=-1;
		}

	if (debug==false)
		return false;
	string s=tabledebug[ke->Line()];
	if (func->Name()!="")
		cout<<"["<<STR(func->Name())<<"]"<<cline<<"> "<<STR(s)<<endl;
	else
		cout<<cline<<"> "<<STR(s)<<endl;

	s="x";
	int compte;
	char ch[20];
	while (s!="") {
		sprintf_s(ch,20,"%d> ",cline);
		cout<<ch;		
		compte=strlen(ch);
		char c;
		s="";
		do {
			c=GETCH();
			if (c>=32) {
				s+=c;
				cout<<c;
				compte++;
			}
		}
		while (c>=32);

		if (s=="$") {
			debug=false;
			return false;
		}
		
		if (s=="$h") {
			cout<<endl<<"$: stop debugging"<<endl;
			cout<<"$f: skip to next function"<<endl;
			cout<<"$g line: goto line"<<endl;
			cout<<"$l: list instruction within function"<<endl;
			cout<<"variable"<<endl;
			cout<<"expression;"<<endl;
			continue;
		}

		if (s=="$f") {
			cout<<endl;
			gotoline=-100;
			debug=false;
			return false;
		}

		if (s!="" && s[0]=='$' && s[1]=='g') {
			cout<<endl;
			gotoline=atoi(STR(s)+2);
			debug=false;
			return true;
		}

		if (s=="$l") {
			cout<<endl;
			for (int j=0;j<func->instructions.size();j++) {
				KifElement* ksi=func->instructions[j];
				cout<<ksi->Line()<<"> "<<tabledebug[ksi->Line()]<<endl;
			}
		}
		else
			if (s!="") {
				cout<<endl;
				if (s[s.size()-1]!=';') {
					KifElement* v=Get(s,NULL);
					if (v==NULL || v==kifNULL) {
						cout<<"Unknown variable"<<endl;
						continue;
					}
					v=v->Returnvalue();
					cout<<STR(v->String())<<endl;
				}
				else {
					KifElement* kret=Evaluate(callfunc,s);
					KifElement* kres=kret->Execute(kifTypeSTRING,dom,kifNULL,false);
					if (kres==NULL || kres==kifNULL)
						cout<<"null"<<endl;
					else 
						cout<<STR(kres->String())<<endl;
				}
			}
	}
	char car=8;
	while(compte>0) {
		cout<<car;
		compte--;
	}
	s="                   ";
	cout<<STR(s);
	compte=s.size();
	while(compte>=0) {
		cout<<car;
		compte--;
	}
	return true;
}


KifElement* KifCode::Execute(KifFunction* func,vector<KifElement*>& parameters) {
	string met="__"+func->name;
	KifCallFunction kfunc(NULL,kstart,met,(KifFunction*)func);
	kfunc.idkifcode=id;
	map<string,KifElement*> stacks;
	kfunc.SetParameters(parameters,&stacks);
	Push(kstart);
	Push(&kfunc);
	KifElement* ret=kfunc.Execute(kifNULL,kstart,kifNULL,NULL);
	Pop();
	Pop();
	kfunc.Resetdeclaration(stacks);
	return ret;
}

void KifCode::Clean() {
	garbage_collector(false,basegarbage);
}

void KifCode::Garbage() {
	garbage_collector(true,0);
	kstart=NULL;
}

KifParser::~KifParser() {	
	XipFreeGrammar(ipar);
}

KifKiF::~KifKiF() {
	if (type==kifKiF)
		KifDelete(ipar);
}

KifCode::~KifCode() {
	Garbage();
	kifCodeVector.affecte(id,NULL);
	delete bnfkif;
}

//Generic GARBAGE: clean all instances
void KifCode::garbage_collector(bool force,int from) {
	if (insidecall>0)
		return;

	deleted.raz();
	KifElement* ke;
	size_t it;

	if (force==true) {
		for (it=0;it<garbage.size();it++) {
			ke=garbage[it];
			if (ke!=NULL)
				delete ke;
		}
		return;
	}


	for (it=from;it<garbage.size();it++) {
		ke=garbage[it];
		if (ke==NULL)
			continue;
		if (ke->reference==0)
			ke->Clean();
	}

	int i=from;

	for (it=from;it<garbage.size();it++) {
		ke=garbage[it];
		if (ke==NULL) {
			deleted.push_back(i,100);
			i++;
			continue;
		}
		if (ke->reference<=0) {
			deleted.push_back(i,100);
			delete ke;
			garbage.affecte(i,NULL);
		}
#ifdef _DEBUG
//		else
//			cout<<"NOT CLEANED"<<endl;
#endif
		i++;
	}

	cursor=0;
}

void KifCode::garbage_initialisation() {
	if (basegarbage==0) {
		basegarbage=garbage.size();
		if (trigger!=-1)
			trigger+=basegarbage;
	}
	for (int it=0;it<basegarbage;it++) 
		garbage[it]->reference++;
	int sizevect=basegarbage;
	deleted.ajuste(sizevect);
	garbage.ajuste(sizevect);
	//We then create some elements in advance
	cursor=0;
	for (int i=0;i<sizevect;i++) {
		garbage.push_back(NULL,100);
		deleted.push_back(i+basegarbage,100);
	}
}

int KifCode::Add(KifElement* ke) {
	int i=-1;
	if (cursor<deleted.size()) {		
		i=deleted[cursor];
		garbage.affecte(i,ke);
		cursor++;
	}
	else {
		i=garbage.size();
		garbage.push_back(ke,100);
	}
	return i;		
}


//Specific GARBAGE: we clean all KifElement elements pointing to a XIP temporary information
void KifCode::garbage_xip() {
	for (size_t it=0;it<garbage.size();it++) {
		KifElement* ke=garbage[it];
		if (ke!=NULL)
			ke->CleanXIP();
	}
}

void KifCode::garbage_xml() {
	for (size_t it=0;it<garbage.size();it++) {
		KifElement* ke=garbage[it];
		if (ke!=NULL)
			ke->CleanXML();
	}
}


void KifFinalClean() {
	if (kifNULL!=NULL) {
		delete kifNULL;
		delete kifMINUSONE;
		delete kifDEFAULT;
		delete kifTRUE;
		delete kifFALSE;
		delete kifBREAK;
		delete kifBREAKPOINT;
		delete kifCONTINUE;
		delete kifNEG;
		//Types
		delete kifTypeSTRING;
		delete kifTypeINTEGER;
		delete kifTypeFLOAT;
		delete kifTypeBOOLEAN;
		delete kifTypeVECTOR;
	}
}

bool KifCode::Trigger() {
	if (trigger!=-1) {
		if (garbage.size()>=trigger) {
			if (cursor>=deleted.dernier-trigger/4)
				return true;
		}
	}
	return false;
}


void KifCode::SetArguments(vector<string>& args) {
	for (int i=0;i<args.size();i++) 
		arguments.push_back(args[i]);
}

bool KifCode::KifConversion(string n) {
	if (n=="string" || n=="int" || n=="boolean" || n=="float")
		return true;
	return false;
}


Exportation int KifLoad(vector<string>& paths,vector<string>& arguments,bool debugkif) {
		//On initialise une valeur par defaut du declencheur 
		//de garbage, pour que celui-ci puisse s'effectuer
		//a chaque fin d'execution
		KifCode* kifcode=new KifCode(NULL,kif_declencheur);
		kifcode->SetArguments(arguments);
		for (int i=0;i<paths.size();i++) {
			ifstream fkif(STR(paths[i]),modeOuverture);
			if (fkif.fail() ) {
				cerr<<"Cannot open kif file: "<<STR(paths[i])<<endl;
				return(-1);
			}
			try {
				kifcode->bnfkif->baseline=1;
				cerr<<"Loading kif file: "<<STR(paths[i])<<endl;
				kifcode->Loadfile(fkif,paths[i],0);
			}
			catch(KifRaiseError* m) {
				cerr<<STR(m->message)<<" at "<<m->left<<endl;
				delete m;
				return -1;
			}	
		}
		kifcode->Setdebug(debugkif);
		kifcode->Run();
		if (kifcode->error==true) {
			cerr<<STR(kifcode->errorptr->String())<<endl;
			return -1;
		}
		return kifcode->id;
}


Exportation int XipKifLoad(vector<string>& paths,vector<string>& arguments,bool debugkif) {
		
		//On initialise une valeur par defaut du declencheur 
		//de garbage, pour que celui-ci puisse s'effectuer
		//a chaque fin d'execution
		GlobalParseur* parseur=new GlobalParseur("","");		

		parseur->iParseurCourant=gVecteurParseurs.dernier;
		gVecteurParseurs.ajoute(parseur);
		parseur->kifcode=new KifCode(parseur,kif_declencheur);
		parseur->kifcode->SetArguments(arguments);

		KifCode* kifcode=parseur->kifcode;
		for (int i=0;i<paths.size();i++) {
			ifstream fkif(STR(paths[i]),modeOuverture);
			if (fkif.fail() ) {
				cerr<<"Cannot open kif file: "<<STR(paths[i])<<endl;
				return(-1);
			}
			try {
				kifcode->bnfkif->baseline=1;
				cerr<<"Loading kif file: "<<STR(paths[i])<<endl;
				kifcode->Loadfile(fkif,paths[i],0);
			}
			catch(KifRaiseError* m) {
				cerr<<STR(m->message)<<" at "<<m->left<<endl;
				delete m;
				return -1;
			}	
		}
		kifcode->Setdebug(debugkif);
		kifcode->Run();
		if (kifcode->error==true) {
			cerr<<STR(kifcode->errorptr->String())<<endl;
			return -1;
		}
		return parseur->iParseurCourant;
}


Exportation string KifExecute(int ikif,string name,vector<string>& parameters,ostringstream* os,bool debugkif) {
	KifCode* kifcode=kifCodeVector[ikif];
    if (kifcode==NULL)
        return "";

	ostream* old_os=kifcode->os;
	kifcode->os=os;
	kifcode->Setdebug(debugkif);

	vector<KifElement*> params;
	char buf[10];
	for (int i=0;i<parameters.size();i++) {
		KifElement* ke=new KifString(kifcode,NULL,parameters[i]);
		string varname="^i_";
		sprintf(buf,"%d",i);
		varname+=buf;

		KifVariableDeclaration* kvar=new KifVariableDeclaration(kifcode,NULL,varname);
		kvar->value=ke;
		KifInstance* ki=new KifInstance(kifcode,NULL,varname,kvar);
		ki->value=ke;
		params.push_back(ki);
	}

	string valeur;

	KifElement* proc_ke=kifcode->Searchraw(name);
	if (proc_ke==NULL || proc_ke->type!=kifFunction) {
		valeur="Unknown function:";
		valeur+=name;
		return valeur;
	}
	
	KifFunction* proc=(KifFunction*)proc_ke;
	KifElement* resultat=kifcode->Execute(proc,params);
	
	if (kifcode->error==true) {
		valeur=resultat->String();
		return valeur;
	}
	
    valeur=resultat->String();
	kifcode->Clean();
	kifcode->os=old_os;
	return valeur;	
}

Exportation string XipKifExecute(int ipar,string name,vector<string>& parameters,ostringstream* os,bool debugkif) {
	GlobalParseur* parseur= gVecteurParseurs[ipar];
	if (parseur==NULL)
		return "";

	KifCode* kifcode=parseur->kifcode;
    if (kifcode==NULL)
        return "";

	ostream* old_os=kifcode->os;
	kifcode->os=os;
	kifcode->Setdebug(debugkif);

	vector<KifElement*> params;
	char buf[10];
	for (int i=0;i<parameters.size();i++) {
		KifElement* ke=new KifString(kifcode,NULL,parameters[i]);
		string varname="^i_";
		sprintf(buf,"%d",i);
		varname+=buf;

		KifVariableDeclaration* kvar=new KifVariableDeclaration(kifcode,NULL,varname);
		kvar->value=ke;
		KifInstance* ki=new KifInstance(kifcode,NULL,varname,kvar);
		ki->value=ke;
		params.push_back(ki);
	}

	string valeur;

	KifElement* proc_ke=kifcode->Searchraw(name);
	if (proc_ke==NULL || proc_ke->type!=kifFunction) {
		valeur="Unknown function:";
		valeur+=name;
		return valeur;
	}
	
	KifFunction* proc=(KifFunction*)proc_ke;
	KifElement* resultat=kifcode->Execute(proc,params);
	
	if (kifcode->error==true) {
		valeur=resultat->String();
		return valeur;
	}
	
    valeur=resultat->String();
	kifcode->Clean();
	kifcode->os=old_os;
	return valeur;	
}

Exportation void KifDelete(int ipar) {
	KifCode* kifcode=kifCodeVector[ipar];
    if (kifcode==NULL)
        return;
	delete kifcode;
}

Exportation void KifSetTrigger(int threshold) {
	kif_declencheur=threshold;
	for (int i=0;i<kifCodeVector.size();i++) 
		kifCodeVector[i]->trigger=kif_declencheur;
}
