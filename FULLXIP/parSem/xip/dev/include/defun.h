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
   Date       : 10/01/1999
   Purpose    : Basic definitions. Key encryption checking function
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifndef defun_h
#define defun_h

#define TVecteur unsigned long long
#define STVecteur long long

#ifdef WIN32
#define SEP '\\'
#define DEB ""
#define ANTISEP '/'
#else
#define SEP '/'
#define DEB "."
#define ANTISEP '\\'
#endif

#define NbCats 64
#define FUSION 64
#define gBitVect 64
#define gBitVectDivise 6 // = 64=2^6

const TVecteur Un=1;
const TVecteur MoinsUn = (TVecteur)-1;
#define SEPARATEUR (char*)"_"
#define ANYSYMBOL (char*)"?"

#define LectureMin(c) minlettre(LectCrypte(c))

#define NON 0
#define OUI 1
#define YES 1
#define NO 0
#define NONECRIT 2
#define ECRIT 3
#define UTILISE 2
#define SOUSDEPENDANCE 2
#define UTILISECONTEXTE 3
#define RCT 0
#define PostRCT 1
#define STD 2
#define PostSTD 3
#define TailleChaine 512
#define HACHE 512
#define BLOQUEE 3
#define MODIFIEE 4
#define NOUVEAU 1000
#define DUPLIQUE 10000
#define AJOUTE 0
#define SAUTE -1
#define GAUCHE 0
#define DROIT 1
#define STACKNONVIDE 2
#define CREEFORMULE 120
#define OP_OU -124
#define OP_ET -38
#define REMPLACEMENT 2
#define CATFUSION -255
#define ANALYSELINEAIRE 2
#define NOEUDSAUTE 2
#define TOUS 125
#define OUTPUTLIBXML 2
#define OUTPUTCOUTLIBXML 3
#define OUTPUTGENEREXML 4
#define XIPPROC 2
#define ALLERA 1
#define SORTIEANALYSE 2
#define MULTILEMME 0
#define MULTISURFACE 1
#define SURDETERMINE 95

//Les differents affichages
const TVecteur AFFICHAGE_LEMME = Un<<0;
const TVecteur AFFICHAGE_SURFACE = Un<<1;
const TVecteur AFFICHAGE_BALISE = Un<<2;
const TVecteur AFFICHAGE_ENTREE = Un<<3;
const TVecteur AFFICHAGE_CATEGORIE = Un<<4;
const TVecteur AFFICHAGE_REDUIT = Un<<5;
const TVecteur AFFICHAGE_COMPLET = Un<<6;
const TVecteur AFFICHAGE_OFFSET = Un<<7;
const TVecteur AFFICHAGE_NUM_MOT = Un<<8;
const TVecteur AFFICHAGE_PHRASE = Un<<9;
const TVecteur AFFICHAGE_STANDARD = Un<<10;
const TVecteur EXECUTE_NUL = Un<<11;
const TVecteur EXECUTE_CHUNK = Un<<12;
const TVecteur EXECUTE_DEPENDENCY = Un<<13;
const TVecteur EXECUTION =EXECUTE_NUL|EXECUTE_CHUNK|EXECUTE_DEPENDENCY;
const TVecteur AFFICHAGE_NUL = Un<<14;
const TVecteur AFFICHAGE_DEP_NOM = Un<<15;
const TVecteur AFFICHAGE_DEP_NOEUD = Un<<16;
const TVecteur AFFICHAGE_DEP_CREATION = Un<<17;
const TVecteur AFFICHAGE_ORDRE_DEPENDANCE =AFFICHAGE_DEP_NOM|AFFICHAGE_DEP_NOEUD|AFFICHAGE_DEP_CREATION;
const TVecteur ARBRE_AFFICHE = Un<<18;
const TVecteur ARBRE_SYNTAGME = Un<<19;
const TVecteur ARBRE_COLONNE = Un<<20;
const TVecteur AFFICHAGE_ARBRE =ARBRE_AFFICHE|ARBRE_SYNTAGME|ARBRE_COLONNE;
const TVecteur AFFICHAGE_FICHIER_XML_ECRAN = Un<<21;
const TVecteur AFFICHAGE_UTF8 = Un<<22;
const TVecteur AFFICHAGE_ERREUR_EXECUTION = Un<<23;
const TVecteur AFFICHAGE_MATHEMATIQUE = Un<<24;
const TVecteur AFFICHAGE_NUMERO_DEPENDANCE = Un<<25;
const TVecteur ENTREE_UTF8 = Un<<26;
const TVecteur EXECUTE_TOKENIZE = Un<<27;
const TVecteur AFFICHAGE_NUMPHRASE = Un<<28;
const TVecteur TESTER_LANGUE = Un<<29;
const TVecteur LIBRE_DE_DROIT = Un<<30;
const TVecteur AFFICHAGE_ARBRE_CHUNK = Un<<31;
const TVecteur AFFICHAGE_TRAIT_VALEUR_DEPENDANCE = Un<<32;
const TVecteur NON_NORMALISATION_TAG = Un<<33;
const TVecteur ENTREE_EN_MINUSCULE = Un<<34;
const TVecteur TEST_INPUT_UTF8 = Un<<35;
const TVecteur AFFICHAGE_CATEGORIE_GENERATION = Un<<36;
const TVecteur AFFICHAGE_GENERATION = Un<<37;
const TVecteur ANALYSE_ALEATOIRE = Un<<38;

