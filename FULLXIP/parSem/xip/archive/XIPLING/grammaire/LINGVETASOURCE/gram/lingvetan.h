#ifndef bnf_lingveta_h
#define bnf_lingveta_h
#include "x_node.h"


class bnf_lingveta {
public:


int currentpos;
int intoken;
int lineerror;
string labelerror;
int errornumber;
char gFail;
int baseline;
x_reading* fx;

bnf_lingveta(int l=0) {
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



char m_common(string& lreturn,x_node** tree);
char m_loc(string& lreturn,x_node** tree);
char m_time(string& lreturn,x_node** tree);
char m_proper(string& lreturn,x_node** tree);
char m_num(string& lreturn,x_node** tree);
char m_sem(string& lreturn,x_node** tree);
char m_sem_0(string& lreturn,x_node** tree);
char m_space(string& lreturn,x_node** tree);
char m_space_1(string& lreturn,x_node** tree);
char m_space_1_2(string& lreturn,x_node** tree);
char m_end(string& lreturn,x_node** tree);
char m_end_3(string& lreturn,x_node** tree);
char m_que(string& lreturn,x_node** tree);
char m_que_4(string& lreturn,x_node** tree);
char m_quo(string& lreturn,x_node** tree);
char m_quo_5(string& lreturn,x_node** tree);
char m_conj(string& lreturn,x_node** tree);
char m_conj_6(string& lreturn,x_node** tree);
char m_nounterm(string& lreturn,x_node** tree);
char m_nounterm_7(string& lreturn,x_node** tree);
char m_nounterm_7_8(string& lreturn,x_node** tree);
char m_adjterm(string& lreturn,x_node** tree);
char m_adjterm_9(string& lreturn,x_node** tree);
char m_detterm(string& lreturn,x_node** tree);
char m_detterm_10(string& lreturn,x_node** tree);
char m_nountermacc(string& lreturn,x_node** tree);
char m_nountermacc_11(string& lreturn,x_node** tree);
char m_nountermacc_11_12(string& lreturn,x_node** tree);
char m_adjtermacc(string& lreturn,x_node** tree);
char m_adjtermacc_13(string& lreturn,x_node** tree);
char m_dettermacc(string& lreturn,x_node** tree);
char m_dettermacc_14(string& lreturn,x_node** tree);
char m_nountermgen(string& lreturn,x_node** tree);
char m_nountermgen_15(string& lreturn,x_node** tree);
char m_nountermgen_15_16(string& lreturn,x_node** tree);
char m_adjtermgen(string& lreturn,x_node** tree);
char m_adjtermgen_17(string& lreturn,x_node** tree);
char m_dettermgen(string& lreturn,x_node** tree);
char m_dettermgen_18(string& lreturn,x_node** tree);
char m_nountermdat(string& lreturn,x_node** tree);
char m_nountermdat_19(string& lreturn,x_node** tree);
char m_nountermdat_19_20(string& lreturn,x_node** tree);
char m_adjtermdat(string& lreturn,x_node** tree);
char m_adjtermdat_21(string& lreturn,x_node** tree);
char m_dettermdat(string& lreturn,x_node** tree);
char m_dettermdat_22(string& lreturn,x_node** tree);
char m_nountermconj(string& lreturn,x_node** tree);
char m_nountermconj_23(string& lreturn,x_node** tree);
char m_nountermconj_23_24(string& lreturn,x_node** tree);
char m_adjtermconj(string& lreturn,x_node** tree);
char m_adjtermconj_25(string& lreturn,x_node** tree);
char m_dettermconj(string& lreturn,x_node** tree);
char m_dettermconj_26(string& lreturn,x_node** tree);
char m_nountermaccconj(string& lreturn,x_node** tree);
char m_nountermaccconj_27(string& lreturn,x_node** tree);
char m_nountermaccconj_27_28(string& lreturn,x_node** tree);
char m_adjtermaccconj(string& lreturn,x_node** tree);
char m_adjtermaccconj_29(string& lreturn,x_node** tree);
char m_dettermaccconj(string& lreturn,x_node** tree);
char m_dettermaccconj_30(string& lreturn,x_node** tree);
char m_nountermgenconj(string& lreturn,x_node** tree);
char m_nountermgenconj_31(string& lreturn,x_node** tree);
char m_nountermgenconj_31_32(string& lreturn,x_node** tree);
char m_adjtermgenconj(string& lreturn,x_node** tree);
char m_adjtermgenconj_33(string& lreturn,x_node** tree);
char m_dettermgenconj(string& lreturn,x_node** tree);
char m_dettermgenconj_34(string& lreturn,x_node** tree);
char m_nountermdatconj(string& lreturn,x_node** tree);
char m_nountermdatconj_35(string& lreturn,x_node** tree);
char m_nountermdatconj_35_36(string& lreturn,x_node** tree);
char m_adjtermdatconj(string& lreturn,x_node** tree);
char m_adjtermdatconj_37(string& lreturn,x_node** tree);
char m_dettermdatconj(string& lreturn,x_node** tree);
char m_dettermdatconj_38(string& lreturn,x_node** tree);
char m_noun(string& lreturn,x_node** tree);
char m_noun_39(string& lreturn,x_node** tree);
char m_noun_39_40(string& lreturn,x_node** tree);
char m_adj(string& lreturn,x_node** tree);
char m_adj_41(string& lreturn,x_node** tree);
char m_det(string& lreturn,x_node** tree);
char m_det_43(string& lreturn,x_node** tree);
char m_nounacc(string& lreturn,x_node** tree);
char m_nounacc_45(string& lreturn,x_node** tree);
char m_adjacc(string& lreturn,x_node** tree);
char m_adjacc_47(string& lreturn,x_node** tree);
char m_detacc(string& lreturn,x_node** tree);
char m_detacc_49(string& lreturn,x_node** tree);
char m_noundat(string& lreturn,x_node** tree);
char m_noundat_51(string& lreturn,x_node** tree);
char m_adjdat(string& lreturn,x_node** tree);
char m_adjdat_53(string& lreturn,x_node** tree);
char m_detdat(string& lreturn,x_node** tree);
char m_detdat_55(string& lreturn,x_node** tree);
char m_noungen(string& lreturn,x_node** tree);
char m_noungen_57(string& lreturn,x_node** tree);
char m_adjgen(string& lreturn,x_node** tree);
char m_adjgen_59(string& lreturn,x_node** tree);
char m_detgen(string& lreturn,x_node** tree);
char m_detgen_61(string& lreturn,x_node** tree);
char m_nounconj(string& lreturn,x_node** tree);
char m_nounconj_63(string& lreturn,x_node** tree);
char m_adjconj(string& lreturn,x_node** tree);
char m_adjconj_65(string& lreturn,x_node** tree);
char m_detconj(string& lreturn,x_node** tree);
char m_detconj_67(string& lreturn,x_node** tree);
char m_nounaccconj(string& lreturn,x_node** tree);
char m_nounaccconj_69(string& lreturn,x_node** tree);
char m_adjaccconj(string& lreturn,x_node** tree);
char m_adjaccconj_71(string& lreturn,x_node** tree);
char m_detaccconj(string& lreturn,x_node** tree);
char m_detaccconj_73(string& lreturn,x_node** tree);
char m_noundatconj(string& lreturn,x_node** tree);
char m_noundatconj_75(string& lreturn,x_node** tree);
char m_adjdatconj(string& lreturn,x_node** tree);
char m_adjdatconj_77(string& lreturn,x_node** tree);
char m_detdatconj(string& lreturn,x_node** tree);
char m_detdatconj_79(string& lreturn,x_node** tree);
char m_noungenconj(string& lreturn,x_node** tree);
char m_noungenconj_81(string& lreturn,x_node** tree);
char m_adjgenconj(string& lreturn,x_node** tree);
char m_adjgenconj_83(string& lreturn,x_node** tree);
char m_detgenconj(string& lreturn,x_node** tree);
char m_detgenconj_85(string& lreturn,x_node** tree);
char m_npm(string& lreturn,x_node** tree);
char m_npm_87(string& lreturn,x_node** tree);
char m_npm_87_88(string& lreturn,x_node** tree);
char m_npm_87_89(string& lreturn,x_node** tree);
char m_npaccm(string& lreturn,x_node** tree);
char m_npaccm_90(string& lreturn,x_node** tree);
char m_npaccm_90_91(string& lreturn,x_node** tree);
char m_npaccm_90_92(string& lreturn,x_node** tree);
char m_npgenm(string& lreturn,x_node** tree);
char m_npgenm_93(string& lreturn,x_node** tree);
char m_npgenm_93_94(string& lreturn,x_node** tree);
char m_npgenm_93_95(string& lreturn,x_node** tree);
char m_npdatm(string& lreturn,x_node** tree);
char m_npdatm_96(string& lreturn,x_node** tree);
char m_npdatm_96_97(string& lreturn,x_node** tree);
char m_npdatm_96_98(string& lreturn,x_node** tree);
char m_npconj(string& lreturn,x_node** tree);
char m_npconj_99(string& lreturn,x_node** tree);
char m_npconj_99_102(string& lreturn,x_node** tree);
char m_npaccconj(string& lreturn,x_node** tree);
char m_npaccconj_103(string& lreturn,x_node** tree);
char m_npaccconj_103_106(string& lreturn,x_node** tree);
char m_npgenconj(string& lreturn,x_node** tree);
char m_npgenconj_107(string& lreturn,x_node** tree);
char m_npgenconj_107_110(string& lreturn,x_node** tree);
char m_npdatconj(string& lreturn,x_node** tree);
char m_npdatconj_111(string& lreturn,x_node** tree);
char m_npdatconj_111_114(string& lreturn,x_node** tree);
char m_np(string& lreturn,x_node** tree);
char m_np_115(string& lreturn,x_node** tree);
char m_npacc(string& lreturn,x_node** tree);
char m_npacc_116(string& lreturn,x_node** tree);
char m_npgen(string& lreturn,x_node** tree);
char m_npgen_117(string& lreturn,x_node** tree);
char m_npdat(string& lreturn,x_node** tree);
char m_npdat_118(string& lreturn,x_node** tree);
char m_verbpres(string& lreturn,x_node** tree);
char m_verbpres_119(string& lreturn,x_node** tree);
char m_verbpres_119_121(string& lreturn,x_node** tree);
char m_verbinf(string& lreturn,x_node** tree);
char m_verbinf_122(string& lreturn,x_node** tree);
char m_verbinf_122_124(string& lreturn,x_node** tree);
char m_verbpast(string& lreturn,x_node** tree);
char m_verbpast_125(string& lreturn,x_node** tree);
char m_verbpast_125_127(string& lreturn,x_node** tree);
char m_verbfut(string& lreturn,x_node** tree);
char m_verbfut_128(string& lreturn,x_node** tree);
char m_verbfut_128_130(string& lreturn,x_node** tree);
char m_verbcond(string& lreturn,x_node** tree);
char m_verbcond_131(string& lreturn,x_node** tree);
char m_verbcond_131_133(string& lreturn,x_node** tree);
char m_verbpresrel(string& lreturn,x_node** tree);
char m_verbpresrel_134(string& lreturn,x_node** tree);
char m_verbpresrel_134_136(string& lreturn,x_node** tree);
char m_verbinfrel(string& lreturn,x_node** tree);
char m_verbinfrel_137(string& lreturn,x_node** tree);
char m_verbinfrel_137_139(string& lreturn,x_node** tree);
char m_verbpastrel(string& lreturn,x_node** tree);
char m_verbpastrel_140(string& lreturn,x_node** tree);
char m_verbpastrel_140_142(string& lreturn,x_node** tree);
char m_verbfutrel(string& lreturn,x_node** tree);
char m_verbfutrel_143(string& lreturn,x_node** tree);
char m_verbfutrel_143_145(string& lreturn,x_node** tree);
char m_verbcondrel(string& lreturn,x_node** tree);
char m_verbcondrel_146(string& lreturn,x_node** tree);
char m_verbcondrel_146_148(string& lreturn,x_node** tree);
char m_adverb(string& lreturn,x_node** tree);
char m_adverb_149(string& lreturn,x_node** tree);
char m_adverb_149_151(string& lreturn,x_node** tree);
char m_vpnrel(string& lreturn,x_node** tree);
char m_vpnrel_152(string& lreturn,x_node** tree);
char m_vpnrel_152_153(string& lreturn,x_node** tree);
char m_vpnrel_152_154(string& lreturn,x_node** tree);
char m_vpnrel_152_154_155(string& lreturn,x_node** tree);
char m_vpnrel_152_156(string& lreturn,x_node** tree);
char m_vprel(string& lreturn,x_node** tree);
char m_vprel_157(string& lreturn,x_node** tree);
char m_vprel_157_159(string& lreturn,x_node** tree);
char m_vprel_157_159_160(string& lreturn,x_node** tree);
char m_vprel_157_161(string& lreturn,x_node** tree);
char m_vp(string& lreturn,x_node** tree);
char m_vp_162(string& lreturn,x_node** tree);
char m_pron(string& lreturn,x_node** tree);
char m_pron_163(string& lreturn,x_node** tree);
char m_pron_163_165(string& lreturn,x_node** tree);
char m_pron_163_165_166(string& lreturn,x_node** tree);
char m_sentencerel(string& lreturn,x_node** tree);
char m_sentencerel_167(string& lreturn,x_node** tree);
char m_sentencerel_167_168(string& lreturn,x_node** tree);
char m_sentencerel_167_168_169(string& lreturn,x_node** tree);
char m_sentence(string& lreturn,x_node** tree);
char m_sentence_170(string& lreturn,x_node** tree);
char m_parse(string& lreturn,x_node** tree);
char m_parse_172(string& lreturn,x_node** tree);
char m_parse_172_173(string& lreturn,x_node** tree);
};
#endif
