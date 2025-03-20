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
   filename   : defun.h
   Date       : 10/03/2002
   Purpose    : Mathematic formulae handling
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef mathemat_h
#define mathemat_h

#include "automate.h"
#include "avltemplate.h"
#include "compatible.h"

class KifElement;
class KifFunction;
class ResultatFonction;
class TestFormule;
class XipFeature;
class graphe;


typedef enum {TESTFONCTION,
              TESTFONCTIONDEPENDANCE,
              TESTFONCTIONNOEUD,
              TESTFONCTIONMATH,
              TESTFONCTIONXML,
              TESTFONCTIONAFFICHAGE,
              TESTFONCTIONPROCEDURE,
              TESTFONCTIONMATHCHAINE,
              TESTFONCTIONAUTOMATE,
              TESTFONCTIONBOUCLE,
              TESTFONCTIONPYTHON,
			  TESTFONCTIONKIF,
			  TESTFONCTIONGRAPHE,
			  TESTRACINEGENERATION,
			  TESTNOEUDGENERATION} typeTESTFONCTION;
//---------------------------------------------------------------------------------
class unindice {
public:
    
    int index;
    int indice;

    unindice(int idx,int idi) {
        index=idx;
        indice=idi;
    };
};
//---------------------------------------------------------------------------------
class Variable {

 public:
    RAMASSAGE(Variable);
    char* nom;    
    int position;
    TestFormule* defaut;
    sorteVariable sorte;
    char utilisee;
    char reinit;
    char procedure;
    typeVariable type;


    Variable(char* n,typeVariable,sorteVariable,char);
    virtual ~Variable();
    virtual void raz() {};

    virtual double Valeur() {return 0;}
    virtual double Valeurs(Noeud* n) {return 0;}    

    virtual int Taille() {return 0;}
    virtual void Affecte(int i,double v) {};
	virtual string valdep(ResultatFonction* rf) {return "";}
    virtual void AffecteValeur(double v) {}
    virtual void AffecteValeurString(unsigned char* v) {}
    virtual int AjouteDependance(ResultatFonction* rf,char ajout=OUI) {return -1;}; 
    virtual int AjouteNoeud(Noeud* n,char ajout=OUI) {return -1;};
    virtual void affiche(ostream& os) {};
    virtual void afficheXML(ostream& os) {};
    virtual void afficheNoeudXML(ostream& os,Noeud*) {};
	virtual void affichageDependanceXML(ostream& os,ResultatFonction* rf) {}
};


#ifdef XIPPYTHON
class VariablePython : public Variable {
public:
    PyObject* echange;
    VariablePython(char* n, typeVariable ty,sorteVariable l,PyObject* e) : Variable(n,ty,l,NON) {
        echange=e;
    }
    ~VariablePython();
};
#endif

//----------------------------------------------------------------------
//Declaration de type: string var;

class VariableString : public Variable {
public:

    string chaine;
    VariableString(char*);
    ~VariableString();    
    void raz();
    void AffecteValeurString(unsigned char* v);
    void affiche(ostream& os) {        
        os<<"string ";
        os<<"\""<<nom<<"\" = "<<chaine<<";"<<Endl;
    }
    void afficheXML(ostream& os);
    void afficheNoeudXML(ostream& os,Noeud* n);

    
    Variable* clone() {
        if (nom[0]=='_')
            return this;
        VariableString* v=new VariableString(nom);
        v->chaine=chaine;
        v->reinit=OUI;
        return v;
    }
};
class VariableGraphe : public Variable {
public:
	graphe* g;

	VariableGraphe(char* n,typeVariable ty) : Variable(n,ty,VGRAPHE,NON) {
		g=NULL;
	}

    ~VariableGraphe();    
    void raz();
	void affiche(ostream& os);        
	void afficheXML(ostream& os);
	void afficheNoeudXML(ostream& os,Noeud* n);

    
	Variable* clone() {
		return this;
	}
};

class VariableKif : public Variable {
public:
	KifElement* kif;

