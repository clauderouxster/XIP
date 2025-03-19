#include <iostream>
#include <sstream>
#include <string>
#include "xipstlres.h"

char xmlMode = 0;
char applyGram = 1;
ofstream* ofs;
int CONST1 = 100;


int test(int gHandler, string input) {
	cout <<"test: " <<input <<endl;
	try {
		for (int ind=0; ind <CONST1; ind++) {
			int err1 = XipParseFileToFile(input, gHandler, ofs, xmlMode, applyGram);
			if (err1 != 0) {
				cout <<"Xip::main() XipParseFileToFile Error" <<endl;
				return 1;
			}
			cout <<ind <<"\r";
			cout.flush();
		}
		return 0;
	} catch (XIPError* xipError) {
	        cout <<"Error:" <<xipError->message <<endl;
	        return 1;
	}
	cout <<endl;
}

int main(int argc, const char* argv[]) {
	try {
		ofs = new ofstream("output");
		if (ofs == NULL) {
	        	cout <<"can't open output" <<endl;
		} else {
			string gram1 = "../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
			string gram2 = "../../GRAMMARS/SPANISH/GRMFILES/spanish.grm";
			string gram3 = "../../GRAMMARS/FRENCH/BASIC/french.grm";
			int gh1 = XipGrmFile(gram1, 0);
			int gh2 = XipGrmFile(gram2, 0);
			int gh3 = XipGrmFile(gram3, 0);
			if (test(gh1, "inputEng") != 0) {exit(-1);};
			if (test(gh2, "inputFre") != 0) {exit( -1);};
			if (test(gh3, "inputSpa") != 0) {exit(-1);};
			ofs->close();
			delete ofs;
		}
	} catch (XIPError* xipError) {
	        cout <<"Error:" <<xipError->message <<endl;
	        return -1;
	}
}
