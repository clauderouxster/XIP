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
   filename   : avlfonc.h
   Date       : 16/12/1999
   Purpose    : This file defines the different methods and class to handle sequence rules and
                deduction rules. ArcArbre which corresponds to a node in a sequence or deduction rule
                is defined here. ArcArbreis also used in marking rules and reshuffling rules.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef avlfonc_h
#define avlfonc_h
#include "avltrait.h"

#define courte 1
#define longue 2

class Noeud;
class GlobalParseur;
class ListeTestArbre;
class XipDependency;
class TestFonction;
class VectTestFonction;
class VectTFInstruction;
class VectTFUneInstruction;
class baseregle;
//-----------------------------------------------------------------------------

                               

class PileNoeuds {
 public:
    
    VECTA<Noeud*> noeuds;
    VECTA<PileNoeuds*> pile;
    int argument;
    int nb;
    int profondeur;

    PileNoeuds() {argument=-1;nb=0;profondeur=-1;}
    PileNoeuds(int a,Noeud* n,int prof) {argument=a;noeuds.ajoute(n);nb=0;profondeur=prof;};
    void ajoute(int a,Noeud* n,int prof) {
        PileNoeuds* p=new PileNoeuds(a,n,prof);
        pile.ajoute(p);
    }
    
    void nettoie();
    
    ~PileNoeuds();
};


class ResultatFonction {
 public:

    GlobalParseur* parseur;
    Cat* Fx;
    VECTA<Noeud*> arg;
    VECTA<Noeud*> variables;
    //  VECTA<char> pos;
    char bon;
    int duree;
    VECTA<int> numeros;
    int position;
    int index;
    int indexpremier;
    int indexsecond;
    int indexcategorie;
	int profondeur;
	int numero_ordre;
    char nb_args;
    char nb_noeuds;
    Traits* traits;
    VECTA<char*> stack;
    Noeud* premier;
    Noeud* dernier;
    VECTA<Variable*> vardep;
    char data;
    Signature compatible;
	genereGraphe* frere;
	XipDependency* soeur;

	ResultatFonction* Duplique();
    ResultatFonction(GlobalParseur*,Cat*x=NULL);
    ResultatFonction(ResultatFonction*);
    void affecte(ResultatFonction*);
    void initialise(Cat* x=NULL);
    char Integre(int);
    XipDependency* ConstruitResultat(GlobalParseur* pars,XipUnit*);
    ~ResultatFonction();
    void Affiche(ostream& os,char cache=OUI);
    void RAZ();
};


class ResultatSequence {

 public:
    
    int debut;
    int fin;
    int increment;
    Cat* X;
    ChaineTraits* traits;
    TVecteur clef;
    short Nombre;
    
    char bon;
    
    ResultatSequence(int inc);

    ResultatSequence(GlobalParseur*,Cat*,Traits*);
    void initialise(GlobalParseur*,Cat*,Traits*);
    ResultatSequence(ResultatSequence*);
    ~ResultatSequence();
    void Detruit();
};
    
//-----------------------------------------------------------------------------
class EtatArbre;
class ArcArbre;

class EtatArc {
public:

    EtatArbre* etat;
    ArcArbre* arc;
    int iar;

    EtatArc(EtatArbre* e,ArcArbre* a,int i) {
        etat=e;
        arc=a;
        iar=i;
    }
};

class ArcArbre {
public:
    Cat* X;
    char negation;
    char profondeurNonBornee;
    char desamb;

    Traits* traits;
    Traits* selection;
    Traits* selectiontous;
    int argument;
    
    
    VECTA<int> numliste;
    EtatArbre* etatarbre;
    char type;
   
    TVecteur clefsuivante;

    char garder;

    int marque;
    int num;
    void print(ostream&,int,int);
    void genere(GlobalParseur* parseur,ostream&,int,int);
    void demarque();


