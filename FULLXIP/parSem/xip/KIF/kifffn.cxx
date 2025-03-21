#include "kifffn.h"


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
"Wrong FUNCTIONAL description"};

static char tab0[]={'_','#',0};
static char tab1[]={'&','|',0};
static char tab2[]={'+','-','*','/',0};
static char tab3[]={'%','^',0};
static char tab4[]={'<','>',0};
static char tab5[]={',',';',0};
static char tab6[]={',','&',0};
const char* varstring6="and";
const char* varstring12="exclusive";
const char* varstring65="elif";
const char* varstring64="false";
const char* varstring48="foldl";
const char* varstring46="iterator";
const char* varstring67="do";
const char* varstring40="assert";
const char* varstring3="is";
const char* varstring0="private";
const char* varstring35="fail";
const char* varstring23="kIf";
const char* varstring8="frame";
const char* varstring31="in";
const char* varstring13="polynomial";
const char* varstring53="foldr1";
const char* varstring22="KiF";
const char* varstring56="takeWhile";
const char* varstring50="scanl";
const char* varstring57="filter";
const char* varstring51="scanr";
const char* varstring2="const";
const char* varstring68="for";
const char* varstring63="true";
const char* varstring42="get";
const char* varstring66="while";
const char* varstring18="kif";
const char* varstring9="strict";
const char* varstring29="continue";
const char* varstring59="zipWith";
const char* varstring15="autorun";
const char* varstring36="retract";
const char* varstring49="foldr";
const char* varstring61="if";
const char* varstring19="Kif";
const char* varstring43="store";
const char* varstring14="function";
const char* varstring58="map";
const char* varstring30="notin";
const char* varstring26="return";
const char* varstring55="scanr1";
const char* varstring34="on";
const char* varstring52="foldl1";
const char* varstring47="otherwise";
const char* varstring28="break";
const char* varstring25="kIF";
const char* varstring4="not";
const char* varstring33="from";
const char* varstring71="catch";
const char* varstring24="kiF";
const char* varstring7="with";
const char* varstring20="KIf";
const char* varstring37="retractdb";
const char* varstring32="absent";
const char* varstring38="asserta";
const char* varstring16="thread";
const char* varstring69="switch";
const char* varstring70="try";
const char* varstring27="_return";
const char* varstring10="joined";
const char* varstring44="remove";
const char* varstring5="or";
const char* varstring41="assertdb";
const char* varstring72="default";
const char* varstring62="else";
const char* varstring11="protected";
const char* varstring1="common";
const char* varstring45="let";
const char* varstring54="scanl1";
const char* varstring39="assertz";
const char* varstring60="where";
const char* varstring21="KIF";
const char* varstring17="lambda";
const int ivarstring6=strlen(varstring6);
const int ivarstring12=strlen(varstring12);
const int ivarstring65=strlen(varstring65);
const int ivarstring64=strlen(varstring64);
const int ivarstring48=strlen(varstring48);
const int ivarstring46=strlen(varstring46);
const int ivarstring67=strlen(varstring67);
const int ivarstring40=strlen(varstring40);
const int ivarstring3=strlen(varstring3);
const int ivarstring0=strlen(varstring0);
const int ivarstring35=strlen(varstring35);
const int ivarstring23=strlen(varstring23);
const int ivarstring8=strlen(varstring8);
const int ivarstring31=strlen(varstring31);
const int ivarstring13=strlen(varstring13);
const int ivarstring53=strlen(varstring53);
const int ivarstring22=strlen(varstring22);
const int ivarstring56=strlen(varstring56);
const int ivarstring50=strlen(varstring50);
const int ivarstring57=strlen(varstring57);
const int ivarstring51=strlen(varstring51);
const int ivarstring2=strlen(varstring2);
const int ivarstring68=strlen(varstring68);
const int ivarstring63=strlen(varstring63);
const int ivarstring42=strlen(varstring42);
const int ivarstring66=strlen(varstring66);
const int ivarstring18=strlen(varstring18);
const int ivarstring9=strlen(varstring9);
const int ivarstring29=strlen(varstring29);
const int ivarstring59=strlen(varstring59);
const int ivarstring15=strlen(varstring15);
const int ivarstring36=strlen(varstring36);
const int ivarstring49=strlen(varstring49);
const int ivarstring61=strlen(varstring61);
const int ivarstring19=strlen(varstring19);
const int ivarstring43=strlen(varstring43);
const int ivarstring14=strlen(varstring14);
const int ivarstring58=strlen(varstring58);
const int ivarstring30=strlen(varstring30);
const int ivarstring26=strlen(varstring26);
const int ivarstring55=strlen(varstring55);
const int ivarstring34=strlen(varstring34);
const int ivarstring52=strlen(varstring52);
const int ivarstring47=strlen(varstring47);
const int ivarstring28=strlen(varstring28);
const int ivarstring25=strlen(varstring25);
const int ivarstring4=strlen(varstring4);
const int ivarstring33=strlen(varstring33);
const int ivarstring71=strlen(varstring71);
const int ivarstring24=strlen(varstring24);
const int ivarstring7=strlen(varstring7);
const int ivarstring20=strlen(varstring20);
const int ivarstring37=strlen(varstring37);
const int ivarstring32=strlen(varstring32);
const int ivarstring38=strlen(varstring38);
const int ivarstring16=strlen(varstring16);
const int ivarstring69=strlen(varstring69);
const int ivarstring70=strlen(varstring70);
const int ivarstring27=strlen(varstring27);
const int ivarstring10=strlen(varstring10);
const int ivarstring44=strlen(varstring44);
const int ivarstring5=strlen(varstring5);
const int ivarstring41=strlen(varstring41);
const int ivarstring72=strlen(varstring72);
const int ivarstring62=strlen(varstring62);
const int ivarstring11=strlen(varstring11);
const int ivarstring1=strlen(varstring1);
const int ivarstring45=strlen(varstring45);
const int ivarstring54=strlen(varstring54);
const int ivarstring39=strlen(varstring39);
const int ivarstring60=strlen(varstring60);
const int ivarstring21=strlen(varstring21);
const int ivarstring17=strlen(varstring17);


