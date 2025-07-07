/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003, Xerox Corporation, XRCE-Grenoble.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIPWIN
   Version    : 3.16
   filename   : kifrulegen.h
   Date       : 10/01/2000
   Purpose    : To kifrulegen a rule
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef kifrulegen_h
#define kifrulegen_h


#define DEPENDENCYRULE 0
#define SEQUENCERULE 1
#define LEFTCONTEXT 2
#define RIGHTCONTEXT 3
#define IDRULE 4
#define TAGGINGRULE 5
const int fieldnumber=17;


class KifSelectedNode {
public:

	vector<string> features;    
	int index;
    Noeud* node;
    char genere;
    char any;
    
    char addFeature(string&,char);
    KifSelectedNode(Noeud* n,int i,vector<string>& f) {
		features=f;
        index=i;
        node=n;
        genere=0;        
        any=0;
    }

	KifSelectedNode(Noeud* n,int i) {
        index=i;
        node=n;
        genere=0;        
        any=0;
    }
};

class KifSelectedDependency {
public:

    int index;
    ResultatFonction* dependency;
    vector<string> features;    
    void addFeature(string&);

    KifSelectedDependency(ResultatFonction* d,int i) {
        index=i;
        dependency=d;        
    }

    KifSelectedDependency(ResultatFonction* d,int i,vector<string>& f) {
		features=f;
        index=i;
        dependency=d;        
    }
};


class KifRuleGeneration {
public:

    VECTA<int> numvar;
    vector<string> categorydependency;
    int num_var;
    string tab;
	bool addquestionmark;

	KifRuleGeneration() {
		num_var=1;
		addquestionmark=true;
	}

    char buildrule(VECTA<KifSelectedNode*>& choices, 
        int& i, 
        string& rule_text,
        int nbelements,
        Noeud* firstnd, 
        char typerule);
    
    char kifrulegenRule(VECTA<int>& focus,
        string& rule_text,
        VECTA<KifSelectedNode*>& nodeselection,
        VECTA<KifSelectedDependency*>& dependencyselection,
        Noeud* boundary,
        string dependencyname,
        int dependencysize,
        char context);

    void builddependency(VECTA<KifSelectedNode*>& choices, 
        int& i, 
        string& rule_text,                                    
        Noeud* mothernode,
        char& addif);
       

    void kifrulegenRuleCategory(int ipar,string filename,vector<string>& categories);
};


extern char* field_name[];
extern char* noms_champ[];
extern char* rule_name[];
extern char* language_fields[];
extern char* champs_langue[];

#endif
