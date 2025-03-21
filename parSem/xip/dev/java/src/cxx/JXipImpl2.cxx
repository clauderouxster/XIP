/*
 *  Xerox Research Centre Europe - Grenoble Laboratory
 *  Copyright Xerox Corporation
 *  All Rights Reserved
 *
 */

/**
 * Native implementation for Java API to access the XIP C++ API.
 */
#include "JXipImpl.h"
#include "kif.h"

externkifmutex(jniprotect, _jniprotect);
void KiFInitialisationMutex();
void XipSetEnvVariable(string var, string val);
string XipGetEnvVariable(string var);


/**
* Set environment variables
* @param var is the  variable name
* @param val is the value to be set
*/
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip2_XipSetEnvVariableImplementation(
	JNIEnv *env, jobject obj, jstring varj, jstring valj) {
	string var = jstringToString(env, varj);
	string val = jstringToString(env, valj);
	XipSetEnvVariable(var, val);
	return 1;
}

/**
* Get environment variables
* @param var is the  variable name
*/
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip2_XipGetEnvVariableImplementation(
	JNIEnv *env, jobject obj, jstring varj) {
	string var = jstringToString(env, varj);
	var = XipGetEnvVariable(var);
	return jstringFromString(env, var);
}


/**
 * Execute and load a KiF program
 * @param filename is the KiF program pathname
 * @param args is a string in which each argument is seperated from the others with a space
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip2_KifExecuteProgramImplementation(
        JNIEnv *env, jobject obj, jstring filename,jstring args) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
	string nameOfFile= jstringToString(env, filename);
	string theargs=jstringToString(env, args);
	int kifhandler = KifExecuteProgram(nameOfFile, theargs);
	if (kifhandler== -1)
		checkXipErrorAndGenerateException(env, obj);
	return kifhandler;
}

/**
* Execute and load a KiF program
* @param filename is the KiF program pathname
* @param args is a string in which each argument is seperated from the others with a space
*/
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip2_KifExecuteCodeProgramImplementation(
	JNIEnv *env, jobject obj, jstring code, jstring args) {
	KiFInitialisationMutex();
	KifThreadLock _lock(jniprotect);
	string codeToCompile = jstringToString(env, code);
	string theargs = jstringToString(env, args);
	int kifhandler = KifExecuteCodeProgram(codeToCompile, theargs);
	if (kifhandler == -1)
		checkXipErrorAndGenerateException(env, obj);
	return kifhandler;
}
/**
 * Execute and load a KiF function
 * @param funcname is the KiF function name
 * @param parameters is a vector of string, each corresponding to a parameter of the function...
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip2_KifExecuteFunctionImplementation(
        JNIEnv *env, jobject obj, jstring funcname, jobjectArray parameters) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
	string nameOfFunction= jstringToString(env, funcname);
	vector<string> params;
    int stringCount = env->GetArrayLength(parameters);
	string value;
	jstring element;
	bool theerror=false;

	for (int i=0; i<stringCount; i++) {
		element = (jstring) env->GetObjectArrayElement(parameters, i);
		value= jstringToString(env, element);		
		params.push_back(value);
	}
	value=XipKifExecute(nameOfFunction,params,theerror);	
	if (theerror==true)
		checkXipErrorAndGenerateException(env, obj);

	return jstringFromString(env,value);
}

/**
* Execute and load a KiF function
* @param kifhandler is the KiFCode space
* @param funcname is the KiF function name
* @param parameters is a vector of string, each corresponding to a parameter of the function...
*/
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip2_KifExecuteFunctionImplementationIndex(
	JNIEnv *env, jobject obj, jint kifhandler, jstring funcname, jobjectArray parameters) {
	KiFInitialisationMutex();
	KifThreadLock _lock(jniprotect);
	string nameOfFunction = jstringToString(env, funcname);
	vector<string> params;
	int stringCount = env->GetArrayLength(parameters);
	string value;
	jstring element;
	bool theerror = false;

	for (int i = 0; i<stringCount; i++) {
		element = (jstring)env->GetObjectArrayElement(parameters, i);
		value = jstringToString(env, element);
		params.push_back(value);
	}
	value = XipKifExecuteIndex(kifhandler,nameOfFunction, params, theerror);
	if (theerror == true)
		checkXipErrorAndGenerateException(env, obj);

	return jstringFromString(env, value);
}

