/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.60
   filename   : stlresul.cxx
   Date       : 10/01/1999
   Purpose    : Implementation of the main methods exported by XIP API (STL Version)
   Programmer : Claude ROUX
   Reviewer   : 
*/

#if defined(XMLINPUT) && defined(XIPSTLAPI)
#include "parsbin.h"

//-----------------------------------------------------------------------
Exportation XipFeature::XipFeature(string a,string v) {
    attribute=a;
    value=v;
}


Exportation void XipFeature::print() {
    cout<<attribute<<":"<<value;
}

Exportation XipFeature::~XipFeature() {}

//-----------------------------------------------------------------------

Exportation XipLeaf::XipLeaf(string c,string s,string l, string initfeat,long g,long d, long gc,long dc) {
    category=c;
    surface=s;
    lemma=l;
	initialfeatures=initfeat;
    left=g;
    right=d;
	leftchar=gc;
	rightchar=dc;
}

Exportation void XipLeaf::print(char feature) {
    size_t i;
    cout<<category<<":"<<lemma;
    
   
    if (feature==OUI) {
        cout<<"("<<left<<"-"<<right<<")";
        if (features.size())
            cout<<"[";
        
        for (i=0;i<features.size();i++) {
            if (i!=0)
                cout<<",";
            features[i]->print();
        }
    
        if (features.size())
            cout<<"]";
    }    
}

Exportation XipLeaf::~XipLeaf() {};

Exportation XipLeaf::XipLeaf(const XipLeaf& x) {
    for (size_t i=0;i<x.features.size();i++)
        features.push_back(new XipFeature(x.features[i]->attribute,x.features[i]->value));
    category=x.category;
    lemma=x.lemma;
    surface=x.surface;
    left=x.left;
    right=x.right;
    index=-1;
}

Exportation void XipLeaf::print_daughters(char feature) {
    
    cout<<lemma<<" ";
}

Exportation XIPRESTYPE XipLeaf::Type() {return XIPLEAF;}

//-----------------------------------------------------------------------

Exportation XipNode::XipNode(const XipNode& x) : features(x.features) {
	idnode=-1;
    category=x.category;
    left=x.left;
    right=x.right;
    leftchar=x.leftchar;
    rightchar=x.rightchar;
	leftchar=
    index=-1;
    mother=NULL;
}


Exportation XipNode::XipNode(string s,long g,long d,long gc,long dc) {
	idnode=-1;
    category=s;
    left=g;
    right=d;
	leftchar=gc;
	rightchar=dc;
    mother=NULL;
    index=-1;
}

Exportation XipNode::~XipNode() {
    size_t i;
    for (i=0;i<features.size();i++)
        delete features[i];
    for (i=0;i<daughters.size();i++)
        delete daughters[i];
    
}


Exportation void XipNode::print(char feature) {
    size_t i;
    cout<<category;
 
    if (feature==OUI) {
        cout<<"("<<left<<"-"<<right<<")";
        if (features.size())
            cout<<"[";
        
        for (i=0;i<features.size();i++) {
            if (i!=0)
                cout<<",";
            features[i]->print();
        }
    
        if (features.size())
            cout<<"]";
    }
    
    if (daughters.size())
        cout<<"{";

    VXipNode::iterator da;
    int deb=1;
    for (da=daughters.begin();da!=daughters.end();da++) {
        if (!deb)            
            cout<<",";
        deb=0;
        XipNode* n=*da;
        n->print(feature);
    }
    
    if (daughters.size())
        cout<<"}";
}

Exportation void XipNode::print_daughters(char feature) {
    
    for (register size_t i=0;i<daughters.size();i++) {
        if (i!=0)
            cout<<" ";
        daughters[i]->print_daughters();
    }
}


Exportation XIPRESTYPE XipNode::Type() {return XIPNODE;}

//-----------------------------------------------------------------------

Exportation void XipDependency::print(char feature) {
    size_t i;
    cout<<name;
    if (features.size()) {
        for (i=0;i<features.size();i++)
            cout<<"_"<<features[i]->attribute;
    }
    cout<<"(";
    for (i=0;i<parameters.size();i++) {
        if (i)
            cout<<",";
        parameters[i]->print_daughters();
    }

    cout<<")"<<Endl;
}

