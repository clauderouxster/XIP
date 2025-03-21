/*
 *  Xerox Research Centre Europe - Grenoble Laboratory
 *  Copyright (C) 2004 - 2009 Xerox Corporation
 *  All Rights Reserved
 *
 */

/**
 * Native implementation to generate xip rule.
 */
#include "JXipImpl.h"
#include "xipstlres.h"

void checkXipErrorAndGenerateException(JNIEnv *env, jobject obj);

extern XipResult* xipResult;
extern VECTA<XipNode*> xipNodes;

VECTA<SelectedNode*> selectedNodes;
VECTA<SelectedDependency*> selectedDependencies;
//Vector of node index used for the focus
VECTA<int> focus;
SelectedNode* selectedNode;
SelectedDependency* selectedDependency;
vector<string> categoryDependencies;

/*
 * this method is called by RuleBuilder::generate() java method to generate the RuleBuilder.
 *
 */
JNIEXPORT jstring JNICALL Java_com_xerox_jxip_RuleBuilder_buildRuleImplementation(
        JNIEnv *env, jobject obj, jlong boundaryIndex, jstring categor,
        jint ruleType, jint dependencyArity) {
    //cout <<"JXipRuleBuilderImpl::buildRuleImplementation() boundaryIndex = " <<boundaryIndex ;
    //cout << " category = " <<jstringToString(env, categor) <<" ruleType = " <<ruleType <<endl;
    jstring script;
    try {
        XipUnit* xipUnit = xipResult->sentences[0];
        RuleGeneration* ruleGeneration = new RuleGeneration(xipUnit);
        //init ruleGeneration
        ruleGeneration->numvar.raz();
        ruleGeneration->num_var = 1;
        for (int ind = 0; ind < xipNodes.dernier; ind++) {
            ruleGeneration->numvar.ajoute(-1);
        }
        for (int ind1 = 0; ind1 < focus.dernier; ind1++) {
            ruleGeneration->numvar.affecte(focus[ind1], ruleGeneration->num_var++);
        }

        XipNode* boundary;
        if (boundaryIndex == -1) {
            // set boundary to NULL when there is no left context
            boundary = NULL;
        } else {
            boundary = xipNodes[(int) boundaryIndex];
            //cout <<"JXipRuleBuilderImpl::buildRuleImplementation() boundary->category = "
            // <<boundary->category <<endl;
        }
        string category = jstringToString(env, categor);
        char typeRule = (char) ruleType;
        string ruleString = ""; // the script rule generated
        char res = ruleGeneration->GenerateRule(focus, ruleString, selectedNodes,
                selectedDependencies, boundary, category, dependencyArity, typeRule);
        script = jstringFromString(env, ruleString);
        // call the java call back routine RuleBuilder::addCategoryDependency()
        jmethodID addCategoryDependencyMID = getMethodID(env, obj, "addCategoryDependency",
                "(Ljava/lang/String;)V");
        vector<string> catDep = ruleGeneration->categorydependency;
        int catDepNb = catDep.size();
        for (int i = 0; i < catDepNb; i++) {
            string catDep = ruleGeneration->categorydependency[i];
            //cout <<"JXipRuleBuilderImpl::buildRuleImplementation() catDep = " <<catDep <<endl;
            jstring catDepStr = jstringFromString(env, catDep);
            env->CallVoidMethod(obj, addCategoryDependencyMID, catDepStr);
            checkIfJniExceptionOccurred(env);
        }
        delete ruleGeneration;
        //cout <<"JXipRuleBuilderImpl::buildRuleImplementation() ruleString = "
        // <<ruleString <<endl;
        if (res != 0) {
            generateJavaException(env, ruleString);
        }
    } catch (...) {
        //cerr <<"XipGetListDependenciesImplementation Error" <<endl;
        checkXipErrorAndGenerateException(env, obj);
    }
    return script;
}

