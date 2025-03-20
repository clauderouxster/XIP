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
filename   : avlfonc.cxx
Date       : 10/01/1999
Purpose    : Implementation of "if" and reshuffling rules.
Programmer : Claude ROUX
Reviewer   : 
*/

#include "parsbin.h"
#include "varlibxml.h"
#include "generation.h"
int appel_construit_index=0;
int erreur_sur_dependance=0;
int erreur_inference_noeud=0;
int echec_a_1=0;
int reussite_inference=0;
int appel_calcul_inference=0;
int appel_inference=0;
extern const char CALCULNEGATION=100;
extern GlobalParseur* parseur_courant;
char XipGetVariableValue(int ipar,string variable,string& val);

#define RETOURFORCEXML 4
#define ERREURNOEUD 3
#define PASERREURNOEUD 2
#define ERREUREXECUTION 1
#define PASERREUR 0
#define PURLEXICAL -99

extern int teste_differentes_modifications[100];

extern "C" {
	Exporting bool xipExecutionEnd();
}

#define TOUTECATEGORIE 2
char majlemmesurface(GlobalParseur* parseur,Traits* RegTraits,Noeud* noeud,char type);
int PreTestEtatCategorie(GlobalParseur* parseur,EtatArbre* et,Noeud* n,char& calcul);
void strcatchr(char* tmp,char c);
void affecteontologielibre(Traits* sommet,Traits* traits);
char parentde(ResultatFonction* fille,ResultatFonction* parent);



int TestPositionElement(TVecteur* v,int p) {
	int ip=p;
	if ((v[0]& 256)==256)
		ip=0;
	else
		if ((v[0]& 512)==512)
			ip=1;
		else
			if ((v[0]& 1024)==1024)
				ip=p-1;
	v[0]&=~1792;
	if (ip<0 || ip>p)
		ip=p;

	return ip;
}

CREERAMASSE(TestArbre);
CREERAMASSE(Dependance);
CREERAMASSE(SeqFonc);
CREERAMASSE(TokenSelection);
CREERAMASSE(Simplifie);
CREERAMASSE(Cartesien);
CREERAMASSE(VectTestFonction);
CREERAMASSE(ReconstruitArbre);
CREERAMASSE(ListeTestArbre);

char TestFoncDep(TestFonction* tf) {
	if (tf->Type()==TESTFONCTIONDEPENDANCE || tf->Type()==TESTRACINEGENERATION || tf->Type()==TESTNOEUDGENERATION)
		return OUI;
	return NON;
}

//--------------------------------------------
char TestClef(GlobalParseur* parseur,TVecteur clef) {

    if ( (clef & parseur->ClefDependances) != clef) 
        return NON;

    return OUI;
}

//--------------------------------------------
//Signature

//Le calcul est le suivant: 
//   rang = reste de i/64 (il y a 64 bits par entier)
//   valeur = 64*rang
#define calculbit(num,rang,val)    rang= num >> gBitVectDivise; val = Un << (num - (rang << gBitVectDivise)); 

inline void NettoieTraits(VECTA<Traits*>& vecttr,int bp) {
    for (int idt=bp;idt<vecttr.dernier;idt++)
        vecttr.detruit(idt);
    vecttr.dernier=bp;
}

inline void NettoieAffectation(VECTA<Traits*>& affectation,int bp) {
    for (int idt=bp;idt<affectation.dernier;idt+=3)
        affectation.detruit(idt+2);
    affectation.dernier=bp;
}


//Nous maintenons un filtre global de valeur en permanence...
void GlobalParseur::FiltreDependance(int nb) {
    int rang;
    TVecteur val;
    int taille=CompteurIncompatible+nb;
    for (int i=CompteurIncompatible;i<taille;i++) {
        calculbit(i,rang,val);
        //S'il y a un changement de rang
        if (filtreGlobalDependance.dernier==rang)
            //On rajoute une valeur de filtreGlobalDependance
            filtreGlobalDependance.ajoute(val);
        else {
            //sinon on fusionne les valeurs en local
            val |= filtreGlobalDependance.fin();
            filtreGlobalDependance.affecte(filtreGlobalDependance.dernier-1,val);
        }   
    }   
    //Nous mettons alors a jour le CompteurIncompatible pour d'autres futures analyses.
    CompteurIncompatible+=nb;
}


Signature::Signature(int s,int f) {
    signature.ajoute(s);
    filtre.ajoute(f);    
}

Signature::Signature(Signature& s) {
    signature.affecte(s.signature);
    filtre.affecte(s.filtre);    
}

void Signature::raz() {
    signature.raz();
    filtre.raz();
    signature.ajoute(1);
    filtre.ajoute(2);
    
}

//Construction du filtre, il depend du nombre de valeurs qui correspondent aux dependances incompatibles
// depart est la valeur initiale pour le premier bit d'incompatibilite
// num est le bit courant
// nbincompatible le nombre total de bits
void Signature::initialise(int depart, int num, int nbincompatible) {
    int rang;
    TVecteur val=0;
    int i;

    int taille=depart+nbincompatible;
    int saut= num+depart;

    for (i=depart;i<taille;i++) {
        calculbit(i,rang,val);
        //Nous construisons le bit correspondant a num, si la valeur de filtre n'existe pas
        //on la construit..
        if (i==saut) {            

            if (signature.dernier==rang)
                signature.ajoute(val);
            else 
                signature.affecte(signature.dernier-1,val);

            if (filtre.dernier<signature.dernier)
                filtre.ajoute(0);
        }
        else {
            //S'il y a un changement de rang
            if (filtre.dernier==rang) {
                //On rajoute une valeur de filtre
                filtre.ajoute(val);
                //Et aussi une valeur de signature
                signature.ajoute(0);
            }
            else {
                val |= filtre.fin();
                filtre.affecte(filtre.dernier-1,val);
            }
        }
    }
}

//On retire cette signature de l'objet courant...
void Signature::retire(Signature& s) {
    int i;
    TVecteur v;
    for (i=0;i<signature.dernier && i<s.signature.dernier;i++) {
        //Evidemment on ne rajoute que la partie qui different au sein de la signature
        v= filtre[i] | (signature[i] ^ s.signature[i]);
        //Il suffit d'ajouter la signature dans le filtre
        filtre.affecte(i,v);
    }
    //Si la liste de s est plus longue que celle courante
    for (i=signature.dernier;i<s.signature.dernier;i++) {
        //Dans ce cas, il n'a pas de signature
        signature.ajoute(0);
        //et l'autre signature devient le filtre
        filtre.ajoute(s.signature[i]);
    }    
}

void Signature::remplace(Signature& s) {
    signature.affecte(s.signature);
    filtre.affecte(s.filtre);
}

//Nous fusionnons les signatures et les filtres
void Signature::fusion(Signature& s) {
    int i;    

    for (i=0;i<signature.dernier && i<s.signature.dernier;i++) {
        signature.affecte(i,signature[i] | s.signature[i]);
        filtre.affecte(i,filtre[i] | s.filtre[i]);
    }

    //Si la liste de s est plus longue que celle courante
    for (i=signature.dernier;i<s.signature.dernier;i++) {
        signature.ajoute(s.signature[i]);
        filtre.ajoute(s.filtre[i]);
    }
}

void Signature::base(Signature& s) {
    int i;    

    for (i=0;i<signature.dernier && i<s.signature.dernier;i++)
        filtre.affecte(i,filtre[i] | s.filtre[i]);

    //Si la liste de s est plus longue que celle courante
    for (i=signature.dernier;i<s.signature.dernier;i++)
        filtre.ajoute(s.filtre[i]);

}

void Signature::interdit(Signature& s) {
    int i;    
    TVecteur v;
    for (i=0;i<signature.dernier && i<s.signature.dernier;i++) {
        v= signature[i] ^ s.signature[i];
        signature.affecte(i,v);
        filtre.affecte(i,filtre[i] | s.signature[i]);
    }

    //Si la liste de s est plus longue que celle courante
    for (i=signature.dernier;i<s.signature.dernier;i++) {        
        filtre.ajoute(s.signature[i]);
        signature.ajoute(0);
    }
}


//Comparaison entre les signatures et les filtres
//Les filtres contiennent les valeurs qui doivent etre a zero dans les signatures.
//Plus exactement, les filtres contiennent les bits correspondant aux dependances 
//incompatibles entre elles.
char Signature::compare(Signature& s) {
    TVecteur f;
    for (int i=0;i<signature.dernier;i++) {
        f= filtre[i] | s.filtre[i];
        //Si l'on trouve un bit dans les signatures provenant des filtres, il y a erreur
        if ( (f & signature[i])!=0)
            return NON;
        if ( (f & s.signature[i])!=0)
            return NON;        
    }
    return OUI;
}

char Signature::compare(ResultatFonction* f) {
    return compare(f->compatible);
}

//Nous comparons deux signatures, pour verifier si l'element doit etre totalement detruit
//ou totalement modifie. Si la methode "global" renvoie OUI, il s'agit d'un objet unique
//et les methodes doivent s'appliquer totalement...
char Signature::global(VECTA<TVecteur>& filtreglobal,Signature& s) {
    //Une signature est globale si l'on a egalite entre la signature de la
    //dependance (l'objet local this ici) et la signature gloable de l'analyse
    
    if (s.signature.dernier != signature.dernier)
        return NON;
    
    for (int i=0; i< signature.dernier;i++) {
        if (signature[i]!=s.signature[i])
            return NON;
    }
    
    return OUI;        
}

void Signature::famille(int borne,VECTA<int>& arbre) {            
    register int i;
    for (i=filtre.dernier-1;i>=0;i--) {   
        STVecteur val=(STVecteur)(filtre[i]);
        if (val!=0) {
            val=~val;
            for (register int j=gBitVect;j>=0;j--) {
                if (j>=borne) {
                    val<<=1;
                    continue;
                }

                if (val==0)
                    break;
                
                if (val < 0) {
                    int pos= i*gBitVect + j;
                    int k;
                    for (k=0;k<arbre.dernier;k++) {
                        if (arbre[k]==pos) {
                            pos=-1;
                            break;
                        }
                        if (arbre[k] > pos)
                            break;
                    }
                    if (pos!=-1)                        
                        arbre.insertion(k,pos);                   
                }
                
                val<<=1;
            }
        }
    }   
}


//----------------------------------------------------------
char* TC(GlobalParseur* parseur,Traits* tr) {
	static char chaine[1000];
    tr->TraitsEnChaine(parseur,chaine);
    return chaine;
}

char* TV(GlobalParseur* parseur,Vecteur* v) {
	static char chaine[1000];
    v->ChaineAttributs(parseur,chaine);
    return chaine;
}

char* TVV(GlobalParseur* parseur, STVecteur val, int i) {
    char debut=1;
    if (val!=0) {
        for (int j=gBitVect-1;j>=0;j--) {
            if (val==0)
                break;
            if (val < 0) {
                int pos= (i*gBitVect + j)*2;       
                
                if (!strstr(parseur->Chaine_traits,parseur->TableDesTraits[pos])) {
                    if (debut) debut=0;else strcat_s(parseur->Chaine_traits,3000,",");		  
                    strcat_s(parseur->Chaine_traits,3000,parseur->TableDesTraits[pos]);
                }                
            }
            val<<=1;
        }
    }        
    return parseur->Chaine_traits;
}

//-------------------------------------------------------------------------

inline char egalArg(ResultatFonction* f1,ResultatFonction* f2) {
    
    if (f1==f2 || f1->nb_args!=f2->nb_args || f1->Fx!=f2->Fx)
        return NON;
    
    for (int i=0;i<f1->nb_args;i++)
        if (f1->arg[i]!=f2->arg[i])
            return NON;
        return OUI;
}


inline char TestArbre::TestCalculClef(GlobalParseur* parseur) {
    
    TVecteur clef=0;
    for (int i=0;i<arbre.dernier;i++) {
        if (arbre[i]->operateur!='@')
            return NON;
        
        if (TestFoncDep(arbre[i]->test)) {
            TestFonctionDependance* tfnoeud=(TestFonctionDependance*)arbre[i]->test;
            if ( tfnoeud->negation==NON)
                clef |= (tfnoeud->Fx)->Val;
        }
    }
    
    if ( (clef & parseur->ClefDependances) != clef) 
        return NON;
    
    return OUI;
}


void PileNoeuds::nettoie() {
    noeuds.raz();
    for (int i=0;i<pile.dernier;i++) {
        int k=i;
        while (pile[i]==pile[i+1])
            i++;
        delete pile[k];
    }
    pile.raz();
}

PileNoeuds::~PileNoeuds() {
    //Il peut y avoir des pointeurs communs.        
    for (int i=0;i<pile.dernier;i++) {
        int k=i;
        while (pile[i]==pile[i+1])
            i++;
        delete pile[k];
    }          
    pile.raz();
    noeuds.raz();
    nb=-1;
}

TokenSelection::~TokenSelection() {
	JETTE(TokenSelection);
}

ResultatSequence::ResultatSequence(int inc) {
    increment=inc;
    fin=-1;
    debut=-1;
    bon=OUI;
    traits=NULL;
}

ResultatSequence::ResultatSequence(GlobalParseur* parseur,Cat* Sx,Traits* tr) {
    X=Sx;
    Nombre=1;
    traits=new ChaineTraits(tr,parseur->XCatVide,NON);
    increment=1;
    
	fin=0;
    debut=0;
    clef=0;
    bon=OUI;
}

void ResultatSequence::initialise(GlobalParseur* parseur,Cat* Sx,Traits* tr) {
    X=Sx;
    increment=1;
    Nombre=1;
    if (traits!=NULL)
        delete traits;
    
    traits=new ChaineTraits(tr,parseur->XCatVide,NON);
    
    fin=0;
    debut=0;
    clef=0;
    bon=OUI;
}

ResultatSequence::ResultatSequence(ResultatSequence* r) {
    
    X=r->X;
    
    increment=r->increment;
    
    traits=new ChaineTraits(r->traits,Nombre);
    
    fin=0;
    debut=0;
    clef=r->clef;
    bon=OUI;
}

void ResultatSequence::Detruit() {
    delete this;
}

ResultatSequence::~ResultatSequence() {   
    if (traits!=NULL)
        delete traits;
}

ResultatFonction::ResultatFonction(GlobalParseur* p,Cat* X) : arg(3),variables(3), stack(1) {
    parseur=p;
	profondeur=0;
	numero_ordre=0;
    
	frere=NULL;
    Fx=X;
    data=NON;
    duree=0;
    nb_noeuds=0;
    traits=new Traits(parseur);
    bon=OUI;
    numeros.raz();
    nb_args=0;
    index=-1;
    indexpremier=-1;
    indexsecond=-1;
    indexcategorie=-1;
    premier=NULL;
    dernier=NULL;
}


ResultatFonction::ResultatFonction(ResultatFonction* r) : arg(r->arg.dernier), variables(r->variables.dernier) {
	profondeur=0;
	numero_ordre=0;
    Fx=r->Fx;
	frere=NULL;
    data=NON;
    duree=r->duree;	
    nb_noeuds=r->nb_noeuds;
    parseur=r->parseur;
    if (r->traits!=NULL)
        traits=new Traits(r->traits);
    else
        traits=new Traits(parseur);
    
        /*    
        for (i=0;i<r->arg.dernier;i++)
        arg.ajoute(r->arg[i]);
        
         for (i=0;i<r->variables.dernier;i++)
         variables.ajoute(r->variables[i]);
    */
    arg.ajoute(r->arg);
    variables.ajoute(r->variables);
    
    bon=OUI;
    index=-1;
    indexpremier=-1;
    indexsecond=-1;
    indexcategorie=-1;
    numeros.affecte(r->numeros);
    nb_args=r->nb_args;
    premier=r->premier;
    dernier=NULL;
    for (int i=0;i<r->stack.dernier;i++) {
        char* res=r->stack[i];
        if (res!=NULL)
            res=DupliqueChaine(res);
        stack.ajoute(res);
    }
}

ResultatFonction* ResultatFonction::Duplique() {
    
    if (duree==0)
        return NULL;
    
    ResultatFonction* r=new ResultatFonction(parseur,Fx);
	r->profondeur=profondeur;
	r->numero_ordre=numero_ordre;
    r->duree=duree-1;
    
    if (traits!=NULL)
        r->traits=new Traits(traits);
    else
        r->traits=new Traits(parseur);
    
    int i;
    for (i=0;i<arg.dernier;i++) {
        Liste* L= (parseur->TasDeListes)->fournir(parseur, 
            (arg[i]->source)->X,
            (arg[i]->source)->traitsbase);
        r->arg.ajoute(&L->N);
    }
    
    for (i=0;i<stack.dernier;i++) {
        char* res=stack[i];
        if (res!=NULL)
            res=DupliqueChaine(res);
        r->stack.ajoute(res);
    }
    return r;
}

void ResultatFonction::initialise(Cat* X) {
    Fx=X;
	frere=NULL;
	numero_ordre=0;
	profondeur=0;
    if (traits!=NULL)
        traits->raz();
    else
        traits=new Traits(parseur);
    compatible.raz();    
    bon=OUI;
    index=-1;
    numeros.raz();
    nb_args=0;
    nb_noeuds=0;
    premier=NULL;
    dernier=NULL;
    arg.raz();
    variables.raz();
    vardep.raz();
    for (int i=0;i<stack.dernier;i++) {
        char* res=stack[i];
        if (res!=NULL)
            libere(res);
    }
    stack.raz();
}

void ResultatFonction::affecte(ResultatFonction* r) {
    profondeur=0;
	numero_ordre=0;
    initialise(r->Fx);
    
    if (r->traits!=NULL)
        traits->ou(r->traits);
    
        /*
        for (i=0;i<r->arg.dernier;i++)
        arg.ajoute(r->arg[i]);
        
         for (i=0;i<r->variables.dernier;i++)
         variables.ajoute(r->variables[i]);
    */
    
    arg.ajoute(r->arg);
    variables.ajoute(r->variables);
    
    bon=OUI;
    numeros.affecte(r->numeros);
    nb_args=r->nb_args;
    nb_noeuds=r->nb_noeuds;
    premier=r->premier;
    dernier=NULL;
    for (int i=0;i<r->stack.dernier;i++) {
        char* res=r->stack[i];
        if (res!=NULL)
            res=DupliqueChaine(res);
        stack.ajoute(res);
    }

}


char ResultatFonction::Integre(int nb) {
    if (bon==NONECRIT || premier==NULL || dernier==NULL)
        return NON;
    
    for (int i=0;i<nb;i++)
        if (arg[i]==NULL)
            return NON;
        return OUI;
}

ResultatFonction::~ResultatFonction() {
    if (traits!=NULL)
        delete traits;
    for (int i=0;i<stack.dernier;i++) {
        char* res=stack[i];
        if (res!=NULL)
            libere(res);
    }
}

inline void ResultatFonction::RAZ() {
	profondeur=0;
	numero_ordre=0;
    arg.raz();
    for (int i=0;i<stack.dernier;i++) {
        char* res=stack[i];
        if (res!=NULL)
            libere(res);
    }
}   


#ifdef XIPLIBXML
/** Start MCR added code for passage XML output **/
void ResultatFonction::ConstruitXML(int& numFonction, char cache) {
	if (parseur->TestOptions(AFFICHAGE_PASSAGE_XML_FORMAT) == OUI) {
		ConstruitXMLPassage(numFonction, cache);
	} else {
		ConstruitXMLStandard(cache);
	}
}
void ResultatFonction::ConstruitXMLPassage(int& numFonction, char cache) {
    int i;
	char nomfonc[100];
	Vecteur* filtre=parseur->FILTREFONCTION;

	if (Fx==NULL || (cache==OUI && Fx->cacher==OUI)) {
        return;
	}

	if (Fx->type==C_GENERATION) {
		sprintf_s(nomfonc,100,"#%s",Fx->nom);
		filtre=parseur->FILTREGENERATION;
	} else {
		strcpy_s(nomfonc,100,Fx->nom);
	}
    xmlNodePtr node=parseur->CreationNoeudXML(parseur->XmlLunit, "R", NULL);
    parseur->AjouteAttributValeur(node, "type", nomfonc);
	char relId[10];
	sprintf_s(relId,10, "E%dR%d", parseur->num_phrase, numFonction++);
    parseur->AjouteAttributValeur(node, "id", relId);

	for (i=0;i<arg.dernier;i++) {
		xmlNodePtr parametre=parseur->CreationNoeudXML(node, "ARG", NULL);
		char argId[2];
		sprintf_s(argId,2, "%d", i);
		parseur->AjouteAttributValeur(parametre, "id", argId);
		parseur->AjouteAttributValeur(parametre, "ref", arg[i]->idPassage);
    }

}
void ResultatFonction::ConstruitXMLStandard(char cache) {
	char chc[10];
    int i,ist;
	char nomfonc[100];
	Vecteur* filtre=parseur->FILTREFONCTION;

    if (Fx==NULL || (cache==OUI && Fx->cacher==OUI))
        return;

	if (Fx->type==C_GENERATION) {
		sprintf_s(nomfonc,100,"#%s",Fx->nom);
		filtre=parseur->FILTREGENERATION;
	}
	else
		strcpy_s(nomfonc,100,Fx->nom);
    xmlNodePtr node=parseur->CreationNoeudXML(parseur->XmlLunit,"DEPENDENCY",NULL);
    parseur->AjouteAttributValeur(node,"name",nomfonc);
    if (traits!=NULL) {        
        traits->ConstruitXML(parseur,node,filtre);

        //Affichage des valeurs de variables
        if (parseur->mathematique)
            for (ist=0;ist<vardep.dernier;ist++)
                if (vardep[ist]!=NULL && vardep[ist]->utilisee==OUI)
                    vardep[ist]->CreationXMLDependance(parseur,node,this);
        
        for (ist=0;ist<stack.dernier;ist++) {
            strcpy_s(parseur->Chaine_traits,3000,stack[ist]);
            xmlNodePtr feature=parseur->CreationNoeudXML(node,"FEATURE",NULL);
            parseur->AjouteAttributValeur(feature,"attribute","$STACK");
            parseur->AjouteAttributValeur(feature,"value",parseur->Chaine_traits);
        }
    }
        
    for (i=0;i<arg.dernier;i++) {
	    string ch;
        xmlNodePtr parametre=parseur->CreationNoeudXML(node,"PARAMETER",NULL);
        sprintf_s(chc,10,"%d",i);
        parseur->AjouteAttributValeur(parametre,"ind",chc);
        sprintf_s(chc,10,"%d",arg[i]->num_noeud);
        parseur->AjouteAttributValeur(parametre,"num",chc);
		if (parseur->TestOptions(AFFICHAGE_LEMME))
			arg[i]->Lemme(ch);
		else
			arg[i]->Surface(ch);
        parseur->AjouteAttributValeur(parametre,"word",STR(ch));
    }
}
/** End MCR added code for passage XML output **/
#endif


//Affichage d'une fonction syntatxique...
void ResultatFonction::Affiche(ostream& os,char cache) {
    char prem=OUI;
	char nomfonc[100];
    int i,ist;
	Vecteur* filtre=parseur->FILTREFONCTION;

    if (Fx==NULL || (cache==OUI && Fx->cacher==OUI))
        return;    

	if (Fx->type==C_GENERATION) {
		sprintf_s(nomfonc,100,"#%s",Fx->nom);
		filtre=parseur->FILTREGENERATION;
	}
	else
		strcpy_s(nomfonc,100,Fx->nom);

    //Affichage XML
    if (parseur->XML==OUI) {
        os<<"<DEPENDENCY";
        if (Fx!=NULL)
            os<<" name=\""<<nomfonc<<"\">"<<Endl;
        
        if (traits!=NULL) {        
            traits->AfficheValeursXML(parseur,os,filtre);
            
            //Affichage des valeurs de variables
            if (parseur->mathematique)
                for (ist=0;ist<vardep.dernier;ist++)
                    if (vardep[ist]!=NULL && vardep[ist]->utilisee==OUI)
                        vardep[ist]->affichageDependanceXML(os,this);
                    
                    for (ist=0;ist<stack.dernier;ist++) {
                        strcpy_s(parseur->Chaine_traits,3000,stack[ist]);
                        os<<"FEATURE attribute=\"$STACK\" value=\"";
                        os<<ConversionChaine(parseur,parseur->Chaine_traits);
                        os<<"\"/>"<<Endl;
                    }
        }
        
        for (i=0;i<arg.dernier;i++) {
            os<<"<PARAMETER ind=\""<<i<<"\" num=\""<<arg[i]->num_noeud;
            os<<"\" word=\"";
            arg[i]->AfficheMot(parseur,os);
            os<<"\"/>"<<Endl;
        }
        os<<"</DEPENDENCY>"<<Endl;
        return;
    }
    
    
    if (parseur->TestAfficheFonction(AFFICHE_FONCTION)) {
        if (Fx!=NULL)
            os<<ConversionChaine(parseur,nomfonc);
        else
            os<<"*";
        
        if (parseur->TestAfficheFonction(AFFICHE_INCOMPATIBLE)) {
            VECTA<int> arbre;
            compatible.famille(parseur->CompteurIncompatible,arbre);
            if (arbre.dernier) {
                os<<"{";
                for (int ica=0;ica<arbre.dernier;ica++) {
                    if (ica)
                        os<<",";
                    os<<arbre[ica];
                }
                os<<"}";
            }
        }

        if (traits!=NULL) {
			char* s=parseur->Chaine_traits;
			s[0]=0;
			if (parseur->TestOptions(AFFICHAGE_CATEGORIE_GENERATION)) {
				s[0]='[';
				s[1]=0;
				(traits->valeur)->ConcateneValeurs(parseur,s+1,parseur->TOUSTRAITS,",",-1);
				if (s[1]) {
					strcat(s,"]");
					os<<ConversionChaine(parseur,parseur->Chaine_traits);
				}
			}
			else {
				if (parseur->TestOptions(AFFICHAGE_TRAIT_VALEUR_DEPENDANCE))
					(traits->valeur)->ConcateneValeurs(parseur,s,filtre);
				else
					(traits->valeur)->ConcateneAttributs(parseur,s,filtre);            

				if (traits->onto!=NULL) {
					for (i=0;i<traits->onto->dernier;i++) {
						int debut=strlen(s);
						traits->onto->cell(i)->ConcateneValeurs(parseur,parseur->hierarchies->lahierarchie,parseur->TraitsFiltreOntologie,s+debut,"_");                    
					}
				}
	            os<<ConversionChaine(parseur,parseur->Chaine_traits);
			}
        }
        
        //Affichage des valeurs de variables
        if (parseur->mathematique) {
            for (ist=0;ist<vardep.dernier;ist++)
                if (vardep[ist]!=NULL) {
                    os<<"'"<<ConversionChaine(parseur,vardep[ist]->nom)<<":";
                    os<<vardep[ist]->valdep(this);
                    os<<"'";
                }
        }

        for (ist=0;ist<stack.dernier;ist++)
            os<<"_"<<"\""<<ConversionChaine(parseur,stack[ist])<<"\"";
        
        if (parseur->TestOptions(AFFICHAGE_NUMERO_DEPENDANCE)) {
            for (int inum=0;inum<numeros.dernier;inum++) {
                if (parseur->VlabelVecteur[numeros[inum]]!=NULL) {
                    if (parseur->VlabelVecteur[numeros[inum]]->type==REGLES_DEDUCTION)
                        os<<"_[";
                    else
                        os<<"_";
                    if (parseur->VlabelVecteur[numeros[inum]]->etiquette[0]=='#')
                        os<<numeros[inum];
                    else
                        os<<ConversionChaine(parseur,parseur->VlabelVecteur[numeros[inum]]->etiquette);
                    if (parseur->VlabelVecteur[numeros[inum]]->type==REGLES_DEDUCTION)
                        os<<"]";
                }
            }
        }
        
        os<<"(";
        
        if (parseur->TestOptions(AFFICHAGE_ARBRE_CHUNK)==NON) {
            os<<parseur->num_phrase;
            prem=NON;
        }
    }
    
    //Affichage des noeuds
	for (i=0;i<arg.dernier;i++) {
		if (!prem)
			os<<",";
		else
			prem=NON;

		if (parseur->TestOptions(AFFICHAGE_CATEGORIE_GENERATION)) {
			//On affiche la categorie
			os<<ConversionChaine(parseur,arg[i]->X->nom)<<"#"<<arg[i]->num_noeud<<"[";
			Traits* Valeur=arg[i]->lesTraitsAffiche();
			parseur->Chaine_traits[0]=0;
			if (Valeur!=NULL) {
				(Valeur->valeur)->ConcateneValeurs(parseur,parseur->Chaine_traits,parseur->TOUSTRAITS,",",-1);
				if (Valeur->onto!=NULL) {
					for (int ix=0;ix<Valeur->onto->dernier;ix++) {
						Valeur->onto->cell(ix)->ConcateneValeurs(parseur,parseur->hierarchies->lahierarchie,parseur->TraitsFiltreOntologie,parseur->Chaine_traits,"_");                    
					}
				}
			}
			if (!parseur->Chaine_traits[0])
				os<<parseur->LEMME<<":\"";
			else
				os<<ConversionChaine(parseur,parseur->Chaine_traits)<<","<<parseur->LEMME<<":\"";
			string st;
			arg[i]->Lemme(st); 
			os<<st.c_str()<<"\"]";
		}
		else {

			if (parseur->TestOptions(AFFICHAGE_COMPLET))
				os<<"<";

			if (parseur->TestOptions(AFFICHAGE_ARBRE_CHUNK)==NON)
				os<<"'";

			if (parseur->TestOptions(AFFICHAGE_CATEGORIE)) {
				os<<ConversionChaine(parseur,arg[i]->X->nom);
				Traits* Valeur=arg[i]->lesTraitsAffiche();
				if (Valeur!=NULL) {
					(Valeur->valeur)->ConcateneValeurs(parseur,
					parseur->Chaine_traits,
					parseur->FILTRENOEUD);
					if (Valeur->onto!=NULL) {
						for (int ix=0;ix<Valeur->onto->dernier;ix++) {
							strcatchr(parseur->Chaine_traits,',');
							Valeur->onto->cell(ix)->ConcateneValeurs(parseur,parseur->hierarchies->lahierarchie,parseur->TraitsFiltreOntologie,parseur->Chaine_traits,",");                    
						}
					}
				}
				os<<ConversionChaine(parseur,parseur->Chaine_traits)<<":";
			}

			arg[i]->AfficheMot(parseur,os);            

			if (parseur->TestOptions(AFFICHAGE_ARBRE_CHUNK)==NON)
				os<<"'";

			if (parseur->TestOptions(AFFICHAGE_COMPLET))
				os<<">";
		}
	}   
    
    if (!parseur->TestAfficheFonction(AFFICHE_FONCTION)) {
        if (traits!=NULL) {
            parseur->Chaine_traits[0]=0;
            (traits->valeur)->ConcateneAttributs(parseur,parseur->Chaine_traits,filtre);
        }
        
        os<<"<"<<nomfonc<<ConversionChaine(parseur,parseur->Chaine_traits)<<">";
        os<<"="<<parseur->num_phrase<<Endl;
        //On reaffiche a l'envers les informations pour optimiser 
        //la recherche par la suite...
        if (parseur->TestAfficheFonction(AFFICHE_INVERSE)) {
            prem=OUI;
            for (i=arg.dernier-1;i>=0;i--) {
                if (!prem)
                    os<<",";
                else
                    prem=NON;                
                
                if (parseur->TestOptions(AFFICHAGE_COMPLET))
                    os<<"<";
                arg[i]->AfficheMot(parseur,os);  
                if (parseur->TestOptions(AFFICHAGE_COMPLET))       
                    os<<">";
            }   
            os<<">"<<nomfonc<<ConversionChaine(parseur,parseur->Chaine_traits)<<"<";
            os<<"="<<parseur->num_phrase<<Endl; 
        }
    }
    else {
		if (cache!=3) {
			if (parseur->TestOptions(AFFICHAGE_ARBRE_CHUNK)==NON)
				os<<")."<<Endl;
			else
				os<<")"<<Endl;    
		}
		else
			os<<")";
    }
}    