    ArcArbre(Cat* x ,Traits* tr,int n);    
    char TestMarquage(GlobalParseur* parseur,
        Noeud* n,
        char profondeur,
        Piles& stacks,
        Fonction* fonc,
        VECTA<VECTA<Noeud*>*>* variables,
        VECTA<int>* pileArg);
	void met(GlobalParseur*,trait*);
    void Empile(Noeud* n,char profondeur,PileNoeuds& pile);
    char TestNoeud(GlobalParseur* parseur,Noeud* n,char calculComparaison);
    void typer(Cat*);
    void ArcEpsilon(VECTA<EtatArc*>&,Cat*,char);
    int CheminCourt(TVecteur& unecle,int max,Cat*);
    void RechercheDesamb(VECTA<ArcArbre*>& vectarbre);
    void arcvers(VECTA<ArcArbre*>& dernier,EtatArbre* cible);
};

class EtatArbre {
public:

    EtatArbre* fils;
    VECTA<ArcArbre*> arcs;
    char etatfin;
    int marque;
    int etat;
    TVecteur clef;
    char type;    
    int nombre;

    char garder;
    EtatArbre();

    void ResolutionFictif(GlobalParseur* parseur,
        VectTFInstruction* vect,
        char& resultat,
        typeDeduction type_deduction,
        int lacouche,
        VECTA<Noeud*>& noeudreference);
    
	void AppliqueInference(ContexteAnalyse& ct,
        char& solution,
        char& fin,
        Noeud* n,
        char profondeur,
        char enProfondeur,
        Piles& stacks,
        PileNoeuds& pile,
        char calculTest,
		char& resultatfinal);
    
	char MotifInference(ContexteAnalyse& ct,
		int iar,
		PileArcNoeud& pile,
        Noeud* n,
		int profondeur,
		char calcul);
    void print(ostream&,int);
    void genere(GlobalParseur* parseur,ostream&,int);
    void demarque();
	void met(GlobalParseur*,trait*);
    void ConstruitClef(Cat*,int&);
    void ajoutearc(GlobalParseur*,ArcArbre* a,Cat*,char);
    void dernierarcs(VECTA<ArcArbre*>& dernier);
    void arcvers(VECTA<ArcArbre*>& dernier,EtatArbre* cible);
    char rechercheCheminEpsilon(Cat* vide,int iar=0) {
        if (etatfin!=0)
            return OUI;
        for (int i=iar;i<arcs.dernier;i++) {
            ArcArbre* ar=arcs[i];
            if (ar->X==vide && ar->type!=1 && ar->marque!=2)
                if (ar->etatarbre->rechercheCheminEpsilon(vide,0)==OUI)
                    return OUI;
        }
        return NON;
    }
    char FusionneArc(GlobalParseur* parseur);
    void ConstruitClefSuivant(GlobalParseur* parseur,Cat* vide);
    char AppliqueLineaire(GlobalParseur* parseur,
                                 char& chemin,
                                 LListe& Phrase,
                                 int iListe,
                                 int increment,                                 
                                 char maj,
                                 ResultatSequence& element,
								 int lePremier,
								 VECTA<Noeud*>* noeuds_arguments,
                                 VECTA<int>* noeuds_reperes=NULL,
                                 VECTA<char>* majCategorie=NULL,
								 Sequence* r=NULL);
    void Marquage(GlobalParseur* parseur,
        char& chemin,
        Noeud* n,
        char profondeur,
        Piles& stacks,
        Fonction* fonc,
        VECTA<VECTA<Noeud*>*>* variables,
        VECTA<int>* pileArg);

