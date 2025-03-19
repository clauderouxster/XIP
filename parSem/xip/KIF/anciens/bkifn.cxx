#include "bkifn.h"


static char* errorlabels[]={"","Wrong index format","Wrong expression after an operator","Wrong list of parameters","Wrong expressions in parentheses","Wrong FRAME declaration","Wrong argument declaration","Wrong FUNCTION declaration","Wrong BOOLEAN EXPRESSION","Wrong BOOLEAN expression","Wrong IF declaration","Wrong WHILE declaration","Wrong FOR declaration","Wrong FUNCTION CALL","Wrong Expression","Wrong Instruction","Wrong SWITCH","Wrong test in a SWITCH","Wrong IN expression","Wrong WITH expression","Wrong RULE declaration","Wrong PATTERN in A RULE","Wrong TRY-CATCH expression","Wrong INTERVAL","Wrong ELSE section","Wrong VECTOR definition","Wrong MAP definition","Wrong Function call in an index reference","Wrong comments","Wrong dotted expression","Wrong instruction","Missing '}'","Wrong declaration"};

char bnf_bkif::m_espace(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="espace";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,32))
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


char bnf_bkif::m_Capital_0_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="Capital";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (x_test_char(lret,']')) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
 subtree=NULL;
 if (x_test_ord(lret,'a','z'))
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
   if (x_test_char(lret,']')) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (x_test_ord(lret,'a','z'))
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


char bnf_bkif::m_Capital_0_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="Capital";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (x_test_ord(lret,'a','z'))
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
   if (x_test_ord(lret,'a','z'))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_bkif::m_Capital_0(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="Capital";
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
 if (m_espace(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_ord(lret,'A','Z')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_Capital_0_1(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_Capital_0_2(lret,&subtree)) 
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


char bnf_bkif::m_Capital(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="Capital";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_Capital_0(lret,&subtree))
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


char bnf_bkif::m_analyse_3(string& lreturn,x_node** tree) {
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
 if (m_espace(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_Capital(lret,&subtree)) 
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


char bnf_bkif::m_analyse(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="analyse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_analyse_3(lret,&subtree))
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




string bnf_bkif::x_errormsg(int i) {
	return errorlabels[i];
}


x_node* bnf_bkif::x_parsing(x_reading* xr,x_parsing_mode mode) {

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
		if (errornumber!=-1) 
			cerr<<endl<<"Error line:"<<fx->stackln[currentpos]<<"="<<errorlabels[errornumber]<<endl;
		else
			cerr<<endl<<"Error line:"<<fx->stackln[currentpos]<<endl;
		if (tree!=NULL)
                    delete tree;
                return NULL;
	}

	if (res==1)
       return tree;
       
    delete tree;
    return NULL;
}


char bnf_bkif::x_test_string(string& lret,int sz,string sub) {
	if (currentpos==fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}

char bnf_bkif::m_word(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos==fx->stack.size())
		return(0);
	static const char label[]="word";
	if (fx->stacktype[currentpos]==4) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}
