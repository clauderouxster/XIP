#pragma once

#include <iostream>
#include <string>
#include <map>


class GlobalParseurTry : public GlobalParseur {
protected:
	unsigned char dummyMemCheck[1000];

public:
	void SentenceAnalysis(char renumerote,char tree);
	void LexiconEntryParsing(char* chaine,char** duplicat);
	int loading(char* path,char,VECTA<char*>& fileparam);
	GlobalParseurTry(char* grammarfile,char* pathname);
	int read(char*,int);
	~GlobalParseurTry ();
};
