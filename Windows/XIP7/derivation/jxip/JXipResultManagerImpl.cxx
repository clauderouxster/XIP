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

#include "com_xerox_jxip_XipResultManager.h"
#include "pasofficiel.h"
#include "JXipImpl.h"
#include "xipstlres.h"
#include "xipvecta.h"

#include "jni.h"

jmethodID createXipNodeMID = 0;
jmethodID createXipLeafMID = 0;
jmethodID createXipUnitMID = 0;
jmethodID createXipResultMID = 0;
jmethodID setMotherNodeMID = 0;
jmethodID setDaughterNodeMID = 0;
jmethodID createDependencyMID = 0;
jmethodID setParameterDependencyMID = 0;
jmethodID createDependencyFeatureMID = 0;
jmethodID createNodeFeatureMID = 0;
jmethodID sentenceInitMID = 0;
jmethodID xipResultInitMID = 0;
jmethodID setDependencyNodeMID = 0;
jmethodID setOutputMID = 0;
//allow to keep or destroy XipResult between sentence processing
int keep = 1;

#define INITVAL 0
long myIndex = INITVAL;
XipResult* xipResult = NULL;
VECTA<XipNode*> xipNodes;
jstring fileName;

/*
 * get all the method ID of each java callback method
 */
int getAllMethodId1(JNIEnv *env, jobject obj) {
    //cout <<"JXipResultManagerImpl::getAllMethodId1 " <<endl;
    jclass cls = env->GetObjectClass(obj);
    char* methodName;

    methodName = "createDependencyFeature";
    createDependencyFeatureMID = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;J)V");
    if (createDependencyFeatureMID == 0) {
        cout << methodName << " not found" << endl;
        return 1;
    }
    methodName = "createNodeFeature";
    createNodeFeatureMID = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;ZJ)V");
    if (createNodeFeatureMID == 0) {
        cout << methodName << " not found" << endl;
        return 1;
    }
    methodName = "createDependency";
    createDependencyMID = env->GetMethodID(cls, methodName, "(Ljava/lang/String;J)V");
    if (createDependencyMID == 0) {
        cout << methodName << " not found" << endl;
        return 1;
    }
    methodName = "setParameterDependency";
    setParameterDependencyMID = env->GetMethodID(cls, methodName, "(J)V");
    if (setParameterDependencyMID == 0) {
        cout << methodName << " not found" << endl;
        return 1;
    }
    methodName = "setMotherNode";
    setMotherNodeMID = env->GetMethodID(cls, methodName, "(JJ)V");
    if (setMotherNodeMID == 0) {
        cout << methodName << " not found" << endl;
        return 1;
    }
    methodName = "setDaughterNode";
    setDaughterNodeMID = env->GetMethodID(cls, methodName, "(JJ)V");
    if (setDaughterNodeMID == 0) {
        cout << methodName << " not found" << endl;
        return 1;
    }
    methodName = "createXipNode";
    createXipNodeMID = env->GetMethodID(cls, methodName, "(Ljava/lang/String;JJJJLjava/lang/String;JJ)V");
    if (createXipNodeMID == 0) {
        cout << methodName << " not found" << endl;
        return 1;
    }
    methodName = "createXipLeaf";
    createXipLeafMID = env->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JJJJLjava/lang/String;JJ)V");
    if (createXipLeafMID == 0) {
        cout << "callBack: " << methodName << " not found" << endl;
        return 1;
    }
    methodName = "createXipUnit";
    createXipUnitMID = env->GetMethodID(cls, methodName, "(JLjava/lang/String;)V");
    if (createXipUnitMID == 0) {
        cout << "callBack: " << methodName << " not found" << endl;
        return 1;
    }
    methodName = "createXipResult";
    createXipResultMID = env->GetMethodID(cls, methodName, "()V");
    if (createXipResultMID == 0) {
        cout << "callBack: " << methodName << " not found" << endl;
        return 1;
    }
    methodName = "sentenceInit";
    sentenceInitMID = env->GetMethodID(cls, methodName, "()V");
    if (sentenceInitMID == 0) {
        cout << "callBack: " << methodName << " not found" << endl;
        return 1;
    }
    methodName = "xipResultInit";
    xipResultInitMID = env->GetMethodID(cls, methodName, "()V");
    if (xipResultInitMID == 0) {
        cout << "callBack: " << methodName << " not found" << endl;
        return 1;
    }
    methodName = "setDependencyNode";
    setDependencyNodeMID = env->GetMethodID(cls, methodName, "(JJ)V");
    if (setDependencyNodeMID == 0) {
        cout << "callBack: " << methodName << " not found" << endl;
        return 1;
    }
    methodName = "setOutput";
    setOutputMID = env->GetMethodID(cls, methodName, "(Ljava/lang/String;)V");
    if (setOutputMID == 0) {
        cout << "callBack: " << methodName << " not found" << endl;
        return 1;
    }
    return 0;
}

