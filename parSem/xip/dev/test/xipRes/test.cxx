#include <stdlib.h>
#include <stdio.h>

#include "xipstlres.h"

void displayNodes(XipNode* root,XipNode* node) {
    /*
    first check the type of the node
    XIPLEAF nodes store the different readings associated to a word.
    A XipLeaf node provides the surface and the lemma of each reading.    
    while XIPNODE stores upper levels nodes such as phrasal nodes. A XipNode
    only provides the category of the node. 
    */
    if (node==root) cout<<"TREE"<<endl<<endl;
    if (node->Type()==XIPLEAF) {
        //It is a lexical node
        //recast the current node in a XipLeaf node
        XipLeaf* leaf=(XipLeaf*)node;
        cout<<leaf->surface<<":"<<leaf->lemma<<"<"<<leaf->left<<":"<<leaf->right<<">";
    } else {
        //It is not a lexical node
        cout<<node->category<<"<"<<node->left<<":"<<node->right<<">{";
        for (int i=0;i<node->daughters.size();i++) {
            if (i)
                cout<<",";
            displayNodes(root,node->daughters[i]);
        }
        cout<<"}";
    }
    if (node==root)
        cout<<endl;
}

//display each dependency
void displayDependencies(VXipDependency& dependencies) {
    cout<<endl<<"DEPENDENCIES"<<endl<<endl;
    //loop on each dependency
    for (int i=0;i<dependencies.size();i++) {
        XipDependency* dep=dependencies[i];
        cout<<dep->name<<"(";
	//for each parameter
        for (int p=0;p<dep->parameters.size();p++) {
            if (p) cout<<",";
            displayNodes(NULL,dep->parameters[p]);
        }
        cout<<")"<<endl;
    }
}

//display each lexical node
void displayLeaves(VXipNode& leaves) {
    cout<<endl<<"LEAVES"<<endl<<endl;
    for (int i=0;i<leaves.size();i++) {
        displayNodes(NULL,leaves[i]);
        cout<<endl;
    }
}

int main (int argc, char *argv[]) {
    string grmfilename="../../grammar/english/entity/englishEntity.grm";
    try {
        int gHandler=XipGrmFile(grmfilename);
        string inputStr="Peter is happy.";
        //string inputStr="The Java Virtual Machine allows an application to have multiple threads of execution running concurrently.";
        //create a XipResult handler
        XipResult* xipResult=XipCreateCurrentXipResult(gHandler);
        //parse the string, the result is stored in "xipResult"
        XipParseString(inputStr,gHandler,xipResult,1);        
        cout<<"--------------------------------------------------------------"<<endl<<endl;
        for (int i=0;i<xipResult->sentences.size();i++) {
                displayNodes(xipResult->sentences[i]->root,xipResult->sentences[i]->root);
                displayDependencies(xipResult->sentences[i]->dependencies);
                displayLeaves(xipResult->sentences[i]->leaves);
                cout<<"--------------------------------------------------------------"<<endl<<endl;
        }
        //delete the last result
        char XipDeleteCurrentXipResult(gHandler);
        //delete the grammar
        XipFreeGrammar(gHandler);
    }
    catch(XipError* xipError) {
        cerr<<xipError->message<<endl;
        exit(-1);
    }
    return 0;
}