//Gestion de la liste des ResultatFonction. Les objets de ce type sont geres
//sous la forme d'une table dans laquelle on extrait les objets au fur et a
//mesure des besoins. Lorsqu'un element doit etre detruit, on le rend simplement
//disponible dans la table. De cette facon on evite des allocations et des
//desallocation memoire qui sont tres couteuses.
//RendreResFonc desalloue un element et le rend de nouveau disponible pour d'autres
//allocations
void GlobalParseur::RendreResFonc(ResultatFonction* r) {
    
    if (r->position < fournirResFonc->dernier)
        fournirResFonc->dernier=r->position;
    r->initialise(NULL);
}

//FournirResFonc recupere un element dans la table et rend un pointeur dessus.
ResultatFonction* GlobalParseur::FournirResFonc(Cat* x,ResultatFonction* rf) {   
    ResultatFonction* r;
    int i=fournirResFonc->dernier;
    
    while (i < fournirResFonc->taille &&
        fournirResFonc->vecteur[i]->bon!=OUI) i++;
    
    if (i >= fournirResFonc->taille) {
        fournirResFonc->dernier=i;
        fournirResFonc->ajusteVecteur(this,i);
    }
    
    r=fournirResFonc->vecteur[i];
    fournirResFonc->dernier=i+1;        
    
    if (rf!=NULL)
        r->affecte(rf);
    else
        r->initialise(x);
    
    r->bon=BLOQUEE;
    
    return r;
}



//-------------------------------------------------------------------------------

/*
La comparaison suit les regles suivantes:

 a) Si la categorie est de type syntagmatique, aucune ambiguite, une seule categorie est disponible.
 b) Si la categorie est de type lexicale, mais avec une seule interpretation, pas d'ambiguite non plus.
 c) Si la categorie est de type lexicale avec plus d'une interpretation, mais qu'elle est situee sous un
 noeud inferieur dans un arbre, pas d'ambiguite non plus.
 dans les autres cas, on regarde quelle categorie utiliser par une exploration de ses possibilites.
*/
int i_compte=0;
static float i_total=0;

char GlobalParseur::CompareNoeudSousArbre(ArcArbre* opt,Noeud* n) {
    char couv;
    Traits* trtest=opt->traits;
    
    i_compte++;

    if (opt==NULL || n==NULL || n->source==NULL)
        return opt->negation;
    
    char res=CompareCategories(n->source,opt->X,opt->selectiontous,opt->selection,NON,couv);
    if (!res)
        return opt->negation;
    
    char leOUI=1-opt->negation;
    
    if (trtest==NULL)
        return leOUI;
    
    if (n->lesTraits()==NULL)
        return opt->negation;
        
    n->source->instanciateur=NULL;
    Liste* L=n->source;
    
    if (L->voc!=NULL && L->voc->nbX>1 && (trtest->lemme || trtest->surface)) {
        int choix=L->choisie;
        ListeVoc* v=L->voc;
        for (int i=0;i<v->nbX;i++) {
            if (opt->X->Val!=0 && opt->X!=v->X[i])
                continue;

            L->choixCategorie(NULL,i);
            if (ITR(this,trtest,L->traitsbase,NONECRIT,&L->N)) {
                L->choixCategorie(NULL,choix);
                return !opt->negation;
            }
        }

        L->choixCategorie(NULL,choix);
        return opt->negation;
    }

	if (ITR(this,trtest,(n->source)->traitsbase,NONECRIT,n) == opt->negation)
        return NON;
    else
        return OUI;
    
    return leOUI;
}

char GlobalParseur::CompareCategories(Liste* L,Cat* X,Traits* selectiontous,
                                      Traits* selection,char maj,char& couverture) {
    
    couverture=OUI;
    int i;
    ListeVoc* v;
    
    L->aSelectionner=NULL;
    
    //Dans le cas d'une regle de sequence, si l'element est integre via un ?, sa
    //categorie n'est pas definitive...
    if (X->Val==0)
        couverture=UTILISE;
    
    if (selectiontous==NULL && selection==NULL) {
        if (X->Val==0 || X==L->X)
            return OUI;
        
        v=L->voc;
        
        if (L->couvert==OUI || v==NULL || v->nbX==1) {
            if (X==L->X)
                return OUI;
            
            if (X->val==CATFUSION) { //cas particulier
                if ((X->Val & L->X->Val) == L->X->Val)
                    return OUI;     
            }       
            return NON;
        }
        
        if (v==NULL)
            return NON;
        
        if (X->val==CATFUSION) {
            if ((L->index & X->Val))
                return OUI;
            return NON;
        }
               
        for (i=0;i<v->nbX;i++) {
            if (X==v->X[i]) {
                if (maj==OUI)
                    L->choixCategorieTestNoeud(this,i);
                return (2+i);            
            }
        }
    }
    
    //Si les entrees sont vides, il ne s'agit pas d'un noeud lexical, il s'agit d'une erreur
    if (L->voc==NULL)
        return NON;
    
    char test=NON;
    if (selectiontous!=NULL) {
        v=L->voc;
        for (i=0;i<v->nbX;i++) {
            if (X->val!=0 && X!=v->X[i])
                return NON;
            if (!ITR(this,selectiontous,v->tr[i],NONECRIT))
                return NON;
        }
        test=OUI;
    }
    
    //dans le cas ou une selection est associee, on l'utilise pour effectuer des
    //verification plus approfondie...
    if (selection!=NULL) {
        v=L->voc;
        
        //Si la categorie est couverte, on ne peut choisir une nouvelle entree        
        //sinon, on explore le reste
        int choix=L->choisie;
        for (i=0;i<v->nbX;i++) {
            if (X->Val==0 || X==v->X[i]) {
				if (ITR(this,selection,v->tr[i],NONECRIT)) {
					if (maj==OUI && L->couvert==NON) {
						L->choixCategorieTestNoeud(this,i);
						L->aSelectionner=selection;
					}
					return OUI;
				}
			}
        }
        L->choixCategorie(NULL,choix);
        return NON;
    }
    
    return test;
}


//-------------------------------------------------------------------------------
char GlobalParseur::CompareEntree(ListeVoc* v,Cat* X,Traits* traits,
                                  Traits* selectiontous,Traits* selection) {
    char trouve=NON;
    int i;
    
    if (selectiontous!=NULL) {
        for (i=0;i<v->nbX;i++) {
            if (X->val==0 || X==v->X[i]) {
                //dans le cas d'une selection particuliere
                if (!ITR(this,selectiontous,v->tr[i],NONECRIT))
                    return NON;
            }
            else
                return NON;
        }
    }
    
    
    for (i=0;i<v->nbX;i++) {
        if (X->val==0 || X==v->X[i]) {
            //dans le cas d'une selection particuliere
            if (selection!=NULL)
                if (ITR(this,selection,v->tr[i],NONECRIT))
                    return OUI;
                trouve=OUI;
        }
    }
    
    if (v->global==NULL)
        return NON;
    
    //sinon il s'agit d'une selection generale, que l'on fait une seule fois...
	if (trouve==OUI) {
		if (traits==NULL)
			return OUI;
		if (ITR(this,traits,v->global,NONECRIT)) {
			//dans le cas ou il y a un test sur la forme de surface ou sur le lemme
			if (traits->lemme != NULL) {       
				trouve=NON;
				for (i=0;i<v->lemme.dernier;i++) {
					if (!traits->lemme->recherche((char*)v->lemme[i])) {            
						if (traits->maniplemme==NEG) {
							trouve=OUI;
							break;
						}
					}
					else {
						if (traits->maniplemme!=NEG) {
							trouve=OUI;    
							break;
						}
					}
				}
				if (trouve==NON)
					return NON;
			}

			if (traits->surface != NULL) {
				if (!traits->surface->recherche((char*)v->surface)) {            
					if (traits->manipsurface!=NEG)
						return NON;    
				}
				else {
					if (traits->manipsurface==NEG)
						return NON;    
				}
			}
  
            return OUI;
		}
	}
        
	return NON;
}

//-------------------------------------------------------------------------------
char GlobalParseur::CompareNoeudSousArbre(ArcArbre* boucle,Liste* L, char maj,char& couverture) {
    
    Traits* trtest=boucle->traits;
    
    char res=CompareCategories(L,boucle->X,boucle->selectiontous,boucle->selection,maj,couverture);
    if (!res)
        return boucle->negation;
    
    //Si le test reussit, alors on renvoie la difference avec la negation
    //exemple: si le test reussit et la negation vaut OUI, le retour sera 1-OUI=NON
    // sinon si le test echoue le retour sera la negation elle-meme
    char leOUI= 1-boucle->negation;
        
    if (trtest==NULL)
        return leOUI;
    
    L->instanciateur=NULL;
    
    if (L->voc!=NULL && L->voc->nbX>1 && (trtest->lemme || trtest->surface)) {
        int choix=L->choisie;
        ListeVoc* v=L->voc;        
        for (int i=0;i<v->nbX;i++) {
            if (boucle->X->Val!=0 && boucle->X!=v->X[i])
                continue;

            L->choixCategorie(NULL,i);
            if (ITR(this,trtest,L->traitsbase,NONECRIT,&L->N)) {
                if (boucle->negation==NON) {
                    L->instanciateur=boucle->traits;
                    return OUI;            
                }
                return NON;
            }
        }

        L->choixCategorie(NULL,choix);
        return boucle->negation;
    }

    if (ITR(this,trtest,L->traitsbase,NONECRIT,&L->N) != boucle->negation) {
        if (boucle->negation==NON)
            L->instanciateur=boucle->traits;
        return OUI;
    }
    else
        return NON;
    
    return boucle->negation;
}

//-------------------------------------------------------------------------------------

char GlobalParseur::TestTraitsFonction(Traits* tfTraits, 
                                       Traits* resTraits,
                                       VECTA<char*>& stack, 
                                       char clearStack,
                                       VECTA<char*>& resStack) {
    if (tfTraits==NULL)
        return OUI;
    
    if (resTraits==NULL)
        return NON;
    
    //On teste les piles de chaines...
    //dans stack on impose la presence de certaines chaines.
    //si resStack contient moins de chaines que ce que l'on attend, c'est une erreur
    if (resStack.dernier<stack.dernier || 
        (clearStack==OUI && resStack.dernier!=0) || 
        (clearStack==STACKNONVIDE && resStack.dernier==0))
        return NON;
    
    for (int ist=0;ist<stack.dernier;ist++) {
        char trouve=NON;
        char* chaine=stack[ist];
        char neg=NON;
        if (chaine[0]=='~') {
            neg=OUI;
            chaine++;
        }
        
        for (int isr=0;isr<resStack.dernier;isr++)
            if (!strcmp(resStack[isr],chaine)) {
                trouve=OUI;
                break;
            }
            
            if (trouve==neg)
                return NON;
    }
    
    return ITR(this,tfTraits,resTraits,NONECRIT);
}


//Cette comparaison porte sur les bits entre deux ensembles de traits
//Il s'agit de la comparaison non stricte
inline char CompareIntersection(Traits* trsgauche,
                                Traits* trsdroit,    
                                Vecteur* filtre,
                                int taille,
                                typeComparateur comparateur) {
    TVecteur resultat=0;
    char trouveUnBit=NON;
    
    if (filtre==NULL)
        return NON;
    
    for (short i=0;i<taille;i++) {
        TVecteur atester=filtre->vecteur[i];
        TVecteur calcgauche=(trsgauche->valeur)->vecteur[i] & atester;
        TVecteur calcdroit=(trsdroit->valeur)->vecteur[i] & atester;
        
        if (comparateur==EGAUX) {
            //egalite stricte au niveau binaire
            if (calcgauche != calcdroit)
                return NON;
        }
        else {
            //dans le cas d'une simple intersection, il faut que l'on ait trouve
            //des valeurs pour l'un ou l'autre des noeuds, et a ce moment la on calcule
            //l'intersection entre les deux structures.
            if (calcgauche!=0 || calcdroit!=0) {
                trouveUnBit=OUI;
                resultat |= (calcgauche & calcdroit);        
            }
        }
    }
    
    //si les valeurs etaient toujours a zero, on considere dans le cas d'une egalite non stricte
    //que ca marche...
    if (comparateur==EGAUX || trouveUnBit==NON)
        return OUI;
    
    //l'union des intersections rend 0 = erreur
    if (resultat==0)
        return NON;
    return OUI;
}

char GlobalParseur::CompareLesTraits(Noeud* ngauche,
                                     Noeud* ndroit,
                                     Traits* testtrgauche,
                                     Traits* testtrdroit,
                                     typeComparateur compare) {
    
    string chgauche;
    string chdroit;
    if (testtrgauche==NULL)
        if (testtrdroit==NULL)
            return OUI;
        else
            return NON;
        else        
            if (testtrdroit==NULL)
                return NON;
            
            
            typeComparateur comparateur=compare;
            char soustraits=NON;
            
            if (compare==COMPATIBLES_LECTURE) {
                comparateur=COMPATIBLES;
                soustraits=OUI;
            }
            else
                if (compare==EGAUX_LECTURE) {
                    comparateur=EGAUX;
                    soustraits=OUI;
                }
                
                if (soustraits==OUI) {
                    if (ngauche->source->voc== NULL || ndroit->source->voc==NULL)
                        return NON;
                }
                
                //On teste le lemme
                if (testtrgauche->lemme!=NULL &&  testtrdroit->lemme!=NULL) {
                    if (testtrgauche->lemme->recherche("*")  && testtrdroit->lemme->recherche("*")) {
                        chgauche="";
                        chdroit="";

                        ngauche->Lemme(chgauche);
                        ndroit->Lemme(chdroit);
                        
						if (chgauche!=chdroit)
							return NON;
                        
                        if (testtrgauche->present==NULL && testtrdroit->present==NULL)
                            return OUI;
                    }
                }
                
                //On teste la surface
                if (testtrgauche->surface!=NULL &&  testtrdroit->surface!=NULL) {
                    if (testtrgauche->surface->recherche("*")  && 
						testtrdroit->surface->recherche("*")) {
                        chgauche="";
                        chdroit="";
                        
                        ngauche->Surface(chgauche);
                        ndroit->Surface(chdroit);

						if (chgauche!=chdroit)
							return NON;
                        
                        if (testtrgauche->present==NULL && testtrdroit->present==NULL)
                            return OUI;
                    }
                }
                
                if (soustraits==OUI) {
                    ListeVoc* vgauche=ngauche->source->voc;
                    ListeVoc* vdroit=ndroit->source->voc;
                    
                    for (int i1=0;i1<vgauche->nbX;i1++) {
                        for (int i2=0;i2<vgauche->nbX;i2++) {
                            if (CompareIntersection(vgauche->tr[i1],
                                vdroit->tr[i2],
                                testtrgauche->present,
                                iTailleVect,
                                comparateur)) {
                                ngauche->source->choixCategorie(NULL,i1);
                                ndroit->source->choixCategorie(NULL,i2);
                                return OUI;
                            }
                        }
                    }
                    return NON;
                }
                
                return CompareIntersection((ngauche->source)->traitsbase,
                    (ndroit->source)->traitsbase,
                    testtrgauche->present,
                    iTailleVect,
                    comparateur);
}



char TestNoeudSelection(GlobalParseur* parseur,Noeud* n, Traits* selectiontous,
                        Traits* selection, Traits* traits) {        
    char test=OUI;
    if (n==NULL || n->source==NULL)
        return NON;
    
    Liste* L=n->source;
    ListeVoc* v;
    int i;
    
    if (selectiontous!=NULL) {
        if (L->voc==NULL)
            return NON;
        v=L->voc;
        for (i=0;i<v->nbX;i++)
            if (!ITR(parseur,selectiontous,v->tr[i],NONECRIT,n))
                return NON;
    }
    
    if (selection!=NULL) {
        if (L->voc==NULL)
            return NON;
        test=NON;
        v=L->voc;
        for (i=0;i<v->nbX;i++)
            if (ITR(parseur,selection,v->tr[i],NONECRIT,n)) {
                L->choixCategorie(NULL,i);
                test=OUI;
                break;
            }        
    }
    
    if (test==OUI && traits!=NULL)
        test=ITR(parseur,traits,L->traitsbase,ECRIT,n);
    
    return test;
}

TVecteur TrouveValeur(Vecteur* v, Vecteur* vf,int& rang) {
    TVecteur val=0;
    for (int i=0;i<v->TailleVect;i++) {
        val= v->vecteur[i] & vf->vecteur[i];
        if (val!=0) {
            rang=i;
            return val;
        }
    }
    return 0;
}

void TrouveBorneValeur(Vecteur* v, Vecteur* vf,int& gauche,int& droite) {
    TVecteur val=0;
    gauche=-1;
    droite=-1;
    for (int i=0;i<v->TailleVect;i++) {
        val= v->vecteur[i] & vf->vecteur[i];
        if (val!=0) {
            if (gauche==-1)
                gauche=i;            
            droite=i;
        }
    }
}


//Il s'agit d'une verification de compatibilite ou d'egalite entre deux ensembles d'elements ontologiques
char TestOntologie(typeComparateur comparateur,Noeud* ng,Noeud* nd,Traits* tg,Traits* td, char negation) {

    //Compatibilite
    //Tout d'abord, il faut rechercher les elements qui correspondent au test

    if (ng->source->traitsbase->onto==NULL || 
        nd->source->traitsbase->onto==NULL)
        return negation;
   
    VECTA<ontologie*>& vog=*ng->source->traitsbase->onto;
    VECTA<ontologie*>& vod=*nd->source->traitsbase->onto;
    ontologie* og=tg->onto->cell(ONTOLOGIE_COMPARAISON);    
    ontologie* od=td->onto->cell(ONTOLOGIE_COMPARAISON);    

    if (vog[ONTOLOGIE_COMPARAISON]==NULL ||
        vod[ONTOLOGIE_COMPARAISON]==NULL)
        return negation;

    //Dans le cas d'une egalite stricte, il faut que les deux ensembles partagent le meme
    //chemin pour les traits imposes
    if (comparateur==EGAUX_ONTOLOGIE) {
        //On construit une copie des traits du noeud gauche
        ontologie otg(vog[ONTOLOGIE_COMPARAISON]);
        //On ne garde que les elements du chemin qui nous interessent
        otg.et(og);
        //On construit une copie des traits du noeud droit
        ontologie otd(vod[ONTOLOGIE_COMPARAISON]);
        //On ne garde que les elements du chemin qui nous interessent
        otd.et(od);
        //Si les deux chemins sont les memes, c'est gagne
        if (otd.egal(otg))
            return 1-negation;
        else
            return negation;        
    }

    //sinon, on verifie que chaque ensemble comprend notre trait
    if (vog[ONTOLOGIE_COMPARAISON]->present(og) &&
        vod[ONTOLOGIE_COMPARAISON]->present(od))
        return 1-negation;
    
    return negation;
}


char CompareDomaine(Noeud* n1,Noeud* n2, Traits* tr1, Traits* tr2,char egalite) {
    int i;
    TVecteur val1=0;
    TVecteur val2=0;
    int rang1,rang2;
    
    
    //On recupere d'abord la valeur qui nous interesse
    if (n1!=NULL)
        val1 = TrouveValeur(n1->source->traitsbase->valeur,tr1->present,rang1);
    else {
        for (i=0;i<tr2->valeur->TailleVect;i++)
            if (tr2->valeur->vecteur[i]!=0) {
                val1=tr2->valeur->vecteur[i];
                rang1=i;
                break;
            }
    }
    
    if (n2!=NULL)
        val2 = TrouveValeur(n2->source->traitsbase->valeur,tr2->present,rang2);
    else {
        for (i=0;i<tr1->valeur->TailleVect;i++)
            if (tr1->valeur->vecteur[i]!=0) {
                val2=tr1->valeur->vecteur[i];
                rang2=i;
                break;
            }
    }
    
    if (!val1 || !val2 || rang1>rang2)
        return NON;
    
    if (egalite) {
        if (rang1<rang2 || val1 <= val2)
            return OUI;
    }
    else
        if (rang1<rang2 || val1 < val2)
            return OUI;
        
        return NON;
}

char TestFonctionNoeud::AffectationLectFormule(GlobalParseur* parseur, 
                                               Noeud* ngauche,
                                               VECTA<Noeud*>& argNoeuds, 
                                               VECTA<Traits*>* affectation) {
    if (affectation==NULL) return NON;
    Noeud* n=argNoeuds[arg[0]];
    //Calcul des traits sur un noeud donne
    //Si un noeud n'est pas initialise on revient en NON
    //On initialise avec le premier mot ou le mot qui recoit, si celui-ci a deja des lectures
    if (n==NULL || n->source->voc==NULL)
        return NON;
    
    //On parcourt tous les elements de arg (l'operateur est entre les deux)
    int vok;
    int vok1,vok2;
    	
    ListeVoc* nouveau_voc= parseur->fournirListeVocVide();
	ListeVoc* ancien_voc=ngauche->source->voc;
	ListeVoc* vcourant=n->source->voc;
    VECTA<int> correspondances;
	VECTA<char> retient;


	nouveau_voc->garder.raz();
	nouveau_voc->surface=DupliqueChaine(ngauche->X->nom);
	//dans le cas ou il n'y a pas eu d'application de ce type de regle avant
	//on cree notre liste a partir du premier element
	if (ancien_voc==NULL) {
		if (ngauche->source->blocfiltrage==NULL)
			ngauche->source->blocfiltrage=new Vecteur(parseur->iTailleVect);
		else
			ngauche->source->blocfiltrage->raz();
		for (vok=0;vok<vcourant->nbX;vok++) {
			if (ITR(parseur,arg_traits[0],vcourant->tr[vok],NONECRIT)) {
				nouveau_voc->X.ajoute(ngauche->X);
				nouveau_voc->lemme.ajoute(DupliqueChaine(ngauche->X->nom));				
				nouveau_voc->tr.ajoute(new Traits(vcourant->tr[vok]));
				nouveau_voc->garder.ajoute(OUI);
				nouveau_voc->nbX++;
			}
		}
	}
	else {
		//Sinon, on ne garde que les lectures compatibles
		retient.raz();
		for (vok1=0;vok1<vcourant->nbX;vok1++) {		
			if (ITR(parseur,arg_traits[0],vcourant->tr[vok1],NONECRIT)) {
				for (vok2=0;vok2<ancien_voc->nbX;vok2++) {
					if (retient[vok2]==0) {
						if (vcourant->tr[vok1]->contient(ancien_voc->tr[vok2])) {
							correspondances.ajoute(vok2);
							retient.affecte(vok2,1);
							nouveau_voc->tr.ajoute(new Traits(vcourant->tr[vok1]));
							nouveau_voc->garder.ajoute(OUI);
							nouveau_voc->nbX++;
							break;
						}
					}
				}
			}
		}
	}


	if (selection!=NULL) 
		ngauche->source->blocfiltrage->ou(selection->valeur);	
	else
		if (selectiontous!=NULL) 
			ngauche->source->blocfiltrage->ou(selectiontous->valeur);

	char modification=NON;

	for (int i=1;i<arg.dernier;i+=2) {
        Noeud* n=argNoeuds[arg[i+1]];		
        if (n==NULL) {
            //delete nouveau_voc;
            return NON;
        }
        //s'il s'agit d'une intersection
        
        vcourant=n->source->voc;
        if (vcourant==NULL) {
            //delete nouveau_voc;
            return NON;
        }

        char ajout=NON;
                
		for (vok1=0;vok1<nouveau_voc->nbX;vok1++) {
			//dans le cas ou il s'agit d'un element elimine
			//on ne le prend pas en compte...
			if (nouveau_voc->garder[vok1]==NON)
				continue;
			nouveau_voc->garder.affecte(vok1,NON);
			for (vok2=0;vok2<vcourant->nbX;vok2++) { 
				//On verifie aussi que cette entree est compatible avec ancien_voc
				char compatible=OUI;
				if (ancien_voc!=NULL) {
					compatible=NON;
					for (int vok3=0;vok3<ancien_voc->tr.dernier;vok3++) {
						if (nouveau_voc->tr[vok1]->contient(ancien_voc->tr[vok3]) &&
							vcourant->tr[vok2]->contient(ancien_voc->tr[vok3])) {
							compatible=OUI;
							break;
						}
					}
				}
				if (compatible==OUI &&
					ITR(parseur,arg_traits[i+1],vcourant->tr[vok2],NONECRIT) &&
					CompareIntersection(nouveau_voc->tr[vok1],
					vcourant->tr[vok2],
					traits[0]->valeur,
					parseur->iTailleVect,
					COMPATIBLES)) {                    

						//On marque alors l'element pour lequel on a trouve une
						//correspondance
						nouveau_voc->garder.affecte(vok1,OUI);
						ajout=OUI;
						modification=OUI;
						//La structure de traits que l'on garde est une intersection sur la base du trait
						//de comparaison
						nouveau_voc->tr[vok1]->et(vcourant->tr[vok2]);
						nouveau_voc->tr[vok1]->et(traits[0]);
						break;
					}
			}
		}

		//si rien n'a ete trouve en commun c'est une erreur
		if (ajout==NON) {
			//delete nouveau_voc;
			return NON;
		}
	}

	//dans le cas ou notre structure initiale n'etait pas vide
	//Il nous faut realiser l'union avec la precedent structure
	if (modification==OUI) {
		char* lem;
		Traits* trr;
		if (ancien_voc!=NULL) {
			for (vok=0;vok<correspondances.dernier;vok++)
				if (nouveau_voc->garder[vok]==OUI)
					ancien_voc->tr[correspondances[vok]]->ou(nouveau_voc->tr[vok]);
		}

		retient.raz();
		retient.affecte(nouveau_voc->garder);
		//On nettoie la liste des choses differentes, dans le cas d'une intersection
		for (vok=0;vok<nouveau_voc->nbX;vok++) {
			if (nouveau_voc->garder[vok]==NON) {
				nouveau_voc->X.retireElement(vok);
				lem=nouveau_voc->lemme.retireElement(vok);
				libere(lem);
				trr=nouveau_voc->tr.retireElement(vok);
				delete trr;
				nouveau_voc->nbX--;
				nouveau_voc->garder.retireElement(vok);
				//on reste alors sur le meme element
				vok--;
			}
		}

		//Si le nombre de solution est inferieur ou egal a 1, il est inutile de s'embarrasser
		//de nouvelle valeur.
		if (nouveau_voc->nbX==0) {
			//delete nouveau_voc;
			return NON;
		}

		if (ancien_voc!=NULL) {
			for (vok=0;vok<nouveau_voc->garder.dernier;vok++) {
				if (retient[vok]==NON) {
					//On nettoie aussi par la meme occasion la liste dans ancien_voc
					int av=correspondances[vok];
					trr=ancien_voc->tr.retireElement(av);
					ancien_voc->X.retireElement(av);
					lem=ancien_voc->lemme.retireElement(av);
					libere(lem);
					ancien_voc->nbX--;
					delete trr;
					retient.retireElement(vok);
					vok--;
				}
			}

			//On detruit cette nouvelle structure, puisque desormais, c'est la structure initiale
			//qui nous interesse
			//delete nouveau_voc;
			nouveau_voc=ancien_voc;
		}
	}


	ngauche->source->traitsbase->valeur->etnon(ngauche->source->blocfiltrage);
	ngauche->source->traitsactif->valeur->etnon(ngauche->source->blocfiltrage);

    Traits* tbuffer=new Traits(parseur);
	tbuffer->valeur->ou(ngauche->lesTraits()->valeur);
	tbuffer->filtre->ou(n->lesTraits()->filtre);
	char modif=NON;
    Vecteur filtreur(parseur->iTailleVect);
    for (vok=0;vok<nouveau_voc->nbX;vok++) {
        if (parseur->FusionneTraits(tbuffer,nouveau_voc->tr[vok],filtreur))
            modif=OUI;
    }
    
    //s'il n'a y qu'une seule lecture, c'est inutile de la conserver, mais ce n'est pas une erreur
	//On marque la zone de creation avec la valeur CREEFORMULE
	nouveau_voc->passage=CREEFORMULE;
	ngauche->source->voc=nouveau_voc;       

	//la destruction est donc planifiee ici...
	if (nouveau_voc!=ancien_voc)
		parseur->phrasefictifs.ajoute(nouveau_voc);

	ngauche->source->traitsbase->ou(tbuffer);
	ngauche->source->traitsactif->ou(tbuffer);

    
    return OUI;
}

