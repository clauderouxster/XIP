#include "kifn.h"


static const char* errorlabels[]={"","Wrong index format",
"Wrong expression after an operator",
"Wrong list of parameters",
"Wrong expressions in parentheses",
"Wrong FRAME declaration",
"Wrong argument declaration",
"Wrong FUNCTION declaration",
"Wrong BOOLEAN EXPRESSION",
"Wrong BOOLEAN expression",
"Wrong IF declaration",
"Wrong WHILE declaration",
"Wrong FOR declaration",
"Wrong FUNCTION CALL",
"Wrong Expression",
"Wrong Instruction",
"Wrong SWITCH",
"Wrong test in a SWITCH",
"Wrong IN or ON expression",
"Wrong WITH expression",
"Wrong PREDICATE declaration",
"Wrong PATTERN in A RULE",
"Wrong TRY-CATCH expression",
"Wrong INTERVAL",
"Wrong ELSE section",
"Wrong VECTOR definition",
"Wrong MAP definition",
"Wrong Function call in an index reference",
"Wrong comments",
"Wrong dotted expression",
"Wrong expression or Missing ';'",
"Missing '}'",
"Wrong declaration",
"Wrong FOR IN declaration",
"Wrong PREDICATE definition",
"Wrong PREDICATE expression",
"Wrong PREDICATE parameter list",
"Wrong PREDICATE term",
"Wrong tuple",
"Wrong ASSERT definition",
"Wrong PREDICATE VARIABLE",
"Wrong RANGE definition",
"Wrong LISP expression",
"Wrong TAG expression",
"Wrong DCG declaration",
"Wrong DCG or Missing '.'",
"Wrong Predicate Rule or Missing '.'",
"Wrong LAMBDA use",
"Wrong FUNCTIONAL description",
"Wrong FILTER",
"Wrong FOLD or SCAN",
"Wrong MAP",
"Wrong CYCLE or REPEAT",
"Wrong REPLICATE",
"Wrong Haskell LAMBDA",
"Wrong Haskell CASE",
"Wrong Haskell Boolean Use",
"Wrong FLIP",
"Wrong EXTENSION implementation",
"Expecting a meta-character",
"Expecting a '.'",
"Wrong tuple definition"};

static char tab0[]={'_','#',0};
static char tab1[]={'&','|','+','-','*','/','%','^',0};
static char tab2[]={'<','>',0};
static char tab3[]={',',';',0};
static char tab4[]={'|',':',0};
static char tab5[]={':','=',0};
static char tab6[]={',',';',0};
static char tab7[]={'&','+','-','*','/','%','^',0};
static char tab8[]={',',';',0};
static char tab9[]={'d','p','c','C','a','r','s','S','?','%',0};
static char tab10[]={'*','+',0};
static const char* varstring3="and";
static const char* varstring13="exclusive";
static const char* varstring77="elif";
static const char* varstring74="false";
static const char* varstring58="takeWhile";
static const char* varstring48="iterator";
static const char* varstring79="do";
static const char* varstring41="assert";
static const char* varstring0="is";
static const char* varstring82="try";
static const char* varstring78="while";
static const char* varstring4="private";
static const char* varstring26="KIF";
static const char* varstring36="fail";
static const char* varstring66="repeat";
static const char* varstring28="kIf";
static const char* varstring50="foldl";
static const char* varstring32="in";
static const char* varstring14="polynomial";
static const char* varstring60="dropWhile";
static const char* varstring55="foldr1";
static const char* varstring27="KiF";
static const char* varstring75="if";
static const char* varstring52="scanl";
static const char* varstring59="filter";
static const char* varstring43="ponder";
static const char* varstring53="scanr";
static const char* varstring6="const";
static const char* varstring65="zip";
static const char* varstring73="true";
static const char* varstring44="get";
static const char* varstring69="flip";
static const char* varstring25="KIf";
static const char* varstring10="strict";
static const char* varstring22="continue";
static const char* varstring68="replicate";
static const char* varstring61="take";
static const char* varstring64="zipWith";
static const char* varstring16="autorun";
static const char* varstring37="retract";
static const char* varstring80="for";
static const char* varstring51="foldr";
static const char* varstring8="frame";
static const char* varstring45="store";
static const char* varstring15="function";
static const char* varstring63="map";
static const char* varstring31="notin";
static const char* varstring19="return";
static const char* varstring57="scanr1";
static const char* varstring35="on";
static const char* varstring54="foldl1";
static const char* varstring49="otherwise";
static const char* varstring21="break";
static const char* varstring30="kIF";
static const char* varstring1="not";
static const char* varstring34="from";
static const char* varstring83="catch";
static const char* varstring29="kiF";
static const char* varstring7="with";
static const char* varstring23="kif";
static const char* varstring67="cycle";
static const char* varstring38="retractdb";
static const char* varstring33="absent";
static const char* varstring39="asserta";
static const char* varstring9="extension";
static const char* varstring17="thread";
static const char* varstring81="switch";
static const char* varstring71="of";
static const char* varstring20="_return";
static const char* varstring62="drop";
static const char* varstring11="joined";
static const char* varstring46="remove";
static const char* varstring24="Kif";
static const char* varstring42="assertdb";
static const char* varstring84="default";
static const char* varstring76="else";
static const char* varstring12="protected";
static const char* varstring5="common";
static const char* varstring70="case";
static const char* varstring47="let";
static const char* varstring56="scanl1";
static const char* varstring40="assertz";
static const char* varstring72="where";
static const char* varstring2="or";
static const char* varstring18="lambda";
static const int ivarstring3=strlen(varstring3);
static const int ivarstring13=strlen(varstring13);
static const int ivarstring77=strlen(varstring77);
static const int ivarstring74=strlen(varstring74);
static const int ivarstring58=strlen(varstring58);
static const int ivarstring48=strlen(varstring48);
static const int ivarstring79=strlen(varstring79);
static const int ivarstring41=strlen(varstring41);
static const int ivarstring0=strlen(varstring0);
static const int ivarstring82=strlen(varstring82);
static const int ivarstring78=strlen(varstring78);
static const int ivarstring4=strlen(varstring4);
static const int ivarstring26=strlen(varstring26);
static const int ivarstring36=strlen(varstring36);
static const int ivarstring66=strlen(varstring66);
static const int ivarstring28=strlen(varstring28);
static const int ivarstring50=strlen(varstring50);
static const int ivarstring32=strlen(varstring32);
static const int ivarstring14=strlen(varstring14);
static const int ivarstring60=strlen(varstring60);
static const int ivarstring55=strlen(varstring55);
static const int ivarstring27=strlen(varstring27);
static const int ivarstring75=strlen(varstring75);
static const int ivarstring52=strlen(varstring52);
static const int ivarstring59=strlen(varstring59);
static const int ivarstring43=strlen(varstring43);
static const int ivarstring53=strlen(varstring53);
static const int ivarstring6=strlen(varstring6);
static const int ivarstring65=strlen(varstring65);
static const int ivarstring73=strlen(varstring73);
static const int ivarstring44=strlen(varstring44);
static const int ivarstring69=strlen(varstring69);
static const int ivarstring25=strlen(varstring25);
static const int ivarstring10=strlen(varstring10);
static const int ivarstring22=strlen(varstring22);
static const int ivarstring68=strlen(varstring68);
static const int ivarstring61=strlen(varstring61);
static const int ivarstring64=strlen(varstring64);
static const int ivarstring16=strlen(varstring16);
static const int ivarstring37=strlen(varstring37);
static const int ivarstring80=strlen(varstring80);
static const int ivarstring51=strlen(varstring51);
static const int ivarstring8=strlen(varstring8);
static const int ivarstring45=strlen(varstring45);
static const int ivarstring15=strlen(varstring15);
static const int ivarstring63=strlen(varstring63);
static const int ivarstring31=strlen(varstring31);
static const int ivarstring19=strlen(varstring19);
static const int ivarstring57=strlen(varstring57);
static const int ivarstring35=strlen(varstring35);
static const int ivarstring54=strlen(varstring54);
static const int ivarstring49=strlen(varstring49);
static const int ivarstring21=strlen(varstring21);
static const int ivarstring30=strlen(varstring30);
static const int ivarstring1=strlen(varstring1);
static const int ivarstring34=strlen(varstring34);
static const int ivarstring83=strlen(varstring83);
static const int ivarstring29=strlen(varstring29);
static const int ivarstring7=strlen(varstring7);
static const int ivarstring23=strlen(varstring23);
static const int ivarstring67=strlen(varstring67);
static const int ivarstring38=strlen(varstring38);
static const int ivarstring33=strlen(varstring33);
static const int ivarstring39=strlen(varstring39);
static const int ivarstring9=strlen(varstring9);
static const int ivarstring17=strlen(varstring17);
static const int ivarstring81=strlen(varstring81);
static const int ivarstring71=strlen(varstring71);
static const int ivarstring20=strlen(varstring20);
static const int ivarstring62=strlen(varstring62);
static const int ivarstring11=strlen(varstring11);
static const int ivarstring46=strlen(varstring46);
static const int ivarstring24=strlen(varstring24);
static const int ivarstring42=strlen(varstring42);
static const int ivarstring84=strlen(varstring84);
static const int ivarstring76=strlen(varstring76);
static const int ivarstring12=strlen(varstring12);
static const int ivarstring5=strlen(varstring5);
static const int ivarstring70=strlen(varstring70);
static const int ivarstring47=strlen(varstring47);
static const int ivarstring56=strlen(varstring56);
static const int ivarstring40=strlen(varstring40);
static const int ivarstring72=strlen(varstring72);
static const int ivarstring2=strlen(varstring2);
static const int ivarstring18=strlen(varstring18);


