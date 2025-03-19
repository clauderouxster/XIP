#include "xipstlres.h"

void callback(int ipar, XipResult* xip,void* data) {
      //cout<<"callback"<<endl;
      XipCleanCurrentXipResult(ipar);
}

int main(int argc, const char* argv[]) {
	//XipGrmFile("../../GRAMMARS/ENGLISH/GRMFILES/GRAM_NORM/gram_norm_entit.grm");
	XipGrmFile("../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm",1,0);
	XipSetCallBack(0,callback,NULL);
	char ch[10];
	XipResult* xip=XipCreateCurrentXipResult(0);
	string s="/opt/ParSem/Corpora/ENGLISH/REUTERSEXPANDED";
	string who;
	Whoami(who);
	cout <<"-------" <<(char *)who.c_str() <<endl;
	for (int i=1;i<19000;i++) {
	//for (int i=1;i<5;i++) {
		sprintf(ch,"/news-%05d",i);
		string p=s+ch+".txt";
		cout<<p<<endl;
		XipParseFile(p,0,xip,1);           
	}
	return 0;
}

 