char TestFonctionNoeud::AffectationFormule(GlobalParseur* parseur, 
                                           Noeud* ngauche,
                                           VECTA<Noeud*>& argNoeuds, 
                                           VECTA<Traits*>* affectation) {
    if (affectation==NULL)
        return NON;
    //Calcul des traits sur un noeud donne
    //On parcourt tous les elements de arg (l'operateur est entre les deux)
    Noeud* n=argNoeuds[arg[0]];
    //Si un noeud n'est pas initialise on revient en NON
    if (n==NULL)
        return NON;
    //n est le premier element et on initialise buffer avec
    Traits* tbuffer=new Traits(parseur);
	tbuffer->valeur->ou(n->lesTraits()->valeur);
	tbuffer->filtre->ou(n->lesTraits()->filtre);
    //On ne garde que les traits definis dans traits[0]
    tbuffer->et(traits[0]);
    Traits inter(parseur);
    for (int i=1;i<arg.dernier;i+=2) {   
        //On va chercher le noeud a la position suivante
        n=argNoeuds[arg[i+1]];
        if (n==NULL)
            return NON;
        //l'operateur est la position suivante
        //s'il s'agit d'une intersection          
        inter.raz();
        inter.ou(n->lesTraits());
        inter.et(traits[0]);
        
        if (arg[i]==OP_ET)            
            tbuffer->et(&inter);
        else
            //sinon on fait l'union
            tbuffer->ou(&inter);
    }
    
    affectation->ajoute(ngauche->lesTraits());
    affectation->ajoute((ngauche->source)->traitsbase);
    affectation->ajoute(tbuffer);
    return OUI;
}



//On compare deux noeuds #1 et #2 ensemble.
//les comparaisons peuvent porter sur les structures de traits
//sur une comparaison entre des valeurs numeriques (sous forme de traits)
//Il peut aussi s'agir d'une affectation de valeur
char TestFonctionNoeud::TestNoeud(GlobalParseur* parseur,
                                  VECTA<Noeud*>& argNoeuds,
                                  VECTA<Traits*>* affectation=NULL,
                                  VECTA<Traits*>* vecttr=NULL) {
    

    Noeud* ngauche=argNoeuds[arggauche];

	//Cas particulier, ou l'on cree un nouveau noeud a la volee
	if (comparateur==CREATION_NOEUD) {
		//Ce noeud ne doit pas exister
		if (ngauche!=NULL)
			return NON;
		Liste* l=parseur->CreeNoeudFragment(NULL,traits[0],X);
		argNoeuds.affecte(arggauche,&l->N);
		return OUI;
	}
	
    //Nous avons deux cas
    //a) Il s'agit d'un test sur une fonction syntaxique
    //b) Il s'agit d'un test de traits sur les variables
    
    //Si arggauche!=-1 nous sommes dans un test de variable
		
    if (ngauche==NULL)
        return NON;
    
    Noeud* ndroit=argNoeuds[argdroit];
    //Il s'agit d'une comparaison entre deux ensembles de traits
    if (argdroit!=-1 || (comparateur>= INF_TRAITS && comparateur <= SUP_EGAL_TRAITS)) {
                
        if (ndroit==NULL && argdroit!=-1)
            return NON;
        
        typeComparateur compar=comparateur;
        
        switch(comparateur) {
        case EGALITE:
            if (ngauche->num_droite != ndroit->num_droite || ngauche->num_gauche!=ndroit->num_gauche)
                return NON;
            //dans ce cas precis, les traits sont inexistants
            return OUI;
        case INFERIEUR:
            if (ngauche->num_droite >= ndroit->num_gauche)
                return NON;       
            return OUI;
        case SUPERIEUR:
            if (ngauche->num_gauche <= ndroit->num_droite)
                return NON;
            return OUI;
        case INF_TRAITS:
            return CompareDomaine(ngauche,ndroit,traits[GAUCHE],traits[DROIT],NON);
        case INF_EGAL_TRAITS:
            return CompareDomaine(ngauche,ndroit,traits[GAUCHE],traits[DROIT],OUI);
        case SUP_TRAITS:
            return CompareDomaine(ndroit,ngauche,traits[DROIT],traits[GAUCHE],NON);
        case SUP_EGAL_TRAITS:
            return CompareDomaine(ndroit,ngauche,traits[DROIT],traits[GAUCHE],OUI);
        case COMPATIBLES_ONTOLOGIE :
        case EGAUX_ONTOLOGIE:
            return TestOntologie(comparateur,ngauche,ndroit,traits[GAUCHE],traits[DROIT],negation);
        default:
            return parseur->CompareLesTraits(ngauche,ndroit,traits[GAUCHE],traits[DROIT],compar);
        }
    }
        

    if (comparateur==AFFECTATION || comparateur==AFFECTATION_ONTOLOGIE)
        return AffectationFormule(parseur,ngauche,argNoeuds,affectation);
    
    if (comparateur==AFFECTATION_LECTURE)
        return AffectationLectFormule(parseur,ngauche,argNoeuds,affectation);
    
    if (comparateur==TEST) {
        //Il s'agit d'un test sur une variable seule
        if (TestNoeudSelection(parseur,ngauche,selectiontous,selection,traits[GAUCHE])) {            
            if (traits[GAUCHE]!=NULL && traits[GAUCHE]->present==NULL) {
                Liste* lgauche=ngauche->source;
                parseur->InstancieTraits(lgauche->traitsbase,traits[GAUCHE]);
                parseur->InstancieTraits(lgauche->traitsactif,traits[GAUCHE]);
                majlemmesurface(parseur,traits[GAUCHE],ngauche,ECRIT);
                if (vecttr!=NULL && traits[GAUCHE]->libre!=NULL) {
                    Traits* tr=new Traits(lgauche->traitsactif);   
                    parseur->InstancieTraits(tr,traits[GAUCHE]);
                    (tr->libre)->et(traits[GAUCHE]->libre);
                    vecttr->ajoute(tr);
                }
            }
			else {
				ListeVoc* voc=ngauche->source->voc;
				if (voc!=NULL) {
					int it;
					if (selection!=NULL && selection->present==NULL) {					
						for (it=0;it<voc->nbX;it++)
							parseur->InstancieTraits(voc->tr[it],selection);
					}
					if (selectiontous!=NULL && selectiontous->present==NULL) {					
						for (it=0;it<voc->nbX;it++)
							parseur->InstancieTraits(voc->tr[it],selectiontous);
					}
				}
			}
            return OUI;
        }
    }
    
    return NON;
}

VectTFInstruction::~VectTFInstruction() {    
	JETTE(VectTestFonction);
    dependances.nettoie();
    if (regle!=NULL)
        delete regle;
    if (sinon!=NULL)
        delete sinon;
}


//---------------------------------------------------------------------------------------
char TestFonctionDependance::Test(GlobalParseur* parseur,ExtractionFonction* groupe,VECTA<Noeud*>& argNoeuds) {
    
    ResultatFonction* r;
    VECTA<Noeud*> vect;
    char modif=NON;
    
    vect.affecte(argNoeuds);
    
    for (int i=0;i<groupe->dernier;i++) {
        r=(*groupe)[i];
        
        if (r==NULL || (Fx->val!=0 && Fx!=r->Fx))
            continue;
        
        char erreur=NON;
        if (parseur->TestTraitsFonction(traitsFonction,r->traits,stack,clearStack,r->stack)) {
            if (modif==OUI) {
                vect.raz();
                vect.affecte(argNoeuds);
            }
            modif=NON;
            for (int i =0; i<r->arg.dernier;i++) {
                if (arg[i]==-2)
                    break;
                if (arg[i] != -1) {
                    //notre noeud courant:
                    Noeud* n=r->arg[i];
                    
                    //variable manquante, on la rajoute...
                    if (vect[arg[i]] == NULL) {
                        vect.affecte(arg[i],n);
                        modif=OUI;
                    }
                    else
                        if (vect[arg[i]] != n) {
                            erreur=OUI;
                            break;
                        }
                        
                        //s'il y a des traits associes a l'argument, on teste:
                        if (traits[i] != NULL) {
                            //Pas de traits sur le noeud, erreur
                            if (n->lesTraits() == NULL) {
                                erreur=OUI;
                                break;
                            }
                            //erreur sur les traits...
                            if (!ITR(parseur,traits[i],n->lesTraits(),NONECRIT,n)) {
                                erreur=OUI;
                                break;
                            }
                        }                        
                }
            }
            if (erreur==NON) {
                if (modif==OUI) {
                    argNoeuds.raz();
                    argNoeuds.affecte(vect);
                }
                return OUI;
            }
        }
    }
    return NON;
}


//---------------------------------------------------------------------------------------


void ListeTestArbre::Destruction(GlobalParseur* parseur,
                                 VECTA<TestFonction*>& listetf,
                                 VECTA<TestFonction*>& vtf) {
    if (test==NULL)
        return;
    int index=test->index;
    if (listetf[index]!=NULL) {
        vtf.ajoute(test);
        test=NULL;
        listetf.affecte(index,NULL);
    }
}

void VectTestFonctionSemantique::Destruction(GlobalParseur* parseur) {
    if (!aDetruire.dernier)
        return;
    
    VECTA<TestFonction*> vtf;
    VECTA<ListeTestArbre*> vta;
    int i;
    
    for (i=0;i<aDetruire.dernier;i++) {
        if (aDetruire[i]->deterministe!=-1) {
            aDetruire[i]->Destruction(parseur,aDetruireFonc,vtf);
            vta.ajoute(aDetruire[i]);
            aDetruire[i]->deterministe=-1;
        }
    }
    
    for (i=0;i<vtf.dernier;i++)
        delete vtf[i];
    
    for (i=0;i<vta.dernier;i++)
        delete vta[i];
    
    aDetruire.raz();
}


//------------------------------------------------------------------------------------
//Test le where dans une regle...
char ListeTestArbre::TestFormule(GlobalParseur* parseur,
                                 VECTA<Noeud*>& vectNoeuds,
                                 size_t i) {
    
    ExtractionFonction* listeResFonc;
    static VECTA<ResultatFonction*> fonctions;
    
    //si tous les elements ont ete verifies
    //On ajoute une nouvelle fonction
    if (i >= (size_t)liste.dernier) 
        return OUI;
    
    
    TestFonction* ttf=liste[i]->test;
    
    if (ttf==NULL) {
        VECTA<ListeTestArbre*>& sousliste=liste[i]->liste;
        
        //On effectue l'analyse du OU
        for (int il=0;il<sousliste.dernier;il++)
            if ((sousliste[il])->TestFormule(parseur,vectNoeuds,0)==OUI)
                return OUI;        
            return NON;
    }
    
    //Test sur un noeud, et non sur une dependance.
    if (TestFoncDep(ttf)==NON) {
        char testRes=NON;
        char erreur=NON;            
        VECTA<Traits*> affectation;
        TestFonctionNoeud* ttfnoeud;       
        TestFonctionAffichage* taff;
        TVecteur execution;
        TVecteur affichage;        
#ifdef XIPLIBXML            
        int iar;
        xmlresultat resxmlnode;
        VECTA<uchar*>* lesClefs;
        clefXML* entree;
#endif

        switch(ttf->Type()) {
#ifdef XIPLIBXML
        case TESTFONCTIONXML:            
            erreur=OUI;
            resxmlnode.raz();
            entree=((TestFonctionXML*)ttf)->clefxml;
            if (parseur->ExecuteXML(entree,vectNoeuds,fonctions,&resxmlnode) == ttf->negation)                
                return NON;            
            if (resxmlnode.dernier()>0) {
                lesClefs=new VECTA<uchar*>;
                for (iar=0;iar<resxmlnode.dernier();iar++) {                    
                    if (entree->AnalyseClefNodeXML(parseur,resxmlnode[iar],
                        resxmlnode.index,resxmlnode.arboxml,*lesClefs,vectNoeuds,fonctions)) {
                        break;
                    }
                }
                delete lesClefs;
            }            
            testRes=TestFormule(parseur,vectNoeuds,i+1);
            break;
#endif
        case TESTFONCTIONMATHCHAINE:
        case TESTFONCTIONMATH:
        case TESTFONCTIONPROCEDURE:
            //formule de calcul
            parseur->initialise_erreur_instruction();
            testRes=(char)ttf->TraitementFormule(parseur,vectNoeuds,erreur);
            if (erreur==OUI && parseur->TestOptions(AFFICHAGE_ERREUR_EXECUTION))
                parseur->affiche_derniere_erreur(-1);
            if (erreur==NON && testRes!=ttf->negation)
                testRes=TestFormule(parseur,vectNoeuds,i+1);
            else
                testRes=NON;
            break;
        case TESTFONCTIONAFFICHAGE:
            taff=(TestFonctionAffichage*)ttf;
            execution=taff->affiche&EXECUTION;
            affichage=taff->affiche&~EXECUTION;            
            if (affichage) {
                if ( (affichage & AFFICHAGE_STANDARD) == AFFICHAGE_STANDARD) {
                    parseur->gAfficherResultats=parseur->gAfficherResultatsDefini;
                    parseur->ParametresAffichage=parseur->ParametresAffichageDefini;
                    parseur->CalculFonction=parseur->CalculFonctionDefini;
                    parseur->DESAMBIGUISATION=parseur->DESAMBIGUISATIONDefini;
                    parseur->appliqueGrammaire=parseur->appliqueGrammaireDefini;
                    affichage&=~AFFICHAGE_STANDARD;
                }
                if ( (affichage & AFFICHAGE_NUL) == AFFICHAGE_NUL) {
                    parseur->gAfficherResultats=NON;
                    affichage&=~AFFICHAGE_NUL;
                }
                if (affichage!=0)
                    parseur->ParametresAffichage=affichage;
            }
            switch (execution) {
            case EXECUTE_CHUNK:
                parseur->CalculFonction=NON;
                break;
            case EXECUTE_NUL:
                parseur->parse_phrase=NON;                
                break;
            case EXECUTE_DEPENDENCY:
                parseur->CalculFonction=OUI;
            }
            break;
        default:
            //Test sur la negation
            //ce test suit la regle suivante:
            //si TestNoeud renvoie OUI, c'est que les traits se sont unifies, donc s'il y a negation
            //il y a echec. En revanche si TestNoeud renvoie NON, c'est qu'il y a eu echec, donc en cas 
            //de negation le test reussit. Il faut donc que le resultat de TestNoeud soit different de la
            //negation pour reussir.            
            ttfnoeud=(TestFonctionNoeud*)ttf;
            if (ttfnoeud->TestNoeud(parseur,vectNoeuds,&affectation) == ttfnoeud->negation)
                return NON;        
            
            testRes=TestFormule(parseur,vectNoeuds,i+1);
            if (testRes==OUI) {
                //On affecte les valeurs
                for (int iaff=0;iaff<affectation.dernier;iaff+=3) {
                    //On retire les traits en question de n->lesTraits()
                    affectation[iaff]->etnon(affectation[iaff+2]);
                    //puis on les ajoute
                    affectation[iaff]->ou(affectation[iaff+2]);
                    //On fait de meme avec traitsbase
                    affectation[iaff+1]->etnon(affectation[iaff+2]);
                    //puis on les ajoute
                    affectation[iaff+1]->ou(affectation[iaff+2]);
                }
				//Puis on nettoie...
				NettoieAffectation(affectation,0);
            }
            else {
                Noeud* ngauche=vectNoeuds[ttfnoeud->arggauche];
                if (ngauche->source->voc!=NULL && ngauche->source->voc->passage==CREEFORMULE) {
                    ngauche->source->voc->passage=NON;
                    ngauche->source->voc=NULL;
                }
            }
        }
        return testRes;
    }
    
    //Une dependance est associee a une liste de noeuds qui corresponde a ses arguments.
    //Chaque dependance est indexee sur son argument 0, soit sur le numero de noeud correspondant
    //a cet argument (num_noeud)...
    //Si le premier argument est deja trouve, on passe alors par la liste indexee
    //sur les noeuds...
    TestFonctionDependance* ttfdep=(TestFonctionDependance*)ttf;
    
    Noeud* premierNoeud=vectNoeuds[ttfdep->arg[0]];
    ExtractionFonction* choisirSurNoeud=NULL;
    ExtractionFonction* choisirSurCategorie=NULL;
    char choisir=NON;
    listeResFonc=NULL;
    
    //On prend la liste la plus courte...
    if (premierNoeud != NULL) {
        choisirSurNoeud=listeResFonc=parseur->PREMIERFONCTIONS[premierNoeud->num_noeud];
        if (choisirSurNoeud==NULL || choisirSurNoeud->dernier==0) {
            listeResFonc=NULL;
            choisir=BLOQUEE;
        }
        else
            choisir=OUI;
    }
    
    if (choisir!=BLOQUEE) {
        choisirSurCategorie=parseur->INTERFONCTIONS[(ttfdep->Fx)->val];   
        if (choisirSurCategorie==NULL || choisir==NON)
            listeResFonc=choisirSurCategorie;
        else //On choisit la plus courte
            if (choisirSurCategorie->dernier < choisirSurNoeud->dernier)
                listeResFonc=choisirSurCategorie;
            else
                listeResFonc=choisirSurNoeud;
    }
    
    if (listeResFonc==NULL || listeResFonc->dernier==0) {
        if (ttfdep->negation) 
            return TestFormule(parseur,vectNoeuds,i+1);
        else
            return NON;
    }
    
       
    //Test sur la negation
    if (ttfdep->Test(parseur,listeResFonc,vectNoeuds) == ttfdep->negation)
        return NON;        
    
    return TestFormule(parseur,vectNoeuds,i+1);
}

char ListeTestArbre::TestDependances(GlobalParseur* parseur,size_t i) {
	ExtractionFonction* listeResFonc;

	//si tous les elements ont ete verifies
	//On ajoute une nouvelle fonction
	if (i >= (size_t)liste.dernier) 
		return OUI;


	TestFonction* ttf=liste[i]->test;

	if (ttf==NULL) {
		VECTA<ListeTestArbre*>& sousliste=liste[i]->liste;

		//On effectue l'analyse du OU
		for (int il=0;il<sousliste.dernier;il++)
			if ((sousliste[il])->TestDependances(parseur,0)==OUI)
				return OUI;        
		return NON;
	}

	//Test sur un noeud, et non sur une dependance.
	//Nous evitons de tester dans ce cas les negations, car les noeuds ne sont pas present
	if (TestFoncDep(ttf) && ttf->negation==NON) {
		//Une dependance est associee a une liste de noeuds qui corresponde a ses arguments.
		//Chaque dependance est indexee sur son argument 0, soit sur le numero de noeud correspondant
		//a cet argument (num_noeud)...
		//Si le premier argument est deja trouve, on passe alors par la liste indexee
		//sur les noeuds...
		TestFonctionDependance* ttfdep=(TestFonctionDependance*)ttf;
		listeResFonc=parseur->INTERFONCTIONS[(ttfdep->Fx)->val];   
		if (listeResFonc==NULL || listeResFonc->dernier==0)
			return NON;
		char trouve=NON;
		ResultatFonction* r=NULL;
		for (int bouc=0;bouc<listeResFonc->dernier;bouc++) {
			r=(*listeResFonc)[bouc];
			if (r!=NULL && r->bon!=BLOQUEE) {
				if (ITR(parseur,ttfdep->traitsFonction,r->traits,NONECRIT)==OUI) {
					char v=r->bon;
					r->bon=BLOQUEE;
					trouve=TestDependances(parseur,i+1);
					r->bon=v;
					if (trouve==OUI)
						return OUI;
				}
			}
		}
		return NON;				
	}
	return TestDependances(parseur,i+1);
}
//---------------------------------------------------------------------------------------
//Nettoyage d'une dependance
//On les detruit a l'envers. En effet, certains elements peuvent etre le fils d'autres elements
//au-dessus...
void GlobalParseur::DestructionNodes(VECTA<ResultatFonction*>& creees) {
	for (int i = creees.dernier - 1; i >= 0; i--)
		destructiondependance(creees[i]);
}


void GlobalParseur::destructiondependance(ResultatFonction* dep) {
	
	
	//Dans le cas d'une generation, on doit aussi retirer le genereGraphe element de son enclos
	if (dep->frere!=NULL) {
		if (dep->frere->retire()==OUI)
			delete dep->frere;
		dep->frere=NULL;
	}

	if (dep->bon==MODIFIEE)
		return;

	Cat* Xs=dep->Fx;	

	if (Xs==NULL)
		return;

	ExtractionFonction* listeResFonc=INTERFONCTIONS[Xs->val];
	int indexChgt=dep->indexcategorie;
	//Nettoyage de l'element
	if (listeResFonc==NULL)
		return;

	//Nettoyage dans la liste indexee sur le premier noeud...
	int posnoeud=(dep->arg[0])->num_noeud;
	PREMIERFONCTIONS[posnoeud]->affecte(dep->indexpremier,NULL);

	if (dep->arg[1]) {
		posnoeud=(dep->arg[1])->num_noeud;
		//Nettoyage dans la liste indexee sur le second noeud...
		SECONDFONCTIONS[posnoeud]->affecte(dep->indexsecond,NULL);
	}
	//Nettoyage general
	if (listeResFonc!=NULL)
		listeResFonc->affecte(indexChgt,NULL);

	posnoeud=Xs->val;
	NombreDependances.affecte(posnoeud,NombreDependances[posnoeud]-1);

	if (NombreDependances[posnoeud]==0)
		ClefDependances ^= Xs->Val;

	INTERFONCTIONS[0]->affecte(dep->index,NULL);
	RendreResFonc(dep);
}
//---------------------------------------------------------------------------------------
//Cette methode est utilisee pour mettre a jour les dependances lors de l'application d'une regle
//de deduction. Plusieurs cas sont possibles:
//a) Destruction de la dependance dont le pointeur est dans dependanceChgt
//b) Modification du nom de la dependance dont le pointeur est dans dependanceChgt
//c) Modification des traits de la dependance dont le pointeur est dans dependanceChgt
//d) Creation d'une nouvelle dependance
ResultatFonction* GlobalParseur::CreeNouvelleDependance(Categorie* cfonc,Traits* tr,VECTA<Noeud*>& vect) {
	ResultatFonction* nouvelleDependance=FournirResFonc(&cfonc->X,NULL);
	nouvelleDependance->nb_args=vect.dernier;
	nouvelleDependance->numeros.ajoute(-1);
	nouvelleDependance->traits->ou(tr);
	delete tr;
	for (int i=0;i<vect.dernier;i++)
		nouvelleDependance->arg.ajoute(vect[i]);
	AjoutDansINTERMEDIAIRE(nouvelleDependance);
	return nouvelleDependance;
}

