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
   filename   : variable.h
   Date       : 10/09/1992
   Purpose    : Definition of the base class GlobalParseur. Comprises all the methods to compile
                a grammar and parse a sentence.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef variable_h
#define variable_h
/*Variables globales*/


extern int ERREUR_SURVENUE;
class LListe;
class ChaineTraits;
const int MaxCouches=2048;
const int MaxCouchePhrase=128;
const int TailleHachageTraits=16384; // obligatoirement une puissance de 2

#include <stdio.h>
#include "noeud.h"
#include "lliste.h"
#include "sinput.h"
#include "avltemplate.h"
#include "automate.h"

#ifdef WIN32
#include <io.h>
#define ushort unsigned short
#endif

class x_node;
class TestFonctionDependance;
class arbretrait;
class ArcArbre;
class TokenSelection;
class NomProcedure;
class FournirEtatArbre;
class graphe;
class KifCode;
class KifXipRule;
class KifObjectInstruction;
class EtatArbre;
class Stocke_Variable;
class Signature;
class XipResult;
class XipFeature;
class XipDependency;
class XipNode;
class Cartesien;
class TestFormule;
class Liste;
class TestArbre;
class TestFonction;
class Dependance;
class SousDependance;
class Noeud;
class Vecteur;
class Traits;
class RegleTraits;
class OrdreNoeuds;
class PListe;
class Categorie;
class Regle;
class avl_Regle;
class trait;
class ListeDefaut;
class Defauts;
class auto_vocabulaire;
class avl_vocabulaire;
class avl_vocabulaire_terme;
class vocabulaire_terme;
class auto_vocabulaire_semantique;
class vocabulaire;
class abreviation;
class Reecriture;
class ChaineCat;
class ListeVoc;
class Fonction;
struct Cat;
class TasListe;
class ResultatFonction;
class VectTestFonction;
class VectTFInstruction;
class Simplifie;
class VectSequence;
class fournitureRF;
class ChercheRegle;
class ReconstruitArbre;
class SeqFonc;
class Sequence;
class Variable;
class VariableLemmeIndex;
class ListeTestArbre;
class dtdXML;
class vocabulaire_fragmente;
class ontologie;
class hierarchie;
class aligne;
class xmlresultat;
class TestFonctionMathBoucle;
class multimots;
class VariablePython;
class VariableKif;
class DefinitionCategorie;
class genereGraphe;
class genere_traits;
class KifXipRule;
class TokLOOK;
#include "generation.h"
#ifdef XIPPYTHON
class ProcedurePython;

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#endif

#ifdef XIPLIBXML
class XPathIndexation;
class arborescenceXML;
class noeudXML;
class clefXML;
#endif

typedef VECTA<ResultatFonction*> ExtractionFonction;
typedef VECTA<ExtractionFonction*> ResultatExtraction;
typedef void (*XipFunction)(int,XipResult*,void*) ;

#ifdef XIPLIBXML
class xptpos {
public:
    long pos;
    xmlNodePtr xmlres;
    xmlNodePtr courant;

    xptpos(long p,xmlNodePtr n) {
        pos=p;
        xmlres=NULL;
        courant=n;
        n->_private=this;
    }

    ~xptpos() {
        if (courant!=NULL)
            courant->_private=NULL;
    }
};
#endif

char ITR(GlobalParseur*,Traits* RegTraits,Traits* tr,char type,Noeud* noeud=NULL);

class MemoireVirtuelleSemantique {
public:

    int max;
    int indicateur;
    
    
    VECTA<auto_vocabulaire_semantique*> memoire;  
    VECTA<ListeTestArbre*> nouveauxElementsDeRegles;
    VECTA<TestFonction*> nouveauxTests;

    MemoireVirtuelleSemantique(int val);
	void ajuste(int val);

};
/*-------------------------------------------------------------------------*/
class DecompteRegle {
public:

    typeRegle regle;
    char* nom_fichier;
    unsigned long position;
    unsigned int ligne;
    int couche;
    int couche_relative;
    void* user;

    DecompteRegle(typeRegle r,char* nom,unsigned long off,unsigned int l,int c,int cr);
    ~DecompteRegle();
};


/*-------------------------------------------------------------------------*/
class ContexteInference {
public:

    GlobalParseur* parseur;
    VectTFInstruction* vtf;
    ExtractionFonction* liste;
	VECTA<VectTestFonction*>* vtestboucle;
    char lire;
    
    VECTA<Noeud*> noeuds;
    VECTA<ResultatFonction*> chgt;
    int couche;
    typeDeduction type;
	int instruction_suivante;
	TestFonctionDependance* parcours;
	genereGraphe* pere;
	genereGraphe* frere;
	int idx;
	int idxgen;
	ContexteInference* cisuivant;


    ContexteInference(GlobalParseur* p=NULL,VectTFInstruction* v=NULL)  {
		cisuivant=NULL;
		parcours=NULL;
		pere=NULL;
		frere=NULL;
        parseur=p;
        vtf=v;
        liste=NULL;
        lire=0;
		vtestboucle=NULL;
		instruction_suivante=-1;
		idx=-1;
		idxgen=-1;
    }

    void initialise(GlobalParseur* p,VectTFInstruction* v,VECTA<VectTestFonction*>* vtest) {
		parcours=NULL;
		pere=NULL;
		frere=NULL;
		vtestboucle=vtest;
        parseur=p;
        lire=0;
        vtf=v;
        liste=NULL;
        chgt.raz();
        noeuds.raz();        
		instruction_suivante=-1;
		idx=-1;
		idxgen=-1;
    }

    void initialise(GlobalParseur* p,VectTFInstruction* v,VECTA<Noeud*>& n,VECTA<VectTestFonction*>* vtest) {
		cisuivant=NULL;
		idx=-1;
		idxgen=-1;
		parcours=NULL;
		pere=NULL;
		frere=NULL;
		vtestboucle=vtest;
		parseur=p;
		lire=0;
        vtf=v;
        liste=NULL;
        chgt.raz();
        noeuds.raz();        
        noeuds.affecte(n);
		instruction_suivante=-1;
    }

    void affecte(ContexteInference& ci) {
		cisuivant=ci.cisuivant;
		idx=ci.idx;
		idxgen=ci.idxgen;
		parcours=ci.parcours;
		pere=ci.pere;
		frere=ci.frere;
		vtestboucle=ci.vtestboucle;
        chgt.raz();
        noeuds.raz();        
        chgt.affecte(ci.chgt);
        noeuds.affecte(ci.noeuds);
        parseur=ci.parseur;
		instruction_suivante=ci.instruction_suivante;
    }

};
/*-------------------------------------------------------------------------*/
class vocabulaire {
 public:

    Cat* X;
    Traits* initiaux;
    char lemme[2048];
    char surface[2048];
	char surfacebrute[2048];
    long gauche,droit;
	long gauchecar,droitcar;


    vocabulaire() {
        initiaux=NULL;
        X=NULL;
        lemme[0]=0;
        surface[0]=0;
        surfacebrute[0]=0;
        gauche=-1;
        droit=-1;
        gauchecar=-1;
        droitcar=-1;
    } 

    vocabulaire(Cat* x,Traits* tr) {
        initiaux=tr;
        X=x;
    }

    void raz() {
        initiaux=NULL;
        X=NULL;
        lemme[0]=0;
        surface[0]=0;
		surfacebrute[0]=0;
    }
   
    void init(Cat* x,Traits* tr) {
        initiaux=tr;
        X=x;
    }


    void detruit();

    ~vocabulaire();
};

class ContexteAnalyse {
public:
    GlobalParseur* parseur;
    int increment;
    char deterministe;
    VectTFInstruction* fonc;
    int bornemin;
    int bornemax;
    VECTA<VECTA<Noeud*>*> variables;
    VECTA<char> resultats;
    typeDeduction type_deduction;
    int couche;

    ContexteAnalyse() {
        increment=1;
        deterministe=0;
        fonc=NULL;
        parseur=NULL;
        bornemin=0;
        bornemax=0;
    }

    ~ContexteAnalyse() {
        variables.nettoie();
    }

    void initialise(GlobalParseur* p,int i,char d,VectTFInstruction* f,typeDeduction td,int lacouche) {
        parseur=p;
        increment=i;
        deterministe=d;
        fonc=f;
        variables.dernier=0;
        resultats.dernier=0;
        type_deduction=td;
        couche=lacouche;
    }

    int verifie(VECTA<Noeud*>& vars);

};


class Piles {
public:

    VECTA<Traits*> traits;
    VECTA<Noeud*> noeuds;
	VECTA<char> profondeurs;
	VECTA<ArcArbre*> arcs;
	char desamb;

