/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003, Xerox Corporation, XRCE-Grenoble.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 3.16
   filename   : printresult.cxx
   Date       : 10/01/1999
   Purpose    : Implementation of the main methods exported by XIP API (VECTA Version)
   Programmer : Claude ROUX
   Reviewer   : 
*/

#if defined(XMLINPUT) && !defined(XMLSTLAPI)
#include "parsbin.h"

Exportation XipFeature::XipFeature(char* a,char* v) {
    attribute=DupliqueChaine(a);
    value=DupliqueChaine(v);
}


Exportation void XipFeature::print(ostream& os) {
    os<<attribute<<":"<<value;
}

XipFeature::~XipFeature() {
    libere(attribute);
    libere(value);
}

//-----------------------------------------------------------------------

Exportation XipLeaf::XipLeaf(const XipLeaf& x) {
    for (int i=0;i<x.features.dernier;i++) {
        XipFeature* feat=((XipLeaf)x).features[i];
        features.ajoute(new XipFeature(feat->attribute,feat->value));        
    }
    category=x.category;
    lemma=DupliqueChaine(x.lemma);
    surface=DupliqueChaine(x.surface);
    left=x.left;
    right=x.right;
    index=-1;
}

Exportation XipLeaf::XipLeaf(char* c,char* s,char* l, long g,long d) {
    category=DupliqueChaine(c);
    surface=DupliqueChaine(s);
    lemma=DupliqueChaine(l);
    left=g;
    right=d;
}

Exportation void XipLeaf::print(ostream& os,char feature) {
    int i;
    os<<category<<":"<<lemma;
    
   
    if (feature==OUI) {
        os<<"("<<left<<"-"<<right<<")";
        if (features.size())
            os<<"[";
        
        for (i=0;i<features.size();i++) {
            if (i!=0)
                os<<",";
            features[i]->print(os);
        }
    
        if (features.size())
            os<<"]";
    }    
}

Exportation void XipLeaf::print_daughters(ostream& os,char feature) {
    
    os<<lemma<<" ";
}

Exportation XipLeaf::~XipLeaf() {
    libere(lemma);
    libere(surface);
};

Exportation XIPRESTYPE XipLeaf::Type() {return XIPLEAF;}


//-----------------------------------------------------------------------
Exportation XipNode::XipNode(const XipNode& x) : features(x.features) {
    category=x.category;
    left=x.left;
    right=x.right;
    index=-1;
    mother=NULL;
}


Exportation XipNode::XipNode(char* s, long g,long d) {
    category=DupliqueChaine(s);
    left=g;
    right=d;
    mother=NULL;
}

Exportation XipNode::~XipNode() {
    libere(category);
    features.nettoie();
    daughters.nettoie();
}

Exportation void XipNode::print(ostream& os,char feature) {
    int i;
    os<<category;
    
    if (feature==OUI) {
        os<<"("<<left<<"-"<<right<<")";
        if (features.dernier)
            os<<"[";
        
        for (i=0;i<features.dernier;i++) {
            if (i!=0)
                os<<",";
            features[i]->print(os);
        }
    
        if (features.dernier)
            os<<"]";
    }
    
    if (daughters.dernier)
        os<<"{";
    for (i=0;i<daughters.dernier;i++) {
        if (i!=0)
            os<<",";
        daughters[i]->print(os,feature);
    }
    
    if (daughters.dernier)
        os<<"}";
}

Exportation void XipNode::print_daughters(ostream& os,char feature) {
    
    for (register int i=0;i<daughters.dernier;i++) {
        if (i!=0)
            os<<" ";
        daughters[i]->print_daughters(os);
    }
}

Exportation XIPRESTYPE XipNode::Type() {return XIPNODE;}

//-----------------------------------------------------------------------

Exportation void XipDependency::print(ostream& os,char feature) {
    int i;
    os<<name;
    if (features.dernier) {
        for (i=0;i<features.dernier;i++)
            os<<"_"<<features[i]->attribute;
    }
    os<<"(";
    for (i=0;i<parameters.dernier;i++) {
        if (i)
            os<<",";
        parameters[i]->print_daughters(os);
    }

    os<<")"<<Endl;
}

Exportation XipDependency::XipDependency(char* n) {
    name=DupliqueChaine(n);
}

Exportation XipDependency::XipDependency(const XipDependency& x) : parameters(x.parameters) {
    name=x.name;
}


Exportation XipDependency::~XipDependency() {
    libere(name);
    features.nettoie();        
}

//-----------------------------------------------------------------------

Exportation XipUnit::XipUnit() {
    root=NULL;
    language=NULL;
}

Exportation XipUnit::XipUnit(const XipUnit& x) : dependencies(x.dependencies) {
    root=x.root;        
}

Exportation XipUnit::~XipUnit() {
    delete root;
    dependencies.nettoie();    
    if (language!=NULL)
        libere(language);
}

Exportation void XipUnit::print(ostream& os,char feature) {
    if (root==NULL)
        return;
    root->print(os,feature);
    os<<Endl<<Endl;
    for (int i=0;i<dependencies.dernier;i++)
        dependencies[i]->print(os,feature);
}



Exportation void XipResult::printbare(ostream& os,char feature) {
    XipUnit* un=sentences[0];
    if (un !=NULL) {
        XipNode* root = un->root;
        if (root!=NULL) {            
            for (int idau=0;idau<root->daughters.dernier;idau++) {
                XipNode* node=root->daughters[idau];
                os<<"JMAX =\t"<<node->category<<Endl;
                int i;
                os<<"XIP =\t";

                if (feature==OUI) {
                    if (node->features.dernier)
                        os<<"[";
                    
                    for (i=0;i<node->features.dernier;i++) {
                        if (i!=0)
                            os<<",";
                        os<<node->features[i]->attribute;
                    }
                    
                    if (node->features.dernier)
                        os<<"]";
                }
                os<<Endl<<Endl;                    
            }
        }
    }
}

Exportation void XipResult::print(ostream& os,char feature) {
    
    for (int i=0;i<sentences.dernier;i++) {
        os<<"Result: "<<i<<Endl;
        sentences[i]->print(os,feature);
        os<<Endl<<Endl;
    }
}

Exportation XipResult::~XipResult() {
    sentences.nettoie();
}

Exportation XipResult* XipResultCreate() {
    return new XipResult;
}


Exportation void XipResultDelete(XipResult* xip) {
    if (xip!=NULL)
        delete xip;
}


void VariableLemmeIndex::exporter(VariableGlobale* ch,VECTA<XipFeature*>& resultats) {
    
    if (ch==NULL)
        return;
    char chiff[100];
    char nm[100];
    sprintf(nm,"#%s",ch->nom);
    double res = ch->Valeur();
    
    if (ch->type==REEL)
        sprintf(chiff,"%f",res);
    else
        sprintf(chiff,"%d",(int)res);
    
    
    XipFeature* feat=new XipFeature(nm,chiff);
    resultats.ajoute(feat);
}


void VariableLemmeIndex::exporte(VECTA<XipFeature*>& resultats) {
    for (int i=0;i<listes.dernier;i++)    
        exporter(listes[i],resultats);
}




//Exporting DATA for exporting the structure of class
#ifdef XIPDLL_EXPORT
XipResult xipresult;
XipUnit xipunit;
XipNode xipnode("",0,0);
XipDependency xipdependency("");
XipFeature xipfeature("","");

#endif

#endif

