#include <iostream>
#include <sstream>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "User32.Lib")

#ifndef XIPLIBXML
#define XIPLIBXML
#endif
#include "xipstlres.h"

int main(int argc, const char* argv[]) {
	string grammarFile = argv[1];
	int gHandler;
	try {
		gHandler = XipGrmFile(grammarFile, 1, 0);

		int displayMode = 0;
		int col = 80;
		XipSetDisplayMode(gHandler, displayMode, col);
		XipAddFlagDisplayMode(gHandler, DISPLAY_FULL, col);
		XipAddFlagDisplayMode(gHandler, DISPLAY_TREE, col);
		//XipAddFlagDisplayMode(gHandler, DISPLAY_CONVERSION_UTF8, col);
		
		string textInputString(" No one thinks what we are doing is crazy.");
		string xmlInputString("<TEXT> No one thinks what we are doing is crazy.</TEXT>");
		string inputFile = argv[2];
		ostringstream *os = new ostringstream();
		int depth = 0;
		int res ;
		ofstream* ofs = new ofstream("output");

		res = XipParseStringOS(textInputString, gHandler, os);
		//res = XipParseFileOS(inputFile, gHandler, os);
		//res = XipParseFileToFile(inputFile, gHandler, ofs);
		
		//res = XipParseStringXMLOS(xmlInputString, gHandler, os, depth);
		//res = XipParseFileXMLOS(inputFile, gHandler, os, depth);
		//res = XipParseFileXMLToFile(inputFile, gHandler, ofs, depth);
		
		ofs->close();
		cout << "\n Result after parsing: \n" <<os->str() <<endl;
		if (res != 0) {
			cout << "parsing error: " <<res <<endl;
		}
	} catch (char* mess) {
		cerr <<"Error:" <<mess <<endl;
		return -1;
	}
}
