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

externkifmutex(jniprotect, _jniprotect);
void KiFInitialisationMutex();

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
jmethodID implCallBackMID = 0;
jmethodID implCallBackXmlMID = 0;
jmethodID clearXmlTagInfoCB = 0;
jmethodID setJSonStringCB = 0;
jmethodID setXmlTagInfoCB = 0;
//allow to keep or destroy XipResult between sentence processing
int keep = 1;

#define INITVAL 0L
long myIndex = INITVAL;
XipResult* xipResult = NULL;
VECTA<XipNode*> xipNodes;
jstring fileName = NULL;

/*
 * get all the method ID of each java callback method
 */
int getAllMethodId(JNIEnv *env, jobject obj) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //cout <<"JXipResultManagerImpl::getAllMethodId1 " <<endl;
    createDependencyFeatureMID = getMethodID(env, obj, "createDependencyFeatureCB",
            "(Ljava/lang/String;Ljava/lang/String;J)V");
    createNodeFeatureMID = getMethodID(env, obj, "createNodeFeatureCB",
            "(Ljava/lang/String;Ljava/lang/String;ZJ)V");
    createDependencyMID = getMethodID(env, obj, "createDependencyCB",
            "(Ljava/lang/String;J)V");
    setParameterDependencyMID = getMethodID(env, obj, "setParameterDependencyCB", "(J)V");
    setMotherNodeMID = getMethodID(env, obj, "setMotherNodeCB", "(JJ)V");
    setDaughterNodeMID = getMethodID(env, obj, "setDaughterNodeCB", "(JJ)V");
    createXipNodeMID = getMethodID(env, obj, "createXipNodeCB",
            "(Ljava/lang/String;Ljava/lang/String;JJJJLjava/lang/String;JJ)V");
    createXipLeafMID = getMethodID(env, obj, "createXipLeafCB",
            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JJJJLjava/lang/String;JJ)V");
    createXipUnitMID = getMethodID(env, obj, "createXipUnitCB", "(JLjava/lang/String;)V");
    createXipResultMID = getMethodID(env, obj, "createXipResultCB", "()V");
    sentenceInitMID = getMethodID(env, obj, "sentenceInitCB", "()V");
    xipResultInitMID = getMethodID(env, obj, "xipResultInitCB", "()V");
    setDependencyNodeMID = getMethodID(env, obj, "setDependencyNodeCB", "(JJ)V");
    setOutputMID = getMethodID(env, obj, "setOutputCB", "(Ljava/lang/String;)V");
    implCallBackMID = getMethodID(env, obj, "jCallBackCB", "()V");
    implCallBackXmlMID = getMethodID(env, obj, "jCallBackXmlCB", "()V");
    clearXmlTagInfoCB = getMethodID(env, obj, "clearXmlTagInfoCB", "()V");
	setJSonStringCB = getMethodID(env, obj, "setJSonStringCB","(Ljava/lang/String;)V");
    setXmlTagInfoCB = getMethodID(env, obj, "setXmlTagInfoCB","(Ljava/lang/String;Ljava/lang/String;)V");
    return 0;
}

void sentenceInit(JNIEnv *env, jobject obj) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    env->CallVoidMethod(obj, sentenceInitMID);
    checkIfJniExceptionOccurred(env);
}

