/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the XIP library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : XIP
Version    : 10.00
filename   : transduct.h
Date       : 07/04/2016
Purpose    : XIPTRANS automaton
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef transduct_h
#define transduct_h

class GlobalParseurTrans;
class GlobalParseurHMM;
class Kif_Automaton;

class GlobalParseurTrans: public GlobalParseurHMM {
public:
	
	vector<string> filenames;
	vector<Kif_Automaton*> alls;

	charRead* currentreader;

	GlobalParseurTrans(vector<string>& ftrans, char* hmm, char* grammar, char* path);
	char lookup();
	~GlobalParseurTrans();
    virtual typeParseur Type() {return PARSEURTRANS;}

	void Process(charRead& w, vector<string>& res);
    void SentenceAnalysis(char renumerote,char tree);
    
    void Apply();  

    void StringAssociation(unsigned char* chaine);
    FILE* FileAssociation(FILE* fichier);

    void StartAnalysis();
    void CloseAnalysis();

	void reloading();

}; 


#endif
