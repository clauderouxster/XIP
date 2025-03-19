#include "kifn.h"


static char* errorlabels[]={"","Wrong index format",
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
"Wrong IN expression",
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
"Missing ';'",
"Missing '}'",
"Wrong declaration",
"Wrong FOR IN declaration",
"Wrong PREDICATE definition",
"Wrong PREDICATE expression",
"Wrong PREDICATE parameter list",
"Wrong PREDICATE term"};

static char tab0[]={'_','#',0};
static char tab1[]={'&','|',0};
static char tab2[]={'+','-','*','/',0};
static char tab3[]={'%','^',0};
static char tab4[]={'<','>',0};
char bnf_kif::m_char_0(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="char";
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_char(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="char";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_char_0(lret,&subtree))
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


char bnf_kif::m_digit(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="digit";
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


char bnf_kif::m_path_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="path";
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


char bnf_kif::m_path_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="path";
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
 if (m_path_1_2(lret,&subtree)) 
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


char bnf_kif::m_path(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="path";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_path_1(lret,&subtree))
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


char bnf_kif::m_shell_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="shell";
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


char bnf_kif::m_shell_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="shell";
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
 if (m_shell_3_4(lret,&subtree)) 
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


char bnf_kif::m_shell(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="shell";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_shell_3(lret,&subtree))
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


char bnf_kif::m_label_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="label";
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_label(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="label";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_label_5(lret,&subtree))
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


char bnf_kif::m_private(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="private";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,7,"private"))
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
 static const char label[]="common";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,6,"common"))
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
 static const char label[]="const";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,5,"const"))
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


