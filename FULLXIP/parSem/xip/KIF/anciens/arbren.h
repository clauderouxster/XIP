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
	incrementintoken();
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
x_node* x_parsing(x_reading* xr,x_parsing_mode mode);



char m_longestmatch(string& lreturn,x_node** tree);
char m_longestmatch_0(string& lreturn,x_node** tree);
char m_shortestmatch(string& lreturn,x_node** tree);
char m_arrow(string& lreturn,x_node** tree);
char m_arrow_1(string& lreturn,x_node** tree);
char m_match(string& lreturn,x_node** tree);
char m_match_2(string& lreturn,x_node** tree);
char m_var(string& lreturn,x_node** tree);
char m_var_3(string& lreturn,x_node** tree);
char m_pos(string& lreturn,x_node** tree);
char m_pos_4(string& lreturn,x_node** tree);
char m_kleene(string& lreturn,x_node** tree);
char m_kleene_5(string& lreturn,x_node** tree);
char m_featonto(string& lreturn,x_node** tree);
char m_featonto_6(string& lreturn,x_node** tree);
char m_featonto_6_7(string& lreturn,x_node** tree);
char m_feat(string& lreturn,x_node** tree);
char m_feat_8(string& lreturn,x_node** tree);
char m_feat_8_9(string& lreturn,x_node** tree);
char m_valmin(string& lreturn,x_node** tree);
char m_valmin_10(string& lreturn,x_node** tree);
char m_neg(string& lreturn,x_node** tree);
char m_valnorm(string& lreturn,x_node** tree);
char m_valnorm_11(string& lreturn,x_node** tree);
char m_valneg(string& lreturn,x_node** tree);
char m_valneg_12(string& lreturn,x_node** tree);
char m_val(string& lreturn,x_node** tree);
char m_val_14(string& lreturn,x_node** tree);
char m_featvalbloque(string& lreturn,x_node** tree);
char m_featvalbloque_15(string& lreturn,x_node** tree);
char m_featvalfree(string& lreturn,x_node** tree);
char m_featvalfree_16(string& lreturn,x_node** tree);
char m_featvalfreeinstance(string& lreturn,x_node** tree);
char m_featvalfreeinstance_17(string& lreturn,x_node** tree);
char m_featvaltest(string& lreturn,x_node** tree);
char m_featvaltest_18(string& lreturn,x_node** tree);
char m_featvalinstance(string& lreturn,x_node** tree);
char m_featvalinstance_19(string& lreturn,x_node** tree);
char m_feature(string& lreturn,x_node** tree);
char m_feature_20(string& lreturn,x_node** tree);
char m_featurenext(string& lreturn,x_node** tree);
char m_featurenext_21(string& lreturn,x_node** tree);
char m_valeurontologie(string& lreturn,x_node** tree);
char m_featontologie(string& lreturn,x_node** tree);
char m_featontologie_23(string& lreturn,x_node** tree);
char m_featontologie_23_24(string& lreturn,x_node** tree);
char m_listefeature(string& lreturn,x_node** tree);
char m_listefeature_25(string& lreturn,x_node** tree);
char m_listefeature_25_26(string& lreturn,x_node** tree);
char m_features(string& lreturn,x_node** tree);
char m_features_27(string& lreturn,x_node** tree);
char m_featuresreading(string& lreturn,x_node** tree);
char m_featuresreading_28(string& lreturn,x_node** tree);
char m_featuresreading_28_29(string& lreturn,x_node** tree);
char m_nodefeatures(string& lreturn,x_node** tree);
char m_subtree(string& lreturn,x_node** tree);
char m_subtree_31(string& lreturn,x_node** tree);
char m_category(string& lreturn,x_node** tree);
char m_category_32(string& lreturn,x_node** tree);
char m_category_32_33(string& lreturn,x_node** tree);
char m_category_32_33_34(string& lreturn,x_node** tree);
char m_category_32_33_34_35(string& lreturn,x_node** tree);
char m_category_32_33_34_36(string& lreturn,x_node** tree);
char m_category_32_38(string& lreturn,x_node** tree);
char m_category_32_39(string& lreturn,x_node** tree);
char m_categorysequence(string& lreturn,x_node** tree);
char m_categorysequence_40(string& lreturn,x_node** tree);
char m_categorysequence_40_41(string& lreturn,x_node** tree);
char m_categoryexclusive(string& lreturn,x_node** tree);
char m_categoryexclusive_42(string& lreturn,x_node** tree);
char m_categorynext(string& lreturn,x_node** tree);
char m_categorynext_44(string& lreturn,x_node** tree);
char m_categoryopt(string& lreturn,x_node** tree);
char m_categoryopt_45(string& lreturn,x_node** tree);
char m_listecategory(string& lreturn,x_node** tree);
char m_listecategory_47(string& lreturn,x_node** tree);
char m_listecategory_47_49(string& lreturn,x_node** tree);
char m_pointeur(string& lreturn,x_node** tree);
char m_vardepbase(string& lreturn,x_node** tree);
char m_vardepbase_50(string& lreturn,x_node** tree);
char m_vardepbase_50_51(string& lreturn,x_node** tree);
char m_vardepnode(string& lreturn,x_node** tree);
char m_vardepnode_52(string& lreturn,x_node** tree);
char m_vardep(string& lreturn,x_node** tree);
char m_vardep_54(string& lreturn,x_node** tree);
char m_namedep(string& lreturn,x_node** tree);
char m_namedep_55(string& lreturn,x_node** tree);
char m_dependency(string& lreturn,x_node** tree);
char m_dependency_56(string& lreturn,x_node** tree);
char m_dependency_56_57(string& lreturn,x_node** tree);
char m_dependency_56_57_58(string& lreturn,x_node** tree);
char m_dependency_56_57_58_59(string& lreturn,x_node** tree);
char m_dependency_56_57_58_59_60(string& lreturn,x_node** tree);
char m_dependency_56_57_61(string& lreturn,x_node** tree);
char m_subdependency(string& lreturn,x_node** tree);
char m_subdependency_62(string& lreturn,x_node** tree);
char m_adependency(string& lreturn,x_node** tree);
char m_adependency_63(string& lreturn,x_node** tree);
char m_adependency_63_64(string& lreturn,x_node** tree);
char m_dependencynext(string& lreturn,x_node** tree);
char m_dependencynext_65(string& lreturn,x_node** tree);
char m_listedependencies(string& lreturn,x_node** tree);
char m_listedependencies_66(string& lreturn,x_node** tree);
char m_listedependencies_66_67(string& lreturn,x_node** tree);
char m_generation(string& lreturn,x_node** tree);
char m_generation_68(string& lreturn,x_node** tree);
char m_etou(string& lreturn,x_node** tree);
char m_etou_69(string& lreturn,x_node** tree);
char m_varexpression(string& lreturn,x_node** tree);
char m_varexpression_70(string& lreturn,x_node** tree);
char m_egauxontologie(string& lreturn,x_node** tree);
char m_egauxontologie_71(string& lreturn,x_node** tree);
char m_egauxontologie_71_72(string& lreturn,x_node** tree);
char m_compatibleontologie(string& lreturn,x_node** tree);
char m_compatibleontologie_73(string& lreturn,x_node** tree);
char m_affectationontologie(string& lreturn,x_node** tree);
char m_affectationontologie_75(string& lreturn,x_node** tree);
char m_affectationontologie_75_76(string& lreturn,x_node** tree);
char m_compatiblelecture(string& lreturn,x_node** tree);
char m_compatiblelecture_77(string& lreturn,x_node** tree);
char m_egauxlecture(string& lreturn,x_node** tree);
char m_egauxlecture_79(string& lreturn,x_node** tree);
char m_affectationlecture(string& lreturn,x_node** tree);
char m_affectationlecture_81(string& lreturn,x_node** tree);
char m_compatibletrait(string& lreturn,x_node** tree);
char m_compatibletrait_83(string& lreturn,x_node** tree);
char m_egauxtrait(string& lreturn,x_node** tree);
char m_egauxtrait_85(string& lreturn,x_node** tree);
char m_affectationtrait(string& lreturn,x_node** tree);
char m_affectationtrait_87(string& lreturn,x_node** tree);
char m_egalite(string& lreturn,x_node** tree);
char m_egalite_89(string& lreturn,x_node** tree);
char m_inferieur(string& lreturn,x_node** tree);
char m_inferieur_91(string& lreturn,x_node** tree);
char m_superieur(string& lreturn,x_node** tree);
char m_superieur_92(string& lreturn,x_node** tree);
char m_inftraits(string& lreturn,x_node** tree);
char m_inftraits_93(string& lreturn,x_node** tree);
char m_inftraits_93_94(string& lreturn,x_node** tree);
char m_inftraits_93_94_95(string& lreturn,x_node** tree);
char m_suptraits(string& lreturn,x_node** tree);
char m_suptraits_97(string& lreturn,x_node** tree);
char m_infegaltraits(string& lreturn,x_node** tree);
char m_infegaltraits_101(string& lreturn,x_node** tree);
char m_supegaltraits(string& lreturn,x_node** tree);
char m_supegaltraits_105(string& lreturn,x_node** tree);
char m_testsimple(string& lreturn,x_node** tree);
char m_testsimple_109(string& lreturn,x_node** tree);
char m_operateurmathematique(string& lreturn,x_node** tree);
char m_operateurmathematique_110(string& lreturn,x_node** tree);
char m_chaine(string& lreturn,x_node** tree);
char m_chaine_111(string& lreturn,x_node** tree);
char m_valeurnumerique(string& lreturn,x_node** tree);
char m_valeurnumerique_112(string& lreturn,x_node** tree);
char m_valeurnumerique_112_113(string& lreturn,x_node** tree);
char m_valeurnumerique_112_114(string& lreturn,x_node** tree);
char m_valeurnumerique_112_114_115(string& lreturn,x_node** tree);
char m_parenthesees(string& lreturn,x_node** tree);
char m_parenthesees_117(string& lreturn,x_node** tree);
char m_expression(string& lreturn,x_node** tree);
char m_expression_118(string& lreturn,x_node** tree);
char m_expression_118_119(string& lreturn,x_node** tree);
char m_parametresuivant(string& lreturn,x_node** tree);
char m_parametresuivant_120(string& lreturn,x_node** tree);
char m_parametre(string& lreturn,x_node** tree);
char m_parametre_121(string& lreturn,x_node** tree);
char m_parametres(string& lreturn,x_node** tree);
char m_parametres_122(string& lreturn,x_node** tree);
char m_parametres_122_123(string& lreturn,x_node** tree);
char m_procedure(string& lreturn,x_node** tree);
char m_procedure_124(string& lreturn,x_node** tree);
char m_choix(string& lreturn,x_node** tree);
char m_choix_125(string& lreturn,x_node** tree);
char m_testdependency(string& lreturn,x_node** tree);
char m_testdependency_126(string& lreturn,x_node** tree);
char m_testdependency_126_127(string& lreturn,x_node** tree);
char m_testdependency_126_127_128(string& lreturn,x_node** tree);
char m_namestruct(string& lreturn,x_node** tree);
char m_namestruct_129(string& lreturn,x_node** tree);
char m_displaystruct(string& lreturn,x_node** tree);
char m_displaystruct_130(string& lreturn,x_node** tree);
char m_displaystruct_130_131(string& lreturn,x_node** tree);
char m_elementif(string& lreturn,x_node** tree);
char m_elementif_132(string& lreturn,x_node** tree);
char m_elementif_132_133(string& lreturn,x_node** tree);
char m_elementif_132_134(string& lreturn,x_node** tree);
char m_elementif_132_134_135(string& lreturn,x_node** tree);
char m_name(string& lreturn,x_node** tree);
char m_number(string& lreturn,x_node** tree);
char m_layer(string& lreturn,x_node** tree);
char m_valquote(string& lreturn,x_node** tree);
};
#endif
