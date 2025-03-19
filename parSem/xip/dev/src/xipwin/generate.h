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
   filename   : generate.h
   Date       : 10/01/2000
   Purpose    : To generate a rule
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef generate_h
#define generate_h

class XipNode;
class XipResult;


#define DEPENDENCYRULE 0
#define SEQUENCERULE 1
#define LEFTCONTEXT 2
#define RIGHTCONTEXT 3
#define IDRULE 4
#define TAGGINGRULE 5
const int fieldnumber=17;


class SelectedNode {
public:

    int index;
    XipNode* node;
    vector<string> features;
    char genere;
    char any;
    
    char addFeature(string&,char);
    SelectedNode(XipNode* n,int i) {
        index=i;
        node=n;
        genere=0;        
        any=0;
    }
};

class SelectedDependency {
public:

    int index;
    XipDependency* dependency;
    vector<string> features;    
    void addFeature(string&);

    SelectedDependency(XipDependency* d,int i) {
        index=i;
        dependency=d;        
    }
};


class RuleGeneration {
public:

    XipUnit* xipsent;    
    VECTA<int> numvar;
    vector<string> categorydependency;
    int num_var;
    string tab;

    char buildrule(VECTA<SelectedNode*>& choices, 
        int& i, 
        string& rule_text,
        int nbelements,
        XipNode* firstnd, 
        char typerule);
    
    char GenerateRule(VECTA<int>& focus,
        string& rule_text,
        VECTA<SelectedNode*>& nodeselection,
        VECTA<SelectedDependency*>& dependencyselection,
        XipNode* boundary,
        string dependencyname,
        int dependencysize,
        char context);

    void builddependency(VECTA<SelectedNode*>& choices, 
        int& i, 
        string& rule_text,                                    
        XipNode* mothernode,
        char& addif);
    
    
    RuleGeneration(XipUnit* x) {
        xipsent=x;
    }

    void InternalGenerateRuleCategory(int ipar,string filename);
    void GenerateRuleCategory(int ipar,string filename,vector<string>& categories);
};


extern char* field_name[];
extern char* noms_champ[];
extern char* rule_name[];
extern char* language_fields[];
extern char* champs_langue[];

#endif
