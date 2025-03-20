#include "parsbin.h"

#include "ProcessorFactory.hxx"
#include "Processor.hxx"
#include "Analyzer.hxx"
#include "InputBuilder.hxx"
#include "OutputBuilder.hxx"
#include "OutputWriterFactory.hxx"
#include "OutputWriter.hxx"
#include "MemoryWriter.hxx"
#include "OutputIterator.hxx"

#include "xipjmax.h"
#include "xipresult.h"
/*
#define OP 0	//0: output to file
				//1: output to stdout with buffering result in memory
*/

#define PRF 1	//Plain Resutl Format
#define WOF 2	//With Offset Format
#define SWF 4	//Single Word Format
#define CWF 8	//Compound Word Format
#define LEX 16	//Lexical
#define SUR 32	//Surface

GlobalParseur* XipSelectGrammar(unsigned int iParseur);
int CompteTab(char* ligne);

void GlobalParseurJMAX::Analyse(char renum,char tree) {
    int nFlag = 0;


    epr();	//extended plain result
}

void GlobalParseurJMAX::AnalysePhrase(char renum,char tree) {
    int nFlag = 0;


    epr();	//extended plain result
}

GlobalParseurJMAX::GlobalParseurJMAX(char* filename, char* taf4, char* lsc, char* tot,char* path) : GlobalParseur(filename,path) {

    processor = procFactory.createProcessor(taf4,lsc);
    azr = processor->createAnalyzer(tot);

}


void GlobalParseurJMAX::registerEntries(ListeVoc* voc,char* s) {
    char* copieChaine;
    
    if (CompteTab(s)<2) {
        char message[1000];
        if (strlen(s)>100)
            s[100]=0;
        sprintf(message,"Error... Wrong entry: %s (removed from parse)",s);
        insere_erreur(message);
        return;
    }

    if (TestBalise(s)==OUI)
        return;         
    
    if (ContientFinDePhrase(s)) {
        ParseEntreeLexique(s,&copieChaine);
        voc=ConstruitForme(copieChaine,num_mot);     
        
        if (voc!=NULL) {
            if (DESAMBIGUISATION==OUI)
                num_mot++;        
            voc->borneGauche=borneGauche;
            voc->borneDroite=borneDroite;
        }
        
        Parse(OUI);
        AffichageResultat();
        Detruit();
        
        num_phrase++;
        
        if (renumerotation==OUI)
            num_mot=0;
        return;
    }
    else  {
        ParseEntreeLexique(s,&copieChaine);
        if (voc==NULL) {
            voc=ConstruitForme(copieChaine,num_mot);
            if (voc!=NULL) {
                voc->borneGauche=borneGauche;
                voc->borneDroite=borneDroite;
                num_mot++;
            }
        }
        else
            FusionneForme(voc,copieChaine);
    }                      
}

GlobalParseurJMAX::~GlobalParseurJMAX() {
	delete azr;
	delete processor;
}

Exportation int XipLoadGrammarJMAX(const char* grammaire, const char* taf4, const char* lsc, const char* tot) {
    char path[1000];
    char extract=0;
       
    char p_chemin=0;
    char p_fichier=0;
    char pretraitement=NON;
    TVecteur aff_fonc=AFFICHE_FONCTION;

    char tagger=NON;
    char tagging=OUI;
    char aff_err=NON;
    
    typeLangue langue=FRANCAIS;
    VECTA<char*> fileparam;
    int maxSentence=-1;
    char fonction_synt=OUI;
    char theTrace=0;
    
    TVecteur offset=AFFICHAGE_OFFSET;
    TVecteur reduce=AFFICHAGE_SURFACE | AFFICHAGE_REDUIT;
    
    char inputXML=NON;
    char outputXML=NON;
    
    char stringfile[1000];
    char texte[1000];
    char renumerote=OUI;
    char tree=OUI;
    
    char lagrammaire[1000];

    strcpy(lagrammaire,grammaire);    
    char* last_slash= strrchr(grammaire,SEP);
    
    if (last_slash==NULL)
        strcpy(path,DEB);
    else {
        *last_slash=0;
        strcpy(path,grammaire);
        strcpy(lagrammaire,last_slash+1);
    }
    
    
    stringfile[0]=0;
    texte[0]=0;
    
    //the language argument is processed
    int arret=0;
    int numberValues=-1;
    char crypte=NON;


    GlobalParseur* leParseur = new GlobalParseurJMAX(lagrammaire,(char*)taf4,(char*)lsc,(char*)tot,path);    

    int ipar= leParseur->InitialisationParseur(langue,
        stringfile,
        path,
        numberValues,
        theTrace,
        aff_fonc,
        outputXML,
        tagger,
        tagging,
        OUI,                                               
        crypte,
        aff_err,
        OUI,
        AFFICHAGE_DEP_NOM,
        offset,
        reduce,
        NON,
        fonction_synt,
        fileparam);
    
    return ipar;
}


