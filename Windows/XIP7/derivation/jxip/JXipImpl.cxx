/*
 * Copyright (c) 1999,2004 Xerox Corporation,
 * project XIP (Xerox Incremental Parser)
 * Xerox Research Centre Europe
 * All Rights Reserved
 *
 * Copyright protection claimed includes all forms and matters of copyrightable
 * material and information now allowed by statutory or judicial law or
 * hereinafter granted, including without limitation, material generated from
 * the software programs which are displayed on the screen such as icons,
 * screen display looks, etc. All rights reserved. No part of this document
 * may be reproduced, stored in a retrieval system, or transmitted, in any
 * form or by any means, electronic, mechanical, photocopied, recorded, or
 * otherwise, without prior written consent from XEROX corporation.
 *
 */

/**
 * Native implementation for Java API to access the XIP C++ API.
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#ifndef XIPLIBXML
#define XIPLIBXML
#endif

#include "com_xerox_jxip_JXip.h"
#include "xipstlres.h"
#include "pasofficiel.h"

#include "jni.h"

/**
 *  Converts the built-in Unicode representation of a java string into a UTF-8 c++ string
 * Take care if the java input string contains non ASCII characters, utf8 represents them with more than one byte.
 * if the input string contains only ASCII characters, the output UTF-8 string is the same than the input string.
 * @param str the java string
 * @return the utf8 c++ string
 */
string jstringToString(JNIEnv* env, jstring str) {
    jboolean iscopy;
    const char* strChars = env->GetStringUTFChars(str, &iscopy);
    string result = string(strChars);
    //cout <<"jstsringToString result = " <<result <<endl;
    // now the the JVM can free the memory used by jstring str
    env->ReleaseStringUTFChars(str, strChars);
    return result;
}

/**
 * converts a c++ string to a java string
 * @param the pointer to a utf8 c++ string
 * @return the java string
 *
 */
jstring jstringFromChars(JNIEnv* env, const char *chars) {
    return env->NewStringUTF(chars);
}

/**
 * converts a c++ string to a java string
 * @param the utf8 c++ string
 * @return the java string
 *
 */
jstring jstringFromString(JNIEnv* env, string str) {
    return jstringFromChars(env, str.c_str());
}

/**
 * The exception XIPError can't be catch: catch (XIPError *xipError) ????
 * (I do not kwnow why): so we use XipGetLastError() to get the error message,
 *   and generate a java Exception
 *
 **/
