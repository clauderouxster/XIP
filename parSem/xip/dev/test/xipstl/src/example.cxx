/*
Example of XIP client, using xipstl.dll
XIP provides two different API, one which is based on STL (xipstl.dll), the other one
which is based on a XEROX implementation of the vector template (xipdll.dll). 
This example describes a connection to xipstl.dll and is based on STL vectors.
*/

#include <stdlib.h>
#include <stdio.h>

#include "xipstlres.h"

void Usage()
{
    cerr<<"Usage:" <<endl;
    cerr<<"     -call"<<endl;
    cerr<<"     -grm grammar"<<endl;
}

void DisplayNodes(XipNode* root,XipNode* node) {
    /*
    We first check the type of the node
    XIPLEAF nodes store the different readings associated to a word.
    A XipLeaf node provides the surface and the lemma of each reading.    
    while XIPNODE stores upper levels nodes such as phrasal nodes. A XipNode
    only provides the category of the node.
    */
    if (node==root)
        cout<<"TREE"<<endl<<endl;
    if (node->Type()==XIPLEAF) {
        //It is a lexical node
        //We first recast the current node in a XipLeaf node
        XipLeaf* leaf=(XipLeaf*)node;
        cout<<leaf->surface<<":"<<leaf->lemma<<"<"<<leaf->left<<":"<<leaf->right<<">";
    }
    else {
        //It is not a lexical node
        cout<<node->category<<"<"<<node->left<<":"<<node->right<<">{";
        for (int i=0;i<node->daughters.size();i++) {
            if (i)
                cout<<",";
            DisplayNodes(root,node->daughters[i]);
        }
        cout<<"}";
    }
    if (node==root)
        cout<<endl;
}

//We display each dependency
void DisplayDependencies(VXipDependency& dependencies) {
    cout<<endl<<"DEPENDENCIES"<<endl<<endl;
    //we loop on each dependency
    for (int i=0;i<dependencies.size();i++) {
        XipDependency* dep=dependencies[i];
        cout<<dep->name<<"(";
	//for each parameter
        for (int p=0;p<dep->parameters.size();p++) {
            if (p)
                cout<<",";
            DisplayNodes(NULL,dep->parameters[p]);
        }
        cout<<")"<<endl;
    }
}

//We display each lexical node
void DisplayLeaves(VXipNode& leaves) {
    cout<<endl<<"LEAVES"<<endl<<endl;
    for (int i=0;i<leaves.size();i++) {
        DisplayNodes(NULL,leaves[i]);
        cout<<endl;
    }
}

//The call back function
void CallAfterEachSentence(int ipar,XipResult* res,void* data) {
    cout<<"--------------------------------------------------------------"<<endl<<endl;
    cerr<<"We display a new sentence:"<<endl;
    //We display the result of the unique sentence, those functions are defined above
    DisplayNodes(res->sentences[0]->root,res->sentences[0]->root);
    DisplayDependencies(res->sentences[0]->dependencies);
    DisplayLeaves(res->sentences[0]->leaves);

    //After each sentence we clean the current XIPRESULT object to free the memory
    //If one has to parse a very large tree, then this method avoids the memory to
    //explode
    XipCleanCurrentXipResult(ipar);
}

int main (int argc, char *argv[]) {
    //This client needs a context, this line is alas mandatory...
    int i;
    string grmfilename;
    char callback=0;
        
    if (argc==1) {
        Usage();
        return -1;
    }
    for (i = 1; i < argc ;i++) {                
        if (!strcmp(argv[i],"-call")) {
            callback=1;
            continue;
        }
        if (!strcmp(argv[i],"-grm")) {
            if ((i+1)>=argc) {
                cerr<<"Please choose a filename"<<endl;
                Usage();
                return -1;
            }
            else
                grmfilename=argv[++i];
            continue;
        }
        cerr<<"Unknown command:"<<argv[i]<<endl;
        Usage();
        exit(-1);
    }
    try {
        //We load a grammar together with a ntm script (finate-state lexicons).
        //The second parameter is set to 0 to tell the system that the initial sentence
        //(or file) is not an XIP XML result input. ipar is a handler on that grammar. Since, more
        //than one grammar can be loaded at a time, we use this handler to distinguish between 
        //those different grammars.
        /*
        It is possible to load each of those files separatly as shown in the example below:
        */

        int ipar=XipGrmFile(grmfilename);

        //We decide de create a Callback function to intercept each XipResult built after a sentence has been analyzed.
        if (callback==1)
            XipSetCallBack(ipar,CallAfterEachSentence,NULL);        
        //The input example.
        string phrs="They have decided to parse that sentence. The result is displayed in two different ways.";

        //We create a XipResult handler
        XipResult* res=XipCreateCurrentXipResult(ipar);
        //We parse the string, the result is stored in "res"
        XipParseString(phrs,ipar,res,1);        

        //In the case we don't have any call back function, we display all results at once.
        if (callback==0) { 
            cout<<"We display all the results at once..."<<endl;
            cout<<"--------------------------------------------------------------"<<endl<<endl;
            for (i=0;i<res->sentences.size();i++) {
                DisplayNodes(res->sentences[i]->root,res->sentences[i]->root);
                DisplayDependencies(res->sentences[i]->dependencies);
                DisplayLeaves(res->sentences[i]->leaves);
                cout<<"--------------------------------------------------------------"<<endl<<endl;
            }
        }
        
        //We delete the last result
        char XipDeleteCurrentXipResult(ipar);
                
        //We delete the grammar
        XipFreeGrammar(ipar);
    }
    catch(XIPError* xipError) {
        cerr<<xipError->message<<endl;
        exit(-1);
    }
    return 0;
}