/** Start MCR added code for passage XML output **/
const TVecteur AFFICHAGE_PASSAGE_XML_FORMAT = Un<<39;
/** End MCR added code for passage XML output **/

const TVecteur AFFICHAGE_JSON = Un << 40;
const TVecteur AFFICHAGE_JSON_API = Un << 41;
const TVecteur AFFICHAGE_CALCUL_DISTANCE = Un << 42;
const TVecteur AFFICHAGE_CAT_FERMANTE = Un << 43;
const TVecteur AFFICHAGE_TREEBANK = Un << 44;

/*
const TVecteur AFFICHAGE_LEMME=1;
const TVecteur AFFICHAGE_SURFACE=2;
const TVecteur AFFICHAGE_BALISE=4;
const TVecteur AFFICHAGE_ENTREE=8;
const TVecteur AFFICHAGE_CATEGORIE=16;
const TVecteur AFFICHAGE_REDUIT=32;
const TVecteur AFFICHAGE_COMPLET=64;
const TVecteur AFFICHAGE_OFFSET=128;
const TVecteur AFFICHAGE_NUM_MOT=256;
const TVecteur AFFICHAGE_PHRASE=512;
const TVecteur AFFICHAGE_STANDARD=1024;
const TVecteur EXECUTE_NUL=2048;
const TVecteur EXECUTE_CHUNK=4096;
const TVecteur EXECUTE_DEPENDENCY=8192;
const TVecteur EXECUTION=14336;
const TVecteur AFFICHAGE_NUL=16384;
const TVecteur AFFICHAGE_DEP_NOM=32768;
const TVecteur AFFICHAGE_DEP_NOEUD=65536;
const TVecteur AFFICHAGE_DEP_CREATION=131072;
const TVecteur AFFICHAGE_ORDRE_DEPENDANCE=229376;
const TVecteur ARBRE_AFFICHE=262144;
const TVecteur ARBRE_SYNTAGME=524288;
const TVecteur ARBRE_COLONNE=1048576;
const TVecteur AFFICHAGE_ARBRE=1835008;
const TVecteur AFFICHAGE_FICHIER_XML_ECRAN=2097152;
const TVecteur AFFICHAGE_UTF8=4194304;
const TVecteur AFFICHAGE_ERREUR_EXECUTION=8388608;
const TVecteur AFFICHAGE_MATHEMATIQUE=16777216;
const TVecteur AFFICHAGE_NUMERO_DEPENDANCE=33554432;
const TVecteur ENTREE_UTF8=67108864;
const TVecteur EXECUTE_TOKENIZE=134217728;
const TVecteur AFFICHAGE_NUMPHRASE=268435456;
const TVecteur TESTER_LANGUE=536870912;
const TVecteur LIBRE_DE_DROIT=1073741824;
#ifdef WIN32
const TVecteur AFFICHAGE_ARBRE_CHUNK=2147483648;
const TVecteur AFFICHAGE_TRAIT_VALEUR_DEPENDANCE=4294967296;
const TVecteur NON_NORMALISATION_TAG=8589934592;
const TVecteur ENTREE_EN_MINUSCULE=17179869184;
const TVecteur TEST_INPUT_UTF8=34359738368;
const TVecteur AFFICHAGE_CATEGORIE_GENERATION=68719476736;
const TVecteur AFFICHAGE_GENERATION=137438953472;
const TVecteur ANALYSE_ALEATOIRE=274877906944;
#else
const TVecteur AFFICHAGE_ARBRE_CHUNK=2147483648LL;
const TVecteur AFFICHAGE_TRAIT_VALEUR_DEPENDANCE=4294967296LL;
const TVecteur NON_NORMALISATION_TAG=8589934592LL;
const TVecteur ENTREE_EN_MINUSCULE=17179869184LL;
const TVecteur TEST_INPUT_UTF8=34359738368LL;
const TVecteur AFFICHAGE_CATEGORIE_GENERATION=68719476736LL;
const TVecteur AFFICHAGE_GENERATION=137438953472LL;
const TVecteur ANALYSE_ALEATOIRE=274877906944LL;
#endif
**/