    Piles() : traits(20),noeuds(20),arcs(20) {desamb=NON;};

    void raz() {
		desamb=NON;
        traits.raz();
        noeuds.raz();
		profondeurs.raz();
		arcs.raz();
    }

    void ajoute(Traits* tr,Noeud* n,int p,ArcArbre* a=NULL);

    int dernier() {
        return noeuds.dernier;
    }

    void depile(int st) {
        traits.dernier=st;
        noeuds.dernier=st;
		profondeurs.dernier=st;
		arcs.dernier=st;
    }
};

class PileArcNoeud {
public:
	VECTA<EtatArbre*> e;
	VECTA<Noeud*> n;
	VECTA<int> p;
	VECTA<Noeud*> varias;
	VECTA<int> iar;
	VECTA<VECTA<Noeud*>*> dejautilises;
	int nbvariables;
	ContexteInference ci;

	Piles stacks;

	VECTA<Traits*> vecttr;
	VECTA<Traits*> affectation;
	VECTA<ResultatFonction*> chgts;
	VECTA<ResultatFonction*> creees;

	PileArcNoeud() {nbvariables=0;}
	~PileArcNoeud() {
		dejautilises.nettoie();
	}

	void ajoute(EtatArbre* ee,Noeud* nn,int pp,int ii) {
		e.ajoute(ee);
		n.ajoute(nn);
		p.ajoute(pp);
		iar.ajoute(ii);
	}
	void retire() {
		e.retire();
		n.retire();
		p.retire();
		iar.retire();
	}
	void ajout(int arg,Noeud* n);
	void retrait(int arg);
	void raz();
	void initialise() {
		e.raz();
		n.raz();
		p.raz();
		iar.raz();
		varias.raz();
		dejautilises.nettoie();
		nbvariables=0;
		stacks.raz();
		raz();
	}
};


class word_offset {
public:

    char* mot;
    long gauche;
    long droite;
    long gauchecar;
    long droitecar;
    int longueur;
	int taille;
	int pos;

    word_offset(int tail=50) {
		taille=tail;
        mot=(char*)malloc(taille);
        raz();
		pos=-1;
    }

    void raz() {
        mot[0]=0;
        gauche=-1;
        droite=-1;
        gauchecar=-1;
        droitecar=-1;
        longueur=0;
    }

	void realloue(int tail);	
    char* concatene(char* internal);    
	char* concatene(string& internal);
    void ajoute(char internal,long offset,long offsetchar);
	void ajoute(char* internal,long offset,long offsetchar);

    ~word_offset() {
        free(mot);
    }
    void print();
};


class fichiersscript {
public:

    ofstream* fichier;
    char* alias;
    char* nom_fichier;

    fichiersscript(char* a,char* nf);
    char ouvrir();
    ~fichiersscript();
};

class Label;
class rulespace {
public:
	string nom;
	rulespace* pere;
	VECTA<rulespace*> fils;
	VECTA<Label*> regles;
	char actif;

	rulespace(char* n) {
		nom=n;
		pere=NULL;
		actif=OUI;
	}

	~rulespace() {
		for (int i=0;i<fils.dernier;i++)
			delete fils[i];
	}
	string chaine(string&);
};

class baseregle {
public:
	int identificateur;
	int numero;
	int couche;
	Label* label;
	float poids;
	float seuil;
	float valeur;
	int compteur;

	baseregle();
	virtual ~baseregle() {}
	char activationmin(GlobalParseur*);
	char activation(GlobalParseur*);
	char testprobabilite(char);
	char calculaleatoire();
};



class stats {
public:
    baseregle* regle;
    typeRegle type;
    int compteur;
	int couche;

    stats(baseregle* r,typeRegle t,int c) {
        regle=r;
        type=t;
        compteur=0;
		couche=c;
    }
};



class Label {
public:

    char* etiquette;
	string commentaire;
    typeRegle type;
    int index;
	int numero;
    int couche;
    DecompteRegle* dr;
	int indexregle;
	rulespace* space;

    void aff(ostream& os);

    Label(char* e,typeRegle t,int c,int i,DecompteRegle* d);

    ~Label();
};

class FournirEtatArbre {
public:
    VECTA<EtatArbre*> etats;
    VECTA<ArcArbre*> arcs;

    EtatArbre* alloue();
    ArcArbre* allouearc(Cat* x ,Traits* tr,int n);
    ~FournirEtatArbre() {        
        etats.nettoie();
    }

    void demarque();
    void detruit(EtatArbre*);
    void retireEpsilon(EtatArbre*,Cat*,int debutetat,int debutarc,char);
    void demarquage(int de,int da);
    void destruction(int,int);
};

class Hierarchie {
public:
    automate ontologies;
    int indexontologie;
    VECTA<hierarchie*> lahierarchie;

	Hierarchie()  {
		indexontologie=0;
	}
	void affecte(int i,hierarchie* h) {
		lahierarchie.affecte(i,h);
	}
	~Hierarchie() {
		lahierarchie.nettoie();
		ontologies.nettoie();
	}
};

//------------------------------------------------------------------
class KifCodeParseur : public KifCode {
public:

	KifCodeParseur(KifCode* l,int trig,void* d=NULL,ostream* o=NULL);
	~KifCodeParseur();
	Exported KifElement* Get(short n,KifDomain* dom);
	Exported KifElement* Getbasic(short n,KifElement*);
	Exported void CreeVariables();
	Exported KifCode* Newinstance();
};

//------------------------------------------------------------------
class GlobalParseur {
public:
   
    
    char actif;
	char encourspourlangue;
	float ambiguite;
	int taille_phrase;
    VECTA<TestFonction*> ListeDesTestFonctions;
    
    VECTA<ListeTestArbre*> TousLesElementsRegles;
    VECTA<Sequence*> ListeFonctions;
    VECTA<Sequence*> ListeCoupeSequence;

//---------------------------------------------------------------------------------------
	short editdistanceflags;
	int editdistancescore;
	int editdistancemax;
	vector<unsigned short> idsequence;	

//---------------------------------------------------------------------------------------
//Variables dont la taille est definie par MaxCouches
    VectSequence* TableDesSequences[MaxCouches][NbCats];
	VECTA<OrdreNoeuds*> ListeOrdreNoeuds[MaxCouches];
    VECTA<Fonction*> TableDesMarqueurs[MaxCouches];    
    char ModificationDansCouche[MaxCouches];   
    automate* motsmultiples[2][MaxCouches];
    automate* motsmultiplescomplexes[2][MaxCouches];
    VECTA<Simplifie*> TableDesSimplifications[MaxCouches];
    VECTA<Cartesien*> TableProduitCartesien[MaxCouches];
    VECTA<ReconstruitArbre*> TableDesReconstructeurs[MaxCouches];
    TVecteur TabFiltre[MaxCouches][NbCats];
    TVecteur TabDroiteUn[MaxCouches][NbCats];
    TVecteur TabRegle[MaxCouches][NbCats];
    TVecteur CategoriesDansCouche[MaxCouches];
    TVecteur CategoriesObligatoiresDansCouche[MaxCouches];
    
    typeRegle CouchesActives[MaxCouches];
    
    //Les différents vecteurs pour gérer les règles de déduction
	VECTA<TokenSelection*> ReglesTokenSelection[MaxCouches];
    VECTA<VectTestFonction*> ListeDesDeductions[MaxCouches];
    VECTA<VectTestFonction*> Generations[MaxCouches];
    VECTA<VectTestFonction*> ListeDesDeductionsEnCouche[MaxCouches];
    VECTA<VectTestFonction*> TableDesFiltresDeCouche[MaxCouches];
    VECTA<vocabulaire_fragmente*> Fragmentes[MaxCouches];

	typeRegle CouchesDeduction[MaxCouches];

    automate* Semantiques[MaxCouches][2];    
    VECTA<RegleTraits*> PListes[MaxCouches];
    Regle* ListeDesRegles[MaxCouches];
    map<TVecteur,avl_Regle*> ArbReg[MaxCouches];
    automate* Termes[MaxCouches];
    automate* ArbVoc[MaxCouches];
    automate* ArbVocComplexe[MaxCouches];
    automate* ArbSurface[MaxCouches];
    automate* ArbSurfaceComplexe[MaxCouches];
	rulespace* EspaceLexique[MaxCouches];
//---------------------------------------------------------------------------------------
    LListe glisteDesPhrases[MaxCouchePhrase];
//---------------------------------------------------------------------------------------
    MemoireVirtuelleSemantique memoiresem;
	
