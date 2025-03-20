#include "x_node.h"




static char gFail=0;

static void setfail(x_reading& fx, string lab,long pos,char test) {
    if (test) {
        gFail=1;
        fx.lastposerror=pos;
        fx.labelerror=lab;
    }
}

static string join(vector<string>& v) {
   string s;
   for (int i=0;i<v.size();i++) {
      s+=v[i];
    }
   return s;
}

static void extend(vector<string>& lreturn,vector<string>& lret) {
   for (int i=0;i<lret.size();i++) {
     lreturn.push_back(lret[i]);
    }
}


static void x_pop_node(x_node** tree,int addsubtree) {
	if (*tree!=NULL) {
		if ((*tree)->init==0) {
			delete *tree;
			*tree=NULL;
		}
		else
		 (*tree)->pop(addsubtree);
	}
}


static void x_init_node(vector<string>& lreturn,vector<string>& lret,x_node** tree,string name,long d,long e) {
    if (lret.size()>0) {
        if (*tree==NULL)
            *tree=new x_node;
        (*tree)->start=d;
        (*tree)->end=e;
        (*tree)->token=name;
        (*tree)->init=1;
        (*tree)->value=join(lret);
        extend(lreturn,lret);
    }
}

static void x_init_tree(x_node** tree,x_node* subtree,int& addsubtree) {
    if (subtree==NULL)
        return;
    if (*tree==NULL)
       *tree=new x_node;
    if ((*tree)->append(subtree)==0)
        delete subtree;
    else
        addsubtree++;
}

static char x_test_string(x_reading& fx,vector<string>& lret,int sz,string sub) {
  long pos=fx.tell();
  string lst=fx.read(sz);
  if (lst.size()<sz) {
      fx.seek(pos);
      return(0);
  }
  if (lst==sub) {
      lret.push_back(lst);
      return(1);
  }
  fx.seek(pos);
  return(0);
  }


static char x_test_char(x_reading& fx,vector<string>& lret,unsigned char sub) {
  long pos=fx.tell();
  string lst=fx.read(1);
  if (lst.size()!=1) {
      fx.seek(pos);
      return(0);
  }
  unsigned char x=lst[0];
  if (x==sub) {
      lret.push_back(lst);
      return(1);
  }
  fx.seek(pos);
  return(0);
  }


static char x_test_in(x_reading& fx,vector<string>& lret,char* sub) {
  long pos=fx.tell();
  string lst=fx.read(1);
  if (lst.size()!=1) {
      fx.seek(pos);
      return(0);
  }
  char x=lst[0];
  if (strchr(sub,x)!=NULL) {
      lret.push_back(lst);
      return(1);
  }
  fx.seek(pos);
  return(0);
  }


static char x_test_ord(x_reading& fx,vector<string>& lret,unsigned char b,unsigned char g) {
  long pos=fx.tell();
  string lst=fx.read(1);
  if (lst=="") {
     fx.seek(pos);
     return(0);
  }
  unsigned char x=lst[0];
  if (x>=b && x<=g) {
     lret.push_back(lst);
     return(1);
  }
  fx.seek(pos);
  return(0);
  }

static char x_test_any(x_reading& fx,vector<string>& lret) {
  long pos=fx.tell();
  string lst=fx.read(1);
  if (lst=="") {
     fx.seek(pos);
     return(0);
  }
  lret.push_back(lst);
  return(1);
}

static char y_test_string(string lst,vector<string>& lret,int sz,string sub) {
  if (lst==sub)
      return(1);
  return(0);
  }

static char y_test_char(char x,vector<string>& lret,unsigned char sub) {
  if (x==sub)
      return(1);
  return(0);
}


static char y_test_in(char x,vector<string>& lret,char* sub) {
  if (strchr(sub,x)!=NULL)
      return(1);
  return(0);
  }


static char y_test_ord(unsigned char x,vector<string>& lret,unsigned char b,unsigned char g) {
  if (x>=b && x<=g)
     return(1);  
  return(0);
}

static char y_test_any(unsigned char x,vector<string>& lret) {
  return 1;
}