int GlobalParseur::MajDependance(VECTA<Noeud*>& vect,
                                  VECTA<ResultatFonction*>& chgts,
								  VECTA<ResultatFonction*>& creees,
                                  VectTFInstruction* vecTF,
                                  Dependance* dependance,
                                  Signature* s,
                                  int idep,
                                  Traits* temporaire) {   

    VECTA<Noeud*> vect_travail(3);
    
    ResultatFonction* nouvelleDependance;
	ResultatFonction* racineDependance;
    Cat* Xs=NULL;
    ExtractionFonction* listeResFonc=NULL;
    int pos;
    int posnoeud;
    Noeud* ancienPremierArg=NULL;
    char deplace=NON;
    int indexChgt=-1;
    ResultatFonction* dependanceChgt=chgts[0];
	typeRegle letype=REGLES_DEDUCTION;
	genereGraphe* g;
	Cat* nomDep=dependance->Fx;
	char creationnoeud=NON;

	racineDependance=dependanceChgt;

	if (vecTF->label!=NULL)
		letype=vecTF->label->type;

	//Si une variable de dependance existe, cela nous permet de determiner le type de la dependance
	//Dans le cas de la generation, nous desactivons la possibilite de modifier ou de detruire une dependance
	//Chaque dependance est alors explosee en une multitude de dependances...
    if (dependance->vardep!=-1) {
        if (chgts[dependance->vardep]==NULL)
            return NON;
        
        nomDep=chgts[dependance->vardep]->Fx;
		//si c'est une destruction ou une modification
        if (dependance->modification==OUI) {
            //Trois cas
            //La negation, une dependance stockee, une dependance fournie...
            //La negation et la nouvelle dependance fournie, le nom de la dependance est
            //celui fourni par la regle. Si aucune dependance n'est fourni, on se contente
            //de la valeur trouvee en chgts[vardep]...
            if (dependance->Fx!=XCatVide) 
                nomDep=dependance->Fx;    
            
            dependanceChgt=chgts[dependance->vardep];
            //On le retire alors de la liste des dependances...
            chgts.affecte(dependance->vardep,NULL);
        }
    }        
    
	genereGraphe* racinegenere=NULL;
	//Dans ce cas et uniquement dans ce cas, il ne s'agit pas d'ecraser
	//la dependance mais de la placer dans un arbre
	if (letype==REGLES_GENERATION) {
		if (nomDep!=XCatNeg) {
			if (racineDependance!=NULL) {
				//Le frere pointe sur son homologue en graphe
				racinegenere=racineDependance->frere;
				if (racinegenere==NULL) {
					g=graphegeneration.recupere(racineDependance);
					graphegeneration.ajoute(g,graphegeneration.dernier());
					racinegenere=g;
				}
			}

			if (dependance->vardep==-1)
				dependanceChgt=NULL;
			else
				if (chgts[dependance->vardep]!=NULL)
					dependanceChgt=chgts[dependance->vardep];
		}
	}
	else //seul un element peut etre marque, les autres sont alors simplement ajoute...
		chgts.affecte(0,NULL); 

    //Dans le cas d'une creation de disjonction de dependance
    if (vecTF->incompatible==OUI && dependance->vardep!=-1) {
        //Il faut alors creer une signature particuliere
        dependanceChgt=chgts[dependance->vardep];        
        //Et on rajoute les bits de la signature globale
        dependanceChgt->compatible.base(*s);
        FONCTIONSDISJOINTES.ajoute(dependanceChgt);
        //Pas la peine d'aller plus loin, on revient...
        return NON;
    }


    //Verification de la signature. La signature est importante dans deux cas specifiques:
    //a) si la dependance doit etre detruite, la signature permet de savoir si la destruction
    //   doit etre complete ou non.
    //b) si la dependance doit etre modifiee, il nous faut la dupliquer uniquement pour cette 
    //     signature.    

    //s'il s'agit d'une modification et que la signature est specialisee
    //alors, la modification ne sera valable que pour la signature
    Signature* nsign=NULL;
    
    if (dependanceChgt!=NULL && 
        dependanceChgt->data!=OUI &&
        s!=NULL && 
        !dependanceChgt->compatible.global(filtreGlobalDependance,*s)) {
        //on retire alors les bits correspondant aux signatures cumulees des autres dependances
        dependanceChgt->compatible.retire(*s);

        //s'il s'agit d'une modification, on cree un duplicat        
        if (nomDep != XCatNeg) {
            //Mais la version courante ne doit plus etre dispo pour cette signature            
            //Donc le filtre de la signature standard doit prendre en compte
            //la nouvelle signature de la dependance
            nsign=new Signature(*s);
            nsign->interdit(dependanceChgt->compatible);
            dependanceChgt=NULL;
        }
        else //dans le cas d'une destruction, on revient. Pas la peine d'aller plus loin.
            return NON;
    }
    
    if (dependanceChgt==NULL) {
        //Cas particulier, ou la dependance est la dependance vide...
        //Il s'agit simplement d'une mise a jour de traits dans une 
        //dependance ou une structure...
        if (nomDep==XCatVide || nomDep==XCatNeg)
            return NON;            
        else
            nouvelleDependance=FournirResFonc(nomDep,NULL);

        //Cas particulier, s'il s'agit d'une disjonction de dependance
        if (vecTF->incompatible==OUI)
            FONCTIONSDISJOINTES.ajoute(nouvelleDependance);       

        //On rajoute les bits de la signature globale pour la creation d'un nouvel objet
        if (nsign!=NULL) {
            nouvelleDependance->compatible.remplace(*nsign);            
            delete nsign;
        }
        else
            if (s!=NULL)
                nouvelleDependance->compatible.base(*s);                
            
        
        
        nouvelleDependance->nb_args=dependance->nb_args;
        if (dependance->traits!=NULL)
            (nouvelleDependance->traits)->ou(dependance->traits);
        
        //dans le cas ou certains traits auraient remontes
        //on les ajoute.
		if (temporaire!=NULL)
            (nouvelleDependance->traits)->ou(temporaire);
        
        nouvelleDependance->numeros.ajoute(vecTF->identificateur);
    }
    else {    
        //Attention, s'il s'agit d'une dependance de base de donnees, on ne fait rien, il est
        //interdit de la detuire
        if (dependanceChgt->data==OUI)
            return NON;

        //S'il y a changement,Attention desormais, nouvelleDependance pointe sur dependanceChgt...
        nouvelleDependance=dependanceChgt; 
		Xs=dependanceChgt->Fx;
		if (dependance->nb_args!=0 || nomDep==XCatNeg || nomDep!=Xs) {			
			indexChgt=nouvelleDependance->indexcategorie;

			if (Xs==NULL) {
				if (TRACE || TRACEDEPENDANCE) {
					trace<<CHAINES[220]<<":";
					VlabelVecteur[vecTF->numero]->aff(trace);
					trace<<Endl;
				}
				return NON;
			}

			listeResFonc=INTERFONCTIONS[(dependanceChgt->Fx)->val];


			//Nettoyage de l'element
			if (listeResFonc==NULL)
				return NON;

			if (TRACE || TRACEDEPENDANCE) {
				trace<<CHAINES[6]<<"[";
				VlabelVecteur[vecTF->numero]->aff(trace);
				trace<<"]: ";
				nouvelleDependance->Affiche(trace,NON);
				trace<<Endl;
			}


			//Nettoyage dans la liste indexee sur le premier noeud...
			posnoeud=(dependanceChgt->arg[0])->num_noeud;
			PREMIERFONCTIONS[posnoeud]->affecte(nouvelleDependance->indexpremier,NULL);

			if (dependanceChgt->arg[1]) {
				posnoeud=(dependanceChgt->arg[1])->num_noeud;
				//Nettoyage dans la liste indexee sur le second noeud...
				SECONDFONCTIONS[posnoeud]->affecte(nouvelleDependance->indexsecond,NULL);
			}
			//Nettoyage general
			if (listeResFonc!=NULL)
				listeResFonc->affecte(indexChgt,NULL);

			pos=(nouvelleDependance->Fx)->val;
			NombreDependances.affecte(pos,NombreDependances[pos]-1);

			if (NombreDependances[pos]==0)
				ClefDependances ^= (nouvelleDependance->Fx)->Val;

			//destruction complete de l'element
			if (nomDep == XCatNeg) {
				INTERFONCTIONS[0]->affecte(nouvelleDependance->index,NULL);
				//Dans le cas d'une generation, on doit aussi retirer le genereGraphe element de son enclos
				if (nouvelleDependance->frere!=NULL) {
					if (nouvelleDependance->frere->retire()==OUI) {
						if (nouvelleDependance->frere->enfants.size()==0)
							delete nouvelleDependance->frere;
					}
					nouvelleDependance->frere=NULL;
				}

				RendreResFonc(nouvelleDependance);
				//Fin des travaux...
				return NON;
			}

			//Changement de nom de la dependance, cela implique que tous les indexes doivent
			//etre redefinis.
			if (nomDep!= XCatVide && Xs != nomDep) {
				deplace=OUI;
				//On nettoie les traits specifiques de cette dependance
				nouvelleDependance->traits->etnon(Fonctions[Xs->index]->traits);
			}
			else
				ancienPremierArg=nouvelleDependance->arg[0];        

			//On remet a jour les arguments de la dependance, uniquement si des noeuds ont ete precises
			//dans la regle
			if (dependance->nb_args!=0)
				nouvelleDependance->arg.raz();
		}
	}

	//On initialise les noeuds
	int numbisarg;
	Liste* ldup;
	for (int j=0; j<dependance->nb_args;j++) {
		//S'il s'agit de la duplication d'un noeud
		if (dependance->arg[j]>=DUPLIQUE)  {
			creationnoeud=OUI;
			//Nous recuperons d'abord le noeud a dupliquer la ou il est
			numbisarg=dependance->arg[j] - DUPLIQUE;
			Noeud* ndup=vect[numbisarg];
			if (ndup==NULL)
				return NON;
			ldup=DupliqueNoeud(ndup->source);
			InstancieTraits(ldup->traitsactif,dependance->noeuds[j]->traitsbase);
			InstancieTraits(ldup->traitsbase,dependance->noeuds[j]->traitsactif);
			nouvelleDependance->arg.ajoute(&ldup->N);
		}
		else {
			//S'il s'agit de la creation d'un nouveau noeud fictif
			if (dependance->arg[j]>=NOUVEAU)  {
				creationnoeud=OUI;
				vect_travail.affecte(j,nouvelleDependance->arg[j]);
				nouvelleDependance->arg.ajoute(&dependance->noeuds[j]->N);
				
				numbisarg=-100;
				//On peut avoir une variable associee pour la duree de la creation
				//seulement: ##Noun#2
				if (dependance->arg[j] > NOUVEAU) {
					numbisarg=dependance->arg[j] - NOUVEAU;
					//Si cette variable a deja ete utilisee avant, c'est une erreur
					if (vect[numbisarg]!=NULL)
						return NON;
					vect.affecte(numbisarg,&dependance->noeuds[j]->N);
				}

				//Cas particulier dans lequel nous creons un noeud fictif
				//dont le lemme ou la forme de surface provient d'une variable
				//string.
				Liste* lfic=dependance->noeuds[j];
				if (lfic->traitsbase->varlemme!=NULL ||
					lfic->traitsbase->varsurface!=NULL) {
						//alors dans ce cas on cree une instance nouvelle pour ce noeud
						//Tout d'abord on recupere la chaine stockee dans la variable
						string mot;
						char err_mot=NON;
						if (lfic->traitsbase->varlemme!=NULL)
							lfic->traitsbase->varlemme->TraitementFormuleString(this,vect,err_mot,mot,&chgts);
						else
							lfic->traitsbase->varsurface->TraitementFormuleString(this,vect,err_mot,mot,&chgts);

						if (mot.size()) {
							ldup=CreeNoeudFragment((char*)mot.c_str(),lfic->traitsbase,lfic->X);
							NoeudsDependance.ajoute(ldup);

							//On remplace notre noeud par ce dernier                    
							nouvelleDependance->arg.affecte(nouvelleDependance->arg.dernier-1,&ldup->N);
							//Dans ce cas, notre nouveau noeud doit aussi etre place a disposition pour les autres dependences qui voudraient l'utiliser
							if (numbisarg!=-100)
								vect.affecte(numbisarg,&ldup->N);
							lfic=NULL;
						}
				}

				//On marque ce noeud avec une valeur particuliere pour ne pas l'integrer plusieurs fois
				//dans NoeudsDependance (dont nous nous servirons plus tard pour remettre 
				//a jour les noeuds fictifs)...
				//Dans le cas d'une regle de generation, on duplique une version differente du noeud
				if (letype==REGLES_GENERATION && lfic!=NULL) {
					ldup=DupliqueNoeud(lfic);
					InstancieTraits(ldup->traitsactif,dependance->noeuds[j]->traitsbase);
					InstancieTraits(ldup->traitsbase,dependance->noeuds[j]->traitsactif);
					ldup->N.fictif=OUI;
					ldup->N.droite=-10;					
					NoeudsDependance.ajoute(ldup);
					//On remplace notre noeud par ce dernier                    
					nouvelleDependance->arg.affecte(nouvelleDependance->arg.dernier-1,&ldup->N);
					//Dans ce cas, notre nouveau noeud doit aussi etre place a disposition pour les autres dependences qui voudraient l'utiliser
					if (numbisarg!=-100)
						vect.affecte(numbisarg,&ldup->N);
					lfic=NULL;
				}
				if (lfic!=NULL && lfic->N.fictif==OUI && lfic->N.droite==-100) {   
					lfic->N.droite=-10;
					NoeudsDependance.ajoute(lfic);
					NoeudsTemporaires.affecte(dernierNumeroNoeud,lfic);
					lfic->N.num_noeud=dernierNumeroNoeud++;                
				}            
			}
			else {
				if (vect[dependance->arg[j]]==NULL) {
					//Premier cas, on vient de le creer, on rend celui que l'on vient 
					//d'allouer
					if (dependanceChgt==NULL)
						RendreResFonc(nouvelleDependance);
					else {
						//Sinon, on remet la dependance a jour...
						for (int k=0;k<j;k++)
							nouvelleDependance->arg.affecte(k,vect_travail[k]);
					}
					//erreur on retourne. L'un des arguments est manquant
					return NON;
				}

				//On sauvegarde le resultat dans le cas d'une erreur
				//pour retrouver la forme initiale...
				vect_travail.affecte(j,nouvelleDependance->arg[j]);
				nouvelleDependance->arg.ajoute(vect[dependance->arg[j]]);
			}
		}
	}
    
    //Une dependance a une certaine duree de vie..
    nouvelleDependance->duree=dependance->duree;
	//Dans le cas ou cette dependance a zero argument, il s'agit d'une reutilisation d'une dependance...
	if (dependance->nb_args!=0)
		nouvelleDependance->nb_args=dependance->nb_args;
    
    //Initialisation des nouvelles relations
    if (dependanceChgt!=NULL) {        
        
        //Si on est parvenu jusque la, c'est que l'on doit deplacer cette 
        //dependance apres moult modifications...
        if (deplace==OUI) {
            //Modification d'une dependance...
            if (TRACE || TRACEDEPENDANCE) {
                trace<<CHAINES[5]<<"[";
                VlabelVecteur[vecTF->numero]->aff(trace);
                trace<<"]: ";
                trace<<Xs->nom<<" -> "<<nomDep->nom<<"  --> ";
                if (listeResFonc!=NULL)
                    dependanceChgt->Affiche(trace,NON);
                trace<<Endl;
            }                
            
            dependanceChgt->Fx=nomDep;
        }
        else {
            nouvelleDependance->bon=MODIFIEE;
            dependanceModifiee=OUI;
        }
        
        
        nouvelleDependance->numeros.ajoute(vecTF->identificateur);
        //(nouvelleDependance->traits)->raz();
        int finstack=nouvelleDependance->stack.dernier;
        int debutstack=0;
        if (dependance->clearStack<0) {
            debutstack=finstack+dependance->clearStack;
            if (debutstack<0)
                debutstack=0;
        }
        
        for (int df=debutstack;df<finstack;df++) {
            char* rr=nouvelleDependance->stack[df];
            if (rr!=NULL)
                libere(rr);
        }


        nouvelleDependance->stack.dernier=debutstack;
        
        InstancieTraits(nouvelleDependance->traits,dependance->traits);
        
        //dans le cas ou certains traits auraient remontes
        //on les ajoute.
        if (temporaire!=NULL)
            (nouvelleDependance->traits)->ou(temporaire);
    }
    else {        
        //Creation d'une nouvelle dependance (dependanceChgt == NULL)
        nouvelleDependance->bon=UTILISE;
    }
    
    for (int ist=0;ist<dependance->stack.dernier;ist++) {
        char* chaine=dependance->stack[ist];
        if (chaine[0]=='~') {
            chaine++;
            for (int irt=0;irt<nouvelleDependance->stack.dernier;irt++)
                if (!strcmp(nouvelleDependance->stack[irt],chaine)) {
                    char* ret=nouvelleDependance->stack.retireElement(irt);
                    if (ret!=NULL)
                        libere(ret);
                    irt--;
                }
        }
        else {
            if (chaine[0]=='\"')
                nouvelleDependance->stack.ajoute(DupliqueChaine(chaine+1));
            else {
                string valstr;
                if (XipGetVariableValue(iParseurCourant,chaine+1,valstr)==NON)
                    nouvelleDependance->stack.ajoute(DupliqueChaine(chaine+1));
                else
                    nouvelleDependance->stack.ajoute(DupliqueChaine(valstr.c_str()));
            }
        }
    }
    

	if (letype==REGLES_GENERATION) {

		//Dans le cas d'une dependance non token ou stoken
		//On verifie qu'il n'y a pas de duplication
		if (nomDep!=Xcreationtoken && nomDep!=Xsurfacetoken && creationnoeud==NON) {
			for (int i=0;i<FONCTIONSINTERMEDIAIRES.dernier;i++)
				if (egalArg(FONCTIONSINTERMEDIAIRES[i],nouvelleDependance)) {
					RendreResFonc(nouvelleDependance);
					return NON;
				}
		}

		//Rajout pour traiter la generation.
        nouvelleDependance->traits->ou(Fonctions[nouvelleDependance->Fx->index]->traits);
		//S'il s'agit d'une dependance toute neuve
		if (dependance->nb_args!=0 || dependance->modification==OUI) {
			FONCTIONSINTERMEDIAIRES.ajoute(nouvelleDependance);

			if (reentrance ==OUI) {
				if (nouvelleDependance->bon!=MODIFIEE)
					InjecteFONCTION(nouvelleDependance);
			}
		}
		
		//Dans le cas ou aucune racine n'a ete fourni, nous sommes
		//forcement dans la situation d'une racine SOUSDEPENDANCE.
		int p=0;
		if (racinegenere==NULL) {
			//Nous avons deux cas, soit il s'agit d'une toute nouvelle dependance
			//et on doit la placer a l'etage superieur. Soit, il s'agit d'une
			//dependance existante et on ne doit surtout pas la bouger.
			g=nouvelleDependance->frere;
			if (g==NULL) {
				g=graphegeneration.recupere(nouvelleDependance);
				p=TestPositionElement(nouvelleDependance->traits->valeur->vecteur,graphegeneration.dernier());
				while (p>=0) {
					graphegeneration.ajoute(g,p);
					if (TestSimpleOrdreNoeuds(vecTF->couche,graphegeneration.enfants))
						break;
					g->parent->enfants.retireElement(p);
					p--;
				}
				creees.ajoute(nouvelleDependance);
			}
			racinegenere=g;
		}
		else {
			if (!parentde(racinegenere->noeud,nouvelleDependance)) {
				g=racinegenere->recupere(nouvelleDependance);
				p=TestPositionElement(nouvelleDependance->traits->valeur->vecteur,racinegenere->dernier());

				while(p>=0) {
					racinegenere->ajoute(g,p);
					if (TestSimpleOrdreNoeuds(vecTF->couche,racinegenere->enfants))
						break;
					g->parent->enfants.retireElement(p);
					p--;
				}
				creees.ajoute(nouvelleDependance);
			}
			else
				return OUI;
		}
		
		//Dans ce cas nous avons une erreur que nous devons nettoyer
		if (p<0) {
			if (g->parentancien!=NULL) {
				g->parentancien->enfants.insertion(g->posancien,g);
				g->pos=g->posancien;
				g->parentancien=NULL;
				g->posancien=-1;	
			}			
			return OUI;
		}

		int i;
		//Cas particulier, on construit un sous-arbre
		if (dependance->Type()==SOUSDEPENDANCE) {
			//Nous explorons tous les sous-noeuds
			SousDependance* sousdep=(SousDependance*)dependance;
			//nous rappelons alors recursivement notre methode, avec dependance en tant que 
			//dependanceChgt
			VECTA<ResultatFonction*> chgtslocal;
			chgtslocal.affecte(chgts);

			for (i=0;i<sousdep->sousnoeuds.dernier;i++) {
				chgtslocal.affecte(0,nouvelleDependance);
				//en cas d'erreur, nous arretons tout
				if (MajDependance(vect,chgtslocal,creees,vecTF,sousdep->sousnoeuds[i],s,i,temporaire)==OUI)
					return OUI;
			}
		}
		//On initialise alors les traits first et last
		if (racinegenere->enfants.dernier) {
			for (i=0;i<racinegenere->enfants.dernier;i++) {
				RetireDebut(racinegenere->enfants[i]->noeud->traits);
				RetireFin(racinegenere->enfants[i]->noeud->traits);
				racinegenere->enfants[i]->pos=i;
			}
			MetDebut(racinegenere->enfants[0]->noeud->traits);
			MetFin(racinegenere->enfants.fin()->noeud->traits);
		}
	}
	else
		AjoutDansINTERMEDIAIRE(nouvelleDependance);

	return NON;
}

//---------------------------------------------------------------------------------------
void GlobalParseur::AjoutDansINTERMEDIAIRE(ResultatFonction* r) {
    for (int i=0;i<FONCTIONSINTERMEDIAIRES.dernier;i++)
        if (egalArg(FONCTIONSINTERMEDIAIRES[i],r)) {
            RendreResFonc(r);
            return;
        }
        
        FONCTIONSINTERMEDIAIRES.ajoute(r);
        r->traits->ou(Fonctions[r->Fx->index]->traits);
        
        if (reentrance ==OUI) {
            if (r->bon!=MODIFIEE)
                InjecteFONCTION(r);
        }
}
//---------------------------------------------------------------------------------------
void GlobalParseur::MajINTERFONCTIONS() {
    int i;
    ResultatFonction* r;
	
    if (reentrance == NON) {
        int boucle=FONCTIONSINTERMEDIAIRES.dernier;
        for (i=0;i<boucle;i++)
            InjecteFONCTION(FONCTIONSINTERMEDIAIRES[i]);
        FONCTIONSINTERMEDIAIRES.raz();
    }
    else {
        if (dependanceModifiee==OUI) {
            int boucle=FONCTIONSINTERMEDIAIRES.dernier;
            for (i=0;i<boucle;i++) {
                r=FONCTIONSINTERMEDIAIRES.retire();
                if (r->bon==MODIFIEE) {
                    r->bon=2;
                    InjecteFONCTION(r);
                }
            }
        }
        else
            FONCTIONSINTERMEDIAIRES.raz();
        dependanceModifiee=NON;
    }

    //Le premier element (qui peut etre NULL est le representant de la classe de disjonction
    //qui permet de singulariser les dependances disjointes
    ResultatFonction* rdisj=FONCTIONSDISJOINTES[0];
    //Dans le cas ou l'on a construit des disjonctions et qu'il existe un nom 
    //de graphe, on traduit ce nom en ResultatFonction
    //Si des dependances ont ete placees dans une disjonction, 
    //nous creons alors leur signature specifique
    for (i=1;i<FONCTIONSDISJOINTES.dernier;i++) {
        FONCTIONSDISJOINTES[i]->compatible.initialise(CompteurIncompatible,i-1,
            FONCTIONSDISJOINTES.dernier-1);        
        if (rdisj!=NULL)
            rdisj->compatible.fusion(FONCTIONSDISJOINTES[i]->compatible);
    }
    
    if (rdisj!=NULL) {
        //On vire alors les bits de la signature du filtre... Cette "dependance"
        //doit etre compatible avec toutes les dependances issues de cette disjonction
        for (i=0;i<rdisj->compatible.signature.dernier;i++) {
            TVecteur vt=~rdisj->compatible.filtre[i];
            rdisj->compatible.filtre.affecte(i,vt);
            rdisj->compatible.signature.affecte(i,0);
        }
    }
    
    if (FONCTIONSDISJOINTES.dernier)
        FiltreDependance(FONCTIONSDISJOINTES.dernier);    
    FONCTIONSDISJOINTES.raz();
}

void GlobalParseur::InjecteFONCTION(ResultatFonction* nouvelleDependance) {
        
    int pos=(nouvelleDependance->Fx)->val;
    
    NombreDependances.affecte(pos,NombreDependances[pos]+1);

    ClefDependances|=(nouvelleDependance->Fx)->Val;
    TraitsDependances->ou(nouvelleDependance->traits);    

	if (FONCTIONSKIF!=NULL)
		FONCTIONSKIF->ajoute(nouvelleDependance);

    if (INTERFONCTIONS[pos]==NULL)
        INTERFONCTIONS.affecte(pos,new ExtractionFonction(3));        
    INTERFONCTIONS[pos]->ajoute(nouvelleDependance);
    nouvelleDependance->indexcategorie=INTERFONCTIONS[pos]->dernier-1;
    
    //Liste globale des dependances...
    //N'est rajoutee qu'une fois...
    if (nouvelleDependance->index==-1) {
        if (INTERFONCTIONS[0]==NULL)
            INTERFONCTIONS.affecte(0,new ExtractionFonction(3));        
        INTERFONCTIONS[0]->ajoute(nouvelleDependance);                    
        nouvelleDependance->index=INTERFONCTIONS[0]->dernier-1;
    }
    
    //Indexation sur le premier noeud...
	if (nouvelleDependance->arg[0]!=NULL) {
		pos=(nouvelleDependance->arg[0])->num_noeud;
		//On verifie qu'il ne s'agit pas d'un noeud fictif...
		if (pos>=0) {
			if (PREMIERFONCTIONS[pos]==NULL)
				PREMIERFONCTIONS.affecte(pos,new ExtractionFonction(3));        
			PREMIERFONCTIONS[pos]->ajoute(nouvelleDependance);                
			nouvelleDependance->indexpremier=PREMIERFONCTIONS[pos]->dernier-1;
		}
	}

    //Indexation sur le deuxieme noeud...
	if (nouvelleDependance->arg[1]!=NULL) {
		pos=(nouvelleDependance->arg[1])->num_noeud;
		//On verifie qu'il ne s'agit pas d'un noeud fictif...
		if (pos>=0) {
			if (SECONDFONCTIONS[pos]==NULL)
				SECONDFONCTIONS.affecte(pos,new ExtractionFonction(3));        
			SECONDFONCTIONS[pos]->ajoute(nouvelleDependance);                
			nouvelleDependance->indexsecond=SECONDFONCTIONS[pos]->dernier-1;
		}
	}
}
//---------------------------------------------------------------------------------------
//Test pour verifier l'ajout d'une nouvelle fonction syntaxique
ResultatFonction* TestFonctionDependance::TestPourDeduction(GlobalParseur* parseur,
                                                            ExtractionFonction* ef,                                                            
                                                            VECTA<Noeud*>& vectbase,
                                                            VECTA<Traits*>& vecttr,
                                                            Cat* fx,
                                                            int& bouc,
                                                            int& resfonc_modifie,
                                                            Signature& verif_compatible) {
    
    ResultatFonction* r;
    char argu;
    VECTA<Noeud*> vect(10);
    
    //On recherche d'abord la premiere fonction qui porte la meme dependance
    //Les solutions sont triees sur val...
    while (bouc< ef->dernier) {
        r=ef->vecteur[bouc];
    
		if (r==NULL) {
			bouc++;
			continue;
		}

		if (r->nb_args!=arg.dernier) {
			if (pointeur==NON || arg.dernier)  {
				bouc++;
				continue;
			}
		}

        //Si l'element a ete marque comme detruit on ne l'envisage pas...
        if ((r->bon==BLOQUEE && negation==NON) || 
			(fx->val!=0 && fx!=r->Fx) ||
			typecat!=r->Fx->type || 
			!verif_compatible.compare(r) ) {
            bouc++;
            continue;
        }

        
        //Meme categorie fonction syntaxique
        if (parseur->TestTraitsFonction(traitsFonction,r->traits,stack,clearStack,r->stack)) {
            //Les argus de arg correspondent a une position dans
            //vect (il s'agit du numero de la variable)
            argu=NON;
            int i=0;
            Traits* trregle;
            vect.raz();
            vect.affecte(vectbase);
            
            for  (i =0; i<r->arg.dernier;i++) {
                //notre noeud courant:
                Noeud* n=r->arg[i];         
                Traits* trregle=traits[i];
                
                //si arg[i]==-1, c'est une valeur de type ? dans la
                //liste d'argument du test
                if (arg[i] != -1) {
                    Noeud* vectn=vect[arg[i]];
                    
                    //On teste alors, si le noeud correspond, 
                    //sachant que arg[i] correspond a un noeud dans vect
                    //vect peut ne pas etre encore initialisee
                    if (vectn!=NULL && vectn!=n)
                        //ce ne sont pas les memes noeuds: erreur
                        break;
                    
                    //On enregistre alors la position (pour des contraintes supplementaire, si l'on veut
                    //imposer la meme variable au sein de la dependance)
                    if (vectn==NULL)
                        vect.affecte(arg[i],n);
                    
                    //argument permet de rappeler que cette fonction de test
                    //comprend des arguments (#1,#2 etc...)
                    argu=OUI;                                            
                }
                
                
                //s'il y a des traits associes a l'argument, on teste:
                if (trregle != NULL) {
                    //Pas de traits sur le noeud, erreur
                    if (n->lesTraits() == NULL)
                        break;
                    //erreur sur les traits...
                    if (!ITR(parseur,trregle,(n->source)->traitsbase,NONECRIT,n))
                        break;
                }
            }
            
            //si i>=r->arg.dernier, c'est que tous les arguments ont 
            //ete passes en revue, donc ca marche...
            if (i>=r->arg.dernier) {
                Traits* tr=NULL;
                
                //Tout d'abord, on range en position 0 les traits provenant des dependances...
                if (traitsFonction!=NULL && traitsFonction->libre!=NULL) {
                    tr=new Traits(r->traits);
                    parseur->InstancieTraits(tr,traitsFonction);
                    (tr->libre)->et(traitsFonction->libre);
                    vecttr.ajoute(tr);
                }

				//Dans le cas d'une fonction dont on veut modifier les traits
				if (traitsFonction!=NULL && pointeur==1)
					parseur->InstancieTraits(r->traits,traitsFonction);
                
                //on met a jour les noeuds dans vect                        
                for (i =0; r->arg[i]!=NULL;i++) {
                    trregle=traits[i];
                    
                    int pos=arg[i];
                    vectbase.affecte(pos,r->arg[i]); 
                    //c'est uniquement s'il y a des traits libres
                    //fournis par l'element de la regle de deduction
                    //que l'on effectue cette creation...
                    if (trregle!=NULL && trregle->libre!=NULL) {
                        tr=new Traits((vect[pos]->source)->traitsbase);
                        parseur->InstancieTraits(tr,trregle);
                        (tr->libre)->et(trregle->libre);
                        vecttr.ajoute(tr);
                    }
                }
                
                //On initialise l'index de l'element sur lequel porte le
                //test...
                resfonc_modifie=bouc;
                
                //si le test ne comprend aucun test sur des arguments
                //alors on positionne bouc sur le dernier element de la chaine
                //pour eviter de boucler sur toutes les valeurs possibles
                if (argu==NON)
                    bouc=ef->dernier;
                
                return r;//on renvoie alors le ResultatFonction qui a ete trouvee...
            }
        }
        bouc++;
    }
    return NULL;
}



//-------------------------------------------------------------------------------------
//le resultat a changer a ete detruit en fait... Dans le cas d'un changement
//sur une dependance, tous les tests qui suivent l'element pointe pour ce chgt
//deviennent deterministes
inline char TestDependance(VECTA<ResultatFonction*>& dep,VectTFInstruction* vecTF,char res,char testChg) {
    if (testChg==OUI)
        return NON;
    
    if (dep[0]!=NULL && (dep[0]->Fx==NULL || res==OUI))
        return OUI;
    
    //Est-ce que toutes les variables sont toujours disponibles
    for (int i=0;i<vecTF->variables.dernier;i++)
        if (dep[vecTF->variables[i]]==NULL)
            return OUI;
        
        return NON;
}


//Ce test consiste a verifier que tous les traits de trlocal apparaissent dans trglobal
char TestTraits(GlobalParseur* parseur,Traits* RegTraits) {
    if (RegTraits==NULL)
        return OUI;

	Traits* tr=parseur->TraitsDependances;

    TVecteur* nval=(tr->valeur)->vecteur;
    register short i;
    int iTailleVect=parseur->iTailleVect;
    
        
    TVecteur* rval=(RegTraits->valeur)->vecteur;
    TVecteur* rfil=(RegTraits->filtre)->vecteur;
    TVecteur* rpres=NULL;
    if (RegTraits->present!=NULL)
        rpres=(RegTraits->present)->vecteur;
    TVecteur* nfil= (tr->filtre)->vecteur;


    for (i=0;i<iTailleVect;i++) {                

        if (rpres!=NULL) {            
            if ((*nfil & *rpres) != *rpres)
                return NON;
            rpres++;
        }
        
        if (*rval!=0) {
            TVecteur r=*rfil & *nfil;
            if ((*nval & r) != (*rval & r))
                return NON;
        }

        rval++;
        nval++;
        nfil++;
        rfil++;                
    }
	return OUI;
}

//	return ITR(parseur_courant,trlocal,parseur->TraitsDependances,NONECRIT);

 //   register short i;        
 //   TVecteur* locval=NULL;

 //   if (trlocal->present!=NULL)
 //       locval=trlocal->present->vecteur;
 //   else 
 //       return OUI;

 //   TVecteur* globval=(trglobal->valeur)->vecteur;
 //   int taille=(trlocal->valeur)->TailleVect;
 //   
	//TC(parseur_courant,trglobal);
	//cout<<parseur_courant->Chaine_traits<<endl;
 //   TV(parseur_courant,locval);
	//cout<<parseur_courant->Chaine_traits<<endl;
 //   
 //   char erreur=OUI;    
 //   for (i=0;i<taille;i++) {
 //       if (( *globval & *locval) != *locval)
 //           return NON;
 //       globval++;
 //       locval++;
 //   }
 //   return OUI;
//}


