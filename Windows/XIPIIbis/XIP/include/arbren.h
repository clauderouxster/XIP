#ifndef bnf_arbre_h
#define bnf_arbre_h
#include "x_node.h"


class bnf_arbre {
public:


int currentpos;
int intoken;
int lineerror;
string labelerror;
int errornumber;
char gFail;
int baseline;
x_reading* fx;

bnf_arbre(int l=0) {
    baseline=l;
    fx=NULL;
    intoken=0;
    currentpos=0;
}

void initialize(x_reading* xr) {
    baseline=0;
    fx=xr;
    gFail=0;
    intoken=0;
    currentpos=0;
}

void setfail(char test) {
    if (test)
        gFail=1;
}

void initfail(string lab,long pos,int error) {
        if (gFail==1)
            return;
	if (pos>=fx->stackln.size()) {
		lineerror=-1;
		labelerror=lab;
		errornumber=error;
		return;
	}
    lineerror=fx->stackln[pos]+baseline;
    labelerror=lab;
    errornumber=error;
}

inline void x_pop_node(x_node** tree,int addsubtree) {
	if (*tree!=NULL) {
		if ((*tree)->init==0) {
			delete *tree;
			*tree=NULL;
		}
		else
		 (*tree)->pop(addsubtree);
	}
}

inline void x_init_node(string& lreturn,string& lret,x_node** tree,string name,long d,bool conc) {
    if (lret.size()>0 || conc==false) {
        if (*tree==NULL)
            *tree=new x_node;
        (*tree)->start=d;
        (*tree)->end=d;
        (*tree)->token=name;
        (*tree)->init=1;        
        if (conc) {
            (*tree)->value=lret;
            lreturn+=lret;
        }
    }
}


inline void x_init_tree(x_node** tree,x_node* subtree,int& addsubtree) {
    if (subtree==NULL)
        return;
    if (*tree==NULL)
       *tree=new x_node;
    if ((*tree)->append(subtree)==0)
        delete subtree;
    else
        addsubtree++;
}

char x_test_string(string& lret,int sz,string sub);

void incrementintoken() {
	intoken++;
	if (intoken==fx->stack[currentpos].size()) {
		currentpos++;
		intoken=0;
	}
}

void incrementpos() {
    currentpos++;
    intoken=0;

}


inline char x_test_char(string& lret,unsigned char sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	char x=fx->stack[currentpos][intoken];
	if (x==sub) {
		incrementintoken();
		lret+=x;
		return(1);
	}
  return(0);
 }


inline char x_test_in(string& lret,char* sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	char x=fx->stack[currentpos][intoken];
	if (strchr(sub,x)!=NULL) {
		lret+=x;
		incrementintoken();
		return(1);
	}
	return(0);
}


inline char x_test_ord(string& lret,unsigned char b,unsigned char g) {
	if (currentpos>=fx->stack.size())
		return(0);
	char x=fx->stack[currentpos][intoken];
	if (x>=b && x<=g) {
		incrementintoken();
		lret+=x;
		return(1);
	}
	return(0);
}

inline char x_test_any(string& lret) {
	if (currentpos>=fx->stack.size())
		return(0);
	char x=fx->stack[currentpos][intoken];
	incrementintoken();
	lret+=x;
	return(1);
}


char m_EOF(string& lreturn,x_node** tree) {
	if (currentpos>=fx->stack.size())
		return(1);
	return(0);
}

long x_linenumber(long p) {
	if (p<fx->stackln.size())
		return (fx->stackln[p]+baseline);
	return -1;
}

string x_errormsg(int i);
x_node* x_parsing(x_reading* xr,x_parsing_mode mode,bool display=true);



char m_longestmatch(string& lreturn,x_node** tree);
char m_shortestmatch(string& lreturn,x_node** tree);
char m_arrow(string& lreturn,x_node** tree);
char m_match(string& lreturn,x_node** tree);
char m_var(string& lreturn,x_node** tree);
char m_pos(string& lreturn,x_node** tree);
char m_kleene(string& lreturn,x_node** tree);
char m_featonto(string& lreturn,x_node** tree);
char m_featonto_0_1(string& lreturn,x_node** tree);
char m_feat(string& lreturn,x_node** tree);
char m_feat_0_1(string& lreturn,x_node** tree);
char m_valmin(string& lreturn,x_node** tree);
char m_neg(string& lreturn,x_node** tree);
char m_valnorm(string& lreturn,x_node** tree);
char m_valneg(string& lreturn,x_node** tree);
char m_valneg_0_1(string& lreturn,x_node** tree);
char m_val(string& lreturn,x_node** tree);
char m_featvalbloque(string& lreturn,x_node** tree);
char m_featvalbloque_0_1(string& lreturn,x_node** tree);
char m_featvalfree(string& lreturn,x_node** tree);
char m_featvalfreeinstance(string& lreturn,x_node** tree);
char m_featvaltest(string& lreturn,x_node** tree);
char m_featvalinstance(string& lreturn,x_node** tree);
char m_feature(string& lreturn,x_node** tree);
char m_featurenext(string& lreturn,x_node** tree);
char m_valeurontologie(string& lreturn,x_node** tree);
char m_featontologie(string& lreturn,x_node** tree);
char m_featontologie_0_1(string& lreturn,x_node** tree);
char m_listefeature(string& lreturn,x_node** tree);
char m_listefeature_0_1(string& lreturn,x_node** tree);
char m_features(string& lreturn,x_node** tree);
char m_featuresreading(string& lreturn,x_node** tree);
char m_featuresreading_0_1(string& lreturn,x_node** tree);
char m_nodefeatures(string& lreturn,x_node** tree);
char m_subtree(string& lreturn,x_node** tree);
char m_category(string& lreturn,x_node** tree);
char m_category_0_1(string& lreturn,x_node** tree);
char m_category_0_1_2(string& lreturn,x_node** tree);
char m_category_0_1_2_3(string& lreturn,x_node** tree);
char m_category_0_1_2_4(string& lreturn,x_node** tree);
char m_category_0_6(string& lreturn,x_node** tree);
char m_category_0_7(string& lreturn,x_node** tree);
char m_categorysequence(string& lreturn,x_node** tree);
char m_categorysequence_0_1(string& lreturn,x_node** tree);
char m_categoryexclusive(string& lreturn,x_node** tree);
char m_categorynext(string& lreturn,x_node** tree);
char m_categoryopt(string& lreturn,x_node** tree);
char m_listecategory(string& lreturn,x_node** tree);
char m_listecategory_0_2(string& lreturn,x_node** tree);
char m_pointeur(string& lreturn,x_node** tree);
char m_vardepbase(string& lreturn,x_node** tree);
char m_vardepbase_0_1(string& lreturn,x_node** tree);
char m_vardepnode(string& lreturn,x_node** tree);
char m_vardep(string& lreturn,x_node** tree);
char m_namedep(string& lreturn,x_node** tree);
char m_dependency(string& lreturn,x_node** tree);
char m_dependency_0_1(string& lreturn,x_node** tree);
char m_dependency_0_1_2(string& lreturn,x_node** tree);
char m_dependency_0_1_2_3(string& lreturn,x_node** tree);
char m_dependency_0_1_2_3_4(string& lreturn,x_node** tree);
char m_dependency_0_1_5(string& lreturn,x_node** tree);
char m_subdependency(string& lreturn,x_node** tree);
char m_adependency(string& lreturn,x_node** tree);
char m_adependency_0_1(string& lreturn,x_node** tree);
char m_dependencynext(string& lreturn,x_node** tree);
char m_listedependencies(string& lreturn,x_node** tree);
char m_listedependencies_0_1(string& lreturn,x_node** tree);
char m_generation(string& lreturn,x_node** tree);
char m_etou(string& lreturn,x_node** tree);
char m_varexpression(string& lreturn,x_node** tree);
char m_egauxontologie(string& lreturn,x_node** tree);
char m_egauxontologie_0_1(string& lreturn,x_node** tree);
char m_compatibleontologie(string& lreturn,x_node** tree);
char m_affectationontologie(string& lreturn,x_node** tree);
char m_affectationontologie_0_1(string& lreturn,x_node** tree);
char m_compatiblelecture(string& lreturn,x_node** tree);
char m_egauxlecture(string& lreturn,x_node** tree);
char m_affectationlecture(string& lreturn,x_node** tree);
char m_compatibletrait(string& lreturn,x_node** tree);
char m_egauxtrait(string& lreturn,x_node** tree);
char m_affectationtrait(string& lreturn,x_node** tree);
char m_egalite(string& lreturn,x_node** tree);
char m_inferieur(string& lreturn,x_node** tree);
char m_superieur(string& lreturn,x_node** tree);
char m_inftraits(string& lreturn,x_node** tree);
char m_inftraits_0_1(string& lreturn,x_node** tree);
char m_inftraits_0_1_2(string& lreturn,x_node** tree);
char m_suptraits(string& lreturn,x_node** tree);
char m_infegaltraits(string& lreturn,x_node** tree);
char m_supegaltraits(string& lreturn,x_node** tree);
char m_testsimple(string& lreturn,x_node** tree);
char m_operateurmathematique(string& lreturn,x_node** tree);
char m_chaine(string& lreturn,x_node** tree);
char m_valeurnumerique(string& lreturn,x_node** tree);
char m_valeurnumerique_0_1(string& lreturn,x_node** tree);
char m_valeurnumerique_0_2(string& lreturn,x_node** tree);
char m_valeurnumerique_0_2_3(string& lreturn,x_node** tree);
char m_parenthesees(string& lreturn,x_node** tree);
char m_expression(string& lreturn,x_node** tree);
char m_expression_0_1(string& lreturn,x_node** tree);
char m_parametresuivant(string& lreturn,x_node** tree);
char m_parametre(string& lreturn,x_node** tree);
char m_parametres(string& lreturn,x_node** tree);
char m_parametres_0_1(string& lreturn,x_node** tree);
char m_procedure(string& lreturn,x_node** tree);
char m_choix(string& lreturn,x_node** tree);
char m_testdependency(string& lreturn,x_node** tree);
char m_testdependency_0_1(string& lreturn,x_node** tree);
char m_testdependency_0_1_2(string& lreturn,x_node** tree);
char m_namestruct(string& lreturn,x_node** tree);
char m_displaystruct(string& lreturn,x_node** tree);
char m_displaystruct_0_1(string& lreturn,x_node** tree);
char m_elementif(string& lreturn,x_node** tree);
char m_elementif_0_1(string& lreturn,x_node** tree);
char m_elementif_0_2(string& lreturn,x_node** tree);
char m_elementif_0_2_3(string& lreturn,x_node** tree);
char m_name(string& lreturn,x_node** tree);
char m_number(string& lreturn,x_node** tree);
char m_layer(string& lreturn,x_node** tree);
char m_valquote(string& lreturn,x_node** tree);
};
#endif