char bnf_kif::m_char(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="char";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_ord(lret,'A','Z') || x_test_ord(lret,'a','z') || x_test_in(lret,tab0))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_path_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_path(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_path(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="path";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'/')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_path_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_shell_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_path(lret,&subtree) || m_word(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_shell(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'#')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'!')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_shell_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_pop_node(tree,addsubtree);
 return(1);
}


char bnf_kif::m_signature(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,239)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,187)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,191)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_pop_node(tree,addsubtree);
 return(1);
}


char bnf_kif::m_label(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="label";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'#')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_anydouble_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'\\')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_any(lret)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anydouble_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYNEGATION
 subtree=new x_node;
 if (x_test_char(lret,34)==0) {
   x_test_any(lret);
   x_init_tree(tree,subtree,addsubtree);
 }
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anydouble(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="anydouble";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_anydouble_0_1(lret,&subtree) || m_anydouble_0_2(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_anysimple(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="anysimple";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYNEGATION
 subtree=new x_node;
 if (x_test_char(lret,39)==0) {
   x_test_any(lret);
   x_init_tree(tree,subtree,addsubtree);
 }
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_anycolon_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'%')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_any(lret)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anycolon_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYNEGATION
 subtree=new x_node;
 if (x_test_char(lret,';')==0) {
   x_test_any(lret);
   x_init_tree(tree,subtree,addsubtree);
 }
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anycolon(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="anycolon";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_anycolon_0_1(lret,&subtree) || m_anycolon_0_2(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_operator_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'&') || !x_test_char(lret,'&') || !x_test_char(lret,'&')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operator_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,':') || !x_test_char(lret,':')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operator_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'<') || !x_test_char(lret,'<')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operator_0_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'>') || !x_test_char(lret,'>')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operator_0_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'^') || !x_test_char(lret,'^')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operator(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="operator";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case '&':
if (m_operator_0_1(lret,&subtree) || x_test_in(lret,tab1)) 
err=false;
break;
case ':':
if (m_operator_0_2(lret,&subtree)) 
err=false;
break;
case '<':
if (m_operator_0_3(lret,&subtree)) 
err=false;
break;
case '>':
if (m_operator_0_4(lret,&subtree)) 
err=false;
break;
case '^':
if (m_operator_0_5(lret,&subtree) || x_test_in(lret,tab1)) 
err=false;
break;
case '|':
if (x_test_in(lret,tab1)) 
err=false;
break;
case '+':
if (x_test_in(lret,tab1)) 
err=false;
break;
case '-':
if (x_test_in(lret,tab1)) 
err=false;
break;
case '*':
if (x_test_in(lret,tab1)) 
err=false;
break;
case '/':
if (x_test_in(lret,tab1)) 
err=false;
break;
case '%':
if (x_test_in(lret,tab1)) 
err=false;
break;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,true);
return(1);
}

char bnf_kif::m_operatoraffectation_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'&') || !x_test_char(lret,'&') || !x_test_char(lret,'&') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,':') || !x_test_char(lret,':') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (x_test_string(lret,ivarstring1,varstring1))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring0,varstring0)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_operatoraffectation_0_3_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'<') || !x_test_char(lret,'<') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_6(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'>') || !x_test_char(lret,'>') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_7(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'+') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_8(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'-') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_9(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'*') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_10(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'/') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_11(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'%') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_12(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'&') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_13(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'|') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_14(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'^') || !x_test_char(lret,'^') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_15(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'^') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_0_16(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'<') || !x_test_char(lret,'<') || !x_test_char(lret,'<')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="operatoraffectation";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case '&':
if (m_operatoraffectation_0_1(lret,&subtree) || m_operatoraffectation_0_12(lret,&subtree)) 
err=false;
break;
case ':':
if (m_operatoraffectation_0_2(lret,&subtree)) 
err=false;
break;
case '<':
if (m_operatoraffectation_0_5(lret,&subtree) || m_operatoraffectation_0_16(lret,&subtree)) 
err=false;
break;
case '>':
if (m_operatoraffectation_0_6(lret,&subtree)) 
err=false;
break;
case '+':
if (m_operatoraffectation_0_7(lret,&subtree)) 
err=false;
break;
case '-':
if (m_operatoraffectation_0_8(lret,&subtree)) 
err=false;
break;
case '*':
if (m_operatoraffectation_0_9(lret,&subtree)) 
err=false;
break;
case '/':
if (m_operatoraffectation_0_10(lret,&subtree)) 
err=false;
break;
case '%':
if (m_operatoraffectation_0_11(lret,&subtree)) 
err=false;
break;
case '|':
if (m_operatoraffectation_0_13(lret,&subtree)) 
err=false;
break;
case '^':
if (m_operatoraffectation_0_14(lret,&subtree) || m_operatoraffectation_0_15(lret,&subtree)) 
err=false;
break;
case '=':
if (x_test_char(lret,'=')) 
err=false;
break;
default:
if (m_operatoraffectation_0_3(lret,&subtree)) 
err=false;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,true);
return(1);
}

char bnf_kif::m_orand_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'|') || !x_test_char(lret,'|')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_orand_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'&') || !x_test_char(lret,'&')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_orand(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="orand";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_orand_0_1(lret,&subtree) || m_orand_0_2(lret,&subtree) || x_test_string(lret,ivarstring2,varstring2) || x_test_string(lret,ivarstring3,varstring3))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_comparator_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'~') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'!') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'=') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_0_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'<') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_0_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'>') || !x_test_char(lret,'=')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="comparator";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case '~':
if (m_comparator_0_1(lret,&subtree)) 
err=false;
break;
case 33:
if (m_comparator_0_2(lret,&subtree)) 
err=false;
break;
case '=':
if (m_comparator_0_3(lret,&subtree)) 
err=false;
break;
case '<':
if (m_comparator_0_4(lret,&subtree) || x_test_in(lret,tab2)) 
err=false;
break;
case '>':
if (m_comparator_0_5(lret,&subtree) || x_test_in(lret,tab2)) 
err=false;
break;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,true);
return(1);
}

