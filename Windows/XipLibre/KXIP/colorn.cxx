#include "colorn.h"


static char* errorlabels[]={""};

static char tab0[]={'é','à','è','ç','ê','î','ô','â','û','ù','Ç','À','É','È','Ë','Ê','Ï','Î','Û','Ü','ä','ë','ï','ö','ü','_','#',0};
static char tab1[]={13,10,0};
char bnf_color::m_char_0(string& lreturn,x_node** tree) {
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


char bnf_color::m_char(string& lreturn,x_node** tree) {
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


char bnf_color::m_specials_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="specials";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_string(lret,15,"garbagefunction") || x_test_string(lret,11,"garbagesize") || x_test_string(lret,16,"removeerroronkey") || x_test_string(lret,16,"setdebugfunction") || x_test_string(lret,14,"parentthreadid") || x_test_string(lret,12,"sparsevector") || x_test_string(lret,12,"setdebugdata") || x_test_string(lret,12,"createserver") || x_test_string(lret,12,"getstackinfo") || x_test_string(lret,11,"waitonfalse") || x_test_string(lret,11,"environment") || x_test_string(lret,11,"runadthread") || x_test_string(lret,11,"synchronous") || x_test_string(lret,11,"filebrowser") || x_test_string(lret,10,"polynomial") || x_test_string(lret,10,"printlnerr") || x_test_string(lret,10,"waitonjoin") || x_test_string(lret,10,"myvariable") || x_test_string(lret,10,"marginleft") || x_test_string(lret,9,"exclusive") || x_test_string(lret,9,"wtreeitem") || x_test_string(lret,9,"protected") || x_test_string(lret,9,"attribute") || x_test_string(lret,9,"riterator") || x_test_string(lret,8,"fraction") || x_test_string(lret,8,"printerr") || x_test_string(lret,8,"function") || x_test_string(lret,8,"sequence") || x_test_string(lret,8,"threadid") || x_test_string(lret,8,"pathname") || x_test_string(lret,8,"continue") || x_test_string(lret,8,"iterator") || x_test_string(lret,8,"instance") || x_test_string(lret,7,"println") || x_test_string(lret,7,"address") || x_test_string(lret,7,"boolean") || x_test_string(lret,7,"autorun") || x_test_string(lret,7,"private") || x_test_string(lret,7,"browser") || x_test_string(lret,7,"wchoice") || x_test_string(lret,7,"woutput") || x_test_string(lret,7,"waiting") || x_test_string(lret,7,"version") || x_test_string(lret,6,"sparse") || x_test_string(lret,6,"wheely") || x_test_string(lret,6,"wheelx") || x_test_string(lret,6,"remote") || x_test_string(lret,6,"minute") || x_test_string(lret,6,"gcsize") || x_test_string(lret,6,"socket") || x_test_string(lret,6,"button") || x_test_string(lret,6,"scroll") || x_test_string(lret,6,"switch") || x_test_string(lret,6,"xmldoc") || x_test_string(lret,6,"editor") || x_test_string(lret,6,"loadin") || x_test_string(lret,6,"winput") || x_test_string(lret,6,"slider") || x_test_string(lret,6,"wgroup") || x_test_string(lret,6,"wimage") || x_test_string(lret,6,"sqlite") || x_test_string(lret,6,"pflush") || x_test_string(lret,6,"string") || x_test_string(lret,6,"common") || x_test_string(lret,6,"random") || x_test_string(lret,6,"bottom") || x_test_string(lret,6,"vector") || x_test_string(lret,6,"return") || x_test_string(lret,6,"thread") || x_test_string(lret,6,"window") || x_test_string(lret,6,"second") || x_test_string(lret,6,"strict") || x_test_string(lret,5,"print") || x_test_string(lret,5,"catch") || x_test_string(lret,5,"table") || x_test_string(lret,5,"false") || x_test_string(lret,5,"eigen") || x_test_string(lret,5,"wtabs") || x_test_string(lret,5,"frame") || x_test_string(lret,5,"while") || x_test_string(lret,5,"start") || x_test_string(lret,5,"stdin") || x_test_string(lret,3,"not") || x_test_string(lret,5,"notin") || x_test_string(lret,5,"pause") || x_test_string(lret,5,"const") || x_test_string(lret,5,"sleep") || x_test_string(lret,5,"reset") || x_test_string(lret,5,"range") || x_test_string(lret,5,"break") || x_test_string(lret,5,"raise") || x_test_string(lret,5,"float") || x_test_string(lret,5,"wtree") || x_test_string(lret,5,"class") || x_test_string(lret,4,"bool") || x_test_string(lret,4,"self") || x_test_string(lret,4,"eval") || x_test_string(lret,4,"true") || x_test_string(lret,4,"cast") || x_test_string(lret,4,"this") || x_test_string(lret,4,"getc") || x_test_string(lret,4,"long") || x_test_string(lret,4,"time") || x_test_string(lret,4,"weka") || x_test_string(lret,4,"call") || x_test_string(lret,4,"list") || x_test_string(lret,4,"port") || x_test_string(lret,4,"exit") || x_test_string(lret,4,"arff") || x_test_string(lret,4,"file") || x_test_string(lret,4,"enum") || x_test_string(lret,4,"hash") || x_test_string(lret,4,"bits") || x_test_string(lret,4,"null") || x_test_string(lret,4,"toto") || x_test_string(lret,4,"kill") || x_test_string(lret,3,"int") || x_test_string(lret,3,"isa") || x_test_string(lret,3,"use") || x_test_string(lret,3,"lib") || x_test_string(lret,3,"top") || x_test_string(lret,3,"for") || x_test_string(lret,3,"box") || x_test_string(lret,3,"try") || x_test_string(lret,3,"bit") || x_test_string(lret,4,"fmap") || x_test_string(lret,4,"imap") || x_test_string(lret,4,"smap") || x_test_string(lret,3,"map") || x_test_string(lret,3,"sys") || x_test_string(lret,3,"age") || x_test_string(lret,3,"xml") || x_test_string(lret,2,"if") || x_test_string(lret,2,"in") || x_test_string(lret,3,"pas") || x_test_string(lret,7,"default") || x_test_string(lret,4,"curl") || x_test_string(lret,4,"else") || x_test_string(lret,4,"elif") || x_test_string(lret,4,"with"))
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


char bnf_color::m_specials(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="specials";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_specials_1(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_color::m_fonction_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="fonction";
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
 return(1);
 }


char bnf_color::m_fonction(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="fonction";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_fonction_2(lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,false);
 return(1);
}


char bnf_color::m_punct(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="punct";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_any(lret))
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


char bnf_color::m_comments_3_4_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comments";
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


char bnf_color::m_comments_3_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comments";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//CONSTRAINT
 subtree=NULL;
 if (m_comments_3_4_5(lret,&subtree)) {
     x_pop_node(tree,addsubtree);
     currentpos=pos;
		intoken=itok;
     setfail(exitonfail);
     return(0);
 }
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
 int foundsolution=0;
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_comments_3_4_5(lret,&subtree)) {
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
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_color::m_comments_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comments";
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
 if (m_comments_3_4(lret,&subtree)) 
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


char bnf_color::m_comments(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="comments";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_comments_3(lret,&subtree))
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


char bnf_color::m_analyse_6_7(string& lreturn,x_node** tree) {
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
 if (m_fonction(lret,&subtree) || m_specials(lret,&subtree) || m_comments(lret,&subtree) || m_astringdouble(lret,&subtree) || m_astringsimple(lret,&subtree) || m_word(lret,&subtree) || m_punct(lret,&subtree))
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


char bnf_color::m_analyse_6(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="analyse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_analyse_6_7(lret,&subtree))
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
   if (m_analyse_6_7(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_color::m_analyse(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="analyse";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_analyse_6(lret,&subtree))
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




string bnf_color::x_errormsg(int i) {
	return errorlabels[i];
}


x_node* bnf_color::x_parsing(x_reading* xr,x_parsing_mode mode,bool display) {
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


char bnf_color::x_test_string(string& lret,int sz,string sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}

char bnf_color::m_word(string& lreturn,x_node** tree) {
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

char bnf_color::m_astringsimple(string& lreturn,x_node** tree) {
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

char bnf_color::m_astringdouble(string& lreturn,x_node** tree) {
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
