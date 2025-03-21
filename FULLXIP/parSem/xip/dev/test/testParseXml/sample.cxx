#include <iostream>
#include <sstream>
#include <string>

#include "xipstlres.h"
#include "pasofficiel.h"

void xipCallBack(int gHandler, XipResult *res, void *unused) {
	string xmlUnitElem = (const char*)XipGetCurrentInternalStringBuffer(gHandler);
	//cout <<"JXipImpl2::xipCallBack xmlUnitElem=" <<xmlUnitElem <<"  Number of sentences:"<<res->sentences.size()<<endl;
}

void xipCallBackXml(int gHandler, XipResult *res, void *unused) {
	//cout <<"JXipImpl2::xipCallBackXml " <<endl;
	XipCleanCurrentXipResult(gHandler);
}


int gHandler;
int depth =2;
XipResult *xipres;
string inputFileStr = "input.xml";

void f1(const char *argv[]) {
	try {
		string grammarFile = argv[1];
		cout << "JXipImpl::main() grm :" <<grammarFile  <<endl;
		gHandler = XipGrmFile(grammarFile, 0);
		XipSetCallBack(gHandler, xipCallBack, NULL);
		XipSetCallBackXML(gHandler, xipCallBackXml, NULL);
		xipres = XipCreateCurrentXipResult(gHandler);
		int res = XipParseXMLFile(gHandler, inputFileStr, depth, xipres);
		XipDeleteCurrentXipResult(gHandler);

		xipres = XipCreateCurrentXipResult(gHandler);
		XipCleanCurrentXipResult(gHandler);
	} catch (...) {
		 cout <<"+++++++++++++++++++++ Exception ++++++++++++++++++++"  <<endl;
	}
}
	

int main(int argc, const char* argv[]) {
	f1(argv);
	XipParseXMLFile(gHandler, inputFileStr, depth, xipres);
	ofstream* ofs = new ofstream("output");
	XipParseFileToFile(inputFileStr,gHandler, ofs, (char)0, (char) 1);
	cout <<"+++++++++++++++++++++ Done ++++++++++++++++++++"  <<endl;
}
