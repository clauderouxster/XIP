#include <iostream>
#include <sstream>
#include <string>

#include "xipstlres.h"

int main(int argc, const char* argv[]) {
	string grammarFile = "../../grammar/english/basic/english.grm";
	ostringstream *os = new ostringstream();
	string textInputString("Peter is happy.");
	int gHandler;
	try {
		gHandler = XipGrmFile(grammarFile, 0);
	} catch (XIPError* xipError) {
		cerr <<"XipGrmFileImplementation Error:" <<xipError->message <<endl;
		return -1;
	}
	cout << "JXipImpl::main() parsing the string :" <<textInputString  <<endl;
	int displayMode = 0;
	int col = 80;
	XipSetDisplayMode(gHandler, displayMode, col);
	XipParseStringOS(textInputString, gHandler, os, 0, 1);
	cout << "JXipImpl::main() Result after parsing: \n" <<os->str() <<endl;
	XipReloadGrammar(gHandler, 0, NULL);
	XipFreeGrammar(gHandler);
	return 0;
}