	VariableKif(char* n,typeVariable ty) : Variable(n,ty,VKIFOBJET,NON) {
		kif=NULL;
	}

    ~VariableKif();    
	void raz() {
		kif=NULL;
	}
	void affiche(ostream& os) {}
	void afficheXML(ostream& os) {}
	void afficheNoeudXML(ostream& os,Noeud* n) {}

    
	Variable* clone() {
		return this;
	}
};

//----------------------------------------------
//Declaration de type: int var[]; ou vint var;

class VariableIntVector : public Variable {
public:

    VECTA<double> valeurs;    
    VariableIntVector(char*,typeVariable ty);
    
    virtual void raz();    
    void Affecte(int i, double v);    
    void affiche(ostream& os) {        
        os<<"int ";
        for (int i=0;i<valeurs.size();i++)
            os<<"\""<<nom<<"("<<i<<")\" = "<<valeurs[i]<<";"<<Endl;
    }

    void afficheXML(ostream& os);
    void afficheNoeudXML(ostream& os,Noeud* n);
    int recherchepremier(int);
    int recherchetous(int,VariableIntVector*);
    int retirevaleur(int i);
    int inserevaleur(double s,int i);

};

//----------------------------------------------
//Declaration de type: string var[]; ou vector var;

class VariableVector : public Variable {
public:

    VECTA<string*> chaine;    
    VariableVector(char*);
    virtual ~VariableVector();
    
	virtual void setutf8(bool u) {}
    virtual void raz();
    virtual void AffecteValeurStringIndex(string* v,int i);
    virtual void AffecteValeurStringIndexChaine(string* v,string ind) {};

    void affiche(ostream& os) {        
        os<<"string ";
        for (int i=0;i<chaine.size();i++)
            os<<"\""<<nom<<"("<<i<<")\" = "<<chaine[i]<<";"<<Endl;
    }

    void afficheXML(ostream& os);
    void afficheNoeudXML(ostream& os,Noeud* n);
    virtual int recherchepremier(string&);
    virtual int recherchetous(string&,VariableVector*);
    virtual int retirechaine(int i);
    virtual int inserechaine(string& s,int i);


};

//----------------------------------------------
//Declaration de type: int vindex var;

class VariableVectorIndex : public VariableVector {
public:

    automate index;
    void AffecteValeurStringIndex(string* v,int i);
    void raz();
    int recherchepremier(string&);
    int recherchetous(string&,VariableVector*);
    int retirechaine(int i);
    int inserechaine(string& s,int i);
    VariableVectorIndex(char* t) : VariableVector(t) {}
    ~VariableVectorIndex();
 };

class VariableVectorGraphe : public Variable {
public:
	VECTA<graphe*> graphes;

	VariableVectorGraphe(char* n,typeVariable ty) : Variable(n,ty,VVECTEURGRAPHE,NON) {}

	void raz();

	
 };

//----------------------------------------------
//Declaration de type: int dictionary var;
//Nous avons dans ce cas deux automates, l'un pour les chaines que l'on garde dans ce
//tableau, l'autre comme indice...

class VariableDictionnaire : public VariableVector {
public:
    
    map<string,string> dico;
    
    void AffecteValeurStringIndexChaine(string* v,string ind);
	char cherche(string& cle,string& valeur);
    void raz();    
    VariableDictionnaire(char* t) : VariableVector(t) {
        sorte=VDICTIONNAIRE;
    }
    ~VariableDictionnaire();
	char rechercheindex(int i,string& res);
    int recherchechaineindex(string indstr,string& res);
    int recherchetoutechaineindex(string indstr,VariableVector*);
    void AffecteValeurStringIndex(string* v,int i);
	int lesclefs(VariableVector* vvect);
	int lesvaleurs(VariableVector* vvect);
	string valeur(string& clef);
 };

//----------------------------------------------
//Declaration de type: int _var;

class VariableGlobale : public Variable {
 public:

    double valeur;
    VariableGlobale(char* n,typeVariable ty,sorteVariable sorte,char r) : Variable(n,ty,sorte,r) {
        valeur=0;
    }