char TestElement(ContexteInference& ci,ListeTestArbre* laliste) {
    
    for (int i=0;i<laliste->liste.dernier;i++) {
        TestFonction* ttf=laliste->liste[i]->test;
        if (ttf==NULL || TestFoncDep(ttf)==NON)
            return OUI;
        
        //Une dependance est associee a une liste de noeuds qui correspond a ses arguments.
        //Chaque dependance est indexee sur son argument 0, soit sur le numero de noeud correspondant
        //a cet argument (num_noeud)...
        //Si le premier argument est deja trouve, on passe alors par la liste indexee
        //sur les noeuds...
        TestFonctionDependance* ttfdep=(TestFonctionDependance*)ttf;
        Noeud* premierNoeud=ci.noeuds[ttfdep->arg[0]];        
        Noeud* secondNoeud=ci.noeuds[ttfdep->arg[1]];        
        
        if (premierNoeud == NULL && secondNoeud==NULL)
            return OUI;
        
        ExtractionFonction* choisir=NULL;    
        
        char trouve;
        int k;
        
        if (premierNoeud!=NULL) {
            choisir=ci.parseur->PREMIERFONCTIONS[premierNoeud->num_noeud];
            if (choisir==NULL || choisir->dernier==0) {
                if (!ttfdep->negation)
                    return NON;            
            }    
            else {
                if (ttfdep->Fx->val!=0) {
                    trouve=NON;
                    for (k=0;k<choisir->dernier;k++) {
                        if (choisir->vecteur[k]!=NULL) {
                            if (choisir->vecteur[k]->Fx==ttfdep->Fx) {
                                trouve=OUI;
                                break;
                            }
                        }
                    }
                    //Cas particulier, on a trouve aucune dependance de ce
                    //type, et ce n'est pas une negation: c'est une erreur
                    if (trouve==NON && ttfdep->negation==NON)
                        return NON;
                }
            }        
        }
        
        if (secondNoeud!=NULL) {
            choisir=ci.parseur->SECONDFONCTIONS[secondNoeud->num_noeud];
            if (choisir==NULL || choisir->dernier==0) {
                if (!ttfdep->negation)
                    return NON;
            }                            
            else {
                if (ttfdep->Fx->val!=0) {
                    trouve=NON;
                    for (k=0;k<choisir->dernier;k++) {
                        if (choisir->vecteur[k]!=NULL) {
                            if (choisir->vecteur[k]->Fx==ttfdep->Fx) {
                                trouve=OUI;
                                break;
                            }
                        }
                    }
                    if (trouve==NON && ttfdep->negation==NON)
                        return NON;
                }
            }        
        }
    }
    return OUI;
}


char ListeTestArbre::testInference(ContexteInference& ci) {
	if (liste.dernier>0) {
		if (!TestTraits(ci.parseur,traitsdependance))
			return NON;
	}
	return OUI;
}


void ListeTestArbre::calculInference(ContexteInference& ci,
                                     VECTA<Traits*>& vecttr,
                                     VECTA<Traits*>& affectation,
                                     size_t i,
                                     char& resultat) {

    Signature cumule(0,0);

#ifdef XIPLIBXML
    ci.parseur->basexmlcourante->XmlCourant=NULL;
#endif

    if (TestTraits(ci.parseur,traitsdependance) &&
        TestElement(ci,this))
        inference(ci,vecttr,affectation,i, cumule,resultat);

    if (resultat==NON && ci.vtf->sinon!=NULL) {
        ci.vtf=ci.vtf->sinon;
        ci.vtf->listes.calculInference(ci,vecttr,affectation,i,resultat);
    }
}




#ifdef XIPLIBXML 

char TestFonctionXML::execute(ContexteInference& ci,
                              int i,
                              ListeTestArbre* lt,
                              Signature& cumule,
                              VECTA<Traits*>& vecttr,
                              VECTA<Traits*>& affectation,
                              char& resultat) {           
    VECTA<uchar*>* lesClefs=NULL;
    clefXML* entree=clefxml;
    char erreur=ERREUREXECUTION;   
    xmlresultat* resxmlnode=new xmlresultat;            
    //Nous avons plusieurs cas:
    //a) L'instruction est une creation de noeud XML, dans ce cas la liste des noeuds XML est vide
    //b) Il s'agit d'un Test, dans ce cas, des qu'on une valeur qui correspond on s'arrete
    //c) Il s'agit d'un autre cas et la liste peut-etre longue, on applique inference pour chaque xmlNode
    
    if (ci.parseur->ExecuteXML(entree,ci.noeuds,ci.chgt,resxmlnode) != negation) {
        if (resxmlnode->dernier()>0) {
            VECTA<uchar*> lesClefs;
            for (int iar=0;iar<resxmlnode->dernier();iar++) {
                char resanalxml=entree->AnalyseClefNodeXML(ci.parseur,(*resxmlnode)[iar],
                    resxmlnode->index,resxmlnode->arboxml,lesClefs,ci.noeuds,ci.chgt);

                //s'il ne s'agit pas d'une boucle, on s'arrete tout de suite en cas de succes
                if (ci.vtf->typeTest!=INSTRUCTION_WHILEXML) {
                    
                    //dans le cas d'un echec, on analyse le noeud suivant
                    if (resanalxml==NON)
                         continue;

                    //sinon, on s'arrete la et on sort
                    delete resxmlnode;
                    resultat=OUI;
                    return PASERREUR;
                }

                char resLocal=NON;
                if (resanalxml!=NON && lt!=NULL)
                    lt->inference(ci,vecttr,affectation,i+1,cumule,resLocal);   
                if (resLocal==OUI)
                    resultat=OUI;
            }                    
            
            delete resxmlnode;
            return RETOURFORCEXML;
        }
        else
            erreur=PASERREUR;
    }
    delete resxmlnode;
    return erreur;
}

#endif



char TestFonctionAffichage::execute(ContexteInference& ci,int i,ListeTestArbre* listetestarbre,Signature& cumule,
                                    VECTA<Traits*>& vecttr,VECTA<Traits*>& affectation,char& resultat) {
    
    TVecteur execution=affiche&EXECUTION;
    TVecteur affichage=affiche&~EXECUTION;   
	ci.parseur->ParametresAffichage=ci.parseur->ParametresAffichageDefini;
    if (affichage) {
        if ( (affichage & AFFICHAGE_STANDARD) == AFFICHAGE_STANDARD) {
            ci.parseur->gAfficherResultats=ci.parseur->gAfficherResultatsDefini;
            ci.parseur->ParametresAffichage=ci.parseur->ParametresAffichageDefini;
            ci.parseur->CalculFonction=ci.parseur->CalculFonctionDefini;
            ci.parseur->DESAMBIGUISATION=ci.parseur->DESAMBIGUISATIONDefini;
            ci.parseur->appliqueGrammaire=ci.parseur->appliqueGrammaireDefini;
            affichage&=~AFFICHAGE_STANDARD;
        }
        if ( (affichage & AFFICHAGE_NUL) == AFFICHAGE_NUL) {
            ci.parseur->gAfficherResultats=NON;
            affichage&=~AFFICHAGE_NUL;
        }
        if (affichage!=0)
            ci.parseur->ParametresAffichage|=affichage;
    }
    switch (execution) {
    case EXECUTE_CHUNK: 
        ci.parseur->CalculFonction=NON;
        break;
    case EXECUTE_NUL:
        ci.parseur->parse_phrase=NON;                
        break;
    case EXECUTE_DEPENDENCY:
        ci.parseur->CalculFonction=OUI;
    }
    return PASERREUR;
}


char TestFonctionMath::execute(ContexteInference& ci,int i,ListeTestArbre* listetestarbre,Signature& cumule,
                               VECTA<Traits*>& vecttr,VECTA<Traits*>& affectation,char& resultat) {

    ci.parseur->initialise_erreur_instruction();
    char err=NON;
    char erreur=PASERREUR;
    if (TraitementFormule(ci.parseur,ci.noeuds,err,&ci.chgt)==negation)
        erreur=ERREUREXECUTION;
    
    if (err==OUI && ci.parseur->TestOptions(AFFICHAGE_ERREUR_EXECUTION))
        ci.parseur->affiche_derniere_erreur(ci.vtf->numero);
    
    //dans le cas d'un goto ou d'un exit
    if (ci.parseur->aller_label!=NULL) {
        resultat=OUI;
        return erreur;
    }

    if (err==NON && erreur==PASERREUR)
        resultat=OUI;
    
    return erreur;
}

char TestFonctionGraphe::execute(ContexteInference& ci,
                                int i,
                                ListeTestArbre* lt,
                                Signature& cumule,
                                VECTA<Traits*>& vecttr,
                                VECTA<Traits*>& affectation,
                                char& resultat) {
    
									
    char err=NON;
    char erreur=PASERREUR;
	ci.parseur->initialise_erreur_instruction();
    if (TraitementFormule(ci.parseur,ci.noeuds,err,&ci.chgt)==negation)
        erreur=ERREUREXECUTION;
    
    if (err==OUI && ci.parseur->TestOptions(AFFICHAGE_ERREUR_EXECUTION))
        ci.parseur->affiche_derniere_erreur(ci.vtf->numero);
    
    if (err==NON && erreur==PASERREUR)
        resultat=OUI;
    
    return erreur;
}

char TestFonctionNoeud::execute(ContexteInference& ci,
                                int i,
                                ListeTestArbre* lt,
                                Signature& cumule,
                                VECTA<Traits*>& vecttr,
                                VECTA<Traits*>& affectation,
                                char& resultat) {
    
    if (TestNoeud(ci.parseur,ci.noeuds,&affectation,&vecttr) != negation)
        return PASERREURNOEUD;
    return ERREURNOEUD;
}

char TestFonction::execute(ContexteInference& ci,int i,ListeTestArbre* lt,Signature& cumule,
                           VECTA<Traits*>& vecttr,VECTA<Traits*>& affectation,char& resultat) { return NON;}

void ListeTestArbre::inference(ContexteInference& ci,
                               VECTA<Traits*>& vecttr,
                               VECTA<Traits*>& affectation,
                               size_t i,
                               Signature& cumule,
                               char& resultat) {
    
    Cat* Xs=NULL;
    char refiltre=NON;
    ExtractionFonction* listeResFonc;
    int bp;
    TestFonction* ttf;
    char resLocal;
    ResultatFonction* r;
	TestFonctionDependance* ttfdep;
        
    appel_inference++;
    
    //On garde une trace du dernier trait enregistre
    int bpt=vecttr.dernier;
    
	//Dans le cas d'un parcours d'un  arbre de generation, on revient des qu'on
	//a consomme tous les fils
	if (ci.parcours!=NULL && i>=ci.parcours->fils.dernier) {
		resultat=OUI;
		//Plusieurs cas
		//Tout d'abord, si nous avons des contextes a analyser a droite
		ttfdep=(TestFonctionDependance*)ci.parcours->fils.fin();		
		if (ci.cisuivant!=NULL) {
			resultat=NON;
			VECTA<Noeud*>* noeuds=NULL;
			VECTA<ResultatFonction*>* chgt=NULL;
			//On garde l'ensemble des noeuds et des fonctions deja acquis
			//Ce qui nous permet de garder les noeuds et fonctions obtenus au fond
			//de la recursion
			if (ci.cisuivant->noeuds.dernier!=0) {
				noeuds=new VECTA<Noeud*>(ci.cisuivant->noeuds.dernier);
				noeuds->affecte(ci.cisuivant->noeuds);
			}
			if (ci.cisuivant->chgt.dernier!=0) {
				chgt=new VECTA<ResultatFonction*>(ci.cisuivant->chgt.dernier);
				chgt->affecte(ci.cisuivant->chgt);
			}
			ci.cisuivant->noeuds.affecte(ci.noeuds);
			ci.cisuivant->chgt.affecte(ci.chgt);
			inference(*ci.cisuivant,vecttr,affectation,ci.cisuivant->idxgen+1,cumule,resultat);
			//On revient a la situation precedente...						
			if (noeuds!=NULL) {
				ci.cisuivant->noeuds.affecte(noeuds);
				delete noeuds;
			}
			else
				ci.cisuivant->noeuds.raz();
			if (chgt!=NULL) {
				ci.cisuivant->chgt.affecte(chgt);
				delete chgt;
			}
			else
				ci.cisuivant->chgt.raz();
		}

		
		if (ttfdep->final!=0) {
			//Si nous avons une negation sur toute l'expression: final=2 sinon il vaut 1
			if (ttfdep->final==resultat+1)
				return;
			//Apres le dernier element analyse selon notre arbre de generation, on continue en sequence
			//Il s'agit de l'element le plus en profondeur a droite
			if (resultat==OUI) {
				ci.parcours=NULL;
				resultat=NON;
				inference(ci,vecttr,affectation,ci.idx+1,cumule,resultat);
			}
		}
		return;
	}

    //si tous les elements ont ete verifies
    //On ajoute une nouvelle fonction
	if (i >= (size_t)liste.dernier && ci.parcours==NULL) {
        int itt;
        
        //N.B.    affectation[0] --> traitsactif
        //        affectation[1] --> traitsbase
        //        affectation[2] --> traits a recopier
               

        //Affectation des traits sur les noeuds soumis a l'AFFECTATION
        if (affectation.dernier>0) {
			for (itt=0;itt<affectation.dernier;itt+=3) {
                if (!ITR(ci.parseur,affectation[itt+2],affectation[itt],NONECRIT,NULL))
                    return;
			}

			////Stabilisation des traits...
			for (itt=0;itt<affectation.dernier;itt+=3) {  
				ci.parseur->InstancieTraits(affectation[itt],affectation[itt+2]);
				ci.parseur->InstancieTraits(affectation[itt+1],affectation[itt+2]);
			}
		}
        
        
        //On verifie que les traits sont compatibles:
        Traits* temporaire=NULL;
        for (itt=0;itt<vecttr.dernier;itt++) {
            if (temporaire==NULL)
                temporaire=new Traits(ci.parseur);
            if (!ci.parseur->PTT(*temporaire,vecttr[itt])) {
                if (ci.parseur->TRACE || ci.parseur->TRACEDEPENDANCE) {
                    ci.parseur->VlabelVecteur[ci.vtf->numero]->aff(trace);
                    trace<<": "<<ci.parseur->CHAINES[3]<<Endl;
                }
                return;
            }
        }

        ResultatFonction* rdisj=NULL;
        if (ci.vtf->incompatible==OUI && ci.parseur->FONCTIONSDISJOINTES.dernier==0) {
            //Alors ci.vtf->nomgraphe devient une dependance que l'on cree a la volee
            if (ci.vtf->nomgraphe!=NULL) {
                rdisj=ci.parseur->FournirResFonc(ci.vtf->nomgraphe,NULL);               
                rdisj->compatible.signature.raz();
                rdisj->compatible.filtre.raz();
                rdisj->numeros.ajoute(ci.vtf->identificateur);
                rdisj->bon=UTILISE;
                ci.parseur->AjoutDansINTERMEDIAIRE(rdisj);
            }
            ci.parseur->FONCTIONSDISJOINTES.ajoute(rdisj);
        }

        r=ci.chgt[0];
        int idep;


		char erreur=NON;
		VECTA<ResultatFonction*> creees;
		for (idep=0;idep<ci.vtf->dependances.dernier;idep++) {
			if (ci.vtf->identificateur==2122)
				cout<<"";

			//Dans le cas d'une generation, il nous faut aussi construire une structure particuliere
			//pour accueillir nos chaines
            erreur=ci.parseur->MajDependance(ci.noeuds,ci.chgt,creees,ci.vtf,ci.vtf->dependances[idep],
				&cumule,idep,temporaire);
			if (erreur==OUI)
				break;
            //ci.chgt.affecte(0,NULL); //seul un element peut etre marque, les autres sont alors simplement ajoute...
            
        }


        ci.chgt.affecte(0,r);
        if (temporaire!=NULL)
            delete temporaire;

		if (erreur==OUI) {
			ci.parseur->DestructionNodes(creees);
			resultat=NON;
			return;
		}
        
        //cas particulier, il s'agit d'un iftest en tete d'analyse
        //dans ce cas nous avons un appel recursif qui doit etre fait
        char ptest=ci.vtf->premiertest;
        if ( (ptest==OUI && (
                ci.vtf->typeTest==INSTRUCTION_IFTEST || 
                ci.vtf->typeTest==INSTRUCTION_WHILE ||             
                ci.vtf->typeTest==INSTRUCTION_WHILEXML)) || 
             (ptest==UTILISECONTEXTE && ci.vtf->typeTest==INSTRUCTION_CONTEXTE))  {
            //dans le cas d'un while, cela nous aide a gerer le retour apres boucle
            //Cela evite aussi les appels recursifs a outrance chaque fois que la methode
            //inference pourrait etre rappelee sur le while
            ci.vtf->premiertest=UTILISE;
			if (ci.instruction_suivante==-1)
				ci.instruction_suivante=ci.vtf->position+1;
            ci.parseur->DeduitFonction(ci.couche,ci.type,OUI,ci.instruction_suivante,&ci.noeuds,&ci.chgt,ci.vtf->finboucle);        
            ci.vtf->premiertest=ptest;
			ci.instruction_suivante=-1;
        }
		
        resultat=OUI;
        return;
    }
    
	if (ci.parcours==NULL)
		ttf= liste[i]->test;    
	else
		ttf=ci.parcours->fils[i];
    
    //Calcul du ou...
    //Dans la sous-liste situee sous le noeud en question
    
    if (ttf==NULL) {
        VECTA<ListeTestArbre*>& sousliste=liste[i]->liste;
        
        //On effectue l'analyse du OU
        for (int il=0;il<sousliste.dernier;il++) {
            resLocal=NON;
            
            if (TestClef(ci.parseur,sousliste[il]->clefdependance) &&
                TestTraits(ci.parseur,sousliste[il]->traitsdependance))
                sousliste[il]->inference(ci,vecttr,affectation,0,cumule,resLocal);
            
            if (resLocal==OUI)
                resultat=OUI;
            //Le premier element de la liste de sous-listes pointe sur le
            //premier element de la sousliste
            //Autrement dit: sousliste[il]->test == sousliste[il]->liste[0]->test
            //Cette affectation est realisee dans ConstruitListe pour accelerer le test sur le
            //determinisme...
            if (resLocal==OUI && liste[i]->deterministe==OUI)
                return;
            
            if (TestDependance(ci.chgt,ci.vtf,resLocal,NON))
                return;
        }
        return;
    }
    
    //Test sur un noeud, et non sur une dependance.
    if (TestFoncDep(ttf)==NON) {
        char nettoie=NON;
        resLocal=NON;
		//Nous gardons la position du dernier element rajoute dans ce tableau
		//ce qui correspond a une modification de affectation dans execute...
		int localaffect=affectation.dernier;
        char erreur=ttf->execute(ci,i,this,cumule,vecttr,affectation,resLocal);        
        if (erreur==RETOURFORCEXML)
            return;

        //cas particulier pour un tfnoeud, le code de retour est 2 en cas
        //de reussite, le code de retour est 3 en cas d'echec...
        if (erreur==PASERREURNOEUD || erreur==ERREURNOEUD) {
            bp=localaffect;
            nettoie=OUI;
            erreur=erreur-2;
        }

        //Dans le cas d'une simple instruction, ce n'est jamais une erreur
        if (ttf->negation==SAUTE) {
            erreur=PASERREUR;
            //dans le cas d'une suite d'instruction, 
            //il n'est pas necessaire d'aller jusqu'au bout
            if (i==liste.dernier-1 && affectation.dernier==0) {
                resultat=OUI;
                return;
            }
        }

        if (erreur==PASERREUR) {
            resLocal=NON;
            inference(ci,vecttr,affectation,i+1,cumule,resLocal);
            if (resLocal==OUI)
                resultat=OUI;
        }
		else {//dans le cas d'un else...
			if (ci.vtf->typeTest==INSTRUCTION_IFTEST && 
				ci.vtf->finboucle!=-1 && 
				ci.vtestboucle!=NULL && 
				ci.vtestboucle->cell(ci.vtf->finboucle)!=NULL &&
				ci.vtestboucle->cell(ci.vtf->finboucle)->typeTest==INSTRUCTION_ELSE) {
				resLocal=NON;
				ci.instruction_suivante=ci.vtf->finboucle+1;
				inference(ci,vecttr,affectation,i+1,cumule,resLocal);
				if (resLocal==OUI)
					resultat=OUI;
			}
		}
        
        if (nettoie==OUI) {
            NettoieTraits(vecttr,bpt);
            //Nettoyage des traits ajoutees en affectation
            NettoieAffectation(affectation,bp);
        }
        
        return;
    }
    
    
    
    //Une dependance est associee a une liste de noeuds qui correspond a ses arguments.
    //Chaque dependance est indexee sur son argument 0, soit sur le numero de noeud correspondant
    //a cet argument (num_noeud)...
    //Si le premier argument est deja trouve, on passe alors par la liste indexee
    //sur les noeuds...
    ttfdep=(TestFonctionDependance*)ttf;
    ExtractionFonction statiqueextract;
	
    //cas particulier, il s'agit d'un pointeur de dependance et nous avons la reference

    if (ttfdep->pointeur==OUI &&
        ttfdep->vardep!=-1 &&
        ci.chgt[ttfdep->vardep]!=NULL) {
        statiqueextract.ajoute(ci.chgt[ttfdep->vardep]);
        listeResFonc=&statiqueextract;
    }
    else {
        Noeud* premierNoeud=ci.noeuds[ttfdep->arg[0]];
        Noeud* secondNoeud=ci.noeuds[ttfdep->arg[1]];
        
        ExtractionFonction* choisirSurNoeud=NULL;    
        ExtractionFonction* choisirPremier=NULL;    
        ExtractionFonction* choisirSecond=NULL;
        ExtractionFonction* choisirSurCategorie=NULL;
        listeResFonc=NULL;
        
        char choisir=NON;
        
        
        //On prend la liste la plus courte...
        if (premierNoeud != NULL) {
            choisirPremier=ci.parseur->PREMIERFONCTIONS[premierNoeud->num_noeud];
            if (choisirPremier==NULL || choisirPremier->dernier==0) {            
                listeResFonc=NULL;
                choisir=BLOQUEE;
            }
            else
                choisir=OUI;                    
        }
        
        if (secondNoeud != NULL && choisir!=BLOQUEE) {
            choisirSecond=ci.parseur->SECONDFONCTIONS[secondNoeud->num_noeud];
            if (choisirSecond==NULL || choisirSecond->dernier==0) {
                listeResFonc=NULL;
                choisir=BLOQUEE;
            }
            else            
                choisir=OUI;
        }
        
        if (choisir==OUI) {
            if (choisirPremier==NULL)
                choisirSurNoeud=choisirSecond;
            else
                if (choisirSecond==NULL)
                    choisirSurNoeud=choisirPremier;
                else
                    if (choisirSecond->dernier<choisirPremier->dernier)
                        choisirSurNoeud=choisirSecond;
                    else
                        choisirSurNoeud=choisirPremier;
                    listeResFonc=choisirSurNoeud;
        }
        
        if (choisir!=BLOQUEE) {
            int valpos;
            //dans le cas ou la variable a deja ete initialise pour son type de dependance...
            if (ttfdep->vardep!=-1 && ci.chgt[ttfdep->vardep]!=NULL)
                valpos=ci.chgt[ttfdep->vardep]->Fx->val;
            else
                valpos=(ttfdep->Fx)->val;
            
            choisirSurCategorie=ci.parseur->INTERFONCTIONS[valpos];
            if (choisirSurCategorie==NULL || choisir==NON)
                listeResFonc=choisirSurCategorie;
            else {//On choisit la plus courte
                if (choisirSurCategorie->dernier < choisirSurNoeud->dernier)
                    listeResFonc=choisirSurCategorie;
                else
                    listeResFonc=choisirSurNoeud;        }
        }
    }
    //Dans le cas ou listeResFonc vide, on verifie sur la negation
    if (listeResFonc==NULL || listeResFonc->dernier==0) {
        if (ttfdep->negation)
            inference(ci,vecttr,affectation,i+1,cumule,resultat);
        return;
    }
    
    
    
    int suivant_i=i;
    char vbon;
    
    //Le noeud sous est le noeud de l'arbre des chunks sous lequel doivent imperativement
    //se trouver les noeuds des dependances dans le test
    ContexteInference ci_travail(ci.parseur,ci.vtf);
    ci_travail.couche=ci.couche;
    ci_travail.type=ci.type;
    
	int maxboucle=listeResFonc->dernier*10;
    for (int bouc=0;bouc<listeResFonc->dernier;bouc++) {        
		//Test sur la reentrance trop genereuse
		if (ci.parseur->reentrance==OUI) {
			maxboucle--;
			if (maxboucle<=0) {
				ci.parseur->erreur_survenue=153;
				ci.parseur->affiche_derniere_erreur(ci.vtf->numero);
				return;
			}
		}

        Signature cumulocal(cumule);    
        
        //On initialise un vecteur de travail, pour conserver 
        //une trace des choix de variables precedents
        ci_travail.affecte(ci);        
        
        int resfonc_modifie=bouc;
        Cat* fx=ttfdep->Fx;

		//Dans le cas d'une variable $1 deja instanciee, si on ne fournit pas la valeur de la dependance		
		if (ttfdep->pointeur==OUI) {
			if (fx==ci.parseur->XCatVide) {
				if (ci.chgt[ttfdep->vardep]!=NULL)
					fx=ci.chgt[ttfdep->vardep]->Fx;
			}
			vbon=(*listeResFonc)[bouc]->bon;
			(*listeResFonc)[bouc]->bon=OUI;
		}
		else {
			if (ttfdep->vardep!=-1 && ci.chgt[ttfdep->vardep]!=NULL) {
				fx=ci.chgt[ttfdep->vardep]->Fx;
				if (fx==NULL)
					return;
			}
		}


        r=ttfdep->TestPourDeduction(ci.parseur,
            listeResFonc,ci_travail.noeuds,vecttr,fx,bouc,
            resfonc_modifie,cumulocal);
        
        char resul;
        if (r!=NULL) {
            if (ttfdep->pointeur==NON)
                vbon=r->bon;
            r->bon=BLOQUEE;
            resul=OUI;
            //On ajoute alors les compatibilites particulieres de cette dependance
            cumulocal.fusion(r->compatible);
        }
        else
            resul=NON;
        
        //Test sur la negation
        if (ttfdep->negation == resul) {            
            NettoieTraits(vecttr,bpt);
            
            if (r!=NULL)
                r->bon=vbon;
            break;
        }
        
        suivant_i=i+1;
        resLocal=NON;
        char varAjoute=NON;
        
        if (ttfdep->negation==NON && ttfdep->vardep!=-1 && ci_travail.chgt[ttfdep->vardep]==NULL) {
            ci_travail.chgt.affecte(ttfdep->vardep,r);
            ci.vtf->variables.ajoute(ttfdep->vardep);
            varAjoute=OUI;
        }


		if (ttfdep->leType==TESTNOEUDGENERATION) {
			//On verifie d'abord la coherence des elements trouves avec les autres
			//elements
			if (r->frere!=NULL) {
				if (ci.pere==NULL || r->frere->parent==ci.pere)					
					if (!i || ci.frere==NULL || ci.frere->pos<r->frere->pos)
						resLocal=OUI;
			}

			if (resLocal==OUI) {				
				//Si nous devons explorer des sous-noeuds
				ci.idxgen=i;
				
				if (ttfdep->fils.dernier) {
					char modifpere=NON;
					//Alors on effectue en priorite cette analyse
					ci_travail.parcours=ttfdep;
					ci_travail.pere=r->frere;
					resLocal=NON;
					//On garde la trace du precedent contexte que
					//l'on utilisera pour analyser le reste des freres
					ci_travail.cisuivant=&ci;
					//On garde pere et frere pour traiter les elements suivants
					//Dans le cas de la racine de notre expression, ces elements
					//peuvent etre vides
					if (ci.pere==NULL) {
						ci.pere=r->frere->parent;
						ci.frere=r->frere;
						modifpere=OUI;
					}
					inference(ci_travail,vecttr,affectation,0,cumule,resLocal);
					if (modifpere==OUI) {
						ci.pere=NULL;
						ci.frere=NULL;
					}
				}
				else {
					//Sinon, on enchaine avec les freres
					ci_travail.parcours=ci.parcours;
					//Si c'est le premier, son pere devient leur pere a tous
					ci_travail.pere=r->frere->parent;
					//On garde aussi son frere
					ci_travail.frere=r->frere;
					resLocal=NON;
					inference(ci_travail,vecttr,affectation,i+1,cumule,resLocal);
				}

				ci_travail.pere=NULL;
				ci_travail.frere=NULL;
				ci_travail.parcours=NULL;
			}
		}
		else {
			if (ttfdep->changement==OUI) {
				//On reinitialise le resultat pour cet objet en particulier
				// afin de traiter correctement les cas de determinisme dans le
				//traitement des OU... En effet, lorsqu'une modification a ete
				//reclamee sur une dependance et qu'un certain ensemble de conditions
				//a ete rempli, on ne veut pas continuer sur la lancee d'une suite
				//d'interpretation d'autres conditions reunies sous un OU. Cette
				//decision doit etre locale a l'analyse du changement en question...
				ci_travail.chgt.affecte(0,r);

				inference(ci_travail,vecttr,affectation,suivant_i,cumulocal,resLocal);
				//dans ce cas precis, si le changement porte sur une fonction sans argument,
				//on reinitialise bouc avec sa valeur... En effet, dans le cas d'une fonction
				//sans argument, on place le curseur directement sur le dernier element de facon
				//a ne pas boucler inutilement...
				bouc=resfonc_modifie;
			}
			else
				inference(ci_travail,vecttr,affectation,suivant_i,cumulocal,resLocal);
		}

        if (varAjoute==OUI)
            ci.vtf->variables.retire();
        
        if (resLocal==OUI)
            resultat=OUI;
        
        NettoieTraits(vecttr,bpt);
        
        if (r!=NULL && r->bon==BLOQUEE)
            r->bon=vbon;
        
        //Il s'agit d'une operation deterministe (une seule solution envisagee)
        //On s'arrete
		if (resultat==OUI && ci.parcours==NULL && liste[i]->deterministe==OUI)
            return;
        
        //le resultat a changer a ete detruit en fait... Dans le cas d'un changement
        //sur une dependance, tous les tests qui suivent l'element pointe pour ce chgt
        //deviennent deterministes
        if (TestDependance(ci_travail.chgt,ci.vtf,resultat,ttfdep->changement))
            return;
    }
}