void xipResultInit(JNIEnv *env, jobject obj) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    env->CallVoidMethod(obj, xipResultInitMID);
    checkIfJniExceptionOccurred(env);
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
    //cout <<"JXipResultManagerImpl::setNodeIndex category = "
    //<<node->category <<" index= " <<myIndex <<endl;
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
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //cout <<endl <<"JXipResultManagerImpl::createJavaXipNode " <<node->category <<endl;
    jboolean isLeaf;
    if (node->Type() == XIPLEAF) {
        isLeaf = true;
        XipLeaf* nodeL = (XipLeaf*) node;
        //cout <<"JXipResultManagerImpl::createJavaXipNode XipLeaf: "
        // <<nodeL->category <<" " <<nodeL->index <<endl;
        jstring category = jstringFromString(env, nodeL->category);
        jstring surface = jstringFromString(env, nodeL->surface);
        jstring lemma = jstringFromString(env, nodeL->lemma);
        jstring initialfeatures = jstringFromString(env, nodeL->initialfeatures);
		jstring infos = jstringFromString(env, node->infos);
        jlong left = nodeL->left;
        jlong right = nodeL->right;
        jlong leftChar = node->leftchar;
        jlong rightChar = node->rightchar;
        jlong index = nodeL->index;
        jlong gramIdNode = nodeL->idnode;
        env->CallVoidMethod(obj, createXipLeafMID, category, infos, surface,
                lemma, initialfeatures, left, right, index, gramIdNode, fileName,
                leftChar, rightChar);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, initialfeatures);
        deleteLocalJStringRef(env, lemma);
        deleteLocalJStringRef(env, surface);
        deleteLocalJStringRef(env, category);
		deleteLocalJStringRef(env, infos);
    } else {
        isLeaf = false;
        //cout <<"JXipResultManagerImpl::createJavaXipNode XipNode: "
        // <<node->category <<" " <<node->index
        //<<" " <<node->left <<" " <<node->right <<endl ;
        jstring category = jstringFromString(env, node->category);
		jstring infos = jstringFromString(env, node->infos);
        jlong left = node->left;
        jlong right = node->right;
        jlong leftChar = node->leftchar;
        jlong rightChar = node->rightchar;
        jlong index = node->index;
        jlong gramIdNode = node->idnode;
        env->CallVoidMethod(obj, createXipNodeMID, category, infos, left, right,
                index, gramIdNode, fileName, leftChar, rightChar);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, category);
		deleteLocalJStringRef(env, infos);
    }
    //for each feature
    for (int indF = 0; indF < node->features.size(); indF++) {
        XipFeature* xipFeature = node->features[indF];
        //cout <<"  feature: " <<xipFeature->attribute <<"=" <<xipFeature->value <<endl;
        jstring name = jstringFromString(env, xipFeature->attribute);
        jstring value = jstringFromString(env, xipFeature->value);
        jlong indexFeature = indF;
        env->CallVoidMethod(obj, createNodeFeatureMID, name, value, isLeaf, indexFeature);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, value);
        deleteLocalJStringRef(env, name);
    }
}

/*
 * create all java XipNodes from node
 */
void createJavaXipNodes(JNIEnv *env, jobject obj, XipNode* node) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
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
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //cout <<"JXipResultManagerImpl::addMotherAndDaughtersToXipNode node="
    // <<node->category <<endl;
    //process this node
    //set the mother node
    jlong nodeIndex = node->index;
    jlong motherIndex = 0;
    if (node->mother != NULL) {
        motherIndex = (node->mother)->index;
        env->CallVoidMethod(obj, setMotherNodeMID, nodeIndex, motherIndex);
        checkIfJniExceptionOccurred(env);
    }
    //set the daughter nodes
    for (int i = 0; i < node->daughters.size(); i++) {
        jlong daughterIndex = (node->daughters[i])->index;
        env->CallVoidMethod(obj, setDaughterNodeMID, nodeIndex, daughterIndex);
        checkIfJniExceptionOccurred(env);
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
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //cout <<"JXipResultManagerImpl::createJavaXipUnit " <<endl;
    jlong rootIndex = sentence->root->index;
    jstring language = jstringFromString(env, sentence->language);
    env->CallVoidMethod(obj, createXipUnitMID, rootIndex, language);
    checkIfJniExceptionOccurred(env);
    deleteLocalJStringRef(env, language);
}

/*
 * create java XipResult
 */
void createJavaXipResult(JNIEnv *env, jobject obj) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //cout <<"JXipResultManagerImpl::createJavaXipResult " <<endl;
    env->CallVoidMethod(obj, createXipResultMID);
    checkIfJniExceptionOccurred(env);
}

/*
 * create java XipDependency
 */