	int MAXVIRTUEL;
    int DESTRUCTION;
    int iParseurCourant;
    int CompteSolution;
    char* EnCeMoment;
    int colonne;
    unsigned long bouclage;
    TansSinput xipsinput;
    VECTA<char*> CHAINES;
    
    //Valeur de Licenses
    char ChunkerLicense;
    char DeductionLicense;
    char SemantiqueLicense;
    char ServeurLicense;
    char UsageLimitee;
    int licenseTest;
    char DeductionFinale;    
    char DeductionInitiale;
    
    /*
    VECTA<Label*> VlabelInference;
    VECTA<Label*> VlabelSequence;       
    VECTA<Label*> VlabelSimplifie;        
    VECTA<Label*> VlabelMarqueur;
    VECTA<Label*> VlabelDI;        
    VECTA<Label*> VlabelReconstruction;
    VECTA<Label*> VlabelTermes;
    VECTA<Label*> VlabelCartesien;
    */

    
    int nbInference;
    int nbSequence;       
    int nbSimplifie;        
    int nbMarqueur;
    int nbDI;        
    int nbReconstruction;
    int nbTermes;
    int nbCartesien;
	int compteur_iteration;
	int max_iteration;

    char DateLimite[20];
    char CodeClef[255];
    char LaLocale[100];


	//Attention, dans ce cas, on casse la reentrance...
	PileArcNoeud pilebase;

    VECTA<Label*> VlabelVecteur;
	VECTA<Label*> VlabelVecteurIndex;
	VECTA<baseregle*> VBases;
	baseregle* reglecourante;

    map<string,Label*> ArbreLabel;
    
    unsigned int Limite_Numero_Regle;
    unsigned int Limite_Couche;
	unsigned int Numero_Ordre;

    VECTA<int> regles_appliquees;

    
    int numero_vocabulaire;    
    int numero_procedure;
    int numero_regle_semantique;


    char LectureParametres;
    int controle_max_regle;
    int max_controle_regle;

    char renumerotation;
    size_t nbmotsmaximum;
    
    int MaxProfondeurAnalyse;
    int MinProfondeurAnalyse;
    
    char prolog;
    
    short MaxDoublons;
    int SUITE;
    
    char cryptage;
    char encoursCryptage;
    
    int X_SIZE;
    int Y_SIZE;
    
    char gAfficherResultats;
    char gAfficherResultatsDefini;
    typeLangue laLangue; 
    
    short iTailleVect;
    int iBorneVect;
    char** TableDesTraits;
    int iNombreDeValeurs;
    int iNombreDeValeursBis;
    int iNombreDeRegles;
    char NEGATION;
	char* DECOUPAGE;
    
    VECTA<trait*> TableAttributs;
    
    char TRACE;
    char TRACETRAITS; 
    char TRACENOEUD;
    char TRACEREGLE;
    
    TVecteur AfficheFonction;
    char reentrance;
    
    char laTrace; 
    
    int NbFonctionsExtraction;
    VECTA<char*> enregListeTags;
    VECTA<ifstream*> fichiersSemantiques;
    VECTA<char*> fichiersSemantiquesNoms;
	//cette variable detecte le nombre de lecture presente dans une entree
	//provenant des lexiques
	int nb_lectures;
        
    VECTA<char*> codageXML;
    VECTA<char*> caractereXML;
    void LireCodageXML(istream& fichier);
    char reglesmultimots;
    

    
    
    VECTA<Simplifie*> ReglesSimplifie;
    char MajDesCategories(Simplifie* reg, ListeVoc* voc,Liste* L);
    void MajDesAutresCategories(Simplifie* reg,LListe& Phrase);    
    
	Label* rechercheLabel(char* label);

	char nom_fichier_grm[1024];
    char nom_fichier[1024];
    char Nom_Fichier[1024];
    char repertoire[1024];
    char Nom_Sortie[1024];
    char Nom_Trace[1024];
    char version_grammaire[1024];
    char Nom_Desamb[1024];
    char commentaire[1024];

	string GrmPathname;
    char fichieroptimise;
    char* nom_fichier_courant;        
    
    VECTA<char*> fparametres;
    char detruitfichierparametre;
    char PremierCaractereBalise[256];    
    
    VECTA<char*> Nom_regle;
    VECTA<char*> Nom_fonction;
    VECTA<char*> Nom_lexique;
    VECTA<char*> Nom_traits;
	VECTA<char*> Nom_scripts;
	VECTA<char*> Nom_divers;
    
    VECTA<char*> FinDePhrase;
    VECTA<automate*> ExpressionsFinDePhrase;
    char TAG_NOUVEAU[256]; 
	char GUESSED_TAG[256]; 
    
    int MaxCoucheAnalysee;
    int BaseCoucheNumero;
	int ValeurCoucheInitiale;
    
    char* NOM;
    char CalculFonction;
    char CalculFonctionDefini;
    
    char* TableauDesNoms[NbCats];
    
    ofstream* Sortie; 
    
    int LIGNES;
    int LIGNESERREUR;
    int MaxSentence;
    long POSITION_DANS_FICHIER;
    
    int XML;
    
    int appliqueGrammaire;
    int appliqueGrammaireDefini;
    int appelFactorise;
    int tailleFactorise;
    char parse_phrase;
	int punctuations;
    
    TVecteur Zonage;
    TVecteur FusionX;
    TVecteur CategoriesCoupeSequence;
    TVecteur CategoriesCoupeSequenceInterieur;
    
    TVecteur TabDroite[NbCats];
    TVecteur TabGauche[NbCats];
    TVecteur TabFinals[NbCats];
    
    TVecteur TabMembre[NbCats];
    TVecteur TabDerniers[NbCats];
    TVecteur TabContexte[NbCats];
    
	VECTA<VectTestFonction*>* DeductionDeKif;
    
    VECTA<VectTestFonction*> QueueDeductions;
    VECTA<VectTestFonction*> FinDeductions;
    VECTA<VectTestFonction*> DebutDeductions;
    VECTA<VectTestFonction*> PreBaliseDeductions;
    VECTA<VectTestFonction*> PostBaliseDeductions;
    VECTA<VectTestFonction*> ChgtDeductions;
    VECTA<VectTFInstruction*> ListeDesProcedures;
    VECTA<int> IndexProcedures;

    


    int laCoucheDeduction;
	int laCoucheSemantique;
    char couchesemantique;
    
    
    int dernierNumeroNoeud;
    char TabLexique[NbCats];
    TVecteur ATTENTE;
    
    Traits* MAJEURES;
    Traits* MAJUSCULE;
    Traits* TOUTMAJUSCULE; 
    Traits* traithmmselection;

	Traits* traittaggerguessed;
	Traits* traitcrfselection;

    char* LEMME;
    char* SURFACE;
    
    VECTA<RegleTraits*> Vregle[4];
    
    TVecteur ParametresAffichage;
    TVecteur ParametresAffichageDefini;    
    
    Cat* contexteFoncVide;
    int iCategorie;
    int MaxIndiceCat;
    int MaxIndiceFonc;
    
    VECTA<Categorie*> Categories; /* Voici la liste globale qui contient les noms de catégories*/
    VECTA<Categorie*> Fonctions;
    VECTA<Categorie*> Procedures;

    VECTA<Liste*> NoeudsFictifs; 
    VECTA<Traits*> TraitsNoeudsFictifs;
    Categorie* CatVide;
    Categorie* CatNeg;
    Categorie* CatEpsilon;
	Categorie* creationtoken;
	Categorie* surfacetoken;
	Categorie* Xproposecategorie;
    
    Categorie* Affichage;


    Cat* XCatVide;
    Cat* XCatNeg;
    Cat* XCatEpsilon;
	Cat* Xcreationtoken;
	Cat* Xsurfacetoken;
    
    VECTA<avl_Regle*> ListeDesAvlRegles;
    VECTA<Traits*> ListeDesTraits[TailleHachageTraits];
    
    char AFFICHE_ERREUR;
    
    Regle* ListeDesFonctions;
    
    trait* ArbTraits;
    trait* DebutSeq;
    trait* FinSeq;
    trait* DebutPhrase;
    trait* FinPhrase; 
    trait* AmbiguiteErreur;
    trait* TraitNoeudTerminal;
	trait* TraitPosition;
	trait* tokenstart;
	trait* tokeninter;
	trait* tokenend;
	trait* untoken;
	trait* PasAmbigue;
	trait* Fictif;
	trait* guesser;
    

    Hierarchie* hierarchies;
    Hierarchie* relations;
    Hierarchie* concepts;

	VECTA<graphe*> listegraphes;
	int premiergraphelibre;

