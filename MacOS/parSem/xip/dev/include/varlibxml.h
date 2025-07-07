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
   filename   : varlibxml.h
   Date       : 17/02/2003
   Purpose    : Manipulation of XML files as XIP objects
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef varlibxml_h
#define varlibxml_h

#ifdef XIPLIBXML

#include "avltemplate.h"
#include "automate.h"

typedef enum {
        XMLUNKNOWN,
        XMLASSIGNATION,
        XMLASSIGNATIONCHAMP,
        XMLSAUVEGARDE,
        XMLCOMPARAISON,
        XMLCONTENU,
        XMLCHAMP, 
        XMLTEST,
        XMLCREATION,
        XMLCOMPARAISONSOUSCHAINE,
        XMLCREATIONATTRIBUT,        
        XMLRENOMMER,
        XMLFFEATURE,
        XMLFANALYZE,
        XMLFMERGE,
        XMLFSKIP,
        XMLNETTOYAGE,
        XMLINSERTION,
        XMLMETNAMESPACE,
        XMLTOKENIZE,
        XMLCREATIONAVECAJOUT,
        XMLLITDONNEES,
        XMLPOSITIONPARENT,
		XMLMETDANSVAR,
		XMLTOKENIZEATTRIBUTE,
		XMLINITIALHIT,
		XMLHIT,
} INSTRUCTION;


class noeudXML {
public:    
    xmlNodePtr node;    
    noeudXML(xmlNodePtr n) {
        node=n;
    }

};



//Cette classe permet la gestion d'une arborescence
//Nous allons gerer une arborescence sous la forme 
//d'un automate qui reconnait les XPaths...
class arborescenceXML {

public:

    //Il s'agit de la categorie la plus englobante
    //C'est aussi celle qui sert de point d'acces a la recherche
    //d'information
    unsigned char* pointentree;
    int principal;
    int index;
    map<string,xmlXPathObjectPtr> lesindex;    
	string lesclefs[100];
	long taille;
	long tailleinitiale;

    xmlDocPtr document; 
    xmlXPathContextPtr xpath;
    char* nomfichier;    
    char ajout;
    char reference;

    xmlNodePtr XmlNodeCourant;
    xmlNodePtr XmlCourant;
    xmlNodePtr XmlReferenceNode;        
	xmlXPathContextPtr xpathcourant;

	xmlXPathObjectPtr recherche(string clef);
	void concatene(string clef,xmlNodePtr courant,long value=-1);
    arborescenceXML(xmlDocPtr doc,const unsigned char* pe,int rac,char* nf=NULL);
	arborescenceXML() {
		taille=0;
		XmlNodeCourant=NULL;
		XmlCourant=NULL;
		XmlReferenceNode=NULL;        
		xpathcourant=NULL;

		document=NULL;        
		pointentree=NULL;
		principal=0;
		xpath=NULL;        
		nomfichier=NULL;
		index=-1;
		ajout=NON;
		reference=NON;
		nomfichier=NULL;
	}


    void creeXPathContext();
    xmlXPathObjectPtr EvalueXPath(xmlNodePtr,const uchar*);           
    void indexer(VECTA<XPathIndexation*>&,char);
    void retrouveXmlPrivate(xmlNodePtr ancetre,
        xmlNodePtr node,
        uchar* laclef,
        VECTA<uchar*>& attributs,
		int maxelements);
    
    ~arborescenceXML();    
    void trouveIndex(VECTA<XPathIndexation*>& xpathindex);
    void marquer(VECTA<XPathIndexation*>& xpathindex,clefXML* cle);
	void raz();
};



class xmlresultat {
public:

    int index;
    VECTA<xmlNodePtr> xmlnodes;
    arborescenceXML* arboxml;

    void raz() {
        index=-1;
        xmlnodes.raz();
        arboxml=NULL;
    }
    
    void ajoute(int i,arborescenceXML* a,xmlNodePtr n) {
        xmlnodes.ajoute(n);
        index=i;
        arboxml=a;
    }

