#include <iostream>
#include <sstream>
#include <string>

#include "xipstlres.h"

int main(int argc, const char* argv[]) {
	string grammarFile = "../../grammar/english/basic/english.grm";
	ostringstream *os = new ostringstream();
	int gHandler;
	gHandler = XipGrmFile(grammarFile, 0);
	XipParseStringOS("Peter is happy.", gHandler, os, 0, 1);
	cout << "JXipImpl::main() Result after parsing: \n" <<os->str() <<endl;
	XipFreeGrammar(gHandler);
	// as FST can't be reloaded (PB in the FST lib it's necessary to use XipReloadGrammar
	XipReloadGrammar(gHandler);
	XipParseStringOS("Peter is happy.", gHandler, os, 0, 1);
	cout << "JXipImpl::main() Result after parsing: \n" <<os->str() <<endl;
}
