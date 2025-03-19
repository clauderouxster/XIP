#include <iostream>
#include <sstream>
#include <string>

#define XIPLIBXML
#include "xipstlres.h"

void test(string grammarFile, char xmlOutput, string fileOutput, XIPLONG displayMod) {
	string inputFile = "input.xml";
        try {
		cout << "---------------- test ------------ "  <<endl;
		int res ;
		// load the grammar
        	int gHandler = XipGrmFile(grammarFile, 1, 0);

		//set parser option
		XIPLONG displayMode = 0;
		int col = 80;
                XipSetDisplayMode(gHandler, displayMode, col);
                XipAddFlagDisplayMode(gHandler, displayMod, col);

		//parse the xml file
		int depth = 0;
		ofstream* ofs = new ofstream(fileOutput.c_str());
                res = XipParseFileXMLToFile(inputFile, gHandler, ofs, depth, xmlOutput);
                ofs->close();
		//ostringstream* os = new ostringstream();
		//res = XipParseFileXMLOS(inputFile, gHandler, os, depth, xmlOutput);
		//cout << "\n Result after parsing: \n" <<os->str() <<endl;	

                if (res != 0) {
                        cout << "parsing error: " <<res <<endl;
                }
        } catch (char* mess) {
                cerr <<"Error:" <<mess <<endl;
        }
}

int main(int argc, const char* argv[]) {
	test("sample1.grm", XML_NONE,   "output0.xml", DISPLAY_CHUNK_TREE); 
	test("sample1.grm", XML_OUTPUT, "output1.xml", DISPLAY_CHUNK_TREE); 
	test("sample2.grm", XML_INSERT, "output2.xml", DISPLAY_CHUNK_TREE); 
	test("sample3.grm", XML_NONE,   "output3.xml", DISPLAY_NONE); 
}
