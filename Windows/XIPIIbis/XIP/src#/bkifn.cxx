#include "bkifn.h"


static char* errorlabels[]={"","Wrong index format","Wrong expression after an operator","Wrong list of parameters","Wrong expressions in parentheses","Wrong FRAME declaration","Wrong argument declaration","Wrong FUNCTION declaration","Wrong BOOLEAN EXPRESSION","Wrong BOOLEAN expression","Wrong IF declaration","Wrong WHILE declaration","Wrong FOR declaration","Wrong FUNCTION CALL","Wrong Expression","Wrong Instruction","Wrong SWITCH","Wrong test in a SWITCH","Wrong IN expression","Wrong WITH expression","Wrong RULE declaration","Wrong PATTERN in A RULE","Wrong TRY-CATCH expression","Wrong INTERVAL","Wrong ELSE section","Wrong VECTOR definition","Wrong MAP definition","Wrong Function call in an index reference","Wrong comments","Wrong dotted expression","Wrong instruction","Missing '}'","Wrong declaration"};

static char tab0[]={10,13,0};
static char tab1[]={'é','à','è','ç','ê','î','ô','â','û','ù','Ç','À','É','È','Ë','Ê','Ï','Î','Û','Ü','ä','ë','ï','ö','ü','_','#',0};
char bnf_bkif::m_rc_0(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="rc";
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


char bnf_bkif::m_rc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="rc";
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
 x_pop_node(tree,addsubtree);
 return(1);
}