    char enAttente[1000];
	char guesser_feature[100];
    
    abreviation* Abreviation;
    ListeDefaut* Defauts;
    

    automate LesBalises;    
    Traits* TraitsFiltreOntologie;
    

    arbretrait* larbredestraits;

    
    avl_vocabulaire* tag_nouveau;
    
    trait* LTraits;
    char DESAMBIGUISATION;
    char DESAMBIGUISATIONDefini;
    char TAGGING; 
    char IEspace;
    
	Vecteur* TOUSTRAITS;
    Vecteur* FILTRE;
    Vecteur* FILTRENOEUD;
    Vecteur* FILTREFONCTION;
	Vecteur* FILTREGENERATION;
    
    Vecteur* BaliseTraits; 
    
    char LectureRegles;
    char encodage[100];
	encodingType encodage_courant;
	encodingType encodage_source;
	encodingType encodage_grammaire;
	encodingType encodage_fst;
	bool encodageutf8;

	string crfmodel;
	string crfmkcls;
	string crfbrown;
	void* crftagger;

    avl_vocabulaire* Traductions[HACHE];

	VECTA<DefinitionCategorie*> definircategories;
	VECTA<DefinitionCategorie*> definirtraits;
	
	vector<string> fichiersdata;

	//----------------------------------
	//Variables KiF
	vector<string> cheminkifs;	
	//KifCode* kifcode;
	KifCode* kifloader;
	KifObjectInstruction* kifparametres;
	KifFunction* interceptexml;
	VariableKif* vkif_echange;
	string nomvariablekif;
	bool kifdebug;
	//----------------------------------

    int IndiceCreeTrait;
    int IndiceCreeValeur;
    int indiceCreeValeur;
    Categorie* Inconnu;
        
    char ARBREIND;
    char mathematique;
    
    FILE* fichierDonnees; 
    unsigned char* donneesATraiter;
    unsigned char* curseurDonnees;
    VECTA<int> ReglesEntrelacees;
    VECTA<Variable*> Variables;
    VECTA<VariableLemmeIndex*> VariablesLemmes;

    automate VariablesNoms;
    automate VariablesProceduresGlobales;
    
    
    //---------------------------------------------------------------------------------------
    //LISTE DES VARIABLES UTILISEES PENDANT UNE ANALYSE
    //cette variable permet de fournir une liste de LListe pour chacune des couches
    //diminuant ainsi le nombre d'elements a detruire lors des analyses.
    //Les analyses successives puisent chacune dans cette liste pour les phrases...
    
    
    //Le compteur ci-dessous est utilise pour determiner l'incompatibilite entre
    //dependance..
    int CompteurIncompatible;
    int demarre_regle;
    int demarre_couche;
    int compte_boucle_inference;    
    double valeur_retour;

    FournirEtatArbre listeEtatArbre;

    Label* aller_label;
    Label* label_terminal;
    Label* label_retour;
    Label* label_stop;

    fournitureRF* fournirResFonc;
    TVecteur ClefDependances; 
	TVecteur ClefArbre; 
    Traits* TraitsDependances;

    ostream* OS;
    istream* donnees;
    char detruireDonnees;

    Noeud* INTERVICTOIRE;
    Liste* LINTERVICTOIRE;
    
    long baseOffset;    
	long  baseOffsetCar;
    long baseOffsetToken;
	char presencetoken;

	int derniere_erreur_survenue;
	int erreur_survenue;
	char ajoute_info_erreur[4096];

    Traits* gBaliseTraits;
    Traits* gBaliseDebut;
    Traits* gBaliseSansFin;
    
    VECTA<auto_vocabulaire*> commutateurs;
    
    ResultatExtraction INTERFONCTIONS;
    VECTA< ResultatFonction*>* FONCTIONSKIF;
    VECTA< ResultatFonction*> FONCTIONSINTERMEDIAIRES;
    VECTA< ResultatFonction*> FONCTIONSDISJOINTES;
    ResultatExtraction PREMIERFONCTIONS;
    ResultatExtraction SECONDFONCTIONS;
    ExtractionFonction MemoireFONCTIONS;
    
    LListe IndexParCat[NbCats];
    TasListe* TasDeListes;
	VECTA<ListeVoc*> TasDeListeVoc;
    VECTA<Liste*> NoeudsTemporaires;
    	
    XipResult* xipresult;
    XipFunction AppelExterne;
    void* donneesExternes;
    
    VECTA<ListeVoc*> phrase;
    VECTA<ListeVoc*> reste_phrase;
    VECTA<ListeVoc*> phrasefictifs;

    long borneGauche;
    long borneDroite;
    long borneGaucheCar;
    long borneDroiteCar;
    VECTA<int> NombreDependances;
    
    VECTA<Noeud*> NoeudsLexicaux;
    VECTA<Liste*> NoeudsDependance;
    
    int num_phrase;
    int num_mot;
	int num_token;
    
    char Chaine_traits[3000];	
    
    int socketcourante;
    ushort lePortServeur;
    char dependanceModifiee;
    long nombre_mots;
    char TRACEDEPENDANCE;
    //Utilise dans l'analyse d'une forme
    VECTA<auto_vocabulaire*> nouvellesLectures;
    VECTA<auto_vocabulaire*> lemmesDejaLus;
	int readingposition;
    
    vocabulaire vlex;
    
    char LuEntreCotes;
    VECTA<TVecteur> filtreGlobalDependance;
    VECTA<stats*> statistiques;
    VECTA<stats*> lesstats;
	rulespace* spacename;
	rulespace* currentspacename;
	string commentaire_regle;

    int taille_mot;
    int fenetre;
    VECTA<ArcArbre*> declencheurs;
    char FinAnalyse;
    char* nom_procedure;
    long debut_traits_dans_langue;
    long debut_lexique_dans_langue;
    long debut_regles_dans_langue;


    char* langue_de_la_grammaire;
    char langue_de_la_phrase[100];

    char pretraitement;
	string phrasecourante;

	char python_debug;
	int num_python_ligne;
	int num_fichier_python;
	int fullpythonlexical;

    VECTA<fichiersscript*> lesfichiersscripts;
    vector<string> definition_var;
	LListe* phraseCourante;
	string laststrgen;

	//Generation
#ifdef NTMCONNECTION
	genereGraphe graphegeneration;
	VECTA<genere_traits*> LesTraitsdeGenerations;
	VECTA<VECTA<genereGraphe*>*> listedetokens;
	VECTA<genereGraphe*> lestokens;
	void parcourssousnoeud(genereGraphe* g,string& p,char&);
	void parcoursgeneration(string& s);
	int ListeTokens(VECTA<automate*>& expressions);
	void ConstruitMotGraphe(genereGraphe* g,char& premier);
	void parcoursrecursifinit(genereGraphe* g,char& premier);
	void parcoursrecursif(genereGraphe* g);
	ResultatFonction* tokendufond(ResultatFonction* courant,ResultatFonction* suivant,char dernier);
	TokLOOK* tokdown;
#endif

	word_offset wo;
	map<string,int> variablesgenerations;
	void raz();
	VECTA<word_offset*> lectures;
	//Utilises par Trans et Toklook
	word_offset* fournirWordOffset();
	void nettoieLectures();
	void libereLectures(int t);
	virtual void appel(char* internal,long offset) {}
	
     
    //-----------------------------------------------------------------------
    //LISTE DES FONCTIONS D'ANALYSE 
    
    //---------------------------------------------------------------------------------------
    /*Liste des fonctions*/
    void Desambiguisation(char v) {
        DESAMBIGUISATION=v;
        DESAMBIGUISATIONDefini=v;
    }
    
    void Tagging(char v) {
        TAGGING=v;
    }
    
    void erreur(char* Message);
    void insere_erreur(char* Message);
    char construitchainegenere(genereGraphe*,Liste*,string&);
	char lecturemontante(string&,VECTA<string*>&);
	char lecturedescendante(string&,string&,string&);
	char initgenere(Liste* L,string&);
    trait* CreeTrait(const char* tampon);
    int CreeValeur(trait* avl,const char* tampon,const char* NomPere);
    int CreeValeurSans(trait* avl,const char* tampon,const char* NomPere);
    trait* CreationTrait(const char* attribut,const char* valeur);
    trait* ConstAttributs(istream& fichier, char& k);
    trait* RecAtt(istream& fichier,char& K,char racine);
    char RecVal(istream& fichier,trait* tc,const char* nom);
    char RecDomaineValeur(trait* avl,const char* tampon,const char* NomPere);
    char testEspaceRegle(baseregle* b);
	int VariableGeneration(string v,bool);

