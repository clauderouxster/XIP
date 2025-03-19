#include <iostream>
#include <sstream>
#include <string>
#include "xipstlres.h"
int main(int argc, const char* argv[]) {
	try {
		int gHandler = XipGrmFile("../../grammar/english/basic/english.grm", 0);
		XipSetDisplayMode(gHandler, 0, 80);
		string inputStr = "Peter is happy";
		for (int ind=0; ind <10000; ind++) {
			ostringstream os ;
			XipParseStringOS(inputStr, gHandler, &os, 0, 1);
		}
		XipFreeGrammar(gHandler);
	} catch (XIPError* xipError) {
	        cout <<"Error:" <<xipError->message <<endl;
	        return -1;
	}
}