Exportation XipDependency::XipDependency(string n) {
    name=n;
}

Exportation XipDependency::XipDependency(const XipDependency& x) : parameters(x.parameters) {
    name=x.name;
}


Exportation XipDependency::~XipDependency() {
    for (size_t i=0;i<features.size();i++)
        delete features[i];
}

//-----------------------------------------------------------------------

Exportation XipUnit::XipUnit() {
    root=NULL;
}

Exportation XipUnit::XipUnit(const XipUnit& x) : dependencies(x.dependencies) {
    root=x.root;        
}


Exportation XipUnit::~XipUnit() {    
	if (root!=NULL)
		delete root;    
	size_t i;
    for (i=0;i<dependencies.size();i++)
        delete dependencies[i];    
}

Exportation XipGraph::~XipGraph() {
	if (root!=NULL)
		delete root;
}


Exportation XipRelation::~XipRelation() {
	for(size_t i=0;i<concepts.size();i++)
		delete concepts[i];
}

Exportation XipConcept::~XipConcept() {
	for (size_t i=0;i<relations.size();i++)
		delete relations[i];
}


Exportation void XipUnit::print(char feature) {
    if (root==NULL)
        return;
    root->print(feature);
    cout<<Endl<<Endl;
    for (size_t i=0;i<dependencies.size();i++)
        dependencies[i]->print(feature);
}

//-----------------------------------------------------------------------

Exportation void XipResult::printbare(char feature) {
    if (sentences.size() >0) {
        XipUnit* un=sentences[0];
        XipNode* root = un->root;
        if (root!=NULL) {            
            for (size_t idau=0;idau<root->daughters.size();idau++) {
                XipNode* node=root->daughters[idau];
                cout<<"JMAX =\t"<<node->category<<Endl;
                size_t i;
                cout<<"XIP =\t";

                if (feature==OUI) {
                    if (node->features.size())
                        cout<<"[";
                    
                    for (i=0;i<node->features.size();i++) {
                        if (i!=0)
                            cout<<",";
                        cout<<node->features[i]->attribute;
                    }
                    
                    if (node->features.size())
                        cout<<"]";
                }
                cout<<Endl<<Endl;                    
            }
        }
    }
}

Exportation void XipResult::print(char feature) {
    
    for (size_t i=0;i<sentences.size();i++) {
        cout<<"Result: "<<i<<Endl;
        sentences[i]->print(feature);
        cout<<Endl<<Endl;
    }
}

Exportation XipResult::~XipResult() {
	size_t i;
    for (i=0;i<sentences.size();i++)
        delete sentences[i];
}

Exportation XipResult* XipResultCreate() {
    return new XipResult;
}


Exportation void XipResultDelete(XipResult* xip) {
    if (xip!=NULL)
        delete xip;
}

//Exporting DATA for exporting the structure of class
#ifdef XIPDLL_EXPORT
XipResult xipresult;
XipUnit xipunit;
XipNode xipnode("",0,0,0,0);
XipDependency xipdependency("");
XipFeature xipfeature("","");
#endif

void VariableLemmeIndex::exporter(Variable* vr,vector<XipFeature*>& resultats) {
    
    if (vr==NULL)
        return;
    char chiff[100];
    char nm[100];
    sprintf(nm,"#%s",vr->nom);
	XipFeature* feat=NULL;
	if (sorte==VLEMMEINDEX) {
		VariableGlobale* ch=(VariableGlobale*)vr;
		double res = ch->Valeur();

		if (ch->type==REEL)
			sprintf(chiff,"%f",res);
		else
			sprintf(chiff,"%d",(int)res);
		feat=new XipFeature(nm,chiff);
	}
	else {
		VariableString* vs=(VariableString*)vr;
		feat=new XipFeature(nm,vs->chaine);
	}
    
    resultats.push_back(feat);
}

void VariableLemmeIndex::exporte(vector<XipFeature*>& resultats) {
	for (int i=0;i<listes.dernier;i++) 
		exporter(listes[i],resultats);
}



#endif