    void raz() {valeur=0;utilisee=NON;}
    double Valeur() {return valeur;}
    void AffecteValeur(double v) {valeur=v;utilisee=OUI;}
    void affiche(ostream& os) {
        if (type == REEL)
            os<<"float ";
        else
            os<<"int ";
        os<<"\""<<nom<<"\" = "<<valeur<<";"<<Endl;
    }
    void afficheXML(ostream& os);
    void afficheNoeudXML(ostream& os,Noeud* n);
    
    Variable* clone() {
        if (nom[0]=='_')
            return this;
        VariableGlobale* v= new VariableGlobale(nom,type,VGLOBALEREINIT,OUI);
        v->valeur=valeur;
        return v;
    }
};

//----------------------------------------------
//Declaration de type: int var##;

class VariableLemmeIndex : public Variable {
 public:

    VECTA<Variable*> listes;
    hmap<string,Variable*> lemmes;
    
    VariableLemmeIndex(char* n, typeVariable ty,sorteVariable sorte) : Variable(n,ty,sorte,NON) {}
    
    VariableGlobale* recherche(unsigned char* chaine,int);
    VariableGlobale* insere(string& expression,unsigned char*,int);
	virtual VariableString* inserechaine(string& expression,unsigned char*,int) {return NULL;}
	virtual VariableString* recherchechaine(unsigned char* chaine,int) {return NULL;}
    void affiche(ostream& os);
    void afficheXML(ostream& os);
    void afficheNoeudXML(ostream& os,Noeud* n);


#ifdef XIPSTLAPI       
    void exporter(Variable* ch,vector<XipFeature*>& resultats);    
    void exporte(vector<XipFeature*>& resultats);
#else
    void exporter(VariableGlobale* ch,VECTA<XipFeature*>& resultats);
    void exporte(VECTA<XipFeature*>& resultats);
#endif
};

class VariableLemmeChaineIndex : public VariableLemmeIndex {
public:

	VariableLemmeChaineIndex(char* n, typeVariable ty,sorteVariable sorte) : VariableLemmeIndex(n,ty,sorte) {}
	VariableString* recherchechaine(unsigned char* chaine,int);
	VariableString* inserechaine(string& expression,unsigned char*,int);
};

class VariableIndexee : public Variable {
 public:

    VariableIndexee(char* n,typeVariable ty,sorteVariable sorte) : Variable(n,ty,sorte,OUI) {}

    VECTA<double> valeurs;
    int Taille() {return valeurs.dernier;}
    void Affecte(int i,double v) {valeurs.affecte(i,v);utilisee=OUI;};
    void afficheXML(ostream& os);

};

class VariableIndexeeChaine : public Variable {
 public:

    VariableIndexeeChaine(char* n,typeVariable ty,sorteVariable sorte) : Variable(n,ty,sorte,OUI) {}

    VECTA<string*> valeurs;
	VECTA<Noeud*> indexes;

	void raz();
    int Taille() {
		return valeurs.size();
	}

	void Affecte(int i,string v) {
		utilisee=OUI;
		if (valeurs[i]==NULL)
			valeurs.affecte(i,new string(v));
		else
			*valeurs[i]=v;
	}
	~VariableIndexeeChaine() {
		valeurs.nettoie();
	}
    int AjouteNoeud(Noeud* n,char ajout=OUI);    
	void afficheNoeudXML(ostream& os,Noeud*);

};

class VariableIndexeeChaineDependance : public Variable {
 public:

    VECTA<ResultatFonction*> indexes;
	VECTA<string*> valeurs;
    VariableIndexeeChaineDependance(char* n,typeVariable ty,sorteVariable sorte) : Variable(n,ty,sorte,OUI) {}
    void raz() {
        valeurs.nettoie();
        indexes.raz();
        utilisee=NON;
    }

