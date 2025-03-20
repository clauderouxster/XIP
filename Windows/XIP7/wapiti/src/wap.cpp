#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ostream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

extern "C" {
int TaggerError(void* tg);
void* CreateTagger(char* crfmodel,char* crfbrown,char* crfmkcls);
int TaggingTokens(void* tg,char** words,char** tags,int nb);
void CleanTagger(void* tg);
}

int main (int argc, char *argv[]) {
	char* model="W:\\users\\Roux\\xip\\parSem\\xip\\dev\\GRAMMARS\\ENGLISH\\APPLICATIONS\\SCOOP\\TAGGER\\model";
	char* brown="W:\\users\\Roux\\xip\\parSem\\xip\\dev\\GRAMMARS\\ENGLISH\\APPLICATIONS\\SCOOP\\TAGGER\\brownCluster.txt";
	char* mkcls="W:\\users\\Roux\\xip\\parSem\\xip\\dev\\GRAMMARS\\ENGLISH\\APPLICATIONS\\SCOOP\\TAGGER\\mkcls";
	void* crftagger=CreateTagger(model,brown,mkcls);
	int nb=6;
	char** wordSequence=new char*[nb+1];
	char** predictedTag=new char*[nb+1];

	wordSequence[0]="The";
	wordSequence[1]="dog";
	wordSequence[2]="eats";
	wordSequence[3]="a";
	wordSequence[4]="bone";
	wordSequence[5]=".";
	cerr<<"Lancement"<<endl;
	int nbelements=TaggingTokens(crftagger,wordSequence,predictedTag,nb);
	int i;

	for (i=0;i<nbelements;i++) {
		cerr<<predictedTag[i]<<endl;
		free(predictedTag[i]);
	}

	nbelements=TaggingTokens(crftagger,wordSequence,predictedTag,nb);
	for (i=0;i<nbelements;i++) {
		cerr<<predictedTag[i]<<endl;
		free(predictedTag[i]);
	}
	cerr<<"Fin"<<endl;
}