void createJavaDependencies(JNIEnv *env, jobject obj, VXipDependency dependencies) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //cout <<endl <<"JXipResultManagerImpl::createJavaDependencies"  <<endl;
    //for each dependency
    for (long index = 0; index < dependencies.size(); index++) {
        XipDependency* dependency = dependencies[index];
        //cout << endl <<"dependency name=" <<dependency->name <<endl;
        jstring name = jstringFromString(env, dependency->name);
        jlong depIndex = index;
        env->CallVoidMethod(obj, createDependencyMID, name, depIndex);
        checkIfJniExceptionOccurred(env);
        deleteLocalJStringRef(env, name);
        //for each parameter
        for (int indP = 0; indP < dependency->parameters.size(); indP++) {
            XipNode* xipNode = dependency->parameters[indP];
            //cout <<"  node=" <<xipNode->category;
            jlong nodeIndex = xipNode->index;
            env->CallVoidMethod(obj, setParameterDependencyMID, nodeIndex);
            checkIfJniExceptionOccurred(env);
            // add this dependency to the XipNode
            env->CallVoidMethod(obj, setDependencyNodeMID, nodeIndex, depIndex);
            checkIfJniExceptionOccurred(env);
        }
        //for each feature
        for (int indF = 0; indF < dependency->features.size(); indF++) {
            XipFeature* xipFeature = dependency->features[indF];
            //cout <<endl <<"  feature: " <<xipFeature->attribute <<"=" <<xipFeature->value;
            jstring name = jstringFromString(env, xipFeature->attribute);
            jstring value = jstringFromString(env, xipFeature->value);
            jlong indexFeature = indF;
            env->CallVoidMethod(obj, createDependencyFeatureMID, name, value, indexFeature);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, value);
            deleteLocalJStringRef(env, name);
        }
    }
}


//-------------------------------------------------------------------------------------

