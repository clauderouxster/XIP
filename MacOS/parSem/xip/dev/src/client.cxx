/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003, Xerox Corporation, XRCE-Grenoble.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 3.16
   filename   : client.cxx
   Date       : 10/01/1999
   Purpose    : A basic client to connect to XIP server
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream.h>
#include "xipresult.h"
#include "client.h"

#define NON 0
#define OUI 1


int XIPParseString(char* string,int port, char* hostname,char* parameters,XipResult&,char xml);
int XIPParseFile(char* filename,int port, char* hostname,char* parameters,XipResult&,char xml);
void XIPBuildParameter(char* parameters);
void XMLCodingTable(char* filename,char* field);

/*
 * Strings we send to the server.
 */
void Usage()
{
    cerr<<"Usage:" <<endl;
    cerr<<"     port number"<<endl;
    cerr<<"     server name (optional if it's on the same machine)"<<endl;
    cerr<<"     -code filename (to provide the system with an XML coding table a la XIP"<<endl;
    cerr<<"     -tagger (tagging mode, no parsing apply)"<<endl;
    cerr<<"     -a (for a complete display of nodes)"<<endl;
    cerr<<"     -r (for a reduced display of the nodes)"<<endl;
    cerr<<"     -tr (for an even more reduced display of the nodes)"<<endl;
    cerr<<"     -offset (display offset value for each token)"<<endl;
    cerr<<"     -ntree (the chunk tree is not displayed)"<<endl;
    cerr<<"     -f (extraction of the dependencies)"<<endl;
    cerr<<"     -ne  (display the number of the extraction rules together with the extraction function)"<<endl;
    cerr<<"     -np (add the phrase number in the dependencies, do not display the chunk tree)"<<endl;
    cerr<<"     -npi (same as -np but also display the inverted form of the dependencies)"<<endl;
    cerr<<"     -xml (the output is display as an xml entry)"<<endl;
    cerr<<"     -renum, for each new sentence, word number starts at 0"<<endl;
    cerr<<"     -nrenum, the first word number of a new sentence continues those of the previous sentences"<<endl;
    cerr<<"     -dis (display the text that is processed)"<<endl;
    cerr<<"     -t (display the chunk tree as a tree"<<endl;
    cerr<<"     -i filename"<<endl;
    cerr<<"     -ntmonly (NTM) display only the result of NTM (no parsing)"<<endl;
    cerr<<"     -nsc (NTM)"<<endl;
    cerr<<"     -xml (xml output)"<<endl;
    cerr<<"\t\tDo not apply the separator constraint by default (do not require ";
    cerr<<"by default that any token must be delimited with separators)"<<endl;
    cerr<<"     -unknownbychars(NTM)"<<endl;
    cerr<<"\t\tIn case of unknown string, print only one char as an unknown "
        <<"token and proceed ahead starting again from the next char." <<endl
        <<"\t\tWhen this option is not set (default), ntm returns a string"
        <<"delimited with separators as an unknown token."<<endl;
    cerr<<"     -indices (NTM)"<<endl;
    cerr<<"\t\tPrint out the indices (in the input text) of tokens (same as \"offset\")"<<endl;
}

main(int argc, char *argv[])
{
    char c;
    register int i, len;

    int fromlen,res,ns;
    
    const int MAXHOSTNAME=1000;
    char nomDuServeur[MAXHOSTNAME+1];
    nomDuServeur[0]=0;
    char xml=NON;

    if (argc<2) {
        cerr<<"Missing arguments: port server"<<endl;
        Usage();
        exit(-1);
    }

    int port=atoi(argv[1]);
    int debut=2;

    if (argc>2 && argv[2][0]!='-') {
        strcpy(nomDuServeur,argv[2]);
        debut=3;
    }

    char reduce=0;
    char num_extract=NON;
    char tagger=NON;
    char fonction_synt=NON;
    char offset=NON;
    char tree=OUI;
    char renumerote=OUI;
    char affiche=NON;
    char affichearbre=NON;
    char nsc=NON;
    char unknown=NON;
    char analyse=OUI;
    char filename[MAXHOSTNAME+1];
    filename[0]=0;
    char codefilename[1000];
    codefilename[0]=0;


//     cerr<<endl<<"\tCopyright © Xerox Corporation 2000-2001"<<endl
//         <<"\tXIP Xerox Incremental Parser Client (2000-2001)"<<endl
//         <<"\tXerox Research Center Europe"<<endl
//         <<"\tImplementation:"<<endl
//         <<"\t\tClaude Roux"<<endl<<endl;


    for (i = debut; i <argc ;i++) {

        if (!strcmp(argv[i],"-i")) {
            if ((i+1)>=argc) {
                cerr<<"Please choose a filename"<<endl;
                Usage();
                exit(-1);
            }
            else
                strcpy(filename,argv[++i]);
            continue;
        }

        if (!strcmp(argv[i],"-code")) {
            if ((i+1)>=argc) {
                cerr<<"Please choose a filename"<<endl;
                Usage();
                exit(-1);
            }
            else
                strcpy(codefilename,argv[++i]);
            continue;
        }

        if (!strcmp(argv[i],"-ntree")) {
            tree=NON;
            continue;
        }
    
        if (!strcmp(argv[i],"-offset") || !strcmp(argv[i],"-indices")) {
            offset=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-nsc")) {
            nsc=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-unknownbychars")) {
            unknown=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-ntmonly")) {
            analyse=NON;
            continue;
        }


        if (!strcmp(argv[i],"-a")) {
            reduce=0;
            continue;
        }
    
        if (!strcmp(argv[i],"-r")) {
            reduce=1;
            continue;
        }
    
        if (!strcmp(argv[i],"-t")) {
            affichearbre=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-renum")) {
            renumerote=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-nrenum")) {
            renumerote=NON;
            continue;
        }

        if (!strcmp(argv[i],"-tr")) {
            reduce=2;
            continue;
        }

        if (!strcmp(argv[i],"-f")) {
            fonction_synt=OUI;
            continue;
        }
        if (!strcmp(argv[i],"-ne")) {
            num_extract=OUI;
            continue;
        }
        if (!strcmp(argv[i],"-tagger")) {
            tagger=OUI;
            continue;
        }
        if (!strcmp(argv[i],"-np")) {
            num_extract=2;
            continue;
        } 
        if (!strcmp(argv[i],"-npi")) {
            num_extract=3;
            continue;
        }
        if (!strcmp(argv[i],"-dis")) {
            affiche=OUI;
            continue;
        }

        if (!strcmp(argv[i],"-xml")) {
            xml=OUI;
            continue;
        }

        break;
    }
    char parametres[100];
    sprintf(parametres,"%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d|", reduce,num_extract,
            tagger,fonction_synt,
            offset,affichearbre,
            tree,renumerote,nsc,unknown,analyse);

    //XIPBuildParameter(parametres);
    XMLCodingTable(codefilename,"codagexml");
    XipResult result;
    XIPParseFile(filename,port,nomDuServeur,parametres,result,xml);
    result.print(cout);     
}