    inline xmlNodePtr operator [](unsigned int i) {
        return xmlnodes[i];
    }

    inline int dernier() {
        return xmlnodes.dernier;
    }

};



class eleXML {
public:    

    char nature;
    uchar tampon[2];

    eleXML() {nature=0;}
    virtual ~eleXML() {}

    virtual XMLTYPE Type() {return XMLELEMENT;}
    virtual INSTRUCTION Methode() {return XMLUNKNOWN;}  
    virtual unsigned char* Chaine() {
        tampon[0]=nature;
        tampon[1]=0;
        return tampon;
    }
    virtual char AnalyseVariable(GlobalParseur*,string& valeur,
        VECTA<Noeud*>& vectNoeuds,
        VECTA<ResultatFonction*>& vectFonc) {
        return -1;
    }

    virtual char MAJVariable(GlobalParseur* parseur,char* valeur,VECTA<Noeud*>& vectNoeuds) {return 0;}
    virtual double TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& vectNoeuds,string&,VECTA<ResultatFonction*>& focus) {return 0;}
    virtual char RecupereValeur(GlobalParseur* parseur,string& valeur,
        VECTA<Noeud*>& vectNoeuds,
        VECTA<ResultatFonction*>& vectFonc) {return 0;}
    virtual VARIABLEXML Variable() {return XMLVARINONE;}
    virtual Noeud* RenvoieNoeud(VECTA<Noeud*>& vectNoeuds) {return NULL;}
    virtual char ExecuteMethode(GlobalParseur* parseur,
        arborescenceXML* arboxml,
        xmlNodePtr pere,
        xmlNodePtr node,
        VECTA<uchar*>& lesresultats,
        VECTA<Noeud*>& argNoeuds,
        VECTA<ResultatFonction*>& vectFonc) {return 0;}
};



class variXML : public eleXML {
public:

    VARIABLEXML variable;
    XMLTYPE Type() {return XMLVARI;}

    variXML(char* v);

    VARIABLEXML Variable() {return variable;}
};


class chaXML : public eleXML {
public:

    unsigned char* chaine;
    XMLTYPE Type() {
		return XMLCHAINE;
	}

    chaXML(unsigned char* c,char maj);

    unsigned char* Chaine();
    virtual ~chaXML() {
		libere(chaine);
	}
};


class mathXML : public eleXML {
public:

    TestFormule* formule;
    XMLTYPE Type() {return XMLMATH;}
    mathXML() {formule=NULL;}
    ~mathXML() {if (formule!=NULL) delete formule;}
    double TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& vectNoeuds,string&,VECTA<ResultatFonction*>& focus);
    unsigned char* Chaine() {return (uchar*)"MATH";}
    Noeud* RenvoieNoeud(VECTA<Noeud*>& vectNoeuds);
};

class indXML : public eleXML {
public:

    TestFonctionNoeud* test;
    char AnalyseVariable(GlobalParseur*,string& valeur,VECTA<Noeud*>& vectNoeuds,VECTA<ResultatFonction*>& vectFonc);
    char MAJVariable(GlobalParseur* parseur,char* valeur,VECTA<Noeud*>& vectNoeuds);
    char RecupereValeur(GlobalParseur* parseur,string& valeur,VECTA<Noeud*>& vectNoeuds,VECTA<ResultatFonction*>& vectFonc);
    Noeud* RenvoieNoeud(VECTA<Noeud*>& vectNoeuds) {
        return vectNoeuds[test->arggauche];
    }

    XMLTYPE Type() {return XMLINDEX;}
    unsigned char* Chaine() {return (uchar*)"TEST#";}
    indXML() {test=NULL;}
};


class clefXML : public eleXML {
    
public:
    
    VECTA<eleXML*> regles;
    char sauvegarder;
    char indexation;
    char reindexation;
    int index;
    
    unsigned char* Clef() { return regles[0]->Chaine();}
    
    clefXML() {
        sauvegarder=NON;
        indexation=0;
        reindexation=NON;
        index=-1;
    }