int createXipResultImpl(int gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
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
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    //cout <<"JXipResultManagerImpl::initBeforeBuilding()" <<endl;
    //do not keep the XipResult between each sentence processing
    keep = 1;
    try {
        if (getAllMethodId(env, obj) != 0) {
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
	KiFInitialisationMutex();
	KifThreadLock _lock(jniprotect);
	//cout <<"JXipResultManagerImpl::createJavaXipResultAndOtherNode() sentence size = "
	// <<xipResult->sentences.size() <<endl;
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


jstring createJavaJSonXipResult(JNIEnv *env, jobject obj,XipResult* xipResult, int gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    xipResultInit(env, obj);
	string jsonstr = xipResult->JSon();
	return jstringFromString(env, jsonstr.c_str());
}

JNIEnv *env3;
jobject obj3;

JNIEXPORT void JNICALL Java_com_xerox_jxip_XipResultManager_destroyNativeXipResultImplementation(
        JNIEnv *env, jobject obj, jint gHandler) {
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    createXipResultImpl(gHandler);
}

//----------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_XipResultManager_keepNativeXipResultImplementation(
        JNIEnv *env, jobject obj) {
    keep = 0;
}

//clock_t CBdurationTotal = 0;

void implCallBack(int gHandler, XipResult *xipResult, void *unused) {
    //cout <<"-----------------------------------implCallBack" <<endl;
    //cout <<xipResult->sentences.size() <<endl;
    //clock_t start = clock();
	char res = XipTestFlagDisplayMode(gHandler, DISPLAY_JSON_API);
	if (res == 1) {
		jstring jsonstr = createJavaJSonXipResult(env3, obj3, xipResult, gHandler);
		env3->CallVoidMethod(obj3, setJSonStringCB, jsonstr);
		deleteLocalJStringRef(env3, jsonstr);
	}
	else
		createJavaXipResultAndOtherNode(env3, obj3, xipResult, gHandler);

    env3->CallVoidMethod(obj3, implCallBackMID);
    checkIfJniExceptionOccurred(env3);
    //now clean the C++ xipResult(this is mandatory when using java callBack)
    if (keep != 0) {
        createXipResultImpl(gHandler);
    }
    //    clock_t duration = clock() - start;
    //    CBdurationTotal += duration;
    //    cout << "xipResultBuilderImplementation() duration: " << duration << " total: " << CBdurationTotal << endl;
    //    cout << "-----------------------------------implCallBack: freeMem" << endl;
    //    freeMem(env3);
    //checkMem(env3);
}

void implCallBackXml(int gHandler, XipResult *xipResult, void *unused) {
    //cout <<"-------------------------------implCallBackXml" <<endl;
    //cout <<xipResult->sentences.size() <<endl;
    try {
        //clock_t start = clock();
        //clear java tag info
        env3->CallVoidMethod(obj3, clearXmlTagInfoCB);
        checkIfJniExceptionOccurred(env3);
        // send tagName et tag attributes
        string tagname;
        vector<string> properties;
        char res = XipGetCurrentXMLNodeData(gHandler, tagname, properties);
        //cout <<"tagname = " <<tagname <<endl;
        jstring jstr1 = jstringFromString(env3, "tagName");
        jstring jstr2 = jstringFromString(env3, tagname.c_str());
        env3->CallVoidMethod(obj3, setXmlTagInfoCB, jstr1, jstr2);
        checkIfJniExceptionOccurred(env3);
        deleteLocalJStringRef(env3, jstr2);
        deleteLocalJStringRef(env3, jstr1);
        for (int ind = 0; ind < properties.size();) {
            string propertyName = properties[ind++];
            string propertyValue = properties[ind++];
            //cout <<propertyName <<" = " <<propertyValue <<endl;
            jstring jstr1 = jstringFromString(env3, propertyName.c_str());
            jstring jstr2 = jstringFromString(env3, propertyValue.c_str());
            env3->CallVoidMethod(obj3, setXmlTagInfoCB, jstr1, jstr2);
            checkIfJniExceptionOccurred(env3);
            deleteLocalJStringRef(env3, jstr2);
            deleteLocalJStringRef(env3, jstr1);
        }
        createJavaXipResultAndOtherNode(env3, obj3, xipResult, gHandler);
        env3->CallVoidMethod(obj3, implCallBackXmlMID);
        checkIfJniExceptionOccurred(env3);
        //now clean the C++ xipResult(this is mandatory when using java callBack)
        createXipResultImpl(gHandler);
        //        clock_t duration = clock() -start;
        //        CBdurationTotal += duration;
        //        cout << "xipResultBuilderImplementation() duration: " << duration <<" total: " <<CBdurationTotal <<endl;
    } catch (XIPError* xipError) {
        //        freeMem(env3);
        cerr << xipError->message << endl;
        //checkXipErrorAndGenerateException(env3, obj3);
    }
    //    freeMem(env3);
    //checkMem(env3);
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_XipResultManager_xipResultBuilderImplementation(
        JNIEnv *env, jobject obj, jint inputT, jint gHandl, jstring param,
        jstring callBack, jstring callBackXml, jint xmlDepth) {
    //cout << "JXipResultManagerImpl::xipResultBuilderImplementation() " << endl;
    //clock_t start = clock();
    KiFInitialisationMutex();
    KifThreadLock _lock(jniprotect);
    int gHandler = gHandl;
    int inputType = inputT;
    int res = 0;
    int depth = xmlDepth;
    env3 = env;
    obj3 = obj;
    try {
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
        deleteLocalJStringRef(env, fileName); //TODO  take care, fileName is a global
        if (inputType == 1) {
            // String input
            string inputStr = jstringToString(env, param);
            char xmlMode = 0;
            char applyGrammar = 1;
            ostringstream *os = new ostringstream();
            fileName = jstringFromString(env, "");
            res = XipParseStringAndOS(inputStr, gHandler, xipResult, os, xmlMode, applyGrammar);
            //cout << "JXipResultManagerImpl::parseWithCBImplementation() output: \n"
            // <<os->str() <<endl;
            jstring jstr = jstringFromString(env, os->str());
            env->CallVoidMethod(obj, setOutputMID, jstr);
            checkIfJniExceptionOccurred(env);
            deleteLocalJStringRef(env, jstr);
            //TODO            deleteLocalJStringRef(env, fileName);  take care: fileName is a global
        } else if (inputType == 2) {
            // text file input
            string textFileName = jstringToString(env, param);
            fileName = jstringFromString(env, textFileName);
            char applyGrammar = 1;
            res = XipParseFile(textFileName, gHandler, xipResult, applyGrammar);
            //TODO            deleteLocalJStringRef(env, fileName);  take care: fileName is a global
        } else if (inputType == 3) {
            // XML file input
            string xmlFileName = jstringToString(env, param);
            fileName = jstringFromString(env, xmlFileName);
            res = XipParseXMLFile(gHandler, xmlFileName, depth, xipResult);
            //TODO            deleteLocalJStringRef(env, fileName);  take care: fileName is a global
        }
        // desactivate callBack if required
        if (callBack != NULL) {
            XipSetCallBack(gHandler, NULL, NULL);
        }
        if (callBackXml != NULL) {
            XipSetCallBackXML(gHandler, NULL, NULL);
        }
        // when there is no callBack, create the java XipResult otherwise it
        // has been created by the callBack
        if ((callBackXml == NULL) && (callBack == NULL)) {
            createJavaXipResultAndOtherNode(env, obj, xipResult, gHandler);
        }
        //cout << "xipResultBuilderImplementation() duration: " << clock() - start << endl;
    } catch (XIPError* xipError) {
        //freeMem(env);
        cerr << xipError->message << endl;
        return 1;
    }
    //freeMem(env);
    return res;
}