char bnf_kif::m_operation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="operation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_operator(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_comparison(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="comparison";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_comparator(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nocomparisonexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_private(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="private";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring4,varstring4))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_common(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="common";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring5,varstring5))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_const(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="const";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring6,varstring6))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_feature_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_common(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_feature_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_private(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_feature_0_1_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_feature_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_common(lret,&subtree) || m_const(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_feature(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="feature";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_feature_0_1(lret,&subtree) || m_feature_0_3(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_type_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_feature(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_type(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="type";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_type_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_typename(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_depend(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="depend";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring7,varstring7)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,19);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_declaration_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_parameters(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_declaration_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_declaration_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_declaration_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_depend(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_declaration_0_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_declaration_0_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_declaration_0_4_5(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_declaration(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="declaration";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_type(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_0_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_declarationlist_0_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="declarationlist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,32);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_declarationlist_0_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_declarationlist_0_4_5(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_declarationlist_0_6_7(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="declarationlist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,32);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_declarationlist(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_declarationlist_0_6(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_declarationlist_0_6_7(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_declarationlist(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="declarationlist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declarationlist_0_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declarationlist_0_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_multideclaration(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="multideclaration";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_type(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declarationlist_0_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declarationlist_0_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_declarationending(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="declarationending";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,';'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_frame_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_private(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_frame_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_declarationending(lret,&subtree) || m_instruction(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_frame(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="frame";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_frame_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring8,varstring8)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,5);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_typename(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_frame_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_extension(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="extension";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring9,varstring9)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,58);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_typename(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_instruction(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_instruction_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_blocs(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_instruction(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="instruction";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,15);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_instruction_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_nonlimited(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="nonlimited";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'.') || !x_test_char(lret,'.') || !x_test_char(lret,'.')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_arguments_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_nonlimited(lret,&subtree) || m_arguments(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_arguments_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="arguments";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,6);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_arguments_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_arguments_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_arguments_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_arguments(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="arguments";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_arguments_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_strict(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="strict";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring10,varstring10))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_join(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="join";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring11,varstring11))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_protecclusive(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="protecclusive";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring12,varstring12) || x_test_string(lret,ivarstring13,varstring13))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_functionsort(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="functionsort";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring14,varstring14) || x_test_string(lret,ivarstring15,varstring15) || x_test_string(lret,ivarstring16,varstring16) || x_test_string(lret,ivarstring17,varstring17))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_typefunction_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_join(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_typefunction_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_protecclusive(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_typefunction_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_private(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_typefunction_0_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_strict(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_typefunction(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="typefunction";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_typefunction_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_typefunction_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_typefunction_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_typefunction_0_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_functionsort(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_indexname(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="indexname";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'[') || !x_test_char(lret,']')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_intervalname(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="intervalname";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'[') || !x_test_char(lret,':') || !x_test_char(lret,']')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_function_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_word(lret,&subtree) || m_plusplus(lret,&subtree) || m_operator(lret,&subtree) || m_comparator(lret,&subtree) || m_indexname(lret,&subtree) || m_intervalname(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_function_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_arguments(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_function(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="function";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_typefunction(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,7);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_function_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_function_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_frame_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_lambda(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lambda";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring18,varstring18)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_function_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_instruction(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_evallambda(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="evallambda";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_lambda(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,47);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_operationon(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_subfunc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="subfunc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,27);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_regularcall(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_subfuncbis(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="subfuncbis";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_regularcall(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_subvar(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="subvar";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,29);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_framesep(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="framesep";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operator_0_2(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_frameup(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="frameup";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_framesep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_functioncall_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_frameup(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_functioncall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="functioncall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_functioncall_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_regularcall_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_parameters(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_regularcall_0_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_interval(lret,&subtree) || m_indexes(lret,&subtree) || m_subfunc(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_regularcall_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_regularcall_0_2_3(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_regularcall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="regularcall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_functioncall(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,13);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_regularcall_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_regularcall_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_purecall_0_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_interval(lret,&subtree) || m_indexes(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_purecall_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_purecall_0_2_3(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_purecall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="purecall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_functioncall(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,13);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_regularcall_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_purecall_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_returncall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="returncall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring19,varstring19) || x_test_string(lret,ivarstring20,varstring20))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_breakcall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="breakcall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring21,varstring21))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_continuecall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="continuecall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring22,varstring22))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_call(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="call";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_breakcall(lret,&subtree) || m_continuecall(lret,&subtree) || m_regularcall(lret,&subtree) || m_returncall(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_sep(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="sep";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,':'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_parameters_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="parameters";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,3);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_parameters(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_parameters_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_parameters_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_parameters(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="parameters";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_nocomparisonexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_parameters_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_sousbloc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="sousbloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_multideclaration(lret,&subtree) || m_affectation(lret,&subtree) || m_expressions(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_bloc_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="bloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dcg(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,45);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_bloc_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="bloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicatedefinition(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,46);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_bloc_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="bloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_sousbloc(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,30);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,';')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_bloc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="bloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_predicatefact(lret,&subtree) || m_bloc_0_1(lret,&subtree) || m_bloc_0_2(lret,&subtree) || m_bloc_0_3(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_complexbloc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="complexbloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_switch(lret,&subtree) || m_trycatch(lret,&subtree) || m_loop(lret,&subtree) || m_doloop(lret,&subtree) || m_test(lret,&subtree) || m_forin(lret,&subtree) || m_for(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_blocs_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="blocs";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_label(lret,&subtree) || m_extension(lret,&subtree) || m_frame(lret,&subtree) || m_complexbloc(lret,&subtree) || m_function(lret,&subtree) || m_bloc(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_blocs(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="blocs";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_blocs_0_1(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_blocs_0_1(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_analyse_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_signature(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_analyse_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_shell(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_analyse_0_3_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring23,varstring23) || x_test_string(lret,ivarstring24,varstring24) || x_test_string(lret,ivarstring25,varstring25) || x_test_string(lret,ivarstring26,varstring26) || x_test_string(lret,ivarstring27,varstring27) || x_test_string(lret,ivarstring28,varstring28) || x_test_string(lret,ivarstring29,varstring29) || x_test_string(lret,ivarstring30,varstring30))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_analyse_0_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_0_3_4_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_analyse_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_analyse_0_3_4(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_analyse(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="analyse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocs(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_variable_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_interval(lret,&subtree) || m_indexes(lret,&subtree) || m_subfuncbis(lret,&subtree) || m_subvar(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_variable_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_variable_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_variable(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="variable";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_allvariable(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="allvariable";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_predicatevariable(lret,&subtree) || m_variable(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_purevariable_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_interval(lret,&subtree) || m_indexes(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_purevariable_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_purevariable_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_purevariable(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="purevariable";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_purevariable_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_predvariable(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predvariable";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_predicatevariable(lret,&subtree) || m_purevariable(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_plusplus_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'+') || !x_test_char(lret,'+')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_plusplus_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'-') || !x_test_char(lret,'-')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_plusplus(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="plusplus";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_plusplus_0_1(lret,&subtree) || m_plusplus_0_2(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_affectation_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_valmaptail(lret,&subtree) || m_valvectortail(lret,&subtree) || m_variable(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_affectation_0_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="affectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_operatoraffectation(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,14);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_affectation_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_plusplus(lret,&subtree) || m_affectation_0_2_3(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_affectation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="affectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectation_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectation_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_increment(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="increment";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_plusplus(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_notin_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring1,varstring1)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring32,varstring32)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_notin_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring33,varstring33)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring34,varstring34)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_notin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="notin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring31,varstring31) || m_notin_0_1(lret,&subtree) || m_notin_0_2(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_in(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="in";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring32,varstring32))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_on(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="on";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring35,varstring35))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_operatorin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="operatorin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_notin(lret,&subtree) || m_in(lret,&subtree) || m_on(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_operationin_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="operationin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring7,varstring7)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,19);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_operationin_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_operationin_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_operationin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="operationin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_operatorin(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,18);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_operationin_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_operationon(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="operationon";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_on(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,18);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_negation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="negation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_char(lret,'~') || x_test_string(lret,ivarstring1,varstring1))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_minus_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_variable(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_minus(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="minus";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'-')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_minus_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_indexes_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_minus(lret,&subtree) || m_expressions(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_indexes_0_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_interval(lret,&subtree) || m_indexes(lret,&subtree) || m_subfunc(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_indexes_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_indexes_0_2_3(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_indexes(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="indexes";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,1);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_indexes_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_indexes_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_interval_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_indexes_0_1(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_interval(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="interval";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_interval_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,23);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_interval_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_indexes_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_valtuple_0_1_2_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_valtuple_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_valtuple_0_1_2_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_valtuple_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valtuple_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_valtuple_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_valtuple_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valtuple(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="valtuple";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,61);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_valtuple_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_valvector(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="valvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_valtuple_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_valtail(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="valtail";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_valvectortail_0_1_2_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_valvectortail_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_valvectortail_0_1_2_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_valvectortail_0_1_2_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_valtail(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valvectortail_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valvectortail_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valvectortail_0_1_2_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_valvectortail_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_valvectortail_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valvectortail(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="valvectortail";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_valvectortail_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_maptail(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="maptail";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_dicoval(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="dicoval";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_valmaptail_0_1_2_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dicoval(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_valmaptail_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_valmaptail_0_1_2_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_valmaptail_0_1_2_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_maptail(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valmaptail_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dicoval(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valmaptail_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valmaptail_0_1_2_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_valmaptail_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_valmaptail_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valmaptail(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="valmaptail";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,26);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_valmaptail_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_intentionsep(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="intentionsep";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'.') || !x_test_char(lret,'.')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_step(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="step";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_intentionvector_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="intentionvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionsep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_intentionvector_0_1_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_intentionvector_0_1_3_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_step(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_intentionvector_0_1_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="intentionvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionsep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionvector_0_1_3_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionvector_0_1_3_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_intentionvector_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_intentionvector_0_1_2(lret,&subtree) || m_intentionvector_0_1_3(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_intentionvector(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="intentionvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionvector_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_intentionwithdouble_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="intentionwithdouble";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionsep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_intentionwithdouble_0_1_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="intentionwithdouble";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionsep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionvector_0_1_3_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_intentionwithdouble_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_intentionwithdouble_0_1_2(lret,&subtree) || m_intentionwithdouble_0_1_3(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_intentionwithdouble(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="intentionwithdouble";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionwithdouble_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_intentionvect(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="intentionvect";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
bool fnd=false;
bool nbcomma=false;
unsigned char c;
for (int ix=currentpos+1;ix<fx->stack.size();ix++) {
c=fx->stack[ix][0];
if (c=='[' || c==']')
return 0;
if (c==',')
{
if (nbcomma)
return 0;
nbcomma=true;}
if ((ix+1)<fx->stack.size() && c=='.' && fx->stack[ix+1][0]=='.')
{fnd=true;
break;}
}
if (!fnd)
return 0;

//BODYOR
subtree=NULL;
if (m_intentionvector(lret,&subtree) || m_intentionwithdouble(lret,&subtree))
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,false);
return(1);
}

char bnf_kif::m_dico(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="dico";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_valmap_0_1_2_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dico(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_valmap_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_valmap_0_1_2_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_valmap_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dico(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valmap_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_valmap_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_valmap_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valmap(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="valmap";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,26);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_valmap_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_jvector_0_1_2_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_jexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_jvector_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_jvector_0_1_2_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_jvector_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_jexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_jvector_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_jvector_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_jvector_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_jvector(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="jvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_jvector_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_jdico_0_1(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case 34:
if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
err=false;
break;
case 39:
if (m_astringsimple(lret,&subtree)) 
err=false;
break;
case '0':
if (m_xnumber(lret,&subtree) || m_anumber(lret,&subtree)) 
err=false;
break;
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '-':
if (m_anumber(lret,&subtree)) 
err=false;
break;
case 't':
case 'f':
if (m_abool(lret,&subtree)) 
{err=false;
break;
}
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
return(1);
}

char bnf_kif::m_jdico(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="jdico";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_jdico_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_jexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_jmap_0_1_2_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_jdico(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_jmap_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_jmap_0_1_2_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_jmap_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_jdico(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_jmap_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_jmap_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_jmap_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_jmap(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="jmap";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,26);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_jmap_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_jexpression(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="jexpression";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case 34:
if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
err=false;
break;
case 39:
if (m_astringsimple(lret,&subtree)) 
err=false;
break;
case '[':
if (m_jvector(lret,&subtree)) 
err=false;
break;
case '{':
if (m_jmap(lret,&subtree)) 
err=false;
break;
case '0':
if (m_xnumber(lret,&subtree) || m_anumber(lret,&subtree)) 
err=false;
break;
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '-':
if (m_anumber(lret,&subtree)) 
err=false;
break;
case 't':
case 'f':
if (m_abool(lret,&subtree)) 
{err=false;
break;
}
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,false);
return(1);
}

char bnf_kif::m_cut(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="cut";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,'!'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_fail(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="fail";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring36,varstring36))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_predicate_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_probaexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_predicate_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_predicate_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_predicate_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_predicateparameters(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_predicate(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicate_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicate_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_predicatecall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicatecall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_functioncall(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicate_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_probaexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="probaexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_localif(lret,&subtree) || m_abool(lret,&subtree) || m_xnumber(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_call(lret,&subtree) || m_optional(lret,&subtree) || m_predvariable(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_retractcommand(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="retractcommand";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring37,varstring37) || x_test_string(lret,ivarstring38,varstring38))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_assertcommandbefore(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="assertcommandbefore";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring39,varstring39))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_assertcommandafter(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="assertcommandafter";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring40,varstring40) || x_test_string(lret,ivarstring41,varstring41) || x_test_string(lret,ivarstring42,varstring42))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_assertweight(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="assertweight";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring43,varstring43))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_assertpredicate_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_retractcommand(lret,&subtree) || m_assertcommandbefore(lret,&subtree) || m_assertcommandafter(lret,&subtree) || m_assertweight(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_assertpredicate_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_valvectortail_0_1_2_3_4(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_assertpredicate(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="assertpredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_assertpredicate_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,39);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicate(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_assertpredicate_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_predicatevariable(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicatevariable";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'?')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,40);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_term_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_predicateparameters(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_term(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="term";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'?')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,37);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_term_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_tuple(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="tuple";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateparameters(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_affectexpression_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_anumber(lret,&subtree) || m_pexpression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_affectexpression_0_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_operation(lret,&subtree) || m_comparison(lret,&subtree) || m_operationin(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_affectexpression_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_affectexpression_0_2_3(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_affectexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="affectexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectexpression_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectexpression_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_affectationpredicate_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="affectationpredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_operatoraffectation(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,14);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_affectationpredicate_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_plusplus(lret,&subtree) || m_affectationpredicate_0_1_2(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_affectationpredicate(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="affectationpredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predvariable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationpredicate_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_comparepredicate_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="comparepredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_comparator(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,14);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_comparepredicate(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="comparepredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predvariable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_comparepredicate_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_pintentionvector_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="pintentionvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionsep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_pexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_pintentionvector_0_1_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_pexpression(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_pintentionvector_0_1_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="pintentionvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_pexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionsep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_pintentionvector_0_1_3_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionvector_0_1_3_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_pintentionvector_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_pintentionvector_0_1_2(lret,&subtree) || m_pintentionvector_0_1_3(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_pintentionvector(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="pintentionvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_pintentionvector_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_pintentionwithdouble_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="pintentionwithdouble";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionsep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_pexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_pexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_pintentionwithdouble_0_1_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="pintentionwithdouble";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_pexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_pexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_intentionsep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_pintentionvector_0_1_3_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_pintentionwithdouble_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_pintentionwithdouble_0_1_2(lret,&subtree) || m_pintentionwithdouble_0_1_3(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_pintentionwithdouble(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="pintentionwithdouble";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_pintentionwithdouble_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_pintentionvect(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="pintentionvect";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
bool fnd=false;
bool nbcomma=false;
unsigned char c;
for (int ix=currentpos+1;ix<fx->stack.size();ix++) {
c=fx->stack[ix][0];
if (c=='[' || c==']')
return 0;
if (c==',')
{
if (nbcomma)
return 0;
nbcomma=true;}
if ((ix+1)<fx->stack.size() && c=='.' && fx->stack[ix+1][0]=='.')
{fnd=true;
break;}
}
if (!fnd)
return 0;

//BODYOR
subtree=NULL;
if (m_pintentionvector(lret,&subtree) || m_pintentionwithdouble(lret,&subtree))
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,false);
return(1);
}

char bnf_kif::m_merging(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="merging";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_pexpression(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_apredicatelist_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="apredicatelist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_apredicatelist(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_apredicatelist_0_1_2_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYCONTEXTUAL
 long poscontext=currentpos;
	int itokcontext=intoken;
 string lretbis;
 if (x_test_char(lretbis,']')) {
    currentpos=poscontext;
	intoken=itokcontext;
 }
 else {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_apredicatelist_0_1_2_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="apredicatelist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,25);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_merging(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_apredicatelist_0_1_2_4_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_apredicatelist_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_apredicatelist_0_1_2_3(lret,&subtree) || m_apredicatelist_0_1_2_4(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_apredicatelist_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_apredicatelist_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_apredicatelist(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="apredicatelist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_pexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_apredicatelist_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_valpredicatevector_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_apredicatelist(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valpredicatevector(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="valpredicatevector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valpredicatevector_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_pexpression(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="pexpression";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case '?':
if (m_term(lret,&subtree) || m_predicatevariable(lret,&subtree)) 
err=false;
break;
case '(':
if (m_tuple(lret,&subtree)) 
err=false;
break;
case '[':
if (m_pintentionvect(lret,&subtree) || m_valpredicatevector(lret,&subtree)) 
err=false;
break;
case '{':
if (m_valmappredicate(lret,&subtree)) 
err=false;
break;
case 34:
if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
err=false;
break;
case 39:
if (m_astringsimple(lret,&subtree)) 
err=false;
break;
case '0':
if (m_xnumber(lret,&subtree) || m_anumber(lret,&subtree)) 
err=false;
break;
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '-':
if (m_anumber(lret,&subtree)) 
err=false;
break;
case 't':
case 'f':
if (m_abool(lret,&subtree)) 
{err=false;
break;
}
default:
if (m_purecall(lret,&subtree) || m_purevariable(lret,&subtree)) 
err=false;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,false);
return(1);
}

char bnf_kif::m_predicateparameters_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateparameters(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_predicateparameters_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_predicateparameters_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_predicateparameters(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicateparameters";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_pexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateparameters_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_mapmerging(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="mapmerging";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_pexpression(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_mappredicatelist_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_mappredicatelist(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_mappredicatelist_0_1_2_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_mapmerging(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_mappredicatelist_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_mappredicatelist_0_1_2_3(lret,&subtree) || m_mappredicatelist_0_1_2_4(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_mappredicatelist_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_mappredicatelist_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_mappredicatelist(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="mappredicatelist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicatedico(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_mappredicatelist_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_valmappredicate_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_mappredicatelist(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valmappredicate(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="valmappredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,26);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_valmappredicate_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_predicatedico(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicatedico";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_pexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_pexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_predicateoperator(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicateoperator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab3))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_dboperation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="dboperation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring44,varstring44) || x_test_string(lret,ivarstring45,varstring45) || x_test_string(lret,ivarstring46,varstring46))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_predicatedatabase(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicatedatabase";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dboperation(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_predicateexpression_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_negation(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_predicateexpression_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateexpression_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicatecall(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_predicateexpression_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_fail(lret,&subtree) || m_cut(lret,&subtree) || m_assertpredicate(lret,&subtree) || m_predicateexpression_0_1_2(lret,&subtree) || m_affectationpredicate(lret,&subtree) || m_comparepredicate(lret,&subtree) || m_valpredicatevector(lret,&subtree) || m_valmappredicate(lret,&subtree) || m_expressions(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_predicateexpression_0_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicateexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateoperator(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,35);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_predicateexpression_0_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_predicateexpression_0_4_5(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_predicateexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicateexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateexpression_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateexpression_0_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_predicatedefinition_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_abool(lret,&subtree) || m_predicatedatabase(lret,&subtree) || m_predicateexpression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_predicatedefinition(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicatedefinition";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicate(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'-')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,20);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicatedefinition_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_predicatefact(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="predicatefact";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicate(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_dcgword(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="dcgword";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_predicate(lret,&subtree) || m_word(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_finaltoken_0_1_2_3(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case '?':
if (m_predicatevariable(lret,&subtree)) 
err=false;
break;
case 34:
if (m_astringdouble(lret,&subtree)) 
err=false;
break;
case 39:
if (m_astringsimple(lret,&subtree)) 
err=false;
break;
case '0':
if (m_xnumber(lret,&subtree) || m_anumber(lret,&subtree)) 
err=false;
break;
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '-':
if (m_anumber(lret,&subtree)) 
err=false;
break;
default:
if (m_astring(lret,&subtree)) 
err=false;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
lreturn+=lret;
return(1);
}

char bnf_kif::m_finaltoken_0_1_2_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_finaltoken_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_finaltoken_0_1_2_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_finaltoken_0_1_2_4_5(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_finaltoken_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_finaltoken_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_finaltoken_0_1_2_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_finaltoken_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_finaltoken_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_finaltoken(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="finaltoken";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_finaltoken_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_finaldcg(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="finaldcg";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_dcg_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_finaltoken(lret,&subtree) || m_dcgword(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_dcg_0_1_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dcgword(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_dcg_0_1_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_dcg_0_1_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_dcg_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dcg_0_1_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dcg_0_1_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_dcg_0_5_6(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_finaldcg(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_dcg_0_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_dcg_0_5_6(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_dcg(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="dcg";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dcgword(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'-')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'-')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,44);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_dcg_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dcg_0_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskelltail_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab4))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_haskelltail(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskelltail";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskelltail_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskellexpression(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="haskellexpression";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case '{':
if (m_haskellmap(lret,&subtree)) 
err=false;
break;
case '[':
if (m_haskellvector(lret,&subtree)) 
err=false;
break;
case 34:
if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
err=false;
break;
case 39:
if (m_astringsimple(lret,&subtree)) 
err=false;
break;
case '0':
if (m_xnumber(lret,&subtree) || m_anumber(lret,&subtree)) 
err=false;
break;
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '-':
if (m_anumber(lret,&subtree)) 
err=false;
break;
case 't':
case 'f':
if (m_abool(lret,&subtree)) 
{err=false;
break;
}
default:
if (m_word(lret,&subtree)) 
err=false;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,false);
return(1);
}

char bnf_kif::m_haskellvector_0_1_2_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_haskellvector_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_haskellvector_0_1_2_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_haskellvector_0_1_2_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_haskelltail(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_haskellvector_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellvector_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellvector_0_1_2_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_haskellvector_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_haskellvector_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_haskellvector(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskellvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellvector_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskellmaptail(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskellmaptail";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskellkeymap(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="haskellkeymap";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case 34:
if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
err=false;
break;
case 39:
if (m_astringsimple(lret,&subtree)) 
err=false;
break;
case '0':
if (m_xnumber(lret,&subtree) || m_anumber(lret,&subtree)) 
err=false;
break;
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '-':
if (m_anumber(lret,&subtree)) 
err=false;
break;
case 't':
case 'f':
if (m_abool(lret,&subtree)) 
{err=false;
break;
}
default:
if (m_word(lret,&subtree)) 
err=false;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,false);
return(1);
}

char bnf_kif::m_haskelldico(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskelldico";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellkeymap(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskellmap_0_1_2_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskelldico(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_haskellmap_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_haskellmap_0_1_2_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_haskellmap_0_1_2_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_haskellmaptail(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_haskellmap_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskelldico(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellmap_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellmap_0_1_2_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_haskellmap_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_haskellmap_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_haskellmap(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskellmap";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellmap_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_letkeyword(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="letkeyword";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring47,varstring47) || x_test_string(lret,ivarstring48,varstring48))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_let_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hmetafunctions(lret,&subtree) || m_nocomparisonexpressions(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_let(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="let";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_letkeyword(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectation_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_let_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_letmin_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hmetafunctions(lret,&subtree) || m_nocomparisonnorinexpressions(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_letmin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="letmin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_letmin_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_range_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectation_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'-')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_let_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_range(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="range";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_let(lret,&subtree) || m_range_0_1(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskell_0_1(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case '{':
if (m_haskellmap(lret,&subtree)) 
err=false;
break;
case '[':
if (m_haskellvector(lret,&subtree)) 
err=false;
break;
case 34:
if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
err=false;
break;
case 39:
if (m_astringsimple(lret,&subtree)) 
err=false;
break;
case '0':
if (m_xnumber(lret,&subtree) || m_anumber(lret,&subtree)) 
err=false;
break;
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '-':
if (m_anumber(lret,&subtree)) 
err=false;
break;
case 't':
case 'f':
if (m_abool(lret,&subtree)) 
{err=false;
break;
}
default:
if (m_word(lret,&subtree)) 
err=false;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
return(1);
}

char bnf_kif::m_haskell_0_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskell_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_haskell_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (x_test_char(lret,')')) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_haskell_0_2_3(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_haskell_0_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab5))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_haskell(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskell";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskell_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskell_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskell_0_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_otherwise(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="otherwise";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring49,varstring49)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_guard_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_otherwise(lret,&subtree) || m_guard(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_guard(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="guard";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,48);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_guard_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_hlambda_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_haskellmap(lret,&subtree) || m_haskellvector(lret,&subtree) || m_word(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_hlambda_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (x_test_char(lret,'-')) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_hlambda_0_1(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_hlambda(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hlambda";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'\\')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,54);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_hlambda_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_hlambda_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_hboollambda(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hboollambda";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'\\')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,54);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_hlambda_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_hlambda_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_hfunction_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_expression(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_hfunction(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hfunction";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_hfunction_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_lfold(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lfold";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring50,varstring50))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_rfold(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="rfold";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring51,varstring51))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_lscan(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lscan";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring52,varstring52))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_rscan(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="rscan";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring53,varstring53))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_fold(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="fold";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_lfold(lret,&subtree) || m_rfold(lret,&subtree) || m_rscan(lret,&subtree) || m_lscan(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_folding_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hlambda(lret,&subtree) || m_hfunction(lret,&subtree) || m_operator(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_folding_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hcompose(lret,&subtree) || m_hcomposecall(lret,&subtree) || m_expression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_folding(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="folding";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_fold(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,50);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_folding_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_folding_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskellbooloperator(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskellbooloperator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring3,varstring3) || x_test_string(lret,ivarstring2,varstring2))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_lfold1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lfold1";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring54,varstring54))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_rfold1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="rfold1";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring55,varstring55))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_lscan1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lscan1";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring56,varstring56))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_rscan1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="rscan1";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring57,varstring57))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_fold1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="fold1";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_lfold1(lret,&subtree) || m_rfold1(lret,&subtree) || m_rscan1(lret,&subtree) || m_lscan1(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_folding1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="folding1";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_fold1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,50);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_folding_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_folding_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_filterkeyword(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="filterkeyword";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring58,varstring58) || x_test_string(lret,ivarstring59,varstring59) || x_test_string(lret,ivarstring60,varstring60))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_takekeyword(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="takekeyword";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring61,varstring61) || x_test_string(lret,ivarstring62,varstring62))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_inverted(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="inverted";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_operator(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_mapping_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hlambda(lret,&subtree) || m_operation(lret,&subtree) || m_inverted(lret,&subtree) || m_hfunction(lret,&subtree) || m_operator(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_mapping(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="mapping";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring63,varstring63)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,51);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_mapping_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_folding_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_filtering_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hboollambda(lret,&subtree) || m_comparison(lret,&subtree) || m_operationin(lret,&subtree) || m_hfunction(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_filtering(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="filtering";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_filterkeyword(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,49);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_filtering_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_folding_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskellbooling(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskellbooling";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellbooloperator(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,56);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_filtering_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_folding_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_taking(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="taking";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_takekeyword(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,49);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_folding_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_zipping_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hlambda(lret,&subtree) || m_operator(lret,&subtree) || m_hfunction(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_zipping_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_expression(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_expression(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_zipping(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="zipping";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring64,varstring64)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_zipping_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_zipping_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_pairing(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="pairing";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring65,varstring65)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_zipping_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_cycle(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="cycle";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring66,varstring66) || x_test_string(lret,ivarstring67,varstring67))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_cycling(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="cycling";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_cycle(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,52);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_repeating(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="repeating";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring68,varstring68)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,53);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_flipping(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="flipping";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring69,varstring69)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,57);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_mapping_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskellcaseotherwise(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskellcaseotherwise";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring49,varstring49)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskellresult_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_valmaptail(lret,&subtree) || m_valvectortail(lret,&subtree) || m_expression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_haskellresult(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskellresult";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellresult_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'-')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_haskellcase_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellresult(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_haskellcase_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_haskellcase_0_1_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_haskellcase_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_haskellcaseotherwise(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_haskellcase(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="haskellcase";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring70,varstring70)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,55);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring71,varstring71)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellresult(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellcase_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_haskellcase_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_hfunctioncall_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_operator(lret,&subtree) || m_word(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_hfunctioncall_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_folding_0_2(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_folding_0_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_hfunctioncall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hfunctioncall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_hfunctioncall_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_hfunctioncall_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_hoperator(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hoperator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab6))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_hmetafunctions(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static map<string,int> keys;
static bool init=false;
if (!init) {
keys["foldl"]=1;
keys["foldl1"]=0;
keys["scanr1"]=0;
keys["scanr"]=1;
keys["foldr1"]=0;
keys["foldr"]=1;
keys["scanl1"]=0;
keys["filter"]=2;
keys["and"]=3;
keys["scanl"]=1;
keys["takeWhile"]=2;
keys["flip"]=4;
keys["dropWhile"]=2;
keys["take"]=5;
keys["drop"]=5;
keys["map"]=6;
keys["zipWith"]=7;
keys["zip"]=8;
keys["repeat"]=9;
keys["cycle"]=9;
keys["replicate"]=10;
keys["or"]=3;
init=true;
}
static const char* label="hmetafunctions";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
string x=fx->stack[currentpos];
if (keys.find(x)==keys.end())
return 0;
int i=keys[x];
bool err = true;
switch (i) {
case 1:
if (m_folding(lret,&subtree)) 
err=false;
break;
case 0:
if (m_folding1(lret,&subtree)) 
err=false;
break;
case 6:
if (m_mapping(lret,&subtree)) 
err=false;
break;
case 2:
if (m_filtering(lret,&subtree)) 
err=false;
break;
case 3:
if (m_haskellbooling(lret,&subtree)) 
err=false;
break;
case 5:
if (m_taking(lret,&subtree)) 
err=false;
break;
case 7:
if (m_zipping(lret,&subtree)) 
err=false;
break;
case 8:
if (m_pairing(lret,&subtree)) 
err=false;
break;
case 9:
if (m_cycling(lret,&subtree)) 
err=false;
break;
case 10:
if (m_repeating(lret,&subtree)) 
err=false;
break;
case 4:
if (m_flipping(lret,&subtree)) 
err=false;
break;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,false);
return(1);
}

char bnf_kif::m_hcompose_0_1(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static map<string,int> keys;
static bool init=false;
if (!init) {
keys["foldl"]=1;
keys["foldl1"]=0;
keys["scanr1"]=0;
keys["scanr"]=1;
keys["foldr1"]=0;
keys["foldr"]=1;
keys["scanl1"]=0;
keys["filter"]=2;
keys["and"]=3;
keys["scanl"]=1;
keys["takeWhile"]=2;
keys["flip"]=4;
keys["dropWhile"]=2;
keys["take"]=5;
keys["drop"]=5;
keys["map"]=6;
keys["zipWith"]=7;
keys["zip"]=8;
keys["repeat"]=9;
keys["cycle"]=9;
keys["replicate"]=10;
keys["or"]=3;
init=true;
}
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
string x=fx->stack[currentpos];
if (keys.find(x)==keys.end())
return 0;
int i=keys[x];
bool err = true;
switch (i) {
case 6:
if (m_mapping(lret,&subtree)) 
err=false;
break;
case 2:
if (m_filtering(lret,&subtree)) 
err=false;
break;
case 3:
if (m_haskellbooling(lret,&subtree)) 
err=false;
break;
case 5:
if (m_taking(lret,&subtree)) 
err=false;
break;
case 7:
if (m_zipping(lret,&subtree)) 
err=false;
break;
case 8:
if (m_pairing(lret,&subtree)) 
err=false;
break;
case 1:
if (m_folding(lret,&subtree)) 
err=false;
break;
case 0:
if (m_folding1(lret,&subtree)) 
err=false;
break;
case 9:
if (m_cycling(lret,&subtree)) 
err=false;
break;
case 10:
if (m_repeating(lret,&subtree)) 
err=false;
break;
case 4:
if (m_flipping(lret,&subtree)) 
err=false;
break;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
return(1);
}

char bnf_kif::m_hcompose(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hcompose";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_hcompose_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_hcomposecall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hcomposecall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_hfunctioncall(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_hoper(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="hoper";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case '&':
if (m_operator_0_1(lret,&subtree) || x_test_in(lret,tab7)) 
err=false;
break;
case ':':
if (m_operator_0_2(lret,&subtree)) 
err=false;
break;
case '<':
if (m_operator_0_3(lret,&subtree)) 
err=false;
break;
case '>':
if (m_operator_0_4(lret,&subtree)) 
err=false;
break;
case '^':
if (m_operator_0_5(lret,&subtree) || x_test_in(lret,tab7)) 
err=false;
break;
case '+':
if (x_test_in(lret,tab7)) 
err=false;
break;
case '-':
if (x_test_in(lret,tab7)) 
err=false;
break;
case '*':
if (x_test_in(lret,tab7)) 
err=false;
break;
case '/':
if (x_test_in(lret,tab7)) 
err=false;
break;
case '%':
if (x_test_in(lret,tab7)) 
err=false;
break;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,true);
return(1);
}

char bnf_kif::m_hoperation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hoperation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_hoper(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_hexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_hexpression(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
static const char* label="hexpression";
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case 34:
if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
err=false;
break;
case 39:
if (m_astringsimple(lret,&subtree)) 
err=false;
break;
case '(':
if (m_valtuple(lret,&subtree) || m_optional(lret,&subtree)) 
err=false;
break;
case '<':
if (m_telque(lret,&subtree)) 
err=false;
break;
case '[':
if (m_intentionvect(lret,&subtree) || m_valvector(lret,&subtree)) 
err=false;
break;
case '{':
if (m_valmap(lret,&subtree)) 
err=false;
break;
case '0':
if (m_xnumber(lret,&subtree) || m_anumber(lret,&subtree)) 
err=false;
break;
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '-':
if (m_anumber(lret,&subtree)) 
err=false;
break;
case 'c':
if (m_haskellcase(lret,&subtree) || m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
err=false;
break;
case 'i':
if (m_localif(lret,&subtree) || m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
err=false;
break;
case 't':
case 'f':
if (m_abool(lret,&subtree) || m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
err=false;
break;
default:
if (m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
err=false;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
x_init_node(lreturn,lret,tree,label,pos,false);
return(1);
}

char bnf_kif::m_hexpressions_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_hoperation(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_hexpressions(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hexpressions";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_hexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_hexpressions_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_whereexpression_0_1_2_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_whereexpression_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_whereexpression_0_1_2_3_4(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_whereexpression_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_whereexpression_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nocomparisonexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,';')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_whereexpression_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_whereexpression_0_1_2(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_whereexpression_0_1_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_whereexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="whereexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring72,varstring72)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_whereexpression_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_hinexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hinexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring32,varstring32)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nocomparisonexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_telque_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (x_test_char(lret,'@'))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_telque_0_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_haskell(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_telque_0_2_4_5_6_7_8(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab8))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_5_6_7(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_5_6_7_8(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_letmin(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_5_6(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_hinexpression(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_telque_0_2_4_5_6_7(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_5_9_10(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_whereexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_5_9(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_telque_0_2_4_5_9_10(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_telque_0_2_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring47,varstring47)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_letmin(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_5_9(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_12(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 long max=0;
 vector<x_couple*> results;
 lret="";
 subtree=NULL;
 currentpos=pos;
	intoken=itok;
 if (m_hexpressions(lret,&subtree)) {
   results.push_back(new x_couple(currentpos,intoken,subtree,lret));
    if (max<currentpos) {
        max=currentpos;
    }
 }
 gFail=0;
 lret="";
 subtree=NULL;
 currentpos=pos;
	intoken=itok;
 if (m_hfunctioncall(lret,&subtree)) {
   results.push_back(new x_couple(currentpos,intoken,subtree,lret));
    if (max<currentpos) {
        max=currentpos;
    }
 }
 gFail=0;
 if (results.size()==0) {
     currentpos=pos;
	intoken=itok;
     setfail(exitonfail);
     return 0;
  }
 char add=0;
 for (int i=0;i<results.size();i++) {
    if (add==0 && max==results[i]->pos) {
        add=1;
        currentpos=results[i]->pos;
	intoken=results[i]->intoken;
        results[i]->set(lret);
        x_init_tree(tree,results[i]->tree,addsubtree);
     }
     else 
       delete results[i]->tree;
     delete results[i];
  }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13_14_15(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_char(lret,'|') || x_test_string(lret,ivarstring72,varstring72))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13_14_16_17_18_19(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_hoperator(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_range(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13_14_16_17_18(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_telque_0_2_4_11_13_14_16_17_18_19(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13_14_16_17_20_21_22_23(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_5_9(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13_14_16_17_20_21_22(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_whereexpression(lret,&subtree) || m_telque_0_2_4_11_13_14_16_17_20_21_22_23(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13_14_16_17_20_21(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_11_13_14_16_17_20_21_22(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13_14_16_17_20(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_telque_0_2_4_11_13_14_16_17_20_21(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13_14_16_17(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_range(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_11_13_14_16_17_18(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_11_13_14_16_17_20(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13_14_16(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_telque_0_2_4_11_13_14_16_17(lret,&subtree) || m_whereexpression(lret,&subtree) || m_booleanexpression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13_14(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_11_13_14_15(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_11_13_14_16(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11_13(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_telque_0_2_4_11_13_14(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_telque_0_2_4_11(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_11_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4_11_13(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque_0_2_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hmetafunctions(lret,&subtree) || m_guard(lret,&subtree) || m_telque_0_2_4_5(lret,&subtree) || m_telque_0_2_4_11(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_telque_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_telque(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="telque";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,48);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_optional(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="optional";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,4);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_expression_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_negation(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_expression_0_2(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case 34:
if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
err=false;
break;
case 39:
if (m_astringsimple(lret,&subtree)) 
err=false;
break;
case '(':
if (m_valtuple(lret,&subtree) || m_optional(lret,&subtree)) 
err=false;
break;
case '<':
if (m_telque(lret,&subtree)) 
err=false;
break;
case '[':
if (m_intentionvect(lret,&subtree) || m_valvector(lret,&subtree)) 
err=false;
break;
case '{':
if (m_valmap(lret,&subtree)) 
err=false;
break;
case '0':
if (m_xnumber(lret,&subtree) || m_anumber(lret,&subtree)) 
err=false;
break;
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '-':
if (m_anumber(lret,&subtree)) 
err=false;
break;
case 'c':
if (m_haskellcase(lret,&subtree) || m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
err=false;
break;
case 'i':
if (m_localif(lret,&subtree) || m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
err=false;
break;
case 't':
case 'f':
if (m_abool(lret,&subtree) || m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
err=false;
break;
default:
if (m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
err=false;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
lreturn+=lret;
return(1);
}

char bnf_kif::m_expression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="expression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_wnexpressions_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_anumber(lret,&subtree) || m_negated(lret,&subtree) || m_expression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_wnexpressions_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions_0_1_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectexpression_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_wnexpressions(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="wnexpressions";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_evallambda(lret,&subtree) || m_wnexpressions_0_1(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_nocomparisonexpressions_0_1_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_operation(lret,&subtree) || m_operationin(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_nocomparisonexpressions_0_1_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_nocomparisonexpressions_0_1_3_4(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_nocomparisonexpressions_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions_0_1_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nocomparisonexpressions_0_1_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_nocomparisonexpressions(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="nocomparisonexpressions";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_evallambda(lret,&subtree) || m_nocomparisonexpressions_0_1(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_nocomparisonnorinexpressions_0_1_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_operation(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_nocomparisonnorinexpressions_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions_0_1_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nocomparisonnorinexpressions_0_1_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_nocomparisonnorinexpressions(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="nocomparisonnorinexpressions";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_evallambda(lret,&subtree) || m_nocomparisonnorinexpressions_0_1(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_expressions_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectexpression_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_expressions(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="expressions";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_evallambda(lret,&subtree) || m_expressions_0_1(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_numbers_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_operation(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_numbers(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="numbers";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_numbers_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_negated(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="negated";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'-')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_xnumber_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_ord(lret,'0','9') || x_test_ord(lret,'A','F') || x_test_ord(lret,'a','f'))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_xnumber_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_xnumber_0_1_2(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_xnumber_0_1_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_xnumber(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="xnumber";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'0')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'x')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_xnumber_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_abool(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="abool";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring73,varstring73) || x_test_string(lret,ivarstring74,varstring74))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_blocfor_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_affectation(lret,&subtree) || m_expression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_blocfor_0_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocfor(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_blocfor_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_blocfor_0_2_3(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_blocfor(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="blocfor";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocfor_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocfor_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_localif_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring76,varstring76)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_localif_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_localif_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_localif(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="localif";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring75,varstring75)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,10);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_localif_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_declarationfor_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring32,varstring32)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_declarationfor_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_type(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring32,varstring32)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_declarationfor(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="declarationfor";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_declarationfor_0_1(lret,&subtree) || m_declarationfor_0_2(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_optionalboolean(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="optionalboolean";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_booleanexpression_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 long max=0;
 vector<x_couple*> results;
 lret="";
 subtree=NULL;
 currentpos=pos;
	intoken=itok;
 if (m_optionalboolean(lret,&subtree)) {
   results.push_back(new x_couple(currentpos,intoken,subtree,lret));
    if (max<currentpos) {
        max=currentpos;
    }
 }
 gFail=0;
 lret="";
 subtree=NULL;
 currentpos=pos;
	intoken=itok;
 if (m_expressions(lret,&subtree)) {
   results.push_back(new x_couple(currentpos,intoken,subtree,lret));
    if (max<currentpos) {
        max=currentpos;
    }
 }
 gFail=0;
 if (results.size()==0) {
     currentpos=pos;
	intoken=itok;
     setfail(exitonfail);
     return 0;
  }
 char add=0;
 for (int i=0;i<results.size();i++) {
    if (add==0 && max==results[i]->pos) {
        add=1;
        currentpos=results[i]->pos;
	intoken=results[i]->intoken;
        results[i]->set(lret);
        x_init_tree(tree,results[i]->tree,addsubtree);
     }
     else 
       delete results[i]->tree;
     delete results[i];
  }
 return(1);
 }


char bnf_kif::m_booleanexpression_0_2_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 long max=0;
 vector<x_couple*> results;
 lret="";
 subtree=NULL;
 currentpos=pos;
	intoken=itok;
 if (m_optionalboolean(lret,&subtree)) {
   results.push_back(new x_couple(currentpos,intoken,subtree,lret));
    if (max<currentpos) {
        max=currentpos;
    }
 }
 gFail=0;
 lret="";
 subtree=NULL;
 currentpos=pos;
	intoken=itok;
 if (m_booleanexpression(lret,&subtree)) {
   results.push_back(new x_couple(currentpos,intoken,subtree,lret));
    if (max<currentpos) {
        max=currentpos;
    }
 }
 gFail=0;
 if (results.size()==0) {
     currentpos=pos;
	intoken=itok;
     setfail(exitonfail);
     return 0;
  }
 char add=0;
 for (int i=0;i<results.size();i++) {
    if (add==0 && max==results[i]->pos) {
        add=1;
        currentpos=results[i]->pos;
	intoken=results[i]->intoken;
        results[i]->set(lret);
        x_init_tree(tree,results[i]->tree,addsubtree);
     }
     else 
       delete results[i]->tree;
     delete results[i];
  }
 return(1);
 }


char bnf_kif::m_booleanexpression_0_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="booleanexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_orand(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,8);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression_0_2_3_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_booleanexpression_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_booleanexpression_0_2_3(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_booleanexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="booleanexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_arange(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="arange";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,41);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_wnexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_test_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_instruction(lret,&subtree) || m_complexbloc(lret,&subtree) || m_bloc(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_test_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_testelif(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_test_0_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="test";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring76,varstring76)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,24);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_test_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_test_0_3_4(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_test(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="test";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring75,varstring75)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,10);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_testelif(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="testelif";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring77,varstring77)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,10);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_loop(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="loop";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring78,varstring78)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,11);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_doloop(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="doloop";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring79,varstring79)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring78,varstring78)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,11);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declarationending(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_for_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_multideclaration(lret,&subtree) || m_blocfor(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_for(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="for";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring80,varstring80)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,12);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_for_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,';')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,';')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocfor(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_forin_0_1_2_3(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case '{':
if (m_valmaptail(lret,&subtree)) 
err=false;
break;
case '[':
if (m_valvectortail(lret,&subtree)) 
err=false;
break;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
return(1);
}

char bnf_kif::m_forin_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_forin_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring32,varstring32)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_forin_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_forin_0_1_2(lret,&subtree) || m_declarationfor(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_forin_0_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_arange(lret,&subtree) || m_expressions(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_forin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="forin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring80,varstring80)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_forin_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,33);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_forin_0_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_switch_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (x_test_char(lret,'}')) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
	intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_testswitch(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 int foundsolution=0;
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (x_test_char(lret,'}')) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_testswitch(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_switch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="switch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring81,varstring81)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,16);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_0_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,31);
 bnf_kif_pop pop1(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_switch_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_trycatch_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_word(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_trycatch_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_trycatch_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_trycatch_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_trycatch_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_trycatch_0_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="trycatch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,22);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocs(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_trycatch_0_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_char(lret,';') || m_trycatch_0_4_5(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_trycatch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="trycatch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring82,varstring82)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,22);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocs(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring83,varstring83)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,22);
 bnf_kif_pop pop1(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_trycatch_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_trycatch_0_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_testswitch_0_1(string& lreturn,x_node** tree) {
if (gFail==1) return(0);
string lret;
long pos=currentpos;
int itok=intoken;
x_node* subtree=NULL;
int addsubtree=0;
bool exitonfail=false;
//BODYOR
subtree=NULL;

if (currentpos>=fx->stack.size())
     return 0;
unsigned char x = fx->stack[currentpos][intoken];
bool err = true;
switch (x) {case 34:
if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
err=false;
break;
case 39:
if (m_astringsimple(lret,&subtree)) 
err=false;
break;
case '0':
if (m_xnumber(lret,&subtree) || m_anumber(lret,&subtree)) 
err=false;
break;
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '-':
if (m_anumber(lret,&subtree)) 
err=false;
break;
case 't':
case 'f':
if (m_abool(lret,&subtree)) 
{err=false;
break;
}
default:
if (m_default(lret,&subtree)) 
err=false;
}
if (err==false)
x_init_tree(tree,subtree,addsubtree);
else {
x_pop_node(tree,addsubtree);
currentpos=pos;
intoken=itok;
setfail(exitonfail);
return 0;
}
return(1);
}

char bnf_kif::m_testswitch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="testswitch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_testswitch_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,17);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_default(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="default";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring84,varstring84))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_lispexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lispexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_xnumber(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_astring(lret,&subtree) || m_punct(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_lispexpressions(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lispexpressions";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_onelisp(lret,&subtree) || m_lispexpression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_onelisp_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (x_test_char(lret,Y_var_1)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_lispexpressions(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_onelisp(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="onelisp";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,Y_var_0)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_lispexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,42);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_onelisp_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_lisp_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_onelisp(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_lisp(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lisp";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_onelisp(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_lisp_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_tagexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="tagexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_xnumber(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_astring(lret,&subtree) || m_punct(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_tagexpressions(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="tagexpressions";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_onetag(lret,&subtree) || m_tagexpression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_onetag_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (x_test_string(lret,VS_var_3.size(),VS_var_3)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_tagexpressions(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_onetag(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="onetag";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,VS_var_2.size(),VS_var_2)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_tagexpressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,43);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_onetag_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_tag_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_onetag(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_tag(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="tag";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_onetag(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_tag_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_ameta_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab9))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_ameta_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="ameta";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'%')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,currentpos,59);
 bnf_kif_pop pop0(this);
//BODYSEQUENCE
 subtree=NULL;
 if (m_ameta_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_ameta_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_char(lret,'?') || m_ameta_0_1_2(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_ameta_0_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (x_test_char(lret,'_'))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_ameta(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="ameta";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_ameta_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ameta_0_4(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_amulti(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="amulti";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'$')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_asimple(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="asimple";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_word(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_atoken_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_subgram(lret,&subtree) || m_ameta(lret,&subtree) || m_xnumber(lret,&subtree) || m_amulti(lret,&subtree) || m_astringdouble(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_abool(lret,&subtree) || m_word(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_atoken(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="atoken";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateexpression_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_atoken_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_anitem(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="anitem";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_ameta(lret,&subtree) || m_xnumber(lret,&subtree) || m_amulti(lret,&subtree) || m_astringdouble(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_abool(lret,&subtree) || m_asimple(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_dj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="dj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,';'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_aoptional(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="aoptional";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_atoken(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_aoptitem(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="aoptitem";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_anitem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_akleene(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="akleene";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab10))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,true);
 return(1);
}


char bnf_kif::m_anelement_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_akleene(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_anelement_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_atoken(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_anelement_0_1_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_anelement(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="anelement";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_aoptional(lret,&subtree) || m_anelement_0_1(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_anitemelement_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_anitem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_anelement_0_1_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_anitemelement(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="anitemelement";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_aoptitem(lret,&subtree) || m_anitemelement_0_1(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_body_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_dj(lret,&subtree) || x_test_char(lret,','))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   currentpos=pos;
	intoken=itok;
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_kif::m_body_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_body_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_anitemelement(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_body_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_body_0_1_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_body(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="body";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_anitemelement(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_body_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_rule_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_body_0_1_2_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_anelement(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_rule_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (x_test_char(lret,'.')) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_rule_0_1_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_rule(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="rule";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_anelement(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_rule_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_subgram_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_body(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_subgram_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_subgram_0_1_2_3(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_subgram_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (x_test_char(lret,']')) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_subgram_0_1_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_kif::m_subgram(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="subgram";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_body(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_subgram_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kif::m_rules(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="rules";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_rule(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_rule(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}




string bnf_kif::x_errormsg(int i) {
	return errorlabels[i];
}


x_node* bnf_kif::x_parsing(x_reading* xr,x_parsing_mode mode,bool display) {
        fx=xr;
	string lret;
	x_node* tree=new x_node;
	gFail=0;
	lineerror=-1;
	labelerror="";
	errornumber=-1;
	currentpos=0;
	intoken=0;
	labelerrors.clear();
	lineerrors.clear();
	errornumbers.clear();

	char res=m_analyse(lret,&tree);

	if (currentpos!=fx->stack.size() && mode==FULL) {
            if (display) {
		if (errornumber!=-1) 
			cerr<<endl<<"Error line:"<<lineerror<<"="<<errorlabels[errornumber]<<endl;
		else
			cerr<<endl<<"Error line:"<<lineerror<<endl;
	    }
	    if (tree!=NULL)
		delete tree;
	    return NULL;
	}

	if (res==1)
		return tree;

	delete tree;
	return NULL;
}


char bnf_kif::x_test_string(string& lret,int sz,string sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}

char bnf_kif::x_test_pstring(string& lret,int sz,string& sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}

char bnf_kif::m_punct(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char* label="punct";
	if (fx->stacktype[currentpos]==0) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_kif::m_afullstring(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char* label="afullstring";
	if (fx->stacktype[currentpos]==5) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_kif::m_word(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char* label="word";
	if (fx->stacktype[currentpos]==4) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_kif::m_typename(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char* label="typename";
	if (fx->stacktype[currentpos]==4) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_kif::m_astring(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char* label="astring";
	if (fx->stacktype[currentpos]==4) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_kif::m_anumber(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char* label="anumber";
	if (fx->stacktype[currentpos]==3) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	if ((fx->stack[currentpos]=="-" || fx->stack[currentpos]=="+") && fx->stacktype[currentpos+1]==3) {
		string s=fx->stack[currentpos];
		s+=fx->stack[currentpos+1];
		x_init_node(lreturn,s,tree,label,currentpos,true);
		currentpos+=2;
		return(1);
	}
	return(0);
}

char bnf_kif::m_astringsimple(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char* label="astringsimple";
	if (fx->stacktype[currentpos]==2) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_kif::m_astringdouble(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char* label="astringdouble";
	if (fx->stacktype[currentpos]==1) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}

