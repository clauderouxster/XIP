/*
 *  Xerox Research Centre Europe - Grenoble Laboratory
 *  Copyright  Xerox Corporation
 *  All Rights Reserved
 *
 */

/**
 * Native implementation for Java API to access the XIP C++ API.
 */
#include "JXipImpl.h"
#include "kif.h"

kifmutex(jniprotect, _jniprotect);

void KiFInitialisationMutex() {
    static bool init = false;
    if (init == false)
        KifCreateMutex(jniprotect);
    init = true;
}

/**
 * Converts the built-in Unicode representation of a java string into a UTF-8 c++ string
 * Take care if the java input string contains non ASCII characters,
 * utf8 represents them with more than one byte.
 * if the input string contains only ASCII characters, the output UTF-8 string
 *  is the same than the input string.
 * @param str the java string
 * @return the utf8 c++ string
 */
string jstringToString(JNIEnv* env, jstring str) {
    /*
     * The GetStringUTFChars function retrieves 8-bit characters from a 16-bit jstring
     * using the Unicode Transformation Format (UTF). UTF represents Unicode as a string
     * of 8 or 16 bit characters without losing any information.
     * The third parameter GetStringUTFChars results the result JNI_TRUE if it made
     * a local copy of the jstring or JNI_FALSE otherwise.
     */
    jboolean iscopy;
    const char* strChars = env->GetStringUTFChars(str, &iscopy);
    string result = string(strChars);
    //cout <<"jstsringToString result = " <<result <<endl;
    // now the the JVM can free the memory used by jstring str
    env->ReleaseStringUTFChars(str, strChars);
    return result;
}

/**
 * Constructs a new java.lang.String object from an array of UTF-8 characters.
 * IMPORTANT NOTE: to avoid memory leak use deleteLocalJStringRef() to ask JVM to free memeory
 *  
 * @param the pointer to a utf8 c++ string
 * @return the java string
 *
 */
//VECTA<jstring> listjstring;

jstring jstringFromChars(JNIEnv* env, const char *chars) {
    jstring jstr = env->NewStringUTF(chars);
    //    listjstring.ajoute(jstr);
    return jstr;
}

/**
 * converts a c++ string to a java string
 * IMPORTANT NOTE: to avoid memory leak use deleteLocalJStringRef() to ask JVM to free memeory
 * @param the utf8 c++ string
 * @return the java string
 *
 */
jstring jstringFromString(JNIEnv* env, string str) {
    return jstringFromChars(env, str.c_str());
}

void deleteLocalJStringRef(JNIEnv* env, jstring jstr) {
    //cout << "JXipImpl::deleteLocalJStringRef() " ;
    //    if (jstr == listjstring[listjstring.dernier - 1]) {
    if (jstr != NULL) {
        env->DeleteLocalRef(jstr);
    }
    //        listjstring.retire(); //retire listjstring[listjstring.dernier]
    //cout << " deleted " ;
    //    } else {
    //        int ind = listjstring.cherche(jstr);
    //        if (ind != -1) {
    //            env->DeleteLocalRef(jstr);
    //            listjstring.retireElement(ind);
    //            //cout << " found " ;
    //        } else {
    //            //cout << " not deleted " ;
    //        }
    //    }
    //cout << listjstring.size() << endl;
}

void deleteLocalJclassRef(JNIEnv* env, jclass jClasse) {
    if (jClasse != NULL) {
        env->DeleteLocalRef(jClasse);
    }
}

//void freeMem(JNIEnv* env) {
//    //cout << "--------------------------------------------------JXipImpl::freeMem() " << listjstring.size() << endl;
//    for (int ind = 0; ind < listjstring.dernier; ind++) {
//        jstring jstr = listjstring.vecteur[ind];
//        env->DeleteLocalRef(jstr);
//    }
//    listjstring.raz();
//}

//void checkMem(JNIEnv* env) {
//    cout << "--------------------------------------------------JXipImpl::checkMem() " << listjstring.size() << endl;
//}

/**
 * Check if an Exception occurred during JNI call
 * @param env
 */
void checkIfJniExceptionOccurred(JNIEnv *env) {
    jthrowable exc = env->ExceptionOccurred();
    if (exc) {
        //print a debugging message about exception
        env->ExceptionDescribe();
        generateJavaException(env, "JNI exception occurred");
        // clear the exception
        env->ExceptionClear();
    }
}

/**
 * generate java exception
 * @param env
 * @param message
 */
void generateJavaException(JNIEnv *env, string message) {
    //cout << "generateJavaException: " << message << endl;
    jclass exceptionClass = env->FindClass("com/xerox/jxip/XipException");
    if (exceptionClass != 0) {
        env->ThrowNew(exceptionClass, message.c_str());
        deleteLocalJclassRef(env, exceptionClass);
    }
}

jmethodID getMethodID(JNIEnv *env, jobject obj, char *methodName, char *signature) {
    jclass cls = env->GetObjectClass(obj);
    jmethodID mId = env->GetMethodID(cls, methodName, signature);
    deleteLocalJclassRef(env, cls);
    if (mId == 0) {
        generateJavaException(env, methodName);
    }
    return mId;
}