Exportation int XipGrammarLoadingJMAX(const char* grammaire, const char* taf4, const char* lsc, const char* tot) {
    char path[1000];
    char extract=0;
    
    char p_chemin=0;
    char p_fichier=0;
    char pretraitement=NON;
    TVecteur aff_fonc=AFFICHE_FONCTION;

    char tagger=NON;
    char tagging=OUI;
    char aff_err=NON;
    
    typeLangue langue=FRANCAIS;
    VECTA<char*> fileparam;
    int maxSentence=-1;
    char fonction_synt=OUI;
    char theTrace=0;
    
    TVecteur offset=AFFICHAGE_OFFSET;
    TVecteur reduce=AFFICHAGE_SURFACE | AFFICHAGE_REDUIT;
    
    char inputXML=NON;
    char outputXML=NON;
    
    char stringfile[1000];
    char texte[1000];
    char renumerote=OUI;
    char tree=OUI;
    
    char lagrammaire[1000];

    strcpy(lagrammaire,grammaire);    
    char* last_slash= strrchr(grammaire,SEP);
    
    if (last_slash==NULL)
        strcpy(path,DEB);
    else {
        *last_slash=0;
        strcpy(path,grammaire);
        strcpy(lagrammaire,last_slash+1);
    }
    
    
    stringfile[0]=0;
    texte[0]=0;
    
    //the language argument is processed
    int arret=0;
    int numberValues=-1;
    char crypte=OUI;


    GlobalParseur* leParseur = new GlobalParseurJMAX(lagrammaire,(char*)taf4,(char*)lsc,(char*)tot,path);    

    int ipar= leParseur->InitialisationParseur(langue,
        stringfile,
        path,
        numberValues,
        theTrace,
        aff_fonc,
        outputXML,
        tagger,
        tagging,
        OUI,                                               
        crypte,
        aff_err,
        OUI,
        AFFICHAGE_DEP_NOM,
        offset,
        reduce,
        NON,
        fonction_synt,
        fileparam);

    return ipar;
}



Exportation int XipParseFileJMAX(char* filename,int ipar, XipResult* xip, char jmaxonly) {
    GlobalParseurJMAX* leParseur= (GlobalParseurJMAX*)XipSelectGrammar(ipar);
    if (leParseur==NULL)
        return -1;

    leParseur->appliqueGrammaire=1-jmaxonly;
    ifstream document(filename,modeOuverture);    

    (leParseur->azr)->getInputBuilder()->setStream(document);
    leParseur->xipresult=xip;
    leParseur->Analyse(NON,OUI);
    return 0;
}

Exportation int XipParseStringJMAX(const char* texte,int ipar, XipResult* xip,char jmaxonly) {
    GlobalParseurJMAX* leParseur= (GlobalParseurJMAX*)XipSelectGrammar(ipar);
    if (leParseur==NULL)
        return -1;    

    leParseur->appliqueGrammaire=1-jmaxonly;
    (leParseur->azr)->getInputBuilder()->setText(texte);

    leParseur->xipresult=xip;
    leParseur->Analyse(NON,OUI);
    return 0;
}


