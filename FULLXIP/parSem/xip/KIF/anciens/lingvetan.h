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
char m_nounterm(string& lreturn,x_node** tree);
char m_nounterm_4(string& lreturn,x_node** tree);
char m_nounterm_4_5(string& lreturn,x_node** tree);
char m_adjterm(string& lreturn,x_node** tree);
char m_adjterm_6(string& lreturn,x_node** tree);
char m_detterm(string& lreturn,x_node** tree);
char m_detterm_7(string& lreturn,x_node** tree);
char m_nountermacc(string& lreturn,x_node** tree);
char m_nountermacc_8(string& lreturn,x_node** tree);
char m_nountermacc_8_9(string& lreturn,x_node** tree);
char m_adjtermacc(string& lreturn,x_node** tree);
char m_adjtermacc_10(string& lreturn,x_node** tree);
char m_dettermacc(string& lreturn,x_node** tree);
char m_dettermacc_11(string& lreturn,x_node** tree);
char m_nountermgen(string& lreturn,x_node** tree);
char m_nountermgen_12(string& lreturn,x_node** tree);
char m_nountermgen_12_13(string& lreturn,x_node** tree);
char m_adjtermgen(string& lreturn,x_node** tree);
char m_adjtermgen_14(string& lreturn,x_node** tree);
char m_dettermgen(string& lreturn,x_node** tree);
char m_dettermgen_15(string& lreturn,x_node** tree);
char m_nountermdat(string& lreturn,x_node** tree);
char m_nountermdat_16(string& lreturn,x_node** tree);
char m_nountermdat_16_17(string& lreturn,x_node** tree);
char m_adjtermdat(string& lreturn,x_node** tree);
char m_adjtermdat_18(string& lreturn,x_node** tree);
char m_dettermdat(string& lreturn,x_node** tree);
char m_dettermdat_19(string& lreturn,x_node** tree);
char m_noun(string& lreturn,x_node** tree);
char m_noun_20(string& lreturn,x_node** tree);
char m_noun_20_21(string& lreturn,x_node** tree);
char m_adj(string& lreturn,x_node** tree);
char m_adj_22(string& lreturn,x_node** tree);
char m_det(string& lreturn,x_node** tree);
char m_det_24(string& lreturn,x_node** tree);
char m_nounacc(string& lreturn,x_node** tree);
char m_nounacc_26(string& lreturn,x_node** tree);
char m_adjacc(string& lreturn,x_node** tree);
char m_adjacc_28(string& lreturn,x_node** tree);
char m_detacc(string& lreturn,x_node** tree);
char m_detacc_30(string& lreturn,x_node** tree);
char m_noundat(string& lreturn,x_node** tree);
char m_noundat_32(string& lreturn,x_node** tree);
char m_adjdat(string& lreturn,x_node** tree);
char m_adjdat_34(string& lreturn,x_node** tree);
char m_detdat(string& lreturn,x_node** tree);
char m_detdat_36(string& lreturn,x_node** tree);
char m_noungen(string& lreturn,x_node** tree);
char m_noungen_38(string& lreturn,x_node** tree);
char m_adjgen(string& lreturn,x_node** tree);
char m_adjgen_40(string& lreturn,x_node** tree);
char m_detgen(string& lreturn,x_node** tree);
char m_detgen_42(string& lreturn,x_node** tree);
char m_np(string& lreturn,x_node** tree);
char m_np_44(string& lreturn,x_node** tree);
char m_np_44_45(string& lreturn,x_node** tree);
char m_np_44_46(string& lreturn,x_node** tree);
char m_npacc(string& lreturn,x_node** tree);
char m_npacc_47(string& lreturn,x_node** tree);
char m_npacc_47_48(string& lreturn,x_node** tree);
char m_npacc_47_49(string& lreturn,x_node** tree);
char m_npgen(string& lreturn,x_node** tree);
char m_npgen_50(string& lreturn,x_node** tree);
char m_npgen_50_51(string& lreturn,x_node** tree);
char m_npgen_50_52(string& lreturn,x_node** tree);
char m_npdat(string& lreturn,x_node** tree);
char m_npdat_53(string& lreturn,x_node** tree);
char m_npdat_53_54(string& lreturn,x_node** tree);
char m_npdat_53_55(string& lreturn,x_node** tree);
char m_verbpres(string& lreturn,x_node** tree);
char m_verbpres_56(string& lreturn,x_node** tree);
char m_verbpres_56_58(string& lreturn,x_node** tree);
char m_verbinf(string& lreturn,x_node** tree);
char m_verbinf_59(string& lreturn,x_node** tree);
char m_verbinf_59_61(string& lreturn,x_node** tree);
char m_verbpast(string& lreturn,x_node** tree);
char m_verbpast_62(string& lreturn,x_node** tree);
char m_verbpast_62_64(string& lreturn,x_node** tree);
char m_verbfut(string& lreturn,x_node** tree);
char m_verbfut_65(string& lreturn,x_node** tree);
char m_verbfut_65_67(string& lreturn,x_node** tree);
char m_verbcond(string& lreturn,x_node** tree);
char m_verbcond_68(string& lreturn,x_node** tree);
char m_verbcond_68_70(string& lreturn,x_node** tree);
char m_verbpresrel(string& lreturn,x_node** tree);
char m_verbpresrel_71(string& lreturn,x_node** tree);
char m_verbpresrel_71_73(string& lreturn,x_node** tree);
char m_verbinfrel(string& lreturn,x_node** tree);
char m_verbinfrel_74(string& lreturn,x_node** tree);
char m_verbinfrel_74_76(string& lreturn,x_node** tree);
char m_verbpastrel(string& lreturn,x_node** tree);
char m_verbpastrel_77(string& lreturn,x_node** tree);
char m_verbpastrel_77_79(string& lreturn,x_node** tree);
char m_verbfutrel(string& lreturn,x_node** tree);
char m_verbfutrel_80(string& lreturn,x_node** tree);
char m_verbfutrel_80_82(string& lreturn,x_node** tree);
char m_verbcondrel(string& lreturn,x_node** tree);
char m_verbcondrel_83(string& lreturn,x_node** tree);
char m_verbcondrel_83_85(string& lreturn,x_node** tree);
char m_adverb(string& lreturn,x_node** tree);
char m_adverb_86(string& lreturn,x_node** tree);
char m_adverb_86_88(string& lreturn,x_node** tree);
char m_vpnrel(string& lreturn,x_node** tree);
char m_vpnrel_89(string& lreturn,x_node** tree);
char m_vpnrel_89_90(string& lreturn,x_node** tree);
char m_vpnrel_89_91(string& lreturn,x_node** tree);
char m_vpnrel_89_91_92(string& lreturn,x_node** tree);
char m_vpnrel_89_93(string& lreturn,x_node** tree);
char m_vprel(string& lreturn,x_node** tree);
char m_vprel_94(string& lreturn,x_node** tree);
char m_vprel_94_96(string& lreturn,x_node** tree);
char m_vprel_94_96_97(string& lreturn,x_node** tree);
char m_vprel_94_98(string& lreturn,x_node** tree);
char m_vp(string& lreturn,x_node** tree);
char m_vp_99(string& lreturn,x_node** tree);
char m_parse(string& lreturn,x_node** tree);
char m_parse_100(string& lreturn,x_node** tree);
char m_parse_100_101(string& lreturn,x_node** tree);
};
#endif
