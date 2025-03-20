// testbiblio.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
//#define XIPLIBXML
//#include "xipstlres.h"
//#include "pasofficiel.h"
//
//#define STR(x) (char*)x.c_str()
//
//class garde {
//public:
//	XipResult* xip;
//	
//	garde() {
//		xip=NULL;
//	}
//};
//
//void appelXML(int ipar, XipResult* xip,void* data) {
//	string tagname;
//	vector<string> properties;
//
//	XipGetCurrentXMLNodeData(ipar,tagname,properties);
//	int i;
//	cout<<(char*)tagname.c_str()<<endl;
//	for (i=0;i<properties.size();i+=2) {
//		cout<<(char*)properties[i].c_str()<<":";
//		cout<<(char*)properties[i+1].c_str()<<endl;
//	}
//
//	cout<<endl<<endl;
//	XipCleanCurrentXipResult(ipar);
//}
//
//void appel(int ipar, XipResult* xip,void* data) {
//	XipCreateCurrentXipResult(ipar);
//	XipCleanCurrentXipResult(ipar);
//	cout<<"Essai de Nettoyage"<<endl;
//}
//
//void appelxml(int ipar, XipResult* xip,void* data) {
//	XipCleanCurrentXipResult(ipar);
//	cout<<"Nettoyage"<<endl;
//}
//
//void parsexml(string& filename) {
//	ostringstream os;
//	XipParseFileXMLOS(filename,0, &os,0,XML_NONE,1);
//	cout<<(char*)os.str().c_str()<<endl;
//}
//
//void parsexmlstring(string& texte) {
//	ostringstream os;
//	XipParseStringXMLOS(texte,0, &os,0,XML_NONE,1);
//	cout<<(char*)os.str().c_str()<<endl;
//}
//
//
//void chargement() {
//	char ch[1000];
//	string me;
//	Whoami(me);
//	cout<<me<<endl;
//	int ipar=XipGrmFile("C:\\XIP\\Test\\english\\ENGLISH\\GRMFILES\\GRAM_GEN\\gram.grm");
//	//XipAddFlagDisplayMode(ipar,UTF8_INPUT,80);
//	//XipSetDisplayMode(ipar,UTF8_INPUT,80);
//	string s="This is 10º West and 12° East.";
//	XipResult* xip=XipCreateCurrentXipResult(ipar);
//	ostringstream os;
//	XipParseStringOS(s,ipar, &os);
//	cout<<STR(os.str())<<endl;
//	//XipParameterFile(ipar,"C:\\XIP\\Test\\SemEval\\ENGLISH\\GRMFILES\\GRAM_NORM\\grandxml.xip");
//	//vector<string> paths;
//	//paths.push_back("C:\\XIP\\Test\\SampleLogGalateas\\parse.cxx");
//	//vector<string> arguments;
//	//arguments.push_back("C:\\XIP\\Test\\French\\BASIC\\french.grm");
//	//int ipar=XipKifLoad(paths,arguments,false);
//	//XipSetCallBack(ipar,appel,NULL);
//	//ostringstream os;
//	//arguments.clear();
//	//arguments.push_back("La dame mange une glace.");
//	//XipKifExecute(ipar,"parsing",arguments,&os,false);
//	//cout<<(char*)os.str().c_str()<<endl;
//	//XipSetCallBackXML(ipar,appelXML,NULL);
//	//
//	//XipParseXMLFile(ipar,"C:\\XIP\\Test\\SemEval\\ENGLISH\\GRMFILES\\GRAM_NORM\\essai.xml",2,xip);
//	
//	//XipDeleteCurrentXipResult(ipar);
//
//	//garde g;
//	//XipResult* xip=XipCreateCurrentXipResult(0);
//	//g.xip=xip;
//	//XipSetCallBack(0,appel,&g);
//	//XipSetCallBackXML(0,appelxml,NULL);
//
//	//XipResult* xp=XipGetCurrentXipResult(0);
//	//
//
//	//XipParseXMLFile(0,"C:\\XIP\\Test\\english\\test\\reut.xml",0,xp);
//
//	//ostringstream osx;
//	//XipParseXMLFile(0,"c:\\xip\\test\\sorties\\essai.xml",0,xp);
//	
//	//strcpy(ch,"C:\\XIP\\Test\\english\\AllEntit.txt");
//	//cout<<ch<<endl;
//	//XipParseFile(ch,0,xp,1);		
//
//	//for (i=0;i<20000;i++)
//	//	for (int j=0;j<20000;j++) {}
//
//	//for (i=0;i<20000;i++) {
//	//	sprintf(ch,"C:\\XIP\\patron_reuters\\reuters\\sgm\\res\\news-%05d.txt",i);
//	//	cout<<ch<<endl;
//	//	xp=XipGetCurrentXipResult(0);
//	//	//if (g.xip!=xp) {
//	//	//	cout<<"ERRREUUURRR AAAAAAAA"<<endl;
//	//	//	printf("%x : %x\n",NULL,(long)xp);
//	//	//}
//	//	XipParseFile(ch,0,xp,1);		
//	//	//xp=XipGetCurrentXipResult(0);
//	//	//if (g.xip!=xp) {
//	//	//	cout<<"ERRREUUURRR BBBBBBBBBBB"<<endl;
//	//	//	printf("%x : %x\n",(long)g.xip,(long)xp);
//	//	//}
//	//}
//	XipFreeGrammar(0);
//}
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//
//	
//	
//	chargement();	
//	
//	return 0;
//}
//