	void Affecte(int i,string v) {
		utilisee=OUI;
		if (valeurs[i]==NULL)
			valeurs.affecte(i,new string(v));
		else
			*valeurs[i]=v;
	}
	~VariableIndexeeChaineDependance() {
		valeurs.nettoie();
	}
    int AjouteDependance(ResultatFonction* rf,char ajout=OUI);
    string valdep(ResultatFonction* rf);
    void affichageDependanceXML(ostream& os,ResultatFonction* rf);
};


class VariableIndexeeDependance : public VariableIndexee {
 public:

    VECTA<ResultatFonction*> indexes;
    VariableIndexeeDependance(char* n,typeVariable ty,sorteVariable sorte) : VariableIndexee(n,ty,sorte) {}
    void raz() {
        valeurs.raz();
        indexes.raz();
        utilisee=NON;
    }
    int AjouteDependance(ResultatFonction* rf,char ajout=OUI);
    string valdep(ResultatFonction* rf);
    void affichageDependanceXML(ostream& os,ResultatFonction* rf);
};

class VariableIndexeeNoeud : public VariableIndexee {
 public:

    VECTA<Noeud*> indexes;
    double Valeurs(Noeud* n);

    VariableIndexeeNoeud(char* n,typeVariable ty,sorteVariable sorte) : VariableIndexee(n,ty,sorte) {}
    void raz() {
        valeurs.raz();
        indexes.raz();
        utilisee=NON;
    }
    int AjouteNoeud(Noeud* n,char ajout=OUI);    
    void afficheNoeudXML(ostream& os,Noeud*);

};


//----------------------------------------------------------------------------------
class TestFormuleElementVariable;
class TestFormuleElementDependance;
class TestFormuleElementChaineNoeud;
class TestFormuleElementChaineDependance;

class TestFormuleElement {
 public:
    RAMASSAGE(TestFormuleElement);
    sorteVariable type;    
    
    TestFormuleElement() {
		RAMASSE(TestFormuleElement);
        type=VAUCUN;
    };    

	virtual void setutf8(bool u) {};
    virtual int valeur_operation() {return -1;};
    virtual double valeur_reel() {return -1;};
    virtual char* valeur_valstring() {return NULL;};
    virtual TestFormuleElementVariable* valeur_variable() {return NULL;} 
    virtual TestFormuleElementChaineNoeud* valeur_variable_chaine() {return NULL;} 
    virtual TestFormuleElementDependance* valeur_dependance() {return NULL;}
	virtual TestFormuleElementChaineDependance* valeur_dependance_chaine() {return NULL;}
    virtual VariableGlobale* valeur_global() {return NULL;}
    virtual VariableLemmeIndex* valeur_lemme() {return NULL;}
    virtual VariableString* valeur_string() {return NULL;}    
    virtual int argument(GlobalParseur* parseur,VECTA<Noeud*>& argNoeuds) {return -1;}
    virtual int arg() {return -1;}
	virtual void metPatron(graphe* g) {}
	virtual void remetPatron() {}
    virtual ~TestFormuleElement() {
		JETTE(TestFormuleElement);
	};    
    virtual char assigne(Variable* v) { return NON;}
};



class TestFormuleElementKif : public TestFormuleElement {
public:
    VariableKif* vp;
    TestFormuleElementKif(VariableKif* v) {
        type=VKIFOBJET;
        vp=v;
    }
};

#ifdef XIPPYTHON
class TestFormuleElementPython : public TestFormuleElement {
public:
    VariablePython* vp;
    TestFormuleElementPython(VariablePython* v) {
        type=VPYTHONOBJET;
        vp=v;
    }
};
#endif

class TestFormuleElementGraphe : public TestFormuleElement {
public:
    VariableGraphe* vg;
	graphe* patron;
	graphe* ancien;
    TestFormuleElementGraphe(VariableGraphe* v,graphe* g) {
		if (g==NULL)
			type=VGRAPHE;
		else
			type=VGRAPHECREATION;
        vg=v;
		patron=g;
    }
	graphe* clone(VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* focus);
	void metPatron(graphe* g);
	void remetPatron();
	~TestFormuleElementGraphe() {}

};

class TestFormuleElementGlobal : public TestFormuleElement {
 public:

    VariableGlobale* var;
    TestFormuleElementGlobal(VariableGlobale* v=NULL) {
        type=VGLOBALE;
        var=v;
    }
    
    VariableGlobale* valeur_global() {
        return var;
    }
    
    char assigne(Variable* v) {
        var=(VariableGlobale*)v; 
        //On change le type
        type=var->sorte;
        return OUI;
    }
};



class TestFormuleElementString : public TestFormuleElement {
 public:

    VariableString* var;
    TestFormule* indice_chaine;
    TestFormuleElementString(VariableString* v,TestFormule* ic) {
        type=VCHAINE;
        var=v;
        indice_chaine=ic;
    }
    
    VariableString* valeur_string() {return var;}
    ~TestFormuleElementString();
    char assigne(Variable* v) {var=(VariableString*)v; return OUI;}
};

class TestFormuleElementIntVector : public TestFormuleElement {
 public:

    VariableIntVector* var;
    TestFormule* indice_vecteur;
    TestFormuleElementIntVector(VariableIntVector* v,TestFormule* iv) {
        type=VVECTEURINT;
        indice_vecteur=iv;
        var=v;
    }        
    
    ~TestFormuleElementIntVector();
    char assigne(Variable* v) {var=(VariableIntVector*)v; return OUI;}
};

class TestFormuleElementVectorGraphe : public TestFormuleElement {
 public:

    VariableVectorGraphe* var;
	graphe* patron;
	graphe* ancien;
    TestFormule* indice_vecteur;
    TestFormuleElementVectorGraphe(VariableVectorGraphe* v,TestFormule* iv,graphe* p) {
		patron=p;
		if (patron==NULL)
			type=VVECTEURGRAPHE;
		else
			type=VVECTEURGRAPHECREATION;
        indice_vecteur=iv;
        var=v;
    }        
	graphe* clone(VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* focus);
	void metPatron(graphe* g);
	void remetPatron();
    ~TestFormuleElementVectorGraphe();
    char assigne(Variable* v) {var=(VariableVectorGraphe*)v; return OUI;}
};


class TestFormuleElementVectorString : public TestFormuleElement {
 public:

    VariableVector* var;
    TestFormule* indice_vecteur;
    TestFormule* indice_chaine;
    TestFormuleElementVectorString(VariableVector* v,TestFormule* iv,TestFormule* ic) {
        type=VVECTEURCHAINE;
        indice_vecteur=iv;
        indice_chaine=ic;
        var=v;
    }        
	void setutf8(bool u) {
		var->setutf8(u);
	}
    ~TestFormuleElementVectorString();
    char assigne(Variable* v) {var=(VariableVector*)v; return OUI;}
};


class TestFormuleElementLemme : public TestFormuleElement {
 public:

    VariableLemmeIndex* var;
    int indice;

    TestFormuleElementLemme(VariableLemmeIndex* v=NULL,int i=-1) {
        type=VLEMMEINDEX;
        var=v;
        indice=i;
    }
    
    VariableLemmeIndex* valeur_lemme() {return var;}
    char assigne(Variable* v) {var=(VariableLemmeIndex*)v; return OUI;}
};

class TestFormuleElementDependance : public TestFormuleElement {
 public:

    VariableIndexeeDependance* var;
    int indice;
    TestFormuleElementDependance(VariableIndexeeDependance* v=NULL, int i=-1) {
        type=VDEPENDANCE;
        indice=i;
        var=v;
    }

    TestFormuleElementDependance* valeur_dependance() {return this;}
    char assigne(Variable* v) {var=(VariableIndexeeDependance*)v; return OUI;}
    
};

class TestFormuleElementChaineDependance : public TestFormuleElement {
 public:

    VariableIndexeeChaineDependance* var;
    int indice;
    TestFormuleElementChaineDependance(VariableIndexeeChaineDependance* v=NULL, int i=-1) {
        type=VCHAINEDEPENDANCE;
        indice=i;
        var=v;
    }