#define AFFICHE_FONCTION 1
//#define AFFICHE_NUMERO 2
#define AFFICHE_NUM_PHRASE 4
#define AFFICHE_INVERSE 8
#define AFFICHE_TRAITS 16
#define AFFICHE_INCOMPATIBLE 32

#ifdef _DEBUG
#define statique
#else
#define statique static
#endif

#ifdef XIPPYTHON
#define staticbase
#else
#define staticbase static
#endif

extern int index_parseur_courant;

//#define RAMASSE(x) i_ramassage=x::ramassage[index_parseur_courant].dernier;x::ramassage[index_parseur_courant].ajoute(this)
//#define JETTE(x)  x::ramassage[index_parseur_courant].annule(i_ramassage)
//#define RAMASSAGE(x) static VECTA<x*> ramassage[10]; static int ALaRamasse() {for (int i=0;i<x::ramassage[index_parseur_courant].dernier;i++) {if (x::ramassage[index_parseur_courant][i]!=NULL) return i;} return -1;} static void VideRamasse() {for (int i=0;i<x::ramassage[index_parseur_courant].dernier;i++) {if (x::ramassage[index_parseur_courant][i]!=NULL) delete x::ramassage[index_parseur_courant][i];}} int i_ramassage
//#define VIDERAMASSE(x) x::VideRamasse();x::ramassage[index_parseur_courant].dernier=0
//#define CREERAMASSE(x) VECTA<x*> x::ramassage[10]

#define RAMASSE(x) \
	_precedent=x::r_dernier[index_parseur_courant];\
	_suivant=NULL;\
	if (x::r_racine[index_parseur_courant]==NULL)\
		x::r_racine.affecte(index_parseur_courant,this);\
	else\
		x::r_dernier[index_parseur_courant]->_suivant=this;\
	x::r_dernier.affecte(index_parseur_courant,this)

#define JETTE(x) \
	if (x::r_racine[index_parseur_courant]==this)\
		x::r_racine.affecte(index_parseur_courant,_suivant);\
	if (_precedent!=NULL)\
       _precedent->_suivant=_suivant;\
	if (_suivant!=NULL) \
	   _suivant->_precedent=_precedent;\
	if (x::r_dernier[index_parseur_courant]==this)\
	    x::r_dernier.affecte(index_parseur_courant,_precedent)

#define RAMASSAGE(x) \
	static VECTA<x*> r_racine;\
    static VECTA<x*> r_dernier;\
	x* _precedent;\
	x* _suivant;\
	static void VidageRamasse() {\
		x* _boucle=x::r_dernier[index_parseur_courant];\
		x* _sub;\
		while(_boucle!=NULL) {\
			_sub=_boucle->_precedent;\
			 delete _boucle;\
			_boucle=_sub;}}

#define VIDERAMASSE(x) x::VidageRamasse()
#define CREERAMASSE(x) VECTA<x*> x::r_racine;VECTA<x*> x::r_dernier


#define AJOUT push_back
#define AJOUTTRAITS AjouteTraitsSTL
#define COMPARECHAINE(x,y) x == y
#define CHAINEPTR string
#define DUPLICATION(x) x
#define VALEURCHAINE(x) (char*)x.c_str()