#include <stdlib.h>
#include <stdio.h>

#include "xipstlres.h"

int ghEN,ghFR,ghGE,ghSW;
int cnt;
int decompte;

void callback(int gHandler, XipResult* xipResult,void* data) {
	//cout<<"callback"<<endl;
	for (int i = 0; i < xipResult->sentences.size(); i++) {
		XipUnit *xipUnit = xipResult->sentences[i];
		cout <<"language: " <<xipUnit->language <<" " <<i <<" " <<cnt++  <<"--->"<<decompte<<endl;
	}
	XipCleanCurrentXipResult(gHandler);
}


int loadGrm(string grm) {
        int gHandler=XipGrmFile(grm,1,0);
	XipSetDisplayMode(gHandler, 0, 80);
	XipSetCallBack(gHandler, callback, NULL);
	return gHandler;
}

void loadGrammars() {
	string grmRoot="C:\\XIP\\Test\\Eerqigrammars";
	string grmEN=grmRoot + "\\ENGLISH\\APPLICATIONS\\EERQI\\data\\english_eerqi.grm";
	string grmFR=grmRoot + "\\FRENCH\\APPLICATIONS\\EERQI\\grmfiles\\french_eerqi.grm";
	string grmGE=grmRoot + "\\GERMAN\\APPLICATIONS\\EERQI\\data\\german_eerqi.grm";
	string grmSW=grmRoot + "\\SWEDISH\\APPLICATIONS\\EERQI\\data\\swedish_eerqi.grm";
	try {
		ghEN=loadGrm(grmEN);
		ghFR=loadGrm(grmFR);
		ghGE=loadGrm(grmGE);
		ghSW=loadGrm(grmSW);
	} catch (XIPError* xipError) {
        	cerr << "Grammar loading Error:" << xipError->message << endl;
		exit(-1);
	}
}

void parse(int gHandler,string fileName) {
        cout<<"------------------------------------ " <<fileName <<endl;
	cnt = 0;
	XipResult* xipResult=XipCreateCurrentXipResult(gHandler);
	int res = XipParseFile(fileName, gHandler, xipResult, 1);
        XipDeleteCurrentXipResult(gHandler);
	if (res != 0) {
		cout << "parsing error: " <<res <<endl;
	}
}

void setUtf8() {
        XipAddFlagDisplayMode(0, CHECK_INPUT_UTF8, 80);
        //XipAddFlagDisplayMode(ghFR, CHECK_INPUT_UTF8, 80);
        //XipAddFlagDisplayMode(ghGE, CHECK_INPUT_UTF8, 80);
        //XipAddFlagDisplayMode(ghSW, CHECK_INPUT_UTF8, 80);
}

void setGuesser() {
        XipAddFlagDisplayMode(0, LANGUAGE_GUESSER , 80);
        //XipAddFlagDisplayMode(ghFR, LANGUAGE_GUESSER , 80);
        //XipAddFlagDisplayMode(ghGE, LANGUAGE_GUESSER , 80);
        //XipAddFlagDisplayMode(ghSW, LANGUAGE_GUESSER , 80);
}

int main (int argc, char *argv[]) {
	try {
		loadGrammars();
		setUtf8();
		setGuesser();
		int ghbs=ghEN;
		for (int i=0; i<10000; i++) {
			cout <<"count=" <<i <<endl;
			string base="C:\\XIP\\Test\\SampleLogGalateas\\corpus\\";
			decompte=i;
			parse(ghbs,base+"english.txt");
			parse(ghbs,base+"french.txt");
			parse(ghbs,base+"german.txt");
			parse(ghbs,base+"swedish.txt");
			parse(ghbs,base+"largeEnglish");
		}
	} catch (XIPError* xipError) {
        	cerr << "Error:" << xipError->message << endl;
		exit(-1);
	}

	//XipFreeGrammar(ghEN);
	//XipFreeGrammar(ghGE);

	cout<<"FIN"<<endl;
	while(1);
}