void generateException(JNIEnv *env, jobject obj){
    XIPError* xiperror = XipGetLastError();
    jclass newExceptionCls = env->FindClass( "com/xerox/jxip/XipException");
    if (newExceptionCls != 0)  {
        if (xiperror != 0)  {
            if (xiperror->message != "")  {
                env->ThrowNew(newExceptionCls, xiperror->message.c_str());
            } else {
                env->ThrowNew(newExceptionCls, "Exception: Internal Error");
            }
        } else {
            env->ThrowNew(newExceptionCls, "Exception: Internal Error");
        }
        env->DeleteLocalRef(newExceptionCls);
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_initXipExceptionImplementation(JNIEnv *env, jobject obj){
    XIPError* xiperror = XipGetLastError();
    jstring mess = jstringFromString(env, xiperror->message );
    jstring file = jstringFromString(env, xiperror->filename);
    jstring timeS = jstringFromString(env, xiperror->timestamp);
    jlong line = xiperror->line;
    jlong pos = xiperror->position;
    //cout <<"JXipImpl::initXipExceptionImplementation(): " <<mess <<endl;
    // Init mId with the callback method
    jclass cls = env->GetObjectClass( obj);
    char* methodName = "initXipExceptionImplementationCB";
    jmethodID mId = env->GetMethodID( cls, methodName, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JJ)V");
    if (mId == 0) {
        cout <<"JXipImpl2::getRulesLoadedImplementation  " <<methodName <<" not found" <<endl;
        exit(1);
    }
    // now call the callBack
    env->CallVoidMethod(obj, mId, mess, file, timeS, line, pos);
}




/**
 *  load grammar files using a grm file
 *
 * @param  grmFileName  the grm file name
 * @param  enableXml    when set to one, this parameter tells XIP that the input complies with the XIP XML DTD
 * @param  loadFst 	if true loads the FST otherwise parse only the grammar file without loading FST
 * @return              a handler for a specific grammar or Exception on error
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipGrmFileImplementation(JNIEnv *env, jobject obj,
        jstring grmFileName, jint enableXml, jboolean loadFst ){
    obj = NULL;
    int ipar;
    try {
        string grammarFile = jstringToString(env, grmFileName);
        //cout <<"Java_com_xerox_jxip_JXip_XipGrmFileImplementation  grmFile = " <<grammarFile <<" enableXml = " <<enableXml <<endl;
        char loadall;
        if (loadFst) {
            loadall = 1;
        } else {
            loadall = 0;
        }
        ipar = XipGrmFile(grammarFile, loadall, enableXml);
    } catch (...) {
        generateException(env, obj);
        return -1;
    }
    return ipar;
}

/**
 *  loading  grammars,  NTM and HMM ressources files.
 *
 * @param  grammarFileName  the grammar file name (.../english.xip)
 * @param  ntmFileName      the NTM script file name(.../ntmscript)
 * @param  hmmFileName      the HMM file name
 * @param  enableXml        When set to one , this parameter tells XIP that the input complies with the XIP XML DTD
 * @return                  a handler to the grammar.
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipLoadGrammarImplementation(JNIEnv *env, jobject obj,
        jstring grammarFileName, jstring ntmFileName, jstring hmmFileName, jint enableXml ) {
    obj = NULL;
    int ipar;
    try {
        string grammarFileNameStr = jstringToString(env, grammarFileName);
        string ntmFileNameStr = jstringToString(env, ntmFileName);
        string hmmFileNameStr = jstringToString(env, hmmFileName);
        //cout << "Java_com_xerox_jxip_JXip_loadGrammarImplementation grammarFileNameStr = " <<grammarFileNameStr;
        //cout <<" ntmFileNameStr = " <<ntmFileNameStr  <<" hmmFileNameStr = " <<hmmFileNameStr <<"\n";
        ipar = XipLoadGrammar(grammarFileNameStr, "",enableXml, ntmFileNameStr, hmmFileNameStr);
    } catch (...) {
        generateException(env, obj);
        return -1;
    }
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
 * @param  enableXml        When set to one , this parameter tells XIP that the input complies with the XIP XML DTD
 * @return                  a handler to the grammar. (or -1 if error)
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipGrammarLoadingImplementation(JNIEnv *env, jobject obj,
        jstring grammarFileName, jstring ntmFileName, jstring hmmFileName, jint enableXml ){
    obj = NULL;
    int gHandler;
    try {
        string grammarFileNameStr = jstringToString(env, grammarFileName);
        string ntmFileNameStr = jstringToString(env, ntmFileName);
        string hmmFileNameStr = jstringToString(env, hmmFileName);
        //cout << "Java_com_xerox_jxip_JXip_loadGrammarImplementation grammarFileNameStr = " <<grammarFileNameStr;
        //cout <<" ntmFileNameStr = " <<ntmFileNameStr  <<" hmmFileNameStr = " <<hmmFileNameStr <<"\n";
        gHandler = XipGrammarLoading(grammarFileNameStr, "",enableXml, ntmFileNameStr, hmmFileNameStr);
    } catch (...) {
        generateException(env, obj);
        return -1;
    }
    return (jint)gHandler;
}

/**
 *  Reload a grammar
 *
 * @param  gHandler  a handler to a grammar
 * @param  reloadParameter  if 0 then the parameter files are not reload else parameter files are reloaded
 * @param  grammarFileName  the grm file
 * @return      a handler to the grammar
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipReloadGrammarImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jchar reloadParameter, jstring grammarFileName){
    string grammarFileNameStr = jstringToString(env, grammarFileName);
    //cout <<"XipReloadGrammarImplementation  gHandler = " <<gHandler <<" reloadParameter = " <<reloadParameter <<" grammarFileNameStr = " <<grammarFileNameStr  <<endl;
    try {
        int newGHandler;
        if (reloadParameter == 0) {
            newGHandler = XipReloadGrammar(gHandler, reloadParameter, NULL);
        } else {
            newGHandler = XipReloadGrammar(gHandler, reloadParameter, (char*)grammarFileNameStr.c_str());
        }
        return newGHandler;
    } catch (...) {
        generateException(env, obj);
        return -1;
    }
}

/**
 *  set a value to a string variable
 *
 * @param  gHandler  a handler to a grammar
 * @param  name 		variable name
 * @param  value         variable value
 * @return           return -1 if error, 0 otherwise
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipSetStringVariableImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring name, jstring value){
    string variableName = jstringToString(env, name);
    string variableValue = jstringToString(env, value);
    //cout <<"XipSetStringVariableImplementation  variableName = " <<variableName <<" variableValue = " <<variableValue <<endl;
    try {
        char result = XipSetStringVariable(gHandler, variableName, variableValue);
        if (result == 1) {
            return 0;
        } else {
            return -1;
        }
    } catch (...) {
        cerr <<"XipSetStringVariableImplementation Error" <<endl;
        return -1;
    }
}

/**
 *  get the string value of a variable
 *
 * @param  gHandler  a handler to a grammar
 * @param  name 		variable name
 * @return           string value
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipGetStringVariableImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring name){
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
        cerr <<"XipGetStringVariableImplementation Error" <<endl;
        return jstringFromString(env, "");
    }
}


/**
 *  get the string value of a variable
 *
 * @param  gHandler  a handler to a grammar
 * @param  name 		variable name
 * @return           string value
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipSetIntVariableImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring name, jfloat value){
    string variableName = jstringToString(env, name);
    float variableValue = (float) value;
    //cout <<"XipSetIntVariableImplementation  variableName = " <<variableName <<"  value = " <<variableValue <<endl;
    try {
        char result = XipSetIntVariable(gHandler, variableName, variableValue);
        if (result == 1) {
            return 0;
        } else {
            return -1;
        }
    } catch (...) {
        cerr <<"XipSetIntVariableImplementation Error" <<endl;
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
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipLoadXMLDataBaseImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring fileNam, jstring ali){
    string fileName = jstringToString(env, fileNam);
    string alias = jstringToString(env, ali);
    //cout <<"XipLoadXMLDataBaseImplementation gHandler = " <<gHandler <<" fileName = " << fileName <<" alias = " <<alias <<endl;
    try {
        int result = XipLoadXMLDataBase(gHandler, fileName, alias);
        if (result == 0) {
            return 0;
        } else {
            return -1;
        }
    } catch (...) {
        cerr <<"XipLoadXMLDataBaseImplementation Error" <<endl;
        return -1;
    }
}

/**
 *  get a list of aliases (provided by a parameter file)
 *
 * @param  gHandler  a handler to a grammar
 * @return           return -1 if error, 0 otherwise
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipAliasListImplementation(JNIEnv *env, jobject obj,
        jint gHandler){
    //cout <<"XipAliasListImplementation " <<endl;
    vector<string> aliases;
    try {
        XipAliasList(gHandler, aliases);
        //cout <<"XipAliasListImplementation aliases.size = " <<aliases.size() <<endl;
        jclass cls = env->GetObjectClass( obj);
        char* methodName = "addAliasInList";
        jmethodID methodID = env->GetMethodID( cls, methodName, "(Ljava/lang/String;)V");
        if (methodID == 0) {
            cerr <<methodName <<" not found" <<endl;
            return 1;
        }
        for (int index = 0; index < aliases.size(); index++) {
            jstring alias = jstringFromString(env, aliases[index]);
            //cout <<"XipAliasListImplementation alias = " <<aliases[index] <<endl;
            env->CallVoidMethod(obj, methodID, alias);
        }
        return 0;
    } catch (...) {
        cerr <<"XipAliasListImplementation Error" <<endl;
        return -1;
    }
}

/**
 *  load a parameter file
 *
 * @param  gHandler  a handler to a grammar
 * @param  fileName  a parameter file name
 * @return           return -1 if error, 0 otherwise
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipParameterFileImplementation(JNIEnv *env,
        jobject obj, jint gHandler, jstring configFileName ){
    obj = NULL;
    try {
        string configFileNameStr = jstringToString(env, configFileName);
        //cout <<"XipParameterFileImplementation gHandler = " <<gHandler <<" configFileName = " <<configFileNameStr <<endl;
        int result =  XipParameterFile(gHandler, configFileNameStr);
        if (result == 0) {
            return 0;
        } else {
            return -1;
        }
    } catch (...) {
        cerr <<"XipParameterFileImplementation Error" <<endl;
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
        jobject obj, jint gHandler, jstring alias ){
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
        cerr <<"XipCleanXMLAliasImplementation Error" <<endl;
        return -1;
    }
}

/** *
 * Get version about the current XIP C++ Library used.
 * @return String	     Version number.
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_getVersionImplementation(JNIEnv *env, jobject obj ){
    obj = NULL;
    string question;
    try {
        Whoami(question);
        jstring result = jstringFromString(env, question);
        return result;
    } catch (...) {
        cerr <<"getVersionImplementation Error" <<endl;
        return jstringFromChars(env, "");
    }
}


/**
 * Get information about the current XIP grammar license
 *   (loaded in a specific XIP Session).
 * @return int	Days remaining before license ending.
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipLicenseImplementation(JNIEnv *env, jobject obj, jint gHandler  ) {
    env = NULL;
    obj = NULL;
    try {
        int errCode = XipLicense(gHandler );
        return errCode;
    } catch (...) {
        cerr <<"XipLicenseImplementation Error" <<endl;
        return -1;
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipSetDisplayModeImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jlong displayMode, jint colonne) {
    XipSetDisplayMode(gHandler, (unsigned long) displayMode, colonne);
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipAddFlagDisplayImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jlong displayMode, jint colonne) {
    //cout <<"XipAddFlagDisplayImplementation" <<endl;
    XipAddFlagDisplayMode(gHandler, (unsigned long) displayMode, colonne);
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipRemoveFlagDisplayImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jlong displayMode) {
    //cout <<"XipRemoveFlagDisplayImplementation" <<endl;
    XipRemoveFlagDisplayMode(gHandler, (unsigned long) displayMode);
}

JNIEXPORT jboolean JNICALL Java_com_xerox_jxip_JXip_XipTestFlagDisplayImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jlong displayMode) {
    //cout <<"XipTestFlagDisplayImplementation" <<endl;
    char res = XipTestFlagDisplayMode(gHandler, (unsigned long) displayMode );
    if (res == -1) {
        cout <<"XipTestFlagDisplayImplementation ERROR" <<endl;
        //throw new XipException("XipTestFlagDisplayImplementation ERROR");
    } else {
        if (res == 0) {
            return 0;
        } else {
            return 1;
        }
    }
}

/*
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_setEnvImplementation(JNIEnv *env, jobject obj, jstring variableString) {
    obj = NULL;
    //cout <<"JNICALL Java_com_xerox_jxip_JXip_setEnvImplementation variableString = " <<variableString <<endl;
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
            cerr << "JXip_setEnvImplementation: Problem occurred setting the variable " << cvariableString << '\n';
        }
    } catch (...) {
        cerr <<"JXip_setEnvImplementation Error" <<endl;
        return;
    }
}

/*
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_getEnvImplementation(JNIEnv *env, jobject obj, jstring name) {
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
        cerr <<"JXip_getEnvImplementation Error" <<endl;
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
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipParseStringOSImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring textInput, jchar xmlMode, jchar applyGrammar) {
    jstring parsedResult;
    ostringstream *os;
    //cout <<"XipParseStringOSImplementation" <<endl;
    try {
        os = new ostringstream();
        obj = NULL;
        string textInputString = jstringToString(env, textInput);
        //cout <<"gHandler = " <<gHandler <<" xmlMode = " <<xmlMode <<" apply = " <<applyGrammar <<endl;
        //cout <<"JXipImpl::XipParseStringOSImplementation() textInputString = " <<textInputString <<endl;
        XipParseStringOS(textInputString, gHandler, os, xmlMode, applyGrammar);
        //cout <<"os = " <<os->str() <<endl;
        parsedResult = jstringFromString(env, os->str());
        delete os;
        //cout <<"Java_com_xerox_jxip_JXip_XipParseStringOSImplementation parsedResult = " <<jstringToString(env,parsedResult) <<endl;
        return parsedResult;
    } catch (...) {
        delete os;
        cerr <<"XipParseStringOSImplementation Error" <<endl;
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
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipParseBArrayOSImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jbyteArray bArray, jchar xmlMode, jchar applyGrammar) {
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
            jbyte *data = env->GetByteArrayElements( bArray, &iscopy);
            string textInputString("");
            int ind;
            for (ind=0; ind < len ; ind++) {
                //printf("-%x",data[ind]);
                textInputString += data[ind];
            }
            //cout <<endl;
            env->ReleaseByteArrayElements( bArray, data, 0);
            XipParseStringOS(textInputString, gHandler, os, xmlMode, applyGrammar);
            //cout <<"os = " <<os->str() <<endl;
            parsedResult = jstringFromString(env, os->str());
            delete os;
            //cout <<"Java_com_xerox_jxip_JXip_XipParseStringOSImplementation parsedResult = " <<jstringToString(env,parsedResult) <<endl;
            return parsedResult;
        }
    } catch (...) {
        delete os;
        cerr <<"XipParseBArrayOSImplementation Error" <<endl;
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
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_parseStringXmlOSImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring textInput, jint depth, jchar xmlMode, jchar applyGrammar) {
    ostringstream *os;
    try {
        os = new ostringstream();
        obj = NULL;
        string textInputString = jstringToString(env, textInput);
        int result = XipParseStringXMLOS(textInputString, gHandler, os, depth, xmlMode, applyGrammar);
        jstring parsedResult;
        if (result) {
            cerr << "Error returned from XIP processing" << endl;
            parsedResult = jstringFromString(env, "");
        } else {
            parsedResult = jstringFromString(env, os->str());
        }
        delete os;
        //cout <<"Java_com_xerox_jxip_JXip_parseStringXmlOSImplementation parsedResult = " <<jstringToString(env,parsedResult) <<endl;
        return parsedResult;
    } catch (...) {
        delete os;
        cerr <<"parseStringXmlOSImplementation Error: no error type" <<endl;
        return jstringFromChars(env, "");
    }
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_parseFileXmlOSImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring inputFile, jint depth, jchar xmlMode, jchar applyGrammar) {
    ostringstream *os;
    try {
        os = new ostringstream();
        obj = NULL;
        string inputFileString = jstringToString(env, inputFile);
        int result = XipParseFileXMLOS(inputFileString, gHandler, os, depth, xmlMode, applyGrammar);
        //cout <<"XipParseFileXmlOSImplementation XipParseFileXMLOS return code = " <<result <<endl;
        jstring parsedResult;
        if (result) {
            cerr << "Error returned from XIP processing" << endl;
            parsedResult = jstringFromString(env, "");
        } else {
            parsedResult = jstringFromString(env, os->str());
        }
        delete os;
        //cout <<"Java_com_xerox_jxip_XipParseFileXmlOSImplementation parsedResult = " <<jstringToString(env,parsedResult) <<endl;
        return parsedResult;
    } catch (...) {
        delete os;
        cerr <<"XipParseFileXmlOSImplementation Error" <<endl;
        return jstringFromChars(env, "");
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipSetDependencyExtractionImplementation(JNIEnv *env, jobject obj, jint gHandler, jchar ext) {
    XipSetDependencyExtraction(gHandler, ext);
}


/**
 * Free the memory
 * @param gHandler   	a handler to a grammar.
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_xipFreeGrammarImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    //cout << "JXipImpl::xipFreeGrammarImplementation() gHandler = " <<gHandler <<endl;
    XipFreeGrammar(gHandler);
}

/**
 * parse a file the output is stored in a file
 * @param gHandler   	a handler to a grammar.
 * @return -1 if error otherwise 0 if OK
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipParseFileToFileImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring inputFile, jstring outputFile, jchar xmlMode, jchar applyGrammar) {
    try {
        string outputFileString = jstringToString(env, outputFile);
        char *outputFileStr = (char*)(outputFileString).c_str();
        ofstream* ofs = new ofstream(outputFileStr);
        if (ofs == NULL) {
            return -1;
        }
        string inputFileString = jstringToString(env, inputFile);
        //cout << "JXipImpl::XipParseFileToFileImplementation() input: " <<(char *)inputFileString.c_str() <<endl;
        //cout << "JXipImpl::XipParseFileToFileImplementation() output: " <<outputFileStr <<endl;
        int res = XipParseFileToFile(inputFileString, gHandler, ofs, (char)xmlMode, (char)applyGrammar);
        ofs->close();
        delete ofs;
        return res;
    } catch (...) {
        generateException(env, obj);
        return -1;
    }
}

/**
 * parse a file the output is stored in a file
 * @param gHandler   	a handler to a grammar.
 * @return -1 if error otherwise 0 if OK
 */
JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_XipParseFileXMLToFileImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring inputFile, jstring outputFile, jint depth, jchar xmlMode, jchar applyGrammar) {
    try {
        string outputFileString = jstringToString(env, outputFile);
        char *outputFileStr = (char*)(outputFileString).c_str();
        ofstream* ofs = new ofstream(outputFileStr);
        if (ofs == NULL) {
            return -1;
        }
        string inputFileString = jstringToString(env, inputFile);
        //cout << "JXipImpl::XipParseFileXMLToFileImplementation() input: " <<(char *)inputFileString.c_str() <<endl;
        //cout << "JXipImpl::XipParseFileXMLToFileImplementation() output: " <<outputFileStr <<endl;
        int res = XipParseFileXMLToFile(inputFileString, gHandler, ofs, depth, (char)xmlMode, (char)applyGrammar);
        ofs->close();
        delete ofs;
        return res;
    } catch (...) {
        generateException(env, obj);
        return -1;
    }
}

/**
 *
 * @param gHandler   	a handler to a grammar.
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipGetListCategoriesImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        vector<string> liste_categories;
        liste_categories.clear();
        XipGetListCategories(gHandler, liste_categories);
        // call the java call back routine JXip::addCategory()
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "addCategory";
        jmethodID addCategoryMID = env->GetMethodID(cls, methodName, "(Ljava/lang/String;)V");
        if (addCategoryMID == 0) {
            cerr <<methodName <<" not found" <<endl;
        }
        for(int i = 0; i < liste_categories.size(); i++) {
            string category = liste_categories[i];
            //cout <<"JXipImpl::XipXipGetListCategoriesImplementation() category = " <<category <<endl;
            jstring categoryStr = jstringFromString(env, category);
            env->CallVoidMethod(obj, addCategoryMID, categoryStr);
        }
    } catch (...) {
        generateException(env, obj);
    }
}

/**
 *
 * @param gHandler   	a handler to a grammar.
 */
JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_XipGetListDependenciesImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    try {
        vector<string> liste_dependencies;
        liste_dependencies.clear();
        XipGetListDependencies(gHandler, liste_dependencies);
        // call the java call back routine JXip::addCategory()
        jclass cls = env->GetObjectClass(obj);
        char* methodName = "addDependency";
        jmethodID addDependencyMID = env->GetMethodID(cls, methodName, "(Ljava/lang/String;)V");
        if (addDependencyMID == 0) {
            cerr <<methodName <<" not found" <<endl;
        }
        for(int i = 0; i < liste_dependencies.size(); i++) {
            string dependency = liste_dependencies[i];
            //cout <<"JXipImpl::XipGetListDependenciesImplementation() category = " <<category <<endl;
            jstring dependencyStr = jstringFromString(env, dependency);
            env->CallVoidMethod(obj, addDependencyMID, dependencyStr);
        }
    } catch (...) {
        generateException(env, obj);
    }
}