    TestFormuleElementChaineDependance* valeur_dependance_chaine() {
		return this;
	}
    char assigne(Variable* v) {
		var=(VariableIndexeeChaineDependance*)v; 
		return OUI;
	}    
};


class TestFormuleElementVariable : public TestFormuleElement {
 public:
    
    VariableIndexeeNoeud* var;
    int indice;
    TestFormuleElementVariable(VariableIndexeeNoeud* v=NULL,int i=-1) {
        type=VNOEUD;
        var=v;
        indice=i;
    }
    
    TestFormuleElementVariable* valeur_variable() {return this;}
    char assigne(Variable* v) {var=(VariableIndexeeNoeud*)v; return OUI;}
    
};

class TestFormuleElementChaineNoeud : public TestFormuleElement {
 public:
    
    VariableIndexeeChaine* var;
    int indice;
    TestFormuleElementChaineNoeud(VariableIndexeeChaine* v=NULL,int i=-1) {
        type=VCHAINENOEUD;
        var=v;
        indice=i;
    }
    TestFormuleElementChaineNoeud* valeur_variable_chaine() {return this;}
    char assigne(Variable* v) {var=(VariableIndexeeChaine*)v; return OUI;}
    
};


class TestFormuleElementReel : public TestFormuleElement {
 public:

    double var;
    TestFormuleElementReel(double v=0) {
        type=VALREEL;
        var=v;
    }

    double valeur_reel() {return var;}
    
};

class TestFormuleElementValString : public TestFormuleElement {
 public:

    unsigned char* var;
    TestFormuleElementValString(unsigned char* v) {
        type=VALCHAINE;
        var=DupliqueChaine(v);
    }

    ~TestFormuleElementValString() {
        libere(var);
    }

    char* valeur_valstring() {return (char*)var;};
};

class TestFormuleElementOperation : public TestFormuleElement {
 public:

    int operation;
    TestFormuleElementOperation(int v=0) {
        type=VOPERATEUR;
        operation=v;
    }

    int valeur_operation() {return operation;}
};

class TestFormuleElementNoeud : public TestFormuleElement {
public:

    TestFonction* tf;
    TestFormuleElementNoeud(TestFonction* f) {
        type=VALNOEUD;
        tf=f;
    }

    ~TestFormuleElementNoeud();
    int argument(GlobalParseur* parseur,VECTA<Noeud*>& argNoeuds);
    int arg();
};

class TestFormuleElementAutomate : public TestFormuleElement {
public:

    automate* a;

    TestFormuleElementAutomate(uchar* expression);
    ~TestFormuleElementAutomate() {
        if (a!=NULL)
            delete a;
    }
    

};



class TestFormule {
 public:

    RAMASSAGE(TestFormule);
    TestFormuleElement* element;
    VECTA<TestFormule*> formules;    

    TestFormule();    
    ~TestFormule();
     double TraitementFormuleString(GlobalParseur*,
        VECTA<Noeud*>& noeuds,
        char& erreur,
        string& buff,
		VECTA<ResultatFonction*>* dep=NULL);

    char AffecteVariable(GlobalParseur*,unsigned char* res,VECTA<Noeud*>& noeuds,VECTA<ResultatFonction*>* focus,double valres=-1);
	double TraitementFormuleGraphe(GlobalParseur*,VECTA<graphe*>&,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* dep=NULL,sorteVariable surtype=ACTIONGRAPHEVIDE);
    double TraitementFormule(GlobalParseur*,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* dep=NULL);
    void print(char);
    virtual char TypeString() { return 0;}
    virtual char TypeBoucle() { return 0;}    
	virtual char TypeGraphe() { return 0;}
    char AffecteVariable(Variable*);
    Variable* RenvoieVariable();
    Variable* RenvoieVariableVecteur(GlobalParseur* parseur,
        VECTA<Noeud*>& noeuds,
        char& erreur,
        VECTA<ResultatFonction*>* focus,
        char renvoiebrute=NON);
};

class TestFormuleGraphe : public TestFormule {
public:

	char TypeGraphe() { return 1;}
};

class TestFormuleString : public TestFormule {
public:


    char TypeString() { return 1;}


};

class TestFormuleBoucle : public TestFormule {
public:


    int operateur;
    TestFormuleBoucle(char op) {
        operateur=op;
    }
    char TypeBoucle() { return 1;}

};



//-----------------------------------------------------------------------------
class TestFonction {
 public:

	 RAMASSAGE(TestFonction);
     int index;
     char negation;


     TestFonction() {
		 RAMASSE(TestFonction);
         index=-1;
         negation=NON;
     }

     TestFonction(Cat* X) {
		 RAMASSE(TestFonction);
         index=-1;
         negation=NON;
     }

     virtual ~TestFonction() {
		 JETTE(TestFonction);
	 };

     virtual double TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* dep=NULL) {return 0;}
     virtual void Formule(TestFormule*) {};
     virtual typeTESTFONCTION Type() {return TESTFONCTION;}    
     virtual char execute(ContexteInference& ci,int i,ListeTestArbre* lt,Signature& cumule,
         VECTA<Traits*>& vecttr,VECTA<Traits*>& affectation,char& resultat);
     virtual int IndexDependance() {
         return -1;
     }
     virtual Traits* LesTraits() {
        return NULL;
     }

};


class TestFonctionAffichage : public TestFonction {
public:
    TVecteur affiche;

    TestFonctionAffichage() {affiche=0;}
    typeTESTFONCTION Type() {return TESTFONCTIONAFFICHAGE;}
    char execute(ContexteInference& ci,int i,ListeTestArbre* lt,Signature& cumule,
        VECTA<Traits*>& vecttr,VECTA<Traits*>& affectation,char& resultat);

};


class TestFonctionDependance : public TestFonction {
 public:

    Cat* Fx;    
	typeCat typecat;
    Traits* traitsFonction;
    VECTA<Traits*> traits;
    VECTA<char*> stack;
    VECTA<TestFormule*> formules;
    char clearStack;
    char changement;
	char final;
    int vardep;    
    VECTA<int> arg;
    typeTESTFONCTION leType;
    char pointeur;
	VECTA<TestFonctionDependance*> fils;

    TestFonctionDependance(Cat* X);
    ~TestFonctionDependance();

    char Test(GlobalParseur*,ExtractionFonction* groupe,VECTA<Noeud*>& argNoeuds);
    ResultatFonction* TestPourDeduction(GlobalParseur*,
                                        ExtractionFonction* ef,
                                        VECTA<Noeud*>& vect,
                                        VECTA<Traits*>&,
                                        Cat* fx,
                                        int&,
                                        int&,
                                        Signature& verif_compatible);
    typeTESTFONCTION Type() {return leType;}
    char TestPremierElement(GlobalParseur* parseur,ExtractionFonction* ef,VECTA<Noeud*>& vectbase);
    double TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* dep=NULL);
    void raz(GlobalParseur*);
    int IndexDependance() {
        return vardep;
    }

    Traits* LesTraits() {
        return traitsFonction;
    }
};

#ifdef XIPPYTHON
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

class ProcedurePython {
public:
    PyObject* pFunction;
    PyObject* pDico;
    Dependance* depend;    
    string nom;
    string nomfichier;

    ProcedurePython(Dependance* dep,PyObject* p,PyObject* pdic,char* n) {
        depend=dep;
        pFunction=p;
        pDico=pdic;
        nom=n;
    }
    ~ProcedurePython();
};


class TestFonctionPython : public TestFonction {
 public:

    Cat* Fx;
    VECTA<int> arg;
    VECTA<TestFormule*> formules;    
    ProcedurePython* proc_python;


    TestFonctionPython(Cat* X,ProcedurePython* p) {
        Fx=X;
        proc_python=p;
    }

    ~TestFonctionPython();

    typeTESTFONCTION Type() {return TESTFONCTIONPYTHON;}
    double TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* dep=NULL);
};
#endif


class TestFonctionNoeud : public TestFonction {
 public:

