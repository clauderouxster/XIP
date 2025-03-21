/*
Copyright (c) 1999,2005 Xerox Corporation,
project XIP (Xerox Incremental Parser)
Xerox Research Centre Europe
All Rights Reserved

Copyright protection claimed includes all forms and matters of copyrightable
material and information now allowed by statutory or judicial law or
hereinafter granted, including without limitation, material generated from
the software programs which are displayed on the screen such as icons,
screen display looks, etc. All rights reserved. No part of this document
may be reproduced, stored in a retrieval system, or transmitted, in any
form or by any means, electronic, mechanical, photocopied, recorded, or
otherwise, without prior written consent from XEROX corporation.
*/

/**
* Native implementation for Java API to access the XIP C++ API.		
*/


//take care: do not add ".h" to the next includes (included in the std namespace)
#include <iostream>
#include <sstream>
#include <string>
//to access functions defined in the libstd
using namespace std;

#include "com_xerox_lg_LanguageGuesser.h"
#include <jni.h>


//to access C functions from c++ code
extern "C" {
#include "guess_lib.h"
}

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

JNIEXPORT jint JNICALL Java_com_xerox_lg_LanguageGuesser_initGuesserImplementation(JNIEnv *env, jobject obj){
	obj = NULL;
	//cout <<"initGuesserImplementation() " <<endl;
	int max_lang = 0;
	max_lang = init_guesser();
	return max_lang;
}

JNIEXPORT jstring JNICALL Java_com_xerox_lg_LanguageGuesser_guessLanguageImplementation(JNIEnv *env, jobject obj, jstring input
		, jstring langueType , jstring guessType){
	obj = NULL;
	string inputStr = jstringToString(env, input);
	string langueTypeStr = jstringToString(env, langueType);
	string guessTypeStr = jstringToString(env, guessType);
	//cout <<"guessLanguageImplementation() input = " <<inputStr <<" langueType = " <<langueTypeStr <<" guessType = " <<guessTypeStr <<endl;
	guesser_id_type    GuessType;
	if (strcmp((char*)guessTypeStr.c_str(), "trig") == 0) {
		GuessType =  trig;
	} else if (strcmp((char*)guessTypeStr.c_str(), "shortword") == 0) {
		GuessType = shortword;
	} else {
		GuessType = trig_shortword;
	}
	guesser_lang_type  LangType;
	if (strcmp((char*)langueTypeStr.c_str(), "iso639") == 0) {
		LangType  = guesser_iso639;
	} else if (strcmp((char*)langueTypeStr.c_str(), "iso639_2") == 0) {
		LangType  = guesser_iso639_2;
	} else {
		LangType  = guesser_full;
	}
	char res[20];
	guess_language((const unsigned char*)inputStr.c_str(), res,  LangType, GuessType);
	return jstringFromString(env, res);
}

