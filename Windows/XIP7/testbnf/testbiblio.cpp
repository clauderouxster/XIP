// testbiblio.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define XIPLIBXML
#include "xipstlres.h"
#include "pasofficiel.h"

class garde {
public:
	XipResult* xip;
	
	garde() {
		xip=NULL;
	}
};

void appelXML(int ipar, XipResult* xip,void* data) {
	string tagname;
	vector<string> properties;

	XipGetCurrentXMLNodeData(ipar,tagname,properties);
	int i;
	cout<<(char*)tagname.c_str()<<endl;
	for (i=0;i<properties.size();i+=2) {
		cout<<(char*)properties[i].c_str()<<":";
		cout<<(char*)properties[i+1].c_str()<<endl;
	}
	for (i=0;i<xip->sentences[0]->leaves.size();i++)
		cout<<((XipLeaf*)(xip->sentences[0]->leaves[i]->daughters[0]))->surface<<" ";

	cout<<endl<<endl;
	XipCleanCurrentXipResult(ipar);
}

void appel(int ipar, XipResult* xip,void* data) {
	XipCreateCurrentXipResult(ipar);
	XipCleanCurrentXipResult(ipar);
	cout<<"Essai de Nettoyage"<<endl;
}

void appelxml(int ipar, XipResult* xip,void* data) {
	XipCleanCurrentXipResult(ipar);
	cout<<"Nettoyage"<<endl;
}

void parsexml(string& filename) {
	ostringstream os;
	XipParseFileXMLOS(filename,0, &os,0,XML_NONE,1);
	cout<<(char*)os.str().c_str()<<endl;
}

void parsexmlstring(string& texte) {
	ostringstream os;
	XipParseStringXMLOS(texte,0, &os,0,XML_NONE,1);
	cout<<(char*)os.str().c_str()<<endl;
}


void chargement() {
	char ch[1000];
	string me;
	Whoami(me);
	cout<<me<<endl;
	int ipar=XipGrmFile("R:\\XIPCVS\\parSem\\xip\\dev\\GRAMMARS\\ENGLISH\\GRMFILES\\GRAM_GEN\\gram_gen.grm");
	XipResult* xip=XipCreateCurrentXipResult(ipar);
	//XipSetCallBack(ipar,appel,NULL);
	XipSetCallBackXML(ipar,appelXML,NULL);
	ostringstream os;
	XipParseXMLFile(ipar,"R:\\XIPCVS\\parSem\\xip\\dev\\GRAMMARS\\ENGLISH\\GRMFILES\\GRAM_GEN\\tempo.xml",0,xip);
	
	XipDeleteCurrentXipResult(ipar);

	//garde g;
	//XipResult* xip=XipCreateCurrentXipResult(0);
	//g.xip=xip;
	//XipSetCallBack(0,appel,&g);
	//XipSetCallBackXML(0,appelxml,NULL);

	//XipResult* xp=XipGetCurrentXipResult(0);
	//

	//XipParseXMLFile(0,"C:\\XIP\\Test\\english\\test\\reut.xml",0,xp);

	//ostringstream osx;
	//XipParseXMLFile(0,"c:\\xip\\test\\sorties\\essai.xml",0,xp);
	
	//strcpy(ch,"C:\\XIP\\Test\\english\\AllEntit.txt");
	//cout<<ch<<endl;
	//XipParseFile(ch,0,xp,1);		

	//for (i=0;i<20000;i++)
	//	for (int j=0;j<20000;j++) {}

	//for (i=0;i<20000;i++) {
	//	sprintf(ch,"C:\\XIP\\patron_reuters\\reuters\\sgm\\res\\news-%05d.txt",i);
	//	cout<<ch<<endl;
	//	xp=XipGetCurrentXipResult(0);
	//	//if (g.xip!=xp) {
	//	//	cout<<"ERRREUUURRR AAAAAAAA"<<endl;
	//	//	printf("%x : %x\n",NULL,(long)xp);
	//	//}
	//	XipParseFile(ch,0,xp,1);		
	//	//xp=XipGetCurrentXipResult(0);
	//	//if (g.xip!=xp) {
	//	//	cout<<"ERRREUUURRR BBBBBBBBBBB"<<endl;
	//	//	printf("%x : %x\n",(long)g.xip,(long)xp);
	//	//}
	//}
	XipFreeGrammar(0);
}

int _tmain(int argc, _TCHAR* argv[])
{

	
	
	chargement();	
	
	return 0;
}