    void ExtraitMots(GlobalParseur* parseur,
        VECTA<Noeud*>& pile,
        char& fin,
        int increment,
        char& chemin,
        LListe& Phrase,
        int iListe,
        char deterministe,
        ListeTestArbre* listes);
    void Decoupe(GlobalParseur* parseur,
		Sequence* seq,
        char& chemin,
        VECTA<ListeVoc*>& Phrase,
        int iListe,
        int increment,
        VECTA<ListeVoc*>& pile,
        VECTA<Traits*>& piletraits,
        int& coupe,
        char stabilise,
		VECTA<ListeVoc*>* variables);
    void typer(Cat*);
    void ArcEpsilon(VECTA<EtatArc*>&,Cat*,char);
    int CheminCourt(TVecteur& unecle,int max,Cat*);
    void RechercheDesamb(VECTA<ArcArbre*>& vectarbre);
    Noeud* RecreeArbre(GlobalParseur* parseur,
		LListe& Phrase,
		Noeud* racine,
        VECTA<VECTA<Noeud*>*>& variables,
		int,
		int,
		char top);
	void VerifieLineaire(ContexteAnalyse& ct,
		char& chemin,  
		char& fin,
		Noeud* n,
		Noeud* ndernier,
		char calculComparaison);
};


extern int compte_test_arbre;
//-----------------------------------------------------------------------------
//Une representation arborescente d'un test
class TestArbre {
public:

	RAMASSAGE(TestArbre);
    char operateur;
    char direct;
    char deterministe;
    TestFonction* test;
    VECTA<TestArbre*> arbre;

    TestArbre(TestFonction* tt=NULL) {
		compte_test_arbre++;
		RAMASSE(TestArbre);
        operateur='@';
        direct=NON;
        test=tt;
        deterministe=NON;
    }

    TestArbre(char oper) {
		compte_test_arbre++;
		RAMASSE(TestArbre);
        operateur=oper;
        direct=NON;
        test=NULL;
    }

    ~TestArbre() {
		compte_test_arbre--;
		JETTE(TestArbre);
        //arbre.nettoie();
    }


    char LineariseDirect(GlobalParseur*,TVecteur& clef,ListeTestArbre& listes);
    char TestCalculClef(GlobalParseur* parseur);

    void ConstruitListes(GlobalParseur* parseur,
                         int courant,
                         ListeTestArbre* pere,
                         VECTA<ListeTestArbre*>& listes,
                         char& type,
                         char deter);    
    void AjouteEnQueue(ListeTestArbre* pere,VECTA<ListeTestArbre*>& liste,ListeTestArbre*);
};        
 

//-----------------------------------------------------------------------------
class ListeTestArbre {
 public:    
    RAMASSAGE(ListeTestArbre);
    TVecteur clefdependance;
    Traits* traitsdependance;
    TestFonction* test;
    char deterministe;
    VECTA<ListeTestArbre*> liste;


    char Test(GlobalParseur*,
              VECTA<Noeud*>& vect_noeuds);

    char TestFormule(GlobalParseur* pars,
                     VECTA<Noeud*>& vect,
                     size_t i);

	char TestDependances(GlobalParseur* pars,size_t i);

    void calculInference(ContexteInference& ci,
                         VECTA<Traits*>& vecttr,
                         VECTA<Traits*>&,
                         size_t i,                         
                         char& resultat);
    char testInference(ContexteInference& ci);
    char PreTestInference(ContexteInference& ci,int);

    void inference(ContexteInference& ci,
                   VECTA<Traits*>& vecttr,
                   VECTA<Traits*>&,
                   size_t i,                   
                   Signature& cumule,
                   char& resultat);

    ListeTestArbre(TestFonction* tf=NULL);
    ~ListeTestArbre();
    
    void Destruction(GlobalParseur*,VECTA<TestFonction*>&,VECTA<TestFonction*>&);

};

//-----------------------------------------------------------------------------

class Dependance : public baseregle {
   public:

    RAMASSAGE(Dependance);
    Cat* Fx; /* Sx-> x,Spec(x),..*/    
    VECTA<size_t> arg;
    int nb_args;
    int duree;
    Traits* traits;
    int vardep;
    VECTA<Liste*> noeuds;
    char modification;
	