	void SetEndOfAnalysis(char v) {
		FinAnalyse=v;
	}

    char desactiveEspaceRegle(char* nom);
    char reactiveEspaceRegle(char* nom,char full);
    void TestTraitVide(Traits* tr);
    double AppliqueBoucle(TestFonctionMathBoucle*,
        int laCouche,
        typeDeduction,
        int i,
        VECTA<Noeud*>&,
        VECTA<ResultatFonction*>* vfoncs);
    double DeduitFonction(int laCouche,
        typeDeduction,
        char appel_recursif,
        int demarre=-1,
        VECTA<Noeud*>* vnoeuds=NULL,
        VECTA<ResultatFonction*>* vfoncs=NULL,
        int retour=-1);

    Categorie* CreeFonc(char* mot,int index,Traits*,char* fichier);
    Categorie* CreeProcedure(char* mot);
    Categorie* ChercheFonc(char* mot);
    Categorie* CreeCat(char* mot,char* fichier);
    Categorie* ChercheCat(char* mot);
    Dependance* ConstruitDependance(istream& fichier, VECTA<char>& vari, char k,char* tampon,char test=OUI,typeDeduction type=DEDUCTIONCOUCHE);
    Dependance* ConstruitNomProcedure(istream& fichier,char k,char* tampon);   

    void Commentaires(char* nom,typeLangue langue);
    
    void Nettoyage();
    void Chainage(istream& fichier,char* ChaineTest,char* nom,char* tampon,char k);
    void ChainagePlusieurs(istream& fichier,char* ChaineTest,char* tampon,char&,
        VECTA<char*>& noms,char entrelacement,long&);
    char LectOuiNon(istream& fichier,char* tamp,char* chaine,char* reponse);
    void LitNomFichiers(void);
    void LitMarqueur(istream&);
    void LitReg(char*);
    void LitExtraction(istream&);    
    void LitProcedure(istream& fichier);

#ifdef XIPPYTHON
        VECTA<ProcedurePython*> PythonScript;
        void LitProcedurePython(istream& fichier);
        static PyObject* module_principal;
        PyObject* espace_local;
        void initialisation_python();
        VariablePython* vpython_echange;
		int num_module_python;
#endif

    void LitSequence(istream&,typeRegle type=REGLES_SEQUENCE);
    void LitReconstruction(istream& fichier);
    void LireParametres(char*); 
    Sequence* LitUneSequence(istream&,Cat*,char neg,int sens,typeRegle regle=VIDE);    
    void LitUnAutomate(istream& fichier,Cat* ctete,VectTFInstruction*,char k,typeRegle regle,char stop='|');
    void LitVoc(char*,char);
    void ConsAffiche(istream& fichier,Vecteur**);
    char ConsAtt(istream& fichier);
    char ConsPL(istream& fichier);
    int RetrouveTrait(trait* vr,char* tampon,char gestion_erreur);
    char AppelCons(istream& fichier,char k, char* tampon);
    void ConsVregle(char type,istream& fichier);
    void ConsVregleParametres(char type,char* tampon,istream& fichier);
    void ConsAbrev(istream& fichier);
    void ConsDefaut(istream& fichier);
    char ConsCategorie(istream& fichier);
    void ConsTraduction(istream&);
    void ConsGenereTraits(istream&);
	void ConsDefinitionCategorie(istream& fichier);
	void ConsDefinitionTraits(istream& fichier);
    char ConsFonction(istream&,char);
    char AffichageFonction(istream& fichier,char cacher=OUI);
    void LitCategories(char*);
    char TestBalise(char* entree);
	char TestMarkupTag(char* entree) {
		return TestBalise(entree);
	}
    auto_vocabulaire* RechercheBalise(char* entree);
    void AffecteBalise(auto_vocabulaire* trad);
    ArcArbre* ConstruitArc(istream& fichier,
        char* tampon,
        char& k,
        typeRegle regle,
        char negation,
        char& optionnel,
        char& modification,
        EtatArbre* lesuivant,
        char caracterefinal);

    Traits* BatitTraitsPlus(istream& fichier,Categorie* cc,char* K,char* Tampon,char* testLecture=NULL);
    void ConsSuiteRegDI(istream& fichier,int laCouche,Sequence**,Sequence**,
        VECTA<ChaineCat*>& travail,int& I,char& where);
    char ConsRegDI(istream& fichier);
    void ConsMajuscule(istream& fichier,Traits**);
    Traits* BatitTraits(istream& fichier,char finCaractere=']',VECTA<char*>* pile=NULL,char* clear=NULL);
    char ConsVoc(istream& fichier,char lire_couche);
    char ConsSem(istream& fichier);
    void ChargeSem(auto_vocabulaire_semantique* vs,char positionner);
    void InitRegleDI(Regle* reg,VECTA<ChaineCat*>& travail,int i,int k,int PremierPassage);
    char VireCategorie(ListeVoc* voc,Liste* L,char majtraits);
    void AppliqueDI(LListe& Base,LListe& nouveau,int profondeur);
    void NettoieTraitsIncoherent(Traits* tous, Vecteur& filtreur);
    char FusionneTraits(Traits* recoit, Traits* nouveau, Vecteur& filtreur);
    
    void ActiveCouche(LListe& Base,int laCouche);
    void MAJInterVictoire(LListe& base);
    char coherent(int laCouche,Liste* l,TVecteur Y,TVecteur Cle);
    void Execute(char sauve_resultat,char construit=NON);
    void Parse(char sauve_resultat,char construit=NON);
    void FusionneForme(ListeVoc* lv,char* entree);
	void AddToCurrentReading(ListeVoc* lv,char* entree) {
		FusionneForme(lv,entree);
	}
	void IncrementWordIndexes() {
		num_mot++;
		nombre_mots++;
	}
	void IncrementSentence() {
		num_phrase++;
	}

	void SetColumn(int c) {
		colonne=c;
	}

	void ResetMarkuptags();
	void FusionneUneForme(VECTA<char*>& enreg,ListeVoc* lv,char* entree);
    ListeVoc* ConstruitForme(char* entree,int num_mot);
	ListeVoc* BuildFirstReading(char* entree,int num_mot) {
		return ConstruitForme(entree,num_mot);
	}
	ListeVoc* EclateVoc(char* entree,int num_mot,ListeVoc* lv);
	ListeVoc* ConstruitUneForme(VECTA<char*>&,char* entree,int num_mot,char ajoute);
    char AnalyseForme(VECTA<char*>&,char*,VECTA<Cat*>& c,char,char&,int);
	bool FeatureAnalysis(vector<string>& features,string& catog,Traits* tlocal);
	Cat* AnalyseTrait(string& feature,Traits* tlocal);
            
    void LitSimplifie(istream& fichier);
    void LitCartesien(istream& fichier);
    
    void AppliqueSequence(LListe& Base,LListe& nouveau,int,typeRegle);
    void AppliqueTokenSelection(LListe& Base,LListe& nouveau,int,typeRegle);
    void AppliqueMultiMots(LListe& Phrase,LListe& NouvelleListe,char typeregle,int laCouche);
    void AppliqueFragmente(LListe& Base,LListe& nouveau,int,typeRegle);
    char ConsListePhraseAvecDesamb(char echec);
    void ConsFiltre(istream& fichier,Vecteur**);
    void ConstruitResultat(XipResult* xip);
    ResultatFonction* FournirResFonc(Cat* x, ResultatFonction* rf);
    void RendreResFonc(ResultatFonction* r);
    
    char AppliqueSimplification(LListe& Phrase,int laCouche);
    void AppliqueSemantiques(int laCouche,char typecouche);
    void AppliqueReconstruit(LListe& Phrase,int laCouche);
    void MarqueNoeuds(LListe&,int laCouche);
    char AppliqueProduitCartesien(LListe& Phrase,int laCouche);
    
    void LectureTest(istream& fichier,char& k,char* tampon,char negation,
        VECTA<char>& vari,
        TestArbre** tarbre,
        char carfin=')',
        char profondeur=0);
    
