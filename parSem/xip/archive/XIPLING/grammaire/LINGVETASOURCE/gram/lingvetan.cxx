#include "lingvetan.h"


static char* errorlabels[]={""};

static char tab0[]={9,32,0};
static char tab1[]={'t','f','m',0};
static char tab2[]={'t','f','m',0};
static char tab3[]={'t','f','m',0};
static char tab4[]={'t','f','m',0};
static char tab5[]={'t','f','m',0};
static char tab6[]={'t','f','m',0};
static char tab7[]={'t','f','m',0};
static char tab8[]={'t','f','m',0};
char bnf_lingveta::m_common(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="common";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,'a'))
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


char bnf_lingveta::m_loc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="loc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,'e'))
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


char bnf_lingveta::m_time(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="time";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,'i'))
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


char bnf_lingveta::m_proper(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="proper";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,'o'))
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


char bnf_lingveta::m_num(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="num";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,'u'))
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


char bnf_lingveta::m_sem_0(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sem";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_common(lret,&subtree) || m_loc(lret,&subtree) || m_time(lret,&subtree) || m_proper(lret,&subtree) || m_num(lret,&subtree))
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


char bnf_lingveta::m_sem(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sem";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_sem_0(lret,&subtree))
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


char bnf_lingveta::m_space_1_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="space";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab0))
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


char bnf_lingveta::m_space_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="space";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_space_1_2(lret,&subtree))
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
   if (m_space_1_2(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_lingveta::m_space(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="space";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_space_1(lret,&subtree))
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


char bnf_lingveta::m_end_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="end";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_space(lret,&subtree) || m_EOF(lret,&subtree) || x_test_char(lret,'.'))
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


char bnf_lingveta::m_end(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="end";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_end_3(lret,&subtree))
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


char bnf_lingveta::m_que_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="que";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'q')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'u')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'e')) 
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


char bnf_lingveta::m_que(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="que";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_que_4(lret,&subtree))
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


char bnf_lingveta::m_quo_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="quo";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'q')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'u')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'o')) 
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


char bnf_lingveta::m_quo(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="quo";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_quo_5(lret,&subtree))
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


char bnf_lingveta::m_conj_6(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="conj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_que(lret,&subtree) || m_quo(lret,&subtree))
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


char bnf_lingveta::m_conj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="conj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_conj_6(lret,&subtree))
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


char bnf_lingveta::m_nounterm_7_8(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nounterm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab1))
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


