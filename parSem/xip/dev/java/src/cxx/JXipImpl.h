/*
 *  Xerox Research Centre Europe - Grenoble Laboratory
 *  Copyright (C) 2004 - 2009 Xerox Corporation
 *  All Rights Reserved
 *
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

#include "jni.h"

#include "xipvecta.h"
#include "xipstlres.h"
#include "pasofficiel.h"

#include "generate.h"

#include "com_xerox_jxip_JXip.h"
#include "com_xerox_jxip_JXip2.h"
#include "com_xerox_jxip_RuleBuilder.h"
#include "com_xerox_jxip_RuleLoaded.h"
#include "com_xerox_jxip_RuleLoadedManager.h"
#include "com_xerox_jxip_XipResultManager.h"

string jstringToString(JNIEnv* env, jstring str);
jstring jstringFromChars(JNIEnv* env, const char *chars);
jstring jstringFromString(JNIEnv* env, string str);
void checkXipErrorAndGenerateException(JNIEnv *env, jobject obj);
void generateJavaException(JNIEnv *env, string message);
void checkIfJniExceptionOccurred(JNIEnv *env);
jmethodID getMethodID(JNIEnv *env, jobject obj, char *methodName, char *signature);
void deleteLocalJStringRef(JNIEnv* env, jstring jstr);
void deleteLocalJclassRef(JNIEnv* env, jclass jClasse);
//void freeMem(JNIEnv* env);
//void checkMem(JNIEnv* env);

#define TRUE 1
#define FALSE 0