ListeTestArbre::ListeTestArbre(TestFonction* tf) {
	RAMASSE(ListeTestArbre);
    clefdependance=0;
    traitsdependance=NULL;
    test=tf;
    deterministe=NON;
    if (tf!=NULL && TestFoncDep(tf)) {
        TestFonctionDependance* ttf= (TestFonctionDependance*)tf;
        if (ttf!=NULL && ttf->negation==NON)            
            clefdependance = (ttf->Fx)->Val;        
    }        
}

ListeTestArbre::~ListeTestArbre() {
	JETTE(ListeTestArbre);
    if (traitsdependance!=NULL)
        delete traitsdependance;
}

void TestArbre::AjouteEnQueue(ListeTestArbre* pere,VECTA<ListeTestArbre*>& listes, ListeTestArbre* nouveau) {
    if (listes.fin()==nouveau)
        return;
    
    if (listes.dernier==0 || listes.fin()->test!=NULL) {
        listes.ajoute(nouveau);
        pere->clefdependance|=nouveau->clefdependance;
        if (nouveau->clefdependance!=0) {
            Traits* tr=((TestFonctionDependance*)test)->traitsFonction;
            if (tr!=NULL) {
                if (pere->traitsdependance==NULL)
                    pere->traitsdependance=new Traits(tr);
                else
                    pere->traitsdependance->ou(tr);
            }
        }
        return;
    }
    
    VECTA<ListeTestArbre*>& liste=listes.fin()->liste;
    for (int i=0;i<liste.dernier;i++) 
        AjouteEnQueue(liste[i],liste[i]->liste,nouveau);
}

void GlobalParseur::StockeElementRegles(ListeTestArbre* nouveau) {
    if (memoiresem.indicateur==NON)
        TousLesElementsRegles.ajoute(nouveau);
    else
        memoiresem.nouveauxElementsDeRegles.ajoute(nouveau);    
}

void TestArbre::ConstruitListes(GlobalParseur* parseur,
                                int courant,
                                ListeTestArbre* pere,
                                VECTA<ListeTestArbre*>& listes,
                                char& sorte,
                                char deter) {
    int i;
    
    
    //Trois cas possibles:
    //l'operateur est un &
    //l'operateur est un |
    //l'operateur est un @
    
    
    //dans le cas du |, c'est plus complique
    if (operateur=='|') {
        VECTA<ListeTestArbre*>* branche;
        
        ListeTestArbre* fin=new ListeTestArbre;
        parseur->StockeElementRegles(fin);
        
        
        AjouteEnQueue(pere,listes,fin);
        
        //Tous les noeuds inferieurs sont rajoutes au meme niveau...
        for (i=0;i<arbre.dernier;i++) {
            //On place au sommet de cette nouvelle liste, le premier TestFonction
            //du premier element de la liste sur laquelle pointe l'element...
            //De cette facon, on peut tester plus rapidement le determinisme
            ListeTestArbre* coupure=new ListeTestArbre(arbre[i]->test);
            parseur->StockeElementRegles(coupure);
            fin->liste.ajoute(coupure);
            //si le niveau superieur est de type deterministe
            fin->deterministe=deterministe;
            branche=&coupure->liste;
            
            
            //On analyse donc les noeuds inferieurs du ou
            //On cree une nouvelle branche
            arbre[i]->ConstruitListes(parseur,0,coupure,*branche,sorte,deterministe);
            
            //La liste se continue aussi sur les peres...
            //            for (int prof=profondeur-1;prof>=0;prof--) {
            //             int prof=profondeur-1;
            //             if (peres[prof]!=NULL && iperes[prof] < peres[prof]->arbre.dernier)
            //                 peres[prof]->ConstruitListes(parseur,prof,peres,iperes,iperes[prof],*branche,deterministe);            
        }
    }
    else { 
        char analyse=OUI;
        if (operateur=='@') {
            if (test==NULL && arbre.dernier>0)
                arbre[0]->ConstruitListes(parseur,0,pere,listes,sorte,deter);
            else {
                if (test->Type()!=TESTFONCTIONMATHCHAINE &&
                    test->Type()!=TESTFONCTIONMATH)
                    sorte=OUI;
                //On ajoute cet element...
                ListeTestArbre* nouveau=new ListeTestArbre(test);
                parseur->StockeElementRegles(nouveau);
                //Si le dernier element est une disjonction, il faut arroser tous les elements en-dessous
                AjouteEnQueue(pere,listes,nouveau);
            }
        }
        else
            //dans ce cas on recopie la liste des elements dans l'arbre
            if (operateur=='&') { 
                for (i=courant;i<arbre.dernier;i++)
                    if (arbre[i]->operateur=='|')
                        arbre[i]->ConstruitListes(parseur,i+1,pere,listes,sorte,deterministe);
                    else {
                        //Le premier ne doit pas porter la marque du determinisme
                        int deterministeSurSuivant= (i && deterministe);
                        arbre[i]->ConstruitListes(parseur,0,pere,listes,sorte,deterministeSurSuivant);
                    }
            }    
    }
}


//cette methode permet de verifier si le test n'est reduit qu'a un seul
//element ou est une suite sous un meme '&' sans disjonction
char TestArbre::LineariseDirect(GlobalParseur* parseur,TVecteur& clef,ListeTestArbre& listes) {
    
    static VECTA<TestArbre*> peres(100);
    static VECTA<int> iperes(100);
    
    peres.raz();
    iperes.raz();
    char sorte=2;

    ConstruitListes(parseur,0,&listes,listes.liste,sorte,deterministe);
    if (listes.liste.dernier>0)
        listes.test=listes.liste[0]->test;
    
    if (operateur=='|')
        return NON;
    
    clef=0;
    if (listes.liste.dernier==0)
        return OUI;
    
    VECTA<ListeTestArbre*>& ar=listes.liste;
    for (int i=0;i<ar.dernier;i++) {
        if (ar[i]->test!=NULL && TestFoncDep(ar[i]->test)) {
            TestFonctionDependance* ttf= (TestFonctionDependance*)ar[i]->test;
            if (ttf!=NULL && ttf->negation==NON)
                clef |= (ttf->Fx)->Val;
        }
    }
    
    return sorte;
}




char ListeTestArbre::Test(GlobalParseur* parseur,
                          VECTA<Noeud*>& vect_noeuds) {

#ifdef XIPLIBXML
    xmlNodePtr xc=parseur->basexmlcourante->XmlCourant;
    parseur->basexmlcourante->XmlCourant=NULL;
#endif
    //Dans le cas ou l'expression ne comprend pas de disjonction
    int ret=TestFormule(parseur,vect_noeuds,0);
#ifdef XIPLIBXML
    parseur->basexmlcourante->XmlCourant=xc;
#endif
    return ret;
}





char VectTFInstruction::AppliqueInference(GlobalParseur* parseur,
                                         typeDeduction type_deduction,
                                         int lacouche,
                                         VECTA<Noeud*>& noeuds) {
    char resultat;

    
    EtatArbre* larbre=NULL;
    EtatArbre* nega=NULL;
    
    if (negation!=NULL)
        nega=negation->regle;
    
    if (regle!=NULL)
        larbre=regle->regle;
    
    if (nega!=NULL) {            
        resultat=NON;
        char val_deterministe=deterministe;
        deterministe=CALCULNEGATION;
		parseur->initIteration();
        nega->ResolutionFictif(parseur,this,resultat,type_deduction,lacouche,noeuds);
        deterministe=val_deterministe;
        if (resultat==NON)
            return NON;

        
        //sinon, on s'essaie a quelques lancements d'instruction associee
        if (typeTest!=INSTRUCTION_DETERMINISTE &&
            typeTest!=INSTRUCTION_DEPENDANCE &&
            larbre ==NULL) {            
            
            if (listes.liste.dernier>0) {
                //il s'agit d'un iftest
                //on commence donc a cette regle en particulier
                //Mais on evite de lancer l'analyse de nouveau
                Fonction* fonc_regle=regle;
                regle=NULL;
                parseur->DeduitFonction(lacouche,type_deduction,OUI,position,&noeuds);
                regle=fonc_regle;
            }
            else {
                //il s'agit d'un iftest
                //on commence donc a cette regle en particulier
                //Mais on evite de lancer l'analyse de nouveau
                parseur->DeduitFonction(lacouche,type_deduction,OUI,position+1,&noeuds);
            }
            return OUI;
        }
    }
    
    resultat=NON;
	if (larbre!=NULL) {
		parseur->initIteration();
        larbre->ResolutionFictif(parseur,this,resultat,type_deduction,lacouche,noeuds);
	}
    return resultat;
}


//Il s'agit d'une methode qui permet de boucler autour d'un faisceau d'instructions
double GlobalParseur::AppliqueBoucle(TestFonctionMathBoucle* test,
                                  int laCouche,
                                  typeDeduction type_deduction,
                                  int i,
                                  VECTA<Noeud*>& noeuds,
                                  VECTA<ResultatFonction*>* vfoncs) {    

    char erreur=NON;
    VECTA<VectTestFonction*>* vtestboucle;
    if (TestOptions(AFFICHAGE_ERREUR_EXECUTION)) {            
        switch(type_deduction) {
        case DEDUCTIONENCHARGEANT:
            vtestboucle=&ChgtDeductions;
            break;
        case DEDUCTIONINITIALE:
            vtestboucle=&DebutDeductions;
            break;
		case DEDUCTIONPREBALISE:            
			vtestboucle=&PreBaliseDeductions;
			break;
		case DEDUCTIONPOSTBALISE:            
			vtestboucle=&PostBaliseDeductions;
			break;
        case DEDUCTIONFINALE:
            if (DeductionFinale==NON)
                return OUI;
            vtestboucle=&FinDeductions;
            break;
        case DEDUCTIONCOURANTE:    
            vtestboucle=&ListeDesDeductions[laCouche];
            //listeEtatArbre.demarque();
            break;
		case DEDUCTIONGENERATION:
			vtestboucle=&Generations[laCouche];
			break;
        case DEDUCTIONCOUCHE:
            vtestboucle=&ListeDesDeductionsEnCouche[laCouche];
            break;
        case DEDUCTIONFILTRE:
            vtestboucle=&TableDesFiltresDeCouche[laCouche];
            break;
        case DEDUCTIONPROCEDURE:
            vtestboucle=(VECTA<VectTestFonction*>*)&ListeDesProcedures;
        }            
    }

    double res=OUI;
    double retour=i+1;
    //On initialise le premier element
    initialise_erreur_instruction();
    test->formule->formules[0]->TraitementFormule(this,noeuds,erreur,vfoncs);
    if (erreur==OUI) {
        if (TestOptions(AFFICHAGE_ERREUR_EXECUTION))
            affiche_derniere_erreur((*vtestboucle)[i]->numero);
        return NON;
    }
    do {
        //on lance notre boucle
        initialise_erreur_instruction();
        res=test->formule->formules[1]->TraitementFormule(this,noeuds,erreur,vfoncs);        
        if (erreur==OUI) {
            if (TestOptions(AFFICHAGE_ERREUR_EXECUTION))
                affiche_derniere_erreur((*vtestboucle)[i]->numero);
            return NON;
        }
        if (res==OUI) {
            //On lance l'analyse sur les instructions suivantes
            retour=DeduitFonction(laCouche,type_deduction,OUI,i+1,&noeuds,vfoncs);
            //On incremente alors la variable
            initialise_erreur_instruction();
            test->formule->formules[2]->TraitementFormule(this,noeuds,erreur,vfoncs);
            if (erreur==OUI) {
                if (TestOptions(AFFICHAGE_ERREUR_EXECUTION))
                    affiche_derniere_erreur((*vtestboucle)[i]->numero);
                return NON;
            }
            //Dans le cas d'un stop, d'un return ou d'un exit, on sort
            if (aller_label!=NULL)  
                return retour;
        }
    }
    while (res==OUI);
    
    
    return retour;
}

inline char TestListesBoucle(GlobalParseur* parseur,VectTFInstruction* vtfi) {
	if (TestClef(parseur,vtfi->clef) &&
		(parseur->ClefArbre & vtfi->regle->clef) == vtfi->regle->clef &&
		TestTraits(parseur,vtfi->listes.traitsdependance))
		return OUI;
	return NON;
}


//Cette methode permet d'extraire de nouvelles fonctions syntaxiques
//si un certain contexte de fonctions deja extraites est verifie..
double GlobalParseur::DeduitFonction(int laCouche,
                                     typeDeduction type_deduction,
                                     char appel_recursif,
                                     int demarre,
                                     VECTA<Noeud*>* vnoeuds,
                                     VECTA<ResultatFonction*>* vfoncs,
                                     int retour) {
    ContexteInference ci(this);
    char res;
    char resultat=NON;    
    double resboucle;
	char arret_analyse=NON;
	char testreglenum=OUI;
    VectTestFonction* boucle;
    VECTA<VectTestFonction*>* vtestboucle;
    switch(type_deduction) {
    case DEDUCTIONQUEUE:
        if (DeductionFinale==NON)
            return OUI;
        vtestboucle=&QueueDeductions;
		testreglenum=NON;
        break;
    case DEDUCTIONFINALE:
        if (DeductionFinale==NON)
            return OUI;
        vtestboucle=&FinDeductions;
        break;
    case DEDUCTIONENCHARGEANT:
        vtestboucle=&ChgtDeductions;
        break;
    case DEDUCTIONINITIALE:
        vtestboucle=&DebutDeductions;
        break;
	case DEDUCTIONPREBALISE:            
		vtestboucle=&PreBaliseDeductions;
		break;
	case DEDUCTIONPOSTBALISE:            
		vtestboucle=&PostBaliseDeductions;
		break;
    case DEDUCTIONCOURANTE:    
        vtestboucle=&ListeDesDeductions[laCouche];
        //listeEtatArbre.demarque();
        break;
    case DEDUCTIONCOUCHE:
        vtestboucle=&ListeDesDeductionsEnCouche[laCouche];
        break;
    case DEDUCTIONGENERATION:
        vtestboucle=&Generations[laCouche];
        break;
    case DEDUCTIONFILTRE:
        vtestboucle=&TableDesFiltresDeCouche[laCouche];
        break;
    case DEDUCTIONPROCEDURE:
        vtestboucle=(VECTA<VectTestFonction*>*)&ListeDesProcedures;
    }
        
    if (vtestboucle==NULL || vtestboucle->dernier==0)
        return NON;


    int premier=demarre;
    if (premier==-1)
        premier=demarre_regle;

    //On met a jour notre tableau de noeuds
    if (vnoeuds!=NULL)
        ci.noeuds.affecte(*vnoeuds);

    if (vfoncs!=NULL)
        ci.chgt.affecte(*vfoncs);

    ci.type=type_deduction;
    ci.couche=laCouche;
	ci.vtestboucle=vtestboucle;

    VectTFInstruction* vtfi;
    VectTFUneInstruction* vtfune;
	aller_label=NULL;
	demarre_regle=0;
	demarre_couche=-1;

	VECTA<Traits*> vecttr(0);
	VECTA<Traits*> affectation(0);


    for (int i=premier;i<vtestboucle->dernier;i++) {

		if (xipExecutionEnd())
			break;

        boucle=vtestboucle->cell(i);
		reglecourante=boucle;
		if (!testEspaceRegle(boucle))
			continue;

        if (testreglenum && boucle->activation(this)==NON) {                    
            //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
            //commencer a reappliquer nos regles
            demarre_regle=0;
			if (label_stop!=NULL)
				demarre_couche=label_stop->couche;
            aller_label=label_stop;
            resultat=NON;
            break;
        }
    
        vtfi=NULL;
        vtfune=NULL;
        TVecteur lazone=MoinsUn;
        if (boucle->Type()==OUI) {
            vtfi=(VectTFInstruction*)boucle;
            lazone=vtfi->zones;
        }
        else
            if (boucle->Type()==2) {
                vtfune=(VectTFUneInstruction*)boucle;
                lazone=vtfune->zones;
            }
        
        if (CalculFonction==NON && 
            (boucle->typeTest==INSTRUCTION_DEPENDANCE ||
            boucle->typeTest==INSTRUCTION_DETERMINISTE ))
            continue;

        aller_label=NULL;
        demarre_regle=0;
        demarre_couche=-1;
        resultat=NON;


        if (TRACE || TRACEDEPENDANCE) {            
            if (VlabelVecteur[boucle->numero]->type==REGLES_DEDUCTION)
                trace<<CHAINES[235];
            else
                trace<<CHAINES[234];
            
            VlabelVecteur[boucle->numero]->aff(trace);
            trace<<Endl<<Endl;
        }
        
        if ((lazone & Zonage) != 0) {
            
            if (boucle->regle_contexte==NON) {
                
                switch(boucle->typeTest) {
                case INSTRUCTION_ELSE:
                case INSTRUCTION_FINWHILE:
                    i=boucle->finboucle; //sortie de boucle (else du iftest ou while)
                    continue;
                case INSTRUCTION_FOR:                    
                    resboucle=AppliqueBoucle((TestFonctionMathBoucle*)vtfi->listes.liste[0]->test,
                        laCouche,type_deduction,i,ci.noeuds,&ci.chgt);                        
                    i=boucle->finboucle;
                    if (aller_label!=NULL) {                                                
                        //s'il s'agit d'un stop on arrete l'analyse a cet endroit
                        if (aller_label==label_stop || 
                            aller_label==label_retour)                    
                            return resboucle;
                        
                        //s'il s'agit d'un exit on arrete tout
                        //S'il s'agit d'une regle en couche, on sort immediatement, le saut sera effectue
                        //dans GlobalParseur::ActiveCouche
                        //si nous sommes dans un appel recursif, il faut d'abord en sortir
                        if (aller_label==label_terminal || 
                            type_deduction==DEDUCTIONCOUCHE || 
                            appel_recursif==OUI)
                            return OUI;
                        
                        //sinon if faut verifier si le label correspond a une deduction courante hors couche
                        if (aller_label->couche!=-1 || aller_label->type!=REGLES_DEDUCTION) {
                            //dans ce cas il n'y pas de saut
                            if (TRACE)
                                trace<<"Warning: "<<aller_label->etiquette<<" is not attached to a non layered deduction rule."<<endl;
                        }
                        else 
                            //sinon on saute a la regle dont le numero correspond a aller_label->index
                            //sachant qu'il va etre incremente au passage dans le 'for'
                            i=aller_label->index-1;
                    }
                    continue;
                case INSTRUCTION_FINIFTEST: //on ne fait que la sauter
                    continue;
                case INSTRUCTION_RETOUR:
                case INSTRUCTION_FINFOR://sortie de FOR ou de IFTEST
                    return OUI;
                }
                
                
                
                if (vnoeuds!=NULL)                    
                    ci.vtf=(VectTFInstruction*)boucle;                
                else
                    ci.initialise(this,(VectTFInstruction*)boucle,vtestboucle);
                
                res=NON;
                

                //dans le cas ou l'expression ne comprend pas de disjonction
                if (vtfi!=NULL) {
                    if (TestClef(this,vtfi->clef)) {
                        if (vtfi->listes.liste.dernier>0)
                            vtfi->listes.calculInference(ci,vecttr,affectation,0,res);
                        else
                            res=OUI;
                    }
                }
                else {
                    if (vtfune!=NULL) {
                        Signature cumule(0,0);
                        vtfune->test->execute(ci,0,NULL,cumule,vecttr,affectation,res);                    
                    }
                    else
                        res=OUI;
                }

                vecttr.nettoie();                     
                if (res==OUI) {                    
                    resultat=OUI;

                    //s'il s'agit d'un return, on renvoie la valeur dans valeur_retour
                    if (aller_label==label_retour) {                    
                        //On remet alors a jour notre tableau de variables
                        if (vnoeuds!=NULL)
                            vnoeuds->affecte(ci.noeuds);
                        return valeur_retour;
                    }

                    if (boucle->premiertest==UTILISECONTEXTE)
                        return resultat;

                    //On relance alors l'analyse a partir de l'instruction suivante
                    //dans le cas d'un iftest de facon a garder le tableau des noeuds pour
                    //les autres instructions
                    if (boucle->premiertest==OUI && boucle->finboucle!=-1) {                            
                        i=boucle->finboucle;
                        if (boucle->typeTest!=INSTRUCTION_WHILE) {
                            //si le test contient un else, il nous faut aller au-dela
                            if (vtestboucle->cell(i)->finboucle!=-1)
                                i=vtestboucle->cell(i)->finboucle;                            
                        }
                    }
                }
                else {
                    //dans le cas d'un iftest, en cas d'echec, on saute apres le endif ou le "else"
                    //le "i" pointe sur le "else", il sera incremente
                    if (boucle->finboucle!=-1)  { 
                        i=boucle->finboucle;//a la prochaine iteration pour pointer sur la premiere instruction
                        //s'il n'y a pas de else mais l'accolade fermante du iftest
                        switch((*vtestboucle)[i]->typeTest) {
                        case INSTRUCTION_RETOUR:
                            //Un cas particulier: le while
                        case INSTRUCTION_FINWHILE://fin boucle du while
                            if (appel_recursif==OUI) {
                                if (retour==i)
                                    return OUI; //on revient de notre recursion
                            }
                        }
                    }
                }
            }
            else {
				initIteration();
                if (TestListesBoucle(this,vtfi) &&
					boucle->AppliqueInference(this,type_deduction,laCouche,ci.noeuds)==OUI)
                    resultat=OUI;
                //s'il y a un iftest attache, on saute a l'instruction suivante apres l'accolade fermante
                if (boucle->finboucle!=-1) {
                    //Le retour du iftest est toujours present sous la forme d'une accolade
                    i=boucle->finboucle;
                    //s'il s'agit d'un else, il nous faut aller au-dela
                    if (vtestboucle->cell(i)->finboucle!=-1)
                        i=vtestboucle->cell(i)->finboucle;
                }
            }
        }

        MajINTERFONCTIONS();

        //Dans le cas d'un filtre sur couche
        if (resultat==OUI) {                        
            
            arret_analyse=incrementestats(boucle);
            if (type_deduction==DEDUCTIONFILTRE)
                return OUI;
            
            //Dans le cas d'un GOTO ou d'un exit
            if (aller_label!=NULL) {


                //s'il s'agit d'un stop on arrete l'analyse a cet endroit
                if (aller_label==label_stop) {                    
                    //On remet alors a jour notre tableau de variables
                    if (vnoeuds!=NULL)
                        vnoeuds->affecte(ci.noeuds);
                    return valeur_retour;
                }
                    

                //s'il s'agit d'un return, on renvoie la valeur dans valeur_retour
                if (aller_label==label_retour) {                    
                    //On remet alors a jour notre tableau de variables
                    if (vnoeuds!=NULL)
                        vnoeuds->affecte(ci.noeuds);
                    return valeur_retour;
                }
                
                //s'il s'agit d'un exit on arrete tout
                if (aller_label==label_terminal)
                    return OUI;
                
                //S'il s'agit d'une regle en couche, on sort immediatement, le saut sera effectue
                //dans GlobalParseur::ActiveCouche
                if (type_deduction==DEDUCTIONCOUCHE)
                    return OUI;

                //si nous sommes dans un appel recursif, il faut d'abord en sortir
                if (appel_recursif==OUI)
                    return OUI;
                
                //sinon if faut verifier si le label correspond a une deduction courante hors couche
                if (aller_label->couche!=-1 || aller_label->type!=REGLES_DEDUCTION) {
                    //dans ce cas il n'y pas de saut
                    if (TRACE)
                        trace<<"Warning: "<<aller_label->etiquette<<" is not attached to a non layered deduction rule."<<endl;
                }
                else 
                    //sinon on saute a la regle dont le numero correspond a aller_label->index
                    //sachant qu'il va etre incremente au passage dans le 'for'
                    i=aller_label->index-1;
            }
        }

        //Dans le cas ou l'on demande un arret de l'analyse, on sort
        if (parse_phrase==NON)
            return NON;
    }
    return resultat;
}

Dependance::Dependance(Cat* x) : arg(3) {
	RAMASSE(Dependance);
    Fx=x;
    duree=0;
    nb_args=0;
    traits=NULL;
    clearStack=NON;
    vardep=-1;
    modification=NON;
}

SousDependance::SousDependance(Dependance* d) : Dependance(d->Fx) {
	recopie(d);
}

void SousDependance::recopie(Dependance* d) {
    duree=d->duree;
    nb_args=d->nb_args;
    traits=d->traits;
    clearStack=d->clearStack;
    vardep=d->vardep;
    modification=d->modification;
	int i;
	for (i=0;i<d->stack.dernier;i++)
		stack.ajoute(DupliqueChaine(d->stack[i]));
	for (i=0;i<d->noeuds.dernier;i++)
		noeuds.ajoute(d->noeuds[i]);
	for (i=0;i<d->arg.dernier;i++)
		arg.ajoute(d->arg[i]);	
	delete d;
}

Dependance::~Dependance() {
    int i;
	JETTE(Dependance);
    for (i=0;i<stack.dernier;i++)
        libere(stack[i]);
/*
    for (i=0;i<noeuds.dernier;i++) {
        if (noeuds[i]!=NULL) {
            delete noeuds[i]->N.fils->source;
            if (noeuds[i]->voc!=NULL)
                delete noeuds[i]->voc;
            delete noeuds[i];
        }
    }
*/
}

Fonction::Fonction(Cat* x) : SeqFonc(x), marque(3) {
    
	nbvariables=0;
    categorie=-1;
    regle=NULL;    
    detruireArbre=OUI;
    nb_args=0;
    clef=0;
    traits=NULL;
    deterministe=NON;
    zones=MoinsUn;
    detruire=OUI;
    negation=NON;
    listes=NULL;
}

Fonction::Fonction(Fonction *f) : SeqFonc(f->Fx),marque(f->marque.dernier) {
    
    int i;
    nbvariables=0;
    arg.ajuste(f->arg.dernier+1);
    marque.ajuste(f->arg.dernier+1);
    for (i=0;i<f->arg.dernier;i++) {
        arg.ajoute(f->arg[i]);
        marque.ajoute(f->marque[i]);
    }
    
    
    detruire=NON;
    detruireArbre=OUI;
    negation=f->negation;
    listes=f->listes;
    categorie=f->categorie;
    
    zones=f->zones;
    nb_args=f->nb_args;
    clef=0;
    traits=f->traits;
    regle=f->regle;    

    deterministe=f->deterministe;
}

Fonction::~Fonction() {
	JETTE(SeqFonc);
}

SeqFonc::~SeqFonc() {
    
    if (detruire==OUI) {  
        if (listes!=NULL)
            delete listes;
    }
}

void Fonction::affiche(GlobalParseur* parseur,ostream& os) {
    
    if (Fx!=NULL)
        os<<Fx->nom;
    if (traits!=NULL)
        os<<ConversionChaine(parseur,TC(parseur,traits));
    
    os<<" = ";
    

}

void Sequence::affiche(GlobalParseur* parseur,ostream& os) {
    
    if (Fx!=NULL)
        os<<Fx->nom;
    if (traits!=NULL)
        os<<ConversionChaine(parseur,TC(parseur,traits));
    
    os<<" = ";
}
/*-------------------------------------------------------------------------*/

inline int PreTestNoeud(GlobalParseur* parseur,EtatArbre* et,int increment,Liste* L) {
    int i;
    
    if (L==NULL) {
        if (!et->rechercheCheminEpsilon(parseur->XCatEpsilon))
            return NON;
        return OUI;
    }

    Noeud* n=&L->N;
    //S'il y a un plus d'un arc, on ne peut rien tester
    if (et->arcs.dernier==0 || et->type==1 || et->etatfin!=0)        
        return OUI;   

    if (increment==1 && n->nombre<et->nombre)
        return NON;

    char retour=NON;
    for (i=0;i<et->arcs.dernier;i++) {
        if (parseur->CompareNoeudSousArbre(et->arcs[i],n)==OUI)
            return OUI;
    }
    
    return NON;
}