void sentenceInit(JNIEnv *env, jobject obj) {
    env->CallVoidMethod(obj, sentenceInitMID);
}

void xipResultInit(JNIEnv *env, jobject obj) {
    env->CallVoidMethod(obj, xipResultInitMID);
}

/*
 * insert a index value for each C++ XipNode.
 * This index will be copied in each java XipNode.
 * It allows to keep a link between a c++ and java XipNode
 * add this node in a vector of XipNode (index is the index in this vector)
 * This function must create an index as in the method: RuleBuilding.cpp::NumberIndexNode()
 */
void setNodeIndex(XipNode* node) {
    myIndex++;
    //cout <<"JXipResultManagerImpl::setNodeIndex category = " <<node->category <<" index= " <<myIndex <<endl;
    node->index = myIndex;
    // insert node in the vector of node "xipNodes"
    xipNodes.affecte(myIndex, node);
    if (node->Type() != XIPLEAF) {
        for (int i = 0; i < node->daughters.size(); i++) {
            setNodeIndex(node->daughters[i]);
        }
    }
}

/*
 * create one java XipNode
 */
void createJavaXipNode(JNIEnv *env, jobject obj, XipNode* node) {
    //cout <<endl <<"JXipResultManagerImpl::createJavaXipNode " <<node->category <<endl;
    jboolean isLeaf;
    if (node->Type() == XIPLEAF) {
        isLeaf = true;
        XipLeaf* nodeL = (XipLeaf*) node;
        //cout <<"JXipResultManagerImpl::createJavaXipNode XipLeaf: " <<nodeL->category <<" " <<nodeL->index <<endl;
        jstring category = jstringFromString(env, nodeL->category);
        jstring surface = jstringFromString(env, nodeL->surface);
        jstring lemma = jstringFromString(env, nodeL->lemma);
        jstring initialfeatures = jstringFromString(env, nodeL->initialfeatures);
        jlong left = nodeL->left;
        jlong right = nodeL->right;
        jlong leftChar = node->leftchar;
        jlong rightChar = node->rightchar;
        jlong index = nodeL->index;
        jlong gramIdNode = nodeL->idnode;
        env->CallVoidMethod(obj, createXipLeafMID, category, surface, lemma, initialfeatures, left, right, index, gramIdNode, fileName, leftChar, rightChar);
    } else {
        isLeaf = false;
        //cout <<"JXipResultManagerImpl::createJavaXipNode XipNode: " <<node->category <<" " <<node->index
        //<<" " <<node->left <<" " <<node->right <<endl ;
        jstring category = jstringFromString(env, node->category);
        jlong left = node->left;
        jlong right = node->right;
        jlong leftChar = node->leftchar;
        jlong rightChar = node->rightchar;
        jlong index = node->index;
        jlong gramIdNode = node->idnode;
        env->CallVoidMethod(obj, createXipNodeMID, category, left, right, index, gramIdNode, fileName, leftChar, rightChar);
    }
    //for each feature
    for (int indF = 0; indF < node->features.size(); indF++) {
        XipFeature* xipFeature = node->features[indF];
        //cout <<"  feature: " <<xipFeature->attribute <<"=" <<xipFeature->value <<endl;
        jstring name = jstringFromString(env, xipFeature->attribute);
        jstring value = jstringFromString(env, xipFeature->value);
        jlong indexFeature = indF;
        env->CallVoidMethod(obj, createNodeFeatureMID, name, value, isLeaf, indexFeature);
    }
}