#define TestDebutFin(x)  ((x->valeur)->vecteur[0] & 12) //fin + debut
#define TestDebutPhrase(x) ((x->valeur)->vecteur[0] & 4) == 4
#define MetDebutPhrase(x) (x->valeur)->vecteur[0]|=4;(x->filtre)->vecteur[0]|=4;if (x->permanent==NULL) x->permanent=new Vecteur(x->filtre->TailleVect);(x->permanent)->vecteur[0]|=4
#define TestFinPhrase(x) ((x->valeur)->vecteur[0] & 8) == 8;
#define MetFinPhrase(x) (x->valeur)->vecteur[0]|=8;(x->filtre)->vecteur[0]|=8;if (x->permanent==NULL) x->permanent=new Vecteur(x->filtre->TailleVect);(x->permanent)->vecteur[0]|=8
#define MetDebutFin(x,y)  (x->valeur)->vecteur[0]|=y;(x->filtre)->vecteur[0]|=y

#define MetDebut(x) (x->valeur)->vecteur[0]|=1;(x->filtre)->vecteur[0]|=1
#define MetFin(x) (x->valeur)->vecteur[0]|=2;(x->filtre)->vecteur[0]|=2
#define RetireDebut(x) (x->valeur)->vecteur[0]&=(~1);(x->filtre)->vecteur[0]&=(~1)
#define RetireFin(x) (x->valeur)->vecteur[0]&=(~2);(x->filtre)->vecteur[0]&=(~2)
#define TestFirst(x) ((x->valeur)->vecteur[0] & 1) == 1
#define TestLast(x) (((x->valeur)->vecteur[0] & 2) == 2)

typedef enum {INSTRUCTION_NORMALE,
              INSTRUCTION_DETERMINISTE,
              INSTRUCTION_DEPENDANCE,
              
              INSTRUCTION_PROCEDURE,
              INSTRUCTION_IFTEST,
              INSTRUCTION_CONTEXTE,
              INSTRUCTION_FOR,                            
              INSTRUCTION_ELSE,          
              INSTRUCTION_WHILE,
              INSTRUCTION_WHILEXML,
              
              INSTRUCTION_FINWHILE,              
              INSTRUCTION_RETOUR,
              INSTRUCTION_FINFOR,
              INSTRUCTION_FINIFTEST
} typeInstruction;


typedef enum {C_CATEGORIE, 
              C_FONCTION, 
              C_PROCEDURE,               
              C_PROCEDUREDECLAREE,
              C_PROCEDUREDISPLAY,
              C_PROCEDUREPYTHON,
			  C_PROCEDUREGENERATION,
			  C_GENERATION,
			  C_KIF} typeCat;

typedef enum {ECRASE,COMPARE,NEG,VARECRASE,VARCOMPARE,VARNEG,AFFECTENOEUD,COMPARENOEUD} typeManip;

typedef enum {PERSONNEL,FRANCAIS,ANGLAIS} typeLangue;

typedef enum {
        COMPATIBLES_LECTURE,
        EGAUX_LECTURE,
        COMPATIBLES,
        EGAUX,
        EGALITE,
        INFERIEUR, 
        SUPERIEUR,
        INF_TRAITS,INF_EGAL_TRAITS, 
        SUP_TRAITS, SUP_EGAL_TRAITS,
        AFFECTATION,
        AFFECTATION_LECTURE,
        TEST,
        EGAUX_ONTOLOGIE,
        COMPATIBLES_ONTOLOGIE,
        AFFECTATION_ONTOLOGIE,
		CREATION_NOEUD} typeComparateur;

typedef enum {VIDE,
              REGLES_SEQUENCE,
              REGLES_DI,
              REGLES_SIMPLIFICATION,
              REGLES_EXTRACTION,
              REGLES_DEDUCTION, 
              REGLES_SEMANTIQUE,
              REGLES_MARQUAGE,
              REGLES_TERME,
              REGLES_RECONSTRUCTION,
              REGLES_RECONSTRUCTION_MODELE,
              REGLES_DECOUPESEQUENCE,
              REGLES_CARTESIEN,
              REGLES_BOUCLE,
              REGLES_FRAGMENTE,
              REGLES_VOCABULAIRE,
              REGLES_PL,
              REGLES_MULTIMOTS,
			  REGLES_MULTILEMMES,
			  REGLES_TOKENSELECTION,
			  REGLES_LEXICALES,
			  REGLES_GENERATION,
			  REGLES_ORDRE,
			  REGLES_SEQUENCE_CONTEXTE} typeRegle;

typedef enum {AUCUN,REEL, ENTIER, CHAINE,KIFOBJET,PYTHONOBJET,GRAPHE,XMLOBJET} typeVariable;