    VECTA<char*> stack;
    char clearStack;

    Dependance(Cat*);
    ~Dependance();
    virtual char Type() { return NON;}
};

class SousDependance : public Dependance {
public:

	VECTA<Dependance*> sousnoeuds;
	
	SousDependance(Dependance*);
	void recopie(Dependance*);	
	char Type() { return SOUSDEPENDANCE;}
};

class NomProcedure : public Dependance {
public:


    NomProcedure(Cat* x) : Dependance(x) {};
    VECTA<Variable*> parametres;
    char Type() { return OUI;}
};

//-----------------------------------------------------------------------------
//Cette classe contient les elements communs aux regles de sequence et aux regles d'expression reguliere d'arbre
class SeqFonc : public baseregle {
 public:

    RAMASSAGE(SeqFonc);
    Cat* Fx;
    Traits* traits;
    char deterministe;    
    EtatArbre* regle;
	int deja;
	TVecteur aplat;

    ListeTestArbre* listes;

    TVecteur clef;
    int nombre;
    char negation;
    char detruire;
    VECTA<size_t> arg;
    int nb_args;

    SeqFonc(Cat* X) : arg(3) {   
		RAMASSE(SeqFonc);
        Fx=X;
		deja=-1;
        clef=0;
        listes=NULL;
        deterministe=NON;
        negation=NON;
        detruire=OUI;
        nb_args=0;
        nombre=0;
    }

    void ConstruitClef();
    virtual typeRegle type() {return VIDE;}
    ~SeqFonc();
};
//---------------------------------------------------------------------
class Sequence : public SeqFonc  {
 public:

    char touteCategorie;
    Sequence* gauche;
    Sequence* droit;
    char passageLexical;
    int increment;
	int borneG;
	int borneD;
	Liste* L;
    
    
    typeRegle type() {return REGLES_SEQUENCE;}
    Sequence(Cat* X) : SeqFonc(X) {
		L=NULL;
        increment=1;
        gauche=NULL;
        droit=NULL;
        touteCategorie=NON;
        passageLexical=NON;
        regle=NULL;
		borneG=-1;
		borneD=-1;
    }
    
    ~Sequence() {
		JETTE(SeqFonc);
        //if (gauche!=NULL)
        //    delete gauche;
        //if (droit!=NULL)
        //    delete droit;
    }

    char TestUnContexte(GlobalParseur* pars,
                        LListe& Base,size_t ind,int increment,char type,int&,VECTA<Noeud*>* narg=NULL);
    
	
    void affiche(GlobalParseur*,ostream& os);
    char TestMatching(GlobalParseur*,LListe& L,size_t iListe,ResultatSequence& element,
                      VECTA<Noeud*>& narg,
                      VECTA<char>& majCategorie);
    char MatchTokens(GlobalParseur*,LListe& L,size_t iListe,ResultatSequence& element,
                      VECTA<Noeud*>& narg,
                      VECTA<char>& majCategorie);
    
    char TestExpressionBooleen(GlobalParseur* parseur,VECTA<Noeud*>& vect_noeuds);
    char TestContexte(GlobalParseur*,LListe&,size_t,size_t,VECTA<Noeud*>& narg);
    char TestContexteGauche(GlobalParseur*,LListe&,size_t,VECTA<Noeud*>& narg);
    char TestContexteDroit(GlobalParseur*,LListe&,size_t,VECTA<Noeud*>& narg);
    char TestContexteSimple(GlobalParseur*,LListe&,size_t,size_t,VECTA<Noeud*>& narg);
    void ConstruitClef(Cat* v);
};
//Utilise dans le cadre de la generation
class OrdreNoeuds : public baseregle {
public:

	Cat* Xgauche;
	Cat* Xdroit;
	Traits* Tgauche;
	Traits* Tdroit;
	VECTA<int> argsgauche;
	VECTA<Traits*> argtraitsgauche;
	VECTA<int> argsdroit;
	VECTA<Traits*> argtraitsdroit;

