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
   filename   : xmlxip.h
   Date       : 21/01/2002
   Purpose    : Derivation of GlobalParseur to handle XML input files.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef xmlxip_h
#define xmlxip_h

#ifdef XMLINPUT

class Stocke_Variable {
public:
    Variable* var;
    double val;
    ResultatFonction* rf;
    char* nom;

    Stocke_Variable(Variable* v,double vv,char* n) {
        val=vv;
        var=v;
        rf=NULL;
        nom=DupliqueChaine(n);
    };

    ~Stocke_Variable() {
        libere(nom);
    }
};

//-------------------------------------------------------
class GlobalParseurXML : public GlobalParseur {
 public:
    
    GlobalParseurXML(char* filename,char* path);
    ~GlobalParseurXML();

    void LireUneBalise(char* ligne,char init);
    char LireTraits(Traits*,char*,char*);
    Liste* LireNoeud(char* balise,Liste* pere,VECTA<Noeud*>&,char& finbalise);
    
    virtual typeParseur Type() {return PARSEURXML;}
    
    void LireDependances(char* balise,VECTA<Noeud*>& LesNoeuds,VECTA<Stocke_Variable*>& StockeVar);
    void SentenceAnalysis(char renumerote,char tree);
    unsigned char LirePCDATA(char*);
    void LireBaliseVariable(char* balise,const char*);
    void LireReadings(char* balise,Liste* lpere,char* lemme,char* surface,
        VECTA<Stocke_Variable*>& StockeVar,char&,char&);

#ifdef XIPLIBXML
    void BuildNode(Liste* pere,LListe& Phrase,xmlNodePtr noeud,VECTA<Liste*>&);
    void BuildNodeLeaf(Liste* pere,xmlNodePtr noeud,VECTA<Liste*>&);
    Traits* BuildFeature(xmlNodePtr features,VECTA<Stocke_Variable*>& StockeVar,xmlNodePtr* suivant );    
    void BuildDependency(xmlNodePtr dependencies,VECTA<Stocke_Variable*>& StockeVar,VECTA<Liste*>&);
    void TraduitChaine(char* chaine);

    void AnalyseXipXMLFilename(char* filename);
    void AnalyseDOCXML(xmlDocPtr doc);
    void AnalyseXipXMLString(unsigned char* chaine);
#endif

};

#endif
#endif