    clefXML(unsigned char* c) {
        sauvegarder=NON;
        indexation=0;
        reindexation=NON;
        chaXML* chxml=new chaXML(c,1);
        regles.ajoute(chxml);
        index=-1;
    }
    void ConstruitLaClef(GlobalParseur* parseur,
        string& laclef,
        VECTA<Noeud*>& vectNoeuds,
        VECTA<ResultatFonction*>& vectFonc);
    int ConstruitClefMinimale(GlobalParseur*,string* lesclefs);
    char VerifieCompatibilite(clefXML*);

    int ConstruitClef(GlobalParseur*,
        arborescenceXML* arboxml,
        string* lesclefs,
        VECTA<Noeud*>&,
        VECTA<ResultatFonction*>&,
        int premierElement,
        int dernierElement,
        int& nbelement);

    int ConstruitUneClef(GlobalParseur*,
        arborescenceXML* arboxml,
        string& cle,
        VECTA<Noeud*>&,
        VECTA<ResultatFonction*>&,
        int premierElement,int dernierElement);

    virtual XMLTYPE Type() {return XMLCLEF;}
    virtual ~clefXML() {regles.nettoie();}    
    
    char AnalyseClefNodeXML(GlobalParseur* parseur,
        xmlNodePtr node,
        int position,
        arborescenceXML* arboxml,
        VECTA<uchar*>& lesresultats,
        VECTA<Noeud*>& argNoeuds,
        VECTA<ResultatFonction*>& vectFonc);
    

    char CreerNoeudXML(GlobalParseur*,arborescenceXML* arboxml,
        xmlNodePtr pere,VECTA<Noeud*>& vectNoeuds,VECTA<ResultatFonction*>& vectFonc);

    int TestNode(GlobalParseur* parseur,
        VECTA<Noeud*>& vectNoeuds,
        VECTA<ResultatFonction*>& vectFonc,
        xmlNodePtr node,
        xmlXPathContextPtr xpath);

    char ExecuteClef(GlobalParseur* parseur,
        arborescenceXML* arboxml,
        VECTA<uchar*>& lesresultats,
        VECTA<Noeud*>& argNoeuds,
        VECTA<ResultatFonction*>& vectFonc,
        xmlresultat* vectRes);    

    virtual unsigned char* Chaine() {return (uchar*)"CLEF";}
   
    
};

class metXML : public clefXML {
public:
    INSTRUCTION methode;
    metXML(char* c);
    XMLTYPE Type() {return XMLMETHODE;}
    unsigned char* Chaine();
    INSTRUCTION Methode() {return methode;}
    char ExecuteMethode(GlobalParseur* parseur,
        arborescenceXML* arboxml,
        xmlNodePtr pere,
        xmlNodePtr node,
        VECTA<uchar*>& lesresultats,
        VECTA<Noeud*>& argNoeuds,
        VECTA<ResultatFonction*>& vectFonc);
    
};



class dtdXML {
public:    
    eleXML* clef;
    char* nom;
    XMLEVENEMENT evenement;
    XMLACTION action;
    trait* tr;
    char* vide;
    char* marquage;
	string attribut;

    char* Chaine() {return (char*)clef->Chaine();}
    dtdXML(clefXML* n,XMLEVENEMENT ev);
    dtdXML(char* n,XMLEVENEMENT ev);
    ~dtdXML();
};


class XPathIndexation {
public:

    unsigned char* clef;
    VECTA<clefXML*> listeXpath;
    VECTA<unsigned char*> listeCleMinimale;

    XPathIndexation(clefXML* cxml);
    ~XPathIndexation();
};

class aligne {
public:

    xmlNodePtr noeud;
    int gauche;
    int droit;    
    char profondeur;
	int num;

    aligne(xmlNodePtr n,int g, int d, char pr) {
		num=-1;
        noeud=n;
        gauche=g;
        droit=d;        
        profondeur=pr;
    }
};


#endif
#endif
