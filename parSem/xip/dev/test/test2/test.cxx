#include <iostream>
#include <sstream>
#include <string>

#include "xipstlres.h"

static char v_str[512];

char* convStr(string str) {
	int i;
	for (i = 0; str.c_str()[i]!=0; i++) {
		v_str[i] = str.c_str()[i];
	}
	v_str[i] = '\0';
	return v_str;
}

int loadGrm(string grmdir, string gram) {
        cout <<"grmdir = " <<grmdir <<endl;
	//set GRMDIR var env.
	string str1 = "GRMDIR=" + grmdir;
        putenv(convStr(str1));
        string grammarFile = grmdir + "/"+ gram;
        cout <<"grammarFile = " <<grammarFile <<endl;
	int gHandler;
	try {
		gHandler = XipGrmFile(grammarFile, 0);
	} catch (XIPError* xipError) {
		cerr <<"XipGrmFileImplementation Error:" <<xipError->message <<endl;
		return -1;
	}
	return gHandler;
}

int loadEngRef() {
        string grammarFile = "english.grm";
        cout <<"grammarFile = " <<grammarFile <<endl;
	int gHandler;
	try {
		gHandler = XipGrmFile(grammarFile, 0);
	} catch (XipError* xipError) {
		cerr <<"XipGrmFileImplementation Error:" <<xipError->message <<endl;
		return -1;
	}
	return gHandler;
}

int loadEngB() {
	string root = getenv("ROOT");
        string grmdir = root + "/grammar/english/basic";
	return loadGrm(grmdir,"english.grm");
}

int loadEngE() {
	string root = getenv("ROOT");
        string grmdir = root + "/grammar/english/entity";
	return loadGrm(grmdir,"englishEntity.grm");
}

int loadEng() {
	return loadEngB();
}

int loadFr() {
	string root = getenv("ROOT");
        string grmdir = root + "/grammar/french/basic";
	return loadGrm(grmdir,"french.grm");
}

void parse(int gHandler, string textInputString) {
	cout << "parsing the string :" <<textInputString  <<endl;
	int displayMode = 0;
	int col = 80;
	XipSetDisplayMode(gHandler, displayMode, col);
	ostringstream *os = new ostringstream();
	XipParseStringOS(textInputString, gHandler, os, 0, 1);
	cout << "Result after parsing: \n" <<os->str() <<endl;
}

void t1() {
	cout <<"--------------------------------------- t1 -------------------" <<endl;
	int gHandlerEng = loadEng();
	parse(gHandlerEng,"Peter is happy");
}	

void t2() {
	cout <<"--------------------------------------- t2 -------------------" <<endl;
	int gHandlerFr = loadFr();
	parse(gHandlerFr,"Le chat est gris");
}	

void t3() {
	cout <<"--------------------------------------- t3 -------------------" <<endl;
	int gHandlerEng = loadEng();
	int gHandlerFr = loadFr();
	parse(gHandlerEng,"Peter is happy");
}	

void t4() {
	cout <<"--------------------------------------- t4 -------------------" <<endl;
	int gHandlerEng = loadEngRef();
	int gHandlerFr = loadFr();
	parse(gHandlerEng,"Peter is happy");
}

void t5() {
	cout <<"--------------------------------------- t5 -------------------" <<endl;
	int gHandlerEng = loadEngRef();
	parse(gHandlerEng,"Peter is happy");
}

int main(int argc, const char* argv[]) {
	t1(); 
	t2(); 
	t3(); 
	t4(); 
	t5();
	return 0;
}