/*
 * create all java XipNodes from node
 */
void createJavaXipNodes(JNIEnv *env, jobject obj, XipNode* node) {
    //cout <<"JXipResultManagerImpl::createJavaXipNodes " <<endl;
    createJavaXipNode(env, obj, node);
    if (node->Type() != XIPLEAF) {
        for (int i = 0; i < node->daughters.size(); i++) {
            createJavaXipNodes(env, obj, node->daughters[i]);
        }
    }
}

/*
 * create all mother and daughter link for each each java XipNode from node
 */
void addMotherAndDaughtersToXipNode(JNIEnv *env, jobject obj, XipNode* node) {
    //cout <<"JXipResultManagerImpl::addMotherAndDaughtersToXipNode node=" <<node->category <<endl;
    //process this node
    //set the mother node
    jlong nodeIndex = node->index;
    jlong motherIndex = 0;
    if (node->mother != NULL) {
        motherIndex = (node->mother)->index;
        env->CallVoidMethod(obj, setMotherNodeMID, nodeIndex, motherIndex);
    }
    //set the daughter nodes
    for (int i = 0; i < node->daughters.size(); i++) {
        jlong daughterIndex = (node->daughters[i])->index;
        env->CallVoidMethod(obj, setDaughterNodeMID, nodeIndex, daughterIndex);
    }
    //Go to the next node
    if (node->Type() != XIPLEAF) {
        for (int i = 0; i < node->daughters.size(); i++) {
            addMotherAndDaughtersToXipNode(env, obj, node->daughters[i]);
        }
    }
}

/*
 * create java XipUnit
 */
void createJavaXipUnit(JNIEnv *env, jobject obj, XipUnit *sentence) {
    //cout <<"JXipResultManagerImpl::createJavaXipUnit " <<endl;
    jlong rootIndex = sentence->root->index;
    jstring language = jstringFromString(env, sentence->language);
    env->CallVoidMethod(obj, createXipUnitMID, rootIndex, language);
}

/*
 * create java XipResult
 */
void createJavaXipResult(JNIEnv *env, jobject obj) {
    //cout <<"JXipResultManagerImpl::createJavaXipResult " <<endl;
    env->CallVoidMethod(obj, createXipResultMID);
}

/*
 * create java XipDependency
 */
void createJavaDependencies(JNIEnv *env, jobject obj, VXipDependency dependencies) {
    //cout <<endl <<"JXipResultManagerImpl::createJavaDependencies"  <<endl;
    //for each dependency
    for (long index = 0; index < dependencies.size(); index++) {
        XipDependency* dependency = dependencies[index];
        //cout << endl <<"dependency name=" <<dependency->name <<endl;
        jstring name = jstringFromString(env, dependency->name);
        jlong depIndex = index;
        env->CallVoidMethod(obj, createDependencyMID, name, depIndex);
        //for each parameter
        for (int indP = 0; indP < dependency->parameters.size(); indP++) {
            XipNode* xipNode = dependency->parameters[indP];
            //cout <<"  node=" <<xipNode->category;
            jlong nodeIndex = xipNode->index;
            env->CallVoidMethod(obj, setParameterDependencyMID, nodeIndex);
            // add this dependency to the XipNode
            env->CallVoidMethod(obj, setDependencyNodeMID, nodeIndex, depIndex);
        }
        //for each feature
        for (int indF = 0; indF < dependency->features.size(); indF++) {
            XipFeature* xipFeature = dependency->features[indF];
            //cout <<endl <<"  feature: " <<xipFeature->attribute <<"=" <<xipFeature->value;
            jstring name = jstringFromString(env, xipFeature->attribute);
            jstring value = jstringFromString(env, xipFeature->value);
            jlong indexFeature = indF;
            env->CallVoidMethod(obj, createDependencyFeatureMID, name, value, indexFeature);
        }
    }
}


//-------------------------------------------------------------------------------------