static char tab0[]={'é','à','è','ç','ê','î','ô','â','û','ù','Ç','À','É','È','Ë','Ê','Ï','Î','Û','Ü','ä','ë','ï','ö','ü','_',0};
static char tab1[]={10,13,0};
static char tab2[]={10,13,32,9,0};
static char tab3[]={'*','+',0};
static char tab4[]={'-','_',0};
static char tab5[]={'+','-','*','#','?',0};
static char tab6[]={'|','&',0};
static char tab7[]={'+','-','*','/',0};
static char tab8[]={'#','^',0};
static char tab9[]={'|','&',')','.',0};
char m_longestmatch(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_longestmatch_0(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_shortestmatch(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_arrow(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_arrow_1(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_match(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_match_2(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_name(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_name_3(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_name_3_4(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_number(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_number_5(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_var(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_var_6(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_eol(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_eol_7(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_espace(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_espace_8(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_espace_8_9(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_pos(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_pos_10(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_pos_10_11(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_pos_10_11_12(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_layer(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_layer_13(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_kleene(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_kleene_14(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_basename(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_basename_15(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_basename_15_16(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_basename_15_17(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_basename_15_17_18(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featonto(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featonto_19(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featonto_19_20(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featonto_19_20_21(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featonto_19_20_21_22(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_feat(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_feat_23(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_feat_23_24(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valmin(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valmin_25(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valmin_25_26(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valmin_25_26_27(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valquote(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valquote_28(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valquote_28_29(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_neg(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valnorm(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valnorm_30(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valneg(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valneg_31(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valneg_31_32(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_val(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_val_33(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featvalbloque(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featvalbloque_34(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featvalfree(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featvalfree_35(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featvalfreeinstance(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featvalfreeinstance_36(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featvaltest(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featvaltest_37(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featvalinstance(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featvalinstance_38(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_feature(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_feature_39(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featurenext(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featurenext_40(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featurenext_40_41(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valeurontologie(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valeurontologie_43(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featontologie(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featontologie_44(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featontologie_44_45(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listefeature(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listefeature_46(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listefeature_46_47(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_features(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_features_48(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featuresreading(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featuresreading_49(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_featuresreading_49_50(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_subtree(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_subtree_51(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_subtree_51_52(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_category(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_category_55(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_category_55_56(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_category_55_56_57(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_category_55_56_57_58(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_category_55_56_57_59(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_category_55_61(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_category_55_62(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_category_55_63(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categorysequence(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categorysequence_64(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categorysequence_64_65(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categorysequence_64_66(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categoryexclusive(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categoryexclusive_67(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categoryexclusive_67_68(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categoryexclusive_67_69(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categorynext(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categorynext_70(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categoryopt(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categoryopt_71(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_categoryopt_71_72(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listecategory(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listecategory_73(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listecategory_73_74(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listecategory_73_75(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listecategory_73_76(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_pointeur(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_vardepbase(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_vardepbase_77(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_vardepbase_77_78(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_vardepnode(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_vardepnode_79(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_vardepnode_79_80(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_vardep(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_vardep_81(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_namedep(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_namedep_82(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_dependency(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_dependency_83(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_dependency_83_84(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_dependency_83_84_85(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_dependency_83_84_85_86(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_dependency_83_84_85_86_87(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_dependency_83_84_88(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_subdependency(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_subdependency_89(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_subdependency_89_90(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_adependency(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_adependency_92(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_adependency_92_93(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_adependency_92_94(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_dependencynext(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_dependencynext_95(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_dependencynext_95_96(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listedependencies(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listedependencies_98(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_listedependencies_98_99(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_generation(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_generation_100(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_generation_100_101(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_etou(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_etou_102(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_varexpression(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_varexpression_103(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_varexpression_103_104(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxontologie(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxontologie_106(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxontologie_106_107(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxontologie_106_108(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatibleontologie(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatibleontologie_110(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatibleontologie_110_111(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatibleontologie_110_112(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationontologie(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationontologie_114(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationontologie_114_115(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationontologie_114_118(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatiblelecture(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatiblelecture_120(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatiblelecture_120_121(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatiblelecture_120_122(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxlecture(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxlecture_124(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxlecture_124_125(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxlecture_124_126(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationlecture(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationlecture_128(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationlecture_128_129(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationlecture_128_132(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatibletrait(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatibletrait_134(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatibletrait_134_135(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_compatibletrait_134_136(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxtrait(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxtrait_138(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxtrait_138_139(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egauxtrait_138_140(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationtrait(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationtrait_142(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationtrait_142_143(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_affectationtrait_142_146(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egalite(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egalite_148(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egalite_148_149(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_egalite_148_150(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_inferieur(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_inferieur_152(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_inferieur_152_153(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_superieur(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_superieur_155(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_superieur_155_156(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_inftraits(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_inftraits_158(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_inftraits_158_159(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_inftraits_158_161(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_inftraits_158_161_162(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_inftraits_158_161_162_163(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_suptraits(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_suptraits_164(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_suptraits_164_165(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_suptraits_164_167(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_suptraits_164_167_168(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_suptraits_164_167_168_169(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_infegaltraits(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_infegaltraits_170(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_infegaltraits_170_171(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_infegaltraits_170_173(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_infegaltraits_170_173_174(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_infegaltraits_170_173_174_175(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_supegaltraits(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_supegaltraits_176(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_supegaltraits_176_177(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_supegaltraits_176_179(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_supegaltraits_176_179_180(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_supegaltraits_176_179_180_181(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_testsimple(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_testsimple_182(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_operateurmathematique(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_operateurmathematique_183(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_chaine(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_chaine_184(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valeurnumerique(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valeurnumerique_185(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valeurnumerique_185_186(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valeurnumerique_185_187(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_valeurnumerique_185_187_188(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_parenthesees(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_parenthesees_190(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_expression(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_expression_191(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_expression_191_192(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_parametresuivant(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_parametresuivant_193(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_parametre(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_parametre_194(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_parametres(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_parametres_195(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_parametres_195_196(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_procedure(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_procedure_197(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_choix(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_choix_198(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_testdependency(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_testdependency_199(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_testdependency_199_200(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_testdependency_199_200_201(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_namestruct(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_namestruct_202(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_namestruct_202_203(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_displaystruct(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_displaystruct_205(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_displaystruct_205_206(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_displaystruct_205_207(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_elementif(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_elementif_209(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_elementif_209_210(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_elementif_209_211(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_elementif_209_213(x_reading& fx,vector<string>& lreturn,x_node** tree);
char m_elementif_209_213_214(x_reading& fx,vector<string>& lreturn,x_node** tree);

char m_longestmatch_0(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="longestmatch";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'@')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_longestmatch(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="longestmatch";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_longestmatch_0(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_shortestmatch(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="shortestmatch";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (x_test_char(fx,lret,'='))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_arrow_1(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="arrow";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'-')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_arrow(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="arrow";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_arrow_1(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_match_2(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="match";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 long max=0;
 vector<x_couple*> results;
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (m_longestmatch(fx,lret,&subtree)) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (m_shortestmatch(fx,lret,&subtree)) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (m_arrow(fx,lret,&subtree)) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 if (results.size()==0) {
     fx.seek(pos);
     setfail(fx,label,pos,exitonfail);
     return 0;
  }
 char add=0;
 for (int i=0;i<results.size();i++) {
    if (add==0 && max==results[i]->pos) {
        add=1;
        fx.seek(results[i]->pos);
        results[i]->set(lret);
        x_init_tree(tree,results[i]->tree,addsubtree);
     }
     else 
       delete results[i]->tree;
     delete results[i];
  }
 extend(lreturn,lret);
 return(1);
 }


char m_match(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="match";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_match_2(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_name_3_4(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="name";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_ord(fx,lret,'A','Z') || x_test_ord(fx,lret,'a','z') || x_test_in(fx,lret,tab0))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_name_3(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="name";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (m_name_3_4(fx,lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (m_name_3_4(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_name(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="name";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_name_3(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_number_5(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="number";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (x_test_ord(fx,lret,'0','9'))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (x_test_ord(fx,lret,'0','9'))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_number(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="number";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_number_5(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_var_6(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="var";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'#')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_number(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_var(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="var";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_var_6(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_eol_7(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="eol";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_in(fx,lret,tab1))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_eol(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="eol";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_eol_7(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_pop_node(tree,addsubtree);
 return(1);
}


char m_espace_8_9(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="espace";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_in(fx,lret,tab2))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_espace_8(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="espace";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (m_espace_8_9(fx,lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (m_espace_8_9(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_espace(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="espace";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_espace_8(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_pop_node(tree,addsubtree);
 return(1);
}


char m_pos_10_11_12(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="pos";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_ord(fx,lret,'A','Z') || x_test_ord(fx,lret,'a','z'))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_pos_10_11(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="pos";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (m_pos_10_11_12(fx,lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (m_pos_10_11_12(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_pos_10(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="pos";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 long max=0;
 vector<x_couple*> results;
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (m_pos_10_11(fx,lret,&subtree)) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (x_test_char(fx,lret,'?')) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 if (results.size()==0) {
     fx.seek(pos);
     setfail(fx,label,pos,exitonfail);
     return 0;
  }
 char add=0;
 for (int i=0;i<results.size();i++) {
    if (add==0 && max==results[i]->pos) {
        add=1;
        fx.seek(results[i]->pos);
        results[i]->set(lret);
        x_init_tree(tree,results[i]->tree,addsubtree);
     }
     else 
       delete results[i]->tree;
     delete results[i];
  }
 extend(lreturn,lret);
 return(1);
 }


char m_pos(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="pos";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_pos_10(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_layer_13(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="layer";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (x_test_ord(fx,lret,'0','9'))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (x_test_ord(fx,lret,'0','9'))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_layer(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="layer";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_layer_13(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_kleene_14(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="kleene";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_in(fx,lret,tab3))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_kleene(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="kleene";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_kleene_14(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_basename_15_16(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="basename";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_ord(fx,lret,'A','Z') || x_test_ord(fx,lret,'a','z'))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_basename_15_17_18(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="basename";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_ord(fx,lret,'A','Z') || x_test_ord(fx,lret,'a','z') || x_test_ord(fx,lret,'0','9') || x_test_in(fx,lret,tab4))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_basename_15_17(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="basename";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (m_basename_15_17_18(fx,lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (m_basename_15_17_18(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_basename_15(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="basename";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_basename_15_16(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_basename_15_17(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_basename(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="basename";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_basename_15(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_featonto_19_20_21_22(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featonto";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_ord(fx,lret,'A','Z') || x_test_ord(fx,lret,'a','z'))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featonto_19_20_21(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featonto";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (m_featonto_19_20_21_22(fx,lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (m_featonto_19_20_21_22(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featonto_19_20(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featonto";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'@')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featonto_19_20_21(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featonto_19(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featonto";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (m_featonto_19_20(fx,lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (m_featonto_19_20(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featonto(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featonto";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_featonto_19(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_feat_23_24(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="feat";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'$')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(fx,lret,5,"stack")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_feat_23(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="feat";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 long max=0;
 vector<x_couple*> results;
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (m_featonto(fx,lret,&subtree)) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (m_basename(fx,lret,&subtree)) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (m_feat_23_24(fx,lret,&subtree)) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 if (results.size()==0) {
     fx.seek(pos);
     setfail(fx,label,pos,exitonfail);
     return 0;
  }
 char add=0;
 for (int i=0;i<results.size();i++) {
    if (add==0 && max==results[i]->pos) {
        add=1;
        fx.seek(results[i]->pos);
        results[i]->set(lret);
        x_init_tree(tree,results[i]->tree,addsubtree);
     }
     else 
       delete results[i]->tree;
     delete results[i];
  }
 extend(lreturn,lret);
 return(1);
 }


char m_feat(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="feat";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_feat_23(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_valmin_25_26_27(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valmin";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_ord(fx,lret,'A','Z') || x_test_ord(fx,lret,'a','z') || x_test_ord(fx,lret,'0','9'))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valmin_25_26(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valmin";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (m_valmin_25_26_27(fx,lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (m_valmin_25_26_27(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valmin_25(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valmin";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 long max=0;
 vector<x_couple*> results;
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (m_valmin_25_26(fx,lret,&subtree)) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (x_test_in(fx,lret,tab5)) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 lret.clear();
 subtree=NULL;
 fx.seek(pos);
 if (m_neg(fx,lret,&subtree)) {
   results.push_back(new x_couple(fx.tell(),subtree,lret));
    if (max<fx.tell()) {
        max=fx.tell();
    }
 }
 if (results.size()==0) {
     fx.seek(pos);
     setfail(fx,label,pos,exitonfail);
     return 0;
  }
 char add=0;
 for (int i=0;i<results.size();i++) {
    if (add==0 && max==results[i]->pos) {
        add=1;
        fx.seek(results[i]->pos);
        results[i]->set(lret);
        x_init_tree(tree,results[i]->tree,addsubtree);
     }
     else 
       delete results[i]->tree;
     delete results[i];
  }
 extend(lreturn,lret);
 return(1);
 }


char m_valmin(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valmin";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_valmin_25(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_valquote_28_29(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valquote";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//CONSTRAINT
 subtree=NULL;
 if (x_test_char(fx,lret,34)) {
     x_pop_node(tree,addsubtree);
     fx.seek(pos);
     setfail(fx,label,pos,exitonfail);
     return(0);
 }
 subtree=NULL;
 if (x_test_any(fx,lret))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 int foundsolution=0;
 while (!fx.eof()) {
   subtree=NULL;
   if (x_test_char(fx,lret,34)) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (x_test_any(fx,lret))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valquote_28(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valquote";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,34)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valquote_28_29(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valquote(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valquote";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_valquote_28(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_neg(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="neg";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (x_test_char(fx,lret,'~'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_valnorm_30(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valnorm";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_valquote(fx,lret,&subtree) || m_valmin(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valnorm(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valnorm";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_valnorm_30(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_valneg_31_32(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valneg";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_valquote(fx,lret,&subtree) || m_valmin(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valneg_31(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valneg";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_neg(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valneg_31_32(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valneg(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valneg";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_valneg_31(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_val_33(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="val";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_valneg(fx,lret,&subtree) || m_valnorm(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_val(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="val";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_val_33(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_featvalbloque_34(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featvalbloque";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_feat(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'!')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featvalbloque(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featvalbloque";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_featvalbloque_34(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_featvalfree_35(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featvalfree";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'!')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_feat(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'!')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featvalfree(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featvalfree";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_featvalfree_35(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_featvalfreeinstance_36(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featvalfreeinstance";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'!')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_feat(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_val(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featvalfreeinstance(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featvalfreeinstance";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_featvalfreeinstance_36(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_featvaltest_37(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featvaltest";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_feat(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_val(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featvaltest(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featvaltest";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_featvaltest_37(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_featvalinstance_38(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featvalinstance";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_feat(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_val(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featvalinstance(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featvalinstance";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_featvalinstance_38(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_feature_39(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="feature";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_featvalfree(fx,lret,&subtree) || m_featvalbloque(fx,lret,&subtree) || m_featvalfreeinstance(fx,lret,&subtree) || m_featvaltest(fx,lret,&subtree) || m_featvalinstance(fx,lret,&subtree) || m_feat(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_feature(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="feature";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_feature_39(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_featurenext_40_41(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featurenext";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_featurenext_40(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featurenext";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_featurenext_40_41(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featurenext_40_41(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_feature(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featurenext(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featurenext";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_featurenext_40(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_valeurontologie_43(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valeurontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'@')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_name(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valeurontologie(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valeurontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_valeurontologie_43(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_featontologie_44_45(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//CONSTRAINT
 int foundsolution=0;
 while (!fx.eof()) {
   subtree=NULL;
   if (x_test_char(fx,lret,']')) {
      foundsolution=1;
      x_init_tree(tree,subtree,addsubtree);
      break;
   }
   subtree=NULL;
   if (m_valeurontologie(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
   }
 if (foundsolution==0) {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featontologie_44(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valeurontologie(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie_44_45(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featontologie(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_featontologie_44(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_listefeature_46_47(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listefeature";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 while (!fx.eof()) {
   subtree=NULL;
   if (m_featurenext(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_listefeature_46(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listefeature";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_feature(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listefeature_46_47(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_listefeature(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listefeature";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_listefeature_46(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_features_48(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="features";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'[')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listefeature(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,']')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_features(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="features";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_features_48(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_featuresreading_49_50(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featuresreading";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (x_test_char(fx,lret,'*'))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_featuresreading_49(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featuresreading";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading_49_50(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listefeature(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_featuresreading(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="featuresreading";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_featuresreading_49(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_subtree_51_52(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="subtree";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_subtree_51(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="subtree";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_subtree_51_52(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listecategory(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_subtree_51_52(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_subtree_51_52(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_subtree(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="subtree";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_subtree_51(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_category_55_56_57_58(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="category";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_kleene(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_category_55_56_57_59(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="category";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_var(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_category_55_56_57(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="category";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_pos(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_55_56_57_58(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_55_56_57_59(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_category_55_56(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="category";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_var(fx,lret,&subtree) || m_category_55_56_57(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_category_55_61(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="category";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_features(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_category_55_62(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="category";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_category_55_63(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="category";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_subtree(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_category_55(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="category";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_55_56(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_55_56_57_58(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_55_61(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_55_62(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_category_55_63(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_category(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="category";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_category_55(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_categorysequence_64_65(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categorysequence";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_categorysequence_64_66(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categorysequence";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_categoryopt(fx,lret,&subtree) || m_category(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_categorysequence_64(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categorysequence";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_categorysequence_64_65(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_categorysequence_64_66(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_categorysequence(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categorysequence";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_categorysequence_64(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_categoryexclusive_67_68(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categoryexclusive";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_categoryexclusive_67_69(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categoryexclusive";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_categoryopt(fx,lret,&subtree) || m_category(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_categoryexclusive_67(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categoryexclusive";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,';')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_categoryexclusive_67_68(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_categoryexclusive_67_69(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_categoryexclusive(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categoryexclusive";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_categoryexclusive_67(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_categorynext_70(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categorynext";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_categorysequence(fx,lret,&subtree) || m_categoryexclusive(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_categorynext(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categorynext";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_categorynext_70(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_categoryopt_71_72(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categoryopt";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_kleene(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_categoryopt_71(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categoryopt";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listecategory(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_categoryopt_71_72(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_categoryopt(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="categoryopt";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_categoryopt_71(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_listecategory_73_74(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listecategory";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_categoryopt(fx,lret,&subtree) || m_category(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_listecategory_73_75(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listecategory";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_listecategory_73_76(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listecategory";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 while (!fx.eof()) {
   subtree=NULL;
   if (m_categorynext(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_listecategory_73(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listecategory";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_listecategory_73_74(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listecategory_73_75(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listecategory_73_76(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_listecategory(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listecategory";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_listecategory_73(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_pointeur(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="pointeur";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (x_test_char(fx,lret,'$'))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_vardepbase_77_78(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="vardepbase";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_pointeur(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_vardepbase_77(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="vardepbase";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'$')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_vardepbase_77_78(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_number(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_vardepbase(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="vardepbase";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_vardepbase_77(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_vardepnode_79_80(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="vardepnode";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_pointeur(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_vardepnode_79(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="vardepnode";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'$')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_vardepnode_79_80(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'#')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_number(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_vardepnode(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="vardepnode";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_vardepnode_79(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_vardep_81(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="vardep";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_vardepbase(fx,lret,&subtree) || m_vardepnode(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_vardep(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="vardep";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_vardep_81(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_namedep_82(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="namedep";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_char(fx,lret,'?') || m_name(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_namedep(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="namedep";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_namedep_82(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_dependency_83_84_85_86_87(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_vardep(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_dependency_83_84_85_86(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_namedep(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency_83_84_85_86_87(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_dependency_83_84_85(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_dependency_83_84_85_86(fx,lret,&subtree) || m_vardep(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_dependency_83_84_88(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_features(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_dependency_83_84(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency_83_84_85(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency_83_84_88(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listecategory(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_dependency_83(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_dependency_83_84(fx,lret,&subtree) || m_vardep(fx,lret,&subtree) || x_test_char(fx,lret,'?'))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_dependency(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_dependency_83(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_subdependency_89_90(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="subdependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_subdependency_89(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="subdependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_subdependency_89_90(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listedependencies(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_subdependency_89_90(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_subdependency(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="subdependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_subdependency_89(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_adependency_92_93(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_adependency_92_94(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_subdependency(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_adependency_92(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_adependency_92_93(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_adependency_92_94(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_adependency(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="adependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_adependency_92(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_dependencynext_95_96(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dependencynext";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_dependencynext_95(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dependencynext";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependencynext_95_96(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependencynext_95_96(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_adependency(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_dependencynext(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="dependencynext";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_dependencynext_95(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_listedependencies_98_99(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listedependencies";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 while (!fx.eof()) {
   subtree=NULL;
   if (m_dependencynext(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_listedependencies_98(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listedependencies";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_adependency(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_listedependencies_98_99(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_listedependencies(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="listedependencies";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_listedependencies_98(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_generation_100_101(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="generation";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_generation_100(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="generation";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_listedependencies(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_generation_100_101(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_generation(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="generation";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_generation_100(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_etou_102(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="etou";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_in(fx,lret,tab6))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_etou(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="etou";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_etou_102(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_varexpression_103_104(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="varexpression";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_varexpression_103(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="varexpression";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_varexpression_103_104(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_etou(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_varexpression_103_104(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_varexpression(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="varexpression";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_varexpression_103(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_egauxontologie_106_107(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_egauxontologie_106_108(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_neg(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_egauxontologie_106(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxontologie_106_107(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxontologie_106_108(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxontologie_106_107(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_egauxontologie(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_egauxontologie_106(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_compatibleontologie_110_111(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatibleontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_compatibleontologie_110_112(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatibleontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_neg(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_compatibleontologie_110(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatibleontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_compatibleontologie_110_111(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_compatibleontologie_110_112(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_compatibleontologie_110_111(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_compatibleontologie(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatibleontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_compatibleontologie_110(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_affectationontologie_114_115(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_affectationontologie_114_118(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 while (!fx.eof()) {
   subtree=NULL;
   if (m_varexpression(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_affectationontologie_114(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featontologie(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationontologie_114_115(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationontologie_114_115(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationontologie_114_115(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationontologie_114_118(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationontologie_114_115(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_affectationontologie(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationontologie";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_affectationontologie_114(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_compatiblelecture_120_121(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatiblelecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_compatiblelecture_120_122(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatiblelecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_neg(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_compatiblelecture_120(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatiblelecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_compatiblelecture_120_121(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_compatiblelecture_120_122(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_compatiblelecture_120_121(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_compatiblelecture(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatiblelecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_compatiblelecture_120(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_egauxlecture_124_125(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxlecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_egauxlecture_124_126(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxlecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_neg(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_egauxlecture_124(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxlecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxlecture_124_125(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxlecture_124_126(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxlecture_124_125(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_egauxlecture(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxlecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_egauxlecture_124(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_affectationlecture_128_129(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationlecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_affectationlecture_128_132(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationlecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 while (!fx.eof()) {
   subtree=NULL;
   if (m_varexpression(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_affectationlecture_128(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationlecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_featuresreading(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationlecture_128_129(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationlecture_128_129(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationlecture_128_129(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationlecture_128_132(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationlecture_128_129(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_affectationlecture(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationlecture";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_affectationlecture_128(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_compatibletrait_134_135(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatibletrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_compatibletrait_134_136(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatibletrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_neg(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_compatibletrait_134(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatibletrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_compatibletrait_134_135(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_compatibletrait_134_136(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_compatibletrait_134_135(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_compatibletrait(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="compatibletrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_compatibletrait_134(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_egauxtrait_138_139(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxtrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_egauxtrait_138_140(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxtrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_neg(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_egauxtrait_138(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxtrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxtrait_138_139(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxtrait_138_140(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egauxtrait_138_139(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_egauxtrait(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egauxtrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_egauxtrait_138(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_affectationtrait_142_143(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationtrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_affectationtrait_142_146(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationtrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 while (!fx.eof()) {
   subtree=NULL;
   if (m_varexpression(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_affectationtrait_142(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationtrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationtrait_142_143(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationtrait_142_143(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'{')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationtrait_142_143(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationtrait_142_146(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_affectationtrait_142_143(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'}')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_affectationtrait(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="affectationtrait";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_affectationtrait_142(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_egalite_148_149(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egalite";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_egalite_148_150(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egalite";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_neg(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_egalite_148(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egalite";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egalite_148_149(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egalite_148_150(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,':')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_egalite_148_149(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_egalite(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="egalite";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_egalite_148(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_inferieur_152_153(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="inferieur";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_inferieur_152(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="inferieur";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_inferieur_152_153(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_inferieur_152_153(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_inferieur(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="inferieur";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_inferieur_152(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_superieur_155_156(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="superieur";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_superieur_155(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="superieur";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_superieur_155_156(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_superieur_155_156(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_superieur(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="superieur";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_superieur_155(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_inftraits_158_159(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="inftraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_inftraits_158_161_162_163(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="inftraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_features(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_inftraits_158_161_162(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="inftraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_inftraits_158_161_162_163(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_inftraits_158_161(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="inftraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_number(fx,lret,&subtree) || m_inftraits_158_161_162(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_inftraits_158(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="inftraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_inftraits_158_159(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_inftraits_158_159(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_inftraits_158_161(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_inftraits(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="inftraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_inftraits_158(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_suptraits_164_165(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="suptraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_suptraits_164_167_168_169(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="suptraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_features(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_suptraits_164_167_168(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="suptraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_suptraits_164_167_168_169(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_suptraits_164_167(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="suptraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_number(fx,lret,&subtree) || m_suptraits_164_167_168(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_suptraits_164(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="suptraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_suptraits_164_165(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_suptraits_164_165(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_suptraits_164_167(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_suptraits(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="suptraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_suptraits_164(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_infegaltraits_170_171(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="infegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_infegaltraits_170_173_174_175(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="infegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_features(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_infegaltraits_170_173_174(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="infegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_infegaltraits_170_173_174_175(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_infegaltraits_170_173(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="infegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_number(fx,lret,&subtree) || m_infegaltraits_170_173_174(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_infegaltraits_170(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="infegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_infegaltraits_170_171(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'<')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_infegaltraits_170_171(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_infegaltraits_170_173(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_infegaltraits(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="infegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_infegaltraits_170(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_supegaltraits_176_177(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="supegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_supegaltraits_176_179_180_181(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="supegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_features(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_supegaltraits_176_179_180(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="supegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_supegaltraits_176_179_180_181(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_supegaltraits_176_179(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="supegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_number(fx,lret,&subtree) || m_supegaltraits_176_179_180(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_supegaltraits_176(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="supegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_supegaltraits_176_177(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'>')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'=')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_supegaltraits_176_177(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_supegaltraits_176_179(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_supegaltraits(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="supegaltraits";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_supegaltraits_176(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_testsimple_182(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testsimple";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_var(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_features(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_testsimple(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testsimple";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_testsimple_182(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_operateurmathematique_183(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operateurmathematique";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_in(fx,lret,tab7))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_operateurmathematique(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="operateurmathematique";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_operateurmathematique_183(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_chaine_184(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="chaine";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,34)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_name(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,34)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_chaine(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="chaine";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_chaine_184(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_valeurnumerique_185_186(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valeurnumerique";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (x_test_ord(fx,lret,'0','9'))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (x_test_ord(fx,lret,'0','9'))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valeurnumerique_185_187_188(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valeurnumerique";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'.')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valeurnumerique_185_186(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valeurnumerique_185_187(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valeurnumerique";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_valeurnumerique_185_187_188(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_valeurnumerique_185(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valeurnumerique";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_valeurnumerique_185_186(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_valeurnumerique_185_187(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_valeurnumerique(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="valeurnumerique";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_valeurnumerique_185(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_parenthesees_190(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parenthesees";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_expression(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_parenthesees(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parenthesees";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_parenthesees_190(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_expression_191_192(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expression";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_chaine(fx,lret,&subtree) || m_valeurnumerique(fx,lret,&subtree) || m_operateurmathematique(fx,lret,&subtree) || m_parenthesees(fx,lret,&subtree) || m_procedure(fx,lret,&subtree) || m_name(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_expression_191(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expression";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 subtree=NULL;
 if (m_expression_191_192(fx,lret,&subtree))
     x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 while (!fx.eof()) {
   subtree=NULL;
   if (m_expression_191_192(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_expression(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="expression";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_expression_191(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_parametresuivant_193(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parametresuivant";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_parametre(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_parametresuivant(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parametresuivant";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_parametresuivant_193(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_parametre_194(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parametre";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_var(fx,lret,&subtree) || m_vardep(fx,lret,&subtree) || m_expression(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_parametre(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parametre";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_parametre_194(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_parametres_195_196(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parametres";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 while (!fx.eof()) {
   subtree=NULL;
   if (m_parametresuivant(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_parametres_195(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parametres";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_parametre(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_parametres_195_196(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_parametres(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="parametres";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_parametres_195(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_procedure_197(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="procedure";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_name(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_parametres(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_procedure(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="procedure";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_procedure_197(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_choix_198(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="choix";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_in(fx,lret,tab8))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_choix(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="choix";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_choix_198(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_testdependency_199_200_201(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testdependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_neg(fx,lret,&subtree) || m_choix(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_testdependency_199_200(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testdependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_testdependency_199_200_201(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_testdependency_199(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testdependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_testdependency_199_200(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_dependency(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_testdependency(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="testdependency";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_testdependency_199(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_namestruct_202_203(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="namestruct";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_namestruct_202(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="namestruct";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_namestruct_202_203(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,',')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_namestruct_202_203(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_name(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_namestruct(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="namestruct";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_namestruct_202(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_displaystruct_205_206(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="displaystruct";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_displaystruct_205_207(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="displaystruct";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYWHILE
 while (!fx.eof()) {
   subtree=NULL;
   if (m_namestruct(fx,lret,&subtree))
         x_init_tree(tree,subtree,addsubtree);
   else
      break;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_displaystruct_205(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="displaystruct";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_string(fx,lret,7,"display")) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,'(')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_displaystruct_205_206(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_name(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_displaystruct_205_207(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_displaystruct_205_206(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (x_test_char(fx,lret,')')) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_displaystruct(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="displaystruct";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_displaystruct_205(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}


char m_elementif_209_210(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="elementif";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOPTIONAL
 subtree=NULL;
 if (m_espace(fx,lret,&subtree))
    x_init_tree(tree,subtree,addsubtree);
 else
   lret.clear();
 extend(lreturn,lret);
 return(1);
 }


char m_elementif_209_211(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="elementif";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (m_displaystruct(fx,lret,&subtree) || m_testdependency(fx,lret,&subtree) || m_procedure(fx,lret,&subtree) || m_egauxontologie(fx,lret,&subtree) || m_compatibleontologie(fx,lret,&subtree) || m_affectationontologie(fx,lret,&subtree) || m_egauxlecture(fx,lret,&subtree) || m_affectationlecture(fx,lret,&subtree) || m_compatibletrait(fx,lret,&subtree) || m_egauxtrait(fx,lret,&subtree) || m_affectationtrait(fx,lret,&subtree) || m_egalite(fx,lret,&subtree) || m_inferieur(fx,lret,&subtree) || m_superieur(fx,lret,&subtree) || m_inftraits(fx,lret,&subtree) || m_suptraits(fx,lret,&subtree) || m_infegaltraits(fx,lret,&subtree) || m_supegaltraits(fx,lret,&subtree) || m_testsimple(fx,lret,&subtree))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_elementif_209_213_214(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="elementif";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYOR
 subtree=NULL;
 if (x_test_in(fx,lret,tab9))
  x_init_tree(tree,subtree,addsubtree);
 else {
   x_pop_node(tree,addsubtree);
   fx.seek(pos);
   setfail(fx,label,pos,exitonfail);
   return 0;
 }
 extend(lreturn,lret);
 return(1);
 }


char m_elementif_209_213(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="elementif";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYCONTEXTUAL
 long poscontext=fx.tell();
 x_node* subtreebis=NULL;
 vector<string> lretbis;
 if (m_elementif_209_213_214(fx,lretbis,&subtreebis)) {
    if (subtreebis!=NULL)
        delete subtreebis;
    fx.seek(poscontext);
 }
 else {
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_elementif_209(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="elementif";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
//BODYSEQUENCE
 subtree=NULL;
 if (m_elementif_209_210(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_elementif_209_211(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_elementif_209_210(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
//BODYSEQUENCE
 subtree=NULL;
 if (m_elementif_209_213(fx,lret,&subtree)) 
    x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 extend(lreturn,lret);
 return(1);
 }


char m_elementif(x_reading& fx,vector<string>& lreturn,x_node** tree) {
 if (gFail==1) return(0);
 const char label[]="elementif";
 vector<string> lret;
 long pos=fx.tell();
 x_node* subtree=NULL;
 int addsubtree=0;
 char exitonfail=0;
 if (m_elementif_209(fx,lret,&subtree))
       x_init_tree(tree,subtree,addsubtree);
 else {
    x_pop_node(tree,addsubtree);
    fx.seek(pos);
    setfail(fx,label,pos,exitonfail);
    return(0);
 }
 x_init_node(lreturn,lret,tree,label,pos,fx.tell());
 return(1);
}



void x_displaytree(x_node* tree,int depth) {
    if (tree==NULL)
        return;
    int i;
    for (i=0;i<depth;i++)
        cout<<" ";
    cout<<tree->token<<"("<<tree->start<<","<<tree->end<<"):"<<tree->value<<endl;
    for (i=0;i<tree->nodes.size();i++) 
        x_displaytree(tree->nodes[i],depth+1);
}

void extractmax(x_node* tree,long& themax) {
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


void extracttext(x_reading* fx,long themax) {
	fx->seek(themax);
	string s=fx->read(255);
	cout<<(char*)s.c_str()<<endl;
}

x_node* x_parsing(x_reading* fx,x_parsing_mode mode) {
    if (fx->error()) {
            delete fx;
            return NULL;
    }
   vector<string> lret;
    x_node* tree=new x_node;
    gFail=0;
    fx->lastposerror=0;
    fx->labelerror="";
    char res=m_generation(*fx,lret,&tree);
    long themax=fx->lastposerror;
    if (mode==FULL && !fx->eof()) {
       res=0;
       if (themax==0)
           extractmax(tree,themax);
       cerr<<"Error:"<<themax<<" on "<<(char*)fx->labelerror.c_str()<<endl;
       extracttext(fx,themax);
    }
    
    fx->close();
    if (res==1)
       return tree;
       
    delete tree;
    return NULL;
}


x_node* x_parse_string(string x,x_parsing_mode mode) {
    x_reading* fx=new x_readstring(x);
    return x_parsing(fx,mode);
}

x_node* x_parse_file(string x,x_parsing_mode mode) {
    x_reading* fx=new x_readfile((char*)x.c_str());
    return x_parsing(fx,mode);
}

//int main() {
// x_node* tree=x_parse_file("C:\\ANL\\Mekalinga\\ANL\\Mekalinga\\simple\\test\\generation.xip",FULL);
// if (tree!=NULL)
//    x_displaytree(tree,0);
//}
