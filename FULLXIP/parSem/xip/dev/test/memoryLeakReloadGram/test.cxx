#include <iostream>
#include <sstream>
#include <string>
#include "xipstlres.h"

string gram;
int gHandler;
char xmlMode = 0;
char applyGram = 1;
ofstream* ofs;
int CONST1 = 5;
int CONST2 = 2;

int testEng1() {
	cout <<"--------------------------------------- tesEng1 --------------" <<endl;
	gram = "../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
	gHandler = XipGrmFile(gram, 0);
	XipSetDisplayMode(gHandler, UTF8_INPUT, 80);
	XipAddFlagDisplayMode(gHandler, DISPLAY_TREE, 80);
	for (int ind=0; ind <CONST1; ind++) {
		int err1 = XipParseFileToFile("inputEng", gHandler, ofs, xmlMode, applyGram);
		if (err1 != 0) {
			cout <<"Xip::main() XipParseFileToFile Error" <<endl;
			return 1;
		}
		cout <<ind <<"\r";
		cout.flush();
	}
	return 0;
}

int testEng2() {
	cout <<"--------------------------------------- tesEng2 --------------" <<endl;
	gram = "../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
	gHandler = XipReloadGrammar(gHandler, 1, (char*)gram.c_str());
	for (int ind=0; ind <CONST1; ind++) {
		ostringstream *os = new ostringstream();
		int err1 = XipParseStringOS("Peter is happy.", gHandler,os , xmlMode, applyGram);
		if (err1 != 0) {
			cout <<"Xip::main() XipParseStringOS Error" <<endl;
			return 1;
		}
		delete os;
		cout <<ind <<"\r";
		cout.flush();
	}
	return 0;
}

int testFre() {
	cout <<"--------------------------------------- tesFre --------------" <<endl;
	gram = "../../GRAMMARS/FRENCH/BASIC/french.grm";
	gHandler = XipReloadGrammar(gHandler, 1, (char*)gram.c_str());
	XipRemoveFlagDisplayMode(gHandler, UTF8_INPUT);
	XipRemoveFlagDisplayMode(gHandler, DISPLAY_TREE);
	for (int ind=0; ind <CONST1; ind++) {
		int err1 = XipParseFileToFile("inputFre", gHandler, ofs, xmlMode, applyGram);
		if (err1 != 0) {
			cout <<"Xip::main() XipParseFileToFile Error" <<endl;
			return 1;
		}
		cout <<ind <<"\r";
		cout.flush();
	}
	return 0;
}


int testSpa() {
	cout <<"--------------------------------------- tesSpa --------------" <<endl;
	gram = "../../GRAMMARS/SPANISH/GRMFILES/spanish.grm";
	gHandler = XipReloadGrammar(gHandler, 1, (char*)gram.c_str());
	//XipSetDisplayMode(gHandler, UTF8_INPUT, 80);
	for (int ind=0; ind <CONST1; ind++) {
		int err1 = XipParseFileToFile("inputSpa", gHandler, ofs, xmlMode, applyGram);
		if (err1 != 0) {
			cout <<"Xip::main() XipParseFileToFile Error" <<endl;
			return 1;
		}
		cout <<ind <<"\r";
		cout.flush();
	}
	return 0;
}

int main(int argc, const char* argv[]) {
	try {
		ofs = new ofstream("output");
		if (ofs == NULL) {
	        	cout <<"can't open output" <<endl;
		} else {
			for (int ind0=0; ind0 <CONST2; ind0++) {
				if (testEng1() != 0) {break;};
				if (testEng2() != 0) {break;};
				if (testFre() != 0) {break;};
				if (testSpa() != 0) {break;};
			}
			ofs->close();
			delete ofs;
		}
	} catch (XIPError* xipError) {
	        cout <<"Error:" <<xipError->message <<endl;
	        return -1;
	}
}