int createXipResultImpl(int gHandler) {
    //cout <<"JXipResultManagerImpl::createXipResultImpl()" <<endl;
    try {
        xipResult = XipCreateCurrentXipResult(gHandler);
        if (xipResult != NULL) {
            XipCleanCurrentXipResult(gHandler);
            myIndex = INITVAL;
            xipNodes.raz();
        }
    } catch (XIPError* xipError) {
        cerr << "JXipResultManagerImpl::createXipResultImpl()" << xipError->message << endl;
        return 1;
    }
    return 0;
}

int initBeforeBuilding(JNIEnv *env, jobject obj, int gHandler) {
    //cout <<"JXipResultManagerImpl::initBeforeBuilding()" <<endl;
    //do not keep the XipResult between each sentence processing
    keep = 1;
    try {
        if (getAllMethodId1(env, obj) != 0) {
            return 1;
        }
        createXipResultImpl(gHandler);
    } catch (XIPError* xipError) {
        cerr << "JXipResultManagerImpl::initBeforeBuilding()" << xipError->message << endl;
        return 1;
    }
    return 0;
}

void createJavaXipResultAndOtherNode(JNIEnv *env, jobject obj, XipResult* xipResult, int gHandler) {
    //cout <<"JXipResultManagerImpl::createJavaXipResultAndOtherNode() sentence size = " <<xipResult->sentences.size() <<endl;
    xipResultInit(env, obj);
    for (int i = 0; i < xipResult->sentences.size(); i++) {
        //cout <<"JXipResultManagerImpl::createJavaXipResultAndOtherNode() sentence number = " <<i <<endl;
        sentenceInit(env, obj);
        XipUnit *sentence = xipResult->sentences[i];
        XipNode *root = sentence->root;
        setNodeIndex(root);
        createJavaXipNodes(env, obj, root);
        addMotherAndDaughtersToXipNode(env, obj, root);
        createJavaDependencies(env, obj, sentence->dependencies);
        createJavaXipUnit(env, obj, sentence);
    }
    createJavaXipResult(env, obj);
}


JNIEnv *env3;
jobject obj3;
jmethodID implCallBackMID = 0;
jmethodID implCallBackXmlMID = 0;

JNIEXPORT void JNICALL Java_com_xerox_jxip_XipResultManager_destroyNativeXipResultImplementation(JNIEnv *env, jobject obj, jint gHandler) {
    createXipResultImpl(gHandler);
}
//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_XipResultManager_keepNativeXipResultImplementation(JNIEnv *env, jobject obj) {
    keep = 0;
}

void implCallBack(int gHandler, XipResult *xipResult, void *unused) {
    //cout <<"-----------------------------------implCallBack" <<endl;
    //cout <<xipResult->sentences.size() <<endl;
    createJavaXipResultAndOtherNode(env3, obj3, xipResult, gHandler);
    env3->CallVoidMethod(obj3, implCallBackMID);
    //now clean the C++ xipResult(this is mandatory when using java callBack)
    if (keep != 0) {
        createXipResultImpl(gHandler);
    }
}