    typeComparateur comparateur;
    VECTA<Traits*> traits;
    Traits* selection;
    Traits* selectiontous;
    int arggauche,argdroit;
    VECTA<int> arg;
	VECTA<Traits*> arg_traits;
	Cat* X;

    TestFonctionNoeud();
    ~TestFonctionNoeud();

    char TestNoeud(GlobalParseur*,VECTA<Noeud*>& argNoeuds,VECTA<Traits*>*,VECTA<Traits*>*);
    typeTESTFONCTION Type() {return TESTFONCTIONNOEUD;}
    char AffectationLectFormule(GlobalParseur* parseur, Noeud* ngauche,VECTA<Noeud*>& argNoeuds, VECTA<Traits*>* affectation);    
    char AffectationFormule(GlobalParseur* parseur, Noeud* ngauche,VECTA<Noeud*>& argNoeuds, VECTA<Traits*>* affectation);        
    char execute(ContexteInference& ci,int i,ListeTestArbre* lt,Signature& cumule,
        VECTA<Traits*>& vecttr,VECTA<Traits*>& affectation,char& resultat);

};


class TestFonctionKif : public TestFonction {
 public:

    Cat* Fx;
    VECTA<int> arg;
    VECTA<TestFormule*> formules;    
    KifFunction* proc_kif;
	vector<KifElement*> params;
	vector<KifElement*> nettoyage;

    TestFonctionKif(Cat* X,KifElement* p);
	~TestFonctionKif();
    typeTESTFONCTION Type() {return TESTFONCTIONKIF;}
    double TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* dep=NULL);
};


class TestFonctionGraphe : public TestFonction {
	public:
    TestFormule* formule;

    TestFonctionGraphe() {
        formule=NULL;
    }

    ~TestFonctionGraphe() {
        if (formule!=NULL) 
            delete formule;
    }
    
    void Formule(TestFormule* f) {formule=f;}
    typeTESTFONCTION Type() {return TESTFONCTIONGRAPHE;}    
    double TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* dep=NULL) {
		VECTA<graphe*> vg;
        return formule->TraitementFormuleGraphe(parseur,vg,noeuds,erreur,dep,ACTIONGRAPHEVIDE);		
    }
    char execute(ContexteInference& ci,int i,ListeTestArbre* lt,Signature& cumule,
        VECTA<Traits*>& vecttr,VECTA<Traits*>& affectation,char& resultat);

};


class TestFonctionMath : public TestFonction {
public:
    
    TestFormule* formule;

    TestFonctionMath() {
        formule=NULL;
    }

    ~TestFonctionMath() {
        if (formule!=NULL) 
            delete formule;
    }
    
    void Formule(TestFormule* f) {formule=f;}
    virtual typeTESTFONCTION Type() {return TESTFONCTIONMATH;}    
    virtual double TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* dep=NULL) {
        return formule->TraitementFormule(parseur,noeuds,erreur,dep);
    }
    char execute(ContexteInference& ci,int i,ListeTestArbre* lt,Signature& cumule,
        VECTA<Traits*>& vecttr,VECTA<Traits*>& affectation,char& res);
};


class TestFonctionMathString : public TestFonctionMath {
public:


    TestFonctionMathString() {
        formule=NULL;
    }

    
    typeTESTFONCTION Type() {return TESTFONCTIONMATHCHAINE;}    


    double TraitementFormule(GlobalParseur* parseur,VECTA<Noeud*>& noeuds,char& erreur,VECTA<ResultatFonction*>* dep=NULL) {
        string buff;
        return formule->TraitementFormuleString(parseur,noeuds,erreur,buff,dep);
    }    
    
    ~TestFonctionMathString() {
        if (formule!=NULL)
            delete formule;
        formule=NULL;
    }
};

class TestFonctionMathBoucle : public TestFonctionMath {
public:


    TestFonctionMathBoucle() {
        formule=NULL;
    }
    
    typeTESTFONCTION Type() {return TESTFONCTIONBOUCLE;}    
    ~TestFonctionMathBoucle() {
        if (formule!=NULL)
            delete formule;
        formule=NULL;
    }

};





#endif

