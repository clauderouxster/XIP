#include <iostream>
#include <sstream>
#include <string>
#include "xipstlres.h"

string gram;
int gHandler;

int main(int argc, const char* argv[]) {
	try {
		gram = "../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
		gHandler = XipGrmFile(gram, 0);
		for (int ind0=0; ind0 <10; ind0++) {
			cout<<"Boucle:"<<ind+1<<endl;
			gram = "../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
			gHandler = XipReloadGrammar(gHandler, 1, (char*)gram.c_str());
			gram = "../../GRAMMARS/FRENCH/BASIC/french.grm";
			gHandler = XipReloadGrammar(gHandler, 1, (char*)gram.c_str());
			gram = "../../GRAMMARS/SPANISH/GRMFILES/spanish.grm";
			gHandler = XipReloadGrammar(gHandler, 1, (char*)gram.c_str());
		}
	} catch (...) {
	        cout <<"Exception:" <<endl;
	}
}