void implCallBackXml(int gHandler, XipResult *xipResult, void *unused) {
    //cout <<"-------------------------------implCallBackXml" <<endl;
    //cout <<xipResult->sentences.size() <<endl;
    try {
        //clear java tag info
        jclass cls = env3->GetObjectClass(obj3);
        char* methodName = "clearXmlTagInfoCB";
        jmethodID clearXmlTagInfoCB = env3->GetMethodID(cls, methodName, "()V");
        if (clearXmlTagInfoCB == 0) {
            cout << "callBack: " << methodName << " not found" << endl;
        }
        env3->CallVoidMethod(obj3, clearXmlTagInfoCB);
        // init setXmlTagInfoCB
        methodName = "setXmlTagInfoCB";
        jmethodID setXmlTagInfoCB = env3->GetMethodID(cls, methodName, "(Ljava/lang/String;Ljava/lang/String;)V");
        if (setXmlTagInfoCB == 0) {
            cout << "callBack: " << methodName << " not found" << endl;
        }
        // send tagName et tag attributes
        string tagname;
        vector<string> properties;
        char res = XipGetCurrentXMLNodeData(gHandler, tagname, properties);
        //cout <<"tagname = " <<tagname <<endl;
        env3->CallVoidMethod(obj3, setXmlTagInfoCB, jstringFromString(env3, "tagName"), jstringFromString(env3, tagname.c_str()));
        for (int ind = 0; ind < properties.size();) {
            string propertyName = properties[ind++];
            string propertyValue = properties[ind++];
            //cout <<propertyName <<" = " <<propertyValue <<endl;
            env3->CallVoidMethod(obj3, setXmlTagInfoCB, jstringFromString(env3, propertyName.c_str()), jstringFromString(env3, propertyValue.c_str()));
        }
        //
        createJavaXipResultAndOtherNode(env3, obj3, xipResult, gHandler);
        env3->CallVoidMethod(obj3, implCallBackXmlMID);
        //now clean the C++ xipResult(this is mandatory when using java callBack)
        createXipResultImpl(gHandler);
    } catch (XIPError* xipError) {
        cerr << xipError->message << endl;
        //generateException(env3, obj3);
    }
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_XipResultManager_xipResultBuilderImplementation(JNIEnv *env, jobject obj, jint inputT, jint gHandl, jstring param, jstring callBack, jstring callBackXml, jint xmlDepth) {
    //cout <<"JXipResultManagerImpl::parseWithCBImplementation() " <<endl;
    int gHandler = gHandl;
    int inputType = inputT;
    int res = 0;
    int depth = xmlDepth;
    env3 = env;
    obj3 = obj;
    char* methodName;
    try {
        jclass cls = env->GetObjectClass(obj);
        methodName = "jCallBack";
        implCallBackMID = env->GetMethodID(cls, methodName, "()V");
        if (implCallBackMID == 0) {
            cout << "callBack: " << methodName << " not found" << endl;
            return 1;
        }
        methodName = "jCallBackXml";
        implCallBackXmlMID = env->GetMethodID(cls, methodName, "()V");
        if (implCallBackXmlMID == 0) {
            cout << "callBack: " << methodName << " not found" << endl;
            return 1;
        }
        if (initBeforeBuilding(env, obj, gHandler) == 1) {
            return 1;
        }
        // activate callBack if required
        if (callBack != NULL) {
            XipSetCallBack(gHandler, implCallBack, NULL);
        }
        if (callBackXml != NULL) {
            XipSetCallBackXML(gHandler, implCallBackXml, NULL);
        }
        //cout <<"inputType = " <<inputType <<endl;
        if (inputType == 1) {
            // String input
            string inputStr = jstringToString(env, param);
            char xmlMode = 0;
            char applyGrammar = 1;
            ostringstream *os = new ostringstream();
            fileName = jstringFromString(env, "");
            res = XipParseStringAndOS(inputStr, gHandler, xipResult, os, xmlMode, applyGrammar);
            //cout << "JXipResultManagerImpl::parseWithCBImplementation() output: \n" <<os->str() <<endl;
            env->CallVoidMethod(obj, setOutputMID, jstringFromString(env, os->str()));
        } else if (inputType == 2) {
            // text file input
            string textFileName = jstringToString(env, param);
            fileName = jstringFromString(env, textFileName);
            char applyGrammar = 1;
            res = XipParseFile(textFileName, gHandler, xipResult, applyGrammar);
        } else if (inputType == 3) {
            // XML file input
            string xmlFileName = jstringToString(env, param);
            fileName = jstringFromString(env, xmlFileName);
            res = XipParseXMLFile(gHandler, xmlFileName, depth, xipResult);
        }
        // desactivate callBack if required
        if (callBack != NULL) {
            XipSetCallBack(gHandler, NULL, NULL);
        }
        if (callBackXml != NULL) {
            XipSetCallBackXML(gHandler, NULL, NULL);
        }
        // when there is no callBack create the java XipResult otherwise it has been created by the callBack
        if ((callBackXml == NULL) && (callBack == NULL)) {
            createJavaXipResultAndOtherNode(env, obj, xipResult, gHandler);
        }
    } catch (XIPError* xipError) {
        cerr << xipError->message << endl;
        return 1;
    }
    return res;
}