char bnf_kifff::m_char(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_digit(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="digit";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_ord(lret,'0','9'))
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


char bnf_kifff::m_path_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_path(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_shell_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_shell(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_signature(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_label(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_private(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="private";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring0,varstring0))
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


char bnf_kifff::m_common(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="common";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring1,varstring1))
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


char bnf_kifff::m_const(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="const";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring2,varstring2))
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


char bnf_kifff::m_feature_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_feature_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_feature_0_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_feature(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_type_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_type(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_anydouble_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_anydouble_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_anydouble(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_anysimple(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_anycolon_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_anycolon_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_anycolon(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operator_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operator_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operator_0_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operator_0_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operator_0_5(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operator(string& lreturn,x_node** tree) {
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
 if (m_operator_0_1(lret,&subtree) || m_operator_0_2(lret,&subtree) || x_test_in(lret,tab1) || m_operator_0_3(lret,&subtree) || m_operator_0_4(lret,&subtree) || x_test_in(lret,tab2) || m_operator_0_5(lret,&subtree) || x_test_in(lret,tab3))
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


char bnf_kifff::m_operatoraffectation_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (x_test_string(lret,ivarstring4,varstring4))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kifff::m_operatoraffectation_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring3,varstring3)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_operatoraffectation_0_5(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_6(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_7(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_8(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_9(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_10(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_11(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_12(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_13(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_14(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_15(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation_0_16(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operatoraffectation(string& lreturn,x_node** tree) {
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
 if (m_operatoraffectation_0_1(lret,&subtree) || m_operatoraffectation_0_2(lret,&subtree) || m_operatoraffectation_0_3(lret,&subtree) || x_test_char(lret,'=') || m_operatoraffectation_0_5(lret,&subtree) || m_operatoraffectation_0_6(lret,&subtree) || m_operatoraffectation_0_7(lret,&subtree) || m_operatoraffectation_0_8(lret,&subtree) || m_operatoraffectation_0_9(lret,&subtree) || m_operatoraffectation_0_10(lret,&subtree) || m_operatoraffectation_0_11(lret,&subtree) || m_operatoraffectation_0_12(lret,&subtree) || m_operatoraffectation_0_13(lret,&subtree) || m_operatoraffectation_0_14(lret,&subtree) || m_operatoraffectation_0_15(lret,&subtree) || m_operatoraffectation_0_16(lret,&subtree))
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


char bnf_kifff::m_orand_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_orand_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_orand(string& lreturn,x_node** tree) {
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
 if (m_orand_0_1(lret,&subtree) || m_orand_0_2(lret,&subtree) || x_test_string(lret,ivarstring5,varstring5) || x_test_string(lret,ivarstring6,varstring6))
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


char bnf_kifff::m_comparator_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_comparator_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_comparator_0_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSYMBOL
 if (!x_test_char(lret,'<') || !x_test_char(lret,'>')) {
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kifff::m_comparator_0_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_comparator_0_5(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_comparator_0_6(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_comparator(string& lreturn,x_node** tree) {
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
 if (m_comparator_0_1(lret,&subtree) || m_comparator_0_2(lret,&subtree) || m_comparator_0_3(lret,&subtree) || m_comparator_0_4(lret,&subtree) || m_comparator_0_5(lret,&subtree) || m_comparator_0_6(lret,&subtree) || x_test_in(lret,tab4))
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


char bnf_kifff::m_depend(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_declaration_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_declaration_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_declaration_0_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_declaration_0_4_5(string& lreturn,x_node** tree) {
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
 if (m_expressions(lret,&subtree)) 
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


char bnf_kifff::m_declaration_0_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_declaration(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_declarationlist_0_4_5(string& lreturn,x_node** tree) {
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
 return(1);
 }


char bnf_kifff::m_declarationlist_0_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_declarationlist_0_6_7(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_declarationlist_0_6(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_declarationlist(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_multideclaration(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_declarationending(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_frame_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_frame_0_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_frame(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_instruction_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_instruction(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_nonlimited(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_arguments_0_1_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_arguments_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_arguments_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_arguments(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_strict(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="strict";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring9,varstring9))
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


char bnf_kifff::m_join(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="join";
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


char bnf_kifff::m_protecclusive(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring11,varstring11) || x_test_string(lret,ivarstring12,varstring12))
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


char bnf_kifff::m_functionsort(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring13,varstring13) || x_test_string(lret,ivarstring14,varstring14) || x_test_string(lret,ivarstring15,varstring15) || x_test_string(lret,ivarstring16,varstring16))
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


char bnf_kifff::m_typefunction_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_typefunction_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_typefunction_0_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_typefunction_0_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_typefunction(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_indexname(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_intervalname(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_function_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_function_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_function(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_lambda(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring17,varstring17)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_sousbloc(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_bloc_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_bloc_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_bloc_0_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_bloc(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_complexbloc(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_blocs_0_1(string& lreturn,x_node** tree) {
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
 if (m_label(lret,&subtree) || m_frame(lret,&subtree) || m_complexbloc(lret,&subtree) || m_function(lret,&subtree) || m_bloc(lret,&subtree))
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


char bnf_kifff::m_blocs(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_analyse_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_analyse_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_analyse_0_3_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,ivarstring18,varstring18) || x_test_string(lret,ivarstring19,varstring19) || x_test_string(lret,ivarstring20,varstring20) || x_test_string(lret,ivarstring21,varstring21) || x_test_string(lret,ivarstring22,varstring22) || x_test_string(lret,ivarstring23,varstring23) || x_test_string(lret,ivarstring24,varstring24) || x_test_string(lret,ivarstring25,varstring25))
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


char bnf_kifff::m_analyse_0_3_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_analyse_0_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_analyse(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_subfunc(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_subfuncbis(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_subvar(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_framesep(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_frameup(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_functioncall_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_functioncall(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_regularcall_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_regularcall_0_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_regularcall_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_regularcall(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_returncall(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring26,varstring26) || x_test_string(lret,ivarstring27,varstring27))
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


char bnf_kifff::m_breakcall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="breakcall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring28,varstring28))
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


char bnf_kifff::m_continuecall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="continuecall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring29,varstring29))
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


char bnf_kifff::m_call(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_sep(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_indexes_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_indexes_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_indexes_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kifff::m_indexes(string& lreturn,x_node** tree) {
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
 if (m_indexes_0_1(lret,&subtree)) 
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


char bnf_kifff::m_interval_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_expressions(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kifff::m_interval(string& lreturn,x_node** tree) {
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
 if (m_indexes_0_1(lret,&subtree)) 
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


char bnf_kifff::m_variable_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_variable_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_variable(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_plusplus_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_plusplus_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_plusplus(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_affectation_0_1_2(string& lreturn,x_node** tree) {
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
 return(1);
 }


char bnf_kifff::m_affectation_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_plusplus(lret,&subtree) || m_affectation_0_1_2(lret,&subtree))
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


char bnf_kifff::m_affectation(string& lreturn,x_node** tree) {
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
 if (m_affectation_0_1(lret,&subtree)) 
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


char bnf_kifff::m_increment(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_notin_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring4,varstring4)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,ivarstring31,varstring31)) 
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


char bnf_kifff::m_notin_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
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
 if (x_test_string(lret,ivarstring33,varstring33)) 
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


char bnf_kifff::m_notin(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring30,varstring30) || m_notin_0_1(lret,&subtree) || m_notin_0_2(lret,&subtree))
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


char bnf_kifff::m_in(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="in";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring31,varstring31))
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


char bnf_kifff::m_on(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="on";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring34,varstring34))
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


char bnf_kifff::m_operatorin(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operationin_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operationin_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operationin(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operationon(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_operation(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_comparison(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_valvector_0_1_2_3_4(string& lreturn,x_node** tree) {
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
 if (m_expressions(lret,&subtree)) 
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


char bnf_kifff::m_valvector_0_1_2_3(string& lreturn,x_node** tree) {
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
   if (m_valvector_0_1_2_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kifff::m_valvector_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
//BODYSEQUENCE
 subtree=NULL;
 if (m_valvector_0_1_2_3(lret,&subtree)) 
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


char bnf_kifff::m_valvector_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_valvector_0_1_2(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kifff::m_valvector(string& lreturn,x_node** tree) {
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
//BODYSEQUENCE
 subtree=NULL;
 if (m_valvector_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_intentionsep(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_intentionvector_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_intentionvector_0_1_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_expression(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kifff::m_intentionvector_0_1_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_intentionvector_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_intentionvector(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_negation(string& lreturn,x_node** tree) {
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
 if (x_test_char(lret,'~') || x_test_string(lret,ivarstring4,varstring4))
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


char bnf_kifff::m_cut(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_fail(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="fail";
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


char bnf_kifff::m_predicate_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicate(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicatecall(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_retractcommand(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring36,varstring36) || x_test_string(lret,ivarstring37,varstring37))
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


char bnf_kifff::m_assertcommandbefore(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="assertcommandbefore";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring38,varstring38))
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


char bnf_kifff::m_assertcommandafter(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring39,varstring39) || x_test_string(lret,ivarstring40,varstring40) || x_test_string(lret,ivarstring41,varstring41))
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


char bnf_kifff::m_assertpredicate_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_retractcommand(lret,&subtree) || m_assertcommandbefore(lret,&subtree) || m_assertcommandafter(lret,&subtree))
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


char bnf_kifff::m_assertpredicate_0_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_assertpredicate_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_assertpredicate_0_2_3(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kifff::m_assertpredicate(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicatevariable(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_term_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_term(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_tuple(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_allvariable(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_affectationpredicate(string& lreturn,x_node** tree) {
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
 if (m_allvariable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_kifff::m_comparepredicate_0_1(string& lreturn,x_node** tree) {
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
 return(1);
 }


char bnf_kifff::m_comparepredicate(string& lreturn,x_node** tree) {
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
 if (m_allvariable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_merging(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_apredicatelist_0_1_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_apredicatelist_0_1_2_4_5(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_apredicatelist_0_1_2_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_apredicatelist_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_apredicatelist_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_apredicatelist(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_valpredicatevector_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_valpredicatevector(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_pexpression(string& lreturn,x_node** tree) {
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
 if (m_term(lret,&subtree) || m_tuple(lret,&subtree) || m_predicatevariable(lret,&subtree) || m_valpredicatevector(lret,&subtree) || m_valmappredicate(lret,&subtree) || m_abool(lret,&subtree) || m_xnumber(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_call(lret,&subtree) || m_variable(lret,&subtree))
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


char bnf_kifff::m_predicateparameters_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicateparameters_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicateparameters(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_dico(string& lreturn,x_node** tree) {
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
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_valmap_0_1_2_3_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_valmap_0_1_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_valmap_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_valmap_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_valmap(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_valmappredicate(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicateoperator(string& lreturn,x_node** tree) {
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
 if (x_test_in(lret,tab5))
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


char bnf_kifff::m_dboperation(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring42,varstring42) || x_test_string(lret,ivarstring43,varstring43) || x_test_string(lret,ivarstring44,varstring44))
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


char bnf_kifff::m_predicatedatabase(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicateexpression_0_1_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicateexpression_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicateexpression_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicateexpression_0_4_5(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicateexpression_0_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicateexpression(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicatedefinition_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicatedefinition(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_predicatefact(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_dcgword(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_finaltoken_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_predicatevariable(lret,&subtree) || m_xnumber(lret,&subtree) || m_astringdouble(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_astring(lret,&subtree))
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


char bnf_kifff::m_finaltoken_0_2_3(string& lreturn,x_node** tree) {
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
 if (m_finaltoken_0_1(lret,&subtree)) 
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


char bnf_kifff::m_finaltoken_0_2(string& lreturn,x_node** tree) {
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
   if (m_finaltoken_0_2_3(lret,&subtree))
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


char bnf_kifff::m_finaltoken(string& lreturn,x_node** tree) {
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
 if (m_finaltoken_0_2(lret,&subtree)) 
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


char bnf_kifff::m_finaldcg(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_dcg_0_1_2_3_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_dcg_0_1_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_dcg_0_1_2_3_4(lret,&subtree))
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
   if (m_dcg_0_1_2_3_4(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kifff::m_dcg_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
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
 if (m_dcg_0_1_2_3(lret,&subtree)) 
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


char bnf_kifff::m_dcg_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_finaltoken(lret,&subtree) || m_dcg_0_1_2(lret,&subtree))
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


char bnf_kifff::m_dcg_0_5_6(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_dcg_0_5(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_dcg(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_haskelltail(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_haskellexpression(string& lreturn,x_node** tree) {
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
 if (m_haskellvector(lret,&subtree) || m_abool(lret,&subtree) || m_xnumber(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_word(lret,&subtree))
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


char bnf_kifff::m_haskellvector_0_1_2_3_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_haskellvector_0_1_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_haskellvector_0_1_2_5(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_haskellvector_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_haskellvector_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_haskellvector(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_letkeyword(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring45,varstring45) || x_test_string(lret,ivarstring46,varstring46))
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


char bnf_kifff::m_let_0_1_2(string& lreturn,x_node** tree) {
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
 if (m_hfunctioncall(lret,&subtree)) {
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


char bnf_kifff::m_let_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hmetafunctions(lret,&subtree) || m_let_0_1_2(lret,&subtree))
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


char bnf_kifff::m_let(string& lreturn,x_node** tree) {
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
 if (m_let_0_1(lret,&subtree)) 
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


char bnf_kifff::m_range_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_range(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_haskell_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_haskellvector(lret,&subtree) || m_word(lret,&subtree))
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


char bnf_kifff::m_haskell_0_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_haskell_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_haskell(string& lreturn,x_node** tree) {
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
 if (x_test_char(lret,'=')) 
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


char bnf_kifff::m_otherwise_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_telque(lret,&subtree) || m_expression(lret,&subtree))
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


char bnf_kifff::m_otherwise(string& lreturn,x_node** tree) {
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
 if (m_otherwise_0_1(lret,&subtree)) 
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


char bnf_kifff::m_guard_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_guard(string& lreturn,x_node** tree) {
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
//BODYSEQUENCE
 subtree=NULL;
 if (m_otherwise_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_guard_0_2(lret,&subtree)) 
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


char bnf_kifff::m_hlambda_0_2(string& lreturn,x_node** tree) {
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
   if (m_haskell_0_1(lret,&subtree))
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


char bnf_kifff::m_hlambda(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_hfunction_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_hfunction(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_lfold(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lfold";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring48,varstring48))
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


char bnf_kifff::m_rfold(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="rfold";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring49,varstring49))
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


char bnf_kifff::m_lscan(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lscan";
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


char bnf_kifff::m_rscan(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="rscan";
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


char bnf_kifff::m_fold(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_folding_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_folding_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_evallambda(lret,&subtree) || m_expression(lret,&subtree))
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


char bnf_kifff::m_folding(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_lfold1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lfold1";
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


char bnf_kifff::m_rfold1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="rfold1";
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


char bnf_kifff::m_lscan1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="lscan1";
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


char bnf_kifff::m_rscan1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="rscan1";
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


char bnf_kifff::m_fold1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_folding1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_filterkeyword(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring56,varstring56) || x_test_string(lret,ivarstring57,varstring57))
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


char bnf_kifff::m_inverted(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_mapping_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_mapping_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hcompose(lret,&subtree) || m_hcomposecall(lret,&subtree) || m_evallambda(lret,&subtree) || m_expression(lret,&subtree))
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


char bnf_kifff::m_mapping(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring58,varstring58)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_mapping_0_2(lret,&subtree)) 
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


char bnf_kifff::m_filtering_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hlambda(lret,&subtree) || m_comparison(lret,&subtree) || m_hfunction(lret,&subtree))
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


char bnf_kifff::m_filtering(string& lreturn,x_node** tree) {
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
 if (m_mapping_0_2(lret,&subtree)) 
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


char bnf_kifff::m_zipping_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_operator(lret,&subtree) || m_hfunction(lret,&subtree))
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


char bnf_kifff::m_zipping_0_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_zipping(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring59,varstring59)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_folding_0_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_zipping_0_3(lret,&subtree)) 
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


char bnf_kifff::m_hfunctioncall_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_mapping_0_2(lret,&subtree))
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
   if (m_mapping_0_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kifff::m_hfunctioncall(string& lreturn,x_node** tree) {
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
 if (m_hfunctioncall_0_1(lret,&subtree)) 
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


char bnf_kifff::m_hoperator(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_hmetafunctions(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="hmetafunctions";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_folding(lret,&subtree) || m_folding1(lret,&subtree) || m_mapping(lret,&subtree) || m_filtering(lret,&subtree) || m_zipping(lret,&subtree))
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


char bnf_kifff::m_hcompose_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_mapping(lret,&subtree) || m_filtering(lret,&subtree) || m_zipping(lret,&subtree) || m_folding(lret,&subtree) || m_folding1(lret,&subtree))
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


char bnf_kifff::m_hcompose(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_hcomposecall(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_whereexpression_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_whereexpression_0_2_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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


char bnf_kifff::m_whereexpression_0_2(string& lreturn,x_node** tree) {
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
   if (m_whereexpression_0_2_3(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kifff::m_whereexpression(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring60,varstring60)) 
    x_init_tree(tree,subtree,addsubtree);
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
//BODYSEQUENCE
 subtree=NULL;
 if (m_whereexpression_0_2(lret,&subtree)) 
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


char bnf_kifff::m_hinexpression(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring31,varstring31)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_telque_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_telque_0_1_3_4_5_6(string& lreturn,x_node** tree) {
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
 if (m_let(lret,&subtree)) 
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


char bnf_kifff::m_telque_0_1_3_4_5(string& lreturn,x_node** tree) {
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
   if (m_telque_0_1_3_4_5_6(lret,&subtree))
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


char bnf_kifff::m_telque_0_1_3_4_7(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_whereexpression(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kifff::m_telque_0_1_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_let(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_1_3_4_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_1_3_4_7(lret,&subtree)) 
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


char bnf_kifff::m_telque_0_1_3_8_9_10(string& lreturn,x_node** tree) {
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
 if (m_expression(lret,&subtree)) {
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


char bnf_kifff::m_telque_0_1_3_8_9(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_evallambda(lret,&subtree) || m_telque_0_1_3_8_9_10(lret,&subtree))
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


char bnf_kifff::m_telque_0_1_3_8_11_12_13_14_15_16(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_telque_0_1_3_8_11_12_13_14_15(string& lreturn,x_node** tree) {
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
   if (m_telque_0_1_3_8_11_12_13_14_15_16(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kifff::m_telque_0_1_3_8_11_12_13_14_18_19(string& lreturn,x_node** tree) {
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
 if (m_booleanexpression(lret,&subtree)) 
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


char bnf_kifff::m_telque_0_1_3_8_11_12_13_14_18(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_telque_0_1_3_8_11_12_13_14_18_19(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kifff::m_telque_0_1_3_8_11_12_13_14(string& lreturn,x_node** tree) {
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
 if (m_telque_0_1_3_8_11_12_13_14_15(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_1_3_4_7(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_1_3_8_11_12_13_14_18(lret,&subtree)) 
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


char bnf_kifff::m_telque_0_1_3_8_11_12_13(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_telque_0_1_3_8_11_12_13_14(lret,&subtree) || m_booleanexpression(lret,&subtree))
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


char bnf_kifff::m_telque_0_1_3_8_11_12(string& lreturn,x_node** tree) {
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
 if (m_telque_0_1_3_8_11_12_13(lret,&subtree)) 
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


char bnf_kifff::m_telque_0_1_3_8_11(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_telque_0_1_3_8_11_12(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kifff::m_telque_0_1_3_8(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_1_3_8_9(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_1_3_8_11(lret,&subtree)) 
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


char bnf_kifff::m_telque_0_1_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_hmetafunctions(lret,&subtree) || m_guard(lret,&subtree) || m_telque_0_1_3_4(lret,&subtree) || m_telque_0_1_3_8(lret,&subtree))
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


char bnf_kifff::m_telque_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_1_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
	intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_telque_0_1_3(lret,&subtree)) 
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


char bnf_kifff::m_telque(string& lreturn,x_node** tree) {
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
 exitonfail=true;
 initfail(label,currentpos,48);
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


char bnf_kifff::m_evallambda_0_1(string& lreturn,x_node** tree) {
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
 return(1);
 }


char bnf_kifff::m_evallambda(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="evallambda";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_telque(lret,&subtree) || m_evallambda_0_1(lret,&subtree))
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


char bnf_kifff::m_parameters_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_parameters_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_parameters(string& lreturn,x_node** tree) {
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
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_optional(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_expression_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_expression_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_localif(lret,&subtree) || m_abool(lret,&subtree) || m_xnumber(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_call(lret,&subtree) || m_optional(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree) || m_intentionvector(lret,&subtree) || m_valvector(lret,&subtree) || m_valmap(lret,&subtree))
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


char bnf_kifff::m_expression(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_expressions_0_1_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_expressions_0_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_expressions_0_1_2_3(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kifff::m_expressions_0_1(string& lreturn,x_node** tree) {
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
 if (m_expressions_0_1_2(lret,&subtree)) 
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


char bnf_kifff::m_expressions(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_localif(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring61,varstring61)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (x_test_string(lret,ivarstring62,varstring62)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_numbers_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_numbers(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_xnumber_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_xnumber_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_xnumber(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_abool(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring63,varstring63) || x_test_string(lret,ivarstring64,varstring64))
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


char bnf_kifff::m_blocfor_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_blocfor_0_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_blocfor_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_blocfor(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_optionalboolean(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_booleanexpression_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_booleanexpression_0_2_3_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_booleanexpression_0_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_booleanexpression_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_booleanexpression(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_arange(string& lreturn,x_node** tree) {
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
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_test_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_test_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_test_0_3_4(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring62,varstring62)) 
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


char bnf_kifff::m_test_0_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_test(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring61,varstring61)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_testelif(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_loop(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring66,varstring66)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_doloop(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring67,varstring67)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (x_test_string(lret,ivarstring66,varstring66)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_for(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring68,varstring68)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_forin_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_forin(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring68,varstring68)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (x_test_string(lret,ivarstring31,varstring31)) 
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


char bnf_kifff::m_switch_0_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_switch(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring69,varstring69)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kifff::m_trycatch_0_1_2_3(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_trycatch_0_1_2(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_trycatch_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_trycatch_0_4_5(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_trycatch_0_4(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_trycatch(string& lreturn,x_node** tree) {
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
 if (x_test_string(lret,ivarstring70,varstring70)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (x_test_string(lret,ivarstring71,varstring71)) 
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


char bnf_kifff::m_testswitch_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_default(lret,&subtree) || m_xnumber(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_abool(lret,&subtree))
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


char bnf_kifff::m_testswitch(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_default(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char* label="default";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,ivarstring72,varstring72))
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


char bnf_kifff::m_lispexpression(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_lispexpressions(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_onelisp_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_onelisp(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_lisp_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_lisp(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_tagexpression(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_tagexpressions(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_onetag_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_onetag(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_tag_0_1(string& lreturn,x_node** tree) {
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


char bnf_kifff::m_tag(string& lreturn,x_node** tree) {
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




string bnf_kifff::x_errormsg(int i) {
	return errorlabels[i];
}


x_node* bnf_kifff::x_parsing(x_reading* xr,x_parsing_mode mode,bool display) {
        fx=xr;
	string lret;
	x_node* tree=new x_node;
	gFail=0;
	lineerror=-1;
	labelerror="";
	errornumber=-1;
	currentpos=0;
	intoken=0;
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


char bnf_kifff::x_test_string(string& lret,int sz,string sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}

char bnf_kifff::x_test_pstring(string& lret,int sz,string& sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}

char bnf_kifff::m_punct(string& lreturn,x_node** tree) {
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

char bnf_kifff::m_afullstring(string& lreturn,x_node** tree) {
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

char bnf_kifff::m_word(string& lreturn,x_node** tree) {
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

char bnf_kifff::m_typename(string& lreturn,x_node** tree) {
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

char bnf_kifff::m_astring(string& lreturn,x_node** tree) {
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

char bnf_kifff::m_anumber(string& lreturn,x_node** tree) {
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

char bnf_kifff::m_astringsimple(string& lreturn,x_node** tree) {
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

char bnf_kifff::m_astringdouble(string& lreturn,x_node** tree) {
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