char bnf_lingveta::m_nounterm_7(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nounterm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounterm_7_8(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_nounterm(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nounterm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nounterm_7(lret,&subtree))
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


char bnf_lingveta::m_adjterm_9(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjterm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_adjterm(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjterm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjterm_9(lret,&subtree))
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


char bnf_lingveta::m_detterm_10(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detterm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_detterm(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detterm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_detterm_10(lret,&subtree))
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


char bnf_lingveta::m_nountermacc_11_12(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab2))
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


char bnf_lingveta::m_nountermacc_11(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_nountermacc_11_12(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'n')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_nountermacc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nountermacc_11(lret,&subtree))
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


char bnf_lingveta::m_adjtermacc_13(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'n')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_adjtermacc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjtermacc_13(lret,&subtree))
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


char bnf_lingveta::m_dettermacc_14(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'n')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_dettermacc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dettermacc_14(lret,&subtree))
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


char bnf_lingveta::m_nountermgen_15_16(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermgen";
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
 lreturn+=lret;
 return(1);
 }


char bnf_lingveta::m_nountermgen_15(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_nountermgen_15_16(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_nountermgen(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nountermgen_15(lret,&subtree))
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


char bnf_lingveta::m_adjtermgen_17(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_adjtermgen(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjtermgen_17(lret,&subtree))
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


char bnf_lingveta::m_dettermgen_18(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_dettermgen(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dettermgen_18(lret,&subtree))
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


char bnf_lingveta::m_nountermdat_19_20(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermdat";
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
 lreturn+=lret;
 return(1);
 }


char bnf_lingveta::m_nountermdat_19(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_nountermdat_19_20(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_nountermdat(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nountermdat_19(lret,&subtree))
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


char bnf_lingveta::m_adjtermdat_21(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_adjtermdat(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjtermdat_21(lret,&subtree))
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


char bnf_lingveta::m_dettermdat_22(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_dettermdat(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dettermdat_22(lret,&subtree))
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


char bnf_lingveta::m_nountermconj_23_24(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermconj";
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
 lreturn+=lret;
 return(1);
 }


char bnf_lingveta::m_nountermconj_23(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_nountermconj_23_24(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_nountermconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nountermconj_23(lret,&subtree))
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


char bnf_lingveta::m_adjtermconj_25(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_adjtermconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjtermconj_25(lret,&subtree))
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


char bnf_lingveta::m_dettermconj_26(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_dettermconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dettermconj_26(lret,&subtree))
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


char bnf_lingveta::m_nountermaccconj_27_28(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermaccconj";
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
 lreturn+=lret;
 return(1);
 }


char bnf_lingveta::m_nountermaccconj_27(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_nountermaccconj_27_28(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'n')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_nountermaccconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nountermaccconj_27(lret,&subtree))
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


char bnf_lingveta::m_adjtermaccconj_29(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'n')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_adjtermaccconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjtermaccconj_29(lret,&subtree))
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


char bnf_lingveta::m_dettermaccconj_30(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'n')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_dettermaccconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dettermaccconj_30(lret,&subtree))
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


char bnf_lingveta::m_nountermgenconj_31_32(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_in(lret,tab7))
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


char bnf_lingveta::m_nountermgenconj_31(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_nountermgenconj_31_32(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_nountermgenconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nountermgenconj_31(lret,&subtree))
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


char bnf_lingveta::m_adjtermgenconj_33(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_adjtermgenconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjtermgenconj_33(lret,&subtree))
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


char bnf_lingveta::m_dettermgenconj_34(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_dettermgenconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dettermgenconj_34(lret,&subtree))
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


char bnf_lingveta::m_nountermdatconj_35_36(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermdatconj";
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
 lreturn+=lret;
 return(1);
 }


char bnf_lingveta::m_nountermdatconj_35(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_nountermdatconj_35_36(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_nountermdatconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nountermdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nountermdatconj_35(lret,&subtree))
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


char bnf_lingveta::m_adjtermdatconj_37(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'c')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_adjtermdatconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjtermdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjtermdatconj_37(lret,&subtree))
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


char bnf_lingveta::m_dettermdatconj_38(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'l')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sem(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_conj(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_dettermdatconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dettermdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dettermdatconj_38(lret,&subtree))
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


char bnf_lingveta::m_noun_39_40(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noun";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_ord(lret,'A','Z') || x_test_ord(lret,'a','z'))
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


char bnf_lingveta::m_noun_39(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noun";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_nounterm(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_nounterm(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_noun(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noun";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_noun_39(lret,&subtree))
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


char bnf_lingveta::m_adj_41(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_adjterm(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_adjterm(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_adj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adj_41(lret,&subtree))
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


char bnf_lingveta::m_det_43(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="det";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_detterm(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_detterm(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_det(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="det";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_det_43(lret,&subtree))
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


char bnf_lingveta::m_nounacc_45(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nounacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_nountermacc(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_nountermacc(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_nounacc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nounacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nounacc_45(lret,&subtree))
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


char bnf_lingveta::m_adjacc_47(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_adjtermacc(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_adjtermacc(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_adjacc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjacc_47(lret,&subtree))
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


char bnf_lingveta::m_detacc_49(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_dettermacc(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_dettermacc(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_detacc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_detacc_49(lret,&subtree))
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


char bnf_lingveta::m_noundat_51(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noundat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_nountermdat(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_nountermdat(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_noundat(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noundat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_noundat_51(lret,&subtree))
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


char bnf_lingveta::m_adjdat_53(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_adjtermdat(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_adjtermdat(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_adjdat(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjdat_53(lret,&subtree))
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


char bnf_lingveta::m_detdat_55(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_dettermdat(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_dettermdat(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_detdat(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_detdat_55(lret,&subtree))
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


char bnf_lingveta::m_noungen_57(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noungen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_nountermgen(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_nountermgen(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_noungen(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noungen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_noungen_57(lret,&subtree))
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


char bnf_lingveta::m_adjgen_59(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_adjtermgen(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_adjtermgen(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_adjgen(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjgen_59(lret,&subtree))
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


char bnf_lingveta::m_detgen_61(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_dettermgen(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_dettermgen(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_detgen(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_detgen_61(lret,&subtree))
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


char bnf_lingveta::m_nounconj_63(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nounconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_nountermconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_nountermconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_nounconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nounconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nounconj_63(lret,&subtree))
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


char bnf_lingveta::m_adjconj_65(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_adjtermconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_adjtermconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_adjconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjconj_65(lret,&subtree))
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


char bnf_lingveta::m_detconj_67(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_dettermconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_dettermconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_detconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_detconj_67(lret,&subtree))
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


char bnf_lingveta::m_nounaccconj_69(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nounaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_nountermaccconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_nountermaccconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_nounaccconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nounaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_nounaccconj_69(lret,&subtree))
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


char bnf_lingveta::m_adjaccconj_71(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_adjtermaccconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_adjtermaccconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_adjaccconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjaccconj_71(lret,&subtree))
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


char bnf_lingveta::m_detaccconj_73(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_dettermaccconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_dettermaccconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_detaccconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_detaccconj_73(lret,&subtree))
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


char bnf_lingveta::m_noundatconj_75(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noundatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_nountermdatconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_nountermdatconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_noundatconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noundatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_noundatconj_75(lret,&subtree))
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


char bnf_lingveta::m_adjdatconj_77(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_adjtermdatconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_adjtermdatconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_adjdatconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjdatconj_77(lret,&subtree))
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


char bnf_lingveta::m_detdatconj_79(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_dettermdatconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_dettermdatconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_detdatconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_detdatconj_79(lret,&subtree))
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


char bnf_lingveta::m_noungenconj_81(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noungenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_nountermgenconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_nountermgenconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_noungenconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="noungenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_noungenconj_81(lret,&subtree))
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


char bnf_lingveta::m_adjgenconj_83(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_adjtermgenconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_adjtermgenconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_adjgenconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adjgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adjgenconj_83(lret,&subtree))
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


char bnf_lingveta::m_detgenconj_85(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_dettermgenconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_dettermgenconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_detgenconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="detgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_detgenconj_85(lret,&subtree))
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


char bnf_lingveta::m_npm_87_88(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_det(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_npm_87_89(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_adj(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_npm_87(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_npm_87_88(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npm_87_89(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_noun(lret,&subtree)) 
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


char bnf_lingveta::m_npm(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npm_87(lret,&subtree))
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


char bnf_lingveta::m_npaccm_90_91(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npaccm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_detacc(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_npaccm_90_92(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npaccm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_adjacc(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_npaccm_90(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npaccm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_npaccm_90_91(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npaccm_90_92(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_nounacc(lret,&subtree)) 
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


char bnf_lingveta::m_npaccm(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npaccm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npaccm_90(lret,&subtree))
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


char bnf_lingveta::m_npgenm_93_94(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npgenm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_detgen(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_npgenm_93_95(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npgenm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_adjgen(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_npgenm_93(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npgenm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_npgenm_93_94(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npgenm_93_95(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_noungen(lret,&subtree)) 
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


char bnf_lingveta::m_npgenm(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npgenm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npgenm_93(lret,&subtree))
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


char bnf_lingveta::m_npdatm_96_97(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npdatm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_detdat(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_npdatm_96_98(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npdatm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_adjdat(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_npdatm_96(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npdatm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_npdatm_96_97(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npdatm_96_98(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_noundat(lret,&subtree)) 
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


char bnf_lingveta::m_npdatm(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npdatm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npdatm_96(lret,&subtree))
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


char bnf_lingveta::m_npconj_99_102(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_nounconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun(lret,&subtree))
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
   if (m_nounconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun(lret,&subtree))
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


char bnf_lingveta::m_npconj_99(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_npm_87_88(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npm_87_89(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npconj_99_102(lret,&subtree)) 
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


char bnf_lingveta::m_npconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npconj_99(lret,&subtree))
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


char bnf_lingveta::m_npaccconj_103_106(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_nounaccconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_nounacc(lret,&subtree))
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
   if (m_nounaccconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_nounacc(lret,&subtree))
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


char bnf_lingveta::m_npaccconj_103(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_npaccm_90_91(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npaccm_90_92(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npaccconj_103_106(lret,&subtree)) 
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


char bnf_lingveta::m_npaccconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npaccconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npaccconj_103(lret,&subtree))
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


char bnf_lingveta::m_npgenconj_107_110(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_noungenconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noungen(lret,&subtree))
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
   if (m_noungenconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noungen(lret,&subtree))
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


char bnf_lingveta::m_npgenconj_107(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_npgenm_93_94(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npgenm_93_95(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npgenconj_107_110(lret,&subtree)) 
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


char bnf_lingveta::m_npgenconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npgenconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npgenconj_107(lret,&subtree))
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


char bnf_lingveta::m_npdatconj_111_114(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_noundatconj(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noundat(lret,&subtree))
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
   if (m_noundatconj(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noundat(lret,&subtree))
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


char bnf_lingveta::m_npdatconj_111(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_npdatm_96_97(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npdatm_96_98(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_npdatconj_111_114(lret,&subtree)) 
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


char bnf_lingveta::m_npdatconj(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npdatconj";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npdatconj_111(lret,&subtree))
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


char bnf_lingveta::m_np_115(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="np";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_npconj(lret,&subtree) || m_npm(lret,&subtree))
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


char bnf_lingveta::m_np(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="np";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_np_115(lret,&subtree))
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


char bnf_lingveta::m_npacc_116(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_npaccconj(lret,&subtree) || m_npaccm(lret,&subtree))
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


char bnf_lingveta::m_npacc(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npacc";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npacc_116(lret,&subtree))
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


char bnf_lingveta::m_npgen_117(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_npgenconj(lret,&subtree) || m_npgenm(lret,&subtree))
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


char bnf_lingveta::m_npgen(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npgen";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npgen_117(lret,&subtree))
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


char bnf_lingveta::m_npdat_118(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_npdatconj(lret,&subtree) || m_npdatm(lret,&subtree))
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


char bnf_lingveta::m_npdat(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="npdat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_npdat_118(lret,&subtree))
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


char bnf_lingveta::m_verbpres_119_121(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpres";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'i')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'a')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'g')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_verbpres_119(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpres";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_verbpres_119_121(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_verbpres_119_121(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_verbpres(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpres";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_verbpres_119(lret,&subtree))
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


char bnf_lingveta::m_verbinf_122_124(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbinf";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'i')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'a')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'g')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'a')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_verbinf_122(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbinf";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_verbinf_122_124(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_verbinf_122_124(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_verbinf(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbinf";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_verbinf_122(lret,&subtree))
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


char bnf_lingveta::m_verbpast_125_127(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpast";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'i')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'a')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'p')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_verbpast_125(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpast";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_verbpast_125_127(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_verbpast_125_127(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_verbpast(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpast";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_verbpast_125(lret,&subtree))
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


char bnf_lingveta::m_verbfut_128_130(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbfut";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'i')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'a')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'f')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_verbfut_128(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbfut";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_verbfut_128_130(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_verbfut_128_130(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_verbfut(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbfut";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_verbfut_128(lret,&subtree))
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


char bnf_lingveta::m_verbcond_131_133(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbcond";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'i')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'a')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_verbcond_131(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbcond";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_verbcond_131_133(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_verbcond_131_133(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_verbcond(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbcond";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_verbcond_131(lret,&subtree))
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


char bnf_lingveta::m_verbpresrel_134_136(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpresrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'i')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'e')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'g')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_verbpresrel_134(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpresrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_verbpresrel_134_136(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_verbpresrel_134_136(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_verbpresrel(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpresrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_verbpresrel_134(lret,&subtree))
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


char bnf_lingveta::m_verbinfrel_137_139(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbinfrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'i')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'e')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'g')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'a')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'s')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_verbinfrel_137(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbinfrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_verbinfrel_137_139(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_verbinfrel_137_139(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_verbinfrel(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbinfrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_verbinfrel_137(lret,&subtree))
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


char bnf_lingveta::m_verbpastrel_140_142(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpastrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'i')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'e')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'p')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_verbpastrel_140(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpastrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_verbpastrel_140_142(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_verbpastrel_140_142(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_verbpastrel(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbpastrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_verbpastrel_140(lret,&subtree))
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


char bnf_lingveta::m_verbfutrel_143_145(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbfutrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'i')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'e')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'f')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_verbfutrel_143(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbfutrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_verbfutrel_143_145(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_verbfutrel_143_145(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_verbfutrel(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbfutrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_verbfutrel_143(lret,&subtree))
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


char bnf_lingveta::m_verbcondrel_146_148(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbcondrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'i')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'e')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'d')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_verbcondrel_146(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbcondrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_verbcondrel_146_148(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_verbcondrel_146_148(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_verbcondrel(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="verbcondrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_verbcondrel_146(lret,&subtree))
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


char bnf_lingveta::m_adverb_149_151(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adverb";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'n')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'o')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_adverb_149(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adverb";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_adverb_149_151(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_adverb_149_151(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_adverb(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adverb";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adverb_149(lret,&subtree))
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


char bnf_lingveta::m_vpnrel_152_153(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vpnrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_adverb(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_lingveta::m_vpnrel_152_154_155(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vpnrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_verbinf(lret,&subtree))
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
   if (m_verbinf(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_vpnrel_152_154(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vpnrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_vpnrel_152_154_155(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_lingveta::m_vpnrel_152_156(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vpnrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_verbpres(lret,&subtree) || m_verbpast(lret,&subtree) || m_verbfut(lret,&subtree) || m_verbcond(lret,&subtree))
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


char bnf_lingveta::m_vpnrel_152(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vpnrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_vpnrel_152_153(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_vpnrel_152_154(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_vpnrel_152_156(lret,&subtree)) 
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


char bnf_lingveta::m_vpnrel(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vpnrel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_vpnrel_152(lret,&subtree))
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


char bnf_lingveta::m_vprel_157_159_160(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vprel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_verbinfrel(lret,&subtree))
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
   if (m_verbinfrel(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_vprel_157_159(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vprel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_vprel_157_159_160(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 return(1);
 }


char bnf_lingveta::m_vprel_157_161(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vprel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_verbpresrel(lret,&subtree) || m_verbpastrel(lret,&subtree) || m_verbfutrel(lret,&subtree) || m_verbcondrel(lret,&subtree))
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


char bnf_lingveta::m_vprel_157(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vprel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_vpnrel_152_153(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_vprel_157_159(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_vprel_157_161(lret,&subtree)) 
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


char bnf_lingveta::m_vprel(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vprel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_vprel_157(lret,&subtree))
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


char bnf_lingveta::m_vp_162(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vp";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_vpnrel(lret,&subtree) || m_vprel(lret,&subtree))
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


char bnf_lingveta::m_vp(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vp";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_vp_162(lret,&subtree))
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


char bnf_lingveta::m_pron_163_165_166(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="pron";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (x_test_char(lret,'s'))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_lingveta::m_pron_163_165(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="pron";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'e')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'r')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_pron_163_165_166(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_end(lret,&subtree)) 
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


char bnf_lingveta::m_pron_163(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="pron";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_pron_163_165(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (m_noun_39_40(lret,&subtree))
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
   if (m_pron_163_165(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_noun_39_40(lret,&subtree))
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


char bnf_lingveta::m_pron(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="pron";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_pron_163(lret,&subtree))
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


char bnf_lingveta::m_sentencerel_167_168_169(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sentencerel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_sentencerel(lret,&subtree) || m_npacc(lret,&subtree) || m_npgen(lret,&subtree) || m_npdat(lret,&subtree) || m_np(lret,&subtree))
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


char bnf_lingveta::m_sentencerel_167_168(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sentencerel";
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
   if (m_vprel(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_sentencerel_167_168_169(lret,&subtree))
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


char bnf_lingveta::m_sentencerel_167(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sentencerel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_pron(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_sentencerel_167_168(lret,&subtree)) 
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


char bnf_lingveta::m_sentencerel(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sentencerel";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_sentencerel_167(lret,&subtree))
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


char bnf_lingveta::m_sentence_170(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sentence";
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
   if (m_vpnrel(lret,&subtree)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_sentencerel_167_168_169(lret,&subtree))
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


char bnf_lingveta::m_sentence(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="sentence";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_sentence_170(lret,&subtree))
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


char bnf_lingveta::m_parse_172_173(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_np(lret,&subtree))
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
   if (m_np(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 return(1);
 }


char bnf_lingveta::m_parse_172(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_sentence(lret,&subtree) || m_parse_172_173(lret,&subtree))
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


char bnf_lingveta::m_parse(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_parse_172(lret,&subtree))
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




string bnf_lingveta::x_errormsg(int i) {
	return errorlabels[i];
}


x_node* bnf_lingveta::x_parsing(x_reading* xr,x_parsing_mode mode) {
fx=xr;
	string lret;
	x_node* tree=new x_node;
	gFail=0;
	lineerror=-1;
	labelerror="";
	errornumber=-1;
	currentpos=0;
	intoken=0;
	char res=m_parse(lret,&tree);

	if (currentpos!=fx->stack.size() && mode==FULL) {
		if (errornumber!=-1) 
			cerr<<endl<<"Error line:"<<lineerror<<"="<<errorlabels[errornumber]<<endl;
		else
			cerr<<endl<<"Error line:"<<lineerror<<endl;
		if (tree!=NULL)
			delete tree;
		return NULL;
	}

	if (res==1)
		return tree;

	delete tree;
	return NULL;
}


char bnf_lingveta::x_test_string(string& lret,int sz,string sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}