///**
// * The exception XIPError can't be catch: catch (XIPError *xipError) ????
// * (I do not know why): so we use XipGetLastError() to get the error message,
// *   and generate a java Exception
// *
// **/
//void checkXipErrorAndGenerateException(JNIEnv *env, jobject obj) {
//    cout << "checkXipErrorAndGenerateException" << endl;
//    XIPError* xiperror = XipGetLastError();
//    if (xiperror != 0) {
//        if (xiperror->message != "") {
//            generateJavaException(env, xiperror->message.c_str());
//        } else {
//            generateJavaException(env, "Internal Error");
//        }
//    } else {
//        generateJavaException(env, "Internal Error");
//    }
//}

/**
 * check and clear xiperror and generate java XipException if necessary
 * @param env
 * @param obj
 */
void checkXipErrorAndGenerateException(JNIEnv *env, jobject obj) {
    //cout << "checkXipErrorAndGenerateException" << endl;
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    XIPError* xiperror = XipGetLastError();
    jclass xipExceptionClass = env->FindClass("com/xerox/jxip/XipException");
    if (xiperror != 0) {
        //there is an error
        if (xipExceptionClass != 0) {
            if (xiperror->filename.compare("")) {
                // call XipException(String msg, String fileN, long lineN, long pos)
                // get constructor method ID
                jmethodID methodID = env->GetMethodID(xipExceptionClass, "<init>",
                        "(Ljava/lang/String;Ljava/lang/String;JJ)V");
                jstring mess = jstringFromString(env, xiperror->message);
                jstring file = jstringFromString(env, xiperror->filename);
                jlong line = xiperror->line;
                jlong pos = xiperror->position;
                // construct exception object
                jthrowable xipExceptionObject = (jthrowable) env->NewObject(xipExceptionClass, methodID,
                        mess, file, line, pos);
                checkIfJniExceptionOccurred(env);
                deleteLocalJStringRef(env, file);
                deleteLocalJStringRef(env, mess);
                deleteLocalJclassRef(env, xipExceptionClass);
                // throw java XipException
                env->Throw(xipExceptionObject);
            } else {
                // call XipException(String msg)
                // get constructor method ID
                jmethodID methodID = env->GetMethodID(xipExceptionClass, "<init>",
                        "(Ljava/lang/String;)V");
                jstring mess = jstringFromString(env, xiperror->message);
                // construct exception object
                jthrowable xipExceptionObject = (jthrowable) env->NewObject(xipExceptionClass, methodID,
                        mess);
                checkIfJniExceptionOccurred(env);
                deleteLocalJStringRef(env, mess);
                deleteLocalJclassRef(env, xipExceptionClass);
                // throw java XipException
                env->Throw(xipExceptionObject);
            }
        } else {
            //class not found
            cerr << "Internal error" << endl;
        }
        XipCleanLastError();
    }
}

//JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_initXipExceptionImplementation(
//        JNIEnv *env, jobject obj) {
//    cout << "initXipExceptionImplementation" << endl;
//    KiFInitialisationMutex();
//    KifThreadLock _lock(jniprotect);
//    XIPError* xiperror = XipGetLastError();
//    if (xiperror != 0) {
//        jstring mess = jstringFromString(env, xiperror->message);
//        jstring file = jstringFromString(env, xiperror->filename);
//        jstring timeS = jstringFromString(env, xiperror->timestamp);
//        jlong line = xiperror->line;
//        jlong pos = xiperror->position;
//        //cout <<"JXipImpl::initXipExceptionImplementation(): " <<mess <<endl;
//        //call the callback JXip::initXipExceptionImplementationCB()
//        jmethodID mId = getMethodID(env, obj, "initXipExceptionImplementationCB",
//                "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JJ)V");
//        env->CallVoidMethod(obj, mId, mess, file, timeS, line, pos);
//        checkIfJniExceptionOccurred(env);
//        deleteLocalJStringRef(env, timeS);
//        deleteLocalJStringRef(env, file);
//        deleteLocalJStringRef(env, mess);
//    }
//}