typedef enum {
    VAUCUN,
    VGLOBALEREINIT,
    VNOEUD,
    VDEPENDANCE,
    VGLOBALE,
    VLEMMEINDEX,
    VALREEL,
    VCHAINE,
    VALCHAINE,
    VVECTEURCHAINE,
    VOPERATEUR,
    VALNOEUD,
    VVECTEURINT,
    VAUTOMATE,
    VDICTIONNAIRE,
    VPYTHONOBJET,
	VGRAPHE,
	VKIFOBJET,
	VGRAPHECREATION,
	VVECTEURGRAPHE,
	VVECTEURGRAPHECREATION,
	ACTIONGRAPHEVIDE,
	VXMLOBJET,
	VCHAINENOEUD,
	VLEMMECHAINEINDEX,
	VCHAINEDEPENDANCE} sorteVariable;


typedef enum {VALEUR,ATTRIBUT,LISTE,DUPLICAT,VALEURBASE,PREMIERDUPLICAT,LISTEDUPLICAT} typeTrait;

typedef enum {AJOUTER,
              ECRASER,
              ECRASERTOUT,
              NOUVEL,
              UNEFOISNOUVEAU,
              BALISEDEBUT,
              BALISEFIN,
              BALISECOMMUTATEUR,
              BALISECOMMUTATEURFERMEE,
              BALISEDEBUTSF,
              BALISEFINSF,
              BALISECOMMUTATEURSF,
              BALISECOMMUTATEURFERMEESF} typeVoc;

typedef enum {PARSEURGLOBAL,
              PARSEURHMM,
              PARSEURNTM,
              PARSEURXML,
              PARSEURTOKENLOOK,
			  PARSEURTRANS} typeParseur;

typedef enum {
    DEDUCTIONFINALE,DEDUCTIONCOURANTE,DEDUCTIONCOUCHE,
    DEDUCTIONFILTRE,DEDUCTIONPROCEDURE,DEDUCTIONSCRIPT,
    DEDUCTIONINITIALE,DEDUCTIONENCHARGEANT,
	DEDUCTIONPREBALISE,DEDUCTIONPOSTBALISE,DEDUCTIONGENERATION,DEDUCTIONQUEUE} typeDeduction;


typedef enum {         
        ONTOLOGIE_COMPARAISON,
        ONTOLOGIE_COMPARAISON_STRICTE,
        ONTOLOGIE_AFFECTATION,
        ONTOLOGIE_DESTRUCTION,
        ONTOLOGIE_NEGATION,
        ONTOLOGIE_NEGATION_STRICTE,
		ONTOLOGIE_LIBRE,
		ONTOLOGIE_BLOQUEE
} actionOntologie;

//Defintion specifique au traitement avec libxml
#ifdef XIPLIBXML

typedef enum {XMLELEMENT,XMLCHAINE,XMLCLEF,XMLINDEX,XMLMETHODE,XMLMATH,XMLVARI} XMLTYPE;

typedef enum {XMLVARINONE,
    XMLNODECOURANT,
    XMLNODESUIVANT,
    XMLNODEPRECEDENT,
    XMLNODEPARENT,
    XMLNODEFILS} 
VARIABLEXML;

typedef enum {XMLTOKEN,XMLCHUNK,XMLDEPENDENCY, XMLSKIPTEXT, XMLSKIPALL} XMLACTION;


extern char* langues_reconnaissables[];

typedef enum {SANSDESCRIPTION,LIREAVECDTD,LIREAVECPARAM} XMLDESCRIPTION;

typedef enum {XMLSAUTE=1, XMLACCUMULE=2, XMLANALYSE=4, XMLTRAIT=8, XMLDEJATOKEN=16, XMLGETATTRIBUTE=32 } XMLEVENEMENT;

#endif

extern ofstream* Trace;

#define sortie *Sortie
#define trace *Trace
#define Fregle *FRegle

#define libere(x) delete[] x
#include "xipvecta.h"

inline int CalculClefHachage(unsigned char* mot) {
    int indexMot = mot[0]*10 + mot[1];
    indexMot%=HACHE;
    return indexMot;
}


inline size_t CalculClefHachageTout(unsigned char* mot) {
    size_t indexMot=0;
    int longueur=1;
    while (*mot!=0) {
        indexMot += longueur*(*mot);
        longueur++;
        mot++;
    }
    indexMot%=HACHE;
    return indexMot;
}


#endif
