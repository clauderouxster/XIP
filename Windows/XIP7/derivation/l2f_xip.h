#include <iostream>
#include <string>
#include <map>
#include "Option.cpp"

class GlobalParseurTry : public GlobalParseur {
public:
    GlobalParseurTry(char* grammarfile, char* pathname);

    void SentenceAnalysis(char renumerote, char tree);
    void LexiconEntryParsing(char* chaine, char** duplicat);
    int loading(char* path, VECTA<char*>& fileparam, Options const& options);

    int read(char*, int);
	 //~GlobalParseurTry ();

};