	OrdreNoeuds(Cat* x,Cat* y,Traits* tx,Traits* ty) {
		Xgauche=x;
		Xdroit=y;
		Tgauche=tx;
		Tdroit=ty;
	};
};

//---------------------------------------------------------------------
class TokenSelection : public baseregle {
public:
	RAMASSAGE(TokenSelection);

	Sequence* regle;

	typeRegle type() {return REGLES_TOKENSELECTION;}
	TokenSelection(Sequence* s) {
		RAMASSE(TokenSelection);
		regle=s;
	}

	~TokenSelection();
};

//-----------------------------------------------------

class Fonction : public SeqFonc {
   public:

    VECTA<char> marque;
	int nbvariables;
    int categorie;
            
    TVecteur zones;
    char detruireArbre;    

    typeRegle type() {return REGLES_EXTRACTION;}
    char MarqueNoeuds(GlobalParseur*,Noeud* n,int laCouche);

//    char Test(GlobalParseur*,ResultatFonction* e);

    Fonction(Cat*);
    Fonction(Fonction*);
    ~Fonction();
    void ConstruitClef(Cat* v);
    virtual void affiche(GlobalParseur*,ostream& os);
	void comptevariables() {
		for (int i=0;i<marque.dernier;i++) {
			if (marque[i]!=NON)
				nbvariables++;
		}
	}
 };


//-----------------------------------------------------------------------------
class Simplifie : public baseregle {
   public:

	RAMASSAGE(Simplifie);
    VECTA<Cat*> X; /* Sx-> x,Spec(x),..*/
    VECTA<char> negation;

    int nbX;
    int nbCat;
    VECTA<Cat*> categorie;
    VECTA<Traits*> traits_categorie;
    VECTA<char> tous;
    VECTA<Traits*> traits;
    TVecteur clef;
    Sequence* gauche;
    Sequence* droit;
    VECTA<ArcArbre*> vectarbre;
    Traits* communs;

    ListeTestArbre listes;

    char ecrase;

    Simplifie() : vectarbre(0) {
		RAMASSE(Simplifie);
        nbX=0;
        clef=0;
        nbCat=0;
        nbX=0;
        gauche=NULL;
        droit=NULL;
        communs=NULL;
        //test=NULL;
        ecrase=NON;
    }

    ~Simplifie() {
        
        //if (test!=NULL)
        //  delete test;
		JETTE(Simplifie);
        if (gauche!=NULL)
            delete gauche;

        if (droit!=NULL)
            delete droit;
    }

    void ConstruitClef();
};


class Cartesien : public baseregle {
   public:

    RAMASSAGE(Cartesien);
    char tous;
    TVecteur clef;
    
    Sequence* regle;
    ListeTestArbre listes;

    Cartesien() {
		RAMASSE(Cartesien);
        regle=NULL;
        clef=0;
    }

    ~Cartesien() {
		JETTE(Cartesien);
        delete regle;
    }
};



//-------------------------------------------------------------
//Cette classe est utilisee pour gerer un vecteur de TestFonction
//-------------------------------------------------------------
class VectTestFonction : public baseregle {
public:

	RAMASSAGE(VectTestFonction);
    int finboucle;
    char premiertest;
    typeInstruction typeTest;
    int position;
    char script;
    char regle_contexte;

    VectTestFonction() {
		RAMASSE(VectTestFonction);
        script=NON;
        position=-1;
        premiertest=NON;
        finboucle=-1;
        typeTest=INSTRUCTION_NORMALE;
        regle_contexte=NON;
    }

    virtual char AppliqueInference(GlobalParseur*,typeDeduction type_deduction,int lacouche,VECTA<Noeud*>& noeudreference) {return NON;}
    virtual void ConstruitClef() {}
    virtual char Type() {return 0;}
    VectTFInstruction* derivation();
    virtual VectTFUneInstruction* simplification() { return NULL;}
	virtual ~VectTestFonction() {
		JETTE(VectTestFonction);
	}
};

