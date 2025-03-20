#include "lingvetan.h"


static char* errorlabels[]={""};

static char tab0[]={10,13,0};
static char tab1[]={32,9,0};
char bnf_lingveta::m_rc_0(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="rc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab0))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(label,pos,exitonfail,lasterrornumber);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_rc(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="rc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_rc_0(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_space_1_2(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="space";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab1) || m_rc(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(label,pos,exitonfail,lasterrornumber);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_space_1(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="space";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYWHILE
 subtree=NULL;
 if (m_space_1_2(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 while (!fx->eof()) {
   subtree=NULL;
   if (m_space_1_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_space(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="space";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_space_1(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_pop_node(tree,addsubtree);
 return(1);
}


char bnf_lingveta::m_common(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="common";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (x_test_char(lret,'a'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_loc(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="loc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (x_test_char(lret,'e'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_time(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="time";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (x_test_char(lret,'i'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_proper(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="proper";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (x_test_char(lret,'o'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_num(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="num";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (x_test_char(lret,'u'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_sem_3(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="sem";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYOR
 subtree=NULL;
 if (m_common(lret,&subtree) || m_loc(lret,&subtree) || m_time(lret,&subtree) || m_proper(lret,&subtree) || m_num(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(label,pos,exitonfail,lasterrornumber);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_sem(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="sem";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_sem_3(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_acc(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="acc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (x_test_char(lret,'n'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_dat(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (x_test_char(lret,'d'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_gen(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="gen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (x_test_char(lret,'s'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_case_4(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="case";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYOR
 subtree=NULL;
 if (m_acc(lret,&subtree) || m_dat(lret,&subtree) || m_gen(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(label,pos,exitonfail,lasterrornumber);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_case(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="case";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_case_4(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_nounterm_5_6_7_8(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nounterm";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYOPTIONAL
 subtree=NULL;
 if (m_space(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_nounterm_5_6_7(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nounterm";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6_7_8(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_nounterm_5_6(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nounterm";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYOR
 subtree=NULL;
 if (m_nounterm_5_6_7(lret,&subtree) || m_space(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(label,pos,exitonfail,lasterrornumber);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_nounterm_5(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nounterm";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'t')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_nounterm(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nounterm";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_nounterm_5(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_adjterm_9(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjterm";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_adjterm(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjterm";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_adjterm_9(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_detterm_13(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="detterm";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_detterm(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="detterm";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_detterm_13(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_nountermacc_17(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nountermacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'t')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'n')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_nountermacc(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nountermacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_nountermacc_17(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_adjtermacc_21(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjtermacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'n')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_adjtermacc(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjtermacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_adjtermacc_21(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_dettermacc_25(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dettermacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'n')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_dettermacc(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dettermacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_dettermacc_25(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_nountermgen_29(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nountermgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'t')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_nountermgen(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nountermgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_nountermgen_29(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_adjtermgen_33(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjtermgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_adjtermgen(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjtermgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_adjtermgen_33(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_dettermgen_37(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dettermgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_dettermgen(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dettermgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_dettermgen_37(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_nountermdat_41(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nountermdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'t')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_nountermdat(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nountermdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_nountermdat_41(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_adjtermdat_45(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjtermdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_adjtermdat(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjtermdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_adjtermdat_45(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_dettermdat_49(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dettermdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_dettermdat(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dettermdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_dettermdat_49(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_noun_53_54(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="noun";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYOR
 subtree=NULL;
 if (x_test_ord(lret,'A','Z') || x_test_ord(lret,'a','z'))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(label,pos,exitonfail,lasterrornumber);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_noun_53(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="noun";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_nounterm(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_nounterm(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_noun(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="noun";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_noun_53(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_adj_55(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adj";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_adjterm(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_adjterm(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_adj(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adj";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_adj_55(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_det_57(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="det";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_detterm(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_detterm(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_det(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="det";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_det_57(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_nounacc_59(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nounacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_nountermacc(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_nountermacc(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_nounacc(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="nounacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_nounacc_59(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_adjacc_61(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_adjtermacc(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_adjtermacc(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_adjacc(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_adjacc_61(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_detacc_63(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="detacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_dettermacc(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_dettermacc(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_detacc(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="detacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_detacc_63(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_noundat_65(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="noundat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_nountermdat(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_nountermdat(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_noundat(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="noundat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_noundat_65(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_adjdat_67(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_adjtermdat(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_adjtermdat(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_adjdat(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_adjdat_67(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_detdat_69(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="detdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_dettermdat(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_dettermdat(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_detdat(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="detdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_detdat_69(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_noungen_71(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="noungen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_nountermgen(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_nountermgen(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_noungen(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="noungen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_noungen_71(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_adjgen_73(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_adjtermgen(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_adjtermgen(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_adjgen(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adjgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_adjgen_73(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_detgen_75(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="detgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 subtree=NULL;
 if (m_dettermgen(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     fx->seek(pos);
     setfail(label,pos,exitonfail,lasterrornumber);
     return(0);
 }
 subtree=NULL;
 if (m_noun_53_54(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_dettermgen(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_53_54(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_detgen(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="detgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_detgen_75(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_np_77(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="np";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_adj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_det(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_np(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="np";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_np_77(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_npacc_78(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="npacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_adjacc(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_detacc(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_npacc(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="npacc";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_npacc_78(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_npgen_79(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="npgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_adjgen(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_detgen(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_npgen(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="npgen";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_npgen_79(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_npdat_80(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="npdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//CONSTRAINT
 int foundsolution=0;
 while (!fx->eof()) {
   subtree=NULL;
   if (m_adjdat(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_detdat(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_npdat(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="npdat";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_npdat_80(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}


char bnf_lingveta::m_parse_81_82_83(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parse";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYOR
 subtree=NULL;
 if (m_np(lret,&subtree) || m_npacc(lret,&subtree) || m_npgen(lret,&subtree) || m_npdat(lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx->seek(pos);
   setfail(label,pos,exitonfail,lasterrornumber);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_parse_81_82(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parse";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYWHILE
 subtree=NULL;
 if (m_parse_81_82_83(lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 while (!fx->eof()) {
   subtree=NULL;
   if (m_parse_81_82_83(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_parse_81(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parse";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
//BODYSEQUENCE
 subtree=NULL;
 if (m_parse_81_82(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_5_6_7_8(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char bnf_lingveta::m_parse(vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parse";
 vector<string> lret;
 long pos=fx->tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 int lasterrornumber=-1;
 if (m_parse_81(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx->seek(pos);
    setfail(label,pos,exitonfail,lasterrornumber);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx->tell());
 return(1);
}



void bnf_lingveta::x_displaytree(x_node* tree,int depth) {
    if (tree==NULL)
        return;
    int i;
    for (i=0;i<depth;i++)
        cout<<" ";
    cout<<tree->token<<"("<<tree->start<<","<<tree->end<<"):"<<tree->value<<endl;
    for (i=0;i<tree->nodes.size();i++) 
        x_displaytree(tree->nodes[i],depth+1);
}

void bnf_lingveta::extractmax(x_node* tree,long& themax) {
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


void bnf_lingveta::extracttext(long themax) {
	fx->seek(themax);
	string s=fx->read(255);
	cout<<(char*)s.c_str()<<endl;
}

bool bnf_lingveta::x_complong(const long premier,const long  second) {
	if (premier<second)
		return true;
	return false;
}

x_node* bnf_lingveta::x_parsing(x_reading* xr,x_parsing_mode mode) {
    fx=xr;
    if (fx->error())
            return NULL;

   x_recordcr.clear();
    
   vector<string> lret;
    x_node* tree=new x_node;
    gFail=0;
    fx->lastposerror=0;
    fx->labelerror="";
    fx->errornumber=-1;
    char res=m_parse(lret,&tree);
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


x_node* bnf_lingveta::x_parse_string(string x,x_parsing_mode mode) {
    fx=new x_readstring(x);
    x_node* r=x_parsing(fx,mode);
    delete fx;
    return r;
}

x_node* bnf_lingveta::x_parse_file(string x,x_parsing_mode mode) {
    fx=new x_readfile((char*)x.c_str());
    x_node* r=x_parsing(fx,mode);
    delete fx;
    return r;
}