char bnf_bkif::m_shell_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="shell";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_rc(lret,&subtree)) {
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


char bnf_bkif::m_shell_1_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="shell";
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


char bnf_bkif::m_shell_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="shell";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'#')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
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
 if (m_shell_1_2(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_shell_1_3(lret,&subtree)) 
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


char bnf_bkif::m_shell(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="shell";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_shell_1(lret,&subtree))
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


char bnf_bkif::m_commentaires_4_6(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="commentaires";
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


char bnf_bkif::m_commentaires_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="commentaires";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_shell_1_3(lret,&subtree)) 
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
 if (m_commentaires_4_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_shell_1_3(lret,&subtree)) 
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


char bnf_bkif::m_commentaires(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="commentaires";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_commentaires_4(lret,&subtree))
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


char bnf_bkif::m_char_8(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="char";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_ord(lret,'A','Z') || x_test_ord(lret,'a','z') || x_test_in(lret,tab1))
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


char bnf_bkif::m_char(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="char";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_char_8(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell(),true);
 return(1);
}


char bnf_bkif::m_digit(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="digit";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_ord(lret,'0','9'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell(),true);
 return(1);
}


char bnf_bkif::m_ifor_9_10(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="ifor";
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
 return(1);
 }


char bnf_bkif::m_ifor_9_12(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="ifor";
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
 return(1);
 }


char bnf_bkif::m_ifor_9_13(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="ifor";
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
 return(1);
 }


char bnf_bkif::m_ifor_9(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="ifor";
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
 initfail(label,fx->tell(),12);
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_10(lret,&subtree)) 
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
 if (m_ifor_9_10(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_13(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_bkif::m_ifor(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="ifor";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_ifor_9(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell(),false);
 return(1);
}


char bnf_bkif::m_ifi_15_19(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="ifi";
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
 return(1);
 }


char bnf_bkif::m_ifi_15_21_22(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="ifi";
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
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_13(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_bkif::m_ifi_15_21(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="ifi";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_ifi_15_21_22(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_bkif::m_ifi_15(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="ifi";
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
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_13(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifi_15_19(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifi_15_21(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_bkif::m_ifi(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="ifi";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_ifi_15(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell(),false);
 return(1);
}


char bnf_bkif::m_iloop_26(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="iloop";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_13(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_bkif::m_iloop(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="iloop";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_iloop_26(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell(),false);
 return(1);
}


char bnf_bkif::m_iswitch_31_36(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="iswitch";
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
 return(1);
 }


char bnf_bkif::m_iswitch_31(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="iswitch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_iswitch_31_36(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_bkif::m_iswitch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="iswitch";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_iswitch_31(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell(),false);
 return(1);
}


char bnf_bkif::m_itry_39_45(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="itry";
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
 return(1);
 }


char bnf_bkif::m_itry_39_48_49(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="itry";
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
 initfail(label,fx->tell(),22);
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
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
 return(1);
 }


char bnf_bkif::m_itry_39_48(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="itry";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_char(lret,';') || m_itry_39_48_49(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(exitonfail);
   return 0;
 }
 return(1);
 }


char bnf_bkif::m_itry_39(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="itry";
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
 initfail(label,fx->tell(),22);
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_itry_39_45(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
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
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_itry_39_48(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor_9_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 return(1);
 }


char bnf_bkif::m_itry(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="itry";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_itry_39(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell(),false);
 return(1);
}


char bnf_bkif::m_instruction_53_55_56(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
 if (m_space(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifor(lret,&subtree)) 
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


char bnf_bkif::m_instruction_53_55_57(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
 if (m_space(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_iwhile(lret,&subtree)) 
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


char bnf_bkif::m_instruction_53_55_58(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
 if (m_space(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_ifi(lret,&subtree)) 
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


char bnf_bkif::m_instruction_53_55_59(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
 if (m_space(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_iswitch(lret,&subtree)) 
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


char bnf_bkif::m_instruction_53_55_60(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
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
 if (m_space(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_itry(lret,&subtree)) 
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


char bnf_bkif::m_instruction_53_55(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_instruction_53_55_56(lret,&subtree) || m_instruction_53_55_57(lret,&subtree) || m_instruction_53_55_58(lret,&subtree) || m_instruction_53_55_59(lret,&subtree) || m_instruction_53_55_60(lret,&subtree))
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


char bnf_bkif::m_instruction_53(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_shell_1_3(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_instruction_53_55(lret,&subtree)) 
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


char bnf_bkif::m_instruction(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="instruction";
 string lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_instruction_53(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell(),true);
 return(1);
}



void bnf_bkif::x_displaytree(x_node* tree,int depth) {
    if (tree==NULL)
        return;
    int i;
    for (i=0;i<depth;i++)
        cout<<" ";
    cout<<tree->token<<"("<<tree->start<<","<<tree->end<<"):"<<tree->value<<endl;
    for (i=0;i<tree->nodes.size();i++) 
        x_displaytree(tree->nodes[i],depth+1);
}

string bnf_bkif::x_errormsg(int i) {
	return errorlabels[i];
}

void bnf_bkif::extractmax(x_node* tree,long& themax) {
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


void bnf_bkif::extracttext(long themax) {
	fx->seek(themax);
	string s=fx->read(255);
	cout<<(char*)s.c_str()<<endl;
}

bool bnf_bkif::x_complong(const long premier,const long  second) {
	if (premier<second)
		return true;
	return false;
}

x_node* bnf_bkif::x_parsing(x_reading* xr,x_parsing_mode mode) {
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


x_node* bnf_bkif::x_parse_string(string x,x_parsing_mode mode) {
    fx=new x_readstring(x);
    x_node* r=x_parsing(fx,mode);
    delete fx;
    return r;
}

x_node* bnf_bkif::x_parse_file(string x,x_parsing_mode mode) {
    fx=new x_readfile((char*)x.c_str());
    x_node* r=x_parsing(fx,mode);
    delete fx;
    return r;
}


bool digitonly(string& thestr) {
	string::iterator it=thestr.begin();
	static char tabs[]={'0','1','2','3','4','5','6','7','8','9','.',0};
	if (*it=='-')
		it++;
	for (;it!=thestr.end();it++) 
		if (strchr(tabs,*it)==NULL)
			return false;		
	return true;
}


void bnf_bkif::loadtoken(x_reading* xr) {
        long cpos=xr->pos;
	string s;
	static char tabaccent[]={'é','à','è','ç','ê','î','ô','â','û','ù','Ç','À','É','È','Ë','Ê','Ï','Î','Û','Ü','ä','ë','ï','ö','ü','_','#',0};
	char table[255];
	strcpy(table,tabaccent);
	int i=strlen(table);
	char c;
	for (c=65;c<=90;c++)
		table[i++]=c;
	for (c=97;c<=122;c++)
		table[i++]=c;
	for (c=48;c<=57;c++)
		table[i++]=c;
	table[i]=0;
	string token;
	tokens.clear();
	numbers.clear();
	doublestring.clear();
	simplestring.clear();
	long pos=xr->tell();
	while (!xr->eof()) {
		s=xr->read(1);
		if (s!="") {
			if (s[0]=='/' && token=="") {
				pos=xr->tell();
				s=xr->read(1);
				if (s[0]=='/') {
					//this is a comment
					s=xr->read(1);
					while (s[0]!=10 && s[0]!=13)
						s=xr->read(1);
					pos=xr->tell();
					continue;
				}
				xr->seek(pos);
				s='/';
			}
			c=s[0];
			if (c=='"') {
				token+=c;
				s=xr->read(1);
				c=s[0];
				while (c!='"') {
					if (c=='\\') {
						token+=c;
						s=xr->read(1);
						if (s=="")
							break;
						c=s[0];
					}						
					token+=c;
					s=xr->read(1);
					if (s=="")
						break;
					c=s[0];
				}
				token+='"';
				doublestring[pos]=token;
				pos=xr->tell();
				token="";
				continue;
			}
			if (c=='\'') {
				token+=c;
				s=xr->read(1);
				c=s[0];
				while (c!='\'') {
					token+=c;
					s=xr->read(1);
					if (s=="")
						break;
					c=s[0];
				}
				token+='\'';
				simplestring[pos]=token;
				pos=xr->tell();
				token="";
				continue;
			}
			if (strchr(table,c))
				token+=c;
			else {
				if (token!="" && token!="-") {					
					if (digitonly(token)) {
						if (c=='.') {
							token+=c;
							continue;
						}
						numbers[pos]=token;
						if (token[0]=='-')
							numbers[pos+1]=token.c_str()+1;
					}
					else
						if (token[0]=='-')
							tokens[pos+1]=token.c_str()+1;
						else
							tokens[pos]=token;
					pos=xr->tell();
					//We have consume it already
					if (c=='-')
						pos--;
				}
				if (c=='-')
					token=c;
				else {
					token="";
					pos=xr->tell();
				}
			}
		}
	}
	xr->reset(cpos);
}

char bnf_bkif::x_test_string(string& lret,int sz,string sub) {
	long pos=fx->tell();
	if (tokens.find(pos)==tokens.end())
		return 0;

	string& tok=tokens[pos];
	if (tok==sub) {
		lret+=tok;
		fx->seek(pos+sz);
		return(1);
	}
	else
		return 0;
}

char bnf_bkif::m_word(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	static const char label[]="word";
	string lret;
	long pos=fx->tell();
	if (tokens.find(pos)!=tokens.end()) {
		lret=tokens[pos];		
		fx->seek(pos+lret.size());
		x_init_node(lreturn,lret,tree,label,pos,fx->tell(),true);
		return(1);
	}
	return(0);
}
