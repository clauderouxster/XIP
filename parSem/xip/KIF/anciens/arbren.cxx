#include "arbren.h"


static char* errorlabels[]={""};

static char tab0[]={'*','+',0};
static char tab1[]={'+','-','*','#','?',0};
static char tab2[]={'|','&',0};
static char tab3[]={'+','-','*','/',0};
static char tab4[]={'#','^',0};
static char tab5[]={'|','&',')','.',0};
char bnf_arbre::m_longestmatch_0(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="longestmatch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'@')) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_arbre::m_longestmatch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="longestmatch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_longestmatch_0(lret,&subtree))
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


char bnf_arbre::m_shortestmatch(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="shortestmatch";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,'='))
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


char bnf_arbre::m_arrow_1(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="arrow";
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


char bnf_arbre::m_arrow(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="arrow";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_arrow_1(lret,&subtree))
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


char bnf_arbre::m_match_2(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="match";
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
 if (m_longestmatch(lret,&subtree)) {
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
 if (m_shortestmatch(lret,&subtree)) {
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
 if (m_arrow(lret,&subtree)) {
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
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_match(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="match";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_match_2(lret,&subtree))
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


char bnf_arbre::m_var_3(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="var";
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
 if (m_number(lret,&subtree)) 
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


char bnf_arbre::m_var(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="var";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_var_3(lret,&subtree))
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


char bnf_arbre::m_pos_4(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="pos";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_name(lret,&subtree) || x_test_char(lret,'?'))
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


char bnf_arbre::m_pos(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="pos";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_pos_4(lret,&subtree))
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


char bnf_arbre::m_kleene_5(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="kleene";
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


char bnf_arbre::m_kleene(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="kleene";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_kleene_5(lret,&subtree))
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


char bnf_arbre::m_featonto_6_7(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featonto";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,'@')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_name(lret,&subtree)) 
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


char bnf_arbre::m_featonto_6(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featonto";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_featonto_6_7(lret,&subtree))
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
   if (m_featonto_6_7(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_featonto(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featonto";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_featonto_6(lret,&subtree))
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


char bnf_arbre::m_feat_8_9(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="feat";
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
 if (x_test_string(lret,5,"stack")) 
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


char bnf_arbre::m_feat_8(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="feat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_featonto(lret,&subtree) || m_name(lret,&subtree) || m_feat_8_9(lret,&subtree))
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


char bnf_arbre::m_feat(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="feat";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_feat_8(lret,&subtree))
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


char bnf_arbre::m_valmin_10(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valmin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_name(lret,&subtree) || m_number(lret,&subtree) || x_test_in(lret,tab1) || m_neg(lret,&subtree))
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


char bnf_arbre::m_valmin(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valmin";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_valmin_10(lret,&subtree))
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


char bnf_arbre::m_neg(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="neg";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,'~'))
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


char bnf_arbre::m_valnorm_11(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valnorm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_valquote(lret,&subtree) || m_valmin(lret,&subtree))
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


char bnf_arbre::m_valnorm(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valnorm";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_valnorm_11(lret,&subtree))
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


char bnf_arbre::m_valneg_12(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valneg";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_neg(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valnorm_11(lret,&subtree)) 
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


char bnf_arbre::m_valneg(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valneg";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_valneg_12(lret,&subtree))
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


char bnf_arbre::m_val_14(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="val";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_valneg(lret,&subtree) || m_valnorm(lret,&subtree))
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


char bnf_arbre::m_val(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="val";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_val_14(lret,&subtree))
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


char bnf_arbre::m_featvalbloque_15(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featvalbloque";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_feat(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (x_test_char(lret,'!')) 
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


char bnf_arbre::m_featvalbloque(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featvalbloque";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_featvalbloque_15(lret,&subtree))
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


char bnf_arbre::m_featvalfree_16(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featvalfree";
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
 if (m_feat(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (x_test_char(lret,'!')) 
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


char bnf_arbre::m_featvalfree(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featvalfree";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_featvalfree_16(lret,&subtree))
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


char bnf_arbre::m_featvalfreeinstance_17(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featvalfreeinstance";
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
 if (m_feat(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_val(lret,&subtree)) 
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


char bnf_arbre::m_featvalfreeinstance(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featvalfreeinstance";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_featvalfreeinstance_17(lret,&subtree))
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


char bnf_arbre::m_featvaltest_18(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featvaltest";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_feat(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_val(lret,&subtree)) 
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


char bnf_arbre::m_featvaltest(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featvaltest";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_featvaltest_18(lret,&subtree))
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


char bnf_arbre::m_featvalinstance_19(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featvalinstance";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_feat(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_val(lret,&subtree)) 
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


char bnf_arbre::m_featvalinstance(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featvalinstance";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_featvalinstance_19(lret,&subtree))
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


char bnf_arbre::m_feature_20(string& lreturn,x_node** tree) {
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
 if (m_featvalfree(lret,&subtree) || m_featvalbloque(lret,&subtree) || m_featvalfreeinstance(lret,&subtree) || m_featvaltest(lret,&subtree) || m_featvalinstance(lret,&subtree) || m_feat(lret,&subtree))
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


char bnf_arbre::m_feature(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="feature";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_feature_20(lret,&subtree))
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


char bnf_arbre::m_featurenext_21(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featurenext";
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
 if (m_feature(lret,&subtree)) 
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


char bnf_arbre::m_featurenext(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featurenext";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_featurenext_21(lret,&subtree))
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


char bnf_arbre::m_valeurontologie(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valeurontologie";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_featonto_6_7(lret,&subtree))
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


char bnf_arbre::m_featontologie_23_24(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featontologie";
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
   if (m_valeurontologie(lret,&subtree))
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


char bnf_arbre::m_featontologie_23(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featontologie";
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
 if (m_valeurontologie(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie_23_24(lret,&subtree)) 
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


char bnf_arbre::m_featontologie(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featontologie";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_featontologie_23(lret,&subtree))
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


char bnf_arbre::m_listefeature_25_26(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="listefeature";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_featurenext(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_listefeature_25(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="listefeature";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_feature(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listefeature_25_26(lret,&subtree)) 
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


char bnf_arbre::m_listefeature(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="listefeature";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_listefeature_25(lret,&subtree))
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


char bnf_arbre::m_features_27(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="features";
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
 if (m_listefeature(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_arbre::m_features(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="features";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_features_27(lret,&subtree))
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


char bnf_arbre::m_featuresreading_28_29(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featuresreading";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (x_test_char(lret,'*'))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_featuresreading_28(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featuresreading";
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
 if (m_featuresreading_28_29(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listefeature(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_arbre::m_featuresreading(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="featuresreading";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_featuresreading_28(lret,&subtree))
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


char bnf_arbre::m_nodefeatures(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="nodefeatures";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_features_27(lret,&subtree))
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


char bnf_arbre::m_subtree_31(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="subtree";
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
 if (m_listecategory(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_arbre::m_subtree(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="subtree";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_subtree_31(lret,&subtree))
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


char bnf_arbre::m_category_32_33_34_35(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="category";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_kleene(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_category_32_33_34_36(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="category";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_var(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_category_32_33_34(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="category";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_pos(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_32_33_34_35(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_32_33_34_36(lret,&subtree)) 
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


char bnf_arbre::m_category_32_33(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="category";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_var(lret,&subtree) || m_category_32_33_34(lret,&subtree))
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


char bnf_arbre::m_category_32_38(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="category";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_nodefeatures(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_category_32_39(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="category";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_subtree(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_category_32(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="category";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_32_33(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_32_33_34_35(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_32_38(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_32_39(lret,&subtree)) 
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


char bnf_arbre::m_category(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="category";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_category_32(lret,&subtree))
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


char bnf_arbre::m_categorysequence_40_41(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="categorysequence";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_categoryopt(lret,&subtree) || m_category(lret,&subtree))
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


char bnf_arbre::m_categorysequence_40(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="categorysequence";
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
 if (m_categorysequence_40_41(lret,&subtree)) 
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


char bnf_arbre::m_categorysequence(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="categorysequence";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_categorysequence_40(lret,&subtree))
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


char bnf_arbre::m_categoryexclusive_42(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="categoryexclusive";
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
 if (m_categorysequence_40_41(lret,&subtree)) 
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


char bnf_arbre::m_categoryexclusive(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="categoryexclusive";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_categoryexclusive_42(lret,&subtree))
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


char bnf_arbre::m_categorynext_44(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="categorynext";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_categorysequence(lret,&subtree) || m_categoryexclusive(lret,&subtree))
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


char bnf_arbre::m_categorynext(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="categorynext";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_categorynext_44(lret,&subtree))
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


char bnf_arbre::m_categoryopt_45(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="categoryopt";
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
 if (m_listecategory(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_category_32_33_34_35(lret,&subtree)) 
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


char bnf_arbre::m_categoryopt(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="categoryopt";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_categoryopt_45(lret,&subtree))
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


char bnf_arbre::m_listecategory_47_49(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="listecategory";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_categorynext(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_listecategory_47(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="listecategory";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_categorysequence_40_41(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listecategory_47_49(lret,&subtree)) 
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


char bnf_arbre::m_listecategory(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="listecategory";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_listecategory_47(lret,&subtree))
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


char bnf_arbre::m_pointeur(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="pointeur";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (x_test_char(lret,'$'))
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


char bnf_arbre::m_vardepbase_50_51(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vardepbase";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_pointeur(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_vardepbase_50(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vardepbase";
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
 if (m_vardepbase_50_51(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_number(lret,&subtree)) 
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


char bnf_arbre::m_vardepbase(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vardepbase";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_vardepbase_50(lret,&subtree))
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


char bnf_arbre::m_vardepnode_52(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vardepnode";
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
 if (m_vardepbase_50_51(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
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
 if (m_number(lret,&subtree)) 
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


char bnf_arbre::m_vardepnode(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vardepnode";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_vardepnode_52(lret,&subtree))
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


char bnf_arbre::m_vardep_54(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vardep";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_vardepbase(lret,&subtree) || m_vardepnode(lret,&subtree))
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


char bnf_arbre::m_vardep(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="vardep";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_vardep_54(lret,&subtree))
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


char bnf_arbre::m_namedep_55(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="namedep";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (x_test_char(lret,'?') || m_name(lret,&subtree))
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


char bnf_arbre::m_namedep(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="namedep";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_namedep_55(lret,&subtree))
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


char bnf_arbre::m_dependency_56_57_58_59_60(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_vardep(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_dependency_56_57_58_59(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_namedep(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency_56_57_58_59_60(lret,&subtree)) 
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


char bnf_arbre::m_dependency_56_57_58(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_dependency_56_57_58_59(lret,&subtree) || m_vardep(lret,&subtree))
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


char bnf_arbre::m_dependency_56_57_61(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_features(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_dependency_56_57(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency_56_57_58(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency_56_57_61(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_listecategory(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_arbre::m_dependency_56(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_dependency_56_57(lret,&subtree) || m_vardep(lret,&subtree) || x_test_char(lret,'?'))
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


char bnf_arbre::m_dependency(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dependency_56(lret,&subtree))
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


char bnf_arbre::m_subdependency_62(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="subdependency";
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
 if (m_listedependencies(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_arbre::m_subdependency(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="subdependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_subdependency_62(lret,&subtree))
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


char bnf_arbre::m_adependency_63_64(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_subdependency(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_adependency_63(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_adependency_63_64(lret,&subtree)) 
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


char bnf_arbre::m_adependency(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="adependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_adependency_63(lret,&subtree))
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


char bnf_arbre::m_dependencynext_65(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dependencynext";
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
 if (m_adependency(lret,&subtree)) 
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


char bnf_arbre::m_dependencynext(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="dependencynext";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_dependencynext_65(lret,&subtree))
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


char bnf_arbre::m_listedependencies_66_67(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="listedependencies";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_dependencynext(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_listedependencies_66(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="listedependencies";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_adependency(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listedependencies_66_67(lret,&subtree)) 
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


char bnf_arbre::m_listedependencies(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="listedependencies";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_listedependencies_66(lret,&subtree))
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


char bnf_arbre::m_generation_68(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="generation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_listedependencies(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_arbre::m_generation(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="generation";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_generation_68(lret,&subtree))
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


char bnf_arbre::m_etou_69(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="etou";
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


char bnf_arbre::m_etou(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="etou";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_etou_69(lret,&subtree))
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


char bnf_arbre::m_varexpression_70(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="varexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_etou(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
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


char bnf_arbre::m_varexpression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="varexpression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_varexpression_70(lret,&subtree))
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


char bnf_arbre::m_egauxontologie_71_72(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="egauxontologie";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_neg(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_egauxontologie_71(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="egauxontologie";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxontologie_71_72(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie(lret,&subtree)) 
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


char bnf_arbre::m_egauxontologie(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="egauxontologie";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_egauxontologie_71(lret,&subtree))
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


char bnf_arbre::m_compatibleontologie_73(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="compatibleontologie";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxontologie_71_72(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie(lret,&subtree)) 
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


char bnf_arbre::m_compatibleontologie(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="compatibleontologie";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_compatibleontologie_73(lret,&subtree))
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


char bnf_arbre::m_affectationontologie_75_76(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectationontologie";
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
   if (x_test_char(lret,'}')) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_varexpression(lret,&subtree))
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


char bnf_arbre::m_affectationontologie_75(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectationontologie";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationontologie_75_76(lret,&subtree)) 
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


char bnf_arbre::m_affectationontologie(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectationontologie";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_affectationontologie_75(lret,&subtree))
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


char bnf_arbre::m_compatiblelecture_77(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="compatiblelecture";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxontologie_71_72(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading(lret,&subtree)) 
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


char bnf_arbre::m_compatiblelecture(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="compatiblelecture";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_compatiblelecture_77(lret,&subtree))
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


char bnf_arbre::m_egauxlecture_79(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="egauxlecture";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxontologie_71_72(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading(lret,&subtree)) 
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


char bnf_arbre::m_egauxlecture(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="egauxlecture";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_egauxlecture_79(lret,&subtree))
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


char bnf_arbre::m_affectationlecture_81(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectationlecture";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationontologie_75_76(lret,&subtree)) 
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


char bnf_arbre::m_affectationlecture(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectationlecture";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_affectationlecture_81(lret,&subtree))
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


char bnf_arbre::m_compatibletrait_83(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="compatibletrait";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxontologie_71_72(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(lret,&subtree)) 
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


char bnf_arbre::m_compatibletrait(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="compatibletrait";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_compatibletrait_83(lret,&subtree))
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


char bnf_arbre::m_egauxtrait_85(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="egauxtrait";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxontologie_71_72(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(lret,&subtree)) 
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


char bnf_arbre::m_egauxtrait(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="egauxtrait";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_egauxtrait_85(lret,&subtree))
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


char bnf_arbre::m_affectationtrait_87(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectationtrait";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationontologie_75_76(lret,&subtree)) 
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


char bnf_arbre::m_affectationtrait(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="affectationtrait";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_affectationtrait_87(lret,&subtree))
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


char bnf_arbre::m_egalite_89(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="egalite";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxontologie_71_72(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
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


char bnf_arbre::m_egalite(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="egalite";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_egalite_89(lret,&subtree))
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


char bnf_arbre::m_inferieur_91(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="inferieur";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
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


char bnf_arbre::m_inferieur(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="inferieur";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_inferieur_91(lret,&subtree))
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


char bnf_arbre::m_superieur_92(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="superieur";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_var(lret,&subtree)) 
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


char bnf_arbre::m_superieur(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="superieur";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_superieur_92(lret,&subtree))
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


char bnf_arbre::m_inftraits_93_94_95(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="inftraits";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency_56_57_61(lret,&subtree)) 
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


char bnf_arbre::m_inftraits_93_94(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="inftraits";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_number(lret,&subtree) || m_inftraits_93_94_95(lret,&subtree))
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


char bnf_arbre::m_inftraits_93(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="inftraits";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_inftraits_93_94(lret,&subtree)) 
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


char bnf_arbre::m_inftraits(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="inftraits";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_inftraits_93(lret,&subtree))
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


char bnf_arbre::m_suptraits_97(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="suptraits";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_inftraits_93_94(lret,&subtree)) 
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


char bnf_arbre::m_suptraits(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="suptraits";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_suptraits_97(lret,&subtree))
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


char bnf_arbre::m_infegaltraits_101(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="infegaltraits";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
//BODYSEQUENCE
 subtree=NULL;
 if (m_inftraits_93_94(lret,&subtree)) 
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


char bnf_arbre::m_infegaltraits(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="infegaltraits";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_infegaltraits_101(lret,&subtree))
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


char bnf_arbre::m_supegaltraits_105(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="supegaltraits";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
//BODYSEQUENCE
 subtree=NULL;
 if (m_inftraits_93_94(lret,&subtree)) 
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


char bnf_arbre::m_supegaltraits(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="supegaltraits";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_supegaltraits_105(lret,&subtree))
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


char bnf_arbre::m_testsimple_109(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testsimple";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(lret,&subtree)) 
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


char bnf_arbre::m_testsimple(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testsimple";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_testsimple_109(lret,&subtree))
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


char bnf_arbre::m_operateurmathematique_110(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operateurmathematique";
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


char bnf_arbre::m_operateurmathematique(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="operateurmathematique";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_operateurmathematique_110(lret,&subtree))
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


char bnf_arbre::m_chaine_111(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="chaine";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,34)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_name(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(lret,34)) 
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


char bnf_arbre::m_chaine(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="chaine";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_chaine_111(lret,&subtree))
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


char bnf_arbre::m_valeurnumerique_112_113(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valeurnumerique";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (x_test_ord(lret,'0','9'))
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
   if (x_test_ord(lret,'0','9'))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_valeurnumerique_112_114_115(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valeurnumerique";
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
 if (m_valeurnumerique_112_113(lret,&subtree)) 
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


char bnf_arbre::m_valeurnumerique_112_114(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valeurnumerique";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_valeurnumerique_112_114_115(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_valeurnumerique_112(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valeurnumerique";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_valeurnumerique_112_113(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valeurnumerique_112_114(lret,&subtree)) 
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


char bnf_arbre::m_valeurnumerique(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="valeurnumerique";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_valeurnumerique_112(lret,&subtree))
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


char bnf_arbre::m_parenthesees_117(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parenthesees";
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


char bnf_arbre::m_parenthesees(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parenthesees";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_parenthesees_117(lret,&subtree))
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


char bnf_arbre::m_expression_118_119(string& lreturn,x_node** tree) {
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
 if (m_chaine(lret,&subtree) || m_valeurnumerique(lret,&subtree) || m_operateurmathematique(lret,&subtree) || m_parenthesees(lret,&subtree) || m_procedure(lret,&subtree) || m_name(lret,&subtree))
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


char bnf_arbre::m_expression_118(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="expression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 subtree=NULL;
 if (m_expression_118_119(lret,&subtree))
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
   if (m_expression_118_119(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_expression(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="expression";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_expression_118(lret,&subtree))
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


char bnf_arbre::m_parametresuivant_120(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parametresuivant";
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
 if (m_parametre(lret,&subtree)) 
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


char bnf_arbre::m_parametresuivant(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parametresuivant";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_parametresuivant_120(lret,&subtree))
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


char bnf_arbre::m_parametre_121(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parametre";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_var(lret,&subtree) || m_vardep(lret,&subtree) || m_expression(lret,&subtree))
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


char bnf_arbre::m_parametre(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parametre";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_parametre_121(lret,&subtree))
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


char bnf_arbre::m_parametres_122_123(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parametres";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYWHILE
 while (currentpos<fx->stack.size()) {
   subtree=NULL;
   if (m_parametresuivant(lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_parametres_122(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parametres";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_parametre(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_parametres_122_123(lret,&subtree)) 
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


char bnf_arbre::m_parametres(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="parametres";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_parametres_122(lret,&subtree))
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


char bnf_arbre::m_procedure_124(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="procedure";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_name(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_parametres(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
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


char bnf_arbre::m_procedure(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="procedure";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_procedure_124(lret,&subtree))
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


char bnf_arbre::m_choix_125(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="choix";
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


char bnf_arbre::m_choix(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="choix";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_choix_125(lret,&subtree))
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


char bnf_arbre::m_testdependency_126_127_128(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testdependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_neg(lret,&subtree) || m_choix(lret,&subtree))
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


char bnf_arbre::m_testdependency_126_127(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testdependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOPTIONAL
 subtree=NULL;
 if (m_testdependency_126_127_128(lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret="";
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_testdependency_126(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testdependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_testdependency_126_127(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency(lret,&subtree)) 
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


char bnf_arbre::m_testdependency(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="testdependency";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_testdependency_126(lret,&subtree))
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


char bnf_arbre::m_namestruct_129(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="namestruct";
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
 if (m_name(lret,&subtree)) 
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


char bnf_arbre::m_namestruct(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="namestruct";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_namestruct_129(lret,&subtree))
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


char bnf_arbre::m_displaystruct_130_131(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="displaystruct";
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
   if (m_namestruct(lret,&subtree))
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


char bnf_arbre::m_displaystruct_130(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="displaystruct";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(lret,7,"display")) 
    x_init_tree(tree,subtree,addsubtree);
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
 if (m_name(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_displaystruct_130_131(lret,&subtree)) 
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


char bnf_arbre::m_displaystruct(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="displaystruct";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_displaystruct_130(lret,&subtree))
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


char bnf_arbre::m_elementif_132_133(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="elementif";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYOR
 subtree=NULL;
 if (m_displaystruct(lret,&subtree) || m_testdependency(lret,&subtree) || m_procedure(lret,&subtree) || m_egauxontologie(lret,&subtree) || m_compatibleontologie(lret,&subtree) || m_affectationontologie(lret,&subtree) || m_egauxlecture(lret,&subtree) || m_affectationlecture(lret,&subtree) || m_compatibletrait(lret,&subtree) || m_egauxtrait(lret,&subtree) || m_affectationtrait(lret,&subtree) || m_egalite(lret,&subtree) || m_inferieur(lret,&subtree) || m_superieur(lret,&subtree) || m_inftraits(lret,&subtree) || m_suptraits(lret,&subtree) || m_infegaltraits(lret,&subtree) || m_supegaltraits(lret,&subtree) || m_testsimple(lret,&subtree))
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


char bnf_arbre::m_elementif_132_134_135(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="elementif";
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


char bnf_arbre::m_elementif_132_134(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="elementif";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYCONTEXTUAL
 long poscontext=currentpos;
	int itokcontext=intoken;
 x_node* subtreebis=NULL;
 string lretbis;
 if (m_elementif_132_134_135(lretbis,&subtreebis)) {
    if (subtreebis!=NULL)
        delete subtreebis;
    currentpos=poscontext;
		intoken=itokcontext;
 }
 else {
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
 lreturn+=lret;
 return(1);
 }


char bnf_arbre::m_elementif_132(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="elementif";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
//BODYSEQUENCE
 subtree=NULL;
 if (m_elementif_132_133(lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    currentpos=pos;
		intoken=itok;
    setfail(exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_elementif_132_134(lret,&subtree)) 
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


char bnf_arbre::m_elementif(string& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 static const char label[]="elementif";
 string lret;
 long pos=currentpos;
	int itok=intoken;
 x_node* subtree=NULL;
 int addsubtree=0;
 bool exitonfail=false;
 if (m_elementif_132(lret,&subtree))
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




string bnf_arbre::x_errormsg(int i) {
	return errorlabels[i];
}


x_node* bnf_arbre::x_parsing(x_reading* xr,x_parsing_mode mode) {
fx=xr;
	string lret;
	x_node* tree=new x_node;
	gFail=0;
	lineerror=-1;
	labelerror="";
	errornumber=-1;
	currentpos=0;
	intoken=0;
	char res=m_generation(lret,&tree);

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


char bnf_arbre::x_test_string(string& lret,int sz,string sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}

char bnf_arbre::m_name(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char label[]="name";
	if (fx->stacktype[currentpos]==4) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_arbre::m_number(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char label[]="number";
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

char bnf_arbre::m_layer(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char label[]="layer";
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

char bnf_arbre::m_valquote(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char label[]="valquote";
	if (fx->stacktype[currentpos]==1) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}
