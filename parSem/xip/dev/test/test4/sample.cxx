#include <iostream>
#include <sstream>
#include <string>

#include "xipstlres.h"
#include "pasofficiel.h"

int main(int argc, const char* argv[]) {
//	string grammarFile = argv[0];
	string grammarFile = "../../GRAMMARS/SPANISH/GRMFILES/spanish.grm";
	string textInputString("Ahmed le esperaba en el mismo sitio con su coche.");
	//string textInputString("Peter is happy.");
	ostringstream *os = new ostringstream();
	int gHandler;
	try {
		gHandler = XipGrmFile(grammarFile, 0);
	} catch (XIPError* xipError) {
		cerr <<"XipGrmFileImplementation Error:" <<xipError->message <<endl;
		return -1;
	}
	cout << "JXipImpl::main() parsing the string :" <<textInputString  <<endl;
	int displayMode = 0;
	int col = 80;
	XipSetDisplayMode(gHandler, displayMode, col);

	int ruleNb = 146;   //not OK
	//int ruleNb = 140;   // OK
	//int ruleNb = 130;   //OK
	//int ruleNb = 147; //OK
	cout <<"XipSetRuleLimit rules Nb = " <<ruleNb <<endl;
	XipSetRuleLimit(gHandler, ruleNb);

	XipParseStringOS(textInputString, gHandler, os, 0, 1);
	cout << "JXipImpl::main() Result after parsing: \n" <<os->str() <<endl;

	VECTA<Label*>* liste_labels = XipReturnInfoRuleIndex(gHandler);
	for (int ind = 0; ind < liste_labels->dernier; ind++) {
		Label *label = liste_labels->vecteur[ind];
		DecompteRegle* dr = label->dr;
		if (dr != NULL) {
			cout <<"rules Nb = " <<ind <<endl;
			cout <<"fichier = " <<dr->nom_fichier <<" line= " <<dr->ligne <<endl;
		}
	}

	vector<int> applyedRulesNb;
	XipReturnRules(gHandler, applyedRulesNb);
	int applyedRuleNb = applyedRulesNb.size() - 1;
	cout <<endl <<"applyed rules Nb = " <<applyedRuleNb <<endl;
	for (int ind=applyedRuleNb; ind >= 0; ind--) {
		int ruleApplyedNb = applyedRulesNb[ind];
		cout <<"ruleNb = " <<ruleApplyedNb <<endl;
	}

	return 0;
}