class VectTFUneInstruction : public VectTestFonction {
 public:

    TVecteur zones;
    char deterministe;
    TestFonction* test;

    VectTFUneInstruction(TestFonction* tf,TVecteur z) {
        deterministe=NON;
        zones=z;
        test=tf;
    }
    
    
    char Type() {return 2;}
};

class VectTFInstruction : public VectTestFonction {
 public:

    //Un vecteur de TestFonction
    ListeTestArbre listes;    
    VECTA<Dependance*> dependances;
    VECTA<char> variables;
    Fonction* regle;
    Fonction* negation;
    TVecteur zones;

    char incompatible;    
    Cat*  nomgraphe;
    char deterministe;
    
    VectTFInstruction* sinon;
    TVecteur clef;



    VectTFInstruction() {
        clef=0;
        nomgraphe=NULL;
        incompatible=NON;
        sinon=NULL;        
        regle=NULL;
        negation=NULL;
        deterministe=NON;
        zones=MoinsUn;
    }

    ~VectTFInstruction();
    

    VectTFUneInstruction* simplification();
    char AppliqueInference(GlobalParseur*,typeDeduction type_deduction,int lacouche,VECTA<Noeud*>& noeudreference);
    void ConstruitClef();    
    virtual char Type() {return 1;}
};

class VectTestFonctionSemantique : public VectTFInstruction {
public:

    VECTA<ListeTestArbre*> aDetruire;
    VECTA<TestFonction*> aDetruireFonc;
    void Destruction(GlobalParseur*);
    char Type() {return 2;}
};

//Cette classe permet la gestion d'un vecteur de fonction
class VectSequence : public VECTA<Sequence*> {
public:
    
    VectSequence(int i=3) : VECTA<Sequence*>(3) {};
    
    char Match(GlobalParseur*,
        ResultatSequence& element,
        LListe& L,size_t,char lexical,int& ifonc,
        VECTA<Noeud*>& narg,
        VECTA<char>& majCategorie);
    
};


class ReconstruitArbre : public baseregle {
 public:

	RAMASSAGE(ReconstruitArbre);
    int categorie;
    ListeTestArbre* testPatron;
    TVecteur clef;
    EtatArbre* patron;
    EtatArbre* modele;
    int nbvariables;
    char ajoutvariable;
	TVecteur aplat;

    char Applique(GlobalParseur* parseur,LListe&,Noeud* n,int laCouche);

    ReconstruitArbre() {
		RAMASSE(ReconstruitArbre);
        ajoutvariable=0;
        nbvariables=0;
        patron=NULL;
        modele=NULL;
        testPatron=NULL;
    }

    ~ReconstruitArbre() {         
		JETTE(ReconstruitArbre);
        if (testPatron!=NULL)
            delete testPatron;
    }
};
    

class fournitureRF : public VECTA<ResultatFonction*> {
 public:
    
    fournitureRF(GlobalParseur* p,int tt) : VECTA<ResultatFonction*>(tt) {
        for (int i=0;i<taille;i++) {
            vecteur[i]=new ResultatFonction(p);
            vecteur[i]->position=i;
        }
    }
    
    void ajusteVecteur(GlobalParseur* parseur,int posDernier) {
        int ancienneTaille=taille;
        int nouvelleTaille=taille+taille/2;

        ajuste(nouvelleTaille);

        for (int j=ancienneTaille;j<nouvelleTaille;j++) {
            vecteur[j]=new ResultatFonction(parseur);
            vecteur[j]->position=j;
        }

        dernier=posDernier;
    }


    ~fournitureRF() {
        for (int i=0;i<taille;i++)
            delete vecteur[i];
    }
};


/*-------------------------------------------------------------------------*/
char TestClef(GlobalParseur* parseur,TVecteur clef);

#endif