//-------------------------------------------------------------------------
char Sequence::TestUnContexte(GlobalParseur* parseur,
                              LListe& Base,
                              size_t ind,
                              int incr, 
                              char type,
                              int& dernierElement,
                              VECTA<Noeud*>* noeuds_arguments) {
    
    if (Fx==parseur->contexteFoncVide)
        if (Base[ind]==NULL)
            return !negation;
        else
            return negation;
        
        char solution=-1;
        
        if (regle==NULL)
            return NON;
        
        Liste* L=Base[ind];
        Liste* Ls=Base[ind+incr];

        if (L!=NULL && incr==1 && regle->arcs.dernier==1) {
            if ((L->N.clef_suivant & regle->arcs[0]->clefsuivante) != regle->arcs[0]->clefsuivante)
                return negation;
        }

        if (!PreTestNoeud(parseur,regle,incr,L) )
            return negation;
        
        ResultatSequence element(incr);      
		parseur->initIteration();
        regle->AppliqueLineaire(parseur,solution,Base,ind,incr,type,element,-1,noeuds_arguments);
        //Cas tres particulier, ou l'etat initial est un etat final
        if (solution==NON && regle->etatfin!=0)
            solution=OUI;
        dernierElement=element.fin;
        return abs(solution-negation);
}


char Sequence::TestContexte(GlobalParseur* parseur,
                            LListe& Base, 
                            size_t ipremier,
                            size_t idernier,
                            VECTA<Noeud*>& noeuds_arguments) {
    
	borneG=-1;
	borneD=-1;
    if (gauche!=NULL) {
        if (gauche->TestUnContexte(parseur,Base,ipremier,-1,NON,borneG,&noeuds_arguments)==NON) {
            if (parseur->TRACE) 
                trace<<parseur->CHAINES[205]<<Endl;
            return NON;
        }
    }
    
    if (droit!=NULL) {
        if (droit->TestUnContexte(parseur,Base,idernier,1,NON,borneD,&noeuds_arguments)==NON) {
            if (parseur->TRACE)
                trace<<parseur->CHAINES[206]<<Endl;
            return NON;
        }
    }    
    return OUI;
}

char Sequence::TestContexteGauche(GlobalParseur* parseur,
                            LListe& Base, 
                            size_t ipremier,
                            VECTA<Noeud*>& noeuds_arguments) {
    
	borneG=-1;
    if (gauche!=NULL) {
        if (gauche->TestUnContexte(parseur,Base,ipremier,-1,NON,borneG,&noeuds_arguments)==NON) {
            if (parseur->TRACE) 
                trace<<parseur->CHAINES[205]<<Endl;
            return NON;
        }
    }
    
    return OUI;
}

char Sequence::TestContexteDroit(GlobalParseur* parseur,
							LListe& Base, 
							size_t idernier,
							VECTA<Noeud*>& noeuds_arguments) {

	borneD=-1;    
    if (droit!=NULL) {
        if (droit->TestUnContexte(parseur,Base,idernier,1,NON,borneD,&noeuds_arguments)==NON) {
            if (parseur->TRACE)
                trace<<parseur->CHAINES[206]<<Endl;
            return NON;
        }
    }    
    return OUI;
}

							
char Sequence::TestExpressionBooleen(GlobalParseur* parseur,VECTA<Noeud*>& vect_noeuds) {
    if (listes!=NULL)
        return listes->Test(parseur,vect_noeuds);
    return OUI;
}

char Sequence::TestContexteSimple(GlobalParseur* parseur,
                                  LListe& Base, 
                                  size_t ipremier,
                                  size_t idernier,
                                  VECTA<Noeud*>& noeuds_arguments) {
    
	borneG=-1;
	borneD=-1;
 
	if (gauche!=NULL) {
        if (gauche->TestUnContexte(parseur,Base,ipremier,-1,NON,borneG,&noeuds_arguments)==NON) {
            if (parseur->TRACE) 
                trace<<parseur->CHAINES[205]<<Endl;
            return NON;
        }
    }
    
    if (droit!=NULL) {
        if (droit->TestUnContexte(parseur,Base,idernier,1,NON,borneD,&noeuds_arguments)==NON) {
            if (parseur->TRACE)
                trace<<parseur->CHAINES[206]<<Endl;
            return NON;
        }
    }    
    return OUI;
}

void SeqFonc::ConstruitClef() {
    clef=0;
    nombre=0;
}


void Sequence::ConstruitClef(Cat* epsilon) {
    clef=0;
    nombre=0;

    if (regle!=NULL) {
        EtatArbre ea;
        ea.fils=regle;
        ea.ConstruitClef(epsilon,nombre);
        clef=ea.clef;
    }
}


void Fonction::ConstruitClef(Cat* epsilon) {
    clef=0;
    nombre=0;
    
    EtatArbre ea;
    ea.fils=regle;
    ea.ConstruitClef(epsilon,nombre);        
    clef=ea.clef;
}



//Cette methode est utilisee pour les regles de sequence...
char Sequence::TestMatching(GlobalParseur* parseur,LListe& Base,size_t iListe,
                            ResultatSequence& element,
                            VECTA<Noeud*>& noeuds_arguments,
                            VECTA<char>& majCategorie) {
    
    VECTA<int> noeuds_reperes;
	element.debut=iListe;
    char chemin=-1;
    //noeuds_arguments.raz();
    //majCategorie.raz();
    element.increment=increment;

	parseur->initIteration();
    regle->AppliqueLineaire(parseur,chemin,
        Base,iListe,increment,
        OUI,
        element,
		iListe,
        &noeuds_arguments,
		&noeuds_reperes,
        &majCategorie,
		this);
    if (parseur->testIteration() && chemin && element.bon==OUI && element.fin !=-1) {
        if (increment==-1) {
            int debutE=element.debut; //on inverse les positions dans la chaine...
            element.debut=element.fin;
            element.fin=debutE;
        }
        return OUI;
    }
    
    return NON;
}

char Sequence::MatchTokens(GlobalParseur* parseur,
						   LListe& Phrase,
						   size_t iListe,
						   ResultatSequence& element,
						   VECTA<Noeud*>& noeuds_arguments,
						   VECTA<char>& majCategorie) {

    Liste* L=Phrase[iListe];
    
	if ((clef & L->N.clef_suivant) != clef)
		return NON;

	if (!TestContexteGauche(parseur,Phrase,iListe-1,noeuds_arguments))
		return NON;

	if (!PreTestNoeud(parseur,regle,1,L))
		return NON;

	if (regle->arcs[0]->X!=parseur->XCatEpsilon && regle->arcs.dernier==1) {
		EtatArbre* suivant=regle->arcs[0]->etatarbre;
		if (suivant->arcs.dernier==1 && suivant->arcs[0]->X!=parseur->XCatEpsilon) {        
			if (!PreTestNoeud(parseur,suivant,1,Phrase[iListe+increment]))
				return NON;
		}
	}

	element.initialise(parseur,Fx,traits);
	if (TestMatching(parseur,Phrase,iListe,element,noeuds_arguments,majCategorie)) 
		return OUI;            
	return NON;
}

//--------------------------------------------------------------------------
char VectSequence::Match(GlobalParseur* parseur,
                         ResultatSequence& element,
                         LListe& Phrase,size_t iListe,char lexical,int& ifonc,
                         VECTA<Noeud*>& noeuds_arguments,
                         VECTA<char>& majCategorie) {

    Liste* L=Phrase[iListe];
    
    for (;ifonc<dernier;ifonc++) {     
        Sequence* me=vecteur[ifonc];
        if (me==NULL)
            break;

		if (!parseur->testEspaceRegle(me) || me->activationmin(parseur)==NON)
			break;

        if (me->increment==1 && (me->clef & L->N.clef_suivant) != me->clef)
            continue;

		noeuds_arguments.raz();
		if (me->increment==1) {
			if (!me->TestContexteGauche(parseur,Phrase,iListe-1,noeuds_arguments))
				continue;
		}
		else {
			if (!me->TestContexteDroit(parseur,Phrase,iListe+1,noeuds_arguments))
				continue;
		}

        if (!PreTestNoeud(parseur,me->regle,me->increment,L))
            continue;
      
        if (me->regle->arcs[0]->X!=parseur->XCatEpsilon && me->regle->arcs.dernier==1) {
            EtatArbre* suivant=me->regle->arcs[0]->etatarbre;
            if (suivant->arcs.dernier==1 && suivant->arcs[0]->X!=parseur->XCatEpsilon) {        
                if (!PreTestNoeud(parseur,suivant,me->increment,Phrase[iListe+me->increment]))
                    continue;
            }
        }
        
        element.initialise(parseur,me->Fx,me->traits);
		majCategorie.raz();
		me->L=NULL;
        if (me->TestMatching(parseur,Phrase,iListe,element,noeuds_arguments,majCategorie)) 
            return OUI;            
    }

    return NON;
}


/*-------------------------------------------------------------------------*/
//Cette methode permet une reanalyse de l'arbre des chunks pour construire une
//suite d'index sur l'arbre lui-meme. Ainsi, tous les noeuds de l'arbre sont conserves dans
//une table (IndexParCat) indexe sur la categorie presente sur ce noeud. On dispose alors, par exemple,
//d'une table ou la liste des NP est disponible a travers la position correspondante dans la
//table des NP.
TVecteur Noeud::ConstruitIndex(GlobalParseur* parseur,Noeud* lePere,char ajouteNoeudLex,int lahauteur) {
    Noeud* n=this;
    Noeud* ledernier=this;
    TVecteur laClef=0;

	if (lahauteur==0) {
		parseur->ClefArbre=0;
		parseur->noeudsaplat.raz();
	}

    source->instanciateur=NULL;
    
    appel_construit_index++;
    
	
    //si c'est un mot (une feuille dans l'arbre), on ne s'en occupe pas...
    if (fils==NULL)
        return X->Val;
    
    while (n!=NULL) {       
		parseur->noeudsaplat.ajoute(n);

        n->modifie=0;
        n->pere=lePere;
        
        if (n->source!=NULL) {
            n->hauteur=lahauteur;
            (n->source)->indexCat=parseur->IndexParCat[(n->X)->val].dernier;
            
            if (ajouteNoeudLex==TOUTECATEGORIE && n->source->voc!=NULL && 
                (lePere==NULL || lePere->X == &(parseur->Categories[1])->X) ) {
                //Dans ce cas, chaque possibilite correspond a une entree
                TVecteur Val=0;                
                for (int i=0;i<n->source->voc->nbX;i++) {
                    Cat* x=n->source->voc->X[i];
                    if ((Val & x->Val)==0)
                        parseur->IndexParCat[x->val].ajoute(n->source);
                    Val|=x->Val;
                }
            }
            else
                //On conserve la position dans la liste...                
                parseur->IndexParCat[(n->X)->val].ajoute(n->source);
            
            parseur->IndexParCat[0].ajoute(n->source); //liste accessible via CatVide
            //Cette valeur UTILISE est donnee dans deux cas:
            //a) Le noeud est directement place sous le noeud GROUPE
            //b) Le noeud a ete place dans l'arbre par une regle de sequence et           
            //   l'element dans la regle qui s'est associe etait un ? (donc pas de
            //   choix impose de categories)
            //Correction, on ne fait cette operation qui vise a bloquer les manipulations sur 
            //les noeuds dans une regle de deduction, que lorsque l'on s'apprete a appliquer ces
            //regles. Dans les autres cas, on ne touche pas a ces valeurs, sinon, on peut avoir
            //un probleme avec des regles de chunking
            if (ajouteNoeudLex!=TOUTECATEGORIE) {
                if ((n->source)->couvert==UTILISE)
                    (n->source)->couvert=NON;
                else
                    (n->source)->couvert=OUI;
            }
        }
        
        if (n->fils)
            n->clef=(n->fils)->ConstruitIndex(parseur,n,ajouteNoeudLex,lahauteur+1);        
        
        //si c'est un noeud lexical, on le conserve dans une liste a cet effet
        if (n->filsTerminal() && ajouteNoeudLex!=NON) {
            parseur->NoeudsLexicaux.ajoute(n);
            n->source->traitsbase->ou(parseur->TraitNoeudTerminal);
            n->source->traitsactif->ou(parseur->TraitNoeudTerminal);
        }
        
        ledernier=n;
        n=n->frere;
    }
    
    
    dernier=ledernier;
    n=ledernier;
    int nb=1;
	TVecteur laVal;

    while (n!=NULL) {           
        laVal=0;
        Liste* L=n->source;
        
        if (L==NULL || L->voc==NULL || (L->voc)->nbX==1 || L->couvert==OUI)
            laVal=(n->X)->Val;
        else
            laVal=L->index;
        
        laClef|=laVal;
        n->clef_suivant=laClef;
        n->nombre=nb++;
        n=n->precedent;
    }
    parseur->ClefArbre|=laClef;
	if (lahauteur==0) {
		laVal=0;
		for (nb=parseur->noeudsaplat.dernier-1;nb>=0;nb--) {
			n=parseur->noeudsaplat[nb];
			n->index=nb;
			laVal|=n->X->Val;
			n->clefsurindex=laVal;
		}
	}

    return laClef;
}