/**
 *  load grammar files using a grm file
 *
 * @param  grmFileName  the grm file name
 * @param  enableXml    when set to one, this parameter tells XIP that
 * the input complies with the XIP XML DTD
 * @param  loadFst 	if true loads the FST otherwise parse only the grammar
 * file without loading FST
 * @return              a handler for a specific grammar or Exception on error
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipGrmFileImplementation(
        JNIEnv *env, jobject obj,
        jstring grmFileName, jint enableXml, jboolean loadFst) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //cout << "XipGrmFileImplementation" << endl;
    obj = NULL;
    int ipar;
    try {
        string grammarFile = jstringToString(env, grmFileName);
        //cout <<"Java_com_xerox_jxip_JXip_XipGrmFileImplementation  grmFile = "
        // <<grammarFile <<" enableXml = " <<enableXml <<endl;
        char loadall;
        if (loadFst) {
            loadall = 1;
        } else {
            loadall = 0;
        }
        ipar = XipGrmFile(grammarFile, loadall, enableXml);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return -1;
    }
    checkXipErrorAndGenerateException(env, obj);
    return ipar;
}

/**
 *  loading  grammars,  NTM and HMM ressources files.
 *
 * @param  grammarFileName  the grammar file name (.../english.xip)
 * @param  ntmFileName      the NTM script file name(.../ntmscript)
 * @param  hmmFileName      the HMM file name
 * @param  enableXml        When set to one , this parameter tells XIP that
 * the input complies with the XIP XML DTD
 * @return                  a handler to the grammar.
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipLoadGrammarImplementation(
        JNIEnv *env, jobject obj,
        jstring grammarFileName, jstring ntmFileName, jstring hmmFileName, jint enableXml) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    obj = NULL;
    int ipar;
    try {
        string grammarFileNameStr = jstringToString(env, grammarFileName);
        string ntmFileNameStr = jstringToString(env, ntmFileName);
        string hmmFileNameStr = jstringToString(env, hmmFileName);
        //cout << "Java_com_xerox_jxip_JXip_loadGrammarImplementation grammarFileNameStr = "
        // <<grammarFileNameStr;
        //cout <<" ntmFileNameStr = " <<ntmFileNameStr  <<" hmmFileNameStr = "
        // <<hmmFileNameStr <<"\n";
        ipar = XipLoadGrammar(grammarFileNameStr, "", enableXml, ntmFileNameStr, hmmFileNameStr);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return -1;
    }
    checkXipErrorAndGenerateException(env, obj);
    return ipar;
}

/**
 *  loading  grammars,  NTM and HMM ressources files.
 *   ressources files assuming that the input files
 *   fit with the XIP XML DTD specifications.
 *
 * @param  grammarFileName  the grammar file name (.../english.xip)
 * @param  ntmFileName      the NTM script file name(.../ntmscript)
 * @param  hmmFileName      the HMM file name
 * @param  enableXml        When set to one , this parameter tells XIP
 * that the input complies with the XIP XML DTD
 * @return                  a handler to the grammar. (or -1 if error)
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipGrammarLoadingImplementation(
        JNIEnv *env, jobject obj,
        jstring grammarFileName, jstring ntmFileName, jstring hmmFileName, jint enableXml) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    obj = NULL;
    int gHandler;
    try {
        string grammarFileNameStr = jstringToString(env, grammarFileName);
        string ntmFileNameStr = jstringToString(env, ntmFileName);
        string hmmFileNameStr = jstringToString(env, hmmFileName);
        //cout << "Java_com_xerox_jxip_JXip_loadGrammarImplementation grammarFileNameStr = "
        // <<grammarFileNameStr;
        //cout <<" ntmFileNameStr = " <<ntmFileNameStr  <<" hmmFileNameStr = "
        // <<hmmFileNameStr <<"\n";
        gHandler = XipGrammarLoading(grammarFileNameStr, "", enableXml,
                ntmFileNameStr, hmmFileNameStr);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return -1;
    }
    checkXipErrorAndGenerateException(env, obj);
    return (jint) gHandler;
}

/**
 *  Reload a grammar
 *
 * @param  gHandler  a handler to a grammar
 * @param  reloadParameter  if 0 then the parameter files are not
 * reload else parameter files are reloaded
 * @param  grammarFileName  the grm file
 * @return      a handler to the grammar
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipReloadGrammarImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jchar reloadParameter, jstring grammarFileName) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    string grammarFileNameStr = jstringToString(env, grammarFileName);
    //cout << "XipReloadGrammarImplementation  gHandler = " << gHandler
    // << " reloadParameter = " << reloadParameter << " grammarFileNameStr = "
    // << grammarFileNameStr << endl;
    try {
        int newGHandler;
        if (reloadParameter == 0) {
            newGHandler = XipReloadGrammar(gHandler, reloadParameter, NULL);
        } else {
            newGHandler = XipReloadGrammar(gHandler, reloadParameter,
                    (char*) grammarFileNameStr.c_str());
        }
        return newGHandler;
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return -1;
    }
    checkXipErrorAndGenerateException(env, obj);
}

/**
 *  set a value to a string variable
 *
 * @param  gHandler  a handler to a grammar
 * @param  name 		variable name
 * @param  value         variable value
 * @return           return -1 if error, 0 otherwise
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipSetStringVariableImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jstring name, jstring value) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    string variableName = jstringToString(env, name);
    string variableValue = jstringToString(env, value);
    //cout <<"XipSetStringVariableImplementation  variableName = "
    // <<variableName <<" variableValue = " <<variableValue <<endl;
    try {
        char result = XipSetStringVariable(gHandler, variableName, variableValue);
        if (result == 1) {
            return 0;
        } else {
            return -1;
        }
    } catch (...) {
        cerr << "XipSetStringVariableImplementation Error" << endl;
        return -1;
    }
}

/**
 *  get the string value of a variable
 *
 * @param  gHandler  a handler to a grammar
 * @param  name    variable name
 * @return      string value
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipGetStringVariableImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jstring name) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    string variableName = jstringToString(env, name);
    //cout <<"XipGetStringVariableImplementation  variableName = " <<variableName  <<endl;
    try {
        string variableValue;
        char result = XipGetVariableValue(gHandler, variableName, variableValue);
        if (result == 1) {
            jstring result = jstringFromString(env, variableValue);
            return result;
        } else {
            return jstringFromString(env, "");
        }
    } catch (...) {
        cerr << "XipGetStringVariableImplementation Error" << endl;
        return jstringFromString(env, "");
    }
}

/**
 *  get the string value of a variable
 *
 * @param  gHandler  a handler to a grammar
 * @param  name variable name
 * @return  string value
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipSetIntVariableImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jstring name, jfloat value) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    string variableName = jstringToString(env, name);
    float variableValue = (float) value;
    //cout <<"XipSetIntVariableImplementation  variableName = "
    // <<variableName <<"  value = " <<variableValue <<endl;
    try {
        char result = XipSetIntVariable(gHandler, variableName, variableValue);
        if (result == 1) {
            return 0;
        } else {
            return -1;
        }
    } catch (...) {
        cerr << "XipSetIntVariableImplementation Error" << endl;
        return -1;
    }
}

/**
 *  load a XML data base file
 *
 * @param  gHandler  a handler to a grammar
 * @param  fileNam  a XML data base file name
 * @param  depth
 * @param  ali       an alias that must be associated with this xml database file
 * @return           return -1 if error, 0 otherwise
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipLoadXMLDataBaseImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jstring fileNam, jstring ali) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    string fileName = jstringToString(env, fileNam);
    string alias = jstringToString(env, ali);
    //cout <<"XipLoadXMLDataBaseImplementation gHandler = " <<gHandler
    // <<" fileName = " << fileName <<" alias = " <<alias <<endl;
    try {
        int result = XipLoadXMLDataBase(gHandler, fileName, alias);
        if (result == 0) {
            return 0;
        } else {
            return -1;
        }
    } catch (...) {
        cerr << "XipLoadXMLDataBaseImplementation Error" << endl;
        return -1;
    }
}

/**
 *  get a list of aliases (provided by a parameter file)
 *
 * @param  gHandler  a handler to a grammar
 * @return           return -1 if error, 0 otherwise
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipAliasListImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //cout <<"XipAliasListImplementation " <<endl;
    vector<string> aliases;
    try {
        XipAliasList(gHandler, aliases);
        //cout <<"XipAliasListImplementation aliases.size = " <<aliases.size() <<endl;
        jmethodID mId = getMethodID(env, obj, "addAliasInListCB", "(Ljava/lang/String;)V");
        for (int index = 0; index < aliases.size(); index++) {
            jstring alias = jstringFromString(env, aliases[index]);
            //cout <<"XipAliasListImplementation alias = " <<aliases[index] <<endl;
            env->CallVoidMethod(obj, mId, alias);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, alias);
        }
    } catch (...) {
        cerr << "XipAliasListImplementation Error" << endl;
        return -1;
    }
    return 0;
}

/**
 *  load a parameter file
 *
 * @param  gHandler  a handler to a grammar
 * @param  fileName  a parameter file name
 * @return           return -1 if error, 0 otherwise
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipParameterFileImplementation(JNIEnv *env,
        jobject obj, jint gHandler, jstring configFileName) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    obj = NULL;
    try {
        string configFileNameStr = jstringToString(env, configFileName);
        //cout <<"XipParameterFileImplementation gHandler = "
        // <<gHandler <<" configFileName = " <<configFileNameStr <<endl;
        int result = XipParameterFile(gHandler, configFileNameStr);
        if (result == 0) {
            return 0;
        } else {
            return -1;
        }
    } catch (...) {
        cerr << "XipParameterFileImplementation Error" << endl;
        return -1;
    }
}

/**
 *  This method clean the data in memory corresponding to the xmltree
 *
 * @param  gHandler  the handler to the grammar to reload
 * @param  alias     the alias
 * @return int       0 ik OK, -1 if error
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipCleanXMLAliasImplementation(JNIEnv *env,
        jobject obj, jint gHandler, jstring alias) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    obj = NULL;
    string aliasStr = jstringToString(env, alias);
    //cout <<"XipCleanXMLAliasImplementation aliasStr = " <<aliasStr <<endl;
    try {
        char result = XipCleanXMLAlias(gHandler, aliasStr);
        if (result == 0) {
            //cout <<"XipCleanXMLAliasImplementation OK: " <<endl;
            return 0;
        } else {
            if (result == 1) {
                //cout <<"XipCleanXMLAliasImplementation Error: 1 " <<endl;
                return 1;
            } else {
                //cout <<"XipCleanXMLAliasImplementation Error: -1 " <<endl;
                return -1;
            }
        }
    } catch (...) {
        cerr << "XipCleanXMLAliasImplementation Error" << endl;
        return -1;
    }
}

/** *
 * Get version about the current XIP C++ Library used.
 * @return String	     Version number.
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_getVersionImplementation(
        JNIEnv *env, jobject obj) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    obj = NULL;
    string question;
    try {
        Whoami(question);
        jstring result = jstringFromString(env, question);
        return result;
    } catch (...) {
        cerr << "getVersionImplementation Error" << endl;
        return jstringFromChars(env, "");
    }
}

/**
 * Get information about the current XIP grammar license
 *   (loaded in a specific XIP Session).
 * @return int	Days remaining before license ending.
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipLicenseImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    env = NULL;
    obj = NULL;
    try {
        int errCode = XipLicense(gHandler);
        return errCode;
    } catch (...) {
        cerr << "XipLicenseImplementation Error" << endl;
        return -1;
    }
}

//One and XIPLONG are defined in xipstlres.h

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipSetDisplayModeImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jlong displayMode, jint colonne) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    XIPLONG value;
    if (displayMode == 0) {
        value = 0L;
    } else {
        value = One << (displayMode - 1);
    }
    //cout << "XipSetDisplayModeImplementation " << displayMode - 1 << " " << value << endl;
    XipSetDisplayMode(gHandler, value, colonne);
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipAddFlagDisplayImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jlong displayMode, jint colonne) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    XIPLONG value = One << (displayMode - 1);
    //cout << "XipAddFlagDisplayImplementation " << displayMode - 1 << " " << value << endl;
    XipAddFlagDisplayMode(gHandler, value, colonne);
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipRemoveFlagDisplayImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jlong displayMode) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    XIPLONG value = One << (displayMode - 1);
    //cout << "XipRemoveFlagDisplayImplementation " << displayMode - 1 << " " << value << endl;
    XipRemoveFlagDisplayMode(gHandler, value);
}

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_JXip_XipTestFlagDisplayImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jlong displayMode) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    XIPLONG value = One << (displayMode - 1);
    //cout << "XipTestFlagDisplayImplementation" << displayMode - 1 << " " << value << endl;
    char res = XipTestFlagDisplayMode(gHandler, value);
    if (res == 0) {
        return true;
    } else {
        return false;
    }
}

/*
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_setEnvImplementation(
        JNIEnv *env, jobject obj, jstring variableString) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    obj = NULL;
    //cout <<"JNICALL Java_com_xerox_jxip_JXip_setEnvImplementation variableString = "
    // <<variableString <<endl;
    string cvariableString = jstringToString(env, variableString);
    // copy the string to a static area
    try {
        static char v_str[512];
        int i;
        for (i = 0; cvariableString.c_str()[i] != 0; i++) {
            v_str[i] = cvariableString.c_str()[i];
        }
        v_str[i] = '\0';
        //cout <<"JNICALL Java_com_xerox_jxip_JXip_setEnvImplementation v_str = " <<v_str <<endl;
        if (putenv(v_str) != 0) {
            cerr << "JXip_setEnvImplementation: Problem occurred setting the variable "
                    << cvariableString << '\n';
        }
    } catch (...) {
        cerr << "JXip_setEnvImplementation Error" << endl;
        return;
    }
}

/*
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_getEnvImplementation(
        JNIEnv *env, jobject obj, jstring name) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        obj = NULL;
        string string_name = jstringToString(env, name);
        char * result = getenv(string_name.c_str());
        if (result != NULL) {
            jstring jstringResult = jstringFromChars(env, result);
            return jstringResult;
        } else {
            return NULL;
        }
    } catch (...) {
        cerr << "JXip_getEnvImplementation Error" << endl;
        return jstringFromChars(env, "");
    }
}

/**
 * Parse one text in a String.
 * @param gHandler a handler to a grammar
 * @param textInput the string to parse
 * @param xmlMode   XML_NONE=0, XML_OUTPUT=1, XML_INSERT=2
 * @param applyGrammar
 * @return the result as a String object.
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipParseStringOSImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jstring textInput, jchar xmlMode, jchar applyGrammar) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    jstring parsedResult;
    ostringstream *os;
    //cout <<"XipParseStringOSImplementation" <<endl;
    try {
        os = new ostringstream();
        obj = NULL;
        string textInputString = jstringToString(env, textInput);
        //cout <<"gHandler = " <<gHandler <<" xmlMode = " <<xmlMode
        // <<" apply = " <<applyGrammar <<endl;
        //cout <<"JXipImpl::XipParseStringOSImplementation() textInputString = "
        // <<textInputString <<endl;
        XipParseStringOS(textInputString, gHandler, os, xmlMode, applyGrammar);
        //cout <<"os = " <<os->str() <<endl;
        parsedResult = jstringFromString(env, os->str());
        delete os;
        //cout <<"Java_com_xerox_jxip_JXip_XipParseStringOSImplementation parsedResult = "
        // <<jstringToString(env,parsedResult) <<endl;
        return parsedResult;
    } catch (...) {
        delete os;
        cerr << "XipParseStringOSImplementation Error" << endl;
        return jstringFromChars(env, "");
    }
}

/**
 * Use a string structure as a seed to a parse
 * @param gHandler a handler to a grammar
 * @param textInput the string to parse
 * @param xmlMode   XML_NONE=0, XML_OUTPUT=1, XML_INSERT=2
 * @param applyGrammar
 * @return the result as a String object.
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipParseStringStructureImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jstring textInput, jchar xmlMode, jchar applyGrammar) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    jstring parsedResult;
    ostringstream *os;
    //cout <<"XipParseStringStructureImplementation" <<endl;
    try {
        os = new ostringstream();
        obj = NULL;
        string textInputString = jstringToString(env, textInput);
        //cout <<"gHandler = " <<gHandler <<" xmlMode = " <<xmlMode
        // <<" apply = " <<applyGrammar <<endl;
        //cout <<"JXipImpl::XipParseStringOSImplementation() textInputString = "
        // <<textInputString <<endl;
        XipParseStringStructure(textInputString, gHandler, os, xmlMode, applyGrammar);
        //cout <<"os = " <<os->str() <<endl;
        parsedResult = jstringFromString(env, os->str());
        delete os;
        //cout <<"Java_com_xerox_jxip_JXip_XipParseStringOSImplementation parsedResult = "
        // <<jstringToString(env,parsedResult) <<endl;
        return parsedResult;
    } catch (...) {
        delete os;
        cerr << "XipParseStringOSImplementation Error" << endl;
        return jstringFromChars(env, "");
    }
}

/**
 * Parse one text stored in a array of byte.
 * @param gHandler a handler to a grammar
 * @param bArray the data to parse (an array of byte)
 * @param xmlMode   XML_NONE=0, XML_OUTPUT=1, XML_INSERT=2
 * @param applyGrammar
 * @return the result as a String object.
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipParseBArrayOSImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jbyteArray bArray,
        jchar xmlMode, jchar applyGrammar) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    jstring parsedResult;
    ostringstream *os;
    //cout <<"XipParseBArrayOSImplementation" <<endl;
    try {
        obj = NULL;
        jboolean iscopy;
        //cout <<"JXipImpl::XipParseBArrayOSImplementation()  byteArray = " <<endl;
        jsize len = env->GetArrayLength(bArray);
        if (len == 0) {
            parsedResult = jstringFromChars(env, "");
        } else {
            os = new ostringstream();
            jbyte *data = env->GetByteArrayElements(bArray, &iscopy);
            string textInputString("");
            int ind;
            for (ind = 0; ind < len; ind++) {
                //printf("-%x",data[ind]);
                textInputString += data[ind];
            }
            //cout <<endl;
            env->ReleaseByteArrayElements(bArray, data, 0);
            XipParseStringOS(textInputString, gHandler, os, xmlMode, applyGrammar);
            //cout <<"os = " <<os->str() <<endl;
            parsedResult = jstringFromString(env, os->str());
            delete os;
            //cout <<"Java_com_xerox_jxip_JXip_XipParseStringOSImplementation parsedResult = "
            // <<jstringToString(env,parsedResult) <<endl;
            return parsedResult;
        }
    } catch (...) {
        delete os;
        cerr << "XipParseBArrayOSImplementation Error" << endl;
        return jstringFromChars(env, "");
    }
}

/**
 * Parse one text in a String.
 * @param gHandler   	a handler to a grammar.
 * @param textInput   	Text to parse.
 * @param depth
 * @param xmlMode         XML_NONE=0, XML_OUTPUT=1, XML_INSERT=2
 * @param applyGrammar
 * @return String containing the Xip processed XML
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_parseStringXmlOSImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jstring textInput, jint depth, jchar xmlMode, jchar applyGrammar) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    ostringstream *os;
    try {
        os = new ostringstream();
        obj = NULL;
        string textInputString = jstringToString(env, textInput);
        int result = XipParseStringXMLOS(textInputString, gHandler, os,
                depth, xmlMode, applyGrammar);
        jstring parsedResult;
        if (result) {
            cerr << "Error returned from XIP processing" << endl;
            parsedResult = jstringFromString(env, "");
        } else {
            parsedResult = jstringFromString(env, os->str());
        }
        delete os;
        //cout <<"Java_com_xerox_jxip_JXip_parseStringXmlOSImplementation parsedResult = "
        // <<jstringToString(env,parsedResult) <<endl;
        return parsedResult;
    } catch (...) {
        delete os;
        cerr << "parseStringXmlOSImplementation Error: no error type" << endl;
        return jstringFromChars(env, "");
    }
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_parseFileXmlOSImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jstring inputFile, jint depth, jchar xmlMode, jchar applyGrammar) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    ostringstream *os;
    try {
        os = new ostringstream();
        obj = NULL;
        string inputFileString = jstringToString(env, inputFile);
        int result = XipParseFileXMLOS(inputFileString, gHandler, os, depth,
                xmlMode, applyGrammar);
        //cout <<"XipParseFileXmlOSImplementation XipParseFileXMLOS return code = "
        // <<result <<endl;
        jstring parsedResult;
        if (result) {
            cerr << "Error returned from XIP processing" << endl;
            parsedResult = jstringFromString(env, "");
        } else {
            parsedResult = jstringFromString(env, os->str());
        }
        delete os;
        //cout <<"Java_com_xerox_jxip_XipParseFileXmlOSImplementation parsedResult = "
        //<<jstringToString(env,parsedResult) <<endl;
        return parsedResult;
    } catch (...) {
        delete os;
        cerr << "XipParseFileXmlOSImplementation Error" << endl;
        return jstringFromChars(env, "");
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipSetDependencyExtractionImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jchar ext) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    XipSetDependencyExtraction(gHandler, ext);
}

/**
 * Free the memory
 * @param gHandler   	a handler to a grammar.
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_xipFreeGrammarImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    //cout << "JXipImpl::xipFreeGrammarImplementation() gHandler = " <<gHandler <<endl;
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    XipFreeGrammar(gHandler);
}

/**
 * Free the memory
 * @param gHandler   	a handler to a grammar.
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_xipFreeAllGrammarsImplementation(
        JNIEnv *env, jobject obj) {
    //cout << "JXipImpl::xipFreeAllGrammarsImplementation()  <<endl;
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    XipFreeAllGrammars();
}

/**
 * parse a file the output is stored in a file
 * @param gHandler   	a handler to a grammar.
 * @return -1 if error otherwise 0 if OK
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipParseFileToFileImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring inputFile,
        jstring outputFile, jchar xmlMode, jchar applyGrammar) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        string outputFileString = jstringToString(env, outputFile);
        char *outputFileStr = (char*) (outputFileString).c_str();
        ofstream* ofs = new ofstream(outputFileStr);
        if (ofs == NULL) {
            return -1;
        }
        string inputFileString = jstringToString(env, inputFile);
        //cout << "JXipImpl::XipParseFileToFileImplementation() input: "
        // <<(char *)inputFileString.c_str() <<endl;
        //cout << "JXipImpl::XipParseFileToFileImplementation() output: "
        // <<outputFileStr <<endl;
        int res = XipParseFileToFile(inputFileString, gHandler, ofs,
                (char) xmlMode, (char) applyGrammar);
        ofs->close();
        delete ofs;
        return res;
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return -1;
    }
    checkXipErrorAndGenerateException(env, obj);
}

/**
 * parse a file the output is stored in a file
 * @param gHandler   	a handler to a grammar.
 * @return -1 if error otherwise 0 if OK
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipParseFileXMLToFileImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring inputFile,
        jstring outputFile, jint depth, jchar xmlMode, jchar applyGrammar) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        string outputFileString = jstringToString(env, outputFile);
        char *outputFileStr = (char*) (outputFileString).c_str();
        ofstream* ofs = new ofstream(outputFileStr);
        if (ofs == NULL) {
            return -1;
        }
        string inputFileString = jstringToString(env, inputFile);
        //cout << "JXipImpl::XipParseFileXMLToFileImplementation() input: "
        // <<(char *)inputFileString.c_str() <<endl;
        //cout << "JXipImpl::XipParseFileXMLToFileImplementation() output: "
        // <<outputFileStr <<endl;
        int res = XipParseFileXMLToFile(inputFileString, gHandler, ofs, depth,
                (char) xmlMode, (char) applyGrammar);
        ofs->close();
        delete ofs;
        return res;
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return -1;
    }
    checkXipErrorAndGenerateException(env, obj);
}

/**
 *
 * @param gHandler   	a handler to a grammar.
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipGetListCategoriesImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        vector<string> liste_categories;
        liste_categories.clear();
        XipGetListCategories(gHandler, liste_categories);
        // call the java call back routine JXip::addCategory()
        jmethodID addCategoryMID = getMethodID(env, obj, "addCategoryCB", "(Ljava/lang/String;)V");
        for (int i = 0; i < liste_categories.size(); i++) {
            string category = liste_categories[i];
            //cout <<"JXipImpl::XipXipGetListCategoriesImplementation() category = "
            // <<category <<endl;
            jstring categoryStr = jstringFromString(env, category);
            env->CallVoidMethod(obj, addCategoryMID, categoryStr);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, categoryStr);
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
    checkXipErrorAndGenerateException(env, obj);
}

/**
 *
 * @param gHandler   	a handler to a grammar.
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipGetListDependenciesImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        vector<string> liste_dependencies;
        liste_dependencies.clear();
        XipGetListDependencies(gHandler, liste_dependencies);
        // call the java call back routine JXip::addDependency()
        jmethodID addDependencyMID = getMethodID(env, obj, "addDependencyCB", "(Ljava/lang/String;)V");
        for (int i = 0; i < liste_dependencies.size(); i++) {
            string dependency = liste_dependencies[i];
            //cout <<"JXipImpl::XipGetListDependenciesImplementation() category = "
            // <<category <<endl;
            jstring dependencyStr = jstringFromString(env, dependency);
            env->CallVoidMethod(obj, addDependencyMID, dependencyStr);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, dependencyStr);
        }
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
    }
    checkXipErrorAndGenerateException(env, obj);
}

ostringstream *messErrorOS = NULL;

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_InitErrorMsgImplementation(
        JNIEnv *env, jobject obj) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    if (messErrorOS != NULL) {
        delete messErrorOS;
    }
    messErrorOS = new ostringstream();
    XipSetErrorStream(messErrorOS);
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_getErrorMsgImplementation(
        JNIEnv *env, jobject obj) {
    return jstringFromString(env, messErrorOS->str());
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipGetSourceCharsetImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        encodingType encodType = XipGetSourceCharset(gHandler);
        string encoding;
        switch (encodType) {
            case UTF8_ENCODING:
                encoding = "UTF8_ENCODING";
                break;
            case LATIN_ENCODING:
                encoding = "LATIN_ENCODING";
                break;
            default:
                encoding = "LATIN_ENCODING";
                break;
        }
        return jstringFromString(env, encoding);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return jstringFromString(env, "");
    }
    checkXipErrorAndGenerateException(env, obj);
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipGetGrammarCharsetImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        encodingType encodType = XipGetGrammarCharset(gHandler);
        string encoding;
        switch (encodType) {
            case UTF8_ENCODING:
                encoding = "UTF8_ENCODING";
                break;
            case LATIN_ENCODING:
                encoding = "LATIN_ENCODING";
                break;
            default:
                encoding = "LATIN_ENCODING";
                break;
        }
        return jstringFromString(env, encoding);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return jstringFromString(env, "");
    }
    checkXipErrorAndGenerateException(env, obj);
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_xipSetIndentFilePathnameImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jstring path) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        string filePath = jstringToString(env, path);
        return XipSetIndentFilePathname(gHandler, filePath);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return -1;
    }
    checkXipErrorAndGenerateException(env, obj);
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_xipSetWorkingFilesPathImplementation(
        JNIEnv *env, jobject obj,
        jint gHandler, jstring path) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    try {
        string filePath = jstringToString(env, path);
        return XipSetWorkingFilesPath(gHandler, filePath);
    } catch (...) {
        checkXipErrorAndGenerateException(env, obj);
        return -1;
    }
    checkXipErrorAndGenerateException(env, obj);
}

//----------------------------------------------------------------------------

#ifdef WINDOWS
#include <windows.h>
#include <psapi.h> // For PPROCESS_MEMORY_COUNTERS

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1
// build.xml:   <libset libs="psapi" if="windows" />

void printSysInfo() {
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;
    DWORD processID = GetCurrentProcessId();
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (hProcess == NULL) {
        return;
    }
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof (pmc))) {
        cout << "\tPageFaultCount: " << pmc.PageFaultCount << endl;
        cout << "\tPeakWorkingSetSize: " << pmc.PeakWorkingSetSize / 1000000 << "mb " << pmc.PeakWorkingSetSize % 1000000 << endl;
        cout << "\tWorkingSetSize: " << pmc.WorkingSetSize / 1000000 << "mb " << pmc.WorkingSetSize % 1000000 << endl;
        cout << "\tQuotaPeakPagedPoolUsage: " << pmc.QuotaPeakPagedPoolUsage << endl;
        cout << "\tQuotaPeakNonPagedPoolUsage: " << pmc.QuotaPeakNonPagedPoolUsage << endl;
        cout << "\tQuotaPagedPoolUsage: " << pmc.QuotaPagedPoolUsage << endl;
        cout << "\tQuotaNonPagedPoolUsage: " << pmc.QuotaNonPagedPoolUsage << endl;
        cout << "\tPagefileUsage: " << pmc.PagefileUsage / 1000000 << "mb " << pmc.PagefileUsage % 1000000 << endl;
        cout << "\tPeakPagefileUsage: " << pmc.PeakPagefileUsage / 1000000 << "mb " << pmc.PeakPagefileUsage % 1000000 << endl;
    }
    CloseHandle(hProcess);
}
#else

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
using std::ios_base;
using std::ifstream;
using std::string;

void printSysInfo() {
    ifstream stat_stream("/proc/self/stat", ios_base::in);
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;
    unsigned long vsize;
    long rss;
    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
            >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
            >> utime >> stime >> cutime >> cstime >> priority >> nice
            >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest
    stat_stream.close();
    //    cout << "vsize:Taille de la mémoire virtuelle en octets " << endl;
    //    cout << "rss: Taille de l'ensemble résident " << endl;
    //    cout << "\tnombre de pages dont le processus" << endl;
    //    cout << "\tdispose en mémoire réelle, moins 3 pour des raisons administratives. " << endl;
    //    cout << "\tIl s'agit juste des pages contenant les espaces de code, donnée et pile." << endl;
    //    cout << "\tCeci n'inclut ni les pages en attente de chargement ni celles qui ont été swappées: " << endl;
    //    cout << "userTime: Le temps passé en mode utilisateur: " << endl;
    //    cout << "sysTime: Le temps passé en mode noyau: " << endl;
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024;
    //cout << page_size_kb << endl;
    cout << "--- " << "vsize\t\trss\t\tuserTime\t\tsysTime" << endl;
    cout << "--- " << vsize / 1000000 << "." << vsize % 1000000 << "\t\t";
    cout << rss / 1000000 << "." << page_size_kb % 1000000 << "\t\t";
    cout << utime << "\t\t\t" << stime << endl; //take care: utime and stime are string
}
#endif

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_printSysInfoImplementation(
        JNIEnv *env, jobject obj) {
    printSysInfo();
}
//--------------------------------------------------------------------------

int main(int argc, const char* argv[]) {
    string grammarFile = "../../../grammar/english/basic/english.grm";
    ostringstream *os = new ostringstream();
    string textInputString("This is a test. Peter is happy.");
    int gHandler;
    try {
        gHandler = XipGrmFile(grammarFile, 0);
        cout << "JXipImpl::main() parsing the string :" << textInputString << endl;
        XipParseStringOS(textInputString, gHandler, os, 0, 1);
        cout << "JXipImpl::main() Result after parsing: \n" << os->str() << endl;
    } catch (...) {
        cerr << "main Error:" << endl;
        return -1;
    }
    return 0;
}
