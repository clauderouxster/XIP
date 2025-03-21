// test.cpp : Defines the entry point for the console application.

#include "xipstlres.h"

//Linker avec:
//libmySQL.lib mysql++.lib mysqlclient.lib LibStorage.lib xips.lib ws2_32.lib user32.lib

int agde_analysis(char* fileName, char* machine, char* database, char* user, char *password,
                 char* filename, char store_sentence, char cleanbase);

void aidemoi() {
    cout<<"lancer: stocke grammar.grm machine database user password filename storesentence cleantable"<<endl;
    cout<<"exemple:"<<endl<<"\\tstocke english.grm izoard test root root myfile.txt 1 1"<<endl;
}

int main(int argc, char* argv[]) {
    char store_sentence=0;
    char cleanbase=0;
    int gHandler;

    if (argc != 9) {
        aidemoi();
        exit(-1);
    }
    if (argv[7][0] == '1') {
        store_sentence=1;
    }
    if (argv[8][0] == '1') {
        cleanbase=1;
    }
    try {
     	agde_analysis(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], store_sentence, cleanbase);
    	XipFreeGrammar(gHandler);
    } catch (XIPError* xipError) {
	cerr <<"Error:" <<xipError->message <<endl;
	exit(-1);
    }
    exit(0);
}
