
#include <iostream>
using namespace std;

#include <strstream>
#include <fstream>
#include <vector>
#include <string>
#include "xipvecta.h"
#include "xipstlres.h"

void Usage()
{
    cerr<<"Usage:" <<endl;
    cerr<<"     -ntm ntmscript"<<endl;
    cerr<<"     -l (parameter file for a given language)"<<endl;
    cerr<<"     -p filename (parameter file)"<<endl;
    cerr<<"     -text filename (file to process)"<<endl;
}


//#######################################################################################
    
int main (int argc, char *argv[]) {

//This client needs a context, this line is alas mandatory...
    int i, data_pos,fromPipe=0;       

    if (!isatty(0)) {
        fromPipe=1;
    }

    if (!fromPipe && argc <= 1) {
        cerr<<"Please enter the name of the file to process"<<endl;
        return -1;
    }

    char filename[1000];
    char path[1000];
    VECTA<char*> fileparam;
    VECTA<char*> textes;
    int maxSentence=-1;

    char ntmfile[1000];
    char hmmfilename[1000];
    char docfilename[1000];

    ntmfile[0]=0;
    filename[0]=0;
    hmmfilename[0]=0;
    docfilename[0]=0;

    //the language argument is processed
    int arret=0;
    int numberValues=-1;
    data_pos=1;
    int max=1000;

    for (i = 1; i <argc ;i++) {

        if (!strcmp(argv[i],"-l") ) {
            if ((i+1)>=argc) {
                cerr<<"Please choose a filename"<<endl;
                Usage();
                return -1;
            }
            else
                strcpy(filename,argv[++i]);
            continue;
        }

        if (!strcmp(argv[i],"-p")) {
            if ((i+1)>=argc) {
                cerr<<"Please choose a filename"<<endl;
                Usage();
                return -1;
            }
            else
                fileparam.ajoute(strdup(argv[++i]));
            continue;
        }

        if (!strcmp(argv[i],"-text")) {
            if ((i+1)>=argc) {
                cerr<<"Please choose a filename"<<endl;
                Usage();
                return -1;
            }
            else
                textes.ajoute(strdup(argv[++i]));
            continue;
        }

        if (!strcmp(argv[i],"-hmm")) {
            if ((i+1)>=argc) {
                cerr<<"Please choose a filename"<<endl;
                Usage();
                return -1;
            }
            else
                strcpy(hmmfilename,argv[++i]);
            continue;
        }

       if (!strcmp(argv[i],"-max")) {
            if ((i+1)>=argc) {
                cerr<<"Please give a number"<<endl;
                Usage();
                return -1;
            }
            else
                max=atoi(argv[++i]);
            continue;
        }

        if (!strcmp(argv[i],"-ntm")) {
            if ((i+1)>=argc) {
                cerr<<"Please choose a filename"<<endl;
                Usage();
                return -1;
            }
            else
                strcpy(ntmfile,argv[++i]);
            continue;
        }

        cerr<<"Unknown command:"<<argv[i]<<endl;
        Usage();
        exit(-1);
    }

    int ipar=XipLoadGrammar(filename,0,ntmfile,hmmfilename);
    for (int ipa=0;ipa<fileparam.dernier;ipa++) {
        XipParameterFile(ipar,fileparam[ipa]);
        free(fileparam[ipa]);
    }

    for (int itex=0;itex<textes.dernier;itex++) {
        XipResult* res=XipResultCreate();
        XipParseFile(textes[itex],ipar,res);
        XipResultDelete(res);
        free(textes[itex]);
    }

    XipFreeGrammar(ipar);
}