/**
* Execute and load a KiF program in a given grammar
* @param filename is the KiF program pathname
* @param args is a string in which each argument is seperated from the others with a space
*/
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_KifExecuteProgramParseurImplementation(
	JNIEnv *env, jobject obj, jint gHandler, jstring filename, jstring args) {
	KiFInitialisationMutex();
	KifThreadLock _lock(jniprotect);
	string nameOfFile = jstringToString(env, filename);
	string theargs = jstringToString(env, args);
	if (KifExecuteProgramParseur(gHandler, nameOfFile, theargs) == -1)
		checkXipErrorAndGenerateException(env, obj);
}

/**
* Execute and load a KiF program in a given grammar
* @param filename is the KiF program pathname
* @param args is a string in which each argument is seperated from the others with a space
*/
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_KifExecuteCodeProgramParseurImplementation(
	JNIEnv *env, jobject obj, jint gHandler, jstring code, jstring args) {
	KiFInitialisationMutex();
	KifThreadLock _lock(jniprotect);
	string codeToCompile = jstringToString(env, code);
	string theargs = jstringToString(env, args);
	if (KifExecuteCodeProgramParseur(gHandler, codeToCompile, theargs) == -1)
		checkXipErrorAndGenerateException(env, obj);
}

/**
 * Execute and load a KiF function from a given a grammar
 * @param funcname is the KiF function name
 * @param parameters is a vector of string, each corresponding to a parameter of the function...
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip2_KifExecuteFunctionParseurImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring funcname, jobjectArray parameters) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
	string nameOfFunction= jstringToString(env, funcname);
	vector<string> params;
    int stringCount = env->GetArrayLength(parameters);
	string value;
	jstring element;
	bool theerror=false;

	for (int i=0; i<stringCount; i++) {
		element = (jstring) env->GetObjectArrayElement(parameters, i);
		value= jstringToString(env, element);		
		params.push_back(value);
	}
	value=XipKifExecuteParseur(gHandler,nameOfFunction,params,theerror);	
	if (theerror==true)
		checkXipErrorAndGenerateException(env, obj);

	return jstringFromString(env,value);
}


/**
 * Parse until the value of rule number
 * @param gHandler a handler to a grammar
 * @param ruleNb rule number
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_XipSetRuleLimitImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jint ruleNb) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //cout <<"JXipImpl2::XipSetRuleLimitImplementation ruleNb = " <<gHandler
    // <<" " <<ruleNb <<endl;
    XipSetRuleLimit(gHandler, ruleNb);
}

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_JXip2_XipSetLabelRuleLimitImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring label) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    string labelStr = jstringToString(env, label);
    //cout <<"JXipImpl2::XipSetLabelRuleLimitImplementation label = " <<labelStr <<endl;
    char res = XipSetLabelRuleLimit(gHandler, labelStr);
    if (res == 1) {
        return 1;
    } else {
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_XipReturnRulesImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        //cout <<"JXipImpl2::XipReturnRulesImplementation "  <<endl;
        vector<int> applyedRulesNb;
        XipReturnRules(gHandler, applyedRulesNb);
        // now call the callback for each elem of the vector
        jmethodID mId = getMethodID(env, obj, "XipReturnRulesCB", "(I)V");
        int ind = applyedRulesNb.size() - 1;
        //cout <<"JXipImpl2::XipReturnRulesImplementation applyed rules Nb = " <<ind <<endl;
        for (; ind >= 0; ind--) {
            //cout <<"JXipImpl2::XipReturnRulesImplementation ruleNb = "
            // <<applyedRulesNb[ind] <<" index=" <<ind <<endl;
            jint ruleApplyedNb = applyedRulesNb[ind];
            //cout <<"JXipImpl2::XipReturnRulesImplementation ruleNb = " <<ruleApplyedNb <<endl;
            env->CallVoidMethod(obj, mId, ruleApplyedNb);
            checkIfJniExceptionOccurred(env);
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_RuleLoaded_XipDisplayActivatedRulesImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jint index) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        ostringstream *os = new ostringstream();
        char display = 0;
        //cout <<"JXipImpl2::XipDisplayActivatedRulesImplementation index = " <<index <<endl;
        XipDisplayActivatedRules(gHandler, os, index, display);
        //cout <<"JXipImpl2::XipDisplayActivatedRulesImplementation os = " <<os->str() <<endl;
        jstring ruleSrc = jstringFromString(env, os->str());
        delete os;
        return ruleSrc;
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return jstringFromString(env, "");
    }
}

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_JXip2_XipIsGrammarCryptedImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        //cout <<"JXipImpl2::XipIsGrammarCryptedImplementation "  <<endl;
        char res = XipIsGrammarCrypted(gHandler);
        if (res == 1) {
            return 1;
        } else {
            return 0;
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return 0;
    }
}

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_RuleLoadedManager_getRulesLoadedImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        //cout <<"JXipImpl2::getRulesLoadedImplementation " <<endl;
        // Init mId with the callback method RuleLoadedManager::RuleLoadedManagerCB()
        jmethodID mId = getMethodID(env, obj, "RuleLoadedManagerCB",
                "(ILjava/lang/String;JIIIILjava/lang/String;II)V");
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
                        cout << "rule type unknown" << endl;
                        generateJavaException(env, "rule type unknown");
                        break;
                }
                jstring fileName = jstringFromString(env, dr->nom_fichier);
                jlong offset = dr->position;
                jint lineNb = dr->ligne;
                jint absLayer = dr->couche;
                jint relLayer = dr->couche_relative;
                jint ruleNb = ind;
                //cout <<"nom_fichier = " <<dr->nom_fichier <<" prevFileName = "
                // <<prevFileName <<endl;
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
                env->CallVoidMethod(obj, mId, type, fileName, offset,
                        lineNb, absLayer, relLayer, ruleNb, etiquette, gHandler, firstRule);
                checkIfJniExceptionOccurred(env);
                deleteLocalJStringRef(env, fileName);
                deleteLocalJStringRef(env, etiquette);
            }
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return false;
    }
    return true;
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_ListOfOperationsImplementation(
        JNIEnv *env, jobject obj) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        //cout <<"JXipImpl2::ListOfOperationsImplementation" <<endl;
        vector<string> messages;
        // Init mId1 with the callback method
        jmethodID mId1 = getMethodID(env, obj, "listOfOperationsCB1",
                "(Ljava/lang/String;Ljava/lang/String;)V");
        // Init mId2 with the callback method
        jmethodID mId2 = getMethodID(env, obj, "listOfOperationsCB2",
                "(Ljava/lang/String;)V");
        ListOfOperations(messages);
        unsigned char car;
        for (int ind = 0; ind < messages.size(); ind++) {
            car = messages[ind][0];
            if (car >= '0' && car <= '9') {
                //cout <<"------" <<messages[ind]  <<"--------------" <<endl;
                jstring group = jstringFromString(env, messages[ind]);
                env->CallVoidMethod(obj, mId2, group);
                checkIfJniExceptionOccurred(env);
                deleteLocalJStringRef(env, group);
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
                checkIfJniExceptionOccurred(env);
                deleteLocalJStringRef(env, mesStr);
                deleteLocalJStringRef(env, fctStr);
            }
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}

#ifdef LIBSTORAGE
int agde_analysis(char* grmFileName, char* machine, char* database, char* user, char *password,
        char* filename, char store_sentence, char cleanbase);
#endif

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_parseToDBImplementation(
        JNIEnv *env, jobject obj, jstring grmfile, jstring machine, jstring dataBase,
        jstring user, jstring password, jstring fileName,
        jboolean storeSentence, jboolean cleanDB) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        string grmFileName = jstringToString(env, grmfile);
        string mach = jstringToString(env, machine);
        string db = jstringToString(env, dataBase);
        string userName = jstringToString(env, user);
        string pw = jstringToString(env, password);
        string file = jstringToString(env, fileName);
        cout << "JXipImpl2::parseToDBImplementation  machine:"
                << mach << " dataBase::" << db << " user:" << userName
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
        agde_analysis((char*) grmFileName.c_str(), (char*) mach.c_str(),
                (char*) db.c_str(), (char*) userName.c_str(),
                (char*) pw.c_str(), (char*) file.c_str(), storeSentenceChar, cleanDBChar);
#endif

    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}

void constructionFeatureTree(JNIEnv *env, jobject obj, arbretrait* arbreTrait, int *pIndexFile) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    // Init mId with the callback method
    jmethodID mId1 = getMethodID(env, obj, "xipGetFeatureTreeCB1",
            "(Ljava/lang/String;Ljava/lang/String;I)V");
    jmethodID mId2 = getMethodID(env, obj, "xipGetFeatureTreeCB2",
            "(Ljava/lang/String;)V");
    jmethodID mId3 = getMethodID(env, obj, "xipGetFeatureTreeCB3",
            "(Ljava/lang/String;I)V");
    jmethodID mId4 = getMethodID(env, obj, "xipGetFeatureTreeCB4",
            "(Ljava/lang/String;I)V");
    for (int ind = 0; ind < arbreTrait->fils.dernier; ind++) {
        if (arbreTrait->fils[ind]->type() == 0) {
            string file = ((featureroot*) arbreTrait->fils[ind])->filename.c_str();
            (*pIndexFile)++;
            //cout <<"+++++++++++ file = " <<file <<endl;
            jstring fileName = jstringFromString(env, file);
            // now call the callBack CB4
            env->CallVoidMethod(obj, mId4, fileName, *pIndexFile);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, fileName);
        }
        if (arbreTrait->fils[ind]->fils.dernier == 1 && arbreTrait->
                fils[ind]->fils[0]->fils.dernier == 0) {
            string label = arbreTrait->fils[ind]->nom.c_str();
            string value = arbreTrait->fils[ind]->fils[0]->nom.c_str();
            jstring name = jstringFromString(env, label);
            jstring val = jstringFromString(env, value);
            // now call the callBack CB1
            env->CallVoidMethod(obj, mId1, name, val, *pIndexFile);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, val);
            deleteLocalJStringRef(env, name);
        } else {
            string label = arbreTrait->fils[ind]->nom.c_str();
            jstring name = jstringFromString(env, label);
            // now call the callBack CB2
            env->CallVoidMethod(obj, mId2, name);
            checkIfJniExceptionOccurred(env);
            constructionFeatureTree(env, obj, arbreTrait->fils[ind], pIndexFile);
            // now call the callBack CB3
            env->CallVoidMethod(obj, mId3, name, *pIndexFile);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, name);
        }
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGetFeatureTreeImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        //cout <<"JXipImpl2::xipGetFeatureTreeImplementation" <<endl;
        int indexFile = -1;
        arbretrait* arbreTrait = XipGetFeatureTree(gHandler);
        constructionFeatureTree(env, obj, arbreTrait, &indexFile);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGetListCategoriesImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        //cout <<"JXipImpl2::xipGetListCategoriesImplementation" <<endl;
        vector<string> categoryVect;
        categoryVect.clear();
        // Init mId with the callback method
        jmethodID mId = getMethodID(env, obj, "xipGetListCategoriesCB",
                "(Ljava/lang/String;Ljava/lang/String;)V");
        XipGetListCategories(gHandler, categoryVect);
        for (int i = 0; i < categoryVect.size(); i) {
            jstring catName = jstringFromString(env, categoryVect[i++].c_str());
            jstring fileName = jstringFromString(env, categoryVect[i++].c_str());
            // now call the callBack
            env->CallVoidMethod(obj, mId, catName, fileName);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, fileName);
            deleteLocalJStringRef(env, catName);
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGetListDependenciesImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        //cout <<"JXipImpl2::xipGetListDependenciesImplementation" <<endl;
        vector<string> dependencyVect;
        dependencyVect.clear();
        // Init mId with the callback method
        jmethodID mId = getMethodID(env, obj, "xipGetListDependenciesCB",
                "(Ljava/lang/String;Ljava/lang/String;)V");
        XipGetListDependencies(gHandler, dependencyVect);
        for (int i = 0; i < dependencyVect.size();) {
            jstring depName = jstringFromString(env, dependencyVect[i++].c_str());
            jstring fileName = jstringFromString(env, dependencyVect[i++].c_str());
            // now call the callBack
            env->CallVoidMethod(obj, mId, depName, fileName);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, fileName);
            deleteLocalJStringRef(env, depName);
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGetListVariablesImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        //cout <<"JXipImpl2::xipGetListVariablesImplementation" <<endl;
        vector<string> variableNameVect;
        vector<typeVariable> variableTypeVect;
        variableNameVect.clear();
        variableTypeVect.clear();
        // Init mId with the callback method
        jmethodID mId = getMethodID(env, obj, "xipGetListVariablesCB",
                "(Ljava/lang/String;I)V");
        XipGetListVariables(gHandler, variableNameVect, variableTypeVect);
        for (int i = 0; i < variableNameVect.size(); i++) {
            string varNameStr = variableNameVect[i].c_str();
            int varType = variableTypeVect[i];
            // now call the callBack
            jstring varName = jstringFromString(env, varNameStr);
            env->CallVoidMethod(obj, mId, varName, varType);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, varName);
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip2_xipGrammarPathnamesImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
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
        jmethodID mId = getMethodID(env, obj, "xipGrammarPathnamesCB",
                "(Ljava/lang/String;Ljava/lang/String;)V");
        XipGrammarPathnames(gHandler, languageFilename, feature_pathnames, offset_begins[0],
                lexicon_pathnames, offset_begins[1], rule_pathnames, offset_begins[2],
                param_pathnames, scripts_pathnames, divers_pathnames);
        for (int ind = 0; ind < feature_pathnames.size(); ind++) {
            //cout <<feature_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, feature_pathnames[ind].c_str());
            jstring jstr1 = jstringFromString(env, "feature_pathnames");
            env->CallVoidMethod(obj, mId, jstr1, path);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, jstr1);
            deleteLocalJStringRef(env, path);
        }
        for (int ind = 0; ind < lexicon_pathnames.size(); ind++) {
            //cout <<lexicon_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, lexicon_pathnames[ind].c_str());
            jstring jstr1 = jstringFromString(env, "lexicon_pathnames");
            env->CallVoidMethod(obj, mId, jstr1, path);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, jstr1);
            deleteLocalJStringRef(env, path);
        }
        for (int ind = 0; ind < rule_pathnames.size(); ind++) {
            //cout <<rule_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, rule_pathnames[ind].c_str());
            jstring jstr1 = jstringFromString(env, "rule_pathnames");
            env->CallVoidMethod(obj, mId, jstr1, path);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, jstr1);
            deleteLocalJStringRef(env, path);
        }
        for (int ind = 0; ind < param_pathnames.size(); ind++) {
            //cout <<param_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, param_pathnames[ind].c_str());
            jstring jstr1 = jstringFromString(env, "param_pathnames");
            env->CallVoidMethod(obj, mId, jstr1, path);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, jstr1);
            deleteLocalJStringRef(env, path);
        }
        for (int ind = 0; ind < scripts_pathnames.size(); ind++) {
            //cout <<scripts_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, scripts_pathnames[ind].c_str());
            jstring jstr1 = jstringFromString(env, "scripts_pathnames");
            env->CallVoidMethod(obj, mId, jstr1, path);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, jstr1);
            deleteLocalJStringRef(env, path);
        }
        for (int ind = 0; ind < divers_pathnames.size(); ind++) {
            //cout <<divers_pathnames[ind] <<endl;
            // now call the callBack
            jstring path = jstringFromString(env, divers_pathnames[ind].c_str());
            jstring jstr1 = jstringFromString(env, "divers_pathnames");
            env->CallVoidMethod(obj, mId, jstr1, path);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, jstr1);
            deleteLocalJStringRef(env, path);
        }
        return jstringFromString(env, languageFilename.c_str());
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return jstringFromString(env, "");
    }
}

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_JXip2_xipSetTaggerModeImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jboolean mode) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        char modeChar = 0;
        if (mode) {
            modeChar = 1;
        }
        return XipSetTaggerMode(gHandler, modeChar);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return false;
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGrammarInformationImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
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
        jmethodID mId1 = getMethodID(env, obj, "xipGrammarInformationCB1",
                "(Ljava/lang/String;Ljava/lang/String;)V");
        jmethodID mId2 = getMethodID(env, obj, "xipGrammarInformationCB2",
                "(Ljava/lang/String;I)V");
        string licenceStr = "";
        for (int ind = 0; ind < licence.size(); ind++) {
            licenceStr += licence[ind] + ", ";
        }
        jstring jstr1 = jstringFromString(env, "licence");
        jstring jstr2 = jstringFromString(env, licenceStr.c_str());
        env->CallVoidMethod(obj, mId1, jstr1, jstr2);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr2);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "clef");
        jstr2 = jstringFromString(env, clef.c_str());
        env->CallVoidMethod(obj, mId1, jstr1, jstr2);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr2);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "version");
        jstr2 = jstringFromString(env, version.c_str());
        env->CallVoidMethod(obj, mId1, jstr1, jstr2);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr2);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "language");
        jstr2 = jstringFromString(env, language.c_str());
        env->CallVoidMethod(obj, mId1, jstr1, jstr2);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr2);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "grammarlanguage");
        jstr2 = jstringFromString(env, grammarlanguage.c_str());
        env->CallVoidMethod(obj, mId1, jstr1, jstr2);
        deleteLocalJStringRef(env, jstr2);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "thelocale");
        jstr2 = jstringFromString(env, thelocale.c_str());
        env->CallVoidMethod(obj, mId1, jstr1, jstr2);
        checkIfJniExceptionOccurred(env);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr2);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "charset");
        jstr2 = jstringFromString(env, charset.c_str());
        env->CallVoidMethod(obj, mId1, jstr1, jstr2);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr2);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "tracepathname");
        jstr2 = jstringFromString(env, tracepathname.c_str());
        env->CallVoidMethod(obj, mId1, jstr1, jstr2);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr2);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "indentpathname");
        jstr2 = jstringFromString(env, indentpathname.c_str());
        env->CallVoidMethod(obj, mId1, jstr1, jstr2);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr2);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "max_words");
        env->CallVoidMethod(obj, mId2, jstr1, max_words);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "max_size_word");
        env->CallVoidMethod(obj, mId2, jstr1, max_size_word);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "featurenumber");
        env->CallVoidMethod(obj, mId2, jstr1, featurenumber);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "hmm");
        env->CallVoidMethod(obj, mId2, jstr1, hmm);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "reentrance");
        env->CallVoidMethod(obj, mId2, jstr1, reentrance);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr1);

        jstr1 = jstringFromString(env, "encodage_lexiquej");
        env->CallVoidMethod(obj, mId2, jstr1, encodage_lexique);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, jstr1);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip2_xipIndentFileImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jboolean mode) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    int res;
    try {
        if (mode) {
            res = XipIndentFile(gHandler, 1);
        } else {
            res = XipIndentFile(gHandler, 0);
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
    return res;
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip2_xipSetResultDirectoryImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring dir) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    string dirStr = jstringToString(env, dir);
    int res;
    try {
        res = XipSetResultDirectory(gHandler, (char *) dirStr.c_str());
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
    return res;
}

//----------------------------------------------------------------------------
JNIEnv *env1;
jobject obj1;
jmethodID mId1;

void xipCallBack(int gHandler, XipResult *xipResult, void *unused) {
    string xmlUnitElem = (const char*) XipGetCurrentInternalStringBuffer(gHandler);
    //cout <<"JXipImpl2::xipCallBack xmlUnitElem=" <<xmlUnitElem
    // <<"  Number of sentences:"<<xipResult->sentences.size()<<endl;
    if (xipResult->sentences.size() == 1) {
        jstring jstr1 = jstringFromString(env1, xmlUnitElem);
        env1->CallVoidMethod(obj1, mId1, jstr1);
        checkIfJniExceptionOccurred(env1);
        deleteLocalJStringRef(env1, jstr1);
    }
}

void xipCallBackXml(int gHandler, XipResult *xipResult, void *unused) {
    XipCleanCurrentXipResult(gHandler);
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xmlUnitExtractionImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring inputFile) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    env1 = env;
    obj1 = obj;
    string inputFileStr = jstringToString(env, inputFile);
    //cout <<"JXipImpl2::xmlUnitExtractionImplementation: " <<inputFileStr <<endl;
    mId1 = getMethodID(env, obj, "xmlUnitExtractionImplementationCB",
            "(Ljava/lang/String;)V");
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
        //freeMem(env);
        checkXipErrorAndGenerateException(env, obj);
    }
    //freeMem(env);
}

//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipLayerRenumberingImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jint idRule) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        XipLayerRenumbering(gHandler, idRule);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}

//----------------------------------------------------------------------------

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip2_xipGetLabelsImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring labelName) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
	string surface;
	string lemme;
	try {
        string labelNameStr = jstringToString(env, labelName);
        XipGetLabels(gHandler, surface, lemme);
        if (strcmp(labelNameStr.c_str(), "lemme")) {
            return jstringFromString(env, surface);
        } else {
            return jstringFromString(env, lemme);
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}

//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipGetDictionaryVariableImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring varName) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        //cout <<"xipGetDictionaryVariableImplementation() "
        // <<jstringToString(env, varName) <<endl;
        vector<string> nameValue;
        char res = XipGetDictionaryVariable(gHandler, jstringToString(env, varName), nameValue);
        jmethodID mId1 = getMethodID(env, obj, "xipGetDictionaryVariableCB",
                "(Ljava/lang/String;Ljava/lang/String;)V");
        //cout <<"JXipImpl2::xipGetDictionaryVariableImplementation  " <<nameValue.size() <<endl;
        for (int ind = 0; ind < nameValue.size();) {
            string name = nameValue[ind++];
            string value = nameValue[ind++];
            //cout <<name <<" = " <<value <<endl;
            jstring jstr1 = jstringFromString(env, name.c_str());
            jstring jstr2 = jstringFromString(env, value.c_str());
            env->CallVoidMethod(obj, mId1, jstr1, jstr2);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, jstr2);
            deleteLocalJStringRef(env, jstr1);
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}
//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipSetRuleRankNumberLimitImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jint index) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        XipSetRuleRankNumberLimit(gHandler, index);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}
//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipSetMaxSentenceImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jint maxSentenceNumber) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        XipSetMaxSentence(gHandler, maxSentenceNumber);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
}
//----------------------------------------------------------------------------

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_JXip2_xipDeactivateRuleSpaceImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring label) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    string labelStr = jstringToString(env, label);
    if (XipAddBlockingRuleSpace(gHandler, labelStr)) {
        return true;
    } else {
        return false;
    }
}
//----------------------------------------------------------------------------

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_JXip2_xipActivateRuleSpaceImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring label, jboolean full) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    string labelStr = jstringToString(env, label);
    char fullChar;
    if (full) {
        fullChar = 0;
    } else {
        fullChar = 1;
    }
    if (XipRemoveBlockingRuleSpace(gHandler, labelStr, fullChar)) {
        return true;
    } else {
        return false;
    }
}

//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip2_xipCopyAndEncryptGrammarImplementation
(JNIEnv *env, jobject obj, jstring grmFileName, jstring destDir) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //convert jstring (unicode format) in ASCII characters
    const char* grmChars = env->GetStringUTFChars(grmFileName, 0);
    const char* destChars = env->GetStringUTFChars(destDir, 0);
    XipEncryptGrammar(grmChars, destChars, 1, 0);
    env->ReleaseStringUTFChars(grmFileName, grmChars);
    env->ReleaseStringUTFChars(destDir, destChars);
}