    char CompareNoeudSousArbre(ArcArbre* opt,Noeud* n);
    char ExistePL(int laCouche,Traits* vg,Traits* vd,Cat* cg,Cat* cd);    
    RegleTraits* TrouvePLDroite(VECTA<RegleTraits*>& PL,int&,Traits* valeur,Cat*);
    void ConstruitTeteDroite(int laCouche,VECTA<Traits*>& Trait,VECTA<Cat*>& liste);
    void ConstruitDerniers(int laCouche,Cat* tete, TVecteur  cle);
    void ConsMembre(int laCouche);
    void ConsFinals(int laCouche);
    void ConsATTENTE(int laCouche);
    void FinalisePourEtoile(int laCouche);
    void Applatit(ChaineTraits* Tr,VECTA<Traits*>& listeTraits,VECTA<Cat*>& liste);
    void CalculTeteDroite();
    Traits* FactoriseTraits(Traits* ts);
    void RAZContextePL();
    void Detruit();
	void Clean() {
		Detruit();
	}
	void SetWordNumber(char renumber) {
        if (renumber==1)
            num_mot=0;
        else //otherwise we increment it
            num_mot++;
		nombre_mots++;
	}


    void ReinitPhrase(Liste* L, ListeVoc* voc,int i,Traits* reg_traits);
    
    char TestTransitivite(int laCouche,Traits* X,Traits* Y,VECTA<Traits*>& Trait,Traits* tr,Traits* Tr,Cat*,Cat*);
    void ResultatLecture(int* nbRegles,int* nbValeurs, int* borneVec,int* nb_fonc);
    void Graphe();
    char ChoixLectureRegle(istream& fichier,char* tampon);
    void AffichePhrase(FILE* fichier);
    
    char PTT(Traits& sommet,Traits* traits);
    char PTT(Traits* temporaire,Noeud* premier,Noeud* dernier);
    char PTT(Liste* l,LListe& Base,size_t ipremier,size_t idernier);
    char AppVregle(Traits* traits,Cat*,short type);
    char InitTraits(Liste* l,LListe&,size_t,size_t,
        ChaineTraits*,ChaineTraits*,
        int nbTraits,
        VECTA<Noeud*>* vect_noeuds=NULL);
    char AppPlCouche(int laCouche,LListe& Base,size_t ipremier,size_t idernier);
    char AppPl(int laCouche,LListe& Base,size_t ipremier,size_t idernier);
    
    int MajDependance(VECTA<Noeud*>& vect,
        VECTA<ResultatFonction*>& chgts,
		VECTA<ResultatFonction*>& creees,
        VectTFInstruction* vecTF,
        Dependance* dep,
        Signature* s,
        int num_dep,
        Traits* temporaire);
    
    
    void ListeTagsInitialisation();
    
    
    void ListeTagsRAZ(char*,char**);
    void ListeTagsAjouter(char* entree);
	void SetLowerCase(unsigned char* iterateur);
    unsigned char LectCrypte(istream& fichier);	

    void MetEnAttenteLecture(char k,char* tampon);
    char lit(istream& fichier,char *tampon);
	char litOperateur(istream& fichier);    
	char litChemin(istream& fichier,char *tampon);
	char* lireRulespaceLabel(istream& fichier,char* tampon,unsigned char&,char);
	void LitUneLigne(istream& fichier,char *tampon,char premier);
	char litFormule(istream& fichier,char *tampon,char stop=NON);
    char litSans(istream& fichier,char *tampon);
    char Llit(istream& fichier,char *tampon);
    char LitEgal(istream& fichier,unsigned char *tampon);
    char LitEgalBrut(istream& fichier,unsigned char *tampon,char type_lecture);
    char LitJusque(istream& fichier,unsigned char *tampon,char stop);
    char litBis(istream& fichier,char *tampon);    
    void lireunevariable(istream& fichier,unsigned char& k,char *tampon);
    void LitLigne(istream& fichier,char *tampon);
    char LitTantQue(istream& fichier,unsigned char *tampon,const char* stop,const char* guillemet);
    char LitCommentaires(istream& fichier,unsigned char cc,unsigned char& r);
    void LitDefinition(istream& fichier,unsigned char cc,unsigned char& r);
	void LitInstructions(istream& fichier,unsigned char cc,unsigned char& r);
    void LectureBlancs(istream&,unsigned char& k,char commentaire='/');
    void LectureBruteBlancs(istream&,unsigned char& k,char commentaire='/');
    string CrypteFichier(char* nom);
    
    
    void RemplaceChaine(ostream& buffer,char* chaine);
    void NettoieLaChaine(char* chaine,char* resul);
    void MetEcran(int y,int x,char c);
    unsigned char LitEcran(int y,int x);
    void InitEcran();
    void AfficheEcran(ostream& os,long maxX,long maxY);
    void AfficheCoupe(ostream& os,long maxX,long maxY);
    Regle* recherche(ChercheRegle& reg,avl_Regle** ar,short laCouche,char& premier);
    char RechercheDefaut(Vecteur* v);
    char MetDefaut(Vecteur* v,Vecteur* valeur);
    void InstancieTraits(Traits* tr,Traits* RegTraits);
    char TestTraitsReg(Traits* reg, Traits* noeud,char* lemme,char* surface);
    char SelectionneVocabulaire(Liste* L);
    char TestTraitsDeListe(Liste* L,Traits* tr);
    //char ITR(Traits* RegTraits,Traits* tr,char type,Noeud* noeud=NULL);
    char CompareCategories(Liste* L,Cat* X,Traits* selectiontous,Traits* selection,char maj,char& couverture);    
    char CompareNoeudSousArbre(ArcArbre* boucle,Liste* L, char maj,char& couverture);
    void Espace(char);
    void MajINTERFONCTIONS();
    void InjecteFONCTION(ResultatFonction* nouvelleDependance);
    void AjoutDansINTERMEDIAIRE(ResultatFonction* r);
    void AffichageResultat();
    char CompareLesTraits(Noeud* ngauche,
        Noeud* ndroit,
        Traits* testtrgauche,                      
        Traits* testtrdroit,
        typeComparateur comparateur);    
    char CompareEntree(ListeVoc* v,Cat* X,Traits* tr,Traits* selectiontous,Traits* selection);
    void ExecuteExtraction();
    
    void LitCoupeSequence(istream& fichier);
    void InstancieContexte(LListe& Base,int ipremier,int idernier,char tout=NON);
    char LitVariables(istream& fichier,NomProcedure* nomproc=NULL);
    
    char TestTraitsFonction(Traits* tfTraits, Traits* resTraits,VECTA<char*>&,char,VECTA<char*>&);
    void lireLigne(char* ligne,unsigned int taille);
    int finLecture(void);
    void LitDoubleTab(char* debut,
        char* ligne,
        int taille,
        char premier);
    
    
    
    void Analyse(char renumerote,char tree);
	int ChargerDependances(string);
    void TestUtf8Chaine(unsigned char* chaine);
	void TestUtf8Fichier(FILE*);

	//These methods are the one to derive to implement another XIP based application
    void FileStreamAssociation(istream* is) {
        donnees=is;
        detruireDonnees=NON;
    }
    virtual void LexiconEntryParsing(char* chaine,char** duplicat);
    virtual char EndOfSentence(char* ligne,char type=OUI);
    virtual void SentenceAnalysis(char renumerote,char tree);
    virtual void Initialisation(char* rep);
    virtual void InitialisationParametres(char* parametres);
    virtual void AddAToken(unsigned char* tok);    
    virtual FILE* FileAssociation(FILE* fichier) {
		fichierDonnees=fichier;
		if (TestOptions(TEST_INPUT_UTF8))
			TestUtf8Fichier(fichier);
		return fichierDonnees;
	};
    virtual void StringAssociation(unsigned char* chaine) {
        donneesATraiter=chaine;
        curseurDonnees=chaine;
		if (TestOptions(TEST_INPUT_UTF8))
			TestUtf8Chaine(chaine);

	};

    void StringStreamAssociation(unsigned char* chaine) {
#if (defined(XIPSTLUSE) || defined(XIPSTLAPI))
        donnees = new istringstream((char*)chaine);
#else
        donnees = new istrstream((char*)chaine);
#endif
        detruireDonnees=OUI;
    }

	void CleanStream() {
		if (detruireDonnees==OUI)
			delete donnees;
		detruireDonnees=NON;
		donnees=NULL;
	}

    virtual void StartAnalysis() {}
    virtual void CloseAnalysis() {}

    void ChargeIndex(istream& fichier);
    
    
	void InitialisationAvecParseur(int ipar);
    int InitialisationParseur(typeLangue langue,
        char* stringfile,
        char* chemin,
        int numberValues,
        char laTrace,
        TVecteur affFonc,
        char outputXML,
        char tagger,
        char tagging,
        char math,
        char crypte,
        char aff_err,
        char afficherResultats,
        TVecteur ordreDependances,
        TVecteur offset,
        TVecteur red,
        TVecteur tree,
        char fonc_syn,
        VECTA<char*>& fileparam);


