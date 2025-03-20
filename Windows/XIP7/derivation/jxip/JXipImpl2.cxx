/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */

/**
 * Native implementation for Java API to access the XIP C++ API.
 */
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#ifndef XIPLIBXML
#define XIPLIBXML
#endif

#include <stdio.h>
#include <stdlib.h>
#include "com_xerox_jxip_JXip2.h"
#include "com_xerox_jxip_RuleLoadedManager.h"
#include "com_xerox_jxip_RuleLoaded.h"
#include "xipstlres.h"
#include "pasofficiel.h"

#include "jni.h"

string jstringToString(JNIEnv* env, jstring str);
jstring jstringFromChars(JNIEnv* env, const char *chars);
jstring jstringFromString(JNIEnv* env, string str);
void generateException(JNIEnv *env, jobject obj);

/**
 * Parse until the value of rule number
 * @param gHandler a handler to a grammar
 * @param ruleNb rule number
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_XipSetRuleLimitImplementation(JNIEnv *env, jobject obj, jint gHandler, jint ruleNb) {
    //cout <<"JXipImpl2::XipSetRuleLimitImplementation ruleNb = " <<gHandler <<" " <<ruleNb <<endl;
    XipSetRuleLimit(gHandler, ruleNb);
}

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_JXip2_XipSetLabelRuleLimitImplementation(JNIEnv *env, jobject obj, jint gHandler, jstring label) {
    string labelStr = jstringToString(env, label);
    //cout <<"JXipImpl2::XipSetLabelRuleLimitImplementation label = " <<labelStr <<endl;
    char res = XipSetLabelRuleLimit(gHandler, labelStr);
    if (res == 1) {
        return 1;
    } else {
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_XipReturnRulesImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        //cout <<"JXipImpl2::XipReturnRulesImplementation "  <<endl;
        vector<int> applyedRulesNb;
        XipReturnRules(gHandler, applyedRulesNb);
        // now call the callback for each elem of the vector
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "XipReturnRulesCB";
        jmethodID mId = env->GetMethodID(cls, methodName, "(I)V");
        if (mId == 0) {
            cout << "JXipImpl2::XipReturnRulesImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        int ind = applyedRulesNb.size() - 1;
        //cout <<"JXipImpl2::XipReturnRulesImplementation applyed rules Nb = " <<ind <<endl;
        for (; ind >= 0; ind--) {
            //cout <<"JXipImpl2::XipReturnRulesImplementation ruleNb = " <<applyedRulesNb[ind] <<" index=" <<ind <<endl;
            jint ruleApplyedNb = applyedRulesNb[ind];
            //cout <<"JXipImpl2::XipReturnRulesImplementation ruleNb = " <<ruleApplyedNb <<endl;
            env->CallVoidMethod(obj, mId, ruleApplyedNb);
        }
    } catch (...) {
        generateException(env, obj);
    }
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_RuleLoaded_XipDisplayActivatedRulesImplementation(JNIEnv *env, jobject obj, jint gHandler, jint index) {
    try {
        ostringstream *os = new ostringstream();
        ;
        char display = 0;
        //cout <<"JXipImpl2::XipDisplayActivatedRulesImplementation index = " <<index <<endl;
        XipDisplayActivatedRules(gHandler, os, index, display);
        //cout <<"JXipImpl2::XipDisplayActivatedRulesImplementation os = " <<os->str() <<endl;
        jstring ruleSrc = jstringFromString(env, os->str());
        delete os;
        return ruleSrc;
    } catch (...) {
        generateException(env, obj);
        return jstringFromString(env, "");
    }
}

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_JXip2_XipIsGrammarCryptedImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        //cout <<"JXipImpl2::XipIsGrammarCryptedImplementation "  <<endl;
        char res = XipIsGrammarCrypted(gHandler);
        if (res == 1) {
            return 1;
        } else {
            return 0;
        }
    } catch (...) {
        generateException(env, obj);
        return 0;
    }
}

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_RuleLoadedManager_getRulesLoadedImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        //cout <<"JXipImpl2::getRulesLoadedImplementation " <<endl;
        // Init mId with the callback method
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "RuleLoadedManagerCB";
        jmethodID mId = env->GetMethodID(cls, methodName, "(ILjava/lang/String;JIIIILjava/lang/String;II)V");
        if (mId == 0) {
            cout << "JXipImpl2::getRulesLoadedImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        char prevFileName[2000] = "";
        VECTA<Label*>* liste_labels = XipReturnInfoRuleIndex(gHandler);
        for (int ind = 0; ind < liste_labels->dernier; ind++) {
            Label *label = liste_labels->vecteur[ind];
            DecompteRegle* dr = label->dr;
            jstring etiquette = jstringFromString(env, label->etiquette);
            if (dr != NULL) {
                jint type = 0;
                typeRegle typeRegle = dr->regle;
                switch (typeRegle) {
                    case REGLES_SEQUENCE:
                        type = 1;
                        break;
                    case REGLES_DI:
                        type = 2;
                        break;
                    case REGLES_SIMPLIFICATION:
                        type = 3;
                        break;
                    case REGLES_EXTRACTION:
                        type = 4;
                        break;
                    case REGLES_DEDUCTION:
                        type = 5;
                        break;
                    case REGLES_SEMANTIQUE:
                        type = 6;
                        break;
                    case REGLES_MARQUAGE:
                        type = 7;
                        break;
                    case REGLES_TERME:
                        type = 8;
                        break;
                    case REGLES_RECONSTRUCTION:
                        type = 9;
                        break;
                    case REGLES_RECONSTRUCTION_MODELE:
                        type = 10;
                        break;
                    case REGLES_DECOUPESEQUENCE:
                        type = 11;
                        break;
                    case REGLES_CARTESIEN:
                        type = 12;
                        break;
                    case REGLES_BOUCLE:
                        type = 13;
                        break;
                    case REGLES_FRAGMENTE:
                        type = 14;
                        break;
                    case REGLES_VOCABULAIRE:
                        type = 15;
                        break;
                    case REGLES_PL:
                        type = 16;
                        break;
                    case REGLES_MULTIMOTS:
                        type = 17;
                        break;
                    default:
                        cout << "rule type unknow" << endl;
                        break;
                }
                jstring fileName = jstringFromString(env, dr->nom_fichier);
                jlong offset = dr->position;
                jint lineNb = dr->ligne;
                jint absLayer = dr->couche;
                jint relLayer = dr->couche_relative;
                jint ruleNb = ind;
                //cout <<"nom_fichier = " <<dr->nom_fichier <<" prevFileName = " <<prevFileName <<endl;
                jint firstRule;
                if (strcmp(dr->nom_fichier, prevFileName)) {
                    //cout <<"NOT SAME" <<endl;
                    firstRule = 1;
                } else {
                    //cout <<"SAME" <<endl;
                    firstRule = 0;
                }
                strcpy(prevFileName, dr->nom_fichier);
                // now call the callBack
                env->CallVoidMethod(obj, mId, type, fileName, offset, lineNb, absLayer, relLayer, ruleNb, etiquette, gHandler, firstRule);
            }
        }
        return true;
    } catch (...) {
        generateException(env, obj);
        return false;
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_ListOfOperationsImplementation(JNIEnv *env, jobject obj) {
    try {
        //cout <<"JXipImpl2::ListOfOperationsImplementation" <<endl;
        vector<string> messages;
        // Init mId1 with the callback method
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "ListOfOperationsCB1";
        jmethodID mId1 = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;)V");
        if (mId1 == 0) {
            cout << "JXipImpl2::ListOfOperationsImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        // Init mId2 with the callback method
        methodName = "ListOfOperationsCB2";
        jmethodID mId2 = env->GetMethodID(cls, methodName, "(Ljava/lang/String;)V");
        if (mId2 == 0) {
            cout << "JXipImpl2::ListOfOperationsImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        ListOfOperations(messages);
        unsigned char car;
        for (int ind = 0; ind < messages.size(); ind++) {
            car = messages[ind][0];
            if (car >= '0' && car <= '9') {
                //cout <<"------" <<messages[ind]  <<"--------------" <<endl;
                jstring group = jstringFromString(env, messages[ind]);
                env->CallVoidMethod(obj, mId2, group);
            } else {
                char* fct = (char*) messages[ind].c_str();
                char* mes = strchr(fct, '\t');
                if (mes != NULL) {
                    *mes = 0;
                }
                mes++;
                if (mes != NULL) {
                    while (*mes == '\t') {
                        mes++;
                    }
                }
                //cout <<"#" <<fct <<"#" <<"\t\t\t" <<"#" <<mes <<"#"  <<endl;
                // now call the callBack
                jstring fctStr = jstringFromString(env, fct);
                jstring mesStr = jstringFromString(env, mes);
                env->CallVoidMethod(obj, mId1, fctStr, mesStr);
            }
        }
    } catch (...) {
        generateException(env, obj);
    }
}

#ifdef LIBSTORAGE
int agde_analysis(char* grmFileName, char* machine, char* database, char* user, char *password,
        char* filename, char store_sentence, char cleanbase);
#endif

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_parseToDBImplementation(JNIEnv *env, jobject obj, jstring grmfile, jstring machine, jstring dataBase, jstring user, jstring password, jstring fileName, jboolean storeSentence, jboolean cleanDB) {
    try {
        string grmFileName = jstringToString(env, grmfile);
        string mach = jstringToString(env, machine);
        string db = jstringToString(env, dataBase);
        string userName = jstringToString(env, user);
        string pw = jstringToString(env, password);
        string file = jstringToString(env, fileName);
        cout << "JXipImpl2::parseToDBImplementation  machine:" << mach << " dataBase::" << db << " user:" << userName
                << " password:" << pw << " file:" << file << endl;
        char storeSentenceChar = 0;
        char cleanDBChar = 0;
        if (storeSentence) {
            storeSentenceChar = 1;
        }
        if (cleanDB) {
            cleanDBChar = 1;
        }
#ifdef LIBSTORAGE
        agde_analysis((char*) grmFileName.c_str(), (char*) mach.c_str(), (char*) db.c_str(), (char*) userName.c_str(),
                (char*) pw.c_str(), (char*) file.c_str(), storeSentenceChar, cleanDBChar);
#endif

    } catch (...) {
        generateException(env, obj);
    }
}

void constructionFeatureTree(JNIEnv *env, jobject obj, arbretrait* arbreTrait, int *pIndexFile) {
    // Init mId with the callback method
    jclass cls = env->GetObjectClass(obj);
    char* methodName = "xipGetFeatureTreeCB1";
    jmethodID mId1 = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;I)V");
    if (mId1 == 0) {
        cout << "JXipImpl2::constructionFeatureTree  " << methodName << " not found" << endl;
        exit(1);
    }
    methodName = "xipGetFeatureTreeCB2";
    jmethodID mId2 = env->GetMethodID(cls, methodName, "(Ljava/lang/String;)V");
    if (mId2 == 0) {
        cout << "JXipImpl2::constructionFeatureTree  " << methodName << " not found" << endl;
        exit(1);
    }
    methodName = "xipGetFeatureTreeCB3";
    jmethodID mId3 = env->GetMethodID(cls, methodName, "(Ljava/lang/String;I)V");
    if (mId3 == 0) {
        cout << "JXipImpl2::constructionFeatureTree  " << methodName << " not found" << endl;
        exit(1);
    }
    methodName = "xipGetFeatureTreeCB4";
    jmethodID mId4 = env->GetMethodID(cls, methodName, "(Ljava/lang/String;I)V");
    if (mId4 == 0) {
        cout << "JXipImpl2::constructionFeatureTree  " << methodName << " not found" << endl;
        exit(1);
    }
    for (int ind = 0; ind < arbreTrait->fils.dernier; ind++) {
        if (arbreTrait->fils[ind]->type() == 0) {
            string file = ((featureroot*) arbreTrait->fils[ind])->filename.c_str();
            (*pIndexFile)++;
            //cout <<"+++++++++++ file = " <<file <<endl;
            jstring fileName = jstringFromString(env, file);
            // now call the callBack CB4
            env->CallVoidMethod(obj, mId4, fileName, *pIndexFile);
        }
        if (arbreTrait->fils[ind]->fils.dernier == 1 && arbreTrait->fils[ind]->fils[0]->fils.dernier == 0) {
            string label = arbreTrait->fils[ind]->nom.c_str();
            string value = arbreTrait->fils[ind]->fils[0]->nom.c_str();
            jstring name = jstringFromString(env, label);
            jstring val = jstringFromString(env, value);
            // now call the callBack CB1
            env->CallVoidMethod(obj, mId1, name, val, *pIndexFile);
        } else {
            string label = arbreTrait->fils[ind]->nom.c_str();
            jstring name = jstringFromString(env, label);
            // now call the callBack CB2
            env->CallVoidMethod(obj, mId2, name);
            constructionFeatureTree(env, obj, arbreTrait->fils[ind], pIndexFile);
            // now call the callBack CB3
            env->CallVoidMethod(obj, mId3, name, *pIndexFile);
        }
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGetFeatureTreeImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        //cout <<"JXipImpl2::xipGetFeatureTreeImplementation" <<endl;
        int indexFile = -1;
        arbretrait* arbreTrait = XipGetFeatureTree(gHandler);
        constructionFeatureTree(env, obj, arbreTrait, &indexFile);
    } catch (...) {
        generateException(env, obj);
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGetListCategoriesImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        //cout <<"JXipImpl2::xipGetListCategoriesImplementation" <<endl;
        vector<string> categoryVect;
        categoryVect.clear();
        // Init mId with the callback method
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "xipGetListCategoriesCB";
        jmethodID mId = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;)V");
        if (mId == 0) {
            cout << "JXipImpl2::xipGetListCategoriesImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        XipGetListCategories(gHandler, categoryVect);
        for (int i = 0; i < categoryVect.size(); i) {
            jstring catName = jstringFromString(env, categoryVect[i++].c_str());
            jstring fileName = jstringFromString(env, categoryVect[i++].c_str());
            // now call the callBack
            env->CallVoidMethod(obj, mId, catName, fileName);
        }
    } catch (...) {
        generateException(env, obj);
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGetListDependenciesImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        //cout <<"JXipImpl2::xipGetListDependenciesImplementation" <<endl;
        vector<string> dependencyVect;
        dependencyVect.clear();
        // Init mId with the callback method
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "xipGetListDependenciesCB";
        jmethodID mId = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;)V");
        if (mId == 0) {
            cout << "JXipImpl2::xipGetListDependenciesImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        XipGetListDependencies(gHandler, dependencyVect);
        for (int i = 0; i < dependencyVect.size();) {
            jstring depName = jstringFromString(env, dependencyVect[i++].c_str());
            jstring fileName = jstringFromString(env, dependencyVect[i++].c_str());
            // now call the callBack
            env->CallVoidMethod(obj, mId, depName, fileName);
        }
    } catch (...) {
        generateException(env, obj);
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGetListVariablesImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        //cout <<"JXipImpl2::xipGetListVariablesImplementation" <<endl;
        vector<string> variableNameVect;
        vector<typeVariable> variableTypeVect;
        variableNameVect.clear();
        variableTypeVect.clear();
        // Init mId with the callback method
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "xipGetListVariablesCB";
        jmethodID mId = env->GetMethodID(cls, methodName, "(Ljava/lang/String;I)V");
        if (mId == 0) {
            cout << "JXipImpl2::xipGetListVariablesImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        XipGetListVariables(gHandler, variableNameVect, variableTypeVect);
        for (int i = 0; i < variableNameVect.size(); i++) {
            string varNameStr = variableNameVect[i].c_str();
            int varType = variableTypeVect[i];
            // now call the callBack
            jstring varName = jstringFromString(env, varNameStr);
            env->CallVoidMethod(obj, mId, varName, varType);
        }
    } catch (...) {
        generateException(env, obj);
    }
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip2_xipGrammarPathnamesImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        //cout <<"JXipImpl2::xipGrammarPathnamesImplementation" <<endl;
        string languageFilename;
        vector<string> feature_pathnames;
        vector<string> lexicon_pathnames;
        vector<string> rule_pathnames;
        vector<string> param_pathnames;
        vector<string> scripts_pathnames;
        vector<string> divers_pathnames;
        long offset_begins[3];
        feature_pathnames.clear();
        lexicon_pathnames.clear();
        rule_pathnames.clear();
        param_pathnames.clear();
        scripts_pathnames.clear();
        divers_pathnames.clear();
        // Init mId with the callback method
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "xipGrammarPathnamesCB";
        jmethodID mId = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;)V");
        if (mId == 0) {
            cout << "JXipImpl2::xipGrammarPathnamesImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        XipGrammarPathnames(gHandler, languageFilename, feature_pathnames, offset_begins[0],
                lexicon_pathnames, offset_begins[1], rule_pathnames, offset_begins[2], param_pathnames,
                scripts_pathnames, divers_pathnames);
        for (int ind = 0; ind < feature_pathnames.size(); ind++) {
            //cout <<feature_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, feature_pathnames[ind].c_str());
            env->CallVoidMethod(obj, mId, jstringFromString(env, "feature_pathnames"), path);
        }
        for (int ind = 0; ind < lexicon_pathnames.size(); ind++) {
            //cout <<lexicon_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, lexicon_pathnames[ind].c_str());
            env->CallVoidMethod(obj, mId, jstringFromString(env, "lexicon_pathnames"), path);
        }
        for (int ind = 0; ind < rule_pathnames.size(); ind++) {
            //cout <<rule_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, rule_pathnames[ind].c_str());
            env->CallVoidMethod(obj, mId, jstringFromString(env, "rule_pathnames"), path);
        }
        for (int ind = 0; ind < param_pathnames.size(); ind++) {
            //cout <<param_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, param_pathnames[ind].c_str());
            env->CallVoidMethod(obj, mId, jstringFromString(env, "param_pathnames"), path);
        }
        for (int ind = 0; ind < scripts_pathnames.size(); ind++) {
            //cout <<scripts_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, scripts_pathnames[ind].c_str());
            env->CallVoidMethod(obj, mId, jstringFromString(env, "scripts_pathnames"), path);
        }
        for (int ind = 0; ind < divers_pathnames.size(); ind++) {
            //cout <<divers_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, divers_pathnames[ind].c_str());
            env->CallVoidMethod(obj, mId, jstringFromString(env, "divers_pathnames"), path);
        }
        return jstringFromString(env, languageFilename.c_str());
    } catch (...) {
        generateException(env, obj);
        return jstringFromString(env, "");
    }
}

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_JXip2_xipSetTaggerModeImplementation(JNIEnv *env, jobject obj, jint gHandler, jboolean mode) {
    try {
        char modeChar = 0;
        if (mode) {
            modeChar = 1;
        }
        return XipSetTaggerMode(gHandler, modeChar);
    } catch (...) {
        generateException(env, obj);
        return false;
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGrammarInformationImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        vector<string> licence;
        string clef;
        string version;
        string language;
        string grammarlanguage;
        string thelocale;
        string charset;
        char hmm;
        char reentrance;
        int max_words;
        int max_size_word;
        char encodage_lexique;
        int featurenumber;
        string tracepathname;
        string indentpathname;
        XipGrammarInformation(gHandler, licence, clef, version, language,
                grammarlanguage, thelocale, charset, hmm, reentrance, max_words,
                max_size_word, encodage_lexique, featurenumber, tracepathname, indentpathname);
        //for (int ind = 0; ind < licence.size(); ind++) {
        //	cout <<"licence: "<<licence[ind] <<endl;
        //}
        //cout <<"clef: "<<clef <<endl;
        //cout <<"version: " <<version <<endl;
        //cout <<"language: "<<language <<endl;
        //cout <<"grammarlanguage: "<<grammarlanguage <<endl;
        //cout <<"thelocale: "<<thelocale <<endl;
        //cout <<"charset: "<<charset <<endl;
        //cout <<"hmm: "<<hmm <<endl;
        //cout <<"reentrance: "<<reentrance <<endl;
        //cout <<"max_words: "<<max_words <<endl;
        //cout <<"max_size_word: "<<max_size_word <<endl;
        //cout <<"encodage_lexique: "<<encodage_lexique <<endl;
        //cout <<"featurenumber: "<<featurenumber <<endl;
        //cout <<"tracepathname: "<<tracepathname <<endl;
        //cout <<"indentpathname: "<<indentpathname <<endl;
        // Init mId1 with the callback method
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "xipGrammarInformationCB1";
        jmethodID mId1 = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;)V");
        if (mId1 == 0) {
            cout << "JXipImpl2::xipGrammarInformationImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        methodName = "xipGrammarInformationCB2";
        jmethodID mId2 = env->GetMethodID(cls, methodName, "(Ljava/lang/String;I)V");
        if (mId2 == 0) {
            cout << "JXipImpl2::xipGrammarInformationImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        string licenceStr = "";
        for (int ind = 0; ind < licence.size(); ind++) {
            licenceStr += licence[ind] + ", ";
        }
        env->CallVoidMethod(obj, mId1, jstringFromString(env, "licence"), jstringFromString(env, licenceStr.c_str()));
        env->CallVoidMethod(obj, mId1, jstringFromString(env, "clef"), jstringFromString(env, clef.c_str()));
        env->CallVoidMethod(obj, mId1, jstringFromString(env, "version"), jstringFromString(env, version.c_str()));
        env->CallVoidMethod(obj, mId1, jstringFromString(env, "language"), jstringFromString(env, language.c_str()));
        env->CallVoidMethod(obj, mId1, jstringFromString(env, "grammarlanguage"), jstringFromString(env, grammarlanguage.c_str()));
        env->CallVoidMethod(obj, mId1, jstringFromString(env, "thelocale"), jstringFromString(env, thelocale.c_str()));
        env->CallVoidMethod(obj, mId1, jstringFromString(env, "charset"), jstringFromString(env, charset.c_str()));
        env->CallVoidMethod(obj, mId1, jstringFromString(env, "tracepathname"), jstringFromString(env, tracepathname.c_str()));
        env->CallVoidMethod(obj, mId1, jstringFromString(env, "indentpathname"), jstringFromString(env, indentpathname.c_str()));
        env->CallVoidMethod(obj, mId2, jstringFromString(env, "max_words"), max_words);
        env->CallVoidMethod(obj, mId2, jstringFromString(env, "max_size_word"), max_size_word);
        env->CallVoidMethod(obj, mId2, jstringFromString(env, "featurenumber"), featurenumber);
        env->CallVoidMethod(obj, mId2, jstringFromString(env, "hmm"), hmm);
        env->CallVoidMethod(obj, mId2, jstringFromString(env, "reentrance"), reentrance);
        env->CallVoidMethod(obj, mId2, jstringFromString(env, "encodage_lexique"), encodage_lexique);
    } catch (...) {
        generateException(env, obj);
    }
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip2_xipIndentFileImplementation(JNIEnv *env, jobject obj, jint gHandler, jboolean mode) {
    int res;
    try {
        if (mode) {
            res = XipIndentFile(gHandler, 1);
        } else {
            res = XipIndentFile(gHandler, 0);
        }
    } catch (...) {
        generateException(env, obj);
    }
    return res;
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip2_xipSetResultDirectoryImplementation(JNIEnv *env, jobject obj, jint gHandler, jstring dir) {
    string dirStr = jstringToString(env, dir);
    int res;
    try {
        res = XipSetResultDirectory(gHandler, (char *) dirStr.c_str());
    } catch (...) {
        generateException(env, obj);
    }
    return res;
}

//----------------------------------------------------------------------------
JNIEnv *env1;
jobject obj1;
jmethodID mId1;

void xipCallBack(int gHandler, XipResult *xipResult, void *unused) {
    string xmlUnitElem = (const char*) XipGetCurrentInternalStringBuffer(gHandler);
    //cout <<"JXipImpl2::xipCallBack xmlUnitElem=" <<xmlUnitElem <<"  Number of sentences:"<<xipResult->sentences.size()<<endl;
    if (xipResult->sentences.size() == 1) {
        env1->CallVoidMethod(obj1, mId1, jstringFromString(env1, xmlUnitElem));
    }
}

void xipCallBackXml(int gHandler, XipResult *xipResult, void *unused) {
    XipCleanCurrentXipResult(gHandler);
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xmlUnitExtractionImplementation(JNIEnv *env, jobject obj, jint gHandler, jstring inputFile) {
    env1 = env;
    obj1 = obj;
    string inputFileStr = jstringToString(env, inputFile);
    //cout <<"JXipImpl2::xmlUnitExtractionImplementation: " <<inputFileStr <<endl;
    jclass cls = env->GetObjectClass(obj);
    char* methodName = "xmlUnitExtractionImplementationCB";
    mId1 = env->GetMethodID(cls, methodName, "(Ljava/lang/String;)V");
    if (mId1 == 0) {
        cout << "JXipImpl2::xmlUnitExtractionImplementation  " << methodName << " not found" << endl;
        exit(1);
    }
    try {
        //surtout pas de call a : XipDeleteCurrentXipResult(gHandler)
        XipCleanCurrentXipResult(gHandler);
        XipSetCallBack(gHandler, xipCallBack, NULL);
        XipSetCallBackXML(gHandler, xipCallBackXml, NULL);
        int depth = 2;
        XipResult *xipResult = XipCreateCurrentXipResult(gHandler);
        int res = XipParseXMLFile(gHandler, inputFileStr, depth, xipResult);
        // remove the callbacks
        XipSetCallBack(gHandler, NULL, NULL);
        XipSetCallBackXML(gHandler, NULL, NULL);

    } catch (...) {
        generateException(env, obj);
    }
}

//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipLayerRenumberingImplementation(JNIEnv *env, jobject obj, jint gHandler, jint idRule) {
    try {
        XipLayerRenumbering(gHandler, idRule);
    } catch (...) {
        generateException(env, obj);
    }
}

//----------------------------------------------------------------------------

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip2_xipGetLabelsImplementation(JNIEnv *env, jobject obj, jint gHandler, jstring labelName) {
    char surface[20];
    char lemme[20];
    try {
        string labelNameStr = jstringToString(env, labelName);
        XipGetLabels(gHandler, surface, lemme);
        if (strcmp(labelNameStr.c_str(), "lemme")) {
            return jstringFromString(env, surface);
        } else {
            return jstringFromString(env, lemme);
        }
    } catch (...) {
        generateException(env, obj);
    }
}

//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGetDictionaryVariableImplementation(JNIEnv *env, jobject obj, jint gHandler, jstring varName) {
    try {
        //cout <<"xipGetDictionaryVariableImplementation() " <<jstringToString(env, varName) <<endl;
        vector<string> nameValue;
        char res = XipGetDictionaryVariable(gHandler, jstringToString(env, varName), nameValue);
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "xipGetDictionaryVariableCB";
        jmethodID mId1 = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;)V");
        if (mId1 == 0) {
            cout << "JXipImpl2::xipGetDictionaryVariableImplementation  " << methodName << " not found" << endl;
            exit(1);
        }
        //cout <<"JXipImpl2::xipGetDictionaryVariableImplementation  " <<nameValue.size() <<endl;
        for (int ind = 0; ind < nameValue.size();) {
            string name = nameValue[ind++];
            string value = nameValue[ind++];
            //cout <<name <<" = " <<value <<endl;
            env->CallVoidMethod(obj, mId1, jstringFromString(env, name.c_str()), jstringFromString(env, value.c_str()));
        }
    } catch (...) {
        generateException(env, obj);
    }
}
//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipSetRuleRankNumberLimitImplementation(JNIEnv *env, jobject obj, jint gHandler, jint index) {
    try {
        XipSetRuleRankNumberLimit(gHandler, index);
    } catch (...) {
        generateException(env, obj);
    }
}
//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipSetMaxSentenceImplementation(JNIEnv *env, jobject obj, jint gHandler, jint maxSentenceNumber) {
    try {
        XipSetMaxSentence(gHandler, maxSentenceNumber);
    } catch (...) {
        generateException(env, obj);
    }
}
//----------------------------------------------------------------------------
