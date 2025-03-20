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
   filename   : parseur.cxx
   Date       : 10/01/1999  
   Purpose    : Rule Compiler
   Programmer : Claude ROUX
   Reviewer   : 
*/

#ifdef WIN32
#include "Windows.h"
#endif

#include "parsbin.h"
#include <locale.h>
#include "ntmhmm.h"
#include "transduct.h"
#include "generation.h"
#include "conversion.h"
void GetVarName(int i,string& s);

#define EXTRAIRE 2
#define DECRYPTAGE 3

#ifdef XIPLIBXML
#include "varlibxml.h"
#endif
#include "graphe.h"
#include "kif.h"
#include "xipkif.h"

#ifdef CRFTAGGER
extern "C" {
void* CreateTagger(char* crfmodel,char* crfbrown,char* crfmkcls);
int TaggerError(void* tg);
}
#endif

void KifCleaning();
int KifLoadin(KifCode* kifcode,string path);

extern long ERREUR_POSITION_DANS_FICHIER;
int compte_test_arbre=0;
char crypte_repertoire[1000];
char duplication_crypte=0;
int VERIF=NON;
char cryptejustelesregles=NON;
extern char afficheautomate;
char affiche_regle_appliquee;
extern GlobalParseur* parseur_courant;
char majlemmesurface(GlobalParseur* parseur,Traits* RegTraits,Noeud* noeud,char type);
void NormaliseNomFichier(char* nomFichier,char* tampon,char* repertoire);
void ajouteautomate(automate* principal,automate* nouveau);
void nettoieduplicats(automate* principal);
void KiFSetdebugmin(bool d);
string retournemessageerreur(GlobalParseur* parseur);
int posreglesdansfichier=-1;
vector<string> basenomsregles;
char compilekif=NON;
string choix_ambigue="";
void inittableutf8();
vector<string> kifarguments;
vector<string> grmkifarguments;
GlobalParseur* BaseParseur=NULL;
bool DirectoryCreation(char* noms, int pos);
void AddALoadingPath(string thelocalname);

//char python_debug=NON;
//int num_python_ligne=0;
//int num_fichier_python=0;

ostream* flot_erreur=&cerr;
void codesplit(char* t, VECTA<char*>& r,char coupure);
void lance_erreur(char* message);

#define DISPLAY_INFORMATION 1
#define DISPLAY_COMMENTS 2
#define DISPLAY_RULESPACE 4
#define TESTRAMASSAGE(z,c,x,y) cout<<"Test sur "<<y<<": ";c=0;for(z=0;z<x::ramassage.dernier;z++) {if (x::ramassage[z]!=NULL) c++;};*flot_erreur<<y<<":"<<c<<" Taille="<<x::ramassage.taille<<Endl;
void ParseurKifInit(KifCode* kifcodeptr);

char* langues_reconnaissables[] = {"Danish","Dutch","English","French","German","Italian","Norwegian", "Portuguese","Russian","Spanish","Swedish","@"};

extern char VERSION_XIP[];
char statistique_interne=NON;

char* TC(GlobalParseur* parseur,Traits* tr);
char* TV(GlobalParseur* parseur,Vecteur* v);
void ListerCategories(STVecteur v,VECTA<int>& vv);
void Trim(char* chaine);
char alettre(unsigned char c);
char minlettre(unsigned char c);
char TestChiffre(unsigned char* forme);

char* champs_anglais[] = {"",
"sequence:",
"idrules:",
"tagging:",
"extraction:",
"dependencyrules:",
"semantic:",
"marker:",
"terms:",
"reshuffling:",
"reshuffling:",
"split:",
"doublereduction:",
"",
"fragment:",
"vocabulary:",
"lprules:",
"functions:",
"multiwords:",
"tokenselection:",
"lexicals:",
"generation:",
"order:",
"sequenceoncontext:"};

static char* champs_francais[] = {"",
                      "séquence:",
                      "dirègles:",
                      "désambiguïsation:",
                      "extraction:",
                      "déduction:",
                      "sémantique:",
                      "marqueur:",
                      "termes:",
                      "reconstructeur:",
                      "reconstructeur:",
                      "découpeséquence:",
                      "doubleréduction:",
                      "",
                      "fragmenter:",
                      "vocabulaires:",
                      "plrègles:",
                      "fonctions:",
					  "multimots:",
					  "tokenselection:"
					  "lexicals:",
					  "génération:",
					  "ordre:",
					  "séquencesurcontexte:"};

#define ADRESSAGE_ABSOLU 1
#define ADRESSAGE_RELATIF 2
#define FICHIER_CRYPTE 4
#define FICHIER_NON_CRYPTE 8

#define USAGE_LIMITE 1
#define USAGE_INTERDIT 2

char* strdup_vit (char* ch);
char* strdup_vit (const char* ch);

char TestTypeChgtFichier(char val,char test) {
    if ((val & test) == test)
        return OUI;
    return NON;
}

#ifdef XIPPYTHON
static char importation_python=NON;
#endif

extern int kif_declencheur;

void GlobalParseur::RenvoieErreurAutomate(autoerreur err) {
    switch(err) {
    case AERR_WRONG_VARIABLE_ORDER:
        erreur(CHAINES[289]);        
    case AERR_WRONG_USE_OF_COMMA:
        erreur(CHAINES[284]);      
    case AERR_WRONG_KLEENE:
        erreur(CHAINES[285]);      
    case AERR_MISMATCHED_PARENTHESIS:
        erreur(CHAINES[286]);      
    case AERR_MISMATCHED_BRACKET:
        erreur(CHAINES[287]);      
    case AERR_MISSING_PARENTHESIS:
        erreur(CHAINES[275]);
    case AERR_MISSING_BRACKET:
        erreur(CHAINES[276]);
    case AERR_WRONG_NEGATION_POSITION:
        erreur(CHAINES[277]);
	case AERR_WRONG_CODE:
		erreur(CHAINES[364]);
    }
}

inline char CheckCRC(VECTA<char*>& contents)
{
    const size_t size = 16;

    char* code = contents.retire();
    const size_t contents_length = contents.dernier;
    const size_t number_of_row = (contents_length%size);

    unsigned char colBuffer[size] = "INTERNAL ERROR ";

    unsigned long rowBuffer = 0; 
    // the columns
    size_t num_row = 0;
    size_t i;
    

    for (i = 0; i < contents_length; i++)
    {
        for (size_t lettre=0;lettre < strlen(contents[i]);lettre++) {
        
            size_t i_col = (i+lettre)%size;
            if (i_col == 0 && i > 0) 
            {
                unsigned char sum_line = (unsigned char) ((rowBuffer << 4) + (rowBuffer >> 4));
                // start of a new line
                for (size_t j = 0; j < size; j++)
                    colBuffer[j] += sum_line;
                rowBuffer = 0;
            }

            colBuffer[i_col] += contents[i][lettre];
            rowBuffer += contents[i][lettre];
        }
    }

    char res[1000];
    res[0]=0;
    for (i = 0; i < 8; i++)
    {
        const size_t base = 41;
        unsigned short s = (((unsigned short)colBuffer[2 * i]) << 8) | colBuffer[(2 * i) + 1];
        char conv[4] = "000";
        size_t j = 2;
        while (s != 0)
        {
            char c = s % base;
            if (c < 26)
                c += 'A';
            else
                c += 'a' - 26;
            conv[j] = c;
            j--;
            s /= base;
        }
        conv[3] = '\0';
        strcat(res,conv);
    }

    char validator_res;

    if  (!strcmp(res,code))
        validator_res = OUI;
    else
        validator_res = NON;
    return (validator_res);
}

/*--------------------------------------------------------------------------*/
void Cheminabsolu(char* nom,string& nomabsolu) {
	//On recupere le nom du repertoire associe au fichier    
	char cheminabsolu[4096];
#ifdef WIN32
	_fullpath(cheminabsolu,nom,4096);
#else
	realpath(nom,cheminabsolu);
#endif

	//nous sautons les deux premiers caracteres qui correspondent a "c:"
	//Nous initialisons avec notre chemin dans res
	nomabsolu=cheminabsolu;
}

DecompteRegle::DecompteRegle(typeRegle r,char* nom,unsigned long off,unsigned int l,int c,int cr) {
    regle=r;
    position=off;
    couche=c;
    couche_relative=cr;
	string cheminabsolu;
	Cheminabsolu(nom,cheminabsolu);
    nom_fichier=strdup_vit(STR(cheminabsolu));
    user=NULL;
    ligne=l;
}

DecompteRegle::~DecompteRegle() {
    free(nom_fichier);
}


/*--------------------------------------------------------------------------*/
Label::Label(char* e,typeRegle t,int c,int i,DecompteRegle* d) {
    etiquette=e;
    type=t;
    index=i;
	numero=i;
    couche=c;
    dr=d;
	indexregle=i;
	space=NULL;
}

Label::~Label() {
    libere(etiquette);
    if (dr!=NULL)
        delete dr;
}

Label* GlobalParseur::AjouteLabel(VECTA<Label*>& v, char* e, typeRegle t,int c,int& nbs,DecompteRegle* d) {

	if (e==NULL)
		e=DupliqueChaine("#");
	else
		e=DupliqueChaine(e);
    Label* l=new Label(e,t,c,v.dernier,d);   
	l->space=currentspacename;
	l->commentaire=commentaire_regle;
	
	rulespace* est=currentspacename;
	while (est!=NULL) {
		est->regles.ajoute(l);
		est=est->pere;
	}

	commentaire_regle="";
    v.ajoute(l);
	if (e[0]!='#') {
		if (rechercheLabel(e)!=NULL) {
			char tampon[200];
			sprintf(tampon,"%s: %s",e,CHAINES[257]);
			erreur(tampon);
		}
		ArbreLabel[e]=l;
	}

    //AVL<Label*>* avl=new AVL<Label*>((uchar*)e,l);

    //if (ArbreLabel==NULL)
    //    ArbreLabel=avl;
    //else {
    //    if (e[0]!='#')
    //        if (ArbreLabel->recherche((uchar*)e)) {
    //            char tampon[200];
    //            sprintf(tampon,"%s: %s",e,CHAINES[257]);
    //            erreur(tampon);
    //        }
    //    ArbreLabel->insere(&ArbreLabel,avl);                 
    //}
    nbs++;
    return l;
}

void Label::aff(ostream& os) {
    if (etiquette==NULL || etiquette[0]=='#')
        os<<index;
    else
        os<<etiquette<<"("<<index<<")";
}
/*--------------------------------------------------------------------------*/
void GlobalParseur::AddAToken(uchar* tok) {}


void GlobalParseur::MAJNumeroRegles() {
	int i;
	//On renumerote toutes les regles selon l'ordre des couches
	VECTA<VECTA<baseregle*>*> triparcouche(MaxCouches);
	for (i=0;i<VBases.dernier;i++) {
		int lc=VBases[i]->couche;
		if (lc==-1)
			lc=MaxCouches+1;
		if (triparcouche[lc]==NULL)
			triparcouche.affecte(lc,new VECTA<baseregle*>);
		triparcouche[lc]->ajoute(VBases[i]);
	}

	int num_regle=0;
	for (i=0;i<triparcouche.dernier;i++) {
		if (triparcouche[i]==NULL)
			continue;

		for (int j=0;j<triparcouche[i]->dernier;j++) {
			if (triparcouche[i]->cell(j)->label!=NULL) {
				triparcouche[i]->cell(j)->label->index=num_regle;
				if (triparcouche[i]->cell(j)->label->type==REGLES_DI) {
					//Il faut boucler pour tout mettre a jour
					Regle* reg=(Regle*)triparcouche[i]->cell(j);
					while (reg!=NULL) {
						reg->identificateur=num_regle;
						reg=reg->frere;
					}
				}
			}
			triparcouche[i]->cell(j)->identificateur=num_regle++;
		}
	}

	for (i=0;i<VlabelVecteur.dernier;i++) {
		num_regle=VlabelVecteur[i]->index;
		if (num_regle!=-1)
			VlabelVecteurIndex.affecte(num_regle,VlabelVecteur[i]);
	}

	triparcouche.nettoie();
}


int GlobalParseur::Preparation(char* rep) {
    char CHSEP[]={SEP,0};
    CHAINES.ajuste(300);
	

    //On verifie que la chaine n'a pas deja ete initialisee 
    //(dans le cas d'un rechargement, ce n'est pas necessaire d'effectuer 
    //la modification du chemin)
//    if (strcmp(repertoire,rep)) {
        strcpy(repertoire,rep);
        if (repertoire[strlen(repertoire)-1]!=CHSEP[0])
            strcat(repertoire,CHSEP);
        strcpy(Nom_Fichier,repertoire);
        strcat(Nom_Fichier,nom_fichier);
        strcpy(Nom_Desamb,repertoire);    
//    }
  
    if (laLangue!=PERSONNEL)
        strcpy(commentaire,repertoire);



    appliqueGrammaire=OUI;
    appliqueGrammaireDefini=appliqueGrammaire;
    
     if (laLangue==FRANCAIS)
         strcat(commentaire,"chaines.fic");
     else
         if (laLangue==ANGLAIS)
             strcat(commentaire,"strings.file");

    
    Commentaires(commentaire,laLangue);
    
    //Si le nom du fichier est vide, on est forcement dans une optique NTM pure
    //On n'effectue alors aucune lecture
    if (!*nom_fichier)
        return NON;

    parseur_courant=this;
	index_parseur_courant=iParseurCourant;
    return OUI;
}

/*-------------------------------------------------------------------------*/
//INITIALISATION
void GlobalParseur::Initialisation(char* rep) {

    int i;
	
	choix_ambigue="";
    if (!Preparation(rep))
        return;

    if (access(Nom_Fichier,00)) {
        NOM=CHAINES[262];
        sprintf(commentaire,"%s: %s",CHAINES[259],Nom_Fichier);
        erreur(commentaire);
    }

	inittableutf8();
	//Nous preparons la variable pour accueillir le code kif 
	kifparametres=NULL;
	DeductionDeKif=NULL;
    ReglesEntrelacees.raz();


    //--------------------------------------------------------
    char souvcrypt=cryptage;
    if (cryptage==UTILISE) {
		if (duplication_crypte==DECRYPTAGE)
			cryptage=OUI;
		else
			cryptage=NON;
	}

	max_iteration=10000;
	encodageutf8=false;
	//---------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------
    //Lecture des noms de fichiers (regles, sorties, traits)
    LitNomFichiers();		
	//---------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------


    hierarchies=new Hierarchie;
    relations=new Hierarchie;
    concepts=new Hierarchie;

	FONCTIONSKIF=NULL;

	TOUSTRAITS=new Vecteur(iTailleVect);
	TOUSTRAITS->un();
    FILTRE=new Vecteur(iTailleVect);
    FILTREFONCTION=new Vecteur(iTailleVect);
    FILTRENOEUD=new Vecteur(iTailleVect);
	FILTREGENERATION=new Vecteur(iTailleVect);

    if (TRACE || TRACENOEUD || TRACETRAITS || TRACEREGLE || TRACEDEPENDANCE) {
        *flot_erreur<<CHAINES[260]<<Endl;
        if (Trace==NULL)
            Trace= new ofstream(Nom_Trace);
    }

    cryptage=souvcrypt;
    
	//Mode cryptage
    if (cryptage==UTILISE) {
		
		if (cryptejustelesregles==OUI) {
			duplication_crypte=EXTRAIRE;
			*flot_erreur<<"On crypte uniquement les fichiers de règles"<<Endl;
		}
		if (duplication_crypte!=NON)
			DupliqueGrmFiles((char*)GrmPathname.c_str());
		
		string nouveaufichierlangue=CrypteFichier(Nom_Fichier);

		for (i=0;i<Nom_traits.dernier;i++)            
			CrypteFichier(Nom_traits[i]);
		

		for (i=0;i<Nom_lexique.dernier;i++)            
			CrypteFichier(Nom_lexique[i]);

		if (cryptejustelesregles==OUI)
			duplication_crypte=OUI;		
		
		for (i=0;i<Nom_regle.dernier;i++) {
			if (TestTypeChgtFichier(ReglesEntrelacees[i],FICHIER_NON_CRYPTE))
				cryptage=NON;
            CrypteFichier(Nom_regle[i]);
			cryptage=souvcrypt;
		}
        for (i=0;i<Nom_fonction.dernier;i++) 
            CrypteFichier(Nom_fonction[i]);
		
		char prevdup=duplication_crypte;
		duplication_crypte=EXTRAIRE;
		cryptage=NON;
		for (i=0;i<fichiersdata.size();i++) 
			CrypteFichier(STR(fichiersdata[i]));
		duplication_crypte=prevdup;
		cryptage=souvcrypt;

		//dans le cas d'un cryptage partiel, il faut effectuer un certain traitement dans le fichier de langue
		if (cryptejustelesregles==OUI) {
			ifstream modiflangue(Nom_Fichier,modeOuverture);
			modiflangue.read(Chaine_traits,posreglesdansfichier);
			ofstream cryptee((char*)nouveaufichierlangue.c_str(),ios::binary);
			cryptee.write(Chaine_traits,posreglesdansfichier);
			
			for (i=0;i<basenomsregles.size();i++) {
				string s=basenomsregles[i];
				char plus=NON;
				if (s[0]=='+') {
					plus=OUI;
					s=s.substr(1,s.size()-1);
				}
				if (s[0]!='%' && s[0]!='@') {
					if (plus)
						s="+%"+s;
					else
						s="%"+s;
				}
				if (i)
					s=", "+s;
				else
					s=" "+s;
				cryptee.write((char*)s.c_str(),s.size());
			}
			cryptee.write("\n",1);
		}
        return;
    }

    if (cryptage!=NON)
        *flot_erreur<<CHAINES[261]<<Endl;

    if (ARBREIND)
        Sortie= new ofstream(Nom_Sortie);
    else
        Sortie=NULL;
    
    //--------------------------------------------------------

    iNombreDeRegles=0;
    
    larbredestraits=new arbretrait(this);
    larbredestraits->nom="*";


    DebutSeq=CreationTrait("first","+"); //Debut d'une sequence, trait: premier --> 1
    FinSeq=CreationTrait("last","+"); //fin d'une sequence, trait: dernier --> 2
    DebutPhrase=CreationTrait("start","+"); // --> 4
    FinPhrase=CreationTrait("end","+"); // --> 8
	PasAmbigue=CreationTrait("noambiguity","+"); // --> 16
	Fictif=CreationTrait("fictivenode","+"); // --> 32
    AmbiguiteErreur=CreationTrait("ambiguityerror","+"); // --> 64
    TraitNoeudTerminal=CreationTrait("terminal","+"); // --> 128

	//Position 0: En premiere place: 256
	TraitPosition=CreationTrait("$","0");
	//Position 1: Deuxieme place: 512
	CreationTrait("$","1");
	//Juste avant le dernier element:1024
	CreationTrait("$","-1");
	
    //Creation de procedures predefinies d'acces au noeud    
    Affichage = CreeProcedure((char*)"display");
    Affichage->X.type=C_PROCEDUREDISPLAY;
    Dependance* depend=new Dependance(&Affichage->X);
    depend->arg.ajoute(1);    
    VectTFInstruction* vtf=new VectTFInstruction;
    vtf->dependances.ajoute(depend);
    vtf->numero=numero_procedure++;    
    IndexProcedures.affecte(depend->Fx->index,ListeDesProcedures.dernier);
    ListeDesProcedures.ajoute(vtf);

	const char* taggerguessed="taggerguessed";
	trait* taggerguessedspecial=CreationTrait(taggerguessed,"+");
	traittaggerguessed=new Traits(this);
	traittaggerguessed->ou(taggerguessedspecial);
    const char* crfselection="crfselection";
    trait* crfspecial=CreationTrait(crfselection,"+");
    traitcrfselection = new Traits(this);
    traitcrfselection->ou(crfspecial);


    const char* hmmselection="hmmselection";
    trait* hmmspecial=CreationTrait(hmmselection,"+");

    traithmmselection = new Traits(this);
    traithmmselection->ou(hmmspecial);

	tokenstart=CreationTrait("tokenstart","+"); // --> 256
	tokeninter=CreationTrait("tokeninter","+"); // --> 512
	tokenend=CreationTrait("tokenend","+"); // --> 1024
	untoken=CreationTrait("istoken","+"); // --> 2048
    
    TraitsDependances=new Traits(this);

    size_t indmot=CalculClefHachageTout((uchar*)hmmselection);
    Traductions[indmot]=new avl_vocabulaire(this,(char*)hmmselection,NULL,NULL,traithmmselection,NULL);
    

    contexteFoncVide = new Cat;
    contexteFoncVide->nom=(char*)"@";
	contexteFoncVide->courant=this;

    CatVide=CreeCatVide(this);
    XCatVide = &CatVide->X;
  

	CatNeg = CreeFonc((char*)"~",NbFonctionsExtraction++,NULL,NULL);
    CatNeg->X.index=0;
    CatNeg->X.val=0;
    CatNeg->X.Val=0;
    CatNeg->X.index=0;    
	CatNeg->X.courant=this;
	XCatNeg = &CatNeg->X;

	creationtoken = CreeFonc((char*)"token",NbFonctionsExtraction++,NULL,NULL);
	Xcreationtoken=&creationtoken->X;
	Xcreationtoken->type=C_GENERATION;

	surfacetoken = CreeFonc((char*)"stoken",NbFonctionsExtraction++,NULL,NULL);
	Xsurfacetoken=&surfacetoken->X;
	Xsurfacetoken->type=C_GENERATION;

    //Lecture des categories et des traits...
    for (i=0;i<Nom_traits.dernier;i++)
        LitCategories(Nom_traits[i]);

    if (LEMME==NULL)
        LEMME=DupliqueChaine("lemma");
    if (SURFACE==NULL)
        SURFACE=DupliqueChaine("surface");

#ifdef XIPPYTHON
    //On cree la variable python_exchange_data, qui est automatiquement assigne de l'exterieur
    //par un appel de type xips.parse ou xips.parsefile
    vpython_echange=(VariablePython*)CreeVariable("python_exchange_data",PYTHONOBJET,VPYTHONOBJET,0,1,0);
    //Cette variable peut etre appelee de n'importe ou
#endif

    vkif_echange=(VariableKif*)CreeVariable("kif_exchange_data",KIFOBJET,VKIFOBJET,0,1,0);
    
    //------------------------------------------------------------
    INTERFONCTIONS.ajuste(NbFonctionsExtraction+10);
    INTERFONCTIONS.raz();


    PREMIERFONCTIONS.ajuste(200); //nombre de noeuds maximum
    PREMIERFONCTIONS.raz();
    for (i=0;i<PREMIERFONCTIONS.taille;i++)
        PREMIERFONCTIONS.affecte(i,new ExtractionFonction);
    SECONDFONCTIONS.ajuste(200); //nombre de noeuds maximum
    SECONDFONCTIONS.raz();
    for (i=0;i<SECONDFONCTIONS.taille;i++)
        SECONDFONCTIONS.affecte(i,new ExtractionFonction);
    
    fournirResFonc=new fournitureRF(this,500);

    CatEpsilon=CreeCatVide(this,"EPSILON");    
    XCatEpsilon=&CatEpsilon->X;
	label_stop=NULL;
    label_terminal=NULL;
    label_retour=NULL;
    BaseCoucheNumero=ValeurCoucheInitiale;
    char val_crypte=cryptage;
    for (i=0;i<Nom_regle.dernier;i++) {
        if (TestTypeChgtFichier(ReglesEntrelacees[i],ADRESSAGE_RELATIF))
            BaseCoucheNumero=MaxProfondeurAnalyse; //adressage de couche relatif
        else
            BaseCoucheNumero=ValeurCoucheInitiale; //sinon, c'est un adressage absolu

        if (TestTypeChgtFichier(ReglesEntrelacees[i],FICHIER_CRYPTE))
            cryptage=OUI;
		else
			if (TestTypeChgtFichier(ReglesEntrelacees[i],FICHIER_NON_CRYPTE))
				cryptage=NON;
        
        LitReg(Nom_regle[i]);
        cryptage=val_crypte;
    }

    for (i=0;i<Nom_fonction.dernier;i++)
        LitReg(Nom_fonction[i]);

    for (i=0;i<fparametres.dernier;i++)
        LireParametres(fparametres[i]);

    for (i=0;i<Nom_lexique.dernier;i++)
        LitVoc(Nom_lexique[i],NON);
  

    CalculTeteDroite();

    //Cet ensemble de traits permet la gestion 
    //des traits provenant des balises. On peut utiliser ces
    //traits pour 
    BaliseTraits=new Vecteur(iTailleVect);
    gBaliseTraits=new Traits(this);
    gBaliseDebut=new Traits(this);
    gBaliseSansFin=new Traits(this);


    TasDeListes=new TasListe(1000);

    if (Sortie != NULL)
        Sortie->flush();

    if (Trace != NULL)
        Trace->flush();
    if (cryptage!=NON)
        *flot_erreur<<"Grammar Loaded"<<Endl<<Endl;
  
    
    if (TAG_NOUVEAU[0]) {
        indmot=CalculClefHachageTout((uchar*)TAG_NOUVEAU);
        if (Traductions[indmot]!=NULL)
            tag_nouveau=Traductions[indmot]->recherchecasse(TAG_NOUVEAU);
    }

    char mes[100];
    //Verification que toutes les procedures sont declarees
    for (i=0;i<Procedures.dernier;i++) {
        if (Procedures[i]->X.type==C_PROCEDUREDECLAREE) {            
            sprintf(mes,"%s:%s",CHAINES[255],Procedures[i]->X.nom);
            erreur(mes);
        }
    }
	TasDeListeVoc.ajuste(150);
    //On ajoute le label terminal qui correspond a la fin de la grammaire
    int nbLoc;
    label_stop=AjouteLabel(VlabelVecteur,"@",REGLES_DEDUCTION,0,nbLoc,NULL);
    label_terminal=AjouteLabel(VlabelVecteur,"$",REGLES_DEDUCTION,0,nbLoc,NULL);
    label_retour=AjouteLabel(VlabelVecteur,"*",REGLES_DEDUCTION,0,nbLoc,NULL);
    

    //Minimisation de la taille des automates
    LesBalises.aupluspres();
    //listeEtatArbre.destruction(0,0);

    for (i=0;i<MaxCouches;i++) {
        if (ArbVoc[i]!=NULL)
            ArbVoc[i]->aupluspres();    
        if (ArbSurface[i]!=NULL)
            ArbSurface[i]->aupluspres();    

        if (Termes[i]!=NULL)
            Termes[i]->aupluspres();

        if (Semantiques[i][NON]!=NULL)
            Semantiques[i][NON]->aupluspres();
        
        if (Semantiques[i][OUI]!=NULL)
            Semantiques[i][OUI]->aupluspres();
		
		//nettoieduplicats(ArbSurfaceComplexe[i]);				
		//nettoieduplicats(ArbVocComplexe[i]);
		//nettoieduplicats(motsmultiplescomplexes[0][i]);
		//nettoieduplicats(motsmultiplescomplexes[1][i]);
    }

    if (VERIF==OUI) {
        *flot_erreur<<"VERIFICATION IS DONE (exit)"<<endl;
        exit(-1);
    }

#ifdef XIPLIBXML
        //On cree aussi une nouvelle categorie
    XmlTagCat = CreeCat((char*)"xmlcat",NULL);
	basexmlcourante=new arborescenceXML;
#endif
	for (i=0;i<=iNombreDeValeurs*2+1;i++)
		Majuscule(TableDesTraits[i]); 

	if (DECOUPAGE !=NULL && !TestOptions(NON_NORMALISATION_TAG))
		MetEnMinuscule((unsigned char*)DECOUPAGE);


	MAJNumeroRegles();	
	if (GUESSED_TAG[0]==0)
		strcpy(GUESSED_TAG,"+guessed+Noun");



	if (guesser_feature[0]!=0) {
		if (!TestOptions(NON_NORMALISATION_TAG))
			MetEnMinuscule((unsigned char*)guesser_feature);

		//guesser=CreationTrait(guesser_feature,"+");    

		guesser=CreeTrait(guesser_feature);
		int i=CreeValeurSans(guesser,"+",guesser_feature);
		(guesser->valeur)->un(this,i);
		larbredestraits->ajoute(guesser->freretrait);

		Traits* traits=new Traits(this);
		traits->ou(guesser);
		avl_vocabulaire* trad=new avl_vocabulaire(this,guesser_feature,NULL,NULL,traits,NULL);
		size_t indexMot=CalculClefHachageTout((uchar*)guesser_feature);
        
        if (Traductions[indexMot]==NULL) {
            Traductions[indexMot]=trad;
        }
        else
            Traductions[indexMot]->inserecasse(&Traductions[indexMot],trad);
	}

	//Cette dependance doit toujours etre cachee
	creationtoken->X.cacher=OUI;
	surfacetoken->X.cacher=OUI;
	//Aleatoire
	srand(time(NULL)); 

	//Loading NTAGGER (see tagger.h)
#ifdef CRFTAGGER
	if (crfmodel!="") {
		*flot_erreur<<"Loading CRF model:"<<STR(crfmodel)<<endl;
		*flot_erreur<<"Loading Brown:"<<STR(crfbrown)<<endl;
		*flot_erreur<<"Loading MKCLS:"<<STR(crfmkcls)<<endl;
		crftagger=CreateTagger(STR(crfmodel),STR(crfbrown),STR(crfmkcls));		
		int errtagger=TaggerError(crftagger);
		switch(errtagger) {
		case 1:
			erreur("Error while reading BROWN file");
			break;
		case 2:
			erreur("Error while reading MKCLS file");
			break;
		case 3:
			erreur("Error while reading MODEL file");
			break;
		}
	}
#endif
}
//FIN INITIALISATION
/*-------------------------------------------------------------------------*/
void AjouteDansBase(VECTA<baseregle*>& VBases,baseregle* reg,int laCouche,Label* l) {
	VBases.ajoute(reg);
	reg->couche=laCouche;
	reg->label=l;
}

void GlobalParseur::AjouteStats(stats* s) {
	if (statistique_interne==NON) {
		delete s;
        return;
	}
    lesstats.affecte(s->regle->numero,s);
    statistiques.ajoute(s);
}

char GlobalParseur::NumeroOrdre() {
	if (Numero_Ordre!=-1 && Numero_Ordre < regles_appliquees.dernier)
		return OUI;
	return NON;
}

char GlobalParseur::incrementestats(baseregle* r) {
	char retour=NON;
	r->compteur++;
	//Des que le numero d'ordre est atteint, on doit arreter les traitements
	if (Numero_Ordre!=-1 && Numero_Ordre <= regles_appliquees.dernier) {
		if (Numero_Ordre == regles_appliquees.dernier)
			regles_appliquees.ajoute(r->identificateur);
		return OUI;
	}

	regles_appliquees.ajoute(r->identificateur);

	if (statistique_interne==NON)
        return NON;
    
    stats* s=lesstats[r->numero];
	if (s!=NULL) {
        s->compteur++;
        return NON;
    }    
	return NON;
}

void GlobalParseur::AfficheLesStats() {
    if (statistique_interne==NON)
        return;
    int i;
    static char buff[500];
    static char type_regle[20];

    ostream* os=&cout;
    if (OS!=NULL)
        os=OS;

    VECTA<ofstream*> outputs;
    ofstream* retires=NULL;
    if (fichieroptimise==OUI) {
        *os<<"Optimize grammar"<<endl;
        strcpy(buff,Nom_Fichier);
        strcat(buff,".removed");
        retires=new ofstream(buff);
        for (i=0;i<Nom_regle.dernier;i++) {
            strcpy(buff,Nom_regle[i]);
            strcat(buff,".opt");
            outputs.ajoute(new ofstream(buff));
        }
    }

    if (affiche_regle_appliquee!=NON)
        *os<<endl<<"Rules that applied:"<<endl<<endl;    

    int ind,stat,affiche_ces_regles;
    int dernierfichier=-1;
    type_regle[0]=0;
    for (i=0;i<statistiques.dernier;i++) {
		affiche_ces_regles=NON;
        stat=statistiques[i]->compteur;
		if (stat!=0)
			affiche_ces_regles=OUI;

		ind=statistiques[i]->regle->identificateur;
        switch(statistiques[i]->type) {
        case REGLES_DI:			
            if (affiche_regle_appliquee==NON)
                *os<<"DI:"<<ind<<"="<<stat<<endl; 
            strcpy(buff,CHAINES[21]);
            break;
        case REGLES_SEQUENCE:     			
            if (affiche_regle_appliquee==NON)
                *os<<"Sequence:"<<ind<<"="<<stat<<endl;
			strcpy(buff,CHAINES[202]);
            break;            
		case REGLES_SEQUENCE_CONTEXTE:
            if (affiche_regle_appliquee==NON)
                *os<<"Sequence in contexte:"<<ind<<"="<<stat<<endl;
			strcpy(buff,CHAINES[402]);
            break;            
        case REGLES_TERME:		
            if (affiche_regle_appliquee==NON)
                *os<<"Terme:"<<ind<<"="<<stat<<endl;
            strcpy(buff,CHAINES[1]);
            break;
        case REGLES_TOKENSELECTION:			
            if (affiche_regle_appliquee==NON)
                *os<<"TokenSelection:"<<ind<<"="<<stat<<endl; 
            strcpy(buff,"tokenselection");
            break;
        case REGLES_SIMPLIFICATION:			
            if (affiche_regle_appliquee==NON)
                *os<<"Tagging:"<<ind<<"="<<stat<<endl; 
            strcpy(buff,CHAINES[14]);
            break;
        case REGLES_CARTESIEN:			
            if (affiche_regle_appliquee==NON)
                *os<<"Cartesian:"<<ind<<"="<<stat<<endl;
            strcpy(buff,CHAINES[221]);
            break;
        case REGLES_RECONSTRUCTION:			
            if (affiche_regle_appliquee==NON)
                *os<<"Reshuffling:"<<ind<<"="<<stat<<endl; 
            strcpy(buff,CHAINES[2]);
            break;
        case REGLES_EXTRACTION:
        case REGLES_DEDUCTION:			
            if (affiche_regle_appliquee==NON)
                *os<<"Dependency:"<<ind<<"="<<stat<<endl;
            if (VlabelVecteur[ind]->type==REGLES_DEDUCTION)
                strcpy(buff,CHAINES[27]);  
            else
                strcpy(buff,CHAINES[201]);
            if (((VectTestFonction*)statistiques[i]->regle)->script==OUI) {
                affiche_ces_regles=NON;
                if (affiche_regle_appliquee==NON)
                    affiche_ces_regles=OUI;
                strcpy(buff,"Script");
            }
            break;
        case REGLES_MARQUAGE: 
			if (affiche_regle_appliquee==NON)
				*os<<"Marker:"<<ind<<"="<<stat<<endl;
            strcpy(buff,CHAINES[23]); 
            break;
        case REGLES_PL:
			affiche_ces_regles=NON;
            strcpy(buff,CHAINES[53]); 
            stat=0;
            break;        
        }

    
        if (affiche_regle_appliquee!=NON && affiche_ces_regles==OUI)
            FormatRuleText(*os,ind,DISPLAY_INFORMATION|DISPLAY_RULESPACE);
            

        if (fichieroptimise==OUI) {
            if (affiche_ces_regles==OUI) {
                for (int j=0;j<Nom_regle.dernier;j++) {
                    if (!strcmp(VlabelVecteur[ind]->dr->nom_fichier,Nom_regle[j])) {
                        if (strcmp(type_regle,buff) || dernierfichier!=j) {
                            *outputs[j]<<buff<<":"<<endl<<endl;
                            strcpy(type_regle,buff);
                            dernierfichier=j;
                        }
                        FormatRuleText(*outputs[j],ind,DISPLAY_RULESPACE);
                        break;
                    }
                }
            }
            else//on construit le fichier des regles retirees
                FormatRuleText(*retires,ind,DISPLAY_INFORMATION|DISPLAY_RULESPACE);
        }

        //Si c'est un interrules, alors on remet a zero les compteurs pour la phrase suivante
        if (affiche_regle_appliquee==2)
            statistiques[i]->compteur=0;
    }
    
    if (retires!=NULL) {
        retires->close();
        delete retires;
    }
    for (i=0;i<outputs.dernier;i++) {
        outputs[i]->close();
        delete outputs[i];
    }
}

void GlobalParseur::readTextFromFile(DecompteRegle* dr,DecompteRegle* drnext,int idr,char* buffer_read) {
                    
    long gauche=dr->position;
    char* fichier=dr->nom_fichier;
    int nbchar=1000;

    if (drnext!=NULL && !strcmp(dr->nom_fichier,drnext->nom_fichier))
        nbchar=drnext->position-gauche;
    
    if (nbchar>=10000)
        nbchar=9999;
    ifstream ansText(fichier,modeOuverture);
            
    ansText.seekg(gauche);
    ansText.read(buffer_read, nbchar); 
    int nb=ansText.gcount();
    buffer_read[nb] = 0;    
}

Exportation void FindRuleBoundaries(uchar* buffer,long& firstsel,long& lastsel,DecompteRegle* dr,DecompteRegle* drnext) {
    int i;
	
	int lg=strlen((char*)buffer);    
    for (i=0;i<lg;i++) {
        if (buffer[i]>32)
            break;
    }

	//Dans ce cas precis, nous sommes face a la presence d'un nom de champ
	//que l'on va retirer.
	uchar* nomchamp=NULL;
	if (drnext!=NULL && dr->regle!=drnext->regle && !strcmp(dr->nom_fichier,drnext->nom_fichier)) {		
		if (parseur_courant->laLangue==ANGLAIS)
			nomchamp=(uchar*)champs_anglais[drnext->regle];
		else
			nomchamp=(uchar*)champs_francais[drnext->regle];				
		for (int p=i;p<lg;p++) {
			//si le caractere est entre double cotes, on consomme les caracteres internes
			if (buffer[p]==34) {
				p++;
				while (p<lg && buffer[p]!=34) p++;
				continue;
			}

			if (buffer[p]=='/') {
				if (buffer[p+1]=='/') {
					while (p<lg && buffer[p]!=10 && buffer[p]!=13) p++;
					while (p<lg && buffer[p]<=32) p++;
					p--;
					continue;
				}
				else {
					while (p<lg && buffer[p]!='\\') p++;
					p++;
					while (p<lg && buffer[p]<=32) p++;                
					p--;
					continue;
				}
			}

			//dans le cas de la presence d'un nom de champ, on verifie
			//si il est la
			if (nomchamp!=NULL && (buffer[p]==nomchamp[0] || tolower(buffer[p])==nomchamp[0])) {
				int k=1;
				while (nomchamp[k]!=0 && (buffer[p+k]==nomchamp[k] || tolower(buffer[p+k])==nomchamp[k]) ) 
					k++;
				if (nomchamp[k]==0) {
					lg=p-1;
					break;
				}
			}
		}
	}


    
    firstsel=-1;
    lastsel=-1;
    for (;i<lg;i++) {
		//si le caractere est entre double cotes, on consomme les caracteres internes
		if (buffer[i]==34) {
			i++;
			while (i<lg && buffer[i]!=34) i++;
			continue;
		}

        if (buffer[i]=='/') {
            if (buffer[i+1]=='/') {
                while (i<lg && buffer[i]!=10 && buffer[i]!=13) i++;
                while (i<lg && buffer[i]<=32) i++;
                i--;
                continue;
            }
            else {
                while (i<lg && buffer[i]!='\\') i++;
                i++;
                while (i<lg && buffer[i]<=32) i++;                
                i--;
                continue;
            }
        }       				

        if (buffer[i]==10 || buffer[i]==13) {            
            while (buffer[i]==10 || buffer[i]==13) i++;
            i--;
        }
        else {
            if (firstsel==-1)
                firstsel=i;
            lastsel=i;						
        }
    }    
        
    for (i=lastsel;i>=firstsel;i--) {
        if (buffer[i]>=32) {
            lastsel=i+1;
            break;
        }
    }
}



void GlobalParseur::FormatRuleText(ostream& fichier,int idregle,char affiche_nom) {
    static char buffer[10000];
    
    
    if (VlabelVecteurIndex[idregle]==NULL || VlabelVecteurIndex[idregle]->dr==NULL)
        return;

	int index=VlabelVecteurIndex[idregle]->numero;
	if (VlabelVecteurIndex[index]==NULL)
		return;

    string ruletext;
	if ((affiche_nom & DISPLAY_RULESPACE) == DISPLAY_RULESPACE) {
		string rs;
        ruletext+="//--------------------------------------------------------------";
		ruletext+=Endl;
		if (VlabelVecteurIndex[index]->space!=NULL) {
			VlabelVecteurIndex[index]->space->chaine(rs);
			ruletext+="@RULESPACE=";
			ruletext+=rs;
			ruletext+=Endl;
		}
	}

    if ((affiche_nom & DISPLAY_INFORMATION) == DISPLAY_INFORMATION) {
		if ((affiche_nom & DISPLAY_RULESPACE) != DISPLAY_RULESPACE) {
			ruletext+="//--------------------------------------------------------------";
			ruletext+=Endl;
		}
		ruletext+="//In file";
        ruletext+=" : (";
		sprintf(buffer,"%d",idregle);
        ruletext+=buffer;
		if (VlabelVecteurIndex[idregle]->dr!=NULL)
			sprintf(buffer," line:%d) :",VlabelVecteurIndex[idregle]->dr->ligne);
		else
			strcpy(buffer,") :");
        ruletext+=buffer;
        ruletext+=VlabelVecteur[index]->dr->nom_fichier;
        ruletext+=Endl;
    }


    if (VlabelVecteur[index+1]!=NULL)
        readTextFromFile(VlabelVecteur[index]->dr,VlabelVecteur[index+1]->dr,index,buffer);
    else
        readTextFromFile(VlabelVecteur[index]->dr,NULL,index,buffer);
    int i=0;
    
    Trim(buffer);
    
    long firstsel=-1;
    long lastsel=-1;


    //If there is a change of rule in the same file
    DecompteRegle* dr=VlabelVecteur[index]->dr;    
    DecompteRegle* drnext=NULL;
    
    if (VlabelVecteur[index+1]!=NULL)
        drnext=VlabelVecteur[index+1]->dr;    

	FindRuleBoundaries((uchar*)buffer,firstsel,lastsel,dr,drnext);
    if ((affiche_nom & DISPLAY_COMMENTS) == 0) {
        for (i=firstsel;i<lastsel;i++) {
#ifdef WIN32
            if (buffer[i]==10)
                ruletext+="\r";
#endif
            ruletext+=buffer[i];
        }
    }
    else
        ruletext+=buffer;
    fichier<<ruletext<<Endl<<Endl;
}


void detruitnoeudfictif(Noeud* n) {
    if (n==NULL)
        return;

    detruitnoeudfictif(n->fils);
    detruitnoeudfictif(n->frere);

    if (n->source!=NULL) {
        if (n->source->voc!=NULL)
            delete n->source->voc;
        delete n->source;
    }
}


Label* GlobalParseur::rechercheLabel(char* label) {	
	map<string, Label*>::iterator test=ArbreLabel.find(label);
	if (test==ArbreLabel.end())
		return NULL;
	return (*test).second;
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::Nettoyage() {
	int i;
	if (!DESTRUCTION) return;

	if (DECOUPAGE!=NULL)
		libere(DECOUPAGE);

	if (affiche_regle_appliquee!=2)
		AfficheLesStats();
	statistiques.raz();
	VBases.raz();

	if (TasDeListes!=NULL)
		delete TasDeListes;

	if (spacename!=NULL)
		delete spacename;

	graphegeneration.nettoie();

#ifdef XIPLIBXML
	if (refdocument != NULL)
		xmlFreeDoc(refdocument);
	refdocument = NULL;
#endif

	TasDeListeVoc.nettoie();
	lectures.nettoie();
	LesTraitsdeGenerations.nettoie();
	vlarbredestraits.nettoie();

#ifdef XIPPYTHON
/*
	try {
		if (Py_IsInitialized()) {
			module_principal=NULL;
			Py_Finalize();
		}
	}
	catch(...) {
		*flot_erreur<<"Error on re-initializing Python..."<<Endl;
	}
*/
	//Py_DECREF(espace_local);
	PythonScript.raz();
	//espace_local=NULL;
#endif
	enregListeTags.nettoietable();   
	TousLesElementsRegles.nettoie();  

	vlex.raz();

	definircategories.nettoie();
	definirtraits.nettoie();

	if (TraitsFiltreOntologie!=NULL)
		delete TraitsFiltreOntologie;

	listegraphes.nettoie();


	/*
	VxlabelInference.nettoie();
	VxlabelSequence.nettoie();       
	VxlabelSimplifie.nettoie();        
	VxlabelMarqueur.nettoie();
	VxlabelDI.nettoie();        
	VxlabelReconstruction.nettoie();
	VxlabelTermes.nettoie();
	VxlabelCartesien.nettoie();
	*/

	VlabelVecteur.nettoie();
	VlabelVecteurIndex.raz();
	lesstats.nettoie();

	if (TraitsDependances!=NULL)
		delete TraitsDependances;    

	for (i=0;i<NoeudsFictifs.dernier;i++) {
		//Il s'agit d'un noeud brutalement cree pere et fils a la fois
		if (NoeudsFictifs[i]->voc!=NULL) {
			delete NoeudsFictifs[i]->N.fils->source;
			delete NoeudsFictifs[i]->voc;
		}
		else
			//Il faut detruire les noeuds de facon recursive
			detruitnoeudfictif(NoeudsFictifs[i]->N.fils);        
		delete NoeudsFictifs[i];
	}
	

	for (i=0;i<memoiresem.memoire.dernier;i++) {
		auto_vocabulaire_semantique* av=memoiresem.memoire[i];        
		if (av->semantique!=NULL) {
			av->semantique->Destruction(this);
			delete av->semantique;
			av->semantique=NULL;
		}
	}
	memoiresem.memoire.raz();


	for (i=0;i<FinDePhrase.dernier;i++)
		libere(FinDePhrase[i]);

	for (i=0;i<fichiersSemantiques.dernier;i++) {
		fichiersSemantiques[i]->close();
		delete fichiersSemantiques[i];
	}
	for (i=0;i<fichiersSemantiquesNoms.dernier;i++)
		libere(fichiersSemantiquesNoms[i]);

	delete contexteFoncVide;
	delete BaliseTraits;
	delete gBaliseTraits;
	delete gBaliseDebut;
	delete gBaliseSansFin;


	delete hierarchies;
	delete relations;
	delete concepts;



	for (i=0;i<codageXML.dernier;i++) {
		libere(codageXML[i]);
		libere(caractereXML[i]);
	}
	VariablesNoms.nettoie();
	if (MAJUSCULE !=NULL) 
		delete MAJUSCULE;
	if (TOUTMAJUSCULE !=NULL) 
		delete TOUTMAJUSCULE;

	if (MAJEURES!=NULL) 
		delete MAJEURES;

	INTERFONCTIONS.nettoie();
	PREMIERFONCTIONS.nettoie();
	SECONDFONCTIONS.nettoie();
	ReglesSimplifie.nettoie();

	for (i=0;i<4;i++) {
		Vregle[i].nettoie();
	}

	for (i=0;i<TailleHachageTraits;i++) 
		ListeDesTraits[i].nettoie();	

	ListeCoupeSequence.nettoie();
	declencheurs.raz();

	//Nettoyage de structures partagees
	ListeDesTestFonctions.raz();
	DebutDeductions.nettoie();
	PreBaliseDeductions.nettoie();
	PostBaliseDeductions.nettoie();
	FinDeductions.nettoie();
	QueueDeductions.nettoie();
	ChgtDeductions.nettoie();

	for (i=0;i<ListeDesProcedures.dernier;i++) {
		switch(ListeDesProcedures[i]->Type()) {
		case 0:        
			delete (VectTestFonction*)ListeDesProcedures[i];
			break;
		case 1:
			delete ListeDesProcedures[i];
			break;
		case 2:
			delete (VectTFUneInstruction*)ListeDesProcedures[i];
		}
	}
	ListeDesProcedures.raz();
	int p;

	for (i=0;i<MaxCouches;i++) {
		TableDesSimplifications[i].nettoie();
		TableProduitCartesien[i].nettoie();
		TableDesReconstructeurs[i].nettoie();

		Fragmentes[i].nettoie();
		ReglesTokenSelection[i].nettoie();

		ListeDesDeductions[i].nettoie();
		ListeDesDeductionsEnCouche[i].nettoie();
		Generations[i].nettoie();
		TableDesFiltresDeCouche[i].nettoie();

		PListes[i].nettoie(); 

		if (ListeOrdreNoeuds[i].dernier!=0)
			ListeOrdreNoeuds[i].nettoie();

		if (TableDesMarqueurs[i].dernier!=0)
			TableDesMarqueurs[i].nettoie();      

		for (int mp=0;mp<2;mp++) {
			if (motsmultiples[mp][i]!=NULL) {
				motsmultiples[mp][i]->nettoie();
				delete motsmultiples[mp][i];
				motsmultiples[mp][i]=NULL;
			}

			if (motsmultiplescomplexes[mp][i]!=NULL) {
				motsmultiplescomplexes[mp][i]->nettoie();
				delete motsmultiplescomplexes[mp][i];
				motsmultiplescomplexes[mp][i]=NULL;
			}
		}

		if (Termes[i]!=NULL)
			delete Termes[i];            			

		if (Semantiques[i][NON]!=NULL)
			delete Semantiques[i][NON]; 

		if (Semantiques[i][OUI]!=NULL)
			delete Semantiques[i][OUI]; 

		if (ArbVocComplexe[i]!=NULL) {
			ArbVocComplexe[i]->nettoie();
			delete ArbVocComplexe[i];
			ArbVocComplexe[i]=NULL;
		}


		if (ArbVoc[i]!=NULL) {
			ArbVoc[i]->nettoie();
			delete ArbVoc[i];
			ArbVoc[i]=NULL;
		}

		if (ArbSurfaceComplexe[i]!=NULL) {
			ArbSurfaceComplexe[i]->nettoie();
			delete ArbSurfaceComplexe[i];
			ArbSurfaceComplexe[i]=NULL;
		}
		
		if (ArbSurface[i]!=NULL) {
			ArbSurface[i]->nettoie();
			delete ArbSurface[i];
			ArbSurface[i]=NULL;
		}
		ArbReg[i].clear();
		EspaceLexique[i]=NULL;
	}

	if (traittaggerguessed != NULL)
		delete 	traittaggerguessed;
	if (traitcrfselection)
		delete traitcrfselection;

	ListeDesAvlRegles.nettoie();
	for (p=0;p<VariablesProceduresGlobales.lesetats.dernier;p++) {
		if (VariablesProceduresGlobales.lesetats[p] != NULL &&
			VariablesProceduresGlobales.lesetats[p]->valeur != NULL) {
				VECTA<TestFormule*>* vtform=
					(VECTA<TestFormule*>*)(VariablesProceduresGlobales.lesetats[p]->valeur);
				delete vtform;
			}
	}
	VariablesProceduresGlobales.nettoie();

	for (i=0;i< NbCats;i++) {
		for (int laCouche=0;laCouche<MaxCouches;laCouche++)
			if (TableDesSequences[laCouche][i]!=NULL)
				delete TableDesSequences[laCouche][i];
	}


	delete fournirResFonc;
	for (i=0;i<HACHE;i++)
		if (Traductions[i]!=NULL)
			delete Traductions[i];

	ListeFonctions.nettoie();
	Variables.nettoie();
	VariablesLemmes.raz();

	if (Abreviation!=NULL) delete Abreviation;
	if (Defauts!=NULL) delete Defauts;                        

	delete TOUSTRAITS;
	if (FILTRE!=NULL) 
		delete FILTRE;
	if (FILTREFONCTION!=NULL)
		delete FILTREFONCTION;
	if (FILTRENOEUD!=NULL) 
		delete FILTRENOEUD;
	if (FILTREGENERATION!=NULL) 
		delete FILTREGENERATION;

	if (ArbTraits!=NULL) 
		delete ArbTraits;
	if (CatVide != NULL &&
		CatVide->traits!=NULL)
		delete CatVide->traits;
	if (CatEpsilon != NULL &&
		CatEpsilon->traits!=NULL)
		delete CatEpsilon->traits;
	Categories.nettoie();
	Fonctions.nettoie();
	Procedures.nettoie();

	if (LEMME!=NULL)
		libere(LEMME);

	if (SURFACE!=NULL)
		libere(SURFACE);

	DESTRUCTION=0;
	if (iNombreDeValeurs && TableDesTraits!=NULL) {
		for (i=0;i<iNombreDeValeurs*2;i++)
			libere(TableDesTraits[i]);
		delete[] TableDesTraits;
	}

	for (i=0;i<Nom_divers.dernier;i++) 
		libere(Nom_divers[i]); 

	for (i=0;i<Nom_scripts.dernier;i++) 
		libere(Nom_scripts[i]); 

	for (i=0;i<Nom_traits.dernier;i++) 
		libere(Nom_traits[i]); 

	for (i=0;i<Nom_lexique.dernier;i++) 
		libere(Nom_lexique[i]); 

	for (i=0;i<Nom_regle.dernier;i++) 
		libere(Nom_regle[i]); 

	for (i=0;i<Nom_fonction.dernier;i++) 
		libere(Nom_fonction[i]);


	if (Sortie!=NULL) {
		Sortie->flush();
		delete Sortie;
	}

	if (Trace != NULL) {
		Trace->flush();
		delete Trace;
	}

	listeEtatArbre.arcs.nettoie();
	listeEtatArbre.etats.nettoie();
#ifdef XIPLIBXML
	delete 	basexmlcourante;
	if (xipdtd!=NULL)
		xmlFreeDtd(xipdtd);
	//xmlCleanupParser();
	BaseDonneesXML.nettoie();   
	InformationDtd.nettoie();
	xpathindex.nettoie();
	balisesinsertion.nettoie();
	alphrase.nettoie();
	ListeAlias.nettoie();
#endif


	LesBalises.nettoie();
	for (i=0;i<ExpressionsFinDePhrase.dernier;i++) {
		if (ExpressionsFinDePhrase[i]!=NULL) 
			ExpressionsFinDePhrase[i]->nettoie();
	}

	ExpressionsFinDePhrase.nettoie();
	ArbreLabel.clear();

	//Quelques verifs finales
	//int cperr=0;
	//int cpi;
	//TESTRAMASSAGE(cpi,cperr,Simplifie,"Simplifie");
	//TESTRAMASSAGE(cpi,cperr,Cartesien,"Cartesien");
	//TESTRAMASSAGE(cpi,cperr,VectTestFonction,"VectTestFonction");
	//TESTRAMASSAGE(cpi,cperr,ReconstruitArbre,"ReconstruitArbre");
	//TESTRAMASSAGE(cpi,cperr,SeqFonc,"SeqFonc");
	//TESTRAMASSAGE(cpi,cperr,Dependance,"Dependance");
	//TESTRAMASSAGE(cpi,cperr,TestArbre,"TestArbre");
	//TESTRAMASSAGE(cpi,cperr,Categorie,"Categorie");
	//TESTRAMASSAGE(cpi,cperr,Traits,"Traits");
	//TESTRAMASSAGE(cpi,cperr,arbretrait,"arbretrait");
	//TESTRAMASSAGE(cpi,cperr,trait,"trait");
	//TESTRAMASSAGE(cpi,cperr,CatTraits,"CatTraits");
	//TESTRAMASSAGE(cpi,cperr,ChaineCat,"ChaineCat");
	//TESTRAMASSAGE(cpi,cperr,auto_vocabulaire,"auto_vocabulaire");
	//TESTRAMASSAGE(cpi,cperr,avl_vocabulaire,"avl_vocabulaire");
	//TESTRAMASSAGE(cpi,cperr,avl_vocabulaire_semantique,"avl_vocabulaire_semantique");
	//TESTRAMASSAGE(cpi,cperr,auto_vocabulaire_semantique,"auto_vocabulaire_semantique");
	//TESTRAMASSAGE(cpi,cperr,vocabulaire_terme,"vocabulaire_terme");
	//TESTRAMASSAGE(cpi,cperr,vocabulaire_fragmente,"vocabulaire_fragmente");
	//TESTRAMASSAGE(cpi,cperr,ListeVoc,"ListeVoc");
	//TESTRAMASSAGE(cpi,cperr,avl_vocabulaire_terme,"avl_vocabulaire_terme");
	//TESTRAMASSAGE(cpi,cperr,TestFormuleElement,"TestFormuleElement");
	//TESTRAMASSAGE(cpi,cperr,Variable,"Variable");
	//TESTRAMASSAGE(cpi,cperr,TestFormule,"TestFormule");
	//TESTRAMASSAGE(cpi,cperr,TestFonction,"TestFonction");
	//TESTRAMASSAGE(cpi,cperr,ListeTestArbre,"ListeTestArbre");
	index_parseur_courant=iParseurCourant;
	VIDERAMASSE(TestFonction);
	VIDERAMASSE(multimots);
	VIDERAMASSE(TestFormule);
	VIDERAMASSE(TestFormuleElement);
	VIDERAMASSE(avl_vocabulaire_semantique);
	VIDERAMASSE(auto_vocabulaire_semantique);
	VIDERAMASSE(auto_vocabulaire);
	VIDERAMASSE(avl_vocabulaire);
	VIDERAMASSE(vocabulaire_terme);
	VIDERAMASSE(vocabulaire_fragmente);
	VIDERAMASSE(avl_vocabulaire_terme);

	//Nettoyage KIF
	if (kifloader!=NULL && kifloader->master==false) {
		delete kifloader;
		kifloader=NULL;
	}

	VIDERAMASSE(Variable);
	VIDERAMASSE(ChaineCat);
	VIDERAMASSE(Simplifie);
	VIDERAMASSE(Cartesien);
	VIDERAMASSE(VectTestFonction);
	VIDERAMASSE(ReconstruitArbre);
	VIDERAMASSE(SeqFonc);
	VIDERAMASSE(Dependance);
	VIDERAMASSE(TestArbre);
	VIDERAMASSE(Categorie);
	//VIDERAMASSE(Traits);
	VIDERAMASSE(trait);
	VIDERAMASSE(CatTraits);
	VIDERAMASSE(ListeTestArbre);

	raz(NON);      
}

/*----------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/


void calculpath(char* nom,string& noms) {
	string path;
	string name;
	//On recupere le nom du repertoire associe au fichier
	CutPathFromName(nom,path,name);
        
    
	string grm_path;
	GetVarName(0,grm_path);
	char basecheminabsolu[4096];
	char cheminabsolu[4096];
#ifdef WIN32
	_fullpath(basecheminabsolu,crypte_repertoire,4096);
	_fullpath(cheminabsolu,path.c_str(),4096);
#else
	realpath(crypte_repertoire,basecheminabsolu);
	realpath(path.c_str(),cheminabsolu);
#endif

	//nous sautons les deux premiers caracteres qui correspondent a "c:"
	//Nous initialisons avec notre chemin dans res
	noms=basecheminabsolu;
#ifdef WIN32
	noms+=cheminabsolu+2;
#else
	noms+=cheminabsolu;
#endif
	DirectoryCreation((char*)noms.c_str(), 0);
	noms+=SEP;
	noms+=name;
}

void duplicationfichier(string nom) {
    ifstream fichier((char*)nom.c_str(),modeOuverture);
    string noms;
	//On recupere le nom du repertoire associe au fichier

	calculpath((char*)nom.c_str(),noms);

	ofstream cryptee((char*)noms.c_str(),ios::binary);
	char c;
	*flot_erreur<<"On cree:"<<noms<<endl;    
	c=fichier.get();
    while (!fichier.eof()) {
        cryptee<<c;
		c=fichier.get();
    }
}


void GlobalParseur::DupliqueGrmFiles(char* nom) {

	char bufread[1000];
	string noms;
	calculpath(nom,noms);
    ifstream grm(nom,modeOuverture);
    if (grm.fail()) {
        sprintf(bufread,"Cannot open: %s",nom);
        lance_erreur(strdup(bufread));
    }
	
	ofstream grmsortie(noms.c_str());

//----------------------------------------------------------------
    while (!grm.eof()) {
        GrmTrim(grm,(uchar*)bufread,999,NON);
        
        if (!*bufread)
            break;

        if (!strcmp(bufread,"grammar:") && duplication_crypte==OUI)
			grmsortie<<"gram:"<<endl;
		else
			grmsortie<<bufread<<endl;
		GrmTrim(grm,(uchar*)bufread,999,NON);            
		grmsortie<<bufread<<endl<<endl;
	}

	if (Type()==PARSEURTRANS) {
		int ii;
		GlobalParseurTrans* gtrans=(GlobalParseurTrans*)this;
		for (ii=0;ii<gtrans->filenames.size();ii++)
			duplicationfichier(gtrans->filenames[ii]);
	}

	int i;
	for (i=0;i<Nom_divers.dernier;i++)
        duplicationfichier(Nom_divers[i]);
	for (i=0;i<Nom_scripts.dernier;i++)
        duplicationfichier(Nom_scripts[i]);
}


string GlobalParseur::CrypteFichier(char* nom) {
    bouclage=0;
    ifstream fichier(nom,modeOuverture);
    string noms;
	//On recupere le nom du repertoire associe au fichier
	if (duplication_crypte==DECRYPTAGE)
		cryptage=OUI;

	if (fichier.fail() ) {
		noms="Cryptage impossible pour:";
		noms+=nom;
		noms+=" Fichier inconnu";
		erreur((char*)noms.c_str());
	}

	if (duplication_crypte==NON) {
		string path;
		string name;
		CutPathFromName(nom,path,name);
		noms=crypte_repertoire;
		noms+=name;
		noms+=".crypte";
	}
	else 
		calculpath(nom,noms);

	ofstream cryptee((char*)noms.c_str(),ios::binary);
	char c;
	*flot_erreur<<"On cree:"<<noms<<"=";    
	if (duplication_crypte==EXTRAIRE)
		c=fichier.get();
	else
		c=LectCrypte(fichier);
    while (!fichier.eof()) {
        cryptee<<c;
		if (duplication_crypte==EXTRAIRE)
			c=fichier.get();
		else
			c=LectCrypte(fichier);
    }
    *flot_erreur<<bouclage<<endl;
	return noms;
}

/*-------------------------------------------------------------------------*/

//Cette fonction permet la factorisation des traits des regles...
//La liste ListeDesTraits contient ces traits...
//Cette factorisation autorise une gestion extrement compacte des traits.
//En effet, les listes de traits qui existent deja ne sont pas dupliquees.
Traits* GlobalParseur::FactoriseTraits(Traits* ts) {
    if (ts==NULL)
        return NULL;

    int i;
    appelFactorise++;

    //Table de hachage
    // Modified by Aaron 10/2006
    TVecteur code=ts->ClefHachage(TailleHachageTraits);

    for (i=0;i<ListeDesTraits[code].dernier;i++) {
		//En cas de double factorisation, on ne fait rien
		//ce pointeur a deja ete utilise
		if (ts==ListeDesTraits[code][i])
			return ts;
		//C'est la meme structure mais avec des pointeurs differents
        if (ListeDesTraits[code][i]->egaux(ts)) {
            delete ts;
            return ListeDesTraits[code][i];
        }
    }

    tailleFactorise++;
    ListeDesTraits[code].ajoute(ts);
    return  ts;
}

//------------------------------------------------------------------------
Liste* GlobalParseur::FactoriseNoeudFictif(Traits* ts,Categorie* c,char ajoutefils,char& ajouter) {
    Cat* X=&c->X;

    ajouter=OUI;
    if (ajoutefils==OUI) {
        for (int i=0;i<NoeudsFictifs.dernier;i++) {
            if (TraitsNoeudsFictifs[i]->egaux(ts) && NoeudsFictifs[i]->X==X) {
                delete ts;
                ajouter=NON;
                return NoeudsFictifs[i];
            }
        }
    }

    Liste* L=new Liste;
    L->traitsbase=ts;
    L->traitsactif=new Traits(ts);
    L->traitsbase->ou(Fictif);
    L->traitsactif->ou(Fictif);

    L->X=&c->X;
    L->N.surface=c->X.nom;
    L->N.num_gauche=-100;
    L->N.num_droite=-100;
    L->N.gauche=-100;
    L->N.droite=-100;
    L->N.X=&c->X;
    L->N.fictif=OUI;

    if (ajoutefils==OUI || ajoutefils==NONECRIT) {
        Liste* lfils=new Liste;
        lfils->N.gauche=-100;
        lfils->N.droite=-100;
        lfils->N.num_gauche=-100;
        lfils->N.num_droite=-100;
        lfils->N.fictif=OUI;
        lfils->traitsbase=new Traits(this);
        lfils->traitsactif=new Traits(this);
		lfils->traitsbase->ou(Fictif);
		lfils->traitsactif->ou(Fictif);

        ListeVoc* voc=new ListeVoc();
        L->voc=voc;
        voc->tr.ajoute(new Traits(ts));
        voc->X.ajoute(L->X);
        voc->nbX=1;
        char tampon[100];
        if (ts->lemme!=NULL) {
            ts->lemme->mot(tampon,100);
            lfils->N.lemme=DupliqueChaine(tampon);
            voc->lemme.ajoute(lfils->N.lemme);
        }
        else {
            lfils->N.lemme=c->X.nom;
            voc->lemme.ajoute(DupliqueChaine(c->X.nom));
        }
        
        if (ts->surface!=NULL) {
            ts->surface->mot(tampon,100);
            lfils->N.surface=DupliqueChaine(tampon);
            voc->surface=lfils->N.surface;
        }
        else  {
            if (ts->lemme!=NULL) {
                ts->lemme->mot(tampon,100);
                lfils->N.surface=DupliqueChaine(tampon);
                voc->surface=lfils->N.surface;
            }
            else {
                lfils->N.surface=c->X.nom;
                voc->surface=DupliqueChaine(c->X.nom);
            }
        }
        lfils->X=L->X;
        L->N.fils=&lfils->N;
        lfils->N.pere=&L->N;       
    }

    return L;
}
//---------------------------------------------------------------------------
Liste* GlobalParseur::CreeNoeudFragment(char* mot,Traits* arcts,Cat* X,int ngauche,int ndroit) {    

    static char buffer[1000];
    Categorie* c=ChercheCat(X->nom);
    Traits* ts;

    if (arcts==NULL)
        ts=new Traits(this);
    else
        ts=new Traits(arcts);

    ts->ou(c->traits);
    Liste* L=TasDeListes->fournir(this,X,ts);
    L->N.surface=X->nom;
    L->N.fictif=OUI;
    L->N.gauche=ngauche;
    L->N.droite=ndroit;
    L->N.num_gauche=ngauche;
    L->N.num_droite=ndroit;
    L->traitsbase->ou(Fictif);
	L->traitsactif->ou(ts);
    L->traitsactif->ou(Fictif);
    Liste* lfils=TasDeListes->fournir(this,X,ts);
    lfils->traitsbase->ou(Fictif);
	lfils->traitsactif->ou(ts);
    lfils->traitsactif->ou(Fictif);
    lfils->N.fictif=OUI;
    lfils->N.gauche=ngauche;
    lfils->N.droite=ndroit;
    lfils->N.num_gauche=ngauche;
    lfils->N.num_droite=ndroit;
    ListeVoc* voc=fournirListeVocVide();
	phrasefictifs.ajoute(voc);	
    L->voc=voc;
    lfils->voc=voc;
    voc->tr.ajoute(ts);
    voc->X.ajoute(L->X);
    voc->nbX=1;
    L->N.fils=&lfils->N;
    lfils->N.pere=&L->N;       

	if (mot==NULL) {
		char erreur=NON;        
		buffer[0]=0;
		VECTA<Noeud*> vnoeuds;				
		if (arcts->lemme!=NULL)
			arcts->lemme->mot(buffer,1000);
		else {
			if (arcts->varlemme!=NULL) {
				string bufflemme;
				arcts->varlemme->TraitementFormuleString(this,vnoeuds,erreur,bufflemme);
				if (erreur==OUI)
					sprintf(buffer,"?");
				else {
					strcpy(buffer,bufflemme.c_str());
					buffer[bufflemme.length()]=0;
				}
			}
			else
				strcpy(buffer,"?");
		}
		voc->lemme.ajoute(DupliqueChaine(buffer));

		if (arcts->surface!=NULL) {
			buffer[0]=0;
			arcts->surface->mot(buffer,1000);
		}
		else {
			if (arcts->varsurface!=NULL) {
				buffer[0]=0;
				erreur=NON;        
				string buffsurface;
				arcts->varsurface->TraitementFormuleString(this,vnoeuds,erreur,buffsurface);
				if (erreur==OUI)
					strcpy(buffer,"?");
				else {
					strcpy(buffer,buffsurface.c_str());
					buffer[buffsurface.length()]=0;
				}
			}
		}		
		voc->surface=DupliqueChaine(buffer);        
	}
	else {
		//Le premier est directement utilise
		if (ts->surface==NULL)
			voc->surface=DupliqueChaine(mot);
		else {
			buffer[0]=0;
			arcts->surface->mot(buffer,1000);
			voc->surface=DupliqueChaine(buffer);        
		}

		//le second pour le lemme est duplique
		if (ts->lemme==NULL)
			voc->lemme.ajoute(DupliqueChaine(mot));
		else {
			buffer[0];
			arcts->lemme->mot(buffer,1000);
			voc->lemme.ajoute(DupliqueChaine(buffer));
		}
	}

	lfils->N.surface=voc->surface;
	lfils->N.lemme=voc->lemme[0];        
        
    return L;
}
//---------------------------------------------------------------------------
Liste* GlobalParseur::DupliqueNoeud(Liste* noeud) {    


    Liste* L=TasDeListes->fournir(this,noeud->X,noeud->traitsbase);
    L->N.surface=noeud->N.surface;
    L->N.gauche=noeud->N.gauche;
    L->N.droite=noeud->N.droite;
    L->N.num_gauche=noeud->N.num_gauche;
    L->N.num_droite=noeud->N.num_droite;
	L->traitsactif->recopie(noeud->traitsactif);

	if (noeud->voc!=NULL) {
		Liste* lfils=TasDeListes->fournir(this,noeud->X,noeud->traitsbase);
		lfils->traitsactif->recopie(noeud->traitsactif);
		lfils->N.gauche=noeud->N.gauche;
		lfils->N.droite=noeud->N.droite;
		lfils->N.num_gauche=noeud->N.num_gauche;
		lfils->N.num_droite=noeud->N.num_droite;

		ListeVoc* voc=fournirListeVocVide();
		phrasefictifs.ajoute(voc);	
		L->voc=voc;
		lfils->voc=voc;


		voc->nbX=noeud->voc->nbX;
		voc->surface=DupliqueChaine(noeud->voc->surface);        
		for (int i=0;i<noeud->voc->nbX;i++) {
			voc->tr.ajoute(new Traits(noeud->voc->tr[i]));
			voc->X.ajoute(noeud->voc->X[i]);
			voc->lemme.ajoute(DupliqueChaine(noeud->voc->lemme[i]));
		}

		lfils->N.surface=voc->surface;
		lfils->N.lemme=voc->lemme[0];

		L->N.fils=&lfils->N;
		lfils->N.pere=&L->N;       
	}
    return L;
}
/*-------------------------------------------------------------------------*/
extern const char* CHAINESFR[];
extern const char* CHAINESEN[];

void GlobalParseur::Commentaires(char* nom,typeLangue langue) {

    CHAINES.raz();
    int i=0;

    if (langue==FRANCAIS) {
        while (strcmp(CHAINESFR[i],"@"))
            CHAINES.ajoute((char*)CHAINESFR[i++]);
    }
    else {
        while (strcmp(CHAINESEN[i],"@"))
            CHAINES.ajoute((char*)CHAINESEN[i++]);
    }        
    laLangue=langue;
}

/*--------------------------------------------------------------------------*/
char CheminComplet(char* chemin) {
#ifdef WIN32
    if (chemin[1]==':')
        return OUI;
    if (chemin[0]=='\\')
        return OUI;
    return NON;
#endif

    if (chemin[0]==SEP)
        return OUI;
    return NON;
}

    
/*-------------------------------------------------------------------------*/
void GlobalParseur::ChainagePlusieurs(istream& fichier,
                                      char* ChaineTest,
                                      char* tampon,
                                      char& k,
                                      VECTA<char*>& noms,
                                      char entrelacement,
                                      long& position) {
    char nomFichier[2048];
	char lregle=NON;
     
    if (tampon[0]==0)
        k=Llit(fichier,tampon);
    
    position=fichier.tellg();
    
    if (tampon[0]==0 && k==':')
        return;

    if (strcmp(tampon,ChaineTest) || k!=':')
        erreur(Concatene(CHAINES[94],tampon));

	if (!strcmp(ChaineTest,CHAINES[20])) {
		posreglesdansfichier=fichier.tellg();
		lregle=OUI;
		basenomsregles.clear();
	}
	

    //Une liste se finit par un ";"
	static char inter[1000];
    while (k!=';') {    
        k=litChemin(fichier,tampon);
		if (k==',' && tampon[0]==0) {
			*flot_erreur<<"Please check your file, there might be one comma too many on line: "<<LIGNESERREUR<<" in "<<NOM<<endl;
			continue;
		}
		if (tampon[0] == 0) {
			if (k == ':')
				break;
			continue;
		}
		//On garde les chemins relatifs dans le cas des regles pour la reconstitution du fichier langue.xip
		//en cas de cryptage partiel
		if (lregle==OUI)
			basenomsregles.push_back(tampon);

        if (entrelacement==OUI) {
            int type_adressage=ADRESSAGE_ABSOLU;
            if (tampon[0]=='+') {
                type_adressage=ADRESSAGE_RELATIF;
				strcpy(inter,tampon+1);
				strcpy(tampon,inter);
            }

			if (tampon[0]=='@') {
				strcpy(inter,tampon+1);
				strcpy(tampon,inter);
                type_adressage|=FICHIER_NON_CRYPTE;
            }
            
            if (tampon[0]=='%') {
				strcpy(inter,tampon+1);
				strcpy(tampon,inter);
                type_adressage|=FICHIER_CRYPTE;
            }
            
            ReglesEntrelacees.ajoute(type_adressage);
        }
        
        NormaliseNomFichier(nomFichier,tampon,repertoire);

        if (entrelacement==2) { //Cas particulier pour la lecture des modules...
            strcpy(nomFichier,tampon);
            Trim(tampon);
        }
        else
            strcat(nomFichier,tampon);
        noms.ajoute(DupliqueChaine(nomFichier));

        if (k==':') {
            tampon[0]=0;
            return;
        }

        if (k!=',') {
            tampon[0]=k;
            k=Llit(fichier,tampon+1);
            return;
        }

    }
    tampon[0]=0;
}
/*-------------------------------------------------------------------------*/
void GlobalParseur::Chainage(istream& fichier,char* ChaineTest,
                             char* nom,char* tampon,char k) {
    char tamp[100];
     
     
    if (strcmp(tampon,ChaineTest) || k!=':')
        erreur(Concatene(CHAINES[94],tampon));
    
    k=Llit(fichier,tamp);
    if (k!='.') 
        erreur(CHAINES[12]);

    strcpy(nom,tamp);    

    k=Llit(fichier,tamp);
    strcat(nom,".");
    strcat(nom,tamp);
}

/*-------------------------------------------------------------------------*/
char GlobalParseur::LectOuiNon(istream& fichier,char* tmp,
                               char* chaine,char* reponse) {
    char k,tampon[300];
    
    if (!strcmp(tmp,chaine)) {
        k=Llit(fichier,tampon);
        if (!strcmp(tampon,CHAINES[95])) 
            *reponse=OUI;
        else
            if (!strcmp(tampon,CHAINES[96])) 
                *reponse=NON;
            else 
                erreur(Concatene(CHAINES[13],tampon));
        return OUI;
    }    
    return NON;
}

/*-------------------------------------------------------------------------*/

void GlobalParseur::VerifieEtCompare(char* tampon) {
    struct tm *newtime;
    time_t long_time;
    
    time( &long_time );                /* Get time as long integer. */
    newtime = localtime( &long_time ); /* Convert to local time. */

    int anneE;
    int moiS;
    int jouR;
    //On recherche le premier chiffre
    int i=0;
    while (tampon[i]<'0' && tampon[i]>'9') i++;
    char* pt=strchr(tampon+i,'/');
    if (pt==NULL)
        erreur((char*)"Bad date format: error");
    anneE=atoi(tampon+i)-1900; //la valeur de l'annee est 100 pour 2000...
    pt++;
    char* suite=strchr(pt,'/');
    *suite=0;
    moiS=atoi(pt)-1;
    jouR=atoi(suite+1);
    
    if (anneE > newtime->tm_year)
        return;
    if (anneE == newtime->tm_year) {
        if (moiS > newtime->tm_mon)
            return;
        if (moiS == newtime->tm_mon) {
            licenseTest=jouR-newtime->tm_mday;
            *flot_erreur<<"License will expired in: "<<licenseTest<<". Please contact Xerox to renew..."<<Endl;
            if (jouR >= newtime->tm_mday)
                return;
        }
    }
    erreur((char*)"License has Expired...");
}
 
/*--------------------------------------------------------------------------*/
//Decoupe une chaine en sous chaine selon le caractere de coupure
//Est capable d'interpretrer une chaine: #12#23#45
void codesplit(char* t, VECTA<char*>& r,char coupure) {
	char* pt;
	char* curseur=t;
	int i;
	Trim(curseur);
	pt=strchr(curseur,coupure);
	while (pt!=NULL) {
		*pt=0;
		if (curseur[0]=='\\') {
			VECTA<char*> code;
			char cc[100];
			codesplit(curseur+1,code,'\\');
			for (i=0;i<code.dernier;i++)
				cc[i]=atoi(code[i]);
			cc[i]=0;
			r.ajoute(DupliqueChaine(cc));
		}
		else
			r.ajoute(DupliqueChaine(curseur));
		*pt=coupure;
		curseur=pt+1;
		pt=strchr(curseur,coupure);
	}
	if (curseur[0]=='\\') {
		VECTA<char*> codef;
		char ccf[100];
		codesplit(curseur+1,codef,'\\');
		for (i=0;i<codef.dernier;i++)
			ccf[i]=atoi(codef[i]);
		ccf[i]=0;
		r.ajoute(DupliqueChaine(ccf));
	}
	else
		r.ajoute(DupliqueChaine(curseur));
}

void GlobalParseur::TestUtf8Fichier(FILE* f) {
	//On garde la position dans le fichier
	long pos=ftell(f);
	fclose(f);
	ifstream fichier(fichier_en_cours,modeOuverture);
	string ligne;
	bool utf=false;
	
	while (!fichier.eof()) {
		getline(fichier,ligne);
		if (s_is_utf8(USTR(ligne),ligne.size())==true) {
			utf=true;
			break;
		}
	}

	if (utf) {
		ParametresAffichage|=ENTREE_UTF8;
		ParametresAffichageDefini|=ENTREE_UTF8;
	}
	else {
		ParametresAffichage&=~ENTREE_UTF8;
		ParametresAffichageDefini&=~ENTREE_UTF8;
	}

	fichier.close();
	f=fopen(fichier_en_cours,"rb");
	fseek(f,pos,SEEK_SET);
	fichierDonnees=f;
}

void GlobalParseur::TestUtf8Chaine(uchar* chaine) {
	if (s_is_utf8(chaine,strlen((char*)chaine))==true) {
		ParametresAffichage|=ENTREE_UTF8;
		ParametresAffichageDefini|=ENTREE_UTF8;
		return;
	}

	ParametresAffichage&=~ENTREE_UTF8;
	ParametresAffichageDefini&=~ENTREE_UTF8;
}


/*-------------------------------------------------------------------------*/
void GlobalParseur::LitNomFichiers(void) {
	char k,tampon[300];
	bouclage=0; 
	ifstream fichier(Nom_Fichier,modeOuverture);
	int taille;


	if (fichier.fail() ) {
		strcpy(tampon,CHAINES[97]);
		strcat(tampon,Nom_Fichier);
		erreur(tampon);
	}
	LIGNES=0; 
	POSITION_DANS_FICHIER=0;
	NOM=Nom_Fichier;
	LIGNESERREUR=0;     

	VECTA<char*> CalculClef;
	//On extirpe le nom du fichier
	int rec=strlen(Nom_Fichier)-1;
	char UtiliseLimite[300];
	while (rec>=0 && Nom_Fichier[rec]!='/' && Nom_Fichier[rec]!='\\') rec--;
	strcpy(UtiliseLimite,Nom_Fichier+rec+1);
	Trim(UtiliseLimite);

	k=Llit(fichier,tampon);    
	if (strcmp(tampon,"license"))
		erreur((char*)"No license found... Error..");
	//On lit le reste de la ligne
	LitLigne(fichier,tampon);
	Trim(tampon);
	CalculClef.ajoute(DupliqueChaine(tampon));

	tampon[0]=0;
	k=0;
	long debut_sans_interet=0;
	ChainagePlusieurs(fichier,(char*)"modules",tampon,k,CalculClef,2,debut_sans_interet);

	//On initialise les modules
	int imo;
	for (imo=1;imo<CalculClef.dernier;imo++) {
		if (!strcmp(CalculClef[imo],(char*)"chunker"))
			ChunkerLicense=OUI;
		else
			if (!strcmp(CalculClef[imo],(char*)"inference"))
				DeductionLicense=OUI;
			else
				if (!strcmp(CalculClef[imo],(char*)"extraction"))
					DeductionLicense=OUI;
				else
					if (!strcmp(CalculClef[imo],(char*)"semantic"))
						SemantiqueLicense=OUI;
					else
						if (!strcmp(CalculClef[imo],(char*)"server"))
							ServeurLicense=OUI;
						else
							if (!memcmp(CalculClef[imo],(char*)"limited",7)) {
								CalculClef.ajoute(DupliqueChaine(UtiliseLimite));
								UsageLimitee=USAGE_LIMITE;
								max_controle_regle=atoi(CalculClef[imo]+7);
								if (max_controle_regle<10)
									max_controle_regle=10;
							}
							else
								if (!strcmp(CalculClef[imo],(char*)"noparameter")) {
									CalculClef.ajoute(DupliqueChaine(UtiliseLimite));
									UsageLimitee=USAGE_INTERDIT;
								}
	}

	//On lit maintenant la date d'expiration en format americain: Annee/mois/jour
	if (strcmp(tampon,"expiration"))
		erreur((char*)"No expiration date found... Error..");     
	LitLigne(fichier,tampon);
	Trim(tampon);
	strcpy(DateLimite,tampon);
	CalculClef.ajoute(DupliqueChaine(tampon));     
	if (duplication_crypte!=DECRYPTAGE)	
		VerifieEtCompare(tampon);

	k=Llit(fichier,tampon);     
	//On lit maintenant la date d'expiration en format americain: Annee/mois/jour
	if (strcmp(tampon,"code"))
		erreur((char*)"No code found... Error..");     
	LitLigne(fichier,tampon);
	Trim(tampon);
	strcpy(CodeClef,tampon);
	CalculClef.ajoute(DupliqueChaine(tampon));
	if (duplication_crypte!=DECRYPTAGE)	 {
		if (CheckCRC(CalculClef)==NON)
			erreur((char*)"Invalid License Number... Please contact Xerox to update your license...");
	}

	//On nettoie cette structure...
	for (imo=0;imo<CalculClef.dernier;imo++)
		libere(CalculClef[imo]);
	CalculClef.raz();

#ifdef XIPLIBXML     
	char basedoc[]="<basedoc/>";
	//On cree la racine de notre document
	refdocument=xmlSAXParseMemory(NULL,basedoc,strlen(basedoc),0);
	refdocument->encoding=xmlStrdup((const unsigned char*)encodage);
#endif

	char pretrait=NON;
	X_SIZE=500;
	Y_SIZE=50;
	MaxDoublons=1024;
	NEGATION='~';
	VECTA<char*> hosts;
	char nombrelu=NON;

	while (1) {
		k=Llit(fichier,tampon);
		if (!strcmp(tampon,"hosts")) {
			ChainagePlusieurs(fichier,(char*)"hosts",tampon,k,hosts,2,debut_sans_interet);
#ifdef WIN32
			hosts.nettoie();
#else
			char trouve=0;
			unsigned long id=gethostid();
			char nom_host[100];
			sprintf(nom_host,"%x",id);
			for (imo=0;imo<hosts.dernier;imo++) {
				if (!strcmp(hosts[imo],nom_host)) {
					trouve=1;
					break;
				}
			}
			hosts.nettoie();
			if (trouve==0)
				erreur("WRONG MACHINE");
			continue;
#endif
		}

		if (!strcmp(tampon,"version")) {
			LitLigne(fichier,tampon);
			Trim(tampon);
			//Lecture de la version, c'est une simple chaine
			strcpy(version_grammaire,tampon);
			continue;
		}

		if (!strcmp(tampon,"guesserfeature")) {
			LitLigne(fichier,tampon);
			Trim(tampon);
			//Lecture de la version, c'est une simple chaine
			strcpy(guesser_feature,tampon);
			continue;
		}


		if (!strcmp(tampon,"language")) {
			k=Llit(fichier,tampon);
			if (!strcmp(tampon,"english")) {
				strcpy(commentaire,repertoire);
				strcat(commentaire,"strings.file");
				Commentaires(commentaire,ANGLAIS);
			}
			else {
				if (!strcmp(tampon,"french")) {
					strcpy(commentaire,repertoire);
					strcat(commentaire,"chaines.fic");
					Commentaires(commentaire,FRANCAIS);
				}
			}
			continue;
		}

		if (!strcmp(tampon, "onguessed")) {
			LitLigne(fichier, tampon);
			Trim(tampon);
			strcpy(GUESSED_TAG, tampon);
			continue;
		}

		if (!strcmp(tampon,"grammarlanguage")) {
			LitLigne(fichier,tampon);
			Trim(tampon);
			//On verifie s'il s'agit d'une langue connue
			int i=0;
			langue_de_la_grammaire=NULL;
			while (langues_reconnaissables[i][0]!='@') {
				if (!strcmp(langues_reconnaissables[i],tampon)) {
					langue_de_la_grammaire=langues_reconnaissables[i];
					break;
				}
				i++;
			}

			if (langue_de_la_grammaire==NULL)
				*flot_erreur<<"Warning... Unknown language:"<<tampon;

			continue;
		}

		//Lecture des codes de detection utf8 (correspondant a une langue donnee)
		if (!strcmp(tampon,"testutf8")) {
			LitLigne(fichier,tampon);
			//il s'agit donc de code sur un ou n caracteres:  a b #12#13 #23#25 #244#344#455 
			continue;
		}

		//lecture de locale
		if (!strcmp(tampon,"locale")) {
			k=Llit(fichier,tampon);
			setlocale(LC_CTYPE,(const char*)tampon);
			strcpy(LaLocale,tampon);
			*flot_erreur<<"LOCALE:"<<tampon<<endl;
			continue;
		}

		//lecture du charset
		if (!strcmp(tampon,"charset")) {
			k=Llit(fichier,tampon);
			strcpy(encodage,tampon);
			*flot_erreur<<"CHARSET:"<<tampon<<endl;
			continue;
		}

		//Il s'agit de l'encodage des lexiques a la XIP...
		//On va convertir les lexiques et les tests sur LEMME et SURFACE en UTF8
		//On definit aussi depuis quel encodage latin on doit partir (1 signifie latin-1 etc.)
		if (!strcmp(tampon, "convert_lexicon_to_utf8_from_latin")) {
			k = Llit(fichier, tampon);
			agnostring tmp(tampon);
			if (tmp.isdigit()) {
				tablelatin = tmp.tonumber();
				gAfficherResultats |= TEST_INPUT_UTF8;
				gAfficherResultatsDefini |= TEST_INPUT_UTF8;
			}
			continue;
		}

		//lecture du charset de la grammaire
		if (!strcmp(tampon,"externalcharset") || !strcmp(tampon,"fstcharset")) {
			k=Llit(fichier,tampon);
			if (!strcmp(tampon,"latin"))
				encodage_fst=LATIN_ENCODING;
			else
				if (!strcmp(tampon,"utf8"))
					encodage_fst=UTF8_ENCODING;
				else
					erreur(CHAINES[368]);
			*flot_erreur<<"EXTERNAL LEXICON CHARSET:"<<tampon<<endl;
			continue;
		}

		//lecture du charset de la grammaire
		if (!strcmp(tampon,"grammarcharset")) {
			k=Llit(fichier,tampon);
			if (!strcmp(tampon,"latin"))
				encodage_grammaire=LATIN_ENCODING;
			else
				if (!strcmp(tampon,"utf8")) {
					encodage_grammaire=UTF8_ENCODING;
					encodageutf8=true;
				}
				else
					erreur(CHAINES[368]);
			*flot_erreur<<"GRAMMAR CHARSET:"<<tampon<<endl;
			continue;
		}

		//lecture du charset des chaines en entree
		//Le fonctionnement est le suivant:
		//Le resultat des analyses par les transducteurs n'est pas forcement en ISO-Latin
		//S'il y a une difference entre les deux sorties, il nous faut le gerer
		if (!strcmp(tampon, "sourcecharset")) {
			k = Llit(fichier, tampon);
			if (!strcmp(tampon, "latin"))
				encodage_source = LATIN_ENCODING;
			else
			if (!strcmp(tampon, "utf8"))
				encodage_source = UTF8_ENCODING;
			else
				erreur(CHAINES[368]);
			*flot_erreur << "SOURCE CHARSET:" << tampon << endl;
			continue;
		}

		if (LectOuiNon(fichier,tampon,(char*)"hmm",&pretrait)==OUI) 
			continue;
		if (LectOuiNon(fichier,tampon,(char*)"reentrance",&reentrance)==OUI)
			continue;

		//Lecture du maximum autorise de mots analyse en une seule passe
		if (!strcmp(tampon,CHAINES[278])) {
			k=Llit(fichier,tampon);
			nbmotsmaximum=(short)ChaineVersNombre(tampon);         
			continue;
		}

		if (!strcmp(tampon,"loopmax")) {
			k=Llit(fichier,tampon);
			max_iteration=(short)ChaineVersNombre(tampon);         
			continue;
		}

		if (!strcmp(tampon,CHAINES[41])) {
			k=Llit(fichier,tampon);
			taille_mot=(short)ChaineVersNombre(tampon);
			enregListeTags.nettoietable();
			ListeTagsInitialisation();
			continue;
		}


		if (LectOuiNon(fichier,tampon,CHAINES[100],&TRACE)==OUI)
			continue;
		if (LectOuiNon(fichier,tampon,CHAINES[101],&TRACENOEUD)==OUI)
			continue;
		if (LectOuiNon(fichier,tampon,CHAINES[159],&ARBREIND)==OUI)
			continue;

		if (!strcmp(tampon,"rulebuffer")) {
			k=Llit(fichier,tampon);
			MAXVIRTUEL=ChaineVersNombre(tampon);
			memoiresem.ajuste(MAXVIRTUEL);
			continue;
		}			 

		//Lecture de Nombre... Obligatoire
		if (!strcmp(tampon,CHAINES[102]) && k==':') {
			k=Llit(fichier,tampon);
			int taillevect=(short)ChaineVersNombre(tampon);
			//iTailleVect peut avoir ete initialisee de facon externe sur la ligne de commande
			if (iTailleVect==-1)
				iTailleVect=taillevect;

			iBorneVect=gBitVect*iTailleVect;
			taille=(iBorneVect+1)*2;
			TableDesTraits=new char*[taille];
			nombrelu=OUI;
			continue;
		}			 

		//Lecture de Doublons (optionnel)
		if (!strcmp(tampon,CHAINES[103]) && k==':') {
			k=Llit(fichier,tampon);
			MaxDoublons=ChaineVersNombre(tampon);
			continue;
		}

		//Lecture du signe de negation, optionnel
		if (!strcmp(tampon,CHAINES[17]) && k==':') {
			NEGATION=Llit(fichier,tampon);
			continue;
		}

		//Lecture des tailles de matrice d'affichage
		if ((tampon[0]=='x' || tampon[0]=='X') && k==':') {
			k=Llit(fichier,tampon);
			X_SIZE=ChaineVersNombre(tampon);
			continue;
		}


		if ((tampon[0]=='y' || tampon[0]=='Y') && k==':') {
			k=Llit(fichier,tampon);
			Y_SIZE=ChaineVersNombre(tampon);
			continue;
		}

		//Lecture du signe de decoupage, optionnel: categorysplit ou découpecatégorie
		if (!strcmp(tampon,CHAINES[366]) && k==':') {
			litBis(fichier,tampon);
			DECOUPAGE=DupliqueChaine(tampon);
			continue;
		}

		string err=tampon;
		if (err==CHAINES[106] || err==CHAINES[100] || err==CHAINES[108])
			break;
		err=CHAINES[217];
		err+=tampon;
		erreur((char*)err.c_str());
	}

	if (nombrelu==NON)
		erreur(CHAINES[15]);

	Pretraitement(pretrait);
	*flot_erreur<<Endl<<"Reentrance:"<<(int)reentrance<<Endl;
	*flot_erreur<<Endl<<"Rulebuffer:"<<MAXVIRTUEL<<Endl;
	*flot_erreur<<Endl<<"Maximum words in a sentence:"<<nbmotsmaximum<<Endl<<Endl;
	*flot_erreur<<Endl<<"Maximun size of a word:"<<taille_mot<<Endl<<Endl;


	//Lecture du nom du fichier d'indentation
	Chainage(fichier,CHAINES[106],Nom_Sortie,tampon,k);     

	//Lecture du nom du fichier de trace
	k=Llit(fichier,tampon);
	Chainage(fichier,CHAINES[100],Nom_Trace,tampon,k);     


	//Modification pour une lecture de plusieurs fichiers sur la ligne
	//separee par une virgule
	tampon[0]=0;
	k=0;

	ChainagePlusieurs(fichier,CHAINES[108],tampon,k,Nom_traits,NON,debut_traits_dans_langue);
	ChainagePlusieurs(fichier,CHAINES[109],tampon,k,Nom_lexique,NON,debut_lexique_dans_langue);
	ChainagePlusieurs(fichier,CHAINES[20],tampon,k,Nom_regle,OUI,debut_regles_dans_langue);     
	ChainagePlusieurs(fichier,CHAINES[192],tampon,k,Nom_fonction,NON,debut_sans_interet);     

	if (laTrace==1 || laTrace==11) {
		TRACE=OUI;
		TRACENOEUD=OUI;
	}

	if (laTrace>=10)
		ARBREIND=OUI;

}

	 
/*-------------------------------------------------------------------------*/
void GlobalParseur::LitReg(char* nom_regle) {
    char k,tampon[300];
    bouclage=0;
    ifstream fichier(nom_regle,modeOuverture);
    LIGNES=0; 
    POSITION_DANS_FICHIER=0;
    LIGNESERREUR=0;
    NOM=nom_regle;
    int num=0;    
    char metendl=OUI;

	if (fichier.fail() || strlen(nom_regle)<=1 || nom_regle[strlen(nom_regle) - 1] == SEP) {
        strcpy(tampon,CHAINES[97]);
        strcat(tampon,nom_regle);
        erreur(tampon);
    }
    else {
        int i=strlen(nom_regle)-1;
        while (i>=0 && nom_regle[i]!=SEP) i--;
        nom_fichier_courant=nom_regle;
        strcpy(nom_fichier,nom_regle+i+1);
        k=Llit(fichier,tampon);

#ifdef XIPPYTHON
        importation_python=NON;
        if (k=='"' && tampon[0]==0) {
            //nous sommes dans un espace Python
            k=Llit(fichier,tampon);
            if (strcmp(tampon,"python"))
                erreur(CHAINES[217]);
            importation_python=OUI;
            metendl=NON;
        }
        else
            if (!strcmp(tampon,"python"))
                metendl=NON;
#endif
        
        MinProfondeurAnalyse=MaxCouches;
        *flot_erreur<<"Loading file:"<<nom_regle<<" from layer:";
        ChoixLectureRegle(fichier,tampon);
        if (MinProfondeurAnalyse==MaxCouches)
            MinProfondeurAnalyse=MaxProfondeurAnalyse;
        *flot_erreur<<MinProfondeurAnalyse<<" to "<<MaxProfondeurAnalyse;
        if (metendl==OUI)
            *flot_erreur<<Endl;

    }
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::LitVoc(char* nom_lexique,char lire_couche) {
    char k;
    char tampon[300];
    bouclage=0;
    ifstream fichier(nom_lexique,modeOuverture);
    LIGNES=0; 
    POSITION_DANS_FICHIER=0;
    LIGNESERREUR=0;
    NOM=nom_lexique;
 
 
    if (fichier.fail() ) {
        strcpy(tampon,CHAINES[97]);
        strcat(tampon,nom_lexique);
        erreur(tampon);
    }
    else {
        *flot_erreur<<"Loading lexicon:"<<nom_lexique<<endl;
        k=Llit(fichier,tampon);
        if (!strcmp(tampon,CHAINES[110]) && k==':') 
            while(ConsVoc(fichier,lire_couche));
        else
            erreur(Concatene(CHAINES[94],tampon));
    }
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::ConsAffiche(istream& fichier,Vecteur** leFiltre) {
    char k,tampon[300];
    trait* vr;
	char ajoute=NON;

    k=lit(fichier,tampon);
	if (tampon[0]=='+')
		ajoute=OUI;
    
	Vecteur* filtre;
	if (!strcmp(tampon,"full") || !strcmp(tampon,"FULL")) {
		//Dans ce cas, on genere une structure ou tous les bits sont a 1...
		if (*leFiltre==NULL)
			*leFiltre=new Vecteur(iTailleVect);
		
		filtre=*leFiltre;
		for (int i=0;i<iTailleVect;i++)
			filtre->vecteur[i]=-1;
		return;
	}
    
	if (k!='[')
        erreur(CHAINES[111]);
	filtre=new Vecteur(iTailleVect);
	while (k!=']') {
        k=lit(fichier,tampon);

        if (k==']') {
            if (tampon[0]==0)
                break;
        }
        else
            if (k!=',') 
                erreur(CHAINES[112]);

        vr=ArbTraits->recherche(tampon);
        if (!vr) 
            erreur(Concatene(CHAINES[113],tampon));
        filtre->ou(vr->valeur);
    }

	if (*leFiltre!=NULL) {
		if (ajoute==OUI) {
			(*leFiltre)->ou(filtre);
			delete filtre;
			return;
		}
		delete *leFiltre;
	}
    *leFiltre=filtre;
}
    
/*-------------------------------------------------------------------------*/

char GlobalParseur::ConsAtt(istream& fichier) {
    char k=0,tampon[300];
 
    k=Llit(fichier,tampon); 

    if (k==':') 
        erreur(CHAINES[115]);
    
    if (k=='[') {
        ConstAttributs(fichier,k);
        while (k==',')
            ConstAttributs(fichier,k);        
    }

    if (k!=']')
        erreur(CHAINES[116]);

    return OUI;
}



void GlobalParseur::InitialiseVariablesGlobalesDansKif() {
	if (kifloader==NULL)
		return;

	KifElement* ke;
	VariableVector* vecteurchaine;
	VariableIntVector* vecteurint;
	VariableDictionnaire* vdict;
	VariableString* vstr;
	VariableGlobale* vglob;
	VariableXML* vxml;
	string buffbis;
	char erreurs;
	static VECTA<Noeud*> noeuds;
	VariableGraphe* vgraph;

	for (size_t i=0;i<Variables.dernier;i++) {
		//On les insere dans kstart
		Variable* variab=Variables[i];
		ke=NULL;
		KifVariableDeclaration* ki=NULL;
		string nom=variab->nom;
		if (kifloader->Find(nom)!=NULL)
			continue;
		switch(variab->sorte) {
			case VGRAPHE:
				vgraph=(VariableGraphe*)variab;
				ki=new KifVariableDeclaration(kifloader,kifloader->kstart,nom);
				ke=new KifGraph(kifloader,ki,vgraph->g);
				break;
			case VVECTEURCHAINE:
				//Creation d'un simple objet KifVectorStringPtr
				vecteurchaine=(VariableVector*)variab;
				ki=new KifVariableDeclaration(kifloader,kifloader->kstart,nom);
				ke=new KifVectorStringPtr(kifloader,ki,&vecteurchaine->chaine);
				break;
			case VVECTEURINT:
				//Dans le cas d'une liste, on boucle sur les valeurs
				vecteurint=(VariableIntVector*)variab;
				ki=new KifVariableDeclaration(kifloader,kifloader->kstart,nom);
				ke=new KifVectorNumberPtr(kifloader,ki,&vecteurint->valeurs);
				break;
			case VDICTIONNAIRE:
				vdict=(VariableDictionnaire*)variab;
				ki=new KifVariableDeclaration(kifloader,kifloader->kstart,nom);
				ke=new KifMapPtr(kifloader,ki,&vdict->dico);
				break;     
			case VCHAINE:
				vstr=(VariableString*)variab;
				if (vstr->defaut!=NULL) {
					buffbis="";
					vstr->defaut->TraitementFormuleString(this,noeuds,erreurs,buffbis,NULL);
					vstr->AffecteValeurString((uchar*)buffbis.c_str());                
					vstr->utilisee=OUI;
				}
				ki=new KifVariableDeclaration(kifloader,kifloader->kstart,nom);
				ke=new KifStringPtr(kifloader,ki,&vstr->chaine);
				break;
#ifdef XIPLIBXML
			case VXMLOBJET:
				vxml=(VariableXML*)variab;
				ki=new KifVariableDeclaration(kifloader,kifloader->kstart,nom);
				ke=new KifXMLNodePtr(kifloader,ki,vxml);
				break;
#endif
			case VGLOBALEREINIT:
			case VGLOBALE:
				vglob=(VariableGlobale*)variab;
				if (vglob->defaut) {
					vglob->valeur=vglob->defaut->TraitementFormule(this,noeuds,erreurs,NULL);
					vglob->utilisee=OUI;
				}
				ki=new KifVariableDeclaration(kifloader,kifloader->kstart,nom);
				ke=new KifFloatPtr(kifloader,ki,&vglob->valeur,&vglob->utilisee);
				break;			
		}

		if (ki!=NULL)
			kifloader->kstart->Setstring(nom,ki,NULL);		
	}
}

//Chargement d'un fichier KIF...
void GlobalParseur::LitKif(istream& fichier) {	
	long pos=fichier.tellg();	
	ostream* os=&cout;
	if (OS!=NULL)
		os=OS;

	ifstream f(NOM,modeOuverture);
	f.seekg(pos);
	string lecode;
	string uneligne;
	while (!f.eof()) {
		getline(f,uneligne);
		uneligne=Trim(uneligne);
		if (uneligne.size())
			lecode+=uneligne;
		lecode+=10;
	}

	int firstinstruction=0;
	if (kifloader==NULL) {
		if (kifdebug==true)
			KiFSetdebugmin(true);
		kifloader=new KifCodeParseur(kifloader,kif_declencheur,(void*)this,OS);
		kifloader->AddArguments(grmkifarguments);
		kifloader->AddArguments(kifarguments);
		kifloader->session=true;
		ParseurKifInit(kifloader);
		kifGlobals->compile=compilekif;
	}
	else
		firstinstruction=kifloader->kstart->instructions.size();		


	AddALoadingPath(NOM);
	kifGlobals->PushStack(kifloader->kstart);
	kifloader->kstart->adding=true;
	kifloader->addtolocal=true;
	//Pour les variables deja presentes, on les ajoute dans KiF avant toute chose pour qu'elles soient disponibles lors de la compilation
	InitialiseVariablesGlobalesDansKif();		

	try {
		//On se positionne devant le nom de champ kif
		kifloader->Setbaseline(LIGNES+1);		
		kifloader->Evaluate(kifloader->kstart,lecode,false);
		kifGlobals->PopStack();
		if (!Error())
			kifloader->kstart->Run(firstinstruction);
		kifloader->addtolocal=false;
	}
	catch(KifRaiseError* m) {
		ERREUR_POSITION_DANS_FICHIER=m->right;
		LIGNESERREUR=m->left;
		string msg="(in ";
		msg+=m->filename;
		msg+=") ";
		msg+=m->message;
		m->message=msg;
		Cleanerror();
		erreur((char*)m->message.c_str());
		delete m;
	}

	if (Error()) {
		string msg="(In ";
		msg+=NOM;
		msg+=") ";
		msg+=Errorstring();
		Cleanerror();
		erreur(STR(msg));
	}
}

/*-------------------------------------------------------------------------*/

char GlobalParseur::ChoixLectureRegle(istream& fichier,char* tampon) {

    if (LectureParametres==OUI) {        
        if (controle_max_regle>max_controle_regle)
            erreur(CHAINES[190]);
    }

	char lect;
    if (!strcmp(tampon,"variables")) {
		lect=LectureRegles;
		LectureRegles=OUI;
        LitVariables(fichier);
		LectureRegles=lect;
        return NON;
    }

    if (!strcmp(tampon,CHAINES[110])) {
        while(ConsVoc(fichier,OUI));
        return NON;
    }

    if (tampon[0]==0)
        return NON;

    //Filtre Couche
    if (!strcmp(tampon,CHAINES[48])) {
        LitLesDeductions(fichier,DEDUCTIONFILTRE);
        return NON;
    }

#ifdef XIPLIBXML
    if (!strcmp(tampon,"xmlguide")) {
        LitXMLGuide(fichier);    
        return NON;
    }
#endif

    if (!strcmp(tampon,"procedures")) {
        LitProcedure(fichier);
        return NON;
    }

	if (!strcmp(tampon,"tokenselection")) {
        LitTokenSelection(fichier);
        return NON;
    }

#ifdef XIPPYTHON
    if (!strcmp(tampon,"python")) {
        LitProcedurePython(fichier);
        return NON;
    }
#endif

	int laCouche;
    if (!strcmp(tampon,"files")) {   
        if (TRACEREGLE) TRACEREGLE=2;
        
		if (BaseCoucheNumero==ValeurCoucheInitiale) {
			if (CouchesDeduction[laCoucheDeduction]	!= VIDE)
				laCoucheDeduction++;			   

			CouchesDeduction[laCoucheDeduction]=REGLES_SEMANTIQUE;
			laCoucheSemantique=laCoucheDeduction;
            couchesemantique=OUI;
		}
		else {//il s'agit alors d'un ajout dans les couches standards
			laCouche=BaseCoucheNumero+1;
			if (CouchesActives[laCouche]!=VIDE && 
				CouchesActives[laCouche] != REGLES_SEMANTIQUE)
				erreur(Concatene(CHAINES[208],tampon));

			if (MinProfondeurAnalyse > laCouche)
				MinProfondeurAnalyse=laCouche;

			if (MaxProfondeurAnalyse < laCouche)
				MaxProfondeurAnalyse=laCouche;

			CouchesActives[laCouche]=REGLES_SEMANTIQUE;
			laCoucheSemantique=laCouche;
            couchesemantique=NON;
		}
        
        ChargeIndex(fichier);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }


    if (!strcmp(tampon,CHAINES[0])) {
        if (TRACEREGLE) TRACEREGLE=2;
        
		if (BaseCoucheNumero==ValeurCoucheInitiale) {
			if (CouchesDeduction[laCoucheDeduction]	!= VIDE)
				laCoucheDeduction++;			   

			CouchesDeduction[laCoucheDeduction]=REGLES_SEMANTIQUE;
			laCoucheSemantique=laCoucheDeduction;
            couchesemantique=OUI;
		}
		else {//il s'agit alors d'un ajout dans les couches standards
			laCouche=BaseCoucheNumero+1;
			if (CouchesActives[laCouche]!=VIDE && 
				CouchesActives[laCouche] != REGLES_SEMANTIQUE)
				erreur(Concatene(CHAINES[208],tampon));

			if (MinProfondeurAnalyse > laCouche)
				MinProfondeurAnalyse=laCouche;

			if (MaxProfondeurAnalyse < laCouche)
				MaxProfondeurAnalyse=laCouche;

			CouchesActives[laCouche]=REGLES_SEMANTIQUE;
			laCoucheSemantique=laCouche;
            couchesemantique=NON;
		}
               
        if (VERIF==NON) {
            indexeur ind;
			try {
				((ifstream&)fichier).close();
			}
			catch(...) {
			}
            ind.indexer(this,nom_fichier_courant);
        }
        else
            ChargementSemantique(fichier,nom_fichier_courant);

                
        //while(ConsSem(fichier));
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

    

    //Decoupe en unite linguistique
    if (!strcmp(tampon,CHAINES[49])) {
        if (TRACEREGLE) TRACEREGLE=2;
        LitCoupeSequence(fichier);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

    //DI-regle
    if (!strcmp(tampon,CHAINES[21])) {
        if (TRACEREGLE) TRACEREGLE=2;
        while(ConsRegDI(fichier));
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }


    //Termes
    if (!strcmp(tampon,CHAINES[1])) {
        if (TRACEREGLE) TRACEREGLE=2;
        LitSequence(fichier,REGLES_TERME);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    } 

    //Fragmentation
    if (!strcmp(tampon,CHAINES[288])) {
        if (TRACEREGLE) TRACEREGLE=2;
        LitSequence(fichier,REGLES_FRAGMENTE);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    } 

    //Simplification
    if (!strcmp(tampon,CHAINES[14])) {
        if (TRACEREGLE) TRACEREGLE=2;
        LitSimplifie(fichier);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

    //Cartesien
    if (!strcmp(tampon,CHAINES[221])) {
        if (TRACEREGLE) TRACEREGLE=2;
        LitCartesien(fichier);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }


    //Marquage
    if (!strcmp(tampon,CHAINES[23])) {
        if (TRACEREGLE) TRACEREGLE=2;
        LitMarqueur(fichier);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

    //Reconstruction
    if (!strcmp(tampon,CHAINES[2])) {
        if (TRACEREGLE) TRACEREGLE=2;
        LitReconstruction(fichier);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

    //Deduction
    if (!strcmp(tampon,CHAINES[27])) {
        if (TRACEREGLE) TRACEREGLE=2;

        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_EXTRACTION)
            laCoucheDeduction++;
        
        CouchesDeduction[laCoucheDeduction]=REGLES_EXTRACTION;
        LitLesDeductions(fichier,DEDUCTIONCOURANTE);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

    //Script
    if (!strcmp(tampon,CHAINES[314])) {
        if (TRACEREGLE) TRACEREGLE=2;

        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_EXTRACTION)
            laCoucheDeduction++;
        
        CouchesDeduction[laCoucheDeduction]=REGLES_EXTRACTION;
        LitLesDeductions(fichier,DEDUCTIONSCRIPT);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

	if (!strcmp(tampon,"kif")) {
        if (TRACEREGLE) TRACEREGLE=2;

        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_EXTRACTION)
            laCoucheDeduction++;
        
        CouchesDeduction[laCoucheDeduction]=REGLES_EXTRACTION;
        LitKif(fichier);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

    //En chargeant
    if (!strcmp(tampon,CHAINES[328])) {
        if (TRACEREGLE) TRACEREGLE=2;

        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_EXTRACTION)
            laCoucheDeduction++;
        
        CouchesDeduction[laCoucheDeduction]=REGLES_EXTRACTION;
        int poschg=ChgtDeductions.dernier;
        LitLesDeductions(fichier,DEDUCTIONENCHARGEANT);
        if (TRACEREGLE) TRACEREGLE=1;
        //Puis on lance l'execution immediatement
        DeduitFonction(-1,DEDUCTIONENCHARGEANT,NON,poschg);
        return NON;
    }

	//Lecture de l'ORDRE
	if (!strcmp(tampon,CHAINES[395])) {
		ConsOrdreNodes(fichier);
		return NON;
	}

    //Initial
    if (!strcmp(tampon,CHAINES[329])) {
        if (TRACEREGLE) TRACEREGLE=2;

        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_EXTRACTION)
            laCoucheDeduction++;
        
        CouchesDeduction[laCoucheDeduction]=REGLES_EXTRACTION;
        LitLesDeductions(fichier,DEDUCTIONINITIALE);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

	//Generation
	if (!strcmp(tampon,CHAINES[384])) {
        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_GENERATION &&
			CouchesDeduction[laCoucheDeduction]!=REGLES_ORDRE)
            laCoucheDeduction++;

		CouchesDeduction[laCoucheDeduction]=REGLES_GENERATION;
        LitLesDeductions(fichier,DEDUCTIONGENERATION);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
	}

	//Final
    if (!strcmp(tampon,CHAINES[320])) {
        if (TRACEREGLE) TRACEREGLE=2;

        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_EXTRACTION)
            laCoucheDeduction++;
        
        CouchesDeduction[laCoucheDeduction]=REGLES_EXTRACTION;
        LitLesDeductions(fichier,DEDUCTIONFINALE);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

    if (!strcmp(tampon,"tail")) {
        if (TRACEREGLE) TRACEREGLE=2;

        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_EXTRACTION)
            laCoucheDeduction++;
        
        CouchesDeduction[laCoucheDeduction]=REGLES_EXTRACTION;
        LitLesDeductions(fichier,DEDUCTIONQUEUE);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

	//PREBALISE
    if (!strcmp(tampon,"openmarkup")) {
        if (TRACEREGLE) TRACEREGLE=2;

        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_EXTRACTION)
            laCoucheDeduction++;
        
        CouchesDeduction[laCoucheDeduction]=REGLES_EXTRACTION;
        LitLesDeductions(fichier,DEDUCTIONPREBALISE);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

	//POSTBALISE
    if (!strcmp(tampon,"closemarkup")) {
        if (TRACEREGLE) TRACEREGLE=2;

        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_EXTRACTION)
            laCoucheDeduction++;
        
        CouchesDeduction[laCoucheDeduction]=REGLES_EXTRACTION;
        LitLesDeductions(fichier,DEDUCTIONPOSTBALISE);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }


    //Sequence
    if (!strcmp(tampon,CHAINES[202])) {
        if (TRACEREGLE) TRACEREGLE=2;
        LitSequence(fichier,REGLES_SEQUENCE);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }

    if (!strcmp(tampon,CHAINES[402])) {
        if (TRACEREGLE) TRACEREGLE=2;
        LitSequence(fichier,REGLES_SEQUENCE_CONTEXTE);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }


    //PL regle
    if (!strcmp(tampon,CHAINES[53])) {
        while(ConsPL(fichier));
        return NON;
    }

	if (!strcmp(tampon,"probabilities")) {
		chargementprobabilites(fichier);
		return NON;
	}

    //Multimots
    if (!strcmp(tampon,CHAINES[334])) {
        LitMultimots(fichier,MULTISURFACE);
        return NON;
    }

	if (!strcmp(tampon,CHAINES[375])) {
        LitMultimots(fichier,MULTILEMME);
        return NON;
    }

    //Extraction
    if (!strcmp(tampon,CHAINES[201])) {
        if (TRACEREGLE) TRACEREGLE=2;

        if (CouchesDeduction[laCoucheDeduction] != VIDE && 
            CouchesDeduction[laCoucheDeduction]!=REGLES_DEDUCTION)
            laCoucheDeduction++;
        
        CouchesDeduction[laCoucheDeduction]=REGLES_DEDUCTION;
        LitExtraction(fichier);
        if (TRACEREGLE) TRACEREGLE=1;
        return NON;
    }


    char ch[200];
    strcpy(ch,CHAINES[217]);
    strcat(ch,tampon);
    erreur(ch);

    return OUI;
}

 
/*-------------------------------------------------------------------------*/
char GlobalParseur::ConsPL(istream& fichier) {
    char k,tampon[300];
    Traits* trgauche;
    Traits* trdroit;
    int laCouche,couchelocale;
 
    if (LectureParametres==OUI) {
        controle_max_regle++;
        if (controle_max_regle>max_controle_regle)
            erreur(CHAINES[190]);
    }

    long offset=fichier.tellg();
    k=lit(fichier,tampon);         
	char* etiquette=lireRulespaceLabel(fichier,tampon,(uchar&)k,NON);

    unsigned int lignecourante=LIGNES+1;
    if (k=='.' && tampon[0]==0)
        k=lit(fichier,tampon);
	

    if (k==':')
        if (ChoixLectureRegle(fichier,tampon)==NON)
            return NON;
        else
            erreur(Concatene(CHAINES[94],tampon));

        //la couche par defaut est la couche 0
    laCouche=0;
	couchelocale=0;
    

    //L'indice de couche est: chiffre>
    if (k=='>') {
        offset=fichier.tellg();
        offset-=strlen(tampon)+1;
        couchelocale=atoi(tampon);
        laCouche=BaseCoucheNumero+couchelocale;

        if (laCouche<1 || laCouche>MaxCouches)
            erreur(CHAINES[107]);

        if (CouchesActives[laCouche]!=REGLES_DI)
            erreur(CHAINES[214]);

        k=lit(fichier,tampon);
    }

    Label* etiqlab=AjouteLabel(VlabelVecteur,etiquette,REGLES_PL,laCouche,nbDI, 
        new DecompteRegle(REGLES_PL,NOM,offset,lignecourante,laCouche,couchelocale) );

    if (k!='[') 
        erreur(CHAINES[116]);
    trgauche=BatitTraits(fichier);
    if ((trgauche->valeur)->Zero() ) {
        sprintf(tampon,"%s %s",CHAINES[137],TC(this,trgauche));
        erreur(tampon);
    }
    
    k=lit(fichier,tampon);
    if (k!= '<') 
        erreur(CHAINES[117]);
    k=lit(fichier,tampon);
    if (k!='[') 
        erreur(CHAINES[111]);
    trdroit=BatitTraits(fichier);
    if ((trdroit->valeur)->Zero()) {
        sprintf(tampon,"%s %s",CHAINES[137],TC(this,trdroit));
        erreur(tampon);
    }
    RegleTraits* regtr=new RegleTraits(trgauche,trdroit,NULL);
    AjouteStats(new stats(regtr,REGLES_PL,laCouche));
	AjouteDansBase(VBases,regtr,laCouche,etiqlab);
	etiqlab->indexregle=PListes[laCouche].dernier;
    PListes[laCouche].ajoute(regtr);

    return OUI;
}
/*-------------------------------------------------------------------------*/
int GlobalParseur::RetrouveTrait(trait* vr,char* tampon,char gestion_erreur) {
    trait* vrr;
    char* Message;
  
    vrr=vr->Autre();
    while (vrr!=NULL && compchmin((unsigned char*)tampon,vrr->cle.c)) 
        vrr=vrr->Autre();
    
    if (!vrr) {
        Message=Concatene(tampon,CHAINES[119]);
        strcat(Message,(char*)vr->cle.c);

        if (gestion_erreur==OUI)
            erreur(Message);

        if (AFFICHE_ERREUR==OUI) {
            insere_erreur(Message);
            *flot_erreur<<Message<<endl;
        }
        return -1;
    }

    return vrr->indice;
}

trait* GlobalParseur::RetrouveValeurTrait(trait* vr,char* tampon,char gestion_erreur) {
    trait* vrr;
    char* Message;
  
    vrr=vr->Autre();
    while (vrr!=NULL && compchmin((unsigned char*)tampon,vrr->cle.c)) 
        vrr=vrr->Autre();
    
    if (!vrr) {
        Message=Concatene(tampon,CHAINES[119]);
        strcat(Message,(char*)vr->cle.c);

        if (gestion_erreur==OUI)
            erreur(Message);

        if (AFFICHE_ERREUR==OUI) {
            insere_erreur(Message);
            *flot_erreur<<Message<<endl;
        }
        return NULL;
    }

    return vrr;
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::ConsVregle(char type,istream& fichier) {
    char k=0,tampon[300];
    Traits* trgauche;
    Traits* trdroit;
    Categorie* cg;
    Categorie* cd;

 
 
    while (k!=':') {

        k=lit(fichier,tampon);
        if (AppelCons(fichier,k,tampon)==OUI)
            return;

        cg=NULL;
        if (tampon[0]!=0)
            cg=ChercheCat(tampon);

        if (k==':')
            if (tampon[0] !='\x0') 
                erreur(Concatene(CHAINES[94],tampon));
            else
                return;
 
        if (k=='>') 
            trgauche=NULL;
        else {
            if (k!='[') 
                erreur(CHAINES[116]);
            trgauche=BatitTraits(fichier);
            if (cg!=NULL)
                trgauche->ou(cg->traits);
            
            k=lit(fichier,tampon);
            if (k!= '>') erreur(CHAINES[117]);
        }
        
        k=lit(fichier,tampon);
        cd=NULL;
        if (tampon[0]!=0)
            cd=ChercheCat(tampon);

        if (k!='[') 
            erreur(CHAINES[111]);
        trdroit=BatitTraits(fichier);

        if (cd!=NULL)
            trdroit->ou(cd->traits);
        Cat* x=NULL;
        
        if (cg!=NULL)
            x=&cg->X;

        Vregle[type].ajoute(new RegleTraits(trgauche,trdroit,x));        
    }
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::ConsAbrev(istream& fichier) {
    char k=0,tampon[300];
    char* nom;
    Traits* trcourant;


    while (k!=':') {
        
        k=lit(fichier,tampon);
        
        if (AppelCons(fichier,k,tampon)==OUI)
            return;

        if (k==':')
            if (tampon[0] !='\x0') 
                erreur(Concatene(CHAINES[94],tampon));
            else
                return;
        nom=DupliqueChaine(tampon);
        if (k!='=') erreur(CHAINES[123]);
        k=lit(fichier,tampon);
        if (k!='[') erreur(CHAINES[111]);
        trcourant=BatitTraits(fichier);
 
        if (!Abreviation) 
            Abreviation=new abreviation(nom,trcourant);
        else
            Abreviation->insere(this,nom,trcourant);
    }
} 
 
/*-------------------------------------------------------------------------*/
void GlobalParseur::ConsDefaut(istream& fichier) {
    char k=0,tampon[300];
    trait* vr;
    int i,j,reponse;
    Traits* trcourant;


    while (k!=':') {
 
        k=lit(fichier,tampon);
    
        if (AppelCons(fichier,k,tampon)==OUI)
            return;

        if (k==':')
            if (tampon[0] !='\x0') 
                erreur(Concatene(CHAINES[94],tampon));
            else
                return;
 
        if (k!='=') 
            erreur(CHAINES[34]);
        
        vr=ArbTraits->recherche(tampon);
        if (!vr) erreur(Concatene(tampon,CHAINES[124]));
        k=lit(fichier,tampon);
        if (k!='{') erreur(CHAINES[35]);
        while (k!='}') {
  
            k=lit(fichier,tampon);
            trcourant=NULL;
            if (k=='[') {
                trcourant=BatitTraits(fichier);
       
                k=lit(fichier,tampon);
                if (k!='|') erreur(CHAINES[36]);
                k=lit(fichier,tampon);
            }
            if (k!='&') 
                erreur(CHAINES[37]);

            i=RetrouveTrait(vr,tampon,OUI);
            
            k=lit(fichier,tampon);
            j=RetrouveTrait(vr,tampon,OUI);
            
            if (k!='=') 
                erreur(CHAINES[34]);

            k=lit(fichier,tampon);
            reponse=RetrouveTrait(vr,tampon,OUI);
   
            if (k!=',' && k!='}') 
                erreur(CHAINES[39]);

            if (!Defauts) 
                Defauts=new ListeDefaut(vr->valeur,trcourant,i,j,reponse);
            else 
                Defauts->insere(vr->valeur,trcourant,i,j,reponse);
            if (trcourant) 
                delete trcourant;
        }
    }   
}
/*-------------------------------------------------------------------------*/
void UneChaine(GlobalParseur* parseur,char* tampon) {
	Trim(tampon);
	if (tampon[0]=='"') {
		//Le dernier caractere doit etre aussi un '"'
		if (tampon[strlen(tampon)-1]!='"') 
			parseur->erreur(parseur->CHAINES[382]);
		tampon[strlen(tampon)-1]=0;
		strcpy(tampon,tampon+1);
	}
}

char GlobalParseur::AppelCons(istream& fichier,char k, char* tampon) {
 
    if (k!=':')
        return NON;    

    if (!strcmp(tampon,"variables")) {
        LitVariables(fichier);
        return OUI;
    }

    if (!strcmp(tampon,"guessed") && k==':') {
        k=litBis(fichier,tampon);
        if (k!=':')
            strcpy(GUESSED_TAG,tampon); 
        return OUI;
    }


    if (!strcmp(tampon,CHAINES[181]) && k==':') {
        k=litBis(fichier,tampon);
        if (k!=':')
            strcpy(TAG_NOUVEAU,tampon); 
		if (!TestOptions(NON_NORMALISATION_TAG))
			MetEnMinuscule((unsigned char*)TAG_NOUVEAU);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[99]) && k==':') {
        LitLigne(fichier,tampon);
		UneChaine(this,tampon);
        FinDePhrase.ajoute(DupliqueChaine(tampon));
        if (cryptage==NON)
            *flot_erreur<<FinDePhrase.fin()<<Endl;
        return OUI;
    }         

	if (!strcmp(tampon,CHAINES[381]) && k==':') {
        LitLigne(fichier,tampon);
		UneChaine(this,tampon);
		automate* a=new automate((uchar*)tampon,(void*)(ExpressionsFinDePhrase.dernier+1));
        ExpressionsFinDePhrase.ajoute(a);
        return OUI;
    }         
        
    if (!strcmp(tampon,CHAINES[128]) && k==':') {
        k=lit(fichier,tampon);
        if (k!='[')
            erreur(CHAINES[111]);
        k=lit(fichier,tampon);        
        LEMME=DupliqueChaine(tampon);
        if (k==':')
            k=lit(fichier,tampon);
        if (k!=']')
            erreur(CHAINES[116]);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[167]) && k==':') {
        k=lit(fichier,tampon);
        if (k!='[')
            erreur(CHAINES[111]);
        k=lit(fichier,tampon);        
        SURFACE=DupliqueChaine(tampon);
        if (k==':')
            k=lit(fichier,tampon);
        if (k!=']')
            erreur(CHAINES[116]);
        return OUI;
    }


    if (!strcmp(tampon,CHAINES[51]) && k==':') 
        return OUI;


    if (!strcmp(tampon,CHAINES[18]) && k==':') {
        ConsMajuscule(fichier,&TOUTMAJUSCULE);
        return OUI;
    }

    
    if (!strcmp(tampon,CHAINES[110]) && k==':')
        erreur(CHAINES[98]);

    if (!strcmp(tampon,CHAINES[179]) && k==':') {
        ConsMajuscule(fichier,&MAJUSCULE);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[175]) && k==':') {
        ConsAffiche(fichier,&FILTRENOEUD);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[398]) && k==':') {
        ConsAffiche(fichier,&FILTREGENERATION);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[295]) && k==':') {
        k=lit(fichier,tampon);
		if (tampon[0]=='+') {
			Traits* tr=BatitTraits(fichier);
			if (TraitsFiltreOntologie==NULL)
				TraitsFiltreOntologie=tr;
			else {
				TraitsFiltreOntologie->ou(tr);
				delete tr;
			}
		}
		else
			TraitsFiltreOntologie=BatitTraits(fichier);
        return OUI;
    }


    if (!strcmp(tampon,CHAINES[90]) && k==':') {
        ConsAffiche(fichier,&FILTREFONCTION);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[114]) && k==':') {
        ConsAffiche(fichier,&FILTRE);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[218]) && k==':') {
        LireCodageXML(fichier);
        return OUI;
    }

    if (!strcmp(tampon,"ontologies") && k==':') {
        char x=-1;
        LitOntologies(fichier,*hierarchies,NULL,x);
        return OUI;
    }

	if (!strcmp(tampon,"relations") && k==':') {
        char x=-1;
        LitOntologies(fichier,*relations,NULL,x);
        return OUI;
    }

	if (!strcmp(tampon,"concepts") && k==':') {
        char x=-1;
        LitOntologies(fichier,*concepts,NULL,x);
        return OUI;
    }


    if (!strcmp(tampon,CHAINES[108]) && k==':') {
        ConsAtt(fichier);
        return OUI;
    }
    

    if (!strcmp(tampon,CHAINES[40])) {
        ConsDefaut(fichier);
        return OUI;
    }
 
    if (!strcmp(tampon,CHAINES[26])) {
        ConsAbrev(fichier);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[24])) {
        ConsVregle(STD,fichier);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[25])) {
        ConsVregle(RCT,fichier);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[120])) {
        ConsVregle(PostSTD,fichier);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[121])) {
        ConsVregle(PostRCT,fichier);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[192])) {
        ConsFonction(fichier,NON);
        return OUI;
    }

	//Lecture des NODES
	if (!strcmp(tampon,CHAINES[391])) {
        ConsFonction(fichier,OUI);
        return OUI;
    }


    if (!strcmp(tampon,CHAINES[196])) {
        AffichageFonction(fichier,OUI);
        return OUI;
    }

    if (!strcmp(tampon,CHAINES[241])) {
        AffichageFonction(fichier,NON);
        return OUI;
    }

	//Traduction
    if (!strcmp(tampon,CHAINES[188])) {
        ConsTraduction(fichier);
        return OUI;
    }

	//GenereTraits
	if (!strcmp(tampon,CHAINES[385])) {
        ConsGenereTraits(fichier);
        return OUI;
    }

	//category definition
	if (!strcmp(tampon,CHAINES[365])) {
        ConsDefinitionCategorie(fichier);
        return OUI;
    }
	//feature definition
	if (!strcmp(tampon,CHAINES[380])) {
        ConsDefinitionTraits(fichier);
        return OUI;
    }

    if (LectureRegles==OUI)
        ChoixLectureRegle(fichier,tampon);

    return NON;
}

/*-------------------------------------------------------------------------*/
char GlobalParseur::ConsCategorie(istream& fichier) {
    char k=0,tampon[300];
    int inconnu=NON;
    Categorie* c;
    Traits* trcourant;
 
 

    k=lit(fichier,tampon);
    
    if (AppelCons(fichier,k,tampon)==OUI)
        return OUI;

    if (k==':')
        if (tampon[0] !='\x0') 
            erreur(Concatene(CHAINES[94],tampon));
        else 
            return NON;

    if (k!='=') 
        erreur(CHAINES[123]);

    c=CreeCat(tampon,NOM);
    k=lit(fichier,tampon);
    if (k=='#') {
        inconnu=OUI;
        k=lit(fichier,tampon);
    }
    if (k!='[') 
        erreur(CHAINES[111]);
 
    trcourant=BatitTraits(fichier);
    
    c->traits= FactoriseTraits(trcourant);
    if (inconnu) 
        Inconnu=c;

    k=lit(fichier,tampon);
    if (k!='.') {
        MetEnAttenteLecture(k,tampon);
    }

    return OUI;
}
/*-------------------------------------------------------------------------*/
void GlobalParseur::ConsMajuscule(istream& fichier,Traits** maj) {
    char k,tampon[300];
    Traits* trcourant;
 
 
    k=lit(fichier,tampon);
    
    if (k==':') 
        erreur(Concatene(CHAINES[94],tampon));
    if (k!='[') 
        erreur(CHAINES[111]);
    trcourant=BatitTraits(fichier);
 
    *maj=trcourant; 
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::LitCategories(char* nom_def) {
    char tampon[300];
    bouclage=0; 
    ifstream fichier(nom_def,modeOuverture);
    LIGNES=0; 
    POSITION_DANS_FICHIER=0;
    LIGNESERREUR=0;
    NOM=nom_def;

 
    if (fichier.fail() ) {
        strcpy(tampon,CHAINES[97]);
        strcat(tampon,nom_def);
        erreur(tampon);
    }

    while(ConsCategorie(fichier));    
}

/*-------------------------------------------------------------------------*/
void GlobalParseur::ConsTraduction(istream& fichier) {
    char k,tampon[300];
    char etiq[100];
    
    avl_vocabulaire* trad;
    Categorie* c;
    Traits* traits;

    if (TestOptions(NON_NORMALISATION_TAG))
        k=litSans(fichier,etiq);
    else
        k=lit(fichier,etiq);    
    char saut;

    while (etiq[0]) {
        if (AFFICHE_ERREUR==OUI)
            cout<<"--> "<<etiq<<Endl;

        if (k==':') {
            if (AppelCons(fichier,k,etiq)==OUI)
                return;
            else
                erreur(Concatene(CHAINES[94],etiq));
        }

		if (k!='=')
            erreur(CHAINES[34]);

        k=lit(fichier,tampon);

        saut=NON;
        //Il s'agit d'une categorie qui devra etre saute
        if (k==NEGATION) {
            saut=OUI;
            k=lit(fichier,tampon);
        }

        if (k!='.' && k!='[')
            erreur(CHAINES[189]);

        if (saut==NON) {
            if (tampon[0]) {
                c=ChercheCat(tampon);
            
                traits=BatitTraitsPlus(fichier,c,&k,tampon);                          
                trad=new avl_vocabulaire(this,etiq,NULL,&c->X,traits,NULL);
            }
            else {
                traits=BatitTraits(fichier);                
                trad=new avl_vocabulaire(this,etiq,NULL,NULL,traits,NULL);
                k=lit(fichier,tampon);
                if (k!='.')
                    erreur(CHAINES[189]);                
            }
        }
        else
            trad=new avl_vocabulaire(this,etiq,NULL,NULL,NULL,NULL); //la categorie doit etre sautee

        size_t indexMot=CalculClefHachageTout((uchar*)etiq);
        
        if (Traductions[indexMot]==NULL) {
            Traductions[indexMot]=trad;
        }
        else
            Traductions[indexMot]->inserecasse(&Traductions[indexMot],trad);

        if (TestOptions(NON_NORMALISATION_TAG))
            k=litSans(fichier,etiq);
        else
            k=lit(fichier,etiq);   
    }
}
/*-------------------------------------------------------------------------*/
//Il s'agit d'une regle du type: cat[traits] = String
void GlobalParseur::ConsGenereTraits(istream& fichier) {
    char k,tampon[300];   
    
    Categorie* c;
    Traits* traits;


	k=lit(fichier,tampon);
	while (tampon[0] || k=='[') {
        if (k==':') {
            if (AppelCons(fichier,k,tampon)==OUI)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        }
		
		traits=NULL;
		c=NULL;
		//Si c'est une structure de traits
		if (tampon[0]!=0) {
			c=ChercheCat(tampon);
			if (k=='[') {
				traits=BatitTraitsPlus(fichier,c,&k,tampon);                          
				traits=FactoriseTraits(traits);
			}
		}
		else {
			if (k=='[') {
				traits=BatitTraits(fichier);                          
				traits=FactoriseTraits(traits);
				k=lit(fichier,tampon);
			}
		}

		if (c==NULL && traits==NULL)
			erreur(CHAINES[386]);

		if (k!='=')
			erreur(CHAINES[34]);

		k=litSans(fichier,tampon);
		if (k!='.') 
			erreur(CHAINES[204]);
		genere_traits* gt=new genere_traits(&c->X,traits,tampon);
		LesTraitsdeGenerations.ajoute(gt);
		k=lit(fichier,tampon);
	}
}

/*-------------------------------------------------------------------------*/
//Les regles sont de la forme: [tr,tr] = cat[tr,tr].
//Le mot clef est: categorydefinition
void GlobalParseur::ConsDefinitionCategorie(istream& fichier) {
    char k,tampon[300];
	Traits* trtest;
	Traits* trcat;

	k=lit(fichier,tampon);
    while (!fichier.eof()) {
        
        if (k==':') {
            if (AppelCons(fichier,k,tampon)==OUI)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        }
		//On lit d'abord le test
		trtest= BatitTraits(fichier);
		trtest=FactoriseTraits(trtest);
		k=lit(fichier,tampon);
		if (k!='=')
			erreur(CHAINES[123]);
		//On lit categorie
		k=lit(fichier,tampon);
		Categorie* c=ChercheCat(tampon);
		trcat=NULL;
		if (k=='[') {
			trcat=BatitTraits(fichier);
			trcat=FactoriseTraits(trcat);
			k=lit(fichier,tampon);
		}

		if (k!='.')
			erreur(CHAINES[204]);

		DefinitionCategorie* dc=new DefinitionCategorie(&c->X,trtest,trcat);
		definircategories.ajoute(dc);
		k=lit(fichier,tampon);
	}
}
/*-------------------------------------------------------------------------*/
//Les regles sont de la forme: [tr,tr] = [tr,tr].
//Le mot clef est: traitdefinition
void GlobalParseur::ConsDefinitionTraits(istream& fichier) {
    char k,tampon[300];
	Traits* trtest;
	Traits* trcat;

	k=lit(fichier,tampon);
    while (!fichier.eof()) {
        
        if (k==':') {
            if (AppelCons(fichier,k,tampon)==OUI)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        }
		//On lit d'abord le test
		trtest= BatitTraits(fichier);
		trtest=FactoriseTraits(trtest);
		k=lit(fichier,tampon);
		if (k!='=')
			erreur(CHAINES[123]);
		//On lit categorie
		k=lit(fichier,tampon);
		trcat=NULL;
		if (k=='[') {
			trcat=BatitTraits(fichier);
			trcat=FactoriseTraits(trcat);
			k=lit(fichier,tampon);
		}

		if (k!='.')
			erreur(CHAINES[204]);

		DefinitionCategorie* dc=new DefinitionCategorie(NULL,trtest,trcat);
		definirtraits.ajoute(dc);
		k=lit(fichier,tampon);
	}
}
/*-------------------------------------------------------------------------*/
Traits* GlobalParseur::BatitTraitsPlus(istream& fichier,Categorie* cc,char* K,char* Tampon,char* testLecture) {
    Traits* traits=NULL;
 
    if (!cc->traits)
        erreur(Concatene(Tampon,CHAINES[52]));

    if (*K=='[') {
        traits=BatitTraits(fichier);
        *K=lit(fichier,Tampon);
    }
    else
        if (*K=='<') {
            traits=BatitTraits(fichier,'>');
            *K=lit(fichier,Tampon);
            if (testLecture!=NULL)
                *testLecture=OUI;
        }


    if (!traits) 
        traits=new Traits(this);
    
    traits->ou(cc);
    
    return traits;
}

/*-------------------------------------------------------------------------*/

void GlobalParseur::ConsSuiteRegDI(istream& fichier,
                                 int laCouche,
                                 Sequence** gauche,
                                 Sequence** droit,
                                 VECTA<ChaineCat*>& travail,
                                 int& I,
                                 char& where) {
    int i=1;
    char k=' ',tampon[300];
    Categorie* c;
    Traits* traits;
    char code=0;
    char* pt;
    char debut=OUI;
    
    
    while (k!='.') {
        k=lit(fichier,tampon);

        if (!strcmp(tampon,"where") && k=='(') {
            where=OUI;
            break;
        }

        char negContexte=NON;
        if (k==NEGATION && tampon[0]==0) {
            negContexte=OUI;
            k=lit(fichier,tampon);
        }

        if (debut==OUI && !tampon[0]) {
            if (k=='|') {
                if (afficheautomate==OUI) 
                    cout<<"DI(G):"<<VlabelVecteur.dernier<<endl;

                *gauche=LitUneSequence(fichier,travail[0]->X,negContexte,-1);
                k=lit(fichier,tampon);
            }
        }
        
        debut=NON;
        

        char uneEtoile=NON;
        char testLecture=NON;

        if (k=='(') {
            k=lit(fichier,tampon);
            pt=strchr(tampon,'+');
            if (pt==NULL)
                pt=strchr(tampon,'*');

            if (pt!=NULL)
                *pt=0;
            
          
            c=ChercheCat(tampon);
            
            traits=BatitTraitsPlus(fichier,c,&k,tampon,&testLecture);
                
            if (pt!=NULL) {
                uneEtoile=OUI;
                c->etoile=OUI;
            }

            travail.affecte(i,new ChaineCat(this,OUI,uneEtoile,&c->X,traits,testLecture));
            if (k!=')') 
                erreur(CHAINES[130]);
                k=lit(fichier,tampon);
        }
        else {
            char etoiletrouvee=NON;            

            pt=strchr(tampon,'+');
            if (pt!=NULL)
                *pt=0;                
            else {
                pt=strchr(tampon,'*');
                if (pt!=NULL) {
                    *pt=0;
                    etoiletrouvee=OUI;
                }
            }


            c=ChercheCat(tampon);
            
            int arg=-1;
            if (k=='#') {
                k=lit(fichier,tampon);
                //une variable
                arg=atoi(tampon);
            }

            traits=BatitTraitsPlus(fichier,c,&k,tampon,&testLecture);
                
            if (pt!=NULL) {
                c->etoile=OUI;
                uneEtoile=OUI;
            }
            else
                CategoriesObligatoiresDansCouche[laCouche]|=c->X.Val;
            
            travail.affecte(i,new ChaineCat(this,etoiletrouvee,uneEtoile,&c->X,traits,testLecture));
            travail[i]->arg=arg;
        }

        negContexte=NON;
        if (k==NEGATION) {
            negContexte=OUI;
            k=lit(fichier,tampon);
        }

        if (k=='|') {
            if (afficheautomate==OUI) 
                cout<<"DI(D):"<<VlabelVecteur.dernier<<endl;

            *droit=LitUneSequence(fichier,travail[0]->X,negContexte,1);
            
            k=lit(fichier,tampon);
			if (k=='(' && !strcmp(tampon,"where")) {
				where=OUI;
				I=i;
				return;
			}
            if (k!='.') 
                erreur(CHAINES[204]);
        }

        if (k!=',' && k !='.') 
            erreur(CHAINES[132]);
        i++;
    }
    I=i;
}

/*---------------- Lecture et construction d'une règle...-------------------*/
ArcArbre* rechercheArcMarque(EtatArbre* et) {
    int i;
	if (et->marque==100)
		return NULL;
	et->marque=100;
    for (i=0;i<et->arcs.dernier;i++) {
        if (et->arcs[i]->argument!=-1)
            return et->arcs[i];
    }
    for (i=0;i<et->arcs.dernier;i++) {
        ArcArbre* ar=rechercheArcMarque(et->arcs[i]->etatarbre);
        if (ar!=NULL)
            return ar;
    }

    return NULL;
}




void GlobalParseur::LitCoupeSequence(istream& fichier) {
    char k=0,tampon[300];
    Sequence* seq;
    k=lit(fichier,tampon);

    while (!fichier.eof()) {
        if (LectureParametres==OUI) {
            controle_max_regle++;
            if (controle_max_regle>max_controle_regle)
                erreur(CHAINES[190]);
        }

        
        if (k==':') {
            if (ChoixLectureRegle(fichier,tampon)==NON)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        }
        

        char negContexte=NON;
        if (k==NEGATION) {
            negContexte=OUI;
            k=lit(fichier,tampon);
        }
 
        
        seq=NULL;
        if (k=='|') 
            seq=LitUneSequence(fichier,XCatVide,negContexte,1,REGLES_DECOUPESEQUENCE);

        if (seq==NULL)
            erreur(CHAINES[57]);

        Cat* X=seq->regle->arcs[0]->X;
        //if (seq->regle->arcs.dernier!=1)
        //    erreur(CHAINES[57]);

        CategoriesCoupeSequence|=X->Val;
        
        //Recherche de l'arc avec argument!=-1
        char boucle=0;
        ArcArbre* ar=rechercheArcMarque(seq->regle);
        if (ar!=NULL) {
            if (ar->X->Val!=0)
                CategoriesCoupeSequenceInterieur|=ar->X->Val;
            else
                CategoriesCoupeSequenceInterieur=MoinsUn;
            declencheurs.ajoute(ar);
        }

        ListeCoupeSequence.ajoute(seq);
        k=lit(fichier,tampon);
        if (!strcmp(tampon,"where") && k=='(') {
            TestArbre* tarbre=NULL;
            static VECTA<char> vari(5);

            //lecture du test... Il s'agit d'un test en debut de definition            
            tarbre=new TestArbre;
            k=' ';
            LectureTest(fichier,k,tampon,NON,vari,&tarbre);
            static TVecteur clefArbre;
			clefArbre=0;
			seq->listes=new ListeTestArbre;
            tarbre->direct=tarbre->LineariseDirect(this,clefArbre,*seq->listes);
            delete tarbre;
        }

        if (k=='.')
            k=lit(fichier,tampon);
    }
}

/*---------------- Lecture et construction d'une règle DI...-------------------*/
char GlobalParseur::ConsRegDI(istream& fichier) {
    char k,tampon[300];
    static VECTA<ChaineCat*> travail(100);
    Categorie* c;
    Traits* traits;
    int i;
    int laCouche=0,couchelocale=0;
    char* etiquette=NULL;

    if (LectureParametres==OUI) {
        controle_max_regle++;
        if (controle_max_regle>max_controle_regle)
            erreur(CHAINES[190]);
    }

    int numregle=VlabelVecteur.dernier;    
    afficheregle(numregle,"ID_Rule");
    travail.raz();

    long offset=fichier.tellg();
    k=Llit(fichier,tampon);
    unsigned int lignecourante=LIGNES+1;

    char* pt=NULL;
    etiquette=NULL;

	etiquette=lireRulespaceLabel(fichier,tampon,(uchar&)k,NON);

    if (k==':') {
        if (ChoixLectureRegle(fichier,tampon)==NON)
            return NON;
        else
            erreur(Concatene(CHAINES[94],tampon));
    }

	laCouche=-1;
    if (k=='>' || (pt=strchr(tampon,'_'))) {
        offset=fichier.tellg();
        offset-=strlen(tampon)+1;
        couchelocale=atoi(tampon);
        laCouche=BaseCoucheNumero+couchelocale;


        if (laCouche<1 || laCouche>MaxCouches)
            erreur(CHAINES[107]);

        if (pt!=NULL)
            strcpy(tampon,pt+1);        
        else 
            k=lit(fichier,tampon);
        
        if (CouchesActives[laCouche]!=VIDE && 
            CouchesActives[laCouche] != REGLES_DI)
            erreur(Concatene(CHAINES[208],tampon));

        if (MinProfondeurAnalyse > laCouche)
            MinProfondeurAnalyse=laCouche;
        
        if (MaxProfondeurAnalyse < laCouche)
            MaxProfondeurAnalyse=laCouche;
        
        CouchesActives[laCouche]=REGLES_DI;
    }    

	if (laCouche==-1) 
		erreur(CHAINES[369]);   

   //Dans le cas d'une regle d'affectation de valeurs (aucun noeud pere n'est alors cree)
   //seules les regles de couche permettent ce genre de manipulation...
    if (tampon[0]=='?' && laCouche==NON) 
        erreur(CHAINES[211]);

    c=ChercheCat(tampon);
   
    traits=BatitTraitsPlus(fichier,c,&k,tampon);
    
    travail.affecte(0,new ChaineCat(this,NON,NON,&c->X,traits,NON));

    if (k=='-') {
        k=lit(fichier,tampon);
        tampon[0]='-';
    }

/* Lecture de la flèche*/
    if (tampon[0]!='-' || k!='>') k=lit(fichier,tampon);
    if (tampon[0]!='-' || k!='>') erreur(CHAINES[54]);

    Sequence* gauche=NULL;
    Sequence* droit=NULL;
    char where=NON;
    TestArbre* tarbre=NULL;
    ConsSuiteRegDI(fichier,laCouche,&gauche,&droit,travail,i,where);

    if (where==OUI) {
        static VECTA<char> vari(5);
        
       //lecture du test... Il s'agit d'un test en debut de definition            
        tarbre=new TestArbre;
        k=' ';
        LectureTest(fichier,k,tampon,NON,vari,&tarbre);        
    }

    Regle* reg=new Regle(travail[0]->X,travail[0]->traits);
    reg->gauche=gauche;
    reg->droit=droit;
    reg->passageLexical=NON;
    reg->numero=VlabelVecteur.dernier;
    Label* etiqlab=AjouteLabel(VlabelVecteur,etiquette,REGLES_DI,laCouche,nbDI, 
        new DecompteRegle(REGLES_DI,NOM,offset,lignecourante,laCouche,couchelocale) );
    AjouteStats(new stats(reg,REGLES_DI,laCouche));
	AjouteDansBase(VBases,reg,laCouche,etiqlab);


    if (tarbre!=NULL) {
        TVecteur clef=0;
        reg->listes=new ListeTestArbre;
        tarbre->LineariseDirect(this,clef,*reg->listes);
        delete tarbre;
    }
    //On conserve l'ensemble des categories dans la partie droite de la regle
    //La couche 0 correspond a la couche generale...
   

    InitRegleDI(reg,travail,1,i,laCouche);
    
    travail.nettoie();    
    return OUI;
}


char est_balise(typeVoc ecrase) {
    switch (ecrase) {
    case BALISEDEBUT:
    case BALISEFIN:
    case BALISECOMMUTATEUR:
    case BALISECOMMUTATEURFERMEE:
    case BALISEDEBUTSF:
    case BALISEFINSF:
    case BALISECOMMUTATEURSF:
    case BALISECOMMUTATEURFERMEESF:
        return 1;
    }
    return 0;
}
/*-------------------------------------------------------------------------*/
/* Les modeles de regles sont:

mot : cat += [traits].
mot : cat[traits] = catbis[traits]. //remplacement d'une entree par une autre == ECRASER
mot : cat[traits] += [traits]. //ajout de traits == AJOUTER
mot : cat[traits] += catnouveau[traits]. //ajout d'une nouvelle interpretation ==NOUVEL
mot = cat[traits]. //une seule entree ==ECRASERTOUT
mot += [traits]. //ajout de traits systematiques partout ==AJOUTER
mot += cat[traits]. //ajout d'une nouvelle interpretation avec un jeu de traits. ==UNEFOISNOUVEAU
*/
char GlobalParseur::ConsVoc(istream& fichier,char lit_couche) {
    char k,tampon[1024];
    char mot[1024];
    char categ[1024];
    Categorie* c;
    Categorie* cnouveau;
    Traits* trcourant;
    Traits* mot_tr;
    auto_vocabulaire* auv;
    Traits* trinit;


    tampon[0]=0;
    
    //lecture jusqu'au signe '='
        //k=LitEgal(fichier,mot);

        //Un mot de vocabulaire est sous la forme: mot[traits] = cat[traits]
        //on lit d'abord le mot avec sa structure de traits si elle est disponible...
    int laCouche=0,couchelocale=0;
    mot[0]=0;
    if (lit_couche==NON)
        k=LitEgalBrut(fichier,(uchar*)mot,NON);
    else {
        k=LitEgalBrut(fichier,(uchar*)mot,OUI);
		if (!strcmp(mot,"#@rulespace")) {
			MetEnAttenteLecture(k,mot+2);
			LitInstructions(fichier,'@',(uchar&)k);
			mot[0]=0;
			MetEnAttenteLecture(k,mot);
			k=LitEgalBrut(fichier,(uchar*)mot,OUI);
		}
	}

    
    if (k==':' && mot[0]==0)
        return NON;
    
    //L'indice de couche est: chiffre>
    if (k=='>' && TestChiffre((uchar*)mot) && LuEntreCotes==NON) {
        couchelocale=atoi(mot);
        laCouche=BaseCoucheNumero+couchelocale;
        
        if (laCouche<1 || laCouche>MaxCouches)
            erreur(CHAINES[107]);
        
        if (CouchesActives[laCouche]!=VIDE && 
            CouchesActives[laCouche]!=REGLES_VOCABULAIRE)
            erreur(CHAINES[214]);
        
        if (MinProfondeurAnalyse > laCouche)
            MinProfondeurAnalyse=laCouche;
        
        if (MaxProfondeurAnalyse < laCouche)
            MaxProfondeurAnalyse=laCouche;
        
        CouchesActives[laCouche]=REGLES_VOCABULAIRE;
        k=LitEgalBrut(fichier,(uchar*)mot,OUI);
    }
    else {
        if (lit_couche==OUI)
            erreur(CHAINES[310]);
    }

	//traitement des espaces de regle generalises au lexique
	if (lit_couche!=NON)
		EspaceLexique[laCouche]=currentspacename;

	Sequence* gauche=NULL;
	//Le contexte precede la regle elle-meme
	if (k=='|') {
		gauche=LitUneSequence(fichier,XCatVide,NON,-1,REGLES_LEXICALES);
		k=LitEgalBrut(fichier,(uchar*)mot,NON);
	}

    mot_tr=NULL;

    if (k==':' && mot[0]==0)
        return NON;

    typeVoc ecrase=ECRASER;
    
    char directEgal=NON;
    c=CatVide;
    char regle_surface=NON;
    if (k==':') {
        //nous sommes dans le cadre des deux regles du dessus
        //on lit d'abord une categorie
        k=lit(fichier,categ);
        if (k==':' && categ[0]==0) {
            regle_surface=OUI;
            k=lit(fichier,categ);
        }

        if (categ[0]==0)
            strcpy(categ,"?");

        c=ChercheCat(categ);
        mot_tr = BatitTraitsPlus(fichier,c,&k,tampon);
        if (k=='+' || tampon[0]=='+') {
            ecrase=AJOUTER;
            if (k=='+')
                k=lit(fichier,tampon);  
        }
        if (k!='=')
            erreur(CHAINES[123]);
    }
    else {
        directEgal=OUI;
        if (k=='+') {
            ecrase=AJOUTER;
            k=lit(fichier,tampon);  
        }

        if (k=='<') { //Balise de debut
            ecrase=BALISEDEBUT;
            k=lit(fichier,tampon);
            if (k=='>') {
                ecrase=BALISECOMMUTATEUR;
                k=lit(fichier,tampon);
            }
        }
        else {
            if (k=='>') {//Balise de fin
                ecrase=BALISEFIN;
                k=lit(fichier,tampon);
            }
        }
        //s'il s'agit d'une balise trans-phrase
        if (tampon[0]=='*' && k=='=') {
            switch(ecrase) {
            case BALISEDEBUT:
                ecrase=BALISEDEBUTSF;
                break;
            case BALISEFIN:
                ecrase=BALISEFINSF;
                break;
            case BALISECOMMUTATEUR:
                ecrase=BALISECOMMUTATEURSF;
            }
        }
        /* Lecture de la flèche*/
        if (k!='=')
            erreur(CHAINES[123]);
    }

    k=lit(fichier,categ);
    trinit=NULL;
    tampon[0]=0;
    Traits* trbalise=NULL;
    if (k=='<') {
        if (!est_balise(ecrase))
            erreur(CHAINES[73]);
        //Initialisateur pour le premier ou le dernier mot d'une sequence de mots entre balises...
        trbalise=BatitTraits(fichier,'>'); 
        k=lit(fichier,tampon);
    }

    if (k=='[') {
        trcourant=BatitTraits(fichier);        
        trinit=new Traits(trcourant);
        k=lit(fichier,tampon);
    }
    else
        trcourant=new Traits(this);
    
    if (categ[0]==0 || categ[0]=='?') {
        if (ecrase==ECRASER)
            erreur(CHAINES[122]);
        cnouveau=c;
    }
    else {
        cnouveau=ChercheCat(categ);
        TabLexique[cnouveau->X.val]=OUI;
        if (!cnouveau->traits)
            erreur(Concatene(tampon,CHAINES[52]));
        trcourant->ou(cnouveau);
        if (trinit==NULL)
            trinit=new Traits(trcourant);
        else
            trinit->ou(cnouveau);

        if (ecrase==ECRASER && directEgal==OUI)
            ecrase=ECRASERTOUT; //ce nouveau lemme remplacera toutes les autres occurences
        else
            if (ecrase==AJOUTER) {
                if (directEgal==OUI)
                    ecrase=UNEFOISNOUVEAU; //ce nouveau lemme ne sera ajoute qu'une seule fois
                else
                    if (ecrase==AJOUTER && c != cnouveau)
                        ecrase=NOUVEL; //ce nouveau lemme sera ajoute selon le nombre de cas qui correspondent...            
            }
    }
    
	if (tablelatin) {
		//Dans ce cas, on force la conversion a UTF8
		agnostring m(mot);
		m = m.latintoutf8(tablelatin);
		strcpy(mot, STR(m));
	}

    int ppi;
    tampon[0]=0;
    char* lelem=NULL;
    if (trcourant->lemme!=NULL) {
        trcourant->lemme->mot(tampon,1024);
        lelem=tampon;
    }
    auv=new auto_vocabulaire(this,mot,lelem,&c->X,&cnouveau->X,trcourant,trinit,gauche);
    auv->traits_mot=FactoriseTraits(mot_tr);
    auv->ecraser=ecrase;
    etat* a;
    auto_vocabulaire* auvancien;
    if (est_balise(ecrase)) {
        
        auv->initiaux=FactoriseTraits(trbalise);
        
        //On conserve une liste des balises qui commutent
        //pour les remettre a zero a chaque analyse
        if (ecrase==BALISECOMMUTATEUR)
            commutateurs.ajoute(auv);
                
        a=LesBalises.parse((uchar*)mot,auv);
        if (a!=NULL) {
            auvancien = (auto_vocabulaire*)a->valeur;
            if (auvancien!=auv)
                auvancien->ajoute(auv);
        }
        else
            RenvoieErreurAutomate(LesBalises.erreur);

        if (!strchr(PremierCaractereBalise,mot[0])) {
            ppi=strlen(PremierCaractereBalise);
            PremierCaractereBalise[ppi]=mot[0];
            PremierCaractereBalise[ppi+1]=0;
        }
    } 
    else {
        char typeauto=chainecomplexe((uchar*)mot,0,ARCINITIAL);
        if (!typeauto) {
            if (regle_surface==OUI) {                
                if (ArbSurface[laCouche]==NULL)
                    ArbSurface[laCouche]=new automatevecteur;
                a=ArbSurface[laCouche]->parse((uchar*)mot,auv);
                if (a!=NULL) {
                    auvancien = (auto_vocabulaire*)a->valeur;
                    if (auvancien!=auv)
                        auvancien->ajoute(auv);
                }
                else
                    RenvoieErreurAutomate(ArbSurface[laCouche]->erreur);
            }
            else {
                if (ArbVoc[laCouche]==NULL)
                    ArbVoc[laCouche]=new automatevecteur;
                
                a=ArbVoc[laCouche]->parse((uchar*)mot,auv);
                if (a!=NULL) {
                    auvancien = (auto_vocabulaire*)a->valeur;
                    if (auvancien!=auv)
                        auvancien->ajoute(auv);
                }
                else
                    RenvoieErreurAutomate(ArbVoc[laCouche]->erreur);
            }
        }
        else {
            automate* alocal;
            typeauto=typeautomate((uchar*)mot,0);
            if (typeauto==AUTOMATEPILE) 
                alocal=new automatepile((uchar*)mot,auv);
            else
                alocal=new automate((uchar*)mot,auv);
           
            if (alocal->erreur!=AERR_NO_ERROR)
                RenvoieErreurAutomate(alocal->erreur);
            
			alocal->aupluspres();
            if (regle_surface==OUI) {
				if (ArbSurfaceComplexe[laCouche]==NULL)
					ArbSurfaceComplexe[laCouche]=alocal;				
				else
					ajouteautomate(ArbSurfaceComplexe[laCouche],alocal);				
            }
            else {
				if (ArbVocComplexe[laCouche]==NULL)
                    ArbVocComplexe[laCouche]=alocal;
				else
					ajouteautomate(ArbVocComplexe[laCouche],alocal);				
            }
        }
    }

    if (k!='.') 
        erreur(CHAINES[132]);
    
    return OUI;
}
//--------------------------------------------------------------------------------------------------------
char GlobalParseur::ConsSem(istream& fichier ) {

    char k,tampon[300];
    char mot[300];
    char categ[300];
    Categorie* c;
    Traits* mot_tr;
    auto_vocabulaire_semantique* vs;

    
    //lecture jusqu'au signe '='
        //k=LitEgal(fichier,mot);

    //Un mot de vocabulaire est sous la forme: mot[traits] = cat[traits]
    //on lit d'abord le mot avec sa structure de traits si elle est disponible...
    int numregle=numero_regle_semantique;
    afficheregle(numregle,"Semantic Rule");

   
    k=LitEgal(fichier,(unsigned char*)mot);
    mot_tr=NULL;

    if (k==':' && mot[0]==0)
        return NON;

    if (k=='-') {
        k=lit(fichier,tampon);  
        if (k!='>')
            erreur(CHAINES[56]);
    }

    c=CatVide;
    //nous sommes dans le cadre des deux regles du dessus
    //on lit d'abord une categorie
    k=lit(fichier,categ);
    
    c=ChercheCat(categ);
    if (k=='[')
        mot_tr = BatitTraitsPlus(fichier,c,&k,tampon);

    if (k!='=') 
        erreur(CHAINES[123]);

    k=lit(fichier,tampon);
    //si une regle de dependence est attachee
    VectTestFonctionSemantique* vtf=new VectTestFonctionSemantique;
    vtf->numero=numero_regle_semantique;
    Dependance* depend=NULL;
    static VECTA<char> vari(5);
    vari.raz();
    //La variable 0 est une variable par defaut qui correspond 
    //au mot que l'on cherche a desambiguiser
    vari.affecte(0,OUI);

    if (!strcmp(tampon,"if")) {

        TestArbre* tarbre=new TestArbre;
        k=' ';
        LectureTest(fichier,k,tampon,NON,vari,&tarbre);

        tarbre->direct=tarbre->LineariseDirect(this,vtf->clef,vtf->listes);
        delete tarbre;
    }
     
    char boucle=OUI;
    while (boucle) {
        depend=ConstruitDependance(fichier,vari,k,tampon,OUI);            
        vtf->dependances.ajoute(depend);
        k=lit(fichier,tampon);
        if (k==',')
            k=lit(fichier,tampon);
        else 
            boucle=NON;
    }
    
                                             
    vs= new auto_vocabulaire_semantique(this,mot,&c->X,mot_tr,vtf);

    if (Semantiques[laCoucheDeduction][couchesemantique]==NULL)
        Semantiques[laCoucheDeduction][couchesemantique]=new automatevecteur;
    etat* esem=Semantiques[laCoucheDeduction][couchesemantique]->parse((uchar*)mot,vs);
    if (esem!=NULL) {
        auto_vocabulaire_semantique* vvs=(auto_vocabulaire_semantique*)esem->valeur;
        if (vvs!=(void*)vs)
            vvs->ajoute(vs);
    }
    else 
        RenvoieErreurAutomate(Semantiques[laCoucheDeduction][couchesemantique]->erreur);
    
    numero_regle_semantique++;

    if ((numero_regle_semantique%10000) == 0)
        *flot_erreur<<numero_regle_semantique/10000;
    else
        if ((numero_regle_semantique%1000) == 0)
            *flot_erreur<<".";
    
    if (k!='.') 
        erreur(CHAINES[132]);    
    return OUI;
}

/*-------------------------------------------------------------------------*/
/* Cette règle est fondamentale, en effet on lit chacun des éléments d'une règle séparément 
et on les enregistre dans la structure travail. Chacun des éléments se voit assigner un code:
  code=0   cet élément est unique dans la règle...
  code=1   cet élément est optionnel
  code=2   cet élément fait partie d'une structure de type {X,Y,Z}
  
  Ainsi on va donc générer: Un seul élément de type Regle si code==0;
			    Deux éléments de type Regle si code==1, c'est à dire une règle
			    avec l'élément X marqué et une règle sans cet élément X
			    Autant de règle qu'il y a d'éléments dans la structure disjontive
			    si code==2...
 
   Les paramêtres de InitRegleDI sont les suivants:
	 reg     : le pointeur courant de la règle que l'on construit...
	 travail : le tableau qui contient l'ensemble des éléments de la règle que 
			   l'on vient de lire dans le fichier texte...
	 i           : l'indice courant de l'élément que l'on est en train de lire dans travail...
	 k               : la borne supérieur dans travail...
 */                         
			   
void GlobalParseur::InitRegleDI(Regle* reg,VECTA<ChaineCat*>& travail,int i,int k,int laCouche) {
    Regle* Nreg;
    int j,m;
    TVecteur l;

/* Lorsque l'on a analysé tous les éléments de travail, on construit le filtre associé à 
chacun des éléments travail. Ce filtre permet de savoir quels sont les éléments qui
peuvent cohabiter ensemble.
 Ainsi par exemple si une règle contient "det n", pour det et n on va construire la 
disjonction binaire correspondante... Il faut se rappeler que chaque élément de type
det,n,a etc... est associé	avec une valeur binaire, soit avec une puissance de deux. 
Cette puissance de deux peut être aussi vu comme un entier compris entre 0 et 31,
qui permet d'avoir accès directement au tableau ArbReg qui comprend 32 éléments... 
 La case de numéro x correspond à l'élément X associé à «2^x» et contient la 
disjonction de tous les éléments avec lesquels X peut cohabiter dans une règle.
Ainsi on peut donc savoir lors de l'analyse d'une séquence de catégories si 
deux catégories peuvent appartenir à  une même règle*/

    if (i>=k) {

        if (reg->traits==NULL)
            erreur(CHAINES[139]);

        for (j=1;j<k;j++) {
            l=(travail[j]->X)->Val;
            m=(travail[j]->X)->val;
            if ((l&reg->cle)==l) 
                TabFiltre[laCouche][m]|=reg->cle;
        }

        //Pour une couche donnee, on constitue l'ensemble des categories qui
        //apparaissent dans cette couche... On s'en servira pour filtrer dans
        //le cas de categories ambigues...
        //La couche 0 correspond a la couche generale... Celle qui est non deterministe
        CategoriesDansCouche[laCouche] |= reg->cle;

        iNombreDeRegles++;

        //Si la categorie est de type '?', il ne s'agit pas a proprement
        //parle d'une regle
        if ((reg->Sx)->Val)
            TabRegle[laCouche][(reg->Sx)->val]|=reg->cle;
        
        avl_Regle* avlreg=new avl_Regle(reg);
        if (ListeDesRegles[laCouche]==NULL) 
            ListeDesRegles[laCouche]=reg;
        else
            ListeDesRegles[laCouche]->insere(reg);

        ListeDesAvlRegles.ajoute(avlreg);
        
        //if (!ArbReg[laCouche])
        //    ArbReg[laCouche]=avlreg;
        //else 
        //    ArbReg[laCouche]->insere(&ArbReg[laCouche],avlreg);

		ajouteregle(laCouche,avlreg);
        reg->affiche(NON);
    }
    
/* Sinon et bien on continue l'analyse en fonction du code 
associé à chaque catégorie lors de la lecture de la règle...*/
    else {
        ChaineCat* trav=travail[i];
       if (trav->Code==0) {
           reg->insere(this,trav->X,trav->traits,trav->etoile,trav->arg,trav->testLecture);
	   InitRegleDI(reg,travail,i+1,k,laCouche);
       }
       else
           if (trav->Code==1) {
               Nreg=new Regle(reg);            
               //AjouteStats(new stats(Nreg,REGLES_DI));
			   //AjouteDansBase(VBases,Nreg,laCouche,NULL);
               Nreg->passageLexical=NON;
               Nreg->insere(this,trav->X,trav->traits,trav->etoile,trav->arg,trav->testLecture);
               InitRegleDI(reg,travail,i+1,k,laCouche);            
               InitRegleDI(Nreg,travail,i+1,k,laCouche);
           }
    }
}


//--------------------------------------------------------------------------------------------

void GlobalParseur::ResultatLecture(int* nbRegles,int* nbValeurs, int* borneVec,int* nb_fonc) {
    *nbRegles=iNombreDeRegles;
    *nbValeurs=iNombreDeValeurs;
    *borneVec=iBorneVect ;
    *nb_fonc=nbInference;
}


void GlobalParseur::ConsFiltre(istream& fichier, Vecteur** pfiltre) {
    Vecteur* filtre=*pfiltre;
    char k;
    char tampon[300];
    trait* vr;

    k=lit(fichier,tampon);
    if (k!='[') 
        erreur(CHAINES[111]);

    if (filtre!=NULL)
        delete filtre;
    
    filtre=new Vecteur(iTailleVect);
    *pfiltre=filtre;
    while (k!=']') {
        k=lit(fichier,tampon);
        if (k!=']' && k!=',') 
            erreur(CHAINES[112]);
        vr=ArbTraits->recherche(tampon);
        if (!vr) 
            erreur(Concatene(CHAINES[113],tampon));
        filtre->ou(vr->valeur);
    }
}
/*-------------------------------------------------------------------------*/
void GlobalParseur::TestTraitVide(Traits* tr) {

    short i;
	if (tr==NULL)
		erreur(CHAINES[168]);

    if (tr->lpresent!=NULL) {
        if (tr->present!=NULL)
            erreur(CHAINES[168]);
        tr->present=tr->lpresent;
        tr->lpresent=NULL;
    }

    if (tr->present==NULL && tr->lemme==NULL && tr->surface==NULL && tr->onto==NULL) 
        erreur(CHAINES[168]);

    for (i=0;i<iTailleVect;i++) {   
        if ((tr->valeur)->vecteur[i]!=0)
            erreur(CHAINES[168]);
        
        if ((tr->filtre)->vecteur[i]!=0)
            erreur(CHAINES[168]);
    }
    
    if (tr->bloquage!=NULL) 
        erreur(CHAINES[168]);
    if (tr->libre!=NULL) 
        erreur(CHAINES[168]);
    if (tr->negation!=NULL)
        erreur(CHAINES[168]);
    if (tr->efface!=NULL) 
        erreur(CHAINES[168]);
}

/*-------------------------------------------------------------------------*/

//Lecture d'un contexte...
Sequence* GlobalParseur::LitUneSequence(istream& fichier,Cat* ctete,char neg,int sens,typeRegle regle) {
    
    EtatArbre* cle=NULL;
    EtatArbre* queue;
    Sequence* fonc;
    

    fonc=new Sequence(ctete);

    //passageLexical==-1 indique qu'il s'agit de la lecture d'un contexte...
    fonc->passageLexical=-1;
    fonc->negation=neg;

    cle=ConstructionAutomate(fichier,fonc,NON,'|',NON,sens,regle,&queue); 
    

    //cle=ConstruitArbre(fichier,fonc,nettoyage,NON,'|',NON,sens,regle);
    //LiensSousArbre(cle,nettoyage);

    if (cle == NULL) {
        //Cette @ signifie que le contexte doit etre vide...
        //Autrement dit, il ne peut y avoir de categorie a gauche ou a 
        //droite selon le type de contexte... (premiere ou derniere categorie)
        fonc->Fx=contexteFoncVide;
        return fonc;
    }

    if (ERREUR_SURVENUE) {
        delete fonc;
        return NULL;
    }

    fonc->regle=cle;
    //fonc->ConstruitClef(XCatEpsilon);
    fonc->numero=0;

    return fonc;
}


void EtatArbre::genere(GlobalParseur* parseur,ostream& os,int profondeur) {
    marque=1;

    switch(etatfin) {
    case 0:
        break;
    case 1:    
        break;
    case 2:
        break;
    }

    
    if (fils) {        
        fils->genere(parseur,os,profondeur+1);        
    }
        
    for (int i=0;i<arcs.dernier;i++)
        arcs[i]->genere(parseur,os,etat,profondeur);
}

void ArcArbre::genere(GlobalParseur* parseur,ostream& os,int id,int profondeur) {
    int taille=2;
    char ch[20];
    char chh[20];
    
    char prev=marque;
    marque=1;    
    os<<"Phrase[index+"<<profondeur<<"]->X";
    if (negation==1)
        os<<"!";

    os<<"=TableauDesNoms["<<X->val<<"]";
    if (argument!=-1) {
        sprintf(chh,"#%d",argument);
        strcat(ch,chh);
    }
    if (type==2)
        strcat(ch,"^");
    strcat(ch,")-");
    os<<ch;
    if (etatarbre->marque==-1)
        etatarbre->genere(parseur,os,profondeur+strlen(ch));    
    else {
        if (type==1)
            os<<"<b";
        else
            os<<"<g";

        switch (etatarbre->etatfin) {
        case 0:
            os<<etatarbre->etat<<">";
            break;
        case 1:
            os<<"e"<<etatarbre->etat<<">";
            break;
        case 2:
            os<<"x"<<etatarbre->etat<<">";
        }            
        os<<Endl;
    }
}


void EtatArbre::print(ostream& os,int profondeur) {
    char ch[20];
    char chh[20];
    ch[0]=0;
    chh[0]=0;
    marque=1;

    switch(etatfin) {
    case 0:
        strcat(ch,"<n");
        break;
    case 1:    
        strcat(ch,"<e");
        break;
    case 2:
        strcat(ch,"<x");
    }

    sprintf(chh,"%d:%d>",etat,nombre);
    strcat(ch,chh);
    profondeur+=strlen(ch);
    os<<ch;
    int k;
    if (fils) {
        os<<Endl;
        for (k=0;k<profondeur;k++)
            os<<" ";        
        os<<"{";        
        profondeur;
        fils->print(os,profondeur+1);        
        for (k=0;k<profondeur;k++)
            os<<" ";
        os<<"}"<<Endl;
        for (k=0;k<profondeur;k++)
            os<<" ";
    }
    
    int cr=1;
    for (int i=0;i<arcs.dernier;i++) {
        arcs[i]->print(os,etat,profondeur);
        if (arcs[i]->etatarbre->etatfin==1 || arcs[i]->etatarbre->marque==1)
            cr=0;
        if (i<arcs.dernier-1) {
            for (k=0;k<profondeur;k++)
                os<<" ";
        }
    }
    

    if (cr==1 && (profondeur==0 || etatfin!=0))
        os<<Endl;

}

void ArcArbre::print(ostream& os,int id,int profondeur) {
    int taille=2;
    char ch[200];
    char chh[200];
    
    char prev=marque;
    marque=1;
    strcpy(ch,"-(");
    if (negation==1) {
        strcat(ch,"~");
        taille++;
    }
    strcat(ch,X->nom);
    ch[taille+5]=0;
    sprintf(chh,"%d:%d",num,clefsuivante);
    strcat(ch,chh);
    if (argument!=-1) {
        sprintf(chh,"#%d",argument);
        strcat(ch,chh);
    }
    if (type==2)
        strcat(ch,"^");
    strcat(ch,")-");
    os<<ch;
    if (etatarbre->marque==-1)
        etatarbre->print(os,profondeur+strlen(ch));    
    else {
        if (type==1)
            os<<"<b";
        else
            os<<"<g";

        switch (etatarbre->etatfin) {
        case 0:
            os<<etatarbre->etat<<">";
            break;
        case 1:
            os<<"e"<<etatarbre->etat<<">";
            break;
        case 2:
            os<<"x"<<etatarbre->etat<<">";
        }            
        os<<Endl;
    }
}


//Lecture d'un arbre de contexte pour une regle d'inference
void GlobalParseur::LitUnAutomate(istream& fichier,Cat* ctete,VectTFInstruction* vtf,char fin,typeRegle regle,char stop) {
    
    EtatArbre* cle;
    EtatArbre* queue=NULL;
    Fonction* fonc=NULL;
    Fonction* nega=NULL;
    char k=fin;
    char tampon[300];

    fonc=new Fonction(ctete);

    //On lit une regle d'extraction
    if (k=='=') {
        cle=ConstructionAutomate(fichier,fonc,NON,stop,NON,1,regle,&queue);        
        
        if (ERREUR_SURVENUE) {
            delete fonc;
            return;
        }
        
        fonc->regle=cle;
        //fonc->ConstruitClef(XCatEpsilon);
        fonc->numero=0;
    }
    else {               
        if (fin=='~') {
            k=lit(fichier,tampon);
            cle=ConstructionAutomate(fichier,fonc,NON,stop,NON,1,regle,&queue);
                        
            nega=fonc;
            nega->regle=cle;
            nega->ConstruitClef(XCatEpsilon);
            nega->numero=0;
            fonc=new Fonction(ctete);
            k=lit(fichier,tampon);
        }
        
        if (k=='|') {  
            queue=NULL;            
            fonc->numero=vtf->numero;
            cle=ConstructionAutomate(fichier,fonc,NON,stop,NON,1,regle,&queue);                                  

            if (ERREUR_SURVENUE) {
                delete fonc;
                return;
            }
                        
            fonc->regle=cle;
            //fonc->ConstruitClef(XCatEpsilon);
            fonc->numero=0;
        }
        else
            if (fin=='~')
                MetEnAttenteLecture(k,tampon);
    }
    fonc->comptevariables();
    vtf->regle=fonc;
    vtf->negation=nega;

}

//Cette procedure permet de verifier qu'une regle n'a pas deja ete indexe sur une meme categorie
//exemple: NP = (num),(noun),num,noun. cette regle pourrait etre indexee deux fois sur num sans cette
//protection...
inline void AjouteTestVecteurElement(VectSequence* uneCase,Sequence* fonc) {
    if (uneCase->dernier==0 || uneCase->fin()!=fonc)
        uneCase->ajoute(fonc);
}

char verificationregle(EtatArbre* cle) {
	if (cle->marque==543)
		return OUI;

	cle->marque=543;
	if (cle==NULL)
		return OUI;
	if (cle->fils!=NULL)
		return NON;

	for (int i=0;i<cle->arcs.dernier;i++) {
		if (!verificationregle(cle->arcs[i]->etatarbre))
			return NON;
	}
	return OUI;
}

void GlobalParseur::DeterminePremiereCategorie(Sequence* fonc,EtatArbre* cle,int laCouche) {
    VECTA<int> cats;
    int i;
    ArcArbre* ar;

    for (int ider=0;ider<cle->arcs.dernier;ider++) {
        ar=cle->arcs[ider];
        if (ar->X==XCatEpsilon) {            
            DeterminePremiereCategorie(fonc,ar->etatarbre,laCouche);
            continue;
        }
        if (ar->X==XCatVide) {
            //La regle commence par un "?"
            fonc->touteCategorie=OUI;
            CategoriesDansCouche[laCouche]= MoinsUn;
            for (i=0;i<NbCats;i++) {
                if (TableDesSequences[laCouche][i]==NULL)
                    TableDesSequences[laCouche][i]=new VectSequence(1);
                AjouteTestVecteurElement(TableDesSequences[laCouche][i],fonc);
                //TableDesSequences[laCouche][i]->ajoute(fonc);            
            }   
            continue;
        }
        
        if (ar->X->index==CATFUSION) {
            cats.raz();                    
            ListerCategories(ar->X->Val,cats);
            for (int icats=0;icats<cats.dernier;icats++) {
                i=cats[icats];                        
                TVecteur v=Un<<i;
                CategoriesDansCouche[laCouche] |= v;
                if (TableDesSequences[laCouche][i]==NULL)
                    TableDesSequences[laCouche][i]=new VectSequence(3);
                
                AjouteTestVecteurElement(TableDesSequences[laCouche][i],fonc);
            }
            continue;
        }
        
        int vi=(ar->X)->val;
        CategoriesDansCouche[laCouche] |= (ar->X)->Val;
        if (TableDesSequences[laCouche][vi]==NULL)
            TableDesSequences[laCouche][vi]=new VectSequence(3);                
        AjouteTestVecteurElement(TableDesSequences[laCouche][vi],fonc);
    }
}

//Le but de ces regles est d'offrir une possibilite de selection d'une sequence
//de token issue d'un decoupage provenant des FSTs (TOK)
//Une regle de ce type est de la forme: 1> |... | X1,X2 |...|.
void GlobalParseur::LitTokenSelection(istream& fichier) {
    char k,tampon[300];
    char* etiquette=NULL;
    
    EtatArbre* cle;
    char fin=NON;
    int laCouche,couchelocale;
    int coucheCourante=-1;
    
    while (!fin) {        
        if (LectureParametres==OUI) {
            controle_max_regle++;
            if (controle_max_regle>max_controle_regle)
                erreur(CHAINES[190]);
        }
        
        int numregle=VlabelVecteur.dernier;
        afficheregle(numregle,"Token Selection Rule");
        
        long offset=fichier.tellg();
        k=lit(fichier,tampon);
        unsigned int lignecourante=LIGNES+1;
		etiquette=lireRulespaceLabel(fichier,tampon,(uchar&)k,NON);        

		if (k==':') {
            if (ChoixLectureRegle(fichier,tampon)==NON)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        }

        
        laCouche=-1;couchelocale=0;
        
        char* pt=NULL;
        if (k=='>') {
            offset=fichier.tellg();
            offset-=strlen(tampon)+1;
            couchelocale=atoi(tampon);
            laCouche=BaseCoucheNumero+couchelocale;
            
            if (laCouche<1 || laCouche>MaxCouches)
                erreur(CHAINES[107]);
            
                
			if (CouchesActives[laCouche]!=VIDE && 
				CouchesActives[laCouche] != REGLES_TOKENSELECTION)
				erreur(Concatene(CHAINES[212],tampon));               

			if (MinProfondeurAnalyse > laCouche)
				MinProfondeurAnalyse=laCouche;

			if (MaxProfondeurAnalyse < laCouche)
				MaxProfondeurAnalyse=laCouche;

			CouchesActives[laCouche]=REGLES_TOKENSELECTION;
        }
                
		if (laCouche==-1) 
			erreur(CHAINES[369]);

        //Dans le cas d'une lecture de terme, il faut tenir compte de la lecture d'un mot
        // couche> mot: np=...
        
        
        EtatArbre* queue=NULL;
        if (afficheautomate==OUI) {
            cout<<endl;
            if (etiquette==NULL || etiquette[0]=='#')
                cout<<"TokenSelection:"<<VlabelVecteur.dernier<<endl;
            else
                cout<<"TokenSelection:"<<etiquette<<endl;
        }

		Sequence* fonc=new Sequence(XCatVide);
        cle=ConstructionAutomate(fichier,fonc,NON,'.',NON,1,REGLES_SEQUENCE,&queue); 
        
        if (ERREUR_SURVENUE) {
            delete fonc;
			delete cle;
            return;
        }
        
        if (cle==NULL) {
            delete fonc;
            erreur(CHAINES[69]);
        }
        
		if (!verificationregle(cle)) {
			delete fonc;
			delete cle;
			erreur(CHAINES[372]);
		}

		cle->demarque();
		cle->met(this,untoken);
		cle->demarque();
		for (int ic=0;ic<cle->arcs.dernier;ic++) {
			if (cle->arcs[ic]->traits==NULL)
				cle->arcs[ic]->traits=new Traits(this);
			cle->arcs[ic]->traits->ou(tokenstart);
		}
		fonc->regle=cle;
		TokenSelection* ts=new TokenSelection(fonc);        
        ts->numero=VlabelVecteur.dernier;

        Label* etiqlab=AjouteLabel(VlabelVecteur,etiquette,REGLES_TOKENSELECTION,laCouche,nbSequence,
            new DecompteRegle(REGLES_TOKENSELECTION,NOM,offset,lignecourante,laCouche,couchelocale));
        AjouteStats(new stats(ts,REGLES_TOKENSELECTION,laCouche));
		AjouteDansBase(VBases,ts,laCouche,etiqlab);
		etiqlab->indexregle=ReglesTokenSelection[laCouche].dernier;
		ReglesTokenSelection[laCouche].ajoute(ts);
		fonc->label=etiqlab;
	}
}

void GlobalParseur::LitSequence(istream& fichier,typeRegle leType) {
    char k,tampon[300];
    char* etiquette=NULL;
    
    EtatArbre* cle;
    Categorie* c;
    Sequence* fonc;
    char fin=NON;
    Traits* traits;
    int laCouche,couchelocale;
    int coucheCourante=-1;
    char mot[300];
    
    while (!fin) {        
        if (LectureParametres==OUI) {
            controle_max_regle++;
            if (controle_max_regle>max_controle_regle)
                erreur(CHAINES[190]);
        }
        
        int numregle=VlabelVecteur.dernier;
        afficheregle(numregle,"Sequence Rule");
        
        long offset=fichier.tellg();
        k=lit(fichier,tampon);
        unsigned int lignecourante=LIGNES+1;
		etiquette=lireRulespaceLabel(fichier,tampon,(uchar&)k,NON);        

		if (k==':') {
            if (ChoixLectureRegle(fichier,tampon)==NON)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        }

        
        laCouche=-1;couchelocale=0;
        
        char* pt=NULL;
        if (k=='>' || (pt=strchr(tampon,'_'))) {
            offset=fichier.tellg();
            offset-=strlen(tampon)+1;
            couchelocale=atoi(tampon);
            laCouche=BaseCoucheNumero+couchelocale;
            
            if (laCouche<1 || laCouche>MaxCouches)
                erreur(CHAINES[107]);
            
            if (pt!=NULL)
                strcpy(tampon,pt+1);        
            else 
                if (leType==REGLES_SEQUENCE || leType==REGLES_SEQUENCE_CONTEXTE)
                    k=lit(fichier,tampon);      
                else //On lit le mot qui servira de base a l'analyse
                    k=LitEgal(fichier,(unsigned char*)mot);
                
                if (CouchesActives[laCouche]!=VIDE && 
                    CouchesActives[laCouche] != leType)
                    erreur(Concatene(CHAINES[212],tampon));               
                
                if (MinProfondeurAnalyse > laCouche)
                    MinProfondeurAnalyse=laCouche;
                
                if (MaxProfondeurAnalyse < laCouche)
                    MaxProfondeurAnalyse=laCouche;
                
                CouchesActives[laCouche]=leType;
        }
                
		if (laCouche==-1) 
			erreur(CHAINES[369]);

        //Dans le cas d'une lecture de terme, il faut tenir compte de la lecture d'un mot
        // couche> mot: np=...
        char regle_surface=NON;
        if (leType==REGLES_TERME || leType==REGLES_FRAGMENTE) {
            k=lit(fichier,tampon);
            if (k==':' && tampon[0]==0) {
                regle_surface=OUI;
                k=lit(fichier,tampon);
            }
        }
        
        c=ChercheCat(tampon);      
        traits=NULL;
        if (k=='[') {
            traits=BatitTraits(fichier);
            k=lit(fichier,tampon);
        }
        if (!traits)
            traits=new Traits(this);
        traits->ou(c);
        
        fonc=new Sequence(&c->X);
        fonc->traits=FactoriseTraits(traits);
        fonc->passageLexical=NON;
        fonc->deterministe=courte;
        fonc->numero=VlabelVecteur.dernier;
        
		char surdetermine=NON;
        if (k=='@') {
            k=lit(fichier,tampon);
            fonc->deterministe=longue;
			surdetermine=longue;
			if (k=='@') {
				surdetermine=SURDETERMINE;
				k=lit(fichier,tampon);
			}
        }
        
        int increment=1;
        //<= signifie que la regle s'applique a l'envers
        if (k=='<') {
            k=lit(fichier,tampon);
            fonc->increment=-1;
            increment=-1;
        }
        
        if (k!='=') {
            delete fonc;
            erreur(CHAINES[34]);
        }
        
        EtatArbre* queue=NULL;
        if (afficheautomate==OUI) {
            cout<<endl;
            if (etiquette==NULL || etiquette[0]=='#')
                cout<<"Sequence:"<<VlabelVecteur.dernier<<endl;
            else
                cout<<"Sequence:"<<etiquette<<endl;
        }
        
        cle=ConstructionAutomate(fichier,fonc,NON,'.',NON,increment,REGLES_SEQUENCE,&queue); 
                
        if (ERREUR_SURVENUE) {
            delete fonc;
			delete cle;
            return;
        }
        
        if (cle==NULL) {
            delete fonc;
            erreur(CHAINES[69]);
        }
        
		if (!verificationregle(cle)) {
			delete fonc;
			delete cle;
			erreur(CHAINES[372]);
		}
		cle->demarque();

        ArcArbre* curseur=cle->arcs[0];
        vocabulaire_fragmente* frag;
        
        //cette categorie est utilisee pour indexer les regles de sequence        
        fonc->regle=cle;
		fonc->deterministe=surdetermine;

        if (cle->arcs[0]!=NULL)
            cle->clef=cle->arcs[0]->clefsuivante;
        //fonc->ConstruitClef(XCatEpsilon);        
        
        Label* etiqlab=AjouteLabel(VlabelVecteur,etiquette,leType,laCouche,nbSequence,
            new DecompteRegle(leType,NOM,offset,lignecourante,laCouche,couchelocale));
        AjouteStats(new stats(fonc,leType,laCouche));
		AjouteDansBase(VBases,fonc,laCouche,etiqlab);

        vocabulaire_terme* vs;
        etat* eterme;
        Label* lab;

        //On enregistre la position du premier element dans la fonction...
        //fonc->premier=cle;
        switch (leType) {
        case REGLES_TERME:
            if (curseur->traits==NULL || (curseur->traits)->lemme == NULL) {
                if (curseur->traits==NULL)
                    curseur->traits=new Traits(this);
                else
                    curseur->traits=new Traits(curseur->traits);
                
                (curseur->traits)->lemme=new automate((uchar*)mot,NULL);
                
                curseur->traits=FactoriseTraits(curseur->traits);
            }
            
            //La premiere categorie est obligatoire...
            if ((curseur->X)->val==0)
                CategoriesDansCouche[laCouche] = MoinsUn;
            else
                CategoriesDansCouche[laCouche] |= (curseur->X)->Val;
            
            fonc->numero=VlabelVecteur.dernier;
            if (Termes[laCouche]==NULL)
                Termes[laCouche]=new automatevecteur;
            
            vs=new vocabulaire_terme(fonc);
            eterme=Termes[laCouche]->parse((uchar*)mot,vs);            
            if (eterme!=NULL) {
                vocabulaire_terme* vvs=(vocabulaire_terme*)eterme->valeur;
                //S'il existait deja
                if (vvs!=vs) {
                    vs->terme->raz();
                    delete vs;
                    vvs->terme->ajoute(fonc);               
                }
            }
            else 
                RenvoieErreurAutomate(Termes[laCouche]->erreur);
            
            coucheCourante=laCouche;
            lab=VlabelVecteur.fin();            
            break;
        case REGLES_FRAGMENTE:
            //La tete de la regle ici sert a verifier la categorie du lemme
            if (c->X.val==0)
                CategoriesDansCouche[laCouche] = MoinsUn;
            else
                CategoriesDansCouche[laCouche] |= c->X.Val;
            frag=new vocabulaire_fragmente((uchar*)mot,fonc);
            frag->surface=regle_surface;
            //Il s'agit dans ce cas d'une suite d'automate que l'on appliquera a chaque mot            
			etiqlab->indexregle=Fragmentes[laCouche].dernier;
            Fragmentes[laCouche].ajoute(frag);
            break;
        case REGLES_SEQUENCE:
		case REGLES_SEQUENCE_CONTEXTE:
            //Cette liste n'a d'autre but que d'avoir une liste unifie des fonctions
            //pour pouvoir toutes les detruire en une seule operation.
            ListeFonctions.ajoute(fonc);
            
            //Deux cas, la regle commence par une categorie dont la valeur est donnee, 
            //ou la regle commence par un "?".
            
            fonc->touteCategorie=NON;
            DeterminePremiereCategorie(fonc,cle,laCouche);
            break;
        }
    }
}

//Lecture des expressions multimots qui sont de la forme:
//couche> "Expressions Regulieres" = Cat[feat].

void GlobalParseur::LitMultimots(istream& fichier,char typeregle) {
    char k,tampon[300];
    char* etiquette=NULL;
    
    Categorie* c;
    char fin=NON;
    Traits* traits;
    int laCouche,couchelocale;
    int coucheCourante=-1;
    char mot[1000];
    multimots* mmots;
    typeRegle tyR=REGLES_MULTILEMMES;

	if (typeregle==MULTISURFACE)
		tyR=REGLES_MULTIMOTS;

    while (!fin) {        
        if (LectureParametres==OUI) {
            controle_max_regle++;
            if (controle_max_regle>max_controle_regle)
                erreur(CHAINES[190]);
        }
        
        int numregle=VlabelVecteur.dernier;
        afficheregle(numregle,"Multiword Rule");
        
        long offset=fichier.tellg();
        k=lit(fichier,tampon);
		etiquette=lireRulespaceLabel(fichier,tampon,(uchar&)k,NON);        
        unsigned int lignecourante=LIGNES+1;
        if (k==':') {
            if (ChoixLectureRegle(fichier,tampon)==NON)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        }
        
        
        
        laCouche=-1;couchelocale=0;
        
        char* pt=NULL;
        if (k=='>' || (pt=strchr(tampon,'_'))) {
            offset=fichier.tellg();
            offset-=strlen(tampon)+1;
            couchelocale=atoi(tampon);
            laCouche=BaseCoucheNumero+couchelocale;
            
            if (laCouche<1 || laCouche>MaxCouches)
                erreur(CHAINES[107]);
            
            if (pt!=NULL)
                strcpy(tampon,pt+1);        
            else 
                k=LitEgal(fichier,(unsigned char*)mot);
            
            if (CouchesActives[laCouche]!=VIDE && 
                CouchesActives[laCouche] != tyR)
                erreur(Concatene(CHAINES[212],tampon));               
            
            if (MinProfondeurAnalyse > laCouche)
                MinProfondeurAnalyse=laCouche;
            
            if (MaxProfondeurAnalyse < laCouche)
                MaxProfondeurAnalyse=laCouche;
            
            CouchesActives[laCouche]=tyR;
        }

		if (laCouche==-1) 
			erreur(CHAINES[369]);
                

        //Dans le cas d'une lecture de terme, il faut tenir compte de la lecture d'un mot
        // couche> mot: np=...        
		char aupluslong=NON;
		if (k=='@') {
			aupluslong=OUI;
			k=lit(fichier,tampon);
		}
		if (k!='=')
            erreur(CHAINES[34]);

		k=lit(fichier,tampon);
		
		Sequence* gauche=NULL;
		Sequence* droit=NULL;
		//On lit alors un contexte gauche
		if (k=='|') {
			gauche=LitUneSequence(fichier,XCatVide,NON,-1,tyR);
			k=lit(fichier,tampon);
		}
		
        c=ChercheCat(tampon);      
        traits=NULL;
        if (k=='[') {
            traits=BatitTraits(fichier);
            k=lit(fichier,tampon);
        }

		//On lit alors un contexte droit
		if (k=='|') {
			droit=LitUneSequence(fichier,XCatVide,NON,1,tyR);
			k=lit(fichier,tampon);
		}
		
		if (k==',') //lecture d'un where
			k=lit(fichier,tampon);

		ListeTestArbre* lsts=NULL;
        if (!strcmp(tampon,"where") && k=='(') {
            TestArbre* tarbre=NULL;
            static VECTA<char> vari(5);

            //lecture du test... Il s'agit d'un test en debut de definition            
            tarbre=new TestArbre;
            k=' ';
            LectureTest(fichier,k,tampon,NON,vari,&tarbre);
            static TVecteur clefArbre;
			clefArbre=0;
			lsts=new ListeTestArbre;
            tarbre->direct=tarbre->LineariseDirect(this,clefArbre,*lsts);
            delete tarbre;
        }

		if (k!='.') 
			erreur(CHAINES[383]);

        if (!traits)
            traits=new Traits(this);
        traits->ou(c);
        mmots=new multimots(&c->X,FactoriseTraits(traits),gauche,droit,lsts);
        char typeauto=chainecomplexe((uchar*)mot,0,ARCINITIAL);
        etat* e;
        if (typeauto==NON) {//si c'est une expression simple
            //on le rajoute dans l'automate simple
			if (motsmultiples[typeregle][laCouche]==NULL) {
                motsmultiples[typeregle][laCouche]=new automatevecteur;
				motsmultiples[typeregle][laCouche]->aupluslong=aupluslong;
			}
            
            e=motsmultiples[typeregle][laCouche]->parse((uchar*)mot,mmots);
			if (e==NULL) {
				delete mmots;
                RenvoieErreurAutomate(motsmultiples[typeregle][laCouche]->erreur);            
			}
        }
        else {
            automate* a=new automate;  
			a->aupluslong=aupluslong;
            e=a->parse((uchar*)mot,mmots);
			if (aupluslong==NON)
				a->aupluspres();

			if (e==NULL) {
				delete mmots;
                RenvoieErreurAutomate(a->erreur);
			}
			if (motsmultiplescomplexes[typeregle][laCouche]==NULL)
				motsmultiplescomplexes[typeregle][laCouche]=a;
			else
				ajouteautomate(motsmultiplescomplexes[typeregle][laCouche],a);
        }

        mmots->numero=VlabelVecteur.dernier;
        Label* etiqlab=AjouteLabel(VlabelVecteur,etiquette,REGLES_MULTIMOTS,laCouche,nbCartesien,
            new DecompteRegle(REGLES_MULTIMOTS,NOM,offset,lignecourante,laCouche,couchelocale));
        AjouteStats(new stats(mmots,REGLES_MULTIMOTS,laCouche));        
		AjouteDansBase(VBases,mmots,laCouche,etiqlab);
    }
}



void GlobalParseur::LitCartesien(istream& fichier) {
    char k,tampon[300];

    Cartesien* produit;
    char fin=NON;
    int laCouche,couchelocale;
    char* etiquette=NULL;
    
    long offset=fichier.tellg();
    k=lit(fichier,tampon);  
    unsigned int lignecourante=LIGNES+1;

    while (!fin) {
        if (LectureParametres==OUI) {
            controle_max_regle++;
            if (controle_max_regle>max_controle_regle)
                erreur(CHAINES[190]);
        }

        int numregle=VlabelVecteur.dernier;
        afficheregle(numregle,"Cartesian Rule");

        laCouche=-1;couchelocale=-1;
        
		etiquette=lireRulespaceLabel(fichier,tampon,(uchar&)k,NON);

		if (k==':') {
            if (ChoixLectureRegle(fichier,tampon)==NON)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        }
        
        char factorise=NON;
        //Nous sommes dans une couche
                

		laCouche=-1;
        if (k=='>') {
            offset=fichier.tellg();
            offset-=strlen(tampon)+1;
            couchelocale=atoi(tampon);
            laCouche=BaseCoucheNumero+couchelocale;
            


            if (laCouche>MaxCouches || 
                CouchesActives[laCouche]!=VIDE && 
                CouchesActives[laCouche] != REGLES_CARTESIEN)
                erreur(Concatene(CHAINES[199],tampon));               

            if (MinProfondeurAnalyse > laCouche)
                MinProfondeurAnalyse=laCouche;
            
            if (MaxProfondeurAnalyse < laCouche)
                MaxProfondeurAnalyse=laCouche;

            CouchesActives[laCouche]=REGLES_CARTESIEN;
            
            k=lit(fichier,tampon);
        }
            
		if (laCouche==-1) 
			erreur(CHAINES[369]);

        produit=new Cartesien;
        produit->numero=VlabelVecteur.dernier;
        Label* etiqlab=AjouteLabel(VlabelVecteur,etiquette,REGLES_CARTESIEN,laCouche,nbCartesien,
            new DecompteRegle(REGLES_CARTESIEN,NOM,offset,lignecourante,laCouche,couchelocale));
        AjouteStats(new stats(produit,REGLES_CARTESIEN,laCouche));
		AjouteDansBase(VBases,produit,laCouche,etiqlab);

        if (k=='|') {
            if (afficheautomate==OUI) {
                cout<<endl;
                if (etiquette==NULL || etiquette[0]=='#')
                    cout<<"Cartesien:"<<VlabelVecteur.dernier<<endl;
                else
                    cout<<"Cartesien:"<<etiquette<<endl;
            }

            produit->regle=LitUneSequence(fichier,XCatVide,NON,1,REGLES_SIMPLIFICATION);
            EtatArbre* curseur=produit->regle->regle;
            produit->clef=0;
            for (int ider=0;ider<curseur->arcs.dernier;ider++) {
                Cat* x=curseur->arcs[ider]->X;
                if (x==XCatEpsilon)
                    continue;
                if (x->Val==0) {
                    produit->clef=0;
                    break;
                }
                produit->clef |= x->Val;
            }

            k=lit(fichier,tampon);
        }

        

        if (k=='=') {
            k=lit(fichier,tampon);
            if (k!='>')
                erreur(CHAINES[54]);
        }    

        TestArbre* tarbre=NULL;
        static VECTA<char> vari(5);

        //lecture du test... Il s'agit d'un test en debut de definition            
        tarbre=new TestArbre;
        k=' ';
        LectureTest(fichier,k,tampon,NON,vari,&tarbre,'.');
        TVecteur clefArbre;
        tarbre->LineariseDirect(this,clefArbre,produit->listes);
        delete tarbre;

        offset=fichier.tellg();
		etiqlab->indexregle=TableProduitCartesien[laCouche].dernier;
		TableProduitCartesien[laCouche].ajoute(produit);                
    }
}


void GlobalParseur::LitSimplifie(istream& fichier) {
    char k,tampon[300];
    char* etiquette=NULL;
    Categorie* c;
    Simplifie* simpli;
    char fin=NON;
    Traits* traits;
    int laCouche,couchelocale;

    while (!fin) {
        if (LectureParametres==OUI) {
            controle_max_regle++;
            if (controle_max_regle>max_controle_regle)
                erreur(CHAINES[190]);
        }

        int numregle=VlabelVecteur.dernier;
        afficheregle(numregle,"Tagging Rule");

        laCouche=-1;couchelocale=-1;    
        
        long offset=fichier.tellg();
        k=lit(fichier,tampon);  
        unsigned int lignecourante=LIGNES+1;
        etiquette=lireRulespaceLabel(fichier,tampon,(uchar&)k,NON);

        if (k==':')
            if (ChoixLectureRegle(fichier,tampon)==NON)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        
        char factorise=NON;

        //Nous sommes dans une couche
        if (k=='>') {
            offset=fichier.tellg();
            offset-=strlen(tampon)+1;
            couchelocale=atoi(tampon);
            laCouche=BaseCoucheNumero+couchelocale;

            if (laCouche>MaxCouches || 
                CouchesActives[laCouche]!=VIDE && 
                CouchesActives[laCouche] != REGLES_SIMPLIFICATION)
                erreur(Concatene(CHAINES[199],tampon));               

            if (MinProfondeurAnalyse > laCouche)
                MinProfondeurAnalyse=laCouche;
            
            if (MaxProfondeurAnalyse < laCouche)
                MaxProfondeurAnalyse=laCouche;

            CouchesActives[laCouche]=REGLES_SIMPLIFICATION;
            
            k=lit(fichier,tampon);
        }
        

        char negation=NON;
        //Premier cas: (cat,cat)[traits] =
        if (k=='(') {
            factorise='(';
            k=lit(fichier,tampon);
        }

        if (k==NEGATION) {
            negation=OUI;
            k=lit(fichier,tampon);
        }

        simpli=new Simplifie;
        c=ChercheCat(tampon);
        
        traits=NULL;
        char tous=NON;

        //traits locaux sur la premiere categorie: cat<traits> ou cat>traits<
        if (k=='<') {
            k=litBis(fichier,tampon);
            if (k=='*')
                tous=OUI;
            else {
                MetEnAttenteLecture(k,tampon);
            }

            traits=BatitTraits(fichier,'>');
            
            traits->ou(c);
            k=lit(fichier,tampon);
        }
        
        simpli->X.affecte(simpli->nbX,&c->X);
        simpli->negation.affecte(simpli->nbX,negation);
        simpli->traits.affecte(simpli->nbX,FactoriseTraits(traits));
        simpli->tous.affecte(simpli->nbX,tous);

        simpli->nbX++;
        simpli->numero=VlabelVecteur.dernier;
        Label* etiqlab=AjouteLabel(VlabelVecteur,etiquette,REGLES_SIMPLIFICATION,laCouche,
            nbSimplifie,new DecompteRegle(REGLES_SIMPLIFICATION,NOM,offset,lignecourante,laCouche,couchelocale));
        
        

        //Traits globaux sur une categorie sans parenthese autour: cat[traits]
        if (k=='[') {
            if (factorise!=NON)
                erreur(CHAINES[130]);
            factorise='[';
            traits=BatitTraits(fichier,']');
            simpli->communs=FactoriseTraits(traits);
            k=lit(fichier,tampon);
        }

        if (k=='%') {
            k=lit(fichier,tampon);
            simpli->ecrase=OUI;
        }

        //Lecture des categories avant le signe '='
        while (k!='=' && tampon[0]!='%' && k!=')') {
            negation=NON;
            k=lit(fichier,tampon);
            if (k==NEGATION) {
                negation=OUI;
                k=lit(fichier,tampon);
            }
            c=ChercheCat(tampon);

            traits=NULL;

            if (k=='[')
                erreur(CHAINES[130]);

            tous=NON;
            //Deux types de test: >tous< OU <un>

            if (k=='<') {
                k=litBis(fichier,tampon);
                if (k=='*')
                    tous=OUI;
                else {
                    MetEnAttenteLecture(k,tampon);
                }

                traits=BatitTraits(fichier,'>');
                traits->ou(c);
                k=lit(fichier,tampon);
            }

            simpli->X.affecte(simpli->nbX,&c->X);
            simpli->negation.affecte(simpli->nbX,negation);
            simpli->traits.affecte(simpli->nbX,FactoriseTraits(traits));
            simpli->tous.affecte(simpli->nbX,tous);

            simpli->nbX++;
			if (k=='%') {
				k=lit(fichier,tampon);
				if (k!='=') {
					delete simpli; 
					erreur(CHAINES[46]);
				}
				simpli->ecrase=OUI;
			}

            if (k!='=' && k!=',' && k!=')' && tampon[0]!='%') {
                delete simpli;
                erreur(CHAINES[46]);
            }
        }

        //Il s'agit d'un ecrasement de categories: X,Y %=
        if (tampon[0]=='%')
            simpli->ecrase=OUI;

        if (factorise==NON && k==')') {
            delete simpli;
            erreur(CHAINES[30]);
        }

        if (factorise=='(') {
            k=lit(fichier,tampon);
            if (k=='[') {
                simpli->communs=FactoriseTraits(BatitTraits(fichier));
                k=lit(fichier,tampon);
            }

            if (tampon[0]=='%')
                simpli->ecrase=OUI;
           

            if (k!='=') {
                delete simpli;
                erreur(CHAINES[34]);
            }
        }

        char negContexte=NON;
        k=lit(fichier,tampon);        
        if (k==NEGATION) {
            negContexte=OUI;
            k=lit(fichier,tampon);
        }
        //Lecture du contexte gauche
        if (k=='|' && !tampon[0]) {            
            if (afficheautomate==OUI) {
                cout<<endl;
                if (etiquette==NULL || etiquette[0]=='#')
                    cout<<"Simplifie(G):"<<numregle<<endl;
                else
                    cout<<"Simplifie(G):"<<etiquette<<endl;
            }

            simpli->gauche=LitUneSequence(fichier,XCatVide,negContexte,-1,REGLES_SIMPLIFICATION);
            k=lit(fichier,tampon);
        }

        char fin=NON;
        //lecture des categories que l'on veut conserver
        while (!fin) { 
            if (!strcmp(tampon,"where"))
                break;

            c=ChercheCat(tampon);
            
            simpli->categorie.affecte(simpli->nbCat,&c->X);
            traits=BatitTraitsPlus(fichier,c,&k,tampon);
            simpli->traits_categorie.affecte(simpli->nbCat,FactoriseTraits(traits));
            simpli->nbCat++;
            if (k!=',')
                break;
            k=lit(fichier,tampon);
        }
        
        negContexte=NON;
        char suite[100];
        if (k==NEGATION) {
            negContexte=OUI;
            k=lit(fichier,suite);
        }

        //Lecture du contexte droit
        if (k=='|') {
            if (afficheautomate==OUI) {
                cout<<endl;
                if (etiquette==NULL || etiquette[0]=='#')
                    cout<<"Simplifie(D):"<<numregle<<endl;
                else
                    cout<<"Simplifie(D):"<<etiquette<<endl;
            }
            simpli->droit=LitUneSequence(fichier,XCatVide,negContexte,1,REGLES_SIMPLIFICATION);
            k=lit(fichier,tampon);
        }

        if (k=='w' || k=='W') {
            tampon[0]='w';
            k=lit(fichier,tampon+1);
        }

        if (!strcmp(tampon,"where") && k=='(') {
            TestArbre* tarbre=NULL;
            static VECTA<char> vari(5);

            //lecture du test... Il s'agit d'un test en debut de definition            
            tarbre=new TestArbre;
            k=' ';
            LectureTest(fichier,k,tampon,NON,vari,&tarbre);
            TVecteur clefArbre;
            tarbre->direct=tarbre->LineariseDirect(this,clefArbre,simpli->listes);
            delete tarbre;
            //simpli->test=tarbre;
        }
        
        if (k=='(')
            erreur(CHAINES[105]);

        if (k!='.') {
            delete simpli;
            erreur(CHAINES[132]);
        }            
        
        simpli->ConstruitClef();       
        AjouteStats(new stats(simpli,REGLES_SIMPLIFICATION,laCouche));
		AjouteDansBase(VBases,simpli,laCouche,etiqlab);

		if (laCouche==-1) {
			etiqlab->indexregle=ReglesSimplifie.dernier;
            ReglesSimplifie.ajoute(simpli);
		}
		else {
			etiqlab->indexregle=TableDesSimplifications[laCouche].dernier;
            TableDesSimplifications[laCouche].ajoute(simpli);                
		}
    }
}

//Cette fonction lit les regles qui marquent certains noeuds dans l'arbre...
//Ces regles sont de la forme: > Cat[traits]...Cat[traits].
void GlobalParseur::LitMarqueur(istream& fichier) {
    char k,tampon[300];

    EtatArbre* cle;
    Fonction* fonc;
    char fin=NON;
    int laCouche;    
    char* etiquette=NULL;
    

    while (!fin) {
        int numregle=VlabelVecteur.dernier;
        afficheregle(numregle,"Marking Rule");
        if (LectureParametres==OUI) {
            controle_max_regle++;
            if (controle_max_regle>max_controle_regle)
                erreur(CHAINES[190]);
        }

        long offset=fichier.tellg();
        k=lit(fichier,tampon);   
        unsigned int lignecourante=LIGNES+1;
		etiquette=lireRulespaceLabel(fichier,tampon,(uchar&)k,NON);

        if (k==':')
            if (ChoixLectureRegle(fichier,tampon)==NON)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));

        //On se moque du type de categorie en tete...
        fonc=new Fonction(XCatVide);


        //En revanche toutes regles doient commencer avec un '>'
        if (k!='>' || tampon[0] < '0' || tampon[0] > '9') {
            delete fonc;
            erreur(CHAINES[33]);
        }
 
        offset=fichier.tellg();
        offset-=strlen(tampon)+1;

        int couchelocale=atoi(tampon);
        laCouche=BaseCoucheNumero+couchelocale;
        
        
        if (laCouche<1 || laCouche>MaxCouches)
            erreur(CHAINES[107]);

        if (CouchesActives[laCouche]!=VIDE && 
            CouchesActives[laCouche] != REGLES_MARQUAGE)
            erreur(Concatene(CHAINES[208],tampon));

        if (MinProfondeurAnalyse > laCouche)
            MinProfondeurAnalyse=laCouche;

        if (MaxProfondeurAnalyse < laCouche)
            MaxProfondeurAnalyse=laCouche;
        
        CouchesActives[laCouche]=REGLES_MARQUAGE;
        

        EtatArbre* queue=NULL;
        if (afficheautomate==OUI) {
            cout<<endl;
            if (etiquette==NULL || etiquette[0]=='#')
                cout<<"Marqueur:"<<VlabelVecteur.dernier<<endl;
            else
                cout<<"Marqueur:"<<etiquette<<endl;
        }

        cle=ConstructionAutomate(fichier,fonc,NON,'.',NON,1,REGLES_RECONSTRUCTION_MODELE,&queue); 

        if (ERREUR_SURVENUE) {
            delete fonc;
            return;
        }

        
        if (cle->arcs.dernier!=1) {
            delete fonc;
            delete cle;
            erreur(CHAINES[197]);
        }
                
        fonc->categorie=(cle->arcs[0]->X)->val;
        fonc->regle=cle;
		fonc->comptevariables();
        //fonc->ConstruitClef(XCatEpsilon);
        fonc->numero=VlabelVecteur.dernier;
        Label* etiqlab=AjouteLabel(VlabelVecteur,etiquette,REGLES_MARQUAGE,laCouche,
            nbMarqueur,new DecompteRegle(REGLES_MARQUAGE,NOM,offset,lignecourante,laCouche,couchelocale));
        AjouteStats(new stats(fonc,REGLES_MARQUAGE,laCouche));
		AjouteDansBase(VBases,fonc,laCouche,etiqlab);
        
		etiqlab->indexregle=TableDesMarqueurs[laCouche].dernier;
        TableDesMarqueurs[laCouche].ajoute(fonc);        
    }
}


   
     //Cette fonction lit les regles qui marquent certains noeuds dans l'arbre...
//Ces regles sont de la forme: > Cat[traits]...Cat[traits].
void GlobalParseur::LitReconstruction(istream& fichier) {
    char k,tampon[300];

    
    char fin=NON;
    int laCouche;    
    ReconstruitArbre* ra;
    char* etiquette=NULL;

    while (!fin) {
        int numregle=VlabelVecteur.dernier;
        afficheregle(numregle,"Reshuffling Rule");

        if (LectureParametres==OUI) {
            controle_max_regle++;
            if (controle_max_regle>max_controle_regle)
                erreur(CHAINES[190]);
        }


        long offset=fichier.tellg();
        k=lit(fichier,tampon);   
        unsigned int lignecourante=LIGNES+1;
        etiquette=lireRulespaceLabel(fichier,tampon,(uchar&)k,NON);

        if (k==':')
            if (ChoixLectureRegle(fichier,tampon)==NON)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));

        //On se moque du type de categorie en tete...
        ra=new ReconstruitArbre;


        //En revanche toutes regles doivent commencer avec un '>'
        if (k!='>' || tampon[0] < '0' || tampon[0] > '9') {
            delete ra;
            erreur(CHAINES[33]);
        }
 
        offset=fichier.tellg();
        offset-=strlen(tampon)+1;

        int couchelocale=atoi(tampon);
        laCouche=BaseCoucheNumero+couchelocale;

        if (laCouche<1 || laCouche>MaxCouches)
            erreur(CHAINES[107]);

        if (CouchesActives[laCouche]!=VIDE && 
            CouchesActives[laCouche] != REGLES_RECONSTRUCTION)
            erreur(Concatene(CHAINES[208],tampon));

        if (MinProfondeurAnalyse > laCouche)
            MinProfondeurAnalyse=laCouche;
        
        if (MaxProfondeurAnalyse < laCouche)
            MaxProfondeurAnalyse=laCouche;
        
        CouchesActives[laCouche]=REGLES_RECONSTRUCTION;
       

        ra->numero=VlabelVecteur.dernier;
        VECTA<char> vari(5);
        Fonction fonc(XCatVide);
        EtatArbre* queue=NULL;
        fonc.numero=ra->numero;
        
        if (afficheautomate==OUI) {
            cout<<endl;
            if (etiquette==NULL || etiquette[0]=='#')
                cout<<"Reconstruction:"<<VlabelVecteur.dernier<<endl;
            else
                cout<<"Reconstruction:"<<etiquette<<endl;
        }

        //Pointe sur notre premier noeud
        int debutetat=listeEtatArbre.etats.dernier;

        EtatArbre* patcle=ConstructionAutomate(fichier,&fonc,NON,'=',NON,1,REGLES_RECONSTRUCTION,&queue);        

        if (ERREUR_SURVENUE) {
            delete ra;
            return;
        }
        
        //Nous parcourons tous les etats crees pour affecter des variables aux noeuds
        //qui n'en n'ont pas, en partant du numero le plus extreme donne
        int valvar=fonc.marque.dernier-1;
        for (int ie=debutetat;ie<listeEtatArbre.etats.dernier;ie++) {
            if (listeEtatArbre.etats[ie]!=NULL) {
                VECTA<ArcArbre*>& lesarcs=listeEtatArbre.etats[ie]->arcs;
                int ia;
                if (lesarcs.dernier) {
                    //Pour chaque arc sans argument, nous lui en affectons un                    
                    for (ia=0;ia<lesarcs.dernier;ia++) {
                        if (lesarcs[ia]->argument==-1) {
                            ra->ajoutvariable=OUI;
                            valvar++;
                            lesarcs[ia]->argument=valvar;
                            
                        }
                    }
                }
            }
        }

        vari.affecte(fonc.marque);
        fonc.marque.raz();
        ra->nbvariables=valvar;
        ra->patron=patcle;
        ra->categorie=patcle->arcs[0]->X->val;
        fonc.regle=patcle;
		ra->aplat=fonc.aplat;
        //fonc.ConstruitClef(XCatEpsilon);                
        ra->clef=fonc.clef;
        patcle->clef=fonc.clef;
        fonc.regle=NULL;
        fonc.listes=NULL;

        //patcle = ConstruitAutomate(fichier,&fonc,nettoyage,NON,'.',NON,1,REGLES_RECONSTRUCTION_MODELE);
        queue=NULL;
        patcle=ConstructionAutomate(fichier,&fonc,NON,'.',NON,1,REGLES_RECONSTRUCTION_MODELE,&queue);        
        

        //verification des variables
        for (int vf=0;vf<fonc.marque.dernier;vf++)
            if (fonc.marque[vf]==OUI)
                if (vari[vf]==NON)
                    erreur(CHAINES[127]);

        ra->modele=patcle;
        ra->testPatron=fonc.listes;
        fonc.listes=NULL;
        
        if (patcle==NULL) {
            delete ra;
            erreur(CHAINES[370]);
        }

        if (patcle->arcs.dernier!=1) {
            delete ra;
            erreur(CHAINES[197]);
        }

        
        Label* etiqlab=AjouteLabel(VlabelVecteur,etiquette,REGLES_RECONSTRUCTION,
            laCouche,nbReconstruction,new DecompteRegle(REGLES_RECONSTRUCTION,NOM,offset,lignecourante,laCouche,couchelocale));
        AjouteStats(new stats(ra,REGLES_RECONSTRUCTION,laCouche));
		AjouteDansBase(VBases,ra,laCouche,etiqlab);
		etiqlab->indexregle=TableDesReconstructeurs[laCouche].dernier;
        TableDesReconstructeurs[laCouche].ajoute(ra);        
    }
}         


void GlobalParseur::LitProcedure(istream& fichier) {
    char k,tampon[300];
    
    int total=0;

    TVecteur zoneLocale= MoinsUn;
    

    while (!fichier.eof()) {                

        k=lit(fichier,tampon);
        if (LectureParametres==OUI) {
            controle_max_regle++;
            if (controle_max_regle>max_controle_regle)
                erreur(CHAINES[190]);
        }

        if (k==':') {
            if (ChoixLectureRegle(fichier,tampon)==NON)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        }

        Dependance* depend= ConstruitNomProcedure(fichier,k,tampon);
        

        k=lit(fichier,tampon);

        //S'il s'agit d'une simple declaration, on ne va pas plus loin
        if (k=='.') {
            depend->Fx->type=C_PROCEDUREDECLAREE;
            delete depend;
            continue;
        }

        if (k!='{')
            erreur(CHAINES[34]);
        
        MetEnAttenteLecture(k,tampon);

        depend->Fx->type=C_PROCEDURE;
        int num_nouveau=ListeDesProcedures.dernier;
        
        nom_procedure=depend->Fx->nom;
        LitLesDeductions(fichier,DEDUCTIONPROCEDURE);
        nom_procedure=NULL;

        VectTFInstruction* vtf=ListeDesProcedures.fin();
        IndexProcedures.affecte(depend->Fx->index,num_nouveau);

        //Le dernier element doit etre un return
        TestFonctionMath* tfm=NULL;
        if (vtf->Type()==2)
            tfm=(TestFonctionMath*)((VectTFUneInstruction*)vtf)->test;

        if (tfm==NULL ||                        
            tfm->Type()!=TESTFONCTIONMATH ||
            tfm->formule==NULL ||
            tfm->formule->element==NULL ||
            tfm->formule->element->type!=VOPERATEUR || 
            tfm->formule->element->valeur_operation()!=43)
            erreur(CHAINES[306]);

        vtf=ListeDesProcedures[num_nouveau];
        vtf->dependances.ajoute(depend);
        vtf->premiertest=NON;
                

        //lecture du test... Il s'agit d'un test en debut de definition
        //C'est un test de type "where=deterministe"
        vtf->typeTest=INSTRUCTION_DETERMINISTE;
        vtf->numero=numero_procedure++;        
    }
}


#ifdef XIPPYTHON
    //debugstring=""
    //while (debugstring!="exit"):
    //  debugstring=raw_input("Debug: ")
    //  try:
    //    exec(debugstring)
    //  except:
    //    print "Error..."

string insere_classes_xip(char);
void minuscule(unsigned char* c);
//Lecture d'un programme Python
//Attention, la premiere partie correspond a la lecture de l'interface
//Le reste est le code proprement dit
char TestBlanc(string& ligne) {
	for (int i=0;i<ligne.size();i++)
		if (ligne[i]>32)
			return NON;
	return OUI;
}

char TestFinLigne(string& ligne) {
	int i=ligne.size()-1;
	for (;i>0;i--)
		if (ligne[i]>32)
			break;
	if (!i || ligne[i]!='\\')
		return NON;

	char* ch=(char*)ligne.c_str();
	ch[i]=' ';
	return OUI;
}

void GlobalParseur::LitProcedurePython(istream& fichier) {
    char k,tampon[1000];
    char espace_module[100];
    char nom[100];
    int total=0;	
    vector<string> noms;
	int bd;

    TVecteur zoneLocale= MoinsUn;
    
    ostream* os=&cout;
    if (OS!=NULL)
        os=OS;

    k=lit(fichier,nom);
    if (LectureParametres==OUI) {
        controle_max_regle++;
        if (controle_max_regle>max_controle_regle)
            erreur(CHAINES[190]);
    }
    
    //Il ne peut y avoir que du python dans ce fichier
    if (k==':') 
        return;
    	
	Dependance* depend;
	VECTA<Dependance*> depends;
	while (!fichier.eof() && k!='.') {
		depend= ConstruitNomProcedure(fichier,k,nom);
		depends.ajoute(depend);
		noms.push_back(nom);
		depend->Fx->type=C_PROCEDUREPYTHON;
		k=lit(fichier,tampon);

		//S'il s'agit d'une simple declaration, on ne va pas plus loin
		if (k!='.' && k!=',')
			erreur(CHAINES[358]);
		if (k==',')
			k=lit(fichier,nom);
	}

	if (fichier.eof())
		erreur(CHAINES[358]);
    initialisation_python();
	char initialise_premiere_fois=NON;
	if (module_principal==NULL) {
		initialise_premiere_fois=OUI;
        module_principal= PyImport_ImportModule("__main__");        
	}

    PyObject* pDict_principal = PyModule_GetDict(module_principal);
    PyObject* pFunc=NULL;
    PyObject* pDict=NULL;
	int j;

    if (importation_python==NON) {
		int num_ligne=LIGNESERREUR;

        *flot_erreur<<" module number:"<<num_module_python<<" (stored in __main__)"<<Endl;
        //A partir de la on lit le code python
        unsigned char c=LectCrypte(fichier);    
        //on cree le domaine des variables globales
        string che="from xips import *\n";
		if (python_debug==OUI && num_module_python==num_fichier_python) {
			che+="step_by_step=0\n";
			che+="debug_line_number=";
			sprintf(tampon,"%d\n",num_python_ligne);
			che+=tampon;

			che+="def test_mode_debug(num):\n";
			che+="   global step_by_step\n";
			che+="   global debug_line_number\n";
			che+="   if (step_by_step==1 or debug_line_number==num):\n";
			che+="       return 1\n";
			che+="   return 0\n\n";

			che+="def input_mode_debug():\n";
			che+="   global step_by_step\n";
			che+="   global debug_line_number\n";
			che+="   debugstring=' '\n";
			che+="   debugstring=raw_input(\"Debug(i(nside),n[ext],e[nd],g[o] line): \")\n";
			che+="   if debugstring=='e':\n";
			che+="      step_by_step=0\n";
			che+="      debug_line_number=-1\n";
			che+="      return 0\n";
			che+="   if debugstring=='i' or debugstring=='':\n";
			che+="      step_by_step=1\n";
			che+="      return 0\n";
			che+="   if debugstring=='n':\n";
			che+="      step_by_step=0\n";
			che+="      debug_line_number=debug_line_number+1\n";
			che+="      return 0\n";
			che+="   if debugstring[0]=='g':\n";
			che+="      step_by_step=0\n";
			che+="      try:\n";
			che+="        debug_line_number=int(debugstring[2:])\n";
			che+="        return 0\n";
			che+="      except:\n";
			che+="        return 0\n";
			che+="   return debugstring\n";
		}

        //on rajoute quelques retours chariot pour que python
        //s'y retrouve dans le numero de lignes dans le fichier
        for (int i=0;i<LIGNES-1;i++)
            che+=10;
        string ligne="";
		string fonction_chaine;
		char ajout_global=NON;
		int num_def_ligne;
        while (!fichier.eof()) {        
            if (c<32 && c!=9 && c!=10) {
                c=LectCrypte(fichier);    
                continue;
            }        
				
            //les tabulations sont virees...
            if (c==9)
                c=32;

            //On vient de rencontrer une ligne
			if (python_debug==OUI && num_module_python==num_fichier_python) {
				if (c==10) {		
					if (TestFinLigne(ligne)==OUI) {
						num_ligne++;
						c=LectCrypte(fichier);
						continue;
					}
					if (TestBlanc(ligne)==NON && ligne[0]==' ' && 
						ligne.find("class ",0)==-1 && 
						ligne.find("import ",0)==-1 && 
						ligne.find("def ",0)==-1 &&
						ligne.find("except",0)==-1) {
						//ensuite on recupere tous les blancs en tete
						j=0;
						string blc="";
						while (ligne[j]==' ' || ligne[j]==9) {
							blc+=ligne[j];
							j++;
						}
						//Les commentaires sont evidemment sautes
						if (ligne[j]=='#') {
							while (!fichier.eof() && c!=10)
								c=LectCrypte(fichier);
							ligne="";
							if (c==10)
								c=LectCrypte(fichier);
							num_ligne++;
							continue;
						}

						string sub;
						if (ajout_global==OUI) {
							ajout_global=NON;
							che+=blc;
							che+="print '";
							sprintf(tampon,"%d:",num_def_ligne);
							che+=tampon;
							sub=fonction_chaine;
							char* csub=(char*)sub.c_str();
							for (j=0;j<sub.size();j++) {
								if (csub[j]=='\'' || csub[j]=='\"')
									csub[j]=' ';
							}
							che+=sub;
							che+="'\n";
						}
						sub=ligne;
						char* csub=(char*)sub.c_str();
						for (j=0;j<sub.size();j++) {
							if (csub[j]=='\'' || csub[j]=='\"')
								csub[j]=' ';
						}
						//On rajoute un print dans le tas
						che+=blc;
						sprintf(tampon,"if test_mode_debug(%d)==1:\n",num_ligne);
						che+=tampon;
						blc+=" ";
						che+=blc;
						che+="print '";						
						sprintf(tampon,"%d:",num_ligne);
						che+=tampon;
						che+=sub;
						che+="'\n";
						che+=blc;
						che+="debugstring=input_mode_debug()\n";
						che+=blc;
						che+="while debugstring!=0:\n";
						che+=blc;
						che+="  debugstring='print '+debugstring\n";
						che+=blc;
						che+="  try:\n";
						che+=blc;
						che+="    exec(debugstring)\n";
						che+=blc;
						che+="  except:\n";
						che+=blc;
						che+="    print 'Error...'\n";
						che+=blc;
						che+="  debugstring=input_mode_debug()\n";
					}
					che+=ligne;
					che+=c;
					if (ligne.find("def ",0)!=-1) {
						ajout_global=OUI;
						fonction_chaine=ligne;
						num_def_ligne=num_ligne;
					}
					ligne="";
					num_ligne++;
				}
				else
					ligne+=c;
			}
			else
				che+=c;

            c=LectCrypte(fichier);
        }
        
		//ofstream codegenere("C:\\XIP\\test\\code.txt");
		//codegenere<<che.c_str()<<endl;
		//codegenere.close();
		if (initialise_premiere_fois==NON) {
			for (bd=0;bd<noms.size();bd++) {
				pFunc = PyDict_GetItemString(pDict_principal, noms[bd].c_str());
				if (pFunc!=NULL) {
					*os<<CHAINES[363]<<Endl;
					return;
				}
			}
		}
        PyRun_SimpleString(che.c_str());
        //pFunc = PyDict_GetItemString(pDict_principal, nom);        
        pDict=pDict_principal;
    }
    else {
        //On importe le code python... sous la forme d'une importation
        //NOM contient le nom du module a importer
		try {
			((ifstream&)fichier).close();
		}
		catch(...) {
		}
        //L'importation se fait avec PyImport_Import
        //Le nom du module est le nom de notre fichier
        //sans son extension
        char* pt=strrchr(NOM,SEP);
        string chemin="import sys\nimport traceback\n";

        if (pt!=NULL) {
            strcpy(espace_module,pt+1);
            *pt=0;
            string ch;
            if (SEP=='\\') {
                char* str=NOM;
                char* ptt=strchr(NOM,SEP);
                while (ptt!=NULL) {
                    *ptt=0;
                    ch+=str;
                    ch+="\\\\";
                    *ptt=SEP;
                    str=ptt+1;
                    ptt=strchr(str,SEP);
                }
                ch+=str;
            }
            else
                ch=NOM;
            chemin+="sys.path.append(\"";
            chemin+=ch;
            chemin+="\")\n";
            *pt=SEP;
        }
        else
            strcpy(espace_module,NOM);
                
        //On retire le point
        pt=strchr(espace_module,'.');
        if (pt!=NULL)
            *pt=0;
		*flot_erreur<<" module number:"<<num_module_python<<" (stored as "<<espace_module<<")"<<Endl;		
        chemin+="import ";
        chemin+=espace_module;
        chemin+="\n";
        PyRun_SimpleString(chemin.c_str());
        espace_local=PyDict_GetItemString(pDict_principal, espace_module);        
        if (espace_local!=NULL) {
            pDict=PyModule_GetDict(espace_local);
            chemin="from xips import *\n";
            //chemin+=insere_classes_xip(NON);
            PyRun_String(chemin.c_str(),Py_file_input,pDict_principal,pDict);
            //pFunc = PyDict_GetItemString(pDict, nom);        
        }
    }

	num_module_python++;

    /*
    PyObject* key;
    PyObject* value;
    int pos=0;
    cout<<endl;
    cout<<"LOCAL"<<endl<<endl;
    while (PyDict_Next(pDict, &pos, &key, &value)) {
        if (key!=NULL && PyString_Check(key))
            cout<<"Cle="<<PyString_AsString(key)<<endl;
        if (value!=NULL && PyString_Check(value))
            cout<<"Value="<<PyString_AsString(value)<<endl;
        PyModule_AddObject(espace_local, PyString_AsString(key), value);
    }

    cout<<endl<<endl<<"PRINCIPAL"<<endl<<endl;
    pos=0;
    cout<<endl;
    while (PyDict_Next(pDict_principal, &pos, &key, &value)) {
        if (key!=NULL && PyString_Check(key))
            cout<<"Cle="<<PyString_AsString(key)<<endl;
        if (value!=NULL && PyString_Check(value))
            cout<<"Value="<<PyString_AsString(value)<<endl;
    }
    */

	for (bd=0;bd<depends.dernier;bd++) {
		if (importation_python==NON)
			pFunc = PyDict_GetItemString(pDict_principal, noms[bd].c_str());
		else
			pFunc = PyDict_GetItemString(pDict, noms[bd].c_str());
		if (pFunc==NULL || !PyCallable_Check(pFunc)) 
			erreur(CHAINES[359]);

		ProcedurePython* tfp=new ProcedurePython(depends[bd],pFunc,pDict,(char*)noms[bd].c_str());
		tfp->nomfichier=NOM;
		PythonScript.ajoute(tfp);    
	}



    /*
    //On recherche tous les imports
    int pos=0;
    int previouspos=-1;
    string mainstr="";
    while (pos!=string::npos) {
        pos=che.find("import",pos);
        if (pos==string::npos)
            pos=che.find("from",pos);            
        if (pos!=string::npos) {
            //On va jusqu'au bout de la chaine...
            pos=che.find("\n",pos);
            previouspos=pos;
        }
    }

    
    initialisation_python();
    if (module_principal==NULL)
        module_principal= PyImport_ImportModule("__main__");        

    
    //On cree donc un  module specifique pour chaque procedure python de facon a
    //eviter les interferences
    char* pt=strrchr(Nom_Fichier,SEP);
    if (pt!=NULL)
        strcpy(tampon,pt+1);
    else
        strcpy(tampon,Nom_Fichier);
    //On retire le point
    pt=strchr(tampon,'.');
    if (pt!=NULL)
        *pt=0;
    sprintf(espace_module,"%s%d",tampon,num_module_python);
    espace_local=PyModule_New(espace_module);   
    
    if (espace_local==NULL) {        
        if (espace_local==NULL)
            erreur("The local Python space could not be created");
        cout<<endl<<"\tPython Module:"<<espace_module<<" has been created...";
        num_module_python++;
    }

    PyObject* pespace_module=PyString_FromString(espace_module);
    PyObject* plocal_module=PyImport_Import(pespace_module);
    PyObject* pFunc;
    ProcedurePython* tfp;

    PyObject* pDict_principal = PyModule_GetDict(module_principal);
    PyObject* pDict= PyModule_GetDict(espace_local);

    //On charge et on execute alors le code en memoire    
    if (previouspos!=string::npos) {
        mainstr=che.substr(0,previouspos+1);
        che=che.erase(0,previouspos+1);
        PyRun_SimpleString(mainstr.c_str());
    }
    
    PyRun_String(che.c_str(),Py_file_input,pDict_principal,pDict);    
    PyObject* perreur=PyErr_Occurred();
    if (perreur!=NULL) {
        PyObject *ptype;
        PyObject *pvalue; 
        PyObject *ptraceback;
        PyErr_Fetch(&ptype,&pvalue,&ptraceback);                
        ostringstream os;
        os<<"Error in file:"<<Endl;
        
        if (pvalue!=NULL && PyString_Check(pvalue))
            os<<"In python procedure: "<<PyString_AsString(pvalue)<<Endl;
        
        if (ptype!=NULL)
            Py_DECREF(ptype);
        if (pvalue!=NULL)
            Py_DECREF(pvalue);
        if (ptraceback!=NULL)
            Py_DECREF(ptraceback);
        os<<Endl;
        //erreur((char*)os.str().c_str());
        cout<<(char*)os.str().c_str()<<endl;
    }

    
    PyObject* key;
    PyObject* value;
    pos=0;
    cout<<endl;
    while (PyDict_Next(pDict, &pos, &key, &value)) {
        cout<<"Cle="<<PyString_AsString(key)<<endl;
        if (PyString_Check(value))
            cout<<"Value="<<PyString_AsString(value)<<endl;
        PyModule_AddObject(espace_local, PyString_AsString(key), value);
    }
    pos=0;
    cout<<endl;
    while (PyDict_Next(pDict_principal, &pos, &key, &value)) {
        cout<<"Cle="<<PyString_AsString(key)<<endl;
        if (PyString_Check(value))
            cout<<"Value="<<PyString_AsString(value)<<endl;
    }
    //Il faut qu'il y ait au moins une reference dans le script python correspondant
    //a l'interface        


    pFunc = PyDict_GetItemString(pDict, nom);
    if (pFunc==NULL || !PyCallable_Check(pFunc)) 
        erreur(CHAINES[359]);
    */
}

#endif

//-----------------------------------------------------------------------
void GlobalParseur::LitExtraction(istream& fichier) {
    char k,tampon[300];
    char* etiquette=NULL;

    int total=0;

    TVecteur zoneLocale= MoinsUn;
    
    long offset=fichier.tellg();
    k=lit(fichier,tampon);
    unsigned int lignecourante=LIGNES+1;

    while (!fichier.eof()) {
        if (LectureParametres==OUI) {
            controle_max_regle++;
            if (controle_max_regle>max_controle_regle)
                erreur(CHAINES[190]);
        }
        
		etiquette=lireRulespaceLabel(fichier,tampon,(uchar&)k,NON);
        
        if (k==':') {
            if (ChoixLectureRegle(fichier,tampon)==NON)
                return;
            else
                erreur(Concatene(CHAINES[94],tampon));
        }


        //Lecture d'une zone
        if (k=='<') {
            k=lit(fichier,tampon);
            if (strcmp(tampon,CHAINES[47]))
                erreur(CHAINES[89]);
            zoneLocale=0;
            while (k!='>') {
                k=lit(fichier,tampon);
                if (tampon[0]=='*') 
                    zoneLocale=MoinsUn;
                else {
                    int vz=atoi(tampon);
                    zoneLocale|= (Un << vz);
                }
            }
            k=lit(fichier,tampon);
        }
        
        int laCouche=-1;

		int couchelocale=-1;
        if (k=='>') {
            offset=fichier.tellg();
            offset-=strlen(tampon)+1;
            couchelocale=atoi(tampon);
            laCouche=BaseCoucheNumero+couchelocale;
            
            if (laCouche<1 || laCouche>MaxCouches)
                erreur(CHAINES[107]);
            
            if (CouchesActives[laCouche]!=VIDE && 
                CouchesActives[laCouche] != REGLES_DEDUCTION)
                erreur(Concatene(CHAINES[208],tampon));
            
            if (MinProfondeurAnalyse > laCouche)
                MinProfondeurAnalyse=laCouche;
            
            if (MaxProfondeurAnalyse < laCouche)
                MaxProfondeurAnalyse=laCouche;
            
            CouchesActives[laCouche]=REGLES_DEDUCTION;                
            k=lit(fichier,tampon);
        }

        VectTFInstruction* vtf=new VectTFInstruction;
        vtf->typeTest=INSTRUCTION_DEPENDANCE;
        static VECTA<char> vari;


        //lecture du test... Il s'agit d'un test en debut de definition
        if (!strcmp(tampon,"if")) {
            //C'est un test de type "where=deterministe"
            vtf->typeTest=INSTRUCTION_DETERMINISTE;
            //lecture de la parenthese...
            if (k!='(')
                erreur(CHAINES[30]);

            TestArbre* tarbre=NULL;
            
            tarbre=new TestArbre;
            k=' ';
            LectureTest(fichier,k,tampon,NON,vari,&tarbre);
            tarbre->direct=tarbre->LineariseDirect(this,vtf->clef,vtf->listes);
            delete tarbre;
        }


        
        vari.raz();

        Dependance* depend= ConstruitDependance(fichier,vari,k,tampon,NON);
        vtf->dependances.ajoute(depend);

        k=lit(fichier,tampon);

        if (k=='!') {
            k=lit(fichier,tampon);
            vtf->deterministe=courte;
        }
        else
            if (k=='@') {
                k=lit(fichier,tampon);
                vtf->deterministe=longue;
            }

        if (k!='=')
            erreur(CHAINES[34]);
        
        if (afficheautomate==OUI) {
            cout<<endl;
            if (etiquette==NULL || etiquette[0]=='#')
                cout<<"Extraction:"<<VlabelVecteur.dernier<<endl;
            else
                cout<<"Extraction:"<<etiquette<<endl;
        }
        
        LitUnAutomate(fichier,XCatVide,vtf,k,REGLES_DEDUCTION,'.');
        Fonction* regle=vtf->regle;
        if (regle!=NULL)
            vtf->regle_contexte=OUI;
        
        //On verifie si tous les arguments de la dependance sont presents dans la regle
        for (int vi=0;vi<vari.dernier;vi++)
            if (vari[vi]==OUI && regle->arg[vi] ==NON)
                erreur(CHAINES[194]);
            
        vtf->zones=zoneLocale;
        vtf->numero=VlabelVecteur.dernier;
        Label* la=AjouteLabel(VlabelVecteur,etiquette,REGLES_EXTRACTION,-1,nbInference,
            new DecompteRegle(REGLES_EXTRACTION,NOM,offset,lignecourante,laCouche,couchelocale));
        
        AjouteStats(new stats(vtf,REGLES_EXTRACTION,laCouche));
		AjouteDansBase(VBases,vtf,laCouche,la);

//En revanche l'index est autre
        if (laCouche==-1) {
            la->indexregle= ListeDesDeductions[laCoucheDeduction].dernier;                        
			vtf->position=la->index;
            ListeDesDeductions[laCoucheDeduction].ajoute(vtf);            
        }
        else {
            la->indexregle=ListeDesDeductionsEnCouche[laCouche].dernier;
            vtf->position=la->index;
            ListeDesDeductionsEnCouche[laCouche].ajoute(vtf);      
        }
        
        offset=fichier.tellg();
        k=lit(fichier,tampon);
        lignecourante=LIGNES+1;
    }
}

void GlobalParseur::ConstruitLesFilsFictifs(Liste* L,
                                            istream& fichier,
                                            char* tampon,
                                            char& k,
                                            VECTA<char>& vari,
                                            char test) {


    int num_arg;
    Liste* lfils=NULL;
    while (k!='}') {
        k=lit(fichier,tampon);
        if (k!='#')
            erreur(CHAINES[361]);
        k=lit(fichier,tampon);
        if (k!='#')
            erreur(CHAINES[361]);
        num_arg=NOUVEAU;
        k=lit(fichier,tampon);
        Categorie* c=ChercheCat(tampon);
        if (k=='#') {
            //Une variable
            k=lit(fichier,tampon);
            int numargbis=atoi(tampon);
            //Verification que cette variable 
            //est bien absente dans les tests...
            if (test==NON)
                vari.affecte(numargbis,OUI);
            else
                if (vari[numargbis]==OUI)
                    erreur(CHAINES[249]);
                else
                    vari.affecte(numargbis,OUI);
                
                //On le met en negatif
                num_arg=NOUVEAU+numargbis;
        }
        
        Traits* traits=BatitTraitsPlus(fichier,c,&k,tampon);
        lfils=NULL;
        if (k!=',' && k!='}')
            erreur(CHAINES[39]);

        static char ajouter;
        if (k=='{') {
            lfils=FactoriseNoeudFictif(traits,c,NON,ajouter);
            ConstruitLesFilsFictifs(L,fichier,tampon,k,vari,test);
        }
        else
            lfils=FactoriseNoeudFictif(traits,c,NONECRIT,ajouter);

        if (L->N.fils==NULL)
            L->N.fils=&lfils->N;        
        else
            L->N.fils->insere(&lfils->N);
        lfils->N.pere=&L->N;
    }
    k=lit(fichier,tampon);
}

//------------------------------------------------------------------------------
//Il s'agit de la construction du resultat d'une regle de deduction ou semantique
//Une simple dependance ou une destruction de dependance
Dependance* GlobalParseur::ConstruitDependance(istream& fichier, 
                                               VECTA<char>& vari, 
                                               char k,
                                               char* tampon,
                                               char test,
											   typeDeduction type) {
    Dependance* depend=NULL;
    int duree=0;
    Categorie* c;
    char suite[100];
    char neg=k;
    char curseur=NON;


    int numdep=-1;
    /*
      Nous avons les cas suivants:
      ~   il s'agit alors de la destruction de l'element marque dans le texte.
          vardep=-1
          modification=NON
          Fx=XCatNeg

      ~$1 il s'agit de la destruction d'une variable
          vardep=2
          modification=OUI
          Fx = XCatNeg

     $1[..](..)   cas avec une variable
          vardep=2
          modification=NON
          Fx = ?
          
    ^$1[..](..)  cas de modification d'une variable
         vardep=2
         modification=OUI
         Fx=?

    dependance[..](..) cas standard
           vardep=-1
           modification=NON
           Fx=dependance

    ^dependance$1[..](..) cas particulier
           vardep=2
           modification=OUI
           Fx=dependance
    */

    //Si k==~ il s'agit d'une destruction de la fonction
    //et non de son remplacement...
    if (k==NEGATION) {
        depend=new Dependance(XCatNeg);
        k=lit(fichier,tampon);
    }
    else 
        if (k=='^') {
            curseur=OUI;
            k=lit(fichier,tampon);
        }

    if (tampon[0]=='$') {
		if (tampon[1]>='0' && tampon[1]<='9')
			numdep=atoi(tampon+1)+1;
		else
			if (tampon[1]!=0)
				numdep=VariableGeneration(tampon+1,false);
        tampon[0]='?';
        tampon[1]=0;
		//Variable $#
		if (k=='#') {
			k=lit(fichier,suite);
			numdep=atoi(suite)+1;
		}
    }
    else
        if (tampon[0]=='?')
            numdep=1;

    if (neg==NEGATION) {
        //Si ce n'etait pas un ~$1, alors on remet le tampon et k en attente...
        if (numdep==-1) {
            MetEnAttenteLecture(k,tampon);
        }
        else {
            MetEnAttenteLecture(k,"");
        }

        depend->modification=OUI;     
        depend->vardep=numdep;
        return depend;
    }
    
    //si la dependance est fournie et avec une variable
    char* pt=strchr(tampon,'$');
    if (pt!=NULL) {
		if (k=='#') {
			//Il s'agit d'une variable de generation
			k=lit(fichier,suite);
			numdep=atoi(suite)+1;
		}
		else
			if (pt[1]>='0' && pt[1]<='9')
				numdep=atoi(pt+1)+1;
			else
				numdep=VariableGeneration(pt+1,false);

		//Dans ce cas present, la presence d'une variable accompagnee de sa dependance
		//indique une modification
		curseur=OUI;
        *pt=0;
    }

    if (curseur==OUI && numdep==-1)
        erreur(CHAINES[225]);
     

    //dans ce cas on peut se retrouver a creer aussi une procedure
    if (test==XIPPROC)
        c=CreeProcedure(tampon);
    else {
        c=ChercheFonc(tampon);
        if (c->X.type==C_PROCEDURE)
            erreur(CHAINES[254]);
    }

	if (type==DEDUCTIONGENERATION) {
		if (c->X.type!=C_GENERATION && c->X.nom[0]!='?') {
			sprintf(suite,"%s: %s",CHAINES[392],c->X.nom);
			erreur(suite);
		}
	}
        
    depend=new Dependance(&c->X);
    depend->vardep=numdep;
    depend->modification=curseur;
    

    Traits* tr=NULL;
    //Lecture des traits associes aux arguments...
    if (k=='[') {
        tr=BatitTraits(fichier,']',&depend->stack,&depend->clearStack);
        k=lit(fichier,suite);
    }

    char PasDArgument=NON;
	if (k != '(') {
        if (c==CatVide) {
            PasDArgument=OUI;
            RemetCaractere(k);
        }
		else {
			if (numdep<=1)
				erreur(CHAINES[55]);
			else {
				PasDArgument=OUI;
				RemetCaractere(k);
			}
		}
	}
        
    depend->traits=FactoriseTraits(tr);
    depend->duree=duree;

        //lecture des arguments...
    int i=0;
    int num_arg=-1; 
	Traits* traits;
	char ajouter;
	Liste* L;
	int numargbis;
	bool lepremier = OUI;
    while (PasDArgument==NON && k != ')') {
        k=lit(fichier,tampon);
		//Nous acceptons six formes differentes:
		//1. ##CAT#1[ NOUVEAU
		//2. ##CAT[ NOUVEAU
		//3. CAT#1[ NOUVEAU
		//4. CAT[ NOUVEAU
		//5. ##1 DUPLIQUE
		//6. #1 PARTAGE
		numargbis=-1;
		c=CatVide;
		L=NULL;
		if (lepremier && k == ')' && depend->Fx->type == C_FONCTION) 
			erreur("Missing parameters in a dependency declaration");
		
		num_arg=NOUVEAU;
		lepremier = NON;
		if (tampon[0]==0) {
			if (k=='#') {//cas 1,2,5,6
				k=lit(fichier,tampon);
				if (k=='#') {//cas 1,2,5				
					k=lit(fichier,tampon);
					if (tampon[0]!=0) {
						if (tampon[0]>'0' && tampon[0]<'9') {//cas 5
							//Il s'agit d'une duplication d'un noeud fourni par une variable
							//avec aussi quelques traits
							num_arg=DUPLIQUE+atoi(tampon);					
							traits=BatitTraitsPlus(fichier,CatVide,&k,tampon);
							L=FactoriseNoeudFictif(traits,CatVide,NON,ajouter);
							NoeudsFictifs.ajoute(L);
							TraitsNoeudsFictifs.ajoute(traits);
						}
						else {
							c=ChercheCat(tampon); //cas 1,2
							if (k=='#') {//cas 1
								k=lit(fichier,tampon);
								numargbis=atoi(tampon);
								num_arg=NOUVEAU+numargbis;
							}
							traits=BatitTraitsPlus(fichier,c,&k,tampon);
						}
					}				
				}
				else {//cas 6
					//Une variable
					num_arg=atoi(tampon);
					//Verification que cette variable 
					//est bien presente dans les tests...
					if (test!=OUI)
						vari.affecte(num_arg,OUI);
					else
						if (vari[num_arg]==NON)
							erreur(CHAINES[32]);
				}
			}
		}
		else {//cas 3,4
			c=ChercheCat(tampon);			
			if (k=='#') {//cas 3
				k=lit(fichier,tampon);
				numargbis=atoi(tampon);
				num_arg=NOUVEAU+numargbis;
			}
			traits=BatitTraitsPlus(fichier,c,&k,tampon);
		}

		//Verification que cette variable 
		//est bien absente dans les tests...
		if (numargbis!=-1) {
			if (test==NON)
				vari.affecte(numargbis,OUI);
			else
				if (vari[numargbis]==OUI)
					erreur(CHAINES[249]);
				else
					vari.affecte(numargbis,OUI);
		}
		
		if (c!=CatVide) {
			if (k=='{') {
				L=FactoriseNoeudFictif(traits,c,NON,ajouter);
				ConstruitLesFilsFictifs(L,fichier,tampon,k,vari,test);
			}
			else
				L=FactoriseNoeudFictif(traits,c,OUI,ajouter);
		}

		if (L!=NULL)
			depend->noeuds.affecte(i,L);

		depend->arg.affecte(i,num_arg);

        if (k!= ',' && k!=')') {
            delete depend;
            erreur(CHAINES[133]);
        }
            
        i++;
    }
        
    depend->nb_args=i;

    return depend;
}

Dependance* GlobalParseur::ConstruitNomProcedure(istream& fichier, 
                                                 char k,
                                                 char* tampon) {
    NomProcedure* depend=NULL;
    Categorie* c;
    char neg=k;
    char curseur=NON;


    int numdep=-1;
     

    //dans ce cas on peut se retrouver a creer aussi une procedure
    c=CreeProcedure(tampon);

    depend=new NomProcedure(&c->X);
    depend->vardep=numdep;
    depend->modification=curseur;
    


    char PasDArgument=NON;
    if (k=='{') {
        depend->nb_args=depend->arg.dernier;
        MetEnAttenteLecture(k,"");
        return depend;
    }

    if (k != '(') 
        erreur(CHAINES[55]);
        

    static char inter[1000];
    k=lit(fichier,inter);
    
    //S'il n'y a pas de parametre
    if (k!=')' || inter[0]!=0) {
        MetEnAttenteLecture(k,inter);
        k=0;
    }


    //lecture des arguments...
    while (PasDArgument==NON && k != ')') {
        k=LitVariables(fichier,depend);
        if (k!= ',' &&  k!=')' && k!=';') {
            delete depend;
            erreur(CHAINES[133]);
        }        
    }
        
    depend->nb_args=depend->arg.dernier;

    return depend;
}


/*-------------------------------------------------------------------------*/
//Lecture des regles de deduction...
//Elles sont de la forme:
// if (... & ... | ...) {
// extraction 
//}

VectTFInstruction* VectTestFonction::derivation() {
    VectTFInstruction* vtf=new VectTFInstruction;
    vtf->script=script;
    vtf->position=position;
    vtf->premiertest=premiertest;
    vtf->finboucle=finboucle;
    vtf->numero=numero;
    vtf->typeTest=typeTest;
    vtf->regle_contexte=regle_contexte;
    return vtf;
}

//Dans le cas d'une forme tres simplifiee (une seule instruction)
//On renvoie un VectTFUneInstruction
VectTFUneInstruction* VectTFInstruction::simplification() {
    
    if (typeTest==INSTRUCTION_NORMALE &&
        listes.liste[0]->test->negation==SAUTE) {
        VectTFUneInstruction*  vtfune=new VectTFUneInstruction(listes.liste[0]->test,zones);
        vtfune->script=script;
        vtfune->position=position;
        vtfune->premiertest=premiertest;
        vtfune->finboucle=finboucle;
        vtfune->numero=numero;
        vtfune->typeTest=typeTest;
        vtfune->regle_contexte=regle_contexte;
        delete this;
        return vtfune;    
    }
    return NULL;
}

void GlobalParseur::litsousdependances(istream& fichier,uchar& k,char* tampon,SousDependance* sdep,VECTA<char>& vari) {
	SousDependance* sousdep=NULL;
	Dependance* depend=NULL;
	int l;

	k=lit(fichier,tampon);
	while (OUI) {
		if (tampon[0]=='$') {
			depend=ConstruitDependance(fichier,vari,k,tampon,OUI,DEDUCTIONGENERATION);         
			sdep->sousnoeuds.ajoute(depend);                   
			k=LectureMin(fichier);
			LectureBlancs(fichier,k);
			continue;
		}
		switch(k) {
		case '#'://Uniquement si le dernier caractere est un $
			l=strlen(tampon)-1;
			if (l>0 && tampon[l]=='$') {
				depend=ConstruitDependance(fichier,vari,k,tampon,OUI,DEDUCTIONGENERATION);         
				sdep->sousnoeuds.ajoute(depend);                   
				k=LectureMin(fichier);
				LectureBlancs(fichier,k);			
			}			
			break;
		case ',':               
			k=lit(fichier,tampon);
			break;
		case '{'://uniquement dans le cas des regles de generation						
			sdep->sousnoeuds.retire();
			if (depend==NULL)
				erreur("Wrong NODE structure");
			sousdep=new SousDependance(depend);			
			sdep->sousnoeuds.ajoute(sousdep);  
			litsousdependances(fichier,k,tampon,sousdep,vari);
			k=lit(fichier,tampon);
			break;
		case '}':
			k=lit(fichier,tampon);
			if (k!='.')
				MetEnAttenteLecture(k,tampon);
			return;
		case '[':
		case '(':
			depend=ConstruitDependance(fichier,vari,k,tampon,OUI,DEDUCTIONGENERATION);         
			sdep->sousnoeuds.ajoute(depend);                   
			k=LectureMin(fichier);
			LectureBlancs(fichier,k);
			break;
		default:
			erreur(CHAINES[394]);
		}
	}
}


void GlobalParseur::LitLesDeductions(istream& fichier,typeDeduction typededuc) {


    uchar k=0;
    char K;
    char tampon[1000];
    typeDeduction typedefaut=typededuc;
    char script=NON;

    switch(typededuc) {
    case DEDUCTIONSCRIPT:
        typedefaut=DEDUCTIONCOURANTE;
    case DEDUCTIONENCHARGEANT:
    case DEDUCTIONINITIALE:
    case DEDUCTIONPROCEDURE:
	case DEDUCTIONQUEUE:
    case DEDUCTIONFINALE:
	case DEDUCTIONPREBALISE:
	case DEDUCTIONPOSTBALISE:
        script=OUI;
    }

    ListeTestArbre* nouveau;
    TestFonctionMathBoucle* tfmb;
    TestFonctionMath* tfm;
    VECTA<TestFonctionDependance*> listeTests;
    VectTestFonction* vtf=NULL;
    VectTFInstruction* precvtf=NULL;
    VectTFInstruction* instructions=NULL;    
    VectTestFonction* ifvtf=NULL;    
    VectTFInstruction* vtfinstruction=NULL;
    VectTFUneInstruction* vtfune;
    TVecteur zoneLocale=MoinsUn;
    int vi;
    int laCouche=-1,couchelocale=-1;
    typeDeduction type_deduction=typedefaut;
    typeDeduction precedtype=typedefaut;
    char* etiquette=NULL;
    Label* la;
    tampon[0]=0;
    SousDependance* sousdep;
    VECTA<char> vari(20);
                                           
    int retireaccolade=0;
    unsigned int lignecourante;
    
    //ce noeud enregistre sous quel noeud doivent se trouver les deductions.
    Dependance* depend;
    char deterministe=NON;
    char ajouteVTF=OUI;
    char pasDeResultat=NON;
    char suiteInstruction=NON;
    TestArbre* tarbre=NULL;
    char changetypedefaut=OUI;
    
    char etat=0;
    int fi,idep,ich=0;    
	char testelse=NON;
    int fif=-1;
    VECTA<int> comptefor;
    VECTA<int> comptewhile;
    VECTA<int> compteaccolade;
    VECTA<int> etats_instructions;
    char boucle;
    char etatsuivant;
    Fonction* regle;
    char disjonction=NON;
    Categorie* nom_graphe=NULL;    
    char ch[20];
    k=LectureMin(fichier);
    VECTA<Noeud*> vect;
    VECTA<ResultatFonction*> chgts;    
	VECTA<ResultatFonction*> creees;    
    char testif=OUI;    
    char testa;
    char lecture_finale=NON;
    long offset=-1;
    long enregpos=-1;
    char utilisation_couches=NON;

    while (etat!=100) {            
        
        switch (etat) {
        case 0: {
            etatsuivant=0;
            tampon[0]=0;
            //On absorbe les blancs            
            LectureBlancs(fichier,k);
            
            if (fichier.eof()) {
                //si nous sommes dans le cas d'une etiquette de goto final
                if (etiquette!=NULL) {
                    //alors on insere notre vtf dans le bloc
                    etat=23;
                    vtf=new VectTestFonction;                    
                    vtf->script=script;
                    vtfinstruction=NULL;
                }
                else
                    etat=100;
                continue;
            }
            enregpos=-1;
            offset=fichier.tellg();
            lignecourante=LIGNES+1;
            offset-=strlen(enAttente);
            offset--;
            if (strlen(enAttente))
                offset--;
            switch (k) {
            case '<':
                etat=17;
                continue;
            case '#':
                //un label est forcement en-dehors d'une regle
                if (etats_instructions.dernier==0) {
                    etat=18;
                    continue;
                }
                etat=2;
                break;
            case '~':
                etat=1;                
                break;        
            case '}':
                if (etats_instructions.dernier==0)
                    erreur(CHAINES[304]);
                etat=etats_instructions.retire();                
                break;                
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                ich=0; //on lit le numero de couche
                etat=20;
                continue;
            case '|':                
                etat=1;                    
                break;
            default:
                etat=2;
            }
                        
            
            if (afficheautomate==OUI) {
                cout<<endl;
                if (etiquette==NULL || etiquette[0]=='#')
                    cout<<"Deduction:"<<VlabelVecteur.dernier<<endl;
                else
                    cout<<"Deduction:"<<etiquette<<endl;
            }

            if (vtf!=NULL)
                erreur(CHAINES[292]);

            if (LectureParametres==OUI) {
                controle_max_regle++;
                if (controle_max_regle>max_controle_regle)
                    erreur(CHAINES[190]);
            }

            //cas particulier s'il s'agit d'une regle normale sans couche
            //mais avec le "+" precise devant le nom de fichier
            if (BaseCoucheNumero!=0 && typedefaut==DEDUCTIONCOURANTE) {
                //Nous utilisons cette variable pour savoir s'il nous
                //faut nous positionner sur la couche suivante,
                //sachant qu'en cas de couche deja utilise, il faut se placer
                //un cran apres. Cette variable est aussi remise a zero si une couche
                //est detectee. Ce qui nous permet d'etre toujours apres.
                if (utilisation_couches==NON) {
                    MaxProfondeurAnalyse++;
                    BaseCoucheNumero=MaxProfondeurAnalyse;
                }

                if (laCouche>MaxCouches)
                    erreur(CHAINES[107]);

                laCouche=MaxProfondeurAnalyse;
                
                if (laCouche<1 || laCouche>MaxCouches)
                    erreur(CHAINES[107]);
                
                if (CouchesActives[laCouche]!=VIDE && 
                    CouchesActives[laCouche] != REGLES_DEDUCTION)
                    erreur(Concatene(CHAINES[208],tampon));
                                
				if (typedefaut!=DEDUCTIONFILTRE) {      
					CouchesActives[laCouche]=REGLES_DEDUCTION;
					type_deduction=DEDUCTIONCOUCHE;
				}
                utilisation_couches=OUI;
                changetypedefaut=NON;
            }
			else {
				//Dans ce cas, nous sommes en train d'enregistrer une regle de generation
				if (type_deduction==DEDUCTIONGENERATION) {
					laCouche=0;
					//NOUS NE TESTONS PAS LA COHERENCE DE LA COUCHE
				}
			}

            vtf=new VectTestFonction;            
            vtfinstruction=NULL;
            vtf->script=script;

            vtf->typeTest=INSTRUCTION_NORMALE;
            precedtype=type_deduction;
            if (changetypedefaut==OUI)
                type_deduction=typedefaut;

            if (lecture_finale==OUI)
                type_deduction=DEDUCTIONFINALE;
            disjonction=NON;
            nom_graphe=NULL;
            break;
                }
        case 1: //lecture d'un automate portant sur les chunks
            instructions=NULL;
            for (vi=0;vi<vari.taille;vi++)
                vari.vecteur[vi]=0;
            //ce type de regles portent sur l'arbre des granules
            vtf->numero=VlabelVecteur.dernier;  
            if (vtfinstruction==NULL) {
                vtfinstruction=vtf->derivation();
                delete vtf;
                vtf=vtfinstruction;
            }
            LitUnAutomate(fichier,XCatVide,vtfinstruction,k,REGLES_DEDUCTION,'|');
            regle=vtfinstruction->regle;
            vtfinstruction->regle_contexte=OUI;
            
            for (vi=0;vi<regle->arg.dernier;vi++)
                vari.affecte(regle->arg[vi],1);
            k=LectureMin(fichier);
            LectureBlancs(fichier,k);
            etat=30;
            break;
        case 30: //apres un |...|, on peut avoir les choses suivantes: if, iftest, compute ou {
            //On lit maintenant un des mots clefs suivants: if, where, compute, iftest.                
            if (k=='{') {
                if (script==NON)
                    erreur(CHAINES[319]);
                etat=11; 
                etatsuivant=23;                
                continue;      
            }

            if (k=='~') {
                tampon[0]=0;
                etat=10;
                etatsuivant=23;
                continue;
            }
            //sinon on lit la suite
            tampon[0]=tolower(k);            
            k=lit(fichier,tampon+1);
            switch(tampon[0]) {
            case 'i'://un if ou un iftest
                etat=4;                          
                continue;
            default: //sinon, c'est forcement une liste d'arguments
                etat=10;
                etatsuivant=23;                
            }
            break;
        case 38:// lecture d'un iftest
            boucle=NON;
            while (k!='}') {
                tarbre=new TestArbre;
                if (boucle==OUI)
                    MetEnAttenteLecture(k,tampon);
                K=' ';               
                LectureTest(fichier,K,tampon,NON,vari,&tarbre,';');k=(uchar)K;                
                nouveau=new ListeTestArbre(tarbre->arbre.fin()->test);
                StockeElementRegles(nouveau);
                if (vtfinstruction==NULL) {
                    vtfinstruction=vtf->derivation();
                    delete vtf;
                    vtf=vtfinstruction;
                }
                tarbre->AjouteEnQueue(&vtfinstruction->listes,vtfinstruction->listes.liste,nouveau);
                tarbre->arbre.fin()->test=NULL;
                delete tarbre;
                boucle=OUI;
            }                     
            k=LectureMin(fichier);
            etat=etatsuivant; //on sauvegarde ce test
            break;
        case 2:            
            //On lit maintenant un des mots clefs suivants: if, where, compute, iftest.      
            if (k=='{') {
                etat=11;
                continue;
            }

            //sinon on lit la suite
            tampon[0]=tolower(k);            
            k=Llit(fichier,tampon+1);
            
            if (!strcmp(tampon,"while")) {
                etat=22;
                etatsuivant=37;
                continue;
            }
            else {
                if (!strcmp(tampon,"whilexml")) {
                    etat=22;
                    etatsuivant=41;
                    continue;
                }
            }

            switch(tampon[0]) {
            case 'w':
                etat=15;
                etatsuivant=3;//un where
                continue;
            case 'i'://un if ou un iftest
                etat=15;
                etatsuivant=4;
                continue;
            case 'e':
                etat=12;
                continue;
            case 'd': //database
                etat=15;
                etatsuivant=24;
                continue;
            default:
                etat=15;
                etatsuivant=27; //lecture d'une instruction sans if ni compute ni rien
            }
            break;        
        case 15://dans tous les cas sauf celui du else, on remet la structure des
            //variables a zero, sauf si cette structure etait precede d'un contexte            
            if (vtfinstruction==NULL) {
                vtfinstruction=vtf->derivation();
                delete vtf;
                vtf=vtfinstruction;
            }

            if (vtfinstruction->regle_contexte==NON) {
				vari.raz();
                //for (vi=0;vi<vari.taille;vi++)
                //    vari.vecteur[vi]=0;
            }
            etat=etatsuivant;
            break;
        case 3: //on lit "where"            
            if (!strcmp(tampon,"where")) {
                vtf->typeTest=INSTRUCTION_DETERMINISTE;
                etat=6; //traitement du where
            }
            else 
                etat=27;
            break;
        case 4://on le lit le if ou le iftest            
            if (!strcmp(tampon,"if")) {
                if (script==NON) {
                    etatsuivant=7;
                    etat=22;
                }
                else {
                    //erreur(CHAINES[315]);
                    offset=enregpos;
                    etatsuivant=8;
                    etat=22;
                }
            }
            else {
                if (!strcmp(tampon,"iftest")) {
                    offset=enregpos;
                    if (script==OUI) {
                        etatsuivant=8;
                        etat=22;
                    }
                    else 
                        erreur(CHAINES[316]);
                }
                else //soit l'on est en train d'analyser un contexte en tete
                    if (vtf->regle_contexte==OUI) {
                        etat=10;
                        etatsuivant=23;
                    }
                    else {//sinon ce n'est pas le cas
                        offset=enregpos;
                        etat=27;
                    }
            }
            break;
        case 6: //c'est un where
        case 7: //c'est un if
        case 8: //c'est un iftest
        case 28: //c'est un compute ou un final
        case 37: //c'est un while
        case 41://traitement du whilexml
            //lecture du test... Il s'agit d'un test en debut de definition  
            if (vtfinstruction==NULL) {
                vtfinstruction=vtf->derivation();
                delete vtf;
                vtf=vtfinstruction;
            }
            
            tarbre=new TestArbre;
            K=' ';
            LectureTest(fichier,K,tampon,NON,vari,&tarbre);k=(uchar)K;
            testa=tarbre->LineariseDirect(this,vtfinstruction->clef,vtfinstruction->listes);
            delete tarbre;
            switch (etat) {
            case 6: //traitement du where
                etat=9; //lecture de la fin de l'analyse                        
                break;
            case 7: //traitement du if
                etatsuivant=23;
                testif=OUI;
                if (k=='<') //il s'agit d'une disjonction
                    etat=16; //traitement de la disjonction
                else
                    etat=10; //lecture d'un if normal
                break;
            case 8: //traitement du iftest
                //si l'on est en train d'analyser un contexte
                etat=11;
                etatsuivant=23;
                testif=NON;
                break;
            case 28: //traitement du compute ou du final
                etat=9;
                break;
            case 41://traitement du whilexml
                etat=42;
                if (k!='{')
                    erreur(CHAINES[35]);
                k=LectureMin(fichier);  
                break;
            case 37: //traitement du while
                etat=33;
                if (k!='{')
                    erreur(CHAINES[35]);
                k=LectureMin(fichier);  
                break;
            }
            break;
            case 9:
                if (k!='.')
                    erreur(CHAINES[12]);
                depend=new Dependance(XCatNeg);                   
                vtfinstruction->dependances.ajoute(depend);
                k=LectureMin(fichier);
                etat=23;
                break;
            case 16:                    
                //nom_graphe correspond au nom que la disjonction cree
                //de facon a pouvoir reperer l'origine d'une disjonction
                //ce nom_graphe est alors traduit en une dependance sans
                //argument qui aura comme signature, la fusion de toutes 
                //les signatures de ceux qui auront ete disjoint...
                if (tampon[0]!=0)
                    nom_graphe=ChercheFonc(tampon);
                disjonction=OUI;
                k=lit(fichier,tampon);
                etat=10;
                break;
            case 10: {//c'est un if, il doit etre suivi d'une liste de dependances
                boucle=OUI;
                if (script==OUI)
                    erreur(CHAINES[318]);
				sousdep=NULL;
                while (boucle) {                
                    depend=ConstruitDependance(fichier,vari,k,tampon,OUI,typedefaut);         
					vtfinstruction->dependances.ajoute(depend);                   
                    k=LectureMin(fichier);
                    LectureBlancs(fichier,k);
                    
                    switch(k) {
                    case ',':               
                        k=lit(fichier,tampon);
                        K=',';
                        break;
                    case '>':
                        if (disjonction==NON)
                            break;
					case '{'://uniquement dans le cas des regles de generation						
						if (typedefaut!=DEDUCTIONGENERATION || sousdep!=NULL)
							erreur(CHAINES[394]);
						sousdep=new SousDependance(depend);
						vtfinstruction->dependances.affecte(vtfinstruction->dependances.dernier-1,sousdep);
						litsousdependances(fichier,k,tampon,sousdep,vari);
                    case ';':
                    case '.':
                        k=LectureMin(fichier);
                        boucle=NON;
                        break;
                    default:
                        boucle=NON;
                    }
                }
                                
                if (nom_graphe!=NULL)
                    vtfinstruction->nomgraphe=&nom_graphe->X;
                vtfinstruction->incompatible=disjonction;
                vtfinstruction->typeTest=INSTRUCTION_DEPENDANCE;
                etat=etatsuivant;
                break;
                     }
            case 11:// lecture d'un iftest
                if (k!='{')
                    erreur(CHAINES[35]);
                //Il s'agit d'une structure avec ifend
                //On remet alors l'instruction dans le nourin
                etat=34;
                changetypedefaut=NON;
                k=LectureMin(fichier);  
                fif=-1;
                break;
            case 12: //lecture du else
                if (!strcmp(tampon,"else")) {
					testelse=OUI;
                    if (testif==OUI) {
                        etat=10;        //lecture des arguments
                        etatsuivant=13; //traitement du else
                        k=lit(fichier,tampon);
                    }
                    else {                        
                        if (fif==-1) {
                            etat=38; //il s'agit d'un else en contexte
                            etatsuivant=13;
                        }
                        else {
                            etat=35;
                            changetypedefaut=NON;
                            //cas particulier on remet le precedent type
                            type_deduction=precedtype;
                            //de plus on reinjecte le precedent indice
                            compteaccolade.ajoute(fif);
                            fif=-1;
                        }
                    }
                    continue;
                }
                //ce n'est rien de tout cela
                etat=15;
                etatsuivant=27;                                        
                break;
            case 13: //c'est un else
                precvtf->sinon=vtfinstruction;
                vtfinstruction->deterministe=precvtf->deterministe;
                vtfinstruction->zones=precvtf->zones;
                vtf->numero=precvtf->numero;
                precvtf=vtfinstruction;            
                vtf=NULL;                
                vtfinstruction=NULL;
                etat=0; //on repart de 0
                break;                    
            case 17://lecture de la zone
                k=lit(fichier,tampon);
                if (strcmp(tampon,CHAINES[47]))
                    erreur(CHAINES[89]);
                zoneLocale=0;
                while (k!='>') {
                    k=lit(fichier,tampon);
                    if (tampon[0]=='*')
                        zoneLocale=MoinsUn;
                    else {
                        int vz=atoi(tampon);
                        zoneLocale|= (Un << vz);
                    }
                }
                k=LectureMin(fichier);
                etat=0;
                break;
            case 18://lecture de l'etiquette
				etiquette=lireRulespaceLabel(fichier,tampon,k,OUI);
                //Dans le cas de la presence d'un label, il
                //faut generer absolument un nouveau jeu d'instruction
                instructions=NULL;
                k=LectureMin(fichier);
                etat=0;
                break;
            case 20: //lecture du numero de couche
                if (vtf!=NULL)
                    erreur(CHAINES[292]);

                if (k=='>') {
                    ch[ich]=0;
                    offset=fichier.tellg();
                    offset-=strlen(ch)+1;
                    couchelocale=atoi(ch);
                    laCouche=BaseCoucheNumero+couchelocale;
                    utilisation_couches=NON;
                    
                    if (laCouche<1 || laCouche>MaxCouches)
                        erreur(CHAINES[107]);
                    
					if (CouchesActives[laCouche]!=VIDE) {
						if (typedefaut!=DEDUCTIONGENERATION) {
							if (CouchesActives[laCouche] != REGLES_DEDUCTION)
								erreur(Concatene(CHAINES[208],tampon));
						}
						else {
							if (CouchesActives[laCouche] != REGLES_GENERATION)
								erreur(Concatene(CHAINES[208],tampon));
						}
					}
                    
                    if (MinProfondeurAnalyse > laCouche)
                        MinProfondeurAnalyse=laCouche;
                    
                    if (MaxProfondeurAnalyse < laCouche)
                        MaxProfondeurAnalyse=laCouche;
                    
					if (typedefaut!=DEDUCTIONFILTRE) {            
						if (typedefaut!=DEDUCTIONGENERATION) {
							CouchesActives[laCouche]=REGLES_DEDUCTION;
							type_deduction=DEDUCTIONCOUCHE;
						}
						else
							CouchesActives[laCouche]=REGLES_GENERATION;
					}
                    k=LectureMin(fichier);
                    LectureBlancs(fichier,k);
                    
                    //On cree alors un objet vtf
                    vtf=new VectTestFonction;
                    vtfinstruction=NULL;
                    vtf->typeTest=INSTRUCTION_NORMALE;                             
                    vtf->script=script;
                    //Nous sommes en lecture de deduction couche
					if (typedefaut!=DEDUCTIONGENERATION)
						typedefaut=DEDUCTIONCOUCHE;
                                        

                    if (k=='|')
                        etat=1;
                    else
                        etat=2;                    
                }
                else {
                    ch[ich++]=k;
                    k=LectureMin(fichier);
                }
                break;
            case 39: //accolade fermante du final
                delete vtf;
                vtf=NULL;
                vtfinstruction=NULL;
                etat=0;        
                lecture_finale=NON;
                k=LectureMin(fichier);
                break;
            case 22: //On lit la parenthese du test introduit par if
                instructions=NULL;
                LectureBlancs(fichier,k);
                afficheregle(VlabelVecteur.dernier,"Deduction Rule");
                if (k!='(')
                    erreur(CHAINES[30]);
                etat=etatsuivant;
                break;
            case 29: //boucle "for" accolade fermante
                //On verifie s'il y a une boucle avant    
                if (comptefor.dernier==0)
                    erreur(CHAINES[299]);
                instructions=NULL;
                fi=comptefor.retire();                
                switch(type_deduction) {
                case DEDUCTIONPROCEDURE:
                    ListeDesProcedures[fi]->finboucle=ListeDesProcedures.dernier;
                    break;
                case DEDUCTIONENCHARGEANT:            
                    ChgtDeductions[fi]->finboucle=ChgtDeductions.dernier;
                    break;
                case DEDUCTIONINITIALE:            
                    DebutDeductions[fi]->finboucle=DebutDeductions.dernier;
                    break;
                case DEDUCTIONPREBALISE:            
                    PreBaliseDeductions[fi]->finboucle=PreBaliseDeductions.dernier;
                    break;
                case DEDUCTIONPOSTBALISE:            
                    PostBaliseDeductions[fi]->finboucle=PostBaliseDeductions.dernier;
                    break;
                case DEDUCTIONQUEUE:            
                    QueueDeductions[fi]->finboucle=QueueDeductions.dernier;
                    break;
                case DEDUCTIONFINALE:            
                    FinDeductions[fi]->finboucle=FinDeductions.dernier;
                    break;
                case DEDUCTIONGENERATION:            
                    Generations[laCouche][fi]->finboucle=Generations[laCouche].dernier;
                    break;
                case DEDUCTIONCOURANTE:                    
                    ListeDesDeductions[laCoucheDeduction][fi]->finboucle=ListeDesDeductions[laCoucheDeduction].dernier;
                    break;
                case DEDUCTIONCOUCHE:                    
                    ListeDesDeductionsEnCouche[laCouche][fi]->finboucle=ListeDesDeductionsEnCouche[laCouche].dernier;                          
                    break;
                case DEDUCTIONFILTRE:
                    if (laCouche==-1)
                        erreur(CHAINES[76]);
                    if (TableDesFiltresDeCouche[laCouche].dernier)
                        erreur(CHAINES[71]);        
                    TableDesFiltresDeCouche[laCouche][fi]->finboucle=TableDesFiltresDeCouche[laCouche].dernier;
                }
                vtf->typeTest=INSTRUCTION_FINFOR;
                //On reboucle alors vers le test (c'est un for)
                vtf->finboucle=fi-1;     
                k=LectureMin(fichier);
                etat=23;
                break;
            case 31://debut boucle "for"
                if (script==NON)
                    erreur(CHAINES[317]);
                instructions=NULL;                
                switch(type_deduction) {
                case DEDUCTIONPROCEDURE:
                    comptefor.ajoute(ListeDesProcedures.dernier);
                    break;
                case DEDUCTIONENCHARGEANT:
                    comptefor.ajoute(ChgtDeductions.dernier);
                    break;
                case DEDUCTIONINITIALE:            
                    comptefor.ajoute(DebutDeductions.dernier);
                    break;
                case DEDUCTIONPREBALISE:            
                    comptefor.ajoute(PreBaliseDeductions.dernier);
                    break;
                case DEDUCTIONPOSTBALISE:            
                    comptefor.ajoute(PostBaliseDeductions.dernier);
                    break;
                case DEDUCTIONQUEUE:            
                    comptefor.ajoute(QueueDeductions.dernier);                    
                    break;
                case DEDUCTIONFINALE:            
                    comptefor.ajoute(FinDeductions.dernier);                    
                    break;
                case DEDUCTIONGENERATION:            
                    comptefor.ajoute(Generations[laCouche].dernier);                    
                    break;
                case DEDUCTIONCOURANTE:
                    comptefor.ajoute(ListeDesDeductions[laCoucheDeduction].dernier);
                    break;
                case DEDUCTIONCOUCHE:
                    comptefor.ajoute(ListeDesDeductionsEnCouche[laCouche].dernier);
                    break;
                case DEDUCTIONFILTRE:
                    if (laCouche==-1)
                        erreur(CHAINES[76]);
                    if (TableDesFiltresDeCouche[laCouche].dernier)
                        erreur(CHAINES[71]);        
                    comptefor.ajoute(TableDesFiltresDeCouche[laCouche].dernier);                    
                }
                //On insere alors dans la pile l'etat pour la fermeture de l'accolade
                etats_instructions.ajoute(29);
                vtf->typeTest=INSTRUCTION_FOR;
                etat=23;
                k=LectureMin(fichier);                
                break;
            case 43://Accolade fermante du whilexml
            case 32: //Accolade fermante du while
                //On verifie s'il y a une boucle avant    
                if (script==NON)
                    erreur(CHAINES[317]);
                if (comptewhile.dernier==0)
                    erreur(CHAINES[272]);
                instructions=NULL;
                fi=comptewhile.retire();                
                switch(type_deduction) {
                case DEDUCTIONPROCEDURE:            
                    ListeDesProcedures[fi]->finboucle=ListeDesProcedures.dernier;
                    break;
                case DEDUCTIONENCHARGEANT:
                    ChgtDeductions[fi]->finboucle=ChgtDeductions.dernier;
                    break;
                case DEDUCTIONINITIALE:            
                    DebutDeductions[fi]->finboucle=DebutDeductions.dernier;
                    break;
                case DEDUCTIONPREBALISE:            
                    PreBaliseDeductions[fi]->finboucle=PreBaliseDeductions.dernier;
                    break;
                case DEDUCTIONPOSTBALISE:            
                    PostBaliseDeductions[fi]->finboucle=PostBaliseDeductions.dernier;
                    break;
                case DEDUCTIONQUEUE:            
                    QueueDeductions[fi]->finboucle=QueueDeductions.dernier;
                    break;
                case DEDUCTIONFINALE:
                    FinDeductions[fi]->finboucle=FinDeductions.dernier;
                    break;
                case DEDUCTIONGENERATION:                    
                    Generations[laCouche][fi]->finboucle=Generations[laCouche].dernier;                          
                    break;
                case DEDUCTIONCOURANTE:                    
                    ListeDesDeductions[laCoucheDeduction][fi]->finboucle=ListeDesDeductions[laCoucheDeduction].dernier;
                    break;
                case DEDUCTIONCOUCHE:                    
                    ListeDesDeductionsEnCouche[laCouche][fi]->finboucle=ListeDesDeductionsEnCouche[laCouche].dernier;                          
                    break;
                case DEDUCTIONFILTRE:
                    if (laCouche==-1)
                        erreur(CHAINES[76]);
                    if (TableDesFiltresDeCouche[laCouche].dernier)
                        erreur(CHAINES[71]);        
                    TableDesFiltresDeCouche[laCouche][fi]->finboucle=TableDesFiltresDeCouche[laCouche].dernier;
                }  
                if (etat==43)
                    vtf->typeTest=INSTRUCTION_RETOUR;
                else {
                    vtf->typeTest=INSTRUCTION_FINWHILE;
                    //On reboucle alors vers le test (c'est un while)
                    vtf->finboucle=fi-1;
                }
                k=LectureMin(fichier);
                etat=23;                
                break;
            case 42://Traitement du whilexml
            case 33://Traitement du while
                instructions=NULL;                
                switch(type_deduction) {
                case DEDUCTIONPROCEDURE:            
                    comptewhile.ajoute(ListeDesProcedures.dernier);
                    break;
                case DEDUCTIONENCHARGEANT:
                    comptewhile.ajoute(ChgtDeductions.dernier);
                    break;
                case DEDUCTIONINITIALE:            
                    comptewhile.ajoute(DebutDeductions.dernier);
                    break;
                case DEDUCTIONPREBALISE:            
                    comptewhile.ajoute(PreBaliseDeductions.dernier);
                    break;
                case DEDUCTIONPOSTBALISE:            
                    comptewhile.ajoute(PostBaliseDeductions.dernier);
                    break;
                case DEDUCTIONQUEUE:            
                    comptewhile.ajoute(QueueDeductions.dernier);                    
                    break;
                case DEDUCTIONFINALE:            
                    comptewhile.ajoute(FinDeductions.dernier);                    
                    break;
                case DEDUCTIONGENERATION:            
                    comptewhile.ajoute(Generations[laCouche].dernier);                    
                    break;
                case DEDUCTIONCOURANTE:
                    comptewhile.ajoute(ListeDesDeductions[laCoucheDeduction].dernier);
                    break;
                case DEDUCTIONCOUCHE:
                    comptewhile.ajoute(ListeDesDeductionsEnCouche[laCouche].dernier);
                    break;
                case DEDUCTIONFILTRE:
                    if (laCouche==-1)
                        erreur(CHAINES[76]);
                    if (TableDesFiltresDeCouche[laCouche].dernier)
                        erreur(CHAINES[71]);        
                    comptewhile.ajoute(TableDesFiltresDeCouche[laCouche].dernier);                    
                }                
                if (etat==33) {
                    vtf->typeTest=INSTRUCTION_WHILE;
                    etats_instructions.ajoute(32);
                    if (etats_instructions.dernier==0)
                        vtf->premiertest=OUI;
                }
                else {
                    vtf->typeTest=INSTRUCTION_WHILEXML;
                    etats_instructions.ajoute(43);
                    vtf->premiertest=OUI;
                }
                etat=23;
                break;
            case 34: //il s'agit d'un iftest (avec un endif a la clef)
                instructions=NULL;                
                switch(type_deduction) {
                case DEDUCTIONPROCEDURE:
                    compteaccolade.ajoute(ListeDesProcedures.dernier);
                    break;
                case DEDUCTIONENCHARGEANT:
                    compteaccolade.ajoute(ChgtDeductions.dernier);                    
                    break;
                case DEDUCTIONINITIALE:
                    compteaccolade.ajoute(DebutDeductions.dernier);                    
                    break;
                case DEDUCTIONPREBALISE:            
                    compteaccolade.ajoute(PreBaliseDeductions.dernier);
                    break;
                case DEDUCTIONPOSTBALISE:            
                    compteaccolade.ajoute(PostBaliseDeductions.dernier);
                    break;
                case DEDUCTIONQUEUE:            
                    compteaccolade.ajoute(QueueDeductions.dernier);                    
                    break;
                case DEDUCTIONFINALE:            
                    compteaccolade.ajoute(FinDeductions.dernier);                    
                    break;
                case DEDUCTIONGENERATION:            
                    compteaccolade.ajoute(Generations[laCouche].dernier);                    
                    break;
                case DEDUCTIONCOURANTE:
                    compteaccolade.ajoute(ListeDesDeductions[laCoucheDeduction].dernier);
                    break;
                case DEDUCTIONCOUCHE:
                    compteaccolade.ajoute(ListeDesDeductionsEnCouche[laCouche].dernier);
                    break;
                case DEDUCTIONFILTRE:
                    if (laCouche==-1)
                        erreur(CHAINES[76]);
                    if (TableDesFiltresDeCouche[laCouche].dernier)
                        erreur(CHAINES[71]);        
                    compteaccolade.ajoute(TableDesFiltresDeCouche[laCouche].dernier);                    
                }

                //Par defaut le typeTest vaut INSTRUCTION_IFTEST
                //Si nous sommes dans une procedure, alors s'il s'agit
                //seulement de la premiere '{' de la procedure, on change le type
                //du test...
  
                if (vtf->typeTest==INSTRUCTION_NORMALE)
                    vtf->typeTest=INSTRUCTION_IFTEST;

                if (etats_instructions.dernier==0) {
                    if (type_deduction==DEDUCTIONPROCEDURE) {
                        //il s'agit dans ce cas d'une entree de procedure
                        vtf->typeTest=INSTRUCTION_PROCEDURE;
                        if (vtfinstruction==NULL) {
                            vtfinstruction=vtf->derivation();
                            delete vtf;
                            vtf=vtfinstruction;
                        }                
                    }
                }
                
                if (vtf->regle_contexte==NON && vtf->typeTest==INSTRUCTION_IFTEST)
                    vtf->premiertest=OUI;
                        

                //s'il s'agit de l'accolade ouvrante d'un |...|
                if (vtf->regle_contexte==OUI)
                    vtf->typeTest=INSTRUCTION_CONTEXTE;

                //Dans les cas suivants nous avons besoin d'une accolade fermante pour retourner
                //d'un appel recursif...
                if (vtf->typeTest==INSTRUCTION_CONTEXTE ||
                    vtf->premiertest==OUI) 
                    etats_instructions.ajoute(40);//On rajoute alors une accolade physique sous la forme d'un pointeur
                else
                    etats_instructions.ajoute(36);

                etat=23;
                break;
            case 35://Il s'agit d'un else (on place alors un noeud fictif dans le lot)
                //On verifie s'il y a un iftest avant    
                if (compteaccolade.dernier==0)
                    erreur(CHAINES[301]);
                instructions=NULL;
                fif=compteaccolade.retire();                
                switch(type_deduction) {
                case DEDUCTIONPROCEDURE:
                    ifvtf=ListeDesProcedures[fif];
                    ListeDesProcedures[fif]->finboucle=ListeDesProcedures.dernier;
                    break;
                case DEDUCTIONENCHARGEANT:
                    ifvtf=ChgtDeductions[fif];
                    ChgtDeductions[fif]->finboucle=ChgtDeductions.dernier;
                    break;
                case DEDUCTIONINITIALE:
                    ifvtf=DebutDeductions[fif];
                    DebutDeductions[fif]->finboucle=DebutDeductions.dernier;
                    break;
                case DEDUCTIONPREBALISE:            
                    ifvtf=PreBaliseDeductions[fif];
                    PreBaliseDeductions[fif]->finboucle=PreBaliseDeductions.dernier;
					break;
                case DEDUCTIONPOSTBALISE:            
                    ifvtf=PostBaliseDeductions[fif];
                    PostBaliseDeductions[fif]->finboucle=PostBaliseDeductions.dernier;
					break;
                case DEDUCTIONQUEUE:            
                    ifvtf=QueueDeductions[fif];
                    QueueDeductions[fif]->finboucle=QueueDeductions.dernier;
                    break;
                case DEDUCTIONFINALE:
                    ifvtf=FinDeductions[fif];
                    FinDeductions[fif]->finboucle=FinDeductions.dernier;
                    break;
                case DEDUCTIONGENERATION:
                    ifvtf=Generations[laCouche][fif];
                    Generations[laCouche][fif]->finboucle=Generations[laCouche].dernier;
                    break;
                case DEDUCTIONCOURANTE:
                    ifvtf=ListeDesDeductions[laCoucheDeduction][fif];
                    ListeDesDeductions[laCoucheDeduction][fif]->finboucle=ListeDesDeductions[laCoucheDeduction].dernier;
                    break;
                case DEDUCTIONCOUCHE:
                    ifvtf=ListeDesDeductionsEnCouche[laCouche][fif];
                    ListeDesDeductionsEnCouche[laCouche][fif]->finboucle=ListeDesDeductionsEnCouche[laCouche].dernier;
                    break;
                case DEDUCTIONFILTRE:
                    if (laCouche==-1)
                        erreur(CHAINES[76]);
                    if (TableDesFiltresDeCouche[laCouche].dernier)
                        erreur(CHAINES[71]);        
                    TableDesFiltresDeCouche[laCouche][fif]->finboucle=TableDesFiltresDeCouche[laCouche].dernier;
                }
                //dans tous les cas de figure, si l'on passe en sequence sur un "else", il faut evidemment
                //le sauter
                if (ifvtf->typeTest==INSTRUCTION_CONTEXTE)
                    vtf->typeTest=INSTRUCTION_CONTEXTE;
                else
                    vtf->typeTest=INSTRUCTION_ELSE;
                
                etat=11;
                fif=-1;
                break;
            case 36: //il s'agit de l'accolade fermante du iftest
                retireaccolade=0;
            case 40:
                //On verifie s'il y a un iftest avant    
                changetypedefaut=OUI;
                if (compteaccolade.dernier==0)
                    erreur(CHAINES[301]);
                instructions=NULL;
                fif=compteaccolade.retire();
                switch(type_deduction) {
                case DEDUCTIONPROCEDURE:
                    ListeDesProcedures[fif]->finboucle=ListeDesProcedures.dernier-retireaccolade;
                    break;
                case DEDUCTIONENCHARGEANT:
                    ChgtDeductions[fif]->finboucle=ChgtDeductions.dernier-retireaccolade;
                    break;
                case DEDUCTIONINITIALE:
                    DebutDeductions[fif]->finboucle=DebutDeductions.dernier-retireaccolade;
                    break;
                case DEDUCTIONPREBALISE:            
                    PreBaliseDeductions[fif]->finboucle=PreBaliseDeductions.dernier-retireaccolade;
                    break;
                case DEDUCTIONPOSTBALISE:            
                    PostBaliseDeductions[fif]->finboucle=PostBaliseDeductions.dernier-retireaccolade;
                    break;
                case DEDUCTIONQUEUE:            
                    QueueDeductions[fif]->finboucle=QueueDeductions.dernier-retireaccolade;
                    break;
                case DEDUCTIONFINALE:
                    FinDeductions[fif]->finboucle=FinDeductions.dernier-retireaccolade;
                    break;
                case DEDUCTIONGENERATION:
                    Generations[laCouche][fif]->finboucle=Generations[laCouche].dernier-retireaccolade;
                    break;
                case DEDUCTIONCOURANTE:
                    ListeDesDeductions[laCoucheDeduction][fif]->finboucle=ListeDesDeductions[laCoucheDeduction].dernier-retireaccolade;
                    break;
                case DEDUCTIONCOUCHE:
                    ListeDesDeductionsEnCouche[laCouche][fif]->finboucle=ListeDesDeductionsEnCouche[laCouche].dernier-retireaccolade;
                    break;
                case DEDUCTIONFILTRE:
                    if (laCouche==-1)
                        erreur(CHAINES[76]);
                    if (TableDesFiltresDeCouche[laCouche].dernier)
                        erreur(CHAINES[71]);        
                    TableDesFiltresDeCouche[laCouche][fif]->finboucle=TableDesFiltresDeCouche[laCouche].dernier-retireaccolade;
                }
                if (type_deduction==DEDUCTIONPROCEDURE) {
                    if (etats_instructions.dernier==0) {
                        etat=100;//On ne lit qu'une seule procedure a la fois
                        delete vtf;
                        vtf=NULL;
                        vtfinstruction=NULL;
                    }
                    else {
                        vtf->typeTest=INSTRUCTION_RETOUR;
                        etat=23;
                        k=LectureMin(fichier); 
                    }
                }
                else {
                    vtf->typeTest=INSTRUCTION_RETOUR;                                            
                    etat=23;
                    k=LectureMin(fichier); 
                }
                retireaccolade=0;
				testelse=NON;
                break;
            case 23: //Etat final, on rajoute le vtf courant
                if (vtfinstruction!=NULL) {
                    vtfinstruction->deterministe=deterministe;
                    vtfinstruction->zones=zoneLocale;
                    //cas particulier d'une simplification d'une instruction
                    vtfune=vtfinstruction->simplification();
                    if (vtfune!=NULL) {
                        vtf=(VectTestFonction*)vtfune;
                        vtfinstruction=NULL;
                        instructions=NULL;
                    }
                }
                
                vtf->numero=VlabelVecteur.dernier;
				if (type_deduction==DEDUCTIONGENERATION) {
					la=AjouteLabel(VlabelVecteur,etiquette,REGLES_GENERATION,laCouche,nbInference,
						new DecompteRegle(REGLES_GENERATION,NOM,offset,lignecourante,laCouche,couchelocale));
					AjouteStats(new stats(vtf,REGLES_GENERATION,laCouche));
				}
				else {
					la=AjouteLabel(VlabelVecteur,etiquette,REGLES_DEDUCTION,laCouche,nbInference,
						new DecompteRegle(REGLES_DEDUCTION,NOM,offset,lignecourante,laCouche,couchelocale));
					AjouteStats(new stats(vtf,REGLES_DEDUCTION,laCouche));
				}
				AjouteDansBase(VBases,vtf,laCouche,la);
				etiquette=NULL;
                
                
                switch(type_deduction) {
                case DEDUCTIONPROCEDURE:
                    vtf->position=ListeDesProcedures.dernier;
					la->indexregle=vtf->position;
                    if (vtfinstruction==NULL)
                        vtfinstruction=(VectTFInstruction*)vtf;                 
                    ListeDesProcedures.ajoute(vtfinstruction);                    
                    afficheregle(VlabelVecteur.dernier,"PROCEDURE");
                    break;
                case DEDUCTIONENCHARGEANT:
                    vtf->position=DebutDeductions.dernier;
					la->indexregle=vtf->position;
                    ChgtDeductions.ajoute(vtf);                    
                    afficheregle(VlabelVecteur.dernier,"LOADING");
                    break;
                case DEDUCTIONINITIALE:
                    vtf->position=DebutDeductions.dernier;
					la->indexregle=vtf->position;
                    DebutDeductions.ajoute(vtf);                    
                    afficheregle(VlabelVecteur.dernier,"INITIAL");
                    break;
                case DEDUCTIONPREBALISE:            
                    vtf->position=PreBaliseDeductions.dernier;
					la->indexregle=vtf->position;
                    PreBaliseDeductions.ajoute(vtf);                    
                    afficheregle(VlabelVecteur.dernier,"OPENMARKUP");
                    break;
                case DEDUCTIONPOSTBALISE:            
                    vtf->position=PostBaliseDeductions.dernier;
					la->indexregle=vtf->position;
                    PostBaliseDeductions.ajoute(vtf);                    
                    afficheregle(VlabelVecteur.dernier,"CLOSEMARKUP");
                    break;
				case DEDUCTIONQUEUE:
                    vtf->position=QueueDeductions.dernier;
					la->indexregle=vtf->position;
                    QueueDeductions.ajoute(vtf);                    
                    afficheregle(VlabelVecteur.dernier,"FINAL");
                    break;
                case DEDUCTIONFINALE:
                    vtf->position=FinDeductions.dernier;
					la->indexregle=vtf->position;
                    FinDeductions.ajoute(vtf);                    
                    afficheregle(VlabelVecteur.dernier,"FINAL");
                    break;
                case DEDUCTIONGENERATION:
                    la->indexregle=Generations[laCouche].dernier;
                    vtf->position=Generations[laCouche].dernier;
                    Generations[laCouche].ajoute(vtf);      
                    afficheregle(VlabelVecteur.dernier,"DEDUCTIONGENERATION");                
                    break;
                case DEDUCTIONCOURANTE:
                    la->indexregle=ListeDesDeductions[laCoucheDeduction].dernier;
                    vtf->position=ListeDesDeductions[laCoucheDeduction].dernier;
                    ListeDesDeductions[laCoucheDeduction].ajoute(vtf);            
                    afficheregle(VlabelVecteur.dernier,"DEDUCTIONCOURANTE");                
                    break;
                case DEDUCTIONCOUCHE:
                    la->indexregle=ListeDesDeductionsEnCouche[laCouche].dernier;
                    vtf->position=ListeDesDeductionsEnCouche[laCouche].dernier;
                    ListeDesDeductionsEnCouche[laCouche].ajoute(vtf);      
                    afficheregle(VlabelVecteur.dernier,"DEDUCTIONCOUCHE");                
                    break;
                case DEDUCTIONFILTRE:
                    if (laCouche==-1)
                        erreur(CHAINES[76]);
                    if (TableDesFiltresDeCouche[laCouche].dernier)
                        erreur(CHAINES[71]);        
                    vtf->position=TableDesFiltresDeCouche[laCouche].dernier;
					la->indexregle=vtf->position;
                    TableDesFiltresDeCouche[laCouche].ajoute(vtf);
                    afficheregle(VlabelVecteur.dernier,"DEDUCTIONFILTRE");
                }
                precvtf=vtfinstruction;
                vtf=NULL;                
                vtfinstruction=NULL;
                etat=0;
                break;
                case 24: //on lit "database"                    
                    if (!strcmp(tampon,"database"))
                        etat=25;                    
                    else 
                        etat=27;                                       
                    break;
                case 25:
                    //lecture du database, on verifie le signe egal
                    if (k!='=')
                        erreur(CHAINES[34]);
                    k=lit(fichier,tampon);
                    etat=10;
                    etatsuivant=26;
                    break;
                case 26: //traitement de la database
                    vect.raz();
                    chgts.raz();
					creees.raz();
                    for (idep=0;idep<vtfinstruction->dependances.dernier;idep++)
                        MajDependance(vect,chgts,creees,vtfinstruction,vtfinstruction->dependances[idep],NULL,idep,NULL);    
                    for (fi=0;fi<FONCTIONSINTERMEDIAIRES.dernier;fi++)
                        FONCTIONSINTERMEDIAIRES[fi]->data=OUI;
                    MajINTERFONCTIONS();
                    delete vtf;     
                    vtf=NULL;
                    vtfinstruction=NULL;
                    etat=0;
                    break;
                case 27: //traitement d'une instruction normale  
                    //Tout cela est un peu complique
                    //Tout d'abord on verifie qu'on n'est pas en train
                    //de lire une autre sorte de regle
                    if (k==':') {
                        if (comptefor.dernier!=0)
                            erreur(CHAINES[300]);
                        if (comptewhile.dernier!=0)
                            erreur(CHAINES[271]);
                        if (compteaccolade.dernier!=0)
                            erreur(CHAINES[302]);
                        if (etats_instructions.dernier)
                            erreur(CHAINES[304]);

                        if (ChoixLectureRegle(fichier,tampon)==NON) {                            							
                            delete vtf;
                            return;
                        }
						else
							erreur(Concatene(CHAINES[94], tampon));
                    }                    
                    //On remet alors les caracteres lu precedemment dans le flot de caracteres
                    //de facon a relancer l'analyse depuis le debut
                    MetEnAttenteLecture(k,tampon);
                    tarbre=new TestArbre;
                    //Quelques remises a jour
                    K=' ';tampon[0]=0;
                    //on relit alors tout et on reanalyse la structure
                    LectureTest(fichier,K,tampon,NON,vari,&tarbre,';');k=(uchar)K;
                    tfm=NULL;
                    if (tarbre->arbre[0] &&
                        tarbre->arbre[0]->test!=NULL &&
                        tarbre->arbre[0]->test->Type()==TESTFONCTIONMATH) {
                        tfm=(TestFonctionMath*)tarbre->arbre[0]->test; 
                        //On teste si c'est un return
                        if (tfm->formule==NULL ||
                            tfm->formule->element==NULL ||
                            tfm->formule->element->type!=VOPERATEUR || 
                            tfm->formule->element->valeur_operation()!=43)
                            tfm=NULL;
                    }

                    if (tfm!=NULL) {
                        instructions=vtfinstruction;
                        etatsuivant=23; 
                    }
                    else {
                        //s'il s'agit d'une boucle, on la conserve toute seule
                        if (tarbre->arbre[0] &&
                            tarbre->arbre[0]->test!=NULL &&
                            tarbre->arbre[0]->test->Type()==TESTFONCTIONBOUCLE) {
                            if (k!='{')
                                erreur(CHAINES[35]);
                            instructions=vtfinstruction;
                            tfmb=(TestFonctionMathBoucle*)tarbre->arbre[0]->test;
                            fi=tfmb->formule->element->valeur_operation();
                            etatsuivant=31; //for
                        }
                        else {
                            //Dans le cas d'une serie d'instruction, on les sauvegarde dans un
                            //seul objet vtf. instructions garde la trace de ce vtf initial
                            if (instructions==NULL) {                       
                                instructions=vtfinstruction;    //il ne faut pas oublier d'enregistrer ce vtf
                                etatsuivant=23;
                            }
                            else {
                                //Un vtf a donc ete cree de trop, on le detruit
                                delete vtf;
                                vtf=NULL;  
                                vtfinstruction=NULL;
                                etatsuivant=0;
                            }
                        }
                    }
                    //enfin on fusionne le "test" avec le vtf courant
                    nouveau=new ListeTestArbre(tarbre->arbre.fin()->test);
                    StockeElementRegles(nouveau);
                    tarbre->AjouteEnQueue(&instructions->listes,instructions->listes.liste,nouveau);    
                    tarbre->arbre.fin()->test=NULL;
                    delete tarbre;
                    
                    //Il peut y avoir quelques caracteres de residu qu'on replace dans
                    //le flot courant
                    MetEnAttenteLecture(k,tampon);
                    tampon[0]=0;
                    k=LectureMin(fichier);
                    //c'est soit un enregistrement (etat=23)
                    //soit une instruction suivante (etat=0) et on redemarre depuis le debut alors
                    etat=etatsuivant;                    
                    break;
                }
           }

           if (vtf!=NULL)
               erreur(CHAINES[292]);
           if (comptefor.dernier!=0)
               erreur(CHAINES[300]);
           if (comptewhile.dernier!=0)
               erreur(CHAINES[271]);
           if (compteaccolade.dernier!=0)
               erreur(CHAINES[302]);
           if (etats_instructions.dernier)
               erreur(CHAINES[304]);

}


/*-------------------------------------------------------------------------*/


char GlobalParseur::AffichageFonction(istream& fichier,char cacher) {
    Categorie* c;

    char k,tampon[300],fin=0;
    k = lit(fichier,tampon);
    //On reinitialise toutes les fonctions avec la valeur inverse
    for (int i=0;i<Fonctions.dernier;i++)
        Fonctions[i]->X.cacher=OUI-cacher;

    char boucle=OUI;

    while (boucle) {

        if (AppelCons(fichier,k,tampon)==OUI)
            return OUI;
        
        if (tampon[0]==0 || k==':')
            return NON;
        
        if (k!='.' && k!=',')
            erreur(CHAINES[132]);
        
        c=ChercheFonc(tampon);
        
        c->X.cacher=cacher;
        k=lit(fichier,tampon);
    }
    return NON;
}

/*-------------------------------------------------------------------------*/
//lecture des arguments...
int GlobalParseur::lirearguments(istream& fichier,char& k, char* tampon,
								  VECTA<int>& args,VECTA<Traits*>& argtr) {
	Traits* tr;
	int i=0;
	while (k != ')') {
		k=lit(fichier,tampon);
		//Une variable
		if (k!='#')
			erreur(CHAINES[245]);
		k=lit(fichier,tampon);
		int num_arg=atoi(tampon);
		args.ajoute(num_arg);
		if (k=='[') {
			tr=BatitTraits(fichier);
			tr=FactoriseTraits(tr);
			argtr.affecte(i,tr);
			k=lit(fichier,tampon);
		}
		if (k!=',' && k!=')')
			erreur(CHAINES[133]);
		i++;
	}
	k=lit(fichier,tampon);
	return i;
}
/*-------------------------------------------------------------------------*/
//Il s'agit de lire des relations precedances entre des NODES sous la forme: np[] < nd[].
char GlobalParseur::ConsOrdreNodes(istream& fichier) {
    Categorie* c1;
	Categorie* c2;
	Traits* tr1;
	Traits* tr2;
	int nb1;
	int nb2;
	VECTA<int> args2;
	VECTA<Traits*> argtr2;
	VECTA<int> args1;
	VECTA<Traits*> argtr1;
	long offset;
	int laCouche,couchelocale;

    char k,tampon[300];
    k = lit(fichier,tampon);
    
    while (!fichier.eof()) {
        
		if (k==':') {
			if (ChoixLectureRegle(fichier,tampon)==NON)
				return NON;
			else
				erreur(Concatene(CHAINES[94],tampon));
		}

        if (tampon[0]==0 || k==':')
            return NON;

		offset=fichier.tellg();
		laCouche=0;
		//L'indice de couche est: chiffre>
		if (k=='>') {			
			offset-=strlen(tampon)+1;
			couchelocale=atoi(tampon);
			laCouche=BaseCoucheNumero+couchelocale;

			if (laCouche<1 || laCouche>MaxCouches)
				erreur(CHAINES[107]);

			if (CouchesActives[laCouche]!=REGLES_GENERATION && CouchesActives[laCouche]!=VIDE)
				erreur(CHAINES[208]);

			k=lit(fichier,tampon);
		}

		tr1=NULL;
		tr2=NULL;
		nb1=0;
		nb2=0;
		c1=CatVide;
		c2=CatVide;
		args1.raz();
		argtr1.raz();
		args2.raz();
		argtr2.raz();
		if (tampon[0]!=0 && tampon[0]!='?') {
			c1=ChercheFonc(tampon);		
			if (c1->X.type!=C_GENERATION)
				erreur(CHAINES[396]);
		}

		if (k=='[') {
			tr1=BatitTraits(fichier);
			tr1=FactoriseTraits(tr1);
			k=lit(fichier,tampon);
		}			

		if (k=='(')
			nb1=lirearguments(fichier,k,tampon,args1,argtr1);
		if (k!='<')
			erreur(CHAINES[117]);

		k=lit(fichier,tampon);
		if (tampon[0]!=0 && tampon[0]!='?') {
			c2=ChercheFonc(tampon);
			if (c2->X.type!=C_GENERATION)
				erreur(CHAINES[396]);
		}
		if (k=='[') {
			tr2=BatitTraits(fichier);
			tr2=FactoriseTraits(tr2);
			k=lit(fichier,tampon);
		}			
		if (k=='(')
			nb2=lirearguments(fichier,k,tampon,args2,argtr2);
		if (k!='.')
			erreur(CHAINES[401]);
		OrdreNoeuds* ordnoeud=new OrdreNoeuds(&c1->X,&c2->X,tr1,tr2);
		ordnoeud->argsgauche.affecte(args1);
		ordnoeud->argtraitsgauche.affecte(argtr1);
		ordnoeud->argsdroit.affecte(args2);
		ordnoeud->argtraitsdroit.affecte(argtr2);
		ListeOrdreNoeuds[laCouche].ajoute(ordnoeud);
		k=lit(fichier,tampon);
    }

    return NON;
}

/*-------------------------------------------------------------------------*/
char GlobalParseur::ConsFonction(istream& fichier,char nodes) {
    Categorie* c;

    char k,tampon[300],fin=0;
    k = lit(fichier,tampon);
    
    while (!fichier.eof()) {
        Traits* tr=NULL;
        //Si il y a des traits declares
        if (k=='=') {
            char suite[100];
            k=lit(fichier,suite);
            if (k!='[')
                erreur(CHAINES[111]);
            tr=BatitTraits(fichier);
            k=lit(fichier,suite);
        }
            
        if (AppelCons(fichier,k,tampon)==OUI)
            return OUI;

        if (tampon[0]==0 || k==':')
            return NON;

        c=CreeFonc(tampon,NbFonctionsExtraction,FactoriseTraits(tr),NOM);
		if (nodes==OUI)
			c->X.type=C_GENERATION;
        //s'il s'agit d'une nouvelle dependance
        if (c->X.index==Fonctions.dernier-1)
            NbFonctionsExtraction++;
        else {
            if (AFFICHE_ERREUR==OUI)
                *flot_erreur<<CHAINES[322]<<tampon<<endl;
        }
        k=lit(fichier,tampon);
    }

    return NON;
}

/*-------------------------------------------------------------------------*/

void GlobalParseur::ConsVregleParametres(char type,char* tampon,istream& fichier) {
    char k;
    Traits* trgauche;
    Traits* trdroit;


    k=lit(fichier,tampon);
 
    while (k!=':') {

        if (k=='>') 
            trgauche=NULL;
        else {
            if (k!='[') 
                erreur(CHAINES[116]);
            trgauche=BatitTraits(fichier);
    
            k=lit(fichier,tampon);
            if (k!= '>') erreur(CHAINES[117]);
        }

        k=lit(fichier,tampon);
        if (k!='[') 
            erreur(CHAINES[111]);
        trdroit=BatitTraits(fichier);
    
        Vregle[type].ajoute(new RegleTraits(trgauche,trdroit,NULL));
        k=lit(fichier,tampon);
    }
}

void GlobalParseur::LireParametres(char* nom) {
    char tampon[300];
    NOM=nom;

    LIGNES=0; 
    POSITION_DANS_FICHIER=0;
    LIGNESERREUR=0;
    nom_fichier_courant=nom;
    //Dans le cas d'une mention "limited" dans le fichier de langue, on interdit l'utilisation
    //de fichiers de parametre supplementaires.
    //Dans le cas d'une mention "limited" dans le fichier de langue, on interdit l'utilisation
    //de fichiers de parametre supplementaires.
    if (UsageLimitee==USAGE_INTERDIT)
        erreur(CHAINES[263]);            

    if (UsageLimitee==USAGE_LIMITE)
        LectureParametres=OUI;

    BaseCoucheNumero=ValeurCoucheInitiale; //sinon, c'est un adressage absolu

    if (nom[0]=='+') {
        BaseCoucheNumero=MaxProfondeurAnalyse; //adressage de couche relatif
		int l=strlen(nom);
		for (int i=0;i<l;i++)
			nom[i]=nom[i+1];
    }

    ifstream fichier(nom,modeOuverture);
    
    if (fichier.fail() ) {
        strcpy(tampon,CHAINES[97]);
        strcat(tampon,NOM);
        erreur(tampon);
    }
    *flot_erreur<<"Loading parameter file: "<<nom;
    encoursCryptage=cryptage;
    cryptage=NON;
	ChargeParametres(fichier,nom,NON);
	cryptage=encoursCryptage;
}

void GlobalParseur::LireParametresEnChaine(string& texte,char ajoute) {
    NOM="STRING";

    LIGNES=0; 
    POSITION_DANS_FICHIER=0;
    LIGNESERREUR=0;
    nom_fichier_courant=NOM;
    //Dans le cas d'une mention "limited" dans le fichier de langue, on interdit l'utilisation
    //de fichiers de parametre supplementaires.
    //Dans le cas d'une mention "limited" dans le fichier de langue, on interdit l'utilisation
    //de fichiers de parametre supplementaires.
    if (UsageLimitee==USAGE_INTERDIT)
        erreur(CHAINES[263]);            

    if (UsageLimitee==USAGE_LIMITE)
        LectureParametres=OUI;

    BaseCoucheNumero=ValeurCoucheInitiale; //sinon, c'est un adressage absolu

    if (ajoute==OUI)
        BaseCoucheNumero=MaxProfondeurAnalyse; //adressage de couche relatif

    istringstream fichier(texte);
    encoursCryptage=cryptage;
    cryptage=NON;
	ChargeParametres(fichier,NOM,OUI);
	cryptage=encoursCryptage;
	MAJNumeroRegles();
}

void GlobalParseur::ChargeParametres(istream& fichier,char* nom,char type_chaine) {
    char k;
    char tampon[300];

	char metendl=OUI;
	if (type_chaine==OUI)
		metendl=NON;
	//On lit les trois premiers caracteres, potentiellement une amorce utf8
    k=lit(fichier,tampon);

#ifdef XIPPYTHON
    importation_python=NON;
    if (k=='"' && tampon[0]==0) {
        //nous sommes dans un espace Python
        k=lit(fichier,tampon);
        if (strcmp(tampon,"python"))
            erreur(CHAINES[217]);
        importation_python=OUI;
        metendl=NON;
    }
    else {
        if (!strcmp(tampon,"python"))
            metendl=NON;
    }
#endif

    if (metendl==OUI)
        *flot_erreur<<Endl;

    //Dans le cas d'un fichier de vocabulaire, il ne peut y avoir rien d'autre...
    if (!strcmp(tampon,CHAINES[110]) && k==':') {
        while(ConsVoc(fichier,TOUS));
        return;
    }
 
   //Dans le cas d'un fichier de vocabulaire, il ne peut y avoir rien d'autre...
    if (!strcmp(tampon,CHAINES[0]) && k==':') {
		ifstream* fluxcourant=NULL;
		if (type_chaine==NON)
			fluxcourant=(ifstream*)&fichier;

		if (BaseCoucheNumero==ValeurCoucheInitiale) {
			if (CouchesDeduction[laCoucheDeduction]	!= VIDE)
				laCoucheDeduction++;			   

			CouchesDeduction[laCoucheDeduction]=REGLES_SEMANTIQUE;
			laCoucheSemantique=laCoucheDeduction;
            couchesemantique=OUI;
		}
		else {//il s'agit alors d'un ajout dans les couches standards
			int laCouche=BaseCoucheNumero+1;
			if (CouchesActives[laCouche]!=VIDE && 
				CouchesActives[laCouche] != REGLES_SEMANTIQUE)
				erreur(Concatene(CHAINES[208],tampon));

			if (MinProfondeurAnalyse > laCouche)
				MinProfondeurAnalyse=laCouche;

			if (MaxProfondeurAnalyse < laCouche)
				MaxProfondeurAnalyse=laCouche;

			CouchesActives[laCouche]=REGLES_SEMANTIQUE;
			laCoucheSemantique=laCouche;
            couchesemantique=NON;
		}

		indexeur ind;
		if (fluxcourant!=NULL)
			fluxcourant->close();
        ind.indexer(this,nom);        
                        
        //while(ConsSem(fichier));
        return;
    }
  
  	if (!strcmp(tampon,CHAINES[381]) && k==':') {
		for (int i=0;i<ExpressionsFinDePhrase.dernier;i++) {
			if (ExpressionsFinDePhrase[i]!=NULL) 
				ExpressionsFinDePhrase[i]->nettoie();
		}

		ExpressionsFinDePhrase.nettoie();
        LitUneLigne(fichier,tampon,OUI);
		UneChaine(this,tampon);
        while (tampon[0]!=0) {
			automate* a=new automate((uchar*)tampon,(void*)(ExpressionsFinDePhrase.dernier+1));
			ExpressionsFinDePhrase.ajoute(a);
            LitUneLigne(fichier,tampon,NON);
			UneChaine(this,tampon);
        }
		k=lit(fichier,tampon);
    }

    if (!strcmp(tampon,CHAINES[99])) {
        int nbp;
        for (nbp=0;nbp<FinDePhrase.dernier;nbp++)
            libere(FinDePhrase[nbp]);
        nbp=0;
        FinDePhrase.raz();
        tampon[0]=0;
        LitUneLigne(fichier,tampon,OUI);
		UneChaine(this,tampon);
        while (tampon[0]!=0) {
            FinDePhrase.ajoute(DupliqueChaine(tampon));
            *flot_erreur<<"-->"<<tampon<<"<---"<<Endl;
            LitUneLigne(fichier,tampon,NON);
			UneChaine(this,tampon);
        }
        k=lit(fichier,tampon);    
    }

    //Lecture des zones
    if (!strcmp(tampon,CHAINES[47])) {
        Zonage=0;
        while (!fichier.eof() && k!='.') {
            k=lit(fichier,tampon);
            if (tampon[0]=='*')
                Zonage=MoinsUn;
            else {
                int vz=atoi(tampon);
                Zonage|= (Un << vz);
            }
        }
        k=lit(fichier,tampon);
    }

    LectureRegles=OUI;

    while (tampon[0]!=0) {
        if (!TestNomChamp(tampon,k)) {
            char err[100];
            sprintf(err,"%s:%s",CHAINES[217],tampon);
            erreur(err);
        }
        
		if (!strcmp(tampon,CHAINES[51])) {
			while (ConsCategorie(fichier));
			break;
		}

        if (AppelCons(fichier,k,tampon)==OUI)
            k=lit(fichier,tampon);
        else
            tampon[0]=0;
    }

    LectureParametres=NON;
	//Cette dependance doit toujours etre cachee
	creationtoken->X.cacher=OUI;
	surfacetoken->X.cacher=OUI;

}


void GlobalParseur::LireCodageXML(istream& fichier) {
    char k=0,tampon[300];
    int i;

    while (k!=':') {

        k=LitJusque(fichier,(uchar*)tampon,'=');

        if (AppelCons(fichier,k,tampon)==OUI)
            return;

        if (tampon[0]==0)
            return;

        //On elimine les blancs en fin de chaine
        for (i=strlen(tampon)-1;i>0;i--)
            if (tampon[i]<=32)
                tampon[i]=0;
            else
                break;

        //s'il s'agit d'un caractere code (un numero de caractere)
        //attention, a ce moment la il peut y avoir plus d'un caractere
        if (tampon[0]=='#') {
            char* ch=tampon;
            char* fin;
            int d;
            char tab[200];
            int pos=0;        
            if (xipsinput.Extract(ch,"#%d%b",&fin,&d)) {
                tab[pos++]=d;
                tab[pos]=0;
                ch=fin;
                while (xipsinput.Extract(ch,",%b#%d%b",&fin,&d)) {
                    tab[pos++]=d;
                    tab[pos]=0;
                    ch=fin;
                }
            }
            caractereXML.ajoute(DupliqueChaine(tab));
        }                
        else
            caractereXML.ajoute(DupliqueChaine(tampon));
        
        //On lit toute la chaine qui suit...
        LitLigne(fichier,tampon);      

        //On elimine les blancs en fin de chaine
        for (i=strlen(tampon)-1;i>0;i--)
            if (tampon[i]<=32)
                tampon[i]=0;
            else
                break;

        codageXML.ajoute(DupliqueChaine(tampon));
    }
}

void GlobalParseur::afficheregle(int num,const char* nom) {
    if (!TRACEREGLE)
        return;

    *Trace<<Endl<<"//-------------------------------------------------------"<<Endl
        <<"//"<<nom<<":"<<num<<" in "<<nom_fichier<<Endl;
}


char GlobalParseur::TestNomChamp(char* tampon,char k) {
    if (k!=':')
        return NON;
    
    if (!strcmp(tampon,"variables"))
        return OUI;
   
    //traduction
    if (!strcmp(tampon,CHAINES[188]))
        return OUI;

	//GenereTraits
    if (!strcmp(tampon,CHAINES[385]))
        return OUI;

	//categorydefiniton
	if (!strcmp(tampon,CHAINES[365]))
        return OUI;

	//featuredefiniton
	if (!strcmp(tampon,CHAINES[380]))
        return OUI;

    //affichagenoeud    
    if (!strcmp(tampon,CHAINES[175]))
        return OUI;

	//affichagegeneration
    if (!strcmp(tampon,CHAINES[398]))
        return OUI;
    
    //affichagefonction
    if (!strcmp(tampon,CHAINES[90]))
        return OUI;    
    
    //affichage
    if (!strcmp(tampon,CHAINES[114]))
        return OUI;
    
    //codagexml
    if (!strcmp(tampon,CHAINES[218]))
        return OUI;
    
    //traits
    if (!strcmp(tampon,CHAINES[108]))
        return OUI;        

    //ontologies
    if (!strcmp(tampon,"ontologies"))
        return OUI;        
    if (!strcmp(tampon,"relations"))
        return OUI;        
    if (!strcmp(tampon,"concepts"))
        return OUI;        

	//affichageontologies
    if (!strcmp(tampon,CHAINES[295]))
        return OUI;        

    //prestd
    if (!strcmp(tampon,CHAINES[24]))
        return OUI;    
    
    //prerct
    if (!strcmp(tampon,CHAINES[25]))
        return OUI;    
    
    //poststd
    if (!strcmp(tampon,CHAINES[120]))
        return OUI;
    

    //postrct
    if (!strcmp(tampon,CHAINES[121]))
        return OUI;    
    
    //fonctions
    if (!strcmp(tampon,CHAINES[192]))
        return OUI;   

	//Nodes
    if (!strcmp(tampon,CHAINES[391]))
        return OUI;   

	//Ordre
	if (!strcmp(tampon,CHAINES[395]))
        return OUI;   
    
    //cacher
    if (!strcmp(tampon,CHAINES[196]))
        return OUI;
   
    //garder
    if (!strcmp(tampon,CHAINES[241]))
        return OUI;   


    //filtrecouche
    if (!strcmp(tampon,CHAINES[48]))
        return OUI;    
    

    if (!strcmp(tampon,"xmlguide"))
        return OUI;
    
    if (!strcmp(tampon,"procedures"))
        return OUI;

	if (!strcmp(tampon,"tokenselection"))
        return OUI;

    if (!strcmp(tampon,"python"))
        return OUI;
    
    
    if (!strcmp(tampon,"files"))
        return OUI;    
    
    //semantique
    if (!strcmp(tampon,CHAINES[0]))
        return OUI;
    
    //Decoupe en unite linguistique
    if (!strcmp(tampon,CHAINES[49]))
        return OUI;
    
    //DI-regle
    if (!strcmp(tampon,CHAINES[21]))
        return OUI;
    
    //Termes
    if (!strcmp(tampon,CHAINES[1]))
        return OUI;
    //Simplification
    if (!strcmp(tampon,CHAINES[14]))
        return OUI;
    
    //Cartesien
    if (!strcmp(tampon,CHAINES[221]))
        return OUI;
    
    //Marquage
    if (!strcmp(tampon,CHAINES[23])) 
        return OUI;
    
    //Reconstruction
    if (!strcmp(tampon,CHAINES[2]))
        return OUI;
    
    //Deduction
    if (!strcmp(tampon,CHAINES[27])) 
        return OUI;

    //Script
    if (!strcmp(tampon,CHAINES[314])) 
        return OUI;

	if (!strcmp(tampon,"kif")) 
        return OUI;

    //POS
    if (!strcmp(tampon,CHAINES[51])) 
        return OUI;

    //Initial
    if (!strcmp(tampon,CHAINES[329])) 
        return OUI;

    //Final
    if (!strcmp(tampon,CHAINES[320])) 
        return OUI;

	if (!strcmp(tampon,"tail")) 
        return OUI;

	//Generation
    if (!strcmp(tampon,CHAINES[384])) 
        return OUI;

	if (!strcmp(tampon,"openmarkup")) 
        return OUI;
    
	if (!strcmp(tampon,"closemarkup")) 
        return OUI;

    //En chargeant
    if (!strcmp(tampon,CHAINES[328])) 
        return OUI;

    //Sequence
    if (!strcmp(tampon,CHAINES[202]))
        return OUI;
	
	//Sequence en contexte
    if (!strcmp(tampon,CHAINES[402]))
        return OUI;
    
    //PL regle
    if (!strcmp(tampon,CHAINES[53])) 
        return OUI;
    
    //Extraction
    if (!strcmp(tampon,CHAINES[201]))
        return OUI;

    //Multimots
    if (!strcmp(tampon,CHAINES[334]))
        return OUI;
    if (!strcmp(tampon,CHAINES[375]))
        return OUI;
	if (!strcmp(tampon,"probabilities"))
		return OUI;
    return NON;
}



void GlobalParseur::RenumeroteCouche(int idrule,int sens) {

	if (VlabelVecteurIndex[idrule]==NULL)
		return;

	char ch[100];
	//Nous extrayons le numero de la regle
	int numero=VlabelVecteurIndex[idrule]->numero;
	//Puis le nom du fichier
	string name=VlabelVecteur[numero]->dr->nom_fichier;
	int i=numero;
	int max=i;
	while (VlabelVecteur[i]!=NULL && VlabelVecteur[i]->dr!=NULL) {
		if (name!=VlabelVecteur[i]->dr->nom_fichier)
			break;
		max=i;
		i+=1;
	}
	i=numero;
	int min=i;
	while (VlabelVecteur[i]!=NULL && VlabelVecteur[i]->dr!=NULL) {
		if (name!=VlabelVecteur[i]->dr->nom_fichier)
			break;
		min=i;
		i-=1;
	}
	
	int basecouche=VlabelVecteurIndex[idrule]->dr->couche_relative;
	//max est position sur la derniere regle du fichier
	//Nous remontons le fichier par le bout, chaque element pointe sur le numero de couche
	i=max;
	FILE* f=fopen(name.c_str(),"r+");
	if (f==NULL)
		return;
	while (i>=min) {
		//tous les elements dont la couche relative est plus grande ou egale sont renumerotes
		if (VlabelVecteur[i]->dr->couche_relative>=basecouche) {
			//on se positionne dans le fichier a l'endroit precis
			fseek(f,VlabelVecteur[i]->dr->position,0);
			//dans le cas ou i<=numero et couche_relative==basecouche, on fait rien
			if (i>numero || VlabelVecteur[i]->dr->couche_relative>basecouche) {
				sprintf(ch,"%d>",VlabelVecteur[i]->dr->couche_relative+1);
				fwrite(ch,1,strlen(ch),f);
			}
		}
		i-=1;
	}
	fclose(f);
}


//Lecture d'un fichier contenant des dependances...
int GlobalParseur::ChargerDependances(string nomfichier) {
	initialise_erreur_instruction();
	NOM=(char*)nomfichier.c_str();
	LIGNES=0; 
	POSITION_DANS_FICHIER=0;
	LIGNESERREUR=0;     

	char tampon[255];
	char numero[255];
	char k;
	if (donnees==NULL) {
		erreur_survenue=143;
		affiche_derniere_erreur(erreur_survenue);
		return 143;
	}

	TasDeListes->Raz();
	NoeudsTemporaires.raz();

	k=lit(*donnees,tampon);
	//On lit d'abord un nom de dependance, puis les parametres
	Categorie* c;
	Traits* tr;
	VECTA<Liste*> listes;

	int phrasecourante=0;
	int i;

	while (!donnees->eof()) {
		c=RetrouveFonc(tampon);
		if (c==NULL) {
			erreur_survenue=142;
			strcpy(ajoute_info_erreur,tampon);
			affiche_derniere_erreur(erreur_survenue);
			Detruit();
			TasDeListes->Raz();
			NoeudsTemporaires.raz();
			glisteDesPhrases[0].raz();
			return 142;
		}
		tr=NULL;
		if (k=='[') {
			tr=BatitTraits(*donnees);
			k=lit(*donnees,tampon);
		}			
		if (k!='(') {
			erreur_survenue=141;
			affiche_derniere_erreur(erreur_survenue);
			Detruit();
			TasDeListes->Raz();
			NoeudsTemporaires.raz();
			glisteDesPhrases[0].raz();
			return 141;
		}
		//lecture des parametres
		//On lit d'abord le numero de phrase
		k=lit(*donnees,numero);
		int idx=atoi(numero);
		if (phrasecourante!=idx) {
			for (i=0;i<listes.dernier;i++) {
				if (listes[i]!=NULL) {
					phrase.ajoute(listes[i]->voc);
					glisteDesPhrases[0].ajoute(listes[i]);
				}
			}
			phrasecourante=idx;			
			Parse(OUI,OUI);
			Detruit();
			TasDeListes->Raz();
			NoeudsTemporaires.raz();
			listes.raz();
			glisteDesPhrases[0].raz();
			num_phrase++;
		}

		ResultatFonction* fonc=FournirResFonc(c->Categ(),NULL);
		fonc->bon=UTILISE;
		if (tr!=NULL) {
			fonc->traits->ou(tr);		
			delete tr;
		}

		//Puis on lit la virgule ou la (
		if (k!=',' && k!=')') {
			erreur_survenue=144;
			affiche_derniere_erreur(erreur_survenue);
			Detruit();
			TasDeListes->Raz();
			NoeudsTemporaires.raz();
			glisteDesPhrases[0].raz();
			return 144;
		}

		//On lit d'abord la categorie
		while (k!=')') {
			tr=NULL;
			k=lit(*donnees,tampon);
			c=RetrouveCat(tampon);
			if (c==NULL) {
				erreur_survenue=154;
				affiche_derniere_erreur(erreur_survenue);
				Detruit();
				TasDeListes->Raz();
				NoeudsTemporaires.raz();
				glisteDesPhrases[0].raz();
				return 154;
			}
			if (k!='#') {
				erreur_survenue=145;
				affiche_derniere_erreur(erreur_survenue);
				Detruit();
				TasDeListes->Raz();
				NoeudsTemporaires.raz();
				glisteDesPhrases[0].raz();
				return 145;
			}

			//On lit alors le numero de noeud
			k=lit(*donnees,numero);
			idx=atoi(numero);
			Liste* L=listes[idx];
			if (k=='[') {
				//Puis les traits
				tr=BatitTraits(*donnees);			
				k=lit(*donnees,tampon);
			}
			else
				if (L==NULL){
					erreur_survenue=146;
					affiche_derniere_erreur(erreur_survenue);
					Detruit();
					TasDeListes->Raz();
					NoeudsTemporaires.raz();
					glisteDesPhrases[0].raz();
					return 146;
				}

				if (L==NULL) {
					if (tr->lemme==NULL){
						erreur_survenue=147;
						affiche_derniere_erreur(erreur_survenue);
						Detruit();
						TasDeListes->Raz();
						NoeudsTemporaires.raz();
						glisteDesPhrases[0].raz();
						return 147;
					}
					L=CreeNoeudFragment(NULL,tr,c->Categ(),idx,idx);
					listes.affecte(idx,L);
					//Puis on rajoute les traits provenant du lexique
					vlex.initiaux=L->traitsbase;
					vlex.X=c->Categ();
					strcpy(vlex.lemme,L->N.fils->lemme);
					strcpy(vlex.surface,L->N.fils->surface);
					char tv=TraitementVocabulaire(OUI,0,phrase.dernier-1);
					L->SauveTraits();
				}

				if (tr!=NULL)
					delete tr;
				fonc->arg.ajoute(&L->N);
				//Puis on lit la virgule ou la (
				if (k!=',' && k!=')') {
					erreur_survenue=144;
					affiche_derniere_erreur(erreur_survenue);
					Detruit();
					TasDeListes->Raz();
					NoeudsTemporaires.raz();
					glisteDesPhrases[0].raz();
					return 144;
				}
		}
		//On relit ensuite la ligne suivante
		fonc->nb_args=fonc->arg.dernier;
		AjoutDansINTERMEDIAIRE(fonc);
		k=lit(*donnees,tampon);
		if (k=='.')
			k=lit(*donnees,tampon);
		else {
			erreur_survenue=148;
			affiche_derniere_erreur(erreur_survenue);
			Detruit();
			TasDeListes->Raz();
			NoeudsTemporaires.raz();
			glisteDesPhrases[0].raz();
			return 148;
		}
	}

	for (i=0;i<listes.dernier;i++) {
		if (listes[i]!=NULL) {
			phrase.ajoute(listes[i]->voc);
			glisteDesPhrases[0].ajoute(listes[i]);
		}
	}
	Parse(OUI,OUI);
	Detruit();
	TasDeListes->Raz();
	NoeudsTemporaires.raz();
	glisteDesPhrases[0].raz();
	return 0;

}

//Sauvegarde des probas sous une forme tres simple: id,poids,seuil\n
void GlobalParseur::sauvegardeprobabilites(string nom) {
	ofstream fichier((char*)nom.c_str(),ios::binary);
	fichier<<"probabilities:\n";
	for (int i=0;i<VBases.dernier;i++) {
		float v1=VBases[i]->poids;
		float v2=VBases[i]->seuil;
		float v3=VBases[i]->valeur;
		int compteur=VBases[i]->compteur;
		fichier<<i<<"="<<v1<<":"<<v2<<":"<<v3<<"*"<<compteur<<";";
	}
}

char GlobalParseur::chargementprobabilites(istream& fichier) {
	char tampon[500];
	int i;
	float poids,seuil,valeur;
	int compteur;
	char c1,c2,c3,c4,sep;
	while (!fichier.eof()) {
		fichier>>i>>c1>>poids>>c2>>seuil>>c3>>valeur>>c4>>compteur>>sep;
		POSITION_DANS_FICHIER=fichier.tellg();
		
		if (c1!='=' || c2!=':' || c3!=':' || c4!='*' || sep!=';') {
			strcpy(tampon,CHAINES[400]);
			erreur(tampon);
		}

		if (VBases[i]!=NULL) {
			VBases[i]->seuil=seuil;
			VBases[i]->poids=poids;
			VBases[i]->valeur=valeur;
		}
		else {
			strcpy(tampon,CHAINES[400]);
			erreur(tampon);
		}
	}
	return (1);
}

char GlobalParseur::chargeprobabilites(string nom) {
	ifstream fichier((char*)nom.c_str(),modeOuverture);
	strcpy(Nom_Fichier,(char*)nom.c_str());
	NOM=Nom_Fichier;
	char tampon[500];
	if (fichier.fail()) {
		strcpy(tampon,CHAINES[399]);
		strcat(tampon,Nom_Fichier);
		erreur(tampon);
	}

	LIGNES=0;
	POSITION_DANS_FICHIER=0;
	fichier.read(tampon,14);
	return chargementprobabilites(fichier);
}

bool GlobalParseur::UTF8() {
	if (encodageutf8)
		return true;
	return false;
}
int GlobalParseur::OccupationMemoire() {
	int sz=0;
	int i;
	for (i=0;i<MaxCouchePhrase;i++)		
			sz+=glisteDesPhrases[i].taille;
	sz+=TasDeListeVoc.taille;
	if (TasDeListes!=NULL)
		sz+=TasDeListes->taille;
	sz+=NoeudsTemporaires.taille;
	sz+=enregListeTags.taille;
	return sz;
}