	void InitialisationParseurMin(TVecteur affFonc,
		char outputXML,
		char tagger,
		char tagging,
		char math,
		char aff_err,
		char afficherResultats,
		TVecteur ordreDependances,
		TVecteur offset,
		TVecteur red,
		TVecteur tree,
		char fonc_syn);

    void NettoieVecteursTravail();
    //Les initialisations...
    GlobalParseur(char* nom,char* path);
    
    virtual ~GlobalParseur();
#ifdef XIPSERVER    
    void LancerServeur();
#endif
    void RemetCaractere(unsigned char);
    void VerifieEtCompare(char* tampon);
    int LicenseTest();
    void raz(char);
    //char TestLexique(avl_vocabulaire* trad);
    char TestLexique(auto_vocabulaire* trad,char ajoute_lecture,int);
    void NettoiePhrase();
    TestFonction* LectureArgumentTest(istream& fichier, VECTA<char>& vari,char,
        char* tampon,char& k,char neg,char lectxml=0);
    
    TestFormule* LireUneFormule(istream& fichier,char&,VECTA<char>&,char test=OUI,char kifmode=NON);    
    char LireEntreCotes(istream& fichier,char* tampon,unsigned char& cc,int& i,const char* guillemet="\"");
    char LireEntreCotesBrute(istream& fichier,char* tampon,unsigned char& cc,int& i,const char* guillemet="\"");
    void AjouteMajuscule(char majus);
    void AfficheVariablesGlobales(); 
    void StockeElementRegles(ListeTestArbre* l);
    void StockeTestFonction(TestFonction*);
    void Pretraitement(char v) {
        pretraitement=v;
    }
    void InitialiseVariable(Liste* L,VECTA<Stocke_Variable*>& StockeVar);
    char XipFeatureTraits(Traits* tr,char* attribut,char* valeur);
    char TestXipFeatureTraits(Traits* tr,char* attribut,char* valeur);
	char XipFeatureSubTraits(Noeud* n,char* attribut,char* valeur);
    Traits* BuildXipFeature(VXipFeature& features,VECTA<Stocke_Variable*>& StockeVar);
    void BuildXipNodeLeaf(Liste* pere,XipNode* noeud);
    void BuildXipNode(Liste* pere,LListe& Phrase,XipNode* noeud);
    void BuildXipDependency(XipDependency* depend,VECTA<Stocke_Variable*>& StockeVar);
    void AnalyseXipResult(XipResult* xipres);
    void AnalyseXipNode(XipNode* racine);
    void AnalyseXipLeaves(XipUnit* unite);
    void EcrasementCategorie(Cat* nouvX, Traits* nouvTR,ListeVoc* voc,Liste* L);
    void AnalyseXipVECTA(VECTA<XipNode*>& unite);
    int Preparation(char*);
    void afficheregle(int num,const char* nom);
    
    virtual typeParseur Type() {return PARSEURGLOBAL;}
    void AffichageArbreModeTexte(ostream*,Noeud*);
	void AffichageGenerationModeTexte(ostream*);
    trait* RetrouveValeurTrait(trait* vr,char* tampon,char gestion_erreur);
    void IndexationArbrePartiel(LListe& Phrase,char);
    void AnnulationArbrePartiel(LListe& Phrase);
    Liste* FactoriseNoeudFictif(Traits* ts,Categorie* c,char ajoutefils,char& ajouter);
    Liste* CreeNoeudFragment(char* mot,Traits* ts,Cat* c,int ngauche=-100,int ndroit=-10);
	Liste* DupliqueNoeud(Liste* noeud);
    void FiltreDependance(int nb);
    void TraiteXipResult();    
	rulespace* regleEspaceRegle(char* sname);
	void LireAutomate(istream& fichier,                                                                                        
        char fin,
        SeqFonc* fonc,
        typeRegle regle,
        VECTA<ArcArbre*>& liste,
        char sens);
        
    inline char TestOptions(TVecteur test) {
        if ((ParametresAffichage & test) == test)
            return OUI;
        return NON;
    }

	bool UTF8();
    
    EtatArbre* ConstruitAutomate(VECTA<ArcArbre*>& liste,
        int& iliste,
        SeqFonc* fonc,
        char negation,
        typeRegle regle,
        char,int,int,
        EtatArbre** queue);

    inline char TestAfficheFonction(TVecteur test) {
        if ((AfficheFonction & test) == test)
            return OUI;
        return NON;
        
    }
    
    
    Label* AjouteLabel(VECTA<Label*>& v, char* e, typeRegle t,int c,int&,DecompteRegle*);        

    string chainetraduction;
    unsigned char* conversionUTF8VersAscii(unsigned char* contenu);
    unsigned char* conversionAsciiVersUTF8(unsigned char* contenu);        
    char* fichier_en_cours;

    void renseigne(char* f);

#ifdef XIPLIBXML
    
    
    string repertoireresultat;

    VECTA<xptpos*> liste_offsets;
    automate balisesinsertion;
    VECTA<aligne*> alphrase;

    xmlDocPtr refdocument;
    string chaineinsertioncourante;
    string amorce_affichage;	

	VECTA<arbretrait*> vlarbredestraits;

	arborescenceXML* basexmlcourante;
	arborescenceXML* basexmlreference;
    VECTA<arborescenceXML*> BaseDonneesXML;
    xmlDtdPtr xipdtd;
    xmlNodePtr noeudxmltokencourrant;
    XipFunction AppelExterneXml;
    void* donneesExternesXml;

    XMLEVENEMENT defautEvenement;
    XMLACTION defautAction;
    XMLDESCRIPTION CompareAvecDTD;
    VECTA<char*> ListeAlias;
	VECTA<Noeud*> noeudsaplat;

    Categorie* XmlTagCat;    
    xmlNodePtr XmlLunit;
    xmlNodePtr XmlResult;

 //   int XmlBDXMLCourant;	
 //	  xmlNodePtr XmlNodeCourant;
 //   xmlNodePtr XmlCourant;
 //   xmlNodePtr XmlReferenceNode;        
 //   xmlXPathContextPtr xpathcourant;
    VECTA<XPathIndexation*> xpathindex;
	VECTA<xmlNodePtr> listeDesNodesCourants;
	void NodeReference(xmlNodePtr node);