ostringstream *messErrorOS = NULL;

JNIEXPORT void JNICALL Java_com_xerox_jxip_JXip_InitErrorMsgImplementation(JNIEnv *env, jobject obj ) {
    if (messErrorOS != NULL) {
        delete messErrorOS;
    }
    messErrorOS = new ostringstream();
    XipSetErrorStream(messErrorOS);
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_getErrorMsgImplementation(JNIEnv *env, jobject obj ) {
    return jstringFromString(env, messErrorOS->str());
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipGetSourceCharsetImplementation(JNIEnv *env, jobject obj, jint gHandler ) {
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
        generateException(env, obj);
        return jstringFromString(env, "");
    }
}

JNIEXPORT jstring JNICALL Java_com_xerox_jxip_JXip_XipGetGrammarCharsetImplementation(JNIEnv *env, jobject obj, jint gHandler ) {
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
        generateException(env, obj);
        return jstringFromString(env, "");
    }
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_xipSetIndentFilePathnameImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring path ) {
    try {
        string filePath = jstringToString(env, path);
        return XipSetIndentFilePathname(gHandler, filePath);
    } catch (...) {
        generateException(env, obj);
        return -1;
    }
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_JXip_xipSetWorkingFilesPathImplementation(JNIEnv *env, jobject obj,
        jint gHandler, jstring path ) {
    try {
        string filePath = jstringToString(env, path);
        return XipSetWorkingFilesPath(gHandler, filePath);
    } catch (...) {
        generateException(env, obj);
        return -1;
    }
}

/*
int main(int argc, const char* argv[]) {
    string grammarFile = "../../../grammar/english/basic/english.grm";
    ostringstream *os = new ostringstream();
    string textInputString("This is a test. Peter is happy.");
    int gHandler;
    try {
        gHandler = XipGrmFile(grammarFile, 0);
        cout << "JXipImpl::main() parsing the string :" <<textInputString  <<endl;
        XipParseStringOS(textInputString, gHandler, os, 0, 1);
        cout << "JXipImpl::main() Result after parsing: \n" <<os->str() <<endl;
    } catch (...) {
        cerr <<"main Error:" <<endl;
        return -1;
    }
    return 0;
}
*/