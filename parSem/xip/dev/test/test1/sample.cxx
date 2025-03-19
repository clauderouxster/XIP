#include <iostream>
#include <sstream>
#include <string>

#include "xipstlres.h"

int main(int argc, const char* argv[]) {
	string grammarFile = argv[1];
	string question;
	Whoami(question);
	cout << question  <<endl;
	cout << "JXipImpl::main() grm :" <<grammarFile  <<endl;
	ostringstream *os = new ostringstream();
	string textInputString("Peter is happy.");
	int gHandler;
	try {
		//throw "a test";
		//XIPError *xiperror = new XIPError("mes", "file", "date", 10, 20);
		//throw xiperror;
		gHandler = XipGrmFile(grammarFile, 0);
		cout << "JXipImpl::main() parsing the string :" <<textInputString  <<endl;
		int displayMode = 0;
		int col = 80;
		XipSetDisplayMode(gHandler, displayMode, col);
		XipParseStringOS(textInputString, gHandler, os, 0, 1);
		cout << "JXipImpl::main() Result after parsing: \n" <<os->str() <<endl;
		XipReloadGrammar(gHandler, 0, NULL);
		XipFreeGrammar(gHandler);
	} catch (XIPError* xipError) {
		cerr <<"XipError:"  <<endl;
		cerr <<"message:" <<xipError->message <<endl;
		cerr <<"filename:" <<xipError->filename <<endl;
		cerr <<"timestamp:" <<xipError->timestamp <<endl;
		cerr <<"line:" <<xipError->line <<endl;
		cerr <<"position:" <<xipError->position <<endl;
		return -1;
	} catch (char* msg) {
		cerr <<"Exception message:" <<msg <<endl;
	} catch (...) {
		 cerr <<"Exception: no message"  <<endl;
	}
	return 0;
}
