#ifndef bnf_kif_h
#define bnf_kif_h
#include "x_node.h"


class bnf_kif {
public:


int currentpos;
int intoken;
int lineerror;
string labelerror;
int errornumber;
char gFail;
int baseline;
x_reading* fx;

bnf_kif(int l=0) {
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



char m_char(string& lreturn,x_node** tree);
char m_char_0(string& lreturn,x_node** tree);
char m_digit(string& lreturn,x_node** tree);
char m_path(string& lreturn,x_node** tree);
char m_path_1(string& lreturn,x_node** tree);
char m_path_1_2(string& lreturn,x_node** tree);
char m_shell(string& lreturn,x_node** tree);
char m_shell_3(string& lreturn,x_node** tree);
char m_shell_3_4(string& lreturn,x_node** tree);
char m_label(string& lreturn,x_node** tree);
char m_label_5(string& lreturn,x_node** tree);
char m_private(string& lreturn,x_node** tree);
char m_common(string& lreturn,x_node** tree);
char m_const(string& lreturn,x_node** tree);
char m_feature(string& lreturn,x_node** tree);
char m_feature_6(string& lreturn,x_node** tree);
char m_feature_6_7(string& lreturn,x_node** tree);
char m_feature_6_7_8(string& lreturn,x_node** tree);
char m_feature_6_9(string& lreturn,x_node** tree);
char m_type(string& lreturn,x_node** tree);
char m_type_10(string& lreturn,x_node** tree);
char m_type_10_11(string& lreturn,x_node** tree);
char m_anydouble(string& lreturn,x_node** tree);
char m_anydouble_12(string& lreturn,x_node** tree);
char m_anydouble_12_13(string& lreturn,x_node** tree);
char m_anydouble_12_14(string& lreturn,x_node** tree);
char m_anysimple(string& lreturn,x_node** tree);
char m_anysimple_15(string& lreturn,x_node** tree);
char m_anycolon(string& lreturn,x_node** tree);
char m_anycolon_16(string& lreturn,x_node** tree);
char m_anycolon_16_17(string& lreturn,x_node** tree);
char m_anycolon_16_18(string& lreturn,x_node** tree);
char m_operator(string& lreturn,x_node** tree);
char m_operator_19(string& lreturn,x_node** tree);
char m_operator_19_20(string& lreturn,x_node** tree);
char m_operator_19_21(string& lreturn,x_node** tree);
char m_operator_19_22(string& lreturn,x_node** tree);
char m_operatoraffectation(string& lreturn,x_node** tree);
char m_operatoraffectation_23(string& lreturn,x_node** tree);
char m_operatoraffectation_23_24(string& lreturn,x_node** tree);
char m_operatoraffectation_23_25(string& lreturn,x_node** tree);
char m_operatoraffectation_23_26(string& lreturn,x_node** tree);
char m_operatoraffectation_23_27(string& lreturn,x_node** tree);
char m_operatoraffectation_23_28(string& lreturn,x_node** tree);
char m_operatoraffectation_23_29(string& lreturn,x_node** tree);
char m_operatoraffectation_23_30(string& lreturn,x_node** tree);
char m_operatoraffectation_23_31(string& lreturn,x_node** tree);
char m_operatoraffectation_23_32(string& lreturn,x_node** tree);
char m_operatoraffectation_23_33(string& lreturn,x_node** tree);
char m_operatoraffectation_23_34(string& lreturn,x_node** tree);
char m_orand(string& lreturn,x_node** tree);
char m_orand_35(string& lreturn,x_node** tree);
char m_orand_35_36(string& lreturn,x_node** tree);
char m_orand_35_37(string& lreturn,x_node** tree);
char m_comparator(string& lreturn,x_node** tree);
char m_comparator_38(string& lreturn,x_node** tree);
char m_comparator_38_39(string& lreturn,x_node** tree);
char m_comparator_38_40(string& lreturn,x_node** tree);
char m_comparator_38_41(string& lreturn,x_node** tree);
char m_comparator_38_42(string& lreturn,x_node** tree);
char m_comparator_38_43(string& lreturn,x_node** tree);
char m_comparator_38_44(string& lreturn,x_node** tree);
char m_depend(string& lreturn,x_node** tree);
char m_depend_45(string& lreturn,x_node** tree);
char m_declaration(string& lreturn,x_node** tree);
char m_declaration_46(string& lreturn,x_node** tree);
char m_declaration_46_47(string& lreturn,x_node** tree);
char m_declaration_46_47_48(string& lreturn,x_node** tree);
char m_declaration_46_49(string& lreturn,x_node** tree);
char m_declaration_46_50(string& lreturn,x_node** tree);
char m_declaration_46_50_51(string& lreturn,x_node** tree);
char m_declarationlist(string& lreturn,x_node** tree);
char m_declarationlist_52(string& lreturn,x_node** tree);
char m_declarationlist_52_56(string& lreturn,x_node** tree);
char m_declarationlist_52_56_57(string& lreturn,x_node** tree);
char m_declarationlist_52_58(string& lreturn,x_node** tree);
char m_declarationlist_52_58_59(string& lreturn,x_node** tree);
char m_multideclaration(string& lreturn,x_node** tree);
char m_multideclaration_60(string& lreturn,x_node** tree);
char m_declarationending(string& lreturn,x_node** tree);
char m_frame(string& lreturn,x_node** tree);
char m_frame_68(string& lreturn,x_node** tree);
char m_frame_68_70(string& lreturn,x_node** tree);
char m_instruction(string& lreturn,x_node** tree);
char m_instruction_71(string& lreturn,x_node** tree);
char m_instruction_71_72(string& lreturn,x_node** tree);
char m_nonlimited(string& lreturn,x_node** tree);
char m_nonlimited_73(string& lreturn,x_node** tree);
char m_arguments(string& lreturn,x_node** tree);
char m_arguments_74(string& lreturn,x_node** tree);
char m_arguments_74_75(string& lreturn,x_node** tree);
char m_arguments_74_75_76(string& lreturn,x_node** tree);
char m_arguments_74_75_76_77(string& lreturn,x_node** tree);
char m_strict(string& lreturn,x_node** tree);
char m_join(string& lreturn,x_node** tree);
char m_protecclusive(string& lreturn,x_node** tree);
char m_protecclusive_78(string& lreturn,x_node** tree);
char m_functionsort(string& lreturn,x_node** tree);
char m_functionsort_79(string& lreturn,x_node** tree);
char m_typefunction(string& lreturn,x_node** tree);
char m_typefunction_80(string& lreturn,x_node** tree);
char m_typefunction_80_81(string& lreturn,x_node** tree);
char m_typefunction_80_82(string& lreturn,x_node** tree);
char m_typefunction_80_83(string& lreturn,x_node** tree);
char m_typefunction_80_84(string& lreturn,x_node** tree);
char m_indexname(string& lreturn,x_node** tree);
char m_indexname_85(string& lreturn,x_node** tree);
char m_intervalname(string& lreturn,x_node** tree);
char m_intervalname_86(string& lreturn,x_node** tree);
char m_function(string& lreturn,x_node** tree);
char m_function_87(string& lreturn,x_node** tree);
char m_function_87_88(string& lreturn,x_node** tree);
char m_function_87_89(string& lreturn,x_node** tree);
char m_sousbloc(string& lreturn,x_node** tree);
char m_sousbloc_91(string& lreturn,x_node** tree);
char m_bloc(string& lreturn,x_node** tree);
char m_bloc_92(string& lreturn,x_node** tree);
char m_bloc_92_93(string& lreturn,x_node** tree);
char m_complexbloc(string& lreturn,x_node** tree);
char m_complexbloc_94(string& lreturn,x_node** tree);
char m_blocs(string& lreturn,x_node** tree);
char m_blocs_95(string& lreturn,x_node** tree);
char m_blocs_95_96(string& lreturn,x_node** tree);
char m_analyse(string& lreturn,x_node** tree);
char m_analyse_97(string& lreturn,x_node** tree);
char m_analyse_97_98(string& lreturn,x_node** tree);
char m_analyse_97_99(string& lreturn,x_node** tree);
char m_analyse_97_99_100(string& lreturn,x_node** tree);
char m_analyse_97_99_100_101(string& lreturn,x_node** tree);
char m_subfunc(string& lreturn,x_node** tree);
char m_subfunc_102(string& lreturn,x_node** tree);
char m_subfuncbis(string& lreturn,x_node** tree);
char m_subfuncbis_103(string& lreturn,x_node** tree);
char m_subvar(string& lreturn,x_node** tree);
char m_subvar_104(string& lreturn,x_node** tree);
char m_framesep(string& lreturn,x_node** tree);
char m_framesep_105(string& lreturn,x_node** tree);
char m_frameup(string& lreturn,x_node** tree);
char m_frameup_106(string& lreturn,x_node** tree);
char m_functioncall(string& lreturn,x_node** tree);
char m_functioncall_107(string& lreturn,x_node** tree);
char m_functioncall_107_108(string& lreturn,x_node** tree);
char m_regularcall(string& lreturn,x_node** tree);
char m_regularcall_109(string& lreturn,x_node** tree);
char m_regularcall_109_110(string& lreturn,x_node** tree);
char m_regularcall_109_111(string& lreturn,x_node** tree);
char m_regularcall_109_111_112(string& lreturn,x_node** tree);
char m_cut(string& lreturn,x_node** tree);
char m_predicate(string& lreturn,x_node** tree);
char m_predicate_113(string& lreturn,x_node** tree);
char m_predicate_113_114(string& lreturn,x_node** tree);
char m_predicatecall(string& lreturn,x_node** tree);
char m_predicatecall_115(string& lreturn,x_node** tree);
char m_returncall(string& lreturn,x_node** tree);
char m_returncall_119(string& lreturn,x_node** tree);
char m_breakcall(string& lreturn,x_node** tree);
char m_continuecall(string& lreturn,x_node** tree);
char m_call(string& lreturn,x_node** tree);
char m_call_120(string& lreturn,x_node** tree);
char m_sep(string& lreturn,x_node** tree);
char m_indexes(string& lreturn,x_node** tree);
char m_indexes_121(string& lreturn,x_node** tree);
char m_indexes_121_122(string& lreturn,x_node** tree);
char m_indexes_121_122_123(string& lreturn,x_node** tree);
char m_interval(string& lreturn,x_node** tree);
char m_interval_124(string& lreturn,x_node** tree);
char m_interval_124_125(string& lreturn,x_node** tree);
char m_variable(string& lreturn,x_node** tree);
char m_variable_129(string& lreturn,x_node** tree);
char m_variable_129_130(string& lreturn,x_node** tree);
char m_variable_129_130_131(string& lreturn,x_node** tree);
char m_predicatevariable(string& lreturn,x_node** tree);
char m_predicatevariable_132(string& lreturn,x_node** tree);
char m_predicatevariable_132_133(string& lreturn,x_node** tree);
char m_term(string& lreturn,x_node** tree);
char m_term_134(string& lreturn,x_node** tree);
char m_term_134_135(string& lreturn,x_node** tree);
char m_allvariable(string& lreturn,x_node** tree);
char m_allvariable_136(string& lreturn,x_node** tree);
char m_plusplus(string& lreturn,x_node** tree);
char m_plusplus_137(string& lreturn,x_node** tree);
char m_plusplus_137_138(string& lreturn,x_node** tree);
char m_plusplus_137_139(string& lreturn,x_node** tree);
char m_affectation(string& lreturn,x_node** tree);
char m_affectation_140(string& lreturn,x_node** tree);
char m_affectation_140_141(string& lreturn,x_node** tree);
char m_affectation_140_141_142(string& lreturn,x_node** tree);
char m_affectationpredicate(string& lreturn,x_node** tree);
char m_affectationpredicate_143(string& lreturn,x_node** tree);
char m_increment(string& lreturn,x_node** tree);
char m_increment_146(string& lreturn,x_node** tree);
char m_notin(string& lreturn,x_node** tree);
char m_notin_147(string& lreturn,x_node** tree);
char m_notin_147_148(string& lreturn,x_node** tree);
char m_notin_147_149(string& lreturn,x_node** tree);
char m_in(string& lreturn,x_node** tree);
char m_operatorin(string& lreturn,x_node** tree);
char m_operatorin_150(string& lreturn,x_node** tree);
char m_operationin(string& lreturn,x_node** tree);
char m_operationin_151(string& lreturn,x_node** tree);
char m_operationin_151_152(string& lreturn,x_node** tree);
char m_operationin_151_152_153(string& lreturn,x_node** tree);
char m_operation(string& lreturn,x_node** tree);
char m_operation_154(string& lreturn,x_node** tree);
char m_comparison(string& lreturn,x_node** tree);
char m_comparison_155(string& lreturn,x_node** tree);
char m_alist(string& lreturn,x_node** tree);
char m_alist_156(string& lreturn,x_node** tree);
char m_alist_156_157(string& lreturn,x_node** tree);
char m_alist_156_157_158(string& lreturn,x_node** tree);
char m_valvector(string& lreturn,x_node** tree);
char m_valvector_159(string& lreturn,x_node** tree);
char m_valvector_159_160(string& lreturn,x_node** tree);
char m_merging(string& lreturn,x_node** tree);
char m_alistnomerge(string& lreturn,x_node** tree);
char m_alistnomerge_161(string& lreturn,x_node** tree);
char m_alistnomerge_161_162(string& lreturn,x_node** tree);
char m_alistnomerge_161_162_163(string& lreturn,x_node** tree);
char m_apredicatelist(string& lreturn,x_node** tree);
char m_apredicatelist_164(string& lreturn,x_node** tree);
char m_apredicatelist_164_165(string& lreturn,x_node** tree);
char m_apredicatelist_164_165_166(string& lreturn,x_node** tree);
char m_apredicatelist_164_165_166_167(string& lreturn,x_node** tree);
char m_apredicatelist_164_165_166_168(string& lreturn,x_node** tree);
char m_valpredicatevector(string& lreturn,x_node** tree);
char m_valpredicatevector_169(string& lreturn,x_node** tree);
char m_valpredicatevector_169_170(string& lreturn,x_node** tree);
char m_dvalue(string& lreturn,x_node** tree);
char m_dico(string& lreturn,x_node** tree);
char m_dico_171(string& lreturn,x_node** tree);
char m_dico_171_172(string& lreturn,x_node** tree);
char m_dico_171_172_173(string& lreturn,x_node** tree);
char m_valmap(string& lreturn,x_node** tree);
char m_valmap_174(string& lreturn,x_node** tree);
char m_valmap_174_175(string& lreturn,x_node** tree);
char m_dicopredicate(string& lreturn,x_node** tree);
char m_dicopredicate_176(string& lreturn,x_node** tree);
char m_dicopredicate_176_177(string& lreturn,x_node** tree);
char m_dicopredicate_176_177_178(string& lreturn,x_node** tree);
char m_valmappredicate(string& lreturn,x_node** tree);
char m_valmappredicate_179(string& lreturn,x_node** tree);
char m_valmappredicate_179_180(string& lreturn,x_node** tree);
char m_negation(string& lreturn,x_node** tree);
char m_negation_181(string& lreturn,x_node** tree);
char m_parameters(string& lreturn,x_node** tree);
char m_parameters_182(string& lreturn,x_node** tree);
char m_parameters_182_183(string& lreturn,x_node** tree);
char m_parameters_182_183_184(string& lreturn,x_node** tree);
char m_predicateparameters(string& lreturn,x_node** tree);
char m_predicateparameters_185(string& lreturn,x_node** tree);
char m_predicateparameters_185_186(string& lreturn,x_node** tree);
char m_predicateparameters_185_187(string& lreturn,x_node** tree);
char m_predicateparameters_185_187_188(string& lreturn,x_node** tree);
char m_optional(string& lreturn,x_node** tree);
char m_optional_189(string& lreturn,x_node** tree);
char m_expression(string& lreturn,x_node** tree);
char m_expression_190(string& lreturn,x_node** tree);
char m_expression_190_191(string& lreturn,x_node** tree);
char m_expression_190_192(string& lreturn,x_node** tree);
char m_expressions(string& lreturn,x_node** tree);
char m_expressions_193(string& lreturn,x_node** tree);
char m_expressions_193_194(string& lreturn,x_node** tree);
char m_expressions_193_194_195(string& lreturn,x_node** tree);
char m_numbers(string& lreturn,x_node** tree);
char m_numbers_196(string& lreturn,x_node** tree);
char m_numbers_196_197(string& lreturn,x_node** tree);
char m_xnumber(string& lreturn,x_node** tree);
char m_xnumber_198(string& lreturn,x_node** tree);
char m_xnumber_198_199(string& lreturn,x_node** tree);
char m_xnumber_198_199_200(string& lreturn,x_node** tree);
char m_abool(string& lreturn,x_node** tree);
char m_abool_201(string& lreturn,x_node** tree);
char m_dboperation(string& lreturn,x_node** tree);
char m_dboperation_202(string& lreturn,x_node** tree);
char m_predicatedatabase(string& lreturn,x_node** tree);
char m_predicatedatabase_203(string& lreturn,x_node** tree);
char m_blocfor(string& lreturn,x_node** tree);
char m_blocfor_204(string& lreturn,x_node** tree);
char m_blocfor_204_205(string& lreturn,x_node** tree);
char m_blocfor_204_206(string& lreturn,x_node** tree);
char m_blocfor_204_206_207(string& lreturn,x_node** tree);
char m_optionalboolean(string& lreturn,x_node** tree);
char m_optionalboolean_208(string& lreturn,x_node** tree);
char m_booleanexpression(string& lreturn,x_node** tree);
char m_booleanexpression_209(string& lreturn,x_node** tree);
char m_booleanexpression_209_210(string& lreturn,x_node** tree);
char m_booleanexpression_209_211(string& lreturn,x_node** tree);
char m_booleanexpression_209_211_212(string& lreturn,x_node** tree);
char m_booleanexpression_209_211_212_213(string& lreturn,x_node** tree);
char m_predicateexpression(string& lreturn,x_node** tree);
char m_predicateexpression_214(string& lreturn,x_node** tree);
char m_predicateexpression_214_215(string& lreturn,x_node** tree);
char m_predicateexpression_214_215_216(string& lreturn,x_node** tree);
char m_predicateexpression_214_215_216_217(string& lreturn,x_node** tree);
char m_predicateexpression_214_218(string& lreturn,x_node** tree);
char m_predicateexpression_214_218_219(string& lreturn,x_node** tree);
char m_test(string& lreturn,x_node** tree);
char m_test_220(string& lreturn,x_node** tree);
char m_test_220_221(string& lreturn,x_node** tree);
char m_test_220_222(string& lreturn,x_node** tree);
char m_test_220_223(string& lreturn,x_node** tree);
char m_test_220_223_224(string& lreturn,x_node** tree);
char m_testelif(string& lreturn,x_node** tree);
char m_testelif_226(string& lreturn,x_node** tree);
char m_loop(string& lreturn,x_node** tree);
char m_loop_232(string& lreturn,x_node** tree);
char m_doloop(string& lreturn,x_node** tree);
char m_doloop_234(string& lreturn,x_node** tree);
char m_for(string& lreturn,x_node** tree);
char m_for_236(string& lreturn,x_node** tree);
char m_forin(string& lreturn,x_node** tree);
char m_forin_238(string& lreturn,x_node** tree);
char m_switch(string& lreturn,x_node** tree);
char m_switch_240(string& lreturn,x_node** tree);
char m_switch_240_242(string& lreturn,x_node** tree);
char m_trycatch(string& lreturn,x_node** tree);
char m_trycatch_243(string& lreturn,x_node** tree);
char m_trycatch_243_244(string& lreturn,x_node** tree);
char m_trycatch_243_244_245(string& lreturn,x_node** tree);
char m_trycatch_243_244_245_246(string& lreturn,x_node** tree);
char m_trycatch_243_247(string& lreturn,x_node** tree);
char m_trycatch_243_247_248(string& lreturn,x_node** tree);
char m_testswitch(string& lreturn,x_node** tree);
char m_testswitch_249(string& lreturn,x_node** tree);
char m_testswitch_249_250(string& lreturn,x_node** tree);
char m_default(string& lreturn,x_node** tree);
char m_predicatedefinition(string& lreturn,x_node** tree);
char m_predicatedefinition_252(string& lreturn,x_node** tree);
char m_predicatedefinition_252_253(string& lreturn,x_node** tree);
char m_predicatefact(string& lreturn,x_node** tree);
char m_predicatefact_254(string& lreturn,x_node** tree);
char m_word(string& lreturn,x_node** tree);
char m_typename(string& lreturn,x_node** tree);
char m_astringdouble(string& lreturn,x_node** tree);
char m_astringsimple(string& lreturn,x_node** tree);
char m_afullstring(string& lreturn,x_node** tree);
char m_anumber(string& lreturn,x_node** tree);
};
#endif
