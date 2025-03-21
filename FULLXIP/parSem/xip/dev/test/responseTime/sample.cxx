#include <iostream>
#include <sstream>
#include <string>
#include <sys/time.h>

#ifndef XIPLIBXML
#define XIPLIBXML
#endif
#include "xipstlres.h"

int gHandler;
struct timeval ts1,ts2,te1,te2;

void parse(string inputFile) {
	try {
		ofstream* ofs;
		ofs = new ofstream("output");
		gettimeofday(&ts2,NULL);
		XipParseFileToFile(inputFile, gHandler, ofs);
		gettimeofday(&te2,NULL);
		printf("time to parse file %s    : %f seconds\n",inputFile.c_str(),  ((double) (te2.tv_sec - ts2.tv_sec)));
		ofs->close();

	} catch (char* mess) {
		cerr <<"Error:" <<mess <<endl;
	}
}

int main(int argc, const char* argv[]) {
	string grammarFile = argv[1];
	try {
		gettimeofday(&ts1,NULL);
		gHandler = XipGrmFile(grammarFile, 1, 0);
		gettimeofday(&te1,NULL);
		printf("time to load grammar  : %f seconds\n", ((double) (te1.tv_sec - ts1.tv_sec)));
		parse("10k.txt");
		parse("20k.txt");
		parse("100k.txt");
		parse("500k.txt");
	} catch (char* mess) {
		cerr <<"Error:" <<mess <<endl;
		return -1;
	}
}