/*-------------------------------------------------------------------------*/
inline char PreTestMarquage(GlobalParseur* parseur,Noeud* n,EtatArbre* regle) {
    EtatArbre* et=NULL;
    char calcul;

    if (PreTestEtatCategorie(parseur,regle,n,calcul)==-1)
        return NON;    

    if (regle->etatfin!=1 && regle->arcs.dernier==1) {
        EtatArbre* et=regle->arcs[0]->etatarbre;
        if (n->frere==NULL)
            if (!et->rechercheCheminEpsilon(parseur->XCatEpsilon))
                return NON;
            else {
                if (PreTestEtatCategorie(parseur,et,n->frere,calcul)==-1)
                    return NON;
            }
            
            if (et->fils!=NULL && 
                PreTestEtatCategorie(parseur,et->fils,n->fils,calcul)==-1)
                return NON;
    }
    return OUI;
}
/*-------------------------------------------------------------------------*/
//La fonction de marquage des noeuds 
char Fonction::MarqueNoeuds(GlobalParseur* parseur,Noeud* n,int laCouche) {
    char arret_analyse=NON;
    if (clef != (n->clef_suivant & clef)) 
        return NON;
    
    Piles stacks;
    //On verifie que les elements que la regle de marqueur recherche 
    //sont bien present sur l'ensemble des noeuds
    if (regle != NULL) {
        char chemin=-1;   
        parseur->reglecourante=this;
        char testrace= parseur->TRACEDEPENDANCE;
        if (parseur->TRACEDEPENDANCE==OUI && parseur->TRACE==NON)
            parseur->TRACEDEPENDANCE=NON;
        
        
        if (n->pere==NULL ||  (n->pere->clef & clef) == clef)
			if (PreTestMarquage(parseur,n,regle)) {
				parseur->initIteration();
                regle->Marquage(parseur,chemin,n,1,stacks,this,NULL,NULL);
			}
            
            parseur->TRACEDEPENDANCE=testrace;           
            if (chemin==OUI) {
                arret_analyse=parseur->incrementestats(this);
                parseur->ModificationDansCouche[laCouche]=OUI;
                if (parseur->TRACE)
                    trace<<parseur->CHAINES[236]<<parseur->CHAINES[237]<<n->surface<<","<<n->num_mot<<"."<<endl;
            }
    }
    stacks.raz();
	return arret_analyse;
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::IndexationArbrePartiel(LListe& LaPhrase,char ipos) {
    int i;
    NoeudsLexicaux.raz();
    Noeud* premier=NULL;
    
    for (i=0;i<NbCats;i++)
        IndexParCat[i].dernier=0;
    
    if (ipos==UTILISE) {
        MAJInterVictoire(LaPhrase);
        INTERVICTOIRE->ConstruitIndex(this,NULL,TOUTECATEGORIE,0);
    }
    else {
        premier=&LaPhrase[0]->N;
        if (ipos==OUI)
            LaPhrase[0]->position=0;
        
        for (i=1;i<LaPhrase.dernier;i++) {
            premier->insere(&LaPhrase[i]->N);
            if (ipos==OUI)
                LaPhrase[i]->position=i;
        }
        
        premier->ConstruitIndex(this,NULL,TOUTECATEGORIE,0);
    }
}

void GlobalParseur::AnnulationArbrePartiel(LListe& Phrase) {

    for (int ik=0;ik<Phrase.dernier;ik++)
        Phrase[ik]->N.annule();  
    if (Phrase.dernier) {
        RetireDebut(Phrase[0]->traitsbase);
        RetireFin(Phrase.fin()->traitsbase);
    }
    INTERVICTOIRE=NULL;
}

/*-------------------------------------------------------------------------*/
//Cette fonction permet le marquage des noeuds, 
//dont le placement dans un arbre est conforme a la regle
void GlobalParseur::MarqueNoeuds(LListe& Phrase,int laCouche) {
    
    Fonction* fonc;
	char arret_analyse=NON;
    int itm;

	if (laCouche>Limite_Couche) {                    
		//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
		//commencer a reappliquer nos regles
		demarre_regle=0;
		demarre_couche=label_stop->couche;
		aller_label=label_stop;
		return;
	}
    
    IndexationArbrePartiel(Phrase,NON);
        
    for (itm=demarre_regle;itm<TableDesMarqueurs[laCouche].dernier;itm++) {
        fonc=TableDesMarqueurs[laCouche][itm];
        //Dans le cas d'un GOTO on saute les regles avant le label
		if (!testEspaceRegle(fonc))
			continue;
        if (fonc->activation(this)==NON) {                    
            //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
            //commencer a reappliquer nos regles
            demarre_regle=0;
            demarre_couche=label_stop->couche;
            aller_label=label_stop;
            break;
        }

        if (fonc->numero<demarre_regle)
            continue;

        if (TRACE) {
            trace<<CHAINES[238];
            VlabelVecteur[fonc->numero]->aff(trace);
            trace<<endl<<endl;
        }
        
        //pour chacun des noeuds obtenus qui 
        //partagent la premiere categorie de fonction
        for (int i=0; i< IndexParCat[fonc->categorie].dernier;i++)            
            arret_analyse=fonc->MarqueNoeuds(this,&IndexParCat[fonc->categorie][i]->N,laCouche);
    }
    
    AnnulationArbrePartiel(Phrase);    
}



void affphrase(GlobalParseur* parseur,LListe& Phrase) {
    for (int ki=0;ki<Phrase.dernier;ki++) {
        cout<<ki<<":";
        Phrase[ki]->N.AfficheMotPF(parseur);
        cout<<endl;
    }
    cout<<endl;
}
/*-------------------------------------------------------------------------*/
//Cette fonction permet le marquage des noeuds, 
//dont le placement dans un arbre est conforme a la regle
void GlobalParseur::AppliqueReconstruit(LListe& LaPhrase,int laCouche) {
    char arret_analyse=NON;
    VECTA<ReconstruitArbre*>& table=TableDesReconstructeurs[laCouche];

	if (laCouche>Limite_Couche) {                    
		//Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
		//commencer a reappliquer nos regles
		demarre_regle=0;
		demarre_couche=label_stop->couche;
		aller_label=label_stop;
		return;
	}

    IndexationArbrePartiel(LaPhrase,OUI);
    
    for (int it=demarre_regle;it<table.dernier;it++) {
		if (!testEspaceRegle(table[it]))
			continue;
        if (table[it]->activation(this)==NON) {                    
            //Sinon, on remet a jour la couche et le numero de regle a partir de laquelle on peut
            //commencer a reappliquer nos regles
            demarre_regle=0;
            demarre_couche=label_stop->couche;
            aller_label=label_stop;
            break;
        }
        
        //Dans le cas d'un GOTO on saute les regles avant le label
        if (table[it]->numero<demarre_regle)
            continue;

        VECTA<Liste*> lliste;
        int i;
        for (i=0; i< IndexParCat[table[it]->categorie].dernier;i++)
            lliste.ajoute(IndexParCat[table[it]->categorie][i]);   
        
        //pour chacun des noeuds obtenus qui 
        //partagent la premiere categorie de fonction
        for (i=0; i< lliste.dernier;i++) { 
            if (lliste[i]==NULL || lliste[i]->N.modifie==OUI)
                continue;
            arret_analyse=table[it]->Applique(this,LaPhrase,&lliste[i]->N,laCouche);
        }
    }

    AnnulationArbrePartiel(LaPhrase);
}


/*-------------------------------------------------------------------------*/
int nb_test_marquage=0;

//La fonction de marquage des noeuds 
char ReconstruitArbre::Applique(GlobalParseur* parseur,LListe& Phrase,Noeud* n,int laCouche) {
    
    if (clef != (n->clef_suivant & clef))
        return NON;
    char arret_analyse=NON;
    int i;

    if (parseur->TRACE) {
        trace<<parseur->CHAINES[239];
        parseur->VlabelVecteur[numero]->aff(trace);
        trace<<endl<<endl;
    }
	
	VECTA<VECTA<Noeud*>*> variables;
    
	parseur->reglecourante=this;
    //On verifie que les elements que la regle de marqueur recherche 
    //sont bien present sur l'ensemble des noeuds
    if (patron != NULL) {
        char chemin=-1; 
		Piles stacks;
		VECTA<int> pileArg;
		VECTA<Noeud*> varias;
		stacks.raz();

        if (n->pere==NULL ||  (n->pere->clef & clef) == clef)
			if (PreTestMarquage(parseur,n,patron)) {
				parseur->initIteration();
                patron->Marquage(parseur,chemin,n,1,stacks,NULL,&variables,&pileArg);
			}
            
            
            nb_test_marquage++;
            
            if (chemin==OUI && modele!=NULL) { //on a trouve une solution a partir de n
                if (testPatron) {
                    varias.raz();
                    for (i=0;i<=nbvariables;i++) {
                        if (variables[i]!=NULL && variables[i]->dernier==1)
                            varias.affecte(i,(*variables[i])[0]);
                    }
                    if (!testPatron->Test(parseur,varias)) {
                        for (i=0;i<variables.dernier;i++) {
                            if (variables[i]!=NULL)
                                variables[i]->raz();
                        }
                        return NON;
                    }
                }
                
                //Dans le cas ou il y a des noeuds lexicaux potentiellement detruits, on met un message d'avertissement
                //en mode DEBUG
                if (ajoutvariable==OUI && 
                    parseur->TestOptions(AFFICHAGE_ERREUR_EXECUTION) &&
                    variables[nbvariables]!=NULL) {
                    ostream* os=&cout;
                    if (parseur->OS!=NULL)
                        os=parseur->OS;

                    //ces noeuds sont les derniers du tableau
                    for (i=0;i<variables[nbvariables]->dernier;i++) {
                        if ((*variables[nbvariables])[i]->fils->fils==NULL) {                            
                            *os<<"Deleting terminal nodes:"<<(*variables[nbvariables])[i]->fils->lemme
                               <<"("<<(*variables[nbvariables])[i]->gauche<<")"<<" rule:"<<identificateur<<Endl;
                        }
                    }
                }

                if (modele->RecreeArbre(parseur,Phrase,NULL,variables,nbvariables,identificateur,OUI)!=NULL) {                                       
                    
                    parseur->ModificationDansCouche[laCouche]=OUI;
					arret_analyse=parseur->incrementestats(this);
                    //Puis par mesure de securite, on reconstruction l'index
                    if (parseur->TRACE)
                        trace<<parseur->CHAINES[240]<<numero<<parseur->CHAINES[237]<<n->surface<<","<<n->num_mot<<"."<<endl;
                }
            }
    }

    for (i=0;i<variables.dernier;i++) {
        if (variables[i]!=NULL)
            delete variables[i];
    }
    return arret_analyse;
}

//On recalcule les profondeurs des noeuds...
void calculprofondeur(Noeud* n,int profondeur) {
	Noeud* bfn;
    while (n!=NULL) {
        n->hauteur=profondeur;        
        RetireDebut(n->source->traitsactif);        
        RetireFin(n->source->traitsbase);
        n->dernier=n;

        Noeud* nfils=n->fils;
        if (nfils!=NULL) {
            calculprofondeur(nfils,profondeur+1);
            //On initialise les traits first et last
            //On remet a jour les offsets
            //Les nouveaux noeuds crees n'en ont pas...

            if (nfils->fils!=NULL) {
                Noeud* ncourant=nfils;
                MetDebut(ncourant->source->traitsactif);        
                MetDebut(ncourant->source->traitsbase);
                while (ncourant->frere!=NULL)
                    ncourant=ncourant->frere;
                MetFin(ncourant->source->traitsactif);        
                MetFin(ncourant->source->traitsbase);
				n->fils->dernier=ncourant;
            }           

			//La recuperation des offsets peut etre perturbe par le deplacement
			//d'un element lexical. On effectue donc une boucle pour rechercher
			//les offsets gauche et droit
			bfn=nfils;
			long gauche=bfn->gauche;
			long droite=bfn->droite;
			long gauchecar=bfn->gauchecar;
			long droitecar=bfn->droitecar;
			int num_gauche=bfn->num_gauche;
			int num_droite=bfn->num_droite;
			while (bfn!=NULL) {
				if (bfn->gauche<gauche)
					gauche=bfn->gauche;
				if (bfn->droite>droite)
					droite=bfn->droite;
				if (bfn->gauchecar<gauchecar)
					gauchecar=bfn->gauchecar;
				if (bfn->droitecar>droitecar)
					droitecar=bfn->droitecar;
				if (bfn->num_gauche<num_gauche)
					num_gauche=bfn->num_gauche;
				if (bfn->num_droite>num_droite)
					num_droite=bfn->num_droite;
				bfn=bfn->frere;
			}
            n->gauche=gauche;
            n->droite=droite;
            n->gauchecar=gauchecar;
            n->droitecar=droitecar;

            n->num_gauche=num_gauche;
            n->num_droite=num_droite;
        }
		else {
			//Si c'est un nouveau noeud cree, purement lexical
			if (n->num_droite==PURLEXICAL) {
				//dans ce cas, on a besoin de lui ajouter un peu d'info, en se basant sur son frere ou son pere
				bfn=NULL;
				if (n->precedent!=NULL) {
					bfn=n->precedent;
					n->gauche=bfn->droite;
					n->droite=bfn->droite;
					n->gauchecar=bfn->droitecar;
					n->droitecar=bfn->droitecar;
					n->num_gauche=bfn->num_droite+1;
					n->num_droite=bfn->num_droite+1;
				}
				else
					if (n->pere!=NULL) {
						bfn=n->pere;
						n->num_gauche=bfn->num_gauche-1;
						n->num_droite=bfn->num_gauche-1;
						n->gauche=bfn->gauche;
						n->droite=bfn->gauche;
						n->gauchecar=bfn->gauchecar;
						n->droitecar=bfn->gauchecar;
					}
					else
						n->num_droite=0;
			}
		}
		n=n->frere;
    }
}

void affinter(GlobalParseur* parseur,VECTA<VECTA<Noeud*>*>& variables,int nbvariables) {
    int i;
    Noeud* n;
    for (i=0;i<=nbvariables;i++) {
        if (variables[i]!=NULL && variables[i]->dernier!=0) {                
            for (int j=0;j<variables[i]->dernier;j++) {
                n=(*variables[i])[j];
                cout<<i<<":";
                n->AfficheMotPF(parseur);
                cout<<endl;
            }
        }
    }
}


char TestValiditeAConstruire(EtatArbre* et,VECTA<VECTA<Noeud*>*>& variables) {
	ArcArbre* ar;
	EtatArbre* ets;
	while (et->etatfin==0) {        
        ar=et->arcs[0];
        //ets est l'etat suivant de notre regle
        ets=ar->etatarbre;
        
        //si cet arc est associe a une variable, nous construisons immediatement notre
        //serie de noeuds
        
		if (ar->argument!=-1) {
            if (variables[ar->argument]==NULL || variables[ar->argument]->dernier==0)
				if (ets->fils!=NULL)
					return NON;
		}
		if (ets->fils!=NULL) {
			char test=TestValiditeAConstruire(ets->fils,variables);
			if (test==NON)
				return NON;
		}
			
		et=ets;
	}
	return OUI;
}


Noeud* EtatArbre::RecreeArbre(GlobalParseur* parseur,
                              LListe& Phrase,
                              Noeud* racine,
                              VECTA<VECTA<Noeud*>*>& variables,
                              int nbvariables,
							  int ident,
                              char top) {

    int i,j;
    Noeud* bornegauche=NULL;
    Noeud* bornedroite=NULL;
    Noeud* borneracine=NULL;
    Noeud* n;

    if (top==OUI) {
        //dans un premier temps on recherche les noeuds dont la hauteur est la plus faible
        //la hauteur d'un noeud est calculee comme une distance a la racine
        int profondeur=-1;
        char yatilunnoeud=NON;
		//On teste la validite des noeuds...

		if (!TestValiditeAConstruire(this,variables))
			return NULL;


        for (i=0;i<=nbvariables;i++) {
            if (variables[i]!=NULL && variables[i]->dernier!=0) {
                yatilunnoeud=OUI;
                for (j=0;j<variables[i]->dernier;j++) {
                    n=(*variables[i])[j];
                    if (profondeur==-1)
                        profondeur=n->hauteur;
                    else
                        if (n->hauteur<profondeur)
                            profondeur=n->hauteur;
                }
            }
        }

        //dans le cas ou le tableau de noeuds est vide, on s'arrete la et on revient...
        if (yatilunnoeud==NON)
            return NULL;

        //Puis on detecte les bornes
        for (i=0;i<=nbvariables;i++) {
            if (variables[i]!=NULL && variables[i]->dernier!=0) {
                for (j=0;j<variables[i]->dernier;j++) {
                    n=(*variables[i])[j];   
                    //Nous gardons la trace des noeuds racines, ceux dont la hauteur
                    //correspond a ce qui a ete calcule plus haut
                    if (n->hauteur==profondeur) {                    
                        if (bornegauche==NULL) {
                            bornegauche=n;
                            bornedroite=n;
                            borneracine=n->pere;
                        }
                        else {//Ceux dont la valeur nombre est la plus grande sont a gauche
                            if (n->num_droite<bornegauche->num_gauche)
                                bornegauche=n;
                            //ceux dont la valeur nombre est la plus petite sont a droite
                            if (n->num_gauche>bornedroite->num_droite)
                                bornedroite=n;
                        }
                    }
                }
            }
        }


        //Ces bornes sont donc le noeud a gauche de notre noeud le plus a gauche
        bornegauche=bornegauche->precedent;
        //Le noeud le plus a droite de notre noeud le plus a droite...
        bornedroite=bornedroite->frere;

        //dans un second temps, on annule les noeuds pris dans les variables
        for (i=0;i<=nbvariables;i++) {
            if (variables[i]!=NULL && variables[i]->dernier!=0) {
                //ndebut est le premier noeud de notre liste
                Noeud* ndebut=(*variables[i])[0];           
                //nfin est le dernier noeud de notre liste
                Noeud* nfin=variables[i]->fin();
                //Nous devons les separer de leur environnement immediat
                //Tout d'abord nous calculons nf, le noeud le plus a droite
                //de la liste [ndebut..nfin]
                Noeud* nfrere=nfin->frere;                
                Noeud* nprec=ndebut->precedent;

                //cas ou nfin est le dernier noeud de la structure
                if (nfrere==NULL) {
                    //dans ce cas, il faut remettre a jour le dernier
                    //qui pointe alors sur nprec...
                    if (nfin->pere!=NULL)
                        nfin->pere->fils->dernier=nprec;
                }
                else
                    nfrere->precedent=nprec;

                //cas ou ndebut est aussi le premier noeud
                if (nprec==NULL) {
                    //dans ce cas le nouveau premier noeud est nfrere
                    if (ndebut->pere!=NULL) {
                        if (nfrere!=NULL)
                            nfrere->dernier=ndebut->pere->fils->dernier;
                        ndebut->pere->fils=nfrere;
                    }
                }
                else
                    nprec->frere=nfrere;

                //On les deconnecte du reste.
                ndebut->precedent=NULL;
                nfin->frere=NULL;

                //dans tous les cas de figures nprec et nfrere sont rebranches
                //ensemble
                
                //Puis nous isolons ce bloc de noeud de son ancien attachement
                for (j=0;j<variables[i]->dernier;j++)
                    (*variables[i])[j]->annulereconstruction(); 
                
            }
        }
    }


    EtatArbre* et=this;
    EtatArbre* ets;
    ArcArbre* ar;
    Noeud* nouvN=NULL;

    Noeud *premierNoeud=NULL;
    char buffer[100];
    
    //Nous avons donc desormais une structure de noeuds totalement disjointe
    //Nous allons parcourir alors notre regle pour reconstruire les noeuds selon
    //la structure donnee par la regle
    while (et->etatfin==0) {        
        //Une regle commence par un etat prolonge par un arc qui porte la categorie
        //que l'on recherche. Cet arc peut etre associe a une variable, le champ argument
        //est alors different de -1, il vaut la valeur de la variable elle-meme
        ar=et->arcs[0];
        //ets est l'etat suivant de notre regle
        ets=ar->etatarbre;
        
        //si cet arc est associe a une variable, nous construisons immediatement notre
        //serie de noeuds
        
        if (ar->argument!=-1) {
            if (variables[ar->argument]!=NULL && variables[ar->argument]->dernier!=0) {
                VECTA<Noeud*>& vn=*variables[ar->argument];
                
                //s'il y a plus d'un noeud pour cette variable, c'est une erreur            
                if (ets->fils!=NULL && vn.dernier!=1) {
                    sprintf_s(buffer,100,"%s:#%d",parseur->CHAINES[362],ar->argument);
                    parseur->erreur(buffer);
                }
                
                nouvN=vn[0];
                //Si l'arc courant presente des modifications par rapport au noeud courant
                //il faut pouvoir en tenir compte
                if ( (ar->X)->val!=0 && ar->X != nouvN->X) {
                    //s'il y a plus d'un noeud pour cette variable, c'est une erreur
                    if (vn.dernier!=1) {
                        sprintf_s(buffer,100,"%s:#%d",parseur->CHAINES[362],ar->argument);
                        parseur->erreur(buffer);
                    }
                    
                    parseur->IndexParCat[(nouvN->X)->val].affecte((nouvN->source)->indexCat,NULL);
                    
                    //Categories differentes
                    //On retire les traits specifiques de l'ancienne categorie
                    Categorie* cat= parseur->Categories[(nouvN->X)->index];
                    ((nouvN->source)->traitsbase)->etnon(cat->traits);
                    ((nouvN->source)->traitsactif)->etnon(cat->traits);
                    
                    
                    if (nouvN->surface==(nouvN->X)->nom)
                        nouvN->surface=(ar->X)->nom;
                    
                    nouvN->X=ar->X;
                    (nouvN->source)->X=ar->X;
					(nouvN->source)->index=ar->X->Val;
                    (nouvN->source)->indexCat=parseur->IndexParCat[(nouvN->X)->val].dernier;
                    parseur->IndexParCat[(nouvN->X)->val].ajoute(nouvN->source);
                    //Dans le cas d'une feuille, il faut de plus verifier si cette nouvelle categorie
                    //correspond a une ancienne categorie
                    if ((nouvN->source)->voc!=NULL) {
                        ListeVoc* voc= (nouvN->source)->voc;
                        char trouve=NON;
                        for (int icat=0;icat<voc->nbX;icat++) {
                            if (voc->X[icat]==ar->X) {
                                nouvN->source->choixCategorie(NULL,icat);
                                trouve=OUI;
                                break;
                            }
                        }
                        //sinon, il s'agit d'un remplacement definitif
                        if (trouve==NON)
                            parseur->EcrasementCategorie(ar->X,NULL,voc,nouvN->source);
                    }
                }
                
                //Nous gardons la trace du premier noeud
                if (premierNoeud==NULL) {
                    premierNoeud=nouvN;
                    MetDebut(premierNoeud->source->traitsactif);                
                    MetDebut(premierNoeud->source->traitsbase);
                    //s'il y a une racine, on l'insere dessous
                    if (racine!=NULL)
                        racine->inseresous(nouvN);
                }
                
                //des traits differents, il s'ajoute aux traits courants...
                if (ar->traits!=NULL) {
                    parseur->InstancieTraits( (nouvN->source)->traitsactif,ar->traits);
                    ((nouvN->source)->traitsbase)->ou((nouvN->source)->traitsactif);
                }
                
                
                //On insere alors les noeuds freres;
                for (i=0;i<vn.dernier;i=i+1) {
                    //des traits differents, il s'ajoute aux traits courants...
                    if (ar->traits!=NULL) {
                        parseur->InstancieTraits(vn[i]->source->traitsactif,ar->traits);
                        vn[i]->source->traitsbase->ou(vn[i]->source->traitsactif);
                    }
                    vn[i]->modifie=OUI;
                    //en faisant attention a ne pas inserer deux fois le meme noeud
                    if (premierNoeud!=vn[i])
                        premierNoeud->insertion(vn[i]);
                }
            }
        }
        else {//sinon, il s'agit d'un noeud qu'il faut construire de toute piece
            //On utilise alors les informations fournis par l'arc
            Liste* L=(parseur->TasDeListes)->fournir(parseur,ar->X,ar->traits);
			L->N.numero=ident;
            nouvN=&L->N;
            nouvN->modifie=OUI;
			//On garde une trace de la creation de ce nouveau noeud
			nouvN->num_droite=PURLEXICAL;
            //Il peut alors s'agir du premier noeud de l'arbre
            //Nous gardons la trace du premier noeud
            if (premierNoeud==NULL) {
                premierNoeud=nouvN;
                //s'il y a une racine, on l'insere dessous
                if (racine!=NULL)
                    racine->inseresous(nouvN);
            }
            else //sinon on le rajoute comme frere
                premierNoeud->insertion(nouvN);
        }
        
        //dans tous les cas de figure, nouvN pointe sur le dernier noeud ajoute
        if (ets->fils!=NULL) {
            //Nous avons deja verifie qu'il ne peut y avoir qu'un seul noeud nouvN
            //Nous faisons un appel recursif pour construire les fils
            Noeud* nfils=ets->fils->RecreeArbre(parseur,Phrase,nouvN,variables,nbvariables,ident,NON);
            //On fait remonter les traits libres
            if (nfils!=NULL)
                parseur->PTT(nouvN->source->traitsbase,nfils,nfils->dernier);
            else {//dans le cas ou les fils sont NULL
                //Autrement dit, dans le cas ou l'on a ajoute un noeud supplementaire 
                //et qu'on s'apercoit que ses fils n'existent pas, on retire alors ce noeud
                //le dernier de premierNoeud
                //le noeud est le premier
                if (premierNoeud==premierNoeud->dernier)
                    premierNoeud=NULL;
                else //on retire le dernier noeud de la liste
                    premierNoeud->retire();
            }
        }
        et=ets;
    }

    
    if (top==OUI) {
        int db;
        //s'il s'agit du sommet de la hierarchie, nous pouvons reconstruire l'arbre
        //Tous d'abord nous savons que la portion modifiee est situee entre bornegauche et
        //bornedroite
        //si bornegauche n'est pas NULL, nous lui accrochons le premier noeud
        if (bornegauche!=NULL) {
            bornegauche->frere=premierNoeud;
            premierNoeud->precedent=bornegauche;
        }
        else {//sinon, il s'agit du pere 
            if (borneracine!=NULL)
                borneracine->fils=premierNoeud;
        }

        //cette borne racine est partagee par tous les nouveaux noeuds
        n=premierNoeud;
        while (n->frere!=NULL) {
            n->pere=borneracine;
            n=n->frere;
        }
        //n pointe sur le nouveau noeud le plus a droite
        n->pere=borneracine;
        if (bornedroite!=NULL) {
            //on raccroche alors la borne a droite a ce nouveau noeud
            n->frere=bornedroite;
            bornedroite->precedent=n;
        }

        if (borneracine!=NULL) {
            if (borneracine->fils!=NULL && bornedroite!=NULL) {
                //On met a jour alors le dernier element de notre liste
                n=bornedroite;
                while (n->frere!=NULL)
                    n=n->frere;
                borneracine->fils->dernier=n;
            }
        }
        else {
            //S'il n'y a pas de borne racine, c'est que l'on doit modifier Phrase
            //tous les elements entre borne gauche et borne droite doivent etre retire
            db=0;
            if (bornegauche!=NULL)
                db=bornegauche->source->position+1;
            int dd=Phrase.dernier;
            if (bornedroite!=NULL)
                dd=bornedroite->source->position;
            for (i=dd-1;i>=db;i--)
                Phrase.retireElement(i);
            
            n=premierNoeud;
            while (n!=bornedroite) {
                Phrase.insere(db,n->source);
                db++;
                n=n->frere;
            }
            for (db=0;db<Phrase.dernier;db++)
                Phrase[db]->position=db;            
        }
		if (Phrase[0]!=NULL)
            calculprofondeur(&Phrase[0]->N,0);
    }

    return premierNoeud;
}
        



//--------------------------------------------------------------------------------------
//On construit une clef sur la base de toutes les dependances
//qui doivent exister si l'on veut que la regle s'applique
//Il ne faut pas qu'il y ait de negation ni de ou
void VectTFInstruction::ConstruitClef() {
    //
    clef=0;
    if (listes.liste.dernier==0)
        return;
    
    VECTA<ListeTestArbre*>& ar=listes.liste;
    for (int i=0;i<ar.dernier;i++) {
        if (ar[i]->test!=NULL && TestFoncDep(ar[i]->test)) {
            TestFonctionDependance* ttf= (TestFonctionDependance*)ar[i]->test;
            if (ttf!=NULL && ttf->negation==NON)
                clef |= (ttf->Fx)->Val;
        }
    }
}

void ArcArbre::RechercheDesamb(VECTA<ArcArbre*>& vectarbre) {
    if (marque==1)
        return;
    marque=1;
    
    if (desamb==OUI)
        vectarbre.ajoute(this);
        
    etatarbre->RechercheDesamb(vectarbre);
    marque=-1;            
}

void EtatArbre::RechercheDesamb(VECTA<ArcArbre*>& vectarbre) {    
    if (marque==1)
        return;
    marque=1;
    
    for (int i=0;i<arcs.dernier;i++) 
        arcs[i]->RechercheDesamb(vectarbre);

    marque=-1;
        
}


//On construit la liste de tous les arcs pour lesquels il y aura une
//desamb dans un contexte: |% ... |
void Simplifie::ConstruitClef() {
    clef=0;
    for (int i=0;i<nbX;i++) {
        if (negation[i]==NON)
            clef|=X[i]->Val;
    }
    
    if (gauche!=NULL && gauche->regle!=NULL)
        gauche->regle->RechercheDesamb(vectarbre);
    
    if (droit!=NULL && droit->regle!=NULL) 
        droit->regle->RechercheDesamb(vectarbre);    
}


void ArcArbre::ArcEpsilon(VECTA<EtatArc*>& eas,Cat* epsilon,char type) {
    if (marque==1)
        return;
    marque=1;
    etatarbre->ArcEpsilon(eas,epsilon,type);
}

void EtatArbre::ArcEpsilon(VECTA<EtatArc*>& eas,Cat* epsilon,char type) {
    if (marque==1)
        return;
    marque=1;
    if (fils!=NULL)
        fils->ArcEpsilon(eas,epsilon,type);
    for (int i=0;i<arcs.dernier;i++) {
        if (arcs[i]->X==epsilon && (type!=longue || arcs.dernier==1)) {
            int j;
            EtatArc* ea=new EtatArc(this,arcs[i],i);
            for (j=0;j<eas.dernier;j++)  {
                if (arcs[i]->etatarbre->etat>eas[j]->arc->etatarbre->etat)
                    break;
            }
            eas.insere(j,ea);
        }
        arcs[i]->ArcEpsilon(eas,epsilon,type);
    }
}


void FournirEtatArbre::demarque() {
    for (int i=0;i<etats.dernier;i++)
        if (etats[i]!=NULL)
            etats[i]->demarque();        
}

void FournirEtatArbre::demarquage(int de,int da) {
    int i;
    for (i=de;i<etats.dernier;i++) {
        if (etats[i]!=NULL)
            etats[i]->marque=-1;
    }
    
    for (i=da;i<arcs.dernier;i++) {
        if (arcs[i]!=NULL)
            arcs[i]->marque=-1;
    }
}


EtatArbre* FournirEtatArbre::alloue() {
    EtatArbre* e=new EtatArbre;
    e->etat=etats.dernier;
    etats.ajoute(e);
    return e;
}

ArcArbre* FournirEtatArbre::allouearc(Cat* x ,Traits* tr,int n) {
    ArcArbre* e=new ArcArbre(x,tr,n);   
    e->num=arcs.dernier;
    arcs.ajoute(e);
    return e;
}

void FournirEtatArbre::detruit(EtatArbre* a) {
    etats.affecte(a->etat,NULL);
    delete a;
}

void FournirEtatArbre::retireEpsilon(EtatArbre* premier,Cat* epsilon,int debutetat,int debutarc,char determ) {
    //On retire les epsilons du dernier automate cree
    //Tout d'abord on recherche tous les arcs epsilons
    int i,j,k,l;
    
    VECTA<EtatArc*> eas;
    
    
    char boucle=OUI;
    demarquage(debutetat,debutarc);
    premier->ArcEpsilon(eas,epsilon,determ);
    demarquage(debutetat,debutarc);

    
    if (eas.dernier==0)
        return;
    
    for (i=0;i<eas.dernier;i++) {
        //on recupere tous les arcs semblables:
        k=i+1;
        char det=determ;        
        EtatArbre* dernier=eas[i]->arc->etatarbre;
        
        
        while (k<eas.dernier && eas[i]->arc->etatarbre==eas[k]->arc->etatarbre) k++;
        
        if (dernier->etatfin==2 || dernier->fils!=NULL) {
            i=k-1;
            continue;
        }
        
        
        //Nous avons dans etatarbre toutes les transitions necessaires
        for (j=k-1;j>=i;j--) {
            if (dernier==eas[j]->etat && eas[i]->arc->X==epsilon)
                continue;
            
            //On remplace les arcs epsilon par les arcs suivants            
            //La position de l'arc epsilon que l'on remplace
            //Tous les arcs seront places apres
            int pos=eas[j]->iar;
            EtatArbre* ajout=eas[j]->etat;
            if (pos>ajout->arcs.dernier)
                pos=ajout->arcs.dernier;
            char trouve;
            for (l=0;l<dernier->arcs.dernier;l++) {
                trouve=NON;
                for (int lememe=0;lememe<ajout->arcs.dernier;lememe++) {
                    if (ajout->arcs[lememe]==dernier->arcs[l]) {
                        trouve=OUI;
                        break;
                    }
                }
                if (trouve==NON)
                    ajout->arcs.insere(pos++,dernier->arcs[l]);
            }
            
            //on recupere notre transition finale            
            if (eas[j]->etat->etatfin==0)
                eas[j]->etat->etatfin=dernier->etatfin;                                    
        }

        //On retire les transitions epsilons
        for (j=i;j<k;j++) {
            for (l=eas[j]->etat->arcs.dernier-1;l>=0;l--)
                if (eas[j]->etat->arcs[l]==eas[j]->arc)
                    eas[j]->etat->arcs.retireElement(l);
        }

        i=k-1;
    }
    eas.nettoie();
}

void EtatArbre::ajoutearc(GlobalParseur* parseur,ArcArbre* a,Cat* epsilon,char determinisme) {
    int i;
    if (arcs.dernier==0) {
        arcs.ajoute(a);
        return;
    }

    if (a->etatarbre==NULL) {
        if (parseur!=NULL)
            parseur->erreur(parseur_courant->CHAINES[337]);
        else
            exit(-1);
    }
    
    a->etatarbre->ConstruitClefSuivant(parseur,epsilon);

    if (determinisme==longue) {        
        if (a->type==1) {
            arcs.insere(0,a);
            return;
        }

        //On place les arcs EPSILON de type 2 (saut d'optionalite)
        //tout en bas
        if (a->X==epsilon) {
            switch(a->type) {
            case 1:            
                arcs.insere(0,a);
                break;
            case 2:            
                arcs.ajoute(a);                
                break;
            default:
            //sinon on les place apres les arcs commandant le chemin le plus long
                for (i=0;i<arcs.dernier;i++) {
                    //Les arcs bouclants sont places en tete
                    if (arcs[i]->type==1)
                        continue;
                    if (arcs[i]->etatarbre->nombre<a->etatarbre->nombre || arcs[i]->type==2)
                        break;
                }
                arcs.insere(i,a);
            }
        }
        else {
            for (i=0;i<arcs.dernier;i++) {
                //Les arcs bouclants sont places en tete
                if (arcs[i]->type==1)
                    continue;
                if (arcs[i]->etatarbre->nombre<a->etatarbre->nombre || arcs[i]->type==2)
                    break;
            }
            arcs.insere(i,a);
        }
        return;
    }
    
    if (a->type==1) {
        arcs.ajoute(a);
        return;
    }

    //On place les arcs EPSILON de type 2 (saut d'optionalite)
    //tout en haut
    if (a->X==epsilon) {
        switch(a->type) {
        case 1:            
            arcs.ajoute(a);
            break;
        case 2:            
            arcs.insere(0,a);                
            break;
        default:
            for (i=0;i<arcs.dernier;i++) {
                if (arcs[i]->type==2)
                    continue;
                if (arcs[i]->etatarbre->nombre>=a->etatarbre->nombre)
                    break;
            }
            arcs.insere(i,a);
        }
    }
    else {
        //Pour les autres arcs, on les place apres les arcs de type 2            
        for (i=0;i<arcs.dernier;i++) {
            if (arcs[i]->type==2 || 
                (arcs[i]->etatarbre->nombre==a->etatarbre->nombre && arcs[i]->X==epsilon) )
                continue;
            if (arcs[i]->etatarbre->nombre>a->etatarbre->nombre || arcs[i]->type==1)
                break;            
        }
        arcs.insere(i,a);
    }
}

void EtatArbre::ConstruitClef(Cat* epsilon,int& nombfils) {
    //On construit la clef associee aux fils
    if (fils==NULL)
        return;
    clef=0;   
    nombfils=fils->CheminCourt(clef,0,epsilon);    
}

void EtatArbre::ConstruitClefSuivant(GlobalParseur* parseur,Cat* epsilon) {
    TVecteur unecle=0;
    if (arcs.dernier!=0)
        parseur->listeEtatArbre.demarquage(etat,arcs[0]->num);
    CheminCourt(unecle,0,epsilon);
    if (arcs.dernier!=0)
        parseur->listeEtatArbre.demarquage(etat,arcs[0]->num);
}


//On veut calculer la distance maximale d'un etat par rapport a un autre
//et garder la trace des categories obligatoires
int ArcArbre::CheminCourt(TVecteur& unecle,int max,Cat* epsilon) {
    //On marque le passage par cet arc
    marque=1;   
    //si c'est une boucle et que cet arc boucle sur lui-meme, on utilise une marque speciale
    if (type==1)
        marque=2;
    
    if (etatarbre==NULL) {
        if (parseur_courant!=NULL)
            parseur_courant->erreur(parseur_courant->CHAINES[337]);
        else
            exit(-1);
    }

    int nb=0;
    //si l'etat suivant est deja calcule
    if (etatarbre->marque==1) {
        //on recupere la taille du chemin
        //Dans le cas d'un etat final, ce calcul est inutile
        if (etatarbre->etatfin==0) {
            nb=etatarbre->nombre;
            char debut=OUI;
            for (int i=0;i<etatarbre->arcs.dernier;i++) {
                //si c'est une boucle, on n'y touche pas, on ne connait pas
                //encore toutes les informations qui sont en cours de calcul...
                if (etatarbre->arcs[i]->type==1 && 
                    etatarbre->arcs[i]->marque==2)
                    continue;
                
                if (debut==OUI) {
                    unecle=etatarbre->arcs[i]->clefsuivante;
                    debut=NON;
                }
                else
                    unecle&=etatarbre->arcs[i]->clefsuivante;            
            }        
        }
		else {
			if (etatarbre->etatfin==2) {
				//On verifie si cet arc est le dernier
				if (etatarbre->arcs.dernier!=0) {
					if (parseur_courant!=NULL)
						parseur_courant->erreur(parseur_courant->CHAINES[374]);
					else
						exit(-1);
				}
			}

		}
	}
    else
        nb=etatarbre->CheminCourt(unecle,max,epsilon);   

	if (X==NULL) {
        if (parseur_courant!=NULL)
            parseur_courant->erreur(parseur_courant->CHAINES[371]);
        else
            exit(-1);
	}

    clefsuivante=unecle|X->Val;    
    unecle=clefsuivante;

    //dans le cas du marquage boucle, on revient a un marquage simple
    if (marque==2)
        marque=1;

    //s'il s'agit d'un arc epsilon, le chemin est nul
    if (X==epsilon)    
        return nb;

    //sinon, le chemin vaut +1
    return nb+1;
}

int EtatArbre::CheminCourt(TVecteur& unecle,int max,Cat* epsilon) {
    //on marque le passage par cet etat
    marque=1;

    if (fils!=NULL) {
        clef=0;
        fils->CheminCourt(clef,0,epsilon);
    }    

    int minarc=0;
    TVecteur mincle=0;
    int i;
    int nb;
    TVecteur cleinter;
    char debut=OUI;
    for (i=0;i<arcs.dernier;i++) {        
        cleinter=0;
        //si on est deja passe par cet arc, on sait que la clefsuivante est porte par celui-ci
        if (arcs[i]->marque!=-1) {
            //si c'est une boucle, il ne faut pas y toucher
            if (arcs[i]->type==1 && arcs[i]->marque==2)
                continue;

            cleinter=arcs[i]->clefsuivante;
            //la taille est aussi portee par l'etat suivant de cet arc
            nb=arcs[i]->etatarbre->nombre;
            //si cet arc n'est pas un epsilon, on incremente de 1
            if (arcs[i]->X!=epsilon)
                nb++;
        }
        else        
            nb=arcs[i]->CheminCourt(cleinter,0,epsilon);        
        
        //On garde le chemin le plus court
        //Il faut que l'on garde l'intersection des categories
        if (debut==OUI) {
            minarc=nb;
            mincle=cleinter;
            debut=NON;
        }
        else {
            if (nb<minarc)
                minarc=nb;
                mincle&=cleinter;
        }
    }
    

    //si ce n'est pas un etat final
    if (etatfin==0) {
        //on initialise avec le minarc
        nombre=minarc;
        //On garde l'union des clefs trouvees
        unecle|=mincle;
    }
    else
        nombre=0;

    //marque=-1;
    return nombre;
}

ArcArbre::ArcArbre(Cat* x, Traits* tr,int arg) : numliste(3) {
    negation=NON;
    selection=NULL;
    selectiontous=NULL;
    X=x;
    traits=tr;
    argument=arg;
    profondeurNonBornee=NON;
    desamb=NON;
    type=0;
    clefsuivante=0;
    marque=-1;
    num=0;
    garder=NON;
    etatarbre=NULL;
}


//On recupere l'ensemble des arcs finaux
void EtatArbre::dernierarcs(VECTA<ArcArbre*>& dernier) {
    for (int i=0;i<arcs.dernier;i++) {
        if (arcs[i]->etatarbre->arcs.dernier==0)
            dernier.ajoute(arcs[i]);
        else
            arcs[i]->etatarbre->dernierarcs(dernier);
    }
}

void ArcArbre::arcvers(VECTA<ArcArbre*>& dernier,EtatArbre* cible) {
    if (marque==1)
        return;
    marque=1;
    if (etatarbre==cible)
        dernier.ajoute(this);
    etatarbre->arcvers(dernier,cible);
}

void EtatArbre::arcvers(VECTA<ArcArbre*>& dernier,EtatArbre* cible) {
    if (marque==1)
        return;
    marque=1;
    for (int i=0;i<arcs.dernier;i++)
        arcs[i]->arcvers(dernier,cible);
}

void EtatArbre::demarque() {
    if (marque==-1)
        return;
    marque=-1;
    if (fils!=NULL)
        fils->demarque();
    for (int i=0;i<arcs.dernier;i++) {
        if (arcs[i]->etatarbre==this)
            continue;
        arcs[i]->demarque();
    }
}

void ArcArbre::demarque() {
    if (marque==-1)
        return;
    marque=-1;
    if (type!=1)
        etatarbre->demarque();
}

void EtatArbre::met(GlobalParseur* parseur,trait* tr) {
    if (marque==1)
        return;
    marque=1;
    if (fils!=NULL)
        fils->met(parseur,tr);
    for (int i=0;i<arcs.dernier;i++) {
        if (arcs[i]->etatarbre==this)
            continue;
        arcs[i]->met(parseur,tr);
    }
}

void ArcArbre::met(GlobalParseur* parseur,trait* tr) {
    if (marque==1)
        return;
    marque=1;
	if (traits==NULL)
		traits=new Traits(parseur);
	traits->ou(tr);
	if (type!=1)
        etatarbre->met(parseur,tr);
}


void EtatArbre::typer(Cat* epsilon) {    
    if (marque==1)
        return;
    if (fils!=NULL)
        fils->typer(epsilon);
    marque=1;
    garder=OUI;
    for (int i=0;i<arcs.dernier;i++) {
        if (arcs[i]->X==epsilon || arcs[i]->profondeurNonBornee==OUI)
            type=1;
        arcs[i]->typer(epsilon);
    }
}


void ArcArbre::typer(Cat* epsilon) {    
    if (marque==-1) {
        garder=OUI;
        marque=1;
        etatarbre->typer(epsilon);
    }
}

EtatArbre::EtatArbre() {
    fils=NULL;
    etatfin=0;
    marque=-1;
    etat=0;
    clef=0;
    type=0;
    nombre=0;
    garder=NON;
}

void FournirEtatArbre::destruction(int de,int da) {
    int i;
    for (i=etats.dernier-1;i>=de;i--) {
        if (etats[i]!=NULL) {
            if (etats[i]->garder==NON) {
                EtatArbre* e=etats[i];
                etats.retireElement(i);
                delete e;
            }
        }
    }
    
    
    for (i=arcs.dernier-1;i>=da;i--) {
        if (arcs[i]!=NULL) {
            if (arcs[i]->garder==NON) {
                ArcArbre* a=arcs[i];
                arcs.retireElement(i);
                delete a;
            }
        }
    }
    
}



//Fusion des arcs de type epsilon
char EtatArbre::FusionneArc(GlobalParseur* parseur) {
    if (arcs.dernier<=1)
        return NON;
        
    ArcArbre* ar;
    int iv,jv;
    VECTA<int> vn;        
    //Tout d'abord, on verifie s'il n'existe pas des noeuds epsilon ayant le meme but commun
    for (iv=0;iv<arcs.dernier;iv++) {
        ar=arcs[iv];
        if (ar->X==parseur->XCatEpsilon) {
            for (jv=iv+1;jv<arcs.dernier;jv++)
                if (ar->etatarbre==arcs[jv]->etatarbre && ar->X==arcs[jv]->X) {
                    if (vn.cherche(jv)==-1)
                        vn.ajoute(jv);
                }
        }
    }

    if (vn.dernier>=1) {
        for (int jv=vn.dernier-1;jv>=0;jv--) {
            ar=arcs.retireElement(vn[jv]);
            parseur->listeEtatArbre.arcs.affecte(ar->num,NULL);
            delete ar;
        }
    }

    return OUI;
}




