#include "kifn.h"


static char* errorlabels[]={"","Wrong index format","Wrong expression after an operator","Wrong list of parameters","Wrong expressions in parentheses","Wrong FRAME declaration","Wrong argument declaration","Wrong FUNCTION declaration","Wrong BOOLEAN EXPRESSION","Wrong BOOLEAN expression","Wrong IF declaration","Wrong WHILE declaration","Wrong FOR declaration","Wrong FUNCTION CALL","Wrong XPATH","Wrong Instruction","Wrong SWITCH","Wrong test in a SWITCH","Wrong IN expression","Wrong WITH expression","Wrong RULE declaration","Wrong PATTERN in A RULE","Wrong TRY-CATCH expression","Wrong INTERVAL","Wrong ELSE section","Wrong VECTOR definition","Wrong MAP definition","Wrong Function call in an index reference","Wrong comments","Wrong dotted expression","Wrong instruction","Missing '}'"};

static char tab0[]={10,13,0};
static char tab1[]={32,9,0};
static char tab2[]={'é','à','è','ç','ê','î','ô','â','û','ù','Ç','À','É','È','Ë','Ê','Ï','Î','Û','Ü','ä','ë','ï','ö','ü','_',0};
static char tab3[]={'&','|',0};
static char tab4[]={'+','-','*','/','^','%',0};
static char tab5[]={'<','>',':',0};
static char tab6[]={'k','K',0};
static char tab7[]={'i','I',0};
static char tab8[]={'f','F',0};
char bnf_kif::m_rc_0(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="rc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab0))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_rc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="rc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_rc_0(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_space_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="space";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab1) || m_rc(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_space_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="space";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_space_1_2(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 while (!fx->eof()) {
   subtree=NULL;
   if (m_space_1_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_space(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="space";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_space_1(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_pop_node(tree,addsubtree);
 return(1);
}


char bnf_kif::m_char_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="char";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_ord(lret,'A','Z') || x_test_ord(lret,'a','z') || x_test_in(lret,tab2))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_char(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="char";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_char_3(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_word_4_5_6(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="word";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_char(lret,&subtree) || x_test_ord(lret,'0','9'))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_word_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="word";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (!fx->eof()) {
   subtree=NULL;
   if (m_word_4_5_6(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_word_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="word";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_char(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word_4_5(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_word(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="word";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_word_4(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_type(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="type";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_word_4(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_anydouble_10_11(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anydouble";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'\\')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_any(lret)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anydouble_10_12(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anydouble";
 string lret;
 long pos=fx->tell();
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
   fx->seek(pos);
   setfail(exitonfail);
   return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anydouble_10(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anydouble";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_anydouble_10_11(lret,&subtree) || m_anydouble_10_12(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anydouble(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anydouble";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_anydouble_10(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_anysimple_13(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anysimple";
 string lret;
 long pos=fx->tell();
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
   fx->seek(pos);
   setfail(exitonfail);
   return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anysimple(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anysimple";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_anysimple_13(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_anycolon_14_15(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anycolon";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'%')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_any(lret)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anycolon_14_16(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anycolon";
 string lret;
 long pos=fx->tell();
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
   fx->seek(pos);
   setfail(exitonfail);
   return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anycolon_14(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anycolon";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_anycolon_14_15(lret,&subtree) || m_anycolon_14_16(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anycolon(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anycolon";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_anycolon_14(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_operator_17_18(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operator_17_19(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operator_17(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab3) || m_operator_17_18(lret,&subtree) || m_operator_17_19(lret,&subtree) || x_test_in(lret,tab4))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operator(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operator_17(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_operatoraffectation_20_21(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_20_22(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_20_23(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'+')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_20_24(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'-')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_20_25(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'*')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_20_26(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'/')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_20_27(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'^')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_20_28(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'%')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_20_29(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'&')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_20_30(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation_20(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_char(lret,'=') || m_operatoraffectation_20_21(lret,&subtree) || m_operatoraffectation_20_22(lret,&subtree) || m_operatoraffectation_20_23(lret,&subtree) || m_operatoraffectation_20_24(lret,&subtree) || m_operatoraffectation_20_25(lret,&subtree) || m_operatoraffectation_20_26(lret,&subtree) || m_operatoraffectation_20_27(lret,&subtree) || m_operatoraffectation_20_28(lret,&subtree) || m_operatoraffectation_20_29(lret,&subtree) || m_operatoraffectation_20_30(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operatoraffectation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operatoraffectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operatoraffectation_20(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_orand_31_32(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="orand";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_orand_31_33(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="orand";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'&')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'&')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_orand_31(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="orand";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_orand_31_32(lret,&subtree) || m_orand_31_33(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_orand(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="orand";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_orand_31(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_comparator_34_35(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="comparator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'!')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_34_36(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="comparator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'~')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_34_37(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="comparator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_34_38(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="comparator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_34_39(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="comparator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_34_40(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="comparator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator_34(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="comparator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_comparator_34_35(lret,&subtree) || m_comparator_34_36(lret,&subtree) || m_comparator_34_37(lret,&subtree) || m_comparator_34_38(lret,&subtree) || m_comparator_34_39(lret,&subtree) || m_comparator_34_40(lret,&subtree) || x_test_in(lret,tab5))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparator(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="comparator";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_comparator_34(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_declaration_41_42(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="declaration";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_space(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_declaration_41_43_44(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="declaration";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_parameters(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_declaration_41_43(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="declaration";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_declaration_41_43_44(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_declaration_41_46_47_48(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="declaration";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_bodyrule(lret,&subtree) || m_expressions(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_declaration_41_46_47(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="declaration";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_46_47_48(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_declaration_41_46(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="declaration";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_declaration_41_46_47(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_declaration_41(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="declaration";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_type(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_43(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_46(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_declaration(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="declaration";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_declaration_41(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_treepattern_49_50(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="treepattern";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (x_test_char(lret,'|')) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (x_test_any(lret))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (x_test_char(lret,'|')) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (x_test_any(lret))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_treepattern_49(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="treepattern";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'|')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),21);
//BODYSEQUENCE
 subtree=NULL;
 if (m_treepattern_49_50(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_treepattern(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="treepattern";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_treepattern_49(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_xipblocend_51_52(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="xipblocend";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYCONTEXTUAL
 long poscontext=fx->tell();
 string lretbis;
 if (x_test_char(lretbis,';')) {
    fx->seek(poscontext);
 }
 else {
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_xipblocend_51(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="xipblocend";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_xipblocend_51_52(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_xipblocend(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="xipblocend";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_xipblocend_51(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_xipbloc_53_54(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="xipbloc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_xipblocend(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (x_test_any(lret))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_xipblocend(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (x_test_any(lret))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_xipbloc_53(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="xipbloc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),20);
//BODYSEQUENCE
 subtree=NULL;
 if (m_xipbloc_53_54(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_xipbloc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="xipbloc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_xipbloc_53(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_bodyrule_55_57_58_60(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="bodyrule";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (x_test_string(lret,2,"if"))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_bodyrule_55_57_58(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="bodyrule";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_treepattern(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_bodyrule_55_57_58_60(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_bodyrule_55_57(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="bodyrule";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_bodyrule_55_57_58(lret,&subtree) || x_test_string(lret,2,"if"))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_bodyrule_55_61_62(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="bodyrule";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_anycolon(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 while (!fx->eof()) {
   subtree=NULL;
   if (m_anycolon(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_bodyrule_55_61(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="bodyrule";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_xipbloc(lret,&subtree) || m_bodyrule_55_61_62(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_bodyrule_55(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="bodyrule";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_bodyrule_55_57(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_bodyrule_55_61(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_bodyrule(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="bodyrule";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_bodyrule_55(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_declarationending(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="declarationending";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,';'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_frame_63_66(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="frame";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_declarationending(lret,&subtree) || m_instruction(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_frame_63(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="frame";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,5,"frame")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_space(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),5);
//BODYSEQUENCE
 subtree=NULL;
 if (m_type(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_frame_63_66(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_frame(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="frame";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_frame_63(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_instruction_68_70(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="instruction";
 string lret;
 long pos=fx->tell();
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


char bnf_kif::m_instruction_68(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="instruction";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),15);
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_instruction_68_70(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_instruction(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="instruction";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_instruction_68(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_arguments_72_74_75(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="arguments";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),6);
//BODYSEQUENCE
 subtree=NULL;
 if (m_arguments(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_arguments_72_74(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="arguments";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_arguments_72_74_75(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_arguments_72(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="arguments";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_arguments_72_74(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_arguments(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="arguments";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_arguments_72(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_function_76_78(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="function";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_arguments(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_function_76(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="function";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,8,"function")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_space(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),7);
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_function_76_78(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_frame_63_66(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_function(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="function";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_function_76(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_commentaires_82_84(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="commentaires";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_rc(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (x_test_any(lret))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_commentaires_82(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="commentaires";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'/')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),28);
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'/')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_commentaires_82_84(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_commentaires(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="commentaires";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_commentaires_82(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_pop_node(tree,addsubtree);
 return(1);
}


char bnf_kif::m_sousbloc_86(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="sousbloc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_declaration(lret,&subtree) || m_affectation(lret,&subtree) || m_expressions(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_sousbloc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="sousbloc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_sousbloc_86(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_bloc_87(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="bloc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sousbloc(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),30);
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,';')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_bloc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="bloc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_bloc_87(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_complexbloc_90(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="complexbloc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_switch(lret,&subtree) || m_trycatch(lret,&subtree) || m_loop(lret,&subtree) || m_test(lret,&subtree) || m_for(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_complexbloc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="complexbloc";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_complexbloc_90(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_blocs_91_92(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="blocs";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_commentaires(lret,&subtree) || m_frame(lret,&subtree) || m_complexbloc(lret,&subtree) || m_function(lret,&subtree) || m_bloc(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_blocs_91(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="blocs";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_blocs_91_92(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 while (!fx->eof()) {
   subtree=NULL;
   if (m_blocs_91_92(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_blocs(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="blocs";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_blocs_91(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_analyse_93_95(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="analyse";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_commentaires(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_analyse_93_96_97(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="analyse";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab6))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_analyse_93_96_98(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="analyse";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab7))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_analyse_93_96_99(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="analyse";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab8))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_analyse_93_96(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="analyse";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_93_96_97(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_93_96_98(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_93_96_99(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_analyse_93(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="analyse";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_93_95(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_analyse_93_96(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocs(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_analyse(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="analyse";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_analyse_93(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_regularcall_102_103(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="regularcall";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_parameters(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_regularcall_102_104_105_106(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="regularcall";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),27);
//BODYSEQUENCE
 subtree=NULL;
 if (m_regularcall(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_regularcall_102_104_105(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="regularcall";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_interval(lret,&subtree) || m_indexes(lret,&subtree) || m_regularcall_102_104_105_106(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_regularcall_102_104(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="regularcall";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_regularcall_102_104_105(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_regularcall_102(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="regularcall";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),13);
//BODYSEQUENCE
 subtree=NULL;
 if (m_regularcall_102_103(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_regularcall_102_104(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_regularcall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="regularcall";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_regularcall_102(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_returncall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="returncall";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,6,"return"))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_breakpointcall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="breakpointcall";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,10,"breakpoint"))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_breakcall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="breakcall";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,5,"break"))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_continuecall(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="continuecall";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,8,"continue"))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_call_107(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="call";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_breakpointcall(lret,&subtree) || m_breakcall(lret,&subtree) || m_continuecall(lret,&subtree) || m_regularcall(lret,&subtree) || m_returncall(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_call(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="call";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_call_107(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_sep_108(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="sep";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_sep(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="sep";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_sep_108(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_indexes_111(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="indexes";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),1);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_regularcall_102_104(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_indexes(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="indexes";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_indexes_111(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_interval_115_117(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="interval";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_numbers(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_interval_115(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="interval";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_interval_115_117(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),23);
//BODYSEQUENCE
 subtree=NULL;
 if (m_interval_115_117(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_regularcall_102_104(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_interval(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="interval";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_interval_115(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_variable_123_124_125_126_127(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="variable";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_regularcall(lret,&subtree) || m_variable(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_variable_123_124_125_126(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="variable";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),29);
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable_123_124_125_126_127(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_variable_123_124_125(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="variable";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_interval(lret,&subtree) || m_indexes(lret,&subtree) || m_variable_123_124_125_126(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_variable_123_124(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="variable";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_variable_123_124_125(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_variable_123(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="variable";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable_123_124(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_variable(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="variable";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_variable_123(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_affectation_128(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_variable(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_operatoraffectation(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),14);
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_46_47_48(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_affectation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_affectation_128(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_operationin_132_133_134(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operationin";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,4,"with")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),19);
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operationin_132_133(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operationin";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_operationin_132_133_134(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operationin_132(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operationin";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,2,"in")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),18);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_operationin_132_133(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operationin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operationin";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operationin_132(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_operation_137(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_operator(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_operation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operation_137(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_comparison_139(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="comparison";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_comparator(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_comparison(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="comparison";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_comparison_139(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_list_141_144_145(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="list";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_list(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_list_141_144(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="list";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_list_141_144_145(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_list_141(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="list";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_list_141_144(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_list(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="list";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_list_141(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_valvector_147_148(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valvector";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_list(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_valvector_147(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valvector";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),25);
//BODYSEQUENCE
 subtree=NULL;
 if (m_valvector_147_148(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_valvector(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valvector";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_valvector_147(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_dkey_150(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dkey";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_astringdouble(lret,&subtree) || m_astringsimple(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_dkey(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dkey";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dkey_150(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_dvalue(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dvalue";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_expressions(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_dico_151_156_157(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dico";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dico(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_dico_151_156(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dico";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_dico_151_156_157(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_dico_151(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dico";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dkey(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dvalue(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dico_151_156(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_dico(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dico";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dico_151(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_valmap_159_160(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valmap";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_dico(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_valmap_159(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valmap";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),26);
//BODYSEQUENCE
 subtree=NULL;
 if (m_valmap_159_160(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_valmap(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valmap";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_valmap_159(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_negation_162_163(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="negation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,3,"not")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_space(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_negation_162(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="negation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_char(lret,'~') || m_negation_162_163(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_negation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="negation";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_negation_162(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_parameters_164_165_166(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parameters";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),3);
//BODYSEQUENCE
 subtree=NULL;
 if (m_parameters(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_parameters_164_165(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parameters";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_parameters_164_165_166(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_parameters_164(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parameters";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_parameters_164_165(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_parameters(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parameters";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_parameters_164(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_optional_167(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="optional";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),4);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_optional(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="optional";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_optional_167(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_expression_168_170(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expression";
 string lret;
 long pos=fx->tell();
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


char bnf_kif::m_expression_168_172(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_xpathexpression(lret,&subtree) || m_abool(lret,&subtree) || m_astringdouble(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_call(lret,&subtree) || m_optional(lret,&subtree) || m_variable(lret,&subtree) || m_valvector(lret,&subtree) || m_valmap(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_expression_168(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression_168_170(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression_168_172(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_expression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_expression_168(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_expressions_174_175_176(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expressions";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_operationin(lret,&subtree) || m_operation(lret,&subtree) || m_comparison(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_expressions_174_175(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expressions";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_expressions_174_175_176(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_expressions_174(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expressions";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expressions_174_175(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_expressions(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expressions";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_expressions_174(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_numbers_177_178(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="numbers";
 string lret;
 long pos=fx->tell();
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


char bnf_kif::m_numbers_177(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="numbers";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_numbers_177_178(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_numbers(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="numbers";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_numbers_177(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_astringdouble_179_180(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="astringdouble";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (x_test_char(lret,34)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_anydouble(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_astringdouble_179(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="astringdouble";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,34)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_astringdouble_179_180(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_astringdouble(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="astringdouble";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_astringdouble_179(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_astringsimple_181_182(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="astringsimple";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (x_test_char(lret,39)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_anysimple(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_astringsimple_181(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="astringsimple";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,39)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_astringsimple_181_182(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_astringsimple(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="astringsimple";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_astringsimple_181(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_anumber_183_184(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anumber";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (x_test_char(lret,'-'))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anumber_183_185(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anumber";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (x_test_ord(lret,'0','9'))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 while (!fx->eof()) {
   subtree=NULL;
   if (x_test_ord(lret,'0','9'))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anumber_183_186_187(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anumber";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_anumber_183_185(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anumber_183_186(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anumber";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_anumber_183_186_187(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anumber_183(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anumber";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_anumber_183_184(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_anumber_183_185(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_anumber_183_186(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_anumber(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="anumber";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_anumber_183(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_abool_189(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="abool";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,4,"true") || x_test_string(lret,5,"false"))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_abool(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="abool";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_abool_189(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_optionalboolean_190(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="optionalboolean";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),9);
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_optionalboolean(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="optionalboolean";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_optionalboolean_190(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_booleanexpression_191_193_194_196(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="booleanexpression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_optionalboolean(lret,&subtree) || m_booleanexpression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_booleanexpression_191_193_194(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="booleanexpression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_orand(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),8);
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression_191_193_194_196(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_booleanexpression_191_193(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="booleanexpression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_booleanexpression_191_193_194(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_booleanexpression_191(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="booleanexpression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_expressions(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression_191_193(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_booleanexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="booleanexpression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_booleanexpression_191(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_test_197_201(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="test";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_instruction(lret,&subtree) || m_complexbloc(lret,&subtree) || m_bloc(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_test_197_203(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="test";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_testelif(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_test_197_205_206(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="test";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,4,"else")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),24);
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_197_201(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_test_197_205(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="test";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_test_197_205_206(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_test_197(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="test";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,2,"if")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),10);
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_197_201(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_197_203(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_197_205(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_test(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="test";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_test_197(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_testelif_210(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testelif";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,4,"elif")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),10);
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_197_201(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_197_203(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_197_205(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_testelif(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testelif";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_testelif_210(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_loop_223(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="loop";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,5,"while")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),11);
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_197_201(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_loop(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="loop";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_loop_223(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_for_229_232(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="for";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_affectation(lret,&subtree) || m_expression(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_for_229(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="for";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,3,"for")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),12);
//BODYSEQUENCE
 subtree=NULL;
 if (m_for_229_232(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,';')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_booleanexpression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,';')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_for_229_232(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_197_201(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_for(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="for";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_for_229(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_switch_237_242(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="switch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_testswitch(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 while (!fx->eof()) {
   subtree=NULL;
   if (m_testswitch(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_switch_237(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="switch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,6,"switch")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),16);
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),31);
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_switch_237_242(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_switch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="switch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_switch_237(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_trycatch_245_253(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="trycatch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_word(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_trycatch_245(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="trycatch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,3,"try")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),22);
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_blocs(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,5,"catch")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_trycatch_245_253(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,';')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_trycatch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="trycatch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_trycatch_245(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_testswitch_257_258(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testswitch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_default(lret,&subtree) || m_astringdouble(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_abool(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_testswitch_257(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testswitch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_testswitch_257_258(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),17);
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_test_197_201(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_testswitch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testswitch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_testswitch_257(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_default(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="default";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_string(lret,7,"default"))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_kif::m_xpathexpression_263_265(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="xpathexpression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (x_test_char(lret,')')) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (x_test_any(lret))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_xpathexpression_263(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="xpathexpression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'@')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_declaration_41_42(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 exitonfail=true;
 initfail(label,fx->tell(),14);
//BODYSEQUENCE
 subtree=NULL;
 if (m_xpathexpression_263_265(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'-')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_word(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_xpathexpression_263_265(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_kif::m_xpathexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="xpathexpression";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_xpathexpression_263(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}



void bnf_kif::x_displaytree(x_node* tree,int depth) {
    if (tree==NULL)
        return;
    int i;
    for (i=0;i<depth;i++)
        cout<<" ";
    cout<<tree->token<<"("<<tree->start<<","<<tree->end<<"):"<<tree->value<<endl;
    for (i=0;i<tree->nodes.size();i++) 
        x_displaytree(tree->nodes[i],depth+1);
}

string bnf_kif::x_errormsg(int i) {
	return errorlabels[i];
}

void bnf_kif::extractmax(x_node* tree,long& themax) {
    if (tree==NULL)
        return;

    for (int i=0;i<tree->nodes.size();i++) {
        if (tree->nodes[i]==NULL)
            continue;
        if (tree->nodes[i]->end>themax)
            themax=tree->nodes[i]->end;
        extractmax(tree->nodes[i],themax);
    }
}


void bnf_kif::extracttext(long themax) {
	fx->seek(themax);
	string s=fx->read(255);
	cout<<(char*)s.c_str()<<endl;
}

bool bnf_kif::x_complong(const long premier,const long  second) {
	if (premier<second)
		return true;
	return false;
}

x_node* bnf_kif::x_parsing(x_reading* xr,x_parsing_mode mode) {
    fx=xr;
    if (fx->error())
            return NULL;

   x_recordcr.clear();
    
   string lret;
    x_node* tree=new x_node;
    gFail=0;
    fx->lastposerror=0;
    fx->labelerror="";
    fx->errornumber=-1;
    char res=m_analyse(lret,&tree);
    long themax=fx->lastposerror;
    long line=0;
    map<long,long>::iterator it;
    line=baseline;
    for (it=x_recordcr.begin();it!=x_recordcr.end();it++) {
        it->second=line;
        line++;
    }
    
    if (mode==FULL && !fx->eof()) {
       res=0;
       if (themax==0)
           extractmax(tree,themax);
       line=x_linenumber(themax);
       if (fx->errornumber!=-1)
		   cerr<<endl<<"Error line:"<<line<<"="<<errorlabels[fx->errornumber]<<endl;
       else
		   cerr<<endl<<"Error line:"<<line<<"="<<(char*)fx->labelerror.c_str()<<endl;           
       extracttext(themax);
    }
    
    fx->close();
    if (res==1)
       return tree;
       
    delete tree;
    return NULL;
}


x_node* bnf_kif::x_parse_string(string x,x_parsing_mode mode) {
    fx=new x_readstring(x);
    x_node* r=x_parsing(fx,mode);
    delete fx;
    return r;
}

x_node* bnf_kif::x_parse_file(string x,x_parsing_mode mode) {
    fx=new x_readfile((char*)x.c_str());
    x_node* r=x_parsing(fx,mode);
    delete fx;
    return r;
}