JNIEXPORT void JNICALL
Java_com_xerox_jxip_RuleBuilder_addSelectedFeatureToSelectedNodeImplementation(
        JNIEnv *env, jobject obj, jlong indSelectNode, jlong indSelectFeature) {
    long indexSelectedNode = (long) indSelectNode;
    long indexSelectedFeature = (long) indSelectFeature;
    //cout <<"JXipRuleBuilderImpl::addSelectedFeatureToSelectedNodeImplementation() indSelectNode = "
    // <<indSelectNode <<" indSelectFeature = " <<indSelectFeature <<endl;
    XipNode* xipNode = xipNodes[(int) indexSelectedNode];
    //cout <<"JXipRuleBuilderImpl::addSelectedFeatureToSelectedNodeImplementation() XipNode category = "
    // <<xipNode->category <<endl;
    XipFeature* xipFeature = xipNode->features[indexSelectedFeature];
    string featureStr = xipFeature->attribute + ":" + xipFeature->value;
    //cout <<"JXipRuleBuilderImpl::addSelectedFeatureToSelectedNodeImplementation() featureStr = "
    // <<featureStr <<endl;
    // selectedNode is a global and has been initialized by addSelectedNode
    vector<string> features = selectedNode->features;
    features.push_back(featureStr);
}

//rajoute par Vianney et Claude le 21/06/05

JNIEXPORT void JNICALL
Java_com_xerox_jxip_RuleBuilder_addSelectedFeatureStringToSelectedNodeImplementation(
        JNIEnv *env, jobject obj, jstring featureAttribute, jstring featureValue) {
    string attribute = jstringToString(env, featureAttribute);
    string value = jstringToString(env, featureValue);
    string featureStr = attribute + ":" + value;
    vector<string> features = selectedNode->features;
    features.push_back(featureStr);
}