char bnf_kif::m_feature_6_7_8(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="feature";
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


char bnf_kif::m_feature_6_7(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="feature";
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
 if (m_feature_6_7_8(lret,&subtree)) 
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


char bnf_kif::m_feature_6_9(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="feature";
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


char bnf_kif::m_feature_6(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="feature";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_feature_6_7(lret,&subtree) || m_feature_6_9(lret,&subtree))
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
 static const char label[]="feature";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_feature_6(lret,&subtree))
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


char bnf_kif::m_type_10_11(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="type";
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


char bnf_kif::m_type_10(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="type";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_type_10_11(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_type(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="type";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_type_10(lret,&subtree))
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


char bnf_kif::m_anydouble_12_13(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="anydouble";
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


char bnf_kif::m_anydouble_12_14(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="anydouble";
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


char bnf_kif::m_anydouble_12(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="anydouble";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_anydouble_12_13(lret,&subtree) || m_anydouble_12_14(lret,&subtree))
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


char bnf_kif::m_anydouble(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="anydouble";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_anydouble_12(lret,&subtree))
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


char bnf_kif::m_anysimple_15(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="anysimple";
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anysimple(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="anysimple";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_anysimple_15(lret,&subtree))
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


char bnf_kif::m_anycolon_16_17(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="anycolon";
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


char bnf_kif::m_anycolon_16_18(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="anycolon";
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


char bnf_kif::m_anycolon_16(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="anycolon";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_anycolon_16_17(lret,&subtree) || m_anycolon_16_18(lret,&subtree))
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


char bnf_kif::m_anycolon(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="anycolon";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_anycolon_16(lret,&subtree))
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


char bnf_kif::m_operator_19_20(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operator";
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
 if (x_test_char(lret,'<')) 
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


char bnf_kif::m_operator_19_21(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
 if (x_test_char(lret,'>')) 
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


char bnf_kif::m_operator_19_22(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'^')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'^')) 
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


char bnf_kif::m_operator_19(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab1) || m_operator_19_20(lret,&subtree) || m_operator_19_21(lret,&subtree) || x_test_in(lret,tab2) || m_operator_19_22(lret,&subtree) || x_test_in(lret,tab3))
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


char bnf_kif::m_operator(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operator_19(lret,&subtree))
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


char bnf_kif::m_operatoraffectation_23_24(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
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
 if (x_test_char(lret,'=')) 
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


char bnf_kif::m_operatoraffectation_23_25(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
 if (x_test_char(lret,'=')) 
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


char bnf_kif::m_operatoraffectation_23_26(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'+')) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_23_27(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
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
 if (x_test_char(lret,'=')) 
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


char bnf_kif::m_operatoraffectation_23_28(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'*')) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_23_29(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
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
 if (x_test_char(lret,'=')) 
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


char bnf_kif::m_operatoraffectation_23_30(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
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
 if (x_test_char(lret,'=')) 
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


char bnf_kif::m_operatoraffectation_23_31(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'&')) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_23_32(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
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
 if (x_test_char(lret,'=')) 
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


char bnf_kif::m_operatoraffectation_23_33(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'^')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'^')) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_23_34(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'^')) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_23(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,2,"is") || x_test_char(lret,'=') || m_operatoraffectation_23_24(lret,&subtree) || m_operatoraffectation_23_25(lret,&subtree) || m_operatoraffectation_23_26(lret,&subtree) || m_operatoraffectation_23_27(lret,&subtree) || m_operatoraffectation_23_28(lret,&subtree) || m_operatoraffectation_23_29(lret,&subtree) || m_operatoraffectation_23_30(lret,&subtree) || m_operatoraffectation_23_31(lret,&subtree) || m_operatoraffectation_23_32(lret,&subtree) || m_operatoraffectation_23_33(lret,&subtree) || m_operatoraffectation_23_34(lret,&subtree))
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


char bnf_kif::m_operatoraffectation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatoraffectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operatoraffectation_23(lret,&subtree))
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


char bnf_kif::m_orand_35_36(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="orand";
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
 if (x_test_char(lret,'|')) 
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


char bnf_kif::m_orand_35_37(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="orand";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'&')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'&')) 
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


char bnf_kif::m_orand_35(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="orand";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_orand_35_36(lret,&subtree) || m_orand_35_37(lret,&subtree))
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


char bnf_kif::m_orand(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="orand";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_orand_35(lret,&subtree))
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


char bnf_kif::m_comparator_38_39(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comparator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'~')) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_38_40(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comparator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
 if (x_test_char(lret,'=')) 
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


char bnf_kif::m_comparator_38_41(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comparator";
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
 if (x_test_char(lret,'>')) 
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


char bnf_kif::m_comparator_38_42(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comparator";
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
 if (x_test_char(lret,'=')) 
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


char bnf_kif::m_comparator_38_43(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comparator";
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
 if (x_test_char(lret,'=')) 
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


char bnf_kif::m_comparator_38_44(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comparator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
 if (x_test_char(lret,'=')) 
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


char bnf_kif::m_comparator_38(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comparator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_comparator_38_39(lret,&subtree) || m_comparator_38_40(lret,&subtree) || m_comparator_38_41(lret,&subtree) || m_comparator_38_42(lret,&subtree) || m_comparator_38_43(lret,&subtree) || m_comparator_38_44(lret,&subtree) || x_test_in(lret,tab4))
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


char bnf_kif::m_comparator(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comparator";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_comparator_38(lret,&subtree))
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


char bnf_kif::m_depend_45(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="depend";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,4,"with")) 
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_depend(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="depend";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_depend_45(lret,&subtree))
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


char bnf_kif::m_declaration_46_47_48(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declaration";
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


char bnf_kif::m_declaration_46_47(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declaration";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_declaration_46_47_48(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_declaration_46_49(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declaration";
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


char bnf_kif::m_declaration_46_50_51(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declaration";
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


char bnf_kif::m_declaration_46_50(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declaration";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_declaration_46_50_51(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_declaration_46(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declaration";
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
 if (m_declaration_46_47(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_46_49(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_46_50(lret,&subtree)) 
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


char bnf_kif::m_declaration(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declaration";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_declaration_46(lret,&subtree))
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


char bnf_kif::m_declarationlist_52_56_57(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declarationlist";
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


char bnf_kif::m_declarationlist_52_56(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declarationlist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_declarationlist_52_56_57(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_declarationlist_52_58_59(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declarationlist";
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


char bnf_kif::m_declarationlist_52_58(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declarationlist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_declarationlist_52_58_59(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_declarationlist_52(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declarationlist";
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
 if (m_declaration_46_47(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_46_49(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declarationlist_52_56(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declarationlist_52_58(lret,&subtree)) 
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


char bnf_kif::m_declarationlist(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="declarationlist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_declarationlist_52(lret,&subtree))
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


char bnf_kif::m_multideclaration_60(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="multideclaration";
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
 if (m_declaration_46_47(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_46_49(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declarationlist_52_56(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declarationlist_52_58(lret,&subtree)) 
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


char bnf_kif::m_multideclaration(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="multideclaration";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_multideclaration_60(lret,&subtree))
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
 static const char label[]="declarationending";
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


char bnf_kif::m_frame_68_70(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="frame";
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


char bnf_kif::m_frame_68(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="frame";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,5,"frame")) 
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
 if (m_declaration_46_49(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_frame_68_70(lret,&subtree)) 
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


char bnf_kif::m_frame(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="frame";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_frame_68(lret,&subtree))
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


char bnf_kif::m_instruction_71_72(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
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


char bnf_kif::m_instruction_71(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
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
 if (m_instruction_71_72(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_instruction(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_instruction_71(lret,&subtree))
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


char bnf_kif::m_nonlimited_73(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nonlimited";
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
 if (x_test_char(lret,'.')) 
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


char bnf_kif::m_nonlimited(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nonlimited";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nonlimited_73(lret,&subtree))
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


char bnf_kif::m_arguments_74_75_76_77(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="arguments";
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


char bnf_kif::m_arguments_74_75_76(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="arguments";
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
 if (m_arguments_74_75_76_77(lret,&subtree)) 
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


char bnf_kif::m_arguments_74_75(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="arguments";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_arguments_74_75_76(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_arguments_74(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="arguments";
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
 if (m_arguments_74_75(lret,&subtree)) 
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


char bnf_kif::m_arguments(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="arguments";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_arguments_74(lret,&subtree))
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
 static const char label[]="strict";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,6,"strict"))
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
 static const char label[]="join";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,4,"join"))
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


char bnf_kif::m_protecclusive_78(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="protecclusive";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,9,"protected") || x_test_string(lret,9,"exclusive"))
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


char bnf_kif::m_protecclusive(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="protecclusive";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_protecclusive_78(lret,&subtree))
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


char bnf_kif::m_functionsort_79(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="functionsort";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,10,"polynomial") || x_test_string(lret,8,"function") || x_test_string(lret,7,"autorun") || x_test_string(lret,6,"thread"))
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


char bnf_kif::m_functionsort(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="functionsort";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_functionsort_79(lret,&subtree))
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


char bnf_kif::m_typefunction_80_81(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="typefunction";
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


char bnf_kif::m_typefunction_80_82(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="typefunction";
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


char bnf_kif::m_typefunction_80_83(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="typefunction";
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


char bnf_kif::m_typefunction_80_84(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="typefunction";
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


char bnf_kif::m_typefunction_80(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="typefunction";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_typefunction_80_81(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_typefunction_80_82(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_typefunction_80_83(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_typefunction_80_84(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_typefunction(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="typefunction";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_typefunction_80(lret,&subtree))
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


char bnf_kif::m_indexname_85(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="indexname";
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
 if (x_test_char(lret,']')) 
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


char bnf_kif::m_indexname(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="indexname";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_indexname_85(lret,&subtree))
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


char bnf_kif::m_intervalname_86(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="intervalname";
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
 if (x_test_char(lret,']')) 
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


char bnf_kif::m_intervalname(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="intervalname";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_intervalname_86(lret,&subtree))
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


char bnf_kif::m_function_87_88(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="function";
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


char bnf_kif::m_function_87_89(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="function";
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


char bnf_kif::m_function_87(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="function";
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
 if (m_function_87_88(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_function_87_89(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_frame_68_70(lret,&subtree)) 
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


char bnf_kif::m_function(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="function";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_function_87(lret,&subtree))
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


char bnf_kif::m_sousbloc_91(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sousbloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_multideclaration(lret,&subtree) || m_affectation(lret,&subtree) || m_predicatedefinition(lret,&subtree) || m_expressions(lret,&subtree))
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


char bnf_kif::m_sousbloc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sousbloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_sousbloc_91(lret,&subtree))
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


char bnf_kif::m_bloc_92_93(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="bloc";
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


char bnf_kif::m_bloc_92(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="bloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_predicatefact(lret,&subtree) || m_bloc_92_93(lret,&subtree))
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


char bnf_kif::m_bloc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="bloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_bloc_92(lret,&subtree))
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


char bnf_kif::m_complexbloc_94(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="complexbloc";
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
 return(1);
 }


char bnf_kif::m_complexbloc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="complexbloc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_complexbloc_94(lret,&subtree))
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


char bnf_kif::m_blocs_95_96(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="blocs";
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


char bnf_kif::m_blocs_95(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="blocs";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_blocs_95_96(lret,&subtree))
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
   if (m_blocs_95_96(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_kif::m_blocs(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="blocs";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_blocs_95(lret,&subtree))
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


char bnf_kif::m_analyse_97_98(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="analyse";
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


char bnf_kif::m_analyse_97_99_100_101(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="analyse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,3,"kif") || x_test_string(lret,3,"Kif") || x_test_string(lret,3,"KIf") || x_test_string(lret,3,"KIF") || x_test_string(lret,3,"KiF") || x_test_string(lret,3,"kIf") || x_test_string(lret,3,"kiF") || x_test_string(lret,3,"kIF"))
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


char bnf_kif::m_analyse_97_99_100(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="analyse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_97_99_100_101(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kif::m_analyse_97_99(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="analyse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_analyse_97_99_100(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_analyse_97(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="analyse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_97_98(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_97_99(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 return(1);
 }


char bnf_kif::m_analyse(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="analyse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_analyse_97(lret,&subtree))
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


char bnf_kif::m_subfunc_102(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="subfunc";
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
 return(1);
 }


char bnf_kif::m_subfunc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="subfunc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_subfunc_102(lret,&subtree))
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


char bnf_kif::m_subfuncbis_103(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="subfuncbis";
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
 return(1);
 }


char bnf_kif::m_subfuncbis(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="subfuncbis";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_subfuncbis_103(lret,&subtree))
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


char bnf_kif::m_subvar_104(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="subvar";
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
 return(1);
 }


char bnf_kif::m_subvar(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="subvar";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_subvar_104(lret,&subtree))
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


char bnf_kif::m_framesep_105(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="framesep";
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
 if (x_test_char(lret,':')) 
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


char bnf_kif::m_framesep(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="framesep";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_framesep_105(lret,&subtree))
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


char bnf_kif::m_frameup_106(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="frameup";
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_frameup(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="frameup";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_frameup_106(lret,&subtree))
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


char bnf_kif::m_functioncall_107_108(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="functioncall";
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


char bnf_kif::m_functioncall_107(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="functioncall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_functioncall_107_108(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_functioncall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="functioncall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_functioncall_107(lret,&subtree))
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


char bnf_kif::m_regularcall_109_110(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="regularcall";
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


char bnf_kif::m_regularcall_109_111_112(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="regularcall";
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


char bnf_kif::m_regularcall_109_111(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="regularcall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_regularcall_109_111_112(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_regularcall_109(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="regularcall";
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
 if (m_regularcall_109_110(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_regularcall_109_111(lret,&subtree)) 
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


char bnf_kif::m_regularcall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="regularcall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_regularcall_109(lret,&subtree))
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


char bnf_kif::m_cut(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="cut";
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


char bnf_kif::m_predicate_113_114(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicate";
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


char bnf_kif::m_predicate_113(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicate";
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
 exitonfail=true;
 initfail(label,currentpos,34);
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicate_113_114(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kif::m_predicate(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_predicate_113(lret,&subtree))
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


char bnf_kif::m_predicatecall_115(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatecall";
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
 initfail(label,currentpos,34);
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicate_113_114(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_regularcall_109_111(lret,&subtree)) 
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


char bnf_kif::m_predicatecall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatecall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_predicatecall_115(lret,&subtree))
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


char bnf_kif::m_returncall_119(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="returncall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,6,"return") || x_test_string(lret,7,"_return"))
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


char bnf_kif::m_returncall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="returncall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_returncall_119(lret,&subtree))
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


char bnf_kif::m_breakcall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="breakcall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,5,"break"))
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
 static const char label[]="continuecall";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,8,"continue"))
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


char bnf_kif::m_call_120(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="call";
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
 return(1);
 }


char bnf_kif::m_call(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="call";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_call_120(lret,&subtree))
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


char bnf_kif::m_sep(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sep";
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


char bnf_kif::m_indexes_121_122_123(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="indexes";
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


char bnf_kif::m_indexes_121_122(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="indexes";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_indexes_121_122_123(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_indexes_121(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="indexes";
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
 if (m_indexes_121_122(lret,&subtree)) 
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


char bnf_kif::m_indexes(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="indexes";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_indexes_121(lret,&subtree))
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


char bnf_kif::m_interval_124_125(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="interval";
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


char bnf_kif::m_interval_124(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="interval";
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
 if (m_interval_124_125(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_interval_124_125(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_indexes_121_122(lret,&subtree)) 
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


char bnf_kif::m_interval(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="interval";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_interval_124(lret,&subtree))
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


char bnf_kif::m_variable_129_130_131(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="variable";
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


char bnf_kif::m_variable_129_130(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="variable";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_variable_129_130_131(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_variable_129(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="variable";
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
 if (m_variable_129_130(lret,&subtree)) 
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


char bnf_kif::m_variable(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="variable";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_variable_129(lret,&subtree))
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


char bnf_kif::m_predicatevariable_132_133(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatevariable";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_term(lret,&subtree) || m_word(lret,&subtree))
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


char bnf_kif::m_predicatevariable_132(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatevariable";
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
 initfail(label,currentpos,34);
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicatevariable_132_133(lret,&subtree)) 
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


char bnf_kif::m_predicatevariable(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatevariable";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_predicatevariable_132(lret,&subtree))
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


char bnf_kif::m_term_134_135(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="term";
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


char bnf_kif::m_term_134(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="term";
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
 exitonfail=true;
 initfail(label,currentpos,37);
//BODYSEQUENCE
 subtree=NULL;
 if (m_term_134_135(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_term(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="term";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_term_134(lret,&subtree))
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


char bnf_kif::m_allvariable_136(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="allvariable";
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_allvariable(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="allvariable";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_allvariable_136(lret,&subtree))
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


char bnf_kif::m_plusplus_137_138(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="plusplus";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'+')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'+')) 
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


char bnf_kif::m_plusplus_137_139(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="plusplus";
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
 if (x_test_char(lret,'-')) 
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


char bnf_kif::m_plusplus_137(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="plusplus";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_plusplus_137_138(lret,&subtree) || m_plusplus_137_139(lret,&subtree))
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


char bnf_kif::m_plusplus(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="plusplus";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_plusplus_137(lret,&subtree))
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


char bnf_kif::m_affectation_140_141_142(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectation";
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


char bnf_kif::m_affectation_140_141(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_plusplus(lret,&subtree) || m_affectation_140_141_142(lret,&subtree))
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


char bnf_kif::m_affectation_140(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectation";
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
 if (m_affectation_140_141(lret,&subtree)) 
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


char bnf_kif::m_affectation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_affectation_140(lret,&subtree))
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


char bnf_kif::m_affectationpredicate_143(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectationpredicate";
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
 if (m_affectation_140_141(lret,&subtree)) 
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


char bnf_kif::m_affectationpredicate(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectationpredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_affectationpredicate_143(lret,&subtree))
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


char bnf_kif::m_increment_146(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="increment";
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
 return(1);
 }


char bnf_kif::m_increment(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="increment";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_increment_146(lret,&subtree))
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


char bnf_kif::m_notin_147_148(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="notin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,3,"not")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,2,"in")) 
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


char bnf_kif::m_notin_147_149(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="notin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,6,"absent")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,4,"from")) 
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


char bnf_kif::m_notin_147(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="notin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,5,"notin") || m_notin_147_148(lret,&subtree) || m_notin_147_149(lret,&subtree))
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


char bnf_kif::m_notin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="notin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_notin_147(lret,&subtree))
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


char bnf_kif::m_in(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="in";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,2,"in"))
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


char bnf_kif::m_operatorin_150(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatorin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_notin(lret,&subtree) || m_in(lret,&subtree))
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


char bnf_kif::m_operatorin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operatorin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operatorin_150(lret,&subtree))
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


char bnf_kif::m_operationin_151_152_153(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operationin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,4,"with")) 
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


char bnf_kif::m_operationin_151_152(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operationin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_operationin_151_152_153(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_operationin_151(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operationin";
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
 if (m_operationin_151_152(lret,&subtree)) 
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


char bnf_kif::m_operationin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operationin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operationin_151(lret,&subtree))
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


char bnf_kif::m_operation_154(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operation";
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
 return(1);
 }


char bnf_kif::m_operation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operation_154(lret,&subtree))
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


char bnf_kif::m_comparison_155(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comparison";
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
 return(1);
 }


char bnf_kif::m_comparison(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comparison";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_comparison_155(lret,&subtree))
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


char bnf_kif::m_alist_156_157_158(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="alist";
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
 if (m_alist(lret,&subtree)) 
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


char bnf_kif::m_alist_156_157(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="alist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_alist_156_157_158(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_alist_156(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="alist";
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
 if (m_alist_156_157(lret,&subtree)) 
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


char bnf_kif::m_alist(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="alist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_alist_156(lret,&subtree))
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


char bnf_kif::m_valvector_159_160(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_alist(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valvector_159(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valvector";
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
 if (m_valvector_159_160(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 return(1);
 }


char bnf_kif::m_valvector(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valvector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_valvector_159(lret,&subtree))
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


char bnf_kif::m_merging(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="merging";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_alistnomerge(lret,&subtree))
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


char bnf_kif::m_alistnomerge_161_162_163(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="alistnomerge";
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
 if (m_alistnomerge(lret,&subtree)) 
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


char bnf_kif::m_alistnomerge_161_162(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="alistnomerge";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_alistnomerge_161_162_163(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_alistnomerge_161(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="alistnomerge";
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
 if (m_alistnomerge_161_162(lret,&subtree)) 
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


char bnf_kif::m_alistnomerge(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="alistnomerge";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_alistnomerge_161(lret,&subtree))
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


char bnf_kif::m_apredicatelist_164_165_166_167(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="apredicatelist";
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


char bnf_kif::m_apredicatelist_164_165_166_168(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="apredicatelist";
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
 return(1);
 }


char bnf_kif::m_apredicatelist_164_165_166(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="apredicatelist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_apredicatelist_164_165_166_167(lret,&subtree) || m_apredicatelist_164_165_166_168(lret,&subtree))
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


char bnf_kif::m_apredicatelist_164_165(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="apredicatelist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_apredicatelist_164_165_166(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_apredicatelist_164(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="apredicatelist";
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
 if (m_apredicatelist_164_165(lret,&subtree)) 
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


char bnf_kif::m_apredicatelist(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="apredicatelist";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_apredicatelist_164(lret,&subtree))
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


char bnf_kif::m_valpredicatevector_169_170(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valpredicatevector";
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


char bnf_kif::m_valpredicatevector_169(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valpredicatevector";
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
 if (m_valpredicatevector_169_170(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 return(1);
 }


char bnf_kif::m_valpredicatevector(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valpredicatevector";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_valpredicatevector_169(lret,&subtree))
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


char bnf_kif::m_dvalue(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dvalue";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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


char bnf_kif::m_dico_171_172_173(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dico";
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


char bnf_kif::m_dico_171_172(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dico";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_dico_171_172_173(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_dico_171(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dico";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dvalue(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_dvalue(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dico_171_172(lret,&subtree)) 
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


char bnf_kif::m_dico(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dico";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dico_171(lret,&subtree))
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


char bnf_kif::m_valmap_174_175(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valmap";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_dico(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valmap_174(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valmap";
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
 if (m_valmap_174_175(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kif::m_valmap(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valmap";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_valmap_174(lret,&subtree))
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


char bnf_kif::m_dicopredicate_176_177_178(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dicopredicate";
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
 if (m_dicopredicate(lret,&subtree)) 
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


char bnf_kif::m_dicopredicate_176_177(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dicopredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_dicopredicate_176_177_178(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_dicopredicate_176(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dicopredicate";
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
//BODYSEQUENCE
 subtree=NULL;
 if (m_dicopredicate_176_177(lret,&subtree)) 
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


char bnf_kif::m_dicopredicate(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dicopredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dicopredicate_176(lret,&subtree))
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


char bnf_kif::m_valmappredicate_179_180(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valmappredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_dicopredicate(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_valmappredicate_179(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valmappredicate";
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
 if (m_valmappredicate_179_180(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kif::m_valmappredicate(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valmappredicate";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_valmappredicate_179(lret,&subtree))
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


char bnf_kif::m_negation_181(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="negation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_char(lret,'~') || x_test_string(lret,3,"not"))
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


char bnf_kif::m_negation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="negation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_negation_181(lret,&subtree))
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


char bnf_kif::m_parameters_182_183_184(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parameters";
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


char bnf_kif::m_parameters_182_183(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parameters";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_parameters_182_183_184(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_parameters_182(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parameters";
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
 if (m_parameters_182_183(lret,&subtree)) 
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


char bnf_kif::m_parameters(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parameters";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_parameters_182(lret,&subtree))
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


char bnf_kif::m_predicateparameters_185_186(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateparameters";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_predicatevariable(lret,&subtree) || m_valpredicatevector(lret,&subtree) || m_valmappredicate(lret,&subtree) || m_expressions(lret,&subtree))
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


char bnf_kif::m_predicateparameters_185_187_188(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateparameters";
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


char bnf_kif::m_predicateparameters_185_187(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateparameters";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_predicateparameters_185_187_188(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_predicateparameters_185(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateparameters";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateparameters_185_186(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateparameters_185_187(lret,&subtree)) 
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


char bnf_kif::m_predicateparameters(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateparameters";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_predicateparameters_185(lret,&subtree))
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


char bnf_kif::m_optional_189(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="optional";
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
 return(1);
 }


char bnf_kif::m_optional(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="optional";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_optional_189(lret,&subtree))
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


char bnf_kif::m_expression_190_191(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="expression";
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


char bnf_kif::m_expression_190_192(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="expression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_abool(lret,&subtree) || m_xnumber(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_call(lret,&subtree) || m_optional(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree) || m_valvector(lret,&subtree) || m_valmap(lret,&subtree))
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


char bnf_kif::m_expression_190(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="expression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression_190_191(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression_190_192(lret,&subtree)) 
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


char bnf_kif::m_expression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="expression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_expression_190(lret,&subtree))
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


char bnf_kif::m_expressions_193_194_195(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="expressions";
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


char bnf_kif::m_expressions_193_194(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="expressions";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_expressions_193_194_195(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_expressions_193(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="expressions";
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
 if (m_expressions_193_194(lret,&subtree)) 
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
 static const char label[]="expressions";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_expressions_193(lret,&subtree))
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


char bnf_kif::m_numbers_196_197(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="numbers";
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


char bnf_kif::m_numbers_196(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="numbers";
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
 if (m_numbers_196_197(lret,&subtree)) 
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


char bnf_kif::m_numbers(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="numbers";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_numbers_196(lret,&subtree))
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


char bnf_kif::m_xnumber_198_199_200(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="xnumber";
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


char bnf_kif::m_xnumber_198_199(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="xnumber";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_xnumber_198_199_200(lret,&subtree))
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
   if (m_xnumber_198_199_200(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_xnumber_198(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="xnumber";
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
 if (m_xnumber_198_199(lret,&subtree)) 
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


char bnf_kif::m_xnumber(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="xnumber";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_xnumber_198(lret,&subtree))
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


char bnf_kif::m_abool_201(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="abool";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,4,"true") || x_test_string(lret,5,"false"))
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


char bnf_kif::m_abool(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="abool";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_abool_201(lret,&subtree))
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


char bnf_kif::m_dboperation_202(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dboperation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,3,"get") || x_test_string(lret,5,"store") || x_test_string(lret,6,"remove"))
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


char bnf_kif::m_dboperation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dboperation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dboperation_202(lret,&subtree))
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


char bnf_kif::m_predicatedatabase_203(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatedatabase";
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
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_predicatedatabase(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatedatabase";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_predicatedatabase_203(lret,&subtree))
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


char bnf_kif::m_blocfor_204_205(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="blocfor";
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


char bnf_kif::m_blocfor_204_206_207(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="blocfor";
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


char bnf_kif::m_blocfor_204_206(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="blocfor";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_blocfor_204_206_207(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_blocfor_204(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="blocfor";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocfor_204_205(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocfor_204_206(lret,&subtree)) 
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


char bnf_kif::m_blocfor(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="blocfor";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_blocfor_204(lret,&subtree))
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


char bnf_kif::m_optionalboolean_208(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="optionalboolean";
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
 return(1);
 }


char bnf_kif::m_optionalboolean(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="optionalboolean";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_optionalboolean_208(lret,&subtree))
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


char bnf_kif::m_booleanexpression_209_210(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="booleanexpression";
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


char bnf_kif::m_booleanexpression_209_211_212_213(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="booleanexpression";
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


char bnf_kif::m_booleanexpression_209_211_212(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="booleanexpression";
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
 if (m_booleanexpression_209_211_212_213(lret,&subtree)) 
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


char bnf_kif::m_booleanexpression_209_211(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="booleanexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_booleanexpression_209_211_212(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_booleanexpression_209(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="booleanexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression_209_210(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression_209_211(lret,&subtree)) 
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


char bnf_kif::m_booleanexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="booleanexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_booleanexpression_209(lret,&subtree))
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


char bnf_kif::m_predicateexpression_214_215_216_217(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateexpression";
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


char bnf_kif::m_predicateexpression_214_215_216(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateexpression_214_215_216_217(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kif::m_predicateexpression_214_215(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_cut(lret,&subtree) || m_predicateexpression_214_215_216(lret,&subtree) || m_affectationpredicate(lret,&subtree) || m_valpredicatevector(lret,&subtree) || m_valmappredicate(lret,&subtree) || m_expressions(lret,&subtree))
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


char bnf_kif::m_predicateexpression_214_218_219(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateexpression";
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


char bnf_kif::m_predicateexpression_214_218(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_predicateexpression_214_218_219(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_predicateexpression_214(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateexpression_214_215(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_predicateexpression_214_218(lret,&subtree)) 
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


char bnf_kif::m_predicateexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicateexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_predicateexpression_214(lret,&subtree))
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


char bnf_kif::m_test_220_221(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="test";
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


char bnf_kif::m_test_220_222(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="test";
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


char bnf_kif::m_test_220_223_224(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="test";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,4,"else")) 
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
 if (m_test_220_221(lret,&subtree)) 
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


char bnf_kif::m_test_220_223(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="test";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_test_220_223_224(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_test_220(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="test";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,2,"if")) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_test_220_221(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_220_222(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_220_223(lret,&subtree)) 
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


char bnf_kif::m_test(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="test";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_test_220(lret,&subtree))
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


char bnf_kif::m_testelif_226(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testelif";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,4,"elif")) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_test_220_221(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_220_222(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_220_223(lret,&subtree)) 
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


char bnf_kif::m_testelif(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testelif";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_testelif_226(lret,&subtree))
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


char bnf_kif::m_loop_232(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="loop";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,5,"while")) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_test_220_221(lret,&subtree)) 
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


char bnf_kif::m_loop(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="loop";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_loop_232(lret,&subtree))
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


char bnf_kif::m_doloop_234(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="doloop";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,2,"do")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_220_221(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,5,"while")) 
    x_init_tree(tree,subtree,addsubtree);
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
 return(1);
 }


char bnf_kif::m_doloop(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="doloop";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_doloop_234(lret,&subtree))
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


char bnf_kif::m_for_236(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="for";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,3,"for")) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_test_220_221(lret,&subtree)) 
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


char bnf_kif::m_for(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="for";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_for_236(lret,&subtree))
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


char bnf_kif::m_forin_238(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="forin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,3,"for")) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (x_test_string(lret,2,"in")) 
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
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_220_221(lret,&subtree)) 
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


char bnf_kif::m_forin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="forin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_forin_238(lret,&subtree))
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


char bnf_kif::m_switch_240_242(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="switch";
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


char bnf_kif::m_switch_240(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="switch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,6,"switch")) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_declaration_46_49(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_switch_240_242(lret,&subtree)) 
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


char bnf_kif::m_switch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="switch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_switch_240(lret,&subtree))
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


char bnf_kif::m_trycatch_243_244_245_246(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="trycatch";
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


char bnf_kif::m_trycatch_243_244_245(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="trycatch";
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
 if (m_trycatch_243_244_245_246(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_kif::m_trycatch_243_244(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="trycatch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_trycatch_243_244_245(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_kif::m_trycatch_243_247_248(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="trycatch";
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


char bnf_kif::m_trycatch_243_247(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="trycatch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_char(lret,';') || m_trycatch_243_247_248(lret,&subtree))
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


char bnf_kif::m_trycatch_243(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="trycatch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,3,"try")) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (x_test_string(lret,5,"catch")) 
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
 if (m_trycatch_243_244(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_trycatch_243_247(lret,&subtree)) 
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


char bnf_kif::m_trycatch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="trycatch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_trycatch_243(lret,&subtree))
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


char bnf_kif::m_testswitch_249_250(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testswitch";
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


char bnf_kif::m_testswitch_249(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testswitch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_testswitch_249_250(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_test_220_221(lret,&subtree)) 
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


char bnf_kif::m_testswitch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testswitch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_testswitch_249(lret,&subtree))
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
 static const char label[]="default";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,7,"default"))
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


char bnf_kif::m_predicatedefinition_252_253(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatedefinition";
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


char bnf_kif::m_predicatedefinition_252(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatedefinition";
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
 if (m_predicatedefinition_252_253(lret,&subtree)) 
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


char bnf_kif::m_predicatedefinition(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatedefinition";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_predicatedefinition_252(lret,&subtree))
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


char bnf_kif::m_predicatefact_254(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatefact";
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
 return(1);
 }


char bnf_kif::m_predicatefact(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="predicatefact";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_predicatefact_254(lret,&subtree))
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

char bnf_kif::m_afullstring(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char label[]="afullstring";
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
	static const char label[]="word";
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
	static const char label[]="typename";
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
	static const char label[]="anumber";
	if (fx->stacktype[currentpos]==3) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	if (fx->stack[currentpos]=="-" && fx->stacktype[currentpos+1]==3) {
		string s="-";
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
	static const char label[]="astringsimple";
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
	static const char label[]="astringdouble";
	if (fx->stacktype[currentpos]==1) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}