	VECTA<dtdXML*> InformationDtd;
    int num_analyse_xml;
    unsigned char* balise_analyse;
    int profondeur_analyse;
    char* texte_xml_courant;
    void ajouteXpathIndex(clefXML*);
    void ChargerFichierXML(char* nom,char* alias);
    char ExecuteXML(clefXML* entree,VECTA<Noeud*>&,VECTA<ResultatFonction*>&,xmlresultat*);
    clefXML* LectureXMLDB(istream& fichier,char& k,char* tampon,char negation,VECTA<char>& vari);
    dtdXML* evalueTypeNoeud(xmlNodePtr node);
    void AnalyseRecursiveXML(xmlNodePtr pere,string& accumulateur,dtdXML*,char renumerote,char arbre,XMLEVENEMENT courant,int profondeur);
    void ExtraitMoelleBalise(xmlElementPtr elem);
    void accumulation(string& accumulateur,char* c);
    void AnalyseXDTD(char* xdtd);    
    void AjouteAttributValeur(xmlNodePtr node,char* attribut,char* valeur);
    xmlNodePtr CreationNoeudXML(xmlNodePtr parent,char* balise,char* texte);
    void AnalyseFichierXML(char* textexml,int profondeur,char renumerote,char tree);
    void AnalyseChaineXML(char* ch,int profondeur,char renumerote,char tree);
    void ParseFichierXML(xmlDocPtr doc);    
    void RecupereContenuXmlNode(string& accumulateur,xmlNodePtr node,int);
    clefXML*  ParseXPathExpression(istream& fichier,char& k,char* tampon,char negation,
                                     VECTA<char>& vari,char litXMLGuide=NON);
    void insertiondenoeuds(xmlNodePtr node,Noeud* nds,xmlChar* balise,char* attributs);
	xmlNodePtr creeNouveauNoeudXML(xmlDocPtr doc,xmlNsPtr ns,xmlChar* name,xmlChar* contenu);
    char LitXMLGuide(istream& fichier);
    XMLACTION DetermineAction(XMLEVENEMENT,char* action);
    void GenereFichierXML(char* texte,char* racinebalise,char* encodage,char renumerote,char tree);
    void AnalyseChaineTexteVersXML(string textefichier,char* racinebalise,char* encodage,char renumerote,char tree);
#endif
    void DeterminePremiereCategorie(Sequence* fonc,EtatArbre* cle,int);
    char RecuperationTraitValeur(TestFonction* test,string& valeur,VECTA<Noeud*>& vectNoeuds,VECTA<ResultatFonction*>* focus=NULL);
    EtatArbre* ConstructionAutomate(istream& fichier,
        SeqFonc* fonc,                                            
        char negation,
        char fin,
        char optio,
        int increment,                                         
        typeRegle regle,
        EtatArbre** queue);
    char TestNomChamp(char* tampon,char k);
    void AjouteSem(auto_vocabulaire_semantique* vs);
    void RenvoieErreurAutomate(autoerreur err);
    void LitLesDeductions(istream&,typeDeduction);
	void litsousdependances(istream& fichier,unsigned char&,char*,SousDependance* sdep,VECTA<char>& vari);
    void LitOntologies(istream& fichier,Hierarchie&,hierarchie*,char&);
    void ParseOntologie(istream& fichier,Traits* valeur,char&,char);
    void ParseOntologie(char* chaine,Traits* resultat);
	ontologie* OntologieTrait(char* chaine);
	void AffecteSousNoeuds(Noeud* n,trait* attribut,trait* valeur,ontologie* v);
    void ChoixUTF8(long conv);
    char TraitementVocabulaire(char ajoute_lecture,int couche,int dernierelement);
    Categorie* RetrouveFonc(char* mot);
    void AppliqueVocabulaire(int laCouche);
    void RechercheVocabulaire(VECTA<void*>& a,int laCouche,char* lemme,char* surf);
    void ExtraitVocabulaire(VECTA<void*>& a,int laCouche,vocabulaire& v);
    Categorie* RetrouveCat(char* mot);
    char incrementestats(baseregle* r);
    void AjouteStats(stats* s);
    void AfficheLesStats();
    void readTextFromFile(DecompteRegle* dr,DecompteRegle* drnext,int idr,char* buffer_read);
    void FormatRuleText(ostream& fichier,int index,char affiche_nom);
    void GenereOptimise(char f) {
        fichieroptimise=f;
    }
    Variable* CreeVariable(char* tampon,typeVariable ty,sorteVariable sorte,char index,char ajout,char proc);
    void affiche_derniere_erreur(int numero);
    char TesterUneLangue();
    void LitMultimots(istream& fichier,char);
	void LitTokenSelection(istream& fichier);
    void ChargementSemantique(istream& fichier,char* nom);
    void ConstruitLesFilsFictifs(Liste* L,istream& fichier,char* tampon,char& k,VECTA<char>& vari,char test);
	ListeVoc* fournirListeVoc(vocabulaire& A,char majuscule=NON);
	ListeVoc* fournirListeVocVide();
	void nettoyerListeVoc();
	void RenumeroteCouche(int idrule,int sens);
	void activationpere(rulespace* r);
	void activation(rulespace* r,char);
	
	char NumeroOrdre();
	graphe* litgraphe(istream& fichier);
	graphe* allouegraphe();
	void retiregraphe(graphe*);
	void initIteration() {
		compteur_iteration=0;
	}
	char testIteration() {
		if (compteur_iteration>= max_iteration)
			return 0;
		return 1;
	}
	void incIteration() {
		compteur_iteration++;
	}
	void initialise_erreur_instruction();
	virtual void ParseFinal();
	int TestLangue(string& laphrase);
	virtual char ajoutemot(string& nom,string& surface, string& lem, string& traits);
	virtual void razpile() {}
	void CategorySplitProcessing(char* start);
	void HMMInsertionProcessing();
	void DupliqueGrmFiles(char* nom);
	void filtreCategorie(ArcArbre* ar,Liste*);
	char ConsOrdreNodes(istream&);
	avl_Regle* recherchesurclef(TVecteur v,int);
	void ajouteregle(int lacouche,avl_Regle* avl);
	void destructiondependance(ResultatFonction* dependanceChgt);
	void DestructionNodes(VECTA<ResultatFonction*>&);
	char TestSimpleOrdreNoeuds(int couche,VECTA<genereGraphe*>&);
	char VerifieOrdre(VECTA<OrdreNoeuds*>& liste,VECTA<genereGraphe*>& noeuds);
	char comparecat(Cat* X,Cat* Y);
	int lirearguments(istream& fichier,char& k, char* tampon,VECTA<int>& args,VECTA<Traits*>& argtr);
	char TestArgNodeGauche(OrdreNoeuds* ord,ResultatFonction* r,VECTA<Noeud*>& args);
	char TestArgNodeDroit(OrdreNoeuds* ord,ResultatFonction* r,VECTA<Noeud*>& args);
	void RenvoieMaj(string& s);
	virtual void DeclencheAjusteoffset(bool v) {}
	char proba_metpoids(int id,float);
	char proba_metseuil(int id,float);
	char proba_metvaleur(int id,float);
	float proba_poids(int id);
	float proba_seuil(int id);	
	float proba_valeur(int id);
	void sauvegardeprobabilites(string filename);
	char chargeprobabilites(string filename);
	char chargementprobabilites(istream& fichier);

	void parcoursxnode(TestFonctionDependance* tf,x_node* x,VECTA<char>& variables_utilisees);
	void recuperenom(int&,x_node* x,TestFonctionDependance* ntf,VECTA<char>& variables_utilisees);
	TestFonction* construitarbregenere(TestFonction* tinit,x_node*,VECTA<char>& variables_utilisees,char);
	char CreationDependance(ContexteAnalyse& ct,PileArcNoeud& varias);
	int PreTestCategorie(EtatArbre* et,Noeud* n);
	ResultatFonction* CreeNouvelleDependance(Categorie* fonc,Traits*,VECTA<Noeud*>& vect);
	Traits* TraduitChaineTraits(string& traits);
	void MAJNumeroRegles();
	void ChargeParametres(istream& fichier,char* nom,char type_chaine);
	void LireParametresEnChaine(string& texte,char ajoute);
	void LitKif(istream& fichier);
	Noeud* ProduitListe(ListeVoc* alter);
	int OccupationMemoire();
	virtual void Ajusteoffset() {}
	void InitialiseVariablesGlobalesDansKif();
	string& PhraseCourante();
	bool AnalyseFromKif(KifElement*,KifElement*);
};


class VecteurDeParseurs  {
public:
	GlobalParseur* base;
	int dernier;
	VECTA<GlobalParseur*> parseurs;
	VecteurDeParseurs() {
		base=NULL;
		dernier=0;
	}
     
     inline GlobalParseur* operator [](int i) {
		 if (i==-1)
			 return base;
		 return parseurs[i];
     } 

	 inline int ajoute(GlobalParseur* val,int inc=10) {
		 parseurs.ajoute(val,inc);
		 dernier=parseurs.dernier;
		 return dernier-1;
	 }

	 inline GlobalParseur* affecte(int pos,GlobalParseur* val) {
		 if (pos==-1) {
			 base=val;
			 return base;
		 }
		 else
			 return parseurs.affecte(pos,val);		  
	 }

     inline char detruit(int i) {   
		 if (i==-1) {
			 if (base!=NULL)
				delete base;
			 else
				 return 0;
			 base=NULL;
		 }
		 else {
			 if (parseurs.detruit(i)==0)
				 return 0;
			 dernier=parseurs.dernier;
		 }
		return 1;
	 }
};

//-------------------------------------------------------
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

class Fichiergrm {
public:
	char grmname[1024];
	char  ntm[1024];
	char  trans[1024];
	char  hmm[1024];
	char  grammar[1024];
	char genere[1024];
	char tokenize[1024];
	char  lookup[1024];
	
	char model[1024];
	char brown[1024];
	char mkcls[1024];

	lookup_strategy strategy;
	char lookup_flags[1024];
	char conversion;
	VECTA<char*> parameters;
	vector<string> g_definition_var;
	vector<string> blockingrule;
	vector<string> datafiles;

	int lacoucheinitiale;

	Fichiergrm() {
		lacoucheinitiale=0;
		grmname[0]=0;
		ntm[0]=0;
		hmm[0]=0;
		grammar[0]=0;
		genere[0]=0;
		tokenize[0]=0;
		trans[0]=0;
		lookup[0]=0;
		conversion=NON;
		model[0]=0;
		brown[0]=0;
		mkcls[0]=0;
		lookup_strategy strategy;
		lookup_flags[0]=0;
		strategy=NO_STRATEGY;
	}

	void RAZ() {
		lacoucheinitiale=0;
		trans[0]=0;
		hmm[0]=0;
		genere[0]=0;
		ntm[0]=0;
		tokenize[0]=0;
		model[0]=0;
		brown[0]=0;
		mkcls[0]=0;
		lookup[0]=0;
		lookup_flags[0]=0;
	}
};

#endif
