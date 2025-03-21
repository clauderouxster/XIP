#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "xipstlres.h"

void displayNodes(XipNode* root, XipNode* node) {
    /*
    first check the type of the node
    XIPLEAF nodes store the different readings associated to a word.
    A XipLeaf node provides the surface and the lemma of each reading.    
    while XIPNODE stores upper levels nodes such as phrasal nodes. A XipNode
    only provides the category of the node. 
     */
    if (node == root) {
        //cout << "TREE" << endl << endl;
    }
    if (node->Type() == XIPLEAF) {
        //It is a lexical node
        //recast the current node in a XipLeaf node
        XipLeaf* leaf = (XipLeaf*) node;
        //cout << leaf->surface << ":" << leaf->lemma << "<" << leaf->left << ":" << leaf->right << ">";
    } else {
        //It is not a lexical node
        //cout << node->category << "<" << node->left << ":" << node->right << ">{";
        for (int i = 0; i < node->daughters.size(); i++) {
            if (i) {
                //cout << ",";
            }
            displayNodes(root, node->daughters[i]);
        }
        //cout << "}";
    }
    if (node == root) {
        //cout << endl;
    }
}

//display each dependency

void displayDependencies(VXipDependency& dependencies) {
    //cout << endl << "DEPENDENCIES" << endl << endl;
    //loop on each dependency
    for (int i = 0; i < dependencies.size(); i++) {
        XipDependency* dep = dependencies[i];
        //cout << dep->name << "(";
        //for each parameter
        for (int p = 0; p < dep->parameters.size(); p++) {
            if (p) {
                //cout << ",";
            }
            displayNodes(NULL, dep->parameters[p]);
        }
        //cout << ")" << endl;
    }
}

//display each lexical node

void displayLeaves(VXipNode& leaves) {
    //cout << endl << "LEAVES" << endl << endl;
    for (int i = 0; i < leaves.size(); i++) {
        displayNodes(NULL, leaves[i]);
        //cout << endl;
    }
}

void callback(int gHandler, XipResult* xipResult, void* data) {
    //cout<<"callback"<<endl;
    for (int i = 0; i < xipResult->sentences.size(); i++) {
        XipUnit *xipUnit = xipResult->sentences[i];
        //cout << "language: " << xipUnit->language << " " << i << endl;
        displayNodes(xipResult->sentences[i]->root, xipResult->sentences[i]->root);
        displayDependencies(xipResult->sentences[i]->dependencies);
        displayLeaves(xipResult->sentences[i]->leaves);
        //cout << "--------------------------------------------------------------" << endl << endl;
    }
    XipCleanCurrentXipResult(gHandler);
}

int loadGrammar(string grm) {
    int gHandler = XipGrmFile(grm, 1, 0);
    //XipSetDisplayMode(gHandler, 0, 80);
    //XipAddFlagDisplayMode(gHandler, CHECK_INPUT_UTF8, 80);
    return gHandler;
}

void parse(int gHandler, string fileName) {
    //cout << "------------------------------------ " << fileName << endl;
    clock_t start = clock();
    XipSetCallBack(gHandler, callback, NULL);
    XipResult* xipResult = XipCreateCurrentXipResult(gHandler);
    int res = XipParseFile(fileName, gHandler, xipResult, 1);
    XipDeleteCurrentXipResult(gHandler);
    cout << "parse duration: " << (clock() - start) / CLOCKS_PER_SEC << endl;
    if (res != 0) {
        cout << "parsing error: " << res << endl;
    }
}

int main(int argc, char *argv[]) {
    int gHandler;
    try {
        if (argc == 3) {
            gHandler = loadGrammar(argv[1]);
            parse(gHandler, argv[2]);
        } else {
            gHandler = loadGrammar("..\\..\\..\\..\\GRAMMARS\\ENGLISH\\GRMFILES\\GRAM_GEN\\gram_gen_entit.grm");
            //gHandler = loadGrammar("../../../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm");
            parse(gHandler, "../corpus/input");
        }
    } catch (XIPError* xipError) {
        cerr << "Error:" << xipError->message << endl;
        exit(-1);
    }
}