void addSelectedNode(long index, char any) {
    //cout <<"JXipRuleBuilderImpl::addSelectedNode() xipNodes.taille = " <<xipNodes.taille <<endl;
    // get the c++ node using the index
    XipNode* xipNode = xipNodes[(int) index];
    if (xipNode == NULL) {
        //cout << "JXipRuleBuilderImpl::addSelectedNode() node NOT FOUND index = " << index << endl;
    } else {
        //cout <<"JXipRuleBuilderImpl::addSelectedNode() xipNode->index = "
        // <<xipNode->index <<endl;
        //cout <<"JXipRuleBuilderImpl::addSelectedNode() index = " <<index
        //<<" category = " <<xipNode->category <<endl;
        // selectedNode is a global and will used by
        // addSelectedFeatureToSelectedNodeImplementation
        selectedNode = new SelectedNode(xipNode, index);
        selectedNode->any = any;
        selectedNodes.ajoute(selectedNode);
    }
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_RuleBuilder_addSelectedXipNodeImplementation(
        JNIEnv *env, jobject obj, jlong ind, jboolean anyB) {
    long index = (long) ind;
    char any;
    if (anyB) {
        any = 1;
    } else {
        any = 0;
    }
    //cout <<"JXipRuleBuilderImpl::addSelectedXipNodeImplementation() index = " <<index <<endl;
    addSelectedNode(index, any);
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_RuleBuilder_addSelectedXipLeafImplementation(
        JNIEnv *env, jobject obj, jlong ind, jboolean anyB) {
    long index = (long) ind;
    char any;
    if (anyB) {
        any = 1;
    } else {
        any = 0;
    }
    //cout <<"JXipRuleBuilderImpl::addSelectedXipLeafImplementation() index = " <<index <<endl;
    addSelectedNode(index, any);
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_RuleBuilder_addSelectedFeatureToSelectedDependencyImplementation(
        JNIEnv *env, jobject obj, jlong indSelectDependency, jlong indSelectFeature) {
    long indexSelectedDependency = (long) indSelectDependency;
    long indexSelectedFeature = (long) indSelectFeature;
    //printf("\nJXipRuleBuilderImpl::addSelectedFeatureToSelectedDependencyImplementation() indexSelectedDependency = %d indexSelectedFeature = %d\n",
    // indexSelectedDependency, indexSelectedFeature);
    XipUnit* xipUnit = xipResult->sentences[0];
    //cout <<"JXipRuleBuilderImpl::addSelectedFeatureToSelectedDependencyImplementation() language = "
    // + xipUnit->language <<endl;
    XipDependency* xipDependency = xipUnit->dependencies[indexSelectedDependency];
    //cout <<"JXipRuleBuilderImpl::addSelectedFeatureToSelectedDependencyImplementation() name = "
    // + xipDependency->name <<endl;
    XipFeature* xipFeature = xipDependency->features[indexSelectedFeature];
    //cout <<"JXipRuleBuilderImpl::addSelectedFeatureToSelectedDependencyImplementation() attribute = "
    // + xipFeature->attribute <<endl;
    string featureStr = xipFeature->attribute + ":" + xipFeature->value;
    //cout <<"JXipRuleBuilderImpl::addSelectedFeatureToSelectedDependencyImplementation() featureStr = "
    // + featureStr <<endl;
    // selectedDependency is a global and has been initialized by
    // addSelectedXipDependencyImplementation
    vector<string> features = selectedDependency->features;
    features.push_back(featureStr);
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_RuleBuilder_addSelectedXipDependencyImplementation(
        JNIEnv *env, jobject obj, jlong ind) {
    long index = (long) ind;
    //cout <<"JXipRuleBuilderImpl::addSelectedXipDependencyImplementation() index = "
    // <<index <<endl;
    XipUnit* xipUnit = xipResult->sentences[0];
    XipDependency* xipDependency = xipUnit->dependencies[index];
    // selectedDependency is a global and it will be used by
    // addSelectedFeatureToSelectedDependencyImplementation
    selectedDependency = new SelectedDependency(xipDependency, index);
    selectedDependencies.ajoute(selectedDependency);
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_RuleBuilder_clearGlobalsImplementation(
        JNIEnv *env, jobject obj) {
    selectedDependencies.nettoie();
    selectedDependencies.raz();
    selectedNodes.raz();
    selectedNodes.nettoie();
    focus.raz();
    categoryDependencies.clear();
}

JNIEXPORT void JNICALL Java_com_xerox_jxip_RuleBuilder_addFocusImplementation(
        JNIEnv *env, jobject obj, jlong ind) {
    long index = (long) ind;
    //cout <<"JXipRuleBuilderImpl::addFocusImplementation() index = " <<index <<endl;
    focus.ajoute((long) index);
}

//-----------------------------------------------------------------------------------

JNIEXPORT void JNICALL Java_com_xerox_jxip_RuleBuilder_addCategoryDependencyImplementation(
        JNIEnv *env, jobject obj, jstring catDep) {
    string catDepStr = jstringToString(env, catDep);
    //cout <<"JXipRuleBuilderImpl::addCategoryDependency() catDepStr = " <<catDepStr <<endl;
    categoryDependencies.push_back(catDepStr);
}

JNIEXPORT jint JNICALL Java_com_xerox_jxip_RuleBuilder_generateRuleCategoryImplementation(
        JNIEnv *env, jobject obj, jint gHandler, jstring fileName) {
    XipUnit* xipUnit = xipResult->sentences[0];
    RuleGeneration* ruleGeneration = new RuleGeneration(xipUnit);
    try {
        string fileNameStr = jstringToString(env, fileName);
        //cout <<"JXipRuleBuilderImpl::generateRuleCategory() gHandler = "
        // <<gHandler <<" fileName = " <<fileNameStr <<endl;
        XipUnit* xipUnit = xipResult->sentences[0];
        //categoryDependencies has been init by RuleBuilder::generateRuleCategory()
        ruleGeneration->GenerateRuleCategory(gHandler, fileNameStr, categoryDependencies);
        categoryDependencies.clear();
        delete ruleGeneration;
        return 0;
    } catch (XIPError *xipError) {
        delete ruleGeneration;
        categoryDependencies.clear();
        if (xipError->message != "") {
            cerr << "generateRuleCategory Error: " << xipError->message << endl;
            return -1;
        } else {
            cerr << "generateRuleCategory Error: no error message" << endl;
            return -1;
        }
    } catch (...) {
        cerr << "generateRuleCategory Error: no error type" << endl;
        delete ruleGeneration;
        categoryDependencies.clear();
        return -1;
    }
}

